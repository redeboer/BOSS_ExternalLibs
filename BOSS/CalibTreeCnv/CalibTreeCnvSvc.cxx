
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IConverter.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GenericAddress.h"
#include "CalibTreeCnv/CalibTreeCnvSvc.h"
#include "CalibData/CalibBase.h"
#include "cnv/TreeCalBaseCnv.h"
#include "CalibData/CalibBase1.h"
#include "CalibDataSvc/IInstrumentName.h"
// Make instances only via static factory class
//static SvcFactory<CalibTreeCnvSvc> calibRootCnvSvc_factory;
//const ISvcFactory& CalibTreeCnvSvcFactory = calibRootCnvSvc_factory;

CalibTreeCnvSvc::CalibTreeCnvSvc(const std::string& name, 
                               ISvcLocator* svc) :
  ConversionSvc(name, svc, CALIBTREE_StorageType),
  m_detPersSvc(0), m_detDataSvc(0)   {
  //huangb add
}

StatusCode CalibTreeCnvSvc::queryInterface(const InterfaceID& riid,
                                          void** ppvInterface) {
  /* Uncomment if choose to derive from abstract root conv. interface */
  if (IID_ICalibTreeSvc.versionMatch(riid))  {
    *ppvInterface = (ICalibTreeSvc*)this;
  }
  else {
    // Interface is not directly availible: try out a base class
    return ConversionSvc::queryInterface(riid, ppvInterface);
    /*  }  */
  addRef();
  }
  return StatusCode::SUCCESS;
}

StatusCode CalibTreeCnvSvc::initialize() {
  StatusCode sc = ConversionSvc::initialize();

  MsgStream log(msgSvc(), "CalibTreeCnvSvc");

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
  /*sc = m_detPersSvc->queryInterface(IID_IAddressCreator, 
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
      << "Set it as the address creator of the CalibTreeCnvSvc" << endreq;
  sc = setAddressCreator(iAddrCreator);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 	<< "Cannot set the address creator" << endreq;
    return sc;
  }

  // set properties if there are any??

  return sc;
}

StatusCode CalibTreeCnvSvc::finalize() {
  // If anything was allocated, get rid of it.  So far, nothing was.

  return ConversionSvc::finalize();
}

  StatusCode CalibTreeCnvSvc::createAddress(long svc_type,
					    const CLID& clid,
					    const std::string* par, 
					    const unsigned long* ip,
					    IOpaqueAddress*& refpAddress) {
 //createAddress of   CalibTreeCnvSvc
    MsgStream log( msgSvc(), name() );
    if (svc_type != CALIBTREE_StorageType) {
    log << MSG::ERROR << "bad storage type" << (int)svc_type << endreq;
    return StatusCode::FAILURE;
  }
    std::string dataIdent;
    std::string fullpath;
    int index ;
    if(clid==CLID_Calib_MdcCal) 
      { index=0;}
    else if(clid==CLID_Calib_TofCal)
      {index =1;}
    else if(clid==CLID_Calib_DedxCal)
      {index =2;}
    else if(clid==CLID_Calib_EmcCal)
      {index =3;}
    else{ 
      log << MSG::WARNING<<"Wrong CLID"<<endreq;
    }
    
  //see if svctype set in the CalibDataSvc is MYSQL_StorageType or  CALIBROOT_StorageType
/*  IInstrumentName* iInstrumentName;
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
	dataIdent = m_rootfile[index];
	fullpath = par[0];
      }

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
  // log << MSG::INFO<<"dataIdent is:"<<dataIdent<<endreq;
   //std::string fullpath(par[1]);  // path within TCDS for the object
 //  log << MSG::INFO<<"fullpath is :"<<fullpath<<endreq;
 
 
   int runfrm = ip[0];
   int runto = ip[1];
   dataIdent = par[0];
   fullpath = par[1];

   // for now have to ignore fmtVersion because of defective implementation
  // of GenericAddress. If we want it, should probably write new
  // opaque address implementation for this package to use.  All
  // dealings with (calibration) opaque addresses are confined to
  // the CalibSvc package.
  refpAddress = new GenericAddress(CALIBTREE_StorageType,
                                   clid,
                                   dataIdent,  
                                   fullpath,
                                   runfrm,
                                   runto);
                                   
                               
  return StatusCode::SUCCESS; 

}


StatusCode CalibTreeCnvSvc::updateObj(IOpaqueAddress* pAddress, 
                                       DataObject*     pObject  ) {

  //  using facilities::Timestamp;

   MsgStream log(msgSvc(), "CalibTreeCnvSvc" );
   
   // Don't update when we're using enter time
   log << MSG::DEBUG << "CalibTreeCnvSvc::updateObj starting ...."<<endreq;
}
