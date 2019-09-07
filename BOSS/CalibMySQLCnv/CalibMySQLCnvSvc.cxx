// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibMySQLCnv/src/CalibMySQLCnvSvc.cxx,v 1.49 2013/05/22 07:57:53 maqm Exp $
#include <string>
#include <cstdio>
#include <stdexcept>

#include "CalibMySQLCnv/CalibMySQLCnvSvc.h"
#include "CalibDataSvc/ICalibRootSvc.h"   // for def. of CALIBROOT_StorageType
#include "CalibDataSvc/ICalibTreeSvc.h" 
#include "calibUtil/Metadata.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include "CalibData/CalibBase.h"
#include "CalibData/CalibBase1.h"
//#include "CalibData/CalibTime.h"
#include "CalibData/CalibModelSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "TBuffer.h"
#include "TBufferFile.h"
#include "TTree.h"
#include "CalibMySQLCnv/TreeAddress.h"
#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"
#include "DatabaseSvc/IDatabaseSvc.h"
#include "TROOT.h"
#include "TFile.h"
#include <iostream>

using namespace CalibData;
/// Instantiation of a static factory to create instances of this service
//static SvcFactory<CalibMySQLCnvSvc>          CalibMySQLCnvSvc_factory;
//const ISvcFactory& CalibMySQLCnvSvcFactory = CalibMySQLCnvSvc_factory;

// Local utility to translate calibration quality list to bit map
namespace {
	unsigned int toQualityMask(std::vector<std::string>& qualities) {
		using calibUtil::Metadata;

		unsigned int mask = 0;
		unsigned n = qualities.size();

		for (unsigned i = 0; i < n; i++) {
			std::string iString = qualities[i];
			if (iString.size() < 3) continue;
			iString.resize(3);
			if (iString == "PRO") mask |= Metadata::LEVELProd;
			else if (iString =="DEV") mask |= Metadata::LEVELDev;
			else if (iString =="TES") mask |= Metadata::LEVELTest;
			else if (iString =="SUP") mask |= Metadata::LEVELSuperseded;
		}
		return mask;
	}
}

	CalibMySQLCnvSvc::CalibMySQLCnvSvc( const std::string& name, ISvcLocator* svc)
	: ConversionSvc (name, svc, MYSQL_StorageType)
		, m_meta(0), m_useEventTime(true),m_enterTimeStart(0), m_enterTimeEnd(0),
		m_qualityMask(0)
{
	// declareProperty("Host", m_host = "202.122.37.69");
	declareProperty("Host", m_host = "bes3db2.ihep.ac.cn");
	declareProperty("UseEventTime", m_useEventTime = true);
	declareProperty("EnterTimeEnd", m_enterTimeEndString = std::string("") );
	declareProperty("EnterTimeStart", m_enterTimeStartString = std::string("") );
	//  declareProperty("DbName", m_dbName = std::string("calib") );
	declareProperty("DbName", m_dbName = std::string("offlinedb") );
	declareProperty("QualityList", m_qualityList);
	declareProperty("CrashOnError", m_crash = true);
	declareProperty("Sft_Ver",sft_ver);
	declareProperty("Cal_Ver",cal_ver);
	declareProperty("MdcFlag",m_flag[0]="default");
	declareProperty("DeDxFlag",m_flag[1]="default");
	declareProperty("EmcFlag",m_flag[2]="default");
	declareProperty("TofFlag",m_flag[3]="default");
	declareProperty("MucFlag",m_flag[4]="default");
	declareProperty("EsTimeFlag",m_flag[5]="default");
	declareProperty("EstTofFlag",m_flag[6]="default");
	declareProperty("MdcAlignFlag",m_flag[7]="default");
	declareProperty("TofQElecFlag",m_flag[8]="default");
	declareProperty("TofSimPFlag",m_flag[9]="default");
	declareProperty("DedxSimFlag",m_flag[10]="default");
	declareProperty("MdcDataFlag",m_flag[11]="default");
	declareProperty("MdcCalPar",m_calPar[0]="default");
	declareProperty("DeDxCalPar",m_calPar[1]="default");
	declareProperty("EmcCalPar",m_calPar[2]="default");
	declareProperty("TofCalPar",m_calPar[3]="default"); 
	declareProperty("MucCalPar",m_calPar[4]="default");
	declareProperty("EsTimeCalPar",m_calPar[5]="default");
	declareProperty("EstTofCalPar",m_calPar[6]="default");
	declareProperty("MdcAlignPar",m_calPar[7]="default");
	declareProperty("TofQElecPar",m_calPar[8]="default");
	declareProperty("TofSimPar",m_calPar[9]="default");
	declareProperty("DedxSimPar",m_calPar[10]="default");
	declareProperty("MdcDataConstVer",m_calPar[11]="default");
	declareProperty("MdcBossVer",m_bossver[0]="default");
	declareProperty("DeDxBossVer",m_bossver[1]="default");
	declareProperty("EmcBossVer",m_bossver[2]="default");
	declareProperty("TofBossVer",m_bossver[3]="default");
	declareProperty("MucBossVer",m_bossver[4]="default");
	declareProperty("EsTimeBossVer",m_bossver[5]="default");
	declareProperty("EstTofBossVer",m_bossver[6]="default");
	declareProperty("MdcAlignBossVer",m_bossver[7]="default");
	declareProperty("TofQElecBossVer",m_bossver[8]="default");
	declareProperty("TofSimBossVer",m_bossver[9]="default");
	declareProperty("DedxSimBossVer",m_bossver[10]="default");
	declareProperty("MdcDataConstBossVer",m_bossver[11]="default");
	declareProperty("Align_RunNo",m_MdcAlign_No=8093);
	declareProperty("Db_Status",m_dbStatus="OK");
	declareProperty("BossRelease",m_bossRelease="default");
}

CalibMySQLCnvSvc::~CalibMySQLCnvSvc(){ }

