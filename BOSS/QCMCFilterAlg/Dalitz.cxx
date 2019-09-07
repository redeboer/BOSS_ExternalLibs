// -*- C++ -*-

//
// Author:      Dan Ambrose
// Created:     Mon Mar 5 2012
// based on Cleo package by Eric White and Warner Sun

//
// Revision history
//
//

#include "QCMCFilterAlg/Dalitz.h"
//#include "EvtGenBase/EvtGenKine.hh"
#include <complex>

using std::string;

double Dalitz::PI = 3.1415926; // pi

Dalitz::Dalitz() {
   N = 8;  // Default bins if not specified
}

// Constructor
Dalitz::Dalitz(int binNum) {
   N = binNum; // Set bin number
} // end constructor


TComplex Dalitz::Amplitude(double x, double y, double z) {

   //PRD 73, 112009(2006) Belle
   //for D0  particle 1: K  particle 2: pi-  particle 3: pi+
   //the right order is: (1,2), (1,3), (3,2) 
   double m_mass[4] = {1.86450, 0.497648, 0.139570, 0.139570}; //mass

   TComplex D0(0.0,0.0);

   //x, y, z already squared, need to get back the mass!!!!
   x = sqrt(x);
   y = sqrt(y);
   z = sqrt(z);

   // Array for resonances
   TComplex DK2piRes[19];

   //x->3   y->2   z->1
   DK2piRes[0] = sakurai(x, y, z, 1.00,  0.0,   0.1503, 0.7758);//RHO(770)
   DK2piRes[1] = resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 0.0314,110.8, 0.00849,0.78259,1);//OMEGA
   DK2piRes[2] = f_980(z, 0.980, 0.365, 201.9);//F_0(980)
   DK2piRes[3] = resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 1.32,  348,   0.1851, 1.2754, 2);//F_2(1270)
   DK2piRes[4] = resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 1.44,  82,  0.173,  1.434,  0);//F_0(1370) hep-ph 0009168
   DK2piRes[5] = sakurai(x, y, z, 0.66, 9, 0.400,  1.465);//RHO(1450)
   DK2piRes[6] = resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 1.43,  212,   0.454,  0.519,  0);//Sigma(600)
   DK2piRes[7] = resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 0.23,  237,   0.101,  1.050,  0);//Sigma
   DK2piRes[8] = resAmp(x, z, y, m_mass[3], m_mass[1], m_mass[2], 1.644, 132.1, 0.0508, 0.89166,1);//K*(892)-
   DK2piRes[9] = resAmp(x, z, y, m_mass[3], m_mass[1], m_mass[2], 0.61,  113, 0.232,  1.414,  1);//K*(1410)-
   DK2piRes[10] = resAmp(x, z, y, m_mass[3], m_mass[1], m_mass[2], 2.15,  353.6, 0.294,  1.412,  0);//K*_0(1430)-
   DK2piRes[11] = resAmp(x, z, y, m_mass[3], m_mass[1], m_mass[2], 0.88,  318.7, 0.0985, 1.4256, 2);//K*_2(1430)-
   DK2piRes[12] = resAmp(x, z, y, m_mass[3], m_mass[1], m_mass[2], 1.39,  103,   0.322,  1.717,  1);//K*(1680)-
   DK2piRes[13] = resAmp(y, z, x, m_mass[2], m_mass[1], m_mass[3], 0.144, 320.3, 0.0508, 0.89166,1);//K*(892)+
   DK2piRes[14] = resAmp(y, z, x, m_mass[2], m_mass[1], m_mass[3], 0.45,  254,   0.232,  1.414,  1);//K*(1410)+
   DK2piRes[15] = resAmp(y, z, x, m_mass[2], m_mass[1], m_mass[3], 0.47,  88,    0.294,  1.412,  0);//K*_0(1430)+
   DK2piRes[16] = resAmp(y, z, x, m_mass[2], m_mass[1], m_mass[3], 0.25,  265, 0.0985, 1.4256, 2);//K*_2(1430)+
   DK2piRes[17] = resAmp(y, z, x, m_mass[2], m_mass[1], m_mass[3], 1.2,   118,   0.322,  1.717,  1);//K*(1680)+

   double pi180inv = 3.1415926/180.;
   DK2piRes[18] = TComplex(3.0*cos(164*pi180inv),3.0*sin(164*pi180inv));

   for(int i=0; i<19; i++){
      D0 += DK2piRes[i];
   }

   return D0;
} // Amplitude

