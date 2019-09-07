//--------------------------------------------------------------------------
// File and Version Information:
// $Id: DedxCorrParameters.h,v 1.6 2011/08/17 13:38:37 anff Exp $
// Description:
//  Class DedxCalibParameters contains results of dE/dx calibration
//
//------------------------------------------------------------------------
#ifndef DEDXCORRPARAMETERS_H
#define DEDXCORRPARAMETERS_H

#include <TROOT.h>
#define NormalMean                550
#define Truncate                  0.7
 
#define NumLayers                 43
#define NumWires                  6796
#define NumSlicesDoca             40
#define NumSlicesEAng             40
#define NumSlicesDipAng           40
#define NumSlicesP                60
 
#define NumHistBins               300
#define MinHistValue              10
#define MaxHistValue              2500
#define Iner_MinHistValue         10
#define Iner_MaxHistValue         3500
 
#define RPhi_PathMaxCut           3
#define Iner_RPhi_PathMinCut      0.24
#define Out_RPhi_PathMinCut       0.32
#define Iner_DriftDistCut         0.7
#define Out_DriftDistCut          0.9
#define MinADCValuecut            10
#define MaxADCValuecut            6500
#define MinPHcut                  10
#define MaxPHcut                  6500
 
#define mass_e                    0.511e-3
#define mass_mu                   105.658e-3  
 
#define NumSlicesBGSat            4
//#define BetaGammaLogMaxSat        3.59
//#define BetaGammaLogMinSat        3.55
#define MomentMaxSat              1.95
#define MomentMinSat              1.8
#define OldAnalyse                1
#define UseOldFunctions           1 
 
#define Phi_Min                  -0.25
#define Phi_Max                   0.25
#define Iner_DocaMin             -1.1
#define Iner_DocaMax              1.1
#define Out_DocaMin              -1.1
#define Out_DocaMax               1.1
#define Dcasin_l_iner             1
#define Dcasin_dd_iner            25
#define Dcasin_sin_iner           22
#define Dcasin_l_out              1
#define Dcasin_dd_out             25
#define Dcasin_sin_out            22
#define CellEntry_cut             10 
 
#define NumBinDrifDist            20
#define Iner_DrifDistMin          0
#define Iner_DrifDistMax          0.6
#define Out_DrifDistMin           0
#define Out_DrifDistMax           0.84
#define DrifDistStandard          784 
 
#define NumBinSinEAngle           10
#define SinEAngleMin              0.02
#define SinEAngleMax              0.06
#define SinEAngleStandard         784
 
#define NumBinZhit                20
#define ZhitMin                   0
#define ZhitStandard              784 
 
#define NumBinCostheta            20
#define CosthetaMin               0
#define CosthetaMax               0.83
#define CosthetaStandard          784 
 
#define NumBinSpaceCharge         40
#define SpacethetaMin            -0.83
#define SpacethetaMax             0.83
#define SpacethetaStandard        784


static const Int_t layer_typ[43] =
  {
    0, 1, 1, 2, 0, 1, 1, 2,
    0, 1, 1, 2, 0, 1, 1, 2,
    0, 1, 1, 2, 0, 1, 1, 2,
    0, 1, 1, 2, 0, 1, 1, 2,
    0, 1, 1, 2, 0, 1, 1, 2,
    0, 1, 2 
  };

static const Double_t doca_norm[43] =
  {
     0.585,   0.615,   0.63,   0.61,  0.59,   0.57,  0.565,   0.61,    0.8,  0.855,
       0.8,   0.855,  0.805,  0.845,   0.8,  0.845,  0.785,  0.825,  0.785,  0.815,
     0.765,   0.795,   0.82,  0.855,   0.8,   0.82,   0.86,  0.895,   0.77,  0.805,
      0.83,    0.87,   0.75,   0.78,   0.8,   0.82,    0.8,   0.815,  0.83,   0.87,
      0.77,   0.795,    0.8
  };

