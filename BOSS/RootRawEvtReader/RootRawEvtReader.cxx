#include "RootRawEvtReader/RootRawEvtReader.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/ClassID.h"

#include "RawEvent/DigiEvent.h"   //TDS object
#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"

//Digi
#include "MdcRawEvent/MdcDigi.h"
#include "EmcRawEvent/EmcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "MucRawEvent/MucDigi.h"

#include "RootEventData/TMdcDigi.h"
#include "RootEventData/TTofDigi.h"
#include "RootEventData/TEmcDigi.h"
#include "RootEventData/TMucDigi.h"


#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"

/////////////////////////////////////////////////////////////////////////////

RootRawEvtReader::RootRawEvtReader(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  
  declareProperty("MdcDigi",m_mdcDigi=1);
  declareProperty("TofDigi",m_tofDigi=1);
  declareProperty("EmcDigi",m_emcDigi=1);
  declareProperty("MucDigi",m_mucDigi=1);
  declareProperty("InputFiles",m_inputFiles);
  
  m_totalEvents=0;
  m_fileNumber=0;
  m_tempNumber=-1;
  m_currentEntries=-1;
  m_currentEntry=-1;
}

RootRawEvtReader::~RootRawEvtReader()
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RootRawEvtReader::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc=svcLocator->service("EventDataSvc", m_evtSvc);
  if (sc.isFailure())
    std::cout<<"Could not accesss EventDataSvc!"<<std::endl;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RootRawEvtReader::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  DigiEvent* aDigiEvent = new DigiEvent;
  StatusCode sc = m_evtSvc->registerObject("/Event/Digi",aDigiEvent);
  if(sc!=StatusCode::SUCCESS) {
    std::cout<< "Could not register DigiEvent" <<std::endl;
  }

  
  if(m_tempNumber != m_fileNumber)
  {
    m_currentEntry = 0;  
    TFile* m_input = TFile::Open(m_inputFiles[m_fileNumber].c_str(), "READ");
    std::cout<<"input file: "<<m_inputFiles[m_fileNumber].c_str()<<std::endl;
    m_tree= (TTree *)m_input->Get("Event");
    m_tree->SetMakeClass(1);
    m_currentEntries=(Int_t)m_tree->GetEntries();
    std::cout<<"m_currentEntries: "<<m_currentEntries<<std::endl;

    m_tempNumber = m_fileNumber;
  } 
 
  if(m_mdcDigi)
    readMdc(m_currentEntry);

  if(m_tofDigi)
    readTof(m_currentEntry);

  if(m_emcDigi)
    readEmc(m_currentEntry);

  if(m_mucDigi)
    readMuc(m_currentEntry); 

  log << MSG::INFO << "current Entry: "<< m_currentEntry << endreq;
  //std::cout << "current Entry: "<< m_currentEntry << std::endl;
  m_currentEntry++;

  if(m_currentEntry==m_currentEntries)
  {
    m_fileNumber++;
  }

  m_totalEvents++;
  log << MSG::INFO << "total events: "<< m_totalEvents << endreq;
  //std::cout << "total events: "<< m_totalEvents << std::endl;

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode RootRawEvtReader::readMdc(int entry)
{
  TBranch *branch = m_tree->GetBranch("m_mdcDigiCol");

  TObjArray* mdcDigiCol;
  branch->SetAddress(&mdcDigiCol);
  mdcDigiCol=0;

  branch->GetEntry(entry);

  //commonData m_common;

  TIter mdcDigiIter(mdcDigiCol);
  MdcDigiCol* mdcDigiTdsCol = new MdcDigiCol;
  TMdcDigi *mdcDigiRoot = 0;

  while ((mdcDigiRoot = (TMdcDigi*)mdcDigiIter.Next())) {
    unsigned int  id  = mdcDigiRoot->getIntId();
	  unsigned int  time = mdcDigiRoot->getTimeChannel();
	  unsigned int  charge = mdcDigiRoot->getChargeChannel();
	  unsigned int  overflow = mdcDigiRoot->getOverflow();
	  int trackIndex = mdcDigiRoot->getTrackIndex();

    MdcDigi *mdcDigiTds = new MdcDigi(id);
    //m_common.m_rootMdcDigiMap[mdcDigiRoot] = mdcDigiTds;
	  mdcDigiTds->setTimeChannel(time);
	  mdcDigiTds->setChargeChannel(charge);
	  mdcDigiTds->setOverflow(overflow);
    mdcDigiTds->setTrackIndex(trackIndex);
    mdcDigiTdsCol->push_back(mdcDigiTds);
    /*std::cout<<"trackID: "<<trackIndex;
    std::cout<<"  charge: "<<charge;
    std::cout<<"  time: "<<time<<std::endl;*/
  }

  //register MDC digits collection to TDS
  StatusCode scMdc = m_evtSvc->registerObject("/Event/Digi/MdcDigiCol", mdcDigiTdsCol);
  if(scMdc!=StatusCode::SUCCESS) 
    std::cout<< "Could not register MDC digi collection" <<std::endl;
  

  return StatusCode::SUCCESS;
}

