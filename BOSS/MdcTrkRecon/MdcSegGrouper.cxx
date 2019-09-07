//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegGrouper.cxx,v 1.18 2017/02/22 05:53:33 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors:
//	 Zhang Yao(zhangyao@ihep.ac.cn)  Migrate to BESIII
//
// Copyright (C)  1996  The Board of Trustees of  
// The Leland Stanford Junior University.  All Rights Reserved.
//------------------------------------------------------------------------
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkRecoTrk.h"
#include "MdcTrkRecon/MdcSegGrouper.h"
#include "MdcTrkRecon/mdcWrapAng.h"
#include "MdcTrkRecon/MdcSegInfo.h"  
#include "MdcTrkRecon/MdcSeg.h"  
#include "MdcTrkRecon/mdcTwoVec.h"
#include "MdcTrkRecon/mdcTwoInv.h"
#include "MdcTrkRecon/MdcTrack.h"
#include "MdcData/MdcHitUse.h"
#include "MdcGeom/MdcDetector.h"
#include "MdcData/MdcHit.h"
#include "CLHEP/Alist/AList.h" 
#include "TrkBase/TrkRep.h"
#include "MdcGeom/Constants.h"
#include "MdcTrkRecon/MdcSegInfoSterO.h"//yzhang 2011-05-09 
#include "MdcTrkRecon/MdcLine.h"//yzhang 2011-05-09 
#include "BField/BField.h"//yzhang 2011-05-09 

//yzhang hist cut
#include "AIDA/IHistogram1D.h"
extern AIDA::IHistogram1D* g_maxSegChisqAx;
extern AIDA::IHistogram1D* g_maxSegChisqSt;
//yzhang hist cut
using std::cout;
using std::endl;

//------------------------------------------------------------------------
MdcSegGrouper::~MdcSegGrouper() {
  //------------------------------------------------------------------------
  delete [] segList;
  delete [] combList;
  delete [] currentSeg;
  delete [] leaveGap;
  delete [] gapCounter;
  delete [] firstGood;
  delete [] firstBad;
  if (isValid != 0) {
    for (int i = 0; i < nDeep; i++) {
      delete [] isValid[i];
    }
    delete [] isValid;
  }

}
//------------------------------------------------------------------------
MdcSegGrouper::MdcSegGrouper(const MdcDetector *gm, int nd, int debug) {
  //------------------------------------------------------------------------
  nDeep = nd;
  int nsuper = gm->nSuper();
  segList = new HepAList<MdcSeg>[nsuper];
  currentSeg = new int[nDeep];  
  leaveGap = new bool[nDeep];
  gapCounter = new int[nDeep];
  combList = new HepAList<MdcSeg> * [nDeep]; 
  _gm = gm; 
  firstGood = new int[nDeep];  
  firstBad = new int[nDeep];
  lTestGroup = false;
  lTestSingle = false;
  _debug = debug;
  _noInner = false;
}



