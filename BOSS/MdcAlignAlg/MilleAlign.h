/* Millepede matrix method for alignment, author: Volker Blobel  */
/* First used in BESIII drift chamber by Wu Linghui (wulh@ihep.ac.cn) */

#ifndef MILLEALIGN_H
#define MILLEALIGN_H

#include "MdcAlignAlg/MdcAlign.h"
#include "MdcAlignAlg/Millepede.h"
#include "MdcAlignAlg/Alignment.h"
#include "MdcUtilitySvc/MdcUtilitySvc.h"

#include "TH1F.h"

using namespace Alignment;

class MilleAlign: public MdcAlign {
public:
     MilleAlign();
     ~MilleAlign();
     void clear();
     void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
		     IMdcCalibFunSvc* mdcFunSvc);

     void setParam(MdcAliParams& param);
     bool fillHist(MdcAliEvent* event);
     void updateConst(MdcAlignPar* alignPar);
public:
     std::string fixMomLab;

private:
     int getAlignParId(int lay, int iparHit);

     bool getDeriLoc(int ipar, int lay, int cel, HepVector helix, HepSymMatrix &helixErr, double &deri);
     bool getDeriGlo(int iparHit, int iparGB, int lay, int cel, HepVector helix, HepSymMatrix &helixErr,
         double wpos[], double &deri);

     double moment(double phi0, double tanl);

     MdcAliParams m_param;
     TObjArray* m_hlist;
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     MdcUtilitySvc*   m_mdcUtilitySvc;

     double m_resiCut[LAYERNMAX];
     double m_docaCut[LAYERNMAX][2];

     double m_p[20][15];
     double m_dxini[NEP];
     double m_dyini[NEP];
     double m_rzini[NEP];

     /* for millepede */
     Millepede* m_pMilleAlign;
     int m_npar;
     bool m_dofs[NDOFALIGN];
     double m_sigm[NDOFALIGN];
     int m_nGloHit;		/* number of global pars for each hit */
     int m_nglo;
     int m_nloc;
     std::vector<double> m_par;
     std::vector<double> m_error;
     std::vector<double> m_pull;
     std::vector<double> m_derGB;
     std::vector<double> m_derNonLin;
     std::vector<double> m_derLC;

     /* histograms */
     TH1F* m_hresAll;
     TH1F* m_hresInn;
     TH1F* m_hresStp;
     TH1F* m_hresOut;
     TH1F* m_hresLay[LAYERNMAX];

     TH1F* m_hresAllRec;
     TH1F* m_hresLayRec[LAYERNMAX];

     TH1F* m_hddoca;		/* for debug */
     TH1F* m_hddocaLay[LAYERNMAX];		/* for debug */
};

inline void MilleAlign::setParam(MdcAliParams& param){
     MdcAlign::setParam(param);
     m_param = param;
}

#endif /* MILLEALIGN_H */
