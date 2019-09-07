#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <math.h>
#include "MdcAlignAlg/Alignment.h" 
#include "MdcAlignAlg/MdcAlignPar.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

using namespace std;
using namespace CLHEP;

bool Alignment::gFlagMag;
int Alignment::gNiter;

int Alignment::getEpId(int lay, int iEnd){
     int i;
     if(lay < 8) i = 0;
     else if(lay < 10) i = 1; 
     else if(lay < 12) i = 2;
     else if(lay < 14) i = 3;
     else if(lay < 16) i = 4;
     else if(lay < 18) i = 5;
     else if(lay < 20) i = 6;
     else i = 7;

     int iEP;
     if(1 == iEnd) iEP = i;	// east
     else return iEP = i + 8;	// west
     return iEP;
}

void Alignment::expd(double veca[3], double vecb[3], double val[3]){
     val[0] = veca[1]*vecb[2] - veca[2]*vecb[1];
     val[1] = veca[2]*vecb[0] - veca[0]*vecb[2];
     val[2] = veca[0]*vecb[1] - veca[1]*vecb[0];
}

int Alignment::dist2Line(double sta[3], double stb[3],
			 double veca[3], double vecb[3],
			 double &d, double &za, double &zb, int fgZcal){
     int i;
     double vst[3];   // P0 - W0
     double vp[3];    // (P * W) / |P * W|
     double modvp;
     double m2;
     double seca[3], secb[3];
     double tpa = 0.0;
     double twb = 0.0;

     for(i=0; i<3; i++) vst[i] = sta[i] - stb[i];  // vector P0-W0

     Alignment::expd(veca, vecb, vp); // exterior product

     m2 = vp[0]*vp[0] + vp[1]*vp[1] + vp[2]*vp[2];
     modvp = sqrt(m2);
     for(i=0; i<3; i++) vp[i] /= modvp;  // (P * W) / |P * W|

     d = 0.0;
     for(i=0; i<3; i++)  d += vst[i] * vp[i];

     if( 0 == fgZcal ) return 1;

     double veca00 = veca[0]*veca[0];
     double veca11 = veca[1]*veca[1];
     double veca22 = veca[2]*veca[2];

     double veca01 = veca[0]*veca[1];
     double veca02 = veca[0]*veca[2];
     double veca12 = veca[1]*veca[2];

     double vecb00 = vecb[0]*vecb[0];
     double vecb11 = vecb[1]*vecb[1];
     double vecb22 = vecb[2]*vecb[2];
     double vecb01 = vecb[0]*vecb[1];
     double vecb02 = vecb[0]*vecb[2];
     double vecb12 = vecb[1]*vecb[2];

     seca[0] = veca[1]*vecb01 + veca[2]*vecb02 - veca[0]*(vecb11 + vecb22);
     seca[1] = veca[0]*vecb01 + veca[2]*vecb12 - veca[1]*(vecb00 + vecb22);
     seca[2] = veca[0]*vecb02 + veca[1]*vecb12 - veca[2]*(vecb00 + vecb11);

     secb[0] = vecb[1]*veca01 + vecb[2]*veca02 - vecb[0]*(veca11 + veca22);
     secb[1] = vecb[0]*veca01 + vecb[2]*veca12 - vecb[1]*(veca00 + veca22);
     secb[2] = vecb[0]*veca02 + vecb[1]*veca12 - vecb[2]*(veca00 + veca11);

     for(i=0; i<3; i++){
	  tpa += seca[i] * (sta[i] - stb[i]);
	  twb += secb[i] * (stb[i] - sta[i]);
     }
     tpa /= m2;
     twb /= m2;
     za = veca[2] * tpa + sta[2];
     zb = vecb[2] * twb + stb[2];

     return 1;
}

double Alignment::docaLineWire(double trkpar[], double wirest[],
			       double wirev[], double &zwire, int fgZcal){

     double d;
     double ztrk;
     double ps[3];
     double pv[3];

     ps[0] = trkpar[0] * cos(trkpar[1]);
     ps[1] = trkpar[0] * sin(trkpar[1]);
     ps[2] = trkpar[3];

     pv[0] = cos(trkpar[1] + HFPI);
     pv[1] = sin(trkpar[1] + HFPI);
     pv[2] = trkpar[4];

     Alignment::dist2Line(ps, wirest, pv, wirev, d, ztrk, zwire, fgZcal);

     return d;
}

