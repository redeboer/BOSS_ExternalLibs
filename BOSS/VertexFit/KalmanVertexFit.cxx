//
//  Kalman Vertex fit 
//  The algoritm is based on a paper by 
//  R. Fruhwirth etc Computer Physics Communications 96(1996) 189-208
//  The formulism for BES3 can be find at BESIII Software Note: No. xx
//  Author: Kanglin He and Min Xu,  Apr. 13, 2007 for created
//


#include "VertexFit/KalmanVertexFit.h"
#include "VertexFit/HTrackParameter.h"
#include "VertexFit/WTrackParameter.h"
#include "math.h"

using namespace std;

KalmanVertexFit * KalmanVertexFit::m_pointer = 0;

KalmanVertexFit * KalmanVertexFit::instance() {
  if(m_pointer) return m_pointer;
  m_pointer = new KalmanVertexFit();
  return m_pointer;
}

KalmanVertexFit::~KalmanVertexFit() {;}

KalmanVertexFit::KalmanVertexFit() {;}

void KalmanVertexFit::init() {
  m_x = HepVector(3, 0);
  m_C0 = HepSymMatrix(3, 0);
  m_C = HepSymMatrix(3, 0);
  m_chisq = 0;
  m_ndof = -3;

  m_flag.clear();
  m_p.clear();
  m_hTrkOrigin.clear();
  m_hTrkInfit.clear(); //xum
  //m_wTrkInfit.clear();

  m_G.clear();
  m_A.clear();
  m_B.clear();
  m_c.clear();
  m_W.clear();
  m_GB.clear();
  m_chiF.clear();

  //
  // chi-square cut, number of iteration etc
  //
  m_chisqCutforTrack = 50;
  m_maxVertexIteration = 3;
  m_maxTrackIteration = 5;
  m_chisqCutforTrackIteration = 0.1;
}

void KalmanVertexFit::initVertex(const VertexParameter vtx) {
  int ifail;
  m_x = vtx.Vx();
  m_C0 = (vtx.Evx()).inverse(ifail);
  m_C = m_C0;
}

VertexParameter KalmanVertexFit::vtx() const {
  int ifail;
  VertexParameter v;
  v.setVx(m_x);

  std::vector<int> trackid;
    v.setEvx(m_C.inverse(ifail));
  return v;
}

HepSymMatrix KalmanVertexFit::Ex() const {
  int ifail;
  return m_C.inverse(ifail);
}

void KalmanVertexFit::addTrack(const HTrackParameter htrk) {

  int ifail;

  HepVector p(3, 0);
  p = htrk.p();
  
  m_p.push_back(p);
  m_flag.push_back(0);
  m_chiF.push_back(999.);

  HepMatrix A(5, 3, 0);
  HepMatrix B(5, 3, 0);
  HepSymMatrix G(5, 0); 

  G = (htrk.eHel()).inverse(ifail);

  m_hTrkOrigin.push_back(htrk);
 
  //xum
  m_hTrkInfit.push_back(htrk); 
  //m_wTrkInfit.push_back(htrk.wTrack());

  m_G.push_back(G);
  m_A.push_back(A);
  m_B.push_back(B);
  
  HepSymMatrix W(3, 0);
  HepSymMatrix GB(5, 0);
  HepVector c(5, 0);

  m_W.push_back(W);
  m_GB.push_back(GB);
  m_c.push_back(c);

}

int KalmanVertexFit::numTrack() const {
  int num = 0;
  for(int k = 0; k < m_hTrkOrigin.size(); k++) 
    if(m_flag[k] == 0) num = num +1;
  
  return num;
}

std::vector<int> KalmanVertexFit::trackIDList() const {
  std::vector<int> trackid;
  for(int k = 0; k < m_hTrkOrigin.size(); k++)
    if(m_flag[k] == 0) trackid.push_back(trackID(k));

  return trackid;
}


