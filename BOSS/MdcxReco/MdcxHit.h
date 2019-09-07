//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxHit.h,v 1.10 2009/12/16 08:59:13 zhangy Exp $
//
// Description:
//	Class Header for |MdcxHit|: hit that can calculate derivatives
//      and plot hits self specialized to the drift chamber
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	A. Snyder
//	Zhang Yao(zhangyao@ihep.ac.cn) Migration for BESIII
//
// Copyright Information:
//	Copyright (C) 1995	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef _MDCXHIT_
#define _MDCXHIT_

#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "MdcGeom/MdcLayer.h"
#include "MdcGeom/MdcSWire.h"
#include "MdcData/MdcHit.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "MdcGeom/MdcDetector.h"

class MdcxHel;
class MdcDigi;

using namespace EventModel;

class MdcxHit {
  // A MDC hit that can calculate derivatives
public:
  //constructors
  MdcxHit(const MdcDigi *pdcdatum, float c0=0,  float cresol=.0180);
  MdcxHit(const MdcHit  *pdchhit,  float c0=0,  float cresol=.0180);
  
  //destructor
  virtual ~MdcxHit() { };
  //private copy constructor and oprator
private:
  MdcxHit(const MdcxHit&);
  MdcxHit& operator=(const MdcxHit&);
  bool operator==(const MdcxHit&) const;

public:
  static void setMdcCalibFunSvc(const MdcCalibFunSvc* calibSvc);
  static void setCountPropTime(bool countPropTime); //yzhang for csmc
  static void setMdcDetector(const MdcDetector* gm);
    
  //accessors
  inline const MdcDigi* getDigi()const {return _mdcDigi;}
  inline const MdcHit* getMdcHit()const {return _mdcHit;}
  inline int SuperLayer()const {return _superlayer;} //SuperLayer#
  inline int WireNo()const {return _wirenumber;} // Wire#
  inline int Layer()const {return _layernumber;}// layer#
  inline float t()const {return _t;} // drift time
  inline float x()const {return _x;} // x of wire
  inline float y()const {return _y;} // y of wire
  inline float xpos()const {return _xpos;}  
  inline float ypos()const {return _ypos;} 
  inline float xneg()const {return _xneg;} 
  inline float yneg()const {return _yneg;} 
  inline float wx()const {return _wx;} 
  inline float wy()const {return _wy;} 
  inline float wz()const {return _wz;} 
  inline float pw()const {return _pw;} 
  inline float phiMid()const {return _pw;} //yzhang add
  inline float stereo()const {return _s;} // stereo angle of wire
  inline float v()const {return _v;} // drift velocity
  inline int type()const {return _type;} // flags |MdcxHit|
  
  //workers
  inline void SetConstErr(int i) {_consterr = i;}
  ///////////////////////////////////////////
  float tcor(float zh=0.0, float tof=0.0, float tzero=0.0)const;
  float d(MdcxHel& hel)const; // |drift dist| (changes hel's internal state)
  float d(float zh=0.0, float tof=0.0, float tzero=0.0,
          int wamb=0, float eang=0.0)const; // |drift dist|
  //float tcor(float tof) const;
  //float d(int ambig = 0, double entranceAngle = 0) const;
  //float d(MdcxHel& hel, int ambig = 0, double entranceAngle = 0) const;
  ///////////////////////////

  float e(float dd = 0.0) const;  //drift error currently in use
  float pull(MdcxHel& hel)const;//Chisq contribution to fit
  float residual(MdcxHel &hel)const;	//residual of this hit
  std::vector<float> derivatives(MdcxHel& hel)const; //Derivatives, etc.
  void print(std::ostream &o,int i=0)const;	//print this hit
  void printAll(std::ostream &o,int i=0)const;	//print this hit
  inline void SetUsedOnHel(int i) {usedonhel=i;}
  inline int  GetUsedOnHel()const {return usedonhel;}

protected:
  //functions
  void process();

  //data
  unsigned _wirenumber;
  unsigned _layernumber;
  unsigned _superlayer;
  float _t;
  float _q;         
  unsigned _iTdc;
  unsigned _iAdc;   
  float _T0Walk;    //zoujh
  float _x;
  float _y;
  float _r;
  float _s;
  float _L;//length of this layer
  float _d;
  float _v;
  int   _type;//yzhang add 2009-11-03  
  float _e;
  float _xpos,_ypos,_xneg,_yneg;
  float _p,_sp,_cp;
  double _pw;
  double _wx,_wy,_wz;
  const MdcHit* _mdcHit; 
  const MdcDigi *_mdcDigi; //pointer to |MdcDigi| defining this hit
  int _consterr; 
  int usedonhel;
  float _c0; //accumulated global time offset; changes in t0 go here
  float _cresol;
private:
  double getSigma(float driftDist, int ambig = 0, double entrAngle = 0,
      double dipAngle = 0, double z = 0) const;
  static const MdcCalibFunSvc* m_mdcCalibFunSvc;  //pointer to MdcCalibFunSvc
  static const MdcDetector* m_gm;  //pointer to MdcDetector
  static bool m_countPropTime;   
};//endof MdcxHit

typedef ObjectVector<MdcxHit> MdcxHitCol;

#endif