StatusCode CalibMySQLCnvSvc::initialize()
{
	// Initialize base class
	StatusCode sc = ConversionSvc::initialize();
	if ( !sc.isSuccess() ) return sc;

	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );
	log << MSG::INFO << "Specific initialization starting" << endreq;

	IDataProviderSvc* pCDS = 0;
	sc = serviceLocator()->getService ("CalibDataSvc",  IDataProviderSvc::interfaceID(), (IInterface*&)pCDS);

	if ( !sc.isSuccess() ) {
		log << MSG::ERROR << "Could not locate CalibDataSvc" << endreq;
		return sc;
	}

	sc = serviceLocator()->service("EventDataSvc", m_eventSvc, true);
	if (sc .isFailure() ) {
		log << MSG::ERROR << "Unable to find EventDataSvc " << endreq;
		return sc;
	}

	// Set the CalibDataSvc as data provider service
	sc = setDataProvider(pCDS);
	if ( !sc.isSuccess() ) {
		log << MSG::ERROR << "Could not set data provider" << endreq;
		return sc;
	}

	// Query the IAddressCreator interface of the detector persistency service

	sc = serviceLocator()->service 
		("DetectorPersistencySvc", m_detPersSvc, true);
	if ( !sc.isSuccess() ) {
		log << MSG::ERROR 
			<< "Cannot locate IConversionSvc interface of DetectorPersistencySvc"
			<< endreq;
		return sc;
	} else {
		log << MSG::DEBUG 
			<< "Retrieved IConversionSvc interface of DetectorPersistencySvc"
			<< endreq;
	}

	IAddressCreator* iAddrCreator;
	sc = m_detPersSvc->queryInterface(IAddressCreator::interfaceID(),(void**) &iAddrCreator);
	//  sc = m_detPersSvc->queryInterface(IID_IAddressCreator,(void**) &iAddrCreator);
	if ( !sc.isSuccess() ) {
		log << MSG::ERROR << "Could not locate CalibDataSvc" << endreq;
		return sc;
	}

	sc = setAddressCreator(iAddrCreator);
	if ( !sc.isSuccess() ) {
		log << MSG::ERROR 	<< "Cannot set the address creator" << endreq;
		return sc;
	}

	// Get properties from the JobOptionsSvc
	sc = setProperties();
	if ( !sc.isSuccess() ) {
		log << MSG::ERROR << "Could not set jobOptions properties" << endreq;
		return sc;
	}
	log << MSG::DEBUG << "Properties were read from jobOptions" << endreq;

	// Translate list of calibration quality names to bit mask form used 
	// by calibUtil::Metadata::findBest     Defaults to PROD + DEV for now
	// (that was old fixed value)
	m_qualityMask = toQualityMask(m_qualityList);
	if (!m_qualityMask) {
		m_qualityMask = calibUtil::Metadata::LEVELProd |
			calibUtil::Metadata::LEVELDev;
	}

	// Make a calibUtil::Metadata instance 
	// Conceivably, could start up a different conversion service, depending 
	// on job options parameters, which would look very much like this one
	// except for having a different way to access metadata.
	m_meta = new calibUtil::Metadata(m_host, "*", m_dbName);

	if (!m_meta) {
		log << MSG::ERROR << "Could not open connection to metadata dbs" << endreq;
		return MSG::ERROR;
	}
	// Probably should get this value from job options. 
	// Now we do.  See m_qualityMask, m_qualityList
	//  m_calibLevelMask = calibUtil::Metadata::LEVELProd + 
	//    calibUtil::Metadata::LEVELDev;

	log << MSG::INFO << "Specific initialization completed" << endreq;
	return sc;
}


StatusCode CalibMySQLCnvSvc::finalize()
{
	MsgStream log(msgSvc(), "CalibMySQLCnvSvc");
	log << MSG::DEBUG << "Finalizing" << endreq;
	delete m_meta;
	m_meta = 0;
	return ConversionSvc::finalize();
}


StatusCode CalibMySQLCnvSvc::queryInterface(const InterfaceID& riid, 
		void** ppvInterface)
{
	if ( IID_ICalibMetaCnvSvc == riid )  {
		// With the highest priority return the specific interface of this service
		*ppvInterface = (ICalibMetaCnvSvc*)this;
	} else  {
		// Interface is not directly available: try out a base class
		return ConversionSvc::queryInterface(riid, ppvInterface);
	}
	addRef();
	return StatusCode::SUCCESS;
}


/// Create a transient representation from another representation of an object.
/// Overloaded from ConversionSvc because CalibMySQLCnvSvc has no converters.
/// (The typical conversion service delegates this to an appropriate converter)
StatusCode CalibMySQLCnvSvc::createObj (IOpaqueAddress* pAddress, 
		DataObject*&    refpObject ) {

	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );


	// Create the object according to calib type, flavor, time, instrument, clid.
	// Notice that the CalibMySQLCnvSvc has no converters of its own:
	// object creation is delegated to another CnvSvc via a temporary address
	// The IOpaqueAddress specifies calibration type and specific flavor.
	// The secondary storage type is always discovered dynamically
	StatusCode sc;

	sc = createCalib(refpObject,
			pAddress->par()[0],
			pAddress->clID(),
			pAddress->registry() );


	if ( !sc.isSuccess() ) {
		log << MSG::ERROR << "Could not create calib DataObject" << endreq;
	}
	log << MSG::DEBUG << "Method createObj exiting" << endreq;
	return sc;
}

/// Resolve the references of the created transient object.
/// (Actually, don't, because this operation isn't supported, nor is
/// it needed for the conversion service.)
/// Overloaded from ConversionSvc because CalibMySQLCnvSvc has no converters.
StatusCode CalibMySQLCnvSvc::fillObjRefs(IOpaqueAddress* /*pAddress*/, 
		DataObject*     /*pObject */ ) {
	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );
	return StatusCode::SUCCESS;
}


/// Update a transient representation from another representation of an object.
/// Overloaded from ConversionSvc because CalibMySQLCnvSvc has no converters.
StatusCode CalibMySQLCnvSvc::updateObj(IOpaqueAddress* pAddress, 
		DataObject*     pObject  ) {

	//  using facilities::Timestamp;

	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );

	// Don't update when we're using enter time
	log << MSG::DEBUG << "CalibMySQLCnvSvc::updateObj starting ...."<<endreq;

	if( 0 == pObject ) {
		log << MSG::ERROR << "There is no object to update" << endreq;
		return StatusCode::FAILURE;
	} 

	StatusCode sc = updateCalib(pObject, pAddress->par()[0], 
			pAddress->clID(), pAddress->registry() );
	if ( !sc.isSuccess() ) {
		log << MSG::ERROR << "Could not update calib DataObject" << endreq;
		return sc;
	}

	return StatusCode::SUCCESS;  
}

/// Update the references of an updated transient object. [actually, don't.
/// Calib data doesn't have any inter-object references.]
/// Overloaded from ConversionSvc because CalibMySQLCnvSvc has no converters.
StatusCode CalibMySQLCnvSvc::updateObjRefs (IOpaqueAddress* /*pAddress*/, 
		DataObject*     /*pObject */ ) {
	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );
	return StatusCode::SUCCESS;
}


/// Convert a transient object to a requested representation. Not implemented.
/// Overloaded from ConversionSvc because CalibMySQLCnvSvc has no converters.
StatusCode CalibMySQLCnvSvc::createRep(DataObject* /*pObject*/,
		IOpaqueAddress*& /*refpAddress*/ ) {

	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );
	// log << MSG::WARNING << "Method createRep is not implemented" << endreq;
	return StatusCode::SUCCESS;
}


