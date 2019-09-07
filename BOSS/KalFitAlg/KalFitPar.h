#ifndef KALFITPAR_H  //to avoid repeatly defined the head
#define KALFITPAR_H
#include <cstdio>
#include <string>
using namespace std;

class KalFitPar{


public:

static int  muls_;
static int  loss_;
static int wsag_;
static int back_;
static double pT_;
static int lead_;
static int lr_;
static double matrixg_;
static int debug_;
static int debug_kft_;
static int ntuple_;
static int activeonly_;
static string matfile_;
static string cylfile_;
static double deltachi2_cutf_;
static double deltachi2_cuts_;
static double pt_;
static double pe_cut_;
static double  pmu_cut_;
static double  ppi_cut_;
static double   pk_cut_;
static double   pp_cut_;
static int i_back_;
static int tofflag_;
static int tof_hyp_;
static int resolution_;
static double fstrag_;
static int enhance_;
static double fac_h1_,fac_h2_,fac_h3_,fac_h4_,fac_h5_;
static int numf_;
static int numfcor_;
static double Bznom_;
static int steplev_;
static int Tof_correc_;
static int strag_;
static double chi2mdc_hit2_;
static double  factor_strag_;
static int tofall_;
static int nmdc_hit2_;
static int LR_;
static int resolflag_;
static void LR(int x);
static int lead(void);
static void lead(int i);
static int back(void);
static void back(int i);
static int resol(void);
static void resol(int i);
static int muls(void);
static int loss(void);
static void muls(int i);
static void loss(int i);
};

#endif
