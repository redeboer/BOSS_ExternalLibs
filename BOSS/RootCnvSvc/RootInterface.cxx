#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TChain.h"
#include "TClonesArray.h"

#include "RootCnvSvc/RootInterface.h"
#include "RootEventData/TJobInfo.h"
#include "DataInfoSvc/DataInfoSvc.h"
#include "DataInfoSvc/IDataInfoSvc.h"

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcLocator.h"

#include <algorithm> // for find
#include <iostream>
#include <fstream>
#include <string>
RootInterface*  RootInterface::m_rootInterface = 0;


RootInterface *RootInterface::Instance(MsgStream log) {
  if (m_rootInterface) return m_rootInterface;
  m_rootInterface=new RootInterface(log);
  return m_rootInterface;
}

RootInterface::RootInterface(MsgStream str) : log(str)
{ 
  
  m_branches= new TClonesArray("TBranch",1); 
  m_branchesRead= new TClonesArray("TBranch",1);
  m_entries=-1;
  m_EOF=false;
  m_ENDFILE = false ;
  m_fileNum = 0; //-1
 }


RootInterface::~RootInterface()
{ 
}

std::string RootInterface::getJobOptions()
{
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty> propMgr (iface );
  std::string path;
  propMgr->getProperty( "JobOptionsPath", path);
  log << MSG::INFO << "JobOptions file for current job: " <<path << endreq;
  ifstream fin(path.c_str());
  string jobOptions;
  string tempString;
  while(getline(fin,tempString))
  {
    if( tempString.size()>0 && tempString.find("//")>tempString.size() )
    { 
      jobOptions += tempString;
      jobOptions += "\n";
    }
  }
  log << MSG::INFO << "JobOptions: " << endreq
      << jobOptions << endreq;
  return jobOptions;
}

std::string RootInterface::getDecayOptions()
{ 
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IDataInfoSvc *tmpInfoSvc;
  DataInfoSvc* jobInfoSvc;
  string decayOptions;
  StatusCode status = svcLocator->service("DataInfoSvc",tmpInfoSvc);
  if (status.isSuccess()) {
    log << MSG::INFO << "get the DataInfoSvc" << endreq;
    jobInfoSvc=dynamic_cast<DataInfoSvc *>(tmpInfoSvc);
    decayOptions = jobInfoSvc->getDecayOptions();
    log << MSG::INFO << "get decay options" << endreq
        << decayOptions << endreq;
  }else {
    log << MSG::WARNING << "could not get the DataInfoSvc. Ignore it." << endreq;
  }
  return decayOptions;
}

std::vector<int> RootInterface::getTotEvtNo()
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IDataInfoSvc *tmpInfoSvc;
  DataInfoSvc* jobInfoSvc;
  std::vector<int> totEvtNo;
  StatusCode status = svcLocator->service("DataInfoSvc",tmpInfoSvc);
  if (status.isSuccess()) {
    log << MSG::INFO << "get the DataInfoSvc" << endreq;
    jobInfoSvc=dynamic_cast<DataInfoSvc *>(tmpInfoSvc);
    totEvtNo = jobInfoSvc->getTotEvtNo();
    log << MSG::INFO << "get total event number for each run" << endreq;
  }else {
    log << MSG::WARNING << "could not get the DataInfoSvc. Ignore it." << endreq;
  }
  return totEvtNo;
}