int KalmanVertexFit::filter(const int k) {
  int ifail;
  double chisq[10];
  ////////////////////////////////////////////////
  // start iteration
  ///////////////////////////////////////////////
  HepVector pp(3, 0);
  HepVector xp(3, 0);
  HepSymMatrix CP(3, 0);
  xp = m_x;
  CP = m_C;
  pp = m_p[k];

  for(int iter = 0; iter < m_maxTrackIteration; iter++) {
    //
    // calculate derivative matrices for track k
    //
    updateMatrices(k, pp, xp);
    //
    // vertex covariance matrix updated
    //
    HepSymMatrix CK(3, 0);
    CK = CP + (m_GB[k].similarity(m_A[k].T()));
    //
    // state vector x and p updated
    //
    HepVector x(3, 0);
    HepVector p(3, 0);
    x = CK.inverse(ifail) * (CP * xp + m_A[k].T() * m_GB[k] * 
			     (m_hTrkOrigin[k].hel() - m_c[k]));
    p = m_W[k] * m_B[k].T() * m_G[k] *(m_hTrkOrigin[k].hel()- m_c[k]- 
				       m_A[k]*x);
    // chi-square of filter 
    HepSymMatrix chif(1, 0);
    chif = CP.similarity((x-xp).T()) + 
      m_G[k].similarity((m_hTrkOrigin[k].hel()- 
			 m_c[k]- m_A[k]*x - m_B[k]*p).T());
    
    //
    // save current 3-momentum and vertex for track k
    //
    // 3-momentum
    pp = p;
    // current vertex and its covaiance matrix
    xp = x;
    CP = CK;
    chisq[iter] = chif[0][0];
    m_chiF[k] = chif[0][0];
    //    std::cout << "chisq, iter = " <<m_chiF[k] << ", " << iter << ", " << k <<std::endl;
    if(iter > 0) {
      double delchi = (chisq[iter]-chisq[iter-1]);
      if(fabs(delchi) < m_chisqCutforTrackIteration) break;
    }
  }

  if(m_chiF[k] > m_chisqCutforTrack) {
    //    do not update vertex if failed
    m_flag[k]=1;
    return 1;
  } else { 
    // update vertex if filter success
    m_x = xp;
    m_C = CP;
    m_p[k] = pp;
    return 0;
  }
}

void KalmanVertexFit::inverse(const int k) {
  //
  // inverse kalman filter for track k
  //

  int ifail;
  HepVector x(3, 0);
  HepVector p(3, 0);
  HepSymMatrix C(3, 0);
  C = m_C - m_GB[k].similarity(m_A[k].T());
  x = C.inverse(ifail)*(m_C*m_x-m_A[k].T() * m_GB[k] * 
			(m_hTrkOrigin[k].hel()-m_c[k]));
  //
  //  update vertex position and its covariance matrix
  //
  m_x = x; 
  m_C = C; 
}
void KalmanVertexFit::filter() {
  
  for(int iter = 0; iter < m_maxVertexIteration; iter++) {
    //
    //  at start of each iteration, 
    //  the covariance matrix take the initial values: m_C0
    //  the vertex position take the update value by the last iteration
    //
    m_C = m_C0;   // covariance matrix of verter --> initial values
    m_chisq = 0;  // total chi-square of smooth
    m_ndof = -3;  // number of degrees of freedom
   
    for(int k = 0; k < m_hTrkOrigin.size(); k++) {
      if(m_flag[k]==1) continue;
      if(filter(k)==0) // filter track k
	m_ndof += 2;

    }  
    //
    //  check the chi-square of smooth, make decision to remove bad tracks
    //  user may remove more bad track by tight chi-square cut
    //  through the following interface 
    //  KalmanVertexFit::remove(k)
    //
    for(int k = 0; k < m_hTrkOrigin.size(); k++) {
      if(m_flag[k]==1) continue;
      double chi2 = chiS(k);
      if(chi2 < 0) { // remove track k from vertex
	remove(k);
	continue;
      }
      
      if(chi2 > m_chisqCutforTrack) { // remove track k from vertex
	remove(k);
	continue;
      } 
      m_chisq += chi2;

    }
  } // end of iteration
}

