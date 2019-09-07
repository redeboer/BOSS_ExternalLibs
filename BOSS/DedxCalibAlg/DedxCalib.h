#ifndef DEDXCALIB_H
#define DEDXCALIB_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "DedxCorrecSvc/DedxCorrecSvc.h"

#include "DedxCalibAlg/DedxCalibParameters.h"

class DedxCalib : public Algorithm
{
    public:
        DedxCalib (const std::string& name, ISvcLocator* pSvcLocator);
        ~DedxCalib() {};

        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

        virtual void genNtuple() = 0;
        virtual void initializing() = 0;
        virtual void BookHists() = 0;
        virtual void FillHists() = 0;
        virtual void AnalyseHists() = 0;
        virtual void WriteHists() = 0;

    protected:
        IMdcGeomSvc* geosvc;
        IDedxCorrecSvc* exsvc;

        //do trunction to calculate dE/dx for each track
        float truncate;
        double cal_dedx_bitrunc(float truncate, std::vector<double>  phlist); //do low and high side truncation
        double cal_dedx(float truncate, std::vector<double>  phlist); //no low side truncation

        //calculate chi dE/dx
        void getCurvePar();
        vector<double> Curve_Para;
        vector<double> Sigma_Para;
        int vFlag[3];//vFlag[0]:dedx curve version;  vFlag[1]:dedx sigma version;  vFlag[2]: 0:data; 1:MC
        void set_dEdx( int landau, float dEdx, int trkalg, int runflag, int dedxhit_use, float ptrk, float theta, float pl_rp, int vflag[3] , double t0);
        double m_dedx_exp[5];
        double m_ex_sigma[5];
        double m_pid_prob[5];
        double m_chi_ex[5];

        vector<string> m_recFileVector;
        void ReadRecFileList();
        int ParticleFlag; //particle type flag: 0: electron, 1:muon, 2: pion, 3:kaon, 4:proton
        int m_calibflag;
        int m_phShapeFlag;
        std::string m_eventType;
        std::string m_recFileList;
        std::string m_rootfile;
        std::string m_curvefile;
};
#endif
