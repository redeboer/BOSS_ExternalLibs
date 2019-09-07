// $Header: /bes/bes/BossCvs/Calibration/CalibData/src/Tof/bTofCommonCalibBase.cxx,v 1.1 2008/10/17 01:57:45 huangb Exp $

/** @class bTofCalibBase
 *    Implementation of Mdct0 calibration TCDS class
 */

#include "CalibData/Tof/bTofCommonCalibBase.h"
#include "GaudiKernel/MsgStream.h"

namespace CalibData {
 
  void bTofCommonCalibBase::setOffset(const double* offset){
    for(int i=0; i<2; i++) {
      m_offset[i] = offset[i];
    }
  }

  void bTofCommonCalibBase::setSigmaCorr(const double* sigmaCorr){
    for(int i=0; i<8; i++) {
      m_sigmaCorr[i] = sigmaCorr[i];
    }
  }

}

