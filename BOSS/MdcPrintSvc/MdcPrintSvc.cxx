#include "MdcPrintSvc/MdcPrintSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataSvc.h"
#include "EventModel/EventHeader.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Identifier/MdcID.h"
#include "RawEvent/RawDataUtil.h"
#include "MdcRawEvent/MdcDigi.h"
#include "EvTimeEvent/RecEsTime.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "McTruth/McParticle.h"
#include "McTruth/MdcMcHit.h"
#include "EventModel/EventModel.h"

#include <iomanip>
#include <iostream>

using namespace std;
using namespace Event;

MdcPrintSvc::MdcPrintSvc( const string& name, ISvcLocator* svcloc) :
  Service (name, svcloc) {

    // declare properties
    declareProperty("printHit", 	m_printHit=1);
    declareProperty("printTrackDetail", m_printTrackDetail=1);
    declareProperty("printErrMat", 	m_printErrMat=0);
    declareProperty("printTdcAdc", 	m_printTdcAdc=0);
    declareProperty("printMcTk", 	m_printMcTk=0);
  }

MdcPrintSvc::~MdcPrintSvc(){
}

StatusCode MdcPrintSvc::initialize(){
  MsgStream log(messageService(), name());
  log << MSG::INFO << "MdcPrintSvc::initialize()" << endreq;

  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) return sc;

  ISvcLocator* svcLocator = Gaudi::svcLocator();
  svcLocator->service("EventDataSvc",eventSvc,true);
  if (NULL == eventSvc){
    std::cout << "ERROR Could not load service DataSvc" << std::endl;
    return StatusCode::FAILURE;
  }

  IRawDataProviderSvc* irawDataProviderSvc;
  sc = service ("RawDataProviderSvc", irawDataProviderSvc);
  m_rawDataProviderSvc = dynamic_cast<RawDataProviderSvc*> (irawDataProviderSvc);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load RawDataProviderSvc!" << endreq;
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}

StatusCode MdcPrintSvc::finalize(){
  MsgStream log(messageService(), name());
  log << MSG::INFO << "MdcPrintSvc::finalize()" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode MdcPrintSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
  if( IID_IMdcPrintSvc.versionMatch(riid) ){
    *ppvInterface = static_cast<IMdcPrintSvc*> (this);
  } else{
    return Service::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}

void MdcPrintSvc::printDigi(uint32_t getDigiFlag)const{
  MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag);
  MdcDigiCol::iterator iter = mdcDigiVec.begin();
  std::cout<<"//==== "<<name()<<" print MdcDigiVec, nDigi="<<mdcDigiVec.size()<<" :"<<std::endl;
  for (int iDigi=0;iter!= mdcDigiVec.end(); iter++,iDigi++ ) {
    long l = MdcID::layer((*iter)->identify());
    long w = MdcID::wire((*iter)->identify());
    cout<<"("<<l<<","<<w;
    if(m_printMcTk) {
      int tkTruth = (*iter)->getTrackIndex();
      cout<<";"<<tkTruth;
    }
    if(m_printTdcAdc) {
      double tdc = RawDataUtil::MdcTime((*iter)->getTimeChannel());
      double adc = RawDataUtil::MdcCharge((*iter)->getChargeChannel());
      cout<<",t "<<tdc<<",c "<<adc;
    }
    cout<<")";
    if(iDigi%4==0) std::cout<<std::endl;
  }
  std::cout<<std::endl;
}

void MdcPrintSvc::printRecMdcTrackCol()const{
  MsgStream log(messageService(), name());

  SmartDataPtr<RecMdcTrackCol> recMdcTrackCol(eventSvc,"/Event/Recon/RecMdcTrackCol");
  if (!recMdcTrackCol) {
    log << MSG::WARNING << "Could not find RecMdcTrackCol" << endreq;
    return;
  }    

  std::cout<< "//======= "<<name()<<" RecMdcTrackCol size = "<<recMdcTrackCol->size() << std::endl;
  RecMdcTrackCol::iterator it = recMdcTrackCol->begin();
  for (;it!= recMdcTrackCol->end();it++){
    printRecMdcTrack(*it);
  }

}

