//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegData.cxx,v 1.5 2011/09/26 01:06:37 zhangy Exp $
//
// Author(s): Steve Schaffner
//------------------------------------------------------------------------

#include "MdcTrkRecon/MdcSegData.h"
#include "MdcData/MdcHit.h"
#include "MdcTrkRecon/MdcSegUsage.h"
#include "MdcGeom/MdcDetector.h"
#include "MdcData/MdcHitMap.h"
#include "MdcTrkRecon/mdcWrapWire.h"

//------------------------------------------------------------------------
MdcSegData::MdcSegData(bool skip)
//------------------------------------------------------------------------
{
  eventNumber=0;
  _segUsage = 0;
  _hitMap = 0;
  _bunchTime = 0;
  _skipUsed = skip;

  _theHits = 0;
}

//------------------------------------------------------------------------
MdcSegData::~MdcSegData(void) {
//------------------------------------------------------------------------
  delete [] _segUsage;
}

//------------------------------------------------------------------------
int 
MdcSegData::nhits() const {
//------------------------------------------------------------------------
  return _theHits->size();
}

//------------------------------------------------------------------------
void 
MdcSegData::loadevent(MdcHitCol *hitcol, MdcHitMap *hitmap, double tb) {
//------------------------------------------------------------------------
  _bunchTime = tb;
  _theHits   = hitcol;
  _hitMap    = hitmap;
  assert (_hitMap != 0);

  _segUsageDict.clear();
  delete [] _segUsage;
  _segUsage = 0;
  eventNumber++;

  _segUsage = new MdcSegUsage[nhits()];

  MdcHit* aHit = 0;
  MdcHitCol::iterator iter = _theHits->begin();
  int index = 0;
  for (;iter != _theHits->end(); iter++) {
    aHit = *iter;
    _segUsageDict.put(aHit, &(_segUsage[index]));
    if (skippingUsed() && aHit->usedHit()) {
      // Mark for later skipping
      _segUsage[index].killHit();
    }
    index++;
  }
}

//------------------------------------------------------------------------
const MdcHit* 
MdcSegData::hit(int hitno) const {
//------------------------------------------------------------------------
return (*_theHits)[hitno];
}


//------------------------------------------------------------------------
void 
MdcSegData::poisonHits(const MdcDetector *gm, int debug) {
//------------------------------------------------------------------------
  // Mark hits as poor candidates for segment-finding, if they are located 
  //  within a string of consecutive hits in the same layer (end hits are 
  //  not marked).
  for (int ilayer = 0; ilayer < gm->nLayer(); ilayer++) {
    int nwire = gm->Layer(ilayer)->nWires();
    for (int iwire=0; iwire < nwire;++iwire) {
      int wireNext = mdcWrapWire(iwire+1, nwire);
      if ( 0 == hitMap()->hitWire(ilayer, wireNext) ) { 
              //                        0 1 2 3 4
              // next wire not there:   ? o ? ? ?
              // so the next-to-next wire shouldn't be poisened 
              // (it's left neighbour is empty)
              // and the next candidate to check is + 3
              iwire+=2; continue;
      }

      MdcHit* theHit = hitMap()->hitWire(ilayer, iwire);
      if (theHit == 0) { 
              //                                        0 1 2 3 4
              // next wire is there, this one isn't :   o x ? ? ?
              // and the next candidate to check is + 2
              ++iwire; continue;
      }

      int wirePrev = mdcWrapWire(iwire-1, nwire);
      if ( 0 != hitMap()->hitWire(ilayer, wirePrev)) { 
	  segUsage().get(theHit)->killHit();
	  if(debug>1){
	    theHit->print(std::cout);
	    std::cout<< " killed " << std::endl;
	  }
      }else{
	if(debug>1){
	  theHit->print(std::cout);
	  std::cout<< " ok " << std::endl;
	}
      }
    }      // end wire loop
  }        // end layer loop

  return;
}

