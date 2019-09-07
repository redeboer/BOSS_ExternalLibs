#include <iostream>
#include <cmath>
#include "TPiFormFactor.h"
#include "TConstants.h"

TPiFormFactor::TPiFormFactor():TFormFactor(){
  fPrm  = 0;
  /*
  SetFFParameters(775.77,     // M_rho
		  144.09,     // G_rho
		  1464.6,     // M_rho_p
		  308.72,     // G_rho_p
		  782.70,     // M_omega
		  8.6888,     // G_omega
		  0.595e-3,   // G_omega_ee
		  0.013307,   // Br_omega_pipi
		  13.25,      // Phi_rho_omega in degree
		  -0.80305e-1 // Beta
		  );
  */
  /*
  // CMD-2 pion formfactor parametriztion in rho-meson region
  // 23 june 2003
  SetFFParameters(775.68,
		  143.89,
		  1464.6,
		  308.72,
		  782.70,
		  8.6855,
		  0.59500e-03,
		  0.13033e-01,
		  13.297,
		  -0.80656e-01
		  );
  */

  // pion formactor parametrization with rho' rho'' should be valid up to 3 GeV 
  /*
  double par[20]=
    {
      775.364,       //  M_rho
      143.533,       //  G_rho
      1316.67,       //  M_rho_p
      465.943,       //  G_rho_p
      1732.79,       //  M_rho_p2
      175.142,       //  G_rho_p2
      0.0146604,     //  Br_omega_pipi
      0.191774,      //  Phi_rho_omega
      0.335192,      //  Beta
      -2.19222,      //  Phi_rho_p
      0.035182,      //  Delta
      -2.17322,      //  Phi_rho_p2
      782.57,        //  M_omega
      8.44,          //  G_omega
      6.95e-5*8.44,  //  G_omega_ee
      1019.456,      //  M_phi
      4.26,          //  G_phi
      2.96e-4*4.26,  //  G_phi_ee
      7.3e-5,        //  Br_phi_pipi
      TMath::Pi()    //  Phi_rho_phi
    };
  */

  double par[20]=
    {
      774.824,       //  M_rho
      141.335,       //  G_rho
      1294.08,       //  M_rho_p
      472.468,       //  G_rho_p
      1732.79,       //  M_rho_p2
      175.142,       //  G_rho_p2
      0.0124238,     //  Br_omega_pipi
      0.264541,      //  Phi_rho_omega
      0.37245,       //  Beta
      -2.17681,      //  Phi_rho_p
      0.035182,      //  Delta
      -2.17322,      //  Phi_rho_p2
      782.57,        //  M_omega
      8.44,          //  G_omega
      6.95e-5*8.44,  //  G_omega_ee
      1019.456,      //  M_phi
      4.26,          //  G_phi
      2.96e-4*4.26,  //  G_phi_ee
      7.3e-5,        //  Br_phi_pipi
      M_PI           //  Phi_rho_phi
    };
  SetFFParameters(par);

}

void TPiFormFactor::SetFFParameters(const double &p0, const double &p1, 
				  const double &p2, const double &p3, 
				  const double &p4, const double &p5, 
				  const double &p6, const double &p7,
				  const double &p8, const double &p9){
  const std::complex<double> One = std::complex<double>(1,0), I = std::complex<double>(0,1);
  fPrm  =  1;
  fM_rho         = p0;
  fG_rho         = p1;

  fM_rho_p       = p2;
  fG_rho_p       = p3;

  fM_omega       = p4;
  fG_omega       = p5;

  fG_omega_ee    = p6;
  fBr_omega_pipi = p7;
  fPhi_rho_omega = p8*gConst->Pi()/180;

  fBeta          = p9;

  fM_omega2      = fM_omega*fM_omega;

  std::complex<double> fpi_rho_omega = GS(fM_omega2, fM_rho, fG_rho);
  double br_omega = 2*gConst->Alpha2()*pow(ppi(fM_omega2),3)/(9*fM_omega*fG_omega*fG_omega_ee); 
  fAmp_omega  = (One + fBeta)/std::abs(fpi_rho_omega)*sqrt(fBr_omega_pipi/br_omega);
  fAmp_omega *= exp(I*fPhi_rho_omega);
}