/// Resolve the references of a converted object. [actually, don't.
/// Calib data doesn't have any inter-object references.]
/// Overloaded from ConversionSvc because CalibMySQLCnvSvc has no converters.
StatusCode CalibMySQLCnvSvc::fillRepRefs (IOpaqueAddress* /*pAddress*/, 
		DataObject*     /*pObject */ ) {
	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );
	//  log << MSG::WARNING << "Method fillRepRefs is not implemented" << endreq;
	return StatusCode::SUCCESS;
}


/// Update a converted representation of a transient object.
/// Overloaded from ConversionSvc because CalibMySQLCnvSvc has no converters.
StatusCode CalibMySQLCnvSvc::updateRep (IOpaqueAddress* /*pAddress*/, 
		DataObject*     /*pObject */ ) {
	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );
	// log << MSG::WARNING << "Method updateRep is not implemented" << endreq;
	return StatusCode::SUCCESS;
}


/// Update the references of an already converted object.
/// Overloaded from ConversionSvc because CalibMySQLCnvSvc has no converters.
/// Don't do anything because calib objects have no inter-object references.
StatusCode CalibMySQLCnvSvc::updateRepRefs (IOpaqueAddress* /*pAddress*/, 
		DataObject*     /*pObject */ ) {
	MsgStream log(msgSvc(), "CalibMySQLCnvSvc");
	//log << MSG::WARNING << "Method updateRepRefs is not implemented" << endreq;
	return StatusCode::SUCCESS;
}

/// Overload ConversionSvc implementation of createAddress.  
/// Create an address using explicit arguments to identify a single object.
/// Par[0] is full path in calibration TDS
StatusCode CalibMySQLCnvSvc::createAddress(long svc_type,
		const CLID& clid,
		const std::string* par, 
		const unsigned long* /*ipar*/,
		IOpaqueAddress*& refpAddress ) {
	// First check that requested address is of type MYSQL_StorageType
	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );
	if ( svc_type!= MYSQL_StorageType ) {
		log << MSG::ERROR 
			<< "Cannot create addresses of type " << (int)svc_type 
			<< " which is different from " << (int)MYSQL_StorageType 
			<< endreq;
		return StatusCode::FAILURE;
	}
	log << MSG::INFO
		<< " create address in CalibMySQLCnvSvc "
		<< endreq;

	refpAddress = new GenericAddress( MYSQL_StorageType, 
			clid, 
			par[0]);

	return StatusCode::SUCCESS;
}

//select sftver and parver frm table CalVtxLum
StatusCode CalibMySQLCnvSvc::getSftParVer(std::string& SftVer,
		std::string& CalParVer,
		int &runfrm,
		int &runto,
		int RunNo,
		std::string BossRelease,
		std::string DataType
		)
{
	using namespace rdbModel;
	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );
	char stmt[300];
	int run_No =RunNo;
	MYSQL_RES *res_set;
	IDatabaseSvc* m_dbsvc;
	DatabaseRecordVector res;
	StatusCode sc=serviceLocator()->service("DatabaseSvc",m_dbsvc,true);
	if (sc .isFailure() ) {
		log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
		return sc;
	}

	const char* bossRelease = BossRelease.c_str();
	const char* dataType = DataType.c_str();

	sprintf(stmt,"select RunFrom,RunTo,SftVer,ParVer from CalVtxLumVer where BossRelease = '%s' and RunFrom <= %d and RunTo >= %d and DataType='%s' ",bossRelease,run_No,run_No,dataType);

	int row_no = m_dbsvc->query("offlinedb",stmt,res);
	if(row_no<1){
		std::cout<<"ERROR:error searching with:"<<stmt<<std::endl;
		exit(1);
	}
	if(row_no=1){
		DatabaseRecord* records1 = res[0];
    runfrm=records1->GetInt("RunFrom");
    runto=records1->GetInt("RunTo");
		//cout<<dataType<<" runfrm,runto in getSftParVer is:"<<runfrm<<"::::"<<runto<<endl;
		SftVer=records1->GetString("SftVer");
		CalParVer=records1->GetString("ParVer");
	}
	if(row_no>1){
		cout<<"ERROR: "<<dataType<<" set overlapped run number in the table CalVtxLumVer"<<endl;
		exit(1);
		}
	return StatusCode::SUCCESS;
}


/// Create a calib DataObject by calib type, flavor, time and instrument.
/// This method does not register DataObject in the transient data store,
/// [but may register TDS addresses for its children if needed (e.g. Catalog).
///   - what's all this about? ]
/// The string storage type is discovered at runtime in the metadata dbs
/// The entry name identifies a condition amongst the many in the string.
/// Implementation:
/// - create a temporary address containing storage type and classID;
/// - dispatch to appropriate conversion service according to storage type;
/// - this will dispatch to appropriate converter according to CLID
///   (CalibMySQLCnvSvc has no converters of its own).

