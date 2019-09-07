#ifndef PREXTMDCCALIB_H
#define PREXTMDCCALIB_H

#include "MdcCalibAlg/MdcCalib.h"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeomSvc.h"

#include "TH2F.h"
#include "TGraph.h"
#include "TFolder.h"

class PreXtMdcCalib: public MdcCalib {
public:
     PreXtMdcCalib();              
     ~PreXtMdcCalib();   
     void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
                     IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc);

     void setParam(MdcCalParams& param);
     int  fillHist(MdcCalEvent* event);
     int  updateConst(MdcCalibConst* calconst);
     void clear();

private:
     static Double_t xtfun(Double_t *x, Double_t *par);

     MdcCalParams m_param;
 
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcUtilitySvc*   m_mdcUtilitySvc;
     TObjArray* m_hlist;
     
     int m_nWire;
     int m_nLayer;

     TFolder* m_fdPreXt;
     TFolder* m_fdNhit;
     TGraph* m_grXt[MdcCalNLayer];
     TH2F* m_haxis;
     TH1F* m_htrec[MdcCalNLayer];
     TH1F* m_nhitBin[MdcCalNLayer];
     TH1F* m_nhitTot;

     /* integrated time Spectrum for determination X-T */
     int m_nXtBin;
     double m_tbin[40];
     int m_nEntries[MdcCalNLayer][40];
     int m_nTot[MdcCalNLayer];

     bool m_fgIniTm;
     double m_t0[MdcCalNLayer];
     double m_tm[MdcCalNLayer];
};

inline void PreXtMdcCalib::setParam(MdcCalParams& param){
     MdcCalib::setParam(param);
     m_param = param;
}
#endif  /* PREXTMDCCALIB_H */
