#include "MdcxReco/MdcxParameters.h"

/// constants
const double MdcxParameters::c     = 29.9792458;   //cm/ns

/// Track attribute
const double MdcxParameters::maxTrkOmega  = 0.2;  //FIXME origin 0.08
const double MdcxParameters::maxTrkLength = 250.0;
double MdcxParameters::maxProca     = 0.6;   //OK
double MdcxParameters::minTrkProb   = 0.01;  //FIXME

const double MdcxParameters::maxDd0InMerge   = 2.7;    //OK
const double MdcxParameters::maxDphi0InMerge = 0.15;   //OK
const double MdcxParameters::maxPdradInMerge = 0.22;   //OK
const double MdcxParameters::maxRcsInMerge   = 18.0;   //OK
double MdcxParameters::maxRcsInAddSeg  = 50.0;   //FIXME

const double MdcxParameters::maxAddHitCut = 5.;   //FIXME 
double MdcxParameters::helixFitSigma= 5.;   //FIXME 3, original

double MdcxParameters::nSigAddHitTrk = 5.;
double MdcxParameters::addHitFactor = 10.;//yzhang 2010-05-05 
double MdcxParameters::csmax4 = 50.;   //FIXME 28, original
double MdcxParameters::csmax3 = 1.;   //FIXME 1, original

/// MDC Geometry
const double MdcxParameters::maxMdcRadius = 81.0;
const double MdcxParameters::firstMdcAxialRadius= 18.9;//yzhang 2010-05-05 
const double MdcxParameters::maxMdcZLen   = 170.;//115.4 original

//MdcxFindTracks
const double MdcxParameters::maxDlen = 150.;//FIXME

////N sigma of drop hits in dropHits();
float  MdcxParameters::dropHitsSigma[43] = {
  //yzhang put nSigma of layer 3,4 = 10
  10.,5.,5.,10.,// 0,1,2,3
  10.,5.,5.,10.,// 4,5,6,7
  10.,5.,5.,5.,// 8,9,10,11
  5.,5.,5.,5.,// 12,13,14,15
  5.,5.,5.,10.,// 16,17,18,19
  10.,5.,5.,5.,// 20,21,22,23
  5.,5.,5.,5.,// 24,25,26,27
  5.,5.,5.,5.,// 28,29,30,31
  5.,5.,5.,10.,// 32,33,34,35
  10.,5.,5.,5.,// 36,37,38,39
  5.,5.,10.     // 40,41,42
};

///-- relative to MdcxFindTracks
const int MdcxParameters::findTrkGroup[nSegCombo][3] = {
  { 4, 5, 6 },
  { 2, 1, 0 },
  { 3, 1, 0 },//yzhang add 2011-10-17 
  { 4, 1, 0 },//yzhang add 2011-10-17 
  { 9, 7, 8 },
  { 3, 5, 6 },
  { 10, 7, 8 },
  { 3, 5, 1 },
  { 4, 7, 8 },
  { 9, 5, 6 },
  { 3, 0, 1 },
  { 4, 5, 1 },
  { 2, 5, 1 }// from 2,5,1 to 3,5,1
  /*
  //yzhang bring 9,10 slayer combination forward,
  //for Pt consideration, 2010-09-14 
  { 9, 7, 8 },
  { 10, 7, 8 },
  { 9, 5, 6 },
  { 4, 5, 6 },
  { 2, 1, 0 },
  { 3, 5, 6 },
  { 3, 5, 1 },
  { 4, 7, 8 },
  { 3, 0, 1 },
  { 4, 5, 1 },
  { 2, 5, 1 }// from 2,5,1 to 3,5,1
  */
};

const float MdcxParameters::maxDp12[nSegCombo] = {
  //0:0.19, 1:0.22, 2:0.20, 3:0.19, 4:0.22, 5:0.19, 6:0.22, 7:0.24, 8:0.23, 9:0.18, 10:0.21
  0.5, 0.5,0.5, 0.5,0.5, 0.5,0.5, 0.5,0.5, 0.5, 0.5, 0.5, 0.5
  //0.15, 0.22, 0.20, 0.15, 0.22, 0.15, 0.22, 0.24, 0.23, 0.18, 0.21
};

const float MdcxParameters::maxDp13[nSegCombo] = {
  //0.33, 0.30, 0.25, 0.35, 0.25, 0.24, 0.35, 0.29, 0.30, 0.39, 0.25
  0.5, 0.5,0.5, 0.5,0.5, 0.5,0.5, 0.5,0.5, 0.5, 0.5, 0.5, 0.5
  //0.33, 0.39, 0.25, 0.35, 0.25, 0.24, 0.35, 0.29, 0.30, 0.39, 0.25
};

const int MdcxParameters::layerSet2AddSeg[nSegCombo][11] = {
  { 7, 8, 9, 10, 3, 2, 1, 0, 6, 5, 4 },
  { 3, 4, 5, 6, 7, 8, 9, 10, 2, 1, 0 },
  { 4, 2, 5, 6, 7, 8, 9, 10, 3, 1, 0 },
  { 3, 2, 5, 6, 7, 8, 9, 10, 4, 1, 0 },
  { 10, 6, 5, 4, 3, 2, 1, 0, 9, 8, 7 },
  { 4, 7, 8, 9, 10, 2, 1, 0, 6, 5, 3 },
  { 9, 6, 5, 4, 3, 2, 1, 0, 10, 8, 7 },
  { 4, 2, 6, 7, 8, 9, 10, 0, 5, 3, 1 },
  { 6, 5, 9, 10, 3, 2, 1, 0, 8, 7, 4 },
  { 7, 8, 10, 4, 3, 2, 1, 0, 9, 6, 5 },
  { 2, 4, 5, 6, 7, 8, 9, 10, 3, 1, 0 },
  { 3, 2, 6, 7, 8, 9, 10, 0, 5, 4, 1 },
  { 3, 4, 6, 7, 8, 9, 10, 0, 5, 2, 1 }
};

int MdcxParameters::debug = 0;   
