#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EventModel/EventHeader.h"
#include "SimplePIDSvc/SimplePIDSvc.h"

#include "DstEvent/TofHitStatus.h"
#include "TMath.h"
#include "TFile.h"
#include "TMatrixD.h"
#include "TArray.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
using namespace std;

SimplePIDSvc::SimplePIDSvc(const std::string& name, ISvcLocator* svcLoc) : Service(name, svcLoc)
{
	declareProperty("DedxChiCut",       m_dedx_chi_cut           = 4);
	declareProperty("TofChiCut",        m_tof_chi_cut            = 4);
	declareProperty("IsTofCorr",        m_tof_corr               = true);
	declareProperty("IsDedxCorr",       m_dedx_corr              = true);
	declareProperty("EidRatio",         m_eid_ratio              = 0.80);
}

SimplePIDSvc::~SimplePIDSvc() {;}

StatusCode SimplePIDSvc::initialize()
{
	MsgStream log(messageService(), name());
	log << MSG::INFO << "in SimplePIDSvc initialize()" << endreq;

	StatusCode sc = Service::initialize();

	sc = serviceLocator()->service("EventDataSvc", eventSvc_, true);

	loadHistogram();

	return sc;
}

StatusCode SimplePIDSvc::finalize()
{
	MsgStream log(messageService(), name());
	log << MSG::INFO << "in SimplePIDSvc finalize()" << endreq;

	StatusCode sc = Service::finalize();

	for (unsigned int i = 0; i < 2; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			f_dedx[i][j]->Close();
			f_tof_q[i][j]->Close();
			f_tof_bgcost[i][j]->Close();
			f_tof_wgt[i][j]->Close();
			f_tof_final[i][j]->Close();
			f_tofec_q[i][j]->Close();
			f_tofec_bg[i][j]->Close();
			f_tofec_cost[i][j]->Close();
		}
	}
	for (unsigned int i = 0; i < 3; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			f_emc[i][j]->Close();
		}
	}

	return sc;
}

StatusCode SimplePIDSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
	if (ISimplePIDSvc::interfaceID().versionMatch(riid)) 
	{
		*ppvIF = dynamic_cast<ISimplePIDSvc*>(this);
	}
	else 
	{
		return Service::queryInterface(riid, ppvIF);
	}
	addRef();
	return StatusCode::SUCCESS;
}

void SimplePIDSvc::preparePID(EvtRecTrack* track)
{

	SmartDataPtr<Event::EventHeader> eventHeaderpid(eventSvc_, "/Event/EventHeader");
	m_run = eventHeaderpid->runNumber();

	if (track->isMdcKalTrackValid())
	{    
		RecMdcKalTrack *mdckalTrk = track->mdcKalTrack();
		RecMdcKalTrack::PidType trk_type[5] = {
			RecMdcKalTrack::electron,
			RecMdcKalTrack::muon,
			RecMdcKalTrack::pion,
			RecMdcKalTrack::kaon,
			RecMdcKalTrack::proton,
		};
		double mass[5] = {
			0.000511,
			0.105658,
			0.13957,
			0.493677,
			0.938272,
		};
		for(unsigned int pid = 0; pid < 5; pid++)
		{
			mdckalTrk->setPidType(trk_type[pid]);
			m_p[pid]         = mdckalTrk->p();
			m_betagamma[pid] = m_p[pid] / mass[pid];
			m_charge[pid]    = mdckalTrk->charge();
			m_cost[pid]      = cos(mdckalTrk->theta());
		}
	}
	else
	{
		for(unsigned int i = 0; i < 5; i++)
		{
			m_p[i]         = -99;
			m_betagamma[i] = -99;
			m_cost[i]      = -99;
			m_charge[i]    = 0;
		}

	}

	//dE/dx PID
	loadDedxInfo(track);
	if (m_dedx_corr)
	{
		dedxCorrection();
	}
	//TOF PID
	loadTOFInfo(track);
	if (m_tof_corr)
	{
		if (m_tof_barrel == 1)
		{
			tofBarrelCorrection();
		}
		else if (m_tof_barrel == 0)
			tofEndcapCorrection();
	}
	//EMC
	loadEMCInfo(track);

	calprob();
}

void SimplePIDSvc::calprob()
{
	bool usededx = false;
	bool usetof  = false;

	for (unsigned int i = 0; i < 5 ;i++)
	{
		if (!usededx && fabs(m_dedx_chi[i]) < m_dedx_chi_cut)
			usededx = true;
		if (!usetof && fabs(m_tof_chi[i]) < m_tof_chi_cut)
			usetof = true;

		m_dedx_only[i] = false;
	}
	if (!usededx)
	{
		for(unsigned int i = 0; i < 5; i++)
			m_dedx_chi[i] = -99;
	}
	if (!usetof)
	{
		for(unsigned int i = 0; i < 5; i++)
			m_tof_chi[i] = -99;
	}

	for (unsigned int i = 0; i < 5; i++)
	{
		m_prob[i] = -99;
		double chi2 = 0;
		int ndf = 0;

		if (usededx && usetof)
		{
			chi2 = pow(m_dedx_chi[i], 2) + pow(m_tof_chi[i], 2);
			ndf  = 2;
		}
		else if (usededx && !usetof)
		{
			chi2 = pow(m_dedx_chi[i], 2);
			ndf  = 1;
			m_dedx_only[i] = true;
		}
		else if (!usededx && usetof)
		{
			chi2 = pow(m_tof_chi[i],2);
			ndf  = 1;
		}
		if (ndf > 0 && chi2 > 0)
			m_prob[i] = TMath::Prob(chi2, ndf);
	}
}



