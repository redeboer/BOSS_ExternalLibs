// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibDataSvc/src/CalibDataSvc.cxx,v 1.27 2013/05/02 06:25:03 maqm Exp $

// Include files
#include "CalibDataSvc/CalibDataSvc.h"
#include "CalibDataSvc/CalibCLIDNode.h"
//#include "CalibData/CalibTime.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
//#include "GaudiKernel/TimePoint.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "CalibDataSvc/ICalibRootSvc.h" //maqm add
#include "CalibData/CalibModelSvc.h"
#include "CalibData/CalibBase1.h"
#include "CalibData/Mdc/MdcCalStruct.h"
#include "GaudiKernel/RegistryEntry.h"//huangb add
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"
//#include "EventModel/EventList.h"

//#include "CalibData/Mdc/Mdct0par.h"
#define CAST_REGENTRY(x,y) dynamic_cast<x>(y)
//#define CAST_REGENTRY(x,y) (x)(y)
typedef DataSvcHelpers::RegistryEntry RegEntry;

using namespace CalibData;
// Instantiation of a static factory class used by clients to create
// instances of this service
//static SvcFactory<CalibDataSvc> s_factory;
//const ISvcFactory& CalibDataSvcFactory = s_factory;

/// Standard Constructor
CalibDataSvc::CalibDataSvc(const std::string& name,ISvcLocator* svc) :
  DataSvc(name,svc)
 {
     
  // declare a property which is a list of known calibrations.
  // Have default list in one of the standard options files.  
  // User can add others.
  declareProperty("CalibNameList", m_calibList);
  declareProperty("CalibFlavorList", m_flavorList);
  declareProperty("CalibRootName",   m_calibRootName  = "Calib" ); 
  declareProperty("Mdc_CalibStorageType",
                     m_calibType[0] = CALIBROOT_StorageType );
  declareProperty("Tof_CalibStorageType",
                     m_calibType[1] = CALIBROOT_StorageType );
  declareProperty("Dedx_CalibStorageType",
                     m_calibType[2] = CALIBROOT_StorageType );
  declareProperty("EMC_CalibStorageType",
                     m_calibType[3] = CALIBROOT_StorageType ); 
  declareProperty("MUC_CalibStorageType",
                     m_calibType[4] = CALIBROOT_StorageType );
  declareProperty("EsTime_CalibStorageType",
                     m_calibType[5] = CALIBROOT_StorageType );
  declareProperty("EstTof_CalibStorageType",
                     m_calibType[6] = CALIBROOT_StorageType );
  declareProperty("TofQElec_CalibStorageType",
                     m_calibType[8] = CALIBROOT_StorageType );
  declareProperty("TofSim_CalibStorageType",
                     m_calibType[9] = CALIBROOT_StorageType );
  declareProperty("DedxSim_CalibStorageType",
                     m_calibType[10] = CALIBROOT_StorageType );

  //MdcAlign
  m_calibType[7]=14;
  //MdcDataConst
  m_calibType[11]=14;

  // m_rootName and m_rootCLID are declared in base class DataSvc
  m_rootName = "/" + m_calibRootName;
  m_rootCLID = CLID_DataObject;  

 

}

/// Standard Destructor
CalibDataSvc::~CalibDataSvc()  {
  setDataLoader(0);
  clearStore();
}

