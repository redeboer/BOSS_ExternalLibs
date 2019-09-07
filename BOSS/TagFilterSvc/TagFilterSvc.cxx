#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"


#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "TagFilterSvc/TagFilterSvc.h"
#include <iostream> 
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TFormula.h"

TagFilterSvc::TagFilterSvc( const string& name, ISvcLocator* svcloc) :
  Service (name, svcloc){
  // declare properties
  declareProperty("Criteria",  m_criteria="");
  declareProperty("tagFiles", m_tagFiles);
}

TagFilterSvc::~TagFilterSvc(){
}

StatusCode TagFilterSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
     if( IID_ITagFilterSvc.versionMatch(riid) ){
	  *ppvInterface = static_cast<ITagFilterSvc*> (this);
     } else{
	  return Service::queryInterface(riid, ppvInterface);
     }
     return StatusCode::SUCCESS;
}

StatusCode TagFilterSvc::initialize(){
  MsgStream log(messageService(), name());
  log << MSG::INFO << "TagFilterSvc::initialize()" << endreq;

  StatusCode sc = Service::initialize();
  sc = setProperties();
  return StatusCode::SUCCESS;
}

vector<string> TagFilterSvc::getDstFiles()
{
  int nFiles = m_tagFiles.size();
  std::cout<<"TagFilterSvc, number of tag files: "<<m_tagFiles.size()<<std::endl;
  for(int i=0;i<nFiles;i++)
  {
    TFile* tagFile = new TFile(m_tagFiles[i].c_str());
    TTree* fileTree = (TTree*)tagFile->Get("File");
    std::string* dstFile = new std::string;
    fileTree->SetBranchAddress("fileName",&dstFile);
    fileTree->GetEntry(0);
    std::cout<<"dstFile: "<<*dstFile<<std::endl;
    m_dstFiles.push_back(*dstFile);
    delete dstFile;
    delete tagFile;
  }
  return m_dstFiles;
}


StatusCode TagFilterSvc::getVEntry(string tagFileName, std::vector<int>& ventry) {
  TFormula f("crt",m_criteria.c_str());
  TFile* file = new TFile(tagFileName.c_str());
  TTree* tree = (TTree*)file->Get("Tag");
  int entry=-1;
  int runNo=-1;
  int eventId=-1;
  int totalCharged=-1;
  int totalNeutral=-1;
  int totalTrks=-1;
  tree->SetBranchAddress("entry",&entry);
  tree->SetBranchAddress("runNo",&runNo);
  tree->SetBranchAddress("eventId",&eventId);
  tree->SetBranchAddress("totalCharged",&totalCharged);
  tree->SetBranchAddress("totalNeutral",&totalNeutral);
  tree->SetBranchAddress("totalTrks",&totalTrks);
  for(int j=0;j<tree->GetEntries();j++)
  {
    tree->GetEntry(j);
    f.SetParameter(1,entry);
    f.SetParameter(2,runNo);
    f.SetParameter(3,eventId);
    f.SetParameter(4,totalCharged);
    f.SetParameter(5,totalNeutral);
    f.SetParameter(6,totalTrks);
    if(m_criteria!="")
    {
      if(f.Eval(0)==1)
        ventry.push_back(entry);
    }
    else
      ventry.push_back(entry);
  }
  delete file;

  return StatusCode::SUCCESS; 
}
  
StatusCode TagFilterSvc::finalize(){
     MsgStream log(messageService(), name());
     log << MSG::INFO << "TagFilterSvc::finalize()" << endreq;
    return StatusCode::SUCCESS;
}


