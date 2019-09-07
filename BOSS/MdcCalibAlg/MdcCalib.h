#ifndef MDCCALIB_H
#define MDCCALIB_H


#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"

#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"

#include "MdcCalibAlg/MdcCalParams.h"
#include "MdcCalibAlg/MdcCalEvent.h"
#include "MdcCalibAlg/MdcCalibConst.h"
#include "MdcUtilitySvc/IMdcUtilitySvc.h"
#include "MdcUtilitySvc/MdcUtilitySvc.h"

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"

#include "TROOT.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TH1F.h"
#include "TF1.h"
#include "TTree.h"
#include "TFolder.h"
#include "TGraph.h"

#include <fstream>
#include <vector>
#include <map>

//const double pi = 3.14159265;
class MdcCalib {
public:
     MdcCalib();
     virtual ~MdcCalib();
     virtual void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			     IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) = 0;

     virtual void setParam(MdcCalParams& param) = 0;
     virtual int  fillHist(MdcCalEvent* event) = 0;
     virtual int  updateConst(MdcCalibConst* calconst) = 0;
     virtual void clear() = 0;

private:
     int getHresId(int lay, int entr, int lr, int bin) const;
     int calDetEffi();
     bool getCellTrkPass(MdcCalEvent* event, int iTrk, int cellTrkPass[]);

     MdcCalParams m_param;

     TObjArray* m_hlist;
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcUtilitySvc*   m_mdcUtilitySvc;

     int m_nEvt;
     int m_cut1;
     int m_cut2;
     int m_cut3;
     int m_cut4;
     int m_cut5;
     int m_cut6;

     int m_nlayer;
     int m_nEntr[43];
     int m_nBin[MdcCalNLayer];
     bool fgReadWireEff;

     bool m_layBound[MdcCalNLayer];

     /* for calculating efficiency */
