//
//  Sing.cxx is the code to take care of some variables for anti-D0 tags for each single D0 tag mode.
//  It transfers these variables into a Class or Memmory, from which one can get these variables
//  for doing double tag analysis for study of the D0 meson decays. Sing.cxx was transfered
//  from the Fortran routine "Sing.f" which was orignally used for study of  the D0D0-bar production
//  and D0 meson decays at the BES-II experiment during the time period from 2002 to 2008.
//
//  The orignal Fortran routine "Sing.f" used at the BES-II experiment was coded by G. Rong in 2001.
//
//  Sing.cxx was transfered by G. Rong and J. Liu in December, 2005.
//
//  Since 2008, G. Rong and L.L. Jiang have been working on developing this code to analyze of
//  the data taken at 3.773 GeV with the BES-III detector at the BEPC-II collider.
//
//  During developing this code, many People made significant contributions to this code. These are
//          G. Rong, L.L. Jiang, J. Liu, H.L. Ma, J.C. Chen, D.H. Zhang,
//          M.G. Zhao, B. Zheng, L. Li, Y. Fang, Z.Y. Yi, H.H. Liu, Z.Q. Liu et al.
//
//                                       By G. Rong and L.L. Jiang
//                                       March, 2009
//
//  ==========================================================================================
//
#include "SD0TagAlg/Sing.h"
#include "SD0TagAlg/Kpi.h"
#include "SD0TagAlg/Kpipi0.h"
#include "SD0TagAlg/K3pi.h"
#include "SD0TagAlg/K0pipi.h"
#include "SD0TagAlg/K0pipipi0.h"
#include "SD0TagAlg/K0pi0.h"
#include "SD0TagAlg/Pipipi0.h"
#include "SD0TagAlg/Kk.h"
#include "SD0TagAlg/Pipi.h"
#include "SD0TagAlg/Kkpipi.h"
#include "SD0TagAlg/K0kpi.h"
#include "SD0TagAlg/K0kk.h"
#include "SD0TagAlg/Kkpi0.h"
#include "SD0TagAlg/K3pipi0.h"
#include "SD0TagAlg/Kpipi0pi0.h"
//  --------------------------------------------

#include "SD0TagAlg/SingleBase.h"


Sing::Sing()
{}

Sing::~Sing()
{}

