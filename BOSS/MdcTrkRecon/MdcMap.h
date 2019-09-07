//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcMap.h,v 1.1.1.1 2005/04/21 06:23:43 maqm Exp $
//
// Description:
//     Dictionary to associate a value (size_t) with a key (long).  This
// could undoubtedly be templated (cf DchHitDict), but I'm too lazy to do so.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Gerhard Raven
//
//------------------------------------------------------------------------
#ifndef MDCMAP_HH
#define MDCMAP_HH
#include <map>

// Class interface //
template <class K, class V>
class MdcMap {

public:
  MdcMap();
  ~MdcMap();
  bool get(const K& theKey, V& theAnswer) const;
  V& get(const K& k) const;
  void put(const K&, const V&);
  void clear();

private:
  std::map<K, V> _dict;

  // Preempt
  MdcMap&   operator= (const MdcMap&);
  MdcMap(const MdcMap &);
};

//#ifdef BABAR_COMP_INST
#include "MdcTrkRecon/MdcMap.icc"
//#endif

#endif
