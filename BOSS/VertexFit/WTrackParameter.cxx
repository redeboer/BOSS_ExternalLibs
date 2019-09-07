#include "VertexFit/WTrackParameter.h"

WTrackParameter::WTrackParameter() 
{
	m_w      = HepVector(7, 0);
	m_Ew     = HepSymMatrix(7, 0);
	m_charge = 0;
	m_type   = 1;
	m_mass   = 0;
	m_massInvariable = true;
}

WTrackParameter::WTrackParameter(const WTrackParameter &wtrk)
{
	m_charge = wtrk.m_charge;
	m_w      = wtrk.m_w;
	m_Ew     = wtrk.m_Ew;
	m_type   = wtrk.m_type;
	m_plmp   = wtrk.m_plmp;
	m_Vplm   = wtrk.m_Vplm;
	m_mass   = wtrk.m_mass;
	m_massInvariable = wtrk.m_massInvariable;
}

WTrackParameter& WTrackParameter::operator =(const WTrackParameter &wtrk)
{
	if (this == &wtrk)
		return *this;
	m_charge = wtrk.m_charge;
	m_type   = wtrk.m_type;
	m_w      = wtrk.m_w;
	m_Ew     = wtrk.m_Ew;
	m_plmp   = wtrk.m_plmp;
	m_Vplm   = wtrk.m_Vplm;
	m_mass   = wtrk.m_mass;
	m_massInvariable = wtrk.m_massInvariable;
	return *this;
}

HepVector WTrackParameter::philambdamass() 
{
	HepVector tmp(4,0);
	if (w()[1] >= 0)
		tmp[0] = atan(w()[1]/(w()[0]));
	else 
		tmp[0] = atan(w()[1]/(w()[0])) + 3.1415926;
	tmp[1] = w()[2]/sqrt(w()[0]*w()[0] + w()[1]*w()[1]);
	tmp[2] = p().m();
	tmp[3] = sqrt(w()[3]*w()[3] - p().m()*p().m());
	return tmp;
}

WTrackParameter::WTrackParameter(const int charge, const HepLorentzVector &p, const HepPoint3D &x, const double err[]) 
{
	HepSymMatrix error(5, 0);
	int k = 0;
	for (int i = 0; i < 5; i++) 
	{
		for(int j = i; j < 5; j++) 
		{
			error[i][j] = err[k];
			error[j][i] = err[k];
			k++;
		}
	}
	*this = WTrackParameter(charge, p, x, error);
}

WTrackParameter::WTrackParameter(const int charge, const HepLorentzVector &p, const HepPoint3D &x, const HepSymMatrix &err)
{
	m_w      = HepVector(7, 0);
	m_Ew     = HepSymMatrix(7, 0);
	m_charge = charge;
	m_type   = 1;
	m_mass   = p.m();
	m_massInvariable = true;
	for (int i = 0; i < 4; i++)
		m_w[i] = p[i];
	for (int i = 0; i < 3; i++)
		m_w[i+4] = x[i];

	HepVector helix(5, 0);
	double phi0 = atan2(-p[0], p[1]);
	if (cos(phi0) != 0)
		helix[0] = x[0] / cos(phi0);
	else
		helix[0] = x[1] / sin(phi0);
	helix[1] = phi0;
	helix[2] = charge/p.perp();
	helix[3] = x[3];
	helix[4] = p[3]/p.perp();
	HepMatrix dWdA(7, 5, 0);
	dWdA = GetCvtMatrix(p.m(), helix);
	m_Ew = err.similarity(dWdA);
}

WTrackParameter::WTrackParameter(const double mass, const HepVector &hel, const double err[])
{ 
	HepSymMatrix error(5, 0);
	int k = 0;
	for(int i = 0; i < 5; i++) 
	{
		for(int j = i; j < 5; j++) 
		{
			error[i][j] = err[k];
			error[j][i] = err[k];
			k++;
		}
	}
	*this = WTrackParameter(mass, hel, error);
}


WTrackParameter::WTrackParameter(const double mass, const HepVector &hel, const HepSymMatrix &err)
{
	m_type   = 1;
	m_mass   = mass;
	m_w      = HepVector(7, 0);
	m_Ew     = HepSymMatrix(7, 0);
	HepMatrix dWdA(7, 5, 0);
	dWdA     = GetCvtMatrix(mass, hel);
	m_Ew     = err.similarity(dWdA);
	m_w      = CvtH2W(mass, hel);
	m_charge = hel[2] > 0 ? +1 : -1;
	m_massInvariable = true;
}

