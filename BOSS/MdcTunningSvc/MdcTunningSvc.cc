#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <iostream>

#include "MdcTunningSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"


#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DatabaseSvc/DatabaseSvc.h"

using namespace std;

MdcTunningSvc::MdcTunningSvc( const string& name, ISvcLocator* svcloc) :
  Service (name, svcloc){
  m_BesMdcRes=0; 
  // declare properties
  declareProperty("UseDatabase",m_dbFlag = false);
  declareProperty("UseEndcapTuning",m_EndcapTuning = 1); // 0:no endcap Tuning , 1: using endcap Tuning
  declareProperty("EffFile", m_effFile = std::string("no path"));
  declareProperty("ResFile", m_resFile = std::string("no path"));
  declareProperty("EffFile_endcap", m_effFile_endcap = std::string("no path"));
  declareProperty("ResFile_endcap", m_resFile_endcap = std::string("no path"));
  declareProperty("path_mdc", m_path = std::string("no path"));
  declareProperty("Host" , host = std::string("bes3db2.ihep.ac.cn"));
  declareProperty("DbName" , dbName = std::string("offlinedb"));
  declareProperty("UserName" , userName = std::string("guest"));
  declareProperty("Password" , password = std::string("guestpass"));
  declareProperty("SerialNo" , serialNo = 0);
  declareProperty("fromDB", m_fromDB = true);
  declareProperty("ParBossVer", m_ParBossVer = std::string("unknown"));

  int no[43]={40,44,48,56,64,72,80,80,76,76,88,88,100,100,112,112,128,128,140,140,160,160,160,160,176,176,176,176,208,208,208,208,240,240,240,240,256,256,256,256,288,288,288};

  for(int i=0;i<43;i++){
    cellNo[i]=no[i];
  }

}

MdcTunningSvc::~MdcTunningSvc(){
  if(m_BesMdcRes) delete m_BesMdcRes;
}

StatusCode MdcTunningSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
  if( IID_IMdcTunningSvc.versionMatch(riid) ){
    *ppvInterface = static_cast<IMdcTunningSvc*> (this);
  } else{
    return Service::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}


StatusCode MdcTunningSvc::initialize(){
  MsgStream log(messageService(), name());
  log << MSG::INFO << "========== MdcTunningSvc::initialize() ==========" << endreq;

  m_ParBossVer=getenv("BES_RELEASE");

  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) return sc;
	
	if(m_fromDB)
		cout << " MdcTunningSvc read from database. " << endl;
	else if(!m_fromDB)
		cout << " MdcTunningSvc read from localfile. " << endl;

  if(m_fromDB==true){
    
    sc = serviceLocator()->service("DatabaseSvc", m_dbsvc, true);
    if (sc .isFailure() ) {
      log << MSG::ERROR << " ERROR: unable to find DatabaseSvc " << endreq;
      return sc;
    }

    //  MYSQL *conn;
    //  char *opt_host_name = "202.122.33.53";
    //	  char *opt_user_name = "maqm";
    //	  char *opt_password  = "maqm_offline";
    ////    unsigned int opt_port_num = 3306;
    //// char *opt_socket_name = NULL;
    //	  char *opt_db_name = "offlinedb";
    ////conn = mysql_init(NULL);
    ////unsigned int opt_flags = 0;
    ////mysql_real_connect(conn, host.c_str(), userName.c_str(), password.c_str(),
    ////		       dbName.c_str(), opt_port_num, opt_socket_name, opt_flags);
    //  mysql_real_connect(conn, opt_host_name, opt_user_name, opt_password, 
    //                   opt_db_name, opt_port_num, opt_socket_name, opt_flags);

	
    IIncidentSvc* incsvc;
    sc = service("IncidentSvc", incsvc);
    int priority = 100;
    if( sc.isSuccess() ){
      incsvc -> addListener(this, "NewRun", priority);
    }
    sc = serviceLocator()->service("EventDataSvc", m_eventSvc, true);
    if (sc .isFailure() ) {
      log << MSG::ERROR << " ERROR: unable to find EventDataSvc " << endreq;
      return sc;
    }
  }

  if(m_fromDB!=true){
    bool initStat = initTuningConst();
    //    if(m_path!=std::string("no path")) setMdcRes(m_path);
		if(!initStat){
			cout << "========== MdcTunningSvc::initialize() failure ! ==========" << endl;
			return StatusCode::FAILURE;	
		}
	}

  return StatusCode::SUCCESS;
}

StatusCode MdcTunningSvc::finalize(){
  MsgStream log(messageService(), name());
  log << MSG::INFO << "========== MdcTunningSvc::finalize() ==========" << endreq;
  ////  mysql_close(conn);

  return StatusCode::SUCCESS;
}

void MdcTunningSvc::handle(const Incident& inc){
  cout << "========== MdcTunningSvc::handle() ==========" << endl;
	
	MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;

  if ( inc.type() == "NewRun" ){
    log << MSG::DEBUG << "NewRun" << endreq;
    if(m_fromDB==true) {
			cout << " Start getMdcTuningTableInfo. " << endl;
      StatusCode sc = getMdcTuningTableInfo();   
      if(sc.isFailure()) {
				cout << " ERROR: can not get MdcTuning data from the database. " << endl;
				log << MSG::ERROR << " ERROR: can not get MdcTuning data from the database. " << endreq;
				exit(1);
      }
    }
  }
}

