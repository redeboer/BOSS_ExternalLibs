#include <cassert>

#include "VertexFit/VertexFit.h"
#include "VertexFit/VertexConstraints.h"
#include "VertexFit/BField.h"
#include "VertexFit/HTrackParameter.h"
#include "TStopwatch.h"

const int VertexFit::NTRKPAR = 6;
const int VertexFit::NVTXPAR = 3;
const int VertexFit::NCONSTR = 2;

VertexFit* VertexFit::m_pointer = 0;

VertexFit* VertexFit::instance() 
{
	if (m_pointer) return m_pointer;
	m_pointer = new VertexFit();
	return m_pointer;
}

VertexFit::~VertexFit() 
{
	//if (m_pointer) delete m_pointer;
}

VertexFit::VertexFit() {;} 

void VertexFit::init() 
{
	//derived from TrackPool
	clearWTrackOrigin();
	clearWTrackInfit();
	clearWTrackList();
	clearGammaShape();
	clearGammaShapeList();
	clearMapkinematic();
	clearMappositionA();
	clearMappositionB();
	clearone();
	cleartwo();

	m_vpar_origin.clear();
	m_vpar_infit.clear();
	m_vc.clear();
	m_chisq.clear();
	m_chi = 9999.;
	m_virtual_wtrk.clear();
	m_niter = 10;
	m_chiter = 1.0e-3;
	m_chicut = 1000;

	m_TRA = HepMatrix(6, 7, 0);
	m_TRA[0][0] = 1.0;
	m_TRA[1][1] = 1.0;
	m_TRA[2][2] = 1.0;
	m_TRA[3][4] = 1.0;
	m_TRA[4][5] = 1.0;
	m_TRA[5][6] = 1.0;
	m_TRB = HepMatrix(7, 6, 0);
	m_TRB[0][0] = 1.0;
	m_TRB[1][1] = 1.0;
	m_TRB[2][2] = 1.0;
	m_TRB[4][3] = 1.0;
	m_TRB[5][4] = 1.0;
	m_TRB[6][5] = 1.0;

	m_factor = 1.000;
}

//
//  Add Beam Fit
//
void VertexFit::AddBeamFit(int number, VertexParameter vpar, int n) 
{
	std::vector<int> tlis = AddList(n);
	VertexConstraints vc;
	vc.FixedVertexConstraints(tlis);
	m_vc.push_back(vc);
	m_vpar_origin.push_back(vpar);
	m_vpar_infit.push_back(vpar);
	if ((unsigned int)number != m_vc.size() - 1)
		std::cout << "wrong kinematic constraints index" << std::endl;
}

//
//  Add Vertex
//
void VertexFit::AddVertex(int number, VertexParameter vpar, std::vector<int> tlis)
{
	VertexConstraints vc;
	vc.CommonVertexConstraints(tlis);
	m_vc.push_back(vc);
	HepVector vx(3, 0);
	for (unsigned int i = 0; i < tlis.size(); i++) 
		vx += wTrackOrigin(tlis[i]).X();
	vx = vx/tlis.size();
	VertexParameter n_vpar = vpar;
	n_vpar.setVx(vx);
	m_vpar_origin.push_back(n_vpar);
	m_vpar_infit.push_back(n_vpar);
	WTrackParameter vwtrk;
	m_virtual_wtrk.push_back(vwtrk);
	if ((unsigned int)number != m_vc.size() - 1)
		std::cout << "wrong kinematic constraints index" << std::endl;
}

void VertexFit::AddVertex(int number, VertexParameter vpar, int n1, int n2) 
{
	std::vector<int> tlis = AddList(n1, n2);
	AddVertex(number, vpar, tlis);
}


void VertexFit::AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3) 
{
	std::vector<int> tlis = AddList(n1, n2, n3);
	AddVertex(number, vpar, tlis);
}

void VertexFit::AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4) 
{
	std::vector<int> tlis = AddList(n1, n2, n3, n4);
	AddVertex(number, vpar, tlis);
}

void VertexFit::AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4,	int n5) 
{
	std::vector<int> tlis = AddList(n1, n2, n3, n4, n5);
	AddVertex(number, vpar, tlis);
}

void VertexFit::AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4,	int n5, int n6)
{
	std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6);
	AddVertex(number, vpar, tlis);
}