int SimplePIDSvc::getRunIdx(int run_no)
{
	// -1: beyond correction region
	// 0: 2010 psi(3770) data 
	// 1: 2011 psi(3770) data
	// 2: 2010 psi(3770) mc
	// 3: 2011 psi(3770) mc
	const int RUN_BEGIN_DATA_10 = 11414; 
	const int RUN_END_DATA_10   = 14604;
	const int RUN_BEGIN_MC_10   = -14604;
	const int RUN_END_MC_10     = -11414;
	const int RUN_BEGIN_DATA_11 = 20448;
	const int RUN_END_DATA_11   = 23454;
	const int RUN_BEGIN_MC_11   = -23454;
	const int RUN_END_MC_11     = -20448;
	if (run_no >= RUN_BEGIN_DATA_10 && run_no <= RUN_END_DATA_10)
		return 0;
	else if (run_no >= RUN_BEGIN_DATA_11 && run_no <= RUN_END_DATA_11)
		return 1;
	else if (run_no >= RUN_BEGIN_MC_10 && run_no <= RUN_END_MC_10)
		return 2;
	else if (run_no >= RUN_BEGIN_MC_11 && run_no <= RUN_END_MC_11)
		return 3;
	else
		return -1;
}

void SimplePIDSvc::loadTOFInfo(EvtRecTrack *track)
{
	//Initialization
	for (unsigned int i = 0; i < 8; i++)
	{
		for (unsigned int j = 0; j < 5; j++)
			m_tof_dt[i][j] = -99.;
		m_tof_ph[i] = -99.;
	}
	for (unsigned int i = 0; i < 2; i++)
	{
		m_tof_zr[i]      = -9999.;
		m_tof_counter[i] = -1;
	}
	for (unsigned int i = 0; i < 5; i++)
	{
		m_tof_chi[i] = -99.;
	}
	m_tof_barrel = -1;

	if (!track->isExtTrackValid() || !track->isTofTrackValid()) return;

	SmartRefVector<RecTofTrack> tofTrk = track->tofTrack();
	SmartRefVector<RecTofTrack>::iterator it;
	RecExtTrack* extTrk = track->extTrack();
	double zrhit[2];
	zrhit[0] = extTrk->tof1Position().z();
	zrhit[1] = extTrk->tof2Position().z();

	TofHitStatus *hitst = new TofHitStatus;

	for (it = tofTrk.begin(); it != tofTrk.end(); it++)
	{
		unsigned int st = (*it)->status();
		hitst->setStatus(st);
		if (hitst->is_raw()) continue;  //empty TOF hit
		bool barrel  = hitst->is_barrel();
		bool readout = hitst->is_readout();
		bool counter = hitst->is_counter();
		bool cluster = hitst->is_cluster();
		int  layer   = hitst->layer();
		double tof   = (*it)->tof();
		double ph    = (*it)->ph();
		m_tof_counter[layer-1] = (*it)->tofID();

		if (barrel)
		{
			m_tof_barrel = 1;
		}
		else
		{
			m_tof_barrel = 0;
			zrhit[0] = extTrk->tof1Position().rho();
		}
		m_tof_zr[0] = zrhit[0];
		m_tof_zr[1] = zrhit[1];

		int idx = -1;
		if (readout)
		{
			if (barrel) 
				idx = ((st & 0xC0) >> 5) + (((st ^ 0x20) & 0x20) >> 5) - 2; 
			else 
				idx = 7;
		}
		else if (counter) 
		{
			idx = layer + 3;
		}
		else if (cluster)
		{
			idx = 6;
		}
		if (idx == -1) continue;
		m_tof_ph[idx] = ph;
		for (unsigned int i = 0; i < 5; i++)
		{
			double offset = (*it)->toffset(i);
			double texp   = (*it)->texp(i);
			if (texp < 0.0) continue;
			double dt = tof - offset - texp;
			m_tof_dt[idx][i] = dt;
		}
		//cout << "barrel: " << barrel << "\t" << m_tof_barrel << "\t";
		//cout << "idx: "    << idx << "\t";
		//cout << "m_dt:"    << m_tof_dt[idx][2] << endl;
	}
	delete hitst; 
}