bool MdcTunningSvc::initTuningConst(){
  std::string FilePath = getenv("MDCTUNNINGSVCROOT");

  if(m_effFile==std::string("no path")){
    if(!m_fromDB)	cout << " ERROR: no mdc tuning eff file, please check the input! " << endl;
		//m_effFile=FilePath+"/dat/mc_eff.dat";
    //m_effFile = FilePath+"/dat/mc_eff_psipp.dat";
		return false;
  }
  bool setMcEffStat = setMcEff(m_effFile);
  
	if(m_resFile==std::string("no path")){
    if(!m_fromDB)	cout << " ERROR: no mdc tuning res file, please check the input! " << endl;
    //m_resFile=FilePath+"/dat/mc_res.dat";
    //m_resFile = FilePath+"/dat/mc_res_psipp.dat";
		return false;
	}
  //setMcRes2(m_resFile);
  bool setMcRes3Stat = setMcRes3(m_resFile);
	
	if(!(setMcRes3Stat&&setMcEffStat))
		return false;
 
  return true;
}

bool MdcTunningSvc::setMcEff(std::string eff_con){
  int i,j;
  string line;
  double lay,bin,expect,hit;
  //ifstream readMCEff(eff_con.c_str());
  std::istringstream readMCEff;
  // (eff_con);
  if(m_fromDB){
    readMCEff.str(eff_con);
  }
  if(!m_fromDB){
    ifstream in(eff_con.c_str());
    //char ch;
    //string hhh;
    //while(ift.get(ch)) hhh.putback(ch);
    //std::cout<<"hhh:"<<hhh<<std::endl; 
    //     stringstream strBuf ;
    istreambuf_iterator<char> iter(in) ;
    string strCache = string( iter, (istreambuf_iterator<char>()) );
    readMCEff.str(strCache);
    //std::cout<<"strCache:"<<strCache<<std::endl;
  }

	
	ifstream fin(eff_con.c_str());
	if(!m_fromDB)
		if(!fin){
			cout << " ERROR: the mdc tunning eff file " << m_effFile << " not exist, please check the input! " << endl;
			return false;
		}
  //  ifstream readMCEff(eff_con);
  if(!readMCEff.good()){
    cout << " ERROR: mdc tuning eff file " << m_effFile << " not exist. " << endl;
    return false;
  }else{
    if(!m_fromDB)
			if(fin)
				cout << " Open mdc tuning eff file: " << m_effFile << endl;
    for(i=0;i<43;i++){
      readMCEff>>lay;
      getline(readMCEff,line);
      for(j=0;j<docaNo;j++){
	readMCEff>>bin>>docaEff[i][j]>>expect>>hit; 
      }
      readMCEff>>lay;
      getline(readMCEff,line);
      for(j=0;j<thetaNo;j++){
	readMCEff>>bin>>thetaEff[i][j]>>expect>>hit; 
      }
      readMCEff>>lay;
      getline(readMCEff,line);
      for(j=0;j<cellNo[i];j++){
	readMCEff>>bin>>cellEff[i][j]>>expect>>hit;
			
      }
    }
    for(i=0;i<43;i++){
      readMCEff>>lay;
      getline(readMCEff,line);
      for(j=0;j<docaNo;j++){
	readMCEff>>bin>>docaEff_2[i][j]>>expect>>hit;
      }
      readMCEff>>lay;
      getline(readMCEff,line);
      for(j=0;j<thetaNo;j++){
	readMCEff>>bin>>thetaEff_2[i][j]>>expect>>hit;
			
      }
      readMCEff>>lay;
      getline(readMCEff,line);
      for(j=0;j<cellNo[i];j++){
	readMCEff>>bin>>cellEff_2[i][j]>>expect>>hit;
				
      }
    }
  }
  return true;
}

bool MdcTunningSvc::setMcRes(){

  
  int i,j;
  string line;
  double lay,bin;
  ifstream readMCRes(m_resFile.c_str());
  if(!readMCRes.good()){
    cout << " ERROR: mdc tuning file: " << m_resFile << " not exist. " << endl;
    return false;
  }else{
    cout << " Open mdc tuning file: " << m_resFile << endl;
    for(i=0;i<43;i++){
      readMCRes>>lay;
      getline(readMCRes,line);
      getline(readMCRes,line);
      for(j=0;j<docaNo;j++){
	readMCRes>>bin>>docaRes[i][j][0][0]>>docaRes[i][j][0][1]; //entranceAngle<0
      }
      readMCRes>>lay;
      getline(readMCRes,line);
      getline(readMCRes,line);
      for(j=0;j<docaNo;j++){
	readMCRes>>bin>>docaRes[i][j][1][0]>>docaRes[i][j][1][1]; //entranceAngle>0
	/*
	  if(i==0||i==42){
	  cout<<"lay "<<i<<" docaNo "<<j<<" <0 mean "<<docaRes[i][j][0][0]<<" sigma "<<docaRes[i][j][0][1]<<" >0 mean "<<docaRes[i][j][1][0]<<" sigma "<<docaRes[i][j][1][1]<<endl;
	  }
	*/
      }
    }
  }
  return true;
}