StatusCode RootInterface::finalize() {

  //  Get the messaging service, print where you are
  log << MSG::INFO << "finalize() in RootInterface" << endreq;

  // close file (FIXME for several output files)
  std::vector<TTree *>::const_iterator trees;
  for (trees=m_outputTrees.begin();trees<m_outputTrees.end();trees++)
   if (*trees) {
    int treenr=(*trees)->GetUniqueID();
    if (m_outputFiles[treenr]) {
      if (!m_outputFiles[treenr]->IsOpen()) {
	log << MSG::ERROR << "Could not open file for writing" << endreq;
	return StatusCode::FAILURE;
      } else {
        log << MSG::DEBUG<<" Closing file "<<treenr<<", tree "<<(*trees)->GetName()<<endreq;
        TDirectory *saveDir = gDirectory;
	m_outputFiles[treenr]->cd();

  TJobInfo* jobInfo = new TJobInfo;
  TTree* m_jobInfoTree = new TTree("JobInfoTree","Job info");
  m_jobInfoTree->Branch("JobInfo",&jobInfo);
  
  m_bossVer = getenv("BES_RELEASE");
  log << MSG::INFO << "fill boss version: "<<m_bossVer << endreq;

  string tmpJobOptions = getJobOptions();
  m_jobOptions.push_back( tmpJobOptions );

  if(m_decayOptions.size()==0)
    m_decayOptions = getDecayOptions();
 
  m_totEvtNo = getTotEvtNo(); 
  jobInfo->setBossVer(m_bossVer);
  jobInfo->setJobOptions(m_jobOptions);
  jobInfo->setDecayOptions(m_decayOptions); 
  jobInfo->setTotEvtNo(m_totEvtNo); 
  m_jobInfoTree->Fill();

	//?    m_mcFile->Write(0, TObject::kOverwrite);
 	int st =1;
	st = m_outputFiles[treenr]->Write();
	if(st==0){
	   log << MSG::FATAL<<" can not write the file "<< m_outputFilenames[treenr].c_str()<<endreq;
	   exit(1);
	}
 
	m_outputFiles[treenr]->Close();
	saveDir->cd();
      }
    }
   }
  if(m_outputTrees.size()>0) m_outputTrees.clear();
  return StatusCode::SUCCESS;
}

StatusCode RootInterface::addInput(const std::string& treename,const std::string& file) {
  log << MSG::DEBUG << "addInput  for Tree "<<treename<<endreq;
  StatusCode sc=StatusCode::SUCCESS;
  m_fileNames.push_back(file); //input files 2005-11-28
  m_otherTrees.push_back(NULL);
  inputFiles.push_back(NULL);
  unsigned int treenr;
  sc=getTreeNr(treename,treenr,true);
  m_inputFilenames[treenr]=file; // the last one file is setted 
  m_inputFiles[treenr]=NULL;
  m_inputTrees[treenr]=NULL;
  m_currentFileName = m_fileNames[treenr].c_str();
  return sc;
}


void RootInterface::setTagInputFile(std::vector<std::string> input)
{

  for(int i=0; i<input.size(); i++)
  {
    log << MSG::DEBUG <<"input tag file: "<<i<<" "<<input[i]<<endreq;
    m_tagInputFile.push_back(input[i]);
  }
}

StatusCode RootInterface::addOutput(const std::string& treename,const std::string& file,int split, int bufsize, int compression) {
  static int i =0;
  i ++;
  log << MSG::DEBUG << "addOutput  for Tree "<<treename<<endreq;
  StatusCode sc=StatusCode::SUCCESS;
  unsigned int treenr;
  sc=getTreeNr(treename,treenr,true);
  m_outputFilenames[treenr]=file;
  m_outputFiles[treenr]=NULL;
  m_outputTrees[treenr]=NULL;
  m_splitModes[treenr]=split;
  m_bufSizes[treenr]=bufsize;
  m_compressionLevels[treenr]=compression;

  return sc;
}

StatusCode RootInterface::createBranch(const std::string& treename,const std::string& branchname ,const char *classname,void *addr,int & branchnr){

  log << MSG::DEBUG << "CreateBranch, Tree "<<treename<<" branch "<<branchname<<endreq;

  TBranch *branch;
  unsigned int treenr;
  StatusCode sc=getTreeNr(treename,treenr);
  if (!sc.isSuccess()) return sc;

  if ( m_outputFilenames[treenr].empty() ) {
     log << MSG::DEBUG << "No corresponding output file specified, ignore createBranch: "<<branchname<<endreq;
     return StatusCode::SUCCESS;
  }

  if(!m_outputTrees[treenr])  sc=this->createTree(treenr,treename);
  if (!sc.isSuccess()) return sc;
  TTree *  tree=m_outputTrees[treenr];
  tree->SetUniqueID(treenr);

  branch = tree->Branch(branchname.c_str(),classname,addr,m_bufSizes[treenr],m_splitModes[treenr]);
  branch->SetUniqueID(treenr);
  branchnr=m_branches->GetEntriesFast()+1;
  m_branches->Expand(branchnr);
  TClonesArray &a = *m_branches;
  a[branchnr-1]=branch;
  tree->SetBasketSize(branchname.c_str(),m_bufSizes[treenr]); //some problem with above method to set buffersize, so we set it here.
  return StatusCode::SUCCESS;
}


