
//====================================================================
//      RawDataTofMcHitCnv.cxx
//====================================================================
//
// Description: A converter class to unpack Event Filter packed raw   
//              event persistent data and place it in the Transient    
//              Data Store of Athena.
//
//--------------------------------------------------------------------

// Include files.
#include <assert.h>
#include <vector>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "EventModel/EventModel.h"
#include "Identifier/Identifier.h"
#include "Identifier/TofID.h"
#include "McTruth/TofMcHit.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "RawDataCnv/EventManagement/RawEvent.h" 
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/RawDataTofMcHitCnv.h"

using Event::TofMcHit;
using Event::TofMcHitCol;

extern const CLID& CLID_TofMcHitCol;

// Constructor.
RawDataTofMcHitCnv::RawDataTofMcHitCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
  //init();
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataTofMcHitCnv::classID()
{
  return CLID_TofMcHitCol;
}

StatusCode RawDataTofMcHitCnv::initialize()
{

    std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
    std::string RawDataInputSvc_Name("RawDataInputSvc");
    std::string RawDataTofMcHitCnv_Name("RawDataTofMcHitCnv");
    
    // for Mutil-thread by tianhl
    //ConversionSvc*  pCnvSvc = 0;
    //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
    SmartIF<IService> pCnvSvc(conversionSvc());
      if  (isGaudiThreaded(pCnvSvc->name())){
        PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataTofMcHitCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
      //}
    
  MsgStream log(messageService(), RawDataTofMcHitCnv_Name.c_str());

  StatusCode sc = RawDataBaseCnv::initialize();
  if (StatusCode::SUCCESS != sc) {
    log << MSG::ERROR << "RawDataBaseCnv: Cant initialize PackedRawDataCnvSvc" << endreq;
    return sc;
  }

  // Check RawDataCnvSvc
  IService* isvc = 0;
  sc = serviceLocator()->service(PackedRawDataCnvSvc_Name.c_str(), isvc, true);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Cant get PackedRawDataCnvSvc" << endreq;
  }

  m_RawDataAccess = dynamic_cast<PackedRawDataCnvSvc*> (isvc); 
  if (m_RawDataAccess  == 0 ) {
    log << MSG::ERROR << "RawDataTofCnv: Cant cast to RawDataCnvSvc " << endreq; 
    return StatusCode::FAILURE;
  }

  sc = serviceLocator()->getService(RawDataInputSvc_Name.c_str(), isvc);
  if (sc != StatusCode::SUCCESS ) {
    log << MSG::WARNING << "Cant get RawDataInputSvc " << endreq;
    return sc ;
  }

  m_inputSvc = dynamic_cast<RawDataInputSvc*> (isvc);
  if (m_inputSvc == 0 ) {
    log << MSG::WARNING << "Cant cast to RawDataInputSvc " << endreq;
    return StatusCode::FAILURE ;
  }

  return StatusCode::SUCCESS; 
}


StatusCode RawDataTofMcHitCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
  // Purpose and Method:  This method does nothing other than announce it has
  //   been called.

  //MsgStream log(msgSvc(), "RawDataTofMcHitCnv");
  //log << MSG::DEBUG << "RawDataTofMcHitCnv::updateObj" << endreq;
  return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataTofMcHitCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  MsgStream log(msgSvc(), "RawDataTofMcHitCnv");

  //This converter will create an empty TofMcHitCol on the TDS
  TofMcHitCol *tofMcHitCol = new TofMcHitCol;
  pObj = tofMcHitCol;
   
  RAWEVENT *evt = m_inputSvc->currentEvent(); 
  if (evt == NULL) {
      //log << MSG::ERROR << "RawDataCnv has no event!" << endreq; 
      return StatusCode::FAILURE;
  }
  
  assert((evt->getTofTruth().size())%9 == 0);

  TofTruth_t m_TofTruth;
  TofMcHit* tofMcHit;

  std::vector<uint32_t>::const_iterator iter = evt->getTofTruth().begin();
  std::vector<uint32_t>::const_iterator eiter = evt->getTofTruth().end();

  for (int tofMcHitId = 0; iter != eiter; tofMcHitId++) {
    // retrieve the TofTruth data
    m_tofMcHitBuilder.unPack(iter, eiter, m_TofTruth);
    // construct the identifier
    Identifier ident;
    unsigned int barrel_ec = m_TofTruth.partId;
    // for scintillator
    if( barrel_ec>=0 && barrel_ec<=2 ) {
      unsigned int layer = 0;
      if( m_TofTruth.scinNb > TofID::getPHI_BARREL_MAX() ) {
	layer = 1;
	m_TofTruth.scinNb -= ( TofID::getPHI_BARREL_MAX() + 1 );
      }
      ident = TofID::cell_id( barrel_ec, layer, m_TofTruth.scinNb, 0 );
    }
    // for ETF(MRPC)
    else if( barrel_ec>=3 ) {
      unsigned int endcap = 0;
      if( barrel_ec==4 ) { endcap = 1; }
      ident = TofID::cell_id( 3, endcap, m_TofTruth.scinNb, m_TofTruth.strip, 0 );
    }
    else {
      log << MSG::ERROR << "barrel_ec is not correct! barrel_ec=" << barrel_ec << endreq; 
    }

    //construct the TofMcHit
    tofMcHit = new TofMcHit(ident, m_TofTruth.trackIndex,
	              m_TofTruth.x/(m_tofMcHitBuilder.m_xCoeff*1.0),
		      m_TofTruth.y/(m_tofMcHitBuilder.m_yCoeff*1.0),
		      m_TofTruth.z/(m_tofMcHitBuilder.m_zCoeff*1.0),
	              m_TofTruth.px/(m_tofMcHitBuilder.m_pxCoeff*1.0),
		      m_TofTruth.py/(m_tofMcHitBuilder.m_pyCoeff*1.0),
		      m_TofTruth.pz/(m_tofMcHitBuilder.m_pzCoeff*1.0),
	              m_TofTruth.trackLength/(m_tofMcHitBuilder.m_trackLengthCoeff*1.0),
		      m_TofTruth.time/(m_tofMcHitBuilder.m_timeCoeff*1.0) );
    // And add the stuff to the container
    tofMcHitCol->push_back(tofMcHit);
  }

  return StatusCode::SUCCESS; 

}

StatusCode RawDataTofMcHitCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)  
{
   // convert PixelRaw in the container into ByteStream
  //MsgStream log(messageService(), "RawDataTofMcHitCnv");
  
  WriteRawEvent*& re = m_RawDataAccess->getRawEvent(); 
  
  if (re == 0) {
     //log << " get RawEvent failed !" << endreq;
     return StatusCode::FAILURE; 
  }

  SmartDataPtr<TofMcHitCol> mcHitCol(dataProvider(), EventModel::MC::TofMcHitCol);

  if (mcHitCol == 0) {
     //log << "no TofMcHitCol found" << endreq;
     return StatusCode::FAILURE;
  }
  
  StatusCode sc = m_tofMcHitBuilder.pack(mcHitCol, re);

  return sc; 

}

