//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegGrouperSt.cxx,v 1.14 2011/09/26 01:06:37 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors:
// 	Zhang Yao(zhangyao@ihep.ac.cn)  Migrate to BESIII
//
// Copyright (C)  1996  The Board of Trustees of  
// The Leland Stanford Junior University.  All Rights Reserved.
//------------------------------------------------------------------------
#include "MdcGeom/BesAngle.h"
#include "MdcData/MdcHit.h"
#include "CLHEP/Alist/AList.h" 
#include "MdcGeom/MdcDetector.h"
#include "MdcGeom/MdcSuperLayer.h"
#include "MdcTrkRecon/MdcSeg.h"
#include "MdcTrkRecon/MdcSegGrouperSt.h"
#include "MdcTrkRecon/MdcSegList.h"
#include "MdcTrkRecon/MdcSegInfoSterO.h"
#include "MdcTrkRecon/MdcSegWorks.h"
#include "MdcTrkRecon/MdcSegUsage.h"
#include "MdcTrkRecon/MdcTrack.h"
#include "MdcTrkRecon/GmsList.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkFit.h"
#include "TrkFitter/TrkHelixMaker.h"

//yzhang hist cut
#include "AIDA/IHistogram1D.h"
extern AIDA::IHistogram1D*  g_z0Cut;
extern AIDA::IHistogram1D*  g_ctCut;
extern AIDA::IHistogram1D*  g_delCt;
extern AIDA::IHistogram1D*  g_delZ0;
//zhangy hist cut

//Constructor
//------------------------------------------------------------------------
MdcSegGrouperSt::MdcSegGrouperSt(const MdcDetector *gm, int debug) : 
  MdcSegGrouper(gm, gm->nStereoSuper(-1) + gm->nStereoSuper(1), debug) {
//------------------------------------------------------------------------

    lTestGroup = true;
    lTestSingle = false;

    isValid = new bool * [nPly()];
    for (int j = 0; j < nPly(); j++) {
      isValid[j] = 0;
    }
  }

//------------------------------------------------------------------------
void MdcSegGrouperSt::resetList() {
//------------------------------------------------------------------------
  // Clear existing lists for stereo finding
  int nsuper = _gm->nSuper();
  for (int i = 0; i < nsuper; i++) {
    segList[i].removeAll();
  }
}

