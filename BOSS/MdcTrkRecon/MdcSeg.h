//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSeg.h,v 1.11 2011/09/26 01:06:37 zhangy Exp $
//
// Description:
//       class to hold a superlayer track segment 
//  Takes ownership of Hots that it holds.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): 
//	Steve Schaffner
//	Zhang Yao(zhangyao@ihep.ac.cn)	Migrate to BESIII
//
//------------------------------------------------------------------------
// 
// Interface Dependencies ----------------------------------------------

#ifndef MDCSEG_H
#define MDCSEG_H
#include "MdcTrkRecon/GmsListLink.h"
#include "CLHEP/Alist/AList.h" 

class MdcSegParams;
class MdcLine;
class MdcSuperLayer;
class MdcHit;
class MdcSegInfo;
template <class K, class V> class MdcMap;
class MdcSegUsage;
class MdcHitUse;
class MdcHitMap;

// definitions for quality word
const unsigned segFullFlag = 4; // no missing hits
const unsigned segAmbigFlag = 2;
const unsigned segUsedFlag = 1; 

// Class definition
class MdcSeg : public GmsListLink
{
public:
  MdcSeg(double bunchT);
  virtual ~MdcSeg();
  double phi() const {return _phi;}
  double slope() const {return _slope;}
  double chisq() const {return _chisq;}
  unsigned int quality() const {return _qual;}
  const double * errmat() const {return _errmat;}
  const MdcSuperLayer *superlayer() const {return _slayer;}
  MdcSegInfo *info() const {return _info;}
  void setInfo(MdcSegInfo *ptr);
  void setAmbig() {_qual |= segAmbigFlag;}
  void setUsed() {_qual |= segUsedFlag;}
  void setFull() {_qual |= segFullFlag;}
  void setPattern(unsigned thePatt) {_pattern = thePatt;}
  int segAmbig() {return (_qual & segAmbigFlag);}
  int segUsed() {return (_qual & segUsedFlag);}
  int segFull() {return (_qual & segFullFlag);}
  unsigned segPattern() const {return _pattern;}
  double bunchTime() {return _bunchTime;}

  void plotSeg() const;
  void plotSegAll() const;

  void setValues(int nInPatt, int nhit, MdcHit *hits[], MdcLine *span, 
		 const MdcSuperLayer *slay, int ambig[]);
  void setValues(int nInPatt, double inPhi, double inSlope, 
		  double chi2, double inError[3], const MdcSuperLayer *slay);
  int addHits(MdcLine *span, MdcHit *hits[], const MdcHitMap*, double corr);
  void markHits(const MdcMap<const MdcHit*,MdcSegUsage*>& usedHits) const;

  void append(MdcHitUse*);
  void remove(MdcHitUse*);
  int nHit() const;
  MdcHitUse* hit(int i) const {return _theList[i];}

  static MdcSegParams *segPar() {return segParam;}
  static void setParam(MdcSegParams *segpar) {segParam = segpar;}
  double testCombSeg(const MdcSeg*)const;//yzhang temp
  double testCombSegPt()const;//yzhang temp
  double testCombSegTheta()const;//yzhang temp
  double testCombSegPhi()const;//yzhang temp
  double testCombSegAmbig()const;//yzhang temp
  // Preempt 
  MdcSeg& operator= (const MdcSeg&);
  MdcSeg(const MdcSeg &);

private:
  HepAList<MdcHitUse> _theList;
  static MdcSegParams *segParam;
  const MdcSuperLayer *_slayer;         
  double _phi;
  double _slope;
  double _errmat[3];  // error matrix: sig**2(phi), sig**2(phi-slope), (slope)
  double _chisq;
  unsigned int _qual;
  unsigned int _pattern;
  MdcSegInfo *_info;  // ptr to whatever approach-dependent info needed; kludge
  double _bunchTime;

  void reset();
};

#endif







