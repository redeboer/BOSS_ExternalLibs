// $Header: /bes/bes/BossCvs/Calibration/calibUtil/src/Metadata.cxx,v 1.42 2016/05/27 02:56:18 sunss Exp $

/*
#ifdef  WIN32
#include <windows.h>
#endif
*/

#include "calibUtil/Metadata.h"
#include "facilities/Util.h"
#include "facilities/Timestamp.h"
#include "rdbModel/Management/Manager.h"
#include "rdbModel/Management/XercesBuilder.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"



#include "rdbModel/Db/MysqlConnection.h"
#include "rdbModel/Db/MysqlResults.h"
#include "rdbModel/Rdb.h"
#include "rdbModel/RdbException.h"
#include "rdbModel/Tables/Assertion.h"
#include "rdbModel/Tables/Table.h"
#include "rdbModel/Tables/Column.h"
#include <iostream>
#include <cstdio>
#include <mysql.h>
#include "TTree.h"
#include "TBuffer.h"


namespace calibUtil {

	// Might be getting rid of this
	//  const unsigned int Metadata::s_rowReady = Metadata::eOpened 
	//  | Metadata::eValid  | Metadata::eInputDesc | Metadata::eComment;

	Metadata::Metadata(const std::string& host, const std::string& table,
			const std::string& dbName)  
		: m_readCxt(0), m_writeCxt(0), //  m_row(""), m_rowStatus(0), 
		m_host(host), m_table(table), m_dbName(dbName), m_man(0), m_rdb(0),
		m_match(false) {
			if (table.compare("*") == 0) m_table = std::string("$(MYSQL_METATABLE)");
			if (host.compare("*") == 0) m_host = std::string("$(MYSQL_HOST)");

			int nsub = facilities::Util::expandEnvVar(&m_table);
			// If this doesn't work, use default
			if (nsub < 0) m_table = std::string("metadata");
			StatusCode sc=Gaudi::svcLocator()->service("DatabaseSvc", m_dbsvc, true);
			if ( !sc.isSuccess() ) {
				std::cout <<"Metadata ERROR ::Could not retrieve the DataBaseSvc" << std::endl;;
				exit(1);
			}
		}

	Metadata::~Metadata() {
		disconnectRead();
		disconnectWrite();
		if (m_man) delete m_man;
	}


	Metadata::eRet Metadata::fetchUser(std::string& user) {
		// WARNING: Windows and Linux/Solaris use different standard variables
		// for login name.  The test used below isn't quite right
		// since one could conceivably compile with gcc on cygwin/Windows.
#ifdef    __GNUG__ 
		user = std::string("$(USER)");
#else
		user = std::string("$(USERNAME)");
#endif

		int nsub = facilities::Util::expandEnvVar(&user);
		if (nsub == 1) {
			return RETOk;
		}
		else {
			user = std::string("");
			return RETBadValue;
		}
	}

	// The next 5 methods concern connection to the server
	bool Metadata::connect(rdbModel::Connection* cxt, std::string& host,
			const std::string& user, 
			const std::string& pw, eRet& err,
			const std::string& dbName)  {

		int nSub = facilities::Util::expandEnvVar(&host);
		if (nSub < 0) {
			err = RETBadHost;
			return false;
		}

		bool connected = cxt->open(host, user, pw, dbName);
		if (connected) {
			err = RETOk;
			return true;
		}
		else {
			err = RETNoConnect;
			return false;
		}
	}

	bool Metadata::connectRead(eRet& err) {
		if (m_readCxt == 0) {
			// for now use std::cout, std::cerr 
			m_readCxt = new rdbModel::MysqlConnection();
			//   bool ok = connect(m_readCxt, m_host, std::string("maqm"), 
			//                      std::string("maqm_offline"),  err, m_dbName);
			bool ok = connect(m_readCxt, m_host, std::string("guest"),
					std::string("guestpass"),  err, m_dbName);
			if (!ok) {
				delete m_readCxt;
				m_readCxt = 0;
			} /* else { // look for compatible schema
					 std::string schema = 
					 std::string("$(RDBMODELROOT)/xml/")+ m_dbName + ".xml"; 
					 err = compareSchema(m_readCxt, schema);
					 }*/
			return ok;
		}

		else return true;
	}

	bool Metadata::connectWrite(eRet& err) {
		if (m_writeCxt == 0) {
			m_writeCxt = new rdbModel::MysqlConnection();
			bool ok;
			if (m_dbName == std::string("calib_test") ) {
				ok = connect(m_writeCxt, m_host, std::string("calib_tester"), 
						std::string("udine"), err, m_dbName);
			}
			else { 
				ok = connect(m_writeCxt, m_host, std::string("calibrator"), 
						std::string("calibrator"), err, m_dbName);
			}
			if (!ok) {
				delete m_readCxt;
				m_readCxt = 0;
			}     else { // look for compatible schema
				std::string schema = 
					std::string("$(RDBMODELROOT)/xml/")+ m_dbName + ".xml"; 
				err = compareSchema(m_writeCxt, schema);
			}

			return ok;
		}
		else return true;
	}

	void Metadata::disconnectRead() {
		if (m_readCxt) {
			m_readCxt->close();
			delete m_readCxt;
			m_readCxt = 0;
		}
	}

	void Metadata::disconnectWrite() {
		if (m_writeCxt) {
			m_writeCxt->close();
			delete m_writeCxt;
			m_writeCxt = 0;
		}
	}