TComplex Dalitz::CLEO_Amplitude(double x, double y, double z) {

   double m_mass[4] = {1.86450, 0.497648, 0.139570, 0.139570}; //mass

   TComplex D0(0.0,0.0);

   //x, y, z already squared, need to get back the mass!!!!
   x = sqrt(x);
   y = sqrt(y);
   z = sqrt(z);

   // Array for resonances
   TComplex DK2piRes[3];

   //x->3   y->2   z->1
   DK2piRes[0] = CLEO_resAmp(x, z, y, m_mass[3], m_mass[1], m_mass[2], 2.31, 109.0, 0.0498, 0.89610, 1);//K*(892)
   DK2piRes[1] = CLEO_resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 1.59,-123.0,0.1491,0.7683,1);//RHO(770)
   DK2piRes[2] = TComplex(1.0, 0.0) ;

   for(int i=0; i<3; i++){
      D0 += DK2piRes[i];
   }

   return D0;
}

double Dalitz::Phase(double x, double y, double z, int Babar) {

   TComplex D0(0,0);
   TComplex D0bar(0,0);

   if (Babar == 1) {
      D0 = Babar_Amplitude(x, y, z);
      D0bar = Babar_Amplitude(y, x, z);
   } else{
      D0 = Amplitude(x, y, z);
      D0bar = Amplitude(y, x, z);
   }

   //Compute phase difference
   double deltaD = arg(D0) - arg(D0bar);
   if(x<y)  deltaD = -deltaD;//make it symmetric to the lower half

   if ( deltaD < -PI/N )  deltaD += 2*PI; // shift deltaD to [-PI/8,15*PI/8]
   if ( deltaD > (2*N-1)*PI/N )  deltaD -= 2*PI; // shift deltaD to [-PI/8,15*PI/8]

   return deltaD;

} // end Phase

bool Dalitz::Point_on_DP(double x, double y) {

   double m_mass[4] = {1.86450, 0.497648, 0.139570, 0.139570}; //mass
   double m_mass2[4]= {1.86450*1.86450, 0.497648*0.497648,
                       0.139570*0.139570, 0.139570*0.139570}; //mass square

   double m_XmaxDP = m_mass[0] - m_mass[3]; m_XmaxDP *= m_XmaxDP;
   double m_XminDP = m_mass[1] + m_mass[2]; m_XminDP *= m_XminDP;

   if ( (x > m_XmaxDP) || (x < m_XminDP) ) return false;

   double Low = 0;
   double Up = 0;
   double HInv_m12 = 0.5/sqrt(x);
   double E1 = HInv_m12*(x + m_mass2[1] - m_mass2[2]);
   double E3 = HInv_m12*(m_mass2[0] - m_mass2[3] - x);
   double E1_2 = E1*E1;
   double E3_2 = E3*E3;

   if (E1 < m_mass[1]) { E1=m_mass[1]; E1_2=m_mass2[1]; }
   if (E3 < m_mass[3]) { E3=m_mass[3]; E3_2=m_mass2[3]; }

   double temp = E1_2-m_mass2[1];
   if (temp < 0) temp = 0;
   double P1 = sqrt(temp);
   temp = E3_2 - m_mass2[3];
   if (temp<0) temp = 0;
   double P3 = sqrt(temp);
   double E13_2 = (E1+E3)*(E1+E3);

   // Compute hi and lo y-coord
   Low = E13_2 - (P1+P3)*(P1+P3);
   Up = E13_2 - (P1-P3)*(P1-P3);

   if ( (y > Up) || (y < Low) )  return false;

   return true;
} // end Point_on_DP

