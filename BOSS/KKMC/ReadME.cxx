//======= Lianjin WU <wulj@ihep.ac.cn> ======
#include "KKMC/ReadME.h"
int ReadME::preRunNo=-1;
void ReadME::readMeasuredEcms(int runNo){
  m_sampleName=-1; m_ecms=-1; m_ecmsErr=-1; m_aveEcms=-1; m_aveEcmsErr=-1; m_runNoValid=false;
  if(runNo==0){
    std::cout<<"RunNo can not be 0"<<std::endl;
  }
  else{
    int runNum=std::abs(runNo);
    StatusCode scDbSvc=Gaudi::svcLocator()->service("DatabaseSvc", m_dbSvc, true);
    if(scDbSvc.isFailure()){
      std::cout<<"Gaudi::svcLocator()->service is Failed"<<std::endl;
    }
    else{
      char stmt[400];
      snprintf(stmt, 1024, "select sample, RunFrom, RunTo, Ecms, Ecms_err, Ave_Ecms, Ave_Ecms_err from MeasuredEcms2 where RunFrom <= %d and RunTo >= %d", runNum, runNum);
      DatabaseRecordVector res;
      int rowNo=m_dbSvc->query("offlinedb", stmt, res);
      if(rowNo==0){
        std::cout<<"Can not find this RowNo"<<std::endl;
      }
      else{
	DatabaseRecord* records=res[0];
      	m_runFrom=records->GetDouble("RunFrom");
      	m_runTo=records->GetDouble("RunTo");
      	m_sampleName=records->GetDouble("sample");
      	m_ecms=records->GetDouble("Ecms");
      	m_ecmsErr=records->GetDouble("Ecms_err");
      	m_aveEcms=records->GetDouble("Ave_Ecms");
      	m_aveEcmsErr=records->GetDouble("Ave_Ecms_err");
      	if(m_ecms!=-1) m_runNoValid=true;
      }
    }
  }
}
