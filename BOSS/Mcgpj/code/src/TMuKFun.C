#include "TMuKFun.h"
#include "TLi2.h"
#include "TRadGlobal.h"
//#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

double TMuKFun::Eval_even(const double &s, const double &c){
  double r      = gGlobal->Get_MF2()*0.25*gGlobal->Get_s()/s;
  double rho    =-log(r);
  double beta   = sqrt(1-4*r);
  double beta1p = 1+beta;
  double beta1m = 1-beta;
  double rbeta  = beta1m/beta1p;
  double ibeta  = 1/beta;
  double beta2  = beta*beta;
  double lbeta  = -log(rbeta);
  double k_even = -1 + rho*(1.5 + beta2 - beta)*0.5*ibeta +
    log(0.5*beta1p)*(1.5+beta2)*ibeta - 0.5*ibeta*(1-beta2)*lbeta/(2-beta2*(1-c*c))+
    0.5*ibeta*(1+beta2)*(gConst->Pi2()/6 + 2*TLi2::Eval(rbeta) + lbeta*log(0.5*ibeta*ibeta*beta1p));

  // relative velocity between final particles (inversed)
  double iv = 0.5*ibeta*(1+beta2);

  return k_even - 0.5*gConst->Pi2()*iv;
}

double TMuKFun::Eval_Coloumb_Factor(const double &s){
  // Coloumb final state interaction, the first term of
  // expansion of Sakharov-Sommerfeld factor is taken into
  // account in formulae above, so it should be subtracted from K_Even

  double beta  = sqrt(1 - gGlobal->Get_MF2()*gGlobal->Get_s()/s);
  // relative velocity between final particles (inversed)
  double iv = (1+beta*beta)/(2*beta);
  double z = 2*gConst->Pi()*gConst->Alpha()*iv;
  // Sakharov-Sommerfeld factor
  return z/(1. - exp(-z));
}

double TMuKFun::Eval_even_ultra(const double &s){
  return 3./4.*log(s/(gGlobal->Get_MF2()*0.25*gGlobal->Get_s())) + gConst->Pi2()/6. - 1.;
}

double TMuKFun::Eval_odd(const double &s, const double& c){
  double r      = gGlobal->Get_MF2()*0.25*gGlobal->Get_s()/s;
  double rho    =-log(r);
  double beta2  = 1 - 4*r;
  double beta   = sqrt(beta2);
  double ibeta  = 1/beta;
  double ibeta2 = 0.5*ibeta*ibeta;

  double d      = -rho*ibeta2 + (gConst->Pi2()/12 + 0.25*rho*rho)*
    (1 - ibeta - 0.5*beta + ibeta2*ibeta) + ibeta*(-1 - 0.5*beta2 + ibeta2)*
    (rho*log(0.5*(1 + beta)) - 2*TLi2::Eval(0.5*(1-beta)) - TLi2::Eval(-(1-beta)/(1+beta)));

  double betac  = beta*c;
  double betas2 = beta2*(1-c*c);
  double i2mbetas2 = 1/(2-betas2);
  double tY     = 1 - beta2 - betas2;

  double l_m    = log(0.5*(1-betac));
  double b      = rho + l_m;
  double t1     = (1-beta2)/(2*(1-betac));
  double a      = 0.5*l_m*l_m - log(1 - t1)*b + TLi2::Eval(t1);
  double inv1   = 1/(1 + beta2 - 2*betac);
  double li2t2  = TLi2::Eval(betas2*inv1);

  double res_plus = a + li2t2 + i2mbetas2*(-tY*b*inv1 - 2*r*a + betac*(d - a));

  //changing sign of theta cosine
  betac = -betac;
  l_m    = log(0.5*(1-betac));
  b      = rho + l_m;
  t1     = (1-beta2)/(2*(1-betac));
  a      = 0.5*l_m*l_m - log(1 - t1)*b + TLi2::Eval(t1);
  inv1   = 1/(1 + beta2 - 2*betac);
  li2t2  = TLi2::Eval(betas2*inv1);

  double res_minus= a + li2t2 + i2mbetas2*(-tY*b*inv1 - 2*r*a + betac*(d - a));

  return res_plus - res_minus + Eval_int_odd(beta,c);
}

double TMuKFun::Eval_odd_ultra(const double& c){
  // Ultra-relativistic limit of odd part of K-function 
  double st_h2 = 0.5*(1-c);
  double st_h  = sqrt(st_h2);
  double ct_h2 = 0.5*(1+c);
  double ct_h  = sqrt(ct_h2);
  double lst_h = log(st_h);
  double lct_h = log(ct_h);
  /*
  std::cout<<c<<" "
	   <<st_h2<<" "
	   <<ct_h2<<" "
	   <<st_h<<" "
	   <<ct_h<<" "
	   <<lst_h<<" "
	   <<lct_h<<" "
	   <<std::endl;
  */
  return 2*lst_h*lst_h - 2*lct_h*lct_h - TLi2::Eval(st_h2) + TLi2::Eval(ct_h2)
    + 2/(1+c*c)*(ct_h2*lst_h - st_h2*lct_h - c*(lst_h*lst_h +lct_h*lct_h ));
}

