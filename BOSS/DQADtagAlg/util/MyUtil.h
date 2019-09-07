#ifndef MY_UTIL_H
#define MY_UTIL_H

#include "GaudiKernel/NTuple.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "EmcRecEventModel/RecEmcShower.h"

void fill(NTuple::Array<double>& nt_p4, const HepLorentzVector& p4);
void fill(NTuple::Matrix<double>& nt_p4, const HepLorentzVector& p4, int i);

HepLorentzVector getP4(RecEmcShower* gtrk);

#endif
