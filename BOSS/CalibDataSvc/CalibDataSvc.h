// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibDataSvc/CalibDataSvc/CalibDataSvc.h,v 1.15 2014/04/18 05:01:11 maqm Exp $
#ifndef CalibDataSvc_h
#define CalibDataSvc_h

// Base classes
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "CalibData/CalibBase1.h"
#include "CalibDataSvc/IInstrumentName.h"
//#include "CalibData/CalibTime.h"
using namespace CalibData;
// Forward declarations
//class ITime;
class StatusCode;
class IDataProviderSvc;

/** @class CalibDataSvc

    A DataSvc specialized for calibration data.  
    This Service borrows heavily from DetDataSvc.  In particular it
    implements the IDetDataSvc interface.  The only significant
    difference is in initialize() and in the elimination of members
    concerned with detector (geometry) description.

    Maybe will also need to implement another abstract service
    which gets (and sets?) instrument.

    @author J. Bogart
    @date   15 Oct. 2002   
*/
class IAddressCreator;
class MsgStream;

template <class TYPE> class SvcFactory;  //maqm add
class CalibDataSvc  : public DataSvc,
//                      virtual public IDetDataSvc,
                      virtual public IIncidentListener
 //                     virtual public IInstrumentName
{    

 friend class SvcFactory<CalibDataSvc>;

public:
  
  // Overloaded DataSvc methods

  virtual StatusCode initialize();
  
  virtual StatusCode finalize();
  
  /// Remove all data objects in the data store.
  virtual StatusCode clearStore();
  
  /// Update object
  virtual StatusCode updateObject( DataObject* toUpdate );

  /// Load object.  Override DataSvc implementation to get current 
  /// event time first if necessary
  virtual StatusCode loadObject(IConversionSvc* pLoader, IRegistry* pRegistry);

  virtual StatusCode retrieveObject(const std::string& fullPath,DataObject*& pObject);  

  virtual StatusCode registerObject(const std::string& parentPath,
                                    const std::string& objPath,
                                    DataObject* pObject);


//maqm protected:
public:
  CalibDataSvc(const std::string& name, ISvcLocator* svc);
  virtual ~CalibDataSvc();
  
public:
  
  // Reimplemented from IInterface

  /// Query the interface of the service
  virtual StatusCode queryInterface( const InterfaceID& riid, 
				     void** ppvInterface );  


public:
  //get the CalibType from  CalibDataSvc, either  MYSQL_StorageType or CALIBROOT_StorageType;
//  int  getsvctype(){return m_calibStorageType;}

public:
  
  // Implementation of the IIncidentListener interface

  /// Inform that a new incident has occured
  virtual void handle( const Incident& );

  /// For use of CalibMySQLCnvSvc, to set "use event time mode"
 private:
 StatusCode updateRun(int &runfrm,int &runto,std::string& fullPath);
 //properties
  /// Calibration Data Persistency Storage type
 // int              m_calibStorageType;

  // classfy the Data Persistency Storage type by each calib type
  int m_calibType[12];

  /// Name of the root node of the calib store
  std::string      m_calibRootName;

  /// calibration types
  StringArrayProperty m_calibList;

  //  StringArrayProperty m_flavorList;
  std::vector<std::string>  m_flavorList;


  /// Set to true by handle( ) at BeginEvent; cleared after timestamp acquired
 // bool m_newEvent;

  /// Dumping place for various time-fetching methods to save the timestamp
 // CalibData::CalibTime m_time;

  IDataProviderSvc* m_eventSvc;
  /** Redundant job option to indicate whether or not to check for valid
      event time when fetching calibrations.  There already is a similar thing
      for CalibMySQLCnvSvc, but no easy way for one service to tell the 
      other about it.
  */
//  bool  m_useEventTime;


  /// Private utility, called from initialize()
  StatusCode makeFlavorNodes(IAddressCreator*  calibCreator, 
                             MsgStream* log );

  /// Private utility to check if internal timestamp has been updated for
  /// the event; if not do it.
  // StatusCode updateTime();

  
/*
  enum TIMESOURCE {
    TIMESOURCEnone = 0,
    TIMESOURCEdata,
    TIMESOURCEmc,
    TIMESOURCEclock,
    TIMESOURCEdigi
  };

  TIMESOURCE m_timeSourceEnum;

  std::string m_startTimeAsc;

  /// Absolute time of first event (seconds)
  long m_startTime;

  /// Absolute time spacing between events (milliseconds)
  long m_delayTime;*/

};

#endif //  CalibDataSvc_h