StatusCode CalibMySQLCnvSvc::createCalib(DataObject*&       refpObject,
		const std::string& fullpath,
		const CLID&        classID,
		IRegistry*         entry)
{
	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );

	// Look up calib object in the Metadata database
	std::string cType = CalibData::CalibModelSvc::getCalibType(fullpath);

	// ..and extra special munging for test
	if (std::string("Test") == cType.substr(0, 4)) {
		cType = std::string("Test_Gen");
	}

	std::string testfile = std::string(getenv("CALIBMYSQLCNVROOT"))+"/share/test.root";
	TFile *f1=new TFile(testfile.c_str(),"read");
	unsigned int ser = 0;
	//runfrm,runto are the value from the table ***CalConst;runfrm1,runto1 are the value from the table CalVtxLumVer
	int runfrm,runfrm1;
	int runto,runto1;
	std::string flag="default";
	calibUtil::Metadata::eRet ret;

	MSG::Level msgLevel =  MSG::DEBUG;

	std::string physFmt = "UNK";
	std::string fmtVersion;
	std::string dataIdent;

	// Get the runNo of current event:
	SmartDataPtr<Event::EventHeader> evt(m_eventSvc,"/Event/EventHeader");
	int runNo=0;
	if( !evt ){
		log << MSG::WARNING << "Unable to read the Event for TDS" << endreq;
		//	   return StatusCode::FAILURE;
	}   
	if( evt ){
		if(cType!="TofCal"&&cType!="EstTofCal"&&cType!="TofSim"&&cType!="DedxSim") runNo = fabs(evt -> runNumber());
		if(cType=="TofCal"||cType=="EstTofCal"||cType=="TofSim"||cType=="DedxSim") runNo = evt -> runNumber();
	}
	DatabaseRecordVector res;

	StatusCode st1;

	//TofCalConst
	if((cType=="TofCal")&&(m_bossver[3]== "default" ))
	{
		std::string cType="Tof";
		if(m_bossRelease=="default")
			{
				log << MSG::FATAL << " Boss Release for TofCal not set!" << endreq;
				exit(1);
				
			}
		else
			{
			st1=getSftParVer(m_bossver[3],m_calPar[3],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 is:"<<runfrm1<<"::"<<runto1<<endl;
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1; 					                    
			}
			//cout<<"SftVer and CalParVer are:"<<sft_ver<<";"<<cal_ver<<endl;
			ret = m_meta->getReadTOFInfo(ser,&runfrm,&runto,m_calPar[3],res,runNo,m_bossver[3]);

		}
	}
	else if ((cType=="TofCal")&&(m_bossver[3]!= "default" ))
	{
    m_flag[3]="set";
		flag=m_flag[3];
		//cout<<"flag @create Tof is:"<<m_flag[3]<<endl;
		ret = m_meta->getReadTOFInfo(ser,&runfrm,&runto,m_calPar[3],res,runNo,m_bossver[3]);
	}


	//EmcCalConst
	if((cType=="EmcCal")&&(m_bossver[2]== "default" ))   
	{ 
		std::string cType="Emc";
		if(m_bossRelease=="default") 
		{
			log << MSG::FATAL << " Boss Release for EmcCal not set!" << endreq;
			exit(1);
		}
		else{

			st1=getSftParVer(m_bossver[2],m_calPar[2],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 is:"<<runfrm1<<"::"<<runto1<<endl;
			//cout<<"flag @create Emc is:"<<flag<<endl;
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1; 
			}
			ret = m_meta->getReadEMCInfo(ser,&runfrm,&runto, m_calPar[2], res,runNo,m_bossver[2]);                    
		}
	}
	else if((cType=="EmcCal")&&(m_bossver[2]!= "default" ))
	{
		m_flag[2]="set";
		flag=m_flag[2];
    //cout<<"flag @create Emc is:"<<m_flag[2]<<endl;
		ret = m_meta->getReadEMCInfo(ser,&runfrm,&runto, m_calPar[2], res,runNo,m_bossver[2]);
	}

	//DedxCalConst
	if((cType=="DedxCal")&&(m_bossver[1]== "default" ))
	{ 
		std::string cType="Dedx";
		if(m_bossRelease=="default") 
		{
			log << MSG::FATAL << " Boss Release for DedxCal not set!" << endreq;
			exit(1);										             
		}
		else 		  
		{
			st1=getSftParVer(m_bossver[1],m_calPar[1],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 is:"<<runfrm1<<"::"<<runto1<<endl;
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;	                         
			}
			ret = m_meta->getReadDedxInfo(ser,&runfrm,&runto, m_calPar[1], res,runNo,m_bossver[1]);			
		}	  
	}
	else if((cType=="DedxCal")&&(m_bossver[1]!= "default" ))   
	{
    m_flag[1]="set";
    flag=m_flag[1];
		//cout<<"flag @create Dedx is:"<<m_flag[1]<<endl;
		ret = m_meta->getReadDedxInfo(ser,&runfrm,&runto, m_calPar[1], res,runNo,m_bossver[1]);
	}

	//MdcCalConst
	if((cType=="MdcCal")&&(m_bossver[0]== "default" ))
	{
		std::string cType="Mdc";
		//cout<<"cType is"<<cType<<"m_bossver[0] is:"<<m_bossver[0]<<endl;
		if(m_bossRelease=="default") 
		{
			log << MSG::FATAL << " Boss Release for MdcCal not set!" << endreq;
			exit(1);      
		}
		else 
		{
			st1=getSftParVer(m_bossver[0],m_calPar[0],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 is:"<<runfrm1<<"::"<<runto1<<endl;
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;									       
			}
			ret = m_meta->getReadMDCInfo(ser,&runfrm,&runto,m_calPar[0],res,runNo,m_bossver[0]); 		     	
		}	 
	}
	else if((cType=="MdcCal")&&(m_bossver[0]!= "default"))      
	{
    m_flag[0]="set";
    flag=m_flag[0];
		//cout<<"flag @create Mdc is:"<<m_flag[0]<<endl;
		ret = m_meta->getReadMDCInfo(ser,&runfrm,&runto, m_calPar[0], res,runNo,m_bossver[0]);

	}

	//MucCalConst
	if((cType=="MucCal")&&(m_bossver[4]== "default" ))
	{
		std::string cType="Muc";
		if(m_bossRelease=="default") 
		{
			log << MSG::FATAL << " Boss Release for MucCal not set!" << endreq;
			exit(1);									             
		}
		else 
		{
			st1=getSftParVer(m_bossver[4],m_calPar[4],runfrm1,runto1,runNo,m_bossRelease,cType);
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;																						           
			}
			ret = m_meta->getReadMUCInfo(ser,&runfrm,&runto, m_calPar[4], res,runNo,m_bossver[4]);														         
		}						 
	}
	else if((cType=="MucCal")&&(m_bossver[4]!= "default" ))
	{
    m_flag[4]="set";
    flag=m_flag[4];
		//cout<<"flag @create Muc is:"<<m_flag[4]<<endl;
		ret = m_meta->getReadMUCInfo(ser,&runfrm,&runto, m_calPar[4], res,runNo,m_bossver[4]);						     
	}

	//EsTimeCal
	if((cType=="EsTimeCal")&&(m_bossver[5]== "default" ))
	{
		std::string cType="EsTime";
		if(m_bossRelease=="default") 
		{
			log << MSG::FATAL << " Boss Release for EsTimeCal not set!" << endreq;
			exit(1);														              
		}
		else 
		{
			st1=getSftParVer(m_bossver[5],m_calPar[5],runfrm1,runto1,runNo,m_bossRelease,cType);
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;																													                   
			}
			ret = m_meta->getReadEsTimeInfo(ser,&runfrm,&runto,m_calPar[5], res,runNo,m_bossver[5]);
		}										   
	}
	else if((cType=="EsTimeCal")&&(m_bossver[5]!= "default" ))
	{
    m_flag[5]="set";
    flag=m_flag[5];
		//cout<<"flag @create EsTime is:"<<m_flag[5]<<endl;
		ret = m_meta->getReadEsTimeInfo(ser,&runfrm,&runto, m_calPar[5], res,runNo,m_bossver[5]);								     
	}

	//MdcAlign
	if((cType=="MdcAlign")&&(m_bossver[7]== "default" ))
	{
		std::string cType="MdcAlign";
		if(m_bossRelease=="default") {
			log << MSG::FATAL << " Boss Release for MdcAlignCal not set!" << endreq;
			exit(1);
		}
		else {
			st1=getSftParVer(m_bossver[7],m_calPar[7],runfrm1,runto1,runNo,m_bossRelease,cType);
			if (st1 .isFailure() ) {
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;
			}
			ret = m_meta->getReadMdcAlignInfo(ser,&runfrm,&runto,m_calPar[7], res,runNo,m_bossver[7]);
		}
	}
	else if((cType=="MdcAlign")&&(m_bossver[7]!= "default" ))
	{
    m_flag[7]="set";
    flag=m_flag[7];
		//cout<<"flag @create MdcAlign is:"<<m_flag[7]<<endl;
		ret = m_meta->getReadMdcAlignInfo(ser,&runfrm,&runto, m_calPar[7], res,runNo,m_bossver[7]);
	}

	//TofQElec
	if((cType=="TofQElec")&&(m_bossver[8]== "default" ))
	{
		std::string cType="TofQElec";
		if(m_bossRelease=="default") {
			log << MSG::FATAL << " Boss Release for TofQElecCal not set!" << endreq;
			exit(1);
		}
		else {
			//cout<<"TofQElec @create"<<endl;
			st1=getSftParVer(m_bossver[8],m_calPar[8],runfrm1,runto1,runNo,m_bossRelease,cType);
			if (st1 .isFailure() ) {
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;
			}
			ret = m_meta->getReadTofQElecInfo(ser,&runfrm,&runto,m_calPar[8],res,runNo,m_bossver[8]);
		}
	}
	else if((cType=="TofQElec")&&(m_bossver[8]!= "default" ))
	{
    m_flag[8]="set";
    flag=m_flag[8];
		//cout<<"flag @create TofQElec is:"<<flag<<endl;
		ret = m_meta->getReadTofQElecInfo(ser,&runfrm,&runto, m_calPar[8], res,runNo,m_bossver[8]);
	}

	//TofSim
	if((cType=="TofSim")&&(m_bossver[9]== "default" ))
	{
		std::string cType="TofSim";
		if(m_bossRelease=="default") {
			log << MSG::FATAL << " Boss Release for TofSimCal not set!" << endreq;
			exit(1);
		}
		else {
			st1=getSftParVer(m_bossver[9],m_calPar[9],runfrm1,runto1,runNo,m_bossRelease,cType);
			if (st1 .isFailure() ) {
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;
			}
			ret = m_meta->getReadTofSimInfo(ser,&runfrm,&runto, m_calPar[9], res,runNo,m_bossver[9]);
		}
	}
	else if((cType=="TofSim")&&(m_bossver[9]!= "default" ))
	{
    m_flag[9]="set";
    flag=m_flag[9];
		//cout<<"flag @create TofSim is:"<<m_flag[9]<<endl;
		ret = m_meta->getReadTofSimInfo(ser,&runfrm,&runto, m_calPar[9], res,runNo,m_bossver[9]);
	}

	//DedxSim
	if((cType=="DedxSim")&&(m_bossver[10]== "default" ))
	{
		std::string cType="DedxSim";
		if(m_bossRelease=="default") {
			log << MSG::FATAL << " Boss Release for DedxSimCal not set!" << endreq;
			exit(1);
		}
		else {
			st1=getSftParVer(m_bossver[10],m_calPar[10],runfrm1,runto1,runNo,m_bossRelease,cType);
			if (st1 .isFailure() ) {
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;
			}
			ret = m_meta->getReadDedxSimInfo(ser,&runfrm,&runto,m_calPar[10], res,runNo,m_bossver[10]);
		}
	}
	else if((cType=="DedxSim")&&(m_bossver[10]!= "default" ))
	{
    m_flag[10]="set";
    flag=m_flag[10];
		//cout<<"flag @create DedxSim is:"<<m_flag[10]<<endl;
		ret = m_meta->getReadDedxSimInfo(ser,&runfrm,&runto, m_calPar[10], res,runNo,m_bossver[10]);
	}

	//MdcDataConst
	if((cType=="MdcDataConst")&&(m_bossver[11]== "default" ))
	{
		std::string cType="MdcData";
		if(m_bossRelease=="default") {
			log << MSG::FATAL << " Boss Release for MdcDataConst not set!" << endreq;
			exit(1);
		}
		else {
			st1=getSftParVer(m_bossver[11],m_calPar[11],runfrm1,runto1,runNo,m_bossRelease,cType);
			if (st1 .isFailure() ) {
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;
			}
			ret = m_meta->getReadMdcDataConstInfo(ser,&runfrm,&runto,m_calPar[11], res,runNo,m_bossver[11]);
		}
	}
	else if((cType=="MdcDataConst")&&(m_bossver[11]!= "default" ))
	{
    m_flag[11]="set";
    flag=m_flag[11];
		//cout<<"flag @create MdcData is:"<<m_flag[11]<<endl;
		ret = m_meta->getReadMdcDataConstInfo(ser,&runfrm,&runto, m_calPar[11], res,runNo,m_bossver[11]);
	}

	//EstTofCalConst
	if((cType=="EstTofCal")&&(m_bossver[6]== "default" ))
	{
		std::string cType="EsTof";
		if(m_bossRelease=="default") {
			log << MSG::FATAL << " Boss Release for EstTofCal not set!" << endreq;
			exit(1);
		}
		else {
			st1=getSftParVer(m_bossver[6],m_calPar[6],runfrm1,runto1,runNo,m_bossRelease,cType);
			if (st1 .isFailure() ) {
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;
			}
			ret = m_meta->getReadEstTofInfo(ser,&runfrm,&runto,m_calPar[6],res,runNo,m_bossver[6]);
		}
	}
	else if((cType=="EstTofCal")&&(m_bossver[6]!= "default" ))
	{
    m_flag[6]="set";
    flag=m_flag[6];
		//cout<<"flag @create EstTof is:"<<m_flag[6]<<endl;
		ret = m_meta->getReadEstTofInfo(ser,&runfrm,&runto, m_calPar[6], res,runNo,m_bossver[6]);
	}

	if(ret!=calibUtil::Metadata::RETOk){
		log <<  MSG::ERROR   << "no record in the database" << endreq;
		exit(1);
	}
	int sz=res.size();
	DatabaseRecord* records1 = res[0];
