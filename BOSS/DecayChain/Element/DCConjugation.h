#ifndef DCHAIN_DCCONJUGATION_H
#define DCHAIN_DCCONJUGATION_H

//for backwards compatibility only
#include "DecayChain/Element/conjugation.h"

namespace DCConjugation {
   typedef dchain::conjugation::Label Label;
   static const Label kNone = dchain::conjugation::kNone;
   static const Label kHeads = dchain::conjugation::kHeads;
   static const Label kTails = dchain::conjugation::kTails;
}

#endif /* DCHAIN_DCCONJUGATION_H */
