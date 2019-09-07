#include "McTruth/MdcMcHit.h"

Event::MdcMcHit::MdcMcHit(const Identifier& id, unsigned int trackIndex, double xPosition,
           double yPosition, double zPosition, double driftDistance,
	   double depositEnergy, int posFlag) : m_id(id), m_trackIndex(trackIndex),
           m_xPosition(xPosition), m_yPosition(yPosition),m_zPosition(zPosition),
           m_driftDistance(driftDistance), m_depositEnergy(depositEnergy), m_posFlag(posFlag)
{ }

// Get associated id
Identifier Event::MdcMcHit::identify() const {
  return  m_id;
}
 
// Get the associated track id 
unsigned int Event::MdcMcHit::getTrackIndex() const {
  return m_trackIndex;
}

// Get the position x 
double Event::MdcMcHit::getPositionX() const {
  return m_xPosition;
}

// Get the position y 
double Event::MdcMcHit::getPositionY() const {
  return m_yPosition;
}

// Get the position z 
double Event::MdcMcHit::getPositionZ() const {
  return m_zPosition;
}
  
// Get Drift Distance 
double Event::MdcMcHit::getDriftDistance() const {
  return m_driftDistance;
}

// Get deposited energy 
double Event::MdcMcHit::getDepositEnergy() const {
  return m_depositEnergy;
}

//Get the position flag
int Event::MdcMcHit::getPositionFlag() const {
  return m_posFlag;
}

