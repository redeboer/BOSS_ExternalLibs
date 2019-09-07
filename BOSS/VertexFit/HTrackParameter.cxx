#include "VertexFit/HTrackParameter.h"
#include "VertexFit/WTrackParameter.h"
#include "VertexFit/BField.h"
#include "CLHEP/Units/PhysicalConstants.h"
using namespace CLHEP;
const double alpha = -0.00299792458;
//const double bField = 1.0;

HTrackParameter::HTrackParameter() {
  m_trackID = -1;
  m_partID = -1;
  m_hel = HepVector(5, 0);
  m_eHel = HepSymMatrix(5, 0);
}

HTrackParameter::HTrackParameter(const HTrackParameter &htrk) {
  m_trackID = htrk.m_trackID;
  m_partID = htrk.m_partID;
  m_hel = htrk.m_hel;
  m_eHel = htrk.m_eHel;
}

HTrackParameter &HTrackParameter :: operator = (const HTrackParameter &htrk) {
  m_trackID = htrk.m_trackID;
  m_partID = htrk.m_partID;
  m_hel = htrk.m_hel;
  m_eHel = htrk.m_eHel;
  return (*this);
}

//
// for DstMdcKalTrack --> HTrackParameter
//


HTrackParameter::HTrackParameter(const HepVector h, const HepSymMatrix eH,
				 const int trackid, const int partid) {  

  m_hel = HepVector(5, 0);
  m_eHel = HepSymMatrix(5, 0);

  m_trackID = trackid;
  m_partID = partid;
  m_hel = h;
  m_eHel = eH;

}

//
//  for DstMdcTrack --> HTrackParameter
//


HTrackParameter::HTrackParameter(const HepVector h, const double eH[],
				 const int trackid, const int partid) {

  m_hel = HepVector(5, 0);
  m_eHel = HepSymMatrix(5, 0);

  m_trackID = trackid;
  m_partID = partid;
  m_hel = h;
  int k = 0;
  for(int i = 0; i < 5; i++){
    for(int j = 0; j < 5; j++) {
      m_eHel[i][j] = eH[k];
      m_eHel[j][i] = eH[k];
      k++;
    }
  }
}

//
//  WTrackParameter --> HTrackParameter
//

HTrackParameter::HTrackParameter(const WTrackParameter wtrk) {
  
  HepVector p(3, 0);
  HepVector x(3, 0);
  for(int i = 0; i < 3; i++) {
    p[i] = wtrk.w()[i];
    x[i] = wtrk.w()[i+4];
  }
  
  HTrackParameter htrk(wtrk.charge(), p, x);
  HepMatrix A(5, 3, 0);
  HepMatrix B(5, 3, 0);
  
  A = htrk.dHdx(p, x);
  B = htrk.dHdp(p, x);
  HepMatrix T(5, 7, 0);
  for(int i = 0; i < 5; i++) {
    for(int j = 0; j < 3; j++){
      T[i][j] = B[i][j];
      T[i][j+4] = A[i][j];
    }
  }

  HepSymMatrix eH(5, 0);
  eH = (wtrk.Ew()).similarity(T);
  int m_trackID = -1;
  double mass = (wtrk.p()).m();
  int m_partID = -1;
  for(int i = 0; i < 5; i++) {
    if(fabs(mass - xmass(i)) < 0.01) {
      m_partID = i;
      break;
    }
  }
  //  htrk.setTrackID(trackID);
  //  htrk.setPartID(partID);
  htrk.setEHel(eH);
  m_hel = htrk.hel();
  m_eHel = htrk.eHel();
}

//
//  radius and centers of Helix in xy plane
//

double HTrackParameter::radius() const {
  double bField = VertexFitBField::instance()->getBFieldZ(x3());
  int charge = m_hel[2] > 0 ? +1: -1;
  double a = alpha * bField * charge;
  double pxy = charge/m_hel[2];
  return (pxy/a);
}

