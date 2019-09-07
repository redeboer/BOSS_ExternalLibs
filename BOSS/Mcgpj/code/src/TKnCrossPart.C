#include "TKnCrossPart.h"
#include "TKnFormFactor.h"
#include "TGlobal.h"

TKnCrossPart::TKnCrossPart():TPiCrossPart(){
  // Neutral Kaon Formfactor
  if(fFpi) delete fFpi;
  fFpi   = new TKnFormFactor();

  fNoFSR = true;
}
