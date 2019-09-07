#include "EmcRawEvent/EmcDigi.h"
#include "EmcWaveform/EmcWaveform.h"
#include "RawEvent/RawDataUtil.h"

EmcDigi::EmcDigi(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel) :
  RawData(id, timeChannel, chargeChannel),
  m_measure(0) {
}

EmcDigi::EmcDigi(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel,
    const unsigned int measure) :
  RawData(id, timeChannel, chargeChannel),
  m_measure(measure) {
}

EmcDigi::EmcDigi(const unsigned int id) :
  RawData(id), 
  m_measure(0) {
}

//mixing two digi with EmcWaveform
EmcDigi& EmcDigi::operator+=(const EmcDigi &digi)
{
  if(m_id != digi.identify()) // do nothing
    return *this;
  
  double e1=RawDataUtil::EmcCharge(m_measure,m_chargeChannel);
  double t1=RawDataUtil::EmcTime(m_timeChannel);
  double e2=RawDataUtil::EmcCharge(digi.getMeasure(),digi.getChargeChannel());
  double t2=RawDataUtil::EmcTime(digi.getTimeChannel());

  EmcWaveform wave1,wave2;
  wave1.makeWaveform(e1,t1);
  wave2.makeWaveform(e2,t2);

  //mixing
  wave1 += wave2;

  double energy;
  int time;
  energy = wave1.max(time);

  m_chargeChannel = RawDataUtil::EmcChargeChannel(energy);
  m_timeChannel = RawDataUtil::EmcTimeChannel(time);
  m_measure = RawDataUtil::EmcChargeMeasure(energy);

  return *this;
}
