#ifndef MDCCALIBALG_H
#define MDCCALIBALG_H

#include "GaudiKernel/Algorithm.h"

#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"

#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "MdcUtilitySvc/IMdcUtilitySvc.h"
#include "MdcUtilitySvc/MdcUtilitySvc.h"

#include "MdcCalibAlg/MdcCalibConst.h"
#include "MdcCalibAlg/MdcCalEvent.h"
#include "MdcCalibAlg/MdcCalConstMgr.h"
#include "MdcCalibAlg/MdcCalParams.h"

#include "MdcCalibAlg/MdcCalib.h"
#include "MdcCalibAlg/IniMdcCalib.h"
#include "MdcCalibAlg/PreXtMdcCalib.h"
#include "MdcCalibAlg/PreT0MdcCalib.h"
#include "MdcCalibAlg/XtMdcCalib.h"
#include "MdcCalibAlg/GrXtMdcCalib.h"
#include "MdcCalibAlg/XtInteMdcCalib.h"
#include "MdcCalibAlg/T0MdcCalib.h"
#include "MdcCalibAlg/WrMdcCalib.h"
#include "MdcCalibAlg/Wr2dMdcCalib.h"
#include "MdcCalibAlg/QtMdcCalib.h"

#include "TObjArray.h"
#include "TFile.h"


/////////////////////////////////////////////////////////////////////////////

class MdcCalibAlg: public Algorithm {
public:
     MdcCalibAlg(const std::string& name, ISvcLocator* pSvcLocator);
     ~MdcCalibAlg();

     StatusCode initialize();
     StatusCode execute();
     StatusCode finalize();

private:
     void initParam();

     TFile*           m_fhist;
     TObjArray*       m_hlist;
     IMdcGeomSvc*     m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcUtilitySvc*   m_mdcUtilitySvc;
     MdcCalConstMgr*  m_constmgr;
     MdcCalib*        m_mdccalib;
     MdcCalEvent*     m_mdcevt;
     MdcCalibConst*   m_calconst;
     MdcCalParams     m_param;

     bool m_initCalConstFlg;
     int m_mdcCalFlg;
     int m_flgKalFit;		/* 1 for using Kalman filter, 0 for not */
     int m_evtType;		/* 0-6: e,mu,pi,k,p,cosmic-ray */
     int m_nEvtDisp;
     int m_nEvt;
     bool m_distCalib;
     double m_ecm;

     std::string m_histname;
     std::string m_configFile;
     std::string m_wpcFile;	/* single wire position calibration file */
};

#endif /* MDCCALIBALG_H */
