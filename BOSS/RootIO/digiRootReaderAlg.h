#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Algorithm.h"

#include "EventModel/Event.h"
#include "EventModel/EventModel.h"
#include "RawEvent/DigiEvent.h"
#include "MdcRawEvent/MdcDigi.h"

#include "RootCnvSvc/Util.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TObjArray.h"
#include "TCollection.h"  // Declares TIter
#include "DigiRootData/DigiEvent.h"

//#include "RootCnvSvc/commonData.h"

#include "RootIO/IRootIoSvc.h"

/** @class digiRootReaderAlg
 * @brief Reads Digitization data from a persistent ROOT file and stores the
 * the data in the TDS.
 * Based on digiRootReaderAlg of Glast.
 *
 */

class digiRootReaderAlg : public Algorithm
{	
public:
    
    digiRootReaderAlg(const std::string& name, ISvcLocator* pSvcLocator);
    
    /// Handles setup by opening ROOT file in read mode and creating a new TTree
    StatusCode initialize();
   
    /// Orchastrates reading from ROOT file and storing the data on the TDS for each event
    StatusCode execute();
    
    /// Closes the ROOT file and cleans up
    StatusCode finalize();
            
private:

    /// Reads top-level DigiEvent
    StatusCode readDigiEvent();


    /// Reads TKR digi data from ROOT and puts it on the TDS
    StatusCode readMdcDigi();

    /// Closes the ROOT file
    void close();
   
    /// ROOT file pointer
    TFile *m_digiFile;
    /// ROOT tree pointer
    TChain *m_digiTree;
    /// Top-level Monte Carlo ROOT object
    DigiEvent *m_digiEvt;
    /// name of the input ROOT file
    std::string m_fileName;
    /// Array of input file names
    StringArrayProperty m_fileList;
    /// name of the Monte Carlo TTree stored in the ROOT file
    std::string m_treeName;
    /// Stores number of events available in the input ROOT TTree
    int m_numEvents;
  
//    commonData m_common;
    IRootIoSvc* m_rootIoSvc;

};

static const AlgFactory<digiRootReaderAlg>  Factory;
const IAlgFactory& digiRootReaderAlgFactory = Factory;


digiRootReaderAlg::digiRootReaderAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
Algorithm(name, pSvcLocator)
{
    // Input pararmeters that may be set via the jobOptions file
    // Input ROOT file name
    declareProperty("digiRootFile",m_fileName="");
    StringArrayProperty initList;
    std::vector<std::string> initVec;
    initVec.push_back("digicopy.root");
    initList.setValue(initVec);
    declareProperty("digiRootFileList",m_fileList=initList);
    // Input TTree name
    initVec.clear();
    declareProperty("digiTreeName", m_treeName="Rec");// wensp midify for test 2005/05/14

}

StatusCode digiRootReaderAlg::initialize()
{
    // Purpose and Method:  Called once before the run begins.  This method
    //    opens a new ROOT file and prepares for reading.

    StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), name());
    
    // Use the Job options service to set the Algorithm's parameters
    // This will retrieve parameters set in the job options file
    setProperties();
   
    if ( service("RootIoSvc", m_rootIoSvc, true).isFailure() ){
        log << MSG::INFO << "Couldn't find the RootIoSvc!" << endreq;
        log << MSG::INFO << "Event loop will not terminate gracefully" << endreq;
        m_rootIoSvc = 0;
        //return StatusCode::FAILURE;
    } 

    facilities::Util::expandEnvVar(&m_fileName);
    
    // Save the current directory for the ntuple writer service
    TDirectory *saveDir = gDirectory;   

    m_digiTree = new TChain(m_treeName.c_str());

    std::string emptyStr("");
    if (m_fileName.compare(emptyStr) != 0) {
	  TFile f(m_fileName.c_str());
      if (!f.IsOpen()) {
        log << MSG::ERROR << "ROOT file " << m_fileName.c_str()
            << " could not be opened for reading." << endreq;
        return StatusCode::FAILURE;
      }
	  f.Close();
	  m_digiTree->Add(m_fileName.c_str());
          log << MSG::INFO << "Opened file: " << m_fileName.c_str() << endreq;
    } else {
      const std::vector<std::string> fileList = m_fileList.value( );
      std::vector<std::string>::const_iterator it;
      std::vector<std::string>::const_iterator itend = fileList.end( );
      for (it = fileList.begin(); it != itend; it++) {
        std::string theFile = (*it);
	    TFile f(theFile.c_str());
        if (!f.IsOpen()) {
          log << MSG::ERROR << "ROOT file " << theFile.c_str()
              << " could not be opened for reading." << endreq;
          return StatusCode::FAILURE;
        }
	  f.Close();
	  m_digiTree->Add(theFile.c_str());
          log << MSG::INFO << "Opened file: " << theFile.c_str() << endreq;
	  }
    }


    m_digiEvt = 0;
    m_digiTree->SetBranchAddress("DigiEvent", &m_digiEvt);
    //m_common.m_digiEvt = m_digiEvt;
    m_numEvents = m_digiTree->GetEntries();
    
    if (m_rootIoSvc) {
      m_rootIoSvc->setRootEvtMax(m_numEvents);
      if (!m_digiTree->GetIndex()) m_digiTree->BuildIndex("m_runId", "m_eventId");
      m_rootIoSvc->registerRootTree(m_digiTree);
    }


    saveDir->cd();
    return sc;
    
}

