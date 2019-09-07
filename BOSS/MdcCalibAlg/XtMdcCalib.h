#ifndef XTMDCCALIB_H
#define XTMDCCALIB_H

#include "MdcCalibAlg/MdcCalib.h"
#include <vector>
#include <map>

#include "TFolder.h"

class XtMdcCalib: public MdcCalib {
public:
     XtMdcCalib();
     ~XtMdcCalib();
     void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
		     IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc);

     void setParam(MdcCalParams& param);
     int  fillHist(MdcCalEvent* event);
     int  updateConst(MdcCalibConst* calconst);
     void clear();

     int getHxtKey(int lay, int entr, int lr, int bin) const;

     static void fcnXT(Int_t &npar, Double_t *gin, Double_t &f,
		       Double_t *par, Int_t iflag);
     static void fcnXtEdge(Int_t &npar, Double_t *gin, Double_t &f,
                       Double_t *par, Int_t iflag);
     static double xtFun(double t, double xtpar[]);

     static std::vector<double> XMEAS;
     static std::vector<double> TBINCEN;
     static std::vector<double> ERR;
 
     static double              Tmax;
     static double              Dmax;
     static std::vector<double> XMEASED;
     static std::vector<double> TBINCENED;
     static std::vector<double> ERRED;

private:
     MdcCalParams m_param;

     TObjArray* m_hlist;
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcUtilitySvc* m_mdcUtilitySvc;

     int m_nlayer;
     int m_nEntr[43];
     int m_nLR;
     int m_nbin;
     int m_nxtpar;
     double m_tbinw;

     bool m_fgIni;
     double m_docaMax[MdcCalNLayer];
     double m_tm[MdcCalNLayer][MdcCalNENTRXT][MdcCalLR];

     TFolder* m_fdXt;
     std::vector<TH1D*> m_hxt;
     std::map<int, int> m_hxtmap;


     /* for the index of the histograms */
     static const int HXTLAYER_INDEX    = 13;
     static const int HXTLAYER_MASK     = 0x7E000;

     static const int HXTENTRA_INDEX    = 8;
     static const int HXTENTRA_MASK     = 0x1F00;

     static const int HXTLR_INDEX       = 6;
     static const int HXTLR_MASK        = 0xC0;

     static const int HXTBIN_INDEX      = 0;
     static const int HXTBIN_MASK       = 0x3F;

};

inline void XtMdcCalib::setParam(MdcCalParams& param){
     MdcCalib::setParam(param);
     m_param = param;
}

#endif /* XTMDCCALIB_H */