	Metadata::eRet 
		Metadata::findSoonAfter(unsigned int *ser,
				const std::string& calibType,
				const std::string& SftVer,
				const std::string& cal_ver, 
				const std::string& cal_par,
				const std::string& rec_alg,
				const std::string& rec_par,
				const std::string& machine,
				const std::string& flavor) {
			using namespace rdbModel;

			eRet ret;
			*ser = 0;
			if (!m_readCxt) {
				connectRead(ret);
				if (ret != RETOk) return ret;
			}

			StringVector orderBy;

			std::vector<Assertion::Operator *> conditions;
			conditions.reserve(8);

			Assertion::Operator completeOp(OPTYPEequal, "status", "OK",
					FIELDTYPEold, FIELDTYPElit);
			//                               false, true);
			Assertion::Operator calibTypeOp(OPTYPEequal, "calib_type", calibType,
					FIELDTYPEold, FIELDTYPElit);
			//                                false, true);
			// Assertion::Operator flavorOp(OPTYPEequal, "flavor", flavor, 
			//                              FIELDTYPEold, FIELDTYPElit);
			//false, true);
			Assertion::Operator sftverOp(OPTYPEequal, "SftVer",SftVer,
					FIELDTYPEold, FIELDTYPElit);
			Assertion::Operator calverOp(OPTYPEequal, "cal_ver",cal_ver,
					FIELDTYPEold, FIELDTYPElit);
			Assertion::Operator calparOp(OPTYPEequal, "cal_par",cal_par,
					FIELDTYPEold, FIELDTYPElit);
			Assertion::Operator recalgOp(OPTYPEequal, "rec_alg",rec_alg,
					FIELDTYPEold, FIELDTYPElit);
			Assertion::Operator recparOp(OPTYPEequal, "rec_par",rec_par,
					FIELDTYPEold, FIELDTYPElit);
			Assertion::Operator machineOp(OPTYPEequal, "machine",machine,
					FIELDTYPEold, FIELDTYPElit);

			int run=10005;
			std::string s_run;
			std::cout<<"run no is::"<<s_run<<std::endl;
			facilities::Util::itoa(run,s_run);
			Assertion::Operator runfrmOp(OPTYPElessOrEqual,"RunFrom",s_run,
					FIELDTYPEold, FIELDTYPElit);

			Assertion::Operator runtoOp(OPTYPEgreaterOrEqual,"RunTo",s_run,
					FIELDTYPEold, FIELDTYPElit);

			conditions.push_back(&completeOp);
			conditions.push_back(&calibTypeOp);
			//  conditions.push_back(&flavorOp);
			conditions.push_back(&sftverOp);
			conditions.push_back(&calverOp);
			conditions.push_back(&calparOp);
			conditions.push_back(&recalgOp);
			conditions.push_back(&recparOp);
			conditions.push_back(&machineOp);
			conditions.push_back(&runfrmOp);
			conditions.push_back(&runtoOp);
			ret = doSelect(conditions, orderBy, ser);

			return ret;
		}

	Metadata::eRet Metadata::findBest(unsigned int *ser,
			const std::string& calibType,
			const std::string& SftVer,
			const std::string& cal_ver,
			const std::string& cal_par,
			const std::string& rec_alg,
			const std::string& rec_par,
			const std::string& machine,
			const std::string& flavor)
	{
		using namespace rdbModel;

		eRet ret;
		*ser = 0;
		if (!m_readCxt) {
			connectRead(ret);
			if (ret != RETOk) return ret;
		}

		// Sort rows by timestamp. Would like most recent first
		StringVector orderBy;

		std::vector<Assertion::Operator *> conditions;
		conditions.reserve(8);
		Assertion::Operator completeOp(OPTYPEequal, "status", "OK",
				FIELDTYPEold, FIELDTYPElit);
		//                               false, true);

		Assertion::Operator calibTypeOp(OPTYPEequal, "calib_type", calibType,
				FIELDTYPEold, FIELDTYPElit);
		//                                false, true);
		//    Assertion::Operator flavorOp(OPTYPEequal, "flavor", flavor,
		//                                 FIELDTYPEold, FIELDTYPElit);
		//  false, true);
		Assertion::Operator sftverOp(OPTYPEequal, "SftVer",SftVer,
				FIELDTYPEold, FIELDTYPElit);
		Assertion::Operator calverOp(OPTYPEequal, "cal_ver",cal_ver,
				FIELDTYPEold, FIELDTYPElit);
		Assertion::Operator calparOp(OPTYPEequal, "cal_par",cal_par,
				FIELDTYPEold, FIELDTYPElit);
		Assertion::Operator recalgOp(OPTYPEequal, "rec_alg",rec_alg,
				FIELDTYPEold, FIELDTYPElit);
		Assertion::Operator recparOp(OPTYPEequal, "rec_par",rec_par,
				FIELDTYPEold, FIELDTYPElit);
		Assertion::Operator machineOp(OPTYPEequal, "machine",machine,
				FIELDTYPEold, FIELDTYPElit);
		int run=10005;
		std::string s_run;
		facilities::Util::itoa(run,s_run);     
		Assertion::Operator runfrmOp(OPTYPElessOrEqual,"RunFrom",s_run,
				FIELDTYPEold, FIELDTYPElit);

		Assertion::Operator runtoOp(OPTYPEgreaterOrEqual,"RunTo",s_run,
				FIELDTYPEold, FIELDTYPElit);

		conditions.push_back(&completeOp);
		conditions.push_back(&calibTypeOp);
		//    conditions.push_back(&flavorOp);
		conditions.push_back(&sftverOp);
		conditions.push_back(&calverOp);
		conditions.push_back(&calparOp);
		conditions.push_back(&recalgOp);
		conditions.push_back(&recparOp);
		conditions.push_back(&machineOp);
		conditions.push_back(&runfrmOp);
		conditions.push_back(&runtoOp);
		// Finally, set a value for PROC_LEVEL and make the query (ies)
		return doSelect(conditions, orderBy, ser);
	}


