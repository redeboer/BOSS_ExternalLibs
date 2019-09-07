//===================================================================
//  Implementation of RawDataOutputSvc
//  Revision:  July 11, 2002
//      Modified for RawFileReader
//             
//===================================================================
//
 
// Include files.
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "RawDataCnv/RawDataOutputSvc.h"
 
// Constructor.
RawDataOutputSvc::RawDataOutputSvc(const std::string& name, ISvcLocator* svcloc)
   : IRawDataOutputSvc(name,svcloc),  
     m_fw(0)
{
  PropertyMgr m_propMgr;
  m_propMgr.declareProperty("OutputFile", m_outputFile = "offline.raw");

  IJobOptionsSvc* jobSvc; 
  Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);
  jobSvc->setMyProperties(name, &m_propMgr);

  m_buffer = new uint32_t[64*1024];  // 256k bytes
}
 
// Destructor.
RawDataOutputSvc::~RawDataOutputSvc()
{
  MsgStream log(messageService(), name() );
  log << MSG::DEBUG << "RawDataOutputSvc Destructor called " << endreq;

  delete [] m_buffer;
}
 
 
StatusCode RawDataOutputSvc::finalize()
{
   // clean up
   if ( m_fw ) {
      delete m_fw;
      m_fw = 0;
   }
 
   return StatusCode::SUCCESS;
}
 
// Open the first input file and read the first event.
StatusCode RawDataOutputSvc::initialize()
{
   MsgStream log(messageService(), name() );

   // Open the first output file for writing.
   m_fw = new RawFileWriter(m_outputFile);

   log << MSG::DEBUG << "Opened output File " << m_outputFile << endreq;
  
   return StatusCode::SUCCESS;
}
 
// Write the next event.
bool  RawDataOutputSvc::putEvent(WriteRawEvent* re)
{
  MsgStream log(messageService(), name() );

  //uint32_t evt_size_w = re->size_word();

  const eformat::write::node_t* top = re->bind();
  if (eformat::write::copy(*top, m_buffer, 64*1024) == 0) {
    log << MSG::ERROR << "failed to copy Event to the buffer !" << endreq;
    return false;
  }

  try {
    RawEvent evt(m_buffer);
    evt.check_tree();
  }
  catch (std::exception& ex) {
    std::cerr << "Uncaught std exception: " << ex.what() << std::endl;
    std::exit(1);
  }
  catch (...) {
    std::cerr << std::endl << "Uncaught unknown exception" << std::endl;
    std::exit(1);
  }

  m_fw->writeEvent(m_buffer);

  return true; 
}