/*      int m_nTrk[43]; */
/*      int m_nGoodHit[43]; */
     bool m_fgGoodWire[43][288];
     TH1F* m_effNtrk;
     TH1F* m_effNtrkRecHit;

     /* calculating efficiency without the impact of track fitting */
     int m_hitNum[43][2];

     /* for track cut */
     bool m_fgIni;
     double m_docaMax[MdcCalNLayer];

     /* for calculating hitphi */
     double m_radii[MdcCalNLayer];
     double m_xe[MdcCalTotCell];
     double m_ye[MdcCalTotCell];
     double m_ze[MdcCalTotCell];
     double m_xw[MdcCalTotCell];
     double m_yw[MdcCalTotCell];
     double m_zw[MdcCalTotCell];

     TFolder* m_fdcom;

     TH1F* m_hresAllInc;
     TH1F* m_hresAllExc;
     TH1F* m_hresAllAve;

     TH1F* m_hresInnInc;
     TH1F* m_hresInnExc;

     TH1F* m_hresStpInc;
     TH1F* m_hresStpExc;

     TH1F* m_hresOutInc;
     TH1F* m_hresOutExc;

     TFolder* m_fdResQ;
     TH1F* m_hresAveAllQ[14];
     TH1F* m_hresAveOutQ[14];
     TH1F* m_hresAveLayQ[43][14];

     TH1F* m_hratio;

     TH1F* m_hTes[10];
     TH1F* m_hbbTrkFlg;
     TH1F* m_hTesAll;
     TH1F* m_hTesGood;
     TH1F* m_hTesAllFlag;
     TH1F* m_hTesRec;
     TH1F* m_hTesCalFlag;
     TH1F* m_hTesCalUse;

     TH1F* m_hnRawHit;

     TH1F* m_hpt;
     TH1F* m_hpMax;
     TH1F* m_hpMaxCms;
     TH1F* m_hptPos;
     TH1F* m_hptNeg;

     TH1F* m_hp;
     TH1F* m_hp_cms;
     TH1F* m_hpPos;
     TH1F* m_hpNeg;
     TH1F* m_hpPoscms;
     TH1F* m_hpNegcms;

     //results after four momentum cut
     TH1F* m_hp_cut;

     TH1F* m_hnTrk;
     TH1F* m_hnTrkCal;
     TH1F* m_hnhitsRec;
     TH1F* m_hnhitsRecInn;
     TH1F* m_hnhitsRecStp;
     TH1F* m_hnhitsRecOut;
     TH1F* m_hnhitsCal;
     TH1F* m_hnhitsCalInn;
     TH1F* m_hnhitsCalStp;
     TH1F* m_hnhitsCalOut;
     TH1F* m_wirehitmap;
     TH1F* m_layerhitmap;
     TH1F* m_hchisq;

     // histogram for distribution of noise
     TH1F* m_hnoisephi;
     TH1F* m_hnoiselay;
     TH1F* m_hnoisenhits;

     TH1F* m_hdr;
     TH1F* m_hphi0;
     TH1F* m_hkap;
     TH1F* m_hdz;
     TH1F* m_htanl;
     TH1F* m_hcosthe;
     TH1F* m_hcostheNeg;
     TH1F* m_hcosthePos;

     TH1F* m_hx0;
     TH1F* m_hy0;
     TH1F* m_hdelZ0;
     TGraph* m_grX0Y0;
     int m_nGrPoint;

     TH1F* m_hitEffAll;
     TH1F* m_hitEffRaw;
     TH1F* m_hitEffRec;

     /* X-T ntuple */
     int m_nEvtNtuple;
     NTuple::Tuple*  m_xtTuple[MdcCalNLayer];
     NTuple::Item<long> m_cel[MdcCalNLayer];
     NTuple::Item<long> m_lr[MdcCalNLayer];
     NTuple::Item<long> m_run[MdcCalNLayer];
     NTuple::Item<long> m_evt[MdcCalNLayer];
     NTuple::Item<double> m_doca[MdcCalNLayer];
     NTuple::Item<double> m_dm[MdcCalNLayer];
     NTuple::Item<double> m_tdr[MdcCalNLayer];
     NTuple::Item<double> m_tdc[MdcCalNLayer];
     NTuple::Item<double> m_entr[MdcCalNLayer];
     NTuple::Item<double> m_zhit[MdcCalNLayer];
     NTuple::Item<double> m_qhit[MdcCalNLayer];
     NTuple::Item<double> m_p[MdcCalNLayer];
     NTuple::Item<double> m_pt[MdcCalNLayer];
     NTuple::Item<double> m_phi0[MdcCalNLayer];
     NTuple::Item<double> m_tanl[MdcCalNLayer];
     NTuple::Item<double> m_hitphi[MdcCalNLayer];

     /* histograms drift time */
     TFolder* m_fdTime;
     TH1F* m_htraw[MdcCalNLayer];
     TH1F* m_htdr[MdcCalNLayer];
     TH1F* m_htdrlr[MdcCalNLayer][2];

     /* histograms of adc */
     TFolder* m_fdAdc;
     TH1F* m_hadc[MdcCalNLayer];

     /* histograms for spatial resolution */
     TFolder* m_fdres;
     TH1F* m_hresInc[MdcCalNLayer];
     TH1F* m_hreslrInc[MdcCalNLayer][2];
     TH1F* m_hresExc[MdcCalNLayer];
     TH1F* m_hreslrExc[MdcCalNLayer][2];
     TH1F* m_hresphi[MdcCalNLayer][20];

     TFolder* m_fdresAve;
     TH1F* m_hresAve[MdcCalNLayer];
     TH1F* m_hreslrAve[MdcCalNLayer][2];

     /* histograms for momentum vs phi */
     static const int NPhiBin = 20;
     static const int NThetaBin = 9;
     double m_phiWid;
     double m_theWid;
     TFolder* m_fdmomPhi;
     //in experimental reference frame
     TH1F* m_ppPhi[NPhiBin];
     TH1F* m_pnPhi[NPhiBin];
     TH1F* m_ppThe[NThetaBin];
     TH1F* m_pnThe[NThetaBin];
     TH1F* m_ppThePhi[NThetaBin][NPhiBin];
     TH1F* m_pnThePhi[NThetaBin][NPhiBin];

     //in CMS
     TH1F* m_ppPhiCms[NPhiBin];
     TH1F* m_pnPhiCms[NPhiBin];
     TH1F* m_ppTheCms[NThetaBin];
     TH1F* m_pnTheCms[NThetaBin];
     TH1F* m_ppThePhiCms[NThetaBin][NPhiBin];
     TH1F* m_pnThePhiCms[NThetaBin][NPhiBin];

     /* ntuple for cosmic-ray */
     NTuple::Tuple*  m_cosmic;
     NTuple::Item<double> m_pUpcos;
     NTuple::Item<double> m_pDwcos;
     NTuple::Item<double> m_ptUpcos;
     NTuple::Item<double> m_ptDwcos;
     NTuple::Item<double> m_phiUpcos;
     NTuple::Item<double> m_phiDwcos;
     NTuple::Item<double> m_drUpcos;
     NTuple::Item<double> m_drDwcos;
     NTuple::Item<double> m_dzUpcos;
     NTuple::Item<double> m_dzDwcos;
     NTuple::Item<double> m_ctheUpcos;
     NTuple::Item<double> m_ctheDwcos;
     NTuple::Item<long> m_nhitUpcos;
     NTuple::Item<long> m_nhitDwcos;
     NTuple::Item<long> m_chargecos;
     NTuple::Item<long> m_tesFlagcos;
     NTuple::Item<double> m_tescos;

     /* histograms for spatial resolution vs distance */
     double m_dwid;
     TFolder* m_fdres2d;
     std::map<int, int> m_mapr2d;
     std::vector<TH1F*> m_hr2dInc;
     std::vector<TH1F*> m_hr2dExc;

     TFolder* m_fdres2t;
     TH1F* m_hr2t[MdcCalNLayer][MdcCalNENTRXT][2][45];

     /* for the index of resolution histograms */
     static const int HRESLAYER_INDEX  = 10;
     static const int HRESLAYER_MASK   = 0xFC00;

     static const int HRESENTRA_INDEX  = 7;
     static const int HRESENTRA_MASK   = 0x380;

     static const int HRESLR_INDEX     = 5;
     static const int HRESLR_MASK      = 0x60;

     static const int HRESBIN_INDEX    = 0;
     static const int HRESBIN_MASK     = 0x1F;
};

inline void MdcCalib::setParam(MdcCalParams& param){
     m_param = param;
}

#endif /* MDCCALIB_H */
