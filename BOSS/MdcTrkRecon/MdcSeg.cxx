// MdcSeg.cxx

#include "MdcTrkRecon/MdcSeg.h"
#include <stdlib.h>
#include <math.h>
#include "MdcGeom/BesAngle.h"
#include "MdcTrkRecon/mdcWrapAng.h"
#include "MdcTrkRecon/mdcWrapWire.h"
#include "MdcTrkRecon/MdcLine.h"
#include "MdcTrkRecon/MdcSegParams.h"
#include "MdcData/MdcHit.h"
#include "MdcGeom/MdcSuperLayer.h"
#include "MdcGeom/MdcLayer.h"
#include "MdcTrkRecon/MdcSegInfoSterO.h"
#include "MdcTrkRecon/MdcSegUsage.h"
#include "MdcTrkRecon/MdcMap.h"
#include "MdcData/MdcHitUse.h"
#include "MdcData/MdcHitMap.h"

//yzhang hist cut
#include "AIDA/IHistogram1D.h"
extern AIDA::IHistogram1D*  g_nSigAdd;
//zhangy hist cut


extern int haveDigiTk[43][288];
extern double haveDigiPt[43][288];
extern double haveDigiTheta[43][288];
extern double haveDigiPhi[43][288];
extern int haveDigiAmbig[43][288];
//Initialize static pointer
MdcSegParams* MdcSeg::segParam = 0;
const double twoPi = Constants::twoPi;
//------------------------------------------------------------------------
MdcSeg::MdcSeg(double bt) {
//------------------------------------------------------------------------
  _info = 0;
  _bunchTime = bt;
}

//------------------------------------------------------------------------
MdcSeg::~MdcSeg() {
//------------------------------------------------------------------------
  if (_info != 0) delete _info;
  reset();  // delete Hots
}

//------------------------------------------------------------------------
MdcSeg::MdcSeg(const MdcSeg& other):
  GmsListLink(), _slayer(other._slayer), _phi(other._phi), _slope(other._slope),  _chisq(other._chisq), _qual(other._qual), _pattern(other._pattern), _info(other._info), _bunchTime(other._bunchTime)
 //------------------------------------------------------------------------
{  
  HepAListDeleteAll(_theList);
  for(int i=0; i<other.nHit(); i++){
    _theList.append(other.hit(i));
  }
  for(int j=0; j<3; j++){
    _errmat[0] = other._errmat[0];
    _errmat[1] = other._errmat[1];
    _errmat[2] = other._errmat[2];
  }
  segParam = other.segParam; 
}

//------------------------------------------------------------------------
MdcSeg&
MdcSeg::operator = (const MdcSeg& other) {
  //------------------------------------------------------------------------
  if(&other != this){

    HepAListDeleteAll(_theList);
    for(int i=0; i<other.nHit(); i++){
      _theList.append(other.hit(i));
    }
    _slayer = other._slayer;
    _phi = other._phi;
    _slope= other._slope;
    _errmat[0] = other._errmat[0];
    _errmat[1] = other._errmat[1]; 
    _errmat[2] = other._errmat[2];
    _chisq = other._chisq;
    _qual = other._qual;
    _pattern = other._pattern;
    _info = other._info;
    _bunchTime = other._bunchTime;
    segParam = other.segParam; 
  }

  return *this;
}



//------------------------------------------------------------------------
void
MdcSeg::setInfo(MdcSegInfo *newInfo) {
  //------------------------------------------------------------------------
  delete _info;  // if any
  _info = newInfo;
}

//------------------------------------------------------------------------
void
MdcSeg::setValues(int nInPatt, int nhit, MdcHit *hits[],
    MdcLine *span, const MdcSuperLayer *slay, int ambig[]) {
  //------------------------------------------------------------------------
  _qual = 0;
  if (nInPatt == 4) _qual |= segFullFlag;
  _phi = BesAngle(span->intercept);
  _slope = span->slope;
  _chisq = span->chisq;
  _errmat[0] = span->errmat[0];
  _errmat[1] = span->errmat[1];
  _errmat[2] = span->errmat[2];
  reset();
  _slayer = slay;
  for (int i = 0; i < nhit; i++) {
    MdcHitUse *alink = new MdcHitUse(*(hits[i]), superlayer()->rad0(),
	ambig[i]);
    append(alink);
  }

  return;
}

