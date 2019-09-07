#ifndef Event_EvtRecPi0_H
#define Event_EvtRecPi0_H

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "EvtRecEvent/EvtRecTrack.h"

extern const CLID& CLID_EvtRecPi0;

class EvtRecPi0 : virtual public ContainedObject {

   public :

      virtual const CLID& clID() const { return EvtRecPi0::classID(); }
      static  const CLID& classID() { return CLID_EvtRecPi0; }

      EvtRecPi0();
      ~EvtRecPi0() {;}

      EvtRecPi0(const EvtRecPi0& aPi0);
      EvtRecPi0& operator=(const EvtRecPi0& aPi0);

      double unconMass() const { return m_unconMass; }
      double chisq() const { return m_chisq; }  

      HepLorentzVector hiPfit() const { return m_hiPfit; }
      HepLorentzVector loPfit() const { return m_loPfit; }

      const EvtRecTrack* hiEnGamma() const { return m_hi; }
      const EvtRecTrack* loEnGamma() const { return m_lo; }


      void setUnconMass(const double unconMass) { m_unconMass = unconMass; }
      void setChisq(const double chisq) { m_chisq = chisq; }

      void setHiPfit(const HepLorentzVector& hiPfit) { m_hiPfit = hiPfit; }
      void setLoPfit(const HepLorentzVector& loPfit) { m_loPfit = loPfit; }

      void setHiEnGamma(const EvtRecTrack* trk) { m_hi = trk; }
      void setLoEnGamma(const EvtRecTrack* trk) { m_lo = trk; }


   private :

      double            m_unconMass;

      double            m_chisq;

      HepLorentzVector  m_hiPfit;
      HepLorentzVector  m_loPfit;

      SmartRef<EvtRecTrack> m_hi;
      SmartRef<EvtRecTrack> m_lo;

};

typedef ObjectVector<EvtRecPi0> EvtRecPi0Col;

#endif
