#include "TrigEvent/TrigGTDProvider.h"
TrigGTDProvider::TrigGTDProvider(const unsigned int boardIndex, const unsigned int dataType, const unsigned int timeWindow,                                                     const unsigned int GTDProviderTiming,  const bool preScale,
                                 const std::map<unsigned int, std::vector<unsigned int> > GTDProviderData,                                                                      const std::vector<unsigned int> GTDProviderEvtType, 
                                 const std::map<unsigned int, std::vector<unsigned int> > Num_One, 
                                 const std::map<unsigned int, std::vector<unsigned int> > Num_Zero)
{
  m_dataType = dataType;
  m_timeWindow = timeWindow;
  m_boardIndex = boardIndex;
  m_GTDProviderTiming = GTDProviderTiming;
  m_preScale = preScale;
  m_GTDProviderData = GTDProviderData;
  m_GTDProviderEvtType = GTDProviderEvtType;
  m_Num_One = Num_One;
  m_Num_Zero = Num_Zero;
}

void TrigGTDProvider::setDataType(const unsigned int dataType) {
  m_dataType = dataType;
}

void TrigGTDProvider::setTimeWindow(const unsigned int timeWindow) {
  m_timeWindow = timeWindow;
}

void TrigGTDProvider::setBoardIndex(const unsigned int boardIndex) {
  m_boardIndex = boardIndex;
}

void TrigGTDProvider::setTrigCondList(const std::map<unsigned int, std::vector<unsigned int> >  GTDProviderData) {
  m_GTDProviderData = GTDProviderData;
}

void TrigGTDProvider::setTrigEvtType(const std::vector<unsigned int> GTDProviderEvtType) {
  m_GTDProviderEvtType = GTDProviderEvtType;
}

void TrigGTDProvider::setTrigTiming(const unsigned int GTDProviderTiming) {
  m_GTDProviderTiming = GTDProviderTiming; 
}

void TrigGTDProvider::setPreScale(const bool preScale) {
  m_preScale = preScale;
}

void TrigGTDProvider::setNumOne(const std::map<unsigned int, std::vector<unsigned int> > Num_One) {
 m_Num_One = Num_One;
}

void TrigGTDProvider::setNumZero(const std::map<unsigned int, std::vector<unsigned int> > Num_Zero) {
  m_Num_Zero = Num_Zero;
}

unsigned int TrigGTDProvider::getDataType() const {
  return m_dataType;
}

unsigned int TrigGTDProvider::getTimeWindow() const {
  return m_timeWindow;
}

unsigned int TrigGTDProvider::getBoardIndex() const {
  return m_boardIndex;
}

const std::map<unsigned int, std::vector<unsigned int> >& TrigGTDProvider::getTrigCondList() const {
  return m_GTDProviderData; 
}

const std::vector<unsigned int>& TrigGTDProvider::getTrigEvtType() const {
  return m_GTDProviderEvtType;
}

unsigned int TrigGTDProvider::getTrigTiming() const {
  return m_GTDProviderTiming;
}
 
bool TrigGTDProvider::getPreScale() const {
  return m_preScale;
}

const std::map<unsigned int, std::vector<unsigned int> >& TrigGTDProvider::getNumOne() const {
  return m_Num_One;
}

const std::map<unsigned int, std::vector<unsigned int> >& TrigGTDProvider::getNumZero() const {
  return m_Num_Zero;
}
