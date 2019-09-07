#ifndef QTMDCCALIB_H
#define QTMDCCALIB_H

#include "MdcCalibAlg/MdcCalib.h"
#include "TFolder.h"
#include "TGraphErrors.h"

class QtMdcCalib: public MdcCalib {
public:
     QtMdcCalib();
     ~QtMdcCalib();
     void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
		     IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc);

     void setParam(MdcCalParams& param);
     int  fillHist(MdcCalEvent* event);
     int  updateConst(MdcCalibConst* calconst);
     void clear();

     static Double_t qtFun(Double_t *x, Double_t *par);

private:
     MdcCalParams m_param;

     TObjArray* m_hlist;
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcUtilitySvc*   m_mdcUtilitySvc;

     int m_nlayer;
     int m_nbin;
     int m_innNLay;
     int m_qtorder;

     double m_qhit;
     double m_resi;
     double m_vdr;

     double m_qbinw[MdcCalNLayer]; // width of Q bin for all cells
     double m_qmin[MdcCalNLayer];
     double m_qmax[MdcCalNLayer];
     double m_qtpar[MdcCalNLayer][MdcCalQtOrd];

     TFolder* m_fdQt;
     TH1F* m_hqhit[MdcCalNLayer];
     TH1F* m_hqt[MdcCalNLayer][MdcCalNQBin];
     TFolder* m_fdQ_T;
     TGraphErrors* m_grqt[MdcCalNLayer];
     TGraphErrors* m_grqdt[MdcCalNLayer];
};

inline void QtMdcCalib::setParam(MdcCalParams& param){
     MdcCalib::setParam(param);
     m_param = param;
}

#endif /* QTMDCCALIB_H */
