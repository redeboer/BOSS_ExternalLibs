#include "CalibData/Tof/etfCalibBase.h"
#include "GaudiKernel/MsgStream.h"

namespace CalibData {
 
  void etfCalibBase::setSpeed(const double* Speed){
    for( unsigned int i=0; i<4; i++ ) {
      m_Speed[i] = Speed[i];
    }
  }
   
  void etfCalibBase::setP(const double* TofP){
    for( unsigned int i=0; i<20; i++ ) {
      m_P[i] = TofP[i];
    }
  }

  void etfCalibBase::setP1(const double* TofP){
    for( unsigned int i=0; i<20; i++ ) {
      m_P1[i] = TofP[i];
    }
  }

  void etfCalibBase::setP2(const double* TofP){
    for( unsigned int i=0; i<20; i++ ) {
      m_P2[i] = TofP[i];
    }
  }

}