void SimplePIDSvc::tofBarrelCorrection()
{
	const double EPS      = 1e-4;
	const double BG_LOW   = 0.20;
	const double BG_UP    = 7.40;
	const double COST_LOW = -0.81;
	const double COST_UP  = 0.81;
	const double Q_LOW    = 0.;
	const double Q_UP     = 9000.;
	const double P_LOW    = 0.2;
	const double P_UP     = 1.3;
	const int    BIN_BG   = 15;
	const int    BIN_COST = 15;
	const int    BIN_P    = 15;
	double BG[BIN_BG + 1] = {0.20, 0.87, 1.11, 1.35, 1.55, 1.72, 1.91, 2.17, 2.63, 3.05, 3.47, 3.93, 4.50, 5.27, 6.00, 7.40};
	double COST[BIN_COST + 1] = {-0.81, -0.64, -0.53, -0.43, -0.33, -0.23, -0.13, -0.04, 0.05, 0.14, 0.24, 0.34, 0.44, 0.54, 0.65, 0.81};
	double P[BIN_P + 1] = {0.20, 0.47, 0.56, 0.65, 0.72, 0.79, 0.86, 0.92, 0.98, 1.03, 1.08, 1.13, 1.17, 1.22, 1.26, 1.30};
	int idx = getRunIdx(m_run);

	if (idx != -1)
	{
		for (unsigned int i = 0; i < 4; i++)
		{// only correct e, pi, K
			double bg;
			int bin_bg, bin_cost, bin_wgt;
			int pid;
			if (i == 0)
			{
				bg     = max(P_LOW+EPS, min(P_UP-EPS, m_p[i]));
				bin_bg = findBin(P, BIN_P, bg);
				pid    = 0;
			}
			else if (i == 2 || i == 3)
			{
				bg     = max(BG_LOW+EPS, min(BG_UP-EPS, m_betagamma[i]));
				bin_bg = findBin(BG, BIN_BG, bg);
				pid    = 1;
			}
			else
			{
				continue;
			}
			double cost   = m_cost[i];
			int    charge = m_charge[i];
			double t[5], q;
			double offset, sigma;
			double offset_q, offset_bgcost;
			int flag[4] = {0, 0, 0, 0, };
			cost = max(COST_LOW+EPS, min(COST_UP-EPS, cost));
			bin_cost = findBin(COST, BIN_COST, cost);
			if (bin_bg == -1 || bin_cost == -1) continue;

			//corrections
			for (unsigned int j = 0; j < 4; j++)
			{
				t[j] = m_tof_dt[j][i];
				if (fabs(t[j] + 99.) < EPS)//no readout
					flag[j] = 0;
				else
					flag[j] = 1;
				q = m_tof_ph[j];
				q = max(Q_LOW+EPS, min(Q_UP-EPS, q));
				if (charge == 1)
				{
					offset_q      = h_tof_p_q_offset[pid][idx][j]->Interpolate( q );
					offset_bgcost = h_tof_p_bgcost_offset[pid][idx][j]->Interpolate( bg, cost );
					t[j]          = t[j] - offset_q - offset_bgcost;
				}
				else	
				{
					offset_q      = h_tof_m_q_offset[pid][idx][j]->Interpolate( q );
					offset_bgcost = h_tof_m_bgcost_offset[pid][idx][j]->Interpolate( bg, cost );
					t[j]          = t[j] - offset_q - offset_bgcost;
				}
			}
			bin_wgt = flag[0]*8 + flag[1]*4 + flag[2]*2 + flag[3] - 1;
			if (bin_wgt == -1) continue;
			t[4] = 0;
			for (unsigned int j = 0; j < 4; j++)
			{
				if (charge == 1)
					t[4] += t[j] * h_tof_p_wgt[pid][idx][bin_wgt][j]->GetBinContent( bin_bg+1, bin_cost+1 );
				else
					t[4] += t[j] * h_tof_m_wgt[pid][idx][bin_wgt][j]->GetBinContent( bin_bg+1, bin_cost+1 );
			}
			if (charge == 1)
			{
				t[4] /= h_tof_p_wgt[pid][idx][bin_wgt][4]->GetBinContent( bin_bg+1, bin_cost+1 );
				offset = h_tof_p_final_offset[pid][idx][bin_wgt]->Interpolate( bg, cost );
				sigma  = h_tof_p_final_sigma[pid][idx][bin_wgt]-> Interpolate( bg, cost );
				m_tof_chi[i] = (t[4] - offset) / sigma;
			}
			else 
			{
				t[4] /= h_tof_m_wgt[pid][idx][bin_wgt][4]->GetBinContent( bin_bg+1, bin_cost+1 );
				offset = h_tof_m_final_offset[pid][idx][bin_wgt]->Interpolate( bg, cost );
				sigma  = h_tof_m_final_sigma[pid][idx][bin_wgt]-> Interpolate( bg, cost );
				m_tof_chi[i] = (t[4] - offset) / sigma;
			}
		}
	}
}

