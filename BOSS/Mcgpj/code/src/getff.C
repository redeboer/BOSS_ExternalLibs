#include "TPiFormFactor.h"
#include "TRadGlobal.h"
#include "TConstants.h"
#include <iostream>
#include <complex>
#include <stdexcept>
using namespace std;

//TRadGlobal *gGlobal;
//TConstants *gConst;
int main(unsigned int argc, char **argv){
  gConst = new TConstants();
  gGlobal = new TRadGlobal();
  gGlobal->Set_Type(2);
  gGlobal->Set_E(800.);
  gGlobal->Set_ThetaMin(0.9);
  gGlobal->Set_dE_Abs(0.1);
  gGlobal->Set_Theta0_Rel(1);
  gGlobal->Set_ThetaInt(0.6);
  gGlobal->Set_TotalError(1.);
  gGlobal->Set_RelativeError(0.01);
  
  try{
    gGlobal->Init();
  }catch(std::logic_error lge){
    cout<<lge.what()<<endl;
    return 1;
  }
  
  TPiFormFactor *fPi = new TPiFormFactor();
  fPi->Init();
  for (unsigned int i = 1; i < argc; i++){
    double e = atof(argv[i]);
    complex<double> f = fPi->Eval(4*e*e);
    cout<<abs(f*conj(f))<<endl;
  }
  /*
  double En[] = {185, 195, 205, 215, 225, 235, 240, 250, 255, 260};
  for (unsigned int i = 0; i < 10; i++){
    double e = En[i];
    double beta = sqrt(e*e - gConst->Mpi2())/e;
    complex<double> f = fPi->Eval(4*e*e);
    double cs0 = pow(beta,3)/pow(e,2)*abs(f*conj(f));

    e = 1.001*En[i];
    beta = sqrt(e*e - gConst->Mpi2())/e;
    f = fPi->Eval(4*e*e);
    double cs1 = pow(beta,3)/pow(e,2)*abs(f*conj(f));

    cout<<En[i]<<" "<<(cs1-cs0)/cs0*100<<endl<<flush;
  }
  */
}