bool MdcTunningSvc::setMcRes2(std::string res_con){
 
  int i,j;
  string line;
  double lay,bin;
  //ifstream readMCRes(m_resFile.c_str());
  std::istringstream readMCRes;
  if(m_fromDB)
    {
      readMCRes.str(res_con);
    }
  if(!m_fromDB)
    {
      ifstream in(res_con.c_str());
      istreambuf_iterator<char> iter(in) ;
      string strCache = string( iter, (istreambuf_iterator<char>()) );
      readMCRes.str(strCache);
    }
  if(!readMCRes.good()){
    cout << " ERROR: mdc tuning file: " << m_resFile << " not exist. " << endl;
    return false;
  }else{
    cout << " MdcTunningSvc::setMcRes2() Open mdc tuning resfile " << m_resFile << endl;
    for(i=0;i<43;i++){
      readMCRes>>lay;
      getline(readMCRes,line);
      getline(readMCRes,line);
      for(j=0;j<docaNo;j++){
	readMCRes>>bin>>docaF[i][j][0]>>docaMean1[i][j][0]>>docaSigma1[i][j][0]>>docaMean2[i][j][0]>>docaSigma2[i][j][0]; //entranceAngle<0
      }
      readMCRes>>lay;
      getline(readMCRes,line);
      getline(readMCRes,line);
      for(j=0;j<docaNo;j++){
	readMCRes>>bin>>docaF[i][j][1]>>docaMean1[i][j][1]>>docaSigma1[i][j][1]>>docaMean2[i][j][1]>>docaSigma2[i][j][1]; //entranceAngle>0
      }
    }
    for(i=0;i<43;i++){
      readMCRes>>lay;
      getline(readMCRes,line);
      getline(readMCRes,line);
      for(j=0;j<docaNo;j++){
	readMCRes>>bin>>docaF_2[i][j][0]>>docaMean1_2[i][j][0]>>docaSigma1_2[i][j][0]>>docaMean2_2[i][j][0]>>docaSigma2_2[i][j][0]; //entranceAngle<0
      }
      readMCRes>>lay;
      getline(readMCRes,line);
      getline(readMCRes,line);
      for(j=0;j<docaNo;j++){
	readMCRes>>bin>>docaF_2[i][j][1]>>docaMean1_2[i][j][1]>>docaSigma1_2[i][j][1]>>docaMean2_2[i][j][1]>>docaSigma2_2[i][j][1]; //entranceAngle>0
      }
    }
  }
  return true;
}

bool MdcTunningSvc::setMcRes3(std::string res_con){
 
  int i,j;
  string line;
  double lay,bin;
  //ifstream readMCRes(m_resFile.c_str());
  std::istringstream readMCRes;
  if(m_fromDB){
    readMCRes.str(res_con);
  }
  if(!m_fromDB){
    ifstream in(res_con.c_str());
    istreambuf_iterator<char> iter(in) ;
    string strCache = string( iter, (istreambuf_iterator<char>()) );
    readMCRes.str(strCache);
  }
	
	ifstream fin(res_con.c_str());
	
	if(!m_fromDB)
		if(!fin){
			cout << " ERROR: the mdc tunning res file " << m_resFile << " not exist, please check the input!" << endl;
			return false;
		}
  if(!readMCRes.good()){
    cout << " ERROR: the mdc tuning res file: " << m_resFile << " not exist, please check the input! " << endl;
    return false;
  }else{
    if(!m_fromDB)
			if(fin)
				cout << " MdcTunningSvc::setMcRes3() Open mdc tuning resfile: " << m_resFile << endl;
    for(i=0;i<43;i++){
      readMCRes>>lay;
      getline(readMCRes,line);
      getline(readMCRes,line);
      for(j=0;j<docaNo;j++){
	readMCRes>>bin>>docaF[i][j][0]>>docaMean1[i][j][0]>>docaSigma1[i][j][0]>>docaMean2[i][j][0]>>docaSigma2[i][j][0]>>resLargest[i][j][0]>>resSmallest[i][j][0]>>resRatio[i][j][0]; //entranceAngle<0
      }
      readMCRes>>lay;
      getline(readMCRes,line);
      getline(readMCRes,line);
      for(j=0;j<docaNo;j++){
	readMCRes>>bin>>docaF[i][j][1]>>docaMean1[i][j][1]>>docaSigma1[i][j][1]>>docaMean2[i][j][1]>>docaSigma2[i][j][1]>>resLargest[i][j][1]>>resSmallest[i][j][1]>>resRatio[i][j][1]; //entranceAngle>0
		
      }	
    }
    for(i=0;i<43;i++){
      readMCRes>>lay;
      getline(readMCRes,line);
      getline(readMCRes,line);
      for(j=0;j<docaNo;j++){
	readMCRes>>bin>>docaF_2[i][j][0]>>docaMean1_2[i][j][0]>>docaSigma1_2[i][j][0]>>docaMean2_2[i][j][0]>>docaSigma2_2[i][j][0]>>resLargest_2[i][j][0]>>resSmallest_2[i][j][0]>>resRatio_2[i][j][0]; //entranceAngle<0
      }
      readMCRes>>lay;
      getline(readMCRes,line);
      getline(readMCRes,line);
      for(j=0;j<docaNo;j++){
			
	readMCRes>>bin>>docaF_2[i][j][1]>>docaMean1_2[i][j][1]>>docaSigma1_2[i][j][1]>>docaMean2_2[i][j][1]>>docaSigma2_2[i][j][1]>>resLargest_2[i][j][1]>>resSmallest_2[i][j][1]>>resRatio_2[i][j][1]; //entranceAngle>0
			

      }
    }
  }

  return true;
}



