#include "MdcRawEvent/MdcDigi.h"

MdcDigi::MdcDigi(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel) :
  RawData(id, timeChannel, chargeChannel),
  m_overflow(0) {
}

MdcDigi::MdcDigi(const unsigned int id) :
  RawData(id),
  m_overflow(0) {
}

//mixing two digis 
MdcDigi& MdcDigi::operator+=(const MdcDigi &digi)
{
  if(m_id != digi.identify()) // do nothing
    return *this;

  if(digi.getTimeChannel() < m_timeChannel){
    m_timeChannel=digi.getTimeChannel();
  }else{
    this->setTrackIndex(this->getTrackIndex() + 1999); // track index = 1000+tkId
  }
  /*if(digi.getChargeChannel() < 100000 ){ //maxium is around 30000, a very big number means actually no Q value
  m_chargeChannel += digi.getChargeChannel();
  }*/
  return *this;
}