double Alignment::docaHelixWireNewton(double trkpar[], double wirest[],
				      double wirev[], double &zwire, double zini){
     int ifail;
     double x0 = trkpar[0] * cos(trkpar[1]);
     double y0 = trkpar[0] * sin(trkpar[1]);
     double z0 = trkpar[3];
     double phi0 = trkpar[1] + HFPI;
     double g = 1000 / (0.3 * BFIELD * trkpar[2]); // alpha/kappa
     double tanl = trkpar[4];

     double wx = wirev[0];
     double wy = wirev[1];
     double wz = wirev[2];

     double phi;
     double t;

     CLHEP::HepVector c(2, 0);
     c[0] = phi0 + (zini - z0) / (g * tanl);
     c[1] = (zini - wirest[2]) / wz;

     phi = c[0];
     t = c[1];
     double xh = x0 + g * (sin(phi) - sin(phi0));
     double yh = y0 + g * (-cos(phi) + cos(phi0));
     double zh = z0 + g * tanl * (phi - phi0);
     double xw = wirest[0] + wx*t;
     double yw = wirest[1] + wy*t;
     double zw = wirest[2] + wz*t;
     double doca = sqrt( (xh-xw)*(xh-xw) + (yh-yw)*(yh-yw) + (zh-zw)*(zh-zw) );

     int iter = 0;
     //      cout << "iter " << setw(5) << "ini" << setw(15) << c[0] << setw(15) << c[1]
     // 	  << setw(15) << doca << endl;	// for debug
     for(iter=0; iter<1000; iter++){
	  CLHEP::HepVector a(2, 0);
	  CLHEP::HepSymMatrix omega(2, 0);
	  phi = c[0];
	  t = c[1];

	  a[0] = (x0 - g*sin(phi0) - wirest[0] - wx*t) * cos(phi)
	       + (y0 + g*cos(phi0) - wirest[1] - wy*t) * sin(phi)
	       + (z0 + g*tanl*phi - g*tanl*phi0 - wirest[2] - wz*t) * tanl;
	  a[1] = (x0 + g*sin(phi) - g*sin(phi0) - wirest[0] - wx*t) * wx
	       + (y0 - g*cos(phi) + g*cos(phi0) - wirest[1] - wy*t) * wy
	       + (z0 + g*tanl*phi - g*tanl*phi0 - wirest[2] - wz*t) * wz;
	  omega[0][0] = 0 - (x0 - g*sin(phi0) - wirest[0] - wx*t) * sin(phi)
	       + (y0 + g*cos(phi0) - wirest[1] - wy*t) * cos(phi) + g*tanl*tanl;
	  omega[0][1] = -wx*cos(phi) - wy*sin(phi) - wz*tanl;
	  omega[1][0] = g * (wx*cos(phi) + wy*sin(phi) + wz*tanl);
	  omega[1][1] = -wx*wx - wy*wy - wz*wz;

	  HepVector cc(2, 0);
	  cc = c - omega.inverse(ifail) * a;

	  phi = c[0];
	  t = c[1];
	  xh = x0 + g * (sin(phi) - sin(phi0));
	  yh = y0 + g * (-cos(phi) + cos(phi0));
	  zh = z0 + g * tanl * (phi - phi0);
	  xw = wirest[0] + wx*t;
	  yw = wirest[1] + wy*t;
	  zw = wirest[2] + wz*t;
	  doca = sqrt( (xh-xw)*(xh-xw) + (yh-yw)*(yh-yw) + (zh-zw)*(zh-zw) );
	  // 	  cout << "iter " << setw(5) << iter << setw(15) << cc[0] << setw(15) << cc[1]
	  // 	       << setw(15) << a[0] << setw(15) << a[1]
	  // 	       << setw(15) << doca << endl;	// for debug


	  c = cc;
	  phi = c[0];
	  t = c[1];
	  a[0] = (x0 - g*sin(phi0) - wirest[0] - wx*t) * cos(phi)
	       + (y0 + g*cos(phi0) - wirest[1] - wy*t) * sin(phi)
	       + (z0 + g*tanl*phi - g*tanl*phi0 - wirest[2] - wz*t) * tanl;
	  a[1] = (x0 + g*sin(phi) - g*sin(phi0) - wirest[0] - wx*t) * wx
	       + (y0 - g*cos(phi) + g*cos(phi0) - wirest[1] - wy*t) * wy
	       + (z0 + g*tanl*phi - g*tanl*phi0 - wirest[2] - wz*t) * wz;
	  if((fabs(a[0]) < 0.001) && (fabs(a[1]) < 0.001)) break;

	  // 	  if((fabs(c[0]-cc[0]) < 0.0001) && (fabs(c[1]-cc[1]) < 0.01)){
	  // 	       c = cc;
	  // 	       break;
	  // 	  }

     }
     Alignment::gNiter = iter;

     phi = c[0];
     t = c[1];
     xh = x0 + g * (sin(phi) - sin(phi0));
     yh = y0 + g * (-cos(phi) + cos(phi0));
     zh = z0 + g * tanl * (phi - phi0);
     xw = wirest[0] + wx*t;
     yw = wirest[1] + wy*t;
     zw = wirest[2] + wz*t;
     doca = sqrt( (xh-xw)*(xh-xw) + (yh-yw)*(yh-yw) + (zh-zw)*(zh-zw) );
     zwire = zw;
     cout << endl;

     //      phi = c[0];
     //      t = c[1];
     //      double xh = x0 + g * (sin(phi) - sin(phi0));
     //      double yh = y0 + g * (-cos(phi) + cos(phi0));
     //      double zh = z0 + g * tanl * (phi - phi0);
     //      double xw = wirest[0] + wx*t;
     //      double yw = wirest[1] + wy*t;
     //      double zw = wirest[2] + wz*t;
     //      double doca = sqrt( (xh-xw)*(xh-xw) + (yh-yw)*(yh-yw) + (zh-zw)*(zh-zw) );
     //      zwire = zw;

     //      cout << setw(15) << xh << setw(15) << yh << setw(15) << zh
     // 	  << setw(15) << xw << setw(15) << yw << setw(15) << zw << setw(15) << doca << endl;

     //      double xc = (trkpar[0] - g) * cos(trkpar[1]);
     //      double yc = (trkpar[0] - g) * sin(trkpar[1]);
     //      double dcw = sqrt((xc-xw)*(xc-xw) + (yc-yw)*(yc-yw));
     //      double dch = sqrt((xc-xh)*(xc-xh) + (yc-yh)*(yc-yh));
     //      cout << setw(15) << xc << setw(15) << yc << setw(20) << dch
     // 	  << setw(15) << dcw << setw(15) << g
     // 	  << setw(17) << (dch - fabs(g)) << setw(15) << (dcw - fabs(g)) << endl << endl;

     return doca;
}

