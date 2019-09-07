// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibDataSvc/CalibDataSvc/ICalibTreeSvc.h,v 1.1 2008/04/09 02:24:09 huangb Exp $

#ifndef ICalibTreeSvc_h 
#define ICalibTreeSvc_h

/** @class ICalibTreeSvc
   Abstract interface to be satisfied by any ROOT conversion implementation.
 
   Support write-on-demand of a calibration object in the TDS

*/
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ClassID.h"    // for storage types
#include <string>

// Can't use ROOT_StorageType as our storage type or the 
// Persistency service will get confused.  Instead give an alias
// to one we can be pretty sure is unused for GLAST

unsigned const char CALIBTREE_StorageType  = ESC_StorageType;


static const InterfaceID IID_ICalibTreeSvc("ICalibTreeSvc", 1, 0);

class DataObject;

namespace CalibData {
  class CalibBase;
  class CalibBase1;
}

class ICalibTreeSvc : virtual public IInterface 
{
public:
  // Re-implemented from IInterface
  static const InterfaceID& interfaceID() { return IID_ICalibTreeSvc; }
  
  /**
   * Method to write a ROOT file corresponding to TDS object
   * 
   * @param fileName the name of the file to be written
   * @return the document issued from the parsing
   */
//  virtual StatusCode writeToTree(const std::string& outputFile, 
//                                 const std::string& tdsPath) = 0;
//  virtual StatusCode writeToTree(const std::string& outputFile, 
//                                 CalibData::CalibBase1 *calib) = 0;
 virtual std::string getrootfile() = 0;
  // Do we also want a "reset" or "clearDocument" ?  Can in any case
  // do this internally when a new document is to be parsed so might not
  // be necessary to have explicit public method.
};


#endif
