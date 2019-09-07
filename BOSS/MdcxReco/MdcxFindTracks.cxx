//-------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxFindTracks.cxx,v 1.19 2011/12/08 06:52:29 zhangy Exp $
//
// Description: 
//	Class Implementation for track finding
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      S.  Wagner
//      Zhang Yao(zhangyao@ihep.ac.cn) Migration for BESIII
//
// Copyright Information:
//	Copyright (C) 1995	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#include <math.h>
#include "MdcxReco/MdcxFindTracks.h"
#include "MdcxReco/MdcxParameters.h"
#include "MdcxReco/Mdcxprobab.h"
#include "MdcxReco/MdcxHit.h"
#include "MdcGeom/Constants.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "GaudiKernel/NTuple.h"

#define GET_NAME(n) #n
#define prt(n) <<setw(8)<<GET_NAME(n)<<" "<<setw(8)<<n<<endl

extern int g_eventNo;
extern AIDA::IHistogram1D*  g_omegag;
extern AIDA::IHistogram2D*  g_dPhiAU;
extern AIDA::IHistogram2D*  g_dPhiAV;
extern AIDA::IHistogram1D*  g_trkllmk;
extern AIDA::IHistogram1D*  g_trklcircle;
extern AIDA::IHistogram1D*  g_trklgood;
extern AIDA::IHistogram1D*  g_trklhelix;
extern AIDA::IHistogram1D*  g_trkldrop1;
extern AIDA::IHistogram1D*  g_trkldrop2;
extern AIDA::IHistogram1D*  g_trklappend1;
extern AIDA::IHistogram1D*  g_trklappend2;
extern AIDA::IHistogram1D*  g_trklappend3;
extern AIDA::IHistogram1D*  g_trklfirstProb;
extern AIDA::IHistogram1D*  g_trkltemp;
extern AIDA::IHistogram1D*  g_trklproca;
extern AIDA::IHistogram1D*  g_trkld;
extern AIDA::IHistogram1D*  g_trkle;
extern AIDA::IHistogram1D*  g_trkldoca;
extern AIDA::IHistogram1D*  g_trkllayer;
extern AIDA::IHistogram2D*  g_trkldl;
extern AIDA::IHistogram2D*  g_trklel;
extern AIDA::IHistogram2D*  g_dropHitsSigma;
//extern AIDA::IHistogram1D*  g_trklprocaSl;
extern NTuple::Tuple*  m_xtupleAddSeg1;
extern NTuple::Item<long>             m_addSegSame;
extern NTuple::Item<double>           m_addSegSeedSl;
extern NTuple::Item<double>           m_addSegSeedPhi;
extern NTuple::Item<double>           m_addSegSeedPhiLay;
extern NTuple::Item<double>           m_addSegSeedD0;
extern NTuple::Item<double>           m_addSegSeedLen;
extern NTuple::Item<double>           m_addSegSeedPhi0;
extern NTuple::Item<double>           m_addSegAddSl;
extern NTuple::Item<double>           m_addSegAddPhi;
extern NTuple::Item<double>           m_addSegAddPhiLay;
extern NTuple::Item<double>           m_addSegAddD0;
extern NTuple::Item<double>           m_addSegAddLen;
extern NTuple::Item<double>           m_addSegAddPhi0;

extern NTuple::Tuple* m_xtupleAddSeg2;
extern NTuple::Item<long>             m_addSegEvtNo;
extern NTuple::Item<double>           m_addSegPoca;
extern NTuple::Item<long>             m_addSegSlayer;
extern NTuple::Item<double>           m_addSegLen;

extern NTuple::Tuple*  m_xtupleSegComb;
extern NTuple::Item<double>           m_segCombOmega;
extern NTuple::Item<double>           m_segCombSameAU;
extern NTuple::Item<double>           m_segCombSameUV;
extern NTuple::Item<double>           m_segCombDLenAU;
extern NTuple::Item<double>           m_segCombDLenUV;
extern NTuple::Item<double>           m_segCombSlA;
extern NTuple::Item<double>           m_segCombSlU;
extern NTuple::Item<double>           m_segCombSlV;
extern NTuple::Item<double>           m_segCombPhiA;
extern NTuple::Item<double>           m_segCombPhiU;
extern NTuple::Item<double>           m_segCombPhiV;
extern NTuple::Item<double>           m_segCombPoca;

extern NTuple::Tuple* m_xtupleDropHits;
extern NTuple::Item<long>             m_segDropHitsEvtNo;
extern NTuple::Item<long>             m_segDropHitsLayer;
extern NTuple::Item<long>             m_segDropHitsWire;
extern NTuple::Item<double>           m_segDropHitsPull;
extern NTuple::Item<double>           m_segDropHitsDoca;
extern NTuple::Item<double>           m_segDropHitsSigma;
extern NTuple::Item<double>           m_segDropHitsDrift;
extern NTuple::Item<double>           m_segDropHitsMcTkId;

// IOS_USING_DECLARATION_MARKER - BesIII iostreams migration, do not touch this line!
using std::cout;
using std::endl;
using std::ostream;

MdcxFindTracks::MdcxFindTracks(){ 
  probmin = MdcxParameters::minTrkProb;
  curvmax = 1000000.0;
}

MdcxFindTracks::MdcxFindTracks(const HepAList<MdcxSeg> &segList, int debug){ 
  probmin = MdcxParameters::minTrkProb;
  curvmax = 1000000.0;
  m_debug = debug;
  process(segList); 
}

MdcxFindTracks::~MdcxFindTracks() { KillList(); }