StatusCode RootInterface::createTree(const unsigned int treenr,const std::string treename)
{
  // opens file and creates TTree on it
 
  TDirectory *saveDir = gDirectory;   

  // Create the new ROOT file
  m_outputFiles[treenr] = TFile::Open(m_outputFilenames[treenr].c_str(), "RECREATE");
  if(m_outputFiles[treenr]->IsZombie()){ 
        std::cout<<"RootInterface ERROR::Can't not open file"<<m_outputFilenames[treenr].c_str()<<std::endl; 
        exit(1); 
    } 
  if (!m_outputFiles[treenr]->IsOpen()) {
    log << MSG::FATAL << "ROOT file " << m_outputFilenames[treenr] 
	<< " could not be opened for writing." << endreq;
    exit(1);
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "RootInterface::opened file for output:" << m_outputFilenames[treenr].c_str() <<endreq;
 
  m_outputFiles[treenr]->cd();
  m_outputFiles[treenr]->SetCompressionLevel(m_compressionLevels[treenr]);
  std::string title=treename+" from conversion";
  m_outputTrees[treenr]= new TTree(treename.c_str(), title.c_str());
  TTree::SetMaxTreeSize(20000000000LL);

  saveDir->cd();

  return StatusCode::SUCCESS;
}

TTree * RootInterface::getTree(const std::string treename)
{

  // get TTree for input
  log<<MSG::INFO<<"RootInterface:;getTree"<<endreq; 
  unsigned int treenr;
  getTreeNr(treename,treenr);
 
  if (m_inputTrees[treenr]) return m_inputTrees[treenr];
  if (!m_inputFiles[treenr] ) {
   m_inputFiles[treenr] = TFile::Open(m_fileNames[treenr].c_str(), "READ");
   if (!m_inputFiles[treenr]->IsOpen()) {
     log << MSG::ERROR << "ROOT file " << m_inputFiles[treenr]->GetName()
	 << " could not be opened for reading." << endreq;
     delete m_inputFiles[treenr];
     m_inputFiles[treenr]=NULL;
     m_EOF=true;
     return NULL;
   }
  }
  log << MSG::INFO << "RootInterface::opened file for input:" << m_fileNames[treenr].c_str() <<endreq;
  m_currentFileName = m_fileNames[treenr].c_str();  //liangyt 2008-11-19
  TTree *tree= (TTree *)m_inputFiles[treenr]->Get(treename.c_str());
  if (!tree) {
    log << MSG::ERROR << "ROOT file " << m_inputFiles[treenr]->GetName()
	<< " does not contain requested TTree: " << treename <<endreq;
    return NULL;
  }
  if (tree->GetEntries()<=0)
  {
    log << MSG::ERROR << "ROOT file "<< m_inputFiles[treenr]->GetName()<< " entries <= 0" << endreq;
    exit(1);
  }

  m_inputTrees[treenr]=tree;
  if (m_entries<=0 ) {
    m_entries=(Int_t)tree->GetEntries();
  }

  printJobInfo( m_inputFiles[treenr],1);
  
  return tree;
}

void RootInterface::printJobInfo(TFile* file, int level)
{
  TTree* tree2  = (TTree *)file->Get("JobInfoTree");
  if(!tree2)
  {
    std::cout<<"no JobInfoTree for file "<<file->GetName()<<std::endl;
    exit(1);
  }
  else
  {
    log << MSG::INFO << "get JobInfoTree" << endreq;
    TBranch* branch = tree2->GetBranch("JobInfo");
    if(!branch)
    { 
      std::cout<<"ERROR! No branch in JobInfoTree"<<std::endl;
      exit(1);
    }
    else
    {
      TJobInfo* jobInfo= new TJobInfo;
      branch->SetAddress(&jobInfo);
      branch->GetEntry(0);
      m_bossVer = jobInfo->getBossVer() ;
      std::cout<<std::endl
             << "**************************************************" << std::endl
             <<"Print JobInfo for data file: "<< file->GetName()<<std::endl
             << "          BOSS version: "<< m_bossVer << std::endl
             << "**************************************************" << std::endl
             << std::endl;

      m_decayOptions = jobInfo->getDecayOptions();
      if(m_decayOptions.size()>0)
      {
        std::cout<< std::endl
                 <<"**************************************************" << std::endl
                 <<"          Decay Options: "<<std::endl
                 <<m_decayOptions << std::endl
                 <<"**************************************************" << std::endl
                 << std::endl;
      }

      ISvcLocator* svcLocator = Gaudi::svcLocator();
      IDataInfoSvc *tmpInfoSvc;
      DataInfoSvc* jobInfoSvc;
      StatusCode status = svcLocator->service("DataInfoSvc",tmpInfoSvc);
      if (status.isSuccess()) {
      log << MSG::INFO << "get the DataInfoSvc" << endreq;
      jobInfoSvc=dynamic_cast<DataInfoSvc *>(tmpInfoSvc);
      }else {
       log << MSG::WARNING << "could not get the DataInfoSvc." << endreq;
      }
 
      m_totEvtNo = jobInfo->getTotEvtNo();
      jobInfoSvc->setTotEvtNo(m_totEvtNo);

      if(level>0)
      {    
        std::cout<<std::endl
                 <<"**************************************************" << std::endl
                 <<"          JobOptions for this data file: " << std::endl
                 << std::endl;
      
               
        m_jobOptions = jobInfo->getJobOptions();
        vector<std::string> vs = m_jobOptions;
        int nv = vs.size();
        if(nv>0)
        {
          for(int i=0;i<nv;i++)
          {
            std::cout<<vs[i]<<std::endl;
            std::cout<<"          end of the jobOptions file " << std::endl; 
            std::cout<<"**************************************************" << std::endl
                     <<std::endl;
          }
        }
      }
    }
  } 
}

TTree * RootInterface::getOtherTree(const std::string treename){
  //get other TTree for input
  log<<MSG::INFO<<"RootInterface:;getOtherTree"<<endreq;
  m_ENDFILE = false ;
  if(m_otherTrees[m_fileNum]) return m_otherTrees[m_fileNum];
  // TFile* inputFile = new TFile(m_fileNames[m_fileNum].c_str(),"READ");
  inputFiles[m_fileNum] =  TFile::Open(m_fileNames[m_fileNum].c_str(),"READ");
   
  if(!inputFiles[m_fileNum]->IsOpen()){
    log<<MSG::ERROR<<"ROOT File" <<inputFiles[m_fileNum]->GetName()<<"Coult not be opened for reading."<<endreq;
    delete inputFiles[m_fileNum];
    inputFiles[m_fileNum] = NULL;
    return NULL; // The Root can not be opened
  }
  m_EOF = false;
  log<<MSG::INFO<<"RootIntrFace:;Opened File for input:"<<m_fileNames[m_fileNum].c_str()<<endreq;
  m_currentFileName = m_fileNames[m_fileNum].c_str(); //liangyt 2008-11-19

  TTree* tree =(TTree*)inputFiles[m_fileNum]->Get(treename.c_str());//the same tree name;
  if(!tree){
    log << MSG::ERROR << "ROOT file " << inputFiles[m_fileNum]->GetName()
	<< " does not contain requested TTree: " << treename <<endreq;
    return NULL;
  }

  if(tree->GetEntries()<=0)
  {
    log << MSG::ERROR << "ROOT file "<< m_fileNames[m_fileNum].c_str()<< " entries <= 0" << endreq;
    exit(1);
  }
  
  m_otherTrees[m_fileNum] = tree;
  if (m_entries<=0){
    m_entries=(Int_t)tree->GetEntries();
    log<<MSG::INFO<<"m_entries  =  "<<m_entries<<endreq;
  }

  printJobInfo(inputFiles[m_fileNum],0);

  //delete inputFile;
  // inputFile= NULL;
  return tree; 
}

bool RootInterface::checkEndOfTree(){

  if ( m_fileNum >= int(m_fileNames.size())-1 ){
    if(m_inputFiles[0]){ 
      delete m_inputFiles[0];   
      m_inputFiles[0] = NULL;   
    }   
    return true;
  }
  
  (*m_branchesRead).Clear();
  unsigned int treenr;
  getTreeNr("Event",treenr);
  if(m_inputFiles[treenr]){ 
    delete m_inputFiles[treenr];
    m_inputFiles[treenr] = NULL;
  }
  if(m_inputTrees[treenr]){
    //delete m_inputTrees[treenr];
    m_inputTrees[treenr] = NULL;
  }
  if(m_otherTrees[m_fileNum]) delete m_otherTrees[m_fileNum];
  if(inputFiles[m_fileNum])   delete inputFiles[m_fileNum];

  m_ENDFILE = true;
  m_fileNum++;
  m_currentFileName = m_fileNames[m_fileNum].c_str();

  m_entries=-1;
  m_EOF = false;
  return false;
}

/*
bool RootInterface::checkEndOfTree(){
  static int fileNum = 1;
  
  if (m_EOF){
    if (fileNmu >m_fileNames.size())
      return true;  //End of all Files;
    else{
      Tfile inputFile = TFile::Open(m_fileNames[fileNum].c_str(),"READ");
      TTree *tree;
      if(!inputFile->IsOpen()){
	log<<MSG::ERROR<<"ROOT File" <<inputFile->GetName()<<"Coult not be opened for reading."<<endreq;
	delete inputFile;
	tree = Null; // The Root can not be opened
      }
      log<<MSG::INFO<<"RootIntrFace:;Opened File for input:"<<m_fileNames[fileNum].c_str()<<endreq;
      tree =(TTree*)m_fileNames[fileNum]->Get("Dst");//the same tree name;
      if (m_entries<=0){
	m_entries=(Int_t)tree->GetEntries();
      }
      return false;
    }
  }
  return false;
}
*/

StatusCode RootInterface::setBranchAddress(const std::string treename, const std::string branchname, void *addr,int &branchnr)
{
  log << MSG::DEBUG <<"RootInterface::setbranch address, treename: "<<treename <<", branch "<<branchname <<endreq;

  branchnr=-1;
  
  TTree * tree;
  
  if(m_fileNum != 0){
   
    tree = getOtherTree(treename);
  }else{
    tree = getTree(treename);
  } 
    
  //TTree * tree = getTree("Dst");
  if (!tree) {
    log << MSG::ERROR << "Could not find tree " << treename <<endreq;
    log << MSG::ERROR << "terminate the process handly"<<endreq;
    exit(1);
    return StatusCode::FAILURE;
  }
  tree->SetMakeClass(1);  // necessary for separate branch reading (segv otherwise)!

  //   log << MSG::INFO <<"ok!!!!!!!!!!!!!11"<<endreq; 
  TBranch *b = tree->GetBranch(branchname.c_str());
  if (!b) {
    //tree->Print();
    log << MSG::DEBUG << "Could not find branch xx" << branchname <<"xx"<<endreq;
    return StatusCode::FAILURE;
  }
  // log << MSG::INFO <<"ok!!!!!!!!!!!!!22"<<endreq;
  // log << MSG::INFO <<"ok!!!!!!!!!!!!!22"<<(*addr)<<endreq; 
  b->SetAddress(addr);  
  // log << MSG::INFO <<"ok!!!!!!!!!!!!!33"<<endreq;  
  branchnr=m_branchesRead->GetEntries();
  // log << MSG::INFO <<"ok!!!!!!!!!!!!!44"<<endreq;
  TClonesArray &a = *m_branchesRead;
  m_branchesRead->Expand(branchnr+1);
  a[branchnr]=b;
  return StatusCode::SUCCESS;
}

StatusCode RootInterface::getBranchEntry(int nr, int entry, void * addr, int& nb)
{
  log << MSG::DEBUG <<"RootInterface::getBranchEntry: "<<", branch nr "<<nr  <<", entry "<<entry <<endreq;

  if (nr <0) return StatusCode::FAILURE;
  TBranch *branch=(TBranch *)m_branchesRead->At(nr);
  if (!branch) {
    log << MSG::ERROR << "Could not find branch " << nr <<endreq;
    return StatusCode::FAILURE;
  }

  branch->SetAddress(addr);
  nb=branch->GetEntry(entry);

  if (nb<=0){
    m_EOF=true;

  }
  return StatusCode::SUCCESS;
}


StatusCode RootInterface::getBranchEntry(int nr, int entry, int& nb)
{
  log << MSG::DEBUG <<"RootInterface::getBranchEntry: "<<", branch nr "<<nr  <<", entry "<<entry <<endreq;

  if (nr <0) return StatusCode::FAILURE;
  TBranch *branch=(TBranch *)m_branchesRead->At(nr);
  if (!branch) {
    log << MSG::ERROR << "Could not find branch " << nr <<endreq;
    return StatusCode::FAILURE;
  }
  nb=branch->GetEntry(entry);
  
  if (nb<=0){
    m_EOF=true;
    
  }
  
  return StatusCode::SUCCESS;
}

StatusCode RootInterface::getTreeNr(const std::string treename,unsigned int& treenr,bool doAdd) {
  // look whether this tree has already got a number
  // if not, add it
  std::vector<std::string>::iterator where=std::find(m_treenames.begin(),m_treenames.end(),treename);
  if (where == m_treenames.end()) {
    if (doAdd) {
     treenr=m_treenames.size();
     m_treenames.push_back(treename);
     m_inputFilenames.push_back("");
     m_inputFiles.push_back(NULL);
     m_inputTrees.push_back(NULL);
     m_outputFilenames.push_back("");
     m_outputFiles.push_back(NULL);
     m_outputTrees.push_back(NULL);
     m_splitModes.push_back(0);
     m_bufSizes.push_back(0);
     m_compressionLevels.push_back(0);
     return StatusCode::SUCCESS;
    }else {
      log << MSG::ERROR << "Invalid tree name: " <<treename<< endreq;
      return StatusCode::FAILURE;
    }
  }
  treenr=where-m_treenames.begin();
  return StatusCode::SUCCESS;
}

StatusCode RootInterface::fillTrees() {
  // loop over all trees and fill them
  StatusCode sc=StatusCode::FAILURE;
  int nb;
  std::vector<TTree *>::const_iterator trees;
  for (trees=m_outputTrees.begin();trees<m_outputTrees.end();trees++) {
      if ((*trees)==NULL) continue;
      int treenr=(*trees)->GetUniqueID();
     if(m_outputFiles[treenr]->IsZombie()||(!m_outputFiles[treenr]->IsOpen())){
	    std::cout<<"RootInterface ERROR::The ROOT File:"<<m_outputFilenames[treenr].c_str()<<"status is false"<<std::endl;
	    exit(1);
	 }
      nb=(*trees)->Fill();
      m_outputFiles[treenr] = (*trees)->GetCurrentFile();
      log << MSG::DEBUG << "filled tree "<<(* trees)->GetName() <<" with "<<nb<<" bytes"<< endreq;
      if(nb==-1){
         log << MSG::FATAL << "Error in filling tree "<<(* trees)->GetName() <<" with "<<nb<<" bytes"<< endreq; 
    exit(1);
        }
      sc=StatusCode::SUCCESS;
  }
  return sc;
}





StatusCode RootInterface::f_addOutput(const std::string& treename,
                                      const std::string& file,
                                      int splitx, int bufsize, int compression){
    log << MSG::INFO << "addOutput to single event" << endreq;
    StatusCode status = StatusCode::FAILURE;
    unsigned int treenr;

    status = f_getTreeNr(treename, treenr, true);
    m_single_compressionLevels[treenr]	=	compression;
    m_single_outputFileNames[treenr]	=	file;
    m_single_outputFiles[treenr]	=	NULL;
    m_single_outputTrees[treenr]	=	NULL;
    m_single_splitModes[treenr]	=	splitx;
    m_single_bufSizes[treenr]	=	bufsize;

    std::cout << "finish f_addOutput to single event" << std::endl;
    return status;
}

StatusCode RootInterface::f_createTree(unsigned int treenr,
                                       const std::string treename){
    log << MSG::INFO << "f_createTree()" << endreq;

    TDirectory *saveDir = gDirectory;

    m_single_outputFiles[treenr] =
        TFile::Open(m_single_outputFileNames[treenr].c_str(), "RECREATE");
    if ( !m_single_outputFiles[treenr]->IsOpen()){
        log << MSG::ERROR << "ROOT share file: "
        << m_single_outputFileNames[treenr]
        << " could not be opened for writing"
        << endreq;
        return StatusCode::FAILURE;
    }
    log << MSG::INFO << "f_createTree()::open share file for writing: "
    << m_single_outputFileNames[treenr] << endreq;

    m_single_outputFiles[treenr]->cd();
    m_single_outputFiles[treenr]->SetCompressionLevel(m_single_compressionLevels[treenr]);

    std::string title = treename + " for share";
    m_single_outputTrees[treenr] = new TTree(treename.c_str(), title.c_str());
    saveDir->cd();

    return StatusCode::SUCCESS;
}

StatusCode RootInterface::f_createBranch(const std::string& treename,
        const std::string& branchname,
        const char *classname,
        void *addr, int & branchnr){
    log << MSG::INFO << "f_craeteBranch() create branch, tree name:"
    << treename << ", branch name:" << branchname << endreq;

    TBranch *branch;
    unsigned int treenr;
    StatusCode status = f_getTreeNr(treename, treenr);
    if ( !status.isSuccess()) return status;

    if ( !m_single_outputTrees[treenr])
        status = this->f_createTree(treenr, treename);
    if ( !status.isSuccess()) return status;

    TTree*	tree = m_single_outputTrees[treenr];
    tree->SetUniqueID(treenr);

    branch = tree->Branch(branchname.c_str(),
                          classname,
                          addr,
                          m_single_bufSizes[treenr],
                          m_single_splitModes[treenr]);

}

StatusCode RootInterface::f_getTreeNr(const std::string treename,
                                      unsigned int& treenr,bool doAdd){

    std::vector<std::string>::iterator where =
        std::find(m_single_treenames.begin(), m_single_treenames.end(), treename);

    if ( where == m_single_treenames.end()){
        if ( doAdd){
            treenr = m_single_treenames.size();
            m_single_treenames.push_back(treename);

            m_single_outputFileNames.push_back("");
            m_single_outputFiles.push_back(NULL);
            m_single_outputTrees.push_back(NULL);
            m_single_splitModes.push_back(0);
            m_single_bufSizes.push_back(0);
            m_single_compressionLevels.push_back(0);

            return StatusCode::SUCCESS;
        }
        else {
            log << MSG::ERROR << "Invalid share tree name: "
            << treename << endreq;
            return StatusCode::FAILURE;
        }
    }
    treenr = where - m_single_treenames.begin();
    return StatusCode::SUCCESS;
}

StatusCode RootInterface::f_fillTrees(){
    StatusCode status = StatusCode::FAILURE;
    int byte;

    std::vector<TTree *>::const_iterator tree;
    for ( tree = m_single_outputTrees.begin(); tree < m_single_outputTrees.end(); tree++){
        if ( (*tree) == NULL) continue;
        byte = (*tree)->Fill();
	(*tree)->Print();
        log << MSG::INFO << "f_fillTrees() filled tree " << (*tree)->GetName()
        << " with " << byte << " bytes!" << endreq;
        status = StatusCode::SUCCESS;
    }

    return status;
}

StatusCode RootInterface::f_finalize(){
    log << MSG::INFO << "f_finalize() in RootInterface" << endreq;

    std::vector<TTree *>::const_iterator	tree;
    for ( tree = m_single_outputTrees.begin(); tree < m_single_outputTrees.end(); tree++){
        if ( *tree){
            unsigned int treenr = (*tree)->GetUniqueID();
            log << MSG::INFO << "tree id: " << treenr << endreq;
            if ( m_single_outputFiles[treenr] ){
                if ( !m_single_outputFiles[treenr]->IsOpen()){
                    log << MSG::ERROR << "f_finalize could not open share file for writing"
                    << endreq;
                    return StatusCode::FAILURE;
                }
                else {
                    log << MSG::INFO << "Closing file:" << treenr
                    << ", tree:" << (*tree)->GetName() << endreq;

                    TDirectory *saveDir = gDirectory;
                    m_single_outputFiles[treenr]->cd();
                    log <<MSG::INFO << "WREITE TO FILE BYTES: " 
			    << m_single_outputFiles[treenr]->Write()
			    << endreq;
                    m_single_outputFiles[treenr]->Close();
                    saveDir->cd();
                }
            }
        }
    }
    return StatusCode::SUCCESS;
}
