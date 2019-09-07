#ifndef WR2DMDCCALIB_H
#define WR2DMDCCALIB_H

#include "MdcCalibAlg/MdcCalib.h"

class Wr2dMdcCalib: public MdcCalib {
public:
     Wr2dMdcCalib();
     ~Wr2dMdcCalib();

     void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
		     IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc);

     void setParam(MdcCalParams& param);
     int  fillHist(MdcCalEvent* event);
     int  updateConst(MdcCalibConst* calconst);
     void clear();

     static void fcnWireParab(Int_t &npar, Double_t *gin, Double_t &f,
			      Double_t *par, Int_t iflag);
     static bool fgBIN[MdcCalWrNBin];
     static double xBIN[MdcCalWrNBin];
     static double yBIN[MdcCalWrNBin];
     static double zBIN[MdcCalWrNBin];
     static double zBINERR[MdcCalWrNBin];
     static double zMIN;
     static double zMAX;

private:
     MdcCalParams m_param;

     TObjArray* m_hlist;
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcUtilitySvc*   m_mdcUtilitySvc;

     double m_zwest[MdcCalNLayer];
     double m_zeast[MdcCalNLayer];
     double m_zwid[MdcCalNLayer];
     double m_zbinCen[MdcCalNLayer][MdcCalWrNBin];

     TFolder* m_fdWire;
     TH1F* m_hl[MdcCalTotCell][MdcCalWrNBin];
     TH1F* m_hr[MdcCalTotCell][MdcCalWrNBin];
};

inline void Wr2dMdcCalib::setParam(MdcCalParams& param){
     MdcCalib::setParam(param);
     m_param = param;
}

#endif /* WR2DMDCCALIB_H */
