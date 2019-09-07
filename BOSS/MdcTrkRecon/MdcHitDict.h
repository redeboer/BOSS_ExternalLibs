//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcHitDict.h,v 1.1.1.1 2005/04/21 06:23:43 maqm Exp $
//
// Description:
//     Dictionary: associates a stored (by pointer) object (in current 
//     implementation, a MdcSegUsage object) with a MdcHit.  Given the 
//     hit, it returns the associated object.  Uses a RW hash dictionary.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
#ifndef MMDCITDICT_H
#define MMDCITDICT_H
#include "MdcData/MdcHit.h"
#include "MdcTrkRecon/MdcSegUsage.h"

#include <map>

// Class interface //
class MdcHitDict {

public:
  MdcHitDict();
  virtual ~MdcHitDict();
  MdcSegUsage*  get(const MdcHit*) const;
  void put(MdcHit*, MdcSegUsage*);
  void clear();

private:	
  //**RWTPtrHashDictionary<DchHit, DchSegUsage> m_segUsage;

  //zhangxy  
  std::map<const MdcHit *, MdcSegUsage *> m_segUsage;
  
  // Preempt 
  MdcHitDict&   operator= (const MdcHitDict&);
  MdcHitDict(const MdcHitDict &);
};

#endif







