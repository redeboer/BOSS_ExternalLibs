//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: ChisqConsistency.h,v 1.1.1.1 2005/04/21 01:17:17 zhangy Exp $
//
// Description:
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Bob Jacobsen, Ed Iskander
//
// Copyright Information:
//	Copyright (C) 1996
//
//------------------------------------------------------------------------

#ifndef CHISQCONSISTENCY_HH
#define CHISQCONSISTENCY_HH
//#include "BaBar/BaBar.hh"

//-----------------
// BaBar Headers --
//-----------------
#include "ProbTools/Consistency.h"

class ChisqConsistency : public Consistency {
public:

// default constructor; sets internal state to noMeasure
  ChisqConsistency();
// real constructor
  ChisqConsistency( double chisq, double nDof );
// construct directly from consistency value (chisq will be
// computed from that).  Note this only works for _integral_ DOFs.
// I have to reverse the argument order to keep the implicit casting
// from confusing this with the above
  ChisqConsistency( unsigned nDof, double consistency);
// copy and equivalence
  ChisqConsistency(const ChisqConsistency&);
  ChisqConsistency& operator = (const ChisqConsistency&);
  

  virtual ~ChisqConsistency() {}
// accessors
  const double& chisqValue() const { return _chisq; }
  const double& nDOF() const { return _nDof; }

protected:
  double _chisq;
  double _nDof;
};

#endif