//------------------------------------------------------------------------
void
MdcSeg::setValues(int nInPatt, double inPhi, double inSlope,
    double chi2, double inError[3], const MdcSuperLayer *slay) {
  //------------------------------------------------------------------------
  // Sets segment values with no associated hits
  _qual = 0;
  if (nInPatt == 4) _qual |= segFullFlag;
  _phi = inPhi;
  _slope = inSlope;
  _chisq = chi2;
  _errmat[0] = inError[0];
  _errmat[1] = inError[1];
  _errmat[2] = inError[2];
  _slayer = slay;
  reset();  // clears hit list

  return;
}

//------------------------------------------------------------------------
void
MdcSeg::markHits(const MdcMap<const MdcHit*,MdcSegUsage*>& usedHits) const {
  //------------------------------------------------------------------------
  for (int i = 0; i < nHit(); i++) {
    MdcHitUse *alink = _theList[i];
    MdcSegUsage *x ;
    if ( usedHits.get( alink->mdcHit() , x) ) x->setUsedAmbig( alink->ambig() );
  }
}

//------------------------------------------------------------------------
void
MdcSeg::plotSegAll() const {
  //------------------------------------------------------------------------
  //print hit
  //if(superlayer()!=NULL) std::cout<<"sl"<<superlayer()->slayNum()<<" p"<<segPattern()<<" st"<<quality();
  for (int ihit=0 ; ihit< nHit() ; ihit++){
    hit(ihit)->mdcHit()->print(std::cout);
    std::cout << setw(2)<<hit(ihit)->ambig()<<" ";
  }

  cout<<setiosflags(ios::fixed);
  if (info()!=NULL){
    std::cout<< " phi " << setprecision(2) << phi()
      << " slope " <<std::setw(2)<< setprecision(2) <<slope()<<" ";
    if(superlayer()->whichView()==0){
      std::cout <<setprecision (2) <<"phi0="<<info()->par(0);
      cout<<setprecision(5)<<" cpa="<<info()->par(1);
    }else{
      std::cout <<setprecision(2)<<"z0="<<info()->par(0)
	<<setprecision(2)<<" ct="<<info()->par(1);
    }
    if(fabs(info()->arc())>0.0001){
      std::cout<<setprecision(2)<<" arc="<<info()->arc();
    }
    std::cout<<setprecision(3)<<" chi2="<<_chisq;
  }else{
    std::cout<< " phi " << setprecision(2) << phi()
      << " slope " <<std::setw(2)<< setprecision(2) <<slope()
      << " chi2 "<<setprecision(3) <<chisq();
  }

  cout<<setprecision(6);
  cout<<setiosflags(ios::scientific);
}
//------------------------------------------------------------------------
void
MdcSeg::plotSeg() const {
  //------------------------------------------------------------------------
  std::cout<<superlayer()->slayNum()<<" pat"<<segPattern()<<" ";
  for (int ihit=0 ; ihit< nHit() ; ihit++){
    hit(ihit)->mdcHit()->print(std::cout);//print hit
    std::cout <<hit(ihit)->ambig()<<" ";
  }
  if (info()!=NULL){
    cout<< " . "; 
  }
  //std::cout << std::endl;
}

//------------------------------------------------------------------------
//void
//MdcSeg::plotZ(int openIt, int color) const {
//------------------------------------------------------------------------
/*
// Plot line to indicate segment
double start[2], finish[2];

const double length = 0.018 * display->width(windowZ);
double ct = ((MdcSegInfoSterO *) info())->ct();   // !!! this cast stinks
double z0 = ((MdcSegInfoSterO *) info())->z0();
double arc = ((MdcSegInfoSterO *) info())->arc();
double dels = length / sqrt(1. + ct*ct);
double z = z0 + ((MdcSegInfoSterO *) info())->arc() * ct;
start[0] = arc - dels;
start[1] = z - ct * dels;
finish[0] = arc + dels;
finish[1] = z + ct * dels;
*/
//}