// Service initialization
StatusCode CalibDataSvc::initialize()   {

  StatusCode sc;

  sc  = DataSvc::initialize();     // Call base class initialisation
  if (sc.isFailure() )  return sc;

  // Set up MsgSvc, Data Loader
  MsgStream log(msgSvc(), name());
  IConversionSvc* cnv_svc;
  sc = serviceLocator()->service("DetectorPersistencySvc", cnv_svc, true);
  if (sc .isFailure() ) {
    log << MSG::ERROR << "Unable to find DetectorPersistencySvc " << endreq;
    return sc;
  }
 
  IIncidentSvc* incsvc;
     sc = service("IncidentSvc", incsvc);
     int priority = 100;
     if( sc.isSuccess() ){
          incsvc -> addListener(this, "NewRun", priority);
     }
  
 sc = serviceLocator()->service("EventDataSvc", m_eventSvc, true);
  if (sc .isFailure() ) {
    log << MSG::ERROR << "Unable to find EventDataSvc " << endreq;
    return sc;
  }

  sc = setDataLoader(cnv_svc);
  if (sc.isFailure() ) {
    log << MSG::ERROR << "Unable to set data loader " << endreq;
    return sc;
  }
  sc = setProperties();   

  // Initialize the calibration data transient store
  IAddressCreator*     calibCreator = 0;

  // Use Gaudi-supplied DetectorPersistencySvc; it's essentially
  // the same as base class PersistencySvc, which is all we need
  sc = serviceLocator()->service("DetectorPersistencySvc", calibCreator);
  
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Unable to locate DetectorPersistencySvc." << endreq;
    return StatusCode::FAILURE; 
  }
  
  //   Make the root for the TDDS data
  DataObject* rootObj = new DataObject();
  sc = setRoot(m_rootName, rootObj);
  if (!sc.isSuccess() ) {
    log << MSG::ERROR << "Unable to set calib data store root." << endreq;
    delete rootObj;
    return sc;
  }

// Create and register the next level of nodes.
// Have one per calibration type. They are of a class trivially 
// derived from DataObject, CalibCLIDNode.  Only additional 
// information is CLID of child nodes.  List comes from CalibData 
// namespace
//StatusCode CalibDataSvc::makeFlavorNodes(IAddressCreator*  calibCreator,
//                                         MsgStream* log) {
  typedef std::vector<CalibData::CalibModelSvc::CalibPair>::const_iterator 
    PairIt;
  PairIt  pairIt;
  CalibData::CalibModelSvc svc;
  const std::vector<CalibData::CalibModelSvc::CalibPair>& pairs = 
    svc.getPairs();
  int jj =0;
  for (pairIt = pairs.begin(); pairIt != pairs.end(); pairIt++,jj++) {
    
    CalibCLIDNode* node = new CalibCLIDNode(pairIt->second);

    std::string calibTypePath(pairIt->first);
    // sc =DataSvc::registerObject(calibTypePath, node);

    // Still have to figure out what to do about args, iargs
    unsigned long iargs[]={0,0};
    IOpaqueAddress* pAddress;

    // Set up nodes for each calibration type, default flavor
    // Create and register addresses suitable for the metadata
    // conversion service.  Ultimately, in order to find the "right"
    // set of constants,  it needs to know
    //    Calibration type, e.g. CAL Electronic gain
    //    Flavor            e.g. vanilla 
    //    Event time        validity period of constants must include this time
    //    Instrument        LAT, EM, etc.
    // We save the first two, or equivalent information, in the first
    // string parameter of a generic address
    // Consumers can use utilities in CalibData::CalibModelSvc to
    // extract fields they need
    // Event time and Instrument will be discovered by conversion service
    // when constants are requested by invoking our (CalibDataSvc) time
    // and instrument name services, resp.

    // Always do vanilla
    std::string fullpath = calibTypePath;
    std::string args[] = {fullpath};
  
//    sc = calibCreator->createAddress(m_calibStorageType, 
//				     pairIt->second,   // class id
//                                     args, iargs, pAddress);
    sc = calibCreator->createAddress(m_calibType[jj],
                                     pairIt->second,   // class id
                                     args, iargs, pAddress);


    if (!sc.isSuccess()) { 
      log<< MSG::INFO 
          << "Unable to create Calib address with path " << fullpath << endreq;
    }
 
    // A node unof a specific flavor is a child of the per-calibration type
    // node for which an object was registered above.
    sc = registerAddress(fullpath, pAddress);
    if (!sc.isSuccess()) {
      log<< MSG::ERROR << "Unable to register Calib address with path" 
	 << fullpath << endreq;
    }
    // Now do the same for any requested flavors
    /*
    unsigned int ix;
    
    for (ix = 0; ix < m_flavorList.size(); ix++) { 
      log<<MSG::DEBUG<<"here is the flavor in the CalibDataSvc"<<endreq; 
      // Don't redo vanilla, if present in user list
      if (m_flavorList[ix] == std::string("vanilla")) continue;

      fullpath = calibTypePath + "/" + m_flavorList[ix];
      args[0] = fullpath;

      sc = calibCreator->createAddress(m_calibStorageType, 
                                       pairIt->second, args, iargs, pAddress); 
      if (!sc.isSuccess()) {


        log<< MSG::ERROR << "Unable to create Calib address with path " 
            << fullpath << endreq;
      }
      sc = DataSvc::registerAddress(fullpath, pAddress);
      if (!sc.isSuccess()) {
        log<< MSG::ERROR << "Unable to register Calib address with path " 
            << fullpath << endreq;
      }
    }   
 // end flavor loop 
 */
  } 