void VertexFit::AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7) 
{
	std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7);
	AddVertex(number, vpar, tlis);
}

void VertexFit::AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8) 
{
	std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8);
	AddVertex(number, vpar, tlis);
}

void VertexFit::AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9) 
{
	std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9);
	AddVertex(number, vpar, tlis);
}

void VertexFit::AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10) 
{
	std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);
	AddVertex(number, vpar, tlis);
}

void VertexFit::AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, int n11) 
{
	std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
	AddVertex(number, vpar, tlis);
}

void VertexFit::AddVertex(int number, VertexParameter vpar, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, int n11, int n12) 
{
	std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12);
	AddVertex(number, vpar, tlis);
}

bool VertexFit::Fit(int n) 
{
	bool okfit = false;
	TStopwatch timer;
	m_cpu = HepVector(10, 0);
	if (n < 0 || (unsigned int)n >= m_vc.size()) return okfit;

	timer.Start();
	int ifail;
	m_nvtrk = numberWTrack();

	m_pOrigin = HepVector(m_nvtrk * NTRKPAR, 0);
	m_pInfit  = HepVector(m_nvtrk * NTRKPAR, 0);
	m_pcovOrigin = HepSymMatrix(m_nvtrk * NTRKPAR, 0);
	m_pcovInfit  = HepSymMatrix(m_nvtrk * NTRKPAR, 0);
  
	int ntrk = numberWTrack();
	for(unsigned int itk = 0; itk < ntrk; itk++) 
	{
		setWTrackInfit(itk, wTrackOrigin(itk));
		setPOrigin(itk, Convert76(wTrackOrigin(itk).w()));
		setPCovOrigin(itk, wTrackOrigin(itk).Ew().similarity(m_TRA));
	}
	m_pInfit = m_pOrigin;

	m_xOrigin = HepVector(NVTXPAR, 0);
	m_xInfit  = HepVector(NVTXPAR, 0);
	m_xcovOrigin = HepSymMatrix(NVTXPAR, 0);
	m_xcovInfit  = HepSymMatrix(NVTXPAR, 0);
	m_xcovOriginInversed = HepSymMatrix(NVTXPAR, 0);
	m_xcovInfitInversed  = HepSymMatrix(NVTXPAR, 0);

	m_xOrigin = m_vpar_origin[n].Vx();
	m_xcovOrigin = m_vpar_origin[n].Evx();
	m_xcovOriginInversed = m_xcovOrigin.inverse(ifail);
	m_xInfit = m_xOrigin;

	m_vpar_infit[n] = m_vpar_origin[n];

	m_B = HepMatrix(NCONSTR*m_nvtrk, NVTXPAR, 0);
	m_A = HepMatrix(NCONSTR*m_nvtrk, NTRKPAR*m_nvtrk, 0);
	m_BT = HepMatrix(NVTXPAR,NCONSTR*m_nvtrk, 0);
	m_AT = HepMatrix(NTRKPAR*m_nvtrk, NCONSTR*m_nvtrk, 0);
	m_G = HepVector(NCONSTR*m_nvtrk, 0);
	m_W = HepSymMatrix(NCONSTR*m_nvtrk, 0);
	m_E = HepMatrix(NTRKPAR*m_nvtrk, NVTXPAR, 0);
 
	timer.Stop();
	m_cpu[0] += timer.CpuTime();

	// iteration loop
	std::vector<double> chisq;
	chisq.clear();
	for (int it = 0; it < m_niter; it++)
	{
		timer.Start();
		UpdateConstraints(m_vc[n]);
		timer.Stop();
		m_cpu[1] += timer.CpuTime();
		timer.Start();
		fitVertex(n);
		timer.Stop();
		m_cpu[2] += timer.CpuTime();
		chisq.push_back(m_chisq[n]);
		if (it > 0) 
		{
			double delchi = chisq[it] - chisq[it-1];
			if (fabs(delchi) < m_chiter) break;
		}
	}

	/*REVISED
	if(m_chisq[n] >= m_chicut || m_chisq[n] < 0) return okfit;
	REVISED*/
	if (m_chisq[n] >= m_chicut) return okfit;

	// update vertex and its covariance
	m_vpar_infit[n].setVx(m_xInfit);
	m_vpar_infit[n].setEvx(m_xcovInfit);

	okfit = true;
	return okfit;
}

