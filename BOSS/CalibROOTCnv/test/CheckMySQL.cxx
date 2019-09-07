#include "CheckMySQL.h"
#include "TH1F.h"
#include "TBufferFile.h"
CheckMYSQL::CheckMYSQL( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator )
{
  // Declare properties here.

}


StatusCode CheckMYSQL::initialize() {
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;
  

 return StatusCode::SUCCESS;

}


StatusCode CheckMYSQL::execute( ) {
   MsgStream log(msgSvc(), name());
 // int i;
 char *opt_host_name = "bes3db2.ihep.ac.cn";   
// char *opt_host_name = "koala.ihep.ac.cn";
 char *opt_user_name = "maqm";   
 char *opt_password  = "maqm_offline";   
 unsigned int opt_port_num = 3306;   
 char *opt_socket_name = NULL;   
 char *opt_db_name = "offlinedb";   
 unsigned int opt_flags = 0;   
 MYSQL *conn;   


    
// calibUtil::Metadata *meta = new calibUtil::Metadata(opt_host_name, "*",opt_db_name);
conn = mysql_init(NULL);   
     
 mysql_real_connect(conn, opt_host_name, opt_user_name, opt_password, 
		   opt_db_name, opt_port_num, opt_socket_name, opt_flags);
 printf("connect the database ok\n");


time_t now;
struct tm  *timenow;
timenow = localtime(&now);
printf("Local time is %s\n",asctime(timenow));

   TFile *f = new TFile("/ihepbatch/besd12/max/bBhabha_EsTime_run6018.root");
   cout<<"step11111111"<<endl;
   
   TTree *Level0 =  (TTree*)f-> Get("h2");
   TBufferFile Level0_buf(TBuffer::kWrite);
   Level0->Streamer(Level0_buf); 

   cout<<"step22222222"<<endl;

   TTree *Level1 =  (TTree*)f-> Get("calibconst");
   TBufferFile Level1_buf(TBuffer::kWrite);
   Level1->Streamer(Level1_buf); 


/*
   TH1F* LayerCntMap_1 = (TH1F*)f->Get("LayerCntMap");
//   TH1F* LayerCntMap = new TH1F((TH1F)f->Get("LayerCntMap"));
//   LayerCntMap->Copy(f->Get("LayerCntMap"));
//   const double* con = LayerCntMap_1->GetBuffer();
//   int size = LayerCntMap_1->GetBufferSize();
   std::cout<<""step5555555555"<<std::endl;
   TAxis* xaxis =  LayerCntMap_1->GetXaxis();
    double xmin = xaxis->GetXmin();
    double xmax = xaxis->GetXmax();
    double xbins = xaxis->GetNbins();
    std::cout<<""step6666666"<<std::endl;
    std::cout<<"xbins=="<<xbins<<"xmin=="<<xmin<<"xmax=="<<xmax<<std::endl;
   TH1F* LayerCntMap = new TH1F("LayerCntMap","LayerCntMap",xbins,xmin,xmax);
   for(int i=0;i<size;i++){
     LayerCntMap->Fill(con[i]);
   }
   
   TBuffer LayerCntMap_buf(TBuffer::kWrite);
   LayerCntMap->Copy(*(f->Get("LayerCntMap")));
   LayerCntMap->Streamer(LayerCntMap_buf);
   std::cout<<"55555555555555"<<std::endl;
   f->Close();
   TFile* check = new TFile("check.root","recreate");
//   LayerCntMap->Write();
   check->Write();
*/
/*   int size = 4096; 
   int bins = 10; 
   int xmin = 0; 
   int xmax = 10; 
   TH1F* histogramU16_ = new TH1F ("histogramU16_", "U16 data", bins, xmin, xmax); 
   histogramU16_->SetBit (TH1::kCanRebin); // allow automatic rebinning 

   for (int i = 0; i < 10; i++) { 
     histogramU16_->Fill(i); 
   } 
   
   char buffer[1024000]; 
   TBuffer LayerCntMap_buf(TBuffer::kWrite, 1024000, buffer, true);  
   std::cout<<"aaaaaaaaa"<<std::endl;
   histogramU16_->Streamer(LayerCntMap_buf); 

   cout<<"step55555555555"<<endl;
   TH1F* BoxCntMap = (TH1F*)f->Get("BoxCntMap");
   char buffer_1[52400];
   TBuffer BoxCntMap_buf(TBuffer::kWrite,524000,buffer_1,true);
   BoxCntMap->Streamer(BoxCntMap_buf); 
	 
   cout<<"step66666666"<<std::endl;   
   TH1F* StripCntMap = (TH1F*)f->Get("StripCntMap");
   char buffer_2[52400];
   TBuffer StripCntMap_buf(TBuffer::kWrite,524000,buffer_2,true);
   StripCntMap->Streamer(StripCntMap_buf); 
    cout<<"step7777777777"<<std::endl;
  
   TH1F* LayerProMap = (TH1F*)f->Get("LayerProMap");
    char buffer_3[52400];
   TBuffer LayerProMap_buf(TBuffer::kWrite,524000,buffer_3,true);
   LayerProMap->Streamer(LayerProMap_buf); 
	     
   cout<<"step88888888888888"<<std::endl;
   
   TH1F* BoxProMap = (TH1F*)f->Get("BoxProMap");
   char buffer_4[52400];
   TBuffer BoxProMap_buf(TBuffer::kWrite,524000,buffer_4,true);
   BoxProMap->Streamer(BoxProMap_buf); 

    cout<<"step9999999999999"<<std::endl;
   */ 
char  query[5024000],buf[1024*1000],buf1[1024*1000],*p,*p1;
unsigned int from_len,from_len1;

int status;
int id =60034;


sprintf(query,"INSERT INTO  EsTimeCalConst(SerNo,RunFrom,RunTo,EsTimeH2,EsTimeH9) VALUES(%d,'100','1000','",id);

  	printf("%s\n",query);
	p= query + strlen(query);
p += mysql_escape_string(p,Level0_buf.Buffer(),Level0_buf.BufferSize());

	strcat(p,"','");
cout<<"strlen(query)22222222222="<<strlen(query)<<endl;
     
     p= query + strlen(query);
     p +=mysql_escape_string(p,Level1_buf.Buffer(),Level1_buf.BufferSize());
	strcat(p,"')");
  	 status = mysql_query(conn,query);
         printf("%d\n",status);




//cout<<"status================"<<status<<endl;

unsigned long *lengths;
 
 MYSQL_RES *res_set;
 MYSQL_ROW row;

//char *p1;
 char stmt1[200];//="select context from mytesttable where id = %d,";
 int serno =1;
// sprintf(stmt1,"select BarTofPar,EndTofPar,RunFrom,RunTo from TofCalConst where SerNo=%d",serno);
 sprintf(stmt1,"select EsTimeH2,EsTimeH9,RunFrom,RunTo from TofCalConst where SerNo=%d",serno);

// char stmt1[100]="select  tree1,tree2 from TofCalConst where ser_no = 10026"; 
 printf("%s\n",stmt1);
 if(mysql_real_query(conn, stmt1, strlen(stmt1)) != 0)
  printf("query error\n");
 res_set = mysql_store_result (conn);
 mysql_field_seek (res_set, 0); 

 row = mysql_fetch_row (res_set);
 lengths = mysql_fetch_lengths(res_set);
for(int i=0;i<8;i++){ 
// cout<<"size of row["<<i<<"]"<<sizeof(row[i])<<endl;
  cout<<"lengths["<<i<<"]="<<lengths[i]<<endl;
  }
/*
//string ss(row[0]);
//const char *pp = ss.c_str();

 TTree* newHist= new TTree(); 
  TBuffer rootBuffer2(TBuffer::kRead,lengths[0], row[0], kFALSE); 
//  TBuffer rootBuffer2(TBuffer::kRead,lengths[0], void pp, kFALSE); 
 newHist->Streamer(rootBuffer2); 
   newHist->Print(); 
  int  nentri = newHist -> GetEntries();*/
//  double xtpar;
//  int xtkey;
/*double atten,w3;
  newHist -> SetBranchAddress("Atten", &atten);
  newHist -> SetBranchAddress("W3", &w3);
  cout<<"nentri="<<nentri<<endl;
  for(int ii=0;ii<nentri;ii++){
    newHist->GetEntry(ii);
    cout<<"Atten="<<atten<<"W3="<<w3<<endl;
  }
  */  
/* TTree* newHist1= new TTree(); 
  TBuffer rootBuffer3(TBuffer::kRead,lengths[1], row[1], kFALSE); 
  newHist1->Streamer(rootBuffer3); 
   newHist1->Print(); 
  int  nentri1 = newHist1 -> GetEntries();
//  double xtpar;
//  int xtkey;
//double atten,w3;
  double p6;
  newHist1 -> SetBranchAddress("Atten", &atten);
  newHist1 -> SetBranchAddress("P6", &p6);
  cout<<"nentri1="<<nentri1<<endl;
  for(int ii=0;ii<nentri1;ii++){
    newHist1->GetEntry(ii);
    cout<<"Atten="<<atten<<"P6="<<p6<<endl;
  }
  */  
   return StatusCode::SUCCESS;
}
StatusCode CheckMYSQL::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "        CheckMYSQL FINALIZE!! "
      << endreq;
  
  return StatusCode::SUCCESS;
}


