// $Header: /bes/bes/BossCvs/Calibration/CalibData/src/CalibBase.cxx,v 1.4 2011/02/17 05:10:16 maqm Exp $

/** @class CalibBase
 *    Implementation of base class for all calibration data objects
 */

#include "CalibData/CalibBase.h"
//#include "CalibData/CalibTime.h"
#include "GaudiKernel/StatusCode.h"

namespace CalibData {
 /* CalibBase::CalibBase() : m_validSince(0), m_validTill(0), m_serNo(-1)
  {}

  CalibBase::CalibBase(const ITime& since, const ITime& till, int serNo) :
    m_validSince(0), m_validTill(0), m_serNo(serNo) 
  {
    m_validSince = new CalibData::CalibTime(since);
    m_validTill = new CalibData::CalibTime(till);
  }
*/
  CalibBase::CalibBase(const CalibBase& other) : 
    DataObject( (DataObject&) other) {
   // m_validSince = other.m_validSince;
   // m_validTill = other.m_validTill;
    m_serNo     = other.m_serNo;
  }

  CalibBase::~CalibBase() {
   /* delete m_validSince;
    delete m_validTill;*/
  }


StatusCode CalibBase::update(CalibBase& obj, MsgStream* ) {
   /*
  delete m_validSince;
  delete m_validTill;

    m_validTill = new CalibData::CalibTime(obj.validTill() );

    m_validSince = new CalibData::CalibTime(obj.validSince() );*/
    m_serNo = obj.m_serNo;  
    return StatusCode::SUCCESS;
  }

 /* bool CalibBase::isValid() {
    return ((m_validSince != 0) && (m_validTill != 0)
            && (validSince() <= validTill())   );
  }

*/
  // It makes no sense to go comparing times or setting times
  // using ITime interface unless we have an agreed-upon base;
  // i.e., ITime::absoluteTime() must always return something
  // in consistent units, counting from the same zero point.
  // There is no way to enforce this; it has to be a programmers'
  // agreement.
  // In our case, we assume that the underlying class implementing
  // ITime is always CalibTime.
 
  /*bool CalibBase::isValid (const ITime& t) {
    if (!isValid()) return false;
    return validSince() <= t &&  t <= validTill();
  }

  const ITime& CalibBase::validSince() {
    return *m_validSince;
  }

  const ITime& CalibBase::validTill() {
    return *m_validTill;
  }

  void CalibBase::setValidity(const ITime& since, const ITime& till) {
    setValiditySince(since);
    setValidityTill(till);
  }

  void CalibBase::setValiditySince(const ITime& since) {
    delete m_validSince;
    m_validSince = new CalibTime(since);
  }

  void CalibBase::setValidityTill(const ITime& till) {
    delete m_validTill;
    m_validTill = new CalibTime(till);
  }

  StatusCode CalibBase::updateValidity() {
    return StatusCode::SUCCESS;
  }*/
}