// Alternate, for points outside DP
bool Dalitz::Point_on_DP2(double x,double y) {

   double m_mass[4] = {1.86450, 0.497648, 0.139570, 0.139570}; //mass
   double m_mass2[4] = {1.86450*1.86450, 0.497648*0.497648,
                       0.139570*0.139570, 0.139570*0.139570}; //mass square

   double m_XmaxDP = m_mass[0] - m_mass[3]; m_XmaxDP *= m_XmaxDP;
   double m_XminDP = m_mass[1] + m_mass[2]; m_XminDP *= m_XminDP;

     if ( (x > m_XmaxDP) || (x < m_XminDP) ) return false;

   double Low = 0;
   double Up = 0;
   double HInv_m12 = 0.5/sqrt(x);
   double E1 = HInv_m12*(x + m_mass2[1] - m_mass2[2]);
   double E3 = HInv_m12*(m_mass2[0] - m_mass2[3] - x);
   double E1_2 = E1*E1;
   double E3_2 = E3*E3;

   if (E1 < m_mass[1]) { E1=m_mass[1]; E1_2=m_mass2[1]; }
   if (E3 < m_mass[3]) { E3=m_mass[3]; E3_2=m_mass2[3]; }

   double temp = E1_2-m_mass2[1];
   if (temp < 0) temp = 0;
   double P1 = sqrt(temp);
   temp = E3_2-m_mass2[3];
   if (temp < 0) temp = 0;
   double P3 = sqrt(temp);
   double E13_2 = (E1+E3)*(E1+E3);

   Low = E13_2 - (P1+P3)*(P1+P3);
   Up = E13_2 - (P1-P3)*(P1-P3);

   if ( (y > (Up+0.05)) || (y < (Low-0.05)) ) return false;//make it larger

   return true;
} // end Point_on_DP2


TComplex Dalitz::CLEO_resAmp(double mAC, double mBC, double mAB,
                double mB , double mA , double mC ,
                double _ampl, double _theta, double _gamma, double _bwm, int _spin) {

  double pi180inv = 3.1415926/180.;

  TComplex ampl;

  //EvtVector4R  _p4_d3 = _p4_p-_p4_d1-_p4_d2;

  //get cos of the angle between the daughters from their 4-momenta
  //and the 4-momentum of the parent

  //in general, EvtDecayAngle(parent, part1+part2, part1) gives the angle
  //the missing particle (not listed in the arguments) makes
  //with part2 in the rest frame of both
  //listed particles (12)
 
  //angle 3 makes with 2 in rest frame of 12 (CS3)  
//   double cos_phi_0 = EvtDecayAngle(_p4_p, _p4_d1+_p4_d2, _p4_d1);
// double EvtDecayAngle(const EvtVector4R& p,const EvtVector4R& q,
// 		          const EvtVector4R& d) {

//   double pd=p*d;
//   double pq=p*q;
//   double qd=q*d;
//   double mp2=p.mass2();
//   double mq2=q.mass2();
//   double md2=d.mass2();

//   double cost=(pd*mq2-pq*qd)/sqrt((pq*pq-mq2*mp2)*(qd*qd-mq2*md2));

//   return cost;

// }

  // double mD = 1.86450 ;
  double mD = 1.86484 ;
  double eA = ( mD*mD - mBC*mBC + mA*mA ) / (2.*mD) ;
  double eAB = ( mD*mD - mC*mC + mAB*mAB ) / (2.*mD) ;

  // Take D to be at rest
  double pd = mD * eA ;
  double pq = mD * eAB ;
  double qd = mA*mA + 0.5 * ( mAB*mAB - mA*mA - mB*mB ) ;
  double mp2 = mD*mD ;
  double mq2 = mAB*mAB ;
  double md2 = mA*mA ;
  double cos_phi_0 = (pd*mq2-pq*qd)/sqrt((pq*pq-mq2*mp2)*(qd*qd-mq2*md2));

//angle 3 makes with 1 in 12 is, of course, -cos_phi_0

//   double mAB=(_p4_d1+_p4_d2).mass();
//   double mBC=(_p4_d2+p4_d3).mass();
//   double mAC=(_p4_d1+p4_d3).mass();
//   double mA=_p4_d1.mass(); 
//   double mB=_p4_d2.mass(); 
//   double mD=_p4_p.mass();
//   double mC=p4_d3.mass();

  switch (_spin) {

  case 0 : 
    ampl=(_ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*
	  sqrt(_gamma/(2.*PI))*
	  (1.0/(mAB-_bwm-TComplex(0.0,0.5*_gamma)))); 
    break;

  case 1 : 
    ampl=(_ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*
	  sqrt(_gamma/(2.*PI))*
	  (cos_phi_0/(mAB-_bwm-TComplex(0.0,0.5*_gamma))));
    break;

//      case 2: 
// 	ampl=(_ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*
// 	      sqrt(_gamma/(2.*PI))*
// 	      ((1.5*cos_phi_0*cos_phi_0-0.5)/((_p4_d1+_p4_d2).mass()-_bwm-EvtComplex(0.0, 0.5*_gamma))));
// 	break;
             
//      case 3:  
// 	ampl=(_ampl*EvtComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*
// 	      sqrt(_gamma/EvtConst::twoPi)*
// 	      ((2.5*cos_phi_0*cos_phi_0*cos_phi_0-1.5*cos_phi_0)/((_p4_d1+_p4_d2).mass()-_bwm-EvtComplex(0.0, 0.5*_gamma))));
// 	break;

  default:
    //cout << "EvtGen: wrong spin in CLEO_resAmp()" << endl;
    ampl = TComplex(0.0);
    break;         

  }

  return ampl;
}