void SimplePIDSvc::tofEndcapCorrection()
{
	const double EPS           = 1e-4;
	const double BG_LOW        = 0.30;
	const double BG_UP         = 7.40;
	const double Q_LOW         = 0.;
	const double Q_UP          = 6000.;
	const double COST_EAST_LOW = 0.720;
	const double COST_EAST_UP  = 0.930;
	const double COST_WEST_LOW = -0.930;
	const double COST_WEST_UP  = -0.720;
	const double P_LOW         = 0.2;
	const double P_UP          = 1.3;

	int idx = getRunIdx(m_run);

	if (idx != -1)
	{
		for (unsigned int i = 0; i < 4; i++)
		{// only correct e, pi, K
			int pid;
			double bg;
			if (i == 0)
			{
				bg  = max(P_LOW+EPS, min(P_UP-EPS, m_p[i]));
				pid = 0;
			}
			else if (i == 2 || i == 3)
			{
				bg = max(BG_LOW+EPS, min(BG_UP-EPS, m_betagamma[i]));
				pid = 1;
			}
			else
			{
				continue;
			}

			int flag; //0:east, 1:west
			double cost   = m_cost[i];
			int    charge = m_charge[i];
			double t      = m_tof_dt[7][i];
			double q      = m_tof_ph[7];
			double off_q, off_bg, off_cost;
			double sg_q,  sg_bg,  sg_cost;
			if (cost > 0)
			{
				flag = 0;
				cost = max(COST_EAST_LOW+EPS, min(COST_EAST_UP-EPS, cost));
			}
			else
			{
				flag = 1;
				cost = max(COST_WEST_LOW+EPS, min(COST_WEST_UP-EPS, cost));
			}
			q  = max(Q_LOW+EPS, min(Q_UP-EPS, q));

			//corrections
			if (charge == 1)
			{
				off_q    = h_tofec_p_q_offset[pid][idx][flag]   ->Interpolate( q );
				sg_q     = h_tofec_p_q_sigma[pid][idx][flag]    ->Interpolate( q );
				off_bg   = h_tofec_p_bg_offset[pid][idx][flag]  ->Interpolate( bg );
				sg_bg    = h_tofec_p_bg_sigma[pid][idx][flag]   ->Interpolate( bg );
				off_cost = h_tofec_p_cost_offset[pid][idx][flag]->Interpolate( cost );
				sg_cost  = h_tofec_p_cost_sigma[pid][idx][flag] ->Interpolate( cost );
				m_tof_chi[i] = (((t - off_q) / sg_q - off_bg) / sg_bg - off_cost) / sg_cost;
			}
			else	
			{
				off_q    = h_tofec_m_q_offset[pid][idx][flag]   ->Interpolate( q );
				sg_q     = h_tofec_m_q_sigma[pid][idx][flag]    ->Interpolate( q );
				off_bg   = h_tofec_m_bg_offset[pid][idx][flag]  ->Interpolate( bg );
				sg_bg    = h_tofec_m_bg_sigma[pid][idx][flag]   ->Interpolate( bg );
				off_cost = h_tofec_m_cost_offset[pid][idx][flag]->Interpolate( cost );
				sg_cost  = h_tofec_m_cost_sigma[pid][idx][flag] ->Interpolate( cost );
				m_tof_chi[i] = (((t - off_q) / sg_q - off_bg) / sg_bg - off_cost) / sg_cost;
			}
		}
	}
}

void SimplePIDSvc::loadDedxInfo(EvtRecTrack *track)
{
	if (track->isMdcDedxValid())
	{
		RecMdcDedx* dedx_trk = track->mdcDedx();
		for (unsigned int i = 0; i < 5; i++)
			m_dedx_chi[i] = dedx_trk->chi(i);
	}
	else
	{
		for (unsigned int i = 0; i < 5; i++)
			m_dedx_chi[i] = -99;
	}
}

void SimplePIDSvc::dedxCorrection()
{
	int idx = getRunIdx(m_run);
	const double EPS      = 1e-4;
	const double BG_LOW   = 0.20;
	const double BG_UP    = 7.40;
	const double COST_LOW = -0.93;
	const double COST_UP  = 0.93;
	const double P_LOW    = 0.2;
	const double P_UP     = 1.3;
	if (idx != -1)
	{
		double offset, sigma;
		for (unsigned int i = 0; i < 4; i++)
		{// only correct e, pi, K
			double bg;
			int pid;
			if (i == 0)
			{
				bg  = max(P_LOW+EPS, min(P_UP-EPS, m_p[i]));
				pid = 0;
			}
			else if (i == 2 || i == 3)
			{
				bg  = max(BG_LOW+EPS, min(BG_UP-EPS, m_betagamma[i]));
				pid = 1;
			}
			else
			{
				continue;
			}
			double cost   = m_cost[i];
			double charge = m_charge[i];
			cost = max(COST_LOW+EPS, min(COST_UP-EPS, cost));
			if (charge == 1)
			{
				offset = h_dedx_p_offset[pid][idx]->Interpolate( bg, cost );
				sigma  = h_dedx_p_sigma[pid][idx] ->Interpolate( bg, cost );
				m_dedx_chi[i] = (m_dedx_chi[i] - offset) / sigma;
			}
			else 
			{
				offset = h_dedx_m_offset[pid][idx]->Interpolate( bg, cost );
				sigma  = h_dedx_m_sigma[pid][idx] ->Interpolate( bg, cost );
				m_dedx_chi[i] = (m_dedx_chi[i] - offset) / sigma;
			}
		}
	}
}

