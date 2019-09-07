#include "VertexFit/FastVertexFit.h"
#include "VertexFit/BField.h" //FIXME

const double alpha = -0.00299792458;
using namespace std;

FastVertexFit * FastVertexFit::m_pointer = 0;

FastVertexFit * FastVertexFit::instance() {
  if(m_pointer) return m_pointer;
  m_pointer = new FastVertexFit();
  return m_pointer;
}

FastVertexFit::~FastVertexFit() {;}

FastVertexFit::FastVertexFit() {;}

void FastVertexFit::init() {
  m_D.clear();
  m_W.clear();
  m_DTWD.clear();
  m_xp.clear();
  m_chisq = 999;
  m_Vx = HepVector(3, 0);
  m_Evx = HepSymMatrix(3,0);
}

void FastVertexFit::addTrack(const int number, const HepVector helix, const HepSymMatrix err) {
  int ifail;
  int ierr;
  //HepSymMatrix err0(5, 0); //inverse of err;
  //err0 = err.inverse(ifail);
  HepSymMatrix Wc(2, 0);
  Wc[0][0] = err[0][0];
  Wc[0][1] = Wc[1][0] = err[0][3];
  Wc[1][1] = err[3][3];
  Wc = Wc.inverse(ifail);

  HepVector p0(3, 0), v0(3, 0);
  double pxy = 1./fabs(helix[2]);
  p0[0] = 0 - pxy*sin(helix[1]); //px
  p0[1] = pxy*cos(helix[1]);     //py
  p0[2] = pxy * helix[4];        //pz
  v0[0] = helix[0]*cos(helix[1]);//x
  v0[1] = helix[0]*sin(helix[1]);//y
  v0[2] = helix[3];              //z  

  HepPoint3D vv0(v0[0], v0[1], v0[2]);
  double bField = VertexFitBField::instance()->getBFieldZRef();

  int charge = (helix[2]>0 ? +1 :-1);
  double a = alpha * bField * charge;
  double T0 = sqrt((p0[0]+a*p0[1])*(p0[0]+a*p0[1])+(p0[1]-a*p0[0])*(p0[1]-a*p0[0]));

  HepMatrix Dc(2, 3, 0);
  Dc[0][0] = (p0[1] - a*v0[0])/T0;
  Dc[0][1] = 0 - (p0[0] + a*v0[1])/T0;
  Dc[1][0] = 0 - (p0[2]/T0) * (p0[0] + a*v0[1])/T0;
  Dc[1][1] = 0 - (p0[2]/T0) * (p0[1] - a*v0[0])/T0;
  Dc[1][2] = 1;

  m_D.push_back(Dc);
  m_W.push_back(Wc);

  HepSymMatrix DTWD(3, 0);
  DTWD = Wc.similarity(Dc.T());
  HepVector qTrk(2, 0);
  qTrk[0] = helix[0];
  qTrk[1] = helix[3];
  //HepVector xp(3, 0);
  //xp = Dc.inverse(ierr) * qTrk;
  m_DTWD.push_back(DTWD);
  m_xp.push_back(v0);
}

bool FastVertexFit::Fit() {
  bool fitResult = false;

  int ifail;
  HepSymMatrix total_DTWD(3, 0);
  HepVector total_xp(3, 0);

  for(int i = 0; i < m_DTWD.size(); i++) {
    total_DTWD += m_DTWD[i];  
    total_xp += m_DTWD[i]*m_xp[i]; 
  }
  m_Vx = total_DTWD.inverse(ifail) * total_xp;
  m_Evx = total_DTWD.inverse(ifail);

  double chisq = 0;
  for(int i = 0; i < m_xp.size(); i++) {
    double chi2 = (m_DTWD[i].similarity((m_xp[i] - m_Vx).T()))[0][0];  
    chisq += chi2;
  }
  m_chisq = chisq;

  fitResult = true;
  return fitResult;
}

HepVector FastVertexFit::Pull() const {
  HepVector pull(3, 0);
  pull[0] = m_Vx[0]/sqrt(m_Evx[0][0]);
  pull[1] = m_Vx[1]/sqrt(m_Evx[1][1]);
  pull[2] = m_Vx[2]/sqrt(m_Evx[2][2]);
  return pull;
}

//FastVertexFit::updateMatrices(const HepVector helix, const HepSymMatrix err) {
  
//}