//	DatabaseRecord* records2 = res1[0];
 // runfrm1=records1->GetInt("RunFrom");
 // runto1=records1->GetInt("RunTo");
 // cout<<cType<<" runfrm1,runto1 is:"<<runfrm3<<":::"<<runto3<<endl;
	if(m_dbStatus=="OK"){
		if(std::string((*records1)["Status"])!="OK") {
			log<<MSG::FATAL<<"Status of type "<<cType<<" is "<< (*records1)["Status"]<<"  your setting is OK"<<endreq;
			exit(1);
		}
	}

	log << MSG::DEBUG << "dataIdent is:"<<dataIdent<< endreq;
	unsigned char storageType;
	physFmt="TREE";
	StatusCode sc = decodeDescription(physFmt, storageType);


	// Depending on value of eDataFmt, figure out which private
	// conversion service to invoke.  Pass dataIdent and fmtVersion
	// as part of the opaque address.  
	// Create temporary address for the relevant type and classID 
	log << MSG::DEBUG << "Creating an address of type "
		<< (int)storageType << " for class " << classID << endreq;

	IOpaqueAddress* tmpAddress;
	const std::string par[3] = {dataIdent, fullpath, fmtVersion};
	//const unsigned long ipar[2] = {int(runfrm),int(runto)};
	//log << MSG::DEBUG << "ipar is:"<<ipar[0]<<":"<<ipar[1]<<"ipar[1]"<<endreq;
	const unsigned long ipar[2] = {0,0};//yzhang fix unsigned bug for runfrom runto

	//  sc = addressCreator()->createAddress(storageType, classID,
	//                                       par, ipar, tmpAddress);

	tmpAddress = new TreeAddress(storageType, classID,*records1,ipar);
  //sscanf((*records2)["RunFrom"], "%d", &runfrm1);
  //sscanf((*records2)["RunTo"], "%d", &runto1);
	
	cout<<cType<<" runfrm,runto @CalVtxLumVer is:"<<runfrm1<<":::"<<runto1<<endl;
	sscanf((*records1)["RunFrom"], "%d", &runfrm);
	sscanf((*records1)["RunTo"], "%d", &runto);
  log << MSG::DEBUG << __LINE__<<" records @ runfrm is:"<<(*records1)["RunFrom"]<<" runto is:"<<(*records1)["RunTo"]<<" ser_no is:"<<ser<<endreq;
	//log << MSG::DEBUG <<__LINE__<< " runfrm is:"<<runfrm<<" runto is:"<<runto<<" ser_no is:"<<ser<<endreq;
 if(flag=="default")
 {
	if(runfrm1>runfrm)
	{
		runfrm=runfrm1;
	}
	if(runto1<runto)
	{
		runto=runto1;
	}
 }
 if(flag=="set")
 {
	 flag="default";
 }
	log << MSG::DEBUG <<__LINE__<< " runfrm of max is:"<<runfrm<<" runto min is:"<<runto<<endreq;
	TreeAddress* treeAddress =  dynamic_cast <TreeAddress*> (tmpAddress);
	treeAddress->setRunFrom(runfrm);
	treeAddress->setRunTo(runto);
