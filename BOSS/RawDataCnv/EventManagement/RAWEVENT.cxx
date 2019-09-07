#include "RawDataCnv/EventManagement/RAWEVENT.h"

RAWEVENT::RAWEVENT() : m_eventHeader() {
}

void RAWEVENT::reset() {
  m_bufMdcDigi.reset();
  m_bufTofDigi.reset();
  m_bufEtfDigi.reset();
  m_bufEmcDigi.reset();
  m_bufMucDigi.reset();
  m_bufTrigGTD.reset();
  m_bufZddDigi.reset();
  m_bufHltRaw.reset();
  m_bufMcPar.reset();
}
