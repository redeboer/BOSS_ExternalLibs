//====================================================================
//      RawDataEmcMcHitCnv.cxx
//====================================================================
//
// Description: A converter class to unpack Event Filter packed raw   
//              event persistent data and place it in the Transient    
//              Data Store of Athena.
//
//--------------------------------------------------------------------

// Include files.
#include <assert.h>
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/MsgStream.h"
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "EventModel/EventModel.h"
#include "Identifier/Identifier.h"
#include "Identifier/EmcID.h"
#include "McTruth/EmcMcHit.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "RawDataCnv/EventManagement/RawEvent.h" 
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/RawDataEmcMcHitCnv.h"
                                                                                                                             
using Event::EmcMcHit;
using Event::EmcMcHitCol;

extern const CLID& CLID_EmcMcHitCol;

// Constructor.
RawDataEmcMcHitCnv::RawDataEmcMcHitCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
  //init();
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataEmcMcHitCnv::classID()
{
  return CLID_EmcMcHitCol;
}

StatusCode RawDataEmcMcHitCnv::initialize()
{
    std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
    std::string RawDataInputSvc_Name("RawDataInputSvc");
    std::string RawDataEmcMcHitCnv_Name("RawDataEmcMcHitCnv");
    
    // for Mutil-thread by tianhl
    //ConversionSvc*  pCnvSvc = 0;
    //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
    SmartIF<IService> pCnvSvc(conversionSvc());
      if  (isGaudiThreaded(pCnvSvc->name())){
        PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataEmcMcHitCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
    //}

  MsgStream log(messageService(), RawDataEmcMcHitCnv_Name.c_str());

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
    log << MSG::ERROR << "RawDataEmcCnv: Cant cast to RawDataCnvSvc " << endreq; 
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


StatusCode RawDataEmcMcHitCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
  // Purpose and Method:  This method does nothing other than announce it has
  //   been called.

  //MsgStream log(msgSvc(), "RawDataEmcMcHitCnv");
  //log << MSG::DEBUG << "RawDataEmcMcHitCnv::updateObj" << endreq;
  return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataEmcMcHitCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{

  //This converter will create an empty EmcMcHitCol on the TDS
  EmcMcHitCol *emcMcHitCol = new EmcMcHitCol;
  pObj = emcMcHitCol;
   
  RAWEVENT *evt = m_inputSvc->currentEvent(); 
  if (evt == NULL) {
      //MsgStream log(msgSvc(), "RawDataEmcMcHitCnv");
      //log << MSG::ERROR << "RawDataCnv has no event!" << endreq; 
      return StatusCode::FAILURE;
  }
  
  EmcMcHit* emcMcHit;
  EmcTruth_t m_EmcTruth;

  assert((evt->getEmcTruth().size()%8) == 0);

  std::vector<uint32_t>::const_iterator iter = evt->getEmcTruth().begin();
  std::vector<uint32_t>::const_iterator eiter = evt->getEmcTruth().end();

  for (int emcMcHitId = 0; iter != eiter; emcMcHitId++) {
    // retrieve the McTruth data
    m_emcMcHitBuilder.unPack(iter, eiter, m_EmcTruth);
    // construct the identifier
    Identifier ident = EmcID::crystal_id(m_EmcTruth.partId, m_EmcTruth.numTheta, m_EmcTruth.numPhi);
    // construct the EmcMcHit
    emcMcHit = new EmcMcHit(ident, m_EmcTruth.trackIndex,
	                  m_EmcTruth.x/(m_emcMcHitBuilder.m_xCoeff*1.0),
			  m_EmcTruth.y/(m_emcMcHitBuilder.m_yCoeff*1.0),
			  m_EmcTruth.z/(m_emcMcHitBuilder.m_zCoeff*1.0),
			  m_EmcTruth.px/(m_emcMcHitBuilder.m_pxCoeff*1.0),
			  m_EmcTruth.py/(m_emcMcHitBuilder.m_pyCoeff*1.0),
			  m_EmcTruth.pz/(m_emcMcHitBuilder.m_pzCoeff*1.0),
			  m_EmcTruth.totalEdep/(m_emcMcHitBuilder.m_totalEdepCoeff*1.0)  );
    // And add the stuff to the container
    emcMcHitCol->push_back(emcMcHit);
  }

  return StatusCode::SUCCESS; 

}

StatusCode RawDataEmcMcHitCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)  
{
   // convert PixelRaw in the container into ByteStream
  //MsgStream log(messageService(), "RawDataEmcMcHitCnv");
  
  WriteRawEvent*& re = m_RawDataAccess->getRawEvent(); 
  
  if (re == 0) {
     //log << " get RawEvent failed !" << endreq;
     return StatusCode::FAILURE; 
  }

  SmartDataPtr<EmcMcHitCol> mcHitCol(dataProvider(), EventModel::MC::EmcMcHitCol);

  if (mcHitCol == 0) {
     //log << "no EmcMcHitCol found" << endreq;
     return StatusCode::FAILURE;
  }
  
  StatusCode sc = m_emcMcHitBuilder.pack(mcHitCol, re);

  return sc; 

}