void SimplePIDSvc::loadHistogram()
{
	string path = getenv("SIMPLEPIDSVCROOT");
	vector<string> filename;
	for (unsigned int idx = 0; idx < 2; idx++)
	{
		const char *dir;
		if (idx == 0)
			dir = "electron";
		else if (idx == 1)
			dir = "kpi";
		else
		{
			cout << "Boundary Error! " << endl;
			exit(1);
		}

		//dedx 
		filename.clear();
		filename.push_back( path + Form("/share/%s/dedx/dedx_d10.root", dir) );
		filename.push_back( path + Form("/share/%s/dedx/dedx_d11.root", dir) );
		filename.push_back( path + Form("/share/%s/dedx/dedx_m10.root", dir) );
		filename.push_back( path + Form("/share/%s/dedx/dedx_m11.root", dir) );
		for (unsigned int i = 0; i < filename.size(); i++)
		{
			f_dedx[idx][i] = new TFile(filename[i].c_str(), "READ");
			const char *name;
			if (i == 0)
				name = "d10";
			else if (i == 1)
				name = "d11";
			else if (i == 2)
				name = "m10";
			else if (i == 3)
				name = "m11";
			else
			{
				cout << "Boundary Error! " << endl;
				exit(1);
			}
			h_dedx_p_offset[idx][i] = (TH2D*)f_dedx[idx][i]->Get( Form("h_dedx_p_offset_%s", name) );
			h_dedx_p_sigma[idx][i]  = (TH2D*)f_dedx[idx][i]->Get( Form("h_dedx_p_sigma_%s" , name) );
			h_dedx_m_offset[idx][i] = (TH2D*)f_dedx[idx][i]->Get( Form("h_dedx_m_offset_%s", name) );
			h_dedx_m_sigma[idx][i]  = (TH2D*)f_dedx[idx][i]->Get( Form("h_dedx_m_sigma_%s" , name) );
		}
		//tof_barrel q
		filename.clear();
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_q_d10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_q_d11.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_q_m10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_q_m11.root", dir) );
		for (unsigned int i = 0; i < filename.size(); i++)
		{
			f_tof_q[idx][i] = new TFile(filename[i].c_str(), "READ");
			const char *name;
			if (i == 0)
				name = "d10";
			else if (i == 1)
				name = "d11";
			else if (i == 2)
				name = "m10";
			else if (i == 3)
				name = "m11";
			else
			{
				cout << "Boundary Error! " << endl;
				exit(1);
			}
			for (unsigned int j = 0; j < 4; j++)
			{
				h_tof_p_q_offset[idx][i][j] = (TH1D*)f_tof_q[idx][i]->Get( Form("h_tof_p_q_offset_%s_%d", name, j) );
				h_tof_m_q_offset[idx][i][j] = (TH1D*)f_tof_q[idx][i]->Get( Form("h_tof_m_q_offset_%s_%d", name, j) );
				h_tof_p_q_sigma[idx][i][j]  = (TH1D*)f_tof_q[idx][i]->Get( Form("h_tof_p_q_sigma_%s_%d" , name, j) );
				h_tof_m_q_sigma[idx][i][j]  = (TH1D*)f_tof_q[idx][i]->Get( Form("h_tof_m_q_sigma_%s_%d" , name, j) );
			}
		}
		//tof_barrel bg&cost
		filename.clear();
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_bg_cost_d10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_bg_cost_d11.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_bg_cost_m10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_bg_cost_m11.root", dir) );
		for (unsigned int i = 0; i < filename.size(); i++)
		{
			f_tof_bgcost[idx][i] = new TFile(filename[i].c_str(), "READ");
			const char *name;
			if (i == 0)
				name = "d10";
			else if (i == 1)
				name = "d11";
			else if (i == 2)
				name = "m10";
			else if (i == 3)
				name = "m11";
			else
			{
				cout << "Boundary Error! " << endl;
				exit(1);
			}
			for (unsigned int j = 0; j < 4; j++)
			{
				h_tof_p_bgcost_offset[idx][i][j] = (TH2D*)f_tof_bgcost[idx][i]->Get( Form("h_tof_p_bgcost_offset_%s_%d", name, j) );
				h_tof_m_bgcost_offset[idx][i][j] = (TH2D*)f_tof_bgcost[idx][i]->Get( Form("h_tof_m_bgcost_offset_%s_%d", name, j) );
				h_tof_p_bgcost_sigma[idx][i][j]  = (TH2D*)f_tof_bgcost[idx][i]->Get( Form("h_tof_p_bgcost_sigma_%s_%d" , name, j) );
				h_tof_m_bgcost_sigma[idx][i][j]  = (TH2D*)f_tof_bgcost[idx][i]->Get( Form("h_tof_m_bgcost_sigma_%s_%d" , name, j) );
			}
		}
		//tof_barrel wgt
		filename.clear();
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_wgt_d10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_wgt_d11.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_wgt_m10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_wgt_m11.root", dir) );
		for (unsigned int i = 0; i < filename.size(); i++)
		{
			f_tof_wgt[idx][i] = new TFile(filename[i].c_str(), "READ");
			const char *name;
			if (i == 0)
				name = "d10";
			else if (i == 1)
				name = "d11";
			else if (i == 2)
				name = "m10";
			else if (i == 3)
				name = "m11";
			else
			{
				cout << "Boundary Error! " << endl;
				exit(1);
			}
			for (unsigned int j = 0; j < 15; j++)
			{
				for (unsigned int k = 0; k < 5; k++)
				{
					h_tof_p_wgt[idx][i][j][k] = (TH2D*)f_tof_wgt[idx][i]->Get( Form("h_tof_p_wgt_%s_%d_%d", name, j, k) );
					h_tof_m_wgt[idx][i][j][k] = (TH2D*)f_tof_wgt[idx][i]->Get( Form("h_tof_m_wgt_%s_%d_%d", name, j, k) );
				}
			}
		}
		//tof_barrel corr 
		filename.clear();
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_final_d10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_final_d11.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_final_m10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_barrel/tof_final_m11.root", dir) );
		for (unsigned int i = 0; i < filename.size(); i++)
		{
			f_tof_final[idx][i] = new TFile(filename[i].c_str(), "READ");
			const char *name;
			if (i == 0)
				name = "d10";
			else if (i == 1)
				name = "d11";
			else if (i == 2)
				name = "m10";
			else if (i == 3)
				name = "m11";
			else
			{
				cout << "Boundary Error! " << endl;
				exit(1);
			}
			for (unsigned int j = 0; j < 15; j++)
			{
				h_tof_p_final_offset[idx][i][j] = (TH2D*)f_tof_final[idx][i]->Get( Form("h_tof_p_final_offset_%s_%d", name, j) );
				h_tof_m_final_offset[idx][i][j] = (TH2D*)f_tof_final[idx][i]->Get( Form("h_tof_m_final_offset_%s_%d", name, j) );
				h_tof_p_final_sigma[idx][i][j]  = (TH2D*)f_tof_final[idx][i]->Get( Form("h_tof_p_final_sigma_%s_%d" , name, j) );
				h_tof_m_final_sigma[idx][i][j]  = (TH2D*)f_tof_final[idx][i]->Get( Form("h_tof_m_final_sigma_%s_%d" , name, j) );
			}
		}
		//tof_endcap q 
		filename.clear();
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_q_d10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_q_d11.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_q_m10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_q_m11.root", dir) );
		for (unsigned int i = 0; i < filename.size(); i++)
		{
			f_tofec_q[idx][i] = new TFile(filename[i].c_str(), "READ");
			const char *name;
			if (i == 0)
				name = "d10";
			else if (i == 1)
				name = "d11";
			else if (i == 2)
				name = "m10";
			else if (i == 3)
				name = "m11";
			else
			{
				cout << "Boundary Error! " << endl;
				exit(1);
			}
			for (unsigned int j = 0; j < 2; j++)
			{
				h_tofec_p_q_offset[idx][i][j] = (TH1D*)f_tofec_q[idx][i]->Get( Form("h_tofec_p_q_offset_%s_%d", name, j) );
				h_tofec_m_q_offset[idx][i][j] = (TH1D*)f_tofec_q[idx][i]->Get( Form("h_tofec_m_q_offset_%s_%d", name, j) );
				h_tofec_p_q_sigma[idx][i][j]  = (TH1D*)f_tofec_q[idx][i]->Get( Form("h_tofec_p_q_sigma_%s_%d" , name, j) );
				h_tofec_m_q_sigma[idx][i][j]  = (TH1D*)f_tofec_q[idx][i]->Get( Form("h_tofec_m_q_sigma_%s_%d" , name, j) );
			}
		}
		//tof_endcap bg 
		filename.clear();
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_bg_d10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_bg_d11.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_bg_m10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_bg_m11.root", dir) );
		for (unsigned int i = 0; i < filename.size(); i++)
		{
			f_tofec_bg[idx][i] = new TFile(filename[i].c_str(), "READ");
			const char *name;
			if (i == 0)
				name = "d10";
			else if (i == 1)
				name = "d11";
			else if (i == 2)
				name = "m10";
			else if (i == 3)
				name = "m11";
			else
			{
				cout << "Boundary Error! " << endl;
				exit(1);
			}
			for (unsigned int j = 0; j < 2; j++)
			{
				h_tofec_p_bg_offset[idx][i][j] = (TH1D*)f_tofec_bg[idx][i]->Get( Form("h_tofec_p_bg_offset_%s_%d", name, j) );
				h_tofec_m_bg_offset[idx][i][j] = (TH1D*)f_tofec_bg[idx][i]->Get( Form("h_tofec_m_bg_offset_%s_%d", name, j) );
				h_tofec_p_bg_sigma[idx][i][j]  = (TH1D*)f_tofec_bg[idx][i]->Get( Form("h_tofec_p_bg_sigma_%s_%d" , name, j) );
				h_tofec_m_bg_sigma[idx][i][j]  = (TH1D*)f_tofec_bg[idx][i]->Get( Form("h_tofec_m_bg_sigma_%s_%d" , name, j) );
			}
		}
		//tof_endcap cost 
		filename.clear();
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_cost_d10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_cost_d11.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_cost_m10.root", dir) );
		filename.push_back( path + Form("/share/%s/tof_endcap/tofec_cost_m11.root", dir) );
		for (unsigned int i = 0; i < filename.size(); i++)
		{
			f_tofec_cost[idx][i] = new TFile(filename[i].c_str(), "READ");
			const char *name;
			if (i == 0)
				name = "d10";
			else if (i == 1)
				name = "d11";
			else if (i == 2)
				name = "m10";
			else if (i == 3)
				name = "m11";
			else
			{
				cout << "Boundary Error! " << endl;
				exit(1);
			}
			for (unsigned int j = 0; j < 2; j++)
			{
				h_tofec_p_cost_offset[idx][i][j] = (TH1D*)f_tofec_cost[idx][i]->Get( Form("h_tofec_p_cost_offset_%s_%d", name, j) );
				h_tofec_m_cost_offset[idx][i][j] = (TH1D*)f_tofec_cost[idx][i]->Get( Form("h_tofec_m_cost_offset_%s_%d", name, j) );
				h_tofec_p_cost_sigma[idx][i][j]  = (TH1D*)f_tofec_cost[idx][i]->Get( Form("h_tofec_p_cost_sigma_%s_%d" , name, j) );
				h_tofec_m_cost_sigma[idx][i][j]  = (TH1D*)f_tofec_cost[idx][i]->Get( Form("h_tofec_m_cost_sigma_%s_%d" , name, j) );
			}
		}
	}
	for (unsigned int idx = 0; idx < 3; idx++)
	{       
		const char *dir;
		if (idx == 0)
			dir = "electron/emc";
		else if (idx == 1)
			dir = "kpi/emc_pion";
		else if (idx == 2)
			dir = "kpi/emc_kaon";
		else
		{
			cout << "Boundary Error! " << endl;
			exit(1);
		}
		//emc
		filename.clear();
		filename.push_back( path + Form("/share/%s/emc_d10.root", dir) );
		filename.push_back( path + Form("/share/%s/emc_d11.root", dir) );
		filename.push_back( path + Form("/share/%s/emc_m10.root", dir) );
		filename.push_back( path + Form("/share/%s/emc_m11.root", dir) );
		for (unsigned int i = 0; i < filename.size(); i++)
		{
			f_emc[idx][i] = new TFile(filename[i].c_str(), "READ");
			const char *name;
			if (i == 0)
				name = "d10";
			else if (i == 1)
				name = "d11";
			else if (i == 2)
				name = "m10";
			else if (i == 3)
				name = "m11";
			else
			{
				cout << "Boundary Error! " << endl;
				exit(1);
			}
			for (unsigned int j = 0; j < 15; j++)
			{
				for (unsigned int k = 0; k < 25; k++)
				{
					h_emc_ep[idx][i][j][k]  = (TH1D*)f_emc[idx][i]->Get( Form("h_ep_%s_%d_%d",  name, j, k) );
					h_emc_e35[idx][i][j][k] = (TH1D*)f_emc[idx][i]->Get( Form("h_e35_%s_%d_%d", name, j, k) );
				}
			}
		}
	}
	cout << "Successfully Return from Loading Initializations by package SimplePIDSvc ... " << endl;
}

