// -------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxFittedHel.cxx,v 1.9 2010/09/26 00:31:13 zhangy Exp $
//
// Description:
//	Class Implementation for |MdcxFittedHel|
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	S. Wagner
//	Zhang Yao(zhangyao@ihep.ac.cn)  Migrate to BESIII
//
// Copyright Information:
//	Copyright (C) 1995	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#include <math.h>
#include "MdcxReco/Mdcxmatinv.h"
#include "MdcxReco/MdcxFittedHel.h"
#include "MdcxReco/MdcxHit.h"
#include "MdcxReco/Mdcxprobab.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
using std::cout;
using std::endl;
using std::ostream;

//extern AIDA::IHistogram1D*  g_fitOmega;
int MdcxFittedHel::debug = 0;

void MdcxFittedHel::basics() 
{nhits=0; itofit=0; fittime=0.0;
  prob=0.0; chisq=1000000.0; fail=1300; quality=0; origin=-1; usedonhel=0;
  bailout=1; chidofbail=1200.0; niter=10;
} // endof basics 

void MdcxFittedHel::basics(const HepAList<MdcxHit> &e) {
  basics();
  nhits=e.length();
  xHitList=e;
  origin=OriginIncluded();
} // endof basics

//constructors
MdcxFittedHel::MdcxFittedHel(){
  basics();
}

//points+guess
  MdcxFittedHel::MdcxFittedHel
(HepAList<MdcxHit> &XHitList, MdcxHel& hel, double Sfac)
{ 
  basics(XHitList);
  sfac=Sfac;
  *this=hel;
  fail=IterateFit();
}//endof MdcxFittedHel

//destructor
MdcxFittedHel::~MdcxFittedHel( ){ }//endof ~MdcxFittedHel

//operators
MdcxFittedHel& MdcxFittedHel::operator=(const MdcxHel& rhs){
  copy(rhs);
  return *this;
} //endof MdcxFittedHel::operator=

MdcxFittedHel& MdcxFittedHel::operator=(const MdcxFittedHel& rhs){
  // FIXME
  copy(rhs);
  fail=rhs.Fail();
  chisq=rhs.Chisq();
  rcs=rhs.Rcs();
  prob=rhs.Prob();
  fittime=rhs.Fittime();
  nhits=rhs.Nhits();
  itofit=rhs.Itofit();
  quality=rhs.Quality();
  origin=rhs.Origin();
  xHitList=rhs.XHitList();
  sfac=rhs.Sfac();               
  usedonhel=rhs.GetUsedOnHel();
  bailout=1; chidofbail=1200.0; niter=10;
  return *this;
}//endof MdcxFittedHel::operator=

MdcxFittedHel& MdcxFittedHel::Grow(const MdcxFittedHel& rhs, 
    const HepAList<MdcxHit> &ListOAdds) {
  copy(rhs);
  fail=rhs.Fail();
  chisq=rhs.Chisq();
  //rcs=rhs.Rcs();
  //prob=rhs.Prob();
  fittime=0.0;
  nhits=rhs.Nhits();
  itofit=0;
  quality=rhs.Quality();
  origin=rhs.Origin();
  xHitList=rhs.XHitList();
  sfac=rhs.Sfac();               
  usedonhel=rhs.GetUsedOnHel();
  bailout=1; chidofbail=1200.0; niter=10;
  int kkk=0; while (ListOAdds[kkk]){ListOAdds[kkk]->SetUsedOnHel(0); kkk++;}
  kkk=0; while (xHitList[kkk]){xHitList[kkk]->SetUsedOnHel(1); kkk++;}
  double spull;
  MdcxHel* temp = (MdcxHel*)(&rhs);
  kkk=0; while (ListOAdds[kkk]){
    if (ListOAdds[kkk]->GetUsedOnHel() == 0) {
      spull = ListOAdds[kkk]->pull(*temp)/sfac;
      chisq += spull*spull; 
      xHitList.append(ListOAdds[kkk]);
      nhits++; 
    }
    kkk++;
  }

  int ndof = nhits - nfree;
  prob = Mdcxprobab(ndof, chisq);
  rcs = chisq/ndof;
  return *this;
}//endof MdcxFittedHel::Grow

//accessors
float MdcxFittedHel::Residual(int i){
  //float pull=xHitList[i]->pull(*this);
  //float E=xHitList[i]->e();
  //return pull*E;
  return xHitList[i]->residual(*this);
}//endof Residual

float MdcxFittedHel::Pull(int i){
  return xHitList[i]->pull(*this);
}//endof Pulls

int MdcxFittedHel::Fail(float Probmin)const {
  if(fail) return fail;
  if(prob < Probmin) return 1303;
  // now done in DoFit  if(fabs(omega)>omegmax) {return 1306;}
  return 0;
} // endof Fail

