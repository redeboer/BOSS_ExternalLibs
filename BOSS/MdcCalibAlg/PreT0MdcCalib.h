#ifndef PRET0MDCCALIB_H
#define PRET0MDCCALIB_H

#include "MdcCalibAlg/MdcCalib.h"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeomSvc.h"

#include "TGraph.h"
#include <vector>


class PreT0MdcCalib: public MdcCalib {
public:
     PreT0MdcCalib();              
     ~PreT0MdcCalib();   
     void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
                     IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc);

     void setParam(MdcCalParams& param);
     int  fillHist(MdcCalEvent* event);
     int  updateConst(MdcCalibConst* calconst);
     void clear();

private:

     static Double_t funTmin(Double_t* x, Double_t* par);
     static Double_t funTmax(Double_t* x, Double_t* par);     
     static Double_t xtfun(Double_t *x, Double_t *par);

     MdcCalParams m_param;
 
     TObjArray* m_hlist;
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcUtilitySvc*   m_mdcUtilitySvc;

     int m_nzbin;
     double m_zwid[MdcCalNLayer];
     double m_zst[MdcCalNLayer];
     double m_vp[MdcCalNLayer];

     TFolder* m_fdTrec;
     TFolder* m_fdTrecZ;
     TH1F* m_hTrec[MdcCalNLayer][MdcCalLR];
     TH1F* m_hTrecCosm[MdcCalNLayer][2];
     TH1F* m_hTrecZ[MdcCalNLayer][MdcCalLR][11];
};

inline void PreT0MdcCalib::setParam(MdcCalParams& param){
     MdcCalib::setParam(param);
     m_param = param;
}
#endif  /* PRET0MDCCALIB_H */
