#include "McTruth/TofMcHit.h"

Event::TofMcHit::TofMcHit(const Identifier& id, unsigned int trackIndex, double xPosition,
           double yPosition, double zPosition, double px, double py,
	   double pz, double trackLength, double flightTime) 
         : m_id(id), m_trackIndex(trackIndex),
           m_xPosition(xPosition), m_yPosition(yPosition),m_zPosition(zPosition),
           m_px(px), m_py(py), m_pz(pz), m_trackLength(trackLength), m_flightTime(flightTime){}

// Get associated id
Identifier Event::TofMcHit::identify() const {
  return  m_id;
}
 
// Get the associated track id 
unsigned int Event::TofMcHit::getTrackIndex() const {
  return m_trackIndex;
}

// Get the position x 
double Event::TofMcHit::getPositionX() const {
  return m_xPosition;
}

// Get the position y 
double Event::TofMcHit::getPositionY() const {
  return m_yPosition;
}

// Get the position z 
double Event::TofMcHit::getPositionZ() const {
  return m_zPosition;
}
  
// Get momentum Px 
double Event::TofMcHit::getPx() const {
  return m_px;
}

// Get momentum Py  
double Event::TofMcHit::getPy() const {
  return m_py; 
}

// Get momentum Pz  
double Event::TofMcHit::getPz() const {
  return m_pz; 
}
  
// Get the trajectory length 
double Event::TofMcHit::getTrackLength() const {
  return m_trackLength; 
}

// Get the flight time 
double Event::TofMcHit::getFlightTime() const {
  return m_flightTime; 
}