//------------------------------------------------------------------------
void MdcSegGrouperSt::fillWithSegs(const MdcSegList *inSegs, 
    const MdcTrack *track) {
  //------------------------------------------------------------------------
  // Prepare for stereo finding 
  // Load segments consistent with input track (already in phi order)
  //  Assuming from origin
  int nsuper = _gm->nSuper();
  resetList();    // Clear existing lists 
  if(3==_debug) std::cout<<std::endl<< "=====MdcSegGrouperSt::fillWithSegs====="<<std::endl;

  const TrkFit* tkFit = track->track().fitResult();
  if (tkFit == 0) return;
  TrkExchangePar par = tkFit->helix(0.0);
  double kap = 0.5 * par.omega();
  double phi0 = par.phi0();
  double d0 = par.d0();
  MdcSegWorks segStuff;   // holds some calculated values to pass to segInfo

  bool lStraight = false;
  if (fabs(kap) < 1.e-9) { lStraight = true; }

  double rCurl = 99999999.;
  if (!lStraight) {
    rCurl = fabs(d0 + 1./kap);
  }

  // Create an info object to store the info (will be owned by seg)
  MdcSegInfoSterO *info = new MdcSegInfoSterO;

  // Loop over superlayers
  for (int isuper = 0; isuper < nsuper; isuper++) {
    const GmsList *inList = inSegs->oneList(isuper);

    if (inList->count() == 0) continue;

    MdcSeg *inSeg = (MdcSeg *) inList->first();
    // Only load stereo segments
    if (inSeg->superlayer()->whichView() == 0) continue;

    // Calculate r & phi (approx) of axial track at slayer
    double radius = inSeg->superlayer()->rEnd();  
    if (radius >= rCurl) break;
    double phiArg = kap * radius;
    if (lStraight) phiArg = d0 / radius;
    if (phiArg < -1.0) phiArg = -1.0;
    else if (phiArg > 1.0) phiArg = 1.0;
    segStuff.phiArg = phiArg;
    segStuff.phiAx.setRad(phi0 + asin(phiArg));  // Approx??!!!!
    BesAngle delPhi( fabs(inSeg->superlayer()->delPhi()) );
    //BesAngle maxPhiA = segStuff.phiAx + delPhi + 0.05;  // allow a little slop
    //BesAngle minPhiA = segStuff.phiAx - delPhi - 0.05;
    BesAngle maxPhiA = segStuff.phiAx + delPhi + 0.1;  // allow a little slop yzhang 2011-06-15 
    BesAngle minPhiA = segStuff.phiAx - delPhi - 0.1;//yzhang TEMP 2011-06-15 
    //yzhang FIXME
    double maxPhi = maxPhiA;
    double minPhi = minPhiA;
    bool phitest = (maxPhi > minPhi);
    //phitest = false => the valid range straddles phi = 0, so skip 
    //checking against phimin and phimax

    //Calc some things needed in segInfo::calcStereo
    segStuff.wirLen2inv = 1./ (inSeg->superlayer()->zEnd() * 
	inSeg->superlayer()->zEnd() );
    // Loop over segs in superlayer
    if(_debug==3) std::cout<<std::endl<<"Pick segment by phi from " <<minPhi<< " to "<<maxPhi<<" phiAx="<<segStuff.phiAx<<" delPhi="<<delPhi<<std::endl;
    for ( ; inSeg != 0; inSeg = (MdcSeg *) inSeg->next()) {
      // Test if within allowed phi range
      BesAngle phiSeg(inSeg->phi()); 

      if(_debug==3){ inSeg->plotSeg(); }
      if (phitest) {
	if (phiSeg < minPhi){
	  if (_debug ==3){std::cout << " CUT by phi "<<phiSeg
	    << "<min "<<minPhi << std::endl;}//yzhang debug	
	  continue;  // not up to candidates
	}
	else if(phiSeg > maxPhi) {
	  if (_debug ==3){std::cout << " CUT by phi "<<phiSeg
	    <<">max "<<maxPhi<< std::endl;}//yzhang debug	
	  break;  // past candidates
	}
      } else { // !phitest
	if (phiSeg > maxPhi && phiSeg < minPhi) {
	  if (_debug ==3){std::cout << "!phitest "<<phiSeg
	    <<" max "<<maxPhi<< " min "<<minPhi << std::endl;}//yzhang debug	
	  continue;
	}
      }

      if(_debug == 3) std::cout<<" **KEEP seg phi="<<phiSeg<<  std::endl;

      //std::cout<< __FILE__ << "   " << __LINE__ << " MdcSegGrouperSt::fillWithSegs call calcStereo "<<std::endl;
      int isBad = info->calcStereo(inSeg, track->track(), segStuff);  

      if (isBad) {
	if (_debug ==3){std::cout << " CUT by calcStereo isBad!"<<std::endl;}
	continue;
      }
      // Test for sensible values of ct and z0
      if (g_z0Cut) {g_z0Cut->fill(info->z0());}
      if (g_ctCut) {g_ctCut->fill(info->ct());}
      if (fabs(info->ct()) > inSeg->segPar()->ctcut) {
	if (_debug ==3){std::cout << " CUT by ctcut! "
	  <<fabs(info->ct())<<" > cut:"<< inSeg->segPar()->ctcut<<" "<<phiSeg<<std::endl; }
	continue;
      }
      if (fabs(info->z0()) > inSeg->segPar()->z0cut){
	if (_debug ==3){std::cout << " CUT by z0cut! "
	  <<fabs(info->z0())<<" >cut "<< inSeg->segPar()->z0cut<<" "<<phiSeg<<std::endl; }
	continue;
      }
      inSeg->setInfo(info);
      info = new MdcSegInfoSterO;      
      segList[isuper].append(inSeg);
      //if(_debug==3)std::cout<<" APPEND this stereo seg"<<  std::endl;

    }  // end loop over new segs
  }  //  end loop over superlayers
  delete info;
}