WTrackParameter::WTrackParameter(const HepLorentzVector &p, const double dphi, const double dthe, const double dE)
{
	HepPoint3D x(0, 0, 0);
	*this = WTrackParameter(x, p, dphi, dthe, dE);
}


WTrackParameter::WTrackParameter(const HepPoint3D &x, const HepLorentzVector &p, const double dphi, const double dthe, const double dE)
{
	m_w      = HepVector(7, 0);
	m_Ew     = HepSymMatrix(7, 0);
	m_type   = 2;
	m_mass   = p.m();
	m_charge = 0;
	m_massInvariable = true;
	for (int i = 0; i< 4; i++)
		m_w[i] = p[i];
	for (int i = 0; i< 3; i++)
		m_w[i+4] = x[i];
	
	HepMatrix dwda(7, 3, 0);
	dwda[0][0] = -p.py();
	dwda[1][0] = p.px();
	dwda[4][0] = -x.y();
	dwda[5][0] = x.x();

	dwda[0][1] = p.px()*p.pz()/p.perp();
	dwda[1][1] = p.py()*p.pz()/p.perp();
	dwda[2][1] = -p.perp();
	dwda[6][1] = -x.distance2()/sqrt(x.x()*x.x()+x.y()*x.y());

	dwda[0][2] = p.px()/p.rho();
	dwda[1][2] = p.py()/p.rho();
	dwda[2][2] = p.pz()/p.rho();
	dwda[3][2] = p.rho()/p.e();
	// dwda[4][2] = x.x()/(x.x()*x.x()+x.y()*x.y());
	// dwda[5][2] = x.y()/(x.x()*x.x()+x.y()*x.y());
	// dwda[6][2] = x.z()/(x.x()*x.x()+x.y()*x.y());

	HepSymMatrix emcmea(3, 0);
	emcmea[0][0] = dphi * dphi;
	emcmea[1][1] = dthe * dthe;
	emcmea[2][2] = dE * dE;
	m_Ew = emcmea.similarity(dwda);
}

HepMatrix WTrackParameter::GetCvtMatrix(const double mass, const HepVector &helix)  //just suitable for charged tracks
{
	HepMatrix m(7, 5, 0);
	double drho    = helix[0];
	double phi0    = helix[1];
	double kappa   = helix[2];
	double kappa2  = kappa * kappa;
	double kappa3  = kappa * kappa2;
	double dz      = helix[3];
	double lambda  = helix[4];
	double lambda2 = lambda * lambda;
	double e       = sqrt( (1+lambda2) / kappa2 + mass * mass );
	double sinphi0 = sin(phi0);
	double cosphi0 = cos(phi0);
	int    q       = (kappa>0) ? 1 : (-1);
	m[0][1] = -cosphi0 * q / kappa;
	m[0][2] = sinphi0 * q / kappa2;
	m[1][1] = -sinphi0 * q / kappa;
	m[1][2] = -cosphi0 * q / kappa2;
	m[2][2] = -lambda * q / kappa2;
	m[2][4] = q / kappa;
	m[3][2] = -(1+lambda2) / (kappa3 * e);
	m[3][4] = lambda / (kappa2 * e);
	m[4][0] = cosphi0;
	m[4][1] = -drho * sinphi0;
	m[5][0] = sinphi0;
	m[5][1] = drho * cosphi0;
	m[6][3] = 1;
	return m;
}

HepVector WTrackParameter::CvtH2W(const double mass, const HepVector &helix)
{
	double drho    = helix[0];
	double phi0    = helix[1];
	double kappa   = helix[2];
	double dz      = helix[3];
	double lambda  = helix[4];
	double sinphi0 = sin(phi0);
	double cosphi0 = cos(phi0);
	int    q       = (kappa>0) ? 1 : (-1);
	HepVector w(7, 0);
	w[0] = -sinphi0 * q / kappa;
	w[1] = cosphi0 * q / kappa;
	w[2] = lambda * q / kappa;
	w[3] = sqrt( (1+lambda*lambda) / (kappa*kappa) + mass * mass );
	w[4] = drho * cosphi0;
	w[5] = drho * sinphi0;
	w[6] = dz;
	return w;
}