TComplex Dalitz::resAmp(double mAC, double mBC, double mAB,
                double mB , double mA , double mC ,
                double _ampl, double _theta, double _gamma, double _bwm, int _spin) {

  double pi180inv = 3.1415926/180.;

  TComplex ampl;

  double mD = 1.86450;

  double mR = _bwm;
  double gammaR = _gamma;

  double temp = (mAB*mAB-mA*mA-mB*mB)*(mAB*mAB-mA*mA-mB*mB)/4.0- mA*mA*mB*mB;
  if (temp < 0) temp = 0;
  double pAB = sqrt( temp/(mAB*mAB) );

  temp = (mR*mR-mA*mA-mB*mB)*(mR*mR-mA*mA-mB*mB)/4.0 - mA*mA*mB*mB;
  if (temp<0) temp = 0;
  double pR = sqrt( temp/(mR*mR));

  temp = (mD*mD-mR*mR-mC*mC)*(mD*mD-mR*mR-mC*mC)/4.0 - mR*mR*mC*mC;
  if (temp < 0)  temp = 0;
  double pD = sqrt( temp/(mD*mD) );

  temp = (mD*mD-mAB*mAB-mC*mC)*(mD*mD-mAB*mAB-mC*mC)/4.0 - mAB*mAB*mC*mC;
  if (temp<0)  temp = 0;
  double pDAB = sqrt( temp/(mD*mD));

  double fR = 1;
  double fD = 1;
  int power = 0;
  switch (_spin) {
     case 0:
        fR = 1.0;
        fD = 1.0;
        power = 1;
        break;
     case 1:
        fR = sqrt(1.0+1.5*1.5*pR*pR)/sqrt(1.0+1.5*1.5*pAB*pAB);
        fD = sqrt(1.0+5.0*5.0*pD*pD)/sqrt(1.0+5.0*5.0*pDAB*pDAB);
        power = 3;
        break;
     case 2:
        fR = sqrt( (9+3*pow((1.5*pR),2)+pow((1.5*pR),4))/(9+3*pow((1.5*pAB),2)+pow((1.5*pAB),4)) );
        fD = sqrt( (9+3*pow((5.0*pD),2)+pow((5.0*pD),4))/(9+3*pow((5.0*pDAB),2)+pow((5.0*pDAB),4)) );
        power = 5;
        break;
  }

  double gammaAB= gammaR*pow(pAB/pR,power)*(mR/mAB)*fR*fR;

  switch (_spin) {
     case 0:
        ampl = _ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*
           fR*fD/(mR*mR-mAB*mAB-TComplex(0.0,mR*gammaAB));
        break;
     case 1:
        ampl = _ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*
           fR*fD*(mAC*mAC-mBC*mBC+(mD*mD-mC*mC)*(mB*mB-mA*mA)/(mR*mR))/
           (mR*mR-mAB*mAB-TComplex(0.0,mR*gammaAB));
        break;
     case 2:
        ampl = _ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*
           fR*fD/(mR*mR-mAB*mAB-TComplex(0.0,mR*gammaAB))*
           (pow((mBC*mBC-mAC*mAC+(mD*mD-mC*mC)*(mA*mA-mB*mB)/(mR*mR)),2)-
            (1.0/3.0)*(mAB*mAB-2*mD*mD-2*mC*mC+pow((mD*mD- mC*mC)/mR, 2))*
            (mAB*mAB-2*mA*mA-2*mB*mB+pow((mA*mA-mB*mB)/mR,2)));
        break;
  }

  return ampl;
} // end resAmp

