//$Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/CalibBase1.h,v 1.2 2005/12/01 07:20:04 huangb Exp $
#ifndef CalibData_CalibBase1_h
#define CalibData_CalibBase1_h

/** @class CalibBase

   Used as a base for all objects in the calibration data store.
   Implement IValidity.

   Permits implementation of deep copy by means of virtual update
   method.  [Used in CalibMySQLCnvSvc::updateCalib]

   @author J. Bogart

*/

#include "GaudiKernel/DataObject.h"

#include "CalibData/CalibModel.h"

// extern const CLID& CLID_Calib_CalibBase;

class MsgStream;

namespace CalibData {
  class CalibBase1 : public DataObject
                     {

    friend class XmlBaseCnv;
    friend class RootBaseCnv;
    
  public:
    CalibBase1();
    CalibBase1(int serNo,int runfrm,int runto);
    CalibBase1(int runfrm,int runto);
    CalibBase1(const CalibBase1& obj);
    virtual StatusCode update(CalibBase1& obj, MsgStream* );
    
    /// Following is intended for deep copy
     /* 
    // Re-implemented from DataObject
    /// Class ID of this instance
    inline virtual const CLID& clID() const { return classID(); } 
    
    /// Class ID of this class
    inline static  const CLID& classID() { return CLID_Calib_CalibBase; };
   */
  public:
    
  
    virtual int getSerNo() const {return m_serNo;}
    virtual int getrunfrm() const {return m_runfrm;}
    virtual int getrunto() const {return m_runto;}
    void setSerNo(int ser) { m_serNo = ser;}
    void setrunfrm(int runfrm){ m_runfrm = runfrm;}
    void setrunto(int runto) {m_runto = runto;}
  protected:
    
    int m_serNo;
    int m_runfrm;
    int m_runto;
    
  };

}
#endif