int SimplePIDSvc::findBin(double *a, int length, double value)
{
	for (int i = 0; i < length; i++)
	{
		if (value > a[i] && value <= a[i+1])
		{
			return i;
		}
	}
	if (value < a[0])
	{
		return 0;
	}
	else
	{
		return length;
	}
}

double SimplePIDSvc::getChi2(int i)
{
	return pow(m_dedx_chi[i], 2) + pow(m_tof_chi[i], 2);
}

void SimplePIDSvc::loadEMCInfo(EvtRecTrack *track)
{
	//Initialization
	for (unsigned int i = 0; i < 5; i++)
	{
		m_emc_eop[i]        = -99.;
		m_emc_likelihood[i] = -99.;
	}
	m_emc_e   = -99.;
	m_emc_e13 = -99.;
	m_emc_e35 = -99.;
	m_emc_sec = -99.;
	m_emc_lat = -99.;
	m_lh_electron = -99.;

	if (!track->isEmcShowerValid()) return;

	RecEmcShower* emc_trk = track->emcShower();
	m_emc_e = emc_trk->energy();
	for (unsigned int i = 0; i < 5; i++)
	{
		m_emc_eop[i] = m_emc_e / fabs(m_p[i]);
	}
	double eseed = emc_trk->eSeed();
	double e3    = emc_trk->e3x3();
	double e5    = emc_trk->e5x5();
	m_emc_sec = emc_trk->secondMoment() / 1000.;
	m_emc_lat = emc_trk->latMoment();
	if (e3 != 0)
	{
		m_emc_e13 = eseed / e3;
	}
	if (e5 != 0)
	{
		m_emc_e35 = e3 / e5;
	}
}

