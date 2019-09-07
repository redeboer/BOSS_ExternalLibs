//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcTrackListBase.cxx,v 1.68 2017/08/11 02:24:14 zhangy Exp $
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): 
//	Steve Schaffner
//	Zhang Yao(zhangyao@ihep.ac.cn)	Migrate to BESIII
//
//--------------------------------------------------------------------------
#include <math.h>
#include <iostream>
#include <vector>
#include "MdcGeom/Constants.h"
#include "MdcGeom/BesAngle.h"
#include "MdcData/MdcHit.h"
#include "MdcData/MdcHitOnTrack.h"
#include "MdcGeom/MdcDetector.h"
#include "MdcTrkRecon/MdcTrackListBase.h"
#include "MdcTrkRecon/MdcMap.h"
#include "MdcTrkRecon/MdcTrack.h"
#include "TrkBase/TrkErrCode.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkFit.h"
#include "TrkBase/TrkFitStatus.h"
#include "TrkBase/TrkHitList.h"
#include "TrkBase/TrkExchangePar.h"
#include "MdcRecoUtil/Pdt.h"
#include "MdcGeom/BesAngle.h"
#include "MdcData/MdcRecoHitOnTrack.h"
#include "MdcRawEvent/MdcDigi.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "TrkFitter/TrkContextEv.h"
#include "TrkBase/TrkRep.h"
#include "Identifier/MdcID.h"
#include "Identifier/Identifier.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "TrkBase/TrkDifTraj.h"
//
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

double MdcTrackListBase::m_d0Cut = -999.;
double MdcTrackListBase::m_z0Cut = -999.;
double MdcTrackListBase::m_ptCut = -999.;

#include "GaudiKernel/NTuple.h"

//*************************************************************************
MdcTrackListBase::MdcTrackListBase(const MdcTrackParams &tkPar) {
  //*************************************************************************
  tkParam = tkPar;
  m_noInner = false;
  return;
}

//------------------------------------------------------------------------
MdcTrackListBase::~MdcTrackListBase() {}
//------------------------------------------------------------------------

// ***********************************************************************
void
MdcTrackListBase::store( RecMdcTrackCol *trackList, RecMdcHitCol *hitList){
  // ***********************************************************************
  int trackId = 0;
  for (int itrack = 0; itrack < nTrack(); itrack++) {
    MdcTrack* track = (*this)[itrack];
    //tkStat:  0,PatRec 1,MdcxReco 2,Tsf 3,CurlFinder
    int tkStat = 0;
    track->storeTrack(trackId, trackList, hitList, tkStat);
    ++trackId;
  }
  HepAListDeleteAll(*this);   // Discard the husks
  removeAll(); 
  return;
}

//*************************************************************************
void
MdcTrackListBase::plot() const {
  //*************************************************************************
  std::cout<< "nTrack   "<<nTrack() << std::endl;//yzhang debug
  for (int itrack = 0; itrack < nTrack(); itrack++) {
    MdcTrack *atrack = (*this)[itrack];
    if (atrack == NULL) continue;
    atrack->track().printAll(cout);
  }
}