//utilities&workers

void MdcxFittedHel::VaryRes() {
  int kbl = 0;
  while(xHitList[kbl]) xHitList[kbl++]->SetConstErr(0);
}

int MdcxFittedHel::ReFit(){
  fail = IterateFit();
  return fail;
}//endof ReFit

int MdcxFittedHel::IterateFit() {
  int ftemp = 1301; // not enough hits
  if (nfree > nhits) return ftemp;
  ftemp = 0;

  if(6 == debug) std::cout<<"IterateFit niter="<<niter<<  std::endl;
  if (niter >= 1) {
    float prevchisq = 0.0;
    for (int i = 0; i < niter; i++) {
      itofit = i + 1;
      ftemp = DoFit();
      if (6 == debug) {
	if (nfree == 5) {
	  cout << " iteration number= " << i  << " chisq= " << chisq;
	  cout << " nhits= " << nhits << " " << " fail= " << ftemp << endl;
	}
	print();
      }
      if (ftemp != 0) break;
      if(6 == debug)std::cout<<"in MdcxFittedHel::IterateFit() chisq="<<chisq<<" prechi2="<<prevchisq<<std::endl;//yzhang debug
      if ((fabs(chisq-prevchisq) < 0.01*chisq) || (chisq < 0.001)) break;  /// FIXME
      prevchisq = chisq;
    }//endof iter loop
  } else {
    float prevchisq = 0.0;
    chisq = 1000000.0;
    int iter = 0;
    while ((fabs(chisq-prevchisq) > 0.01) && (iter++ < 1000)) {
      prevchisq = chisq;
      ftemp = DoFit();
      if (ftemp != 0) break;
    }//endof (fabs(chisq-oldchisq).gt.0.01)
  }//endof (niter>=1)
  int ndof = nhits - nfree;
  prob = Mdcxprobab(ndof, chisq);
  rcs = chisq/ndof;
  return ftemp;
}//endof IterateFit

int MdcxFittedHel::DoFit() {
  int ftemp = 1301;
  // if(nfree>nhits) {return Fail;}
  if (nfree > nhits) return ftemp;
  double m_2pi = 2.0 * M_PI;
  ftemp = 0;
  //pointloop
  if (6 == debug) {
    std::cout << "in MdcxFittedHel::DoFit()  nfree = " << nfree
      << "  nhits = " << nhits << std::endl;
  }

  int norder = nfree;
  double A[10][10] = {{0.}}, B[10] = {0.}, D[10] = {0.}, det;
  chisq = 0.0;

  if (6 == debug) {
    std::cout << "xHitList.length " << xHitList.length() << "  ";
    for (int ii = 0; ii < xHitList.length(); ii++) {
      xHitList[ii]->print(std::cout, ii);
    }
    std::cout << std::endl << "sfac = " << sfac << std::endl;
  }

  for (int i = 0; i < nhits; i++) {
    std::vector<float> derivs = xHitList[i]->derivatives(*this);
    if (6 == debug) {
      cout << "derivs " << i<<" ";
      for (unsigned int ii = 0; ii < derivs.size(); ii++) {
	cout << setw(15)<< derivs[ii]; 
      }
      std::cout << std::endl;
    }
    if (sfac != 1.0) {
      for(unsigned int ipar = 0; ipar < derivs.size(); ipar++) {
	derivs[ipar] /= sfac;
	if(6 == debug) cout << " derivs[" << ipar << "] = " << derivs[ipar];
      }
      if(6 == debug) std::cout << std::endl;
    }
    chisq += derivs[0] * derivs[0];
    //outer parameter loop
    for (int ipar = 0; ipar < norder; ipar++) {
      D[ipar] += derivs[0] * derivs[ipar+1];
      //inner parameter loop
      for(int jpar = 0; jpar < norder; jpar++) {
	A[ipar][jpar] += derivs[ipar+1] * derivs[jpar+1];
      }//endof inner parameter loop
    }//endof outer parameter loop
  }//pointloop
  if (6 == debug) cout << "chisq = " << chisq << endl;
  if (chisq == 0 && nhits != 3) {  //zoujh: chisq is invalid??? FIXME
    ftemp = 1310;
    return ftemp;
  }
  if (6 == debug) {
    for (int ii = 0; ii < norder; ii++) {
      cout << "D["<< ii << "]: " << D[ii] << "     A:";
      for (int jj = 0; jj < norder; jj++) cout << "  " << A[ii][jj];
      cout << endl;
    }
  }
  //invert A
  int ierr;
  if (bailout) {
    ftemp = 1308;     // bailout
    int ndof = nhits - nfree;
    if (ndof > 0) {
      if (6 == debug){
	cout << "chisq " << chisq << " ndof " << ndof 
	  << " chiperdof " << chisq/ndof 
	  << " >?chidofbail " << chidofbail << endl;
      }
      float chiperdof = chisq/ndof;
      if(chiperdof > chidofbail) return ftemp;
    } else {
      if (6 == debug){
	cout << " ndof <=0 : chisq " << chisq 
	  << " >? chidofbail/2.5 " << chidofbail/2.5 << endl;
      }
      if (chisq > chidofbail/2.5) return ftemp;  //FIXME
    }
  } // (bailout)
  ftemp = 0;
  ierr = Mdcxmatinv(&A[0][0], &norder, &det);
  if (6 == debug) cout << "ierr = " << ierr << endl;
  if (ierr == 0) {
    for(int ii = 0; ii < norder; ii++)
      for(int jj = 0; jj < norder; jj++)
	B[ii] += A[ii][jj] * D[jj];
    if (6 == debug) {
      for (int ii = 0; ii < norder; ii++) {
	cout << "B[" << ii << "]: " << B[ii] << "     A:";
	for (int jj = 0; jj < norder; jj++) cout << "  " << A[ii][jj];
	cout << endl;
      }
    }
    int bump = -1;
    if (qd0)   d0 -= B[++bump];
    if (qphi0) {
      phi0 -= B[++bump]; 
      if (phi0 > M_PI)  phi0 -= m_2pi;
      if (phi0 < -M_PI) phi0 += m_2pi;
      cphi0 = cos(phi0); sphi0 = sin(phi0);
    }
    if (qomega) {
      omega -= B[++bump];
      ominfl = (fabs(omega) < omin) ? 0 : 1;
    }
    if (qz0)   z0   -= B[++bump];
    if (qtanl) tanl -= B[++bump];
    if (qt0)   t0   -= B[++bump];

    x0 = X0(); y0 = Y0(); xc = Xc(); yc = Yc();
    if ( fabs(d0) > MdcxParameters::maxMdcRadius )   ftemp = 1305; 
    //if(g_fitOmega)g_fitOmega->fill(omega);
    if ( fabs(omega) > 10.0 ) ftemp = 1306; // Too tight (r < 1 cm)//yzhang FIXME 2009-11-03 
  } else {
    ftemp = ierr;
  }
  return ftemp;
}//endof DoFit

