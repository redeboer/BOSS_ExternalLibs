#ifndef MDCALIGNALG_H
#define MDCALIGNALG_H

#include "GaudiKernel/Algorithm.h"

#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"

#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"

#include "MdcAlignAlg/MdcAlignPar.h"
#include "MdcAlignAlg/MdcAliEvent.h"
#include "MdcAlignAlg/MdcAliParams.h"
#include "MdcAlignAlg/Alignment.h"

#include "MdcAlignAlg/MdcAlign.h"

#include "TObjArray.h"

#include <iostream>
#include <fstream>
using namespace std;

/////////////////////////////////////////////////////////////////////////////

class MdcAlignAlg: public Algorithm {
public:
     MdcAlignAlg(const std::string& name, ISvcLocator* pSvcLocator);

     StatusCode initialize();
     StatusCode execute();
     StatusCode finalize();

private:
     void initParam();
     TObjArray*       m_hlist;
     IMdcGeomSvc*     m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     MdcAlign*        m_pAlign;
     MdcAliEvent*     m_mdcevt;
     MdcAlignPar*     m_alignPar;
     MdcAliParams     m_param;

     bool m_distAlign;
     int m_alignMeth;
     int m_flgKalFit;		/* 1 for using Kalman filter, 0 for not */
     int m_evtType;		/* 0: bhabha;  1: dimu */
     int m_nEvtDisp;
     int m_nEvt;
     double m_mag;		/* magnetic field [Tesla] */

     std::string m_alignFile;
     std::string m_configFile;
     std::string m_fixMomLab;
     std::string m_histname;
};

#endif /* MDCALIGNALG_H */
