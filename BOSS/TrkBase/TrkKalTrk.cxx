//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkKalTrk.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//      class TrkKalTrk.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Copyright Information:
//	Copyright (C) 2000	Lawrence Berkeley Laboratory
//
// Author List:
//      Dave Brown 10/31/00
//------------------------------------------------------------------------

//#include "BaBar/BaBar.h"
#include "TrkBase/TrkKalTrk.h"
#include "TrkBase/TrkErrCode.h"
#include "MdcRecoUtil/PdtPid.h"
#include "MdcRecoUtil/Pdt.h"
#include <iostream>
using std::endl;
using std::ostream;

TrkKalTrk::TrkKalTrk()
{}

TrkKalTrk::~TrkKalTrk()
{}

void
TrkKalTrk::print(ostream& os) const {
  os << "TrkKalTrk for track " << trackId() << " fit with t0 = "
     << usedT0() << " with " << nSvt() << " Svt HOTs and "
     << nMdc() << " Mdc HOTs and " << nFit() << " Fit results "<< endl;
  for(unsigned ihypo=0;ihypo<PdtPid::nPidType;ihypo++){
    PdtPid::PidType hypo = (PdtPid::PidType)ihypo;
    TrkErrCode fitstat = fitStatus(hypo);
    os << "When stored, the fit for hypo " << Pdt::lookup(hypo)->name() 
       << " pointed to hypo " << Pdt::lookup(fitHypo(hypo))->name() << " and " 
       << (isValid(hypo) ? " was Valid, " : " was Not Valid, " )
       << (isCurrent(hypo) ? " was Current, " : " was Not Current, " )
       << (fitstat.success() ? " was Succesfully Fit. " : " was Not Successfully Fit. " )
       << endl;
  }
}

ostream& operator << (ostream& os,const TrkKalTrk& kaltrk) {
  kaltrk.print(os);
  return os;
}


