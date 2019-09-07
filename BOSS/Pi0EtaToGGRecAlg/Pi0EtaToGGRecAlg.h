#ifndef Physics_Analysis_Pi0EtaToGGRec_Alg_H
#define Physics_Analysis_Pi0EtaToGGRec_Alg_H 

#include "CLHEP/Vector/LorentzVector.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"

// predeclaration
class RecEmcShower;
class EvtRecTrack;

class Pi0EtaToGGRecAlg : public Algorithm {

   public:

      Pi0EtaToGGRecAlg(const std::string& name, ISvcLocator* pSvcLocator);

      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();  

   private:


      /// Functions
      HepLorentzVector getP4(RecEmcShower* gTrk);

      bool validPhoton(EvtRecTrack* recTrk, 
		       SmartDataPtr<EvtRecEvent> recEvt, 
		       SmartDataPtr<EvtRecTrackCol> recTrkCol);
      
      bool bothInEndcap(HepLorentzVector g1_P4, HepLorentzVector g2_P4);

      
      /////////////////////////
      //// declareProperty cuts

      /// reject pi0/eta candidate if both photon children are in endcap
      bool m_rejectEndEnd;

      /// pi0 cuts
      double m_pi0minmass_cut;
      double m_pi0maxmass_cut;
      double m_pi0chisq_cut;

      /// eta cuts
      double m_etaminmass_cut;
      double m_etamaxmass_cut;
      double m_etachisq_cut;
    
      /// Individual photon cuts
      double m_minEnergy;

      bool   m_useBarrelEndcap;
      double m_maxCosThetaBarrel;
      double m_minCosThetaEndcap;
      double m_maxCosThetaEndcap;
      double m_minEndcapEnergy;

      bool   m_applyTimeCut;
      double m_minTime;
      double m_maxTime;
      double m_deltaTime;

      bool m_applyDangCut;
      double m_minDang;

      int    m_maxNGoodPhoton;
      int    m_maxNPi0;
      //int    m_maxNEta;

      bool   m_doClear;
};
#endif