//-------------------------------------------------------------------------
void MdcSegGrouperSt::plotStereo( ) const {
  //-------------------------------------------------------------------------
  int nsuper = _gm->nSuper();
  int isuper;
  /*
     for (isuper = 0; isuper < nsuper; isuper++) {
     if (segList[isuper].length() == 0) continue;

     MdcSeg *inSeg = (MdcSeg *) segList[isuper].first();
  // Only draw stereo segments
  if (inSeg->superlayer()->whichView() == 0) continue;
  for (int j = 0 ; j < (int) segList[isuper].length(); j++) {
  segList[isuper][j]->plotSeg(0,orange);
  }
  }
  */
  for (isuper = 0; isuper < nsuper; isuper++) {
    if (segList[isuper].length() == 0) continue;

    MdcSeg *inSeg = (MdcSeg *) segList[isuper].first();
    // Only draw stereo segments
    //if (inSeg->superlayer()->whichView() == 0) continue;
    for (int j = 0 ; j < (int) segList[isuper].length(); j++) {
      segList[isuper][j]->plotSeg();
    }
  }

}
//-------------------------------------------------------------------------
int MdcSegGrouperSt::incompWithSeg(const MdcSeg *refSeg, 
    const MdcSeg *testSeg) {
  //-------------------------------------------------------------------------

  return 0;
}

//-------------------------------------------------------------------------
int MdcSegGrouperSt::incompWithGroup(MdcSeg **segGroup, const MdcSeg *testSeg, 
    int iply) {
  //-------------------------------------------------------------------------
  // Test that the latest seg lies more or less in a line with the others
  //  Currently requiring line to point to IP; also require rough 
  //  agreement in ct.
  // iply = depth index of current seg (not yet in group)

  int i;
  // Find first segment already in group
  for (i = iply - 1; i > 0; i--) {
    if (!leaveGap[i]) break;
  }
  const MdcSeg *first = (*combList[i])[currentSeg[i]];

  // Test ct
  MdcSegInfoSterO* firstInfo = (MdcSegInfoSterO *) first->info(); 
  MdcSegInfoSterO* newInfo = (MdcSegInfoSterO *) testSeg->info(); 

  if (g_delCt) {g_delCt->fill( firstInfo->ct() - newInfo->ct());}//yzhang hist cut
  if (fabs( firstInfo->ct() - newInfo->ct() ) > 
      testSeg->segPar()->delCtCut) {
    if(_debug==3){
      cout << "---MdcSegGrouperSt Ct CUT!" << endl;//yzhang debug
      std::cout<<"first:"; first->plotSeg(); std::cout<<std::endl;
      std::cout<<"test:"; testSeg->plotSeg();std::cout<<std::endl;
      std::cout << "first.ct "<< firstInfo->ct()
	<<" test.ct "<<newInfo->ct()
	<<" delta ct "<<firstInfo->ct() - newInfo->ct()
	<<" Cut "<<testSeg->segPar()->delCtCut << std::endl;//yzhang debug

      std::cout<<"--- "<<  std::endl;
    } 
    return -1;
  }else{
  }

  double arcFirst = firstInfo->arc();
  double arcNew = newInfo->arc();
  double zFirst = firstInfo->z0() + firstInfo->ct() * arcFirst;
  double zNew = newInfo->z0() + newInfo->ct() * arcNew; 
  // project line from IP through 1st seg to new seg
  double zProj = zFirst / arcFirst * arcNew;
  if (g_delZ0) {g_delZ0->fill( zProj - zNew);}//yzhang hist cut

  if (fabs(zProj - zNew) > testSeg->segPar()->delZ0Cut) {
    if(3==_debug){
      cout << "MdcSegGrouperSt delZ0Cut  not incompWithGroup CUT!" << endl;//yzhang debug 
      testSeg->plotSeg(); 
      std::cout<<" zProj "<< zProj << " zNew "<< zNew<< " CUT! "
	<< testSeg->segPar()->delZ0Cut <<  std::endl;
    }
    return -1;
  }

  /*
     double delZ = newInfo->z0() + newInfo->ct() * arcNew - zFirst;
     double z0 = zFirst - arcFirst * delZ / (arcNew - arcFirst);
     if (fabs(z0) > testSeg->segPar()->delZ0Cut)return -1;

     for (i = iply - 1; i > 0; i--) {
     if (segGroup[i] != 0) break;
     }
     const MdcSeg *last = segGroup[i];
     MdcSegInfoSterO* lastInfo = (MdcSegInfoSterO *) last->info(); 

  // Test that slope from last segment to new one is roughly = slope from 
  //   first seg to last
  double arcLast = lastInfo->arc();
  double arcFirst = firstInfo->arc();
  double arcNew = newInfo->arc();

  double delZold = lastInfo->z0() - firstInfo->z0() + 
  lastInfo->ct() * arcLast - firstInfo->ct() * arcFirst;
  double delArcold = arcLast - arcFirst;
  double delZnew = newInfo->z0() - lastInfo->z0() + 
  newInfo->ct() * arcNew - lastInfo->ct() * arcLast;
  double delArcnew = arcNew - arcLast;
  double p1 = delZold * delArcnew;
  double p2 = delZnew * delArcold;

  cout << " test: " << p1 << "  " << p2 << endl;

  if (fabs(p2 - p1) > 0.02) return -1;

*/
  return 0;
}

