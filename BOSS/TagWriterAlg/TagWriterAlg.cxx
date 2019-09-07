#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EventModel/EventHeader.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Geometry/Point3D.h"

#include "TagWriterAlg/TagWriterAlg.h"

#include "RootCnvSvc/RootInterface.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IJobOptionsSvc.h"


#include "TMath.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////

TagWriterAlg::TagWriterAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //declareProperty("InputTagFile", m_inputTagFile);
  declareProperty("OutputTagFile", m_outputTagFile);
  //declareProperty("FilterTotalCharged", m_filterTotalCharged=2);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode TagWriterAlg::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;

  SmartIF<IJobOptionsSvc> iSvc(serviceLocator()->service("JobOptionsSvc"));
  if ( iSvc.isValid() ) {
    std::string  dll;  //sub so name
    const std::vector<const Property*>* ps = 0;
    if ( iSvc->getProperties("RootCnvSvc") != 0 ) 
      ps = iSvc->getProperties("RootCnvSvc");
    if ( iSvc->getProperties("EventCnvSvc") != 0 )
      ps = iSvc->getProperties("EventCnvSvc");
    if ( iSvc->getProperties("WriteDst") != 0 )
      ps = iSvc->getProperties("WriteDst");

    if( ps !=0 )
    {
      for(int i=0;i<ps->size();i++)
      {
        if( (*ps)[i]->name() == "digiRootOutputFile" )
         
        { 
          m_dstOutput = (*ps)[i]->toString();
          std::cout<<"read from JobOptionSvc, dstOutput: "<< m_dstOutput<<std::endl;
        }
      }
    }
  }



  m_rootInterface=RootInterface::Instance(log);
  log << MSG::INFO <<"RootInteface TotalFileNum: "<<m_rootInterface->getTotalFileNum()<<endreq;
  std::cout<<"getTotalFileNum: "<<m_rootInterface->getTotalFileNum()<<std::endl;

  if(m_rootInterface->getTotalFileNum()>0)
  {
    if(m_rootInterface->getTotalFileNum() != m_outputTagFile.size() )
    {
      std::cout<<"the input file num != output file number, please check your jobOptions"<<std::endl;
      exit(-1);
    }
  }
  
  m_entry = -1;
  m_entry_in_alg = -1;
  m_dstFile = "";
  m_fileNum=-1; 
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode TagWriterAlg::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  //Write Tag during reconstruction
  if(m_dstOutput!="")
  {
    if(m_entry==-1)
    {
      std::cout << "TagWriterAlg: write tag during reconstruction" << std::endl;
      m_dstFile = m_dstOutput;
      std::cout<< "dst file: "<< m_dstFile << std::endl;
      openOutputTagFile(m_outputTagFile[0]);
      m_oFileTree->Fill();
    }
  }
  else  //Write Tag after reconstruction
  {
    if( m_dstFile != m_rootInterface->getCurrentFileName() )
    {
      std::cout<<"TagWriterAlg, current file: "<<m_rootInterface->getCurrentFileName()<<std::endl;
      m_fileNum++;
      m_dstFile = m_rootInterface->getCurrentFileName();

      m_entry=-1;
      if(m_fileNum>0)
      {
        m_oFile->Write();
        delete m_oTree;
        delete m_oFileTree;
        delete m_oFile;
      }
    
      log << MSG::INFO << "open ROOT output TAG file: "<<m_outputTagFile[m_fileNum]<<endreq;
      openOutputTagFile(m_outputTagFile[m_fileNum]);
      m_oFileTree->Fill();
    }
  }

  m_entry++;
  getTagInfo();  
  log << MSG::INFO << "writing TAG information to ROOT TAG file"<<endreq;
  m_oTree->Fill();

  m_entry_in_alg++;
  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TagWriterAlg::getTagInfo()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "reading event data from DST"<<endreq;
  
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if(!eventHeader)
  {
    log << MSG::FATAL << "Could not find EventHeader." << endreq;
    return StatusCode::FAILURE;
  }

  m_runNo=eventHeader->runNumber();
  m_eventId=eventHeader->eventNumber();
  log << MSG::DEBUG <<"run, evtnum = " << m_runNo << " , "<< m_eventId <<endreq;

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  if(!evtRecEvent)
  {
    log << MSG::FATAL << "Could not find EvtRecEvent." << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
      << evtRecEvent->totalCharged() << " , "
      << evtRecEvent->totalNeutral() << " , "
      << evtRecEvent->totalTracks() <<endreq;
  m_totalCharged = evtRecEvent->totalCharged();
  m_totalNeutral = evtRecEvent->totalNeutral();
  m_totalTrks = evtRecEvent->totalTracks();
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode TagWriterAlg::finalize() {

  MsgStream log(msgSvc(), name());

  m_oFile->Write();

  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void TagWriterAlg::openOutputTagFile(std::string ofile)
{
  m_oFile = new TFile(ofile.c_str(),"RECREATE","ROOT file for TAG");
  m_oTree = new TTree("Tag","Tags for DST file");
  m_oTree->Branch("entry",&m_entry, "entry/I");
  m_oTree->Branch("runNo",&m_runNo, "runNo/I");
  m_oTree->Branch("eventId",&m_eventId, "eventId/I");
  m_oTree->Branch("totalCharged",&m_totalCharged, "totalCharged/I");
  m_oTree->Branch("totalNeutral",&m_totalNeutral, "totalNeutral/I");
  m_oTree->Branch("totalTrks",&m_totalTrks, "totalTrks/I");
  m_oFileTree = new TTree("File", "DST file names");
  m_oFileTree->Branch("fileName", &m_dstFile);

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/*bool TagWriterAlg::eventFilter()
{
  std::cout<<"m_entry in eventFilter: "<<m_entry<<std::endl;

  //set your filter criteria here
  if(m_r_totalCharged==m_filterTotalCharged)
    return true;
  else
    return false;
}*/