//------------------------------------------------------------------------
int MdcSegGrouper::nextGroup( MdcSeg **segGroup, bool printit ) {
  //------------------------------------------------------------------------

  // Loop over the superlayers, moving to next valid seg for each if necessary
  // First, loop over the slayers w/o good segs, filling segGroup w/ 0
  int iply;
  for (iply = nPlyFilled; iply < nDeep; iply++) {
    segGroup[iply] = 0;
  }

restart:
  if (printit) cout <<endl<< "MdcSegGrouper::nextGroup starting group finder, nply = " << nPlyFilled << endl;
  int nFound = 0;
  bool incrementNext = true;
  //int nSegUsed;//yzhang 2010-05-21 
  for (iply = 0; iply < nPlyFilled; iply++) {
    segGroup[iply] = 0;
    if (!incrementNext && currentSeg[iply] >= firstGood[iply]) {
      if(printit) std::cout<< " reach end of list, break." << iply<< std::endl;
      break;
    }
    //if (nSegUsed > segPar.nSegUsedNextGroup) break;
    if (leaveGap[iply]) {
      if(printit) std::cout<< "  leaveGap  " <<std::endl;
      // This ply is currently a gap; move on.
      if (iply == nPlyFilled - 1 && incrementNext) {
	// we've exhausted this gap group; start another
	iply = -1;
	resetSegCounters();
	int lDone = updateGap();
	if (lDone) {
	  // all gap groups for nNull exhausted; increment nNull
	  nNull++;
	  if (nNull > maxNull) return 0;  // All done 
	  resetGap(nNull);
	  updateGap();
	} // end if lDone
      }  //end if exhausted gap group
      continue;
    }
    incrementNext = false;

    // Loop through the segs in this ply until valid one found
    while (1) {
      currentSeg[iply]++;
      if (currentSeg[iply] == firstBad[iply]) {   // reached end of segs
	incrementNext = true;
	currentSeg[iply] = firstGood[iply];
	if(printit) { std::cout<< "reach end of segs  "<<std::endl; }
	if (iply == nPlyFilled - 1) {  
	  // we've exhausted this gap group; start another
	  iply = -1;
	  resetSegCounters();
	  int lDone = updateGap();
	  if (lDone) {
	    // all gap groups for nNull exhausted; increment nNull
	    nNull++;
	    if (nNull > maxNull) {
	      if(printit) { std::cout<<endl<< " All done!  "<<std::endl; }
	      return 0;  // All done
	    }
	    resetGap(nNull);
	    updateGap();
	  } // end if lDone
	}  //end if exhausted gap group
	break;
      }  // end reached end of segs

      if(printit){
	std::cout<< "ply " << iply<< " seg "<<currentSeg[iply]<<": ";
	(*combList[iply])[currentSeg[iply]]->plotSeg();
	if((*combList[iply])[currentSeg[iply]]->superlayer()->whichView()!= 0){
	  MdcSegInfoSterO* info = (MdcSegInfoSterO *) (*combList[iply])[currentSeg[iply]]->info();
	  std::cout<< " ct " << info->ct();
	}
      }

      //yzhang 09-09-28 delete
      if( (*combList[iply])[currentSeg[iply]]->segUsed()) {
	if(printit) {
	  std::cout<< std::endl<<" Used??";
	  (*combList[iply])[currentSeg[iply]]->plotSeg();
	}
	if(printit) { std::cout<< " segUsed! SKIP "<<std::endl; }
	continue;  //yzhang 2010-05-21 add
	//nSegUsed++;
      }

      // Test this seg for validity
      if (lTestSingle) {
	assert(isValid != 0);
	assert(isValid[iply] != 0);
	int invalid = (isValid[iply][currentSeg[iply]] == false);
	if(printit) {
	  if(invalid){ std::cout<<" invalid "<< std::endl;
	  }else{ std::cout<<" KEEP 1 "<< std::endl; }
	}
	if (invalid) continue;
      }else{
	if(printit) std::cout<<" KEEP 2 "<< std::endl; 
      }

      // Whew.  We successfully incremented.  
      break;

    }  // end seg loop
  } // end ply loop

  // Fill segGroup with appropriate segs
  for (iply = 0; iply < nPlyFilled; iply++) {
    if (leaveGap[iply]) {
      segGroup[iply] = 0;
    } else {
      segGroup[iply] = (*combList[iply])[currentSeg[iply]];	
      if (lTestGroup && nFound > 1) {
	int lBad = incompWithGroup(segGroup, segGroup[iply], iply);
	if(printit){
	  if(lBad) std::cout<<" incompWithGroup Bad! restart" << std::endl;
	  //else std::cout<<" incompWithGroup Bad! restart" << std::endl;
	}
	if (lBad) goto restart;
      }
      nFound++;
    }
  }

  if (printit) std::cout<< "nextGoup() found " << nFound << " segment(s)"<<std::endl;
  return nFound;
}

