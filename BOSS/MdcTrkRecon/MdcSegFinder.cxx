//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegFinder.cxx,v 1.17 2012/10/18 08:35:38 zhangy Exp $
//
// Description:
//
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: 
// 	Steve Schaffner
// 	Zhang Yao(zhangyao@ihep.ac.cn) Migration for BESIII
// 	2010-04-13 
// 	Zhang Yao(zhangyao@ihep.ac.cn) Keep best segment for 4-hit pattern
//------------------------------------------------------------------------

#include "MdcTrkRecon/MdcSegFinder.h"
#include "MdcTrkRecon/MdcSegList.h"
#include "MdcTrkRecon/countBits.h"
#include "MdcTrkRecon/mdcWrapWire.h"
#include "MdcTrkRecon/mdcWrapAng.h"
#include "MdcGeom/Constants.h"
#include "MdcTrkRecon/MdcLine.h"
#include "MdcGeom/BesAngle.h"
#include "MdcTrkRecon/MdcSegUsage.h"
#include "MdcTrkRecon/MdcLine.h"
#include "MdcData/MdcHit.h"
#include "MdcTrkRecon/MdcSeg.h"
#include "MdcTrkRecon/GmsList.h"
#include "MdcTrkRecon/MdcMap.h"
#include "MdcGeom/MdcDetector.h"
#include "MdcGeom/MdcSuperLayer.h"
#include "MdcData/MdcHitUse.h"
#include "MdcData/MdcHitMap.h"
//yzhang hist cut
#include "GaudiKernel/NTuple.h"
#include <map>

extern NTuple::Tuple*  g_tupleFindSeg;
extern NTuple::Item<double>            g_findSegChi2;
extern NTuple::Item<double>            g_findSegIntercept;
extern NTuple::Item<double>            g_findSegSlope;
extern NTuple::Item<double>            g_findSegChi2Refit;
extern NTuple::Item<double>            g_findSegChi2Add;
extern NTuple::Item<int>               g_findSegPat;
extern NTuple::Item<int>               g_findSegNhit;
extern NTuple::Item<int>               g_findSegPat34;
extern NTuple::Item<int>               g_findSegSl;
extern NTuple::Item<double>            g_findSegMc;
extern NTuple::Item<double>            g_findSegAmbig;
extern int haveDigiTk[43][288];
extern int haveDigiAmbig[43][288];
//zhangy hist cut

using std::cout;
using std::endl;

//-------------------------------------------------------------------------
MdcSegFinder::MdcSegFinder(int useAllAmbig) :
  patternList(useAllAmbig) {
    //-------------------------------------------------------------------------

  }