BesMdcRes* MdcTunningSvc::getMdcRes(){
  return m_BesMdcRes;
}

void MdcTunningSvc::setMdcRes(std::string path){
  if(m_BesMdcRes) delete m_BesMdcRes;
  m_BesMdcRes = new BesMdcRes(path);
}

double MdcTunningSvc::NewSig(int layerId,double driftD){
  int bindD = 0;
  double mindD = 0.0 ;
  double maxdD = 9.0 ;
  int  maxbin =8.0 ;

  if((driftD<mindD)||(driftD>maxdD)){
    bindD = maxbin ;
  }else {
    for(int kk = 0; kk < 9; kk++){
      if((driftD>=(double)kk)&&(driftD<(double)(kk+1))){
	bindD = kk ; }   
    }
  }

  double sigma1 = 0 ;

  sigma1   = (m_BesMdcRes -> getD_dD(layerId ,bindD)) ;

  return sigma1;
}


double MdcTunningSvc::DeldriftD(int layerId,double driftD){
  int bindD = 0;
  int maxbin =8 ;
  double mindD = 0.0 ;
  double maxdD = 9.0 ;

  for(int jj =0;jj<9;jj++){
    if((driftD<mindD)||(driftD>maxdD)){
      bindD = maxbin;
    }else if((driftD>=jj)&&(driftD<(jj+1))){
      bindD = jj ;
    }
  }
  double y0D  = (m_BesMdcRes -> getD_dD(layerId ,bindD)) ;
  double y1D  = (m_BesMdcRes -> getD_dD(layerId, bindD+1)) ;
  double yD   = y0D + (y1D-y0D)*(driftD - bindD);  // calculate the data 
  double y0M  = (m_BesMdcRes -> getM_dD(layerId ,bindD)) ;
  double y1M  = (m_BesMdcRes -> getM_dD(layerId ,bindD+1));
  double yM   = y0M + (y1M-y0M)*(driftD - bindD);   // calculate the mc data
  double dely = yD - yM ;


  /*if((bindD>=1)&&(bindD<=4)){
  // cout<<"layerId :"<<layerId<<" dritfD :"<<driftD<<" dely*0.618:"<<dely*0.618<<endl;
  return dely*0.618 ;
  }else if((bindD>=5)&&(bindD<7)||(bindD ==8)){
  // cout<<"layerId :"<<layerId<<" dritfD :"<<driftD<<" dely*0.418:"<<dely*0.418<<endl; 
  return dely*0.418 ;
  }else { 
  // cout<<"layerId :"<<layerId<<" dritfD :"<<driftD<<" dely:"<<dely<<endl;
  return dely;
  }*/
  return dely;

}

double MdcTunningSvc::Delcostta(int layerId,double costta){
  int binTa = 0;
  int maxTa = 15;
  double minCos   = -0.8 ;
  double minCos2  = -0.7 ;
  double maxCos   =  0.8 ;

  for(int ii = 0; ii <16; ii++){
    if((costta<minCos)||(costta>maxCos)){
      binTa = maxTa;
    }else if((costta>=(minCos + ii*0.1))&&(costta<(minCos2 + ii*0.1))){
      binTa = ii;
    }
  }

  double  y0D = (m_BesMdcRes -> getD_theta(layerId ,binTa));
  double  y1D = (m_BesMdcRes -> getD_theta(layerId ,binTa+1));
  double  y0M = (m_BesMdcRes -> getM_theta(layerId,binTa));
  double  y1M = (m_BesMdcRes -> getM_theta(layerId,binTa+1));

  double yD[16],yM[16],Del[16];
  for(int ll =0;ll<16;ll++){
    yD[ll]  = y0D + (y1D - y0D)/0.1*(costta - (minCos + ll*0.1));
    yM[ll]  = y0M + (y1M - y0D)/0.1*(costta - (minCos + ll*0.1));
    Del[ll] = yD[ll] - yM[ll] ;
  } 

  double delTha = 0;

  if((binTa>=0)&&(binTa<=5)){
    delTha = Del[binTa] * 0.118 ;
  }else if((binTa>5)&&(binTa<=10)){
    delTha = Del[binTa] * 0.518 ;
  }else if((binTa>10)&&(binTa<=15)){
    delTha = Del[binTa] * 0.218 ;
  }

  return delTha ;

}


