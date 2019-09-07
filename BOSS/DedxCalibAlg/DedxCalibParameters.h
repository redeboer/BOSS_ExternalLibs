#include <vector>

const long int  RUN0    =          0;
const long int  RUN1    =          0;
const long int  RUN2    =        5459;
const long int  RUN3    =        8093;
const long int  RUN4    =        9947;
const long int  RUN5    =       11397;
const long int  RUN6    =       23455;
const long int  RUN7    =       25244;

////runNO<5459, HV:2200
const double HV1_curvep0=500.0*0.1165;
const double HV1_curvep1=9.404;
const double HV1_curvep2=0.009811;
const double HV1_curvep3=1.621;
const double HV1_curvep4=1.19;
const double HV1_index_sin=0;
const double HV1_index_nhit=0.9525;
const double HV1_sigmap0=65;
const double HV1_sigmap1=57.08;
const double HV1_sigmap2=60.03;
const double HV1_sigmap3=-2.747;

////runNO:5960-8093(6201)
const double HV2_curvep0=550.0*0.0182604;
const double HV2_curvep1=35.0428;
const double HV2_curvep2=1.24185e-09;
const double HV2_curvep3= 2.25831;
const double HV2_curvep4=5.81573;
const double HV2_index_sin=0.341267;
const double HV2_index_nhit=0.557516;
const double HV2_sigmap0=33.0;
const double HV2_sigmap1=33.1897;
const double HV2_sigmap2=29.3308;
const double HV2_sigmap3=-2.11828;

const double VR0CUT     =         1.0;
const double VZ0CUT     =         10.0;
const double PT0HighCut =         5.0;
const double PT0LowCut  =         0.05;

#define NormalMean                550    //reference value

#define NumHistBins               300    //set histogram range for gauss and landau distribution
#define MinHistValue              10
#define MaxHistValue              3000
#define MinHistValue1             300
#define MaxHistValue1             950
#define MinChiValue               -1
#define MaxChiValue               1

#define MinADCValuecut            10     //choose hits again in calibration  
#define MaxADCValuecut            35000
#define RPhi_PathMaxCut           3
#define Iner_RPhi_PathMinCut      0.24
#define Out_RPhi_PathMinCut       0.32
#define Iner_DriftDistCut         0.75
#define Out_DriftDistCut          1.0

#define NumSlices                 100
#define NumSlicesDoca             40
#define NumSlicesEAng             40
#define DocaMin                   -1.1
#define DocaMax                   1.1
#define PhiMin                    -0.25
#define PhiMax                    0.25

#define NumBinCostheta            80                       
#define CosthetaMin               -1.00                    
#define CosthetaMax               1.00

static const double doca_norm[43] =
{
    0.585,   0.615,   0.63,   0.61,  0.59,   0.57,  0.565,   0.61,    0.8,  0.855,
    0.8,   0.855,  0.805,  0.845,   0.8,  0.845,  0.785,  0.825,  0.785,  0.815,
    0.765,   0.795,   0.82,  0.855,   0.8,   0.82,   0.86,  0.895,   0.77,  0.805,
    0.83,    0.87,   0.75,   0.78,   0.8,   0.82,    0.8,   0.815,  0.83,   0.87,
    0.77,   0.795,    0.8
};

static const double Eangle_value_cut[15] =
{-0.25,      -0.06,      -0.04,      -0.03,      -0.02,      -0.01,      -0.005,     0,
    0.005,       0.01,       0.02,       0.03,       0.04,       0.06,        0.25
};

static const double Eangle_cut_bin[14] =  {1,  1,  2, 6,  6,  2,  2,   2,   2,  6,  6,  2,    1,   1};

double mylan(double *x, double *par);
double landaun(double *x, double *par);
double Landau(double *x, double *par);
double Vavilov(double *x, double *par);
double AsymGauss(double *x, double *par);

void dedx_pid_exp_old( int landau, int runflag, float dedx,int Nohit, float mom, float theta, float t0, float lsamp,double dedx_exp[5], double ex_sigma[5],double pid_prob[5], double chi_dedx[5]);

void dedx_pid_exp(int vflag[3], float dedx, int trkalg,int Nohit, float mom, float theta, float t0, float lsamp, double dedx_exp[5], double ex_sigma[5],double pid_prob[5], double chi_dedx[5],std::vector<double> & par, std::vector<double> & sig_par);

double SpaceChargeCorrec(double m_theta, double mom, int Particle, double dEdx);
