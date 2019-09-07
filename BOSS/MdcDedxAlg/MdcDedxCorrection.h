//    BesIII MDC dE/dx Reconstruction Module
//    Class: MdcDedxCorrection
//    Created by Dayong WANG 2003/11

#ifndef MDCDEDXCORRECTION_H
#define MDCDEDXCORRECTION_H

#include <ctime>
#include <iostream>
#include "DedxCurSvc/DedxCurSvc.h"

class MdcDedxCorrection
{
    public:
        MdcDedxCorrection();
        ~MdcDedxCorrection();

        /*  ---  To calculate the expects of dE/dx   --- */
        void dedx_pid_exp_old( int landau, int runflag, float dedx, int Nhit, float mom,
                    float theta, float t0, float lsamp, double dedx_ex[5],
                    double ex_sigma[5], double pid_prob[5], double chi_dedx[5]) const;
        void dedx_pid_exp( int vflag[3], float dedx, int trkalg,int Nhit, float mom,
                    float theta, float t0, float lsamp, double dedx_ex[5],
                    double ex_sigma[5], double pid_prob[5], double chi_dedx[5], 
                    std::vector<double> & DedxCurve_Parameter, 
                    std::vector<double> & DedxSigma_Parameter) const;   
        void getCalib(void) const;

};
#endif
