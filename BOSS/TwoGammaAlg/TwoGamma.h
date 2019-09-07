#ifndef Physics_analysis_TwoGamma_H
#define Physics_Analysis_TwoGamma_H

//#include "GaudiKnernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
class TwoGamma : public Algorithm
{
	public:
		TwoGamma(const std::string& name, ISvcLocator* pSvcLocator);
		StatusCode initialize();
		StatusCode execute();
		StatusCode finalize();
	private:
		//  event selection
		//Lum output
		double m_max1;
		double m_max2;
		double m_costheta;

		double m_dphi1;
		double m_dphi2;
		double m_eff;
		double m_sec;
		//Lum output

		int m_event;
		double m_ecms;

		double m_pass[10];

		int Ndata1;
		int Ndata2;
		int m_runNo;

		NTuple::Tuple*  m_tuple1;
		NTuple::Item<double>  m_ngood;
		NTuple::Item<double>  m_nchrg;

		NTuple::Item<long>  m_run;
		NTuple::Item<long>  m_rec;
                NTuple::Item<long>  m_time;

		NTuple::Item<double>  m_e1;
		NTuple::Item<double>  m_e2;
		NTuple::Item<double>  m_e;
		NTuple::Item<double>  m_costheta1;
		NTuple::Item<double>  m_costheta2;
		NTuple::Item<double>  m_dltphi;
		NTuple::Item<double>  m_dltphi_1;
		NTuple::Item<double>  m_dlttheta;
		NTuple::Item<double>  m_phi1;
		NTuple::Item<double>  m_phi2;

		NTuple::Item<double>  m_e1_lab;
		NTuple::Item<double>  m_e2_lab;
		NTuple::Item<double>  m_e_lab;
		NTuple::Item<double>  m_costheta1_lab;
		NTuple::Item<double>  m_costheta2_lab;
		NTuple::Item<double>  m_dltphi_lab;
		NTuple::Item<double>  m_dlttheta_lab;
		NTuple::Item<double>  m_phi1_lab;
		NTuple::Item<double>  m_phi2_lab;

		NTuple::Item<double>  m_xBoost;
		NTuple::Item<double>  m_yBoost;
		NTuple::Item<double>  m_zBoost;
};
#endif
