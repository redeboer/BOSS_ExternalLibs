//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcHitMapGuts.h,v 1.2 2005/07/18 03:16:11 zhangy Exp $
//
// Description:
//     Singleton implementation of MdcHitMap.  Requesting an instance either 
// creates one (first call) or clears the existing one (all subsequent calls).
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

#ifndef DCHHITMAPGUTS_HH
#define DCHHITMAPGUTS_HH

#include <vector>
class MdcHit;
class MdcDetector;

// Class interface //
class MdcHitMapGuts {

  friend class MdcHitMap;   // only one who can make one of these

public:
  virtual ~MdcHitMapGuts();

  MdcHit* hitWire(int lay, int wire) const {
      return _hits[lay][wire];
  }

  void addHit(MdcHit& theHit) ;

  void removeHit(int lay, int wire) {
        // could switch to () notation to avoid overhead for bounds-checking
        _hits[lay][wire] = 0;
  }

  void clear();

private:

  static MdcHitMapGuts* instance(const MdcDetector&);
  MdcHitMapGuts(const MdcDetector&);   // only used internally

  typedef std::vector<std::vector< MdcHit* > > MdcHitMap_t;
  MdcHitMap_t _hits;

  // Preempt
  MdcHitMapGuts& operator= (const MdcHitMapGuts&);
  MdcHitMapGuts(const MdcHitMapGuts &);
};

#endif