//is origin included in fit ?
int MdcxFittedHel::OriginIncluded() {
  for(int i=0; xHitList[i]; i++) {
    int type=xHitList[i]->type();
    if(2==type) {		// origin "hit" ?
      //move to end, move fit point, return hit number
      xHitList.swap(i,nhits-1);
      return nhits-1;
    } 
  } 
  return -1;
}

int MdcxFittedHel::FitPrint() {
  cout << " d0 " << d0;
  cout << " phi0 " << phi0;
  cout << " omega " << omega;
  cout << " z0 " << z0;
  cout << " tanl " << tanl << endl;
  cout << " fail " << fail;
  cout << " chisq " << chisq;
  cout << " iter to fit " << itofit;
  cout << " sfac " << sfac;
  cout << " rcs " << rcs;
  cout << " prob " << prob;
  cout << " fittime " << fittime << endl;
  cout << " nhits= " << nhits << " xHitList.length " << xHitList.length() << endl;
  for (int ii = 0; ii < xHitList.length(); ii++) {
    xHitList[ii]->print(cout, ii);
  }
  cout<<endl;

  return 0;
}//endof FitPrint

int MdcxFittedHel::FitPrint(MdcxHel &hel, ostream &o) {
  double m_2pi=2.0*M_PI;
  double difphi0=phi0-hel.Phi0();
  if (difphi0>M_PI)difphi0-=m_2pi; if (difphi0<-M_PI)difphi0+=m_2pi; 
  cout << " difphi0= " << difphi0 << endl;
  cout << " difomega= " << omega-hel.Omega() << endl;
  cout << " difz0= " << z0-hel.Z0() << endl;
  cout << " diftanl= " << tanl-hel.Tanl() << endl;
  o << "FitPrint "; 
  o << "nhits "<< nhits << " fail " << fail << " chi2 " << chisq ;
  o << "rcs " << rcs << " prob " << prob <<endl;
  return 0;
}//endof FitPrint

//Find layer number of |hitno|
int MdcxFittedHel::Layer(int hitno)const {
  if(hitno >= nhits) return 0;
  return xHitList[hitno]->Layer();
} // endof Layer

//Find superlayer numbber of |hitno|
int MdcxFittedHel::SuperLayer(int hitno) const {
  if(hitno >= nhits) { return 0; }
  if(hitno < 0) { return 0; }
  return xHitList[hitno]->SuperLayer();
} // endof SuperLayer

