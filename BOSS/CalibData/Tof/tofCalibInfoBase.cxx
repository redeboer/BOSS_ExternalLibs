#include "CalibData/Tof/tofCalibInfoBase.h"
#include "GaudiKernel/MsgStream.h"

namespace CalibData {
 
  void tofCalibInfoBase::setBrEast(const int* tofidEast) {
    for(unsigned int i=0; i<5; i++) {
      m_tofidEast[i] = tofidEast[i];
    }
  }

  void tofCalibInfoBase::setBrWest(const int* tofidWest) {
    for(unsigned int i=0; i<5; i++) {
      m_tofidWest[i] = tofidWest[i];
    }
  }

  void tofCalibInfoBase::setEndcap(const int* tofidEndcap) {
    for(unsigned int i=0; i<5; i++) {
      m_tofidEndcap[i] = tofidEndcap[i];
    }
  }

}

