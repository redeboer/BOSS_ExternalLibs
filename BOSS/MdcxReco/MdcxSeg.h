#ifndef MDCXSEG_H
#define MDCXSEG_H
#include <math.h>
#include <iostream>
#include <fstream>
#include "MdcxReco/MdcxFittedHel.h"
#include "MdcxReco/MdcxHit.h"

class MdcxSeg:public MdcxFittedHel{
 public:
  MdcxSeg(MdcxFittedHel &hel, int p, int a);
  virtual ~MdcxSeg( );
  inline double D0_sl_approx(){ return d0+c1*omega; }
  inline double Phi0_sl_approx(){ return phi0+c2*omega; }
  inline double Xline_bbrrf(){ return xref-sphi0*d0; }
  inline double Yline_bbrrf(){ return yref+cphi0*d0; }
  inline double Xline_slope(){ return xHitList[0]->wx()/xHitList[0]->wz(); }
  inline double Yline_slope(){ return xHitList[0]->wy()/xHitList[0]->wz(); }
  inline int Pat(){return pat;}
  void printSeg(); 
  void printSegAll(); 

 private:
  int pat;
  int amb;
  double c1,c2;
};//endof MdcxSeg

#endif
