#include "TKcCrossPart.h"
#include "TKcFormFactor.h"
#include "TGlobal.h"

TKcCrossPart::TKcCrossPart():TPiCrossPart(){
  // Neutral Kaon Formfactor
  if(fFpi) delete fFpi;
  fFpi   = new TKcFormFactor();
  //  fNoFSR = true;
}
