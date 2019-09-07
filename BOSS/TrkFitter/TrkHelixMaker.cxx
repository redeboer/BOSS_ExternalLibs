//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHelixMaker.cxx,v 1.1.1.1 2005/04/21 06:26:56 maqm Exp $
//
// Description:
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: Steve Schaffner
//
//------------------------------------------------------------------------
#include "TrkFitter/TrkHelixMaker.h"
#include "TrkFitter/TrkHelixRep.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkHitOnTrk.h"

//------------------------------------------------------------------------
TrkHelixMaker::~TrkHelixMaker() {}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
TrkHelixMaker::TrkHelixMaker() {}
//------------------------------------------------------------------------


void
TrkHelixMaker::addZValues(TrkRecoTrk& theTrack, double z0,
                          double tanDip, double chi2)
{
  PdtPid::PidType hypo = theTrack.defaultType();
//  const TrkRep* defrep = getRep(theTrack,hypo);
//  const TrkHotList *hl = defrep->hotList();//yzhang delete
  TrkRep* defrep = getRep(theTrack,hypo);
  TrkHotList *hl = defrep->hotList();//yzhang temp
  const TrkFit* theFit = theTrack.fitResult();
  if (theFit == 0) {
    std::cout<<"ErrMsg(error)"
      << "TrkSimpleMaker::addZValues() cannot get helix from invalid fit."
      << std::endl;
    return;
  }

  TrkExchangePar oldPar = theFit->helix(0.0);
  oldPar.setZ0(z0);
  oldPar.setTanDip(tanDip);
  /*
  std::cout << "#########################before newHelix" << std::endl;//yzhang debug
  std::cout << "track--------- " << std::endl;//yzhang debug
  theTrack.printAll(std::cout);//track
  std::cout << "rep of track--------- " << std::endl;//yzhang debug
  getRep(theTrack,hypo)->printAll(std::cout);//track Rep
  std::cout << "circle rep's hotlist--------- " << std::endl;//yzhang debug
  hl->printAll(std::cout);//Circle Rep 's hotList
    */
  
  TrkHelixRep* newRep = new TrkHelixRep(oldPar, &theTrack, hypo, hl);
/*     
  std::cout << "#########################after new helix before setRep" << std::endl;//yzhang debug
  std::cout << "track--------- " << std::endl;//yzhang debug
  theTrack.printAll(std::cout);//track
  std::cout << "rep of track--------- " << std::endl;//yzhang debug
  std::cout <<newRep->hotList()<<std::endl;
  std::cout << "circle hotlist --------- " << std::endl;//yzhang debug
  std::cout <<hl<<std::endl;
  std::cout << "helix hotlist --------- " << std::endl;//yzhang debug
  getRep(theTrack,hypo)->printAll(std::cout);//track Rep
  std::cout << "circle rep's hotlist--------- " << std::endl;//yzhang debug
  hl->printAll(std::cout);//Circle Rep 's hotList
  std::cout << "helix rep's hotlist--------- " << std::endl;//yzhang debug
  newRep->hotList()->printAll(std::cout);//Helix Rep's hotList
  */
  
  newRep->fitter().setFittingPar(false, false);
  setRep(theTrack, newRep);   // theTrack will delete all old Reps

 /* 
  std::cout << "#########################after setRep" << std::endl;//yzhang debug
  std::cout << "track--------- " << std::endl;//yzhang debug 
  theTrack.printAll(std::cout);//track
  std::cout << "rep of track--------- " << std::endl;//yzhang debug
  getRep(theTrack,hypo)->printAll(std::cout);//track Rep
  //std::cout << "circle rep--------- " << std::endl;//yzhang debug
  //defrep->printAll(std::cout);
  //std::cout << "circle rep's hotlist--------- " << std::endl;//yzhang debug
  //hl->printAll(std::cout);//Circle Rep 's hotList
  std::cout << "helix rep's hotlist--------- " << std::endl;//yzhang debug
  newRep->hotList()->printAll(std::cout);//Helix Rep's hotList
  
  std::cout << "######################### " << std::endl;//yzhang debug
    
 */
  newRep->setChisq(chi2);
  if (chi2 > 0.0) newRep->setValid(true);
  newRep->setCurrent(false);
}