//**************************************************************************
void MdcSegGrouper::resetGap(int nGap) {
  //**************************************************************************

  for (int i = 0; i < nPlyFilled; i++) {
    gapCounter[i] = nGap - 1 - i;
  }
  gapCounter[0]--; // so 1st increment will put 1st counter in right place

  return;
}

//**************************************************************************
int MdcSegGrouper::updateGap() {
  //**************************************************************************
  if (nNull == 0) return 1;

  for (int i = 0; i < nPlyFilled; i++) {
    leaveGap[i] = false;
  }
  for (int igap = 0; igap < nNull; igap++) {
    gapCounter[igap]++;
    if (gapCounter[igap] == nPlyFilled - igap) {
      // End of loop for this counter; look at the other counters to 
      //  decide where this one should be reset to.
      int inext = igap + 1;
      while (1) {
	if (inext >= nNull) return 1; // done with all combos
	if (gapCounter[inext] + inext + 1 < nPlyFilled) {
	  // This is the right spot to reset to
	  gapCounter[igap] = gapCounter[inext] + inext + 1 - igap;
	  break;
	}
	inext++;
      }	
    }
    else {
      // We successfully incremented.  Quit looping and return.
      break;
    }
  }  // end loop over igap

  for (int j = 0; j < nNull; j++) {
    leaveGap[gapCounter[j]] = true;
  }
  return 0;

}
//-------------------------------------------------------------------------
void MdcSegGrouper::resetSegCounters() {
  //-------------------------------------------------------------------------
  for (int i = 0; i < nPlyFilled; i++) {
    currentSeg[i] = firstGood[i] - 1;
  }
}

