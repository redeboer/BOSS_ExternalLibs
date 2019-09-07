#ifndef MDCCALPARAMS_H
#define MDCCALPARAMS_H

#include <string>

const int    MdcCalNLayer  =  43;
const int    MdcCalInnNLay =  8;
const int    MdcCalMaxCell =  288;
const int    MdcCalTotCell =  6796;
const int    MdcCalLR      =  3;

const int    MdcCalNENTRXT =  18;
const int    MdcCalXtOrd   =  6;
const int    MdcCalXtNPars =  8;

const int    MdcCalQtOrd   =  2;
const int    MdcCalNQBin   =  10;

const int    MdcCalNENTRSD =  6;
const int    MdcCalSdNBIN  =  24;

const int    MdcCalWrNBin  =  15;

const double MdcCalTdcCnv  = 0.09375;
const double MdcCalAdcCnv  = 1.0;

const double HFPI = 1.570796327;
const double PI2 = 6.283185307;

class MdcCalParams {
public:
     int fillNtuple;
     int nEvtNtuple;
     int fgCalDetEffi; /* calculating hit efficiency without tracking algorithm impacts */

     double ecm;
     double boostPar[3];

     int particle;
     int nEsFlag;
     int esFlag[50];
     double timeShift;		/* if T<0 after subtracting Tes, use this */
     double tesMin;		/* minimun Tes for calibration */
     double tesMax;		/* maximun Tes for calibration */

     int fgIniCalConst; 	/* effective for IniMdcCalib */
     bool preT0SetTm;		/* flag for updating Tm in PreT0Calib */

     double initT0;		/* initial value of T0 fit */
     double t0Shift;		/* t0 shift based on leading edge fitting */
     double tminFitChindf;	/* chisquare cut for Tmin fit */
     double tmaxFitChindf;	/* chisquare cut for Tmax fit */
     double initSigma;		/* initial value of spatial resolution in the first calibration */

     int calSigma;
     int resiType;		/* 0: including measurement point; 1: excluding */

     int fixXtC0;		/* 1: fix c0 at 0 */
     int fixXtEdge;		/* 1: fix X-T in the edge of the cell */

     int fgAdjacLayerCut;	/* a cut for adjacent layers in MdcCalib.cxx */
     int fgBoundLayerCut;	/* a cut for boundary layers in MdcCalib.cxx */
     int nTrkCut[2];
     int nHitLayCut;
     int nHitCut;
     int hitStatCut;		/* hitStatCut=1 for MdcxReco & MdcPatRec */

     bool   noiseCut;		/* cut of noise level */
     double costheCut[2];	/* cut of cos(theta) */
     double drCut;
     double dzCut;
     double maxDocaInner;
     double maxDocaOuter;

     int fgCalib[MdcCalNLayer];
     double tminFitRange[MdcCalNLayer][2];
     double tmaxFitRange[MdcCalNLayer][2];
     double initTm[MdcCalNLayer];		/* initial value of Tm fit */
     double resiCut[MdcCalNLayer];
     double qmin[MdcCalNLayer];
     double qmax[MdcCalNLayer];

     std::string wpcFile;
};

#endif /* MDCCALPARAMS_H */
