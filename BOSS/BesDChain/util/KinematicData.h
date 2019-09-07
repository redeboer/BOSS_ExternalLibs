#ifndef DECAYCHAIN_KINEMATICDATA_H
#define DECAYCHAIN_KINEMATICDATA_H

#include "CLHEP/Vector/LorentzVector.h"

using CLHEP::HepLorentzVector;

namespace DecayChain {

class KinematicData
{
   public:
      KinematicData();
      KinematicData(const HepLorentzVector& p4);

      void setUserTag( const int tag );
      void setCharge( const int aCharge );
      //void setMass( const double aMass );
      void setP4( const HepLorentzVector& aMomentum );
#ifdef EXTEND
      void recover() const { m_p4 = m_p4_bakup; }
#endif

      int    userTag() const;
      int    charge() const;
      double mass() const;
      double energy() const;
      //const Hep3Vector& momentum() const;
      const HepLorentzVector& p4() const;

   private:
      int              m_userTag;
      int              m_charge;
      mutable HepLorentzVector m_p4;
#ifdef EXTEND
      void bakup() { if(m_hasbeenbakup) return; m_p4_bakup = m_p4; m_hasbeenbakup=true; }
      HepLorentzVector m_p4_bakup;
      bool m_hasbeenbakup;
#endif
};
}

#endif
