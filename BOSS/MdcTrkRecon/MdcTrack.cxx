//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcTrack.cxx,v 1.18 2012/08/13 00:09:19 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): 
//	Steve Schaffner
//	Zhang Yao(zhangyao@ihep.ac.cn)  Migrate to BESIII
//
//------------------------------------------------------------------------
#include "MdcTrkRecon/MdcTrack.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "MdcGeom/MdcLayer.h"
#include "MdcData/MdcHitOnTrack.h"
#include "MdcGeom/BesAngle.h"
#include "MdcGeom/Constants.h"
#include "CLHEP/Alist/AList.h"
#include "TrkBase/TrkRep.h"
#include "TrkFitter/TrkHelixRep.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkFitter/TrkCircleMaker.h"
#include "TrkBase/TrkContext.h"

#include "MdcData/MdcRecoHitOnTrack.h"
#include "BField/BField.h"
#include "MdcData/MdcHit.h"
#include "MdcData/MdcHitOnTrack.h"
#include "MdcRawEvent/MdcDigi.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
//************************************************************************/
MdcTrack::MdcTrack(TrkRecoTrk* aTrack){
//************************************************************************/
  _theTrack = aTrack;
  _firstLayer = _lastLayer = 0;
  _haveCurled = 0;
}

//************************************************************************/
MdcTrack::MdcTrack(int nsupers, const TrkExchangePar& par, double chisq, 
		     TrkContext& context, double trackT0) {
//************************************************************************/
    TrkCircleMaker maker;
    _theTrack = maker.makeTrack(par, chisq, context, trackT0);
    _firstLayer = _lastLayer = 0;
    _haveCurled = 0;
}

//************************************************************************/
MdcTrack::~MdcTrack() {
//************************************************************************/
  delete _theTrack;
}
//************************************************************************/
int MdcTrack::projectToR(double radius, BesAngle &phiIntersect, int lCurl) 
  const {
//************************************************************************/
  // note that these are currently circle-only routines

  //return -1 if no intersection
  
  const TrkFit* tkFit = track().fitResult();
  if (tkFit == 0) return -1;
  TrkExchangePar par = tkFit->helix(0.0);
  double d0 = par.d0();
  double omega = par.omega();
  double phi0 = par.phi0();
  double r2d2 = radius * radius - d0 * d0;
  if (r2d2 < 0) return -1;
  double rinv = 1./radius;
  double k2dinv = 1./(1 + omega * d0);
  if (!lCurl) {
    double arg = d0 * rinv + 0.5*omega * rinv * r2d2 * k2dinv;
    if (fabs(arg) > 1.0) return -1;
    phiIntersect.setRad( phi0 + asin(arg) );
  }
  else {
    double arg = -d0 * rinv - 0.5*omega * rinv * r2d2 * k2dinv;
    if (fabs(arg) > 1.0) return -1;
    phiIntersect.setRad( phi0 + Constants::pi + asin(arg) );
  }
   
  return 0;
}

//************************************************************************/
int MdcTrack::projectToR(double radius, BesAngle &phiIntersect, 
			    double &arcLength, int lCurl) const {
//************************************************************************/

  //return -1 if no intersection
  // Only valid for projecting track outwards from point of closest approach
  // Returns arclength from POCA

  const TrkFit* tkFit = track().fitResult();
  if (tkFit == 0) return -1;
  TrkExchangePar par = tkFit->helix(0.0);
  double d0 = par.d0();
  double omega = par.omega();
  double phi0 = par.phi0();
  double r2d2 = radius * radius - d0 * d0;
  if (r2d2 < 0) return -1;
  double rinv = 1./radius;
  double k2dinv = 1./(1 + omega * d0);
  if (k2dinv < 0.0) return -1;
  double arg;

  if (!lCurl) {
    arg = d0 * rinv + 0.5*omega * rinv * r2d2 * k2dinv;
    if (fabs(arg) > 1.0) return -1;
    phiIntersect.setRad( phi0 + asin(arg) );
  }
  else {
    arg = -d0 * rinv - 0.5*omega * rinv * r2d2 * k2dinv;
    if (fabs(arg) > 1.0) return -1;
    phiIntersect.setRad( phi0 + Constants::pi + asin(arg));
  }

  arg = 0.5*omega * sqrt(r2d2 * k2dinv);
  arcLength = 2. * asin(arg) / omega;

  return 0;
}

//----------------------------------------------------------------
bool 
MdcTrack::operator==(const MdcTrack& tk) const {
//----------------------------------------------------------------

  return (track() == tk.track());
}

