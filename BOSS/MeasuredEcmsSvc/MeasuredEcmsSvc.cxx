//<wulj@ihep.ac.cn> 2015.11.28 Modified
#include "MeasuredEcmsSvc/MeasuredEcmsSvc.h"
using namespace std;
MeasuredEcmsSvc::MeasuredEcmsSvc(const std::string& name, ISvcLocator* svcLoc):Service(name, svcLoc){
	declareProperty("RunParamsMin", RunParamsMin=11414);
	declareProperty("RunParamsMax", RunParamsMax=23454);

	declareProperty("MeasuredEcmsMin", MeasuredEcmsMin=23463);
	declareProperty("MeasuredEcmsMax", MeasuredEcmsMax=38140);

	declareProperty("TableName", table="RunParams");
	declareProperty("ColumnName", column="beam_energy");
	declareProperty("DownRangeColumnName", down_column="run_number");
	declareProperty("UpRangeColumnName", up_column="run_number");
}

MeasuredEcmsSvc::~MeasuredEcmsSvc(){}

StatusCode MeasuredEcmsSvc::initialize(){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc initialize begin here"<<endreq;

  StatusCode status=Service::initialize();
  if(!status.isSuccess()){
    log<<MSG::ERROR<<"MeasuredEcmsSvc Can not initialize"<<endreq;
    return status;
  }

  IIncidentSvc* incsvc;
  status=service("IncidentSvc", incsvc);
  int priority = 100;
  if(status.isSuccess()) incsvc->addListener(this, "NewRun", priority);

  status=serviceLocator()->service("DatabaseSvc", m_dbSvc, true);
  if(!status.isSuccess()){
    log<<MSG::ERROR<<"MeasuredEcmsSvc Unable to find DatabaseSvc"<<endreq;
    return status;
  }

  status=serviceLocator()->service("EventDataSvc", m_eventSvc, true);
  if(!status.isSuccess()){
    log<<MSG::ERROR<<"MeasuredEcmsSvc Unable to find EventDataSvc "<<endreq;
    return status;
  }

  log<<MSG::INFO<<"MeasuredEcmsSvc initialized successfully"<<endreq;
  return StatusCode::SUCCESS;
}

StatusCode MeasuredEcmsSvc::finalize(){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc finalize begin here"<<endreq;

  StatusCode status= Service::finalize();
  if(!status.isSuccess()){
    log<<MSG::ERROR<<"MeasuredEcmsSvc Can not finalize"<<endreq;
    return status;
  }

  log<<MSG::INFO<<"MeasuredEcmsSvc finalized successfully"<<endreq;
  return StatusCode::SUCCESS;
}

StatusCode MeasuredEcmsSvc::queryInterface(const InterfaceID& riid, void** ppvIF){
  if(IMeasuredEcmsSvc::interfaceID().versionMatch(riid)) *ppvIF = dynamic_cast<IMeasuredEcmsSvc*>(this);
  else return Service::queryInterface(riid, ppvIF);

  addRef();

  return StatusCode::SUCCESS;
}

void MeasuredEcmsSvc::handle(const Incident& inc){
  MsgStream log(messageService(), name());
  log<<MSG::DEBUG<<"handle: "<<inc.type()<<endreq;

	SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc, "/Event/EventHeader");
	int runNo=eventHeader->runNumber();
  if(inc.type()=="NewRun"){
		log<<MSG::INFO<<"RunNo is new, call the readMeasuredEcms again"<<endreq;
		if(std::abs(runNo)>=RunParamsMin&&std::abs(runNo)<=RunParamsMax) readRunParams2(runNo);
		if(std::abs(runNo)>=MeasuredEcmsMin&&std::abs(runNo)<=MeasuredEcmsMax) readMeasuredEcms2(runNo);
  }
}

