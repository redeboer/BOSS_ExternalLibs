//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegList.cxx,v 1.10 2012/10/13 09:39:26 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors:
// 	Zhang Yao(zhangyao@ihep.ac.cn)	Migrate to BESIII
//
// Copyright (C)  1996  The Board of Trustees of  
// The Leland Stanford Junior University.  All Rights Reserved.
//------------------------------------------------------------------------
#include "MdcGeom/Constants.h"
#include "MdcGeom/BesAngle.h"
#include "MdcData/MdcHit.h"
#include "MdcData/MdcHitUse.h"
#include "MdcGeom/MdcDetector.h"
#include "MdcGeom/MdcSuperLayer.h"
#include "MdcTrkRecon/MdcSegList.h"
#include "MdcTrkRecon/mdcWrapWire.h"
#include "MdcTrkRecon/MdcSeg.h"
#include "MdcTrkRecon/GmsList.h"
//yzhang hist cut
#include "AIDA/IHistogram1D.h"
extern AIDA::IHistogram1D*  g_phiDiff;
extern AIDA::IHistogram1D*  g_slopeDiff;
//zhangy hist cut
const double _twopi = Constants::twoPi;

/**************************************************************************/
MdcSegList::MdcSegList(int nSlayer, const MdcSegParams segP) {
/**************************************************************************/
    segParam = segP;  // bit-wise copy
    MdcSeg::setParam(&segParam);
    zeroSeed();
    segList = new GmsList[nSlayer];
    _nsupers = nSlayer;
}

/**************************************************************************/
MdcSegList::~MdcSegList() { 
/**************************************************************************/
delete [] segList; 
}

/**************************************************************************/
void MdcSegList::zeroSeed() {
/**************************************************************************/
  seedSeg[0] = seedSeg[1] = seedSeg[2] = 0;
  seedSlay[0] = seedSlay[1] = seedSlay[2] = 0;
}
/**************************************************************************/
void MdcSegList::resetSeed(const MdcDetector *gm) {
/**************************************************************************/

  for (int iview = -1; iview <= 1; iview++) {
    int viewIndex = iview + 1;
   /* 
    if(0 == iview){
      seedSlay[viewIndex] = gm->firstSlayInView(iview);
    }else{
      seedSlay[viewIndex] = gm->lastSlayInView(iview);
    }
    */
    seedSlay[viewIndex] = gm->lastSlayInView(iview);
    if (seedSlay[viewIndex] != 0) seedSeg[viewIndex] = 
      (MdcSeg *)   oneList(seedSlay[viewIndex]->index())->first();
  }
}
/**************************************************************************/
void MdcSegList::plot() const {
  /**************************************************************************/

  MdcSeg *aSeg;
  for (int i = 0; i < _nsupers; i++) {
    std::cout << " ---------------MdcSeg of Slayer "<<i<<"-------------" << std::endl;

    aSeg = (MdcSeg *) segList[i].first();
    while (aSeg != 0) {
      aSeg->plotSegAll();
      std::cout << endl;
      aSeg = (MdcSeg *) aSeg->next();
    }
  }
}

/****************************************************************************/
void MdcSegList::destroySegs() {
  /****************************************************************************/

  zeroSeed();
  MdcSeg *aSeg, *bSeg;
  for (int i = 0; i < _nsupers; i++) {
    aSeg = (MdcSeg *) segList[i].first();
    while (aSeg != 0) {
      bSeg = (MdcSeg *) aSeg->next();
      segList[i].remove( aSeg );
      delete aSeg ;
      aSeg = bSeg;
    }
  }
  return;
}

//****************************************************************************
void MdcSegList::sortByPhi() {
  //****************************************************************************
  // Sort the list of segments by phi.  Insertion sort, good only for small n

  for (int isuper = 0; isuper < _nsupers; isuper++) {

    if (segList[isuper].first() == 0) continue;
    MdcSeg *aseg = (MdcSeg *) segList[isuper].first()->next();
    for (int j = 1; j < (int) segList[isuper].count(); j++) {
      double phiSeg = aseg->phi();
      MdcSeg *nextseg = (MdcSeg *) aseg->next();

      MdcSeg *bseg = (MdcSeg *) aseg->prev();
      if (phiSeg < bseg->phi() ) {
	while (bseg != 0 && phiSeg < bseg->phi()) {  
	  bseg = (MdcSeg *) bseg->prev();
	}
	segList[isuper].moveAfter(aseg, bseg);   // insert aseg after bseg
      }

      aseg = nextseg;
    }

  }
  return;
}


