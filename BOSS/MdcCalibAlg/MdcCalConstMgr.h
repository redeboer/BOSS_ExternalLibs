#ifndef MDCCALCONSTMGR_H
#define MDCCALCONSTMGR_H

#include "MdcCalibAlg/MdcCalParams.h"
#include "MdcCalibAlg/MdcCalibConst.h"

#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"

class MdcCalConstMgr {
public:
     MdcCalConstMgr();
     ~MdcCalConstMgr();
     void init(IMdcGeomSvc* mdcGeomSvc, IMdcCalibFunSvc* mdcFunSvc);

/* read and write calibration data from TCDS */
     void rdConstTcds(MdcCalibConst* calconst);
     void wrtConstTcds(MdcCalibConst* calconst);

/* read and write calibration data directly in run directory */
     void rdConst(MdcCalibConst* calconst);
     void wrtConst(MdcCalibConst* calconst);

private:
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcGeomSvc* m_mdcGeomSvc;
};

#endif /* MDCCALCONSTMGR_H */
