#include "CalibData/Tof/etfBunchCalibBase.h"
#include "GaudiKernel/MsgStream.h"

namespace CalibData {
 
  void etfBunchCalibBase::setPBunch(const double* etfBunchP){
    for(int i=0; i<4; i++) {
      m_pBunch[i] = etfBunchP[i];
    }
  }

}