//	log << MSG::DEBUG << __LINE__<<" records @ runfrm is:"<<(*records1)["RunFrom"]<<" runto is:"<<(*records1)["RunTo"]<<" ser_no is:"<<ser<<endreq;
//	log << MSG::DEBUG <<__LINE__<< " runfrm is:"<<runfrm<<" runto is:"<<runto<<" ser_no is:"<<ser<<endreq;
	/*
		 if ( !sc.isSuccess() ) {
		 log << msgLevel  << "Persistency service could not create a new address" << endreq;
		 if (m_crash) {
		 log << msgLevel << std::endl << "Exiting... " << std::endl << endreq;
		 exit(1);
		 }
		 return sc;
		 }
		 */
	tmpAddress->addRef();

	// Set the transient store registry for the object associated to this address
	tmpAddress->setRegistry(entry);

	// Now create the object
	sc = m_detPersSvc->createObj(tmpAddress, refpObject);
	tmpAddress->release();
	if ( !sc.isSuccess() ) {
		log << msgLevel
			<< "Persistency service could not create a new object" << endreq;
		if (m_crash) {
			log << msgLevel << std::endl << "Exiting... " << std::endl << endreq;
			exit(1);
		}
		return sc;
	}
	//  std::cout<<" CalibMySQLCnvSvc  res.size()=="<<res.size()<<std::endl;
	res.clear();
	log << MSG::DEBUG << "New object successfully created" << endreq;
	return StatusCode::SUCCESS;
	f1->Close();
	delete f1;
}


/// Update a calib DataObject by calib type, flavor,and instrument
/// if necessary.
/// This method does not register DataObject in the transient data store,
/// The string storage type is discovered at runtime in the MySQL dbs.
/// Implementation:
/// - create a temporary address containing storage type and classID;
/// - dispatch to appropriate conversion service according to storage type;
/// - this will dispatch to appropriate converter according to CLID
///   (the CalibMySQLCnvSvc has no converters of its own).