HepPoint3D HTrackParameter::center() const {
  double bField = VertexFitBField::instance()->getBFieldZ(x3());
  int charge = m_hel[2] > 0 ? +1: -1;
  double a = alpha * bField * charge;
  double pxy = charge/m_hel[2];
  double rad = pxy/a;
  double x = (m_hel[0] + rad) * cos(m_hel[1]);
  double y = (m_hel[0] + rad) * sin(m_hel[1]);
  double z = 0.0;
  return HepPoint3D(x, y, z);
}

//
// Helix parameter from instantaneous position and momentum
//

HTrackParameter::HTrackParameter(const int charge, const HepVector p, const HepVector vx) {
  m_trackID = -1;
  m_partID = -1;
  m_hel = HepVector(5, 0);
  m_eHel = HepSymMatrix(5, 0);

  HepPoint3D xyz(vx[0], vx[1], vx[2]);
  double bField = VertexFitBField::instance()->getBFieldZ(xyz);
  //std::cout << "bField in HTrackParameter(charge,p,vx) = " << bField << std::endl; 

  double a = alpha * bField * charge;
  double px = p[0];
  double py = p[1];
  double pz = p[2];

  double x = vx[0];
  double y = vx[1];
  double z = vx[2];

  double pxy = sqrt(px*px+py*py);
  double T = sqrt((px+a*y)*(px+a*y)+(py-a*x)*(py-a*x));
  double J= (x*px+y*py)/T*a/pxy;
 
  double drho = (pxy/a)-(T/a);
  double phi0 = fmod((4*CLHEP::pi)+atan2(0-px-a*y, py-a*x), (2*CLHEP::pi));
  double kappa = charge/pxy;
  double dz = z - (pz/a) *asin(J);
  double lambda = pz/pxy;
  
  m_hel[0] = drho; m_hel[1] = phi0; m_hel[2] = kappa; 
  m_hel[3] = dz; m_hel[4] = lambda;
}

//
//  derivative Matrix, used in Kalman Vertex Fit   A= dH/dx
//

HepMatrix HTrackParameter::dHdx(const HepVector p, const HepVector vx){
  HepPoint3D xyz(vx[0], vx[1], vx[2]);
  double bField = VertexFitBField::instance()->getBFieldZ(xyz);
  //std::cout << "bField in dHdx(p,vx) = " << bField << std::endl;

  double a = alpha * bField * charge();
  double px = p[0];
  double py = p[1];
  double pz = p[2];

  double x = vx[0];
  double y = vx[1];
  //  double z = vx[2];

  //  double pxy = sqrt(px*px+py*py);
  double T = sqrt((px+a*y)*(px+a*y)+(py-a*x)*(py-a*x));
  //  double J= (x*px+y*py)/T*a/pxy;

  HepMatrix m_A(5, 3, 0);

  m_A[0][0] = (py-a*x)/T;
  m_A[0][1] = 0 -(px + a*y)/T;
  m_A[1][0] = 0- a*(px + a*y)/T/T;
  m_A[1][1] = 0 - a * (py - a*x)/T/T;
  m_A[3][0] = 0 - (pz/T)*(px + a*y)/T;
  m_A[3][1] = 0 - (pz/T)*(py - a*x)/T;
  m_A[3][2] = 1;

  return m_A;
}

//
//  derivative Matrix, used in Kalman Vertex Fit   B= dH/dp
//