double MdcTunningSvc::GetEff(int layerId,int cellId,double driftD,double cosTheta,int posFlag){
  driftD=fabs(driftD);
  if(driftD>12){
    //std::cout<<"MdcTuningSvc: driftD overflow "<<driftD<<std::endl;
    driftD=12;
  }
  if(posFlag==0)driftD *= -1;

  if(layerId<0 || layerId>42)std::cout<<" MdcTuningSvc:wrong LayerId "<<layerId<<std::endl;
  if(cellId<0 || cellId>=cellNo[layerId])std::cout<<"MdcTuningSvc:wrong cellId "<<cellId<<std::endl;

  if(fabs(cosTheta)>1){
    std::cout<<"MdcTuningSvc:wrong coseTheta "<<cosTheta<<std::endl; 
    cosTheta=1;
  }
  double eff;
  int thetaBin=(int)floor((cosTheta+1)*thetaNo/2.);
  //debug
  //  std::cout<<"cosTheta "<<cosTheta<<" caled "<<(cosTheta+1)*thetaNo/2.<<" floor "<<thetaBin<<std::endl;
  int docaBin=(int)floor((driftD+12)*docaNo/24.);
  if(m_EndcapTuning==0)
    eff = docaEff[layerId][docaBin] * thetaEff[layerId][thetaBin] * cellEff[layerId][cellId];
  else { 
    if(fabs(cosTheta)<=0.83) 
      eff = docaEff[layerId][docaBin] * thetaEff[layerId][thetaBin] * cellEff[layerId][cellId];
    else   
      eff = docaEff_2[layerId][docaBin] * thetaEff_2[layerId][thetaBin] * cellEff_2[layerId][cellId];

  }
  //debug
  //std::cout<<"tuning svc layer "<<layerId<<"doca"<<docaBin<<" theta "<<thetaBin<<"cell"<<cellId<<" eff "<<eff<<std::endl;

  return eff;
}



double MdcTunningSvc::GetRes(int layerId,int cellId,double driftD,double cosTheta,int posFlag,double entranceAngle,double& mean,double& sigma){

  driftD=fabs(driftD);
  if(driftD>12){
    //std::cout<<"MdcTuningSvc: driftD overflow "<<driftD<<std::endl;
    driftD=12;
  }
  if(posFlag==0)driftD *= -1;

  if(layerId<0 || layerId>42)std::cout<<" MdcTuningSvc:wrong LayerId "<<layerId<<std::endl;
  if(cellId<0 || cellId>=cellNo[layerId])std::cout<<"MdcTuningSvc:wrong cellId "<<cellId<<std::endl;

  if(fabs(cosTheta)>1){
    std::cout<<"MdcTuningSvc:wrong cosTheta "<<cosTheta<<std::endl; 
    cosTheta=1;
  }

  ////  int thetaBin=(int)floor((cosTheta+1)*thetaNo/2.);
  //debug
  //  std::cout<<"cosTheta "<<cosTheta<<" caled "<<(cosTheta+1)*thetaNo/2.<<" floor "<<thetaBin<<std::endl;
  int docaBin=(int)floor((driftD+12)*docaNo/24.);
  if(entranceAngle<0){
    mean=docaRes[layerId][docaBin][0][0];
    sigma=docaRes[layerId][docaBin][0][1];
  }else{
    mean=docaRes[layerId][docaBin][1][0];
    sigma=docaRes[layerId][docaBin][1][1];
  }

  //debug
  //std::cout<<"tuning svc layer "<<layerId<<" driftD "<<driftD<<" posFlag "<<posFlag<<" doca "<<docaBin<<" theta "<<thetaBin<<" angle "<<entranceAngle<<" mean "<<mean<<" sigma "<<sigma<<std::endl;
	
  return 1;
}

double MdcTunningSvc::GetRes2(int layerId,int cellId,double driftD,double cosTheta,int posFlag,double entranceAngle,double& f,double& mean1,double& sigma1,double& mean2,double& sigma2){
	
  driftD=fabs(driftD);
  if(driftD>12){
    //std::cout<<"MdcTuningSvc: driftD overflow "<<driftD<<std::endl;
    driftD=12;
  }
  if(posFlag==0)driftD *= -1;

  if(layerId<0 || layerId>42)std::cout<<" MdcTuningSvc:wrong LayerId "<<layerId<<std::endl;
  if(cellId<0 || cellId>=cellNo[layerId])std::cout<<"MdcTuningSvc:wrong cellId "<<cellId<<std::endl;

  if(fabs(cosTheta)>1){
    std::cout<<"MdcTuningSvc:wrong cosTheta "<<cosTheta<<std::endl; 
    cosTheta=1;
  }

  ////  int thetaBin=(int)floor((cosTheta+1)*thetaNo/2.);
  //debug
  //  std::cout<<"cosTheta "<<cosTheta<<" caled "<<(cosTheta+1)*thetaNo/2.<<" floor "<<thetaBin<<std::endl;
  int docaBin=(int)floor((driftD+12)*docaNo/24.);
  if(m_EndcapTuning==0) {
    if(entranceAngle<0){
      f=docaF[layerId][docaBin][0];
      mean1=docaMean1[layerId][docaBin][0];
      sigma1=docaSigma1[layerId][docaBin][0];
      mean2=docaMean2[layerId][docaBin][0];
      sigma2=docaSigma2[layerId][docaBin][0];
    }else{
      f=docaF[layerId][docaBin][1];
      mean1=docaMean1[layerId][docaBin][1];
      sigma1=docaSigma1[layerId][docaBin][1];
      mean2=docaMean2[layerId][docaBin][1];
      sigma2=docaSigma2[layerId][docaBin][1];
    }
  }else {
    if(fabs(cosTheta)<=0.83) {
      if(entranceAngle<0){
	f=docaF[layerId][docaBin][0];
	mean1=docaMean1[layerId][docaBin][0];
	sigma1=docaSigma1[layerId][docaBin][0];
	mean2=docaMean2[layerId][docaBin][0];
	sigma2=docaSigma2[layerId][docaBin][0];
      }else{
	f=docaF[layerId][docaBin][1];
	mean1=docaMean1[layerId][docaBin][1];
	sigma1=docaSigma1[layerId][docaBin][1];
	mean2=docaMean2[layerId][docaBin][1];
	sigma2=docaSigma2[layerId][docaBin][1];
      }  
    } else {
      if(entranceAngle<0){
	f=docaF_2[layerId][docaBin][0];
	mean1=docaMean1_2[layerId][docaBin][0];
	sigma1=docaSigma1_2[layerId][docaBin][0];  
	mean2=docaMean2_2[layerId][docaBin][0];   
	sigma2=docaSigma2_2[layerId][docaBin][0]; 
      }else{
	f=docaF_2[layerId][docaBin][1];
	mean1=docaMean1_2[layerId][docaBin][1];
	sigma1=docaSigma1_2[layerId][docaBin][1];
	mean2=docaMean2_2[layerId][docaBin][1];
	sigma2=docaSigma2_2[layerId][docaBin][1];
      }
    }  
  } 
	
  //debug
  //std::cout<<"tuning svc layer "<<layerId<<" driftD "<<driftD<<" posFlag "<<posFlag<<" doca "<<docaBin<<" theta "<<thetaBin<<" angle "<<entranceAngle<<" f "<<f<<" mean1 "<<mean1<<" sigma1 "<<sigma1<<" mean2 "<<mean2<<" sigma2 "<<sigma2<<std::endl;

  return 1;
}

