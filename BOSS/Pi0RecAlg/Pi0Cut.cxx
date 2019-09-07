#include "Pi0RecAlg/Pi0Cut.h"

EvtRecEvent* UserPi0Cut::recEvt = 0;
EvtRecTrackCol* UserPi0Cut::recTrkCol = 0;

namespace Pi0{
UserPi0Cut VeryLoosePi0Cut(0.02, 0.02, 20, 25, 0.1, 0.18, 1e+10);
UserPi0Cut DefaultPi0Cut;
}
