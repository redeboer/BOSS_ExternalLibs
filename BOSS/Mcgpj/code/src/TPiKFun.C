#include <iostream>
#include "TPiKFun.h"
#include "TLi2.h"
#include "TGlobal.h"
#include "TRadGlobal.h"

/*
double TPiKFun::Eval_b(const double &s){
  // b-function from JHEP10(1997) 006 page 3 equation 2.5
  double r      = gGlobal->Get_MF2()*0.25*gGlobal->Get_s()/s;
  double rho    =-log(r);
  double beta   = sqrt(1-4*r);
  double beta1p = 1+beta;
  double beta1m = 1-beta;
  double rbeta  = beta1m/beta1p;
  double ibeta  = 1/beta;
  double logb   = log(beta);
  double logb1  = log(0.5*beta1p);
  double A      = -TLi2::Eval(-rbeta) + TLi2::Eval(rbeta) - gConst->Pi2()/12 + 
    rho*logb1 - 2*rho*logb + 1.5*logb1*logb1 - 0.5*logb*logb - 3*logb*logb1 +
    rho + 2*logb1;
  double K_even = -1 + 0.5*beta1m*ibeta*rho + ibeta*logb1 + 0.5*(1+beta*beta)*ibeta*A;
  return K_even;
}
*/

double TPiKFun::Eval_even(const double &s, const double &c){
  // b-function from errata of JHEP10(1997) 006
  double r      = gGlobal->Get_MF2()*0.25*gGlobal->Get_s()/s;
  double rho    =-log(r);
  double beta   = sqrt(1-4*r);
  double beta1p = 1 + beta;
  double beta1m = 1 - beta;
  double rbeta  = beta1m/beta1p;
  double ibeta  = 1/beta;
  double A      = rho + gConst->Pi2()/6 - log(rbeta)*log(0.5*ibeta*ibeta*beta1p) + 2*TLi2::Eval(rbeta);
  double k_even = -1 + 0.5*beta1m*ibeta*rho + (2 + beta*beta)*ibeta*log(0.5*beta1p) + 0.5*(1+beta*beta)*ibeta*A;
  return k_even;
}

double TPiKFun::Eval_odd(const double &s, const double &c){
  double r      = gGlobal->Get_MF2()*0.25*gGlobal->Get_s()/s;
  double rho    =-log(r);
  double beta2  = 1 - 4*r;
  double betas2 = beta2*(1-c*c);
  double i2betas2 = 1/(2*betas2);
  double beta   = sqrt(beta2);
  double ibeta  = 1/beta;

  double d      =-0.25*ibeta*(1-beta)*(1-beta)*(rho*rho + gConst->Pi2()/3)
    + ibeta*(1 + beta2)*(-rho*log(0.5*(1 + beta)) - 
			 TLi2::Eval(-(1-beta)/(1+beta)) + 2*TLi2::Eval(0.5*(1-beta)));

  double betac  = beta*c;
  double betc1m = 1 - betac;
  double l_m    = log(0.5*betc1m);
  double l_m2   = 0.5*l_m*l_m;
  double t1     = (1-beta2)/(2*betc1m);
  double a      = l_m2 - log(1 - t1)*(rho + l_m ) + TLi2::Eval(t1);
  double li2t1  = TLi2::Eval(1 - t1);
  double li2t2  = TLi2::Eval(betas2/(1 + beta2 - 2*betac));

  double res_plus = l_m2 - li2t1 + li2t2 + i2betas2*(a*(1 - beta2) + betc1m*(-2*a + d));

  //changing sign of theta cosine
  betac  = -betac;
  betc1m = 1 - betac;
  l_m    = log(0.5*betc1m);
  l_m2   = 0.5*l_m*l_m;
  t1     = (1-beta2)/(2*betc1m);
  a      = l_m2 - log(1 - t1)*(rho + l_m ) + TLi2::Eval(t1);
  li2t1  = TLi2::Eval(1 - t1);
  li2t2  = TLi2::Eval(betas2/(1 + beta2 - 2*betac));
  double res_minus= l_m2 - li2t1 + li2t2 + i2betas2*(a*(1 - beta2) + betc1m*(-2*a + d));

  return res_plus - res_minus + Eval_int_odd(beta,c);
}

double TPiKFun::Eval_odd_ultra(const double &c){
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
  return (2-1/ct_h2)*lst_h*lst_h - (2-1/st_h2)*lct_h*lct_h - TLi2::Eval(st_h2) + TLi2::Eval(ct_h2);
}

double TPiKFun::Eval_even_odd(const double &s, const double &c){
  // b-function from errata of JHEP10(1997) 006
  double r      = gGlobal->Get_MF2()*0.25*gGlobal->Get_s()/s;
  double rho    =-log(r);
  double beta2  = 1 - 4*r;
  double beta   = sqrt(beta2);
  double beta1p = 1 + beta;
  double beta1m = 1 - beta;
  double rbeta  = beta1m/beta1p;
  double ibeta  = 1/beta;
  double lgbeta = log(0.5*beta1p);
  double A      = rho + gConst->Pi2()/6.0 - log(rbeta)*log(0.5*ibeta*ibeta*beta1p) + 2*TLi2::Eval(rbeta);
  double k_even = -1 + 0.5*beta1m*ibeta*rho + (2 + beta*beta)*ibeta*lgbeta + 0.5*(1+beta*beta)*ibeta*A;

  double betas2 = beta2*(1-c*c);
  double i2betas2 = 1/(2*betas2);

  double d      =-0.25*ibeta*beta1m*beta1m*(rho*rho + gConst->Pi2()/3)
    + ibeta*(1 + beta2)*(-rho*lgbeta - TLi2::Eval(-rbeta) + 2*TLi2::Eval(0.5*beta1m));

  double betac  = beta*c;
  double betc1m = 1 - betac;
  double l_m    = log(0.5*betc1m);
  double l_m2   = 0.5*l_m*l_m;
  double t1     = (1-beta2)/(2*betc1m);
  double a      = l_m2 - log(1 - t1)*(rho + l_m ) + TLi2::Eval(t1);
  double li2t1  = TLi2::Eval(1 - t1);
  double li2t2  = TLi2::Eval(betas2/(1 + beta2 - 2*betac));

  double res_plus = l_m2 - li2t1 + li2t2 + i2betas2*(a*(1 - beta2) + betc1m*(-2*a + d));

  //changing sign of theta cosine
  betac  = -betac;
  betc1m = 1 - betac;
  l_m    = log(0.5*betc1m);
  l_m2   = 0.5*l_m*l_m;
  t1     = (1-beta2)/(2*betc1m);
  a      = l_m2 - log(1 - t1)*(rho + l_m ) + TLi2::Eval(t1);
  li2t1  = TLi2::Eval(1 - t1);
  li2t2  = TLi2::Eval(betas2/(1 + beta2 - 2*betac));
  double res_minus= l_m2 - li2t1 + li2t2 + i2betas2*(a*(1 - beta2) + betc1m*(-2*a + d));

  return k_even + res_plus - res_minus + Eval_int_odd(beta,c);
}
