#ifndef WRMDCCALIB_H
#define WRMDCCALIB_H

#include "MdcCalibAlg/MdcCalib.h"

class WrMdcCalib: public MdcCalib {
public:
     WrMdcCalib();
     ~WrMdcCalib();

     void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
		     IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc);

     void setParam(MdcCalParams& param);
     int  fillHist(MdcCalEvent* event);
     int  updateConst(MdcCalibConst* calconst);
     void clear();

private:
     MdcCalParams m_param;

     TObjArray* m_hlist;
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcUtilitySvc*   m_mdcUtilitySvc;

     /* for track cut */
     double m_docaMin[MdcCalNLayer];
     double m_docaMax[MdcCalNLayer];

     TFolder* m_fdWire;
     TFolder* m_fdResiWire;
     TH1F* m_hleft[MdcCalTotCell];
     TH1F* m_hright[MdcCalTotCell];
     TH1F* m_hdwxtot;
     TH1F* m_hddwx;
     TH1F* m_hdwytot;
     TH1F* m_hddwy;
     TH1F* m_hLrResiSum;
     TH1F* m_hLrResiSub;
};

inline void WrMdcCalib::setParam(MdcCalParams& param){
     MdcCalib::setParam(param);
     m_param = param;
}

#endif /* WRMDCCALIB_H */
