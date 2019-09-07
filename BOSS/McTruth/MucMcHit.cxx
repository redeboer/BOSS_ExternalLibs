#include "McTruth/MucMcHit.h"

Event::MucMcHit::MucMcHit(const Identifier& id, unsigned int trackIndex, double xPosition,
           double yPosition, double zPosition, double px, double py,
	   double pz) 
         : m_id(id), m_trackIndex(trackIndex),
           m_xPosition(xPosition), m_yPosition(yPosition),m_zPosition(zPosition),
           m_px(px), m_py(py), m_pz(pz){}

// Get associated id
Identifier Event::MucMcHit::identify() const {
  return  m_id;
}
 
// Get the associated track id 
unsigned int Event::MucMcHit::getTrackIndex() const {
  return m_trackIndex;
}

// Get the position x 
double Event::MucMcHit::getPositionX() const {
  return m_xPosition;
}

// Get the position y 
double Event::MucMcHit::getPositionY() const {
  return m_yPosition;
}

// Get the position z 
double Event::MucMcHit::getPositionZ() const {
  return m_zPosition;
}
  
// Get momentum Px 
double Event::MucMcHit::getPx() const {
  return m_px;
}

// Get momentum Py  
double Event::MucMcHit::getPy() const {
  return m_py; 
}

// Get momentum Pz  
double Event::MucMcHit::getPz() const {
  return m_pz; 
}
  