//****************************************************************************
MdcSeg * MdcSegList::getSeed( int iview, int goodOnly) {
  //***************************************************************************

  // Find a suitable segment for starting a track.  If goodOnly=1, look for 
  //  isolated, unambiguous segments; unambiguous is 
  //  defined as being the only segment within a band in phiseg (typically 
  //  2 cell widths wide), or as differing only slightly from nearby 
  //  segments in phi and slope.  When these have been exhausted, take any 
  //  unused segment.  Return pointer to seed seg, or 0 if none found. 

  int viewIndex = iview + 1;
  if (seedSlay[viewIndex] == 0) return 0;
  while (1) {
    //if(seedSeg[viewIndex] != 0){
      //seedSeg[viewIndex]->plotSeg(0,0);//yzhang debug
      //std::cout<<__FILE__<<" "<<__LINE__<<" goodOnly "<<goodOnly<<  std::endl;
    //}
    if (seedSeg[viewIndex] == 0) {
      //seedSlay[viewIndex] = seedSlay[viewIndex]->nextInView();//yzhang temp 
      seedSlay[viewIndex] = seedSlay[viewIndex]->prevInView(); //yzhang del
      if (seedSlay[viewIndex] == 0) return 0;
      seedSeg[viewIndex] = (MdcSeg *) 
	oneList(seedSlay[viewIndex]->index())->first();
    } // We have a segment; is it a viable seed?
    else if (seedSeg[viewIndex]->segUsed()) {
      //std::cout<<__FILE__<<" segUsed " << std::endl;
      seedSeg[viewIndex] = (MdcSeg *) seedSeg[viewIndex]->next();
    } 
    else if (seedSeg[viewIndex]->segAmbig() && goodOnly) {
      //std::cout<<__FILE__<<" segAmbig && goodOnly" << std::endl;
      seedSeg[viewIndex] = (MdcSeg *) seedSeg[viewIndex]->next();
      //yzhang delete 09-10-10
      //delete (!seedSeg[viewIndex]->segAmbig() && !goodOnly)
    } //yzhang add 09-09-28
    else if ( !seedSeg[viewIndex]->segFull() && goodOnly) {
      //std::cout<<__FILE__<<" segFull && goodOnly" << std::endl;
      seedSeg[viewIndex] = (MdcSeg *) seedSeg[viewIndex]->next();
      //zhangy add
    } else {
      // Reject segments with a lot of used hits
      int nused = 0;
      int navail = seedSeg[viewIndex]->nHit();
      for (int ihit = 0; ihit < seedSeg[viewIndex]->nHit(); ihit++) {
	MdcHitUse *ahit = seedSeg[viewIndex]->hit(ihit);
	if ( ahit->mdcHit()->usedHit() ) nused++;
      }
      // Minimum 2 unused hits (but allow special-purpose segs w/ 0 hits)
      if (navail - nused < 2 && navail > 0) {  
	seedSeg[viewIndex] = (MdcSeg *) seedSeg[viewIndex]->next();
      } else {
	//std::cout<<__FILE__<<" Reject segments with a lot of used hits" << std::endl;
	break;
      }
    }
    /*
    if(seedSeg[viewIndex]!=0){
      seedSeg[viewIndex]->plotSeg();
      std::cout<< __FILE__ << "   " << __LINE__ << " goodOnly  "
	<<goodOnly<<" viewIndex "<<viewIndex<< " full "<<seedSeg[viewIndex]->segFull() <<std::endl;
    }
    */
  }

  // We have a seed
  MdcSeg *theSeed = seedSeg[viewIndex];
  seedSeg[viewIndex] = (MdcSeg *) seedSeg[viewIndex]->next();

  return theSeed;
}
//****************************************************************************
void MdcSegList::tagAmbig() {
  //***************************************************************************
  // Go through the list of segments and tag ones that will not make good 
  //  seeds -- i.e. ones that overlap with each other

  for (int isuper = 0; isuper < _nsupers; isuper++) {
    const GmsList *thisSegList = &segList[isuper];
    if (thisSegList->count() < 2) return;

    // Bunch some declarations:
    double width, diff;
    double thisphi, nextphi;
    double nisocell = 2;
    //  width = cell width in phi for this superlayer
    //  diff = width of band about the track = nisocell * width

    // Calculate road width for this slayer. 
    const MdcLayer *aLayer = 
      ((MdcSeg *)thisSegList->first())->superlayer()->firstLayer();
    width = _twopi / aLayer->nWires();
    diff = nisocell * width;


    for ( MdcSeg *startSeg = (MdcSeg *) thisSegList->first(); 
	startSeg != 0; startSeg = (MdcSeg *) startSeg->next() ) {
      MdcSeg *nextSeg = (MdcSeg *) startSeg->next();
      if (nextSeg == 0) nextSeg = (MdcSeg *) thisSegList->first(); 

      thisphi = startSeg->phi();
      nextphi = nextSeg->phi();
      if (nextphi < thisphi) nextphi += _twopi;

      if (nextphi - thisphi < diff) {
	startSeg->setAmbig();
	nextSeg->setAmbig();
      }

    } // end loop over segments
  }  // end loop over superlayers

  return;
}
//**************************************************************************
//void MdcSegList::newParams(const MdcSegParams &segPar) {
//**************************************************************************
//  segParam = segPar;
//}
/****************************************************************************/
void MdcSegList::append(MdcSeg *aSeg) {
  /****************************************************************************/
  segList[aSeg->superlayer()->index()].append(aSeg); 
}
/****************************************************************************/
void MdcSegList::massageSegs() {
  /****************************************************************************/
  sortByPhi();
  // Delete duplicates only if there are too many segments
  bool drop = (segPar()->dropDups && count() > 200);//yzhang FIXME
  deleteDups(drop);
  tagAmbig();
}
/****************************************************************************/
void MdcSegList::deleteDups(bool ldrop) {
  /****************************************************************************/
  // Removes segments that have almost identical parameters.  A bit risky, 
  // perhaps, but what kind of life would you have if you never took 
  // any risks?

  // On second thought, offer the option of just marking them as used.  

  // Takes pairs of segments, and if they have the same slope and phi 
  // (within cuts), selects better, using nHit and chisq.  Note that 2
  // identical segs may be separated in list by non-identical
  
  bool printit = (5 == segPar()->lPrint);


  if(printit) std::cout<<  "  =======MdcSegList::deleteDups()=====  " << std::endl;
  

  for (int isuper = 0; isuper < _nsupers; isuper++) {
    if(isuper==10) continue;//yzhang add 2012-09-18 
    GmsList *thisSegList = &segList[isuper];
    if (thisSegList->count() < 2) continue;
    if(5 == segPar()->lPrint || 13 == segPar()->lPrint){
      std::cout<<endl<< " -----slayer  " << isuper<<"-----"<< std::endl;
    }

    double thisphi, nextphi;
    double slopediff = segPar()->slopeDiffDrop; //FIXME

    // Calculate road width for this slayer. 
    const MdcLayer *aLayer = 
      ((MdcSeg *)thisSegList->first())->superlayer()->firstLayer();
    double width = _twopi / aLayer->nWires();
    double phidiff = segPar()->phiDiffDropMult * width; //FIXME

    int lWrapped = 0;

    MdcSeg *startSeg = (MdcSeg *) thisSegList->first(); 
    while ( startSeg != 0 ) {
      if (thisSegList->count() < 2) break;
      if (lWrapped) break;
      thisphi = startSeg->phi();
      MdcSeg *nextSeg = startSeg;
      if(5 == segPar()->lPrint || 13 == segPar()->lPrint){
	startSeg->plotSeg();std::cout<<std::endl;
      }

      while (1) {            // loop over nextSeg
	nextSeg = (MdcSeg *) nextSeg->next();
	if (nextSeg == 0) {
	  nextSeg = (MdcSeg *) thisSegList->first();
	  if (lWrapped == 1) break;  // cluster covers 2pi; run, screaming
	  lWrapped = 1;
	}
	if( nextSeg == startSeg ) break;//yzhang add 2011-06-16 
	nextphi = nextSeg->phi();

	if (nextphi < thisphi) nextphi += _twopi;

	if(printit){
	  std::cout<<std::endl<<"----start "; startSeg->plotSeg(); std::cout<<std::endl;
	  std::cout<<"     next "; nextSeg->plotSeg(); std::cout<<std::endl;
	  std::cout<<" phi diff "<< nextphi -thisphi << " cut phidiff "<<phidiff;
	  std::cout<<" slope diff "<< nextSeg->slope() - startSeg->slope()
	    << " cut slopdiff "<<slopediff<<std::endl;
	}

	if (g_phiDiff) {g_phiDiff->fill((nextphi - thisphi)/width);}//yzhang hist cut
	if (g_slopeDiff) {g_slopeDiff->fill(nextSeg->slope() - startSeg->slope());}//yzhang hist cut

	if (nextphi - thisphi > phidiff) {
	  if(printit){ std::cout<<  " --not same phi" <<std::endl; }
	  // Done with this segment (aSeg)
	  startSeg = (MdcSeg *) startSeg->next();
	  break;
	} else if (fabs( nextSeg->slope() - startSeg->slope() ) > slopediff) {
	  if(printit){ std::cout<<  " --attached, but not identical -- move on to next nextSeg "<< std::endl; }
	  // attached, but not identical -- move on to next nextSeg
	  continue;
	} else {
	  // identical; choose better
	  int firstBetter = 0;
	  assert (startSeg->nHit() > 2);
	  double chiFirst = startSeg->chisq() / (startSeg->nHit() - 2);
	  assert (nextSeg->nHit() > 2);
	  double chiNext = nextSeg->chisq() / (nextSeg->nHit() - 2);
	  double theDiff;
	  int cdiff = (int) nextSeg->nHit() - (int) startSeg->nHit();
	  theDiff = (chiFirst - chiNext) + 2. * (cdiff);
	  if (theDiff < 0.) firstBetter = 1;
	  if(printit){ 
	    std::cout<<  " --identical; choose better "<< std::endl; 
	    std::cout<<  " chi start "<<chiFirst<<" chi next "<< chiNext<<" cdiff "<<cdiff<< std::endl; 
	  }

	  if (firstBetter) {
	    if(printit){ std::cout<< " startSeg better,"; }
	    MdcSeg *tempSeg = nextSeg;
	    nextSeg = (MdcSeg *) nextSeg->next();
	    if (ldrop) {
	      if(printit){ std::cout<< " delete nextSeg"<<std::endl; }
	      thisSegList->remove(tempSeg);
	      delete tempSeg;
	    } else {
	      if(printit){ std::cout<< " nextSeg->setUsed(true) "<<std::endl; }
	      tempSeg->setUsed();
	    }
	    if (nextSeg == 0) {
	      nextSeg = (MdcSeg *) thisSegList->first();
	      if (lWrapped == 1) break;  // cluster covers 2pi; run, screaming
	      lWrapped = 1;
	    }
	  } else {
	    if(printit){ std::cout<< " nextSeg better,"; }
	    MdcSeg *tempSeg = startSeg;
	    startSeg = (MdcSeg *) startSeg->next();
	    if (ldrop) {
	      if(printit){ std::cout<< " delete startSeg"<<std::endl; }
	      thisSegList->remove(tempSeg);
	      delete tempSeg;
	    } else {
	      if(printit){ std::cout<< " startSeg->setUsed(true) "<<std::endl; }
	      tempSeg->setUsed();
	    }
	    break;
	  }

	}  // end if identical
      }  // end loop over nextSeg

    } // end primary loop over segments
  }  // end loop over superlayers
}

void 
MdcSegList::setPlot(int lPlt) {
  MdcSeg::segPar()->lPlot = lPlt;
}

//-------------------------------------------------------------------------
const GmsList*
MdcSegList::oneList(int slayIndex) const {
  //-------------------------------------------------------------------------
  return &segList[slayIndex];
}

//-------------------------------------------------------------------------
int 
MdcSegList::count() const { 
  //-------------------------------------------------------------------------
  int cnt = 0;
  for (int i = 0; i < _nsupers; i++) cnt += segList[i].count();
  return cnt;
}
