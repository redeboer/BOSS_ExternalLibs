#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Algorithm.h"

#include "EventModel/Event.h"
#include "EventModel/EventModel.h"
#include "RawEvent/DigiEvent.h"
#include "MDCRawEvent/MdcDigi.h"

#include "RootCnvSvc/Util.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"

#include "DigiRootData/DigiEvent.h"

#include "RootCnvSvc/commonData.h"

#include "RootIO/IRootIoSvc.h"

/** @class digiRootWriterAlg
 * @brief Writes Digi TDS data to a persistent ROOT file.
 * @Based on the digiRootWriterAlg of GLAST.
 * 
 */

class digiRootWriterAlg : public Algorithm
{	
public:
    
    digiRootWriterAlg(const std::string& name, ISvcLocator* pSvcLocator);
    
    /// Handles setup by opening ROOT file in write mode and creating a new TTree
    StatusCode initialize();
   
    /// Orchastrates reading from TDS and writing to ROOT for each event
    StatusCode execute();
    
    /// Closes the ROOT file and cleans up
    StatusCode finalize();

private:

    /// Retrieves event Id and run Id from TDS and fills the McEvent ROOT object
    StatusCode writeDigiEvent();


    /// Retrieves TKR digitization data from the TDS and fills the TkrDigi
    /// ROOT collection
    StatusCode writeMdcDigi();

    /// Calls TTree::Fill for each event and clears m_digiEvt
    void writeEvent();
    /// Performs the final write to the ROOT file and closes
    void close();
   
    /// ROOT file pointer
    TFile *m_digiFile;
    /// ROOT tree pointer
    TTree *m_digiTree;
    /// Top-level Monte Carlo ROOT object
    DigiEvent *m_digiEvt;
    /// name of the output ROOT file
    std::string m_fileName;
    /// name of the TTree in the ROOT file
    std::string m_treeName;
    /// ROOT split mode
    int m_splitMode;
    /// Buffer Size for the ROOT file
    int m_bufSize;
    /// Compression level for the ROOT file
    int m_compressionLevel;
    /// auto save every events
    int m_autoSaveEvents;
    
    commonData m_common;
    IRootIoSvc* m_rootIoSvc;

};

static const AlgFactory<digiRootWriterAlg>  Factory;
const IAlgFactory& digiRootWriterAlgFactory = Factory;

digiRootWriterAlg::digiRootWriterAlg(const std::string& name, 
                                 ISvcLocator* pSvcLocator) : 
Algorithm(name, pSvcLocator)
{
    // Input parameters available to be set via the jobOptions file
    declareProperty("digiRootFile",m_fileName="digi.root");
    declareProperty("splitMode", m_splitMode=1);
    declareProperty("bufferSize", m_bufSize=64000);
    declareProperty("compressionLevel", m_compressionLevel=1);
    //declareProperty("treeName", m_treeName="Digi");
    declareProperty("treeName", m_treeName="Rec"); //wensp modified on 20050515 for test
    declareProperty("autoSave", m_autoSaveEvents=1000);

}

StatusCode digiRootWriterAlg::initialize()
{
    // Purpose and Method:  Called once before the run begins.  This method
    //    opens a new ROOT file and prepares for writing.

    StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), name());
    
    // Use the Job options service to set the Algorithm's parameters
    // This will retrieve parameters set in the job options file
    setProperties();

    if ( service("RootIoSvc", m_rootIoSvc, true).isFailure() ){
        log << MSG::INFO << "Couldn't find the RootIoSvc!" << endreq;
        log << MSG::INFO << "No Auto Saving" << endreq;
        m_rootIoSvc = 0;
    } 

    facilities::Util::expandEnvVar(&m_fileName);

    // Save the current directory for the ntuple writer service
    TDirectory *saveDir = gDirectory;   
    // Create the new ROOT file
    m_digiFile = new TFile(m_fileName.c_str(), "RECREATE");
    if (!m_digiFile->IsOpen()) {
        log << MSG::ERROR << "ROOT file " << m_fileName 
            << " could not be opened for writing." << endreq;
        return StatusCode::FAILURE;
    }
    m_digiFile->cd();
    m_digiFile->SetCompressionLevel(m_compressionLevel);
    m_digiTree = new TTree(m_treeName.c_str(), "Bes Digitization Data");
    m_digiEvt = new DigiEvent();
    m_common.m_digiEvt = m_digiEvt;
    m_digiTree->Branch("DigiEvent","DigiEvent", &m_digiEvt, m_bufSize, m_splitMode);
    saveDir->cd();
    return sc;
    
}

