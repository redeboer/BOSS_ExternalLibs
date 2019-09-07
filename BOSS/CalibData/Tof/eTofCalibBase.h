// $Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/Tof/eTofCalibBase.h,v 1.7 2010/10/14 05:25:29 huangb Exp $
#ifndef CalibData_eTofCalibBase_h
#define CalibData_eTofCalibBase_h

/** @class CalibData_Mdct0_h

  calibration data class t0 
  calibration infrastructure

  @author huangb
*/
#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include <vector>


namespace CalibData {
  class eTofCalibBase{
   public:
     eTofCalibBase(){};

    virtual ~eTofCalibBase() {
      }

    //set the Calibdata of Tof
     void setAtten(const double* Atten);
     void setSpeed(const double* Speed);
     void setP(const double* TofP);
     void setFP(const double* FP);

     //get the CalibData of Tof
     double  getAtten(int No)  const{return m_Atten[No];} 
     double  getSpeed(int No) const{return m_Speed[No];}
     double  getP(int No) const { return m_P[No];}
     double  getFP(int No) const{return m_FP[No];}

  private:
     double m_Atten[5];
     double m_Speed[4];
     double m_P[8];
     double m_FP[5];

  };
}


#endif
