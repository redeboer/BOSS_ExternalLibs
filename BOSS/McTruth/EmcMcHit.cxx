#include "McTruth/EmcMcHit.h"

Event::EmcMcHit::EmcMcHit(const Identifier& id, unsigned int trackIndex, double xPosition,
    double yPosition, double zPosition, double px, double py,
    double pz, double depositEnergy)
        : m_id(id), m_trackIndex(trackIndex),
        m_hitEmc(0),m_PDGCode(0),m_PDGCharge(0),m_time(0),
        m_xPosition(xPosition), m_yPosition(yPosition),m_zPosition(zPosition),
        m_px(px), m_py(py), m_pz(pz), m_depositEnergy(depositEnergy)
{
  m_hitMap.clear();
}
                               
// Get associated id
Identifier Event::EmcMcHit::identify() const {
  return  m_id;
}
 
// Get the associated track id 
unsigned int Event::EmcMcHit::getTrackIndex() const {
  return m_trackIndex;
}

// Whether hit Emc
int Event::EmcMcHit::getHitEmc() const {
  return m_hitEmc;
}

// Get PDG code
int Event::EmcMcHit::getPDGCode() const {
  return m_PDGCode;
}

// Get PDG charge
double Event::EmcMcHit::getPDGCharge() const {
  return m_PDGCharge;
}

// Get time
double Event::EmcMcHit::getTime() const {
  return m_time;
}

// Get the position x 
double Event::EmcMcHit::getPositionX() const {
  return m_xPosition;
}

// Get the position y 
double Event::EmcMcHit::getPositionY() const {
  return m_yPosition;
}

// Get the position z 
double Event::EmcMcHit::getPositionZ() const {
  return m_zPosition;
}
  
// Get momentum Px 
double Event::EmcMcHit::getPx() const {
  return m_px;
}

// Get momentum Py  
double Event::EmcMcHit::getPy() const {
  return m_py; 
}

// Get momentum Pz  
double Event::EmcMcHit::getPz() const {
  return m_pz; 
}
  
// Get the total deposited energy 
double Event::EmcMcHit::getDepositEnergy() const {
   return m_depositEnergy;
}

std::map<Identifier,double>::const_iterator Event::EmcMcHit::begin() const {
  return m_hitMap.begin();
}

std::map<Identifier,double>::const_iterator Event::EmcMcHit::end() const {
  return m_hitMap.end();
}

std::map<Identifier,double>::const_iterator Event::EmcMcHit::find(Identifier& id) const {
  return m_hitMap.find(id);
}

unsigned int Event::EmcMcHit::size() const {
  return m_hitMap.size();
}

std::map<Identifier,double> Event::EmcMcHit::getHitMap() const {
  return m_hitMap;
}
