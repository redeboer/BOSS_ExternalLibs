//===================================================================
//  Implementation of RawDataInputSvc
//             
//===================================================================
//
 
// Include files.
//#include <fcntl.h>
#include "RawDataCnv/RawDataInputSvc.h"
#include "RawFile/RawFileReader.h"
#ifndef OnlineMode
#include "NetDataReader/NetDataReader.h"
#include "DistBossUtil/GetPropertyValue.h"
#endif
#include "RawDataCnv/EventManagement/RawEvent.h"  
#include "IRawFile/RawFileExceptions.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"

// Constructor.
RawDataInputSvc::RawDataInputSvc(const std::string& name, ISvcLocator* svcloc)
   :   
  Service(name,svcloc), 
  m_re(0),
  m_reader(0),
  m_sds(0),
  m_robs(0)
{
   // Get a pointer to the Job Options Service
   IJobOptionsSvc* jobSvc;
   Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);

   PropertyMgr m_propMgr;
   // 0-NormalNetMode, 1-OnlineMode, 2-OfflineMode, 3-DistBossMode
   m_propMgr.declareProperty("RunMode",    m_mode = 2);
   m_propMgr.declareProperty("KeepRandomTrigEvt", m_keepRdm = true);
   m_propMgr.declareProperty("InputFiles", m_inputFiles);

   jobSvc->setMyProperties("RawDataInputSvc", &m_propMgr);
}
 
// Destructor.
RawDataInputSvc::~RawDataInputSvc()
{
  //MsgStream log(messageService(), name() );
  //log << MSG::DEBUG << "RawDataInputSvc Destructor called " << endreq;
}
 
// Open the first input file and read the first event.
StatusCode RawDataInputSvc::initialize()
{
   //MsgStream log(messageService(), name() );
   //log << MSG::DEBUG << " in RawDataInputSvc::initialize()" << endreq;

   //StatusCode sc=  IRawDataInputSvc::initialize();
   //if(!sc.isSuccess()) {
   //  log << MSG::ERROR << "failed to initialize IRawDataInputSvc" << endreq;
   //  return sc;
   //}

#ifndef OnlineMode
   if ( m_mode > 1 ) {
      try {
	 if ( m_mode == 2 ) {  //OfflineMode
	    if ( m_inputFiles.empty() ) return StatusCode::SUCCESS;
	    m_reader = new RawFileReader(m_inputFiles); 
	 }
	 else if ( m_mode == 3 ) {  //DistBossMode
	    DistBoss::GetPropertyValue<std::string>("DistBoss", "ServerName", m_evtServer);
	    m_reader = new NetDataReader(m_evtServer+"/RawEvtSvc");
	 }
	 else {
	    throw RawExMessage("RawDataInputSvc: Invalid RunMode!");
	 }
      }
      catch (RawFileException& ex) {
	 ex.print();
	 return StatusCode::FAILURE;
      }

      m_re = new RAWEVENT;

      m_sds =  new const uint32_t*[64];
      m_robs = new const uint32_t*[256];
   }
#endif

   return StatusCode::SUCCESS;
}

StatusCode RawDataInputSvc::finalize() {
  // clean up 
  if ( m_re ) {
    delete m_re;
    m_re = 0;
  }

  if ( m_reader ) {
    delete m_reader;
    m_reader = 0;
  }

  if ( m_sds )  delete m_sds;
  if ( m_robs ) delete m_robs;

  return StatusCode::SUCCESS;
}
 