//**************************************************************************
void MdcSegGrouperSt::resetComb(const MdcSeg *seed) {
  //**************************************************************************

  // Delete existing list of valid/invalid segs
  if (isValid != 0) {
    int i;
    for (i = 0; i < nDeep; i++) {
      delete [] isValid[i];
      isValid[i] = 0;
    }
  }

  //Grab the seglist for each slayer
  int islay = 0;
  int iply = 0;
  nPlyFilled = 0;
  nNull = 0;


  // Set up all sorts of stuff for fast grouping of segs in nextGroup()
  for (const MdcSuperLayer *thisSlay = _gm->firstSlay(); thisSlay != 0;
      thisSlay = thisSlay->next()) {
    //bool noGoodYet = true;
    islay++;
    if (thisSlay->whichView() == 0) continue;
    firstGood[iply] = 0;
    firstBad[iply] = segList[islay-1].length();
    if (firstGood[iply] > firstBad[iply]) firstGood[iply] = firstBad[iply];
    if (firstGood[iply] == firstBad[iply]) {
      // If there are no valid segs for this ply, skip it
      continue;
    }
    // Associate correct seglist with this ply
    combList[iply] = &segList[islay-1];
    leaveGap[iply] = false;
    iply++;
  }

  nPlyFilled = iply;
  resetSegCounters();
  maxNull = nPlyFilled - 2;
}

//---------------------------------------------------------------------
MdcTrack* MdcSegGrouperSt::storePar(MdcTrack* trk, double parms[2], 
    double chisq, 
    TrkContext&, double) {
  //---------------------------------------------------------------------
  assert (trk != 0);
  TrkHelixMaker maker;
  if(3==_debug) std::cout<< "-----storePar z0 "<<parms[0]<<" ct "<<parms[1]<<std::endl;
  maker.addZValues(trk->track(), parms[0], parms[1], chisq);
  return trk;
}