StatusCode digiRootReaderAlg::execute()
{
    // Purpose and Method:  Called once per event.  This method calls
    //   the appropriate methods to read data from the ROOT file and store
    //   data on the TDS.

    MsgStream log(msgSvc(), name());

    StatusCode sc = StatusCode::SUCCESS;
    
    if (m_digiEvt) m_digiEvt->Clear();

    static Int_t evtId = 0;
	int readInd, numBytes;
	std::pair<int,int> runEventPair = (m_rootIoSvc) ? m_rootIoSvc->runEventPair() : std::pair<int,int>(-1,-1);
	
	if ((m_rootIoSvc) && (m_rootIoSvc->index() >= 0)) {
		readInd = m_rootIoSvc->index();
	} else if ((m_rootIoSvc) && (runEventPair.first != -1) && (runEventPair.second != -1)) {
		int run = runEventPair.first;
		int evt = runEventPair.second;
		readInd = m_digiTree->GetEntryNumberWithIndex(run, evt);
	} else {
		readInd = evtId;
	}

    if (readInd >= m_numEvents) {
        log << MSG::WARNING << "Requested index is out of bounds - no digi data loaded" << endreq;
        return StatusCode::SUCCESS;
    }

    numBytes = m_digiTree->GetEvent(readInd);
	
	if ((numBytes <= 0) || (!m_digiEvt)) {
            log << MSG::WARNING << "Failed to load digi event" << endreq;
            return StatusCode::SUCCESS;
	}


    sc = readDigiEvent();
    if (sc.isFailure()) {
        log << MSG::ERROR << "Failed to read top level DigiEvent" << endreq;
        return sc;
    }

    sc = readMdcDigi();
    if (sc.isFailure()) {
        log << MSG::ERROR << "Failed to load MdcDigi" << endreq;
        return sc;
    }

	evtId = readInd+1;
    return sc;
}


StatusCode digiRootReaderAlg::readDigiEvent() {

    MsgStream log(msgSvc(), name());

    StatusCode sc = StatusCode::SUCCESS;
    
    // Retrieve the Event data for this event
    SmartDataPtr<Event::EventHeader> evt(eventSvc(), EventModel::EventHeader);
    if (!evt) {
        log << MSG::ERROR << "Failed to retrieve Event" << endreq;
        return StatusCode::FAILURE;
    }

    unsigned int eventIdTds = evt->eventNumber();
    unsigned int runIdTds = evt->runNumber();

    unsigned int eventIdRoot = m_digiEvt->getEventId();
    unsigned int runIdRoot = m_digiEvt->getRunId();

    // Check to see if the event and run ids have already been set.
    if (eventIdTds != eventIdRoot) evt->setEventNumber(eventIdRoot);
    if (runIdTds != runIdRoot) evt->setRunNumber(runIdRoot);


    Event::DigiEvent* digiEventTds = 
        SmartDataPtr<Event::DigiEvent>(eventSvc(), EventModel::Digi::Event);
    if (!digiEventTds) {
        sc = eventSvc()->registerObject(EventModel::Digi::Event /*"/Event/Digi"*/,new DataObject);
        if( sc.isFailure() ) {
            log << MSG::ERROR << "could not register " << EventModel::Digi::Event /*<< /Event/Digi "*/ << endreq;
            return sc;
        }
    } else {
        bool fromMc = m_digiEvt->getFromMc();
        digiEventTds->initialize(fromMc);
    }
    return sc;
}


StatusCode digiRootReaderAlg::readMdcDigi() {
    MsgStream log(msgSvc(), name());

    StatusCode sc = StatusCode::SUCCESS;
    const TObjArray *mdcDigiRootCol = m_digiEvt->getMdcDigiCol();
    if (!mdcDigiRootCol) return sc;
    TIter mdcDigiIter(mdcDigiRootCol);

    // create the TDS location for the EmcDigi Collection
    MdcDigiCol* mdcDigiTdsCol = new MdcDigiCol;
    sc = eventSvc()->registerObject(EventModel::Digi::MdcDigiCol, mdcDigiTdsCol);
    if (sc.isFailure()) {
        log << "Failed to register MdcDigi Collection" << endreq;
        return StatusCode::FAILURE;
    }


    
    TMdcDigi *mdcDigiRoot = 0;
    while ((mdcDigiRoot = (TMdcDigi*)mdcDigiIter.Next())!=0) {
	    mdcDigiRoot->Print();
    }

    return sc;
}

void digiRootReaderAlg::close() 
{
    // Purpose and Method:  Writes the ROOT file at the end of the run.
    //    The TObject::kOverWrite parameter is used in the Write method
    //    since ROOT will periodically write to the ROOT file when the bufSize
    //    is filled.  Writing would create 2 copies of the same tree to be
    //    stored in the ROOT file, if we did not specify kOverwrite.

    if (m_digiTree) delete m_digiTree;
}

StatusCode digiRootReaderAlg::finalize()
{
    close();
    
    StatusCode sc = StatusCode::SUCCESS;
    return sc;
}

