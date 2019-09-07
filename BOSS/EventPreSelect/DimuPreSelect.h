#ifndef DimuPreSelect_H
#define DimuPreSelect_H 

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/PropertyMgr.h"

class DimuPreSelect {

  public:
    DimuPreSelect();
    ~DimuPreSelect() {;}
    void BookNtuple(NTuple::Tuple*& tuple);
    int IsDimu(); //1, barrel; 2, endcap; 0, not dimu.
    void Print();

  private:
    //  event selection
    PropertyMgr   m_propMgr;

    bool m_output;
    bool m_mdcPass;
    bool m_tofPass;
    bool m_emcPass;
    bool m_mucPass; 

    long m_totevent;   
    long m_currun;
    long m_curevent;
    double m_ecms;
    double m_vr0cut;
    double m_vz0cut;
    double m_pcut_up;
    double m_pcut_down;
    double m_psymcut;
    double m_tcut;
    double m_ecut_up;
    double m_ecut_down;
    double m_dthetacut;
    double m_dphicut;
    double m_partselect;  //0: all, 1 : barrel only, 2 : endcap only
    double m_mudigicut;
    double m_mutrkcut;

    double m_cutpass[20];
    double m_subpass[4];
    double m_totalpass;

  private:
    NTuple::Tuple*  m_passtuple;   
    NTuple::Item<double>  m_run;
    NTuple::Item<double>  m_event;
    NTuple::Item<double>  m_part; // 1: barrel, 0: endcap
    NTuple::Item<double>  m_c1;
    NTuple::Item<double>  m_c2;
    NTuple::Item<double>  m_r1;
    NTuple::Item<double>  m_r2;
    NTuple::Item<double>  m_z1;
    NTuple::Item<double>  m_z2;
    NTuple::Item<double>  m_p1;
    NTuple::Item<double>  m_p2;
    NTuple::Item<double>  m_t1;
    NTuple::Item<double>  m_t2;
    NTuple::Item<double>  m_e1;
    NTuple::Item<double>  m_e2;
    NTuple::Item<double>  m_theta1;
    NTuple::Item<double>  m_theta2;
    NTuple::Item<double>  m_phi1;
    NTuple::Item<double>  m_phi2;
    NTuple::Item<double>  m_mudigi;
    NTuple::Item<double>  m_mutrk;
    NTuple::Item<double>  m_zeroCFlag;
    NTuple::Item<double>  m_vtRZFlag;
    NTuple::Item<double>  m_pLimFlag;
    NTuple::Item<double>  m_pSymFlag;
    NTuple::Item<double>  m_tLimFlag;
    NTuple::Item<double>  m_eLimFlag;
    NTuple::Item<double>  m_eBBFlag;
    NTuple::Item<double>  m_partFlag;
    NTuple::Item<double>  m_mudigiFlag;
    NTuple::Item<double>  m_mutrkFlag;
    NTuple::Item<double>  m_mdcFlag;
    NTuple::Item<double>  m_tofFlag;
    NTuple::Item<double>  m_emcFlag;
    NTuple::Item<double>  m_mucFlag;

};

#endif