bool VertexFit::BeamFit(int n)
{
	bool okfit = false;
	if (n < 0 || (unsigned int)n >= m_vc.size()) 
		return okfit;
	for (unsigned int i = 0; i < (m_vc[n].Ltrk()).size(); i++) 
	{
		int itk = (m_vc[n].Ltrk())[i];
		setWTrackInfit(itk, wTrackOrigin(itk));
	}
	m_vpar_infit[n] = m_vpar_origin[n];

	// iteration loop
	std::vector<double> chisq;
	chisq.clear();
	for (int it = 0; it < m_niter; it++) 
	{
		std::vector<WTrackParameter> wlis;
		wlis.clear();
		for (unsigned int i = 0; i < (m_vc[n].Ltrk()).size(); i++) 
		{
			int itk = (m_vc[n].Ltrk())[i];
			wlis.push_back(wTrackInfit(itk));
		}
		VertexParameter vpar = m_vpar_infit[n];
		m_vc[n].UpdateConstraints(vpar, wlis);

		fitBeam(n);
		chisq.push_back(m_chisq[n]);
		if (it > 0) 
		{
			double delchi = chisq[it] - chisq[it-1];
			if(fabs(delchi) < m_chiter)
			break;
		}
	}
	if(m_chisq[n]>=m_chicut) return okfit;
	swimBeam(n);
	okfit = true;
	return okfit;
}

bool VertexFit::Fit() 
{
	bool okfit = false;
	double mychi = 0;
	for (unsigned int n = 0; n<(int)(m_vc.size()); n++) 
	{
		Fit(n);
		if (m_chisq[n] >= m_chicut) return okfit;
		swimVertex(n);
		mychi = mychi + m_chisq[n];
	}
	m_chi = mychi;
	okfit = true;
	return okfit;
}

void VertexFit::fitVertex(int n) 
{
	if (m_chisq.size() == 0) 
	{
		for (unsigned int i = 0; i < m_vc.size(); i++)
		m_chisq.push_back(9999.0);
	}
	TStopwatch timer;
	VertexConstraints vc = m_vc[n];

	int ifail;
	int NSIZE = (vc.Ltrk()).size();

	// get new Vx
	timer.Start();
	m_xcovInfitInversed = m_xcovOriginInversed;

	for (unsigned int i = 0; i < NSIZE; i++) 
	{
		int itk = (vc.Ltrk())[i];
		m_xcovInfitInversed += vfW(itk).similarity(vfBT(itk));
	}
	m_xcovInfit = m_xcovInfitInversed.inverse(ifail);
  
	// calculate Kq and E
	m_KQ = HepMatrix(NVTXPAR, m_nvtrk * NCONSTR, 0);
	m_E  = HepMatrix(m_nvtrk*NTRKPAR, NVTXPAR, 0);
	for (unsigned int i = 0; i < NSIZE; i++) 
	{
		int itk = (vc.Ltrk())[i];
		setKQ(itk, (m_xcovInfit * vfBT(itk) * vfW(itk)));
		setE(itk, (-pcovOrigin(itk) * vfAT(itk) * vfKQ(itk).T()));
	}
	// update vertex position
	m_xInfit = m_xOrigin;
	for (unsigned int i = 0; i < NSIZE; i++) 
	{
		int itk = (vc.Ltrk())[i];
		m_xInfit -= vfKQ(itk) * vfG(itk);
	}
	// update Track parameter
	HepVector dq0q(NVTXPAR, 0);
	dq0q = m_xcovInfitInversed * (m_xOrigin - m_xInfit);
	for (unsigned int i = 0; i < NSIZE; i++) 
	{
		int itk = (vc.Ltrk())[i];
		HepVector alpha(NTRKPAR, 0);
		alpha = pOrigin(itk) - pcovOrigin(itk) * vfAT(itk) * (vfW(itk)*vfG(itk) - vfKQ(itk).T()*dq0q);
		setPInfit(itk, alpha);
	}
	// get chisquare value
	m_chisq[n] = (m_W.similarity(m_G.T())- m_xcovInfitInversed.similarity((m_xInfit-m_xOrigin).T()))[0][0];
}