	Metadata::eRet Metadata::getReadInfo(unsigned int serialNo, 
			int *runFrm,
			int *runTo,
			std::string& dataFmt, 
			std::string& filename) {
		using namespace rdbModel;
		eRet ret;
		if (!m_readCxt) {
			connectRead(ret);
			if (ret != RETOk) {
				return ret;
			}
		}
		StringVector colNames(4);
		colNames[0] = "data_fmt";
		colNames[1] = "data_ident";
		colNames[2] = "RunFrom";
		colNames[3] = "RunTo";

		std::string serNoVal;
		facilities::Util::itoa(serialNo, serNoVal);
		Assertion::Operator* serOp = 
			new Assertion::Operator(OPTYPEequal, "ser_no", serNoVal, 
					FIELDTYPEold, FIELDTYPElit);
		//false, true);
		Assertion whereClause(serOp);
		StringVector orderBy;
		orderBy.clear();

		// make the query
		ResultHandle* results = 0;
		try {

			results = m_readCxt->select(m_table, colNames, orderBy, &whereClause);

		}
		catch (RdbException ex) {
			std::cout << ex.getMsg();
			return RETMySQLError;
		}

		if (!results) return RETMySQLError;
		if (!results->getNRows() ) return RETBadValue;

		std::vector<std::string> fields;
		std::cout<<"test id \\"<<std::endl;
		results->getRow(fields);
		dataFmt = fields[0];
		filename = fields[1];
		std::string runfrm,runto;
		runfrm=fields[2];
		runto=fields[3];
		*runFrm=facilities::Util::atoi(runfrm);
		*runTo=facilities::Util::atoi(runto);
		delete results;
		//runFrm=fields[2];
		//runTo=fields[3];
		// std::cout<<"runfrm is:"<<runFrm<<"runto is:"<<runTo<<std::endl;
		if ((dataFmt == "") || (filename == "")) return RETBadValue;
		return RETOk;
	}

	Metadata::eRet Metadata::getReadTOFInfo(unsigned int serialNo, 
			int *runFrm,
			int *runTo,
			std::string& calParVer,
			DatabaseRecordVector& res,
			int runNo,
			std::string& sftver) {
		using namespace rdbModel;
		eRet ret;
		char stmt1[300];
		int run_No =runNo;
		if(sftver=="default")
			sftver = getenv("BES_RELEASE");
		const char* SftVer = sftver.c_str();

		if(calParVer!="default"){
			const char* calpar = calParVer.c_str();
			sprintf(stmt1,"select BarTofPar,EndTofPar,TofConPar,BarTof,EtfTofPar,EtfTofBunch,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from TofCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and CalParVer = %s",SftVer,run_No,run_No,calpar);
		}
		if(calParVer=="default"){
			sprintf(stmt1,"select BarTofPar,EndTofPar,TofConPar,BarTof,EtfTofPar,EtfTofBunch,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from TofCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d order by CalParVer desc",SftVer,run_No,run_No);
		}

		int row_no = m_dbsvc->query("offlinedb", stmt1, res);
		if(row_no<1){
			std::cout<<"ERROR:error searching TOF calibration Data in the database with: "<<stmt1<<std::endl;
			return RETMySQLError;
		}

		return RETOk;
	}

	Metadata::eRet Metadata::getReadMUCInfo(unsigned int serialNo, 
			int *runFrm,
			int *runTo,
			std::string& calParVer,
			DatabaseRecordVector& res,
			int runNo,
			std::string& sftver) {
		using namespace rdbModel;
		eRet ret;
		char stmt1[300];
		int run_No =runNo;
		//char* SftVer = getenv("BES_RELEASE");
		if(sftver=="default")
			sftver = getenv("BES_RELEASE");
		const char* SftVer = sftver.c_str();

		if(calParVer!="default"){
			const char* calpar = calParVer.c_str();
			sprintf(stmt1,"select LayTree,BoxTree,StrTree,ResTree,ClsTree,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from MucCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d  order by CalParVer desc",SftVer,run_No,run_No);	 
		}

		if(calParVer=="default"){
			//  sprintf(stmt1,"select SftVer,LayTree,BoxTree,StrTree,ResTree,ClsTree,RunFrom,RunTo,max(CalParVer),FileName from MucCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d group by CalParVer",SftVer,run_No,run_No);
			sprintf(stmt1,"select LayTree,BoxTree,StrTree,ResTree,ClsTree,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from MucCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d  order by CalParVer desc",SftVer,run_No,run_No);

		}

		int row_no = m_dbsvc->query("offlinedb", stmt1, res);
		if(row_no<1){
			std::cout<<"ERROR:error searching MUC calibration Data in the database with: "<<stmt1<<std::endl;
			return RETMySQLError;
		}


		return RETOk;
	}

