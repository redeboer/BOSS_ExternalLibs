/* Residual distribution method, author: Wu Linghui (wulh@ihep.ac.cn) */

#ifndef RESIALIGN_H
#define RESIALIGN_H

#include "MdcAlignAlg/MdcAlign.h"
#include "MdcAlignAlg/MdcAliParams.h"

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"

#include "TH1F.h"
#include "TGraph.h"

#include <fstream>

using namespace Alignment;

class ResiAlign: public MdcAlign {
public:
     ResiAlign();
     ~ResiAlign();
     void clear();
     void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
		     IMdcCalibFunSvc* mdcFunSvc);

     void setParam(MdcAliParams& param);
     bool fillHist(MdcAliEvent* event);
     void updateConst(MdcAlignPar* alignPar);
    
     static Double_t funResi(double* x, double* par);

private:
     MdcAliParams m_param;
     TObjArray*       m_hlist;
     IMdcGeomSvc*     m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     INTupleSvc*      ntupleSvc;

     int m_ndiv;
     double m_resiCut;
     bool m_layBound[LAYERNMAX];

     Long64_t m_npoint[NEP];
     double m_zrange[LAYERNMAX][2];
     double m_radii[LAYERNMAX];
     double m_docaMin[LAYERNMAX];
     double m_docaMax[LAYERNMAX];
     double m_xe[WIRENMAX];
     double m_ye[WIRENMAX];
     double m_ze[WIRENMAX];
     double m_xw[WIRENMAX];
     double m_yw[WIRENMAX];
     double m_zw[WIRENMAX];

     NTuple::Tuple*        m_tuple[NEP+1];
     NTuple::Item<long>    m_iRun[NEP+1];
     NTuple::Item<long>    m_iEvt[NEP+1];
     NTuple::Item<float>   m_resi[NEP+1];
     NTuple::Item<float>   m_p[NEP+1];
     NTuple::Item<float>   m_pt[NEP+1];
     NTuple::Item<float>   m_phi[NEP+1];
     NTuple::Item<float>   m_lay[NEP+1];
     NTuple::Item<float>   m_lr[NEP+1];
     NTuple::Item<float>   m_cel[NEP+1];

     std::ofstream m_fevt;
     
     TH1F* m_hnTrk;
     TH1F* m_hnHit;
     TH1F* m_hlayHitmap;
     TH1F* m_hresAll;
     TH1F* m_hresInn;
     TH1F* m_hresStp;
     TH1F* m_hresOut;
     TH1F* m_hresLay[LAYERNMAX];
     TGraph* m_gr[NEP];

     int m_ncut1;
     int m_ncut2;
     int m_ncut3;
     int m_ncut4;
     int m_ncut5;
     int m_ncut6;
     int m_ncut7;
     int m_ncut8;
     int m_ncut9;
     int m_ncut10;
     int m_ncut11;
     int m_ncut12;
     int m_ncut13;
};

inline void ResiAlign::setParam(MdcAliParams& param){
     MdcAlign::setParam(param);
     m_param = param;
}

#endif /*RESIALIGN_H*/
