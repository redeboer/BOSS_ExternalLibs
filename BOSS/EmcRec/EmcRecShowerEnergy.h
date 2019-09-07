//
//    Give 3*3, 5*5, and bump energy.
//
//    Created by He Miao 2005, 7, 6
//
#ifndef EMC_REC_SHOWER_ENERGY_H
#define EMC_REC_SHOWER_ENERGY_H

#include "EmcRecEventModel/RecEmcEventModel.h"

class EmcRecShowerEnergy
{
  public:
    void Energy(RecEmcShower& aShower);
    void E3x3(RecEmcShower& aShower);
    void E5x5(RecEmcShower& aShower);
    void ELepton(RecEmcShower& aShower);
    void EAll(RecEmcShower& aShower);
    RecEmcEnergy ECorrection(const RecEmcEnergy eIn);
    RecEmcEnergy ECorrTheta(const RecEmcEnergy eIn, const RecEmcID &id);
};

#endif // EMC_REC_SHOWER_ENERGY_H

