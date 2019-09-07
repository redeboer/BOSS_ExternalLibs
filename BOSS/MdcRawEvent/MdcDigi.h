#ifndef MdcDigi_H
#define MdcDigi_H 
#include <vector>
#include "GaudiKernel/ObjectVector.h"
#include "RawEvent/RawData.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_MdcDigi;

/*
 *
 */

class MdcDigi : public RawData { 
public:
 // Constructor 
 MdcDigi(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel);
 MdcDigi(const unsigned int id);
 

 // Set Overflow Flag
 void setOverflow(const unsigned int overflow) { m_overflow = overflow; }

 // Get Overflow Flag
 unsigned int getOverflow() const { return m_overflow; }  
 // Retrieve reference to class definition structure
 virtual const CLID& clID() const   { return MdcDigi::classID(); }
 static  const CLID& classID()       { return CLID_MdcDigi; }

  // Add two signals
  MdcDigi &operator+=(const MdcDigi &);

private:
 unsigned int m_overflow;

};

typedef ObjectVector<MdcDigi> MdcDigiCol;

#endif
