//
// $Id: MdcTrackListCsmc.cxx,v 1.7 2008/04/01 03:14:36 zhangy Exp $
//
#include "MdcTrkRecon/MdcTrackListCsmc.h"
#include <math.h>
#include <iostream>
#include "MdcData/MdcHitOnTrack.h"
#include "MdcRawEvent/MdcDigi.h"
#include "MdcTrkRecon/MdcTrack.h"
#include "MdcTrkRecon/MdcSegGrouperSt.h"
#include "MdcTrkRecon/MdcSegList.h"
#include "MdcTrkRecon/MdcSegGrouperCsmc.h"
#include "TrkBase/TrkHitList.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkFit.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkErrCode.h"
#include "TrkFitter/TrkLineMaker.h"

//yzhang hist cut
#include "AIDA/IHistogram1D.h"
//extern AIDA::IHistogram1D*  g_tkChi2;
//zhangy hist cut

// IOS_USING_DECLARATION_MARKER - BaBar iostreams migration, do not touch this line!
using std::cout;
using std::endl;

// End Implementation Dependencies -------------------------------------

// *************************************************************************
MdcTrackListCsmc::MdcTrackListCsmc(const MdcTrackParams &tkPar) : 
  MdcTrackListBase(tkPar) { 
// *************************************************************************
    return;
  }

// *************************************************************************
MdcTrackListCsmc::~MdcTrackListCsmc() {} 
// *************************************************************************

// *************************************************************************
int MdcTrackListCsmc::createFromSegs(MdcSegList *segs, const MdcHitMap*, 
				     const MdcDetector* gm, 
				     TrkContext& context, double t0) {
// *************************************************************************
  // Forms tracks out of list of superlayer segments
  int nTracks = 0;
  // Create empty list of stereo segs (to save time)
  MdcSegGrouperSt stereoSegs(gm,tkParam.lPrint);
  // *** Create r-phi track

  // Create list of axial segments, treated as on straight tracks
  MdcSegGrouperCsmc straightSegs(gm, tkParam.lPrint);
  straightSegs.fillWithSegs(segs);
  //std::cout<< "after straight fill  " << std::endl; 
  //segs->plot();//yzhang debug
  MdcSeg *seed;
  int goodOnly = 1;
  MdcTrack* trialTrack = 0;
  while (1) {
    seed = segs->getSeed(0,goodOnly);
    if (seed == 0 && goodOnly == 1) {
      segs->resetSeed(gm);
      goodOnly = 0;
      continue;
    }
    else if (seed == 0 && goodOnly == 0) {
      break;
    }
    delete trialTrack;
    trialTrack = 0;            
    int success = straightSegs.combineSegs(trialTrack, seed,
					   context, t0, tkParam.maxSegChisqO);
    if (!success) {
      //std::cout<< " MdcTrackListCsmc::combineSegs not successed!" << std::endl;
      continue;
    }

    if (tkParam.lPrint > 1) {
      trialTrack->track().printAll(cout);
    }
    // *** End r-phi track-finding

    // *** Add stereo to taste
    stereoSegs.fillWithSegs(segs, trialTrack);

    success = stereoSegs.combineSegs(trialTrack, 0, context, t0, 
				     tkParam.maxSegChisqO);
    if (success) {
    // Finish 3-d track;
      success = finish3d(trialTrack->track());
      //      success = finish3d(trialTrack->track()); // GSciolla: try to repeat
    }  
    if (!success) {
      //std::cout<< " MdcTrackListCsmc::finish3d not successed!" << std::endl;
      continue;
    }

    if (tkParam.lPrint > 1) {
      trialTrack->track().printAll(cout);
    }

    nTracks++;
    append(trialTrack);    // Add to list of Tracks
    trialTrack = 0;
    
  }  // end while(1)   
  delete trialTrack;
  

  //cout << " Number of Tracks found= " << nTracks  ;
  //cout << " " << endl;
  return nTracks;
}

