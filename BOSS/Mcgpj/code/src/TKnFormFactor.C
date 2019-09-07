#include "TKnFormFactor.h"
#include "TRadGlobal.h"
#ifdef K0
#include "xsection.h"

xsection *fFK0 = NULL;

TKnFormFactor::TKnFormFactor():TFormFactor(){
  // Neutral Kaon Formfactor
  fFK0 = new xsection();
  fFK0->SetChannel(0);     // Neutral kaons channel
  fFK0->SetITMvalue(0);    // Integral test mode value - off
  fFK0->SetSpectraType(1); // full VDM rho-omega-phi
  fFK0->SetEMvalue(0);     // must be zero
  fFK0->SetMatrix2Type(true); // form-factor only
  
#ifdef TEST
  cout<<"s="<<4*500*500<<"MeV^2, Fk2="<<fFK0->GetXS(4*500*500)<<endl;
  cout<<"s="<<gGlobal->Get_s()<<"MeV^2, Fk2="<<fFK0->GetXS(gGlobal->Get_s())<<endl;
  cout<<"s="<<1.01*gGlobal->Get_s()<<"MeV^2, Fk2="<<fFK0->GetXS(1.01*gGlobal->Get_s())<<endl;
  cout<<"s="<<1.1*gGlobal->Get_s()<<"MeV^2, Fk2="<<fFK0->GetXS(1.1*gGlobal->Get_s())<<endl;
  cout<<"s="<<1.5*gGlobal->Get_s()<<"MeV^2, Fk2="<<fFK0->GetXS(1.5*gGlobal->Get_s())<<endl;
#endif
  Init(gGlobal->Get_s());
}

TKnFormFactor::~TKnFormFactor(){
  if(fFK0) delete fFK0;
}

std::complex<double> TKnFormFactor::ff(const double &s){
  return std::complex<double>(sqrt(fFK0->GetXS(s)),0);
}

#else

TKnFormFactor::TKnFormFactor():TFormFactor(){}

TKnFormFactor::~TKnFormFactor(){}

std::complex<double> TKnFormFactor::ff(const double &s){
  return std::complex<double>(1,0);
}

#endif
