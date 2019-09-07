//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxFittedHel.h,v 1.5 2010/09/26 00:31:13 zhangy Exp $
//
// Description:
//	Class Header for |MdcxFittedHel| - helix fitting class
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	S. Wagner
//
// Copyright Information:
//	Copyright (C) 1995	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#ifndef _MDCXFITTEDHEL_
#define _MDCXFITTEDHEL_

//MdcxHel & MdcxFittedHel classes ...
#include <iostream>
#include <fstream>
#include "MdcxReco/MdcxHel.h"
#include "CLHEP/Alist/AList.h"

class MdcxHit;

//MdcxFittedHel follows: (fitted helix class) 
class MdcxFittedHel:public MdcxHel{
  public:
    //constructors
    MdcxFittedHel();

    //hits+initial guess constructor
    MdcxFittedHel(HepAList<MdcxHit> &XHitList, MdcxHel &hel, double Sfac=1.0);

    //destructor
    virtual ~MdcxFittedHel( );

    //accessors
    inline float Chisq()const{return chisq;}
    inline float Rcs()const{return rcs;}
    inline float Prob()const{return prob;} 
    inline float Fittime()const{return fittime;} 
    inline int Nhits()const {return nhits;}
    inline int Itofit()const {return itofit;}
    inline int Quality()const {return quality;}
    inline int Origin()const {return origin;}
    inline double Sfac()const {return sfac;}
    inline void SetQuality(const int &q) {quality=q;}
    inline void SetUsedOnHel(const int &i) {usedonhel=i;}
    inline int  GetUsedOnHel()const {return usedonhel;}
    int SuperLayer(int hitno=0)const; // return superlayer of |hitno
    int Layer(int hitno=0)const;	// return layer number of |hitno|
    inline const HepAList<MdcxHit> &XHitList() const { return xHitList; }

    //workers
    float Residual(int i);
    float Pull(int i);
    int Fail(float Probmin=0.0)const;
    int ReFit();
    int FitPrint();
    int FitPrint(MdcxHel &hel, std::ostream &o);
    void VaryRes();

    //operators
    MdcxFittedHel& operator=(const MdcxHel&);// copy helix to fitted helix
    MdcxFittedHel& operator=(const MdcxFittedHel&); // copy fitted helix to fitted helix
    MdcxFittedHel& Grow(const MdcxFittedHel&, const HepAList<MdcxHit> &);

    //workers

  protected:

    //data
    int fail;			//fit failure codes
    float chisq;			//chisq of hit
    float rcs;			//chisq per dof
    float prob;			//chisq prob of hit
    float fittime;		//fit time in clock time (machine dep)
    int nhits;			//number of hits
    int itofit;			//number of iterations to convergence
    int quality;			// bigger quality=>great purity
    int origin;			// origin "hit", -1 if none
    HepAList<MdcxHit> xHitList;	// list-of-hits making this |MdcxFittedHel|
    double sfac;                  // error scale factor for fit
    int usedonhel;

    //functions
    //fitting routine
    int DoFit();
    int IterateFit();

  private:

    //data

    //control
    void basics();
    void basics(const HepAList<MdcxHit> &);

    //check for included origin, if there move to end of |xHitList|
    //-1=>no origin; >=0 is hit number of origin
    int OriginIncluded();		// origin included

    //static control parameters
    int bailout;		// bailout if chisq/ndof too big?
    float chidofbail;	// bailout cutoff
    int niter;		// max number of iterations
    //static control sets
  public:
    static int debug;
    inline void SetBailOut(int i) {bailout=i;}
    inline void SetChiDofBail(float r) {chidofbail=r;}
    inline void SetNiter(int i) {niter=i;}

};// endof MdcxFittedHel

#endif

