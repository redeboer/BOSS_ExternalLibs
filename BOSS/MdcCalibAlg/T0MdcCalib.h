#ifndef T0MDCCALIB_H
#define T0MDCCALIB_H

#include "MdcCalibAlg/MdcCalib.h"

class T0MdcCalib: public MdcCalib {
public:
     T0MdcCalib();
     ~T0MdcCalib();
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

     double m_vdr;

     /* for track cut */
     double m_docaMin[MdcCalNLayer];
     double m_docaMax[MdcCalNLayer];

     TFolder* m_fdT0;
     TFolder* m_fdResiWire;
     TH1F* m_hleft[MdcCalTotCell];
     TH1F* m_hright[MdcCalTotCell];
     TH1F* m_hLrResiSum;
     TH1F* m_hLrResiSub;
};

inline void T0MdcCalib::setParam(MdcCalParams& param){
     MdcCalib::setParam(param);
     m_param = param;
}

#endif /* T0MDCCALIB_H */