//PRL 86, 765 (2001)
TComplex Dalitz::f_980(double mPP, double mR,
                double _ampl, double _theta ) {

   double pi180inv = 3.1415926/180.;
   double mK = 0.493677;
   double mK0 = 0.497648;
   double mP = 0.13957;

   double m2_PP = mPP*mPP;
   double gamma = 0.09*sqrt(m2_PP/4.-mP*mP);
   if( m2_PP/4.>mK*mK )    gamma = gamma + 0.02/2.*sqrt(m2_PP/4.-mK*mK);
   if( m2_PP/4.>mK0*mK0 )  gamma = gamma + 0.02/2.*sqrt(m2_PP/4.-mK0*mK0);

   //form factor both equal 1
   TComplex ampl;
   ampl = _ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*
      1./TComplex(mR*mR-m2_PP, -mR*gamma);

   return ampl;

} // end f_980

//PRL 21, 244 (1968)
TComplex Dalitz::sakurai(double mkp, double mkm, double mpp, double _ampl, double _theta,
                 double gamma_r, double m_r) {

   double pi180inv = 3.1415926/180.;
   double m_pi = 0.139570;
   double m_k = 0.497648;
   double mD = 1.86450;
   double num, m_a, m_b, m_c, m2_ab, m2_ac, m2_bc;
   double m_ab, m_ac, m_bc, m2_a, m2_b, m2_c, m2_d;
   m_a = m_pi;
   m_b = m_pi;
   m_c = m_k;
   m_ab = mpp;
   m_ac = mkp;
   m_bc = mkm;

   m2_ab = m_ab*m_ab;
   m2_ac = m_ac*m_ac;
   m2_bc = m_bc*m_bc;
   m2_a = m_a*m_a;
   m2_b = m_b*m_b;
   m2_c = m_c*m_c;
   m2_d = mD*mD;

   //for spin 1 angular term
   num = m2_ac-m2_bc+(m2_d-m2_c)*(m2_b-m2_a)/(m_r*m_r);

   double pi, m2, m_pi2, ss, ppi2, p02, ppi, p0;
   double d, hs, hm, dhdq, f, gamma_s, dr, di;

   pi = 3.14159265358979;

   m2 = m_r*m_r;
   m_pi2 = m_pi*m_pi;
   ss = sqrt(m2_ab);

   ppi2 = (m2_ab-4.*m_pi2)/4.;
   p02 = (m2-4.*m_pi2)/4.;
   p0 = sqrt(p02);
   ppi = sqrt(ppi2);

   d = 3.*m_pi2/pi/p02*log((m_r+2.*p0)/2./m_pi) + m_r/2./pi/p0 - m_pi2*m_r/pi/(p0*p0*p0);

   hs = 2.*ppi/pi/ss*log((ss+2.*ppi)/2./m_pi);
   hm = 2.*p0/pi/m_r*log((m_r+2.*p0)/2./m_pi);

   dhdq = hm*(1./8./p02 - 1./2./m2) + 1./2./pi/m2;

   f = gamma_r*m_r*m_r/(p0*p0*p0)*(ppi2*(hs-hm) - p02*(m2_ab-m2)*dhdq);

   gamma_s = gamma_r*m2*ppi*ppi*ppi/ss/(p0*p0*p0);

   dr = m2-m2_ab+f;
   di = gamma_s;

   TComplex ampl = num/TComplex(dr, -di);
   ampl = _ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*ampl;

   return ampl;

}

