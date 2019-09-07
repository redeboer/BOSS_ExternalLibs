// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/CalibRootCnvSvc.cxx,v 1.21 2011/02/17 05:12:53 maqm Exp $

#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IConverter.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GenericAddress.h"
#include "CalibROOTCnv/CalibRootCnvSvc.h"
#include "CalibData/CalibBase.h"
#include "cnv/RootCalBaseCnv.h"
#include "CalibData/CalibBase1.h"
#include "CalibDataSvc/IInstrumentName.h"

// Make instances only via static factory class
//static SvcFactory<CalibRootCnvSvc> calibRootCnvSvc_factory;
//const ISvcFactory& CalibRootCnvSvcFactory = calibRootCnvSvc_factory;

CalibRootCnvSvc::CalibRootCnvSvc(const std::string& name, 
                               ISvcLocator* svc) :
  ConversionSvc(name, svc, CALIBROOT_StorageType),
  m_detPersSvc(0), m_detDataSvc(0)   {
  //huangb add
  declareProperty("Mdcrootfile",m_rootfile[0]= std::string("no rootfile"));
  declareProperty("Tofrootfile",m_rootfile[1]= std::string("no rootfile"));
  declareProperty("Dedxrootfile",m_rootfile[2]= std::string("no rootfile"));
  declareProperty("Emcrootfile",m_rootfile[3]= std::string("no rootfile"));
  declareProperty("Mucrootfile",m_rootfile[4]= std::string("no rootfile"));
  declareProperty("EsTimerootfile",m_rootfile[5]= std::string("no rootfile"));
  declareProperty("EstTofrootfile",m_rootfile[6]= std::string("no rootfile"));
  declareProperty("TofQElecrootfile",m_rootfile[7]= std::string("no rootfile"));
  declareProperty("TofSimrootfile",m_rootfile[8]= std::string("no rootfile"));
  declareProperty("DedxSimrootfile",m_rootfile[9]= std::string("no rootfile"));
  // Some day might have a property to declare having to do with path to
  // xml files.
}

StatusCode CalibRootCnvSvc::queryInterface(const InterfaceID& riid,
                                          void** ppvInterface) {
  /* Uncomment if choose to derive from abstract root conv. interface */
  if (IID_ICalibRootSvc.versionMatch(riid))  {
    *ppvInterface = (ICalibRootSvc*)this;
  }
  else {
    // Interface is not directly availible: try out a base class
    return ConversionSvc::queryInterface(riid, ppvInterface);
    /*  }  */
  addRef();
  }
  return StatusCode::SUCCESS;
}

StatusCode CalibRootCnvSvc::initialize() {
  StatusCode sc = ConversionSvc::initialize();

  MsgStream log(msgSvc(), "CalibRootCnvSvc");

  if (!sc.isSuccess()) return sc;

  // Locate the Calib Data Service.  Since it inherits from DataSvc
  // it has to implement IDataProviderSvc
  m_detDataSvc = 0;
  /*sc = serviceLocator()->getService 
    ("CalibDataSvc",  IID_IDataProviderSvc,(IInterface*&) m_detDataSvc);*/
  sc = serviceLocator()->getService 
    ("CalibDataSvc",  IDataProviderSvc::interfaceID(),(IInterface*&) m_detDataSvc);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not locate CalibDataSvc" << endreq;
    return sc;
  }

 /*  IInstrumentName* iInstrumentName;
  sc = m_detDataSvc->queryInterface(IID_IInstrumentName,
                                    (void**) &iInstrumentName);
*/
  // Set the CalibDataSvc as data provider service
  sc = setDataProvider(m_detDataSvc);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not set data provider" << endreq;
    return sc;
  }
  //huangb add 
 sc = setProperties();

  // Locate IConversionSvc interface of the DetectorPersistencySvc
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
  
  // Query the IAddressCreator interface of the detector persistency service
  IAddressCreator* iAddrCreator;
