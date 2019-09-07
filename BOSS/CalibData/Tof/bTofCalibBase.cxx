// $Header: /bes/bes/BossCvs/Calibration/CalibData/src/Tof/bTofCalibBase.cxx,v 1.8 2012/11/06 07:49:40 sunss Exp $

/** @class bTofCalibBase
 *    Implementation of Mdct0 calibration TCDS class
 */

#include "CalibData/Tof/bTofCalibBase.h"
#include "GaudiKernel/MsgStream.h"

namespace CalibData {
 
  void bTofCalibBase::setAtten(const double* Atten){
    for( unsigned int i=0; i<8; i++ ) {
      m_Atten[i] = Atten[i];
    }
  }

  void bTofCalibBase::setSpeed(const double* Speed){
    for( unsigned int i=0; i<2; i++ ) {
      m_Speed[i] = Speed[i];
    }
  }
   
  void bTofCalibBase::setP1(const double* TofP1){
    for(int i=0;i<10;i++){
      m_P1[i] = TofP1[i];
    }
  }

  void bTofCalibBase::setP2(const double* TofP2){
    for(int i=0;i<10;i++){
      m_P2[i] = TofP2[i];
    }
  }

  void bTofCalibBase::setPoff1_bunch0(const double* TofPoff1_bunch0){
    for(int i=0;i<20;i++){
      m_Poff1_bunch0[i] = TofPoff1_bunch0[i];
    }
  }

  void bTofCalibBase::setPoff2_bunch0(const double* TofPoff2_bunch0){
    for(int i=0;i<20;i++){
      m_Poff2_bunch0[i] = TofPoff2_bunch0[i];
    }
  }

  void bTofCalibBase::setPoff1_bunch1(const double* TofPoff1_bunch1){
    for(int i=0;i<20;i++){
      m_Poff1_bunch1[i] = TofPoff1_bunch1[i];
    }
  }

  void bTofCalibBase::setPoff2_bunch1(const double* TofPoff2_bunch1){
    for(int i=0;i<20;i++){
      m_Poff2_bunch1[i] = TofPoff2_bunch1[i];
    }
  }

  void bTofCalibBase::setPoff1_bunch2(const double* TofPoff1_bunch2){
    for(int i=0;i<20;i++){
      m_Poff1_bunch2[i] = TofPoff1_bunch2[i];
    }
  }

  void bTofCalibBase::setPoff2_bunch2(const double* TofPoff2_bunch2){
    for(int i=0;i<20;i++){
      m_Poff2_bunch2[i] = TofPoff2_bunch2[i];
    }
  }  

  void bTofCalibBase::setPoff1_bunch3(const double* TofPoff1_bunch3){
    for(int i=0;i<20;i++){
      m_Poff1_bunch3[i] = TofPoff1_bunch3[i];
    }
  }

  void bTofCalibBase::setPoff2_bunch3(const double* TofPoff2_bunch3){
    for(int i=0;i<20;i++){
      m_Poff2_bunch3[i] = TofPoff2_bunch3[i];
    }
  }  

  void bTofCalibBase::setFPleft(const double* FP1) {
    for(int i=0; i<5; i++){
      m_FP1[i] = FP1[i];
    }
  }

  void bTofCalibBase::setFPright(const double* FP2) {
    for(int i=0; i<5; i++){
      m_FP2[i] = FP2[i];
    }
  }

  void bTofCalibBase::setFPcounter(const double* FP3) {
    for(int i=0; i<10; i++){
      m_FP3[i] = FP3[i];
    }
  }

}