void MdcxFindTracks::process(const HepAList<MdcxSeg> &segList) {
  static int   nfound  = 0;
  static float sumprob = 0.0; 

  MdcxHel thel(0., 0., 0., 0., 0.);
  mhel = thel;
  int nSeg = segList.length();
  for (int i = 0; i < nSeg ; i++) { segList[i]->SetUsedOnHel(0); }

  double xp, yp, xl1, yl1, rl1, xl2, yl2, rl2;
  double d0g, phi0g, omegag, z0g, tanlg;
  double d0g_sl, phi0g_sl, omegag_sl, z0g_sl, tanlg_sl;
  double zintAU, zintAV, zintAU_sl, zintAV_sl = 9999.;
  double rl1_sl, rl2_sl;
  double xc, yc;
  double sinPhi0g_sl, cosPhi0g_sl, xp_sl, yp_sl;
  double phiAx, phiStU, phiStV, xl1_sl, yl1_sl, xl2_sl, yl2_sl, xl1_0, yl1_0, xl2_0, yl2_0;
  double sx1, sy1, sx2, sy2;
  double x0g, y0g;  
  double fltAx, ztest, fltL_sl, ztest_sl;  
  int floop, trkCount= 0;

  //The main loop; first get the 3 SL combo we'll use to make a trial helix
  for (int iSegCombo = 0; iSegCombo < MdcxParameters::nSegCombo; iSegCombo++) {
    //FIXME optimize: group segList by superlayer ahead
    int axlay = MdcxParameters::findTrkGroup[iSegCombo][0];
    int stUlay = MdcxParameters::findTrkGroup[iSegCombo][1];
    int stVlay = MdcxParameters::findTrkGroup[iSegCombo][2];

    if(4 == m_debug) std::cout <<" Test combo: ("<< axlay<<","<<stUlay<<","<<stVlay <<")"<< std::endl; 

    float max_dPhiAU = MdcxParameters::maxDp12[iSegCombo];
    float max_dPhiAV = MdcxParameters::maxDp13[iSegCombo];

    //---------------------------------------------------------------------
    //  Axial SL segment furnishes initial guess for d0, phi0, omega
    //---------------------------------------------------------------------
    for (int iAx = 0; iAx < nSeg; iAx++) {
      bool b_useGood_stU = true; //yzhang add 2009-10-21 17:20:45
      bool b_useGood_stV = true; //yzhang add 2009-10-21 17:20:45
      if ((segList[iAx]->GetUsedOnHel() != 0) || (segList[iAx]->Fail() != 0) 
	  || (segList[iAx]->SuperLayer(0) != axlay) || (segList[iAx]->Origin() != -1) ) continue;

      if (4 == m_debug){
	std::cout<< "---------1.Ax seg------ No."<<iAx<< std::endl;
	segList[iAx]->printSegAll();
      }

      //Skip low pt track
      omegag = segList[iAx]->Omega();
      if (fabs(omegag) > MdcxParameters::maxTrkOmega) {//FIXME 0.2
	if (4 == m_debug) std::cout<< "SKIP by maxTrkOmega "<<fabs(omegag) << std::endl;
	continue;  
      }
      if(4 == m_debug) std::cout << " iAx omegag = " << omegag << std::endl;
      if(g_omegag) g_omegag->fill(omegag);

      const HepAList<MdcxHit>& hitsAx = segList[iAx]->XHitList();

      //Interpret segment as straight line also
      d0g_sl = segList[iAx]->D0_sl_approx();
      phi0g_sl = segList[iAx]->Phi0_sl_approx(); 
      omegag_sl = 0.0;

      //save c-tors; do it locally
      sinPhi0g_sl = -sin(phi0g_sl);
      cosPhi0g_sl = cos(phi0g_sl);
      xp_sl = segList[iAx]->Xref() + sinPhi0g_sl*d0g_sl;
      yp_sl = segList[iAx]->Yref() + cosPhi0g_sl*d0g_sl;
      d0g_sl = yp_sl*cosPhi0g_sl + xp_sl*sinPhi0g_sl;
      fltL_sl = segList[iAx]->Xref()*cosPhi0g_sl - segList[iAx]->Yref()*sinPhi0g_sl; 
      if(4 == m_debug) {
	std::cout << "--Ax :" prt(d0g_sl) prt(phi0g_sl) prt(omegag_sl) prt(sinPhi0g_sl) prt(xp_sl) prt(yp_sl) prt(fltL_sl) << std::endl; 
      }

      if (fabs(omegag) > Constants::epsilon) {
	MdcxHel ohel = TakeToOrigin(*segList[iAx]);
	omegag = ohel.Omega();
	phi0g = ohel.Phi0();
	d0g = ohel.D0(); 
	xc = ohel.Xc();
	yc = ohel.Yc();
	x0g = ohel.X0();
	y0g = ohel.Y0();
	phiAx = atan2(y0g-yc, x0g-xc);
	fltAx = dlen(-2, phi0g,-1, segList[iAx]->Phi0(), omegag); 
	//fltAx = dlen(phi0g,segList[iAx]->Phi0(), omegag); //yzhang TEMP 2011-10-17 
	if(4 == m_debug) {
	  std::cout <<"--Ax :"<< " ohel ";
	  ohel.print();
	}
      }

      //---------------------------------------------------------------------
      //First stereo SL segement and axial segment furnish one z intersection
      //---------------------------------------------------------------------
      //for (int iStU = 0; iStU < nSeg; iStU++ ) //yzhang delete 2009-10-21 18:13:50
      for (int iStU = -1; true; ) {//yzhang add
	if(!b_useGood_stU && (iStU >= (nSeg-1))) break;
	if(b_useGood_stU && (iStU >= (nSeg-1))){
	  iStU = -1;
	  b_useGood_stU = false;
	}
	iStU++;
	if ((segList[iAx]->GetUsedOnHel() != 0) 
	    || (segList[iStU]->GetUsedOnHel() != 0) 
	    || (segList[iStU]->Fail() != 0) 
	    || (segList[iStU]->SuperLayer(0) != stUlay) 
	    || (segList[iStU]->Origin() != -1) 
	    //yzhang add  2009-10-21 17:21:55
	    || (b_useGood_stU && (segList[iStU]->Nhits()< 4))
	   ){ continue;}

	if (4 == m_debug){
	  std::cout <<" Test combo: AUV "<< axlay<<","<<stUlay<<","<<stVlay << std::endl; 
	  std::cout<< "---------2.St U seg ------No."<<iStU<< std::endl;//yzhang debug
	  segList[iStU]->printSeg();
	  std::cout<<" omega of seg iStU = "<<segList[iStU]->Omega()<<std::endl;
	}

	//Test dPhi between Ax slayer and StU slayer 
	const HepAList<MdcxHit>& hitsStU = segList[iStU]->XHitList();
	double dPhiAU = fabs(hitsAx[0]->phiMid() - hitsStU[0]->phiMid());
	if (dPhiAU > Constants::pi) dPhiAU = Constants::twoPi - dPhiAU;
	if(4 == m_debug) {
	  if (dPhiAU > max_dPhiAU){
	    cout << "SKIP by dPhiAU " <<dPhiAU <<" > "<< max_dPhiAU << endl;
	  }else{
	    cout << "KEEP by dPhiAU " <<dPhiAU<< " <= " << max_dPhiAU << endl;
	  }
	  std::cout << " phi mid Ax "<<hitsAx[0]->phiMid()
	    <<" StU "<<hitsStU[0]->phiMid() << std::endl; 
	  std::cout<<  std::endl;
	}
	if (g_dPhiAU) g_dPhiAU->fill(dPhiAU,segList[iStU]->SuperLayer());

	if (dPhiAU > max_dPhiAU){ continue; }

	//Calculate z by Ax and StU segs
	xl1_0 = segList[iStU]->Xline_bbrrf();
	yl1_0 = segList[iStU]->Yline_bbrrf();
	sx1   = segList[iStU]->Xline_slope();
	sy1   = segList[iStU]->Yline_slope();
	if (4 == m_debug) std::cout prt(xl1_0) prt(yl1_0) prt(sx1) prt(sy1) prt(omegag) << std::endl;
	if (fabs(omegag) >Constants::epsilon) {
	  zintAU = findz_cyl(iAx, iStU, segList);
	  xl1 = xl1_0 + sx1*zintAU;
	  yl1 = yl1_0 + sy1*zintAU;
	  rl1 = sqrt(xl1*xl1 + yl1*yl1);
	  phiStU = atan2(yl1-yc, xl1-xc);
	  if (4 == m_debug) cout prt(zintAU) prt(xl1) prt(yl1) prt(rl1) prt(phiStU) <<endl;
	} else {
	  zintAU = -9999.;
	}

	//Calculate z_sl by Ax and StU segs
	zintAU_sl = findz_sl(iAx, iStU, segList); 
	xl1_sl = xl1_0 + sx1*zintAU_sl;
	yl1_sl = yl1_0 + sy1*zintAU_sl;
	rl1_sl = sqrt(xl1_sl*xl1_sl + yl1_sl*yl1_sl);
	if (4 == m_debug) cout prt(zintAU_sl) prt(xl1_sl) prt(yl1_sl) prt(rl1_sl) <<endl;

	//Cut by z and z_sl
	if ( (zintAU < -MdcxParameters::maxMdcZLen) &&
	    (zintAU_sl < -MdcxParameters::maxMdcZLen) ) {
	  if (4 == m_debug) std::cout << " SKIP by zintAU < max MdcZLen "<<MdcxParameters::maxMdcZLen << std::endl; 
	  continue;
	}
	if ( (zintAU > MdcxParameters::maxMdcZLen) &&
	    (zintAU_sl > MdcxParameters::maxMdcZLen) ) {
	  if (4 == m_debug) std::cout << " SKIP by zintAU > max MdcZLen "<<MdcxParameters::maxMdcZLen << std::endl; 
	  continue;
	}

	//---------------------------------------------------------------------
	//Next stereo SL segement and axial segment furnish other z intersection
	//---------------------------------------------------------------------
	for (int iStV = -1; true; ) {
	  if(!b_useGood_stV && (iStV >= (nSeg-1))) {
	    if (4 == m_debug) std::cout << iStV <<" no segments in V slayer "<< std::endl; 
	    break;
	  }
	  if(b_useGood_stV && (iStV >= (nSeg-1))){
	    iStV = -1;
	    b_useGood_stV = false;
	  }
	  iStV++;
	  if ((segList[iStV]->GetUsedOnHel()!=0) 
	      || (segList[iStU]->GetUsedOnHel()!=0) 
	      || (segList[iAx]->GetUsedOnHel()!=0) 
	      || (segList[iStV]->Fail() != 0) 
	      || (segList[iStV]->SuperLayer(0) != stVlay) 
	      || (segList[iStV]->Origin() != -1)  
	      //yzhang add  2009-10-21 17:21:55
	      || (b_useGood_stV && (segList[iStV]->Nhits()< 4))
	     ){ continue; }

	  if (4 == m_debug){
	    std::cout <<" Test combo: AUV "<< axlay<<","<<stUlay<<","<<stVlay << std::endl; 
	    std::cout<< "---------3.St V seg ------No."<<iStV<< std::endl;//yzhang debug
	    segList[iStV]->printSeg();
	    std::cout<<" omega of seg iStV = "<<segList[iStV]->Omega()<<std::endl;
	  }

	  //Calculate dPhi between Ax and StV segs
	  const HepAList<MdcxHit>& hitsStV = segList[iStV]->XHitList();
	  double dPhiAV = fabs(hitsAx[0]->phiMid() - hitsStV[0]->phiMid());
	  if (dPhiAV > Constants::pi) dPhiAV = Constants::twoPi - dPhiAV;

	  if(4 == m_debug) {
	    if (dPhiAV > max_dPhiAV){
	      cout << "SKIP by dPhiAV " <<dPhiAV <<" > "<< max_dPhiAV << endl;
	    }else{
	      cout << "KEEP by dPhiAV " <<dPhiAV<< " <= " << max_dPhiAV << endl;
	    }
	    std::cout << " phi mid Ax "<<hitsAx[0]->phiMid()
	      <<" StV "<<hitsStV[0]->phiMid() << std::endl; 
	    std::cout<<  std::endl;
	  }
	  if (g_dPhiAV) g_dPhiAV->fill(dPhiAV,segList[iStV]->SuperLayer());
	  if (dPhiAV > max_dPhiAV) { continue; }


	  //Calculate z by Ax and StV segs
	  xl2_0 = segList[iStV]->Xline_bbrrf();
	  yl2_0 = segList[iStV]->Yline_bbrrf();
	  sx2   = segList[iStV]->Xline_slope();
	  sy2   = segList[iStV]->Yline_slope();
	  if (fabs(omegag) > Constants::epsilon) {
	    zintAV = findz_cyl(iAx, iStV, segList);
	    xl2 = xl2_0 + sx2*zintAV;
	    yl2 = yl2_0 + sy2*zintAV;
	    rl2 = sqrt(xl2*xl2 + yl2*yl2);
	    if (4 == m_debug){ 
	      segList[iAx]->printSeg();
	      segList[iStV]->printSeg();
	      cout << "zintAV " << zintAV << endl;
	    }
	    phiStV = atan2(yl2-yc, xl2-xc);
	  } else {
	    zintAV = -9999.;
	  }

	  //Calculate z by Ax and StV segs
	  zintAV_sl = findz_sl(iAx, iStV, segList);
	  xl2_sl = xl2_0 + sx2*zintAV_sl;
	  yl2_sl = yl2_0 + sy2*zintAV_sl; 
	  rl2_sl = sqrt(xl2_sl*xl2_sl + yl2_sl*yl2_sl);

	  //Cut by z of AV segs
	  if ( (zintAV < -MdcxParameters::maxMdcZLen) &&
	      (zintAV_sl < -MdcxParameters::maxMdcZLen) ) {
	    if (4 == m_debug) std::cout << " SKIP by zintAV < max MdcZLen "<<MdcxParameters::maxMdcZLen << std::endl; 
	    continue;
	  }
	  if ( (zintAV > MdcxParameters::maxMdcZLen) &&
	      (zintAV_sl > MdcxParameters::maxMdcZLen) ) {
	    if (4 == m_debug) std::cout << " SKIP by zintAV > max MdcZLen "<<MdcxParameters::maxMdcZLen << std::endl; 
	    continue;
	  }

	  //We now have enough info to form and fit a guess helix (inside iAx-iStU-iStV loops)
	  if (4 == m_debug) {
	    //cout << "KEEP layer[klm]: [" << axlay <<","<< stUlay ","<< stVlay << "] " prt(d0g) prt(phi0g) prt(omegag) zrt(zintAU) prt(zintAV) << endl; 
	    segList[iAx]->printSeg();
	    segList[iStU]->printSeg();
	    segList[iStV]->printSeg();
	  }
	  MdcxFittedHel fithel;
	  double helixFitSigma = MdcxParameters::helixFitSigma;
	  double first_prob = 0.0; 
	  HepAList<MdcxHit> hitlist;
	  hitlist.append(hitsAx);
	  hitlist.append(hitsStU);
	  hitlist.append(hitsStV);
	  if(g_trkllmk) for (int i=0; i<hitlist.length(); i++ ){ g_trkllmk->fill(hitlist[i]->Layer()); }


	  //---------------------------------------------------------------------
	  //Try fitting the trial helix from axial SL as circle first
	  //---------------------------------------------------------------------
	  floop = 1;
	  while (floop) {
	    if(4 == m_debug) std::cout<< "---------4.Ax circle fit------"<< std::endl;//yzhang debug
	    if (fabs(omegag) < Constants::epsilon) {
	      if(4 == m_debug) std::cout<<"SKIP by omegag==0   "<<std::endl;
	      break;
	    }
	    if ( (zintAU < -MdcxParameters::maxMdcZLen) ||
		(zintAU > MdcxParameters::maxMdcZLen) ){
	      if(4 == m_debug) std::cout<<"SKIP by zintAU out of range "<<zintAU<<" "<<MdcxParameters::maxMdcZLen<<std::endl;
	      break; 
	    }
	    if ( (zintAV < -MdcxParameters::maxMdcZLen) ||
		(zintAV > MdcxParameters::maxMdcZLen) ){
	      if(4 == m_debug) std::cout<<"SKIP by zintAU out of range "<<zintAU<<" "<<MdcxParameters::maxMdcZLen<<std::endl;
	      break; 
	    }


	    //Calculate flight length of AUV segs
	    if(4 == m_debug) cout<< "dlen calc. slay U "<<segList[iStU]->SuperLayer()<< " slay V "<<segList[iStV]->SuperLayer()<<endl;
	    double fltLenUV = dlen(segList[iStU]->SuperLayer(), phiStU,segList[iStV]->SuperLayer(), phiStV, omegag);//yzhang change TEMP 2011-10-17 
	    //double fltLenUV = dlen(phiStU, phiStV, omegag);
	    if(4 == m_debug){
	      cout prt(fltLenUV) prt(phiStU) prt(phiStV) prt(omegag)<< std::endl;
	    }
	    if (fltLenUV > MdcxParameters::maxDlen) {//15. to 150.
	      if(4 == m_debug) std::cout<<"SKIP by fltLenUV > "<<MdcxParameters::maxDlen<<std::endl;
	      break;   //FIXME
	    }
	    tanlg = (zintAV - zintAU)/fltLenUV; 
	    if(4 == m_debug) cout<< "dlen calc. slay A "<<segList[iAx]->SuperLayer()<< " slay U "<<segList[iStU]->SuperLayer()<<endl;
	    double fltLenAU = dlen(segList[iAx]->SuperLayer(), phiAx, segList[iStU]->SuperLayer(), phiStU, omegag);//yzhang change TEMP 2011-10-17 

	    if(m_xtupleSegComb){
	      m_segCombOmega = omegag;
	      m_segCombSameAU = testFromSameTrack(segList[iAx],segList[iStU]);
	      m_segCombSameUV = testFromSameTrack(segList[iStU],segList[iStV]);
	      m_segCombSlA= segList[iAx]->SuperLayer();
	      m_segCombSlU= segList[iStU]->SuperLayer();
	      m_segCombSlV= segList[iStV]->SuperLayer();
	      m_segCombPhiA = phiAx;
	      m_segCombPhiU = phiStU;
	      m_segCombPhiV = phiStV;
	      m_segCombDLenUV = fltLenUV;
	      m_segCombDLenAU = fltLenAU;
	      m_xtupleSegComb->write();
	    }
	    //double fltLenAU = dlen(phiAx, phiStU, omegag);
	    z0g = zintAU - fltLenAU*tanlg;
	    if (4 == m_debug) cout prt(z0g) prt(tanlg) prt(fltLenAU) prt(zintAU) prt(zintAV)<< endl;

	    //Cut by z0g
	    if ((z0g < -MdcxParameters::maxMdcZLen) || (z0g > MdcxParameters::maxMdcZLen)) {
	      if (4 == m_debug) std::cout<<"SKIP by z0g out of range "<<z0g<<">"<<MdcxParameters::maxMdcZLen<<std::endl;
	      break;
	    }
	    ztest = z0g + fltAx*tanlg; 
	    if (4 == m_debug) std::cout prt(ztest) prt(fltAx)<<std::endl;

	    //---Helix fitting
	    //MdcxHel ghel(d0g,phi0g,omegag,z0g,tanlg); // ghel.print();
	    MdcxHel ghel(segList[iAx]->D0(), segList[iAx]->Phi0(), segList[iAx]->Omega(),
		ztest, tanlg, 0.0, 11111, 0, segList[iAx]->Xref(), segList[iAx]->Yref()); 
	    //zoujh?: ghel.SetTurnFlag(1); // ghel.print();
	    MdcxFittedHel firstfit(hitlist,ghel,helixFitSigma);  
	    first_prob = firstfit.Prob(); 
	    if (4 == m_debug) {
	      std::cout<<" after first fit:  "<<std::endl;
	      firstfit.FitPrint();
	    }
	    if (first_prob >= probmin) {
	      MdcxHel helixOrigin= TakeToOrigin(firstfit);
	      MdcxFittedHel nextfit(hitlist,helixOrigin,helixFitSigma);
	      first_prob = nextfit.Prob();
	      fithel=nextfit;
	      if (g_trklgood) for (int i=0; i<nextfit.Nhits(); i++){ g_trklgood->fill(nextfit.Layer(i));}
	    }
	    if ( g_trklfirstProb) g_trklfirstProb->fill(first_prob);
	    if (4 == m_debug) cout << " prob after helix fitting = " << first_prob << endl;     
	    floop = 0;
	  }//end fitting trial helix

	  //---------------------------------------------------------------------
	  // Try fitting straight line trial helix if fit from axial-SL-as-circle trial
	  //  if helix not good enough
	  //---------------------------------------------------------------------
	  floop = 1;
	  while (floop) {
	    if (4 == m_debug) std::cout<< "---------4.2 straight line fit------"<< std::endl;//yzhang debug
	    if (4 == m_debug) cout << " zintAU_sl " << zintAU_sl << " zintAV_sl " << zintAV_sl << endl; 
	    if (first_prob > probmin) {
	      if (4 == m_debug) cout << " helix fit good , exit straight line fit." << endl;     
	      break;
	    }

	    if ( (zintAU_sl < -MdcxParameters::maxMdcZLen) ||
		(zintAU_sl > MdcxParameters::maxMdcZLen) ) break;
	    if ( (zintAV_sl < -MdcxParameters::maxMdcZLen) ||
		(zintAV_sl > MdcxParameters::maxMdcZLen) ) break;

	    double dx = xl2_sl - xl1_sl;
	    double dy = yl2_sl - yl1_sl;
	    double dl = sqrt(dx*dx + dy*dy);
	    tanlg_sl = (zintAV_sl - zintAU_sl)/dl;
	    dx = xl1_sl + d0g_sl*sin(phi0g_sl);
	    dy = yl1_sl - d0g_sl*cos(phi0g_sl);
	    dl = sqrt(dx*dx + dy*dy);
	    z0g_sl = zintAU_sl - dl*tanlg_sl;
	    if (4 == m_debug) cout prt(d0g_sl) prt(phi0g_sl) prt(z0g_sl) prt(tanlg_sl)<< endl;

	    if ((z0g_sl < -MdcxParameters::maxMdcZLen) || (z0g_sl > MdcxParameters::maxMdcZLen)){
	      if(4 == m_debug) std::cout << "SKIP by z0g_sl:"<<z0g_sl<<" > "<<MdcxParameters::maxMdcZLen << std::endl; 
	      break;
	    }
	    //MdcxHel ghel(d0g_sl,phi0g_sl,omegag_sl,z0g_sl,tanlg_sl); // ghel.print();
	    ztest_sl = z0g_sl + fltL_sl*tanlg_sl;
	    if (4 == m_debug) cout prt(ztest_sl) << endl;

	    //Helix fitting
	    MdcxHel ghel(segList[iAx]->D0_sl_approx(), segList[iAx]->Phi0_sl_approx(), omegag_sl,
		ztest_sl, tanlg_sl, 0.0, 11111, 0, segList[iAx]->Xref(), segList[iAx]->Yref());
	    //zoujh?: ghel.SetTurnFlag(1); // ghel.print();
	    MdcxFittedHel firstfit(hitlist, ghel, helixFitSigma);
	    first_prob = firstfit.Prob();
	    //if (first_prob >= probmin)fithel=firstfit;
	    if(4 == m_debug){
	      ghel.print();
	      std::cout <<"first_prob "<<first_prob << std::endl;  
	    }

	    if (first_prob >= probmin) {
	      MdcxHel helixOrigin = TakeToOrigin(firstfit);
	      MdcxFittedHel nextfit(hitlist, helixOrigin, helixFitSigma);
	      first_prob = nextfit.Prob();
	      fithel = nextfit;
	      if (4 == m_debug) {
		cout << " three seg sl nextfit" <<  endl;
		nextfit.FitPrint();
	      }
	    }
	    floop = 0;
	  }

	  //-----------------------------------------------------------------
	  //Got a good trial helix fit; now try to add other segments onto it
	  //-----------------------------------------------------------------
	  floop = 1;
	  while (floop) {
	    floop = 0;
	    if(4 == m_debug)std::cout<< "---------5. try add seg to helix------"<< std::endl;
	    if (first_prob < probmin) {
	      if(4 == m_debug) std::cout<< "prob"<<first_prob<< " "
		<<probmin<<" fit NOT good, exit add segs "<< std::endl;
	      break;
	    }
	    float bchisq = fithel.Chisq()*helixFitSigma*helixFitSigma;
	    int   bndof = fithel.Nhits() - 5;
	    float bprob = Mdcxprobab(bndof, bchisq);
	    trkCount++; 
	    segList[iAx]->SetUsedOnHel(trkCount);
	    segList[iStU]->SetUsedOnHel(trkCount);
	    segList[iStV]->SetUsedOnHel(trkCount); 

	    if (4 == m_debug){
	      cout << "in add seg to trail helix, klm seg:" << endl;  ///ZOUJH
	      segList[iAx]->printSeg();                  ///ZOUJH
	      segList[iStU]->printSeg();                  ///ZOUJH
	      segList[iStV]->printSeg();                  ///ZOUJH
	    }

	    //Loop superlayers
	    for (int iSlay = 0; iSlay < Constants::nSuperLayer; iSlay++) {
	      int ilay = MdcxParameters::layerSet2AddSeg[iSegCombo][iSlay]; 
	      for (int iSeg = 0; iSeg < nSeg; iSeg++) {
		if ((segList[iSeg]->SuperLayer(0) != ilay) 
		    ||(segList[iSeg]->GetUsedOnHel() != 0)
		    || (segList[iSeg]->Fail() != 0) 
		    || (segList[iSeg]->Origin() != -1)) continue;
		if(4 == m_debug) {
		  std::cout<< endl<< "== add seg "<< iSeg<<" ";
		  segList[iSeg]->printSeg();
		}

		//Cut by phi_diff = (Phi_Ax - Phi_Add)
		float phiAxSeg = segList[iAx]->XHitList()[0]->phiMid();
		float phiAddSeg = segList[iSeg]->XHitList()[0]->phiMid();
		double phiKNSegDiff = fabs(phiAxSeg - phiAddSeg);
		//yzhang add 2011-10-11 
		//double phiKNSegDiff = (phiAxSeg - phiAddSeg);
		//if(phiKNSegDiff<-Constants::pi) phiKNSegDiff+=2*Constants::pi;
		//if(phiKNSegDiff>Constants::pi) phiKNSegDiff-=2*Constants::pi;
		int t_same=-999;
		if(m_xtupleAddSeg1){
		  //std::cout<< __FILE__ << " testFromSameTrack  " << fromSameTrack<< "   "<<std::endl;
		  t_same = testFromSameTrack(segList[iAx],segList[iSeg]);
		  m_addSegSame = t_same;
		  m_addSegSeedSl = segList[iAx]->SuperLayer();
		  m_addSegSeedPhi = segList[iAx]->XHitList()[0]->phiMid();
		  m_addSegSeedPhiLay = segList[iAx]->XHitList()[0]->Layer();
		  m_addSegSeedPhi0 = segList[iAx]->Phi0_sl_approx();
		  m_addSegSeedD0 = segList[iAx]->D0_sl_approx();
		  m_addSegAddSl = segList[iSeg]->SuperLayer();
		  m_addSegAddPhi = segList[iSeg]->XHitList()[0]->phiMid();
		  m_addSegAddPhiLay = segList[iSeg]->XHitList()[0]->Layer();
		  m_addSegAddPhi0 = segList[iSeg]->Phi0_sl_approx();
		  m_addSegAddD0 = segList[iSeg]->D0_sl_approx();
		  m_xtupleAddSeg1->write();
		}
		//yzhang change 2011-10-11 
		if (phiKNSegDiff > 0.5*Constants::pi && phiKNSegDiff < 1.5*Constants::pi ) 
		  //if (fabs(phiKNSegDiff) > 0.7*Constants::pi ) 
		{//yzhang TEMP
		  if(4 == m_debug) std::cout<< " SKIP by phi diff,same "<< t_same <<" Ax "<<
		    phiAxSeg<<" iSeg "<<phiAddSeg<<" diff "<<phiKNSegDiff << std::endl;//yzhang debug 
		  continue;  // zoujh FIXME
		}else{
		  if(4 == m_debug) std::cout<< " phi diff OK, Ax "<<
		    phiAxSeg<<" added "<<phiAddSeg<<" diff="<<phiKNSegDiff << std::endl;//yzhang debug 
		}

		//Calculate doca
		xp = segList[iSeg]->Xref() - segList[iSeg]->SinPhi0()*segList[iSeg]->D0();
		yp = segList[iSeg]->Yref() + segList[iSeg]->CosPhi0()*segList[iSeg]->D0();
		const HepAList<MdcxHit>& hitsSegAdd= segList[iSeg]->XHitList();
		double proca = fithel.Doca( hitsSegAdd[0]->wx(), hitsSegAdd[0]->wy(),
		    hitsSegAdd[0]->wz(), xp, yp );
		if (g_trklproca) g_trklproca->fill(proca);

		//Segment passes loose doca cut; does it belong on this track?
		if(m_xtupleAddSeg2){
		  m_addSegEvtNo = g_eventNo;
		  m_addSegPoca = proca;
		  m_addSegSlayer = iSlay;
		  m_addSegLen = fithel.Doca_Len();
		  m_xtupleAddSeg2->write();
		}

		if (!((fabs(proca) < MdcxParameters::maxProca)&&(fithel.Doca_Len()<fithel.Lmax())) ){
		  //if (g_trklprocaSl) g_trklprocaSl->fill(segList[iSeg]->SuperLayer(0));
		  if(4 == m_debug){
		    std::cout<< " SKIP by fabs(proca):"<< fabs(proca)<< "<"<<MdcxParameters::maxProca <<" or Doca_Len:"<<fithel.Doca_Len() <<"<"<<fithel.Lmax()<< std::endl;//yzhang debug 
		  }
		}else{
		  if(4 == m_debug){
		    std::cout<< " proca & len OK, |proca| "<< fabs(proca)<< "<"<<MdcxParameters::maxProca <<" && Doca_Len:"<<fithel.Doca_Len() <<"<"<<fithel.Lmax()<< std::endl;//yzhang debug 
		  }
		}
		if(fithel.Doca_Len()<0) continue;//yzhang add TEMP 2011-10-11 

		if ((fabs(proca)<MdcxParameters::maxProca)&&(fithel.Doca_Len()<fithel.Lmax())) {
		  MdcxFittedHel newhel;
		  newhel.Grow(fithel, hitsSegAdd);
		  if (newhel.Nhits() != hitlist.length()) {
		    if (4 == m_debug){
		      cout<<" rcs "<<newhel.Rcs()<<"<=?"<<MdcxParameters::maxRcsInAddSeg<< endl;
		    }
		    //yzhang change 2009-10-21 FIXME
		    //if (newhel.Rcs() > MdcxParameters::maxRcsInAddSeg)
		    if (newhel.Prob() < probmin){
		      if(4 == m_debug){ 
			cout << " prob " << newhel.Prob() << "<"<<probmin<<", ReFit" << endl;  ///ZOUJH
		      }
		      newhel.ReFit(); //FIXME
		    }
		    if(4 == m_debug) {
		      cout<<" refit prob "<<newhel.Prob()<<"<"<<probmin<<" Fail? "<<newhel.Fail()<< endl;
		    }
		    //yzhang change 2009-10-21 FIXME
		    if ( (newhel.Prob() >= probmin) && (newhel.Fail() == 0) ) {
		      //if ( (newhel.Rcs() <= MdcxParameters::maxRcsInAddSeg) && (newhel.Fail() == 0) ) 
		      fithel = newhel;
		      hitlist = newhel.XHitList();
		      segList[iSeg]->SetUsedOnHel(trkCount);
		      bchisq = fithel.Chisq()*helixFitSigma*helixFitSigma;
		      bndof = fithel.Nhits() - 5;
		      float tprob = Mdcxprobab(bndof, bchisq); 
		      if (tprob > bprob) bprob = tprob;
		      if (4 == m_debug) {
			cout << " segment ADDED, prob=" << newhel.Prob() << endl;
		      }
		    }	//*end trial track still good so keep new seg 
		    else{
		      if(4 == m_debug)std::cout<<" fit FAILED"<<std::endl;
		    }
		  } else { //*end new hits so refit
		    segList[iSeg]->SetUsedOnHel(trkCount);
		    if (4 == m_debug) cout << " segment ADDED, but no new hits" << endl;
		  }//*end dont need to refit if no new hits
		}//*end fabs(proca) < 0.010
	      }//*end iSeg (loop over segs in a superlayer)
	    }//*end iSlay (loop over superlayers)
	    if (((fithel.Prob() < 0.90) && (fithel.Nhits() == 12)) || (fithel.Fail() != 0)) {
	      for (int i = 0; i < nSeg; i++) { if (segList[i]->GetUsedOnHel() == trkCount) segList[i]->SetUsedOnHel(0); }
	      trkCount--;
	      break; //*jump to end floop first_prob >= probmin
	    }

	    if(4 == m_debug){
	      std::cout<< "Track after add segs" << std::endl;//yzhang debug
	      fithel.FitPrint();
	    }

	    //See if helix better moving backwards
	    if(4 == m_debug) std::cout<< "---------6. flip------"<< std::endl;//yzhang debug
	    fithel.flip();

	    //See if this track shares lots of hits with another saved track
	    int kki = 0;
	    int notduplicate = 1; 
	    while(MdcxTrklist[kki]) {
	      if (4 == m_debug)std::cout<< "---------7. test saved track------"<< std::endl;//yzhang debug
	      const HepAList<MdcxHit>& junk=MdcxTrklist[kki]->XHitList();
	      int overl = 0; 
	      int kkj = 0;
	      while (junk[kkj]) {
		int kkl = 0;
		while (hitlist[kkl]) {
		  if (hitlist[kkl] == junk[kkj]) overl++; 
		  kkl++;
		}
		kkj++;
	      }
	      if (4 == m_debug) cout << "overlap with track # " << kki << " is "
		<< junk.length() << " hitList " << hitlist.length() << " overlap " << overl << endl;
	      if ( (hitlist.length()/4.) <= overl) notduplicate = 0;
	      //yzhang 2009-10-21 16:30:53
	      //if ( hitlist.length() <= (overl+2) ) notduplicate = 0;
	      kki++;
	    }

	    if(g_trklhelix) for (int iHit=0; iHit<fithel.Nhits(); iHit++){ g_trklhelix->fill(fithel.Layer(iHit)); }

	    //If not a duplicate, is it worth saving?
	    if (notduplicate) {
	      if(4 == m_debug) std::cout<< "---------8. test worth saving?------"<< std::endl;//yzhang debug
	      MdcxFittedHel *finehel = new MdcxFittedHel(hitlist, fithel);

	      if(4 == m_debug) {
		std::cout<<__FILE__
		  <<" finehel Prob>0.001 "<<finehel->Prob()
		  <<" nhits "<<finehel->Nhits() <<">=15? "
		  <<" bprob "<< bprob 
		  <<" >=?probmin "<< probmin
		  <<" Fail==0? "<< finehel->Fail()
		  <<  std::endl;
		finehel->FitPrint();
	      }
	      //cut nhits from 15->9 yzhang 2009-10-21 
	      if (( (finehel->Prob()>0.001) || (finehel->Nhits()>=15) || (bprob>probmin) 
		    //|| (bchisq/bndof > MdcxParameters::maxRcsInAddSeg) 
		  ) && (finehel->Fail() == 0) ) {
		nfound++;
		sumprob += finehel->Prob();
		//if ( hitlist.length() == 16 )resout(finehel);

		//Begin hit dropping section
		//Set nodrop != 0 if want to prohibit dropping bad hits 
		int nodrop = 0; 
		if ((finehel->Prob() > 0.05) || nodrop ) {
		  MdcxTrklist.append(finehel);
		  //yzhang hist
		  for (int iHit=0; iHit<finehel->Nhits(); iHit++){
		    if(g_trklappend1) g_trklappend1->fill(finehel->Layer(iHit));//yzhang hist
		  }
		  //zhangy hist
		} else {
		  MdcxFittedHel* drophel = new MdcxFittedHel( drophits(finehel) );
		  float ratdrop = float(drophel->Nhits()) / float(finehel->Nhits());
		  if (4 == m_debug) cout << "APPEND track " << trkCount << ", drops " 
		    << finehel->Nhits()-drophel->Nhits() 
		      << " helixnhit "<<finehel->Nhits()<< " drophit "<<drophel->Nhits()
		      << " hits, drop rate="<<ratdrop <<">?"<<0.5
		      << " prob "<<drophel->Prob() <<" >?"<<finehel->Prob()
		      << " fail==0? "<<drophel->Fail()
		      << endl;
		  if ((drophel->Prob() > finehel->Prob()) && 
		      (ratdrop > 0.50) && (drophel->Fail() == 0)) {  //FIXME
		    if (4 == m_debug) cout << "append drop helix " <<  endl;
		    MdcxTrklist.append(drophel);
		    if(g_trklappend2) for (int iHit=0;iHit<drophel->Nhits();iHit++){g_trklappend2->fill(drophel->Layer(iHit));}
		    delete finehel;
		  } else {
		    if (4 == m_debug) cout << "append fine helix " <<  endl;
		    MdcxTrklist.append(finehel);
		    if(g_trklappend3)for(int iHit=0;iHit<finehel->Nhits();iHit++){g_trklappend3->fill(finehel->Layer(iHit));}
		    delete drophel;
		  }
		}

		//End hit dropping section
		int nwl = MdcxTrklist.length() - 1;
		if ((4 == m_debug) && (nwl > -1)) {
		  if (4 == m_debug) {
		    cout << endl << "found track " << trkCount<<std::endl;
		    MdcxTrklist[nwl]->print();
		    MdcxTrklist[nwl]->FitPrint();
		  }
		}
	      } else { //*endif good fit so add to MdcxTrklist
		for (int i = 0; i < nSeg; i++) {
		  if (segList[i]->GetUsedOnHel() == trkCount) segList[i]->SetUsedOnHel(0);
		}
		delete finehel;
		trkCount--;
	      }
	    }//*end if not duplicate 
	  }//*end floop first_prob >= probmin
	}// end iStV
      }// end iStU  
    }// end iAx
    if(4==m_debug) cout<< "end of this combo"<<endl;
  }// end iSegCombo
  if(4== m_debug) cout << " In MdcxFindTracks, found " << trkCount << " good tracks" << endl;
}//end of process