StatusCode RootRawEvtReader::readTof(int entry)
{
  TBranch *branch = m_tree->GetBranch("m_tofDigiCol");

  TObjArray* tofDigiCol;
  branch->SetAddress(&tofDigiCol);
  tofDigiCol=0;

  branch->GetEntry(entry);

  //commonData m_common;

  TIter tofDigiIter(tofDigiCol);
  TofDigiCol* tofDigiTdsCol = new TofDigiCol;
  TTofDigi *tofDigiRoot = 0;

  while ((tofDigiRoot = (TTofDigi*)tofDigiIter.Next())) {
    unsigned int  id  = tofDigiRoot->getIntId();
	  unsigned int  time = tofDigiRoot->getTimeChannel();
	  unsigned int  charge = tofDigiRoot->getChargeChannel();
	  unsigned int  overflow = tofDigiRoot->getOverflow();
	  int trackIndex = tofDigiRoot->getTrackIndex();

    TofDigi *tofDigiTds = new TofDigi(id);
    //m_common.m_rootTofDigiMap[tofDigiRoot] = tofDigiTds;
	  tofDigiTds->setTimeChannel(time);
	  tofDigiTds->setChargeChannel(charge);
	  tofDigiTds->setOverflow(overflow);
    tofDigiTds->setTrackIndex(trackIndex);
    tofDigiTdsCol->push_back(tofDigiTds);
  }

  //register TOF digits collection to TDS
  StatusCode scTof = m_evtSvc->registerObject("/Event/Digi/TofDigiCol", tofDigiTdsCol);
  if(scTof!=StatusCode::SUCCESS) 
    std::cout<< "Could not register TOF digi collection" <<std::endl;
  

  return StatusCode::SUCCESS;
}

StatusCode RootRawEvtReader::readEmc(int entry)
{
  TBranch *branch = m_tree->GetBranch("m_emcDigiCol");

  TObjArray* emcDigiCol;
  branch->SetAddress(&emcDigiCol);
  emcDigiCol=0;

  branch->GetEntry(entry);

  //commonData m_common;

  TIter emcDigiIter(emcDigiCol);
  EmcDigiCol* emcDigiTdsCol = new EmcDigiCol;
  TEmcDigi *emcDigiRoot = 0;

  while ((emcDigiRoot = (TEmcDigi*)emcDigiIter.Next())) {
    unsigned int  id  = emcDigiRoot->getIntId();
	  unsigned int  time = emcDigiRoot->getTimeChannel();
	  unsigned int  charge = emcDigiRoot->getChargeChannel();
	  unsigned int  measure = emcDigiRoot->getMeasure();
	  int trackIndex = emcDigiRoot->getTrackIndex();

    EmcDigi *emcDigiTds = new EmcDigi(id);
    //m_common.m_rootEmcDigiMap[emcDigiRoot] = emcDigiTds;
	  emcDigiTds->setTimeChannel(time);
	  emcDigiTds->setChargeChannel(charge);
	  emcDigiTds->setMeasure(measure);
    emcDigiTds->setTrackIndex(trackIndex);
    emcDigiTdsCol->push_back(emcDigiTds);
  }

  //register EMC digits collection to TDS
  StatusCode scEmc = m_evtSvc->registerObject("/Event/Digi/EmcDigiCol", emcDigiTdsCol);
  if(scEmc!=StatusCode::SUCCESS) 
    std::cout<< "Could not register EMC digi collection" <<std::endl;
  

  return StatusCode::SUCCESS;
}

StatusCode RootRawEvtReader::readMuc(int entry)
{
  TBranch *branch = m_tree->GetBranch("m_mucDigiCol");

  TObjArray* mucDigiCol;
  branch->SetAddress(&mucDigiCol);
  mucDigiCol=0;

  branch->GetEntry(entry);

  //commonData m_common;

  TIter mucDigiIter(mucDigiCol);
  MucDigiCol* mucDigiTdsCol = new MucDigiCol;
  TMucDigi *mucDigiRoot = 0;

  while ((mucDigiRoot = (TMucDigi*)mucDigiIter.Next())) {
    unsigned int  id  = mucDigiRoot->getIntId();
	  unsigned int  time = mucDigiRoot->getTimeChannel();
	  unsigned int  charge = mucDigiRoot->getChargeChannel();
	  int trackIndex = mucDigiRoot->getTrackIndex();

    MucDigi *mucDigiTds = new MucDigi(id);
    //m_common.m_rootMucDigiMap[mucDigiRoot] = mucDigiTds;
	  mucDigiTds->setTimeChannel(time);
	  mucDigiTds->setChargeChannel(charge);
    mucDigiTds->setTrackIndex(trackIndex);
    mucDigiTdsCol->push_back(mucDigiTds);
  }

  //register MUC digits collection to TDS
  StatusCode scMuc = m_evtSvc->registerObject("/Event/Digi/MucDigiCol", mucDigiTdsCol);
  if(scMuc!=StatusCode::SUCCESS) 
    std::cout<< "Could not register MUC digi collection" <<std::endl;
  

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RootRawEvtReader::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}