bool SimplePIDSvc::calEMCLikelihood()
{
	if (m_emc_eop[0] < 0) 
		return false;
	
	int idx = getRunIdx(m_run);
	const Int_t  BIN_P    = 15;
	const Int_t  BIN_COST = 25;
	const Int_t  BIN_PID  = 3;
	const double EPS      = 1e-4;
	//electron  
	double P[BIN_PID][BIN_P + 1] = {
		{0.20, 0.47, 0.56, 0.65, 0.72, 0.79, 0.86, 0.92, 0.98, 1.03, 1.08, 1.13, 1.17, 1.22, 1.26, 1.30},
		{0.20, 0.26, 0.31, 0.35, 0.39, 0.42, 0.46, 0.49, 0.53, 0.57, 0.62, 0.67, 0.73, 0.80, 0.88, 1.05},
		{0.20, 0.33, 0.39, 0.43, 0.48, 0.52, 0.56, 0.61, 0.67, 0.73, 0.76, 0.81, 0.85, 0.90, 0.96, 1.05}, };      
	double COST[BIN_PID][BIN_COST + 1] = {
		{-0.930, -0.910, -0.905, -0.897, -0.890, -0.881, -0.871, -0.858, -0.775, -0.732, -0.669, -0.561, -0.330, 0.199, 0.515, 0.645, 0.718, 0.766, 0.804, 0.870, 0.882, 0.891, 0.898, 0.906, 0.913, 0.930},
		{-0.930, -0.810, -0.728, -0.648, -0.574, -0.501, -0.431, -0.364, -0.295, -0.228, -0.161, -0.096, -0.031, 0.035, 0.100, 0.167, 0.234, 0.301, 0.370, 0.439, 0.510, 0.580, 0.655, 0.733, 0.813, 0.930},
		{-0.930, -0.804, -0.721, -0.643, -0.568, -0.497, -0.429, -0.362, -0.293, -0.228, -0.161, -0.096, -0.029, 0.035, 0.100, 0.166, 0.233, 0.298, 0.365, 0.432, 0.500, 0.571, 0.644, 0.722, 0.805, 0.930}, }; 
	
	double vp, vcost;
	int pid;
	int bin_p, bin_cost;
	for (unsigned int i = 0; i < 4; i++)
	{
		//only e, pi ,K
		if (i == 0)
			pid = 0;
		else if (i == 2)
			pid = 1;
		else if (i == 3)
			pid = 2;
		else
			continue;
			
		vp    = max(P[pid][0]+EPS, min(P[pid][BIN_P]-EPS, m_p[i]));
		vcost = max(COST[pid][0]+EPS, min(COST[pid][BIN_COST]-EPS, m_cost[i]));
		bin_p    = findBin(P[pid], BIN_P, vp);
		bin_cost = findBin(COST[pid], BIN_COST, vcost);

		m_emc_likelihood[i] = h_emc_ep[pid][idx][bin_p][bin_cost]->Interpolate(m_emc_eop[i]) * h_emc_e35[pid][idx][bin_p][bin_cost]->Interpolate(m_emc_e35);
	}
	double a = m_prob[0] > 0 ? m_prob[0] : 0;
	double b = m_prob[2] > 0 ? m_prob[2] : 0;
	double c = m_prob[3] > 0 ? m_prob[3] : 0;
	double sum = a * m_emc_likelihood[0] + b * m_emc_likelihood[2] + c * m_emc_likelihood[3];

	if (sum > 0 && m_prob[0] > 0)
	{
		m_lh_electron = m_prob[0] * m_emc_likelihood[0] / sum;
		return true;
	}
	else
	{
		return false;	
	}
}