StatusCode CalibMySQLCnvSvc::updateCalib( DataObject*        pObject,
		const std::string& fullpath,
		const CLID&        classID,
		IRegistry*         entry )
{
	using CalibData::CalibBase;

	MsgStream log(msgSvc(), "CalibMySQLCnvSvc" );
	StatusCode status;

	std::string testfile = std::string(getenv("CALIBMYSQLCNVROOT"))+"/share/test.root";
	TFile *f1=new TFile(testfile.c_str(),"read");


	// Look up calib object in the Metadata database
	//std::string flavor = CalibData::CalibModelSvc::getFlavor(fullpath);
	std::string cType = CalibData::CalibModelSvc::getCalibType(fullpath);

	// ..and extra special munging for test
	if (std::string("Test") == cType.substr(0, 4)) {
		cType = std::string("Test_Gen");
	}

	if (0 == pObject) {
		log << MSG::ERROR << "There is no DataObject to update" << endreq;
		return StatusCode::FAILURE;
	}
	// Is object an instance of the specified class?
	if(pObject->clID()!=6411&& classID!=6411){
		if ( classID != pObject->clID() ) {
			log << MSG::ERROR << "Update requested for clID " << classID
				<< " while DataObject is of clID " 
				<< pObject->clID() << endreq;
			exit(1);
			return StatusCode::FAILURE;
		}
	}

	// check if already valid.  If so, we're done.
	//  Need access to IValidity interface
	CalibBase1* pBase = dynamic_cast<CalibBase1*>(pObject);
	if (pBase == 0) {
		log << MSG::WARNING
			<< "Object to be updated is not a calib object! " << endreq;
		return StatusCode::FAILURE;
	}

	// Following comes from createCalib.  Perhaps create and update
	// should be calling common utility since much of what they do is identical.
	unsigned int ser;
	int runfrm,runfrm1;
	int runto,runto1;
	calibUtil::Metadata::eRet ret;

	//  calibUtil::Metadata::eDataFmt physFmt = calibUtil::Metadata::FMTUnknown;
	std::string physFmt;
	std::string fmtVersion;
	std::string dataIdent;
	std::string flag="default";
	// Get the information needed to find and interpret the bulk data:
	//   * physical storage type
	//   * version of the physical format
	//   * pathname or similar identifying information so the data can be found
	//   maqm comment remove fmtversion :
	//    ret = m_meta->getReadInfo(ser, physFmt, fmtVersion, dataIdent);

	SmartDataPtr<Event::EventHeader> evt(m_eventSvc,"/Event/EventHeader");
	int runNo=0;
	if( !evt ){
		log << MSG::WARNING << "Unable to read the Event for TDS" << endreq;
		//     return StatusCode::FAILURE;
	}

	//  eRet ret;

	DatabaseRecordVector res;
	StatusCode st1;
	if( evt ){
		if(cType!="TofCal"&&cType!="EstTofCal"&&cType!="TofSim"&&cType!="DedxSim") runNo = fabs(evt -> runNumber());
		if(cType=="TofCal"||cType=="EstTofCal"||cType=="TofSim"||cType=="DedxSim") runNo = evt -> runNumber();
	}

	//TofCalConst
	if((cType=="TofCal"))
	{
		std::string cType="Tof";
		if(m_flag[3]=="default")
		{
			st1=getSftParVer(m_bossver[3],m_calPar[3],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1; 					                    
			}
		}
	
		flag=m_flag[3];

    //cout<<"flag @update Tof is:"<<flag<<endl;
			//cout<<"SftVer and CalParVer are:"<<sft_ver<<";"<<cal_ver<<endl;
		ret = m_meta->getReadTOFInfo(ser,&runfrm,&runto,m_calPar[3],res,runNo,m_bossver[3]);
	}

	//EmcCalConst
	if((cType=="EmcCal"))   
	{ 
		std::string cType="Emc";
		if(m_flag[2]=="default")
		{
			st1=getSftParVer(m_bossver[2],m_calPar[2],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;                                
			}
		}
		flag=m_flag[2];
		//cout<<"flag @update Emc is:"<<m_flag[2]<<endl;
		ret = m_meta->getReadEMCInfo(ser,&runfrm,&runto, m_calPar[2], res,runNo,m_bossver[2]);                    
	}



	//DedxCalConst
	if((cType=="DedxCal"))
	{ 
		std::string cType="Dedx";
		if(m_flag[1]=="default")
		{
			st1=getSftParVer(m_bossver[1],m_calPar[1],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;	                         
			}
		}
		flag=m_flag[1];
		//cout<<"flag @update Dedx  is:"<<m_flag[1]<<endl;
		ret = m_meta->getReadDedxInfo(ser,&runfrm,&runto, m_calPar[1], res,runNo,m_bossver[1]);			
	}	  

	//MdcCalConst
	if((cType=="MdcCal"))
	{
		std::string cType="Mdc";
		if(m_flag[0]=="default")
		{
			//cout<<"cType is"<<cType<<"m_bossver[0] is:"<<m_bossver[0]<<endl;
			st1=getSftParVer(m_bossver[0],m_calPar[0],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;									       
			}
		}
		flag=m_flag[0];
		//cout<<"flag @update Mdc is:"<<m_flag[0]<<endl;
		ret = m_meta->getReadMDCInfo(ser,&runfrm,&runto,m_calPar[0],res,runNo,m_bossver[0]); 		     	
	}	 

	//MucCalConst
	if((cType=="MucCal"))
	{
		std::string cType="Muc";
		if(m_flag[4]=="default")
		{
			st1=getSftParVer(m_bossver[4],m_calPar[4],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;																						           
			}
		}
		flag=m_flag[4];
		//cout<<"flag @update Muc is:"<<m_flag[4]<<endl;
		ret = m_meta->getReadMUCInfo(ser,&runfrm,&runto, m_calPar[4], res,runNo,m_bossver[4]);														         
	}						 

	//EsTimeCal
	if((cType=="EsTimeCal"))
	{
		std::string cType="EsTime";
		if(m_flag[5]=="default")
		{
			st1=getSftParVer(m_bossver[5],m_calPar[5],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
			if (st1 .isFailure() ) 
			{
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;																													                   
			}
		}
		flag=m_flag[5];
		//cout<<"flag @update EsTime is:"<<m_flag[5]<<endl;
		ret = m_meta->getReadEsTimeInfo(ser,&runfrm,&runto,m_calPar[5], res,runNo,m_bossver[5]);
	}										   

	//MdcAlign
	if((cType=="MdcAlign"))
	{
		std::string cType="MdcAlign";
		if(m_flag[7]=="default")
		{
			st1=getSftParVer(m_bossver[7],m_calPar[7],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
			if (st1 .isFailure() ) {
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;
			}
		}
		flag=m_flag[7];
		//cout<<"flag @update MdcAlign is:"<<m_flag[7]<<endl;
		ret = m_meta->getReadMdcAlignInfo(ser,&runfrm,&runto,m_calPar[7], res,runNo,m_bossver[7]);
	}

	//TofQElec
	if((cType=="TofQElec"))
	{
		std::string cType="TofQElec";
		if(m_flag[8]=="default")
		{
			//cout<<"TofQElec @update"<<endl;
			st1=getSftParVer(m_bossver[8],m_calPar[8],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
		}
		flag=m_flag[8];
		//cout<<"flag @update TofQElec is:"<<m_flag[8]<<endl;
		ret = m_meta->getReadTofQElecInfo(ser,&runfrm,&runto,m_calPar[8],res,runNo,m_bossver[8]);
	}

	//TofSim
	if((cType=="TofSim"))
	{
		std::string cType="TofSim";
		if(m_flag[9]=="default")
		{
			st1=getSftParVer(m_bossver[9],m_calPar[9],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
			if (st1 .isFailure() ) {
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;
			}
		}
		flag=m_flag[9];
		//cout<<"flag @update TofSim is:"<<m_flag[9]<<endl;
		ret = m_meta->getReadTofSimInfo(ser,&runfrm,&runto, m_calPar[9], res,runNo,m_bossver[9]);
	}

	//DedxSim
	if((cType=="DedxSim"))
	{
		std::string cType="DedxSim";
		if(m_flag[10]=="default")
		{
			st1=getSftParVer(m_bossver[10],m_calPar[10],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
			if (st1 .isFailure() ) {
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;
			}
		}
		flag=m_flag[10];
		//cout<<"flag @update DedxSim is:"<<m_flag[10]<<endl;
		ret = m_meta->getReadDedxSimInfo(ser,&runfrm,&runto,m_calPar[10], res,runNo,m_bossver[10]);
	}

	//MdcDataConst
	if((cType=="MdcDataConst"))
	{
		std::string cType="MdcData";
		if(m_flag[11]=="default")
		{
			st1=getSftParVer(m_bossver[11],m_calPar[11],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
			if (st1 .isFailure() ) {
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;
			}
		}
		flag=m_flag[11];
		//cout<<"flag @update MdcData is:"<<m_flag[11]<<endl;
		ret = m_meta->getReadMdcDataConstInfo(ser,&runfrm,&runto,m_calPar[11], res,runNo,m_bossver[11]);
	}

	//EstTofCalConst
	if((cType=="EstTofCal"))
	{
		std::string cType="EsTof";
		if(m_flag[6]=="default")
		{
			st1=getSftParVer(m_bossver[6],m_calPar[6],runfrm1,runto1,runNo,m_bossRelease,cType);
			//cout<<cType<<" runfrm1,runto1 @update is:"<<runfrm1<<":::"<<runto1<<endl;
			if (st1 .isFailure() ) {
				log << MSG::ERROR << " Unable to find DatabaseSvc " << endreq;
				return st1;
			}
		}
		flag=m_flag[6];
		//cout<<"flag @update EstTof is:"<<m_flag[6]<<endl;
		ret = m_meta->getReadEstTofInfo(ser,&runfrm,&runto,m_calPar[6],res,runNo,m_bossver[6]);
	}

	if(ret!=calibUtil::Metadata::RETOk){
		log <<  MSG::ERROR   << "Error searching in the database" << endreq;
		exit(1);
	}

	physFmt="TREE";
	unsigned char storageType;
	status = decodeDescription(physFmt, storageType);

	// Depending on value of eDataFmt, figure out which private
	// conversion service to invoke.  Pass dataIdent and fmtVersion
	// as part of the opaque address.  
	// Create temporary address for the relevant type and classID 
	log << MSG::DEBUG << "Creating an address of type " 
		<< (int)storageType << " for class " << classID << endreq;

	//   int sz=res.size();
	DatabaseRecord* records1 = res[0];

	if(m_dbStatus=="OK"){
		if(std::string((*records1)["Status"])!="OK") {
			log<<MSG::FATAL<<"the status of type  "<<cType<<" is "<< (*records1)["Status"]<<"  your setting is OK"<<endreq;
			exit(1);
		}
	}


	IOpaqueAddress* tmpAddress;
	//const unsigned long ipar[2] = {runfrm,runto};
	const unsigned long ipar[2] = {0,0};//yzhang fix unsigned bug for runfrom runto

	tmpAddress = new TreeAddress(storageType, classID,*records1,ipar);
  log << MSG::DEBUG <<__LINE__<<cType<<" runfrm,runto @CalVtxLumVer/update  is:"<<runfrm1<<":::"<<runto1<<endreq;
	sscanf((*records1)["RunFrom"], "%d", &runfrm);
	sscanf((*records1)["RunTo"], "%d", &runto);
	log << MSG::DEBUG << __LINE__<<" records @update runfrm is:"<<(*records1)["RunFrom"]<<" runto is:"<<(*records1)["RunTo"]<<" ser_no is:"<<ser<<endreq;
	if(flag=="default")
	{
		if(runfrm1>runfrm)
		  {
		    runfrm=runfrm1;
			}
    if(runto1<runto)
      {
       runto=runto1;
      } 
	}
	if(flag=="set")
	{
		flag=="default";
	}

 log << MSG::DEBUG <<__LINE__<< " runfrm of max is:"<<runfrm<<" runto min is:"<<runto<<endreq;
	TreeAddress* treeAddress =  dynamic_cast <TreeAddress*> (tmpAddress);
	treeAddress->setRunFrom(runfrm);
	treeAddress->setRunTo(runto);
	//log << MSG::DEBUG << __LINE__<<" records runfrm is:"<<(*records1)["RunFrom"]<<" runto is:"<<(*records1)["RunTo"]<<" ser_no is:"<<ser<<endreq;
	//log << MSG::DEBUG <<__LINE__<< " runfrm is:"<<runfrm<<" runto is:"<<runto<<" ser_no is:"<<ser<<endreq;

	log << MSG::DEBUG << "Temporary address successfully created" << endreq;
	tmpAddress->addRef();

	// Set the transient store registry for the object associated to this address
	tmpAddress->setRegistry(entry);

	// Now update the object
	DataObject* pNewObject;
	status = m_detPersSvc->createObj(tmpAddress, pNewObject);
	tmpAddress->release();
	if ( !status.isSuccess() ) {
		log << MSG::ERROR 
			<< "Persistency service could not create object" << endreq;
		return status;
	}

	// Since DataObject::operator= operator is not virtual, dynamic cast first!
	// Overloaded virtual method Condition::update() must be properly defined!
	// The memory pointed to by the old pointer must contain the new object    
	// Note this dynamic cast only gets us part-way.  The object is
	// actually of some type derived from CalibBase.
	CalibBase1* pNewBase = dynamic_cast<CalibBase1*>(pNewObject);
	if  (0 == pNewBase) {
		log << MSG::ERROR
			<< "Cannot update objects other than Calib objects: " 
			<< "update() must be defined!"
			<< endreq;
		return StatusCode::FAILURE;
	}

	// Deep copy the new calib into the old DataObject.  To copy the *whole*
	// object, not just the CalibBase part, classes derived from CalibBase
	// must override update method.  
	// NOTE:  classes directly derived from CalibBase must call
	//        CalibBase::update in their own update method.
	pBase->update(*pNewBase, &log);  
	res.clear();
	f1->Close();
	delete f1;
	delete pNewBase;
	return StatusCode::SUCCESS;
}

StatusCode  CalibMySQLCnvSvc::decodeDescription(const std::string& description,
		unsigned char& type )
{
	MsgStream log(msgSvc(), "CalibMySQLCnvSvc");

	//   description ="ROOT";
	if (description == std::string("XML")) {
		type = XML_StorageType;
	}
	else if (description == std::string("ROOT")) {
		type = CALIBROOT_StorageType;
	}
	else if (description == std::string("TREE")) {
		type = CALIBTREE_StorageType;
	}
	else {       // unsupported
		log << MSG::ERROR << "unsupported storage type " << description << endreq;
		return StatusCode::FAILURE;
	}
	return StatusCode::SUCCESS;
}


/// Handle to the MySQL metadata database
calibUtil::Metadata* CalibMySQLCnvSvc::getMeta( ) {
	return m_meta;
}



