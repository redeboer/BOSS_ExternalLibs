//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegGrouper.h,v 1.8 2017/04/10 07:48:39 zhangy Exp $
//
// Description:
//     Base class.  This set of classes creates lists (by superlayer) of 
//      track segments that are compatible with some hypothesis (currently, 
//      either a seed segment for axial segments or an existing axial 
//      track for stereo segments), along with the machinary to form 
//      candidate groups of segments.  The base class holds the lists and 
//      handles the combinatorics, including allowing gaps, and provides 
//      hooks for testing combinations on the fly.  
//
//      I don't know how it works, and I wrote it.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: Steve Schaffner
//
// Copyright (C)  1996  The Board of Trustees of  
// 
// History:
//	Migration for BESIII MDC
// The Leland Stanford Junior University.  All Rights Reserved.
//------------------------------------------------------------------------

#ifndef MDCSEGGROUPER_H
#define MDCSEGGROUPER_H

template <class T> class HepAList;
class MdcDetector;
class MdcSeg;
class MdcTrack;
class TrkExchangePar;
class TrkContext;

// Class interface //
class MdcSegGrouper {

public:
  virtual ~MdcSegGrouper();
protected:
  // constructor
  MdcSegGrouper(const MdcDetector *gm, int nDeep, int debug);

public:
  int nextGroup(MdcSeg **segGroup, bool printit);   
  int updateGap();
  void resetGap(int nGap);
  virtual void resetComb(const MdcSeg *seed) = 0;  
  virtual MdcTrack* storePar(MdcTrack* trk, double parms[2], double chisq,
			     TrkContext&, double trackT0) =0;   
  int nPly() const                                         {return nDeep;}
  
  // Tests for incompatible segments:
  virtual int incompWithSeg(const MdcSeg *refSeg, const MdcSeg *testSeg) = 0;
  virtual int incompWithGroup(MdcSeg **segGroup, const MdcSeg *testSeg, 
			      int iply) = 0;
  int combineSegs(MdcTrack*&, MdcSeg *seed, TrkContext&, double trackT0, 
		  double maxSegChisqO, int combineByFitHits=0); 
  void transferHits(MdcTrack *track, int nSegs, MdcSeg **segGroup);
  void dumpSegList();
  double calcParBySegs(MdcSeg **segGroup, double seedAngle[2], int nToUse, double& qual, int& nSegFit, double param[2]);//yzhang 2011-05-06 
  double calcParByHits(MdcSeg **segGroup, int nToUse, const TrkExchangePar &par,double& qual, int& nSegFit,  double param[2], double Bz);
  void setNoInner(bool noInnerFlag){_noInner = noInnerFlag;}

protected:	
  int nDeep;   //== maxPly; number of slayers to be combined
  int nPlyFilled;  //# of plies with non-null seg lists for this seed/axial trk
  int *currentSeg;   //(HepAList index)
  int *firstGood;   //(  "        ") permits skipping unrelated segs 
  int *firstBad;  
  bool **isValid;
  // isValid[iply][iseg] -- seg is compatible with seed (a seg may be between 
  //    firstGood and firstBad, and still be bad -- e.g. may not match in curv)

  HepAList<MdcSeg> **combList; 
  const MdcDetector *_gm; 
  HepAList<MdcSeg> *segList;  
  // segList[islayer]: holds list of segs for finding.  For axial slayers, 
  //  this is a list of all segs in the slayer, in phi0 order; for stereo, 
  //  it is a list only of ones compatible with current axial track
  //  For simplicity, array length is # slayers in chamber; ones in wrong 
  //  view(s) left empty
  // combList[iply]: current set of lists of segs.  For axial, does not 
  //  include list for seed slayer
  // The following all manage inclusion of gaps in group
  bool *leaveGap;   //leaveGap[ipy] => currently leaving out this ply
  int nNull;  // # plies currently left out of group (should start at zero)
  int maxNull;  // nNull <= maxNull
  int *gapCounter; 

  bool lTestGroup;  // check whether each seg is compat. with preceeding group
  bool lTestSingle; // chech whether compat. with seed

  int _debug;

  void resetSegCounters();
  bool _noInner;


private:
  // Preempt 
  MdcSegGrouper&   operator= (const MdcSegGrouper&);
  MdcSegGrouper(const MdcSegGrouper &);
};

#endif