double KalmanVertexFit::chiS(const int k) const {
  if(m_flag[k]==1) return 999;
  int ifail;
  HepVector x(3, 0);
  HepVector p(3, 0);
  HepSymMatrix C(3, 0);
  C = m_C - m_GB[k].similarity(m_A[k].T());
  x = C.inverse(ifail)*(m_C*m_x-m_A[k].T() * m_GB[k] * 
			(m_hTrkOrigin[k].hel()-m_c[k]));
  p = m_W[k]* m_B[k].T()*m_G[k]*
    (m_hTrkOrigin[k].hel()-m_c[k]-m_A[k]*m_x);
  HepSymMatrix chis(1, 0);
  chis = C.similarity((x-m_x).T())+
    m_G[k].similarity((m_hTrkOrigin[k].hel()-m_c[k]-
		       m_A[k]*m_x-m_B[k]*p).T());
  return chis[0][0];
}

void KalmanVertexFit::smooth(const int k) {
  // xum
  //
  // update htrk and wtrk parameters
  //
  if(m_flag[k] == 1) return;
  int ifail;

  HepVector pp(3, 0);
  HepVector xp(3, 0);
  HepSymMatrix CP(3, 0);
  xp = m_x;
  CP = m_C;
  pp = m_p[k];

  updateMatrices(k, pp, xp);
  pp = m_W[k] * m_B[k].T() * m_G[k] * (m_hTrkOrigin[k].hel() - m_c[k] - m_A[k] * xp);
  
  updateMatrices(k, pp, xp);
  //for htrk
  HepVector helix(5, 0);
  helix = m_c[k] + m_A[k] * xp + m_B[k] * pp;
  HepMatrix E(3, 3, 0);
  E = -CP.inverse(ifail) * m_A[k].T() * m_G[k] * m_B[k] * m_W[k];
  HepSymMatrix D(3, 0);
  D = m_W[k] + CP.similarity(E.T());
  
  HepMatrix middle(5, 5, 0);
  HepSymMatrix error(5, 0);
  middle = (CP.inverse(ifail)).similarity(m_A[k]) + m_A[k] * E * m_B[k].T() +
          (m_A[k] * E * m_B[k].T()).T() + D.similarity(m_B[k]);  
  error.assign(middle);

  m_hTrkInfit[k].setHel(helix);
  m_hTrkInfit[k].setEHel(error);
  
  m_p[k] = pp;
/*
  //for wtrk
  double mass = m_hTrkOrigin[k].xmass(m_hTrkOrigin[k].partID());
  double Energy = sqrt(pp[0]*pp[0] + pp[1]*pp[1] + pp[2]*pp[2] + mass * mass);

  HepMatrix Awtrk(7, 3, 0), Bwtrk(7, 3, 0);
  Awtrk[4][0] = Awtrk[5][1] = Awtrk[6][2] = 1;
  Bwtrk[0][0] = Bwtrk[1][1] = Bwtrk[2][2] = 1;
  Bwtrk[3][0] = pp[0] / Energy;
  Bwtrk[3][1] = pp[1] / Energy;
  Bwtrk[3][2] = pp[2] / Energy;
    
  HepVector w(7, 0);
  HepSymMatrix Ew(7, 0);
  w[0] = pp[0];   w[1] = pp[1];  w[2] = pp[2];  w[3] = Energy; 
  w[4] = xp[0];   w[5] = xp[1];  w[6] = xp[2];
  
  HepSymMatrix Gwtrk(7, 0);
  Gwtrk = m_hTrkOrigin[k].wTrack().Ew().inverse(ifail);
  HepSymMatrix Wwtrk(3, 0);
  Wwtrk = Gwtrk.similarity(Bwtrk.T()).inverse(ifail);

  HepMatrix Ewtrk(3, 3, 0);
  Ewtrk = -CP.inverse(ifail) * m_Awtrk[k].T() * m_Gwtrk[k] * m_Bwtrk[k] * m_Wwtrk[k];
  HepSymMatrix Dwtrk(3, 0);
  Dwtrk = m_Wwtrk[k] + CP.similarity(Ewtrk.T());

  HepMatrix Ewmiddle(7, 7, 0);
  Ewmiddle = (CP.inverse(ifail)).similarity(m_Awtrk[k]) + m_Awtrk[k] * Ewtrk * m_Bwtrk[k].T() +
          (m_Awtrk[k] * Ewtrk * m_Bwtrk[k].T()).T() + Dwtrk.similarity(m_Bwtrk[k]);         
  Ew.assign(Ewmiddle);

  m_wTrkInfit[k].setCharge(m_hTrkOrigin[k].charge());
  m_wTrkInfit[k].setW(w);
  m_wTrkInfit[k].setEw(Ew);*/
}