//-------------------------------------------------------------------------
int
MdcSegFinder::createSegs(const MdcDetector *gm, MdcSegList &segs,
    const MdcMap<const MdcHit*,MdcSegUsage*>& usedHits, const MdcHitMap* map,
    double bunchTime) {
  //-------------------------------------------------------------------------
  // Forms all possible Segs of hits to act as candidate tracks
  // Construct group of 8 hits; translate hit wires into bit-mapped word
  // and look up valid patterns (if any) for this group.

  _addHits = segs.segPar()->addHits;
  int nSegs = 0;
  int newSegs;
  unsigned int groupWord;
  MdcHit *groupHits[8];
  int lPrevHit = 0;  // flags that 2nd layer was hit in last group
  //which layer/wire to look at for each hit in group
  static const int layerWire[8][2] =
  { { 0,  -1}, { 0,  0}, { 1,  0}, { 2,  -1},
    { 2,  0}, { 3, -1}, { 3,  0}, { 3,  1} };

  // Loop through the superlayers
  const MdcSuperLayer *slayer = gm->firstSlay();
  while (slayer != 0) {
    const MdcLayer *layArray[4];
    int wireIndex[4];


    //    layArray[0] = slayer->firstLayer();
    //    for (int i = 1; i < 4; i++) layArray[i] = gm->nextLayer(layArray[i-1]);
    int nslay = slayer->nLayers();
    for (int i = 0; i < nslay; i++) layArray[i] = slayer->layer(i);
    if(nslay != 4) layArray[3] = 0;


    //    std::cout<<"-------------super layer----- "<<slayer->Id() << " nlayer "<<nslay<<std::endl;
    //for (int i = 0; i < nslay; i++) std::cerr<<layArray[i]->Id()<<" "<<layArray[i]->nWires()<<"\n";
    //slayer = slayer->next();continue;



    // Loop over cells (index refers to cells in 2nd layer of superlayer)
    for (int cellIndex = 0; cellIndex < layArray[1]->nWires(); cellIndex++) {
      //yzhang change FIXME	    
      double phi = layArray[1]->getWire(cellIndex)->phiE();
      for(int i = 0; i < 4; i++ ) {
	wireIndex[i] = cellIndex;
	if ( slayer->slayNum() > 4) continue;//nWires over No.4 slayer is uniform
	if ( (slayer->slayNum() > 9) && (i==3) ) break;//stop when get last layer
	if ( i == 1 ) continue;
	if ( i == 3 ) phi = layArray[2]->getWire(wireIndex[2])->phiE();
	BesAngle tmp(phi - layArray[i]->phiEPOffset());
	int wlow = (int)floor(layArray[i]->nWires() * tmp.rad() / twopi );
	int wbig = (int)ceil(layArray[i]->nWires() * tmp.rad() / twopi );
	if (tmp == 0 ){
	  wlow = -1;
	  wbig = 1;
	}
	if ( i!=3 ) {
	  wireIndex[i]=wbig;
	}else{
	  wireIndex[i]= wlow;
	}
	//zhangy change                                   
	wireIndex[i] = mdcWrapWire(wireIndex[i], layArray[i]->nWires());
      }
      // Form group of 8 wires
      groupWord = 0u;
      unsigned bitWord = 1u;
      int nGroup = 0;
      for (int ii = 0; ii < 8; ii++) {
	groupHits[ii] = 0;
	//-----------------------------
	if(layArray[ layerWire[ii][0] ] == 0) continue;
	//-----------------------------
	int laynum = layArray[ layerWire[ii][0] ]->layNum();
	int wire = wireIndex[ layerWire[ii][0] ] + layerWire[ii][1];
	wire = mdcWrapWire(wire, layArray[ layerWire[ii][0] ]->nWires());  //handle wrap-around
	MdcHit* thisHit = map->hitWire(laynum, wire);
	if (thisHit != 0) {
	  if ( !usedHits.get(thisHit)->deadHit() ) {
	    groupHits[ii] = thisHit;
	    groupWord |= bitWord;
	    nGroup++;
	  } else {
	    //	    cout << "Skipping hit " << endl;
	  }
	}
	// Quit if no hits in 1st 2 layers or 1 hit in 3 layers
	if ( (ii == 2 && nGroup < 1) || (ii == 4 && nGroup < 2) ) break;
	bitWord <<= 1;
      }
      if (nGroup < 3) continue;

      //int lastWire = mdcWrapWire(cellIndex - 1, layArray[1]->nWires());//2011-08-10 
      int lastWire = mdcWrapWire(cellIndex - 1, layArray[0]->nWires());
      if (map->hitWire(layArray[1]->layNum(), lastWire) != 0) lPrevHit = 1;
      //      if (layArray[1]->hitWire(lastWire) != 0) lPrevHit = 1;
      else lPrevHit = 0;

      // Try all allowed 4-hit patterns for this group (if any)
      int nPatt = patternList.npatt4[groupWord];

      if ((layArray[1]->layNum()==5) && (cellIndex ==46)) {
	for(int i=0;i<4;i++){
	  //std::cout<<__FILE__<<" "<<__LINE__<<"====("<<layArray[i]->layNum()<<","<< wireIndex[i]<<")" << std::endl;
	}
	//std::cout<<__FILE__<<" "<<__LINE__<< " groupWord:"<<groupWord<<" nPatt:"<<nPatt<< std::endl;
      }

      if (nPatt > 0) {
	newSegs = tryPatterns(groupHits, groupWord, 4, lPrevHit, nPatt,
	    patternList.allowedPatt4[groupWord], slayer, segs, usedHits, map,
	    bunchTime);
	nSegs += newSegs;
      }


      // If unsuccessful, try 3-hit patterns ???? may want to try 2nd pass here
      if (!segs.segPar()->find3[slayer->index()]) continue;
      int nUnused = 0;
      for (int i = 0; i < 8; i++) {
	if (groupHits[i] != 0) {
	  if (usedHits.get(groupHits[i])->usedSeg() == 0) nUnused++;
	}
      }
      if (nUnused > 0) {
	nPatt = patternList.npatt3[groupWord];
	if (nPatt > 0) {

	  newSegs = tryPatterns(groupHits, groupWord, 3, lPrevHit, nPatt,
	      patternList.allowedPatt3[groupWord], slayer, segs, usedHits, map,
	      bunchTime); 
	  nSegs += newSegs;
	}
      }
    }  // cellIndex loop

    slayer = slayer->next();
  }   //superlayer loop

  if (nSegs > 0) {
    segs.massageSegs();
    segs.resetSeed(gm);
  }

  nSegs = segs.count();

  if (5 == segs.segPar()->lPrint){ 
    cout << "Number of Segs found: " << nSegs << "\n";
  }

  return nSegs;
}