//*************************************************************************
int
MdcTrackListBase::arbitrateHits() {
  //*************************************************************************
  // Look at all hits used in two or more tracks.  Assign hits to the track
  // that gives the lower residual.  If, however, many hits are shared by
  // a pair of tracks, assign them all to one or the other.
  // Refit any tracks that have had hits dropped.
  // The implementation is very clumsy, since the arrays were originally
  // indexed by id # => there is an unneeded layer of indexing.

  // return # of tracks deleted

  if (8 == tkParam.lPrint){
    std::cout << "=======Print before arbitrateHits=======" << std::endl;
  }

  int nDeleted = 0;
  std::vector<MdcTrack*> trksToKill;
  trksToKill.reserve(4);

  MdcMap<long,long> idMap;

  //usedInTrackNum records how many shared hits track has with each other track
  int* usedInTrackNum = new int [nTrack()];
  // to navigate from track id # to track pointer:
  MdcTrack** trkXRef = new MdcTrack* [nTrack()];
  //refitTrack flags track id #s of tracks to be refit
  int *refitTrack = new int [nTrack()];
  for (int i = 0; i < nTrack(); i++) {
    refitTrack[i] = 0;
  }

  // Fill xref table
  int itrack;
  for (itrack = 0; itrack < nTrack(); itrack++) {
    MdcTrack *atrack = (*this)[itrack];
    if (atrack == 0) continue;  // I don't think it can be, but . . .
    idMap.put(atrack->track().id(), itrack);
    trkXRef[itrack] = atrack;
  }
  // Loop through the tracks
  for (itrack = 0; itrack < nTrack(); itrack++) {

    if (8 == tkParam.lPrint) std::cout<<"arbitrate track No."<<itrack<<  std::endl;
    MdcTrack *atrack = (*this)[itrack];
    if (atrack == 0) continue;
    TrkRecoTrk& aRecoTrk = atrack->track();
    int lRefit = 0;
    int trackOld = -1;
    const TrkFit* tkFit = aRecoTrk.fitResult();
    assert (tkFit != 0);
    TrkHitList* hitList = aRecoTrk.hits();
    assert (hitList != 0);
restart:
    for (int ii = 0; ii < nTrack(); ii++) usedInTrackNum[ii] = 0;

    // Loop through hits on track, counting # used in other tracks
    int nPrev = 0;
    int nHitDeleted = 0;
    int maxGapLength = 0;//yzhang 2011-07-29 # of max continuous no hits layer for a track, Gap defined as missing layer >=2 
    int nGapGE2= 0;//yzhang 2011-07-29 # of no hits gap for a track 
    int nGapGE3= 0;//yzhang 2011-07-29 # of no hits gap for a track 
    int nHitInLayer[43];//yzhang 2010-09-20 for bad tracking testing
    int nDeleteInLayer[43];//yzhang 2010-09-20 
    for(int i=0;i<43;i++){
      nHitInLayer[i]=0;
      nDeleteInLayer[i]=0;
    }
    if(8 == tkParam.lPrint) std::cout<< "--arbitrate--"<<std::endl;
    for (TrkHitList::hot_iterator ihit(hitList->begin()); ihit != hitList->end(); ++ihit){
      int nUsed = ihit->hit()->nUsedHits();
      if (8 == tkParam.lPrint){
	std::cout<<"nUsed="<<nUsed<<":";
	ihit->hit()->printAll(std::cout);
      }
      if (8 == tkParam.lPrint) {
	double deltaChi = -999;
	ihit->getFitStuff(deltaChi);
	std::cout<< "deltaChi="<<deltaChi<<std::endl;
      }
      int layer = ihit->layerNumber();
      nHitInLayer[layer]++;

      if (!ihit->isActive()) {
	//-----------------------------------
	//yzhang delete not ACT hit 2010-05-14 
	//-----------------------------------
	if(tkParam.lRemoveInActive ) {//2010-05-16 
	  nDeleteInLayer[layer]++;
	  if (8 == tkParam.lPrint) {
	    std::cout<< "=remove above inactive "<<std::endl;
	  }
	  TrkFundHit* hit = const_cast<TrkFundHit*> (ihit->hit());
	  hitList->removeHit(hit);
	  if(ihit == hitList->end()) break;
	  --ihit;//be careful of the iterator, yzhang
	}
	continue;   // active hits only yzhang 2009-11-03 delete
      }
      if (nUsed > 1) {
	bool wasUsed = false;
	std::pair<TrkFundHit::hot_iterator,TrkFundHit::hot_iterator> q =
	  ihit->hit()->getUsedHits();
	for (TrkFundHit::hot_iterator i = q.first; i != q.second; ++i) {
	  if ( !i->isActive() ) continue; //yzhang 2009-11-03 delete
	  TrkRecoTrk * recoTrk=i->parentTrack();
	  //zhangjin
	  int wasDel=0;
	  for(int idel = 0;idel<trksToKill.size();idel++){
	    if( recoTrk == &(trksToKill[idel]->track()) )  wasDel = 1;
	  }
	  if(wasDel==1) continue;
	  //zhangjin
	  int id = recoTrk->id();
	  if (id == aRecoTrk.id()) continue; //skip same track
	  long index = 0;
	  bool findKey = idMap.get(id, index);
	  //	  if( findKey == false ) continue;      //zhangjin 2017/6/27
	  assert(index >= 0);
	  usedInTrackNum[index]++;
	  if (8 == tkParam.lPrint){
	    std::cout<<" track "<<itrack<<"&" <<index
	      << " shared hits "<<usedInTrackNum[index]<<":";
	    ihit->printAll(std::cout);
	  }
	  wasUsed = true;
	}
	if (wasUsed) nPrev++;
      }// end nUsed > 1
    } // end loop over hits

    int testGap = 0;
    //std::cout<< __FILE__ << "   " << itrack<< "   "<<std::endl;
    for (int i=0;i<43;i++){
      //std::cout<< __FILE__ << "   " << i<< " nHitInLayer  "<<nHitInLayer[i]<<" nDeleteInLayer "<<nDeleteInLayer[i]<<std::endl;
      if (8 == tkParam.lPrint) {
	std::cout<<i<<" nHitInLayer  "<<nHitInLayer[i]
	  <<" nDeleteInLayer "<<nDeleteInLayer[i]<<std::endl;
      }
      //1.only hit in layer deleted; 2.no hits in layer; 3.got hits in layer;
      if(nHitInLayer[i]>0 && (nHitInLayer[i]-nDeleteInLayer[i])==0) {
	//only hit in layer i has been deleted
	nHitDeleted++;
	if (8 == tkParam.lPrint) { 
	  cout << "rec hits have been deleted in this layer"<<std::endl; 
	}
	testGap++;
	//std::cout<< __FILE__ << "   " << __LINE__ << " testGap3  "<<testGap<<std::endl;
      }else if(nHitInLayer[i]==0){
	//no hits in this layer i
	testGap++;
	//std::cout<< __FILE__ << "   " << __LINE__ << " testGap3  "<<testGap<<std::endl;
      }else{
	//std::cout<< __FILE__ << "   " << __LINE__ << " testGap3  "<<testGap<<std::endl;
	//got hit in layer i
	if(testGap>=2){
	  nGapGE2++;
	  if(testGap>=3){ nGapGE3++; }
	  if(testGap>maxGapLength) maxGapLength=testGap;
	  //std::cout<< __FILE__ << "   " << __LINE__ << " maxGapLength  "<<maxGapLength<<std::endl;
	}
	testGap=0;
      }//end for layer 43
    }

    bool toBeDeleted = false;

    if(tkParam.lPrint>1) std::cout<< "arbitrateHits tkNo:"<<itrack<<" nGapGE2=  "<<nGapGE2 << " nGapGE3=  "<<nGapGE3 << " maxGapLength=  "<<maxGapLength<<std::endl;
    //yzhang add nHitDeleted cut 2010-09-13 
    // remove track if # not Active 
    if (nHitDeleted >= tkParam.nHitDeleted) {
      if (tkParam.lPrint>1) {
	cout << "arbitrateHits: nHitDeleted "<<nHitDeleted<<" >= "<<tkParam.nHitDeleted
	  <<" Killing tkNo " << itrack << endl;
      }
      toBeDeleted = true;
    }

    //yzhang add nGap cut 2011-07-29 
    // remove track with gaps and big gap
    if (nGapGE2 >= tkParam.nGapGE2) {
      if (tkParam.lPrint>1) {
	cout << "arbitrateHits: nGapGE2 "<<nGapGE2<<" >= "<<tkParam.nGapGE2 <<" Killing tkNo " << itrack << endl;
      }
      toBeDeleted = true;
    } 
    if (nGapGE3 >= tkParam.nGapGE3) {
      if (tkParam.lPrint>1) {
	cout << "arbitrateHits: nGapGE3 "<<nGapGE3<<" >= "<<tkParam.nGapGE3 <<" Killing tkNo " << itrack << endl;
      }
      toBeDeleted = true;
    } 
    if (maxGapLength >= tkParam.maxGapLength) {
      if (tkParam.lPrint>1) {
	cout << "arbitrateHits: maxGapLength "<<maxGapLength<<" >= "<<tkParam.maxGapLength<<" Killing tkNo " << itrack << endl;
      }
      toBeDeleted = true;
    } 

    if(toBeDeleted){
      nDeleted++;
      delete &(atrack->track());    // Delete the RecoTrk inside atrack
      atrack->setTrack(0);
      trksToKill.push_back(atrack);
      continue;
    } 

    //*******
    // How many hits are shared with a single track?
    int nMost = 0;
    int trackMost = 0;
    for (int ii = 0; ii < nTrack(); ii++) {
      if (8 == tkParam.lPrint){
	std::cout<<"tk:"<<itrack<<"&"<<ii
	  <<" shared "<<usedInTrackNum[ii]<<" hits "<<  std::endl;
      }
      if (usedInTrackNum[ii] > nMost) {
	nMost = usedInTrackNum[ii];
	trackMost = ii;  //index of track w/ most hits in common w/ current trk
      }
    }

    // A little precaution against infinite loops:
    if (trackMost == trackOld) {
      std::cout << "ErrMsg(error) MdcTrackListBase:"
	<< "Something ghastly happened in MdcTrackListBase::arbitrateHits"
	<< std::endl;
      return 0;
    }
    trackOld = trackMost;


    //******
    // Decide whether to handle hits individually or in group
    double groupDiff = 0.0;   // relative quality of grouped hits for the two
    // tracks; > 0. => current track worse
    int nFound = 0;    // # of grouped hits located so far
    TrkHitOnTrk **theseHits = 0;  // grouped hits as seen in current track
    TrkHitOnTrk **thoseHits = 0;  // grouped hits as seen in the other track
    int lGroupHits = 0;

    if (nMost >= tkParam.nOverlap) {
      if (8 == tkParam.lPrint){
	std::cout<<"track "<<trackMost<<" shared "<<nMost<<" hits > Cut nOverlap "
	  <<tkParam.nOverlap<<", group hits!"<<std::endl;
      }
      lGroupHits = 1;
      theseHits = new TrkHitOnTrk*[nMost];
      thoseHits = new TrkHitOnTrk*[nMost];
    }

    //*********
    // Go back through hits on this track, looking up the overlap of each
    // if grouping hits, only deal with hits shared with trackMost on this pass
    // otherwise, deal with all shared hits as encountered
    if(8 == tkParam.lPrint) std::cout<<"Go back through hits, looking up overlap hits"<< std::endl;
    if (nMost > 0) {
      if (8 == tkParam.lPrint) std::cout<<" nHits= "<< hitList->nHit()<< std::endl;
      for (TrkHitList::hot_iterator ihit(hitList->begin()); ihit != hitList->end(); ++ihit) {
	int nUsed = ihit->hit()->nUsedHits();

	if (8 == tkParam.lPrint){
	  std::cout<< "--hit go back, nUsed="<<nUsed<<":";
	  ihit->hit()->printAll(std::cout);
	}

	// only shared hits 
	if (nUsed < 2) { continue; }

	// active hits only
	if (!ihit->isActive()) {
	  if (8 == tkParam.lPrint){ std::cout<<"act=0 continue"<<std::endl; }
	  continue;   
	}

	//*** look at all overlaps for this hit
	std::pair<TrkFundHit::hot_iterator,TrkFundHit::hot_iterator> q = ihit->hit()->getUsedHits();
	while (q.first!=q.second) { // nUsed > 0
	  int dropThisHit = 0;
	  TrkHitOnTrk *otherHot = const_cast<TrkHitOnTrk*>((--q.second).get());
	  TrkRecoTrk *otherTrack = otherHot->parentTrack();

	  if (!otherHot->isActive()) continue;

	  // Again, skip "overlap" of track with itself
	  if ( &aRecoTrk == otherTrack) continue;
	  int otherId = otherTrack->id();
	  long otherIndex = -1;
	  idMap.get(otherId, otherIndex); assert(otherIndex >= 0);

	  // if grouping hits, only look at hits shared with trackMost
	  if (lGroupHits && otherIndex != trackMost) continue;

	  if (lGroupHits) {
	    if (8 == tkParam.lPrint) {
	      std::cout<<"group hits "<<  std::endl;
	    }
	    // Calculate contribution of group to each chisq/dof
	    //	      groupDiff += fabs(ihit->resid(0)) -
	    //		fabs(otherHot->resid(0));
	    // Hack to handle tracks with 5 active hits:
	    int aDof = tkFit->nActive() - 5;
	    assert (otherTrack->fitResult() != 0);
	    int otherDof = otherTrack->fitResult()->nActive() - 5;
	    if (aDof <= 0) {groupDiff = 999;}
	    else if (otherDof <= 0) {groupDiff = -999;}
	    else {
	      groupDiff += ihit->resid(0) * ihit->resid(0) * ihit->weight() /
		aDof -
		otherHot->resid(0) * otherHot->resid(0) * otherHot->weight() /
		otherDof;
	    }
	    theseHits[nFound] = const_cast<TrkHitOnTrk*>(ihit.get());
	    thoseHits[nFound] = otherHot;
	    nFound++;
	    dropThisHit = 1;
	  } else {   // handle hits individually

	    if (8 == tkParam.lPrint) {
	      std::cout<<"handle hits individually"<<  std::endl;
	    }
	    nFound++;
	    if (fabs(ihit->resid(0)) > fabs(otherHot->resid(0)) ) {
	      // turn off (inactivate) hit on this track
	      lRefit = 1;
	      //	      ihit->hit()->setUnusedHit(ihit.get());
	      //Should I be setting inactive, or deleting the hit???????
	      const_cast<TrkHitOnTrk*>(ihit.get())->setActivity(0);
	      dropThisHit = 1;
	      if (8 == tkParam.lPrint) {
		std::cout<<"dorp hit ";
		const_cast<TrkHitOnTrk*>(ihit.get())->print(std::cout);
	      }
	      break;      // found other hit, so quit loop
	    } else {
	      // inactivate hit on other track
	      refitTrack[otherIndex] = 1;
	      //	      otherHot->hit()->setUnusedHit(otherHot);
	      otherHot->setActivity(0);
	      if (8 == tkParam.lPrint) {
		std::cout<<"inactive hit on other track";
		const_cast<TrkHitOnTrk*>(ihit.get())->print(std::cout);
	      }
	      break;      // found other hit, so quit loop
	    }
	  } // end grouped/individual treatment

	  if (dropThisHit == 1) break; // don't look for other matches since
	  // this hit is now turned off
	} // end loop over nUsed

	// Quit if we've found all of the shared hits on this track
	if (lGroupHits && nFound == nMost  ||  nFound == nPrev) {
	  if (8 == tkParam.lPrint) {
	    std::cout<<"we've found all of the shared hits on this track,Quit"<<std::endl;
	  }
	  break;
	}

      }  // end loop over hits

      // Decide which track grouped hits belong with and inactivate accordingly
      if (lGroupHits) {
	if (8 == tkParam.lPrint) {
	  cout << "nGroup: " << nMost << "  groupDiff: " << groupDiff << endl;
	  cout << "Track: " << aRecoTrk.id() << "  nHit: "
	    << hitList->nHit() << "  nActive: "
	    << tkFit->nActive() << " chisq/dof: " <<
	    tkFit->chisq()/(tkFit->nActive() - 5) << endl;
	  TrkRecoTrk& othTrack = trkXRef[trackMost]->track();
	  cout << "Track: "<< othTrack.id() << "  nHit: " <<
	    othTrack.hits()->nHit() << "  nActive: " <<
	    othTrack.fitResult()->nActive() << " chisq/dof: " <<
	    othTrack.fitResult()->chisq() /
	    (othTrack.fitResult()->nActive() - 5) << endl;
	}

	if (groupDiff > 0.0) {
	  // inactivate hits on this track
	  lRefit = 1;
	  for (int ii = 0; ii < nMost; ii++) {
	    TrkHitOnTrk *alink = theseHits[ii];
	    TrkFundHit* hit = const_cast<TrkFundHit*> (alink->hit());
	    hitList->removeHit(hit);//yzhang 2011-02-12 
	    //alink->setActivity(0);
	  }
	  if (8 == tkParam.lPrint) std::cout<<"inactive hits on this track, No."<<aRecoTrk.id()<<  std::endl;
	} else {
	  // inactivate hits on other track
	  refitTrack[trackMost] = 1;
	  for (int ii = 0; ii < nMost; ii++) {
	    TrkHitOnTrk *alink = thoseHits[ii];
	    TrkFundHit* hit = const_cast<TrkFundHit*> (alink->hit());
	    TrkRecoTrk& othTrack = trkXRef[trackMost]->track();//zhangjin 
	    othTrack.hits()->removeHit(hit);//zhangjin 
	    //hitList->removeHit(hit);//yzhang 2011-02-12 
	    //alink->setActivity(0);
	  }
	  if (8 == tkParam.lPrint) std::cout<<"inactive hits on other track "<<   std::endl;
	}
	delete [] theseHits;
	delete [] thoseHits;

      } // end if lGroupHits

    } // end if nMost > 0

    //*********
    // Refit this track, if any hits have been dropped
    TrkErrCode fitResult;
    long index = -1;
    idMap.get(aRecoTrk.id(), index); assert (index >= 0);

    if (lRefit || refitTrack[index] == 1) {
      if (8 == tkParam.lPrint) {
	std::cout<<"after group ,refit track"<<aRecoTrk.id()<<  std::endl;
      }
      fitResult = hitList->fit();
      aRecoTrk.status()->addHistory(
	  TrkErrCode(fitResult.success()?TrkErrCode::succeed:TrkErrCode::fail,14,"Arbitrated"), "MdcTrkRecon");
      if (fitResult.failure() && (8 == tkParam.lPrint )) {
	fitResult.print(std::cerr);
      }


      double chisqperDOF;
      bool badFit = true;
      if (fitResult.success()) {
	badFit = false;
	int nDOF = tkFit->nActive() - 5;
	if (nDOF > 5){
	  chisqperDOF = tkFit->chisq() / nDOF;
	}else{
	  chisqperDOF = tkFit->chisq();
	}

	if (chisqperDOF > tkParam.maxChisq) badFit = true;
	if (tkFit->nActive() < tkParam.minHits) badFit = true;
	double tem2 = (float) hitList->nHit() - tkFit->nActive();
	if (tkParam.lUseQualCuts) {
	  if (tem2 >= tkParam.maxNmissTrack) badFit = true;
	  if (tem2 /float(hitList->nHit()) > tkParam.maxNmissNorm){
	    badFit = true;
	  }
	}
	if(8== tkParam.lPrint) std::cout<<"fit quality:"<<
	  " chisqperDof "<<chisqperDOF<<"?>"<<tkParam.maxChisq<<
	    " nActive "<<tkFit->nActive()<<"?<"<<tkParam.minHits<<
	    " nHit "<<hitList->nHit()<<" nhit-act "<<tem2<<"?>= nMiss "<<tkParam.maxNmissTrack<<
	    " hit-act/nhit "<<tem2/float(hitList->nHit())<<"?> MissNorm "<<tkParam.maxNmissNorm
	    <<  std::endl;


      }
      if (8 == tkParam.lPrint)  {
	cout << "Refitting track " << aRecoTrk.id() << " success = "
	  << fitResult.success() << "\n";
      }
      // If the track no longer passes cuts, delete it
      if (fitResult.failure() || badFit ) {
	nDeleted++;
	//  Don't change the track list while we're iterating through it!
	//	remove(atrack);
	//int id = aRecoTrk.id();
	if (8 == tkParam.lPrint) {
	  cout << "fitResult.failure? "<<fitResult.failure()
	    <<" badFit? "<<badFit <<" Killing tkNo " << itrack << endl;
	}
	//delete &(atrack->track());    // Delete the RecoTrk inside atrack
	//atrack->setTrack(0);
	trksToKill.push_back(atrack);
	continue;
      }
    } // end if lRefit

    if (lGroupHits) goto restart;

  } // end loop over tracks
  if (8 == tkParam.lPrint) std::cout<<"end of loop over tracks"<<  std::endl;

  // Remove dead track husks
  for (int itk = 0; itk < (int)trksToKill.size(); itk++) {
    delete &(trksToKill[itk]->track());   //zhangjin
    trksToKill[itk]->setTrack(0);         //zhangjin
    remove(trksToKill[itk]);
    if (8 == tkParam.lPrint) std::cout<<"remode dead track No."<<itk<<  std::endl;
  }
  if (8 == tkParam.lPrint) std::cout<<"---end of arbitrateHits"<<  std::endl;

  delete [] usedInTrackNum;
  delete [] refitTrack;
  delete [] trkXRef;
  return nDeleted;
}

//**************************************************************************
void
MdcTrackListBase::newParams(const MdcTrackParams &tkPar) {
  //**************************************************************************
  tkParam = tkPar;
}

//--------------------------------------------------------------------
void
MdcTrackListBase::remove( MdcTrack *atrack ) {
  //--------------------------------------------------------------------
  if (atrack != 0) {
    HepAList<MdcTrack>::remove( atrack );
    delete atrack;
  }
}

//**************************************************************************
void
MdcTrackListBase::transferTrack() {
  //**************************************************************************

}
