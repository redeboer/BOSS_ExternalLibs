/*****************************************************************************
 * Project: PID for psi(3770) data                                           *
 * Package: SimplePIDSvc                                                     *
 *    File: SimplePIDSvc.h    $v8   2013/11/12$                              *
 * Authors:                                                                  *
 *   Chunlei LIU, chl56@andrew.cmu.edu                                       *
 *   Tian MA,     mat@ihep.ac.cn                                             *
 *                                                                           *
 *****************************************************************************/

#ifndef SIMPLE_PID_SVC_H
#define SIMPLE_PID_SVC_H

#include "GaudiKernel/Service.h"
#include "SimplePIDSvc/ISimplePIDSvc.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"

class IDataProviderSvc;
template <class TYPE> class CnvFactory;

class SimplePIDSvc : public Service, virtual public ISimplePIDSvc
{
	friend class CnvFactory<SimplePIDSvc>;

public:
	SimplePIDSvc(const std::string& name, ISvcLocator* svcLoc);
	virtual ~SimplePIDSvc();

	virtual StatusCode initialize();
	virtual StatusCode finalize();
	virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

	void    setdedxminchi(double x) { m_dedx_chi_cut = x; }
	void    settofminchi(double x)  { m_tof_chi_cut = x; }
	void    seteidratio(double x)   { m_eid_ratio = x; }
	void    preparePID(EvtRecTrack* track);
	bool    iselectron(bool emc = true);
	bool    ispion();
	bool    iskaon();
	// probabilities (dE/dx + TOF)
	double  probElectron() { return m_prob[0]; }
	double  probMuon()     { return m_prob[1]; }
	double  probPion()     { return m_prob[2]; }
	double  probKaon()     { return m_prob[3]; }
	double  probProton()   { return m_prob[4]; }

	double  getdEdxChi(int i) { return m_dedx_chi[i]; }
	double  getTOFChi(int i)  { return m_tof_chi[i]; }
	double  getEmcLikelihood(int i) { return m_emc_likelihood[i]; }
	double  getChi2(int i);

	// get TOF&EMC info for internal test & check
	double  getTofDt(int m, int n)  { return m_tof_dt[m][n]; }
	double  getTofPh(int n)         { return m_tof_ph[n]; }
	double  getTofZR(int n)         { return m_tof_zr[n]; }
	int     getTofCounter(int n)    { return m_tof_counter[n]; }
	int     getTofBarrel()          { return m_tof_barrel; }
	double  getEmcE()               { return m_emc_e; }
	double  getEmcE13()             { return m_emc_e13; }
	double  getEmcE35()             { return m_emc_e35; }
	double  getEmcLatMoment()       { return m_emc_lat; }
	double  getEmcSecMoment()       { return m_emc_sec; }
	double  getElectronLikelihood() { return m_lh_electron; }

private:
	int     getRunIdx(int run_no);
	int     findBin(double *a, int length, double value);
	void    loadDedxInfo(EvtRecTrack *track);
	void    loadTOFInfo(EvtRecTrack *track);
	void    loadEMCInfo(EvtRecTrack *track);
	void    loadHistogram();
	void    dedxCorrection();
	void    tofBarrelCorrection();
	void    tofEndcapCorrection();
	void    calprob();
	bool    calEMCLikelihood();

	// variables in job_options
	bool   m_dedx_corr;
	bool   m_tof_corr;
	double m_dedx_chi_cut;
	double m_tof_chi_cut;
	double m_eid_ratio;

	IDataProviderSvc*  eventSvc_;
	int    m_run;  // run number

	bool   m_dedx_only[5];
	double m_p[5]; 
	double m_cost[5];
	int    m_charge[5];
	double m_betagamma[5];

	// TOF info
	double m_tof_dt[8][5];
	double m_tof_ph[8];
	double m_tof_zr[2];
	int    m_tof_counter[2];
	volatile int m_tof_barrel;
	// EMC info
	double m_emc_e;
	double m_emc_eop[5];
	double m_emc_e13;
	double m_emc_e35;
	double m_emc_sec;
	double m_emc_lat;