//================================
//================================
void MeasuredEcmsSvc::readMeasuredEcms(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc begin read database MeasuredEcmsSvc"<<endreq;

  int runNum=std::abs(runNo);

  if(runNo<0) log<<MSG::INFO<<"MeasuredEcmsSvc for MC events"<<endreq;
  else log<<MSG::INFO<<"MeasuredEcmsSvc for DT events"<<endreq;

  char stmt[400];
  snprintf(stmt, 1024, "select sample, RunFrom, RunTo, Ecms, Ecms_err, Ave_Ecms, Ave_Ecms_err from MeasuredEcms2 where RunFrom <= %d and RunTo >= %d", runNum, runNum);
  DatabaseRecordVector res;
  int rowNo=m_dbSvc->query("offlinedb", stmt, res);
  if(rowNo==0){ log<<MSG::ERROR<<"RowNo can not be 0 for MeasuredEcmsSvc, failed to read DB for "<<runNo<<endreq; }
  else{
    DatabaseRecord* records=res[0];
    log<<MSG::INFO<<"MeasuredEcmsSvc first read the runNo, read the MeasuredEcms database "<<runNo<<endreq;
    m_runFrom=records->GetDouble("RunFrom");
    m_runTo=records->GetDouble("RunTo");
    m_sampleName=records->GetDouble("sample");
    m_ecms=records->GetDouble("Ecms");
    m_ecmsErr=records->GetDouble("Ecms_err");
    m_aveEcms=records->GetDouble("Ave_Ecms");
    m_aveEcmsErr=records->GetDouble("Ave_Ecms_err");
    m_runNoValid=true;
  }
}

void MeasuredEcmsSvc::readRunParams(int runNo){
	MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc begin read database RunParams"<<endreq;

	int runNum=std::abs(runNo);

  if(runNo<0) log<<MSG::INFO<<"MeasuredEcmsSvc for MC events"<<endreq;
  else log<<MSG::INFO<<"MeasuredEcmsSvc for DT events"<<endreq;

	char stmt1[400];
  snprintf(stmt1, 1024, "select beam_energy, px, py, pz from RunParams where run_number = %d", runNum);
  DatabaseRecordVector res;
  int row_no = m_dbSvc->query("offlinedb", stmt1, res);
  if(row_no==0){log<<MSG::ERROR<<"RowNo can not be 0 for RunParams, failed to read DB for "<<runNo<<endreq;}
  else{
		DatabaseRecord* records = res[0];
		log<<MSG::INFO<<"MeasuredEcmsSvc first read the runNo, read the  RunParams database"<<runNo<<endreq;
    double bE=0;
    bE=records->GetDouble("beam_energy");
    m_px=records->GetDouble("px");
    m_py=records->GetDouble("py");
    m_pz=records->GetDouble("pz");
    m_ecms=bE*2.00;
		m_runNoValid=true;
	}
}

void MeasuredEcmsSvc::readDB(int runNo){
	if(std::abs(runNo)>=RunParamsMin&&std::abs(runNo)<=RunParamsMax) readRunParams(runNo);
	if(std::abs(runNo)>=MeasuredEcmsMin&&std::abs(runNo)<=MeasuredEcmsMax) readMeasuredEcms(runNo);
}

int MeasuredEcmsSvc::getSampleName(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the sample name"<<endreq;
	if(old_runNo!=runNo){
    readDB(runNo);
		old_runNo=runNo;
  }
  return m_sampleName;
}

int MeasuredEcmsSvc::getRunFrom(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the RunFrom"<<endreq;
	if(old_runNo!=runNo){
    readDB(runNo);
		old_runNo=runNo;
  }
  return m_runFrom;
}

int MeasuredEcmsSvc::getRunTo(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the RunTo"<<endreq;
	if(old_runNo!=runNo){
    readDB(runNo);
    old_runNo=runNo;
  }
  return m_runTo;
}

double MeasuredEcmsSvc::getEcmsErr(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the error of ecms"<<endreq;
	if(old_runNo!=runNo){
    readDB(runNo);
    old_runNo=runNo;
  }
  return m_ecmsErr;
}

double MeasuredEcmsSvc::getAveEcms(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the average ecms"<<endreq;
	if(old_runNo!=runNo){
    readDB(runNo);
    old_runNo=runNo;
  }
  return m_aveEcms;
}

double MeasuredEcmsSvc::getAveEcmsErr(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the error of the average ecms"<<endreq;
  if(old_runNo!=runNo){
		readDB(runNo);
		old_runNo=runNo;
	}
  return m_aveEcmsErr;
}