HepMatrix HTrackParameter::dHdp(const HepVector p, const HepVector vx){
  HepPoint3D xyz(vx[0], vx[1], vx[2]);
  double bField = VertexFitBField::instance()->getBFieldZ(xyz);

  double a = alpha * bField * charge();
  double px = p[0];
  double py = p[1];
  double pz = p[2];

  double x = vx[0];
  double y = vx[1];
  //  double z = vx[2];

  double pxy = sqrt(px*px+py*py);
  double T = sqrt((px+a*y)*(px+a*y)+(py-a*x)*(py-a*x));
  double J= (x*px+y*py)/T*a/pxy;

  HepMatrix m_B(5, 3, 0);

  m_B[0][0] = (px/pxy - (px+a*y)/T)/a;
  m_B[0][1] = (py/pxy - (py-a*x)/T)/a;
  m_B[1][0] = 0 -(py-a*x)/T/T;
  m_B[1][1] = (px + a*y)/T/T;
  m_B[2][0] = 0-charge() *px/pxy/pxy/pxy;
  m_B[2][1] = 0-charge() *py/pxy/pxy/pxy;
  m_B[3][0] = (pz/a)*(py/pxy/pxy-(py-a*x)/T/T);
  m_B[3][1] = 0-(pz/a)*(px/pxy/pxy-(px+a*y)/T/T);
  m_B[3][2] = 0 - asin(J)/a;
  m_B[4][0] = 0 - (px/pxy)*(pz/pxy)/pxy;  
  m_B[4][1] = 0 - (py/pxy)*(pz/pxy)/pxy;  
  m_B[4][2] = 1/pxy;
  
  return m_B;
}

//
//  HTrackParameter --> WTrackParameter
//

WTrackParameter HTrackParameter::wTrack(const double mass) const {

  WTrackParameter wtrk;
  wtrk.setCharge(charge());
  HepVector w(7,0);
  HepMatrix dWdh(7, 5, 0);
  
  double ptrk = p3().rho(); double E = sqrt(ptrk*ptrk + mass * mass);
  double px = p3().x();
  double py = p3().y();
  double pz = p3().z();
  double x0 = x3().x();
  double y0 = x3().y();
  double z0 = x3().z();
  w[0] = px; w[1] = py; w[2] = pz; w[3] = E;
  w[4] = x0; w[5] = y0; w[6] = z0; 
  wtrk.setW(w);
  dWdh[0][1] = -py; dWdh[0][2] = 0 - px/kappa();
  dWdh[1][1] = px; dWdh[1][2] = 0 - py/kappa();
  dWdh[2][2] = 0 - pz/kappa(); dWdh[2][4] = charge()/kappa();
  dWdh[3][2] = 0 - (1 + lambda() * lambda())/ E / kappa() / kappa() / kappa();
  dWdh[3][4] = lambda() / E / kappa() / kappa();
  dWdh[4][0] = cos(phi0()); dWdh[4][1] = 0 - y0;
  dWdh[5][0] = sin(phi0()); dWdh[5][1] = x0;
  dWdh[6][3] = 1;
  HepSymMatrix Ew(7, 0);
  Ew = m_eHel.similarity(dWdh);
  wtrk.setEw(Ew);
  return wtrk;
}

WTrackParameter HTrackParameter::wTrack() const {
  WTrackParameter wtrk;
  if(m_partID > -1 && m_partID < 5) {
    double mass = xmass(m_partID);
    wtrk = wTrack(mass);
  } 
  return wtrk;
}

//
//  Utilities functions
//

double HTrackParameter::xmass(int n) const {
  double mass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
  if(n < 0 || n >=5) return 0.0;
  return mass[n];
}


