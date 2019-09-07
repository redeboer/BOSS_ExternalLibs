#include <iostream>
#include "TFormFactor.h"
#ifndef ALONE
#  include "TRadGlobal.h"
#endif
#include "TConstants.h"

TFormFactor::TFormFactor(){
  fsvsr = NULL;
  fsvsi = NULL;
  fNp   = 4000;
  fUnitFF = false;
}

TFormFactor::~TFormFactor(){
  if(fsvsr) delete fsvsr;
  if(fsvsr) delete fsvsi;
  //  std::cout<<"Deleting FormFactor"<<std::endl;
}

void TFormFactor::Init(){
#ifndef ALONE
  double smax = gGlobal->Get_s();
#else
  double smax = 4*800*800;
#endif
  Init(smax);
}

void TFormFactor::Init(const double &smax){
  double *s     = new double[fNp];
  double *re_vs = new double[fNp];
  double *im_vs = new double[fNp];
  /*  
  for(int i = 1;i<=1;i++){
    double en = 500 + i;
    std::complex<double> form = ff(pow(2*en,2));
    std::cout<< en << " " << form<<" "<<std::abs(form*std::conj(form))<<std::endl;
  }
  exit(0);
  */
  double smin = gGlobal->Get_MF2()*gGlobal->Get_s();
  double ds = (smax-smin)/(fNp-1);
  for(unsigned int i=0; i<fNp; i++){
    s[i] = smin + i*ds;
    std::complex<double> vs = ff( (i>0) ? s[i] : smin + 0.01*ds);
    //    std::cout<<i<<" "<<s[i]<<" "<<vs<<std::endl;
    re_vs[i] = std::real(vs);
    im_vs[i] = std::imag(vs);
  }

  if(fsvsr) delete fsvsr;
  fsvsr = new TRadSpline3("vsr", s, re_vs, fNp);

  if(fsvsi) delete fsvsi;
  fsvsi = new TRadSpline3("vsi", s, im_vs, fNp);

  delete [] re_vs;
  delete [] im_vs;
  delete [] s;
}

void TFormFactor::Print(){
#ifndef ALONE
  std::complex<double> form = Eval(gGlobal->Get_s());
#else
  std::complex<double> form = Eval(4*200*200);
#endif
  std::cout<<"Formfactor is "<<form<<std::endl;
  std::cout<<"Squared formfactor is "<<std::abs(form*std::conj(form))<<std::endl;
  std::cout<<std::flush;
}
