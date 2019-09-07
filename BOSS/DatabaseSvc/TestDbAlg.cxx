#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DatabaseSvc/TestDbAlg.h"
#include "DatabaseSvc/IDatabaseSvc.h"

#include "TTree.h"
#include "TBufferFile.h"
#include <iostream>

using namespace std;

/////////////////////////////////////////////////////////////////////////////

TestDbAlg::TestDbAlg(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator){
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TestDbAlg::initialize(){
  MsgStream log(messageService(), name());

  IDatabaseSvc* m_dbsvc;
  StatusCode  sc = serviceLocator()->service("DatabaseSvc",m_dbsvc,true);
  if (sc .isFailure() ) {
    log << MSG::ERROR << "Unable to find DatabaseSvc " << endreq;
    return sc;
  }

  std::cout << "*************************************************************" << std::endl;
  std::cout << "Test 1: Numbers" << std::endl;
  std::cout << "*************************************************************" << std::endl;

  char stmt[255];
  sprintf(stmt,"select Vx, Vy, Vz, SigmaVx, SigmaVy, SigmaVz,RunNo from BeamPar where RunNo > 9950 and RunNo < 9980 and SftVer='6.5.1'");

 DatabaseRecordVector res;
 int row_no;
 row_no = m_dbsvc->query("offlinedb",stmt,res);
 if(row_no<0){
   std::cerr << "Query \"" << stmt << "\" failed" << std::endl;
   return StatusCode::FAILURE;
 }

 int row;
 double vx, svx;
 vx = 0;
 svx = 0;

 for (row=0;row<row_no;row++){
   DatabaseRecord& records = *res[row];
   sscanf(records["Vx"], "%lf", &vx);
   sscanf(records["SigmaVx"], "%lf", &svx);
   cout << "Read from DB: RunNo " << records["RunNo"] << " Vx= " << vx << " SigmaVx= " << svx << endl;
   cout << "                    " << records.GetLong("RunNo") << " Vx= " << records.GetDouble("Vx") << " SigmaVx= " << records.GetDouble("SigmaVx") << endl;
 }
 
 // Check BLOBs
 std::cout << "*************************************************************" << std::endl;
 std::cout << "Test 2: BLOBs" << std::endl;
 std::cout << "*************************************************************" << std::endl;

 res.clear();
 sprintf(stmt,"select EndTofPar,BarTofPar from TofCalConst where RunFrom <= 11000 and RunTo >= 11000 and SftVer='6.5.1'");
 row_no = m_dbsvc->query("offlinedb", stmt, res);
 if(row_no<0){
   std::cerr << "Query \"" << stmt << "\" failed" << std::endl;
   return StatusCode::FAILURE;
 }
 for (row=0;row<row_no;row++)
   {
     DatabaseRecord& records = *res[row]; 
   //  TBuffer *buf1 = new TBuffer(TBuffer::kRead);
    TBufferFile *buf1 = new TBufferFile(TBuffer::kRead);
     buf1->SetBuffer(records["EndTofPar"],32768,kFALSE);
     TTree* curvetree = new TTree();
     curvetree->Streamer(*buf1);
     double cnvAtten[8];
     curvetree -> SetBranchAddress("Atten0", &cnvAtten[0]);
     curvetree -> SetBranchAddress("Atten1", &cnvAtten[1]);
     curvetree -> SetBranchAddress("Atten2", &cnvAtten[2]);
     curvetree -> SetBranchAddress("Atten3", &cnvAtten[3]);
     curvetree -> SetBranchAddress("Atten4", &cnvAtten[4]);
     int entries=curvetree->GetEntries();
     if(entries>10) entries = 10;
     for(int iiii=0; iiii<entries;iiii++)
       {
	 curvetree->GetEntry(iiii);
	 for(int jjj=0;jjj<5;jjj++){
	   std::cout<<"cnvAtten["<<jjj<<"]="<<cnvAtten[jjj]<<" ";
	 }
	 std::cout<<std::endl;
       }
   }

 //sleep(600);
 // Test strings
 std::cout << "*************************************************************" << std::endl;
 std::cout << "Test 3: Strings" << std::endl;
 std::cout << "*************************************************************" << std::endl;

 res.clear();
 sprintf(stmt,"select XtTree,QtTree,T0Tree,SdTree,RunFrom,RunTo,CalParVer,FileName from MdcCalConst where SftVer = '6.5.3'");
 row_no = m_dbsvc->query("offlinedb",stmt,res);
 if(row_no<0){
   std::cerr << "Query \"" << stmt << "\" failed" << std::endl;
   return StatusCode::FAILURE;
 }
 
 for (row=0;row<row_no;row++){
   DatabaseRecord& records = *res[row];
   cout << "Read from DB: Runs " << records["RunFrom"] << " " << records["RunTo"] << " FileName = " << records["FileName"] << endl;
 }

 
 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode TestDbAlg::execute() {
  MsgStream log(msgSvc(), name());
  
  log << "TestDbAlg execute()" << endl;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TestDbAlg::finalize() {
  return StatusCode::SUCCESS;
}