// end calibType loop
// initialize the parameters
				     
  return StatusCode::SUCCESS;
}
     


/// Finalize the service.
StatusCode CalibDataSvc::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Finalizing" << endreq;

  //  return StatusCode::SUCCESS;
  // Finalize the base class
  //  huangb temply delete
   return DataSvc::finalize();
}

StatusCode CalibDataSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  // With the highest priority return the specific interfaces
  // If interfaces are not directly available, try out a base class
  if (IDetDataSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = (IDetDataSvc*)this;
  } else if (IInstrumentName::interfaceID().versionMatch(riid) ) {
    *ppvInterface = (IInstrumentName*) this;
//  } else if ( IID_IIncidentListener.versionMatch(riid) ) {
//    *ppvInterface = (IIncidentListener*)this; 
  } else {
    return DataSvc::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}






/// Remove all data objects in the data store.
StatusCode CalibDataSvc::clearStore()   {
  DataSvc::clearStore();
  return StatusCode::SUCCESS;
}

//update the calibration in TCDS if a new run comes
void CalibDataSvc::handle ( const Incident& inc ) {
   MsgStream log( msgSvc(), name() );
     return; 
  }






StatusCode CalibDataSvc::updateObject( DataObject* toUpdate ) {

   
  MsgStream log( msgSvc(), name() );
   log << MSG::INFO<<"begin of CalibDataSvc::updateObject"<<endreq;
  // Check that object to update exists
  if ( 0 == toUpdate ) { 
    log << MSG::ERROR
	<< "There is no DataObject to update" << endreq;
    return INVALID_OBJECT; 
  }

   log << MSG::INFO<<"begin of CalibDataSvc::updateObject"<<endreq;
  // Now delegate update to the conversion service by calling the base class
  StatusCode status = DataSvc::updateObject(toUpdate);
  if ( !status.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not update DataObject" << endreq; 
    if ( status == NO_DATA_LOADER )
      log << MSG::ERROR << "There is no data loader" << endreq; 
    return status;
  } 
   log << MSG::INFO<<"begin of CalibDataSvc::updateObject successfully updated"<<endreq;
  // DataObject was successfully updated
  return StatusCode::SUCCESS;
}

StatusCode CalibDataSvc::loadObject(IConversionSvc* pLoader, 
                                    IRegistry* pRegistry) {
  return DataSvc::loadObject(pLoader, pRegistry);

}