//---------------------------------------------------------------------------
int
MdcSegFinder::tryPatterns(MdcHit *groupHits[8],
    unsigned groupWord, int nInPatt,int lPrevHit, int npatt,
    int *allowedPat, const MdcSuperLayer *slay, MdcSegList &segs,
    const MdcMap<const MdcHit*,MdcSegUsage*>& usedHits, const MdcHitMap* map, double t0) {
  //---------------------------------------------------------------------------
  int nSegs = 0;
  int nbit = 8;

  unsigned *patterns;
  int **ambigPatt;
  if (nInPatt == 3) {
    patterns = patternList.patt3;
    ambigPatt = patternList.ambigPatt3;
  } else  {
    patterns = patternList.patt4;
    ambigPatt = patternList.ambigPatt4;
  }

  MdcSeg *trialSeg = new MdcSeg(t0); // start w/ 1 seg active

  // Create temporary array of hit structures for fitting segment
  MdcLine span(12);
  int spanAmbig[12];
  MdcHit *spanHits[12];
  MdcHit *ahit;

  // *** Loop over the allowed pattern for this group
  for (int iPatt = 0; iPatt < npatt; iPatt++) {
    unsigned thisPat = patterns[ allowedPat[iPatt] ];
    unsigned testWord = thisPat & groupWord;

    if (countbits(testWord) < nInPatt) continue;
    if (lPrevHit && nInPatt == 3 && (thisPat == 051 || thisPat == 0111))
      continue;

    // *** Load the hits into hit structure (radius only at this point)
    // radius is rel. to avg. for superlayer; resolution is converted to delphi
    unsigned bitadd = 1u;
    int nhit = 0;
    for (int ibit = 0; ibit < nbit; ibit++) {
      // Is this cell in the pattern?
      if (bitadd & thisPat) {
	const MdcLayer *layer = groupHits[ibit]->layer();
	if (layer == 0) cout << "huh?" << endl;
	span.x[nhit] = layer->rMid() - slay->rad0();
	spanHits[nhit] = groupHits[ibit];
	nhit++;
	if (nhit == nInPatt) break;
      }
      bitadd <<= 1;
    }

    // *** Loop through all ambiguity combinations; the loop counter also
    //    serves as a bit-mapped pattern of ambiguities
    //    Discard combo if all hits have been used w/ this ambig before

    std::map<int, MdcSeg*> bestTrialSegs;//yzhang 2012-09-18 

    int namb = 1 << nInPatt;
    //std::cout<< __FILE__ << "   " << __LINE__ << " namb  "<<namb<<" nInPatt "<<nInPatt<<std::endl;
    for (int iamb = 0; iamb < namb; iamb++) {

      // Skip all but allowed ambiguity patterns
      if (ambigPatt[ allowedPat[iPatt] ][iamb] != 1) continue;

      // Convert loop counter into physical ambiguity (0=> -1; 1=>+1)
      int nUsed = 0;
      int ihit;
      for (ihit = 0; ihit < nInPatt; ihit++) {
	spanAmbig[ihit] = ( (1u<<ihit) &iamb) ? 1: -1;
	nUsed += usedHits.get(spanHits[ihit])->usedAmbig( spanAmbig[ihit] );
      }
      if (nUsed >= nInPatt) continue;

      // ***Calculate phi for each hit, correcting approx. for angle of track
      /* First calculate a correction factor for the drift distance (since
	 we're treating hits as being at radius of wire).  This may slow
	 things down.*/
      /* corr = 1/cos(theta), where theta = del(d)/del(r), taken over the
	 span.  Use 1/cos = sqrt(1+tan**2) for calculating correction.  */

      double rIn = slay->firstLayer()->rMid();
      double rOut = slay->lastLayer()->rMid();
      double phiIn = mdcWrapAng(spanHits[nInPatt-1]->phi(),spanHits[0]->phi());
      double dphidr = ( (spanHits[nInPatt-1]->phi() + spanAmbig[nhit-1] *
	    spanHits[nInPatt-1]->driftDist(t0,spanAmbig[nhit-1]) /
	    rOut) - (phiIn+ spanAmbig[0] * spanHits[0]->driftDist(t0,spanAmbig[0]) / rIn)) / (rOut - rIn);//yzhang temp FIXME

      double corr = sqrt( 1 + slay->rad0() * slay->rad0() * dphidr * dphidr );

      if(5 == segs.segPar()->lPrint) {
	std::cout<<__FILE__<<" "<<__LINE__<< " corr" <<corr<< " phi_n "
	  <<spanHits[nInPatt-1]->phi()<<" dd_n "<< spanAmbig[nhit-1] *
	  spanHits[nInPatt-1]->driftDist(t0,spanAmbig[nhit-1])
	  <<" phiIn " <<phiIn
	  <<" dd_in "<< spanAmbig[0] * spanHits[0]->driftDist(t0,spanAmbig[0]) <<  std::endl;

      }
      //yzhang add fix phi accross -x axis
      bool crossAxis=false;
      double yOld = 0;
      //zhangy add
      double sigmaSum= 0.;
      double driftSum= 0.;
      double driftHit[12];
      // Actual phi calc
      for (ihit = 0; ihit < nInPatt; ihit++) {
	ahit = spanHits[ihit];
	double rinv = 1. / ahit->layer()->rMid();
	double drift = ahit->driftDist(t0,spanAmbig[ihit]);
	span.y[ihit] = ahit->phi() + spanAmbig[ihit] *
	  drift * (corr * rinv);//yzhang temp FIXME

	if (5 == segs.segPar()->lPrint) {
	  std::cout<<" in segment finding ("<<ahit->layernumber()<<","<<ahit->wirenumber()<<")"<<" |driftDist| "<< fabs(drift)<< " ambig "<<spanAmbig[ihit]<< " corr "<<corr<<" rinv "<<rinv<<" sigma "<<ahit->sigma(drift,spanAmbig[ihit])<<std::endl;
	}
	//span.sigma[ihit] = ahit->sigma(fabs(drift), spanAmbig[ihit]);
	//yzhang 2010-05-20 , FIXME
	span.sigma[ihit] = ahit->sigma(fabs(drift), spanAmbig[ihit]) * (corr * rinv);

	//yzhang add temp FIXME
	sigmaSum+= span.sigma[ihit];
	driftSum+=drift;
	driftHit[ihit]=drift;	
	//zhangy add temp FIXME

	//yzhang add fix phi accross -x axis,set flag
	if ( (span.y[ihit]!=0) && (!crossAxis)){
	  if ( (yOld / span.y[ihit]) < 0) crossAxis = true;
	  yOld = span.y[ihit];
	}
	//zhangy add
      }

      //yzhang add temp FIXME
      //for (ihit = 0; ihit < nInPatt; ihit++) {
      //span.sigma[ihit] = sigmaSum/nInPatt*
      //( fabs(driftHit[ihit]-(driftSum/nInPatt))/(driftSum/nInPatt) );
      //}
      //zhangy add temp FIXME

      //--yzhang add, fix phi accross -x axis , if cross +twopi
      if ( crossAxis ){
	for (int ihit=0 ; ihit < nInPatt; ihit++){
	  //Approx,for max phi of a cell is 2*pi/40, max cross 4 cell=0.628<0.7
	  if (abs(span.y[ihit]) < 0.7) break;
	  if (span.y[ihit] < 0)span.y[ihit]+=twopi;
	}
      }
      //zhangy add


      // Fit the segment
      if (5 == segs.segPar()->lPrint) std::cout<<" first fit("<<nInPatt<<")"<<std::endl;
      span.fit( nInPatt );
      BesAngle temp = span.intercept;
      span.intercept = temp.posRad();
      double t_segchi2 =  span.chisq / (nInPatt - 2) ;

      //yzhang test 2011-06-14  
      if (5 == segs.segPar()->lPrint) {
	for(int ii=0;ii<nInPatt;ii++){
	  std::cout<<__FILE__<<" "<<__LINE__<<" "<<ii <<" span.x "<<setw(10)<<span.x[ii]<<" y "<<setw(10)<<span.y[ii]<<" sigma "<<setw(10)<<span.sigma[ii]<< std::endl;
	}
      }
      if(g_tupleFindSeg != NULL){
	g_findSegSlope = span.slope;
	g_findSegIntercept = span.intercept;
	g_findSegChi2 = t_segchi2;
      }
      //zhangy test 


      if(5 == segs.segPar()->lPrint) {
	std::cout<<__FILE__<<" "<<__LINE__<< " pattern "<< thisPat
	  <<" nHit "<<nInPatt <<" chi2/dof " << t_segchi2
	  <<" chisq "<<span.chisq <<" maxChisq="<<segs.segPar()->maxChisq <<std::endl;
	for(int jj=0; jj<nInPatt; jj++){
	  std::cout << "resid["<<jj<<"] "<<span.resid[jj]<<" sigma "<<span.sigma[jj]<<" chisq add "<<span.resid[jj] * span.resid[jj] / (span.sigma[jj] * span.sigma[jj]) << std::endl;//yzhang debug 
	}
      }
      if (span.chisq / (nInPatt - 2) > segs.segPar()->maxChisq) {
	if (5 == segs.segPar()->lPrint) {
	  std::cout<<__FILE__<<" "<<__LINE__<<"CUT! chi2/(N-2) " <<span.chisq / (nInPatt - 2) <<" > "<< segs.segPar()->maxChisq<<  std::endl;
	  for(std::map<int,MdcSeg*>::iterator iter = bestTrialSegs.begin(); iter != bestTrialSegs.end(); iter++) {
	    cout<<" bestTrialSeg nHit="<<iter->first<<endl;
	  }
	}
	continue;
      }

      if (span.slope>Constants::pi || span.slope<(-1.)*Constants::pi ) {
	if(5 == segs.segPar()->lPrint) std::cout<<" CUT by span.|slope| "<<span.slope<<" > pi"<<std::endl;
	continue; //yzhang add 2012-08-07 
      }

      int nInSeg = nInPatt;

      // *** Pick up adjacent hits and refit
      // Recalculate correction factor (optional)
      if (segs.segPar()->segRefit) {
	dphidr = span.slope;
	corr = sqrt( 1 + slay->rad0() * slay->rad0() * dphidr * dphidr );
	crossAxis = false;
	for (ihit = 0; ihit < nInSeg; ihit++) {
	  ahit = spanHits[ihit];
	  double rinv = 1. / ahit->layer()->rMid();
	  double drift = ahit->driftDist(t0,spanAmbig[ihit]);
	  span.y[ihit] = ahit->phi() + spanAmbig[ihit] *
	    drift * (corr * rinv);//yzhang temp FIXME
	  //yzhang add
	  //span.y[ihit] = ahit->phi();
	  //zhangy add

	  //yzhang 2010-05-20 , FIXME
	  //span.sigma[ihit] = ahit->sigma(fabs(drift), spanAmbig[ihit]);
	  span.sigma[ihit] = ahit->sigma(fabs(drift), spanAmbig[ihit]) * (corr * rinv);
	  //yzhang add fix phi accross -x axis,set flag
	  if ( (span.y[ihit]!=0) && (!crossAxis)){
	    if ( (yOld / span.y[ihit]) < 0) crossAxis = true;
	    yOld = span.y[ihit];
	  }
	  //zhangy add
	}
	//yzhang add, fix phi accross -x axis , if cross +twopi
	if ( crossAxis ){
	  for (int ihit=0 ; ihit < nInPatt; ihit++){
	    //Approx,for max phi of a cell is 2*pi/40, max cross 4 cell=0.628<0.7
	    if (abs(span.y[ihit]) < 0.7) break;
	    if (span.y[ihit] < 0)span.y[ihit]+=twopi;
	  }
	}
	if (5 == segs.segPar()->lPrint) std::cout<<" second fit("<<nInSeg<<")"<<std::endl;
	//zhangy add
	span.fit( nInSeg);
	BesAngle temp = span.intercept;
	span.intercept = temp.posRad();

	if (span.chisq / (nInPatt - 2) > segs.segPar()->maxChisq) {
	  if (5 == segs.segPar()->lPrint) {
	    std::cout<<__FILE__<<" "<<__LINE__<<"CUT! chi2/(N-2) " <<span.chisq / (nInPatt - 2) <<" > "<< segs.segPar()->maxChisq<<  std::endl;
	    for(std::map<int,MdcSeg*>::iterator iter = bestTrialSegs.begin(); iter != bestTrialSegs.end(); iter++) {
	      cout<<" bestTrialSeg nHit="<<iter->first<<endl;
	    }
	  }
	  continue;
	}
      }

      if( g_tupleFindSeg!= NULL){
	g_findSegChi2Refit = span.chisq / (nInSeg - 2);
      }

      trialSeg->setValues(nInPatt, nInSeg, spanHits, &span, slay, spanAmbig);

      if (5 == segs.segPar()->lPrint) {
	std::cout<<" try:  "<<std::endl;
	trialSeg->plotSegAll();//yzhang temp 2012-09-18 
	std::cout<<"   "<<std::endl;
      }
      if (_addHits) {
	// Look for adjacent hits
	nInSeg = trialSeg->addHits(&span, spanHits, map, corr);
      }


      if (5 == segs.segPar()->lPrint) {
	for(std::map<int,MdcSeg*>::iterator iter = bestTrialSegs.begin(); iter != bestTrialSegs.end(); iter++) {
	  cout<<" bestTrialSeg nHit="<<iter->first<<" chi2 "<<iter->second->chisq()<<endl;
	}
	std::cout<<"trialSeg chisq "<<trialSeg->chisq()<<std::endl;//<<" best chi2 " <<bestTrialSeg->chisq() <<  std::endl;
	cout << "segment phi: " <<
	  trialSeg->phi() << "  slope: " << trialSeg->slope() <<
	  "  nhit: " << trialSeg->nHit() << " chi2 "<<trialSeg->chisq() << endl;
	//cout << "layer, wire, ambig, used, drift: " << endl;
	for (int i = 0; i < trialSeg->nHit(); i++) {
	  int ambi = trialSeg->hit(i)->ambig();
	  const MdcHit* theHit = trialSeg->hit(i)->mdcHit();
	  theHit->print(cout);
	  cout << " ambig " <<ambi;
	}
	cout << endl;
      }
      trialSeg->setPattern(iPatt);
      trialSeg->markHits(usedHits);
      if (nInPatt == 4) trialSeg->setFull();

      if( g_tupleFindSeg!= NULL){
	double t_mcRadio = -1.;
	double t_nAmbigRight= 0.;
	int t_tkId= -1;
	for(int ii=0;ii<trialSeg->nHit();ii++){
	  unsigned int l = trialSeg->hit(ii)->mdcHit()->layernumber();
	  unsigned int w = trialSeg->hit(ii)->mdcHit()->wirenumber();
	  if ( haveDigiTk[l][w]<0 || haveDigiTk[l][w]>100 ) continue;
	  if (t_tkId==-1){
	    t_tkId = haveDigiTk[l][w];
	  }else if (haveDigiTk[l][w] != t_tkId){
	    t_mcRadio = -999;//hits in this seg not in same mc track
	  }

	  // test ambig right ratio
	  if ( haveDigiAmbig[l][w] == trialSeg->hit(ii)->ambig() ) t_nAmbigRight++;
	}

	double t_nSame = 0.;
	for(int ii=0;ii<trialSeg->nHit();ii++){
	  unsigned int l = trialSeg->hit(ii)->mdcHit()->layernumber();
	  unsigned int w = trialSeg->hit(ii)->mdcHit()->wirenumber();
	  if (haveDigiTk[l][w] == t_tkId){ ++t_nSame; } 
	  if(t_mcRadio>-998.) t_mcRadio = t_nSame/nInSeg; 
	}
	g_findSegPat = iPatt;
	if(3==nInPatt) g_findSegPat34 = 3;
	else g_findSegPat34 = 4;
	g_findSegSl = slay->slayNum();
	g_findSegMc = t_mcRadio;
	g_findSegAmbig = t_nAmbigRight/nInSeg;
	g_findSegChi2Add = span.chisq / (nInSeg - 2);
	g_findSegNhit = nInSeg;
	g_tupleFindSeg->write();
      }

      //--keep only one segment for ambig
      if(3 == nInPatt){
	segs.append(trialSeg);    // Add to list of Segs
	nSegs++;
      }else{
	//debug
	//cout<<" before insert nBest="<<bestTrialSegs.size()<<endl;
	//for(std::map<int,MdcSeg*>::iterator iter = bestTrialSegs.begin(); iter != bestTrialSegs.end(); iter++) {
	//  cout<<"after insert bestTrialSeg nHit="<<iter->first<<endl;
	//  iter->second->plotSeg();
	//}
	std::map<int,MdcSeg*>::iterator iter = bestTrialSegs.find(trialSeg->nHit());
	if(iter==bestTrialSegs.end()){
	  bestTrialSegs.insert(std::map<int, MdcSeg*>::value_type(trialSeg->nHit(),trialSeg));
	  if(5 == segs.segPar()->lPrint){
	    std::cout<<"   ----insert  "<<trialSeg->nHit()<<std::endl;
	    trialSeg->plotSegAll(); std::cout<<" \n  "<<std::endl;
	  }
	} else {
	  if(trialSeg->chisq()<iter->second->chisq()){
	    MdcSeg* tempSeg= iter->second;
	    delete tempSeg;
	    iter->second = trialSeg;
	    if(5 == segs.segPar()->lPrint){
	      std::cout<<" ----update  "<<iter->first<<std::endl;
	      trialSeg->plotSegAll(); std::cout<<" \n  "<<std::endl;
	    }
	  }else{
	    if(5 == segs.segPar()->lPrint){ 
	      std::cout<< "DROP TrialSeg  " <<std::endl;
	      trialSeg->plotSegAll(); std::cout<<" \n  "<<std::endl;
	    }
	    delete trialSeg;
	  }
	}
	////debug
	//cout<<" after insert nBest="<<bestTrialSegs.size()<<endl;
	//for(std::map<int,MdcSeg*>::iterator iter = bestTrialSegs.begin(); iter != bestTrialSegs.end(); iter++) {
	//  cout<<"after insert bestTrialSeg nHit="<<iter->first<<endl;
	//  iter->second->plotSeg();
	//}
	//if((bestTrialSeg->nHit() == trialSeg->nHit()) && (bestTrialSeg->chisq() > trialSeg->chisq())){
	//  if(5 == segs.segPar()->lPrint) std::cout<< "KEEP trial as best. orignal bestTrialSeg  " 
	//    << bestTrialSeg->chisq()<<"/ndof "<<bestTrialSeg->nHit()-2<< "> trialSeg->chisq()  "
	//      <<trialSeg->chisq()<<"/ndof "<<trialSeg->nHit()-2<<std::endl;
	//  delete bestTrialSeg;
	//  bestTrialSeg = trialSeg;
	//  }
      }

      //--mark best trialSeg
      //std::cout<< __LINE__<<" best " <<bestTrialSeg->chisq()<< " trial  " << trialSeg->chisq() <<std::endl;

      trialSeg = new MdcSeg(t0);
    }// end ambiguity loop


    //--keep only one segment/Hit for ambig
    for(std::map<int,MdcSeg*>::iterator iter = bestTrialSegs.begin(); iter != bestTrialSegs.end(); iter++) {
      segs.append(iter->second);    // Add to list of Segs
      if (5 == segs.segPar()->lPrint) {
	std::cout<<" append bestTrialSeg of nHit " << iter->first <<std::endl;
	iter->second->plotSeg(); std::cout<<std::endl;
      }
    }

    bestTrialSegs.clear();//empty bestTrialSegs

    nSegs++;


  }// end pattern loop

  delete trialSeg;
  return nSegs;
}
