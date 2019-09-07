#include "TrigEvent/TrigGTD.h"

TrigGTD::TrigGTD(const uint32_t* GTDPtr) : m_dataPtr(GTDPtr+1) {
  uint32_t GTDHeader = *GTDPtr;
  m_id         = ( GTDHeader >> 24);
  m_dataSize   = ((GTDHeader >> 14) & 0x3FF) - 1;
  //if (m_id == 0xD7) m_dataSize -= 1;
  m_timeWindow = ((GTDHeader >> 8)  & 0x3F);
  m_dataType   = ((GTDHeader >> 3)  & 0x1F);
}