void MdcPrintSvc::printRecMdcTrack(const RecMdcTrack* tk)const{
  std::cout<< "//==== "<<name()<<" print RecMdcTrack No."<<tk->trackId()<<" :"<< std::endl;
  cout <<" dr "<<tk->helix(0)
    <<" phi0 "<<tk->helix(1)
    <<" cpa "<<tk->helix(2)
    <<" z0 "<<tk->helix(3)
    <<" tanl "<<tk->helix(4)
    <<endl;
  if(m_printTrackDetail){
    std::cout<<" q "<<tk->charge() 
      <<" theta "<<tk->theta()
      <<" phi "<<tk->phi()
      <<" x0 "<<tk->x()
      <<" y0 "<<tk->y()
      <<" z0 "<<tk->z()
      <<" r0 "<<tk->r()
      <<endl;
    std::cout <<" p "<<tk->p()
      <<" pt "<<tk->pxy()
      <<" pxyz("<<tk->px()
      <<","<<tk->py()
      <<","<<tk->pz()
      <<")"<<endl;
    std::cout<<" tkStat "<<tk->stat()
      <<" chi2 "<<tk->chi2()
      <<" ndof "<<tk->ndof()
      <<" nhit "<<tk->getNhits()
      <<" nst "<<tk->nster()
      <<endl;
    if(m_printErrMat){
      std::cout<< "errmat   " << std::endl;
      for (int i=0; i<15; i++){ std::cout<< " "<<tk->err(i); }
      std::cout<< "   " << std::endl;
    }
  }

  if(m_printHit){
    int haveDigi[43][288];
    if(m_printMcTk) {
      for(int ii=0;ii<43;ii++){
	for(int jj=0;jj<43;jj++){
	  haveDigi[ii][jj]=-9999;
	}
      }
      MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec();
      MdcDigiCol::iterator iter = mdcDigiVec.begin();
      std::cout<<name()<<"//==== "<<name()<<" print MdcDigiVec, nDigi="<<mdcDigiVec.size()<<" :"<<std::endl;
      for (int iDigi=0;iter!= mdcDigiVec.end(); iter++,iDigi++ ) {
	long l = MdcID::layer((*iter)->identify());
	long w = MdcID::wire((*iter)->identify());
	haveDigi[l][w]=(*iter)->getTrackIndex();
      }
    }

    int nhits = tk->getVecHits().size();
    std::cout<<"nHits=" <<nhits<< std::endl;
    cout<<"No. ";
    if(m_printMcTk) cout<<"mcTkId ";
    cout<<"(layer,wire,lr) stat z"<<endl;
    for(int ii=0; ii <nhits ; ii++){
      Identifier id(tk->getVecHits()[ii]->getMdcId());
      int layer = MdcID::layer(id);
      int wire = MdcID::wire(id);
      cout<<ii<<":";
      if(m_printMcTk) { cout<<haveDigi[layer][wire]; }
      cout<<" ("<<layer<<","<<wire
	<<","<<tk->getVecHits()[ii]->getFlagLR()
	<<")  "<<tk->getVecHits()[ii]->getStat()
	<<"  "<<tk->getVecHits()[ii]->getZhit()<< " "<<std::endl;
    }//end of hit list
    std::cout << "  "<< std::endl;
  }
}

void MdcPrintSvc::printMdcMcHitCol()const{
  MsgStream log(messageService(), name());

  SmartDataPtr<MdcMcHitCol> mdcMcHitCol(eventSvc,"/Event/MC/MdcMcHitCol");
  if (!mdcMcHitCol) {
    log << MSG::WARNING << "Could not find MdcMcHitCol" << endreq;
    return;
  }

  int ihit= 0;
  cout<<  "//==== "<<name()<<" print MdcMcHitCol nMcHit="<<mdcMcHitCol->size()<< endl;
  if (mdcMcHitCol->size()>0){
    cout<<"No. (layer,wire) mcTk  (x,y,z)   lr  driftdist(cm)  energy"<<endl;
  }
  MdcMcHitCol::iterator iter_mchit = mdcMcHitCol->begin();
  for (;iter_mchit != mdcMcHitCol->end(); iter_mchit++,ihit++ ) {
    const Identifier id= (*iter_mchit)->identify();
    int layer = MdcID::layer(id);
    int wire = MdcID::wire(id);
    int iMcTk =  (*iter_mchit)->getTrackIndex();
    double mcX = (*iter_mchit)->getPositionX()/10.;
    double mcY = (*iter_mchit)->getPositionY()/10.;
    double mcZ = (*iter_mchit)->getPositionZ()/10.;
    double mcLR = (*iter_mchit)->getPositionFlag();
    double mcDrift = (*iter_mchit)->getDriftDistance()/10.;
    double mcEnergy = (*iter_mchit)->getDepositEnergy()/10.;
    if (mcLR == 0) mcLR = -1;
    cout<<ihit <<" ("<<layer<<","<<wire<<") "<< iMcTk
      <<" ("<< mcX<<"," <<mcY<<","<< mcZ
      <<")  " <<mcLR
      <<"  " <<mcDrift
      <<"  " <<mcEnergy
      <<endl;
  }
}
