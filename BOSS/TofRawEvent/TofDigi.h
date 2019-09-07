#ifndef TofDigi_H
#define TofDigi_H 
#include <vector>
#include <iostream>
#include "GaudiKernel/ObjectVector.h"
#include "RawEvent/RawData.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_TofDigi;

/*
 *
 */

class TofDigi : public RawData { 
public: 
  // Constructor 
  TofDigi(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel);
  TofDigi(const unsigned int id);

  // Set Overflow Flag
  void setOverflow(const unsigned int overflow) { m_overflow = overflow; }
 
  // Get Overflow Flag
  unsigned int getOverflow() { 
       //if (m_timeChannel > 65535 || m_chargeChannel > 65535) {
       //   m_overflow = 1;
       //}
       return m_overflow; 
  }

  // Retrieve reference to class definition structure
  virtual const CLID& clID() const   { return TofDigi::classID(); }
  static  const CLID& classID()       { return CLID_TofDigi; }

private:
  unsigned int m_overflow;
};

typedef ObjectVector<TofDigi> TofDigiCol;

#endif