TComplex Dalitz::Babar_sakurai(double mkp, double mkm, double mpp, double _ampl, double _theta,
                 double gamma_r, double m_r) {
  
   double pi180inv = 3.1415926/180.;
   double m_pi = 0.139570;
   double m_k = 0.497648;
   double mD = 1.86450;
   double num, m_a, m_b, m_c, m2_ab, m2_ac, m2_bc;
   double m_ab, m_ac, m_bc, m2_a, m2_b, m2_c, m2_d;
   m_a = m_pi;
   m_b = m_pi;
   m_c = m_k;
   m_ab = mpp;
   m_ac = mkp;
   m_bc = mkm;

   m2_ab = m_ab*m_ab;
   m2_ac = m_ac*m_ac;
   m2_bc = m_bc*m_bc;
   m2_a = m_a*m_a;
   m2_b = m_b*m_b;
   m2_c = m_c*m_c;
   m2_d = mD*mD;

   //for spin 1 angular term
   num=m2_ac-m2_bc+(m2_d-m2_c)*(m2_b-m2_a)/(m_r*m_r);

   //form factor ---------------------------------------------------
   double mAB = m_ab;
   double mA = m_a;
   double mB = m_b;
   double mC = m_c;
   double mR = m_r;
   double temp = (mAB*mAB-mA*mA-mB*mB)*(mAB*mAB-mA*mA-mB*mB)/4.0- mA*mA*mB*mB;
   if (temp < 0) temp = 0;
   double pAB = sqrt( temp/(mAB*mAB) );

   temp = (mR*mR-mA*mA-mB*mB)*(mR*mR-mA*mA-mB*mB)/4.0 - mA*mA*mB*mB;
   if (temp < 0) temp = 0;
   double pR = sqrt( temp/(mR*mR));

   temp = (mD*mD-mR*mR-mC*mC)*(mD*mD-mR*mR-mC*mC)/4.0 - mR*mR*mC*mC;
   if (temp < 0) temp = 0;
   double pD = sqrt( temp/(mD*mD) );

   temp = (mD*mD-mAB*mAB-mC*mC)*(mD*mD-mAB*mAB-mC*mC)/4.0 - mAB*mAB*mC*mC;
   if (temp < 0) temp = 0;
   double pDAB = sqrt( temp/(mD*mD));

   double fR = sqrt(1.0+1.5*1.5*pR*pR)/sqrt(1.0+1.5*1.5*pAB*pAB);
   double fD = sqrt(1.0+5.0*5.0*pD*pD)/sqrt(1.0+5.0*5.0*pDAB*pDAB);
   //-----------------------------------------------------------------

   double pi,m2,m_pi2,ss,ppi2,p02,ppi,p0;
   double d,hs,hm,dhdq,f,gamma_s,dr,di;

   pi = 3.14159265358979;

   m2 = m_r*m_r;
   m_pi2 = m_pi*m_pi;
   ss = sqrt(m2_ab);

   ppi2 = (m2_ab-4.*m_pi2)/4.;
   p02 = (m2-4.*m_pi2)/4.;
   if (p02 < 0) p02 = 0;
   if (ppi2 < 0) ppi2 = 0;
   p0 = sqrt(p02);
   ppi = sqrt(ppi2);

   d = 3.*m_pi2/pi/p02*log((m_r+2.*p0)/2./m_pi) + m_r/2./pi/p0 - m_pi2*m_r/pi/(p0*p0*p0);

   hs = 2.*ppi/pi/ss*log((ss+2.*ppi)/2./m_pi);
   hm = 2.*p0/pi/m_r*log((m_r+2.*p0)/2./m_pi);

   dhdq = hm*(1./8./p02 - 1./2./m2) + 1./2./pi/m2;

   f = gamma_r*m_r*m_r/(p0*p0*p0)*(ppi2*(hs-hm) - p02*(m2_ab-m2)*dhdq);

   gamma_s = gamma_r*m2*ppi*ppi*ppi/ss/(p0*p0*p0);

   dr = m2-m2_ab+f;
   di = gamma_s;

//----------------------------------------------------------------------------
   num *= fR*fD*(1+d*gamma_r/m_r);
//----------------------------------------------------------------------------
   TComplex ampl = num/TComplex(dr, -di);
   ampl = _ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*ampl;

   return ampl;

} // end Babar_sakurai


