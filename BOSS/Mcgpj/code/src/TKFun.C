#include <cmath>
#include "TKFun.h"
#include "TGlobal.h"
#include "TConstants.h"

TKFun *kjr = NULL;

double TKFun::kfun(const double &co){
  double sa  = sqrt((1.-co)*0.5);
  double sa2 = sa*sa;
  double ca  = sqrt((1.+co)*0.5);
  double ca2 = ca*ca;
  double c1  = log(sa);
  double c2  = log(ca);
  double co2 = co*co;
  double co3 = co2*co;
  double co4 = co2*co2;
  double k = -1. + 2.*TLi2::Eval(ca2) - 2.*TLi2::Eval(sa2)+ 
    (gConst->Pi2()/3.*(2.*co4 - 3.*co3 - 15.*co) + 
     2*(2*co4 - 3*co3 + 9*co2 + 3*co + 21.)*(c1*c1) -
     4.*(co4 + co2 - 2.*co)*c2*c2 - 
     4.*(co3 + 4.*co2 + 5.*co + 6.)*(c1-c2)*(c1-c2) +
     2.*(co3 - 3.*co2 + 7.*co - 5.)*c2 +
     2.*(3.*co3 + 9.*co2 + 5.*co + 31.)*c1)/((3.+co2)*(3.+co2));
  return k;
};

double tkfun(double *x){
  return kjr->kfun(*x);
}
/*
double tkfun(double *x, double *p){
  return kjr->kfun(*x);
}
*/
TKFun::TKFun(){
  fk = NULL;
  kjr = this;
  Init(gGlobal->Get_ThetaMin());
}

void TKFun::Init(){
  Init(gGlobal->Get_ThetaMin());
}

void TKFun::Init(const double &theta){
  fxmin = -cos(theta);
  fxmax = cos(theta);
  //  TF1 fkfun("kfun",tkfun,fxmin,fxmax,0);
  if(fk)delete fk;
  fk    = new TRadSpline3("k", fxmin, fxmax, tkfun, 10001);
}

TKFun::TKFun(const double &theta){
  fk = NULL;
  kjr = this;
  Init(theta);
}

TKFun::~TKFun(){
  delete fk;
}