void VertexFit::vertexCovMatrix(int n)
{
	TStopwatch timer;
	timer.Start();

	VertexConstraints vc = m_vc[n];

	unsigned int NSIZE = vc.Ltrk().size();
	int ifail;
	m_pcovInfit = HepSymMatrix(NTRKPAR*m_nvtrk, 0);
	for (unsigned int i = 0; i < NSIZE; i++) 
	{
		int itk = vc.Ltrk()[i];
		setPCovInfit(itk, pcovOrigin(itk) - vfW(itk).similarity(pcovOrigin(itk) * vfAT(itk)));
	}
	m_pcovInfit = m_pcovInfit + m_xcovInfitInversed.similarity(m_E);

	timer.Stop();
	m_cpu[3] += timer.CpuTime();
}

void VertexFit::swimVertex(int n)
{
	TStopwatch timer;
	timer.Start();
	// Track parameter can be expressed as:
	//
	// px = px0 + a*(y0 - yv)
	// py = py0 - a*(x0 - xv)
	// pz = pz0
	// e  = e
	// x  = xv
	// y  = yv
	// z  = zv
	//
	// thus p = A * a + B * vx
	// x = vx
	VertexConstraints vc = m_vc[n];
	unsigned int NSIZE = vc.Ltrk().size();

	HepMatrix A(6, 6, 0);
	A[0][0] = 1.0;
	A[1][1] = 1.0;
	A[2][2] = 1.0;
	HepMatrix B(6, 3, 0);
	B[3][0] = 1.0;
	B[4][1] = 1.0;
	B[5][2] = 1.0;
	HepVector w1(6, 0);
	HepVector w2(7, 0);
	HepSymMatrix Ew(7, 0);
	HepMatrix ew1(6, 6, 0);
	HepMatrix ew2(7, 7, 0);

	for (unsigned int i = 0; i < NSIZE; i++) 
	{
		int itk = vc.Ltrk()[i];
//		double afield = VertexFitBField::instance()->getCBz(m_xInfit, pInfit(itk).sub(4, 6));
		double afield = m_factor * VertexFitBField::instance()->getCBz(m_xInfit, pInfit(itk).sub(4, 6));
		double a = afield * wTrackInfit(itk).charge();

		A[0][4] = a;
		A[1][3] = -a;
		B[0][1] = -a;
		B[1][0] = a;
		w1 = A * pInfit(itk) + B * m_xInfit;
		ew1 = pcovInfit(itk).similarity(A) + m_xcovInfit.similarity(B) + A*vfE(itk)*B.T() + B*(vfE(itk).T())*A.T();

		WTrackParameter wtrk = wTrackOrigin(itk);
		w2 = Convert67(wtrk.mass(), w1);
		wtrk.setW(w2);

		m_TRB[3][0] = w2[0] / w2[3];
		m_TRB[3][1] = w2[1] / w2[3];
		m_TRB[3][2] = w2[2] / w2[3];

		ew2 = m_TRB * ew1 * m_TRB.T();
		Ew.assign(ew2);
		wtrk.setEw(Ew);
		//renew parameters of input track 
		setWTrackInfit(itk, wtrk);
	}
	timer.Stop();
	m_cpu[4] += timer.CpuTime();
}

bool VertexFit::pull(int n, int itk, HepVector& p) 
{
	assert(p.num_row() == 5);
	vertexCovMatrix(n);
	
	WTrackParameter wtrk0, wtrk1;
	HepVector w1(6, 0);
	HepVector w2(7, 0);
	HepSymMatrix ew1(6, 0);
	HepSymMatrix ew2(7, 0);
	wtrk0 = wTrackOrigin(itk);
	w1  = pInfit(itk);
	ew1 = pcovInfit(itk);
	w2  = Convert67(wtrk0.mass(), w1);
	m_TRB[3][0] = w2[0] / w2[3];
	m_TRB[3][1] = w2[1] / w2[3];
	m_TRB[3][2] = w2[2] / w2[3];
	ew2 = ew1.similarity(m_TRB);
	wtrk1.setW(w2);
	wtrk1.setEw(ew2);
	wtrk1.setCharge(wtrk0.charge());

	HTrackParameter htrk0(wtrk0);
	HTrackParameter htrk1(wtrk1);
	for (int i = 0; i < 5; i++) 
	{
		double del = htrk0.eHel()[i][i] - htrk1.eHel()[i][i];
		if (del == 0.0) 
		{
			return false;
		}
		p[i] = (htrk0.helix()[i] - htrk1.helix()[i]) / sqrt(abs(del));
	} 
	return true;
}

