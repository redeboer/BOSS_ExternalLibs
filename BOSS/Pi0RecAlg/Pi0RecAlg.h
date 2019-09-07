#ifndef Physics_Analysis_Pi0Rec_Alg_H
#define Physics_Analysis_Pi0Rec_Alg_H 

#include "CLHEP/Vector/LorentzVector.h"
#include "GaudiKernel/Algorithm.h"
#include "Pi0Cut.h"
// predeclaration
class RecEmcShower;

class Pi0RecAlg : public Algorithm {

   public:

      Pi0RecAlg(const std::string& name, ISvcLocator* pSvcLocator);

      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();  

   private:

      HepLorentzVector getP4(RecEmcShower* gTrk);
      UserPi0Cut _pi0_cut;
      /*      double m_barrel_min_energy;
	      double m_endcap_min_energy;
	      double m_angle ;
	      double m_time_limit;

	      double m_minmass_cut;
	      double m_maxmass_cut;
	      double m_chisq_cut;
	      double m_costheta_cut;*/

};
#endif
