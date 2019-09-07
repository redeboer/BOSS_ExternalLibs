// $Header: /bes/bes/BossCvs/Calibration/CalibData/CalibData/Tof/bTofCalibBase.h,v 1.8 2012/11/06 07:49:40 sunss Exp $
#ifndef CalibData_bTofCalibBase_h
#define CalibData_bTofCalibBase_h

/** @class CalibData_Mdct0_h

  calibration data class t0 
  calibration infrastructure

  @author huangb
*/
#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include <vector>


namespace CalibData {
  class bTofCalibBase{
   public:
     bTofCalibBase(){};

    virtual ~bTofCalibBase() {
      }

    //set the Calibdata of Tof
     void setAtten(const double* Atten);
     void setSpeed(const double* Speed);

     void setP1(const double* TofP1);
     void setP2(const double* TofP2);

     void setPoff1_bunch0(const double* TofPoff1_bunch0);
     void setPoff2_bunch0(const double* TofPoff2_bunch0);
     void setPoff1_bunch1(const double* TofPoff1_bunch1);
     void setPoff2_bunch1(const double* TofPoff2_bunch1);
     void setPoff1_bunch2(const double* TofPoff1_bunch2);
     void setPoff2_bunch2(const double* TofPoff2_bunch2);
     void setPoff1_bunch3(const double* TofPoff1_bunch3);
     void setPoff2_bunch3(const double* TofPoff2_bunch3);

     void setFPleft(const double* FP1);
     void setFPright(const double* FP2);
     void setFPcounter(const double* FP3);

     //get the CalibData of Tof
     double  getAtten(int No)  const{return m_Atten[No];} 
     double  getSpeed(int No) const{return m_Speed[No];}

     double  getP1(int No) const { return m_P1[No];}
     double  getP2(int No) const { return m_P2[No];}

     double  getPoff1_bunch0(int No) const { return m_Poff1_bunch0[No];}
     double  getPoff2_bunch0(int No) const { return m_Poff2_bunch0[No];}
     double  getPoff1_bunch1(int No) const { return m_Poff1_bunch1[No];}
     double  getPoff2_bunch1(int No) const { return m_Poff2_bunch1[No];}
     double  getPoff1_bunch2(int No) const { return m_Poff1_bunch2[No];}
     double  getPoff2_bunch2(int No) const { return m_Poff2_bunch2[No];}
     double  getPoff1_bunch3(int No) const { return m_Poff1_bunch3[No];}
     double  getPoff2_bunch3(int No) const { return m_Poff2_bunch3[No];}

     double  getFleft(int No) const{return m_FP1[No];}
     double  getFright(int No) const{return m_FP2[No];}
     double  getFcounter(int No) const{return m_FP3[No];}

  private:
     double m_Atten[8];
     double m_Speed[2];
     double m_P1[10];
     double m_P2[10];
     double m_Poff1_bunch0[20];
     double m_Poff2_bunch0[20];
     double m_Poff1_bunch1[20];
     double m_Poff2_bunch1[20];
     double m_Poff1_bunch2[20];
     double m_Poff2_bunch2[20];
     double m_Poff1_bunch3[20];
     double m_Poff2_bunch3[20];
     double m_FP1[5];
     double m_FP2[5];
     double m_FP3[10];
  };
}


#endif
