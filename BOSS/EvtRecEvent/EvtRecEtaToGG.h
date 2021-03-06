#ifndef Event_EvtRecEtaToGG_H
#define Event_EvtRecEtaToGG_H

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "EvtRecEvent/EvtRecTrack.h"

extern const CLID& CLID_EvtRecEtaToGG;

class EvtRecEtaToGG : virtual public ContainedObject {

   public :

      virtual const CLID& clID() const { return EvtRecEtaToGG::classID(); }
      static  const CLID& classID() { return CLID_EvtRecEtaToGG; }

      EvtRecEtaToGG();
      ~EvtRecEtaToGG() {;}

      EvtRecEtaToGG(const EvtRecEtaToGG& aEtaToGG);
      EvtRecEtaToGG& operator=(const EvtRecEtaToGG& aEtaToGG);

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

typedef ObjectVector<EvtRecEtaToGG> EvtRecEtaToGGCol;

#endif
