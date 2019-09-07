//-------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxAddHits.cxx,v 1.10 2011/12/08 06:52:29 zhangy Exp $
//
// Description: 
//	Class Implementation for adding unused hits to tracks
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      S.  Wagner
//      Zhang Yao(zhangyao@ihep.ac.cn)  Migrate to BESIII
//      Zou Jiaoheng(zoujh@ihep.ac.cn)  
//
// Copyright Information:
//	Copyright (C) 1997	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#include <math.h>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "MdcxReco/MdcxAddHits.h"
#include "MdcxReco/MdcxHel.h"
#include "MdcxReco/MdcxHit.h"
#include "MdcxReco/MdcxFittedHel.h"
#include "MdcxReco/MdcxParameters.h"
#include "CLHEP/Alist/AIterator.h"
using std::cout;
using std::endl;
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

extern AIDA::IHistogram1D*  g_addHitCut;
extern AIDA::IHistogram2D*  g_addHitCut2d;

MdcxAddHits::MdcxAddHits(HepAList<MdcxFittedHel> &trkl, 
    const HepAList<MdcxHit> &hitl, float addit):
  ncalc(0), nadded(0), sumpull(0.0), thot(0), tuot(0)
{
  ntracks = trkl.length();
  nhits   = hitl.length();
  addcut  = addit;  
  // cout << "MdcxAddHits called with " << ntracks << " tracks, " 
  //      << nhits << " hits, addit = " << addit << endl;
  if ( (ntracks < 1) || (nhits < 1) ) return;
  kkk = 0;
  kkl = 0;
  while(hitl[kkl]) {
    hitl[kkl]->SetUsedOnHel(0);
    hhhh.append(hitl[kkl++]);
  }
  while(trkl[kkk]) {
    MdcxHel* thel = trkl[kkk];
    tttt.append(thel);
    const HepAList<MdcxHit>& dchitlist = trkl[kkk++]->XHitList();
    thot += dchitlist.length();
    kkl = 0;
    while (dchitlist[kkl]) dchitlist[kkl++]->SetUsedOnHel(1);
  }
}

MdcxAddHits::MdcxAddHits(HepAList<MdcxHel> &trkl, const HepAList<MdcxHit> &hitl, float addit):
  ncalc(0), nadded(0), sumpull(0.0)
{
  //
  //  c-tor designed to work with MdcxHitAdder; all MdcxHit's in hitl are
  //  assumed to be fresh and fair game.
  //
  ntracks = trkl.length();
  nhits   = hitl.length();
  addcut  = addit;  
  // cout << "MdcxAddHits called with " << ntracks << " tracks, " 
  //      << nhits << " hits, addit = " << addit << endl;
  if ( (ntracks < 1) || (nhits < 1) ) return;
  hhhh = hitl;
  tttt = trkl;
}

MdcxAddHits::~MdcxAddHits() { 
}

