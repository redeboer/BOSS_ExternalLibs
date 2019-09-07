#ifndef GIGAMALG_H
#define GIGAMALG_H
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "BeamEnergySvc/BeamEnergySvc.h"
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "TH1F.h"
class ITHistSvc;
class IBeamEnergySvc;
class DiGam:public Algorithm {
	public:
		DiGam(const std::string& name, ISvcLocator* pSvcLocator);
		StatusCode initialize();
		StatusCode execute();
		StatusCode finalize();

	private:
	        double CrossSection;
		double MCEff;
		double MCEffBoost;

		double jpsiCrossSection;
		double jpsiMCEff;
		double jpsiMCEffBoost;
		double psi2sCrossSection;
		double psi2sMCEff;
		double psi2sMCEffBoost;
		double psi3770CrossSection;
		double psi3770MCEff;
		double psi3770MCEffBoost;
	
	        int RunModel;
		ITHistSvc* m_thsvc;
	        TH1F* h_lum; 
		IBeamEnergySvc *m_BeamEnergySvc;
	        
		int tot;
		int signal;
	        int runNo;
		int boost_signal;
		int boost_tot;

		//cut:
		double boostPhimin;
		double boostPhimax;
		double boostMinEmin;
		double boostMinEmax;
		double dPhiMin;
		double dPhiMax;
		double dPhiMinSig;
		double dPhiMaxSig;
		
		bool flag;
		double E_cms;

		NTuple::Tuple*  m_tuple2;
		NTuple::Item<double> m_tot;
                NTuple::Item<double> m_maxE;
		NTuple::Item<double> m_minE;
		NTuple::Item<double> m_maxTheta;
		NTuple::Item<double> m_minTheta;
		NTuple::Item<double> m_maxPhi;
		NTuple::Item<double> m_minPhi;
		NTuple::Item<double> m_delPhi;
		NTuple::Item<double> m_delTheta;
                NTuple::Item<double> m_angle;
		NTuple::Item<double> m_boostAngle;
		NTuple::Item<double> m_boostMaxE;
		NTuple::Item<double> m_boostMinE;
		NTuple::Item<double> m_boostDelPhi;
		NTuple::Item<double> m_boostDelTheta;
		NTuple::Item<double> m_boostM;
		NTuple::Item<double> m_boostIM;
		NTuple::Item<double> m_m;
		NTuple::Item<double> m_IM;
		
		NTuple::Item<long>   m_run;
		NTuple::Item<long>   m_rec;
		NTuple::Item<long>   m_idxmc;
		NTuple::Array<long>  m_pdgid;
		NTuple::Array<long>  m_motheridx;
};
#endif