static const Double_t Doca_normal[43] =
  {0.704,   0.740,   0.775,   0.747,   0.718,    0.690,    0.669,    0.733,
   0.994,   1.079,   0.994,   1.079,   1.015,    1.065,    1.001,    1.065,
   0.980,   1.044,   0.980,   1.030,   0.959,    1.001,    1.037,    1.079,
   1.008,   1.037,   1.086,   1.129,   0.966,    1.015,    1.051,    1.086,
   0.938,   0.980,   1.008,   1.037,   1.008,    1.030,    1.051,    1.093,
   0.967,   1.001,   1.008
  };

static const Double_t Zhit_Max[43] =
  { 20.0,     20.2,     20.4,     20.6,     20.8,      21.0,      21.2,      22.0,
    26.5,       30,       31,       34,       37,        39,        42,        44,
      45,       49,       52,       55,       58,        60,        63,        65,
      67,       70,       72,       75,       77,        80,        83,        85,
      87,       89,       91,       94,       98,       100,       102,       105,
     109,      110,      113
  };

/*static const Double_t Eangle_bin[41] = 
  { -1.0,   -0.8,   -0.7,  -0.625,   -0.55,  -0.475,    -0.4,   -0.35,    -0.3,   -0.25,   
    -0.2,  -0.18,  -0.16,   -0.14,   -0.12,   -0.10,   -0.08,   -0.06,   -0.04,   -0.02,    0,
    0.02,   0.04,   0.06,    0.08,    0.10,    0.12,    0.14,    0.16,    0.18,     0.2, 
    0.25,    0.3,   0.35,     0.4,   0.475,    0.55,   0.625,     0.7,     0.8,     1.0 
  };*/


static const Double_t Eangle_bin_value[41] = 
  { -0.25,     -0.12,     -0.085,     -0.05,     -0.0425,     -0.035,     -0.0325,     -0.03,     -0.0275,   -0.025,
  -0.0225,     -0.02,  -0.018345, -0.016675,    -0.01501,   -0.01334,    -0.01167,     -0.01,     -0.0075,   -0.005,    0,
    0.005,    0.0075,       0.01,   0.01167,     0.01334,    0.01501,     0.16675,  0.018345,        0.02,   0.0225,
    0.025,    0.0275,       0.03,    0.0325,       0.035,     0.0425,        0.05,     0.085,        0.12,     0.25
  };
 
/*static const Double_t Eangle_bin[17] = 
  {-0.25,      -0.2,      -0.1,      -0.05,      -0.03,      -0.02,     -0.01,      -0.005,     0,
   0.005,      0.01,      0.02,       0.03,       0.05,        0.1,       0.2,        0.25
  };*/
 
static const Double_t Eangle_value_cut[15] =
  {-0.25,      -0.06,      -0.04,      -0.03,      -0.02,      -0.01,      -0.005,     0,
   0.005,       0.01,       0.02,       0.03,       0.04,       0.06,        0.25
  };
 
static const Double_t Eangle_cut_bin[14] =
  {1,     1,    2,   6,   6,    2,    2,   2,    2,    6,     6,     2,    1,   1};

static const Int_t dead_wire[4][25] = 
{{1248 ,1519,1534, 6257,   6444,    6445,    6446,    6447,    6448,    6449,    6450,   6451, -1, -1, -1, -1, -1, -1,-1, -1, -1, -1, -1, -1, -1},{1248, 1519,1534, 6257, 1742, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},{1248, 1519,1534, 6257, 1742, 6668 , 6669 , 6670 , 6671 , 6672 , 6673 , 6674 , 6675, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},{1204 , 1205 , 1206 , 1207 , 1208 , 1209 , 1210 , 1211 , 1248, 1519,1534, 6257, 1742, 6668 , 6669 , 6670 , 6671 , 6672 , 6673 , 6674 , 6675, -1, -1, -1, -1}};

/*static const Int_t bad_wire[44] =
  {1248, 1519, 1534, 1772, 1773, 1774, 1775, 1776, 1777, 1778,
   1779, 6132, 6133, 6134, 6135, 6136, 6137, 6138, 6139, 6140, 
   6141, 6142, 6143, 6144, 6145, 6146, 6147, 6257, 6444, 6445, 
   6446, 6447, 6448, 6449, 6450, 6451, 6508, 6509, 6510, 6511, 
   6512, 6513, 6514, 6515 
      };*/

#endif