double MdcTunningSvc::GetRes3(int layerId,int cellId,double driftD,double cosTheta,int posFlag,double entranceAngle,double& f,double& mean1,double& sigma1,double& mean2,double& sigma2,double& ResLargest,double& ResSmallest,double& ResRatio){
  
  driftD=fabs(driftD);
  if(driftD>12){
    //std::cout<<"MdcTuningSvc: driftD overflow "<<driftD<<std::endl;
    driftD=12;
  }
  if(posFlag==0)driftD *= -1;

  if(layerId<0 || layerId>42)std::cout<<" MdcTuningSvc:wrong LayerId "<<layerId<<std::endl;
  if(cellId<0 || cellId>=cellNo[layerId])std::cout<<"MdcTuningSvc:wrong cellId "<<cellId<<std::endl;

  if(fabs(cosTheta)>1){
    std::cout<<"MdcTuningSvc:wrong cosTheta "<<cosTheta<<std::endl; 
    cosTheta=1;
  }

  ////int thetaBin=(int)floor((cosTheta+1)*thetaNo/2.);
  //debug
  //  std::cout<<"cosTheta "<<cosTheta<<" caled "<<(cosTheta+1)*thetaNo/2.<<" floor "<<thetaBin<<std::endl;
  int docaBin=(int)floor((driftD+12)*docaNo/24.);
  if(m_EndcapTuning==0) {
    if(entranceAngle<0){
      f=docaF[layerId][docaBin][0];
      mean1=docaMean1[layerId][docaBin][0];
      sigma1=docaSigma1[layerId][docaBin][0];
      mean2=docaMean2[layerId][docaBin][0];
      sigma2=docaSigma2[layerId][docaBin][0];
      ResLargest=resLargest[layerId][docaBin][0];	
      ResSmallest=resSmallest[layerId][docaBin][0];
      ResRatio=resRatio[layerId][docaBin][0];

    }else{
      f=docaF[layerId][docaBin][1];
      mean1=docaMean1[layerId][docaBin][1];
      sigma1=docaSigma1[layerId][docaBin][1];
      mean2=docaMean2[layerId][docaBin][1];
      sigma2=docaSigma2[layerId][docaBin][1];
      ResLargest=resLargest[layerId][docaBin][1];	
      ResSmallest=resSmallest[layerId][docaBin][1];
      ResRatio=resRatio[layerId][docaBin][1];
    }
  }else {
    if(fabs(cosTheta)<=0.83) {
      if(entranceAngle<0){
	f=docaF[layerId][docaBin][0];
	mean1=docaMean1[layerId][docaBin][0];
	sigma1=docaSigma1[layerId][docaBin][0];
	mean2=docaMean2[layerId][docaBin][0];
	sigma2=docaSigma2[layerId][docaBin][0];
	ResLargest=resLargest[layerId][docaBin][0];	
	ResSmallest=resSmallest[layerId][docaBin][0];
	ResRatio=resRatio[layerId][docaBin][0];
      }else{
	f=docaF[layerId][docaBin][1];
	mean1=docaMean1[layerId][docaBin][1];
	sigma1=docaSigma1[layerId][docaBin][1];
	mean2=docaMean2[layerId][docaBin][1];
	sigma2=docaSigma2[layerId][docaBin][1];
	ResLargest=resLargest[layerId][docaBin][1];	
	ResSmallest=resSmallest[layerId][docaBin][1];
	ResRatio=resRatio[layerId][docaBin][1];
      }  
    } else {
      if(entranceAngle<0){
	f=docaF_2[layerId][docaBin][0];
	mean1=docaMean1_2[layerId][docaBin][0];
	sigma1=docaSigma1_2[layerId][docaBin][0];  
	mean2=docaMean2_2[layerId][docaBin][0];   
	sigma2=docaSigma2_2[layerId][docaBin][0]; 
	ResLargest=resLargest_2[layerId][docaBin][0];	
	ResSmallest=resSmallest_2[layerId][docaBin][0];
	ResRatio=resRatio_2[layerId][docaBin][0];
      }else{
	f=docaF_2[layerId][docaBin][1];
	mean1=docaMean1_2[layerId][docaBin][1];
	sigma1=docaSigma1_2[layerId][docaBin][1];
	mean2=docaMean2_2[layerId][docaBin][1];
	sigma2=docaSigma2_2[layerId][docaBin][1];
	ResLargest=resLargest_2[layerId][docaBin][1];	
	ResSmallest=resSmallest_2[layerId][docaBin][1];
	ResRatio=resRatio_2[layerId][docaBin][1];

      }
    }  
  } 
	
  //debug
  //std::cout<<"tuning svc layer "<<layerId<<" driftD "<<driftD<<" posFlag "<<posFlag<<" doca "<<docaBin<<" theta "<<thetaBin<<" angle "<<entranceAngle<<" f "<<f<<" mean1 "<<mean1<<" sigma1 "<<sigma1<<" mean2 "<<mean2<<" sigma2 "<<sigma2<<std::endl;
  //debug information
  //std::cout<<"MdcTunningSvc::GetRes3() debug Info."<<endl
  //		<<"layer	docaBin		thetaBin	entranceAngle		f	mean1		sigma1		mean2	sigma2		largest 	smallest	ratio"<<endl
  //		<<setw(5)<<layerId<<setw(5)<<docaBin<<setw(5)<<thetaBin<<setw(18)<<entranceAngle<<setw(15)<<f<<setw(15)<<mean1<<setw(15)<<sigma1<<setw(15)<<mean2<<setw(15)<<sigma2<<setw(15)<<ResLargest<<setw(15)<<ResSmallest<<ResRatio<<endl;

  return 1;
}

