//====================================================================
//      RawDataMcParticleCnv.cxx
//====================================================================
//
// Description: A converter class to unpack Event Filter packed raw   
//              event persistent data and place it in the Transient    
//              Data Store of Athena.
//
//--------------------------------------------------------------------

// Include files.
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "EventModel/EventModel.h"
#include "McTruth/McParticle.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "RawDataCnv/EventManagement/RawEvent.h" 
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/RawDataMcParticleCnv.h"
                                                                                                                             
using Event::McParticle;
using Event::McParticleCol;


extern const CLID& CLID_McParticleCol;

// Constructor.
RawDataMcParticleCnv::RawDataMcParticleCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
  //init();
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataMcParticleCnv::classID()
{
  return CLID_McParticleCol;
}

StatusCode RawDataMcParticleCnv::initialize()
{

    std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
    std::string RawDataInputSvc_Name("RawDataInputSvc");
    std::string RawDataMcParticleCnv_Name("RawDataMcParticleCnv");
    
    // for Mutil-thread by tianhl
    //ConversionSvc*  pCnvSvc = 0;
    //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
    SmartIF<IService> pCnvSvc(conversionSvc());  
    if  (isGaudiThreaded(pCnvSvc->name())){
        PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataMcParticleCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
    //}
    
  MsgStream log(messageService(), RawDataMcParticleCnv_Name.c_str());

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
    log << MSG::ERROR << "RawDataMcParticleCnv: Cant cast to RawDataCnvSvc " << endreq; 
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


StatusCode RawDataMcParticleCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
  // Purpose and Method:  This method does nothing other than announce it has
  //   been called.

  //MsgStream log(msgSvc(), "RawDataMcParticleCnv");
  //log << MSG::DEBUG << "RawDataMcParticleCnv::updateObj" << endreq;
  return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataMcParticleCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  //MsgStream log(msgSvc(), "RawDataMcParticleCnv");

  //This converter will create an empty McParticleCol on the TDS
  McParticleCol *mcParticleCol = new McParticleCol;
  pObj = mcParticleCol;
   
  RAWEVENT *evt = m_inputSvc->currentEvent(); 
  if (evt == NULL) {
      //log << MSG::ERROR << "RawDataCnv has no event!" << endreq; 
      return StatusCode::FAILURE;
  }

  const BufferHolder& mcParBuf = evt->getMcParBuf();
  if (mcParBuf.nBuf() == 0) {
    //cerr << "no McParticle Data Buffer found!!!" << endl;
    return StatusCode::FAILURE;
  }
  uint32_t* buf = mcParBuf(0);

  uint32_t nParticle = (buf[0] >> 16);
  //uint32_t nVertex   = (buf[0] & 0xFFFF);
  double*  vPointer  = (double*)(buf + nParticle*11 + 1);

  for (uint32_t i = 0; i < nParticle; i++) {
    McParticle* mcPar = new McParticle;
    m_mcParticleBuilder.unPack((buf+1+i*11), vPointer, mcPar);
    mcParticleCol->push_back(mcPar);
  }
  
  // Get primary McParticles
  SmartRefVector<McParticle> primaryParticleCol;
  McParticleCol::iterator iter_particle = mcParticleCol->begin();
  for ( ; iter_particle != mcParticleCol->end(); iter_particle++) {
    if ((*iter_particle)->primaryParticle()) {
      McParticle* mcPart = (McParticle*)(*iter_particle);
      primaryParticleCol.push_back(mcPart);
    }
  }

  if (primaryParticleCol.empty()) {
    //log << MSG::WARNING << "createObj error: no primary particle found!" << endreq;
  }

  // Add mother particle recursively
  SmartRefVector<McParticle>::iterator iter_primary = primaryParticleCol.begin();
  //std::cout << "primaryParticleCol.size() = " << primaryParticleCol.size() << std::endl;
  for ( ; iter_primary != primaryParticleCol.end(); iter_primary++) {

    if (!(*iter_primary)->leafParticle())
      addMother((*iter_primary), mcParticleCol);
  }

  return StatusCode::SUCCESS; 
}

StatusCode RawDataMcParticleCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/)
{
   // convert PixelRaw in the container into ByteStream
  //MsgStream log(messageService(), "RawDataMcParticleCnv");
  
  WriteRawEvent*& re = m_RawDataAccess->getRawEvent(); 
  if (re == 0) {
     //log << " get RawEvent failed !" << endreq;
     return StatusCode::FAILURE; 
  }

  SmartDataPtr<McParticleCol> mcParticleCol(dataProvider(), EventModel::MC::McParticleCol);
  if (mcParticleCol == 0) {
     //log << "no McParticleCol found" << endreq;
     return StatusCode::FAILURE;
  }
 
  return m_mcParticleBuilder.pack(mcParticleCol, re);
}

void RawDataMcParticleCnv::addMother(McParticle* currentParticle, McParticleCol *particleCol)
{

  if (currentParticle->leafParticle()) return;

  bool found = false;
  McParticleCol::iterator iter = particleCol->begin();
  //std::cout << "particleCol->size() = " << particleCol->size() << std::endl;
  for ( ; iter != particleCol->end(); iter++) {
    if (currentParticle->vertexIndex1() == (*iter)->vertexIndex0()) {
      found = true;
      (*iter)->setMother(currentParticle);
      currentParticle->addDaughter(*iter);
      addMother((*iter), particleCol);
    }
  }

  if (!found) 
    std::cout << "RawDataMcParticleCnv::addMother: inconsistency was found!" << std::endl;
}