void MdcxFindTracks::print(ostream &o) {
  o<<"ngood:"<<ngood<<endl;
  o<<"npure:"<<npure<<endl;
  o<<"nbad:"<<nbad<<endl;
  o<<"nfail:"<<nfail<<endl;
  o<<"nhitsmc:"<<nhitsmc<<endl;
  o<<"nhitsgd:"<<nhitsgd<<endl;
} // endof print

void MdcxFindTracks::plot()const {
} // endof plot

void MdcxFindTracks::printpar(ostream &o) {
  o << "|MdcxFindTracks| parameters" << endl;
  o << "probmin:" << probmin << endl;
  o << "curvmax:" << curvmax << endl;
}//endof printpar

void MdcxFindTracks::beginmore(){
  ngood=0; nbad=0; nfail=0; npure=0; // zero scalars
  nhitsmc=0; nhitsgd=0;		// zero scalars
  printpar(cout);		// print parameters to screen
}//endof beginmore

void MdcxFindTracks::endmore(){
  print(cout);
}//endof endmore

double MdcxFindTracks::findz_sl(int iAx, int iStU, const HepAList<MdcxSeg> &segList){
  double Ap = -sin(segList[iAx]->Phi0_sl_approx());
  double Bp =  cos(segList[iAx]->Phi0_sl_approx());
  double xp = segList[iAx]->Xref() + Ap*segList[iAx]->D0_sl_approx();
  double yp = segList[iAx]->Yref() + Bp*segList[iAx]->D0_sl_approx();
  double xl = segList[iStU]->Xref() - segList[iStU]->SinPhi0()*segList[iStU]->D0(); 
  double yl = segList[iStU]->Yref() + segList[iStU]->CosPhi0()*segList[iStU]->D0();

  const HepAList<MdcxHit>& hitsStU = segList[iStU]->XHitList();
  double Cl = hitsStU[0]->wz();
  double ndotl = Bp*hitsStU[0]->wy() + Ap*hitsStU[0]->wx();
  /*
     std::cout<<"ndotl   "<<ndotl<<" "<<Bp<<" "<<hitsStU[0]->wy()<<" "<<
     Ap<<" "<<hitsStU[0]->wx()<<std::endl;
     */
  double zint = (ndotl != 0.0) ? ((Bp*(yp-yl)+Ap*(xp-xl))*Cl/ndotl) : -1000.;
  // cout << " findz_sl segs " << iAx << " " << iStU << " zint " << zint << endl;
  return zint;
}

