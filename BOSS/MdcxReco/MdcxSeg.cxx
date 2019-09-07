#include "MdcxReco/MdcxSeg.h"

MdcxSeg::MdcxSeg(MdcxFittedHel &hel, int p, int a):MdcxFittedHel(hel){
  pat=p; amb=a; 
//  c1=-0.686; c2=1.74;
  c1=-0.77; c2=1.83;
  if ((pat == 0x2244) || (pat == 0x4221)) {
    c1 = -1.13;
    c2 = 2.26;
  }
  if ((pat == 0x1244) || (pat == 0x8421) || (pat == 0x8221)) {
    c1 = -1.54;
    c2 = 2.84;
  }
  if ((pat == 0x2248) || (pat == 0x8441)) {
    c1 = -2.06;
    c2 = 2.99;
  }
}//endof MdcxSeg

MdcxSeg::~MdcxSeg( ){ }

void MdcxSeg::printSeg(){ 
  std::cout<<"sl "<< SuperLayer()<<" amb "<<amb<<" ";
  for (int i =0; i<Nhits(); i++){
    XHitList()[i]->print(std::cout,i);
  }
  std::cout<<" chi2 "<<Chisq()<<std::endl;
} 

void MdcxSeg::printSegAll(){ 
  std::cout<<"MdcxSeg "<< SuperLayer()<<" : ";
  for (int i =0; i<Nhits(); i++){
    XHitList()[i]->print(std::cout,i);
  }
  std::cout <<" pat "<<Pat()<<" amb "<<amb<< " d0 "<<d0<<" phi0 "<<phi0<<" omega "<<omega
    <<" d0Sl "<<D0_sl_approx()<<" phi0Sl "<< Phi0_sl_approx()
    <<" xB "<<Xline_bbrrf()<<" yB "<< Yline_bbrrf()
    <<" xSl "<<Xline_slope()<<" ySl "<< Yline_slope()
    <<" x(y)ref " << Xref() << " " << Yref()
    <<" origin "<<Origin()
    << std::endl; 
  std::cout<< std::endl;
} 
