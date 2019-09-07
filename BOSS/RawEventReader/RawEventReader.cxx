#include "RawEventReader/RawEventReader.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "RawDataCnv/EventManagement/RAWEVENT.h"
#include "RawDataCnv/Util/MdcConverter.h"
#include "RawDataCnv/Util/TofConverter.h"
#include "RawDataCnv/Util/EmcConverter.h"
#include "RawDataCnv/Util/MucConverter.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "Identifier/TofID.h"
#include "Identifier/EmcID.h"
#include "Identifier/MucID.h"
#include "IRawFile/RawFileExceptions.h"

#include "RawFile/RawFileReader.h"
#include "RawEvent/DigiEvent.h"

#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"

/////////////////////////////////////////////////////////////////////////////

RawEventReader::RawEventReader(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  m_mdcCnv = MdcConverter::instance();
  m_tofCnv = TofConverter::instance();
  m_emcCnv = EmcConverter::instance();
  m_mucCnv = MucConverter::instance();

  declareProperty("MdcDigi",m_mdcdigi=1);
  declareProperty("TofDigi",m_tofdigi=1);
  declareProperty("EmcDigi",m_emcdigi=1);
  declareProperty("MucDigi",m_mucdigi=1);
  declareProperty("InputFiles",m_inputFiles);
  
}

RawEventReader::~RawEventReader()
{
    MdcConverter::destroy();
    TofConverter::destroy();
    EmcConverter::destroy();
    MucConverter::destroy();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RawEventReader::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
  
  m_raw_event = new RAWEVENT;

  m_mdcCnv->init(0, 999999);

  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc=svcLocator->service("EventDataSvc", m_evtSvc);
  if (sc.isFailure())
    std::cout<<"Could not accesss EventDataSvc!"<<std::endl;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RawEventReader::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  DigiEvent* aDigiEvent = new DigiEvent;
  StatusCode sc = m_evtSvc->registerObject("/Event/Digi",aDigiEvent);
     if(sc!=StatusCode::SUCCESS) {
       std::cout<< "Could not register DigiEvent" <<std::endl;
           }
 
  readEvent();
  

  if(m_mdcdigi)
  {
    const BufferHolder& mdcBuf = m_raw_event->getMdcBuf();
    
    MdcDigiCol* mdcDigiCol = new MdcDigiCol;
    m_mdcCnv->convert(mdcBuf, mdcDigiCol);
    /*MdcDigiCol::const_iterator mdcDigi = mdcDigiCol->begin();
    for(; mdcDigi!=mdcDigiCol->end(); mdcDigi++ )
    {
      (*mdcDigi)->getTrackIndex();
      std::cout<<"mdc digi: "<<  "  trackIndex: "<<(*mdcDigi)->getTrackIndex()<<std::endl;
    }*/
    //register MdcDigiCol to TDS...
    StatusCode scMdc = m_evtSvc->registerObject("/Event/Digi/MdcDigiCol", mdcDigiCol);
    if( scMdc!=StatusCode::SUCCESS ) {
      std::cout<< "Could not register MDC digi collection" <<std::endl;
    }
  }


  if(m_tofdigi)
  {
    const BufferHolder& tofBuf = m_raw_event->getTofBuf();
    const BufferHolder& etfBuf = m_raw_event->getEtfBuf();
    
    TofDigiCol* tofDigiCol = new TofDigiCol;
    //    m_tofCnv->convert(tofBuf, tofDigiCol);
    m_tofCnv->convert(tofBuf, etfBuf, tofDigiCol);

    /*TofDigiCol::const_iterator tofDigi = tofDigiCol->begin();
    for(; tofDigi!=tofDigiCol->end(); tofDigi++ )
    {
      (*tofDigi)->getTrackIndex();
      const Identifier ident = (*tofDigi)->identify();
      std::cout<<"print tof digi in RawEventReader"<<std::endl;
      std::cout<<"partId: "<<TofID::barrel_ec(ident);
      std::cout<<"  layer: "<<TofID::layer(ident);
      std::cout<<"  scinNb: "<<TofID::phi_module(ident);
      std::cout<<"  end: "<<TofID::end(ident);
      std::cout<<"  charge: "<<(*tofDigi)->getChargeChannel();
      std::cout<<"  time: "<<(*tofDigi)->getTimeChannel()<<std::endl;
    }*/

    //register TofDigiCol to TDS...
    StatusCode scTof = m_evtSvc->registerObject("/Event/Digi/TofDigiCol", tofDigiCol);
    if( scTof!=StatusCode::SUCCESS ) {
      std::cout<< "Could not register TOF digi collection" <<std::endl;
    }
  }

  if(m_emcdigi)
  {
    const BufferHolder& emcBuf = m_raw_event->getEmcBuf();
    
    EmcDigiCol* emcDigiCol = new EmcDigiCol;
    m_emcCnv->convert(emcBuf, emcDigiCol);
    //register EmcDigiCol to TDS...
    StatusCode scEmc = m_evtSvc->registerObject("/Event/Digi/EmcDigiCol", emcDigiCol);
    if( scEmc!=StatusCode::SUCCESS ) {
      std::cout<< "Could not register EMC digi collection" <<std::endl;
    }
  }

  if(m_mucdigi)
  {
    const BufferHolder& mucBuf = m_raw_event->getMucBuf();
    
    MucDigiCol* mucDigiCol = new MucDigiCol;
    m_mucCnv->convert(mucBuf, mucDigiCol);
    //register MucDigiCol to TDS...
    StatusCode scMuc = m_evtSvc->registerObject("/Event/Digi/MucDigiCol", mucDigiCol);
    if( scMuc!=StatusCode::SUCCESS ) {
      std::cout<< "Could not register MUC digi collection" <<std::endl;
    }
  }
  // clear the Memory Buffer, so that m_raw_event can be used directly during next event loop
  m_raw_event->reset();

  return StatusCode::SUCCESS;
}

StatusCode RawEventReader::readEvent()
{
  static RawFileReader* fileReader = new RawFileReader(m_inputFiles);
  const uint32_t* fragment;

  try {
      fragment = fileReader->nextEvent();
  }
  catch (RawFileException& ex) {
          ex.print();
  }
  catch (...) {
          std::cerr << std::endl << "Uncaught unknown exception" << std::endl;
  }

  RawEvent f(fragment);
  uint32_t *robs[64];
  int nrobs = eformat::get_robs(fragment, (const uint32_t **)robs, 64);
  for (int robi = 0; robi < nrobs; robi++) {
    eformat::ROBFragment<uint32_t*> rob(robs[robi]);
    if ((rob.rod_detev_type() & 0x2) != 0) continue;  //bad data caogf add
    uint32_t* dataptr = NULL;
    rob.rod_data(dataptr);
    uint32_t source_id_number = rob.rod_source_id();
    source_id_number <<= 8;
    source_id_number >>= 24;
    switch(source_id_number) {
      case 161:
        m_raw_event->addReMdcDigi(dataptr, rob.rod_ndata());
        break;
      case 163:
        m_raw_event->addReEmcDigi(dataptr, rob.rod_ndata());
        break;
      case 162:
        m_raw_event->addReTofDigi(dataptr, rob.rod_ndata());
        break;
      case 164:
        m_raw_event->addReMucDigi(dataptr, rob.rod_ndata());
        break;
      case 167:
        m_raw_event->addReEtfDigi(dataptr, rob.rod_ndata());
        break;
    }
  }
  return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RawEventReader::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  //TODO: release ...
  delete m_raw_event;
  //delete fileReader;

  return StatusCode::SUCCESS;
}