void KalmanVertexFit::remove(const int k) {
  //
  // remove track k from the vertex fit
  //
  /*
    m_hTrkOrigin.erase(m_hTrkOrigin.begin()+k);
    m_p.erase(m_p.begin()+k);
    m_flag.erase(m_flag.begin()+k);
    m_A.erase(m_A.begin()+k);
    m_B.erase(m_B.begin()+k);
    m_c.erase(m_c.begin()+k);
    m_G.erase(m_G.begin()+k);
    m_GB.erase(m_GB.begin()+k);
    m_W.erase(m_W.begin()+k);
    m_chiF.erase(m_chiF.begin()+k);
  */

  if(m_flag[k]==1) return; // track k already removed
  inverse(k);
  m_ndof -= 2;
  m_flag[k]=1;
  //xum
  //m_chisq -=chiS(k);
}


HTrackParameter KalmanVertexFit::hTrack(const int k) const{
  //add xum
  //HTrackParameter htrk;
  //return htrk;
  return m_hTrkInfit[k];
}

WTrackParameter KalmanVertexFit::wTrack(const int k, const double mass) const {
  if(m_flag[k] == 1) return m_hTrkOrigin[k].wTrack();
  int ifail;

  HepVector pp(3, 0);
  HepVector xp(3, 0);
  HepSymMatrix CP(3, 0);
  xp = m_x;
  CP = m_C;
  pp = m_p[k];

  WTrackParameter wtrk;
  double Energy = sqrt(pp[0]*pp[0] + pp[1]*pp[1] + pp[2]*pp[2] + mass * mass);
  
  HepMatrix Awtrk(7, 3, 0), Bwtrk(7, 3, 0);
  Awtrk[4][0] = Awtrk[5][1] = Awtrk[6][2] = 1;
  Bwtrk[0][0] = Bwtrk[1][1] = Bwtrk[2][2] = 1;
  Bwtrk[3][0] = pp[0] / Energy;
  Bwtrk[3][1] = pp[1] / Energy;
  Bwtrk[3][2] = pp[2] / Energy;
    
  HepVector w(7, 0);
  HepSymMatrix Ew(7, 0);
  w[0] = pp[0];   w[1] = pp[1];  w[2] = pp[2];  w[3] = Energy;
  w[4] = xp[0];   w[5] = xp[1];  w[6] = xp[2];  
  
  HepSymMatrix Gwtrk(7, 0);
  Gwtrk = m_hTrkOrigin[k].wTrack().Ew().inverse(ifail);
  HepSymMatrix Wwtrk(3, 0);
  Wwtrk = Gwtrk.similarity(Bwtrk.T()).inverse(ifail);
  
  HepMatrix Ewtrk(3, 3, 0);
  Ewtrk = -CP.inverse(ifail) * Awtrk.T() * Gwtrk * Bwtrk * Wwtrk;
  HepSymMatrix Dwtrk(3, 0);
  Dwtrk = Wwtrk + CP.similarity(Ewtrk.T());

  HepMatrix Ewmiddle(7, 7, 0);
  Ewmiddle = (CP.inverse(ifail)).similarity(Awtrk) + Awtrk * Ewtrk * Bwtrk.T() +
          (Awtrk * Ewtrk * Bwtrk.T()).T() + Dwtrk.similarity(Bwtrk);
  Ew.assign(Ewmiddle);

  wtrk.setCharge(m_hTrkOrigin[k].charge());
  wtrk.setW(w);
  wtrk.setEw(Ew);

  return wtrk;
}

//
// private functions, update derivative matrices
//
void KalmanVertexFit::updateMatrices(const int k, const HepVector p, const HepVector x) {

  int ifail;
  //
  // expand measurement equation at (x, p)
  //
  HTrackParameter he(m_hTrkOrigin[k].charge(), p, x);
 
  // derivative matrix
  
  m_A[k] = he.dHdx(p, x);
  m_B[k] = he.dHdp(p, x);

  // W, GB, c
  m_W[k] = (m_G[k].similarity(m_B[k].T())).inverse(ifail);
  m_GB[k] = m_G[k] - (m_W[k].similarity(m_B[k])).similarity(m_G[k]);
  m_c[k] = he.hel() - m_A[k] * x - m_B[k] * p;  
}

