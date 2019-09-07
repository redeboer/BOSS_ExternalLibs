//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcMergeDups.h,v 1.3 2012/05/13 23:43:41 zhangy Exp $
//
// Description:
//	Class MdcMergeDups is to be merge duplicated track in MDC.
//
// Environment:
//	Software developed for the BESIII Detector at the BEPCII.
//
// Author List:
//	Yoshi Iwasaki(yoshihito.iwasaki@kek.jp)		Original Author
//	Zhang Yao(zhangyao@ihep.ac.cn)  		Migrate to BESIII
//
// Copyright Information:
//	Copyright (C) 2009	IHEP	
// 
// History:
//	Zhang Yao 2009-10-30 
//		Migration BELLE TTrackManager::merge() for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef MDCMERGECRUL_H
#define MDCMERGECRUL_H

//----------------------
// Base Class Headers --
//----------------------
#include "GaudiKernel/Algorithm.h"
#include "CLHEP/Alist/AList.h"
#include "MdcData/MdcHit.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MdcGeom/MdcDetector.h"
#include "BField/BField.h"

//---------------------
//-- Class Interface --
//---------------------

class MdcMergeDups : public Algorithm {

  //--------------------
  // Instance Members --
  //--------------------

  public: 
    // Constructors
    MdcMergeDups(const std::string& name, ISvcLocator* pSvcLocator);

    // Destructor
    virtual ~MdcMergeDups();

    // Operations
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize(); 
    StatusCode beginRun(); 
    int  mergeDups(void);
    int  mergeCurl(void);
    int  testByOverlapHit(RecMdcTrack* refTk, RecMdcTrack* testTk);
    int  testByParam(RecMdcTrack* refTk, RecMdcTrack* testTk);
    int  doMergeLong(std::vector<RecMdcTrack*> mergeTkList);
    int  doMergeCurl(std::vector<RecMdcTrack*> mergeTkList);
    //int  doMergeOdd(std::vector<RecMdcTrack*> mergeTkList);
    void store(TrkRecoTrk* aTrack);
    bool eraseTdsTrack(RecMdcTrackCol::iterator tk);

    void dumpRecMdcTrack();

  private:
    const MdcDetector* m_gm;
    BField*	m_bfield;

    int    	m_debug;
    double	m_maxDd0InMerge; 
    double	m_maxDphi0InMerge; 
    double	m_maxPdradInMerge; 
    double	m_maxRcsInMerge; 
    double 	m_mergePt;
    double 	m_mergeLoadAx;
    double 	m_mergeLoadSt;
    double 	m_mergeOverlapRatio;
    double      m_bunchT0;
};
#endif
