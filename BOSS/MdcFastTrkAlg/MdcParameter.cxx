#include "MdcFastTrkAlg/MdcParameter.h"
#include "GaudiKernel/Bootstrap.h"

MdcParameter* MdcParameter::s_paramInstance = 0;

MdcParameter* MdcParameter::instance() {
	//return a pointer to an exclusive instance of MdcParameter
	if (!s_paramInstance) s_paramInstance = new MdcParameter();
	return s_paramInstance;
}

MdcParameter::MdcParameter():
	_findEventVertex(1),
	_evtTimeCorr(1),
	_minPt(0.07), // minPt cut on r_phiFit
	_minDr(7.5), //dr cut on r_phiFit
	_t0OffSet(0.),
	_xtCoEff(0.0344), // approx. quadric relation between drift distance and time
	_doIt(1),
	_mkMdst(true),
	_mkTds(true),
	_alpha(333.564095),  //1.0Tesla
	_mfield(-0.0010),

	//control parameter in r-phi segment linking
	_chi2_kappa(4000.),  //
	_Min_chi2(5000.),    // (chi2-chi2_kappa) < Min_chi2 in track2D finding
	_deltaPhi(31.82*M_PI/180),  // seems not used

	_D_phi1(6.53*M_PI/180), // linkAxialSegments_2_34
	_D_phi2(4.45*M_PI/180.),// linkAxialSegments_3_4
	_D_phi3(3.08*M_PI/180.),// linkAxialSegments_9_10
	_chi2_1(0.039*3),  // linkAxialSegments_2_34
	_chi2_2(0.039*3), // linkAxialSegments_3_4
	_chi2_3(0.039*3), // linkAxialSegments_9_10

	_chi2_segfit(500), //reduce_noise

	_hitscut(1), //2D track re-fitting

	//parameter restriction on s_zFit
	_nseg(2), //number of stereo SL segments in s_zFit
	_nlength(2), //number of total hits 
	_nc(2),
	_z_cut1(7.), 
	_z_cut2(4.)
{
}

MdcParameter::~MdcParameter(){
}

