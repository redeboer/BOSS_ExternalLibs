#include "GeneratorObject/McGenEvent.h"

// Constructor 
McGenEvent::McGenEvent(GenEvent* evt) { m_pEvt = evt; }

// Destructor 
McGenEvent::~McGenEvent() { delete  m_pEvt; }

// Public accessors:
std::string McGenEvent::generatorName() const { return m_generatorName; }

GenEvent* McGenEvent::getGenEvt() const {
  return m_pEvt; 
}