// ************************************************************************
int MdcTrackListCsmc::finish3d(TrkRecoTrk &trk) {
// ************************************************************************
  int success = 0;
  int nParamFit = 0 ;

  TrkErrCode fitResult;

  // ------------------------
  //   4 param fit (line)
  // ------------------------
  nParamFit = 4;
  TrkLineMaker makeFit;
  makeFit.changeFit(trk);
  makeFit.setFlipAndDrop(trk, true, true);
  //setCosmic(&trk);    // set hot to cosmics OBSOLETE! REMOVE!
  fitResult = trk.hits()->fit();
  makeFit.setFlipAndDrop(trk, false, false);  
  
  // ---------------------------------------------------------------
  // Are there some TDC that can be replaced with later measurements? ( beginning) 
  // ---------------------------------------------------------------
  // Note to Sciolla: multiple hits commented out for now OK! I will put it back later...
  /*  if( _flags.useMultipleHits ) { 
    int NHITS   = trk.nHit(); 
    
    if (fitResult.success()) {
      int ifirst = 0;     
      for (int ihot = 0; ihot < NHITS ; ihot++) {
	
	MdcHitOnTrack* ahot = (MdcHitOnTrack*)trk.hitAList()[ihot];
	double firstTime = ahot->mdcHit()->rawTime(0); 
	
	// Get the list of times for this Digi 
	const MdcDigi* tmpDigi = ahot->mdcHit()->digi() ; 
// 	int nTDChits = tmpDigi->getTdcTimeAList()->length() ;
	int nTDChits = tmpDigi->tmdcits() ;
	
	double newTime=0. ; 
	
	for( int il = 1; il<nTDChits ; il++){ 
	  
	  // get the time corresponding to this doca ---> distance to time 
	  newTime = tmpDigi->TdcTime(il); 
	  double newDrift = ahot->layer()->timeToDistance(newTime); 
	  
	  double tmp_doca  = ahot->dcaToWire();
	  double new_diff = fabs(fabs(tmp_doca)-fabs(newDrift)); 
	  double tmp_drift = ahot->drift();
	  double tmp_drift2 = 
	    ahot->layer()->timeToDistance(tmpDigi->TdcTime(0)); 
	  double old_diff  = fabs(fabs(tmp_doca)-fabs(tmp_drift2)); 
	  
	  if( ((old_diff-new_diff)>0.1)  // new time better then old of at least 1 mm  
	      && (new_diff<=0.8) ) { // and newresid < 2 mm 
	    //	if(_flags.debug) {                  
	    if(1) { 
	      
	      if(ifirst==0) { 
		ifirst = 1 ; 
		cout <<" nHits | time1  |fittim | time2 | doca(mm) | drift1 " 
		     <<"  drift1(rec) | drift2 | layer | wire | isActive" 
		     << " | chi2 | nactive" << endl; 
	      }
	      
	      int isAct = 1 ; 
	      if (!ahot->isActive()) { 
		isAct = 0 ; 
		//      cout<< " >> " ;    
		//	cout << "This hit is not active " << endl ;
	      }
	      
	      cout << "   " << nTDChits 
		   << "    " << tmpDigi->TdcTime(0) 
		   << "  " << ahot->fitTime()
		   << "  " <<  newTime 	
		   << "  " << tmp_doca*10
		   << "  " << tmp_drift*10 
		   << "  " << tmp_drift2*10 
		   << "  " << newDrift*10 
		   << "  " << ahot->layer()->layNum() 
		   << "  " << ahot->wire()
		   << "  " <<  isAct 
		   << "  " << trk.chisq()
		   << "  " << trk.nActive() 
		   << endl;       
	      
	      cout << " old/new diff (mm) = " << old_diff*10 
		   << " | " << new_diff*10
		   << endl ; 
	    }	  
	    
	    ahot->setTimeIndex(il) ; 
	    //	cout << "new rawtime =" 
	    //     << ahot->mdcHit()->rawTime(0)<< endl ; // check the new value 
	    
	    
	    // store results in ntuple to see improvements ... before ... 		
	    HepTuple* tupleMultHits = _manager->ntuple("multiHits");
	    
	    tupleMultHits->column("ch2Dof1", trk.chisq()/(trk.nActive() - 4));
	    tupleMultHits->column("nActiv1", trk.nActive());
	    tupleMultHits->column("resid1",ahot->resid());
	    tupleMultHits->column("doca1",ahot->resid()+ahot->drift()); 
	    tupleMultHits->column("time1",  ahot->fitTime() );   
	    tupleMultHits->column("rawtime1",  ahot->mdcHit()->rawTime(il) ); 
	    
	    // - make the hit active and usable 
	    ahot->setActivity(true);
	    
	    // now refit the track ... 
	    fitResult = trk.fit();
	    
	    // store results in ntuple to see improvements ... after ...  		
	    tupleMultHits->column("ch2Dof2", trk.chisq()/(trk.nActive() - 4));
	    tupleMultHits->column("nActiv2", trk.nActive());
	    tupleMultHits->column("resid2",ahot->resid());
	    tupleMultHits->column("doca2",ahot->resid()+ahot->drift()); 
	    tupleMultHits->column("time2",  ahot->fitTime() );   
	    tupleMultHits->column("rawtime2",  ahot->mdcHit()->rawTime(il) );   
	    tupleMultHits->dumpData();
	  }
	}
      }
    }  
  } // Are there some TDC that can be replaced with later measurements?  (end)
  */
  
  const TrkFit* tkFit = trk.fitResult();
  double chisqperDOF = 0.;
  if (fitResult.success()) {
    int nDOF = tkFit->nActive() - nParamFit;
    if (nDOF > nParamFit) 
      chisqperDOF = tkFit->chisq() / nDOF;
    else 
      chisqperDOF = tkFit->chisq();
    int goodMatch = 1;
    if (chisqperDOF > tkParam.maxChisq) goodMatch = 0;
    if (tkFit->nActive() < tkParam.minHits) goodMatch = 0;

    if (goodMatch) success = 1;
  }
  
  return success;
}


////--------------------------------------------------------------------
//void MdcTrackListCsmc::setCosmic(TrkRecoTrk  *recoTrk ) {
////--------------------------------------------------------------------
//
  //// get list of hot and set them to cosmics 
  ////----------------------------------------  
  ////  int ihot ; 
  //TrkHitList* hitList = recoTrk->hits();
  //assert (hitList != 0);
  //for (int layer = 1; layer <= 16; layer++) {
    //// Look for hits in this layer
    //TrkHitList::hot_iterator end(hitList->end());
    //for ( TrkHitList::hot_iterator ihot(hitList->begin()) ; ihot != end; ++ihot) {
      //const MdcHitOnTrack* ahot = ihot->mdcHitOnTrack();
//
      //// GS: setCosmic and setPM are not useful anymore since this info 
      //// are now in MdcEnv and MdcHitOnTrack takes them from the MdcEnv
      //// ahot->setCosmic(true);              // set cosmic flag 
      //// ahot->setzPM(_flags.zPM);       // set z Photo Multip. ProtoII  
//
    //}  // loop over hits
  //}
//}