StatusCode digiRootWriterAlg::execute()
{
    // Purpose and Method:  Called once per event.  This method calls
    //   the appropriate methods to read data from the TDS and write data
    //   to the ROOT file.

    MsgStream log(msgSvc(), name());

    StatusCode sc = StatusCode::SUCCESS;
    
    if (!m_digiFile->IsOpen()) {
        log << MSG::ERROR << "ROOT file " << m_fileName 
            << " could not be opened for writing." << endreq;
        return StatusCode::FAILURE;
    }
    
    m_digiEvt->Clear();

    sc = writeDigiEvent();
    if (sc.isFailure()) {
        log << MSG::ERROR << "Failed to write DigiEvent" << endreq;
        return sc;
    }


    sc = writeMdcDigi();
    if (sc.isFailure()) {
        log << MSG::ERROR << "Failed to write Tkr Digi Collection" << endreq;
        return sc;
    }
  
    writeEvent();
    return sc;
}


StatusCode digiRootWriterAlg::writeDigiEvent() {
    // Purpose and Method:  Retrieve the Event object from the TDS and set the
    //    event and run numbers in the DigiEvent ROOT object

    MsgStream log(msgSvc(), name());
    StatusCode sc = StatusCode::SUCCESS;

    // Retrieve the Event data for this event
 SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event");
    if (!eventHeader) return sc;

    Short_t   runId = eventHeader->runNumber();
    Short_t  evtId  = eventHeader->eventNumber();
    Bool_t fromMc = true;


    m_digiEvt->initialize(evtId, runId, fromMc);
   // m_digiEvt->Print();
    
    return sc;
}

StatusCode digiRootWriterAlg::writeMdcDigi() {
    // Purpose and Method:  Retrieve the TkrDigi collection from the TDS and set the
    //    TkrDigi ROOT collection

    MsgStream log(msgSvc(), name());
    StatusCode sc = StatusCode::SUCCESS;
    
    SmartDataPtr<MdcDigiCol> mdcDigiColTds(eventSvc(), EventModel::Digi::MdcDigiCol);
    if (!mdcDigiColTds) return sc;
    MdcDigiCol::const_iterator mdcDigiTds;

    for (mdcDigiTds = mdcDigiColTds->begin(); mdcDigiTds != mdcDigiColTds->end(); mdcDigiTds++) {
          UInt_t overflow  = (*mdcDigiTds)->getOverflow();
	  UInt_t time      = (*mdcDigiTds)->getTimeChannel();
	  UInt_t charge    = (*mdcDigiTds)->getChargeChannel();
	  UInt_t id        = (*mdcDigiTds)->getIntId();
  	  TMdcDigi *mdcDigiRoot = new TMdcDigi();
          m_common.m_mdcDigiMap[(*mdcDigiTds)] = mdcDigiRoot;

         mdcDigiRoot->initialize(id, time ,charge);
         mdcDigiRoot->setOverflow(overflow);
         m_digiEvt->addMdcDigi(mdcDigiRoot);
//	 mdcDigiRoot->Print();
    }

    return sc;
}

void digiRootWriterAlg::writeEvent() 
{
    // Purpose and Method:  Stores the DigiEvent data for this event in the ROOT
    //    tree.  The m_digiEvt object is cleared for the next event.
    static int eventCounter = 0;
    TDirectory *saveDir = gDirectory;
    m_digiTree->GetCurrentFile()->cd();
    //m_digiFile->cd();
    m_digiTree->Fill();
    //m_digiEvt->Clear();
    saveDir->cd();
    ++eventCounter;
    if (m_rootIoSvc)
        if (eventCounter % m_rootIoSvc->getAutoSaveInterval() == 0) m_digiTree->AutoSave();

    return;
}

void digiRootWriterAlg::close() 
{
    // Purpose and Method:  Writes the ROOT file at the end of the run.
    //    The TObject::kWriteDelete parameter is used in the Write method
    //    replacing TObject::kOverwrite - supposed to be safer
    //    since ROOT will periodically write to the ROOT file when the bufSize
    //    is filled.  Writing would create 2 copies of the same tree to be
    //    stored in the ROOT file, if we did not specify kOverwrite.

    TDirectory *saveDir = gDirectory;
    TFile *f = m_digiTree->GetCurrentFile();
    //m_digiFile->cd();
    f->cd();
    m_digiTree->BuildIndex("m_runId", "m_eventId");
    f->Write(0, TObject::kWriteDelete);
    f->Close();
    saveDir->cd();
    return;
}

StatusCode digiRootWriterAlg::finalize()
{
    close();
    
    StatusCode sc = StatusCode::SUCCESS;
    return sc;
}

