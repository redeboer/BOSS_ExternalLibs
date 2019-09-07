#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include <string>
#include <vector>

namespace Alignment{

    /* exterior product */
    void expd(double veca[3], double vecb[3], double val[3]);

    /* distance of two lines */
    int dist2Line(double sta[3], double stb[3], double veca[3],
            double vecb[3],double &d, double &za, double &zb, int fgZcal = 1);

    /* doca of straight track and wire(line) */
    double docaLineWire(double trkpar[], double wirest[],
            double wirev[], double &zwire, int fgZcal = 1);

    /* doca of helix track and wire(line) */
    double docaHelixWireNewton(double trkpar[], double wirest[],
            double wirev[], double &zwire, double zini);
    double docaHelixWire(double trkpar[], double wirest[],
            double wirev[], double &zwire, double zini);

    bool getDoca(double trkpar[], double wpos[], double &doca,
            double whitPos[], double zini);
    double getPhiIni(double trkpar[], double rLayer, double pos[]);

    int getEpId(int lay, int iEnd); /* get index of the endplate */


    /* global variables */
    extern bool gFlagMag;		/* w/o magnetic field */

    /* number of iteration in doca calculation */
    extern int gNiter;

    const double CC   = 2.99792458E10;       // cm/sec, light velocity
    const double PI   = 3.141592653;
    const double PI2  = 6.283185307;
    const double HFPI = 1.570796327;

    const int WIRENMAX  = 6796;
    const int LAYERNMAX = 43;
    const int CELLNMAX  = 288;
    const int INNERNMAX = 8;
    const int NEP = 16;	/* number of endplates */
    const int NTRKPAR = 5;	/* number of track parameters */
    const int NTRKPARALL = 10;	/* number of track parameters and errors */

    const double BFIELD = 1.0;	/* Tesla */

    /* the following definition is for Millepede */

    const std::string MSG_DEBUG("DEBUG:   ");
    const std::string MSG_INFO("INFO:    ");
    const std::string MSG_WARNING("WARNING: ");
    const std::string MSG_ERROR("ERROR:   ");
    const std::string MSG_FATAL("FATAL:   ");

    const int NDOFALIGN = 3;	/* dx, dy, rz */
    /*      const int NDOFALIGN = 2;	/\* dx, rz *\/ */

    /* the following 4 statements are moved from Millepede.h by wulh on 06/08/29 */
    const bool m_iteration  = true;
    const bool debug_mode   = false;   // More printouts
    const bool verbose_mode = false;  // Definitely more printouts (matrices,...)
    const bool verbose_reject = false; /* added by wulh on 06/08/28 */

    const bool g_dofs[3] = {1, 1, 1}; // Degrees of freedom to take into account (Boolean version)
    /*      const bool g_dofs[] = {1, 1}; // Degrees of freedom to take into account (Boolean version) */

    // Individual constraint on alignment constants (set via ParSig in Millepede)

/*     const double g_Sigm[3] = {0.1, 0.1, 0.25};	/\* for dx, dy, rz *\/ */
    const double g_Sigm[3] = {0.1, 0.01, 0.05};	/* for dx, dy, rz */
    /*      const double g_Sigm[] = {0.1, 0.25}; /\* for dx, rz *\/ */

    // Cuts on residuals (when fitting the tracks) (in mm)
    // !!! Adapt them to your situation !!!

    const double g_res_cut = 1.2;
    const double g_res_cut_init = 3.; // Larger for the first iteration (you are misaligned !!!)
    const double g_start_chi_cut = 100.;

    const int gNsamLC = 100;
    const int gNsamGB = 100;
/*     const double gStepLC[5] = {0.00001, 0.000001, 0.00001, 0.0001, 0.00001};	/\* units of dr&dz are cm *\/ */
    const double gStepLC[5] = {0.001, 0.001, 0.00001, 0.0001, 0.0001};	/* units of dr&dz are cm */
    const double gStepGB[48] = {0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, /* dx */
				0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001,
				0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, /* dy */
				0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001,
				0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, /*rz, mrad*/
				0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001};

/*     const double gStepGB[32] = {0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, /\* dx *\/ */
/* 				0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001, */
/* 				0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, /\* rz *\/ */
/* 				0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001}; */

/*     const int gNsamLC = 50; */
/*     const int gNsamGB = 50; */
/*     const double gStepLC[5] = {0.00002, 0.000002, 0.00002, 0.0002, 0.00002};	/\* units of dr&dz are cm *\/ */
/*     const double gStepGB[48] = {0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, /\* dx *\/ */
/* 				0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, */
/* 				0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, /\* dy *\/ */
/* 				0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, 0.0002, */
/* 				0.000002, 0.000002, 0.000002, 0.000002, 0.000002, 0.000002, 0.000002, 0.000002, /\* rz *\/ */
/* 				0.000002, 0.000002, 0.000002, 0.000002, 0.000002, 0.000002, 0.000002, 0.000002}; */

}

#endif/* ALIGNMENT_H */

