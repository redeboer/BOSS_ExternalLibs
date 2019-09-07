#ifndef Validation_ValidRecExtTrack_ValidRecExtTrackAlg_H
#define Validation_ValidRecExtTrack_ValidRecExtTrackAlg_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

//#include "AIDA/IAxis.h"
//#include "AIDA/IHistogram1D.h"

// Specify the namespace
//using AIDA::IHistogram1D;


class ValidRecExtTrackAlg  : public  Algorithm {

public:

  ValidRecExtTrackAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  NTuple::Tuple*  m_tuple1;

  NTuple::Item<double>    m_tof1;
  NTuple::Item<double>    m_tof1Path;
  NTuple::Item<double>    m_tof1PosSigmaAlongX;
  NTuple::Item<double>    m_xTof1Pos;
  NTuple::Item<double>    m_yTof1Pos;
  NTuple::Item<double>    m_zTof1Pos;
  NTuple::Item<double>    m_emTof1_11;
  NTuple::Item<double>    m_emTof1_33;
  NTuple::Item<double>    m_emTof1_55;
  NTuple::Item<double>    m_emTof1_66;
  NTuple::Item<double>    m_emTof1_23;
  NTuple::Item<double>    m_emTof1_35;

};


#endif