/*  sc = m_detPersSvc->queryInterface(IID_IAddressCreator, 

				    (void**) &iAddrCreator);*/

  sc = m_detPersSvc->queryInterface(IAddressCreator::interfaceID(), 
                                    (void**) &iAddrCreator);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Cannot query IAddressCreator interface of DetectorPersistencySvc" 
	<< endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Retrieved IAddressCreator interface of DetectorPersistencySvc" 
	<< endreq;
  }
  log << MSG::DEBUG 
      << "Set it as the address creator of the CalibRootCnvSvc" << endreq;
  sc = setAddressCreator(iAddrCreator);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 	<< "Cannot set the address creator" << endreq;
    return sc;
  }

  // set properties if there are any??

  return sc;
}

StatusCode CalibRootCnvSvc::finalize() {
  // If anything was allocated, get rid of it.  So far, nothing was.

  return ConversionSvc::finalize();
}

//StatusCode CalibRootCnvSvc::createAddress(unsigned char svc_type,
  StatusCode CalibRootCnvSvc::createAddress(long svc_type,
					    const CLID& clid,
					    const std::string* par, 
					    const unsigned long* ip,
					    IOpaqueAddress*& refpAddress) {
  MsgStream log( msgSvc(), name() );
   log << MSG::DEBUG<<"here is the createAddress in the CalibRootCnvSvc"<<endreq;
   if (svc_type != CALIBROOT_StorageType) {
    log << MSG::ERROR << "bad storage type" << (int)svc_type << endreq;
    return StatusCode::FAILURE;
  }
   std::cout<<"clid=="<<clid<<std::endl;
    std::string dataIdent;
    std::string fullpath;
    int index=-99;
    if(clid==6412) return StatusCode::SUCCESS;
    if(clid==CLID_Calib_MdcCal) 
      { index=0;}
    else if(clid==CLID_Calib_TofCal)
      {index =1;}
    else if(clid==CLID_Calib_DedxCal)
      {index =2;}
    else if(clid==CLID_Calib_EmcCal)
      {index =3;}
    else if(clid==CLID_Calib_MucCal)
      {index =4;}
    else if(clid==CLID_Calib_EsTimeCal)   
       {index =5;} 
    else if(clid==CLID_Calib_EstTofCal)
      {index =6;}
     else if(clid==CLID_TofQ_Elec)
      {index =7;}
     else if(clid==CLID_Calib_TofSim)
      {index =8;}
     else if(clid==CLID_Dedx_Sim)
      {index =9;}

    else{ 
      log << MSG::WARNING<<"Wrong CLID"<<endreq;
    }
    std::cout<<"index=="<<index<<std::endl;
  //see if svctype set in the CalibDataSvc is MYSQL_StorageType or  CALIBROOT_StorageType
 /* IInstrumentName* iInstrumentName;
  StatusCode sc = m_detDataSvc->queryInterface(IID_IInstrumentName,
                                    (void**) &iInstrumentName);

  if ( !sc.isSuccess() ) {
    log << MSG::ERROR
        << "Cannot query IInstrumentName interface of CalibDataSvc"
        << endreq;
    return sc;
  } else {
    log << MSG::DEBUG
        << "Retrieved IInstrumentName interface of CalibDataSvc"
        << endreq;
  }

   int svctype = iInstrumentName->getsvctype();
    if(svctype== CALIBROOT_StorageType)
      {
*/ 
//	dataIdent = m_rootfile[index];
if(m_rootfile[index]=="no rootfile")
{
log << MSG::INFO<<"no sepcified calibration file path of type "<< index <<endreq;
return StatusCode::FAILURE;
}
	decodeDescription(m_rootfile[index],dataIdent);
	fullpath = par[0];
  /*    }

   if(svctype== MYSQL_StorageType)
      {   log << MSG::INFO<<"rootfile is not set in the jobOption,get it from MySQL"<<endreq;
      dataIdent = par[0];
      fullpath = par[1];
      }

   if ((svctype != CALIBROOT_StorageType)&&(svctype!= MYSQL_StorageType)) {
    log << MSG::ERROR << "bad storage type" << (int)svctype << endreq;
    return StatusCode::FAILURE;
  }
*/

   // std::string dataIdent(par[0]); // file identifier for PDS version of data
   log << MSG::INFO<<"dataIdent is:"<<dataIdent<<endreq;
   //std::string fullpath(par[1]);  // path within TCDS for the object
   log << MSG::INFO<<"fullpath is :"<<fullpath<<endreq;
 
 
   int runfrm = ip[0];
   int runto = ip[1];
  
   // for now have to ignore fmtVersion because of defective implementation
  // of GenericAddress. If we want it, should probably write new
  // opaque address implementation for this package to use.  All
  // dealings with (calibration) opaque addresses are confined to
  // the CalibSvc package.
  refpAddress = new GenericAddress(CALIBROOT_StorageType,
                                   clid,
                                   dataIdent,  
                                   fullpath,
                                   runfrm,
                                   runto);
                                   
  return StatusCode::SUCCESS; 

}