double MdcxFindTracks::findz_cyl(int iAx, int iStU, const HepAList<MdcxSeg> &segList) {
  double zint = -1000.0;
  double r = 1.0/fabs(segList[iAx]->Omega());
  double xl_0 = segList[iStU]->Xref() - segList[iStU]->SinPhi0()*segList[iStU]->D0(); 
  double yl_0 = segList[iStU]->Yref() + segList[iStU]->CosPhi0()*segList[iStU]->D0();
  double sx = segList[iStU]->Xline_slope();
  double sy = segList[iStU]->Yline_slope();
  double cx = xl_0 - segList[iAx]->Xc();
  double cy = yl_0 - segList[iAx]->Yc();
  double a = sx*sx + sy*sy;
  double b = 2.0*(sx*cx + sy*cy);
  //double c = (cx*cx + cy*cy - r*r);
  double bsq = b * b;
  double fac = 4.0 * a * (cx*cx + cy*cy - r*r);
  double ta = 2.0 * a;
  if (fac < bsq) {
    double sfac = sqrt(bsq - fac);
    double z1   = -(b - sfac)/ta;
    double z2   = -(b + sfac)/ta;
    zint = (fabs(z1) < fabs(z2)) ? z1 : z2;
  }
  // cout << " findz_cyl segs " << iAx << " " << iStU << " zint " << zint << endl;
  return zint;
}

