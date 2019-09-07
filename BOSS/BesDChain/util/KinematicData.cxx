#include <cmath>
#include "BesDChain/util/KinematicData.h"

using DecayChain::KinematicData;

KinematicData::KinematicData()
:m_userTag(0),
 m_charge(0)
#ifdef EXTEND
,m_hasbeenbakup(false)
#endif
{
}

KinematicData::KinematicData(const HepLorentzVector& p4) :
m_userTag(0),
m_charge(0),
    m_p4(p4)
#ifdef EXTEND
,m_hasbeenbakup(false)
#endif
{
#ifdef EXTEND
    bakup();
#endif
}

void KinematicData::setUserTag( const int tag ) {
    m_userTag = tag;
}

void KinematicData::setCharge( const int aCharge ) {
    m_charge = aCharge;
}

void KinematicData::setP4( const HepLorentzVector& aMomentum ) 
{
    m_p4 = aMomentum;
#ifdef EXTEND
    bakup();
#endif
}

int KinematicData::userTag() const {
    return m_userTag;
}

int KinematicData::charge() const {
    return m_charge;
}

double KinematicData::mass() const {
    return m_p4.m();
}

double KinematicData::energy() const {
    return m_p4.e();
}

const HepLorentzVector& KinematicData::p4() const {
    return m_p4;
}
