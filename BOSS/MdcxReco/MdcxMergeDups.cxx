//-------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxMergeDups.cxx,v 1.9 2011/12/08 06:52:29 zhangy Exp $
//
// Description: 
//	Class Implementation for K0s finding
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      S.  Wagner
//
// Copyright Information:
//	Copyright (C) 1997	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#include <math.h>
#include "MdcxReco/MdcxMergeDups.h"
#include "MdcxReco/MdcxHel.h"
#include "CLHEP/Alist/AIterator.h"
#include "MdcxReco/MdcxHit.h"
#include "MdcxReco/MdcxParameters.h"
using std::cout;
using std::endl;

MdcxMergeDups::MdcxMergeDups(HepAList<MdcxFittedHel> &trkl, int debug) {
  m_debug = (debug == 10);
  int iprt = 0;
  int ntracks = trkl.length();
  if (iprt) cout << "MdcxMergeDups called with " << ntracks << " tracks" << endl;
  double m_2pi = 2.0*M_PI;
  int k = 0;
  while(trkl[k]) trkl[k++]->SetUsedOnHel(0);

  if (ntracks > 1) {
    for (int i = 0; i < ntracks-1; i++) {
      MdcxFittedHel* iptr = trkl[i];
      int already_merged = 0;
      if (iptr->GetUsedOnHel()) {
	already_merged = trkl[i]->GetUsedOnHel();
	iptr = NewTrklist[already_merged-1];
      }
      for (int j = i+1; j < ntracks; j++) {
	if (trkl[j]->GetUsedOnHel()) continue;
	double omega1 = iptr->Omega();
	double omega2 = trkl[j]->Omega();
	double phi01  = iptr->Phi0();
	double phi02  = trkl[j]->Phi0();
	double d01    = iptr->D0();
	double d02    = trkl[j]->D0();
	double prodo  = omega1*omega2;
	if (m_debug) cout << "Try track [" << i << "] and [" << j << "],  prodo = " << prodo << endl;
	//  Try to merge pair that looks like duplicates (same charge)
	if (prodo > 0.0) {
	  if(m_debug) std::cout << "  fabs(d01 - d02)  " << fabs(d01 - d02) << std::endl;
	  if (fabs(d01 - d02) < MdcxParameters::maxDd0InMerge) {
	    if(m_debug) std::cout << "  fabs(phi01-phi02)  " << fabs(phi01-phi02) << std::endl;
	    if (fabs(phi01-phi02) < MdcxParameters::maxDphi0InMerge) {
	      double r1=100000.;
	      if (fabs(omega1)>0.00001) r1 = 1.0/fabs(omega1);
	      double r2=100000.;
	      if (fabs(omega2)>0.00001) r2 = 1.0/fabs(omega2); //FIXME
	      double pdrad = fabs((r1-r2)/(r1+r2)) ;
	      if (m_debug) {
		std::cout << "omega1,r1 " << omega1 << " " << r1
		          << " omega2,r2 " << omega2 << " " << r2
			  << " pdrad " << pdrad << std::endl;
	      }
	      if (pdrad < MdcxParameters::maxPdradInMerge) {
		if (iprt)
		  cout << "MdcxMD i j dif " << i << " " << j << " " << d01-d02 << " "
		       << phi01-phi02 << " " << r1 << " " << r2 << " " << pdrad << endl;
		HepAList<MdcxHit> dcxhlist = iptr->XHitList();
		if (iprt) cout << "MdcxMD " << dcxhlist.length() << " " << iptr->Chisq();
		const HepAList<MdcxHit>& dcxh2 = trkl[j]->XHitList();
		if (iprt) cout << " " << dcxh2.length() << " " << trkl[j]->Chisq();
		dcxhlist.append(dcxh2);
		dcxhlist.purge();
		if (iprt) cout << " " << dcxhlist.length() << endl;
		MdcxFittedHel fit1(dcxhlist, *iptr); // fit1.FitPrint(); fit1.print();
		MdcxFittedHel fit2(dcxhlist, *trkl[j]); // fit2.FitPrint(); fit2.print();
		int uf = 0; 
		if ( !fit1.Fail() && (fit1.Rcs()<MdcxParameters::maxRcsInMerge) ) uf = 1; 
		if ( !fit2.Fail() && (fit2.Rcs()<fit1.Rcs()) ) uf = 2;
		if (m_debug) {
		  std::cout << "fit1.Fail() " << fit1.Fail() << " fit1.Rcs " << fit1.Rcs()
		            << " fit2.Fail() " << fit2.Fail() << " fit2.Rcs " << fit2.Rcs()
		            << std::endl;
		}
		if (uf) {
		  MdcxHel fitme = (uf == 1) ? fit1 : fit2;
		  MdcxFittedHel* finehel = new MdcxFittedHel(dcxhlist, fitme);
		  if (!finehel->Fail()) {
		    if (already_merged) {
		      NewTrklist.replace(iptr, finehel);
		      delete iptr;
		      iptr = finehel;
		      trkl[j]->SetUsedOnHel(already_merged);
		    } else {
		      NewTrklist.append(finehel);
		      already_merged = NewTrklist.length();
		      iptr->SetUsedOnHel(already_merged);
		      iptr = finehel; 
		      trkl[j]->SetUsedOnHel(already_merged);
		    }
		  } else {
		    delete finehel;
		  }
		}
	      }
	    }
	  }
	}

	//  Try to merge pair that looks like albedo (opp charge, large d0)
	if (prodo < 0.0) {
	  if ((fabs(d01+d02) < 4.0) && (fabs(d01-d02) > 47.0)) {  /// FIXME
	    double deltap = fabs( fabs(phi01-phi02) - M_PI );
	    if (deltap < MdcxParameters::maxDphi0InMerge) {
	      double r1=100000.;
	      if (fabs(omega1) > 0.00001) r1 = 1.0/fabs(omega1);
	      double r2=100000.;
	      if (fabs(omega2) > 0.00001) r2 = 1.0/fabs(omega2);
	      double pdrad = fabs((r1-r2)/(r1+r2)) ;
	      if (pdrad < MdcxParameters::maxPdradInMerge) {
		if (iprt)
		  cout << "MdcxMD i j sum " << i << " " << j << " " << d01+d02 << " "
		       << deltap << " " << r1 << " " << r2 << " " << pdrad << endl;
		MdcxHel temp1 = *iptr;
		//zoujh?: temp1.SetTurnFlag(1);
		MdcxHel temp2 = *trkl[j];
		temp2.SetTurnFlag(1);
		HepAList<MdcxHit> dcxhlist = iptr->XHitList();
		if (iprt) cout << "MdcxMD " << dcxhlist.length() << " " << iptr->Chisq();
		const HepAList<MdcxHit>& dcxh2 = trkl[j]->XHitList();
		if (iprt) cout << " " << dcxh2.length() << " " << trkl[j]->Chisq();
		dcxhlist.append(dcxh2);
		dcxhlist.purge();
		if (iprt) cout << " " << dcxhlist.length() << endl;
		MdcxFittedHel fit1(dcxhlist, temp1); // fit1.FitPrint(); fit1.print();
		MdcxFittedHel fit2(dcxhlist, temp2); // fit2.FitPrint(); fit2.print();
		int uf = 0; 
		if ( !fit1.Fail() && (fit1.Rcs()<MdcxParameters::maxRcsInMerge) ) uf = 1; 
		if ( !fit2.Fail() && (fit2.Rcs()<fit1.Rcs()) ) uf = 2;
		if (uf) {
		  MdcxHel fitme = (1 == uf) ? fit1 : fit2;
		  MdcxFittedHel* finehel = new MdcxFittedHel(dcxhlist, fitme);
		  if (!finehel->Fail()) {
		    if (already_merged) {
		      NewTrklist.replace(iptr, finehel);
		      delete iptr;
		      iptr = finehel;
		      trkl[j]->SetUsedOnHel(already_merged);
		    } else {
		      NewTrklist.append(finehel);
		      already_merged = NewTrklist.length();
		      iptr->SetUsedOnHel(already_merged);
		      iptr = finehel; 
		      trkl[j]->SetUsedOnHel(already_merged);
		    }
		  } else {
		    delete finehel;
		  }
		} 
	      }
	    }
	  }
	}
      }//end j loop
    }//end i loop
  }

  k = 0;
  while (trkl[k]) {
    if (iprt)cout << "In MdcxMD, trk is used on " << k << " " << trkl[k]->GetUsedOnHel() << endl;
    if (!trkl[k]->GetUsedOnHel()) CleanTrklist.append(trkl[k]); 
    k++;
  }

  k=0;
  while (NewTrklist[k]) {
    if (iprt && m_debug) {
      NewTrklist[k]->FitPrint();
      NewTrklist[k]->print();
    } 

    CleanTrklist.append(NewTrklist[k++]);
  }

  if (iprt) cout << "MdcxMD leaves with " << CleanTrklist.length() << " tracks" << endl;
}

MdcxMergeDups::~MdcxMergeDups() {
  KillList();
}

