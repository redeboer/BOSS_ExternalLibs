#ifndef GRXTMDCCALIB_H
#define GRXTMDCCALIB_H

#include "MdcCalibAlg/MdcCalib.h"
#include <vector>
#include <map>

#include "TH2F.h"
#include "TGraph.h"
#include "TFolder.h"

class GrXtMdcCalib: public MdcCalib {
public:
     GrXtMdcCalib();
     ~GrXtMdcCalib();
     void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
		     IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc);

     void setParam(MdcCalParams& param);
     int  fillHist(MdcCalEvent* event);
     int  updateConst(MdcCalibConst* calconst);
     void clear();

     int findXtEntr(int lay, int iEntr, int lr) const;

     static Double_t xtfun(Double_t *x, Double_t *par);
     static Double_t xtedge(Double_t *x, Double_t *par);

private:
     MdcCalParams m_param;

     TObjArray* m_hlist;
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcUtilitySvc*   m_mdcUtilitySvc;

     int m_maxNhit;		/* for each graph */
     bool m_fgIni;
     double m_docaMax[MdcCalNLayer];

     int m_nhit[MdcCalNLayer][MdcCalNENTRXT][MdcCalLR];
     bool m_fgFit[MdcCalNLayer][MdcCalNENTRXT][MdcCalLR];

     TFolder* m_fdXt;
     TH2F* m_haxis;
     TGraph* m_grxt[MdcCalNLayer][MdcCalNENTRXT][MdcCalLR];

     static double DMAX;
     static double TMAX;
};

inline void GrXtMdcCalib::setParam(MdcCalParams& param){
     MdcCalib::setParam(param);
     m_param = param;
}

#endif /* GRXTMDCCALIB_H */
