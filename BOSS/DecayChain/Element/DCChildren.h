#ifndef DCHAIN_DCCHILDREN_H
#define DCHAIN_DCCHILDREN_H
//only for backwards compatibility

#include "DecayChain/Element/children.h"

namespace DCChildren {
   using dchain::children::Position;
   static const Position First = dchain::children::First;
   static const Position Second = dchain::children::Second;
   static const Position Third = dchain::children::Third;
   static const Position Fourth = dchain::children::Fourth;
   static const Position Fifth = dchain::children::Fifth;

}
#endif