//************************************************************************/
int MdcSegGrouper::combineSegs(MdcTrack*& trk, MdcSeg *seed, 
    TrkContext& context, double t0, double maxSegChisqO, int combineByFitHits) {
  //************************************************************************/
  // forms track from list of segs; does 2-param fit (either r-phi from origin
  //  or s-z) and picks best combination.
  if (3 == _debug) std::cout<<std::endl<<  "=====MdcSegGrouper::combineSegs=====" <<std::endl;
  bool lSeed = (seed != 0); // no seed for stereo group

  int success = 0;
  double qual;
  double qualBest = -1000.;
  int nSegFit = 0;
  int nSegBest = 0;
  //int nHitBest = 0;
  double param[2];
  double paramBest[2];
  double chiBest = 9999.;
  int nToUse = nPly();
  if (lSeed) nToUse++;   // seed isn't included in the segs list
  MdcSeg **segGroup;
  MdcSeg **segGroupBest;
  segGroup = new MdcSeg * [nToUse];
  segGroupBest = new MdcSeg * [nToUse];
  //  static int counter = 0;
  //  counter++;
  //  cout << counter << endl;

  //bool is3d = (seed==NULL);//zhange TEMP test 2011-05-06 

  // Loop over all combinations of segs consistent with seed (including gaps)
  if ((3 == _debug)&&lSeed) {
    std::cout<<"seed segment: ";
    seed->plotSegAll();
    std::cout<< std::endl;
  }
  resetComb(seed); 

  // Save seed params (if angles) for later use as reference angle in 
  //    mdcWrapAng (don't really have to test whether it's an angle, but I do)
  double seedAngle[2] = {0.,0.};
  if (lSeed) {
    if (seed->info()->parIsAngle(0)) seedAngle[0] = seed->info()->par(0);
    if (seed->info()->parIsAngle(1)) seedAngle[1] = seed->info()->par(1);
  }

  int iprint = ( 3 == _debug);
  int nInGroup = 0;
  while ( (nInGroup = nextGroup(segGroup, iprint)) != 0) {

    if (lSeed) {
      segGroup[nToUse-1] = seed;
      nInGroup++;
    }
    if (nInGroup < 0) continue;
    if (!_noInner && nInGroup < 2) break;
    if (nInGroup < nSegBest) break;

    if (3 == _debug || 11 == _debug) {
      cout << endl <<"-----found a segment group by nextGroup()----- nInGroup "<<nInGroup<<" nToUse "<<nToUse<<endl;
      for(int ii=0; ii<nToUse; ii++){ if(segGroup[ii]) {segGroup[ii]->plotSegAll(); cout<<endl;} }
      //cout << endl <<"--calc. parameters of this segment group"<<endl;
    }

    double chisq =-999.;
    nSegFit=0;

    if(lSeed){
      //2d
      chisq = calcParBySegs(segGroup, seedAngle, nToUse, qual, nSegFit, param);
    }else{
      if (combineByFitHits == 0){
	chisq = calcParBySegs(segGroup, seedAngle, nToUse, qual, nSegFit, param);
      }else{
	//3d 
	const TrkFit* tkFit = trk->track().fitResult();
	double Bz = trk->track().bField().bFieldZ();//??
	TrkExchangePar par = tkFit->helix(0.0);
	//par.print(std::cout);
	if (tkFit != 0) chisq = calcParByHits(segGroup, nToUse, par, qual,nSegFit, param, Bz);
	//std::cout<< __FILE__ << "   " << __LINE__ << " calcParByHits"<<std::endl;
	if(chisq<=0){
	  //std::cout<< "calcParByHits failed! calc. by seg" <<std::endl;
	  chisq = calcParBySegs(segGroup, seedAngle, nToUse, qual, nSegFit, param);
	}
      }
    }

    if (chisq < 0.) continue;//yzhang add
    //yzhang 2016-10-12 
    //double chiDof = chisq/(2.*nSegFit - 2.);
    double chiDof;
    if(_noInner)  chiDof = chisq;
    else chiDof = chisq/(2.*nSegFit - 2.);

    if (g_maxSegChisqAx && lSeed ) { g_maxSegChisqAx->fill(chiDof); } //yzhang hist cut
    if (g_maxSegChisqSt && !lSeed) { g_maxSegChisqSt->fill(chiDof); } //yzhang hist cut

    //std::cout<<  " chisq  " << chisq<< " chi2dof "<<chiDof<<std::endl;

    if (3 == _debug || 11 == _debug) {
      if(_noInner) std::cout<<endl<<" no inner chiDof = chisq "<<endl;
      std::cout<< endl<<"chisq "<<chisq<<" nSegFit " << nSegFit<< " chiDof "<<chiDof<<std::endl;
      if(chiDof > maxSegChisqO) {
	cout << "***** DROP this group by chiDof "<<chiDof<<" > maxSegChisqO:"<<maxSegChisqO<<endl;
      }else{
	cout << "***** KEEP this group by chiDof "<<chiDof<<" <= maxSegChisqO:"<<maxSegChisqO<<endl;
      }
    }
    // Chisq test
    if (chiDof > maxSegChisqO) continue;

    success = 1;
    if (qual > qualBest) {
      qualBest = qual;
      nSegBest = nSegFit;
      //nHitBest = nhit;
      paramBest[0] = param[0];
      paramBest[1] = param[1];
      chiBest = chisq;
      for (int i = 0; i < nToUse; i++) {
	segGroupBest[i] = segGroup[i];
      }
      if (3 == _debug && 11 == _debug) std::cout<<"Keep as best group. param: phi0/z0 "
	<<paramBest[0]<< " cpa/ct "<<paramBest[1]<<  std::endl;
    }// end test on qual
  }

  if (success == 1) {
    if(3 == _debug || 11 == _debug) {
      std::cout<< endl<<"-----Parameter best group-----    "<<std::endl;
      std::cout<< "paramBest "<<paramBest[0]<<" "<<paramBest[1]<< " chiBest " << chiBest<< std::endl;
    }
    // Store the results in a track, possibly creating it in the process
    trk = storePar(trk, paramBest, chiBest, context, t0);
    transferHits(trk, nToUse, segGroupBest);     // Store hits with track
  }
  delete [] segGroupBest;
  delete [] segGroup;
  return success;
} 

