//$Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibDataSvc/CalibDataSvc/ICalibMetaCnvSvc.h,v 1.1 2006/04/06 03:04:46 maqm Exp $
#ifndef ICalibMetaCnvSvc_h
#define ICalibMetaCnvSvc_h 1

#include "GaudiKernel/IInterface.h"

// External constants
#include "GaudiKernel/ClassID.h"

// Type definition
/////#include "ConditionsDB/CondDBKey.h"

static const InterfaceID IID_ICalibMetaCnvSvc ("ICalibMetaCnvSvc", 1, 0);

// Forward declarations
class DataObject;
// class IConditionsDBGate;
class IRegistry;
class ITime;

namespace calibUtil {
  class Metadata;
}


/** @class ICalibMetaCnvSvc 

    Abstract interface of a conversion service for GLAST calibration
    persistency.

    Handles creation and updating condition data objects (i.e. DataObjects
    implementing IValidity).

    Adapted from Andrea Valassi's IConditionsDBCnvSvc interface

    @author Joanne Bogart
    @date November 2002
*/

class ICalibMetaCnvSvc : virtual public IInterface
{

 public:

  // Re-implemented from IInterface

  /// Interface ID of this class
  static const InterfaceID& interfaceID() { return IID_ICalibMetaCnvSvc; }

 public:
  
  // Create/update condition MetadataObject not necessarily registered in 
  // the TDS.
  /// Create a calib DataObject -- corresponds to info from metadata -- by 
  /// --> whatever information is needed by Metadata::findBest,
  /// namely calibType, event time, instrument, flavor, maybe level
  //  [was folder name, tag and time for IConditionsDBCnvSvc]
  /// This method does not register DataObject in the transient data store,
  /// The string storage type is discovered at runtime in the MySQL metadata
  /// database.
  virtual StatusCode createCalib(DataObject*& refpObject,
				  const std::string& fullpath,
				  const CLID&        classID,
				  IRegistry*         entry=0) = 0;
   
  /// Update a condition DataObject by type, flavor, time (& instrument?).
  /// This method does not register DataObject in the transient data store,
  /// but may register TDS addresses for its children if needed (e.g. Catalog).
  /// The string storage type is discovered at runtime in the MySQL metadata
  /// database.
   virtual StatusCode updateCalib(DataObject*        pObject,
				  const std::string& fullpath,
		 		   const CLID&        classID,
				  IRegistry*         entry=0) = 0;
   
   
   
  // Get handle for metadata access from calibUtil.
  virtual calibUtil::Metadata* getMeta() = 0;

  /*maqm remove
   virtual StatusCode getValidInterval(unsigned int& serNo,
                                       ITime** pvStart, ITime** pvEnd) = 0;
  */
   // Might want to add additional methods which will return certain metadata
  // in a convenient form, given a serial number.  Would be used
  // by the bulk data conversion services to get some standard set
  // of information which would go in every calib data object --
  // except a "bad channel" list composed from dead + hot doesn't
  // correspond to any single metadata row.  
};

#endif

