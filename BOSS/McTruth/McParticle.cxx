#include <iostream>
#include "McTruth/McParticle.h"

namespace Event {

/// Retrieve particle property
McParticle::StdHepId McParticle::particleProperty() const
{
  return m_particleID;
}

/// Retrieve whether this is a primary particle
bool McParticle::primaryParticle() const
{
  return (m_statusFlags & PRIMARY)==PRIMARY;
}

/// Retrieve whether this is a leave particle
bool McParticle::leafParticle() const
{
  return (m_statusFlags & LEAF)==LEAF;
}

/// Retrieve whether decayed from generator
bool McParticle::decayFromGenerator() const
{
  return (m_statusFlags & DECAYED)==DECAYED;
}

/// Retrieve wheter decayed in flight
bool McParticle::decayInFlight() const
{
  return (m_statusFlags & DECAYFLT)==DECAYFLT;
}

void McParticle::init( McParticle* mother,         
	StdHepId id, 
        unsigned int flags,
        const HepLorentzVector& initialMomentum,
        const HepLorentzVector& finalMomentum,
        const HepLorentzVector& initialPosition,
        const HepLorentzVector& finalPosition, 
        const std::string process)
{
  initialize(mother, id, flags, initialMomentum, initialPosition,process);
  //finalize(finalMomentum, finalPosition);
  finalize(finalPosition);
}

void McParticle::initialize(      
	StdHepId id, 
        unsigned int flags,
        const HepLorentzVector& initialMomentum,
        const HepLorentzVector& initialPosition, const std::string process)
{
    m_particleID = id;
    m_statusFlags = flags;
    m_initialFourMomentum = initialMomentum;
    m_initialPosition = initialPosition;
    m_process = process;
}

void McParticle::initialize( McParticle* mother,         
	StdHepId id, 
        unsigned int flags,
        const HepLorentzVector& initialMomentum,
        const HepLorentzVector& initialPosition, const std::string process)
{
    m_mother = mother;
    m_particleID = id;
    m_statusFlags = flags;
    m_initialFourMomentum = initialMomentum;
    m_initialPosition = initialPosition;
    m_process = process;
    if( mother != this) mother->m_daughters.push_back(this);
}

/*void McParticle::finalize(const HepLorentzVector& finalMomentum,
        const HepLorentzVector& finalPosition)
{
    m_finalFourMomentum = finalMomentum;
    m_finalPosition = finalPosition;
}*/

void McParticle::finalize(const HepLorentzVector& finalPosition)
{
    m_finalPosition = finalPosition;
}

void McParticle::setMother(const SmartRef<McParticle> m)
{
  m_mother = m;
}

const HepLorentzVector& McParticle::initialPosition()const
{
    return m_initialPosition;
}
const HepLorentzVector& McParticle::finalPosition()const
{
    return m_finalPosition;
}
const HepLorentzVector&  McParticle::initialFourMomentum()const
{
    return m_initialFourMomentum;
}
/*const HepLorentzVector&  McParticle::finalFourMomentum()const
{
    return m_finalFourMomentum;
}*/

unsigned int McParticle::statusFlags()const{
    return m_statusFlags;
}

/// access to the mother particle
const McParticle& McParticle::mother()const
{
    return *m_mother;
}

/// Remove daughters when in prune mode
void McParticle::removeDaughter(const SmartRef<McParticle> mcPart)
{
    SmartRefVector<Event::McParticle>::iterator daughtIter;
    for(daughtIter = m_daughters.begin();daughtIter != m_daughters.end();daughtIter++)
    {
        if (mcPart == *daughtIter)
        {
            m_daughters.erase(daughtIter);
            break;
        }
    }
    return;
}

} // namespace Event