double MeasuredEcmsSvc::getEcms(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the ecms"<<endreq;
	if(old_runNo!=runNo){
    readDB(runNo);
		old_runNo=runNo;
  }
  return m_ecms;
}

double MeasuredEcmsSvc::getPx(int runNo){
	MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the ecms"<<endreq;
	if(old_runNo!=runNo){
    readDB(runNo);
    old_runNo=runNo;
  }
  return m_px;
}

double MeasuredEcmsSvc::getPy(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the ecms"<<endreq;
  if(old_runNo!=runNo){
    readDB(runNo);
    old_runNo=runNo;
  }
  return m_py;
}

double MeasuredEcmsSvc::getPz(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the ecms"<<endreq;
  if(old_runNo!=runNo){
    readDB(runNo);
    old_runNo=runNo;
  }
  return m_pz;
}

bool MeasuredEcmsSvc::isRunNoValid(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc judge the valid of runNo"<<endreq;
  if(old_runNo!=runNo){
		readDB(runNo);
		old_runNo=runNo;
	}
  return m_runNoValid;
}

bool MeasuredEcmsSvc::isGetEcmsValid(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc judge the valid of runNo"<<endreq;
  if(old_runNo!=runNo){
    readDB(runNo);
    old_runNo=runNo;
  }
  return m_runNoValid;
}

//=====================================
//=====================================
void MeasuredEcmsSvc::readInfo(int runNo){
	char tab[100];
	char col[100];
	char min_col[100];
	char max_col[100];

	strcpy(tab, table.c_str());
	strcpy(col, column.c_str());
	strcpy(min_col, down_column.c_str());
	strcpy(max_col, up_column.c_str());

  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc begin read database "<<tab<<"-"<<col<<endreq;

  int runNum=std::abs(runNo);

  if(runNo<0) log<<MSG::INFO<<"MeasuredEcmsSvc for MC events"<<endreq;
  else log<<MSG::INFO<<"MeasuredEcmsSvc for DT events"<<endreq;

  char stmt1[500];
  snprintf(stmt1, 1024, "select %s from %s where %s <= %d and %s >= %d", col, tab, min_col, runNum, max_col, runNum);
  DatabaseRecordVector res;
  int row_no = m_dbSvc->query("offlinedb", stmt1, res);
  if(row_no==0){log<<MSG::ERROR<<"RowNo can not be 0 for "<<tab<<", failed to read DB for "<<runNo<<endreq;}
  else{
    DatabaseRecord* records = res[0];
    log<<MSG::INFO<<"MeasuredEcmsSvc first read the runNo, read the "<<tab<<" database "<<runNo<<endreq;
    m_info1=records->GetDouble(col);
    m_runNoValid1=true;
  }
}

double MeasuredEcmsSvc::getInfo(int runNo){
	char tab[100];
  char col[100];
  strcpy(tab, table.c_str());
  strcpy(col, column.c_str());
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the "<<tab<<"-"<<col<<endreq;
  if(old_runNo1!=runNo){
    readInfo(runNo);
		old_runNo1=runNo;
  }
  return m_info1;
}

bool MeasuredEcmsSvc::isGetInfoValid(int runNo){
	char tab[100];
  char col[100];
  strcpy(tab, table.c_str());
  strcpy(col, column.c_str());
	MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc judge the valid of "<<tab<<"-"<<col<<endreq;
  if(old_runNo1!=runNo){
    readInfo(runNo);
    old_runNo1=runNo;
  }
  return m_runNoValid1;
}

//==========================================
//==========================================
void MeasuredEcmsSvc::readInfo(int runNo, char *tab, char *col, char *min_col, char *max_col){
	MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc begin read database "<<tab<<"-"<<col<<endreq;

	int runNum=std::abs(runNo);

	if(runNo<0) log<<MSG::INFO<<"MeasuredEcmsSvc for MC events"<<endreq;
  else log<<MSG::INFO<<"MeasuredEcmsSvc for DT events"<<endreq;

	char stmt1[400];
  snprintf(stmt1, 1024, "select %s from %s where %s <= %d and %s >=%d", col, tab, min_col, runNum, max_col, runNum);
  DatabaseRecordVector res;
	int row_no = m_dbSvc->query("offlinedb", stmt1, res);
  if(row_no==0){log<<MSG::ERROR<<"RowNo can not be 0 for "<<tab<<", failed to read DB for "<<runNo<<endreq;}
	else{
    DatabaseRecord* records = res[0];
    log<<MSG::INFO<<"MeasuredEcmsSvc first read the runNo, read the "<<tab<<" database "<<runNo<<endreq;
    m_info2=records->GetDouble(col);
		m_runNoValid2=true;
  }
}

