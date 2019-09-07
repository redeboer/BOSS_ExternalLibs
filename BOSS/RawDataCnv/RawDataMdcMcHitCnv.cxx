//====================================================================
//      RawDataMdcMcHitCnv.cxx
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
#include "Identifier/MdcID.h"
#include "McTruth/MdcMcHit.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "RawDataCnv/EventManagement/RawEvent.h" 
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/RawDataMdcMcHitCnv.h"

using Event::MdcMcHit;
using Event::MdcMcHitCol;


extern const CLID& CLID_MdcMcHitCol;

// Constructor.
RawDataMdcMcHitCnv::RawDataMdcMcHitCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
  //init();
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataMdcMcHitCnv::classID()
{
  return CLID_MdcMcHitCol;
}

StatusCode RawDataMdcMcHitCnv::initialize()
{

    std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
    std::string RawDataInputSvc_Name("RawDataInputSvc");
    std::string RawDataMdcMcHitCnv_Name("RawDataMdcMcHitCnv");
    
    // for Mutil-thread by tianhl
    //ConversionSvc*  pCnvSvc = 0;
    //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
    SmartIF<IService> pCnvSvc(conversionSvc());  
    if  (isGaudiThreaded(pCnvSvc->name())){
        PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataMdcMcHitCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
    //}
    
  MsgStream log(messageService(), RawDataMdcMcHitCnv_Name.c_str());

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
    log << MSG::ERROR << "RawDataMdcCnv: Cant cast to RawDataCnvSvc " << endreq; 
    return StatusCode::FAILURE;
  }

  IService* svc = 0;
  sc = serviceLocator()->getService(RawDataInputSvc_Name.c_str(), svc);
  if (sc != StatusCode::SUCCESS ) {
    log << MSG::WARNING << "Cant get RawDataInputSvc " << endreq;
    return sc ;
  }

  m_inputSvc = dynamic_cast<RawDataInputSvc*> (svc);
  if (m_inputSvc == 0 ) {
    log << MSG::WARNING << "Cant cast to RawDataInputSvc " << endreq;
    return StatusCode::FAILURE ;
  }

  return StatusCode::SUCCESS; 
}


StatusCode RawDataMdcMcHitCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
  // Purpose and Method:  This method does nothing other than announce it has
  //   been called.

  //MsgStream log(msgSvc(), "RawDataMdcMcHitCnv");
  //log << MSG::DEBUG << "RawDataMdcMcHitCnv::updateObj" << endreq;
  return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataMdcMcHitCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  //MsgStream log(msgSvc(), "RawDataMdcMcHitCnv");

  //This converter will create an empty MdcMcHitCol on the TDS
  MdcMcHitCol *mdcMcHitCol = new MdcMcHitCol;
  pObj = mdcMcHitCol;
   
  RAWEVENT *evt = m_inputSvc->currentEvent(); 
  if (evt == NULL) {
      //log << MSG::ERROR << "RawDataCnv has no event!" << endreq; 
      return StatusCode::FAILURE;
  }

  assert((evt->getMdcTruth().size())%6 == 0);

  MdcTruth_t m_MdcTruth;
  MdcMcHit* mdcMcHit;

  std::vector<uint32_t>::const_iterator iter = evt->getMdcTruth().begin();
  std::vector<uint32_t>::const_iterator eiter = evt->getMdcTruth().end();

  for (int mdcMcHitId = 0; iter != eiter; mdcMcHitId++) {
    // retrieve the MdcTruth data
    m_mdcMcHitBuilder.unPack(iter, eiter, m_MdcTruth);
    //construct the identifier
    const Identifier ident =  MdcID::wire_id (m_MdcTruth.layerId, m_MdcTruth.cellId);
    /// construct the MdcMcHit
    mdcMcHit = new MdcMcHit(ident, m_MdcTruth.trackIndex,
                       m_MdcTruth.x/(m_mdcMcHitBuilder.m_xCoeff*1.0),
		       m_MdcTruth.y/(m_mdcMcHitBuilder.m_yCoeff*1.0),
		       m_MdcTruth.z/(m_mdcMcHitBuilder.m_zCoeff*1.0),
                       m_MdcTruth.driftD/(m_mdcMcHitBuilder.m_driftDCoeff*1.0),
		       m_MdcTruth.edep/(m_mdcMcHitBuilder.m_edepCoeff*1.0),
		       m_MdcTruth.posFlag );
    // And add the stuff to the container
    mdcMcHitCol->push_back(mdcMcHit);
  }

  return StatusCode::SUCCESS; 
}

StatusCode RawDataMdcMcHitCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)  
{
   // convert PixelRaw in the container into ByteStream
  //MsgStream log(messageService(), "RawDataMdcMcHitCnv");
  
  WriteRawEvent*& re = m_RawDataAccess->getRawEvent(); 
  
  if (re == 0) {
     //log << " get RawEvent failed !" << endreq;
     return StatusCode::FAILURE; 
  }

  SmartDataPtr<MdcMcHitCol> mdcHitCol(dataProvider(), EventModel::MC::MdcMcHitCol);

  if (mdcHitCol == 0) {
     //log << "no MdcMcHitCol found" << endreq;
     return StatusCode::FAILURE;
  }
  /*else {
     log << MSG::INFO << endreq << "Detailed dump of 1st event: " << endreq;
     int nMcHit = 0;
     MdcMcHitCol::const_iterator pMdcMcHit = mdcHitCol->begin();
     for ( ; pMdcMcHit != mdcHitCol->end(); pMdcMcHit++) {
        log << MSG::INFO << "McHit " << nMcHit++ << " "
	    << (*pMdcMcHit)->getTrackIndex() << "  "
	    << (*pMdcMcHit)->identify() << "  "
	    << (*pMdcMcHit)->getDepositEnergy() << "  "
            << (*pMdcMcHit)->getDriftDistance() << "  "
	    << (*pMdcMcHit)->getPositionX() << "  "
	    << (*pMdcMcHit)->getPositionY() << "  "
	    << (*pMdcMcHit)->getPositionZ() << endreq;
     }
  }*/
  
  StatusCode sc = m_mdcMcHitBuilder.pack(mdcHitCol, re);

  return sc; 

}

