#include "VertexFit/SecondVertexFit.h"
#include "VertexFit/BField.h"

//----------------------------------------------------
// In Second Vertex Fit:
//
// Fitting Parameters
// (px, py, pz, E, xd, yd, zd, xp, yp, zp)
//
// Constraints (charge = 0)
//
// xp - xd + px/m * ct = 0
// yp - yd + py/m * ct = 0
// zp - zd + pz/m * ct = 0
//
//     ( 0 0 0 0 -1 0 0 1 0 0 )
// D = ( 0 0 0 0 0 -1 0 0 1 0 )
//     ( 0 0 0 0 0 0 -1 0 0 1 )
//
//     ( px/m )
// E = ( py/m )
//     ( pz/m )
//
//     ( xp - xd + px/m * ct )
// d = ( yp - yd + py/m * ct )
//     ( zp - zd + pz/m * ct )
//
// Constraints (charge != 0)
//
// xp - xd + px/a * sin(a ctau/m) + py/a(1-cos(a ctau/m)) = 0
// yp - yd + py/a * sin(a ctau/m) - px/a(1-cos(a ctau/m)) = 0
// zp - zd + pz/m ctau = 0
//     ( 0 0 0 0 -1 0 0 1 0 0 )
// D = ( 0 0 0 0 0 -1 0 0 1 0 )
//     ( 0 0 0 0 0 0 -1 0 0 1 )
//
//     ( px/m cos(a * ct/m) + py/m * sin (a * ct/m))
// E = ( py/m cos(a * ct/m) - px/m * sin (a * ct/m))
//     ( pz/m )
//
//----------------------------------------------------

SecondVertexFit *SecondVertexFit::m_pointer = 0;

SecondVertexFit *SecondVertexFit::instance() 
{
	if (m_pointer) 
		return m_pointer;
	m_pointer = new SecondVertexFit();
	return m_pointer;
}

SecondVertexFit::SecondVertexFit()
{
	HepVector vx(3,0);
	m_vpar_primary.setVx(vx);
	HepSymMatrix evx(3,0);
	evx[0][0] = 0.1 * 0.1;
	evx[1][1] = 0.1 * 0.1;
	evx[2][2] = 1.5 * 1.5;
	m_vpar_primary.setEvx(evx);
}

SecondVertexFit::~SecondVertexFit() 
{
	//if(m_pointer) delete m_pointer;
}

void SecondVertexFit::init() 
{
	clearWTrackOrigin();
	clearWTrackInfit();
	clearWTrackList();
	m_vpar_secondary = VertexParameter();
	m_lxyz       = 0;
	m_lxyz_error = 0;
	m_p4par      = HepLorentzVector(0, 0, 0, 0);
	m_crxyz      = HepVector(3, 0);
	m_chisq      = 9999;
	m_wtrk       = WTrackParameter();
	m_niter      = 10;
	m_chicut     = 500;
	m_chiter     = 1.0e-2;
	m_factor     = 1.000;
}

