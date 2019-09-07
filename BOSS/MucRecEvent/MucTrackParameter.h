
#ifndef MUC_TRACK_PARAMETER_H_
#define MUC_TRACK_PARAMETER_H_

const double kDeltaPhi = 0.01; // extrapolation step
const double kvC = 3e8;
const int    kMdcExtIterMax = 1000; // calculate track from origin step by step

const double kInsctWeight = 0.3; // weight of old intersection pos in calculate new intersection with hits

#endif // MUC_TRACK_PARAMETER_H_