bool SimplePIDSvc::ispion()
{
	if (m_prob[2] > 0.00 && m_prob[2] > m_prob[3]) 
		return true;
	else 
		return false;
}

bool SimplePIDSvc::iskaon()
{
	if (m_prob[3] > 0.00 && m_prob[3] > m_prob[2])
		return true;
	else
		return false;
}

//bool SimplePIDSvc::iselectron_(bool emc)
//{
//	if (m_prob[0] > 0  &&  m_prob[0] > m_prob[2]  &&  m_prob[0] > m_prob[3])
//	{
//		if (!emc)
//			return true;
//		else if (fabs(m_cost[0]) <  0.7  &&  m_emc_eop[0] > 0     &&  m_emc_eop[0] < 0.8) 
//			return false;
//		else if (fabs(m_cost[0]) >= 0.7  &&  fabs(m_cost[0])<0.8  &&  m_emc_eop[0] > 0  &&  m_emc_eop[0] < -7.5*fabs(m_cost[0])+6.05) 
//			return false;
//		else if (fabs(m_cost[0]) > 0.85  &&  m_emc_eop[0] > 0     &&  m_emc_eop[0] < 0.6) 
//			return false;
//		else
//			return true;
//	}
//	else
//		return false;
//}

bool SimplePIDSvc::iselectron(bool emc)
{
	if (!emc)
	{
		if (m_prob[0] > 0 && m_prob[0] > m_prob[2] && m_prob[0] > m_prob[3])
			return true;
		else
			return false;
	}
	else
	{
		if (calEMCLikelihood())
		{
			if (m_lh_electron > m_eid_ratio)
				return true;
			else
				return false;	
		}
		else
		{
			if (m_prob[0] > 0 && m_prob[0] > m_prob[2] && m_prob[0] > m_prob[3])
				return true;
			else
				return false;
		}
	}
}
