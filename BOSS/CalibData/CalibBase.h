//$Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/CalibBase.h,v 1.2 2011/02/17 05:10:16 maqm Exp $
#ifndef CalibData_CalibBase_h
#define CalibData_CalibBase_h

/** @class CalibBase

   Used as a base for all objects in the calibration data store.
   Implement IValidity.

   Permits implementation of deep copy by means of virtual update
   method.  [Used in CalibMySQLCnvSvc::updateCalib]

   @author J. Bogart

*/

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IValidity.h"
//#include "GaudiKernel/ITime.h"

#include "CalibData/CalibModel.h"

// extern const CLID& CLID_Calib_CalibBase;

class MsgStream;

namespace CalibData {
  //class CalibTime;

  class CalibBase : public DataObject,
                    virtual public IValidity {

    friend class XmlBaseCnv;
    friend class RootBaseCnv;
    
  public:
    CalibBase();
//    CalibBase(const ITime& since, const ITime& till, int serNo = -1);
    
    CalibBase(const CalibBase& obj);
    
    /// Following is intended for deep copy
    virtual StatusCode update(CalibBase& obj, MsgStream *);
    virtual ~CalibBase();
    
    // Re-implemented from DataObject
    /// Class ID of this instance
    inline virtual const CLID& clID() const { return classID(); } 
    
    /// Class ID of this class
    inline static  const CLID& classID() { return CLID_Calib_CalibBase; };
    
  public:
    
    // Implementation of IValidity
    
    /// Check if the data object has a well defined validity range
  /*  virtual bool isValid();
    
    /// Check if the data object is valid at the specified time
    virtual bool isValid(const ITime& t);
    
    /// Get start of validity
    virtual const ITime& validSince();
    
    /// Get end of validity
    virtual const ITime& validTill();
    
    /// Set validity range
    virtual void setValidity(const ITime& since, const ITime& till);  
    
    /// Set start of validity
    virtual void setValiditySince(const ITime& since);  
    
    /// Set end of validity
    virtual void setValidityTill(const ITime& till);   
    
    /// Update the validity range (foreseen for tree-like structures)
    virtual StatusCode updateValidity();
*/
    /// Get serial number of metadata row corresponding to calibration. 
    /// Can be used by clients to determine if object has been updated
    /// since last access.
    virtual int getSerNo() const {return m_serNo;}

  /*  virtual const CalibTime* getValidStart() const {return m_validSince;}
    virtual const CalibTime* getValidEnd() const {return m_validTill;}
    */
  protected:
    
    // IValidity data
    
    /// Start of validity
    //CalibTime* m_validSince;
    //    ITime* m_validSince;
    
    /// End of validity
    //CalibTime* m_validTill;
    //    ITime* m_validTill;

    /// Serial number of corresponding metadata row. 
    int m_serNo;

    void setSerNo(int ser) { m_serNo = ser;}

    // Other possible things to keep here:  flavor, calibration type
    
  };

}
#endif
