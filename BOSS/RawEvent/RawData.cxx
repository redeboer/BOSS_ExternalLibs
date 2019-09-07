#include <iostream>
#include "RawEvent/RawData.h"

RawData::RawData(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel) : 
  m_id(id), m_timeChannel(timeChannel), m_chargeChannel(chargeChannel), m_intId(id.get_value())
{
}

RawData::RawData(const unsigned int id) :
  m_id(Identifier(id)), m_timeChannel(0), m_chargeChannel(0), m_intId(id)
{
}

// Get associated id
Identifier RawData::identify() const {
  return  m_id;
}
 
// Get time channel
unsigned int RawData::getIntId() const {
  return m_intId;
}

// Set time channel
void RawData::setTimeChannel(const unsigned int timeChannel)  {
  m_timeChannel = timeChannel;
}
 
// Set charge channel
void RawData::setChargeChannel(const unsigned int chargeChannel) {
  m_chargeChannel = chargeChannel;
}

// Set track index
void RawData::setTrackIndex(const int trackIndex) {
  m_trackIndex = trackIndex;
}

// Get time channel 
unsigned int RawData::getTimeChannel() const {
  return m_timeChannel;
} 
 
// Get charge channel 
unsigned int RawData::getChargeChannel() const {
  return m_chargeChannel;
}

// Get track index
int RawData::getTrackIndex() const {
  return m_trackIndex;
}

RawData& RawData::operator+=(const RawData &rval)
{ 
  if(m_id != rval.identify()) // do nothing
    return *this;

  if( rval.getTimeChannel() < m_timeChannel )
    m_timeChannel = rval.getTimeChannel();

  m_chargeChannel += rval.getChargeChannel();
  return *this;
}