double MeasuredEcmsSvc::getInfo(int runNo, char *tab, char *col, char *min_col, char *max_col){
	MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the "<<tab<<"-"<<col<<endreq;
	if(old_runNo2!=runNo){
		readInfo(runNo, tab, col, min_col, max_col);
		old_runNo2=runNo;
	}
	return m_info2;
}

bool MeasuredEcmsSvc::isGetInfoValid(int runNo, char *tab, char *col, char *min_col, char *max_col){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc judge the valid of "<<tab<<"-"<<col<<endreq;
  if(old_runNo2!=runNo){
    readInfo(runNo, tab, col, min_col, max_col);
    old_runNo2=runNo;
  }
  return m_runNoValid2;
}

//=====================================
//=====================================
void MeasuredEcmsSvc::readRecord(int runNo){
	char tab[100];
	char col[100];
	char min_col[100];
	char max_col[100];

	strcpy(tab, table.c_str());
	strcpy(col, column.c_str());
	strcpy(min_col, down_column.c_str());
	strcpy(max_col, up_column.c_str());

  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc begin read database "<<tab<<"-"<<col<<endreq;

  int runNum=std::abs(runNo);

  if(runNo<0) log<<MSG::INFO<<"MeasuredEcmsSvc for MC events"<<endreq;
  else log<<MSG::INFO<<"MeasuredEcmsSvc for DT events"<<endreq;

  char stmt1[500];
  snprintf(stmt1, 1024, "select %s from %s where %s <= %d and %s >= %d", col, tab, min_col, runNum, max_col, runNum);
  DatabaseRecordVector res;
  int row_no = m_dbSvc->query("offlinedb", stmt1, res);
  if(row_no==0){log<<MSG::ERROR<<"RowNo can not be 0 for "<<tab<<", failed to read DB for "<<runNo<<endreq;}
  else{
    DatabaseRecord* records = res[0];
    log<<MSG::INFO<<"MeasuredEcmsSvc first read the runNo, read the "<<tab<<" database "<<runNo<<endreq;
    m_record1=records->GetString(col);
    m_record_valid1=true;
  }
}

string MeasuredEcmsSvc::getRecord(int runNo){
	char tab[100];
  char col[100];
  strcpy(tab, table.c_str());
  strcpy(col, column.c_str());
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the "<<tab<<"-"<<col<<endreq;
  if(old_runNo3!=runNo){
    readRecord(runNo);
		old_runNo3=runNo;
  }
  return m_record1;
}

bool MeasuredEcmsSvc::isGetRecordValid(int runNo){
	char tab[100];
  char col[100];
  strcpy(tab, table.c_str());
  strcpy(col, column.c_str());
	MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc judge the valid of "<<tab<<"-"<<col<<endreq;
  if(old_runNo3!=runNo){
    readRecord(runNo);
    old_runNo3=runNo;
  }
  return m_record_valid1;
}

//==========================================
//==========================================
void MeasuredEcmsSvc::readRecord(int runNo, char *tab, char *col, char *min_col, char *max_col){
	MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc begin read database "<<tab<<"-"<<col<<endreq;

	int runNum=std::abs(runNo);

	if(runNo<0) log<<MSG::INFO<<"MeasuredEcmsSvc for MC events"<<endreq;
  else log<<MSG::INFO<<"MeasuredEcmsSvc for DT events"<<endreq;

	char stmt1[400];
  snprintf(stmt1, 1024, "select %s from %s where %s <= %d and %s >=%d", col, tab, min_col, runNum, max_col, runNum);
  DatabaseRecordVector res;
	int row_no = m_dbSvc->query("offlinedb", stmt1, res);
  if(row_no==0){log<<MSG::ERROR<<"RowNo can not be 0 for "<<tab<<", failed to read DB for "<<runNo<<endreq;}
	else{
    DatabaseRecord* records = res[0];
    log<<MSG::INFO<<"MeasuredEcmsSvc first read the runNo, read the "<<tab<<" database "<<runNo<<endreq;
    m_record2=records->GetString(col);
		m_record_valid2=true;
  }
}