// Read the next event.
RAWEVENT* RawDataInputSvc::nextEvent() {

  //MsgStream log(messageService(), name() );
#ifndef OnlineMode
  //OfflineMode and DistBossMode
   m_re->reset();

   try {
      RawEvent f;
      const uint32_t* ef = 0;
      do {
         f.assign( m_reader->nextEvent());
         if (!f.check()) {
            std::cerr << "Found invalid event (traceback):" << std::endl;
            std::exit(1);
         }
         //1.print basic event information
         //     log << MSG::DEBUG<< "run" << f.run_no() << " [Event No. #" << f.global_id()
         //	<< "] " << f.fragment_size_word() << " words in "
         //	<< f.nchildren() << " subdetectors "
         //	<< endreq;

         //fucd: get event filter information
         f.event_filter_info(ef);
         if ( !ef ) {
            //log << MSG::ERROR << "Event Filter Data Failed!!!" << endreq;
            exit(1);
         }
         else {
            //log << MSG::DEBUG<< "Event Filter Information*********" <<std::hex<<endreq
            //   <<*ef<< "  "<<*(ef+1)<<"  "<<*(ef+2)<<"  "<<*(ef+3)<<std::dec<<endreq;
            if ( ! m_keepRdm && ((*ef)>>31) != 0 ) {
               continue;
            }
         }
         break;
      } while (true);

      m_re->setRunNo(f.run_no());
      m_re->setEventNo(f.global_id());
      m_re->setTime(f.time());
      m_re->addReHltRaw((uint32_t*)ef, 4);

      //set HV status and get all robs
      uint32_t hv_status = 0;
      int      nrobs = 0;
      int      nsds  = f.children(m_sds, 64);
      for ( int sdi = 0; sdi < nsds; ++sdi )  {
	 eformat::SubDetectorFragment<const uint32_t*> sd(m_sds[sdi]);

	 if ( sd.nspecific() != 0 ) {
	    const uint32_t* specific_header;
	    sd.specific_header(specific_header);
	    uint32_t source_id_number = sd.source_id();
	    source_id_number <<= 8;
	    source_id_number >>= 24;
	    switch( source_id_number ) {
	       case 161:
		  hv_status |= ((8 | ((*specific_header)&0x7)) << 8);
		  break;
	       case 162:
		  hv_status |= ((8 | ((*specific_header)&0x7)) << 4);
		  break;
	       case 164:
		  hv_status |= (8 | ((*specific_header)&0x7));
		  break;
	       default:
		  break;
	    }
	 }

	 nrobs += eformat::get_robs(m_sds[sdi], m_robs+nrobs, 256-nrobs);
      }

      m_re->setFlag1( hv_status );

      //log << MSG::INFO << " nrobs: " << nrobs << endreq;
      for (int robi = 0; robi < nrobs; robi++) {
	 eformat::ROBFragment<uint32_t*> rob((uint32_t*)m_robs[robi]);
	 //uint32_t detev_type = rob.rod_detev_type();
	 if ((rob.rod_detev_type() & 0x2) != 0) continue;  //bad data
	 uint32_t* dataptr = NULL;
	 rob.rod_data(dataptr);

	 //log << MSG::DEBUG<< "addReHltRaw" << endreq;
	 uint32_t source_id_number = rob.rod_source_id();
	 //std::cout<<"#####source_id_number#####"<<source_id_number<<std::endl;
	 source_id_number <<= 8;
	 source_id_number >>= 24;
	 //std::cout<<"#####(source_id_number<<24)>>29#####"<<source_id_number<<std::endl;
	 //be careful here!!!
	 switch(source_id_number) {
	    case 161:
	       m_re->addReMdcDigi(dataptr, rob.rod_ndata());
	       break;
	    case 163:
	       m_re->addReEmcDigi(dataptr, rob.rod_ndata());
	       break;
	    case 162:
	       m_re->addReTofDigi(dataptr, rob.rod_ndata());
	       break;
	    case 167:  //ETF(MRPC)
	       m_re->addReEtfDigi(dataptr, rob.rod_ndata());
	       break;
	    case 164:
	       m_re->addReMucDigi(dataptr, rob.rod_ndata());
	       break;
	    case 165:  // trigger !!!
	       //std::cout << "Get Trigger Data -" << std::endl;
	       //for (int i = 0; i < rob.rod_ndata(); i++) {
	       //  std::cout << "\t0x" << std::hex << dataptr[i] << std::dec << std::endl;
	       //}
	       m_re->addReTrigGTD(dataptr, rob.rod_ndata());
	       break;
	    case 166:
	       m_re->addReZddDigi(dataptr, rob.rod_ndata());
	       break;
	    case 124:  // EventFilter
	       m_re->addReHltRaw(dataptr, rob.rod_ndata());
	       break;
	    case 241:  // McParticle
	       m_re->addMcParticle(dataptr, rob.rod_ndata());
	       break;
	    default:
	       //log << MSG::ERROR << "no such subdetector type: " << source_id_number << endreq;
	       break;
	 }
      }
   }
   catch (RawFileException& ex) {
      ex.print();
      if ( m_reader->stat()&4 ) {  // fstream badbit is set
          std::cerr << std::endl << "reading ifstream error !!!" << std::endl;
          exit(1);
      }
      delete m_re;
      m_re = NULL;
   }
   catch (eformat::Issue& ex) {
      std::cerr << std::endl << "Uncaught eformat issue: " << ex.what() << std::endl;
   }
   catch (ers::Issue& ex) {
      std::cerr << std::endl << "Uncaught ERS issue: " << ex.what() << std::endl;
   }
   catch (std::exception& ex) {
      std::cerr << std::endl << "Uncaught std exception: " << ex.what() << std::endl;
   }
   catch (...) {
      std::cerr << std::endl << "Uncaught unknown exception" << std::endl;
   }
#endif

   return m_re;
}


/**  Get a pointer to the current event.
 */ 
RAWEVENT* RawDataInputSvc::currentEvent()
{
  // Return a pointer to the  raw event.
  //std::cout << "RawDataCnv -> Return a pointer to the  raw event." << std::endl;
  //std::cout << "It is the first time to call the currentEvent()" << std::endl;
  return m_re; 
}

bool RawDataInputSvc::setCurrentEvent(RAWEVENT* m_onlinere)
{
   //std::cout << "setCurrentEvent(RAWEVENT* m_onlinere)" << std::endl;
   return (m_re = m_onlinere);
}
