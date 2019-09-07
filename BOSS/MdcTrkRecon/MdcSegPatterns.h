// MdcSegPatterns.h -- holds list of allowed hits and ambiguity patterns for 
//     track segments in a superlayer.  A set of 8 wires is grouped 
//     together and the hits characterized by a bit-mapped number given by:
//              
//               7   6   5          outer layer of superlayer
//                 4   3
//                   2
//                 1   0            inner layer of superlayer
//
//  Bits corresponding to a hit wire are set to 1; others are 0.  There are 
//  therefore 256 integers representing the 256 possible combinations of 
//  hit wires.  Only certain combinations of hits represent possible tracks -- 
//  there are 8 hit combinations for tracks that span the group from inner
//  radius to outer (numbered 0 to 8):
//
//  X     X     X     X     X     X     X     X
//   X   X       X   X       X   X       X   X
//    X   X     X   X         X   X     X   X
//     X   X     X   X       X   X     X   X
//
//  Similarly, there are 22 allowed three-hit combinations; two of these 
//  are dropped to avoid double-counting between overlapping groups.  
//
//  For each 4(3)-hit pattern there are 16(8) possible L-R ambiguity 
//  combinations; for each pattern, some are allowed.  Allowed ambig patterns 
//  have ambigPatt4[pattern #][ambigPatt #] = 1.

// Interface Dependencies ----------------------------------------------

#ifndef MDCSEGPATTERNS_H
#define MDCSEGPATTERNS_H

//  End Interface Dependencies -----------------------------------------

// Class definition//
class MdcSegPatterns {
public:
  unsigned patt4[8];    // bit-mapped list of allowed patterns
  unsigned patt3[20];
  int npatt4[256];
  int npatt3[256];
  int *allowedPatt4[256];  // pointers to lists of 4-hit allowed patterns 
  int *allowedPatt3[256];  //    "      "   "   "  3-hit    "       "
  int **ambigPatt4;  
  int **ambigPatt3;  
  MdcSegPatterns(int useAllAmbig);
  ~MdcSegPatterns();

};
#endif







