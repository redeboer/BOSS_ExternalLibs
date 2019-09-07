#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "RawDataProviderSvc/MdcRawDataProvider.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "Identifier/MdcID.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "RawEvent/RawDataUtil.h"
#include "EvTimeEvent/RecEsTime.h"

// tianhl for mt
#include "GaudiKernel/ThreadGaudi.h"
// tianhl for mt
#include <stdint.h>
#include <vector>

using namespace std;

MdcRawDataProvider::MdcRawDataProvider() :
  RawDataProviderBase(),
  digiRef(0),
  m_mdcCalibFunSvc(0) 
{}


MdcRawDataProvider::MdcRawDataProvider( const char* name ) :
  RawDataProviderBase( name ),
  digiRef(0),
  m_mdcCalibFunSvc(0) 
{}

MdcRawDataProvider::~MdcRawDataProvider() {
}

StatusCode MdcRawDataProvider::initialize( ISvcLocator* pSvcLoc, IMessageSvc* pMsg ) {
  RawDataProviderBase::initialize( pSvcLoc,pMsg );
  return StatusCode::SUCCESS; 
}

void MdcRawDataProvider::handle(const Incident& inc){
  MsgStream log( m_msgSvc, m_name );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;
  if ( inc.type() == "BeginEvent" ){
    digiRef.clear();
  }
  return;
}

MdcDigiVec& MdcRawDataProvider::getMdcDigiVec(uint32_t control){
  MsgStream log(m_msgSvc, m_name);
  StatusCode sc;
  bool unRedo = ((control&b_unRedo)==b_unRedo);
  if(!unRedo) digiRef.clear();
  else if(digiRef.size()>0) return digiRef;
  //---- get event service
  IDataProviderSvc* evtSvc;
  // tianhl for mt
  std::string evtDataSvc_name("EventDataSvc");
  if(isGaudiThreaded(m_name)){
    evtDataSvc_name += getGaudiThreadIDfromName(m_name);
  }
  // tianhl for mt
  sc = m_svcLocator->service(evtDataSvc_name.c_str(),evtSvc,true);
  if (!sc.isSuccess()){
    log << MSG::FATAL << "Could not load EventDataSvc" << endreq;
    return digiRef;
  } 

  //---- get max MdcDigi
  int maxMdcDigi = control&i_maxMdcDigi;

  //---- prepare for drop Bad TDC
  bool keepBadTdc = ((control&b_keepBadTdc)!=0x10000);
  double t0 = 0.;
  if (keepBadTdc){
    //get MdcCalibFunSvc
    sc = m_svcLocator->service("MdcCalibFunSvc", m_mdcCalibFunSvc);
    if ( sc.isFailure() ){
      log << MSG::FATAL << "Could not load MdcCalibFunSvc!" << endreq;
      return digiRef; 
    }

    //get event start time t0
    SmartDataPtr<RecEsTimeCol> esTimeCol(evtSvc,"/Event/Recon/RecEsTimeCol");
    if (!esTimeCol){
      log << MSG::INFO<< " Could not retrieve RecEsTimeCol"<< endreq;
      keepBadTdc = false;
    }else if( (esTimeCol->size()==0)) {
      log << MSG::INFO<< " Could not retrieve RecEsTimeCol"<< endreq;
      keepBadTdc = false;
    }else{
      RecEsTimeCol::iterator iter= esTimeCol->begin();
      for(; iter!=esTimeCol->end(); iter++){
	t0 = (*iter)->getTest();
      }
    }//end esTimeCol
  }

  //----- mark hit on track
  bool m_hitInUse[43][288];
  for (int i=0; i<43; i++){
    for (int j=0; j<288; j++) m_hitInUse[i][j]=false;
  }
  bool dropHot = ((control&b_dropHot)==0x4000);
  if (dropHot){
    SmartDataPtr<RecMdcHitCol> recHitCol(evtSvc, "/Event/Recon/RecMdcHitCol");
    if (!recHitCol) {
      log << MSG::INFO << "Could not retrieve RecMdcHitCol" << endreq;
      dropHot = false;
    }else{
      RecMdcHitCol::iterator iter = recHitCol->begin();
      for ( ; iter != recHitCol->end(); iter++) {
	Identifier id = (*iter)->getMdcId();
	m_hitInUse[MdcID::layer(id)][MdcID::wire(id)] = true;
      }
    }
  }

  //----- mark skipped layers 
  bool isSkipLayers = ((control&b_keepSkipped)!=0x8000) && (m_skipLayers.size()>0);

  bool skipLayer[43];
  for (int i=0; i<43; i++){skipLayer[i]= false;}
  for(unsigned iSkip =0; iSkip < m_skipLayers.size(); iSkip++){
    skipLayer[m_skipLayers[iSkip]]=true;
  }

  //----- loop MdcDigiCol to select------------------
  int iDigi=0;
  int nMatchedDigi=0;

  SmartDataPtr<MdcDigiCol> mdcDigiCol(evtSvc,"/Event/Digi/MdcDigiCol");
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::FATAL << "Could not find MdcDigiCol!" << endreq;
    return digiRef;
  }
  MdcDigiCol::iterator iter = mdcDigiCol->begin();
  for (;iter != mdcDigiCol->end(); iter++,iDigi++ ) {
    MdcDigi *aDigi = (*iter);
    Identifier id = aDigi->identify();
    int layer = MdcID::layer(id);
    int wire  = MdcID::wire(id);
    unsigned tdc = aDigi->getTimeChannel();
    unsigned adc = aDigi->getChargeChannel();
    unsigned overflow = aDigi->getOverflow();

    //--0. skip dead channel 
    if ( ((control&b_keepAllChannel)!=0x40000) && m_mdcCalibFunSvc 
	&& m_mdcCalibFunSvc->getWireEff(layer,wire) < m_effThreshold) continue;

    //--1. drop Mdc layer hits
    if ( isSkipLayers && skipLayer[layer] ) continue;

    //--2. drop hit on track
    if ( dropHot && m_hitInUse[layer][wire]) continue;

    //--3. drop unmatched, overflow and multi TDC
    //overflow :
    // 1, T overflow
    // 2, Q overflow
    // 4, multi TDC
    // 8, multi TDC, earliest
    if ( ((control&b_keepUnmatch)!=0x2000) &&
	(((overflow & 1)>0) ||
	 (((overflow&12)!=12)&&((overflow&12)!=0)) ||
	 (tdc==0x7FFFFFFF) || (adc== 0x7FFFFFFF) ) ) { continue; }



    //--4. drop bad hit(Tdc - t0 < -10 ns)
    if (keepBadTdc) {
      // Get T0
      double T0Walk = 0.;
      if (m_mdcCalibFunSvc) { T0Walk = m_mdcCalibFunSvc->getT0(layer,wire)
	+ m_mdcCalibFunSvc->getTimeWalk(layer,adc); }
      if((RawDataUtil::MdcTime(tdc) - T0Walk - t0)<-10.) continue;
    }

    //--5. skip by max limit of digi number
    nMatchedDigi++;
    if ((maxMdcDigi>0) && (nMatchedDigi > maxMdcDigi)){
      digiRef.clear();
      return digiRef;
    }

    digiRef.push_back(aDigi);
  }//end loop of MdcDigiCol
  return digiRef;
}