double Alignment::docaHelixWire(double trkpar[], double wirest[],
				double wirev[], double &zwire, double phiIni){
     double x0 = trkpar[0] * cos(trkpar[1]);
     double y0 = trkpar[0] * sin(trkpar[1]);
     double z0 = trkpar[3];
     double phi0 = trkpar[1] + HFPI;
     if(phi0 > PI2) phi0 -= PI2;
     double g = 1000 / (0.3 * BFIELD * trkpar[2]); // alpha/kappa
     double tanl = trkpar[4];

     double trkst[3];
     double trkv[3];
     //double phi = phi0 + (zini - z0) / (g * tanl);
     double phi = phiIni;
     double dphi;

     double doca;
     double ztrk;
     double phiNew;
     int iter = 0;
     //for(iter=0; iter<10; iter++){
     //    trkst[0] = x0 + g * (sin(phi) - sin(phi0));
     //    trkst[1] = y0 + g * (-cos(phi) + cos(phi0));
     //    trkst[2] = z0 + g * tanl * (phi - phi0);

     //    trkv[0] = cos(phi);
     //    trkv[1] = sin(phi);
     //    trkv[2] = tanl;

     //    Alignment::dist2Line(trkst, wirest, trkv, wirev, doca, ztrk, zwire);

     //    phiNew = phi0 + (ztrk - z0) / (g*tanl);
     //    if(fabs(phiNew - phi) < 1.0E-8) break;
     //    phi = phiNew;
     //}
     for(iter=0; iter<10; iter++){
	  dphi = phi - phi0;
	  if(dphi > PI) dphi -= PI2;
	  if(dphi < -PI) dphi += PI2;

	  trkst[0] = x0 + g * (sin(phi) - sin(phi0));
	  trkst[1] = y0 + g * (-cos(phi) + cos(phi0));
	  // 	  trkst[2] = z0 + g * tanl * (phi - phi0);
	  trkst[2] = z0 + g * tanl * dphi;

	  trkv[0] = cos(phi);
	  trkv[1] = sin(phi);
	  trkv[2] = tanl;

	  dist2Line(trkst, wirest, trkv, wirev, doca, ztrk, zwire);

	  phiNew = phi0 + (ztrk - z0) / (g*tanl);
	  if(fabs(phiNew - phi) < 1.0E-8) break;
	  phi = phiNew;
     }

     gNiter = iter;

     return doca;
}