void TPiFormFactor::SetFFParameters(double *p){
  const std::complex<double> One = std::complex<double>(1,0), I = std::complex<double>(0,1);
  fPrm  =  2;
  fM_rho         = p[0];
  fG_rho         = p[1];
  fM_rho_p       = p[2];
  fG_rho_p       = p[3];
  fM_rho_p2      = p[4];
  fG_rho_p2      = p[5];
  fBr_omega_pipi = p[6];
  fPhi_rho_omega = p[7];
  fBeta          = p[8]*exp(I*p[9]);
  fDelta         = p[10]*exp(I*p[11]);
  fM_omega       = p[12];
  fG_omega       = p[13];
  fG_omega_ee    = p[14];
  fM_phi         = p[15];
  fG_phi         = p[16];
  fG_phi_ee      = p[17];
  fBr_phi_pipi   = p[18];
  fPhi_rho_phi   = p[19];
   		 
  fM_omega2      = fM_omega*fM_omega;
  fM_phi2        = fM_phi*fM_phi;

  std::complex<double> fpi_rho_omega = GS(fM_omega2, fM_rho, fG_rho);
  double br_omega  = 2*gConst->Alpha2()*pow(ppi(fM_omega2),3)/(9*fM_omega*fG_omega*fG_omega_ee);
  double amp_omega = std::abs((One + fBeta + fDelta)/fpi_rho_omega)*sqrt(fBr_omega_pipi/br_omega);
  fAmp_omega       = amp_omega*exp(I*fPhi_rho_omega);

  std::complex<double> fpi_rho_phi = GS(fM_phi2, fM_rho, fG_rho);
  double br_phi  = 2*gConst->Alpha2()*pow(ppi(fM_phi2),3)/(9*fM_phi*fG_phi*fG_phi_ee);
  double amp_phi = std::abs((One + fBeta + fDelta)/fpi_rho_phi)*sqrt(fBr_phi_pipi/br_phi);
  fAmp_phi       = amp_phi*exp(I*fPhi_rho_phi);

}

std::complex<double> TPiFormFactor::ff(const double &s){
  const std::complex<double> One = std::complex<double>(1,0), I = std::complex<double>(0,1);
  std::complex<double> cfpi_rho, cfpi_rho_p, cfpi_rho_p2, cfpi_omega, cfpi_phi, Alpha;
  if(fPrm==1){

    cfpi_rho    = GS(s, fM_rho   , fG_rho   );
    cfpi_rho_p  = GS(s, fM_rho_p , fG_rho_p );
    cfpi_omega  = s/(s - fM_omega2 + I*fM_omega*fG_omega);

    Alpha = One - fAmp_omega*cfpi_omega;
    return (
	    Alpha*cfpi_rho +
	    fBeta*cfpi_rho_p
	    )/(One + fBeta);

  }else if(fPrm==2){

    cfpi_rho    = GS(s, fM_rho   , fG_rho   );
    cfpi_rho_p  = GS(s, fM_rho_p , fG_rho_p );
    cfpi_rho_p2 = GS(s, fM_rho_p2, fG_rho_p2);
    cfpi_omega  = s/(s - fM_omega2 + I*fM_omega*fG_omega);
    cfpi_phi    = s/(s - fM_phi2   + I*fM_phi  *fG_phi  );
    
    Alpha = One - fAmp_omega*cfpi_omega - fAmp_phi*cfpi_phi;
    return (
	    Alpha *cfpi_rho   +
	    fBeta *cfpi_rho_p +
	    fDelta*cfpi_rho_p2
	    )/(One + fBeta + fDelta);

  }
  return One;
}

std::complex<double> TPiFormFactor::GS(const double &s, 
				     const double &M, 
				     const double &G){
  const std::complex<double> I = std::complex<double>(0,1);
  double M2 = M*M;
  double prho = ppi(M2);
  double pi = gConst->Pi();
  double mp = gConst->Mpi();
  double d  = 3/pi*mp*mp/(prho*prho)*log((M+2*prho)/(2*mp)) +
    M/(2*pi*prho) - mp*mp*M/(pi*pow(prho,3));
  return (1+d*G/M)/((1+(f(s,M,G)-s)/M2) - I*G_rho(s,M,G)/M);
}

double TPiFormFactor::f(const double &s, 
		      const double &M, 
		      const double &G){
  double M2 = M*M;
  double prho = ppi(M2);
  double dhds = h(M2)*(1/(8*pow(prho/M,2))-1./2.) + 1/(2*gConst->Pi());
  return G*prho*pow(M/prho,2)*(pow(ppi(s)/prho,2)*(h(s)-h(M2)) + (1-s/M2)*dhds);
}

double TPiFormFactor::G_rho(const double &s, 
			  const double &M, 
			  const double &G){
  double M2 = M*M;
  double G_rho = G*pow(ppi(s)/ppi(M2),3)*sqrt(M2/s);
  return G_rho;
}

double TPiFormFactor::h(const double &s){
  return 2/gConst->Pi()*ppi(s)/sqrt(s)*log((sqrt(s)+2*ppi(s))/(2*gConst->Mpi()));
}

double TPiFormFactor::ppi(const double &s){
  return 0.5*sqrt(s-4*gConst->Mpi2());
}