StatusCode CalibRootCnvSvc::writeToRoot(const std::string& outfile, 
                                        const std::string& tdsPath) {
  MsgStream log( msgSvc(), name() );

  // Find corresponding object
  DataObject* pObj;
  m_detDataSvc->findObject(tdsPath, pObj);
  if (!pObj) {
    log << "No object in TDS with path " << tdsPath << endreq;
    return StatusCode::FAILURE;
  }

  CalibData::CalibBase1* pCalib = 
    dynamic_cast<CalibData::CalibBase1*> (pObj);

  if (!pCalib) {
    log << "Object with path " << tdsPath << " not of proper type" << endreq;
    return StatusCode::FAILURE;
  }
  return writeToRoot(outfile, pCalib);
}
StatusCode CalibRootCnvSvc::writeToRoot(const std::string& outfile,
                                        CalibData::CalibBase1* pCalib) {
  MsgStream log(msgSvc(), name() );
 // Find converter corresponding to this object
  IConverter* converter = ConversionSvc::converter(pCalib->clID());
  if (!converter) {
    log << "No converter found for object with CLID  " << pCalib->clID()
        << endreq;
    return StatusCode::FAILURE;
  }
  RootCalBaseCnv* pCnv = dynamic_cast<RootCalBaseCnv*>(converter);
  if (!pCnv) {
    log << "Converter for CLID " << pCalib->clID() <<  " not of proper type" 
        << endreq;
    return StatusCode::FAILURE;
  }
  // Call its createRoot method
  return pCnv->createRoot(outfile, pCalib);
}

StatusCode CalibRootCnvSvc::updateObj(IOpaqueAddress* pAddress, 
                                       DataObject*     pObject  ) {

  //  using facilities::Timestamp;

   MsgStream log(msgSvc(), "CalibRootCnvSvc" );
   
   // Don't update when we're using enter time
   log << MSG::DEBUG << "CalibRootCnvSvc::updateObj starting ...."<<endreq;
   /*
   if (!m_useEventTime) 
     {  log << MSG::DEBUG << "Method updateObj needn't update" << endreq;
	return StatusCode::SUCCESS;
     }
   */
}
 
StatusCode  CalibRootCnvSvc::decodeDescription(const std::string& oldpath,
                                               std::string& realpath )
{
  MsgStream log(msgSvc(), "CalibMySQLCnvSvc");
 
   if(oldpath[0]!='$'){
     realpath=oldpath;
   }
  if(oldpath[0]=='$'){
    std::string::size_type idx =oldpath.find("/");
     if( std::string::npos != idx )
   {  std::string relpath;
      for(int i=1;i<idx-1;i++){
	relpath[i-1] = oldpath[i];
      }
      std::string otherpath;
      for(int i=idx-1;i<oldpath.size();i++){
        otherpath[i-idx+1]=oldpath[i];
      }
    realpath =  std::string(getenv(relpath.c_str()))+otherpath;
    }
   if( std::string::npos == idx ){
     std::string relpath;
      for(int i=1;i<oldpath.size();i++){
	relpath[i-1] = oldpath[i];
      }
     realpath = std::string(getenv(relpath.c_str()));
   }
   }

  return StatusCode::SUCCESS;
}


