#ifndef CalibData_bTofCommonCalibBase_h
#define CalibData_bTofCommonCalibBase_h

/** @class CalibData_Mdct0_h

  calibration data class t0 
  calibration infrastructure

  @author huangb
*/
#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include <vector>


namespace CalibData {
  class bTofCommonCalibBase{
   public:
     bTofCommonCalibBase(){};

    virtual ~bTofCommonCalibBase() {
      }

    //set the Calibdata of Tof
    void setOffset(const double* offset);
    void setSigmaCorr(const double* sigmaCorr);

    //get the CalibData of Tof
    double getOffset(int No) const { return m_offset[No]; }
    double getSigmaCorr(int No) const { return m_sigmaCorr[No]; }

  private:
     double m_offset[2];
     double m_sigmaCorr[8];

  };
}


#endif
