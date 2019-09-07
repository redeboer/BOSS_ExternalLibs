// $Header: /bes/bes/BossCvs/Calibration/CalibData/src/CalibBase1.cxx,v 1.3 2011/02/17 05:10:16 maqm Exp $

/** @class CalibBase
 *    Implementation of base class for all calibration data objects
 */

#include "CalibData/CalibBase1.h"
//#include "CalibData/CalibTime.h"
#include "GaudiKernel/StatusCode.h"

namespace CalibData {
  CalibBase1::CalibBase1() : m_serNo(-1)
  { 
}

  CalibBase1::CalibBase1(int serNo, int runfrm, int runto) :
    m_serNo(serNo), m_runfrm(runfrm), m_runto(runto) 
  {
    }
  CalibBase1::CalibBase1(int runfrm, int runto) :
    m_runfrm(runfrm), m_runto(runto) 
  {
    }
  CalibBase1::CalibBase1(const CalibBase1& other) : 
    DataObject( (DataObject&) other) {
    m_runfrm = other.m_runfrm;
    m_runto = other.m_runto;
    m_serNo     = other.m_serNo;
  }

 

 StatusCode CalibBase1::update(CalibBase1& obj, MsgStream* ) {
   
    m_serNo = obj.m_serNo; 
    m_runfrm = obj.m_runfrm;
    m_runto = obj.m_runto;
    return StatusCode::SUCCESS;
  }
 
}