TComplex Dalitz::Babar_resAmp(double mAC, double mBC, double mAB,
                double mB , double mA , double mC ,
                double _ampl, double _theta, double _gamma, double _bwm, int _spin) {

  double pi180inv = 3.1415926/180.;

  TComplex ampl;

  double mD = 1.86450;

  double mR = _bwm;
  double gammaR = _gamma;

  double temp = (mAB*mAB-mA*mA-mB*mB)*(mAB*mAB-mA*mA-mB*mB)/4.0- mA*mA*mB*mB;
  if (temp < 0) temp = 0;
  double pAB = sqrt( temp/(mAB*mAB) );

  temp = (mR*mR-mA*mA-mB*mB)*(mR*mR-mA*mA-mB*mB)/4.0 - mA*mA*mB*mB;
  if (temp < 0) temp = 0;
  double pR = sqrt( temp/(mR*mR));

  temp = (mD*mD-mR*mR-mC*mC)*(mD*mD-mR*mR-mC*mC)/4.0 - mR*mR*mC*mC;
  if (temp < 0) temp = 0;
  double pD = sqrt( temp/(mD*mD) );

  temp = (mD*mD-mAB*mAB-mC*mC)*(mD*mD-mAB*mAB-mC*mC)/4.0 - mAB*mAB*mC*mC;
  if (temp < 0) temp = 0;
  double pDAB = sqrt( temp/(mD*mD));

  double fR = 1;
  double fD = 1;
  int power = 0;
  switch (_spin) {
     case 0:
        fR = 1.0;
        fD = 1.0;
        power = 1;
        break;
     case 1:
        fR = sqrt(1.0+1.5*1.5*pR*pR)/sqrt(1.0+1.5*1.5*pAB*pAB);
        fD = sqrt(1.0+5.0*5.0*pD*pD)/sqrt(1.0+5.0*5.0*pDAB*pDAB);
        power = 3;
        break;
     case 2:
        fR = sqrt( (9+3*pow((1.5*pR),2)+pow((1.5*pR),4))/(9+3*pow((1.5*pAB),2)+pow((1.5*pAB),4)) );
        fD = sqrt( (9+3*pow((5.0*pD),2)+pow((5.0*pD),4))/(9+3*pow((5.0*pDAB),2)+pow((5.0*pDAB),4)) );
        power = 5;
        break;
  }

  double gammaAB = gammaR*pow(pAB/pR,power)*(mR/mAB)*fR*fR;

  switch (_spin) {
     case 0:
        ampl = _ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*
           fR*fD/(mR*mR-mAB*mAB-TComplex(0.0,mR*gammaAB));
        break;
     case 1:
        ampl = _ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*
           fR*fD*(mAC*mAC-mBC*mBC+(mD*mD-mC*mC)*(mB*mB-mA*mA)/(mAB*mAB))/
           (mR*mR-mAB*mAB-TComplex(0.0,mR*gammaAB));
        break;
     case 2:
        ampl = _ampl*TComplex(cos(_theta*pi180inv),sin(_theta*pi180inv))*
           fR*fD/(mR*mR-mAB*mAB-TComplex(0.0,mR*gammaAB))*
           (pow((mBC*mBC-mAC*mAC+(mD*mD-mC*mC)*(mA*mA-mB*mB)/(mAB*mAB)),2)-
            (1.0/3.0)*(mAB*mAB-2*mD*mD-2*mC*mC+pow((mD*mD- mC*mC)/mAB, 2))*
            (mAB*mAB-2*mA*mA-2*mB*mB+pow((mA*mA-mB*mB)/mAB,2)));
        break;
  }

  return ampl;

}

