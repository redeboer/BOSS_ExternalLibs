#ifndef Validation_ValidRecMucTrack_ValidRecMucTrackAlg_H
#define Validation_ValidRecMucTrack_ValidRecMucTrackAlg_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

//#include "AIDA/IAxis.h"
//#include "AIDA/IHistogram1D.h"

// Specify the namespace
//using AIDA::IHistogram1D;


class ValidRecMucTrackAlg  : public  Algorithm {

public:

  ValidRecMucTrackAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  NTuple::Tuple*  m_tuple1;

  NTuple::Item<double>    m_depth;
  NTuple::Item<double>    m_distance;
  NTuple::Item<double>    m_deltaPhi;
  NTuple::Item<double>    m_maxHits;
  NTuple::Item<double>    m_numLayers;
  NTuple::Item<double>    m_numHits;
  NTuple::Item<double>    m_chi2;
  NTuple::Item<double>    m_xPos;
  NTuple::Item<double>    m_yPos;
  NTuple::Item<double>    m_zPos;



};


#endif

