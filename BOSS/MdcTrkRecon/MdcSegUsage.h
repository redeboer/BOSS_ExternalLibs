//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegUsage.h,v 1.2 2009/12/16 09:02:47 zhangy Exp $
//
// Description:
//     Records usage of a single hit in one of my segments.  Association 
//     with hit must be maintained externally.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): 
// 	Steve Schaffner
// 	Zhang Yao(zhangyao@ihep.ac.cn)	Migrate to BESIII
//------------------------------------------------------------------------
#ifndef MDCSEGUSAGE_H
#define MDCSEGUSAGE_H

class MdcSegUsage {
public:
  MdcSegUsage();
  virtual ~MdcSegUsage();

  inline bool usedSeg() const;
  inline bool usedAmbig(int i) const;
  inline void setUsedAmbig(int i);
  void killHit();
  bool deadHit() const                  {return ((segUse & deadFlag()) != 0);}

private:
  // Bit-mapping definitions for segment hit use word
  unsigned usedNegAmb() const                                     {return 1u;}
  unsigned usedPosAmb() const                                     {return 2u;}
  unsigned deadFlag()   const                                     {return 4u;}

  unsigned segUse;     

  // Preempt 
  MdcSegUsage&   operator= (const MdcSegUsage&);
  MdcSegUsage(const MdcSegUsage &);
};

// inline functs:
bool 
MdcSegUsage::usedSeg(void) const {
  return ( (segUse & (usedNegAmb() | usedPosAmb())) != 0);
}

void MdcSegUsage::setUsedAmbig(int i) {
  if (i <= 0) { segUse |= usedNegAmb(); }
  else { segUse |= usedPosAmb(); }
} 

bool 
MdcSegUsage::usedAmbig(int ambig) const {
  if (ambig <= 0) { 
    return (usedNegAmb() & segUse) != 0; 
  }
  else { 
    return (usedPosAmb() & segUse) != 0; 
  }
}

#endif

