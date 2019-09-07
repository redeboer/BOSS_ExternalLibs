#ifndef XTINTEMDCCALIB_H
#define XTINTEMDCCALIB_H

#include "MdcCalibAlg/MdcCalib.h"
#include <vector>
#include <map>

#include "TH2F.h"
#include "TGraph.h"
#include "TFolder.h"
#include "TProfile.h"

class XtInteMdcCalib: public MdcCalib {
public:
     XtInteMdcCalib();
     ~XtInteMdcCalib();
     void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
		     IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc);

     void setParam(MdcCalParams& param);
     int  fillHist(MdcCalEvent* event);
     int  updateConst(MdcCalibConst* calconst);
     void clear();

private:
     static const int NENTR = 18;
     static const int NTBIN_INN = 72;
     static const int NTBIN_OUT = 79;

     MdcCalParams m_param;

     TObjArray* m_hlist;
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcUtilitySvc*   m_mdcUtilitySvc;

     bool m_fgIni;
     int m_nMaxGrPoint;
     double m_docaMax[MdcCalNLayer];
     double m_tbinWid[MdcCalNLayer][3];
     double m_tbinLim[MdcCalNLayer][4];

     TFolder* m_fdPf;           /* TFolder for profile */
     TGraph* m_grXt[MdcCalNLayer][NENTR][2];
     TProfile* m_pfNear[MdcCalNLayer][NENTR][2];
     TProfile* m_pfMid[MdcCalNLayer][NENTR][2];
     TProfile* m_pfFar[MdcCalNLayer][NENTR][2];
};

inline void XtInteMdcCalib::setParam(MdcCalParams& param){
     MdcCalib::setParam(param);
     m_param = param;
}

#endif /* XTINTEMDCCALIB_H */
