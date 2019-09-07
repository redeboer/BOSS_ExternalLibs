#ifndef EmcDigi_H
#define EmcDigi_H 
#include <vector>
#include "GaudiKernel/ObjectVector.h"
#include "RawEvent/RawData.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_EmcDigi;

/*
 *
 */

class EmcDigi : public RawData { 
public:
  // Constructor 
  EmcDigi(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel);
  EmcDigi(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel, 
      const unsigned int measure);
  EmcDigi(const unsigned int id);
 
  // Set Measure Word
  void setMeasure(const unsigned int measure) { m_measure = measure; } 

  // Get Measure Word
  unsigned int getMeasure() const { return m_measure; }

  // Retrieve reference to class definition structure
  virtual const CLID& clID() const   { return EmcDigi::classID(); }
  static  const CLID& classID()      { return CLID_EmcDigi; }

  // Add two signals, using waveform
  EmcDigi &operator+=(const EmcDigi &);

  virtual std::ostream& fillStream( std::ostream& s ) const;

private:
  unsigned int m_measure;

};

typedef ObjectVector<EmcDigi> EmcDigiCol;

inline std::ostream& EmcDigi::fillStream( std::ostream& s ) const {
  s << "class EmcDigi :" << std::endl
    << " m_id: " << m_intId
    << " m_timeChannel: " << m_timeChannel
    << " m_chargeChannel: " << m_chargeChannel
    << " m_measure: " << m_measure
    << std::endl;
  return s;
}
#endif