double TMuKFun::Eval_even_odd(const double &s, const double &c){
  //  std::cout<<0.25*gGlobal->Get_MF2()*gGlobal->Get_s()<<" "<<gConst->Mmu2()<<std::endl;
  // even part of K-function
  double r      = gGlobal->Get_MF2()*0.25*gGlobal->Get_s()/s;
  double rho    =-log(r);
  double beta2  = 1 - 4*r;
  double beta   = sqrt(beta2);
  double beta1p = 1+beta;
  double beta1m = 1-beta;
  double rbeta  = beta1m/beta1p;
  double ibeta  = 1/beta;
  double lbeta  = -log(rbeta);
  double betas2 = beta2*(1-c*c);
  double i2mbetas2 = 1/(2-betas2);
  double k_even = -1 + rho*(1.5 + beta2 - beta)*0.5*ibeta +
    log(0.5*beta1p)*(1.5+beta2)*ibeta - 0.5*ibeta*(1-beta2)*lbeta*i2mbetas2+
    0.5*ibeta*(1+beta2)*(gConst->Pi2()/6 + 2*TLi2::Eval(rbeta) + lbeta*log(0.5*ibeta*ibeta*beta1p));
  // relative velocity between final particles (inversed). Coloumb
  // final state interaction is taken into account in another place
  double iv = 0.5*ibeta*(1+beta2);
  k_even -= 0.5*gConst->Pi2()*iv;

  double ibeta2 = 0.5*ibeta*ibeta;
  double d      = -rho*ibeta2 + (gConst->Pi2()/12 + 0.25*rho*rho)*
    (1 - ibeta - 0.5*beta + ibeta2*ibeta) + ibeta*(-1 - 0.5*beta2 + ibeta2)*
    (rho*log(0.5*beta1p) - 2*TLi2::Eval(0.5*beta1m) - TLi2::Eval(-rbeta));

  double betac  = beta*c;
  double tY     = 1 - beta2 - betas2;

  double l_m    = log(0.5*(1-betac));
  double b      = rho + l_m;
  double t1     = (1-beta2)/(2*(1-betac));
  double a      = 0.5*l_m*l_m - log(1 - t1)*b + TLi2::Eval(t1);
  double inv1   = 1/(1 + beta2 - 2*betac);
  double li2t2  = TLi2::Eval(betas2*inv1);

  double res_plus = a + li2t2 + i2mbetas2*(-tY*b*inv1 - 2*r*a + betac*(d - a));

  //changing sign of theta cosine
  betac = -betac;
  l_m    = log(0.5*(1-betac));
  b      = rho + l_m;
  t1     = (1-beta2)/(2*(1-betac));
  a      = 0.5*l_m*l_m - log(1 - t1)*b + TLi2::Eval(t1);
  inv1   = 1/(1 + beta2 - 2*betac);
  li2t2  = TLi2::Eval(betas2*inv1);

  double res_minus= a + li2t2 + i2mbetas2*(-tY*b*inv1 - 2*r*a + betac*(d - a));

  return k_even + res_plus - res_minus + Eval_int_odd(beta,c);
}

#define NX 50
#define NY 101
#define T(i,j) fm[NY*(i)+j]

void TMuKFun::Init(std::string intd, std::string intf){
  if(!fm){
    fm = new float[NX*NY];
/*
    char *intd = getenv("INTEG_DIR");
    char *intf = getenv("INTEG_FNAME");
    std::string fname;
    if(intf == NULL){
      if(intd == NULL){
	fname = "integ.dat";
      }else{
	std::ostringstream temp;
	temp<<intd<<"/"<<"integ.dat";
	fname = temp.str();
      }
    } else {
      fname = intf;
    }
*/
    std::string fname;
    if(! intf.empty()) fname = intf;
    else fname = "integ.dat";

    if(! intd.empty()) fname = intd + "/" + fname;
    
    std::ifstream IN(fname.c_str());
    if(IN.is_open() == false ){
      std::cout<<"Can't read \""<<fname<<"\". "
	       <<"Angular distribution will be partially incorrect!"
	       <<std::endl;
      std::cout<<"Probably you should tune INTEG_DIR or INTEG_FNAME variables."<<std::endl;
      for(int i=0;i<NX;i++){
	for(int j=0;j<NY;j++)
	  T(i,j) = 0;
      }
      return;
    }
    float t1,t2;
    for(int i=0;i<NX;i++){
      for(int j=0;j<NY-1;j++)
	IN>>t1>>t2>>T(i,j);
      T(i,0) = 0;
      T(i,NY-1) = 0;
    }
    IN.close();
  }
}

double TMuKFun::Eval_int_odd(const double &beta, const double &c){
  int i = int(floor((beta - 0.01)*50));
  int j = int(floor((c + 1)*50));
  double wy = 50*c - (j - 50);
  double res = 0;
  if(i >= 0 && i < NX-1){
    double wx = 50*beta - i - 0.5;
    res = 
      (T(i,j  )*(1-wx) + T(i+1,j  )*wx)*(1-wy) + 
      (T(i,j+1)*(1-wx) + T(i+1,j+1)*wx)*wy;
  } else if (i == -1){
    double wx = 100*beta;
     res = 
      (T(0,j  )*(1-wy) + T(0,  j+1)*wy)*wx;
  } else if (i == NX - 1){
    double wx = 1 - 100*(beta-0.99);
    res = 
      (T(NX-1,j)*(1-wy)+ T(NX-1,j+1)*wy)*wx;
  }
  return -res;
}

TMuKFun::TMuKFun(){
  fm = NULL;
//  Init();
}

TMuKFun::~TMuKFun(){
  delete fm;
}