//************************************************************************
void MdcSegGrouper::transferHits(MdcTrack *trk, int nSegs, MdcSeg **segGroup) {
  //************************************************************************/
  //Move hits from segments to track hitlist
  // Also note first and last layers in list
  // Only handles Mdc segments
  double smallRad = 1000.;
  if (trk->firstLayer() != 0) smallRad = trk->firstLayer()->rMid();
  double bigRad = 0.;
  if (trk->lastLayer() != 0) bigRad = trk->lastLayer()->rMid();

  //yzhang 2011-05-04 
  //bool maintainAmbiguity = (trk->track().status()->is2d() == 0) ? false: true;
  //bool maintainAmbiguity = false;
  //std::cout<< __FILE__ << "   " << __LINE__ << " maintainAmbiguity "<<maintainAmbiguity<<std::endl;

  if(3 == _debug) std::cout<< endl<<"-----transferHits for this segGroup-----  " <<std::endl;
  for (int i = 0; i < nSegs; i++) {
    if (segGroup[i] == 0) continue;   // skipping this slayer
    if(3 == _debug) {
      cout << i << "  "; segGroup[i]->plotSegAll(); cout<< endl;
    }
    segGroup[i]->setUsed();  // mark seg as used
    for (int ihit = 0; ihit < segGroup[i]->nHit(); ihit++) {
      MdcHitUse *aHit = segGroup[i]->hit(ihit);
      const MdcLayer *layer = aHit->mdcHit()->layer();
      double radius = layer->rMid();
      if (radius < smallRad) {
	smallRad = radius;
	trk->setFirstLayer(layer);
      }

      // Assume that segs aren't added to backside of curler
      if (radius > bigRad && !trk->hasCurled()) {
	bigRad = radius;
	trk->setLastLayer(layer);
      }
      // Provide very crude starting guess of flightlength
      double flt = radius;
      flt += 0.000001 * (aHit->mdcHit()->x() +aHit->mdcHit()->y());

      aHit->setFltLen(flt);

      TrkHitList* theHits = trk->track().hits();

      if (theHits == 0) return;

      //theHits->appendHit(*aHit, maintainAmbiguity);
      theHits->appendHit(*aHit);
      //zhangy

      //std::cout<<"in MdcSegGrouper  append ok"<<std::endl;//yzhang debug
    }	
  }  // end loop over slayers
} 

//************************************************************************
void MdcSegGrouper::dumpSegList(){
  //************************************************************************
  for(int islayer=0; islayer<11; islayer++){
    for(int i=0; i<segList[islayer].length(); i++){
      segList[islayer][i]->plotSegAll();
      std::cout<< std::endl;
    }
  }
}

