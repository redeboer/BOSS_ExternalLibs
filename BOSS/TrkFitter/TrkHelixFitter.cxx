//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHelixFitter.cxx,v 1.16 2011/09/26 00:56:56 zhangy Exp $
//
// Description:
//     Performs least-squares fit for any simple track representation -- 
// circle, line, helix.  Depending on the state of flags, it will also 
// drop hits and flip ambiguities (for hits that allow such action) while 
// fitting.  (Originally written in C, which shows in a few spots.)
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
#include <assert.h>
#include "TrkFitter/TrkHelixFitter.h"
#include "TrkBase/TrkSimpTraj.h"
#include "TrkBase/TrkParams.h"
#include "TrkBase/TrkHitOnTrk.h"
#include "CLHEP/Matrix/Vector.h" 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "TrkBase/TrkEnums.h"
#include "TrkBase/TrkErrCode.h"
#include "TrkBase/TrkHotList.h"
#include <vector>
using std::cout;
using std::endl;

bool TrkHelixFitter::m_debug = false;
double TrkHelixFitter::nSigmaCut[43] = {  
  10.,5.,5.,10., 10.,5.,5.,10., // U,V
  10.,5.,5.,5., 5.,5.,5.,5., 5.,5.,5.,10.,//A,A,A
  10.,5.,5.,5., 5.,5.,5.,5., 5.,5.,5.,5., 5.,5.,5.,10.,//U,V,U,V 
  10.,5.,5.,5., 5.,5.,10.//A,A
};
//------------------------------------------------------------------------
TrkHelixFitter::~TrkHelixFitter() {}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
TrkHelixFitter::TrkHelixFitter(bool allowFlips, bool allowDrops): 
  //------------------------------------------------------------------------
  TrkHitOnTrkUpdater()
{
  _allowFlips = allowFlips;
  _allowDrops = allowDrops;
  _lastChisq = -1.;
}

//------------------------------------------------------------------------
TrkHelixFitter::TrkHelixFitter(const TrkHelixFitter& right): 
  //------------------------------------------------------------------------
  TrkHitOnTrkUpdater()
{
  _allowFlips = right._allowFlips;
  _allowDrops = right._allowDrops;
  _lastChisq = -1.;
}

//------------------------------------------------------------------------
  TrkHelixFitter& 
TrkHelixFitter::operator=(const TrkHelixFitter& right) 
  //------------------------------------------------------------------------
{
  if (&right == this) return *this;
  _allowFlips = right._allowFlips;
  _allowDrops = right._allowDrops;
  _lastChisq =  right._lastChisq;

  return *this;
}

//------------------------------------------------------------------------
void 
TrkHelixFitter::setFittingPar(bool allowFlips, bool allowDrops) {
  //------------------------------------------------------------------------
  _allowFlips = allowFlips;
  _allowDrops = allowDrops;
}

