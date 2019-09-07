#include "TDFun.h"
//#include "TF1.h"
#include "TRadGlobal.h"

TDFun *djr = NULL;

double tdfun(const double &x){
  return djr->Eval(x);
}
/*
double tdfun(double *x, double *p){
  return djr->Eval(*x);
}
*/
TDFun::TDFun(){
  fd = NULL;
  Init(1-gGlobal->Get_XMax(),1-gGlobal->Get_XMin());
}

TDFun::TDFun(const double &cmin, const double &cmax){
  fd = NULL;
  Init(cmin,cmax);
}

void TDFun::Init(){
  Init(1-gGlobal->Get_XMax(),1-gGlobal->Get_XMin());
}

void TDFun::Init(const double &cmin, const double &cmax){
  fxmin = cmin;
  fxmax = cmax;
  fb2   = 0.5*gGlobal->Get_Beta();
  fD0   = gGlobal->Get_D0();
  djr   = this;
  //  TF1 fdfun("dfun",tdfun,fxmin,fxmax,0);
  if(fd) delete fd;
  fd    = new TRadSpline3("d", fxmin, fxmax, tdfun, 10001);
}

TDFun::~TDFun(){
  if(fd) delete fd;
}