//************************************************************************
double MdcSegGrouper::calcParBySegs(MdcSeg **segGroup, double seedAngle[2], 
    int nToUse, double& qual, int& nSegFit, double param[2]) {
  //************************************************************************
  if (11 == _debug) std::cout<< "-----calculate group param by segment param-----"<<std::endl;
  double wgtmat[3], wgtinv[3];
  double wgtpar[2];
  double temvec[2], diff[2];
  // Calculate track & chisq for this group 
  int nhit = 0;
  wgtmat[0] = wgtmat[1] = wgtmat[2] = wgtpar[0] = wgtpar[1] = 0.0;

  int iPly;
  for (iPly = 0; iPly < nToUse; iPly++) {
    if (11 == _debug) {
      //if (!lSeed) //if (segGroup[iPly] == 0) cout << "ply empty: " << iPly << "\n";
    }
    if (segGroup[iPly] == 0) continue;   // skipping this slayer
    nSegFit++;
    MdcSegInfo *segInfo = segGroup[iPly]->info();
    // Accumulate sums
    for (int i = 0; i < 3; i++) wgtmat[i] += (segInfo->inverr())[i];
    for (int k = 0; k < 2; k++) {
      param[k] = segInfo->par(k);
      //zhangy add
      if (segInfo->parIsAngle(k)) {
	param[k] = mdcWrapAng(seedAngle[k], param[k]);
      }
    }
    // Multiply by weight matrix.
    mdcTwoVec( segInfo->inverr(), param, temvec );
    wgtpar[0] += temvec[0];
    wgtpar[1] += temvec[1];
    if(11 == _debug) {
      std::cout<<0<<": param "<<param[0]<<" inverr "<< segInfo->inverr()[0]<<" par*W "<<temvec[0]<<std::endl;
      std::cout<<1<<": param "<<param[1]<<" "<<1/param[1]<<" inverr "<< segInfo->inverr()[1]<<" par*W "<<temvec[1]<<std::endl;
      std::cout<< "   " <<std::endl;
    }
    nhit += segGroup[iPly]->nHit();
  }

  // And the fitted parameters are . . . 
  int error = mdcTwoInv(wgtmat,wgtinv);
  if (error && (11 == _debug)) {
    cout << "ErrMsg(warning) " 
      <<  "failed matrix inversion in MdcTrackList::combineSegs" << endl;
    //continue;
    return -999.;
  }
  mdcTwoVec( wgtinv, wgtpar, param );
  if(11 == _debug) {
    std::cout<< " param of wgtinv * wgtpar" << std::endl;
    std::cout<<0<<": param "<<param[0]<< std::endl;
    std::cout<<1<<": param "<<param[1]<<" "<<1/param[1]<< std::endl;
    std::cout<< "   " <<std::endl;
  }

  //param[0]= 5.312286;
  //param[1]= -0.006;
  //std::cout<< "set param   " <<param[0]<< "   "<<param[1]<<std::endl;
  if(11 == _debug)cout<<endl<<"-- Calculate track & chisq for this group "<<endl;

  // Calc. chisq. = sum( (Vi - V0) * W * (Vi - V0) ) 
  // W = weight, Vi = measurement, V0 = fitted param. 
  double chisq = 0.0;
  for (iPly = 0; iPly < nToUse; iPly++) {
    if (segGroup[iPly] == 0) continue;   // skipping this slayer
    MdcSegInfo *segInfo = segGroup[iPly]->info();
    for (int j = 0; j < 2; j++) {
      double temPar;
      if (segInfo->parIsAngle(j)) {
	temPar = mdcWrapAng(seedAngle[j], segInfo->par(j));
      } else {
	temPar = segInfo->par(j);
      }
      if(11 == _debug) {
	std::cout<<" segPar"<<j<<" "<<temPar<<  std::endl;
      }
      diff[j] = temPar - param[j];
    }

    if(11 == _debug) {
      std::cout<<"inverr " <<segInfo->inverr()[0]<<" "
	<<segInfo->inverr()[1] <<" "<<segInfo->inverr()[2] <<  std::endl;
      std::cout<<"errmat " <<segInfo->errmat()[0]<< " "
	<<segInfo->errmat()[1] << " "<<segInfo->errmat()[2] <<  std::endl;
      std::cout<<"diff " <<diff[0]<<" "<<diff[1]<<" temvec "<<temvec[0]<<" "<<temvec[1]<< std::endl;
      std::cout<<  std::endl;
    }
    mdcTwoVec( segInfo->inverr(), diff, temvec);

    chisq += diff[0] * temvec[0] + diff[1] * temvec[1];

    if(11 == _debug){
      std::cout<<iPly<<" chi2Add:"<<diff[0] * temvec[0] + diff[1] * temvec[1]<<" diff0 "<< diff[0]<< " vec0 "<<temvec[0]<<" diff1 "<< diff[1]<< " vec1 "<<temvec[1] << std::endl;
    }
  }

  //yzhang 2016-10-12 
  double chiDof;
  if(_noInner)  chiDof = chisq;
  else chiDof = chisq/(2.*nSegFit - 2.);

  if (11 == _debug) {
    if(_noInner) cout<< " no inner chiDof = chisq"<<endl;
    cout << "segment:"<<endl<<" chiDof "<<chiDof<<" chisq "<< chisq << " nhit " << nhit << " phi0/z0 " << 
      param[0] << " cpa/cot " << param[1] << " qual "<<(2. * nhit - chiDof) <<endl;
  }

  qual = 2. * nhit - chiDof;

  //if(is3d) std::cout<< __FILE__ << "   " << " z0 "<<param[0] << " ct "<<param[0] <<std::endl;//zhange TEMP test 2011-05-06 
  return chisq;
}