	// chisquares & probilites for each hypothesis
	double m_dedx_chi[5];
	double m_tof_chi[5];
	double m_emc_likelihood[5];
	double m_prob[5];
	double m_lh_electron;
	
	// dedx correction histogram 
	// (p, m)
	// p: 0-electron 1-k,pi
	// m: 0-10 data, 1-11 data, 2-10 mc, 3-11 mc
	TH2D *h_dedx_p_offset[2][4];
	TH2D *h_dedx_p_sigma[2][4];
	TH2D *h_dedx_m_offset[2][4];
	TH2D *h_dedx_m_sigma[2][4];

	// tof barrel correction histogram
	// (p, m, n)
	// p: 0-electron 1-k,pi
	// m: 0-10 data, 1-11 data, 2-10 mc, 3-11 mc
	// n: 0-inner east, 1-inner west, 2-outer east, 3-outer west
	TH1D *h_tof_p_q_offset[2][4][4];
	TH1D *h_tof_p_q_sigma[2][4][4];
	TH1D *h_tof_m_q_offset[2][4][4];
	TH1D *h_tof_m_q_sigma[2][4][4];
	TH2D *h_tof_p_bgcost_offset[2][4][4];
	TH2D *h_tof_p_bgcost_sigma[2][4][4];
	TH2D *h_tof_m_bgcost_offset[2][4][4];
	TH2D *h_tof_m_bgcost_sigma[2][4][4];
	
	TH2D *h_tof_p_wgt[2][4][15][5];
	TH2D *h_tof_m_wgt[2][4][15][5];
	TH2D *h_tof_p_final_offset[2][4][15];
	TH2D *h_tof_p_final_sigma[2][4][15];
	TH2D *h_tof_m_final_offset[2][4][15];
	TH2D *h_tof_m_final_sigma[2][4][15];

	// tof endcap correction histogram
	// (p, m, n)
	// p: 0-electron 1-k,pi
	// m: 0-10 data, 1-11 data, 2-10 mc, 3-11 mc
	// n: 0-east, 1-west
	TH1D *h_tofec_p_q_offset[2][4][2];
	TH1D *h_tofec_p_q_sigma[2][4][2];
	TH1D *h_tofec_m_q_offset[2][4][2];
	TH1D *h_tofec_m_q_sigma[2][4][2];
	TH1D *h_tofec_p_bg_offset[2][4][2];
	TH1D *h_tofec_p_bg_sigma[2][4][2];
	TH1D *h_tofec_m_bg_offset[2][4][2];
	TH1D *h_tofec_m_bg_sigma[2][4][2];
	TH1D *h_tofec_p_cost_offset[2][4][2];
	TH1D *h_tofec_p_cost_sigma[2][4][2];
	TH1D *h_tofec_m_cost_offset[2][4][2];
	TH1D *h_tofec_m_cost_sigma[2][4][2];

	// emc histogram
	// (p, q, m, n)
	// p: 0-electron, 1-pion, 2-kaon
	// q: 0-10 data, 1-11 data, 2-10 mc, 3-11 mc
	// m: 15 momentum-bin
	// n: 25 cost-bin
	TH1D *h_emc_ep[3][4][15][25];
	TH1D *h_emc_e35[3][4][15][25];

	// TFiles
	TFile *f_dedx[2][4];
	TFile *f_tof_q[2][4];
	TFile *f_tof_bgcost[2][4];
	TFile *f_tof_wgt[2][4];
	TFile *f_tof_final[2][4];
	TFile *f_tofec_q[2][4];
	TFile *f_tofec_bg[2][4];
	TFile *f_tofec_cost[2][4];
	TFile *f_emc[3][4];
};

#endif //SIMPLE_PID_SVC_H