double MdcxFindTracks::dlen(int slay1, double p1, int slay2, double p2, double om){
  //double dp = p2 - p1;
  double dp;//yzhang change 2011-10-17 
  if((slay1==2 || slay1==3 || slay1 == 4 || slay1 == 9 || slay1 ==10) ){ //Ax
    dp = p2 - p1;
  }else if((slay2 >= slay1)) {
    dp = p2 - p1;
  }else {
    dp = p1 - p2;
  }

  if (om < 0.0) {
    while (dp < -Constants::pi) dp += Constants::pi;
    while (dp > 0.0)   dp -= 1*Constants::pi;
  } else {
    while (dp < 0.0)   dp += 1*Constants::pi;
    while (dp > Constants::pi)  dp -= 1*Constants::pi;
  }
  //double dl = dp * r;
  double dl = dp/om;
  //if(4 == m_debug) {
  //  double r = 1./om;
  //  cout prt(p1) prt(p2) prt(dp) prt(om) prt(r) prt(dl) << endl;
  //}
  return dl;
}

MdcxFittedHel MdcxFindTracks::drophits(MdcxFittedHel* finehel) {
  MdcxHel mdcxHel = (MdcxHel) *finehel;
  const HepAList<MdcxHit>& hitlist = finehel->XHitList();
  HepAList<MdcxHit> nwhitlist = hitlist;
  int nremove = 0;
  int kkk = 0;
  while(nwhitlist[kkk]) {
    double pull = nwhitlist[kkk]->pull(mdcxHel);
    int layer = nwhitlist[kkk]->Layer();

    if(m_xtupleDropHits){
      float t_doca = mdcxHel.Doca(*(nwhitlist[kkk]));
      float t_e =  nwhitlist[kkk]->e(t_doca);
      m_segDropHitsEvtNo = g_eventNo;
      m_segDropHitsLayer = layer;
      m_segDropHitsWire = nwhitlist[kkk]->WireNo();
      m_segDropHitsPull = pull;
      m_segDropHitsDoca = t_doca;
      m_segDropHitsSigma = t_e;
      m_segDropHitsDrift = nwhitlist[kkk]->d(mdcxHel); 
      m_segDropHitsMcTkId = nwhitlist[kkk]->getDigi()->getTrackIndex();
      m_xtupleDropHits->write();
    }

    if (fabs(pull) > MdcxParameters::dropHitsSigma[layer]){
      nwhitlist.remove(kkk);
      nremove++;
    } else {
      kkk++;
    }
  }
  if(m_debug==4) cout<< " MdcxFindTracks::drophits drop "<<nremove<<" hits "
    << " nhits of drop helix = "<<nwhitlist.length()<<endl;

  MdcxFittedHel newhel(nwhitlist,mdcxHel);
  return newhel;
}//endof drophits

