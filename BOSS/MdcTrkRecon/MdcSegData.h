//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegData.h,v 1.3 2011/09/26 01:06:37 zhangy Exp $
//
// Description:
//     Hold (pointer to) list of Mdc hits, and some auxiliary information. 
//     It mostly exists for historical reasons.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//------------------------------------------------------------------------
#ifndef MDCSEGDATA_H
#define MDCSEGDATA_H

//#include "MdcTrkRecon/MdcHitDict.h"
#include "MdcTrkRecon/MdcMap.h"
#include "MdcData/MdcHit.h"

class MdcDetector;
class MdcHitMap;
class MdcSegUsage;


class MdcSegData
{
public:
  typedef MdcMap<const MdcHit*,MdcSegUsage*> MdcHitDict;

  MdcSegData(bool skipUsedHit);
  virtual ~MdcSegData();

  int nevent() const                                      {return eventNumber;}
  int nhits() const;
  int runNumber() const                                    {return _runNumber;}
  const MdcHit* hit(int hitno) const;
  const MdcHitDict& segUsage() const                    {return _segUsageDict;}
  const MdcHitMap* hitMap() const                             {return _hitMap;}
  void loadevent(MdcHitCol *col, MdcHitMap *hmap, double tbunch);
  double bunchTime(void) const                             {return _bunchTime;}
  bool skippingUsed() const                                 {return _skipUsed;}
  void poisonHits(const MdcDetector *gm, int debug = 0);

private:
  bool _skipUsed;
  int eventNumber;
  int _runNumber;
  MdcHitCol* _theHits;
  MdcHitDict _segUsageDict;
  MdcSegUsage* _segUsage;
  MdcHitMap* _hitMap;
  double _bunchTime;

  // Preempt 
  MdcSegData&   operator= (const MdcSegData&);
  MdcSegData(const MdcSegData &);
};
#endif
