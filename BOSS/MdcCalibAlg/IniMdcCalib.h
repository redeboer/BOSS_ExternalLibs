#ifndef INIMDCCALIB_H
#define INIMDCCALIB_H

#include "MdcCalibAlg/MdcCalib.h"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeomSvc.h"

#include "TH1F.h"
#include "TFolder.h"

class IniMdcCalib: public MdcCalib {
public:
     IniMdcCalib();              
     ~IniMdcCalib();   
     void initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
                     IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc);

     void setParam(MdcCalParams& param);
     int  fillHist(MdcCalEvent* event);
     int  updateConst(MdcCalibConst* calconst);
     void clear();

private:
     static Double_t funTmin(Double_t* x, Double_t* par);
     static Double_t funTmax(Double_t* x, Double_t* par);

     MdcCalParams m_param;
 
     IMdcGeomSvc* m_mdcGeomSvc;
     IMdcCalibFunSvc* m_mdcFunSvc;
     IMdcUtilitySvc*   m_mdcUtilitySvc;
     TObjArray* m_hlist;
     
     int m_nWire;
     int m_nLayer;

     TFolder* m_fdcom;

     TFolder* m_fdTmap;
     TFolder* m_fdTraw;
     TFolder* m_fdTrawCel;
     TFolder* m_fdTrawTes;

     TFolder* m_fdQmap;
     TFolder* m_fdQraw;
     TFolder* m_fdQrawCel;

     TH1F* m_hLayerHitmapT;
     TH1F* m_hWireHitMapT;

     TH1F* m_hLayerHitmapQ;
     TH1F* m_hWireHitMapQ;

     TH1F* m_hTesAllFlag;
     TH1F* m_hTes[10];
     TH1F* m_hTesAll;
     TH1F* m_hTesCal;
     TH1F* m_hTesFlag;

     TH1F* m_htdcTes[MdcCalNLayer][10];
     TH1F* m_htrawTes[MdcCalNLayer][10];

     TH1F* m_hlaymapT[MdcCalNLayer];
     TH1F* m_htdc[MdcCalNLayer];
     TH1F* m_htraw[MdcCalNLayer];
     TH1F* m_htrawCel[MdcCalTotCell];

     TH1F* m_hlaymapQ[MdcCalNLayer];
     TH1F* m_hqraw[MdcCalNLayer];
     TH1F* m_hqrawCel[MdcCalTotCell];
};

inline void IniMdcCalib::setParam(MdcCalParams& param){
     MdcCalib::setParam(param);
     m_param = param;
}
#endif  /* IniMDCCALIB_H */
