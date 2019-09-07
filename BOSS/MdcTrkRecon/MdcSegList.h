#ifndef DCHSEGLIST_H
#define DCHSEGLIST_H

// MdcSegList.hh  -- holds list of track segs.  Actually holds array of 
//   linked lists, one list for each superlayer.

// Interface Dependencies ----------------------------------------------

#include "MdcTrkRecon/MdcSegParams.h"

class MdcSegFinder;
class MdcDetector;
class GmsList;
class MdcSeg;
class MdcSuperLayer;

//  End Interface Dependencies -----------------------------------------

// Class definition//
class MdcSegList {

  friend class MdcSegFinder;
 
public:
  MdcSegList(int nSupers, const MdcSegParams segParms);
  ~MdcSegList();

  //  void newParams(const MdcSegParams &segPar);
  void setPlot(int lPlt);
  void plot() const;
  const GmsList *oneList(int slayIndex) const;
  void append(MdcSeg *aSeg);
  int count() const;

  void destroySegs();
  MdcSeg *getSeed( int iview, int goodOnly );
  void resetSeed(const MdcDetector *gm);
  MdcSegParams *segPar() {return &segParam;}

private:
  MdcSegParams segParam;
  GmsList *segList;   // eff. array (by slayer) of lists of segments
  int _nsupers;
  MdcSeg *seedSeg[3];    // next seed segment to be returned for given view
  const MdcSuperLayer *seedSlay[3];   // slayer of next seed

  void sortByPhi();  
  void zeroSeed();
  void tagAmbig();
  void deleteDups(bool drop);
  void massageSegs();
};

#endif