//------------------------------------------------------------------------
int
MdcSeg::addHits(MdcLine *span, MdcHit** /* hits[]*/, const MdcHitMap* map,
    double corr) {
  //------------------------------------------------------------------------

  /* Pick up hits adjacent to an existing segment.  Output final number of
     hits.  Note: input hits are not refound.  It picks up hits
     in cells that the segment should pass through, checking that they
     have a plausible drift distance.  In event of a wraparound (i.e. track that
     passes through 2pi), all angles are relative to phiseg (i.e. if phiseg is
     just above 0, some phi's may be negative).  */
  //****

  int cell[2], ambig[2]; //wire # and ambig for cells in current layer
  double phiwire[2];
  int cellused[4] = {0}; // list of wire #'s of existing hits in span
  int lAdded = 0;
  int nhits = nHit();
  int m_debug = 0;

  // Note the hits already in the fit, so we don't pick them up again.
  int firstnum = superlayer()->firstLayer()->layNum();
  for (int i = 0; i < nHit(); i++) {
    const MdcHit* dHit = hit(i)->mdcHit();
    int laynum = dHit->layernumber();
    cellused[laynum - firstnum ] = dHit->wirenumber();
  }

  // Loop through the layers, predicting cells that could be hit.
  //  for (int layIndex = 0; layIndex < 4; layIndex++) {
  for (int layIndex = 0; layIndex < superlayer()->nLayers(); layIndex++) {
    const MdcLayer *layer = superlayer()->layer(layIndex);
    int laynum = layer->layNum();

    // Calc. projected phi of hit
    double rinv = 1. / layer->rMid();
    double ncellinv = 1. / (double) layer->nWires();
    double phiproj = phi() + (layer->rMid() - superlayer()->rad0()) * slope();
    // Calc. nearest wire.
    BesAngle tmp(phiproj - layer->phiOffset());
    cell[0] = (int) floor(layer->nWires() *
	tmp.rad() / twoPi + 0.5);
    cell[0] = mdcWrapWire( cell[0], layer->nWires() );
    phiwire[0] = mdcWrapAng( phi(), cell[0] * twoPi * ncellinv +
	layer->phiOffset() );
    // Which ambiguity? +1 if phi(wire) < phi(projected).
    ambig[0] = (phiwire[0] < phiproj) ? 1 : -1;

    // Now next nearest wire.
    ambig[1] = -ambig[0];
    cell[1] = mdcWrapWire( cell[0] + ambig[0], layer->nWires() );
    phiwire[1] = mdcWrapAng( phi(), cell[1] * twoPi * ncellinv +
	layer->phiOffset() );

    if(m_debug) std::cout<< " loop over the two possible wires  " << std::endl;
    //*** Loop over the two possible wires.
    for (int iroad = 0; iroad < 2; iroad++) {
      if (cellused[laynum - firstnum] == cell[iroad]) continue;
      if(m_debug) std::cout<<  "possible wires  "<<laynum<<" "<<cell[iroad]<<std::endl;
      if (map->hitWire(laynum, cell[iroad]) != 0) {
	MdcHit *ahit = map->hitWire(laynum, cell[iroad]);
	// if hit is already used, skip it!
	if (ahit->usedHit()) {
	  if(m_debug) std::cout<< "hit used continue   " <<std::endl;
	  continue;
	}
	// drift as delphi
	// FIXME: flip ambiguity for incoming tracks
	double phidrift = ahit->driftDist(bunchTime(), ambig[iroad]) * corr * rinv;
	double phihit = mdcWrapAng( phi(), phidrift * ambig[iroad] + ahit->phi());

	// Check the drift distance.
	double sigphi = corr * ahit->sigma(bunchTime(), 0) * rinv;
	// Skip hit if more than n sigma away from track.
	if ( g_nSigAdd && fabs(sigphi)>0.0001 ) { g_nSigAdd->fill(fabs(phihit - phiproj) / sigphi); }//yzhang hist cut
	if ( fabs(phihit - phiproj) > sigphi * segParam->nsigAddHit ) {
	  if(m_debug) std::cout<< fabs(phihit-phiproj) <<"> add hit sigma   " 
	    << sigphi<< "*"<< segParam->nsigAddHit <<"="<<sigphi*segParam->nsigAddHit<<std::endl;
	  continue;
	}

	// Load hit for refitting
	lAdded = 1;
	span->sigma[nhits] = sigphi;
	span->x[nhits] = layer->rMid() - superlayer()->rad0();
	span->y[nhits] = mdcWrapAng(span->y[0], phihit);

	// Add this hit to segment
	MdcHitUse *alink = new MdcHitUse(*ahit, superlayer()->rad0(),
	    ambig[iroad]);
	append(alink);
	//std::cout<< __FILE__ << "   " << __LINE__ << " addhit  "<<std::endl;

	nhits++;
      }  // end if hit wire

    } // end loop over 2 cells

  } // end layer loop

  if (lAdded) {
    span->fit(nhits);
    BesAngle tmpAngle(span->intercept);
    span->intercept = tmpAngle;
    _phi = span->intercept;
    _slope = span->slope;
    _chisq = span->chisq;
    _errmat[0] = span->errmat[0];
    _errmat[1] = span->errmat[1];
    _errmat[2] = span->errmat[2];
  }

  return nhits;
}