//
//  Intersection position of two helice in xy plane
//
HepPoint3D HTrackParameter::positionTwoHelix(const HTrackParameter htrk) const{
  double bField1 = VertexFitBField::instance()->getBFieldZ(x3());
  double bField2 = VertexFitBField::instance()->getBFieldZ(htrk.x3());

  HepPoint3D pos1 = x3();
  HepPoint3D pos2 = htrk.x3();
  double rad1 = radius();
  double rad2 = htrk.radius();
  HepPoint3D xc1 = center();
  HepPoint3D xc2 = htrk.center();
  //
  // requires the solution of
  // a * y**2 + b*y + c = 0 
  // and
  // x = (rt - 2*yt * y) / (2 * xt) 
  // where
  // xt = xc2.x() - xc1.x()
  // yt = xc2.y() - xc1.y()
  // rt = rad1*rad1-rad2*rad2-xc1.perp2()+xc2.perp2()
  // a = 1 + yt*yt/(xt*xt);
  // b = 2*xc1.x()*yt/xt-yt*rt/(xt*xt)-2*xc1.y();
  // c = rt*rt/(4*xt*xt)-xc1.x()*rt/xt-rad1*rad1+xc1.perp2();
  //
  
  double xt = xc2.x() - xc1.x();
  double yt = xc2.y() - xc1.y();
  if(xt == 0 && yt == 0)  return HepPoint3D(999,999,999);
  double rt = rad1*rad1-rad2*rad2-xc1.perp2()+xc2.perp2();
  double x1, y1, x2, y2;
  if( xt != 0) {
    double a = 1 + yt*yt/(xt*xt);
    if(a == 0)   return HepPoint3D(999,999,999);
    double b = 2*xc1.x()*yt/xt-yt*rt/(xt*xt)-2*xc1.y();
    double c = rt*rt/(4*xt*xt)-xc1.x()*rt/xt-rad1*rad1+xc1.perp2();
    double d = b*b - 4 * a * c;
    if( d < 0)   return HepPoint3D(999,999,999);
    d = sqrt(d);
    // two solution of intersection points

    y1 = (-b + d)/(2*a);
    x1 = (rt - 2 * yt * y1)/(2*xt);
  
    y2 = (-b - d)/(2*a);
    x2 = (rt - 2 * yt * y2)/(2*xt);
  } else {
    double a = 1 + xt*xt/(yt*yt);
    if(a == 0)   return HepPoint3D(999,999,999);
    double b = 2*xc1.y()*xt/yt-xt*rt/(yt*yt)-2*xc1.x();
    double c = rt*rt/(4*yt*yt)-xc1.y()*rt/yt-rad1*rad1+xc1.perp2();
    double d = b*b - 4 * a * c;
    if( d < 0)   return HepPoint3D(999,999,999);
    d = sqrt(d);
    // two solution of intersection points

    x1 = (-b + d)/(2*a);
    y1 = (rt - 2 * xt * x1)/(2*yt);
  
    x2 = (-b - d)/(2*a);
    y2 = (rt - 2 * xt * x2)/(2*yt);
  }

  double z1[2], z2[2], J1[2], J2[2];

  
  double a1 = alpha * bField1 * charge();
  double a2 = alpha * bField2 * htrk.charge();
  // z for solotion one
  J1[0] = a1*((x1-x3().x())*p3().x()+(y1-x3().y())*p3().y())/p3().perp2();
  J1[1] = a2*((x1-htrk.x3().x())*htrk.p3().x()+(y1-htrk.x3().y())*htrk.p3().y())/htrk.p3().perp2();
  z1[0] = x3().z()+p3().z()/a1*asin(J1[0]);
  z1[1] = htrk.x3().z()+htrk.p3().z()/a2*asin(J1[1]);
  // z for solotion two
  J2[0] = a1*((x2-x3().x())*p3().x()+(y2-x3().y())*p3().y())/p3().perp2();
  J2[1] = a2*((x2-htrk.x3().x())*htrk.p3().x()+(y2-htrk.x3().y())*htrk.p3().y())/htrk.p3().perp2();
  z2[0] = x3().z()+p3().z()/a2*asin(J2[0]);
  z2[1] = htrk.x3().z()+htrk.p3().z()/a2*asin(J2[1]);

  // take the solution if delta z is small

  if(fabs(z1[0]-z1[1]) < fabs(z2[0]-z2[1])) {
    return HepPoint3D(x1, y1, 0.5*(z1[0]+z1[1]));
  } else {
    return HepPoint3D(x2, y2, 0.5*(z2[0]+z2[1]));
  }
}



//
// intersection position of helix and Plane
//