const HepAList<MdcxHit>& MdcxAddHits::GetAssociates(int whichtrack) {
  int debug = MdcxParameters::debug;

  listoasses.removeAll();  /// zoujh
  if ((whichtrack >= ntracks) || (whichtrack < 0)) {
    cout << "asked for associates of track " << whichtrack
      << " while there are only " << ntracks << " tracks in list " << endl;
    return listoasses;
  }

  double m_2pi = 2.0*M_PI;
  MdcxHel* temphel = tttt[whichtrack];

  if(5 == debug) temphel->print();

  double lmax = temphel->Lmax(); 

  //  calc. phim, phip
  double gvin = MdcxParameters::firstMdcAxialRadius;//yzhang 2010-05-05 
  double gvout = MdcxParameters::maxMdcRadius;
  double phiex = 0.38;  ///FIXME epsilon ? yzhang
  double phim = -M_PI;
  double phip = M_PI;
  double rmin = fabs(temphel->D0());
  double rmax, pc, rc, rhel;
  if (temphel->Ominfl()) {
    rmax = fabs(temphel->D0() + 2.0/temphel->Omega());
    double xc = temphel->Xc();
    double yc = temphel->Yc();
    pc = atan2(yc,xc);  
    rc = fabs(temphel->D0() + 1.0/temphel->Omega());
    rhel = fabs(1.0/temphel->Omega());
  } else {
    rmax = 1000.;
    pc = temphel->Phi0();
    rc = 1000.;
    rhel = 1000.;
  }
  if(5 == debug) {
    std::cout<< "==Test add hit phi: rmin >?"<<rmin << " gvin "<<gvin
      << " rmax >?"<<rmax << " gvout "<<gvout <<  std::endl;
  }

  if ((rmin<gvin) && (rmax>gvout)) {
    // case A (exiter)
    if(5 == debug) std::cout<<" case A (exiter) "<<std::endl;

    double len = 0;
    double lstep = m_2pi*rhel/16.;
    if (lstep>10.) lstep = 10.;
    double xl = temphel->Xh(len);
    double yl = temphel->Yh(len);
    double phil = atan2(yl, xl);
    double rl   = sqrt(xl*xl + yl*yl);
    int nstep = 0;
    double philast = phil;
    int isin   = 0;
    int notout = 1;
    while ((notout) && (nstep<20)) {
      len += lstep;
      nstep++;
      xl = temphel->Xh(len);
      yl = temphel->Yh(len);
      phil = atan2(yl, xl);
      rl = sqrt(xl*xl + yl*yl);
      if ((rl<gvin) && (!isin)) {
	philast = phil;
      } else if ((rl>gvin) && (!isin)) {
	isin = 1; phim = philast; phip = philast;
      }
      if (isin) {
	double deltap = phil - philast;
	if (deltap >  M_PI) phil -= m_2pi;
	if (deltap < -M_PI) phil += m_2pi;
	philast = phil;
	if (rl > gvout) notout = 0;
	if (phil < phim) phim = phil;
	if (phil > phip) phip = phil;
      }
    }//end while
  } else if ((rmin>gvin) && (rmax>gvout)) {
    if(5 == debug) std::cout<<" case B (albedo) "<<std::endl;
    // case B (albedo)
    double len=0; double lstep=m_2pi*rhel/16.; if (lstep>10.)lstep=10.;
    double xl=temphel->Xh(len); double yl=temphel->Yh(len);
    double phil=atan2(yl,xl); double rl=sqrt(xl*xl+yl*yl); 
    phim=phil; phip=phil; double phis=phil; double deltap,dp1,dp2;
    int nstep=0; double philast=phil;
    while ((rl<gvout)&&(nstep<20)){
      len+=lstep; nstep++; xl=temphel->Xh(len); yl=temphel->Yh(len); 
      phil=atan2(yl,xl); rl=sqrt(xl*xl+yl*yl);
      deltap=phil-philast; if (deltap>M_PI)phil-=m_2pi;
      if (deltap<-M_PI)phil+=m_2pi; philast=phil;
      if (phil<phim)phim=phil; if (phil>phip)phip=phil;
    }
    dp1=fabs(phis-phim); dp2=fabs(phip-phis); deltap=dp1; 
    if(dp2>dp1)deltap=dp2; phim=phis-deltap; phip=phis+deltap;
  } else if ((rmin<gvin) && (rmax<gvout)) {
    // case C (looper)
    if(5 == debug) std::cout<<" case C (looper) "<<std::endl;
    if (rc>rhel){
      double alp=asin(rhel/rc); phim=pc-alp; phip=pc+alp;
    }else{
      // going to have to step to find min, max
      double len=0; 
      double lstep=m_2pi*rhel/16.; 
      if (lstep>10.)lstep=10.;
      if(5 == debug) std::cout<< "ini step "<<m_2pi*rhel/16.<<" lstep  " << lstep <<"cm"<<std::endl;
      double xl=temphel->Xh(len); double yl=temphel->Yh(len);
      double phil=atan2(yl,xl); double rl=sqrt(xl*xl+yl*yl);
      int nstep=0; double philast=phil; int isin=0; int notout=1;
      while ((notout)&&(nstep<33)){
	len+=lstep; nstep++; xl=temphel->Xh(len); yl=temphel->Yh(len);
	phil=atan2(yl,xl); rl=sqrt(xl*xl+yl*yl);
	if(5 == debug) { 
	  std::cout<< nstep<<" rl "<<rl<< " gvin  " <<gvin<< " isin "<<isin;
	}
	if ((rl<gvin)&&(!isin)){
	  philast=phil;
	}else if((rl>gvin)&&(!isin)){
	  isin=1; phim=philast; phip=philast;
	}
	if (isin){
	  double deltap=phil-philast; if (deltap>M_PI)phil-=m_2pi;
	  if (deltap<-M_PI)phil+=m_2pi; philast=phil; if (rl<gvin)notout=0;
	  if (phil<phim)phim=phil; if (phil>phip)phip=phil;
	}
	//yzhang add 2011-10-10 
	if(len > M_PI*rhel*0.75) {
	  if(5 == debug) { 
	    std::cout<< " len "<<len<<" >pi*R_helix "<<M_PI*rhel<< " rhel "<<rhel<< " break"<<std::endl;
	  }
	  break;
	}
	if(5 == debug) { 
	  std::cout<< " philast "<<philast<<" phim "<<phim << " phip "<<phip <<" len "<<len<<std::endl;
	}
	//zhangy add
      }//end while
    }//end rc<>rhel
  } else if ((rmin>gvin) && (rmax<gvout)) {
    // case D (contained)
    if (rc > rhel) {
      double alp = asin(rhel/rc);
      phim = pc - alp;
      phip = pc + alp;
    }
    // if rc<rhel and case D, it's an orbiter. Use max phim, phip, so do nothing
  }
  phim -= phiex;
  phip += phiex; 

  if(5 == debug) { 
    std::cout<<"add hits phim "<<phim <<" phip "<<phip<<  std::endl;
  }

  //  now try to add hits
  kkl = 0; 
  if(5 == debug) std::cout<<"===== try to add hits:"<< std::endl;
  while(hhhh[kkl]) {
    MdcxHit* temphit = hhhh[kkl++];
    //yzhang add 2011-10-11 
    if(temphel->Doca_Len()<0) {
      if(5 == debug) std::cout<< " len<0 " << temphel->Doca_Len()<< " continue"<<std::endl;
      continue;
    }
    //zhangy
    double factor=1.;
    //if ((0 != temphit->type()) && (temphit->Layer()>7)) factor = MdcxParameters::addHitFactor;//yzhang FIXME 2009-11-03 
    if(5 == debug) { 
      temphit->print(std::cout);
      std::cout<< " pull "<<temphit->pull(*temphel)
	<< " nsig "<<factor * MdcxParameters::nSigAddHitTrk<< " len "<<temphel->Doca_Len() <<std::endl;
    }
    //yzhang 2009-10-21 16:55:25  ///FIXME
    if((!temphit->GetUsedOnHel())&&(fabs(temphit->pull(*temphel))< factor * MdcxParameters::nSigAddHitTrk) ) {

      //if( (!temphit->GetUsedOnHel()) && (fabs(temphit->d())<1.2) ) //delete
      double pw = temphit->pw(); 
      if((phip-pw) > m_2pi)  pw += m_2pi;
      if((phim-pw) < -m_2pi) pw -= m_2pi;
      if(5 == debug) { 
	std::cout<< "phi wire   "<<pw << " phim "<<phim<< " phip  "<<phip<<" len "<<temphel->Doca_Len();
      }
      if ( (pw>phim) && (pw<phip) ) {
	if (5 == debug){
	  std::cout<< " used "<<temphit->GetUsedOnHel()
	    <<" pull " << fabs(temphit->pull(*temphel))
	    <<" <? nSig " << MdcxParameters::nSigAddHitTrk
	    <<  std::endl;
	}
	temphit->SetConstErr(0);
	double pull = temphit->pull( *temphel );
	ncalc++;
	sumpull += fabs(pull);
	//cout << "MdcxAddHits: hit " << kkl-1 << " trk " << whichtrack << " pull " << pull << endl;
	if(g_addHitCut) g_addHitCut->fill(fabs(pull));
	int layer = temphit->Layer();
	if(g_addHitCut2d) g_addHitCut2d->fill(layer,fabs(pull));
	if(5 == debug) {
	  std::cout<< " pull "<<pull 
	    << " addcut "<< MdcxParameters::nSigAddHitTrk 
	    << "* factor:"<< factor <<"="<<factor * MdcxParameters::nSigAddHitTrk
	    << " len "<< temphel->Doca_Len()
	    << " >? lmax "<< lmax
	    << " >? maxTkLen "<< MdcxParameters::maxTrkLength
	    <<  std::endl;
	}
	//  yzhang 2009-10-21 22:55:26
	if(fabs(pull) < factor * MdcxParameters::nSigAddHitTrk) {
	  //if(fabs(pull) < addcut) 
	  double len = temphel->Doca_Len(); 
	  //{ hhhh[kkl-1]->print(std::cout);
	  //  cout << " trk " << whichtrack << " pull " << pull 
	  //     << " d() " << temphit->d() << " len " << len << endl; }
	  //int theflag = temphel->GetTurnFlag();  //zoujh ???
	  if (((len>0.0)&&(len<lmax))||((len<0.0)&&(len>-MdcxParameters::maxTrkLength))) { ///FIXME
	    //templist.append(temphit); 
	    listoasses.append(temphit);
	    temphit->SetUsedOnHel(1);
	    nadded++;
	    if(debug>2){
	      temphit->print(std::cout);
	      std::cout<<"Added "<<  std::endl;
	    }
	  }
	}
	temphit->SetConstErr(1);
      }//end phim,phip cuts
    } 
  }
  //listoasses = templist;
  return listoasses;
}