void MdcxFindTracks::resout(MdcxFittedHel* finehel) {
  MdcxHel mdcxHel = (MdcxHel) *finehel;
  HepAList<MdcxHit>& hitlist = (HepAList<MdcxHit>&)finehel->XHitList();
  int kkk = 0; while(hitlist[kkk]){hitlist[kkk]->SetConstErr(0); kkk++;}
  MdcxFittedHel newhel(hitlist, mdcxHel);
  if(4 == m_debug) newhel.FitPrint(mhel,cout);
  kkk = 0;
  while(hitlist[kkk]) {
    float doca = newhel.Doca(*hitlist[kkk]);
    float zh   = newhel.Doca_Zh();
    float tof  = newhel.Doca_Tof();
    float dd   = hitlist[kkk]->d(newhel);
    float tt   = hitlist[kkk]->tcor(zh, tof);  
    if (4 == m_debug){
      cout << "MdcxFindTracks::resout (" 
	<< hitlist[kkk]->Layer() << "," 
	<< hitlist[kkk]->WireNo() << ") dt " 
	<< tt << " resid " 
	<< fabs(dd)-fabs(doca) << " doca " 
	<< fabs(doca) << endl;
    }
    kkk++;
  }
  kkk=0; 
  while(hitlist[kkk]){hitlist[kkk]->SetConstErr(1); kkk++;}
}//endof drophits