//************************************************************************
double MdcSegGrouper::calcParByHits(MdcSeg **segGroup, int nToUse, const TrkExchangePar &par, double& qual, int& nSegFit, double param[2], double Bz) {
  //************************************************************************
  //*** Calc. z and cot(theta) for stereo 
  int debug = false;
  if (11 == _debug ) debug = true;
  if (debug) std::cout<< "-----calculate group param by hit-----"<<std::endl;
  MdcLine span(50);
  MdcLine spanFit(50);

  int nHit = 0;
  if (debug) std::cout<< "nToUse="<<nToUse<<std::endl;
  for (int iPly = 0; iPly < nToUse; iPly++) {
    if (segGroup[iPly] == 0) continue;   // skipping this slayer
    nSegFit++;
    MdcSegInfoSterO *segInfo = dynamic_cast<MdcSegInfoSterO*> (segGroup[iPly]->info());

    if(debug) std::cout<< "nHit in segment="<<segGroup[iPly]->nHit()<<std::endl;
    for (int ii=0,iHit=0; ii<segGroup[iPly]->nHit(); ii++){

      if(debug)std::cout<< " calcParByHits ("<< segGroup[iPly]->hit(iHit)->mdcHit()->layernumber()<<","<<segGroup[iPly]->hit(iHit)->mdcHit()->wirenumber()<<")";
      //if(segGroup[iPly]->hit(iHit)->mdcHit()->layernumber()<4) continue;//yzhang TEMP TEST 2011-08-01 

      int szCode = segInfo->zPosition(*(segGroup[iPly]->hit(iHit)),par,&span,iHit,segGroup[iPly]->bunchTime(),Bz);
      if(debug)std::cout<< " szCode "<<szCode;
      if(szCode>0&&debug) std::cout<< iHit<<" s "<< span.x[iHit]<< " z "<<span.y[iHit] <<" sigma "<<span.sigma[iHit];
      if(debug)std::cout<<std::endl;

      spanFit.x[nHit]=span.x[iHit];
      spanFit.y[nHit]=span.y[iHit];
      //spanFit.sigma[nHit]=span.sigma[iHit];
      spanFit.sigma[nHit]=1.;
      if(debug)std::cout<< std::endl;
      iHit++;
      if(szCode>0) nHit++; 
    }
  }

  if(debug)std::cout<< __FILE__ << "   " << __LINE__ << " nHit  "<< nHit<<std::endl;
  if (nHit>0) spanFit.fit(nHit);
  else return -999;

  param[0] = spanFit.intercept;
  param[1] = spanFit.slope;
  if(debug)std::cout<< "nHit "<<nHit<<" intercept(z0) "<<param[0]<< " slope(ct) " << param[1] <<std::endl;

  qual = 2.*nHit - spanFit.chisq/(spanFit.nPoint - 2);
  if(debug)std::cout<< "chisq "<<spanFit.chisq<<" qual "<<qual<<std::endl;

  return spanFit.chisq;
} 