void VertexFit::fitBeam(int n) 
{
/*
	if(m_chisq.size() == 0) {
    for(unsigned int i = 0; i < m_vc.size(); i++)
      m_chisq.push_back(0.0);
  }

  VertexConstraints vc = m_vc[n];
  VertexParameter   vpar = m_vpar_origin[n];

  unsigned int NSIZE = vc.Ltrk().size();
  int ifail;

  // get VD, lambda
  for(unsigned int i = 0; i < NSIZE; i++) {
    int itk = vc.Ltrk()[i];
    HepVector dela0(7, 0);
    dela0 = wTrackOrigin(itk).w()-wTrackInfit(itk).w();
    HepSymMatrix vd(2, 0);
    vd = ((wTrackOrigin(itk).Ew()).similarity(vc.Dc()[i])).inverse(ifail);
    HepVector lambda(2, 0);
    lambda = vd*(vc.Dc()[i]*dela0+vc.dc()[i]);
    vc.setVD(i, vd);
    vc.setLambda(i, lambda);
  }
  // get new dela = dela0 - Va0 Dt lambda
  // get new Va
  // get chisq
  HepMatrix covax(7, 3, 0);
  m_chisq[n] = 0;
  for(unsigned int i = 0; i < NSIZE; i++) {
    HepVector DtL(7, 0);
    DtL = (vc.Dc()[i]).T() * vc.lambda()[i];
    int itk = vc.Ltrk()[i];
    HepVector dela(7, 0);
    HepVector dela0(7, 0);
    dela0 = wTrackOrigin(itk).w()-wTrackInfit(itk).w();
    WTrackParameter wtrk = wTrackOrigin(itk);
    dela = -wTrackOrigin(itk).Ew() * DtL; 
    wtrk.setW(wtrk.w()+dela);
    HepSymMatrix Va(7, 0);
    HepSymMatrix Va0(7, 0);
    Va0 = wTrackOrigin(itk).Ew();
    HepMatrix DcT(7, 2, 0);
    DcT = (vc.Dc()[i]).T();
    HepSymMatrix DVdD(7, 0);
    DVdD = (vc.VD()[i]).similarity(DcT);
    Va = Va0 - DVdD.similarity(Va0);
    wtrk.setEw(Va);
    setWTrackInfit(itk, wtrk);
    HepVector dc(2, 0);
    dc = vc.Dc()[i]*dela0 +vc.dc()[i];
    m_chisq[n] = m_chisq[n] + dot(vc.lambda()[i], dc);
  }
  m_vpar_infit[n] = vpar;
  m_vc[n] = vc;
*/
//No needed
}


void VertexFit::swimBeam(int n)
{
/*
  // const double alpha = -0.00299792458;
  //
  //  Track parameter can be expressed as:
  // 
  //  px = px0 + a*(y0 - yv)
  //  py = py0 - a*(x0 - xv)
  //  pz = pz0
  //  e  = e
  //  x  = xv
  //  y  = yv
  //  z  = zv
  //
  //  thus p = A * a + B * vx
  //      x = vx

  
  VertexConstraints vc = m_vc[n];
  VertexParameter  vpar = m_vpar_infit[n];
  unsigned int NSIZE = vc.Ltrk().size();

  for(unsigned int i = 0; i < NSIZE; i++) {
    int itk = vc.Ltrk()[i];
    HepMatrix A(4, 7, 0);
    HepMatrix B(4, 3, 0);
    
    double afield = VertexFitBField::instance()->getCBz(vpar.Vx(), pInfit(itk).sub(4, 6));
    double a = afield * wTrackInfit(itk).charge();
    A[0][0] = 1.0;
    A[0][5] = a;
    A[1][1] = 1.0;
    A[1][4] = -a;
    A[2][2] = 1.0;
    A[3][3] = 1.0;
    B[0][1] = -a;
    B[1][0] = a;
    HepVector p(4, 0);
    p = A * wTrackInfit(itk).w() + B * vpar.Vx();
    HepVector x(3, 0);
    x = vpar.Vx();
    HepVector w(7, 0);
    HepSymMatrix Ew(7, 0);
    for(int j = 0; j < 4; j++)
      w[j] = p[j];
    for(int j = 0; j < 3; j++)
      w[j+4] = x[j];

    WTrackParameter wtrk;
    wtrk.setCharge(wTrackInfit(itk).charge());
    wtrk.setW(w);
    HepSymMatrix Vpv(4, 0);
    Vpv = (wTrackInfit(itk).Ew()).similarity(A);
    for(int j = 0; j < 4; j++) 
      for(int k= 0; k < 4; k++) 
	Ew[j][k] = Vpv[j][k];
    wtrk.setEw(Ew);
    setWTrackInfit(itk, wtrk);
  }
*/
//No needed
}