StatusCode CalibDataSvc::retrieveObject(const std::string& fullPath,DataObject*& pObject)
{  
   MsgStream log( msgSvc(), name() );
  
 int runNo;
 //maqm SmartDataPtr<Event::EventHeader> evt(m_eventSvc,"/Event");
 SmartDataPtr<Event::EventHeader> evt(m_eventSvc,"/Event/EventHeader");
 if( evt ){
   runNo = evt -> runNumber();
   log << MSG::DEBUG <<"The runNumber of current event is  "<<runNo<<endreq;
 }

 else{
   log << MSG::WARNING << "WARNING accessing Event" <<endreq;
//   return StatusCode::FAILURE;
 }

   
StatusCode  sc =DataSvc::retrieveObject(fullPath,pObject);
 if (!sc.isSuccess()) {
        log<< MSG::ERROR << "Unable to get the retrieveObject" 
            << endreq;
	return sc;
  }
// if(fullPath=="/Calib/MdcAlign"&&m_calibType[0]==CALIBROOT_StorageType) 
//   { return StatusCode::SUCCESS;}
if(fullPath=="/Calib/MdcCal"&&m_calibType[0]==CALIBROOT_StorageType){
    return StatusCode::SUCCESS;}
if(fullPath=="/Calib/DedxCal"&&m_calibType[2]==CALIBROOT_StorageType){
    return StatusCode::SUCCESS;}
if(fullPath=="/Calib/TofCal"&&m_calibType[1]==CALIBROOT_StorageType){
    return StatusCode::SUCCESS;}
if(fullPath=="/Calib/EmcCal"&&m_calibType[3]==CALIBROOT_StorageType){
    return StatusCode::SUCCESS;}
if(fullPath=="/Calib/MucCal"&&m_calibType[4]==CALIBROOT_StorageType){
    return StatusCode::SUCCESS;}
if(fullPath=="/Calib/EsTimeCal"&&m_calibType[5]==CALIBROOT_StorageType){
    return StatusCode::SUCCESS;}
if(fullPath=="/Calib/EstTofCal"&&m_calibType[6]==CALIBROOT_StorageType){
    return StatusCode::SUCCESS;}
if(fullPath=="/Calib/TofQElec"&&m_calibType[8]==CALIBROOT_StorageType){
    return StatusCode::SUCCESS;}
if(fullPath=="/Calib/TofSim"&&m_calibType[9]==CALIBROOT_StorageType){
    return StatusCode::SUCCESS;}
if(fullPath=="/Calib/DedxSim"&&m_calibType[10]==CALIBROOT_StorageType){
    return StatusCode::SUCCESS;}
 

CalibData::CalibBase1* tmpObject = dynamic_cast<CalibData::CalibBase1*>(pObject);
if(fullPath=="/Calib/TofCal"&&runNo>= tmpObject->getrunfrm()&&runNo<=tmpObject->getrunto())
{ return StatusCode::SUCCESS;}
if(fullPath=="/Calib/EstTofCal"&&runNo>= tmpObject->getrunfrm()&&runNo<=tmpObject->getrunto())
{ return StatusCode::SUCCESS;}
if(fullPath=="/Calib/TofSim"&&runNo>= tmpObject->getrunfrm()&&runNo<=tmpObject->getrunto())
{ return StatusCode::SUCCESS;}
if(fullPath=="/Calib/DedxSim"&&runNo>= tmpObject->getrunfrm()&&runNo<=tmpObject->getrunto())
{ return StatusCode::SUCCESS;}



if( fabs(runNo) >= tmpObject->getrunfrm() && fabs(runNo)<=tmpObject->getrunto())
{
	log << MSG::DEBUG <<__LINE__<< " runfrm @CalibDataSvc is:"<<tmpObject->getrunfrm()<<" runto min is:"<<tmpObject->getrunto()<<endreq;
	return StatusCode::SUCCESS;}
else
{ 
	log<< MSG::INFO <<"@CalibDataSvc runfrm="<< tmpObject->getrunfrm()<<"runto="<<tmpObject->getrunto()<<"runNo="<<runNo<<endreq;
	log<< MSG::DEBUG << "update the Object" 
		<< endreq;
	sc =DataSvc:: updateObject(pObject);
	return sc;    
}//end of else
}//end of if

StatusCode CalibDataSvc::registerObject (const std::string& parentPath,
                                    const std::string& objPath,
                                    DataObject* pObject)   {
  DataObject* pO = 0;
  StatusCode status =DataSvc::retrieveObject(parentPath, pO);
  if ( !status.isSuccess() && m_forceLeaves )   {
    pO = createDefaultObject();
    status =DataSvc::registerObject(parentPath, pO);
    if ( !status.isSuccess() )   {
      pO->release();
    }//end of if
  }//end of if( !status.isSuccess() && m_forceLeaves )
  if ( status.isSuccess() )   {
    status =DataSvc::registerObject(pO, objPath, pObject);
  }
  if(status.isSuccess() ){
  return status;
}
}

   