// double Alignment::docaHelixWire(double trkpar[], double wirest[],
// 				double wirev[], double &zwire, double zini){
//      double x0 = trkpar[0] * cos(trkpar[1]);
//      double y0 = trkpar[0] * sin(trkpar[1]);
//      double z0 = trkpar[3];
//      double phi0 = trkpar[1] + HFPI;
//      double g = 1000 / (0.3 * BFIELD * trkpar[2]); // alpha/kappa
//      double tanl = trkpar[4];

//      double wx = wirev[0];
//      double wy = wirev[1];
//      double wz = wirev[2];

// //      double phi;
// //      double t;

// //      CLHEP::HepVector c(2, 0);
// //      c[0] = phi0 + (zini - z0) / (g * tanl);
// //      c[1] = (zini - wirest[2]) / wz;

// //      phi = c[0];
// //      t = c[1];
// //      double xh = x0 + g * (sin(phi) - sin(phi0));
// //      double yh = y0 + g * (-cos(phi) + cos(phi0));
// //      double zh = z0 + g * tanl * (phi - phi0);
// //      double xw = wirest[0] + wx*t;
// //      double yw = wirest[1] + wy*t;
// //      double zw = wirest[2] + wz*t;
// //      double doca = sqrt( (xh-xw)*(xh-xw) + (yh-yw)*(yh-yw) + (zh-zw)*(zh-zw) );

//      double docaIter[10000];
//      double phiIni = phi0 + (zini - z0) / (g * tanl) - 0.0000001*5000.0;
//      double phi;
//      double t = (zini - wirest[2]) / wz;

//      int iter;
//      for(iter=0; iter<10000; iter++){
// 	  phi = 0.0000001 * (double)iter + phiIni;
// 	  double xh = x0 + g * (sin(phi) - sin(phi0));
// 	  double yh = y0 + g * (-cos(phi) + cos(phi0));
// 	  double zh = z0 + g * tanl * (phi - phi0);
// 	  double xw = wirest[0] + wx*t;
// 	  double yw = wirest[1] + wy*t;
// 	  double zw = wirest[2] + wz*t;
// 	  docaIter[iter] = sqrt( (xh-xw)*(xh-xw) + (yh-yw)*(yh-yw) + (zh-zw)*(zh-zw) );

// // 	  cout << "iter " << setw(5) << iter << setw(15) << phi << setw(15) << doca << endl;
//      }
//      gNiter = iter;

//      double doca = docaIter[0];
//      for(iter=0; iter<10000; iter++){
// 	  if(fabs(docaIter[iter]) < fabs(doca)) doca = docaIter[iter];
//      }

//      return doca;
// }