MdcxHel MdcxFindTracks::TakeToOrigin(MdcxHel& ihel) {
  double lng;
  double omegag = ihel.Omega();
  double phi0g = ihel.Phi0();
  double phi0s = phi0g; 
  double xp = ihel.Xref() - ihel.SinPhi0()*ihel.D0();
  double yp = ihel.Yref() + ihel.CosPhi0()*ihel.D0();
  double d0g = yp*cos(phi0g) - xp*sin(phi0g);
  if (omegag != 0.0) {
    phi0g = phi0g - Constants::pi;
    double xc = sin(phi0g)/omegag;
    double yc = -cos(phi0g)/omegag;
    double t1 = -xc - xp;
    double t2 = yc + yp;
    double phinew = atan2(t1, t2);
    if (omegag > 0.0) phinew += Constants::pi;
    if (phinew > Constants::pi) phinew -= Constants::twoPi;
    double xh = xc - sin(phinew)/omegag;
    double yh = yc + cos(phinew)/omegag;
    double x0 = xh + xp;
    double y0 = yh + yp;
    d0g = sqrt(x0*x0 + y0*y0);
    phi0g = phinew + Constants::pi;  ////???
    double sd0 = x0*sin(phi0g) - y0*cos(phi0g);
    if (sd0 > 0.0) d0g = -d0g;
    lng = dlen(-2, phi0g, -1, phi0s, omegag);//yzhang TEMP 2011-10-17  
    //lng = dlen(phi0g, phi0s, omegag); 
  } else {
    lng = ihel.Xref()*ihel.CosPhi0() + ihel.Yref()*ihel.SinPhi0(); 
  }
  double tanlg = ihel.Tanl();
  double z0g = ihel.Z0() - lng*tanlg; 
  //cout << " z0g, tanlg, lng " << z0g << " " << tanlg << " " << lng << endl;
  double t0g = ihel.T0();
  int codeg = ihel.Code(); 
  MdcxHel ohel(d0g, phi0g, omegag, z0g, tanlg, t0g, codeg);
  return ohel;
}

