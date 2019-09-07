#ifndef EVENT_TRIG_GTD_H
#define EVENT_TRIG_GTD_H

#include <vector>
#include <stdint.h>  //uint32_t

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"

extern const CLID& CLID_TrigGTD;

// Global trigger data structure
class TrigGTD : virtual public ContainedObject {
public:
  //constructor
  TrigGTD(const uint32_t* GTDPtr);
  //destructor
  virtual ~TrigGTD() {}

  virtual const CLID& clID() const { return TrigGTD::classID(); }
  static const CLID& classID() { return CLID_TrigGTD; }

  const uint32_t getId() const { return m_id; }
  const uint32_t getDataSize() const { return m_dataSize; }
  const uint32_t getTimeWindow() const { return m_timeWindow; }
  const uint32_t getDataType() const { return m_dataType; }

  const uint32_t* getDataPtr() const { return m_dataPtr; }

private:
  uint32_t m_id;
  uint32_t m_dataSize;
  uint32_t m_timeWindow;
  uint32_t m_dataType;

  const uint32_t* m_dataPtr;
};

typedef ObjectVector<TrigGTD> TrigGTDCol;

#endif  // EVENT_TRIG_GTD_H