double MdcTunningSvc::ResvEntr(int layerId,double enterA,int ilr ,double driftD){
  int bindD =0;
  int maxbin = 17;
  int iEntr = 0;
  int ll = -1;
  double mindD = -9.;
  double maxdD = 9.;
  double sigmaE = 0.0;

  if(enterA < 0){
    iEntr = 0 ;
  }else{ iEntr = 1 ;}

  if(ilr == 0 ){
    driftD = -1.*driftD;
  }else{ driftD = driftD ;}

  if( (driftD < mindD) || (driftD > maxdD) ){
    bindD = maxbin ;
  }else{
    for(double dd=-9.;dd<9.;dd++){
      ll++;
      if( (driftD>= dd ) && (driftD < (dd+1.)) ){
	bindD = ll ;
      }
    }
  }

  sigmaE = (m_BesMdcRes -> getD_iEntr(layerId,iEntr,bindD) );
  //cout<<"Svc lay : "<<layerId<<" iEntr : "<<iEntr<<" bindD : "<<bindD<<" sigmaE :"<<sigmaE<<endl;
  return sigmaE;
}

double MdcTunningSvc::DelEtr_Sig(int lay,double enterA,int ilr,double driftD){
  int bindD = 0;
  int maxbin =17;
  int iEntr =0;
  int ll = -1;
  double mindD = -9.;
  double maxdD =9.0;
  //// double delsigE =0;

  if(enterA < 0){
    iEntr = 0;
  }else {iEntr = 1;}

  if(ilr == 0 ){
    driftD = -1.*driftD;
  }else{ driftD = driftD ;}

  if( (driftD < mindD) ||  (driftD > maxdD) ){
    bindD = maxbin;
  }else {
    for(double dd =-9.; dd<9.;dd++){
      ll++;
      if( (driftD>=dd ) && (driftD < (dd+1.)) ){
	bindD = ll;
	dD[bindD] = dd;
      }
    }
  }

  double  y0D = (m_BesMdcRes -> getD_iEntr(lay,iEntr,bindD)  );
  double  y1D = (m_BesMdcRes -> getD_iEntr(lay,iEntr,bindD+1) );  
  double  yD  = y0D + (y1D-y0D)*(driftD - dD[bindD]);  // calculate the data
  double  y0M = (m_BesMdcRes -> getM_iEntr(lay,iEntr,bindD) );
  double  y1M = (m_BesMdcRes -> getM_iEntr(lay,iEntr,bindD+1));
  double  yM  = y0M + (y1M-y0M)*(driftD - dD[bindD]);   // calculate the mc data
  double   dely = yD - yM ;

  //cout<<"Svc lay:"<<lay<<" iEntr :"<<iEntr<<" bindD :"<<bindD
  //    <<" dD["<<bindD<<"] : "<<dD[bindD]
  //   <<" y0D : "<<y0D<<" y1D : "<<y1D<<" yD :"<<yD
  //   <<" y0M : "<<y0M<<" y1M : "<<y1M<<" yM :"<<yM<<" dely : "<<dely<<endl;

  return dely;

}

