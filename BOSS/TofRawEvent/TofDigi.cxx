#include "TofRawEvent/TofDigi.h"

TofDigi::TofDigi(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel) :
  RawData(id, timeChannel, chargeChannel),
  m_overflow(0) {
}

TofDigi::TofDigi(const unsigned int id) :
  RawData(id),
  m_overflow(0) {
}