HepPoint3D HTrackParameter::positionPlane(const double zp) const {
  return HepPoint3D(999,999,999);
}

//
// intersection position of helix and a cylinder
//

HepPoint3D HTrackParameter::positionCylinder(const double R) const {
  return HepPoint3D(999,999,999);
}

//
// intersection position of helix and a cone
//

HepPoint3D HTrackParameter::positionCone() const {
  return HepPoint3D(999,999,999);
}

//
//  Minimum distance of two helice
//

double HTrackParameter::minDistanceTwoHelix(const HTrackParameter G, HepPoint3D &mpos) {
  int ifail;
  double h = radius();
  double g = G.radius();
  double phiH0 = fmod((4*CLHEP::pi)+atan2(p3().y(), p3().x()), (2*CLHEP::pi));
  double phiG0 = fmod((4*CLHEP::pi)+atan2(G.p3().y(), G.p3().x()), (2*CLHEP::pi));
  double lamH = lambda();
  double lamG = G.lambda();
  double a = x3().x() - G.x3().x() + g*sin(phiG0) - h*sin(phiH0);
  double b = x3().y() - G.x3().y() - g*cos(phiG0) + h*cos(phiH0);
  double c1 = h*lamH*lamH;
  double c2 = 0 - g*lamG*lamG;
  double d1 = 0 - g*lamG*lamH;
  double d2 = h*lamG*lamH;
  double e1 = lamH*(x3().z() - G.x3().z() - h*phiH0*lamH + g*phiG0*lamG);
  double e2 = lamG*(x3().z() - G.x3().z() - h*phiH0*lamH + g*phiG0*lamG);

  HepVector phiE(2, 0);
  phiE[0] = phiH0;
  phiE[1] = phiG0;
  for(int iter = 0; iter < 100; iter++) {
    HepVector z(2, 0);
    HepSymMatrix Omega(2, 0);
    double phiH = phiE[0];
    double phiG = phiE[1];
    z[0] = cos(phiH)*(a-g*sin(phiG))+sin(phiH)*(b+g*cos(phiG))+c1*phiH+d1*phiG+e1;
    z[1] = cos(phiG)*(a+h*sin(phiH))+sin(phiG)*(b-h*cos(phiH))+c2*phiG+d2*phiH+e2;
    Omega[0][0] = 0-sin(phiH)*(a-g*sin(phiG))+cos(phiH)*(b+g*cos(phiG))+c1;
    Omega[0][1] = -g*cos(phiH)*cos(phiG)-g*sin(phiH)*sin(phiG)+d1;
    Omega[1][0] =h*cos(phiH)*cos(phiG)+h*sin(phiG)*sin(phiH)+d2;
    Omega[1][1] = -sin(phiG)*(a+h*sin(phiH))+cos(phiG)*(b-h*cos(phiH))+c2;
    HepVector phi(2, 0);
    phi = phiE - Omega.inverse(ifail) * z;
    if((fabs(phi[0]-phiE[0])<1E-3) && (fabs(phi[1]-phiE[1])<1E-3)) {
      phiE = phi;
      //      std::cout << "number of iteration = " << iter <<std::endl;
      break;
    }
    phiE = phi;
  }

  double phiH = phiE[0];
  double phiG = phiE[1];
  HepPoint3D posH = HepPoint3D(x3().x()+h*(sin(phiH)-sin(phiH0)), x3().y()+h*(cos(phiH0)-cos(phiH)), x3().z()+lamH*(phiH-phiH0));
  HepPoint3D posG = HepPoint3D(G.x3().x()+g*(sin(phiG)-sin(phiG0)), G.x3().y()+g*(cos(phiG0)-cos(phiG)), G.x3().z()+lamG*(phiG-phiG0));
  double dis = (posH-posG).rho();
  mpos = 0.5*(posH+posG);
  return dis;
}
//
//  Minimum distance of helix and a line
//
