//====================================================================
//      RawDataMucMcHitCnv.cxx
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
#include "Identifier/MucID.h"
#include "McTruth/MucMcHit.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "RawDataCnv/EventManagement/RawEvent.h" 
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/RawDataMucMcHitCnv.h"


using Event::MucMcHit;
using Event::MucMcHitCol;

// REMOVE THIS LINE AFTER "PACKEDRAWEVENT_StorageType" IS ADDED TO
// THE FILE "GaudiKernel/ClassID.h"
extern const CLID& CLID_MucMcHitCol;

// Constructor.
RawDataMucMcHitCnv::RawDataMucMcHitCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
  //init();
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataMucMcHitCnv::classID()
{
  return CLID_MucMcHitCol;
}

StatusCode RawDataMucMcHitCnv::initialize()
{
    std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
    std::string RawDataInputSvc_Name("RawDataInputSvc");
    std::string RawDataMucMcHitCnv_Name("RawDataMucMcHitCnv");
    
    // for Mutil-thread by tianhl
    //ConversionSvc*  pCnvSvc = 0;
    //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
    SmartIF<IService> pCnvSvc(conversionSvc());  
    if  (isGaudiThreaded(pCnvSvc->name())){
        PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataMucMcHitCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
    //}
    

  MsgStream log(messageService(), RawDataMucMcHitCnv_Name.c_str());

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
    log << MSG::ERROR << "RawDataMucCnv: Cant cast to RawDataCnvSvc " << endreq; 
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


StatusCode RawDataMucMcHitCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
  // Purpose and Method:  This method does nothing other than announce it has
  //   been called.

  //MsgStream log(msgSvc(), "RawDataMucMcHitCnv");
  //log << MSG::DEBUG << "RawDataMucMcHitCnv::updateObj" << endreq;
  return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataMucMcHitCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  //MsgStream log(msgSvc(), "RawDataMucMcHitCnv");

  //This converter will create an empty MucMcHitCol on the TDS
  MucMcHitCol *mucMcHitCol = new MucMcHitCol;
  pObj = mucMcHitCol;
   
  RAWEVENT *evt = m_inputSvc->currentEvent(); 
  if (evt == NULL) {
      //log << MSG::ERROR << "RawDataCnv has no event!" << endreq; 
      return StatusCode::FAILURE;
  }
  
  assert((evt->getMucTruth().size()%7) == 0);

  MucMcHit* mucMcHit;
  MucTruth_t m_MucTruth;

  std::vector<uint32_t>::const_iterator iter = evt->getMucTruth().begin();
  std::vector<uint32_t>::const_iterator eiter = evt->getMucTruth().end();

  for (int mucMcHitId = 0; iter != eiter; mucMcHitId++) {
    //retrieve the MucTruth data
    m_mucMcHitBuilder.unPack(iter, eiter, m_MucTruth);
    //construct the identifier
    const Identifier ident = MucID::channel_id
      (m_MucTruth.partId, m_MucTruth.segId, m_MucTruth.gapId, m_MucTruth.stripId);
    //construct the MucMcHit
    mucMcHit = new MucMcHit(ident, m_MucTruth.trackIndex%1000,
	               m_MucTruth.x/(m_mucMcHitBuilder.m_xCoeff*1.0),
		       m_MucTruth.y/(m_mucMcHitBuilder.m_yCoeff*1.0),
		       m_MucTruth.z/(m_mucMcHitBuilder.m_zCoeff*1.0),
		       m_MucTruth.px/(m_mucMcHitBuilder.m_pxCoeff*1.0),
		       m_MucTruth.py/(m_mucMcHitBuilder.m_pyCoeff*1.0),
		       m_MucTruth.pz/(m_mucMcHitBuilder.m_pzCoeff*1.0)  );
    // And add the stuff to the container
    mucMcHitCol->push_back(mucMcHit);
  }

  return StatusCode::SUCCESS; 

}

StatusCode RawDataMucMcHitCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)  
{
   // convert PixelRaw in the container into ByteStream
  //MsgStream log(messageService(), "RawDataMucMcHitCnv");
  
  WriteRawEvent*& re = m_RawDataAccess->getRawEvent(); 
  
  if (re == 0) {
     //log << " get RawEvent failed !" << endreq;
     return StatusCode::FAILURE; 
  }

  SmartDataPtr<MucMcHitCol> mcHitCol(dataProvider(), EventModel::MC::MucMcHitCol);

  if (mcHitCol == 0) {
     //log << "no MucMcHitCol found" << endreq;
     return StatusCode::FAILURE;
  }
  
  StatusCode sc = m_mucMcHitBuilder.pack(mcHitCol, re);

  return sc; 

}

