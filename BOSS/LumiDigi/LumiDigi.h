#ifndef LumiDigi_H
#define LumiDigi_H 

#include "GaudiKernel/ObjectVector.h"
#include "RawEvent/RawData.h"
#include "TofRawEvent/TofDigi.h"

extern const CLID& CLID_LumiDigi;

class LumiDigi : public RawData { 

public: 
  // Constructor 
  LumiDigi(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel);
  LumiDigi(const unsigned int id);
  LumiDigi(TofDigi* tofDigi);

  // Set Overflow Flag
  void setOverflow(const unsigned int overflow) { m_overflow = overflow; }
 
  // Get Overflow Flag
  unsigned int getOverflow() { 
       return m_overflow; 
  }

  // Retrieve reference to class definition structure
  virtual const CLID& clID() const   { return LumiDigi::classID(); }
  static  const CLID& classID()       { return CLID_LumiDigi; }


private:
  unsigned int m_overflow;

};

typedef ObjectVector<LumiDigi> LumiDigiCol;

#endif
