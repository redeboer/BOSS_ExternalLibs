/**
 * @class McPrimaryParticle
 *
 * @brief Implements the methods described in McPrimaryParticle.h
 *
 */
#include "McTruth/McPrimaryParticle.h"

Event::McParticleRefVec Event::McPrimaryParticle::getTrackVector()
{
/*    Event::McParticleRefVec trackVec;

    trackVec.clear();

    if (m_primary->statusFlags() & Event::McParticle::POSHIT) trackVec.push_back(m_primary);

    Event::McParticleRefVec::const_iterator refIter;

    for(refIter = m_secondaries.begin(); refIter != m_secondaries.end(); refIter++) trackVec.push_back(*refIter);
    for(refIter = m_associated.begin();  refIter != m_associated.end();  refIter++) trackVec.push_back(*refIter);

    return trackVec;
*/
}