StatusCode MdcTunningSvc::getMdcTuningTableInfo(){
  MsgStream log(messageService(), name());
  SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc,"/Event/EventHeader");
  int run=eventHeader->runNumber();

  log << MSG::INFO << "MdcTuningSvc::getMdcTuningTableInfo() run =" << run << endreq;
  if(m_ParBossVer==std::string("unknown"))
		cout << "MdcTuningSvc::getMdcTuningTableInfo() : ERROR: there is no ParBossVer " << endl;
  else	log << MSG::INFO << "MdcTuningSvc::getMdcTuningTableInfo() : ParBossVer = " << m_ParBossVer << endl;

  run = fabs(run);
  ////unsigned long *lengths;
  ////  MYSQL_RES *res_set;
  ////  MYSQL_ROW row;
  int j=0;
  int cnt=0;
  DatabaseRecordVector result;
  for(int i=0;i<1000;i++){
    char stmt1[200];
    cnt = i;
    int run1 = run+i;
    cout << " ==================== " << endl;
    //sprintf(stmt1,"select MdcRes,MdcEff,dEdxTuning from MdcTuning where RunFrom <= %d and RunTo >= %d ",run1,run1); 
    sprintf(stmt1,"select MdcRes,MdcEff from MdcTuning where RunFrom <= %d and RunTo >= %d and SftVer = \"%s\"",run1,run1,m_ParBossVer.c_str()); 
    cout << " stmt1: " << stmt1 << endl;

    result.clear();
    int status = m_dbsvc->query("offlinedb",stmt1,result);
		if(status<0){
      cout << " ERROR: can not read MdcRes, MdcEff from the MdcTuning table " << endl;	 
      log << MSG::ERROR << " ERROR Read MdcRes, MdcEff from the MdcTuning table " << endreq;	 
      return StatusCode::FAILURE;
    }
		
		if(result.size()>=1){
      
      //// int aaa = -1;
      ////aaa = mysql_real_query(conn, stmt1, strlen(stmt1));
    
      ////std::cout<<" mysql_real_query: "<<aaa<<std::endl;
      ////    if(aaa){
      //	   if(mysql_real_query(conn, stmt1, strlen(stmt1))) 
      ////fprintf(stderr, "query error\n");
      ////return StatusCode::FAILURE;
      ////}
      ////res_set = mysql_store_result (conn);
      ////row = mysql_fetch_row(res_set);
      ////int row_no = mysql_num_rows(res_set);
      ////std::cout<<"row_no="<<row_no<<std::endl;
      ////if (row_no) {}
      j=1;
      break;  
    }
    
    int run2 = run-i;
    char stmt2[200];
    //sprintf(stmt2,"select MdcRes,MdcEff.dEdxTuning from MdcTuning where RunFrom <= %d and RunTo >= %d ",run2,run2);
    sprintf(stmt2,"select MdcRes,MdcEff from MdcTuning where RunFrom <= %d and RunTo >= %d and SftVer = \"%s\"",run2,run2,m_ParBossVer.c_str());
    
    result.clear();
    status = m_dbsvc->query("offlinedb",stmt2,result);
    if(status<0){
      log << MSG::ERROR << " ERROR Read MdcRes, MdcEff.dEdxTuning from the MdcTuning table " << endreq;	 
      return StatusCode::FAILURE;
    }
    
    
    if(result.size()>=1){   
      ////  mysql_real_query(conn, stmt2, strlen(stmt2));
      ////res_set = mysql_store_result (conn);
      ////row = mysql_fetch_row(res_set);
      ////row_no = mysql_num_rows(res_set);
      ////std::cout<<"row_no="<<row_no<<std::endl;
      ////if (row_no) {}
      j=-1; 
      break;
    }
  }
	
	if(cnt!=0&&cnt!=1000) {
    log << MSG::INFO << " get MDC tuning data from run " << run + cnt*j << " instead of run " << run<< endreq;
    ////    std::cout<<"get MDC tuning data form run " <<run+cnt*j<<" instread of run"<< run<<std::endl;
  } 
  cout << " cnt = " << cnt << endl;
  if(cnt==1000){
    log << MSG::ERROR << " can not read Data from DB" << endreq;
    ////   mysql_close(conn);
    return StatusCode::FAILURE;
  }  

  ////  mysql_field_seek (res_set, 0);

  //// lengths = mysql_fetch_lengths(res_set);
  //  string fff = std::string(row[0]);
  //   m_BesMdcRes->setMdcRes(fff);
  ////string ggg = std::string(row[1]);
  ////string fff = std::string(row[0]);

  //get last row
  int row = result.size()-1;
  cout << " row = " << row << endl;

	if(row<0){
		cout << " ERROR: can not read Data from DB, please check MdcTunningSvc Version. " << endl;
		return StatusCode::FAILURE; 
	}
	
	string ggg = result[row]->GetString("MdcEff");
  string fff = result[row]->GetString("MdcRes");

  log << MSG::DEBUG << " MdcTunning Data: MdcEff: " << ggg << " MdcRes: " << fff << endreq;
  //cout << " MdcTunning Data: MdcEff: " << ggg << " MdcRes: " << fff << endl;

  bool stRes = setMcRes3(fff);
  //bool stRes = setMcRes2(fff);
  bool stEff = setMcEff(ggg);
 
  if(!stEff) return StatusCode::FAILURE;
  if(!stRes) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}