	Metadata::eRet Metadata::getReadMDCInfo(unsigned int serialNo, 
			int *runFrm,
			int *runTo,
			std::string& calParVer,
			DatabaseRecordVector& res,
			int runNo,
			std::string& sftver) {
		using namespace rdbModel;
		eRet ret;
		char stmt1[200];
		int run_No =runNo;

		if(sftver=="default")
			sftver = getenv("BES_RELEASE");
		const char* SftVer = sftver.c_str();


		if(calParVer!="default"){
			const char* calpar = calParVer.c_str();
			sprintf(stmt1,"select NewXtTrees,XtTree,QtTree,T0Tree,SdTree,R2tTrees,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from MdcCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and CalParVer = '%s'",SftVer,run_No,run_No,calpar);}

			if(calParVer=="default"){
				//sprintf(stmt1,"select NewXtTrees,XtTree,QtTree,T0Tree,SdTree,R2tTrees,RunFrom,RunTo,max(CalParVer),FileName from MdcCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d group by CalParVer",SftVer,run_No,run_No);}
				sprintf(stmt1,"select NewXtTrees,XtTree,QtTree,T0Tree,SdTree,R2tTrees,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from MdcCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d order by CalParVer desc",SftVer,run_No,run_No);}

			int row_no = m_dbsvc->query("offlinedb", stmt1, res);
			if(row_no<1){
				std::cout<<"ERROR:error searching Mdc calibration Data in the database with: "<<stmt1<<std::endl;
				return RETMySQLError;
			}


			return RETOk;
}



Metadata::eRet Metadata::getReadEMCInfo(unsigned int serialNo, 
		int *runFrm,
		int *runTo,
		std::string& calParVer,
		DatabaseRecordVector& res,
		int runNo,
		std::string& sftver) {
	using namespace rdbModel;
	eRet ret;
	char stmt1[300];
	int run_No =runNo;
	if(sftver=="default")
		sftver = getenv("BES_RELEASE");
	const char* SftVer = sftver.c_str();

	if(calParVer!="default"){ 
		const char* calpar = calParVer.c_str();
		sprintf(stmt1,"select DigiCalibConst,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from EmcCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and CalParVer = %s",SftVer,run_No,run_No,calpar);
	}

	if(calParVer=="default"){
		//sprintf(stmt1,"select DigiCalibConst,RunFrom,RunTo,max(CalParVer),FileName from EmcCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d group by CalParVer",SftVer,run_No,run_No);
		sprintf(stmt1,"select DigiCalibConst,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from EmcCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d order by CalParVer desc",SftVer,run_No,run_No);
	} 

	int row_no = m_dbsvc->query("offlinedb", stmt1, res);
	if(row_no<1){
		std::cout<<"ERROR:error searching EMC calibration Data in the database with: "<<stmt1<<std::endl;
		return RETMySQLError;
	}
	return RETOk;
}


Metadata::eRet Metadata::getReadDedxInfo(unsigned int serialNo, 
		int *runFrm,
		int *runTo,
		std::string& calParVer,
		DatabaseRecordVector& res,
		int runNo,
		std::string& sftver) {
	using namespace rdbModel;
	eRet ret;
	char stmt1[400];
	int run_No =runNo;
	if(sftver=="default")
		sftver = getenv("BES_RELEASE");
	const char* SftVer = sftver.c_str();

	if(calParVer!="default"){
		const char* calpar = calParVer.c_str();
		sprintf(stmt1,"select  DriftDist,EntranceAng,MeanGain,GasGain,LayerGain,Resolution,WireGain,ZDep,RunGain,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from DedxCalConst where  SftVer = '%s' and RunFrom <= %d and RunTo >= %d and CalParVer = %s",SftVer,run_No,run_No,calpar);

	}
	if(calParVer=="default"){
		// sprintf(stmt1,"select  DriftDist,EntranceAng,MeanGain,GasGain,LayerGain,Resolution,WireGain,ZDep,RunGain,RunFrom,RunTo,max(CalParVer),FileName from DedxCalConst where  SftVer = '%s' and RunFrom <= %d and RunTo >= %d group by CalParVer",SftVer,run_No,run_No);
		sprintf(stmt1,"select  DriftDist,EntranceAng,MeanGain,GasGain,LayerGain,Resolution,WireGain,ZDep,RunGain,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from DedxCalConst where  SftVer = '%s' and RunFrom <= %d and RunTo >= %d order by CalParVer desc",SftVer,run_No,run_No);
	}

	int row_no = m_dbsvc->query("offlinedb", stmt1, res);
	if(row_no<1){
		std::cout<<"ERROR:error searching Dedx calibration Data in the database with: "<<stmt1<<std::endl;
		return RETMySQLError;
	}
	return RETOk;
}


Metadata::eRet Metadata::getReadEsTimeInfo(unsigned int serialNo,   
		int *runFrm,   
		int *runTo,   
		std::string& calParVer,   
		DatabaseRecordVector& res,
		int runNo,
		std::string& sftver) {   
	using namespace rdbModel;   
	eRet ret;   
	char stmt1[400];   
	int run_No =runNo;   
	if(sftver=="default")
		sftver = getenv("BES_RELEASE");
	const char* SftVer = sftver.c_str();

	if(calParVer!="default"){   
		const char* calpar = calParVer.c_str();   
		sprintf(stmt1,"select EsTimeH2,EsTimeH9,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from EsTimeCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and CalParVer = %s",SftVer,run_No,run_No,calpar);   
	}   

	if(calParVer=="default"){   
		sprintf(stmt1,"select EsTimeH2,EsTimeH9,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from EsTimeCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d order by CalParVer desc",SftVer,run_No,run_No);   
	}   
	int row_no = m_dbsvc->query("offlinedb", stmt1, res);
	if(row_no<1){
		std::cout<<"ERROR:error searching EsTime calibration Data in the database with: "<<stmt1<<std::endl;
		return RETMySQLError;
	}

	return RETOk;   
} 


//get EstTof Information from mysql Database
Metadata::eRet Metadata::getReadEstTofInfo(unsigned int serialNo,
		int *runFrm,
		int *runTo,
		std::string& calParVer,
		DatabaseRecordVector& res,
		int runNo,
		std::string& sftver) {
	using namespace rdbModel;
	eRet ret;
	char stmt1[200];
	int run_No =runNo;
	if(sftver=="default")
		sftver = getenv("BES_RELEASE");
	const char* SftVer = sftver.c_str();

	if(calParVer!="default"){
		const char* calpar = calParVer.c_str();
		sprintf(stmt1,"select BarTofPar,EndTofPar,TofConPar,BarTof,EtfTofPar,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from EstTofCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and CalParVer = %s",SftVer,run_No,run_No,calpar);
	}
	if(calParVer=="default"){
		//  sprintf(stmt1,"select BarTofPar,EndTofPar,TofConPar,BarTof,RunFrom,RunTo,max(CalParVer),FileName from EstTofCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d group by CalParVer",SftVer,run_No,run_No);
		sprintf(stmt1,"select BarTofPar,EndTofPar,TofConPar,BarTof,EtfTofPar,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from EstTofCalConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d order by CalParVer desc",SftVer,run_No,run_No);
	}

	int row_no = m_dbsvc->query("offlinedb", stmt1, res);
	if(row_no<1){
		std::cout<<"ERROR:error searching EstTOF calibration Data in the database with: "<<stmt1<<std::endl;
		return RETMySQLError;
	}
	return RETOk;
}


Metadata::eRet Metadata::getReadTofSimInfo(unsigned int serialNo,
		int *runFrm,
		int *runTo,
		std::string& calParVer,
		DatabaseRecordVector& res,
		int runNo,
		std::string& sftver) {
	using namespace rdbModel;
	eRet ret;
	char stmt1[200];
	int run_No =runNo;
	if(sftver=="default")
		sftver = getenv("BES_RELEASE");
	const char* SftVer = sftver.c_str();

	if(calParVer!="default"){
		const char* calpar = calParVer.c_str();
		sprintf(stmt1,"select BTofSim,ETofSim,SimConstants,RunFrom,RunTo,TofSimParVer,FileName,Status,SftVer from TofSimSvc where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and TofSimParVer = %s",SftVer,run_No,run_No,calpar);
	}
	if(calParVer=="default"){
		// sprintf(stmt1,"select  BTofSim,ETofSim,SimConstants,RunFrom,RunTo,max(TofSimParVer),FileName from TofSimSvc where SftVer = '%s' and RunFrom <= %d and RunTo >= %d group by TofSimParVer",SftVer,run_No,run_No);
		sprintf(stmt1,"select  BTofSim,ETofSim,SimConstants,RunFrom,RunTo,TofSimParVer,FileName,Status,SftVer from TofSimSvc where SftVer = '%s' and RunFrom <= %d and RunTo >= %d order by TofSimParVer desc",SftVer,run_No,run_No);
	}


	int row_no = m_dbsvc->query("offlinedb", stmt1, res);
	if(row_no<1){
		std::cout<<"ERROR:error searching TOFSim calibration Data in the database with: "<<stmt1<<std::endl;
		return RETMySQLError;
	}

	return RETOk;
}


Metadata::eRet Metadata::getReadDedxSimInfo(unsigned int serialNo,
		int *runFrm,
		int *runTo,
		std::string& calParVer,
		DatabaseRecordVector& res,
		int runNo,
		std::string& sftver) {
	using namespace rdbModel;
	eRet ret;
	char stmt1[200];
	int run_No =runNo;
	if(sftver=="default")
		sftver = getenv("BES_RELEASE");
	const char* SftVer = sftver.c_str();

	if(calParVer!="default"){
		const char* calpar = calParVer.c_str();
		sprintf(stmt1,"select TH1F_Col,bin,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from DedxSim where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and CalParVer = %s",SftVer,run_No,run_No,calpar);
	}
	if(calParVer=="default"){
		sprintf(stmt1,"select  TH1F_Col,bin,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from DedxSim where SftVer = '%s' and RunFrom <= %d and RunTo >= %d order by CalParVer desc",SftVer,run_No,run_No);
	}

	int row_no = m_dbsvc->query("offlinedb", stmt1, res);
	if(row_no<1){
		std::cout<<"ERROR:error searching DedxSim calibration Data in the database with: "<<stmt1<<std::endl;
		return RETMySQLError;
	}
	return RETOk;
}



Metadata::eRet Metadata::getReadMdcAlignInfo(unsigned int serialNo,
		int *runFrm,
		int *runTo,
		std::string& calParVer,
		DatabaseRecordVector& res,
		int runNo,
		std::string& sftver) {
	using namespace rdbModel;
	eRet ret;
	char stmt1[200];
	int run_No =runNo;
	if(sftver=="default")
		sftver = getenv("BES_RELEASE");
	const char* SftVer = sftver.c_str();

	if(calParVer!="default"){
		const char* calpar = calParVer.c_str();
		sprintf(stmt1,"select AlignEndPlate,WirePos,WireTension,RunFrom,RunTo,MdcAlignVer,AlignEPFileName,Status,SftVer from MdcAlignment where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and MdcAlignVer = %s",SftVer,run_No,run_No,calpar);
	}
	if(calParVer=="default"){
		//     sprintf(stmt1,"select AlignEndPlate,WirePos,WireTension,RunFrom,RunTo,max(MdcAlignVer),AlignEPFileName from MdcAlignment where SftVer = '%s' and RunFrom <= %d and RunTo >= %d group by MdcAlignVer",SftVer,run_No,run_No);
		sprintf(stmt1,"select AlignEndPlate,WirePos,WireTension,RunFrom,RunTo,MdcAlignVer,AlignEPFileName,Status,SftVer from MdcAlignment where SftVer = '%s' and RunFrom <= %d and RunTo >= %d order by MdcAlignVer desc",SftVer,run_No,run_No);
	}

	int row_no = m_dbsvc->query("offlinedb", stmt1, res);
	if(row_no<1){
		std::cout<<"ERROR:error searching MdcAlignment calibration Data in the database with: "<<stmt1<<std::endl;
		return RETMySQLError;
	}

	return RETOk;
}

Metadata::eRet Metadata::getReadMdcDataConstInfo(unsigned int serialNo,
		int *runFrm,
		int *runTo,
		std::string& calParVer,
		DatabaseRecordVector& res,
		int runNo,
		std::string& sftver) {
	using namespace rdbModel;
	eRet ret;
	char stmt1[200];
	int run_No =runNo;
	if(sftver=="default")
		sftver = getenv("BES_RELEASE");
	const char* SftVer = sftver.c_str();

	if(calParVer!="default"){
		const char* calpar = calParVer.c_str();
		sprintf(stmt1,"select WireEff,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from MdcDataConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and CalParVer = %s",SftVer,run_No,run_No,calpar);
	}
	if(calParVer=="default"){
		sprintf(stmt1,"select WireEff,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from MdcDataConst where SftVer = '%s' and RunFrom <= %d and RunTo >= %d order by CalParVer desc",SftVer,run_No,run_No);
	}

	int row_no = m_dbsvc->query("offlinedb", stmt1, res);
	if(row_no<1){
		std::cout<<"ERROR:error searching MdcDataConst Data in the database with: "<<stmt1<<std::endl;
		return RETMySQLError;
	}

	return RETOk;
}

Metadata::eRet Metadata::getReadTofQElecInfo(unsigned int serialNo,
		int *runFrm,
		int *runTo,
		std::string& calParVer,
		DatabaseRecordVector& res,
		int runNo,
		std::string& sftver) {
	using namespace rdbModel;
	eRet ret;
	char stmt1[400];
	int run_No =runNo;
	if(sftver=="default")
		sftver = getenv("BES_RELEASE");
	const char* SftVer = sftver.c_str();

	if(calParVer!="default"){
		const char* calpar = calParVer.c_str();
		sprintf(stmt1,"select BarBoardNum,EndBoardNum,QELecBarParEast,QELecBarParWest,QELecEndPar,SimQELecBarParEast,SimQELecBarParWest,SimQELecEndPar,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from TofQELec where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and CalParVer = %s",SftVer,run_No,run_No,calpar);
	}
	if(calParVer=="default"){
		sprintf(stmt1,"select BarBoardNum,EndBoardNum,QELecBarParEast,QELecBarParWest,QELecEndPar,SimQELecBarParEast,SimQELecBarParWest,SimQELecEndPar,RunFrom,RunTo,CalParVer,FileName,Status,SftVer from TofQELec where SftVer = '%s' and RunFrom <= %d and RunTo >= %d order by CalParVer desc",SftVer,run_No,run_No);
	}
	int row_no = m_dbsvc->query("offlinedb", stmt1, res);
	if(row_no<1){
		std::cout<<"ERROR:error searching TofQElec calibration Data in the database with: "<<stmt1<<std::endl;
		return RETMySQLError;
	}
	std::cout<<"metadata ok"<<std::endl;
	return RETOk;
}


/*

	 | Field       | Type               | Null | Key | Default | How set          |
	 +-------------+--------------------+------+-----+---------+------------------+
	 | ser_no      | mediumint(9)       |      | PRI | NULL    | auto_increment   |
	 | instrument  | varchar(16)        |      |     |         |   [openRecord]   |
	 | calib_type  | varchar(20)        |      |     |         |   [openRecord    |
	 | data_fmt    | varchar(10)        |      |     |         |   [openRecord]   |
	 | data_size   | int(11)            | YES  |     | NULL    |   [optional]     |
	 | vstart      | datetime           | YES  |     | NULL    |[addValidInterval]|
	 | vend        | datetime           | YES  |     | NULL    |[addValidInterval]|
	 | enter_time  | timestamp(14)      | YES  |     | NULL    | automatic        |
	 | fmt_version | varchar(12)        | YES  |     | NULL    |   [openRecord]   |
	 | completion  | enum('OK','INC','ABORT')           
	 | YES  | MUL | NULL    |   [openRecord]   |
	 | proc_level  | enum('PROD','TEST','DEV', 'SUPSED')
	 |      |     | TEST    |   [openRecord]   |
	 | creator     | varchar(255)       | YES  |     | NULL    |   [addCreator]   |
	 | uid         | varchar(12)        |      |     |         |[insertRecord/
	 addUser]        |
	 | data_ident  | varchar(255)       |      |     |         |   [openRecord]   |
	 | input_desc  | varchar(255)       | YES  |     | NULL    |[addInputDesc]    |
	 | notes       | varchar(255)       | YES  |     | NULL    |   [addNotes]     |
	 +-------------+--------------------+------+-----+---------+------------------+

*/

Metadata::eRet 
Metadata::doSelect(std::vector<rdbModel::Assertion::Operator *>& conditions, 
		rdbModel::StringVector& orderBy,
		unsigned*& ser) {
	using namespace rdbModel;
	*ser = 0;

	StringVector colNames(1);
	colNames[0] = "ser_no"; 

	// make the combined operator
	Assertion::Operator andOp(OPTYPEand, conditions, true);

	//Following creates an assertion such that creator (us) continues
	// to own the associated operator.
	Assertion whereClause(&andOp, 0, true);
	ResultHandle* results = 0;

	if(results){
		delete results;
	}

	try {               // make the query
		results = m_readCxt->select(m_table, colNames, orderBy, &whereClause);

	}
	catch (RdbException ex) {
		std::cout << ex.getMsg();
	}

	if (!results) {   // Error. Should have ResultHandle even if 0 rows.
		std::cout<<"no results"<<std::endl;
		return RETMySQLError;
	}



	std::cout<<"There are results"<<std::endl;
	// handle result.  
	if (results->getNRows() == 0) {
		conditions.pop_back(); 
		std::cout<<"no results"<<std::endl; 
	}
	else{	       
		std::vector<std::string> fields;
		results->getRow(fields);

		if(results){
			delete results;
		}
		*ser = facilities::Util::stringToInt(fields[0]);
		return RETOk;
	}

}

int  Metadata::registerCalib(const std::string& inst, 
		const std::string& flavor,
		const std::string& calib_type, 
		const std::string& data_ident,
		const std::string& data_fmt,
		unsigned int& runfrm,
		unsigned int& runto,
		const std::string& input_desc, 
		const std::string& notes,
		const std::string& proc_level,
		const std::string& locale,
		const std::string& fmt_version,
		const std::string& completion)
{

	using namespace rdbModel;

	eRet ret;

	if (!m_writeCxt) {
		connectWrite(ret);
		if (ret != RETOk) return 0; // we or connectWrite should throw exception
	}
	StringVector cols;
	StringVector vals;
	StringVector nullCols;

	cols.reserve(24);
	vals.reserve(24);
	nullCols.reserve(16);

	if (inst.size() * calib_type.size() * flavor.size() * data_fmt.size() 
			* data_ident.size() * proc_level.size() * completion.size() 
			* locale.size() == 0) {   // something is null that shouldn't be
		return 0;      // should perhaps throw exception
	}
	cols.push_back("calib_type"); vals.push_back(calib_type);
	// cols.push_back("flavor"); vals.push_back(flavor);
	cols.push_back("data_fmt"); vals.push_back(data_fmt);
	cols.push_back("data_ident"); vals.push_back(data_ident);
	cols.push_back("status"); vals.push_back(completion);
	std::string s_runfrm,s_runto;
	facilities::Util::itoa(runfrm,s_runfrm);
	facilities::Util::itoa(runto,s_runto);
	cols.push_back("RunFrom"); vals.push_back(s_runfrm);
	cols.push_back("RunTo"); vals.push_back(s_runto);

	//  These, however, may be null
	if (input_desc.size() > 0 ) {
		cols.push_back("input_desc"); vals.push_back(input_desc); 
	}  else nullCols.push_back("input_desc");

	if (notes.size() > 0 ) {
		cols.push_back("notes"); vals.push_back(notes);
	}    else nullCols.push_back("notes");

	if (fmt_version.size() > 0 )
	{
		cols.push_back("fmt_version"); 
		vals.push_back(fmt_version);
	} 

	// The service -- that's us -- is responsible for creator, uid, enter_time
	cols.push_back("creator"); vals.push_back("Metadata::registerCalib");
	std::string uid;
	fetchUser(uid);
	cols.push_back("uid"); vals.push_back(uid);
	facilities::Timestamp curTime;
	cols.push_back("enter_time");vals.push_back(curTime.getString());
	// update_time is set automatically by MySQL, but MySQL uses
	// local timezone rather than gmt, so we have set it explicitly
	cols.push_back("update_time");vals.push_back(curTime.getString());


	if (m_rdb) {
		bool ok = checkValues(cols, vals);
		if (ok) checkNulls(nullCols);
		if (!ok) return 0;
	}

	// ser_no gets set automatically by MySQL
	int ser_no;
	if (!(m_writeCxt->insertRow(m_table, cols, vals, &ser_no, &nullCols)) ) {
		return 0;
	}    else {
		adjustVend(ser_no);
		return ser_no;
	}
}

Metadata::eRet Metadata::compareSchema(rdbModel::Connection* conn,
		const std::string& schema) {
	using namespace rdbModel;


	if (m_man) { // already did this
		return (m_match) ? RETOk : RETNoSchemaMatch;
	}
	m_man = rdbModel::Manager::getManager();

	m_man->setBuilder(new rdbModel::XercesBuilder);
	m_man->setInputSource(schema);

	// Maybe first check if file exists?  E.g., try opening for read

	// good errcode is 0
	int errcode = m_man->build();

	if (errcode) {
		std::cerr << "Error in database description file " << schema 
			<< std::endl;
		std::cerr << "Parse failed with error " << errcode << std::endl;
		return RETBadCnfFile;
	}
	m_rdb = m_man->getRdb();

	rdbModel::MATCH match = conn->matchSchema(m_rdb, false);

	switch (match) {
		case rdbModel::MATCHequivalent:
		case rdbModel::MATCHcompatible:
			m_match = true;
			return RETOk;
		case rdbModel::MATCHfail:
			std::cout << "XML schema and MySQL database are NOT compatible" 
				<< std::endl;
			return RETBadCnfFile;
		case rdbModel::MATCHnoConnection:
			std::cout << "Connection failed while attempting match" << std::endl;
			return RETNoConnect;
	}
	return RETBadValue;
}

bool Metadata::checkValues(const rdbModel::StringVector& cols,
		const rdbModel::StringVector& vals) const {
	unsigned nCol = cols.size();
	rdbModel::Table* table = m_rdb->getTable(m_table);


	for (unsigned iCol = 0; iCol < nCol; iCol++) {
		rdbModel::Column* col = table->getColumnByName(cols[iCol]);
		if (!col->okValue(vals[iCol])) {
			std::cerr << "Value " << vals[iCol] << " not allowed for column "
				<< cols[iCol] << " in table " << m_table << std::endl;
			return false;
		}
	}
	return true;
}

bool Metadata::checkNulls(const rdbModel::StringVector& cols) const {
	unsigned nCol = cols.size();
	rdbModel::Table* table = m_rdb->getTable(m_table);


	for (unsigned iCol = 0; iCol < nCol; iCol++) {
		rdbModel::Column* col = table->getColumnByName(cols[iCol]);
		if (!col->nullAllowed()) {
			std::cerr << "Column "
				<< cols[iCol] << " in table " << m_table
				<< " is not nullable" << std::endl;
			return false;
		}
	}
	return true;
}


unsigned Metadata::adjustVend(int newSer) {
	using namespace rdbModel;

	StringVector getCols;
	StringVector orderBy;

	orderBy.clear();
	getCols.reserve(7);

	std::string serString;
	facilities::Util::itoa(newSer, serString);

	getCols.push_back("flavor");
	getCols.push_back("calib_type");
	getCols.push_back("completion");

	ResultHandle* results = 0;
	eRet err;
	Assertion* where = 0;
	try {
		if (!m_writeCxt) {
			if (!connectWrite(err)) return 0;
		}

		Assertion::Operator* serOp = 
			new Assertion::Operator(OPTYPEequal, "ser_no", serString, 
					FIELDTYPEold, FIELDTYPElit);
		//false, true);

		where = new Assertion(serOp);

		// Fetch information for new row: vstart, flavor, completion,
		// proc_level, calib_type, flavor
		results = m_writeCxt->select(m_table, getCols, orderBy, where);
		// results = m_writeCxt->select("metadata_v0", getCols, orderBy, where);
		delete where;
		where = 0;
	}
	catch (RdbException ex) {
		std::cout << ex.getMsg();
		delete where;         // return heap memory
		return 0;
	}
	if (!results) { // This is an error. Should be non-null even if no rows
		std::cout << "MySQL failure in SELECT" << std::endl;
		return 0;  // nothing to fix
	}
	if (results->getNRows() != 1) { // also a problem
		std::cout << "Look-up of serial# " << serString << " failed"
			<< std::endl;
		return 0;
	}
	std::vector<std::string> fields;
	results->getRow(fields);
	if (fields[2] != "OK") return 0;  // don't bother fixing in this case

	// Now do an update on rows satisfying
	// ((flavor="f") && (calib_type = "c") && (completion= "OK") && 
	// (instrument = "i") &&  (proc_level = "p") && 
	//  (vstart < "new-start" (vend > "new-vstart") );
	std::vector<Assertion::Operator *> conditions;
	conditions.reserve(7);
	for (unsigned ix = 0; ix < 5; ix++) {
		conditions.push_back(new Assertion::Operator(OPTYPEequal, getCols[ix],
					fields[ix], 
					FIELDTYPEold, FIELDTYPElit));
		// false, true));
	}
	conditions.push_back(new Assertion::Operator(OPTYPElessThan, "vstart",
				fields[5], 
				FIELDTYPEold, FIELDTYPElit));
	//false, true));
	conditions.push_back(new Assertion::Operator(OPTYPEgreaterThan, "vend",
				fields[5], 
				FIELDTYPEold, FIELDTYPElit));
	// false, true));

	Assertion::Operator* andOp = 
		new Assertion::Operator(OPTYPEand, conditions);
	where = new Assertion(andOp);

	StringVector toUpdate;
	toUpdate.push_back("vend");
	StringVector newVal;
	newVal.push_back(fields[5]);
	// also update update_time. If we leave it to MySQL, won't be GMT
	facilities::Timestamp curTime;
	toUpdate.push_back("update_time");newVal.push_back(curTime.getString());

	unsigned nModified = m_writeCxt->update(m_table, toUpdate, newVal, where);
	delete where;
	return nModified;
}

}