void VertexFit::BuildVirtualParticle(int n) 
{

	vertexCovMatrix(n);
	TStopwatch timer;
	timer.Start();

	HepMatrix A(NTRKPAR, NTRKPAR * m_nvtrk, 0);
	HepMatrix B(NTRKPAR, NVTXPAR, 0);
	VertexConstraints vc = m_vc[n];
	unsigned int NSIZE = vc.Ltrk().size();
	int charge = 0;

	HepMatrix Ai(6, 6, 0);
	Ai[0][0] = 1.0;
	Ai[1][1] = 1.0;
	Ai[2][2] = 1.0;
	HepMatrix Bi(6, 3, 0);
	Bi[3][0] = 1.0;
	Bi[4][1] = 1.0;
	Bi[5][2] = 1.0;
	HepVector w1(6, 0);
	HepVector w2(7, 0);
	HepSymMatrix Ew(7, 0);
	HepMatrix ew1(6, 6, 0);
	HepMatrix ew2(7, 7, 0);
	double totalE = 0;

	for(unsigned int i = 0; i < NSIZE; i++) 
	{
		int itk = vc.Ltrk()[i];
		charge += wTrackInfit(itk).charge();
//		double afield = VertexFitBField::instance()->getCBz(m_xInfit, pInfit(itk).sub(4, 6));
		double afield = m_factor * VertexFitBField::instance()->getCBz(m_xInfit, pInfit(itk).sub(4, 6));
		double a = afield * wTrackInfit(itk).charge();
		
		totalE += wTrackOrigin(itk).w()[3];
		Ai[0][4] = a;
		Ai[1][3] = -a;
		Bi[0][1] = -a;
		Bi[1][0] = a;
		A.sub(1, NTRKPAR*itk + 1, Ai);
		B += Bi;
	}
	B[3][0] = 1.0;
	B[4][1] = 1.0;
	B[5][2] = 1.0;

	w1  = A * m_pInfit + B * m_xInfit;
	ew1 = m_pcovInfit.similarity(A) + m_xcovInfit.similarity(B) + A*m_E*B.T()+B*(m_E.T())*A.T();

	//convert w1(6x1) to w2(7x1)
	w2[0] = w1[0];
	w2[1] = w1[1];
	w2[2] = w1[2];
	w2[3] = totalE;
	w2[4] = w1[3];
	w2[5] = w1[4];
	w2[6] = w1[5];
	//convert ew1(6x6) to ew2(7x7)
	m_TRB[3][0] = w1[0] / totalE;
	m_TRB[3][1] = w1[1] / totalE;
	m_TRB[3][2] = w1[2] / totalE;
	ew2 = m_TRB * ew1 * m_TRB.T();
	Ew.assign(ew2);
	WTrackParameter vwtrk;
	vwtrk.setCharge(charge);
	vwtrk.setW(w2);
	vwtrk.setEw(Ew);

	m_virtual_wtrk[n] = vwtrk;
	timer.Stop();
	m_cpu[5] += timer.CpuTime();
}    

