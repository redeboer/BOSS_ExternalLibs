// $Header: /bes/bes/BossCvs/Calibration/CalibData/src/Tof/eTofCalibBase.cxx,v 1.6 2010/10/14 05:25:29 huangb Exp $

/** @class eTofCalibBase
 *    Implementation of Mdct0 calibration TCDS class
 */

#include "CalibData/Tof/eTofCalibBase.h"
#include "GaudiKernel/MsgStream.h"

namespace CalibData {
 
  void eTofCalibBase::setAtten(const double* Atten){
    for( unsigned int i=0; i<8; i++ ) {
      m_Atten[i] = Atten[i];
    }
  }

  void eTofCalibBase::setSpeed(const double* Speed){
    for( unsigned int i=0; i<4; i++ ) {
      m_Speed[i] = Speed[i];
    }
  }
   
  void eTofCalibBase::setP(const double* TofP){
    for( unsigned int i=0; i<8; i++ ) {
      m_P[i] = TofP[i];
    }
  }

  void eTofCalibBase::setFP(const double* TofFP){
    for( unsigned int i=0; i<5; i++ ) {
      m_FP[i] = TofFP[i];
    }
  }

}
