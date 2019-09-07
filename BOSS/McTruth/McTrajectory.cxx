#include <iostream>
#include <math.h>
#include "McTruth/McTrajectory.h"

namespace Event{

/// Retrieve pointer to McParticle (const or non-const)
const McParticle* McTrajectory::getMcParticle() const {
    return m_mcParticle; 
}
McParticle* McTrajectory::getMcParticle() {
    return m_mcParticle; 
}

/// Update pointer to McParticle (by a C++ pointer or a smart reference)
void McTrajectory::setMcParticle( McParticle* value ){
    m_mcParticle = value; 
}

void McTrajectory::setMcParticle( SmartRef<McParticle> value ) { 
    m_mcParticle = value; 
}

void McTrajectory::addPoints(std::vector<Hep3Vector>& points) {
  m_points = points;
}

}