void VertexFit::UpdateConstraints(const VertexConstraints &vc) 
{
	int ntrk = (vc.Ltrk()).size();
	int type = vc.type();
	switch(type) 
	{
		case 1:
		{
			for (unsigned int i = 0; i < ntrk; i++) 
			{
				int itk = (vc.Ltrk())[i];
				HepVector alpha(6, 0);
				double mass, e;
				HepLorentzVector p;
				HepPoint3D x, vx;
				alpha = pInfit(itk);
				
				mass = wTrackOrigin(itk).mass();
				e    = sqrt(mass*mass + alpha[0]*alpha[0] + alpha[1]*alpha[1] + alpha[2]*alpha[2]);
				p    = HepLorentzVector(alpha[0], alpha[1], alpha[2], e);
				x    = HepPoint3D(alpha[3], alpha[4], alpha[5]);

				vx = HepPoint3D(m_xInfit[0], m_xInfit[1], m_xInfit[2]);
				HepPoint3D delx = vx - x;

//				double afield = VertexFitBField::instance()->getCBz(m_xInfit, wTrackOrigin(itk).X());
				double afield = m_factor * VertexFitBField::instance()->getCBz(m_xInfit, wTrackOrigin(itk).X());
				double a = afield * (0.0+wTrackOrigin(itk).charge());

				double J = a * (delx.x()*p.px() + delx.y()*p.py()) / p.perp2();
				J = std::min(J, 1-1e-4);
				J = std::max(J, -1+1e-4);
				double Rx = delx.x() - 2 * p.px() * (delx.x()*p.px() + delx.y()*p.py()) / p.perp2();
				double Ry = delx.y() - 2 * p.py() *	(delx.x()*p.px() + delx.y()*p.py()) / p.perp2();
				double S  = 1.0 / sqrt(1-J*J) / p.perp2();
				// dc
				HepVector dc(2, 0);
				dc[0] = delx.y()*p.px() - delx.x()*p.py() -	0.5*a*(delx.x()*delx.x() + delx.y()*delx.y());
				dc[1] = delx.z() - p.pz()/a*asin(J);
				//setd(itk, dc);
				// Ec
				HepMatrix Ec(2, 3, 0);
				// m_Ec.push_back(Ec);
				// Dc
				HepMatrix Dc(2, 6, 0);
				Dc[0][0] = delx.y();
				Dc[0][1] = -delx.x();
				Dc[0][2] = 0;
				Dc[0][3] = p.py() + a * delx.x();
				Dc[0][4] = -p.px() + a * delx.y();
				Dc[0][5] = 0;
				Dc[1][0] = -p.pz() * S * Rx;
				Dc[1][1] = -p.pz() * S * Ry;
				Dc[1][2] = - asin(J) / a;
				Dc[1][3] = p.px() * p.pz() * S;
				Dc[1][4] = p.py() * p.pz() * S;
				Dc[1][5] = -1.0;
				// setD(itk, Dc);
				// setDT(itk, Dc.T());
				// VD
				HepSymMatrix vd(2, 0);
				int ifail;
				vd = pcovOrigin(itk).similarity(Dc);
				vd.invert(ifail);
				// setVD(itk, vd);
			}
			break;
		}
 		case 2:
		default: 
		{
			for (unsigned int i = 0; i < ntrk; i++) 
			{
				int itk = (vc.Ltrk())[i];
				HepVector alpha(6, 0);
				double mass, e;
				HepLorentzVector p;
				HepPoint3D x, vx;
				alpha = pInfit(itk);
				//p  = HepLorentzVector(alpha[0], alpha[1], alpha[2], alpha[3]);
				//x  = HepPoint3D(alpha[4], alpha[5], alpha[6]);

				mass = wTrackOrigin(itk).mass();
				e    = sqrt(mass*mass + alpha[0]*alpha[0] + alpha[1]*alpha[1] + alpha[2]*alpha[2]);
				p    = HepLorentzVector(alpha[0], alpha[1], alpha[2], e);
				x    = HepPoint3D(alpha[3], alpha[4], alpha[5]);
				
				vx = HepPoint3D(m_xInfit[0], m_xInfit[1], m_xInfit[2]);
				HepPoint3D delx = vx - x;

				if (wTrackOrigin(itk).charge() == 0) 
				{
					// dc -->g
					HepVector dc(2, 0);
					dc[0] = p.pz()*delx.x() - p.px()*delx.z();
					dc[1] = p.pz()*delx.y() - p.py()*delx.z();
					// Ec --> B
					HepMatrix Ec(2, 3, 0);
					Ec[0][0] = p.pz();
					Ec[0][1] = 0;
					Ec[0][2] = -p.px();
					Ec[1][0] = 0;
					Ec[1][1] = p.pz();
					Ec[1][2] = -p.py();
					setB(itk, Ec);
					setBT(itk, Ec.T());
					// Dc
					HepMatrix Dc(2, 6, 0);
					Dc[0][0] = -delx.z();
					Dc[0][1] = 0;
					Dc[0][2] = delx.x();
					Dc[0][3] = -p.pz();
					Dc[0][4] = 0;
					Dc[0][5] = p.px();
					Dc[1][0] = 0;
					Dc[1][1] = -delx.z();
					Dc[1][2] = delx.y();
					Dc[1][3] = 0;
					Dc[1][4] = -p.pz();
					Dc[1][5] = p.py();
					setA(itk, Dc);
					setAT(itk, Dc.T());
					
					// VD --> W
					HepSymMatrix vd(2, 0);
					int ifail;
	
					vd = pcovOrigin(itk).similarity(Dc);
					vd.invert(ifail);
					setW(itk,vd);

					//G=A(p0-pe)+B(x0-xe)+d
					HepVector gc(2,0);
					gc = Dc*(pOrigin(itk)-pInfit(itk)) + Ec*(m_xOrigin-m_xInfit) + dc;
					setG(itk,gc);

				} 
				else 
				{
	//				double afield = VertexFitBField::instance()->getCBz(m_xInfit,wTrackOrigin(itk).X());
					double afield = m_factor * VertexFitBField::instance()->getCBz(m_xInfit,wTrackOrigin(itk).X());
					double a = afield * (0.0+wTrackOrigin(itk).charge());
					double J = a * (delx.x()*p.px() + delx.y()*p.py())/p.perp2();
					J = std::min(J, 1-1e-4);
					J = std::max(J, -1+1e-4);
					double Rx = delx.x() - 2*p.px()*(delx.x()*p.px() + delx.y()*p.py())/p.perp2();
					double Ry = delx.y() - 2*p.py()*(delx.x()*p.px() + delx.y()*p.py())/p.perp2();
					double S  = 1.0 / sqrt(1-J*J) / p.perp2();

					// dc
					HepVector dc(2, 0);
					dc[0] = delx.y() * p.px() - delx.x() * p.py() - 0.5 * a * (delx.x() * delx.x() + delx.y() * delx.y());
					dc[1] = delx.z() - p.pz() / a*asin(J);
					// Ec
					HepMatrix Ec(2, 3, 0);
					Ec[0][0] = -p.py()- a * delx.x();
					Ec[0][1] = p.px() - a * delx.y();
					Ec[0][2] = 0;
					Ec[1][0] = -p.px() * p.pz() * S ;
					Ec[1][1] = -p.py() * p.pz() * S ;
					Ec[1][2] = 1.0;
					setB(itk, Ec);
					setBT(itk, Ec.T());
 
					//Dc
					HepMatrix Dc(2,6,0);
					Dc[0][0] = delx.y();
					Dc[0][1] = -delx.x();
					Dc[0][2] = 0;
					Dc[0][3] = p.py() + a*delx.x();
					Dc[0][4] = -p.px() + a*delx.y();
					Dc[0][5] = 0;

					Dc[1][0] = -p.pz()*S*Rx;
					Dc[1][1] = -p.pz()*S*Ry;
					Dc[1][2] = -asin(J)/a;
					Dc[1][3] = p.px()*p.pz()*S;
					Dc[1][4] = p.py()*p.pz()*S;
					Dc[1][5] = -1.0;
					setA(itk,Dc);
					setAT(itk,Dc.T());
					// VD
					HepSymMatrix vd(2, 0);
					int ifail;
					vd = pcovOrigin(itk).similarity(Dc);
					vd.invert(ifail);
					setW(itk, vd);
					// G = A(p0-pe)+B(x0-xe)+d
					HepVector gc(2, 0);
					gc = Dc * (pOrigin(itk) - pInfit(itk)) + Ec *(m_xOrigin - m_xInfit) + dc;
					setG(itk, gc);
				}
			}
			break;
		}
	}
}

HepVector VertexFit::Convert67(const double &mass, const HepVector &p)
{
//	assert(p.num_row() == 6);
	HepVector m(7, 0);
	m.sub(1, p.sub(1, 3));
	m.sub(5, p.sub(4, 6));
	m[3] = sqrt(mass*mass + p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
	return m;
}

HepVector VertexFit::Convert76(const HepVector &p)
{
//	assert(p.num_row() == 7);
	HepVector m(6, 0);
	m.sub(1, p.sub(1, 3));
	m.sub(4, p.sub(5, 7));
	return m;
}