//yzhang for store to TDS
void
MdcTrack::storeTrack(int trackId, RecMdcTrackCol* trackList, RecMdcHitCol* hitList,int tkStat){
    //check the fit succeeded
   // if (status().failure()) { return; }
    
    //get the results of the fit to this track
    const TrkFit* fitresult = track().fitResult();
    //check the fit worked
    if (fitresult == 0) return;
    
    //new a Rec. Track MdcTrack 
    RecMdcTrack* recMdcTrack = new RecMdcTrack();
    const TrkHitList* aList = track().hits();
    //some track info
    const BField& theField = track().bField();
    double Bz = theField.bFieldZ();
    //Fit info
    int hitId = 0;
    int nHits=0;
    int nSt=0;
    //int nAct=0; int nSeg=0; 
    //int maxlay = 0; int minlay = 43; int seg[11];/* 11 slayer */ int segme;
    //****************************
    //* active flag open this
    //****************************
    //* if (allHit>0){
    //*   nHits = aList->nHit();//add inActive hit also
    //* }else{
    //*   nHits = fitresult->nMdc();// = nActive()
    //* }
    //****************************
    //for 5.1.0 use all hits
    nHits = aList->nHit();
    //****************************
    // use active only
    // nHits = fitresult->nMdc();// = nActive()
    //****************************

    int    q        = fitresult->charge();
    double chisq    = fitresult->chisq();
    int    nDof     = fitresult->nDof();
    //track parameters at closest approach to beamline
    double fltLenPoca = 0.0;
    TrkExchangePar helix = fitresult->helix(fltLenPoca);
    //std::cout<< __FILE__ << " phi0  " << helix.phi0() << " omega  " <<helix.omega()<<std::endl;
    double phi0 = helix.phi0();
    double tanDip = helix.tanDip();
    //double z0 = helix.z0();
    double d0 = helix.d0();
    //momenta and position
    //CLHEP::Hep3Vector mom = fitresult->momentum(fltLenPoca);
    HepPoint3D poca = fitresult->position(fltLenPoca);

    double helixPar[5];
    //dro =-d0
    helixPar[0] = -d0;
    //phi0 = phi0 - pi/2   [0,2pi)
    double tphi0 = phi0 - Constants::pi/2.;
    if (tphi0 < 0. ) tphi0 += Constants::pi * 2.;
    helixPar[1] = tphi0;
    //kappa = q/pxy
    double pxy = fitresult->pt();
    if (pxy == 0.)  helixPar[2] = 9999.;
    else helixPar[2] = q/fabs(pxy);
    if(pxy>9999.) helixPar[2] = 0.00001;
    //dz = z0
    helixPar[3] = helix.z0();
    //tanl 
    helixPar[4] = tanDip;
    //error
    //V(Y)=S * V(X) * ST , mS = S , mVy = V(Y) , helix.covariance() = V(X)
    HepSymMatrix mS(helix.params().num_row(),0);
    mS[0][0]=-1.;//dr0=-d0
    mS[1][1]=1.;
    mS[2][2]=-333.567/Bz;//pxy -> cpa
    mS[3][3]=1.;
    mS[4][4]=1.;
    HepSymMatrix mVy = helix.covariance().similarity(mS);
    double errorMat[15];
    int k = 0;
    for (int ie = 0 ; ie < 5 ; ie ++){
      for (int je = ie ; je < 5 ; je ++) {
	errorMat[k] = mVy[ie][je];
	k++;
      }
    }
    double p,px,py,pz;
    px = pxy * (-sin(helixPar[1]));
    py = pxy * cos(helixPar[1]);
    pz = pxy * helixPar[4];
    p = sqrt(pxy*pxy + pz*pz);
    //theta, phi
    double theta = acos(pz/p);
    double phi = atan2(py,px);
    recMdcTrack->setTrackId(trackId);
    recMdcTrack->setHelix(helixPar); 
    recMdcTrack->setCharge(q);
    recMdcTrack->setPxy(pxy);
    recMdcTrack->setPx(px);
    recMdcTrack->setPy(py);
    recMdcTrack->setPz(pz);
    recMdcTrack->setP(p);
    recMdcTrack->setTheta(theta);
    recMdcTrack->setPhi(phi);
    recMdcTrack->setPoca(poca);
    recMdcTrack->setX(poca.x());//poca
    recMdcTrack->setY(poca.y());
    recMdcTrack->setZ(poca.z());
    recMdcTrack->setR(sqrt(poca.x()*poca.x() + poca.y()*poca.y()));
    HepPoint3D pivot(0.,0.,0.);
    recMdcTrack->setPivot(pivot);
    recMdcTrack->setVX0(0.);//pivot 
    recMdcTrack->setVY0(0.);
    recMdcTrack->setVZ0(0.);
    recMdcTrack->setError(errorMat);
    recMdcTrack->setError(mVy);
    recMdcTrack->setChi2(chisq);
    recMdcTrack->setNdof(nDof);
    recMdcTrack->setStat(tkStat);
    recMdcTrack->setNhits(nHits);
    //-----------hit list-------------
    HitRefVec  hitRefVec;
    vector<int> vecHits;
    //for fiterm
    int maxLayerId = -1;
    int minLayerId = 43;
    double fiTerm = 999.;
    const MdcRecoHitOnTrack* fiTermHot = NULL;
    TrkHitList::hot_iterator hot = aList->begin();
    for (;hot!=aList->end();hot++){
      const MdcRecoHitOnTrack* recoHot;   		  
      recoHot = dynamic_cast<const MdcRecoHitOnTrack*> (&(*hot));
      //if (!recoHot->mdcHit()) return;
      RecMdcHit* recMdcHit = new RecMdcHit;
      //id
      recMdcHit->setId(hitId);
      //---------BESIII----------
      //phiWire - phiHit <0 flagLR=0 left driftleft<0 ;
      //phiWire - phiHit >0 flagLR=1 right driftright>0;
      //flag = 2 ambig;
      //-----Babar wireAmbig----
      //-1 -> right, 0 -> don't know, +1 -> left
      //+1 phiWire-phiHit<0 Left,
      //-1 phiWire-phiHit>0 Right, 
      //0  don't know   
      //ambig() w.r.t track direction
      //wireAmbig() w.r.t. wire location 
      double hotWireAmbig = recoHot->wireAmbig();
      double driftDist = fabs(recoHot->drift());
      double sigma = recoHot->hitRms();	    
      double doca = fabs(recoHot->dcaToWire());
      int flagLR=2;
      if ( hotWireAmbig == 1){
	flagLR = 0; //left driftDist <0
	doca *= -1.;//2012-07-19 
      }else if( hotWireAmbig == -1){
	flagLR = 1; //right driftDist >0
      }else if( hotWireAmbig == 0){
	flagLR = 2; //don't know 
      }
      recMdcHit->setFlagLR(flagLR);
      recMdcHit->setDriftDistLeft((-1)*driftDist);
      recMdcHit->setErrDriftDistLeft(sigma);
      recMdcHit->setDriftDistRight(driftDist);
      recMdcHit->setErrDriftDistRight(sigma);
      //Mdc Id
      Identifier mdcId = recoHot->mdcHit()->digi()->identify();
      recMdcHit->setMdcId(mdcId);
      //corrected ADC

      //contribution to chisquare
      //fill chisq
      double res=999.,rese=999.;
      if (recoHot->resid(res,rese,false)){
      }else{}
      double deltaChi=0;
      recoHot->getFitStuff(deltaChi);//yzhang open 2010-09-20 
      recMdcHit->setChisqAdd( deltaChi * deltaChi );
      //set tracks containing this hit
      recMdcHit->setTrkId(trackId);
      //doca
      recMdcHit->setDoca(doca);//doca sign left<0
      //entrance angle
      
      recMdcHit->setEntra(recoHot->entranceAngle());
      //z of hit 
      HepPoint3D pos; Hep3Vector dir;
      double fltLen = recoHot->fltLen(); 
      recMdcHit->setFltLen(fltLen);
      recoHot->getParentRep()->traj().getInfo(fltLen,pos,dir);
      recMdcHit->setZhit(pos.z());
      double tof = recoHot->getParentRep()->arrivalTime(fltLen);
      recMdcHit->setTdc(recoHot->mdcHit()->tdcIndex());
      recMdcHit->setAdc(recoHot->mdcHit()->adcIndex()); 
      //drift time      
      recMdcHit->setDriftT(recoHot->mdcHit()->driftTime(tof,pos.z()));
      //for fiterm
      int layerId = recoHot->mdcHit()->layernumber();
      int wireId = recoHot->mdcHit()->wirenumber();
      //std::cout<<" MdcTrack::store() ("<<layerId<<","<<wireId<<") fltLen "<<fltLen<<" wireAmbig  "<<hotWireAmbig<<"    y "<<pos.y()<<std::endl;
	//<<recoHot->entranceAngle()<<std::endl;
      if (layerId >= maxLayerId){
	maxLayerId = layerId;
	fiTermHot = recoHot;
      } 
      if (layerId < minLayerId){
        minLayerId = layerId;
      }
      // status flag
      if (recoHot->isActive()) {
        recMdcHit->setStat(1);
      }else{
        recMdcHit->setStat(0);    
      }
      // for 5.1.0 use all hits
      if (recoHot->layer()->view()) {
        ++nSt;
      }
      hitList->push_back(recMdcHit);
      SmartRef<RecMdcHit> refHit(recMdcHit);
      hitRefVec.push_back(refHit);	    
      vecHits.push_back(mdcId.get_value());
      ++hitId;	      
    }
    //fi terminal angle
    if (fiTermHot!=NULL){
      fiTerm=(1./sqrt(1.+tanDip*tanDip))*fiTermHot->fltLen()*helix.omega();
    }
    recMdcTrack->setFiTerm(fiTerm);
    // number of  stereo hits contained
    recMdcTrack->setNster(nSt);  
    recMdcTrack->setFirstLayer(maxLayerId);  
    recMdcTrack->setLastLayer(minLayerId);  
    recMdcTrack->setVecHits(hitRefVec);
    trackList->push_back(recMdcTrack);
}//end of storeTrack
