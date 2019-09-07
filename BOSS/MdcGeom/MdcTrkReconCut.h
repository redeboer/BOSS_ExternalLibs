#ifndef MDCTRKRECONCUT_H
#define MDCTRKRECONCUT_H

//namespace MdcTrkReconCut{

double MdcTrkReconCut_helix_fit[43] ={
  //TrkHelixFitter cut N sigma of abDelChi
  10.,5.,5.,5.,// 0,1,2,3
  5.,5.,5.,10.,// 4,5,6,7
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

double MdcTrkReconCut_combAxPhi0 = 5.0;
double MdcTrkReconCut_combAxCurv = 5.0;
double MdcTrkReconCut_combAxPhi0Cut = 0.5;
double MdcTrkReconCut_combAxCurvCut = 0.1;


//}

#endif
