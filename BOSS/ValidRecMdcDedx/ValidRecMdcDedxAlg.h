#ifndef Validation_ValidRecMdcDedx_ValidRecMdcDedxAlg_H
#define Validation_ValidRecMdcDedx_ValidRecMdcDedxAlg_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"

// Specify the namespace
using AIDA::IHistogram1D;


class ValidRecMdcDedxAlg  : public  Algorithm {

public:

  ValidRecMdcDedxAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  NTuple::Tuple*  m_tuple1;

  NTuple::Item<double>           m_dedx;
  NTuple::Item<double>           m_parttype;
  NTuple::Item<double>           m_chidedx;
  NTuple::Item<double>           m_chidedxE;
  NTuple::Item<double>           m_chidedxMu;
  NTuple::Item<double>           m_chidedxPi;
  NTuple::Item<double>           m_chidedxK;
  NTuple::Item<double>           m_chidedxP;
  	    
 };


#endif