bool MdcxFindTracks::testFromSameTrack(MdcxSeg* seg1, MdcxSeg* seg2){
  /*
     std::cout<< " tkid seg1 "<<std::endl;
     for (int i =0; i<seg1->Nhits(); i++){
     int tkid = seg1->XHitList()[i]->getDigi()->getTrackIndex();
     std::cout<< tkid <<std::endl;
     }
     std::cout<< " tkid seg2 "<<std::endl;
     for (int i =0; i<seg2->Nhits(); i++){
     int tkid = seg2->XHitList()[i]->getDigi()->getTrackIndex();
     std::cout<< tkid <<std::endl;
     }
     */

  int trackId = -9999;
  for (int i =0; i<seg1->Nhits(); i++){
    int tkid = seg1->XHitList()[i]->getDigi()->getTrackIndex();
    if(tkid>=1000) tkid -=1000;
    if((trackId == -9999)){
      if(tkid>=0) trackId = tkid;
    }else{
      if(tkid!=trackId) return false;
    }
  } 
  for (int i =0; i<seg2->Nhits(); i++){
    int tkid = seg2->XHitList()[i]->getDigi()->getTrackIndex();
    if(tkid>=1000) tkid -=1000;
    if((trackId == -9999)){
      return false;
    }else{             
      if(tkid!=trackId) return false;
    }
  }
  return true;
}
