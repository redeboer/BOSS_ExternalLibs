//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcHitMap.h,v 1.3 2009/09/22 06:28:08 zhangy Exp $
//
// Description:
//      Holds map of (layer,wire) -> MdcHit*.  To avoid time wasted in
// building the map, its guts are implemented as a singleton class
// (MdcHitMapGuts); a MdcHitMap can be created, stored in AbsEvent, and
// destroyed every event, but the underlying guts remain the same (they are
// cleared every time an instance is requested).  Or maybe I did it this way
// because I'm bored today.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
#ifndef MDCHITMAP_HH
#define MDCHITMAP_HH

#include "MdcData/MdcHitMapGuts.h"
#include "GaudiKernel/DataObject.h"
class MdcHit;
class MdcDetector;

// Class interface //
class MdcHitMap : public DataObject {
	
public:
  MdcHitMap(const MdcDetector&);
  virtual ~MdcHitMap();

  MdcHit* hitWire(int lay, int wire) const  {return _guts->hitWire(lay, wire);}
  void addHit(MdcHit& theHit) { _guts->addHit(theHit);}
  void removeHit(int lay, int wire) { _guts->removeHit(lay, wire); }
  void clearMap() { _guts->clear(); }

private:
  MdcHitMapGuts* _guts;

  // Preempt
  MdcHitMap& operator= (const MdcHitMap&);
  MdcHitMap(const MdcHitMap &);
};

#endif