bool Alignment::getDoca(double trkpar[], double wpos[], double &doca,
			double whitPos[], double zini){
     int i = 0;
     double zp;      // z of the point above in the plane of the wire
     double xyz[3];  // coordinate of the point on wire according to zc
     double dxyz[3]; // orientation of the tangent line at the point above

     double ten = wpos[6];
     double a = 9.47e-06 / (2 * ten); // a = density(g/mm)/2T(g)
     double dx = wpos[0] - wpos[3]; // the differential of xyz between the end planes
     double dz = wpos[2] - wpos[5]; // 
     double length = sqrt(dx*dx + dz*dz);

     double ztan = 0.0;  // z of the doca point in the tangent line
     if(whitPos[2] < 0.5*length)  ztan = whitPos[2];

     double zc=0.0;  // z of the calculated point of the wire
     if( Alignment::gFlagMag ) zc = zini;

     // alf is the angle between z and the projection of the wire on xz
     double sinalf = dx / sqrt(dx*dx + dz*dz);
     double cosalf = dz / sqrt(dx*dx + dz*dz);
     double tanalf = dx / dz;

     double posIni[3];
     double rLayer = sqrt((wpos[3] * wpos[3]) + (wpos[4] * wpos[4]));
     double phiIni = getPhiIni(trkpar, rLayer, posIni);

     if(dz < 50){
	  std::cout << "ERROR: wire position error in getdocaLine() !!!"
		    << std::endl;
	  return false;
     }

     while( 1 ){
	  i++;
	  if(i > 5){
	       return false;
	  }
	  zp = zc / cosalf;

	  xyz[0] = (zc - wpos[5]) * tanalf + wpos[3];
	  xyz[1] = a*zp*zp + (wpos[1] - wpos[4])*zp/length
	       + 0.5*(wpos[1] + wpos[4]) - a*length*length/4.0;
	  xyz[2] = zc;

	  dxyz[0] = sinalf;
	  dxyz[1] = 2.0 * a * zp + (wpos[1] - wpos[4]) / length;
	  dxyz[2] = cosalf;

	  if( Alignment::gFlagMag ) doca = docaHelixWire(trkpar, xyz, dxyz, ztan, phiIni);
	  else doca = docaLineWire(trkpar, xyz, dxyz, ztan);

	  if( fabs(zc-ztan) < 0.5 )  break;
	  else if( fabs(ztan) > (0.5*length) ){
	       doca = 99999.0;
	       break;
	  }
	  zc = ztan;
     }
     whitPos[2] = ztan;
     zp = ztan / cosalf;
     whitPos[1] = a*zp*zp + (wpos[1] - wpos[4])*zp/length
	  + 0.5*(wpos[1] + wpos[4]) - a*length*length/4.0;
     whitPos[0] = (ztan - wpos[5]) * tanalf + wpos[3];

     return true;
}
double Alignment::getPhiIni(double trkpar[], double rLayer, double pos[]){
     double dr = trkpar[0];
     double fi0 = trkpar[1];
     double kap = trkpar[2];
     double rw = rLayer;

     double phi0 = fi0 + HFPI;
     if(phi0 > PI2) phi0 -= PI2;
     double g = 1000 / (0.3 * 1.0 * kap); // alpha/kappa

     double aa = rw*rw - (dr-g)*(dr-g) - g*g;
     double bb = 2*g*(dr - g);
     double cc = aa/bb;
     double dd = acos(cc);	// dd (0, PI)

     double phi;
     if(kap > 0) phi = phi0 + dd;
     else phi = phi0 - dd;

     if(phi > PI2) phi -= PI2;
     if(phi < 0) phi += PI2;

     double x0 = dr * cos(fi0);
     double y0 = dr * sin(fi0);
     pos[0] = x0 + g * (sin(phi) - sin(phi0));
     pos[1] = y0 + g * (-cos(phi) + cos(phi0));
     //      pos[2] = trkpar[3] + g * trkpar[4] * (phi - phi0);
     if(kap > 0) pos[2] = trkpar[3] + g * trkpar[4] * dd;
     else pos[2] = trkpar[3] - g * trkpar[4] * dd;

     return phi;
}