//------------------------------------------------------------------------
void
MdcSeg::reset() {
  //------------------------------------------------------------------------
  HepAListDeleteAll( _theList );
}

//------------------------------------------------------------------------
void
MdcSeg::append(MdcHitUse* theHitUse) {
  //------------------------------------------------------------------------
  _theList.append(theHitUse);
}

//------------------------------------------------------------------------
void
MdcSeg::remove(MdcHitUse* theHitUse) {
  //------------------------------------------------------------------------
  _theList.remove(theHitUse);
  delete theHitUse;
}

//------------------------------------------------------------------------
int
MdcSeg::nHit() const {
  //------------------------------------------------------------------------
  return _theList.length();
}


//------------------------------------------------------------------------
double 
MdcSeg::testCombSeg(const MdcSeg* testSeg)const{
  //------------------------------------------------------------------------
  int tkId= -1;
  for (int i=0; i<nHit(); i++){
    const MdcHit* h = hit(i)->mdcHit();
    unsigned int l = h->layernumber();
    unsigned int w = h->wirenumber();
    //std::cout<< __FILE__ << " ref  " << i << " haveDigiTk("<<l<<","<<w<<")"<<haveDigiTk[l][w]<<std::endl;
    if ( haveDigiTk[l][w]<0 || haveDigiTk[l][w]>100 ) continue;
    if (tkId<0){
      tkId = haveDigiTk[l][w];
    }else if (haveDigiTk[l][w] != tkId){
      return -1;//hits in this seg not in same mc track
    }
  }//end for

  double nSame = 0.;
  for (int i=0; i<testSeg->nHit(); i++){
    const MdcHit* h = testSeg->hit(i)->mdcHit();
    unsigned int l = h->layernumber();
    unsigned int w = h->wirenumber();
    if (haveDigiTk[l][w] == tkId){
      ++nSame;
    }
  }

  return nSame/testSeg->nHit();
}

//------------------------------------------------------------------------
double 
MdcSeg::testCombSegPt()const{
  //------------------------------------------------------------------------
  double truthPt = -1.;
  for (int i=0; i<nHit(); i++){
    const MdcHit* h = hit(i)->mdcHit();
    unsigned int l = h->layernumber();
    unsigned int w = h->wirenumber();
    if ( haveDigiPt[l][w]<0 ) continue;
    //std::cout<< __FILE__ << "   " << __LINE__ << "   haveDigiPt("<<l<<","<<w<<")"<<haveDigiPt[l][w]<<std::endl;
    if (truthPt<0){ truthPt = haveDigiPt[l][w]; }
  }//end for

  return truthPt;
}

//------------------------------------------------------------------------
double 
MdcSeg::testCombSegTheta()const{
  //------------------------------------------------------------------------
  double truthTheta = -999.;
  for (int i=0; i<nHit(); i++){
    const MdcHit* h = hit(i)->mdcHit();
    unsigned int l = h->layernumber();
    unsigned int w = h->wirenumber();
    if ( haveDigiTheta[l][w]<-998. ) continue;
    //std::cout<< __FILE__ << "   " << __LINE__ << "   haveDigiTheta("<<l<<","<<w<<")"<<haveDigiTheta[l][w]<<std::endl;
    if (truthTheta<-998.){ truthTheta = haveDigiTheta[l][w]; }
  }//end for

  return truthTheta;
}

//------------------------------------------------------------------------
double 
MdcSeg::testCombSegPhi()const{
  //------------------------------------------------------------------------
  double truthPhi = -999.;
  for (int i=0; i<nHit(); i++){
    const MdcHit* h = hit(i)->mdcHit();
    unsigned int l = h->layernumber();
    unsigned int w = h->wirenumber();
    if ( haveDigiPhi[l][w]<-998. ) continue;
    //std::cout<< __FILE__ << "   " << __LINE__ << "   haveDigiPhi("<<l<<","<<w<<")"<<haveDigiPhi[l][w]<<std::endl;
    if (truthPhi<-998.){ truthPhi = haveDigiPhi[l][w]; }
  }//end for

  return truthPhi;
}

//------------------------------------------------------------------------
double 
MdcSeg::testCombSegAmbig()const{
  //------------------------------------------------------------------------
  double ambigOk = 0.;
  for (int i=0; i<nHit(); i++){
    const MdcHit* h = hit(i)->mdcHit();
    unsigned int l = h->layernumber();
    unsigned int w = h->wirenumber();
    if( hit(i)->ambig() == haveDigiAmbig[l][w] ) ambigOk++;
  }//end for

  return ambigOk/nHit();
}