//------------------------------------------------------------------------
TrkErrCode 
TrkHelixFitter::fit(TrkHotList& hitlist,
    TrkSimpTraj& theTraj) {
  //------------------------------------------------------------------------
  // Assumes that weight matrix is diagonal. */
  /* Least-squares fit; the measured 
     quantity is the residual.  The fit is accomplished by linearizing 
     the equation, using the derivatives of the residual w/r/t the 
     track parameters; because of this approximation, the fit may be iterated. 
     The fitted parameters are given by:
     delta-param() = Vparam * Atran * Vyinv * delChi
     where Vyinv = covariance matrix for the measurements
     Atran = transpose of A
     A = matrix of derivatives of delChi wrt trk params 
     (size = no. of params x no. of hits)
     Vparam = covariance (error)" matrix of the fitted parameters
     = (Atran * Vyinv * A)**-1
     */

  bool permitFlips = _allowFlips;
  bool lPickHits = _allowDrops;
  // permitFlips = 1 => permit state changes like ambiguity flips
  // lPickHits = 1 => choose the best set of active hits on each iteration     
  int i;
  TrkErrCode status(TrkErrCode::succeed);
  int lPicked = 0;   // = 1 => have either picked up or dropped an active hit
  // on this iteration
  register double chisqold;
  double chisqnew, chichange;
  double chitest = 0.01; //delta(chi2) < chitest => fit has converged
  int nZ = 0, nXY = 0;    //  # active hits in each view
  int nActive = 0;

  // vparam = Vparam defined above ( = symmetric matrix)
  // diffsum = Atran * Vyinv * delChi defined above (column vector)
  // iter = iteration loop index
  // itermax = max number of iterations
  // delpar = change in parameters during this iteration
  // chisqold, chisqnew = chisq before and after latest iteration

  /***************************************************************************/
  setLastChisq(-1.);
  //bool shiftRef = false;//yzhang FIXME
  //  HepPoint3D storePoint;

  // Change reference point of trajectory to be at first hit -- reduces 
  //   numerical problems
  //  double oldT0 = hitlist[0]->parentTrack()->trackT0();
  //if (shiftRef) {
  //    double firstFlight = hitlist[0]->fltLen();
  //    double newTime = hitlist[0]->parentTrack()->fitResult()->arrivalTime(firstFlight);
  //    hitlist[0]->parentTrack()->resetT0(newTime);
  //    Point3D.here = theTraj.position(firstFlight);
  //
  //    storePoint = here;
  //    DifPoint dfPos;
  //    DifVector dfDir, dfDelDir;
  //    theTraj.getDFInfo(firstFlight, dfPos, dfDir, dfDelDir);
  //
  //    theTraj.changePoint(here, fltOffset);
  //}    

  //*** Things that don't change with each iteration 
  int nhits = hitlist.nHit();
  std::vector<double> delChi(nhits,0);
  std::vector<std::vector<double> > deriv(nhits);

  TrkParams &params = *(theTraj.parameters());
  //  int npar = params.nPar();//yzhang temp
  int npar = theTraj.nPar();//yzhang temp

  // Decide minimum numbers of hits required.  This could turn out to be wrong
  //     someday.

  bool l3d = (npar > 3);  // I hope always true
  const int minZ = l3d ? 2 : 0;
  const int minXY = npar - minZ;
  const int minAct = minZ + minXY;

  HepSymMatrix vparam(npar,0);
  HepVector diffsum(npar);
  HepVector delpar(npar);

  std::vector<std::vector<double> >::iterator ideriv = deriv.begin();
  std::vector<double>::iterator idelChi = delChi.begin();
  assert(((int)deriv.size()) ==(hitlist.end()-hitlist.begin()));
  for (TrkHotList::nc_hot_iterator ihit = hitlist.begin(); ihit != hitlist.end(); ++ihit,++ideriv,++idelChi) {
    ideriv->resize(npar);
    if (ihit->isActive()) {
      nActive++;
      if (ihit->whatView() == TrkEnums::xyView) nXY++;
      else if (ihit->whatView() == TrkEnums::zView) nZ++;
      else if (ihit->whatView() == TrkEnums::bothView) {
	nZ++;
	nXY++;
      }
    }

    // Update the Hots to reflect new reference point
    //if (shiftRef) {
    //      ihit->setFltLen( ihit->fltLen() - fltOffset );
    //}
  }  //end loop over hits
  if (nXY < minXY || nZ < minZ || nActive < minAct) {
    status.setFailure(11,"Not enough hits in TrkHelixFitter! ");
    return status;
  }


  //if (shiftRef) {
  //    double firstFlight = hitlist[0]->fltLen();
  //    Point3D.here = theTraj.position(firstFlight);
  //    DifPoint dfPos;
  //    DifVector dfDir, dfDelDir;
  //    theTraj.getDFInfo(firstFlight, dfPos, dfDir, dfDelDir);
  //    double dummy = 0.; 
  //    hitlist[0]->updateFitStuff(dummy, 0, !permitFlips);
  //    hitlist[0]->updateMeasurement();
  //    hitlist[0]->updateFitStuff(dummy, 0, !permitFlips);
  //}
  //  static HepVector derivs(npar);//yzhang temp
  HepVector derivs(npar);//zhang change
  //  HepVector derivs(npar);//yzhang temp
  TrkErrCode calcResult;
  //**** Iterate fit. 
  size_t itermax = 12;
  for (size_t iter = 1; iter <= itermax; iter++) {
    bool mustIterate(false);   // flag to force another iteration
    chisqold = 0.0;
    for (i = 0; i < npar; i++) diffsum[i] = 0.0;
    vparam *= 0.0;  // dumb way of clearing matrix

    /* Loop over hits, accumulate sums, calculate chisq for current params. */
    std::vector<std::vector<double> >::iterator ideriv = deriv.begin();
    std::vector<double>::iterator idelChi = delChi.begin();
    assert(((int)deriv.size())==(hitlist.end()-hitlist.begin()));
    for (TrkHotList::nc_hot_iterator ihit = hitlist.begin(); ihit != hitlist.end(); ++ihit,++ideriv,++idelChi) {

      // Ask the hit to do the calculations
      calcResult = updateMeasurement(*ihit,0,!permitFlips);
      double deltaChiNew;
      if (calcResult.success()) {
	if (iter < 2) { // FIXME? only update derivatives at first iteration...
	  calcResult = ihit->getFitStuff(derivs, deltaChiNew);
	  for (i=0; i<npar; ++i) (*ideriv)[i] = derivs[i];
	  //if(m_debug){
	  //  std::cout<<"in TrkHelixFitter "  <<std::endl;//yzhang deubg
	  //  cout << "deriv: ";
	  //  for (i=0;i<npar;++i) cout << (*ideriv)[i] << " " ;
	  //  cout << endl;
	  //}
	} else  {
	  calcResult = ihit->getFitStuff(deltaChiNew);
	}
      }
      if (calcResult.failure()) {
	if(m_debug){
	  cout<<"ErrMsg(warning) TrkHelixFitter:" 
	    << "unable to getFitStuff for hit " << *ihit << endl;
	}
	ihit->setUsability(false);  // something bombed
	continue;
      }
      mustIterate = (mustIterate || (calcResult.success() != 1));
      *idelChi = deltaChiNew;
      if(m_debug){
	cout << (ihit-hitlist.begin()); 
	ihit->print(std::cout);
	cout << " dChi " << *idelChi
	  << " amb "      << ihit->ambig() 
	  << " resid "    << ihit->resid() 
	  << " rms "      << ihit->hitRms() 
	  << " hitlen "   << ihit->hitLen() 
	  << " fltlen "   << ihit->fltLen()  << endl;
      }
      if (ihit->isActive() == false) {
	if(m_debug) std::cout<<"SKIP not active hit"<<  std::endl;
	continue;
      }
      chisqold += deltaChiNew * deltaChiNew;

      for (i = 0; i < npar; ++i) {
	diffsum[i] += (*ideriv)[i] * deltaChiNew;
	for (int j = 0; j < i+1; ++j) {
	  vparam.fast(i+1,j+1) += (*ideriv)[i] * (*ideriv)[j];
	}
      }
    }  // end loop over hits


    //****  Calculate new paramters
    int ierr;
    vparam.invert(ierr);
    if (ierr) {
      if(m_debug){
	cout<<"ErrMsg(warning) TrkHelixFitter:" 
	  << "Matrix inversion failed " << endl;
      }
      status.setFailure(12, "Matrix inversion failed in TrkHelixFitter");
      //break;
    }
    delpar = vparam * (-diffsum);
    if(m_debug){
      cout << " delpar = "<<delpar << endl;
    }
    // The following test relies on having a fixed location for phi0 in 
    //   all simple params; it should be made robust somehow!!!
    if (fabs(delpar[1]) > 1.) {
      if(m_debug){
	cout<<"ErrMsg(warning) TrkHelixFitter:" 
	  << "Pathological  fit " << endl;
      }
      status.setFailure(13, "Pathological fit in TrkHelixFitter.");
      //break;
    }

    for (i = 0; i < npar; ++i) params.parameter()[i] += delpar[i];
    if(m_debug){
      cout << " params "<<params.parameter() << endl;
    }

    //***** Loop through the hits again, calculating the approx change 
    //   in residuals and chisq., and picking which hits should be active 
    //      for next iteration.

    chisqnew = 0.0;
    lPicked = 0;
    double bigDelChi = 0.0;
    TrkHotList::nc_hot_iterator bigHit = hitlist.end();

    mustIterate = (mustIterate || (iter <= 2 && lPickHits));   // iterate until hit-dropping allowed
    ideriv = deriv.begin();
    idelChi = delChi.begin();
    for (TrkHotList::nc_hot_iterator ihit = hitlist.begin(); ihit != hitlist.end(); ++ihit,++ideriv,++idelChi) {
      if(m_debug) {
	ihit->print(std::cout);
      }
      if(!ihit->isUsable()){
	if(m_debug) { std::cout<<"hit NOT usable "<<  std::endl; }
	continue;
      }
      //double weight = ihit->weight(); // FIXME: why isn't weight used???
      for (i = 0; i < npar; i++) {
	*idelChi += (*ideriv)[i] * delpar[i];
      }
      if (ihit->isActive()) chisqnew += *idelChi * *idelChi;

      // Hit-picking
      if (!mustIterate && lPickHits) {
	double abDelChi = fabs(*idelChi);
	//yzhang fix nSigmaCut for each layers 2010-04-13 
	if (abDelChi <= nSigmaCut[ihit->layerNumber()] ) {
	  if(m_debug){
	    std::cout<< "abDelChi "<<abDelChi
	      <<"<?"<<nSigmaCut[ihit->layerNumber()] << std::endl;//yzhang debug
	  }
	  if (ihit->isActive() == 0) {
	    ihit->setActivity(1);  // reactivate hit
	    if(m_debug){ cout << "set ACTIVE, Added " << endl; }
	    lPicked = 1;
	    nActive++;
	    if (ihit->whatView() == TrkEnums::xyView) nXY++;
	    else if (ihit->whatView() == TrkEnums::zView) nZ++;
	    else if (ihit->whatView() == TrkEnums::bothView) {
	      nZ++;
	      nXY++;
	    }
	  }
	} else {
	  if (ihit->isActive()) {
	    if (abDelChi > bigDelChi) {
	      if(m_debug){ 
		std::cout<<"bigest set INACTIVE, abDelChi = "<<abDelChi
		  <<">"<<nSigmaCut[ihit->layerNumber()] <<" bigDelChi=" <<bigDelChi<< std::endl; }
	      bigDelChi = abDelChi;
	      bigHit = ihit;
	    }
	  }
	}
      } // end if iter > 2 (hit-picking)
    } //end loop over hits

    // Drop hit with worst residual
    if (lPickHits) {
      int lDrop = 0;
      if (bigHit != hitlist.end() && (nActive > minAct)) {
	if ( bigHit->whatView() == TrkEnums::xyView && nXY > minXY)  {
	  nXY--;
	  lDrop = 1;
	} else if ( bigHit->whatView() == TrkEnums::zView && nZ > minZ)  {
	  nZ--;
	  lDrop = 1;
	} else if ( bigHit->whatView() == TrkEnums::bothView && nZ > minZ && 
	    nXY > minXY) {
	  nZ--;
	  nXY--;
	  lDrop = 1;
	}
	if (lDrop == 1) {
	  lPicked = 1;
	  nActive--;
	  bigHit->setActivity(0);    // deactivate hit
	  if(m_debug){
	    std::cout<<"---deactivate hit!! delChi2="<<bigDelChi<<  std::endl;
	    std::cout<<"---";
	    bigHit->print(std::cout);
	    std::cout<<"--------------------!! "<<  std::endl;
	  } 
	}
      } 
    }// end if lPickHits

    /* Test for convergence. */
    chichange = chisqold - chisqnew;
    if(m_debug){
      cout << "chisq from "<<chisqold << " -> " << chisqnew << endl;
    }
    if (chichange < -0.5 && !mustIterate && lPicked == 0) {
      if(m_debug){
	cout<<"ErrMsg(warning)" << " blowing up: " << chichange << endl;
      }
      /* It's blowing up. */
      setLastChisq(chisqnew);
      status.setFailure(1);

      if(m_debug) std::cout<<"failure 1 "<<  std::endl;
      break;
    } else if (chichange < chitest && !mustIterate && lPicked ==0){
      // We converged.
      status.setSuccess(1); 
      setLastChisq(chisqnew);
      if(m_debug) std::cout<<"success 1 "<<  std::endl;
      break;
    }

    if (iter == itermax) {
      setLastChisq(chisqnew);
      status.setSuccess(2);
      if(m_debug) std::cout<<"success 2 "<<  std::endl;
    }
  }    /* end iteration loop */

  // store the error matrix  
  params.covariance() = vparam;

  // Attempt to calculate deltaChisq for this hit (compared to leaving hit 
  //     out of the fit).
  /*  chisqnew = 0;
      if (status.success()) {
      HepVector deltaAlpha(npar);
      for (ihit = 0; ihit < nhits; ihit++) {
      thisHot = hitlist(ihit);
      if (!thisHot->isActive()) continue;
      HepVector derivs(npar, 0);
      for (i = 0; i < npar; i++) {
      derivs[i] = deriv[ihit][i];
      }
      double weight = thisHot->weight();
      double resid = thisHot->resid();
      deltaAlpha = vparam * derivs;
      deltaAlpha *= (resid * weight);
  //      cout  << resid * resid * weight
  //           << "  " << resid * weight * dot(derivs, deltaAlpha) << endl;
  //      thisHot->chi2Contrib = -dot(deltaAlpha, temp) + resid * resid * weight
  //        + 2. * resid * weight * dot(derivs, deltaAlpha);
  }
  }
  */

  // Change reference point back to origin
  //if (shiftRef) {
  //    Point3D.home(0.,0.,0.);
  //    theTraj.changePoint(home, fltOffset);
  //    hitlist[0]->parentTrack()->resetT0(oldT0);
  //for (ihit = 0; ihit < nhits; ihit++) {
  //      thisHot = hitlist[ihit];
  //      thisHot->setFltLen( thisHot->fltLen() - fltOffset );
  // }
  //}
  return status;
}