void KalmanVertexFit::updateMatrices(const int k) {

  int ifail;
  HepVector p(3, 0);
  HepVector x(3, 0);
  p = m_p[k];
  x = m_x;
  updateMatrices(k, p, x);
}

HepVector KalmanVertexFit::pull(const int k) {
  HepVector n_pull(5, 0);
  n_pull[0] = n_pull[1] = n_pull[2] = n_pull[3] = n_pull[4] = 999.;
  if(m_flag[k] == 1) return n_pull; 
  for (int i = 0 ; i < 5; i++) {
    double cov = m_hTrkOrigin[k].eHel()[i][i] - m_hTrkInfit[k].eHel()[i][i];
    if (cov == 0.) continue;    
    n_pull[i] = (m_hTrkOrigin[k].hel()[i] - m_hTrkInfit[k].hel()[i]) / sqrt(cov);
  }
  return n_pull;
}

double KalmanVertexFit::pullmomentum(const int k) {
  double pull = 999.;
  if(m_flag[k] == 1) return pull;

  double kappa_origin = m_hTrkOrigin[k].kappa();
  double kappa_infit = m_hTrkInfit[k].kappa();
  double lamb_origin = m_hTrkOrigin[k].lambda();
  double lamb_infit = m_hTrkInfit[k].lambda();

  double Vkappa_origin = m_hTrkOrigin[k].eHel()[2][2];
  double Vkappa_infit = m_hTrkInfit[k].eHel()[2][2];
  double Vlamb_origin = m_hTrkOrigin[k].eHel()[4][4];
  double Vlamb_infit = m_hTrkInfit[k].eHel()[4][4];
  double V_kappa_lamb_origin = m_hTrkOrigin[k].eHel()[4][2];
  double V_kappa_lamb_infit = m_hTrkInfit[k].eHel()[4][2];

  double P_origin = calculationP(kappa_origin, lamb_origin);
  //cout << "P_origin = " << P_origin << endl;
  //P_origin = m_hTrkOrigin[k].p()[0] * m_hTrkOrigin[k].p()[0]
  //          + m_hTrkOrigin[k].p()[1] * m_hTrkOrigin[k].p()[1]
  //          + m_hTrkOrigin[k].p()[2] * m_hTrkOrigin[k].p()[2];
  //cout << "P = " << P_origin << endl;
  double P_infit = calculationP(kappa_infit, lamb_infit);

  double SigmaP_origin = calculationSigmaP(kappa_origin, lamb_origin, Vkappa_origin,
                        Vlamb_origin, V_kappa_lamb_origin);
  double SigmaP_infit = calculationSigmaP(kappa_infit, lamb_infit, Vkappa_infit,
                        Vlamb_infit, V_kappa_lamb_infit);
  if ((SigmaP_origin - SigmaP_infit) <= 0.)  return pull;
  pull = (P_origin - P_infit) / sqrt (SigmaP_origin - SigmaP_infit);
 
  return pull;
}

//====================== sub routine ==================================
double KalmanVertexFit::calculationP(const double kappa, const double lamb) {
  double P = 0.0;
  P = sqrt(1 + lamb * lamb) / kappa;
  //cout << "P in calculationP = " << P << endl;
  return P; 
}

double KalmanVertexFit::calculationSigmaP(const double kappa, const double lamb, 
                                  const double Vkappa, const double Vlamb, 
                                  const double Vkappa_lamb) {
  double SigmaP = 0.0;
  double dp_dkappa = -sqrt(1 + lamb*lamb) /kappa/kappa;
  double dp_dlamb = lamb / kappa / sqrt(1 + lamb*lamb);
  SigmaP = dp_dkappa * dp_dkappa * Vkappa + dp_dlamb * dp_dlamb * Vlamb
         + 2 * dp_dkappa * dp_dlamb * Vkappa_lamb;
  return SigmaP;
}