void Sing::Mdset(double event,SmartDataPtr<EvtRecTrackCol> evtRecTrkCol, Vint iGood,Vint iGam, int mdset, double Ebeam, int PID_flag, int Charge_candidate_D)
{
  oktg=false;

  if(mdset==1)  {
    //   ---- mode 11 ----- D0-->KPi ---------
    Kpi  kpi;
    kpi.MTotal(event,evtRecTrkCol, iGood,iGam,Ebeam,  PID_flag,Charge_candidate_D);
    bool kpimd=kpi.Getkpimd();

    if(kpimd) {
      tagmd_temp=kpi.Gettagmd();
      mass_bc_temp=kpi.Getmass_bc();
      cqtm_temp = kpi.GetCQtm();
      delE_tag_temp=kpi.GetdelE_tag();
      iGoodtag_temp=kpi.Gettagtrk1();
      iGamtag_temp=kpi.GettagGam1();
      ptag_temp=kpi.Gettagp1();

      oktg=true;

    }
  }

  if(mdset==2)  {
    //   ---- mode 12 ----- D0 --> KPiPi0---------
    Kpipi0  kpipi0;
    kpipi0.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  kpipi0md=kpipi0.Getkpipi0md();
    if(kpipi0md) {
      tagmd_temp=kpipi0.Gettagmd();
      iGoodtag_temp=kpipi0.Gettagtrk1();
      iGamtag_temp=kpipi0.GettagGam1();
      ptag_temp=kpipi0.Gettagp1();
      delE_tag_temp=kpipi0.GetdelE_tag();
      mass_bc_temp=kpipi0.Getmass_bc();
      cqtm_temp = kpipi0.GetCQtm();

      oktg=true;
    }
  }

  if(mdset==4)  {
    //   ---- mode 13 ----- D0-->K3Pi ---------
    K3pi  k3pi;
    k3pi.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  k3pimd=k3pi.Getk3pimd();
    if(k3pimd) {
      tagmd_temp=k3pi.Gettagmd();
      iGoodtag_temp=k3pi.Gettagtrk1();
      iGamtag_temp=k3pi.GettagGam1();
      ptag_temp=k3pi.Gettagp1();
      delE_tag_temp=k3pi.GetdelE_tag();
      mass_bc_temp=k3pi.Getmass_bc();
      cqtm_temp     = k3pi.GetCQtm();
      
      oktg=true;
    }
  }

  if(mdset==8)  {
    //   ---- mode 14-----D0-->K0PiPi ---------
    K0pipi  k0pipi;
    k0pipi.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  k0pipimd=k0pipi.Getk0pipimd();
    if(k0pipimd) {
      tagmd_temp=k0pipi.Gettagmd();
      iGoodtag_temp=k0pipi.Gettagtrk1();
      iGamtag_temp=k0pipi.GettagGam1();
      ptag_temp=k0pipi.Gettagp1();
      delE_tag_temp=k0pipi.GetdelE_tag();
      mass_bc_temp=k0pipi.Getmass_bc();
      cqtm_temp     = k0pipi.GetCQtm();

      oktg=true;
    }	
  }	

  if(mdset==16)  {
    //   ---- mode 15 ----- D0-->KK ---------
    Kk  kk;
    kk.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  kkmd=kk.Getkkmd();
    if(kkmd) {
      tagmd_temp=kk.Gettagmd();
      iGoodtag_temp=kk.Gettagtrk1();
      iGamtag_temp=kk.GettagGam1();
      ptag_temp=kk.Gettagp1();
      delE_tag_temp=kk.GetdelE_tag();
      mass_bc_temp=kk.Getmass_bc();
      cqtm_temp = kk.GetCQtm();

      oktg=true;
    }
  }

  if(mdset==32)  {
    //   ---- mode 16 ----- D0-->PiPiPi0 ---------
    Pipipi0  pipipi0;
    pipipi0.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  pipipi0md=pipipi0.GetPipipi0md();
    if(pipipi0md) {
      tagmd_temp=pipipi0.Gettagmd();
      iGoodtag_temp=pipipi0.Gettagtrk1();
      iGamtag_temp=pipipi0.GettagGam1();
      ptag_temp=pipipi0.Gettagp1();
      delE_tag_temp=pipipi0.GetdelE_tag();
      mass_bc_temp=pipipi0.Getmass_bc();
      cqtm_temp = pipipi0.GetCQtm();

      oktg=true;
    }
  }



  if(mdset==64)  {
    //   ---- mode 17 ----- D0-->K0PiPiPi0 ---------
    K0pipipi0  k0pipipi0;
    k0pipipi0.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  k0pipipi0md=k0pipipi0.GetK0pipipi0md();
    if(k0pipipi0md) {
      tagmd_temp=k0pipipi0.Gettagmd();
      iGoodtag_temp=k0pipipi0.Gettagtrk1();
      iGamtag_temp=k0pipipi0.GettagGam1();
      ptag_temp=k0pipipi0.Gettagp1();
      delE_tag_temp=k0pipipi0.GetdelE_tag();
      mass_bc_temp=k0pipipi0.Getmass_bc();
      cqtm_temp     = k0pipipi0.GetCQtm();

      oktg=true;
    }
  }



  if(mdset==128)  {
    //   ---- mode 18 ----- D0-->K0Pi0  ---------
    K0pi0  k0pi0;
    k0pi0.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  k0pi0md=k0pi0.GetK0pi0md();
    if(k0pi0md) {
      tagmd_temp=k0pi0.Gettagmd();
      iGoodtag_temp=k0pi0.Gettagtrk1();
      iGamtag_temp=k0pi0.GettagGam1();
      ptag_temp=k0pi0.Gettagp1();
      delE_tag_temp=k0pi0.GetdelE_tag();
      mass_bc_temp=k0pi0.Getmass_bc();
      cqtm_temp     = k0pi0.GetCQtm();
      
      oktg=true;
    }
  }
  
  
  if(mdset==256)  
  {
    //   ---- mode 19 ----- D0-->Pi+Pi- mode -----
    Pipi  pipi;
    pipi.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  pipimd = pipi.GetPipimd();
    if(pipimd) {
      tagmd_temp    = pipi.Gettagmd();
      iGoodtag_temp = pipi.Gettagtrk1();
      iGamtag_temp=pipi.GettagGam1();
      ptag_temp     = pipi.Gettagp1();
      delE_tag_temp  = pipi.GetdelE_tag();
      mass_bc_temp = pipi.Getmass_bc();
      cqtm_temp     = pipi.GetCQtm();
      
      oktg=true;
    }
  }

  if(mdset==512)
  {
    //   ---- mode 20 ----- D0-->K+K-Pi+Pi- mode -----
    Kkpipi  kkpipi;
    kkpipi.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  kkpipimd = kkpipi.Getkkpipimd();
    if(kkpipimd) {
      tagmd_temp    = kkpipi.Gettagmd();
      iGoodtag_temp = kkpipi.Gettagtrk1();
      iGamtag_temp=kkpipi.GettagGam1();
      ptag_temp     = kkpipi.Gettagp1();
      delE_tag_temp  = kkpipi.GetdelE_tag();
      mass_bc_temp = kkpipi.Getmass_bc();
      cqtm_temp     = kkpipi.GetCQtm();
      
      oktg=true;
    }
  }

  if(mdset==1024)
  {
    //   ---- mode 21 ----- D0-->K0K+Pi- mode -----
    K0kpi  k0kpi;
    k0kpi.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  k0kpimd = k0kpi.Getk0kpimd();
    if(k0kpimd) {
      tagmd_temp    = k0kpi.Gettagmd();
      iGoodtag_temp = k0kpi.Gettagtrk1();
      iGamtag_temp=k0kpi.GettagGam1();
      ptag_temp     = k0kpi.Gettagp1();
      delE_tag_temp  = k0kpi.GetdelE_tag();
      mass_bc_temp = k0kpi.Getmass_bc();
      cqtm_temp     = k0kpi.GetCQtm();

      oktg=true;
    }
  }

  if(mdset==2048)
  {
    //   ---- mode 22 ----- D0-->K0K+K- mode -----
    K0kk  k0kk;
    k0kk.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  k0kkmd = k0kk.Getk0kkmd();
    if(k0kkmd) {
      tagmd_temp    = k0kk.Gettagmd();
      iGoodtag_temp = k0kk.Gettagtrk1();
      iGamtag_temp=k0kk.GettagGam1();
      ptag_temp     = k0kk.Gettagp1();
      delE_tag_temp  = k0kk.GetdelE_tag();
      mass_bc_temp = k0kk.Getmass_bc();
      cqtm_temp     = k0kk.GetCQtm();

      oktg=true;
    }
  }

  if(mdset==4096)
  {
    //   ---- mode 23 ----- D0-->K-K+Pi0 mode  -----
    Kkpi0  kkpi0;
    kkpi0.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  kkpi0md = kkpi0.Getkkpi0md();
    if(kkpi0md) {
      tagmd_temp    = kkpi0.Gettagmd();
      iGoodtag_temp = kkpi0.Gettagtrk1();
      iGamtag_temp=kkpi0.GettagGam1();
      ptag_temp     = kkpi0.Gettagp1();
      delE_tag_temp  = kkpi0.GetdelE_tag();
      mass_bc_temp = kkpi0.Getmass_bc();
      cqtm_temp     = kkpi0.GetCQtm();

      oktg=true;
    }
  }

  if(mdset==8192)
  {
    //  ---- mode 24 ----- D0-->K-PiPiPiPi0 mode -----
    K3pipi0  k3pipi0;
    k3pipi0.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  k3pipi0md = k3pipi0.Getk3pipi0md();
    if(k3pipi0md) {
      tagmd_temp    = k3pipi0.Gettagmd();
      iGoodtag_temp = k3pipi0.Gettagtrk1();
      iGamtag_temp  = k3pipi0.GettagGam1();
      ptag_temp     = k3pipi0.Gettagp1();
      delE_tag_temp = k3pipi0.GetdelE_tag();
      mass_bc_temp  = k3pipi0.Getmass_bc();
      cqtm_temp     = k3pipi0.GetCQtm();
      oktg=true;
    }
  }

  if(mdset==16384)
  {
    //   ----mode 25 ----- D0-->K-PiPi0Pi0 mode -----
    Kpipi0pi0  kpipi0pi0;
    kpipi0pi0.MTotal(event,evtRecTrkCol, iGood,iGam, Ebeam,  PID_flag,Charge_candidate_D);
    bool  kpipi0pi0md = kpipi0pi0.Getkpipi0pi0md();
    if(kpipi0pi0md) {
      tagmd_temp    = kpipi0pi0.Gettagmd();
      iGoodtag_temp = kpipi0pi0.Gettagtrk1();
      iGamtag_temp  = kpipi0pi0.GettagGam1();
      ptag_temp     = kpipi0pi0.Gettagp1();
      delE_tag_temp = kpipi0pi0.GetdelE_tag();
      mass_bc_temp  = kpipi0pi0.Getmass_bc();
      cqtm_temp     = kpipi0pi0.GetCQtm();
      oktg=true;
    }
  }

  return;

}