TComplex Dalitz::Babar_Amplitude(double x, double y, double z) {

   //PRD 73, 112009(2006) Belle
   //for D0  particle 1: K  particle 2: pi-  particle 3: pi+
   //the right order is: (1,2), (1,3), (3,2) 
   double m_mass[4] = { 1.86450, 0.497648, 0.139570, 0.139570}; //mass

   TComplex D0(0.0,0.0);

   //x, y, z already squared, need to get back the mass!!!!
   x = sqrt(x);
   y = sqrt(y);
   z = sqrt(z);

   TComplex DK2piRes[17];
   DK2piRes[0] = Babar_sakurai(x, y, z, 1.00,  0.0,   0.1464, 0.7758);//RHO(770)
   DK2piRes[1] = Babar_resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 0.0391,115.3, 0.00849,0.78259,1);//OMEGA
   DK2piRes[2] = Babar_resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 0.482, -141.8, 0.044,  0.975,  0);//F_0(980)
   DK2piRes[3] = Babar_resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 0.922, -21.3,   0.1851, 1.2754, 2);//F_2(1270)
   DK2piRes[4] = Babar_resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 2.25, 113.2,  0.173,  1.434,  0);//F_0(1370) hep-ph 0009168
   DK2piRes[5] = Babar_sakurai(x, y, z, 0.52, 38, 0.455,  1.406);//RHO(1450)
   DK2piRes[6] = Babar_resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 1.36,  -177.9,   0.383,  0.484,  0);//Sigma(600)
   DK2piRes[7] = Babar_resAmp(x, y, z, m_mass[3], m_mass[2], m_mass[1], 0.340,  153.0,   0.088,  1.014,  0);//Sigma
   DK2piRes[8] = Babar_resAmp(x, z, y, m_mass[3], m_mass[1], m_mass[2], 1.781, 131.0, 0.0508, 0.89166,1);//K*(892)-
   DK2piRes[9] = Babar_resAmp(x, z, y, m_mass[3], m_mass[1], m_mass[2], 0.52,  154, 0.232,  1.414,  1);//K*(1410)-
   DK2piRes[10] = Babar_resAmp(x, z, y, m_mass[3], m_mass[1], m_mass[2], 2.45,  -8.3, 0.294,  1.412,  0);//K*_0(1430)-
   DK2piRes[11] = Babar_resAmp(x, z, y, m_mass[3], m_mass[1], m_mass[2], 1.05,  -54.3, 0.0985, 1.4256, 2);//K*_2(1430)-
   DK2piRes[12] = Babar_resAmp(x, z, y, m_mass[3], m_mass[1], m_mass[2], 0.89, -139,   0.322,  1.717,  1);//K*(1680)-
   DK2piRes[13] = Babar_resAmp(y, z, x, m_mass[2], m_mass[1], m_mass[3], 0.180, -44.1, 0.0508, 0.89166,1);//K*(892)+
   DK2piRes[14] = Babar_resAmp(y, z, x, m_mass[2], m_mass[1], m_mass[3], 0.37,  18,    0.294,  1.412,  0);//K*_0(1430)+
   DK2piRes[15] = Babar_resAmp(y, z, x, m_mass[2], m_mass[1], m_mass[3], 0.075, -104, 0.0985, 1.4256, 2);//K*_2(1430)+

   double pi180inv = 3.1415926/180.;
   DK2piRes[16] = TComplex(3.53*cos(128*pi180inv),3.53*sin(128*pi180inv));

   for(int i=0; i<17; i++){
      D0 += DK2piRes[i];
   }

   return D0;
} // End Babar_amplitude

int Dalitz::getBin(double mx, double my, double mz) {

   // For computing Dalitz variable kinemtaics
   double m_mass_2[4]={ 1.86450*1.86450, 0.497648*0.497648,
                        0.139570*0.139570, 0.139570*0.139570}; //mass square
   double m_sum_m_2 = m_mass_2[0] + m_mass_2[1] + m_mass_2[2] + m_mass_2[3];

   // Check if on DP
   if( !(Point_on_DP2(mx, my)) && !(Point_on_DP2(my, mx)) ) return -1;

   // Over-ride user z, use computed z(x,y) instead
   mz = m_sum_m_2 - mx - my;
   if (mz < 0) mz = 0;

   // Determine phase
   double thisPhase = Phase(mx, my, mz);

   // Change this to bin that is found (-1 means not found)
   int thisbin = -1;

   // Determine the bin and increment
   for (int bin = 0; bin < N; bin++) {
      if((thisPhase >= (bin-0.5)*2*PI/N) && (thisPhase < (bin+0.5)*2*PI/N))  thisbin = bin;
   }

   return thisbin;

} // end getBin

// ------------   end Dalitz.cxx
