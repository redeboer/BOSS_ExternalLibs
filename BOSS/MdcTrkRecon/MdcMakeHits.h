#ifndef MDCMAKEHITS_H
#define MDCMAKEHITS_H

#include "GaudiKernel/Algorithm.h"
#include <string>
#include "RawDataProviderSvc/RawDataProviderSvc.h"

class MdcDetector;
class MdcCalibFunSvc;

class MdcMakeHits : public Algorithm{

public:
  MdcMakeHits(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode beginRun();

private:
  bool  m_cosmicFit;
  bool  m_doSag;

  uint32_t 	    m_getDigiFlag;
  int		    m_maxMdcDigi;
  bool		    m_keepBadTdc;
  bool		    m_dropHot;
  bool		    m_keepUnmatch;
  int		    m_minMdcDigi;
  bool		    m_combineTracking;

  bool	m_countPropTime;
  const MdcDetector * _gm;
  const MdcCalibFunSvc* m_mdcCalibFunSvc;
  RawDataProviderSvc* m_rawDataProviderSvc;

#ifdef MDCPATREC_RESLAYER
  int  m_resLayer;
#endif
};



#endif
