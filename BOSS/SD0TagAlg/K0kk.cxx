//
//  K0kk.cxx is the single D0 tag code to reconstruct D0 or anti-D0 meson through the final states of
//  K0kk from D0 or anti-D0 meson decays. K0kk.cxx was transfered from the Fortran routine "K0kk.f"
//  which was orignally used for study of the D0D0-bar production and D0 decays at the BES-II
//  experiment during the time period from 2002 to 2008.
//
//  The orignal Fortran routine "Pipipi0.f" used at the BES-II experiment was coded by G. Rong in 2002.
//
//  K0kk.cxx was transfered by G. Rong and J. Liu in December, 2005.
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
#include "SD0TagAlg/K0kk.h"
#include "SD0TagAlg/SingleBase.h"

K0kk::K0kk()
{}

K0kk::~K0kk()
{}


void K0kk::MTotal(double event,SmartDataPtr<EvtRecTrackCol> evtRecTrkCol, Vint iGood,Vint iGam, double Ebeam, int PID_flag, int Charge_candidate_D)
{

  int nGood=iGood.size();
  int nGam=iGam.size();

  iGoodtag.clear();
  iGamtag.clear();
  
  double mass_bcgg,delE_tag_temp;
  int m_chargetag, m_chargepi1,m_chargepi2,m_chargek1,m_chargek2;
  int ik1_temp,ik2_temp,ipi1_temp,ipi2_temp;
  HepLorentzVector pddd;
  HepLorentzVector pddd_temp;

  IDataProviderSvc* eventSvc = NULL;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc,EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");

  int runNo=eventHeader->runNumber();
  int rec=eventHeader->eventNumber();

  double xecm=2*Ebeam;

  k0kkmd=false;
  double  tagmode=0;

  if((evtRecEvent->totalCharged() < 4)){    return;  }

  double ecms = xecm;

  ISimplePIDSvc* simple_pid;
  Gaudi::svcLocator()->service("SimplePIDSvc", simple_pid);

  double deltaE_tem = 0.20;
  int ncount1 = 0; 

  Hep3Vector xorigin(0,0,0);
  HepSymMatrix xoriginEx(3,0);
  IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if(vtxsvc->isVertexValid())
  {
    double* dbv = vtxsvc->PrimaryVertex();
    double*  vv = vtxsvc->SigmaPrimaryVertex();
    xorigin.setX(dbv[0]);
    xorigin.setY(dbv[1]);
    xorigin.setZ(dbv[2]);

    xoriginEx[0][0] = vv[0] * vv[0];
    xoriginEx[1][1] = vv[1] * vv[1];
    xoriginEx[2][2] = vv[2] * vv[2];
  }

  double xv=xorigin.x();
  double yv=xorigin.y();
  double zv=xorigin.z();

  HepPoint3D point0(0.,0.,0.);
  HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]);
  //////////////////////////////////////////////////////////////////

  for(int i = 0; i < evtRecEvent->totalCharged(); i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + i;
    
    int ipi1= (*itTrk)->trackId();

    if(!(*itTrk)->isMdcKalTrackValid()) continue;
    RecMdcKalTrack*  mdcKalTrk1 = (*itTrk)->mdcKalTrack();
    RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);

    m_chargepi1=mdcKalTrk1->charge();
    if(abs(m_chargepi1) != 1) continue;

    /////////////////////////////////////////
    HepVector a1 = mdcKalTrk1->getZHelix();
    HepSymMatrix Ea1 = mdcKalTrk1->getZError();

    VFHelix helixip3_1(point0,a1,Ea1);
    helixip3_1.pivot(IP);
    HepVector  vecipa1 = helixip3_1.a();

    double dr1 = fabs(vecipa1[0]);
    double dz1 = fabs(vecipa1[3]);
    double costheta1 = cos(mdcKalTrk1->theta());

    if (  dr1 >= 15.0) continue;
    if (  dz1 >= 25.0) continue; 
    if ( fabs(costheta1) >= 0.93) continue; 
    /////////////////////////////////////////
    WTrackParameter pip(xmass[2],mdcKalTrk1->getZHelix(),mdcKalTrk1->getZError() );

    //	
    // select pi2
    //	
    for(int j = 0; j < evtRecEvent->totalCharged(); j++) {
      EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + j;
      
      int ipi2= (*itTrk)->trackId();
      if(ipi1==ipi2)  continue;

      if(!(*itTrk)->isMdcKalTrackValid()) continue;
      RecMdcKalTrack*  mdcKalTrk2 = (*itTrk)->mdcKalTrack();
      RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);

      m_chargepi2=mdcKalTrk2->charge();
      if((m_chargepi2 + m_chargepi1) != 0) continue;

      /////////////////////////////////////////
      HepVector a2 = mdcKalTrk2->getZHelix();
      HepSymMatrix Ea2 = mdcKalTrk2->getZError();
      VFHelix helixip3_2(point0,a2,Ea2);
      helixip3_2.pivot(IP);
      HepVector  vecipa2 = helixip3_2.a();

      double dr2 = fabs(vecipa2[0]);
      double dz2 = fabs(vecipa2[3]);
      double costheta2 = cos(mdcKalTrk2->theta());
      if (  dr2 >= 15.0) continue;
      if (  dz2 >= 25.0) continue; 
      if ( fabs(costheta2) >= 0.93) continue; 
      /////////////////////////////////////////
      WTrackParameter pim(xmass[2],mdcKalTrk2->getZHelix(),mdcKalTrk2->getZError() );


      HepPoint3D vx(xorigin.x(), xorigin.y(), xorigin.z());
      HepSymMatrix Evx(3, 0);
      double bx = 1E+6; Evx[0][0] = bx*bx;
      double by = 1E+6; Evx[1][1] = by*by;
      double bz = 1E+6; Evx[2][2] = bz*bz;
      VertexParameter vxpar; vxpar.setVx(vx); vxpar.setEvx(Evx);

      HepVector pip_val = HepVector(7,0);
      HepVector pim_val = HepVector(7,0);
      pip_val = pip.w(); 
      pim_val = pim.w();
      HepLorentzVector ptrktagk0(pip_val[0]+pim_val[0],pip_val[1]+pim_val[1],pip_val[2]+pim_val[2],pip_val[3]+pim_val[3]);
      double m_xmtagk0_tem = ptrktagk0.mag();
      if(fabs(ptrktagk0.m()-0.498)>0.1) continue;

      VertexFit *vtxfit0 = VertexFit::instance();
      vtxfit0->init();
      vtxfit0->AddTrack(0, pip);
      vtxfit0->AddTrack(1, pim);
      vtxfit0->AddVertex(0, vxpar, 0, 1);
      if(!(vtxfit0->Fit(0))) continue;
      vtxfit0->Swim(0); 
      vtxfit0->BuildVirtualParticle(0);
      WTrackParameter wksp = vtxfit0->wtrk(0);
      WTrackParameter wksm = vtxfit0->wtrk(1);
      WTrackParameter wks_Trk = vtxfit0->wVirtualTrack(0);
      VertexParameter wks_var = vtxfit0->vpar(0); 
      
      //	
      //select k1
      //	
      for(int k = 0; k < evtRecEvent->totalCharged(); k++) {
        EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + k;

        int ik1= (*itTrk)->trackId();
        if(ipi2==ik1 || ipi1==ik1)  continue;

        if(!(*itTrk)->isMdcKalTrackValid()) continue;
        RecMdcKalTrack*  mdcKalTrk3 = (*itTrk)->mdcKalTrack();
        RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon);

        m_chargek1=mdcKalTrk3->charge();
        if(abs(m_chargek1) != 1) continue;        

        /////////////////////////////////////////
        HepVector a3 = mdcKalTrk3->getZHelixK();
        HepSymMatrix Ea3 = mdcKalTrk3->getZErrorK();
        VFHelix helixip3_3(point0,a3,Ea3);
        helixip3_3.pivot(IP);
        HepVector  vecipa3 = helixip3_3.a();

        double dr3 = fabs(vecipa3[0]);
        double dz3 = fabs(vecipa3[3]);
        double costheta3 = cos(mdcKalTrk3->theta());
        if (  dr3 >= 1.0) continue;
        if (  dz3 >= 10.0) continue; 
        if ( fabs(costheta3) >= 0.93) continue; 
        /////////////////////////////////////////
        if(PID_flag == 5) {
          simple_pid->preparePID(*itTrk);
          if(simple_pid->probKaon() < 0.0 || simple_pid->probKaon() < simple_pid->probPion()) continue;  
        } 
        /////////////////////////////////////////
        WTrackParameter kap(xmass[3],mdcKalTrk3->getZHelixK(),mdcKalTrk3->getZErrorK() );

        //
        //select k2
        //
        for(int l = 0; l < evtRecEvent->totalCharged(); l++) {
          EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + l;

          int ik2= (*itTrk)->trackId();
          if(ik2==ik1 || ik2==ipi2 || ik2 ==ipi1)  continue;

          if(!(*itTrk)->isMdcKalTrackValid()) continue;
          RecMdcKalTrack*  mdcKalTrk4 = (*itTrk)->mdcKalTrack();
          RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon);

          m_chargek2=mdcKalTrk4->charge();
          if((m_chargek1 + m_chargek2) != 0) continue;          

          /////////////////////////////////////////
          HepVector a4 = mdcKalTrk4->getZHelixK();
          HepSymMatrix Ea4 = mdcKalTrk4->getZErrorK();
          VFHelix helixip3_4(point0,a4,Ea4);
          helixip3_4.pivot(IP);
          HepVector  vecipa4 = helixip3_4.a();

          double dr4 = fabs(vecipa4[0]);
          double dz4 = fabs(vecipa4[3]);
          double costheta4 = cos(mdcKalTrk4->theta());
          if (  dr4 >= 1.0) continue;
          if (  dz4 >= 10.0) continue; 
          if ( fabs(costheta4) >= 0.93) continue; 
          /////////////////////////////////////////
          if(PID_flag == 5) {
            simple_pid->preparePID(*itTrk);
            if(simple_pid->probKaon() < 0.0 || simple_pid->probKaon() < simple_pid->probPion()) continue;  
          } 
          /////////////////////////////////////////

          WTrackParameter kam(xmass[3],mdcKalTrk4->getZHelixK(),mdcKalTrk4->getZErrorK() );

          VertexFit* vtxfit_2 = VertexFit::instance();
          vtxfit_2->init();
          vtxfit_2->AddTrack(0,  kap);
          vtxfit_2->AddTrack(1,  kam);
          vtxfit_2->AddVertex(0, vxpar, 0, 1);
          if(!vtxfit_2->Fit(0))  continue;
          vtxfit_2->Swim(0);

          WTrackParameter wkap = vtxfit_2->wtrk(0);
          WTrackParameter wkam = vtxfit_2->wtrk(1);

          SecondVertexFit *vtxfit = SecondVertexFit::instance();
          vtxfit->init();
          vxpar.setEvx(xoriginEx); 
          vtxfit->setPrimaryVertex(vxpar);
          vtxfit->AddTrack(0, wks_Trk);
          vtxfit->setVpar(wks_var);
          if(!vtxfit->Fit()) continue;

          if(vtxfit->chisq() >999.) continue;
          if(vtxfit->decayLength()<0.0) continue;

          double m_massks1_tem = vtxfit->p4par().m();
          if(m_massks1_tem < 0.485 || m_massks1_tem > 0.515)  continue;
          HepLorentzVector  p4kstag =  vtxfit->p4par();
          WTrackParameter para_ks = vtxfit0->wVirtualTrack(0);

          HepVector kap_val = HepVector(7,0);
          HepVector kam_val = HepVector(7,0);
          HepVector ksp_val = HepVector(7,0);
          HepVector ksm_val = HepVector(7,0);

          kap_val = wkap.w();
          kam_val = wkam.w();
          ksp_val = wksp.w();
          ksm_val = wksm.w();

          HepLorentzVector P_KAP(kap_val[0],kap_val[1],kap_val[2],kap_val[3]);
          HepLorentzVector P_KAM(kam_val[0],kam_val[1],kam_val[2],kam_val[3]);
          HepLorentzVector P_KSP(ksp_val[0],ksp_val[1],ksp_val[2],ksp_val[3]);
          HepLorentzVector P_KSM(ksm_val[0],ksm_val[1],ksm_val[2],ksm_val[3]);
          
          P_KAM.boost(-0.011,0,0);
          P_KAP.boost(-0.011,0,0);
          P_KSP.boost(-0.011,0,0);
          P_KSM.boost(-0.011,0,0);
          p4kstag.boost(-0.011,0,0);
          //pddd = P_KAM + P_KAP + P_KSP + P_KSM;
          pddd = P_KAM + P_KAP + p4kstag;

          double   pk0kk=pddd.rho();

          double temp1 = (ecms/2)*(ecms/2)-pk0kk*pk0kk ;
          if(temp1<0) temp1 =0;
          double mass_bc_tem  = sqrt(temp1);
          if(mass_bc_tem < 1.82 || mass_bc_tem > 1.89) continue;
          
          double 	delE_tag_tag = ecms/2-pddd.e();

          if(fabs(delE_tag_tag)<deltaE_tem)  {
            deltaE_tem = fabs(delE_tag_tag);
            delE_tag_temp = delE_tag_tag;
            mass_bcgg = mass_bc_tem;

            pddd_temp = pddd;

            ipi1_temp=ipi1;
            ipi2_temp=ipi2;
            ik1_temp=ik1;
            ik2_temp=ik2;

            ncount1 = 1;

          }   
        }   
      }        
    }
  }

  if(ncount1 == 1){
    tagmode=22;
    if(m_chargetag<0)  tagmode=-22;
    tagmd=tagmode;
    mass_bc = mass_bcgg;
    delE_tag = delE_tag_temp;
    cqtm     = 0.0;

    iGoodtag.push_back(ipi1_temp);
    iGoodtag.push_back(ipi2_temp);
    iGoodtag.push_back(ik1_temp);
    iGoodtag.push_back(ik2_temp);
   
    iGamtag.push_back(9999);
    iGamtag.push_back(9999);
    iGamtag.push_back(9999);
    iGamtag.push_back(9999);

    ptag = pddd_temp;

    k0kkmd=true;
  }

}



