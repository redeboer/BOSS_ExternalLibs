#ifndef ESTIME_FLAG_
#define ESTIME_FLAG_
#include <vector>
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/NTuple.h"
#include "HepPDT/ParticleDataTable.hh"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include <stdio.h>
#include "CalibData/EsTime/EsTimeCalibData.h"
#include "CalibData/CalibModel.h"
#include "GaudiKernel/DataSvc.h"
#include "CalibDataSvc/ICalibRootSvc.h"
#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
#include "EstTofCaliSvc/IEstTofCaliSvc.h"
#include "CalibData/Tof/TofCalibData.h"
#include "TofQElecSvc/ITofQElecSvc.h"
#include "MdcUtilitySvc/MdcUtilitySvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"

#include "EsTimeAlg/EstParameter.h"
#include <iostream>
#include <fstream>

typedef std::vector<double> Vdouble;

class EsTimeAlg :public Algorithm{

  public:

    EsTimeAlg(const std::string& name, ISvcLocator* pSvcLocator);  

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  public:
    int m_flag;   //1: use segment linear fitting method
    //2: use fasttrk and mdc hit

    int m_nbunch; //3: 3 bunches,0ns,8ns,16ns
    //6: 6 bunches,0,4,8,12,16,20ns
    double m_bunchtime_MC;
    int m_ntupleflag; //0: donot draw ntuple
    //1: add ntuple

    int m_optCosmic;   // optCosmic = 1: employ cosmic correction, 0: don't
    int m_cosmicScheme; // cosmicScheme to decide incoming and outgoing legs
    // 0: 2hits+TDCdiff  1: 2hits+Phi  2: All hits+Phi 

    int m_phase;   // Detector Version, 1 ETOF->96Scin, 2 ETOF->2MRPC+92Scin, 3->72MRPC
    int m_userawtime_opt;  //1: use raw tdc time of TOF;  0: use TOF calibration TDC time   
    double toffset_rawtime;//offset for barrel using rawtime of TOF
    double toffset_rawtime_e;//offset for endcap using rawtime of TOF
    double offset_dt;
    double offset_dt_end;
    int m_debug; 
    int m_beforrec; 
    //  double m_t0shift;   //TOF event start time need abstract m_t0shift when do MDC calibration
    int m_evtNo;       //1: cout event Number; 0: don't
    double m_ebeam;    // beam energy;
    //  void doMDCMatching( int * ){};    // Simple hit matching
    // int   _doMatch;
    //int   _optCosmic, _cosmicScheme;
    //int   _SkipMC,_Skip;
    //double _ptCut, _drCut, _dzCut;
    //double _pathlCut, _ztofCutmin, _ztofCutmax;
    //double _timeOfs;
    //int   _MDC_Skip;
    //double _MDC_ptCut, _MDC_drCut, _MDC_dzCut;
    //int   _MDC_Tof, _MDC_Prop, _MDC_Walk;
    //int   _MDC_Debug;
    //int   _MDC_Xt;
    int   _MDC_Inner;

    // private:
    //  static evtime * _evtprt;
    bool m_useXT;
    bool m_useT0cal;
    bool m_useSw;
    bool m_mdcopt;
    bool m_TofOpt;
    double m_TofOpt_Cut;
    bool m_useTimeFactor;

  private:

    StatusCode storeTDS(double tEst, int tEstFlag, double quality);//yzhang add
    double Opt_new(const double arr[],const  int size,const double sigma_cut);
    double EST_Trimmer(double t0_original,double t0_offset,double raw_offset,double offset_dt,double bunchTime);
    // double Opt_new(	vector<double>::iterator beg,vector<double>::iterator end);
    int  ndriftt;
    int  ntrkMC;
    int  ntrk;
    int  m_pass[5];

    IDataProviderSvc* m_pCalibDataSvc;
    ICalibRootSvc*    m_pRootSvc;
    IEstTofCaliSvc* tofCaliSvc;
    ITofQElecSvc* tofQElecSvc;
    IMdcCalibFunSvc* imdcCalibSvc;
    MdcCalibFunSvc* m_mdcCalibFunSvc;
    MdcUtilitySvc*   m_mdcUtilitySvc;

    HepPDT::ParticleDataTable* m_particleTable;
    IRawDataProviderSvc* m_rawDataProviderSvc;
    NTuple::Tuple*  m_tuple2;
    //NTuple::Tuple*  m_tuple9;

    NTuple::Item<int>  g_eventNo, g_runNo;
    //MCTruth

    NTuple::Item<int>  g_ntrkMC;
    NTuple::Array<double> g_theta0MC, g_phi0MC;
    NTuple::Array<double> g_pxMC, g_pyMC, g_pzMC, g_ptMC;

    //recon
    NTuple::Item <double>  g_nmatchbarrel,g_nmatchend;
    NTuple::Item <double>  g_nmatchbarrel_1,g_nmatchbarrel_2;
    NTuple::Item <int>   g_nmatch_tot;
    NTuple::Item <int>   g_ntrk,g_trigtiming;
    NTuple::Array<double>  g_ttof;
    NTuple::Array<double>  g_vel;
    NTuple::Array<double>  g_abmom;
    NTuple::Array<int>     g_pid;
    NTuple::Array<double>  g_t0for,g_t0back;
    NTuple::Item <double>  g_meant0,g_t0less,g_t0barrelTof;
    NTuple::Item <double>  g_ndriftt,g_nmatchmdc,g_EstimeMdc;
    NTuple::Item <double>  g_t0mean,g_T0,g_t0,g_mcTestime;
    NTuple::Item <double>  g_meantdc, g_Testime_fzisan, g_Testime;
    NTuple::Item <int>    g_ntofup,g_ntofdown,g_ntofup1,g_ntofdown1;
    NTuple::Item <double>  g_difftof_b, g_difftof_e;
    NTuple::Array <double>  g_meantevup,g_meantevdown;
    NTuple::Item<int>  m_estFlag;
    NTuple::Item<double>m_estTime;

    NTuple::Tuple*  m_tuple9;
    NTuple::Array<double>  g_meantev;
    NTuple::Item<int>     g_nmatch;

    NTuple::Tuple*  m_tuple3;
    NTuple::Item <double> g_t0offset_b;
    NTuple::Item <double> g_t0offset_e;
    NTuple::Item <int>    g_bunchtime;
};


#endif