string MeasuredEcmsSvc::getRecord(int runNo, char *tab, char *col, char *min_col, char *max_col){
	MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc read the "<<tab<<"-"<<col<<endreq;
	if(old_runNo4!=runNo){
		readRecord(runNo, tab, col, min_col, max_col);
		old_runNo4=runNo;
	}
	return m_record2;
}

bool MeasuredEcmsSvc::isGetRecordValid(int runNo, char *tab, char *col, char *min_col, char *max_col){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc judge the valid of "<<tab<<"-"<<col<<endreq;
  if(old_runNo4!=runNo){
    readRecord(runNo, tab, col, min_col, max_col);
    old_runNo4=runNo;
  }
  return m_record_valid2;
}

//=============================================
//=============================================
void MeasuredEcmsSvc::readMeasuredEcms2(int runNo){
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc begin read database MeasuredEcmsSvc"<<endreq;

  int runNum=std::abs(runNo);

  if(runNo<0) log<<MSG::INFO<<"MeasuredEcmsSvc for MC events"<<endreq;
  else log<<MSG::INFO<<"MeasuredEcmsSvc for DT events"<<endreq;

  char stmt[400];
  snprintf(stmt, 1024, "select sample, RunFrom, RunTo, Ecms, Ecms_err, Ave_Ecms, Ave_Ecms_err from MeasuredEcms2 where RunFrom <= %d and RunTo >= %d", runNum, runNum);
  DatabaseRecordVector res;
  int rowNo=m_dbSvc->query("offlinedb", stmt, res);
  if(rowNo==0){ log<<MSG::ERROR<<"RowNo can not be 0 for MeasuredEcmsSvc, failed to read DB for "<<runNo<<endreq; }
  else{
    DatabaseRecord* records=res[0];
    log<<MSG::INFO<<"MeasuredEcmsSvc first read the runNo, read the MeasuredEcms database "<<runNo<<endreq;

		m_runFrom2=records->GetDouble("RunFrom");
    m_runTo2=records->GetDouble("RunTo");
    m_sampleName2=records->GetDouble("sample");
    m_ecms2=records->GetDouble("Ecms");
    m_ecmsErr2=records->GetDouble("Ecms_err");
    m_aveEcms2=records->GetDouble("Ave_Ecms");
    m_aveEcmsErr2=records->GetDouble("Ave_Ecms_err");
    m2_runNoValid=true;
  }
}

void MeasuredEcmsSvc::readRunParams2(int runNo){
	MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"MeasuredEcmsSvc begin read database RunParams"<<endreq;

	int runNum=std::abs(runNo);

  if(runNo<0) log<<MSG::INFO<<"MeasuredEcmsSvc for MC events"<<endreq;
  else log<<MSG::INFO<<"MeasuredEcmsSvc for DT events"<<endreq;

	char stmt1[400];
  snprintf(stmt1, 1024, "select beam_energy, px, py, pz from RunParams where run_number = %d", runNum);
  DatabaseRecordVector res;
  int row_no = m_dbSvc->query("offlinedb", stmt1, res);
  if(row_no==0){log<<MSG::ERROR<<"RowNo can not be 0 for RunParams, failed to read DB for "<<runNo<<endreq;}
  else{
		DatabaseRecord* records = res[0];
		log<<MSG::INFO<<"MeasuredEcmsSvc first read the runNo, read the  RunParams database"<<runNo<<endreq;
    double bE=0;
    bE=records->GetDouble("beam_energy");

		m_px2=records->GetDouble("px");
    m_py2=records->GetDouble("py");
    m_pz2=records->GetDouble("pz");
    m_ecms2=bE*2.00;
    m2_runNoValid=true;
	}
}




