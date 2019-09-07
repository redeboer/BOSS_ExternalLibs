#ifndef MDCALIPARAMS_H
#define MDCALIPARAMS_H

#include <string>

class MdcAliParams {
public:
     int particle;
     int nEsFlag;
     int esFlag[50];
     double tesMin;		/* minimun Tes for calibration */
     double tesMax;		/* maximun Tes for calibration */

     double tmaxFitChindf;	/* chisquare cut for Tmax fit */

     int resiType;		/* 0: including measurement point; 1: excluding */


     int fgAdjacLayerCut;	/* a cut for adjacent layers in MdcCalib.cxx */
     int fgBoundLayerCut;	/* a cut for boundary layers in MdcCalib.cxx */
     int nTrkCut[2];
     int nHitLayCut;
     int nHitCut;
     int hitStatCut;		/* hitStatCut=1 for MdcxReco & MdcPatRec */

     double ptCut[2];	        /* cut of Pt */
     double costheCut[2];	/* cut of cos(theta) */
     double drCut;
     double dzCut;
     double maxDocaInner;
     double maxDocaOuter;
     double maxResi;


};


#endif /* MDCCALPARAMS_H */
