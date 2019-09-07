//     Class:  MdcDedxParam
//   purpose:  this class contains the parameters used
//    Origin:  Dayong Wang 
//

#ifndef MDCDEDXPARAM_H
#define MDCDEDXPARAM_H

#include <TROOT.h>
#define MinPHValue                10
#define MaxPHValue                4500
#define MinHistValue              10
#define MaxHistValue              35000
#define Iner_MinHistValue         10 
#define Iner_MaxHistValue         35000


#define RPhi_PathMaxCut           3
#define Iner_RPhi_PathMinCut      0.24
#define Out_RPhi_PathMinCut       0.32
#define Iner_DriftDistCut         0.75
#define Out_DriftDistCut          1.0

static const Double_t doca_norm[43] =
{
    0.585,   0.615,   0.63,   0.61,  0.59,   0.57,  0.565,   0.61,    0.8,  0.855,
    0.8,   0.855,  0.805,  0.845,   0.8,  0.845,  0.785,  0.825,  0.785,  0.815,
    0.765,   0.795,   0.82,  0.855,   0.8,   0.82,   0.86,  0.895,   0.77,  0.805,
    0.83,    0.87,   0.75,   0.78,   0.8,   0.82,    0.8,   0.815,  0.83,   0.87,
    0.77,   0.795,    0.8
};

class MdcDedxParam {

    public:
        static long int RUN0;
        static long int RUN1;
        static long int RUN2;
        static long int RUN3;
        static long int RUN4;
        static long int RUN5;


        static double HV1_curvep0;
        static double HV1_curvep1;
        static double HV1_curvep2;
        static double HV1_curvep3;
        static double HV1_curvep4;
        static double HV1_index_sin;
        static double HV1_index_nhit;
        static double HV1_sigmap0;
        static double HV1_sigmap1;
        static double HV1_sigmap2;
        static double HV1_sigmap3;

        static double HV2_curvep0;
        static double HV2_curvep1;
        static double HV2_curvep2;
        static double HV2_curvep3;
        static double HV2_curvep4;
        static double HV2_index_sin;
        static double HV2_index_nhit;
        static double HV2_sigmap0;
        static double HV2_sigmap1;
        static double HV2_sigmap2;
        static double HV2_sigmap3; 

};
#endif