bool SecondVertexFit::Fit() 
{
	bool okfit = false;

	HepVector aOrigin(10, 0);
	HepVector aInfit(10, 0);
	HepSymMatrix VaOrigin(10, 0);
	HepSymMatrix VaInfit(10, 0);
	aOrigin.sub(1, wTrackOrigin(0).w());
	aOrigin.sub(8, m_vpar_primary.Vx());
	VaOrigin.sub(1, wTrackOrigin(0).Ew());
	VaOrigin.sub(8, m_vpar_primary.Evx());
	HepVector ctOrigin(1, 0);
	HepVector ctInfit(1, 0);
	HepSymMatrix Vct(1, 0);
	aInfit  = aOrigin;
	ctInfit = ctOrigin;

	std::vector<double> chisq;
	chisq.clear();
	double chi2 = 999;
	for(int it = 0; it < m_niter; it++)
	{
		HepMatrix D(3, 10, 0);
		HepLorentzVector p4par = HepLorentzVector(aInfit[0], aInfit[1], aInfit[2], aInfit[3]);
		HepMatrix E(3,1,0);
		HepVector d(3, 0);
		if (wTrackOrigin(0).charge() == 0) 
		{
			D[0][4] = -1.0;
			D[0][7] = 1.0;
			D[1][5] = -1.0;
			D[1][8] = 1.0;
			D[2][6] = -1.0;
			D[2][9] = 1.0;

			E[0][0] = p4par.px()/p4par.m();
			E[1][0] = p4par.py()/p4par.m();
			E[2][0] = p4par.pz()/p4par.m();

			d[0] = aInfit[7]-aInfit[4]+ctInfit[0]*p4par.px()/p4par.m();
			d[1] = aInfit[8]-aInfit[5]+ctInfit[0]*p4par.py()/p4par.m();
			d[2] = aInfit[9]-aInfit[6]+ctInfit[0]*p4par.pz()/p4par.m();
		}
		else 
		{
//			double afield = VertexFitBField::instance()->getCBz(m_vpar_primary.Vx(), m_vpar_secondary.Vx());
			double afield = m_factor * VertexFitBField::instance()->getCBz(m_vpar_primary.Vx(), m_vpar_secondary.Vx());
			double a = afield * wTrackOrigin(0).charge();
			D[0][4] = -1.0;
			D[0][7] = 1.0;
			D[1][5] = -1.0;
			D[1][8] = 1.0;
			D[2][6] = -1.0;
			D[2][9] = 1.0;

			E[0][0] = p4par.px() / p4par.m() * cos(a * ctInfit[0] / p4par.m()) + p4par.py() / p4par.m() * sin(a * ctInfit[0] / p4par.m());
			E[1][0] = p4par.py() / p4par.m() * cos(a * ctInfit[0] / p4par.m()) - p4par.px() / p4par.m() * sin(a * ctInfit[0] / p4par.m());
			E[2][0] = p4par.pz() / p4par.m();

			d[0] = aInfit[7] - aInfit[4]+p4par.px()/a * sin(a * ctInfit[0]/p4par.m()) + p4par.py()/a*(1-cos(a*ctInfit[0]/p4par.m()));
			d[1] = aInfit[8] - aInfit[5]+p4par.py()/a * sin(a * ctInfit[0]/p4par.m()) - p4par.px()/a*(1-cos(a*ctInfit[0]/p4par.m()));
			d[2] = aInfit[9] - aInfit[6]+ctInfit[0]*p4par.pz()/p4par.m();
		}
		
		HepSymMatrix VD(3, 0);
		HepVector dela0(10, 0);
		HepVector lambda0(3, 0);
		HepVector delct(1, 0);
		HepVector lambda(3, 0);
		int ifail;

		VD      = (VaOrigin.similarity(D)).inverse(ifail);
		dela0   = aOrigin - aInfit;
		lambda0 = VD*(D*dela0 + d);
		Vct     = (VD.similarity(E.T())).inverse(ifail);
		delct   = -(Vct * E.T()) * lambda0;
		ctInfit = ctInfit + delct;
		lambda  = lambda0 + (VD * E) * delct;
		aInfit  = aOrigin - (VaOrigin * D.T()) * lambda;
		chi2    = dot(lambda, D*dela0 + d);
		VaInfit = VaOrigin - (VD.similarity(D.T())).similarity(VaOrigin);
		VaInfit = VaInfit + (((Vct.similarity(E)).similarity(VD)).similarity(D.T())).similarity(VaOrigin);

		chisq.push_back(chi2);

		if(it > 0) 
		{
			double delchi = chisq[it] - chisq[it-1];
			if (fabs(delchi) < m_chiter) break;
		}
	}
	if (chi2 < 0 || chi2 > m_chicut)
		return okfit;

	HepLorentzVector p4par = HepLorentzVector(aInfit[0], aInfit[1], aInfit[2], aInfit[3]);
	m_ctau       = ctInfit[0];
	m_ctau_error = sqrt(Vct[0][0]);
	m_lxyz       = ctInfit[0] * p4par.rho() / p4par.m();
	m_lxyz_error = sqrt(Vct[0][0]) * p4par.rho() / p4par.m();
	m_chisq      = chi2;
	m_p4par      = p4par;
	for(int i = 0; i < 3; i++)
		m_crxyz[i] = aInfit[4+i];
	HepVector w(7, 0);
	HepSymMatrix Ew(7, 0);
	for(int i = 0; i < 7; i++) 
	{
		w[i] = aInfit[i];
		for(int j = 0; j < 7; j++) 
		{
			Ew[i][j] = VaInfit[i][j];
		}
	}
	m_wtrk.setW(w);
	m_wtrk.setEw(Ew);
	m_wtrk.setCharge(wTrackOrigin(0).charge());
	okfit = true;
	return okfit;
}


