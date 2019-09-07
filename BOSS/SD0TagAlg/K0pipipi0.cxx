//
//  K0pipipi0.cxx is the single D0 tag code to reconstruct D0 or anti-D0 meson through the final states of
//  K0pipipi0 from D0 decays. K0pipipi0.cxx was transfered from the Fortran routine "K0pipipi0.f"
//  which was orignally used for study of the D0D0-bar production and D0 decays at the BES-II
//  experiment during the time period from 2002 to 2008.
//
//  The orignal Fortran routine "K0pipipi0.f" used at the BES-II experiment was coded by G. Rong in 2002.
//
//  K0pipipi0.cxx was transfered by G. Rong and J. Liu in December, 2005.
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
#include "SD0TagAlg/K0pipipi0.h"
#include "SD0TagAlg/SingleBase.h"


K0pipipi0::K0pipipi0()
{}

K0pipipi0::~K0pipipi0()
{}


void K0pipipi0::MTotal(double event,SmartDataPtr<EvtRecTrackCol> evtRecTrkCol, Vint iGood,Vint
    iGam,double Ebeam, int PID_flag, int Charge_candidate_D)
{

  int nGood=iGood.size();
  int nGam=iGam.size();

  iGoodtag.clear();
  iGamtag.clear();
  
  double mass_bcgg, delE_tag_temp;
  int m_chargetag,m_chargepi1,m_chargepi2,m_chargepi3,m_chargepi4;
  int ik1_temp,ipi1_temp,ipi2_temp,ipi3_temp,ipi4_temp, iGam1_temp, iGam2_temp;
  HepLorentzVector pddd;
  HepLorentzVector pddd_temp;

  IDataProviderSvc* eventSvc = NULL;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc,EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");

  int runNo=eventHeader->runNumber();
  int rec=eventHeader->eventNumber();

  double xecm=2*Ebeam;

  k0pipipi0md=false;
  double  tagmode=0;

  if((evtRecEvent->totalCharged() < 4||nGam<2)){    return;  }

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
  
  HepLorentzVector p2gfit;
  HepLorentzVector p2gg;

  for(int i = 0; i < evtRecEvent->totalCharged(); i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + i;

    int ipi1= (*itTrk)->trackId();

    if(!(*itTrk)->isMdcKalTrackValid()) continue;
    RecMdcKalTrack*  mdcKalTrk1 = (*itTrk)->mdcKalTrack();
    RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);

    m_chargepi1=mdcKalTrk1->charge();
    if(m_chargepi1 != 1) continue;

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
    WTrackParameter pip1(xmass[2],mdcKalTrk1->getZHelix(),mdcKalTrk1->getZError() );

    //	
    // select pi2
    //	

    for(int j = 0; j < evtRecEvent->totalCharged();j++) {
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
      WTrackParameter pim1(xmass[2],mdcKalTrk2->getZHelix(),mdcKalTrk2->getZError() );


      HepVector pip1_val = HepVector(7,0);
      HepVector pim1_val = HepVector(7,0);
      pip1_val = pip1.w(); 
      pim1_val = pim1.w();
      HepLorentzVector ptrktagk0(pip1_val[0]+pim1_val[0],pip1_val[1]+pim1_val[1],pip1_val[2]+pim1_val[2],pip1_val[3]+pim1_val[3]);
      double m_xmtagk0_tem = ptrktagk0.mag();
      if(fabs(ptrktagk0.m()-0.498)>0.1) continue;

      HepPoint3D vx(xorigin.x(), xorigin.y(), xorigin.z());
      HepSymMatrix Evx(3, 0);
      double bx = 1E+6; Evx[0][0] = bx*bx;
      double by = 1E+6; Evx[1][1] = by*by;
      double bz = 1E+6; Evx[2][2] = bz*bz;
      VertexParameter vxpar; vxpar.setVx(vx); vxpar.setEvx(Evx);


      VertexFit *vtxfit0 = VertexFit::instance();
      vtxfit0->init();
      vtxfit0->AddTrack(0, pip1);
      vtxfit0->AddTrack(1, pim1);
      vtxfit0->AddVertex(0, vxpar, 0, 1);
      if(!(vtxfit0->Fit(0))) continue;
      vtxfit0->Swim(0); 
      vtxfit0->BuildVirtualParticle(0);
      WTrackParameter wksp = vtxfit0->wtrk(0);
      WTrackParameter wksm = vtxfit0->wtrk(1);
      WTrackParameter wks_Trk = vtxfit0->wVirtualTrack(0);
      VertexParameter wks_var = vtxfit0->vpar(0);     

      //	
      //select pi3
      //	
      for(int k = 0; k < evtRecEvent->totalCharged(); k++) {
        EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + k;

        int ipi3= (*itTrk)->trackId();
        if(ipi2==ipi3 || ipi1==ipi3)  continue;

        if(!(*itTrk)->isMdcKalTrackValid()) continue;
        RecMdcKalTrack*  mdcKalTrk3 = (*itTrk)->mdcKalTrack();
        RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);

        m_chargepi3=mdcKalTrk3->charge();
        if(abs(m_chargepi3) != 1) continue;

        /////////////////////////////////////////
        HepVector a3 = mdcKalTrk3->getZHelix();
        HepSymMatrix Ea3 = mdcKalTrk3->getZError();
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
          if(simple_pid->probPion() < 0.0 || simple_pid->probPion() < simple_pid->probKaon()) continue;  
        } 
        /////////////////////////////////////////
        WTrackParameter pip2(xmass[2],mdcKalTrk3->getZHelix(),mdcKalTrk3->getZError() );

        //
        //select pi4
        //
        for(int l = 0; l < evtRecEvent->totalCharged(); l++) {
          EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + l;

          int ipi4= (*itTrk)->trackId();
          if(ipi4==ipi3 || ipi4==ipi2 || ipi4 ==ipi1)  continue;

          if(!(*itTrk)->isMdcKalTrackValid()) continue;
          RecMdcKalTrack*  mdcKalTrk4 = (*itTrk)->mdcKalTrack();
          RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);

          m_chargepi4=mdcKalTrk4->charge();
          if((m_chargepi4 + m_chargepi3) != 0) continue;

          /////////////////////////////////////////
          HepVector a4 = mdcKalTrk4->getZHelix();
          HepSymMatrix Ea4 = mdcKalTrk4->getZError();
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
            if(simple_pid->probPion() < 0.0 || simple_pid->probPion() < simple_pid->probKaon()) continue;  
          } 
          /////////////////////////////////////////
          WTrackParameter pim2(xmass[2],mdcKalTrk4->getZHelix(),mdcKalTrk4->getZError() );

          for(int m = 0; m < nGam-1; m++) {
            if(iGam[m]==-1) continue;
            RecEmcShower *g1Trk = (*(evtRecTrkCol->begin()+iGam[m]))->emcShower();
            double eraw1 = g1Trk->energy();
            double phi1 = g1Trk->phi();
            double the1 = g1Trk->theta();
            HepLorentzVector ptrkg1,ptrkg10,ptrkg12;
            ptrkg1.setPx(eraw1*sin(the1)*cos(phi1));
            ptrkg1.setPy(eraw1*sin(the1)*sin(phi1));
            ptrkg1.setPz(eraw1*cos(the1));
            ptrkg1.setE(eraw1);
            ptrkg10 = ptrkg1;
            ptrkg12 = ptrkg1.boost(-0.011,0,0);

            for(int n = m+1; n < nGam; n++) {
              if(iGam[n]==-1) continue;
              RecEmcShower *g2Trk = (*(evtRecTrkCol->begin()+iGam[n]))->emcShower();
              double eraw2 = g2Trk->energy();
              double phi2 = g2Trk->phi();
              double the2 = g2Trk->theta();
              HepLorentzVector ptrkg2,ptrkg20,ptrkg22;
              ptrkg2.setPx(eraw2*sin(the2)*cos(phi2));
              ptrkg2.setPy(eraw2*sin(the2)*sin(phi2));
              ptrkg2.setPz(eraw2*cos(the2));
              ptrkg2.setE(eraw2);
              ptrkg20 = ptrkg2;
              ptrkg22 = ptrkg2.boost(-0.011,0,0);

              /////////////////////////////////////////////////////////////
              HepLorentzVector  ptrkpi0;
              ptrkpi0 = ptrkg12+ptrkg22;
              double m_xmpi0_tem = ptrkpi0.m();
              if(m_xmpi0_tem>0.150||m_xmpi0_tem<0.115)  continue;
              /////////////////////////////////////////////////////////////
              bool IsEndcap1 = false; bool IsEndcap2 = false;
              if(fabs(cos(the1)) > 0.86 && fabs(cos(the1)) < 0.92) IsEndcap1 = true; 
              if(fabs(cos(the2)) > 0.86 && fabs(cos(the2)) < 0.92) IsEndcap2 = true; 
              if(IsEndcap1 && IsEndcap2)  continue;
              /////////////////////////////////////////////////////////////
              KalmanKinematicFit * kmfit = KalmanKinematicFit::instance();
              kmfit->init();
              kmfit->setChisqCut(2500);
              kmfit->AddTrack(0, 0.0, g1Trk);
              kmfit->AddTrack(1, 0.0, g2Trk);
              kmfit->AddResonance(0, mpi0, 0, 1);

              kmfit->Fit(0);  // Perform fit
              kmfit->BuildVirtualParticle(0);

              double pi0_chisq = kmfit->chisq(0);
              if ( pi0_chisq >= 2500) continue;
              HepLorentzVector p2gfit = kmfit->pfit(0) + kmfit->pfit(1);
              p2gfit.boost(-0.011,0,0);
              /////////////////////////////////////////////////////////////

              VertexFit* vtxfit_2 = VertexFit::instance();
              vtxfit_2->init();
              vtxfit_2->AddTrack(0,  pip2);
              vtxfit_2->AddTrack(1,  pim2);
              vtxfit_2->AddVertex(0, vxpar, 0, 1);
              if(!vtxfit_2->Fit(0))  continue;
              vtxfit_2->Swim(0);

              WTrackParameter wpip2 = vtxfit_2->wtrk(0);
              WTrackParameter wpim2 = vtxfit_2->wtrk(1);

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

              HepVector pip2_val = HepVector(7,0);
              HepVector pim2_val = HepVector(7,0);
              HepVector ksp_val = HepVector(7,0);
              HepVector ksm_val = HepVector(7,0);

              pip2_val = wpip2.w();
              pim2_val = wpim2.w();
              ksp_val = wksp.w();
              ksm_val = wksm.w();

              HepLorentzVector P_PIP2(pip2_val[0],pip2_val[1],pip2_val[2],pip2_val[3]);
              HepLorentzVector P_PIM2(pim2_val[0],pim2_val[1],pim2_val[2],pim2_val[3]);
              HepLorentzVector P_KSP(ksp_val[0],ksp_val[1],ksp_val[2],ksp_val[3]);
              HepLorentzVector P_KSM(ksm_val[0],ksm_val[1],ksm_val[2],ksm_val[3]);

              p4kstag.boost(-0.011,0,0);
              P_PIP2.boost(-0.011,0,0);
              P_PIM2.boost(-0.011,0,0);
              P_KSP.boost(-0.011,0,0);
              P_KSM.boost(-0.011,0,0);

              //pddd = P_PIP2 + P_PIM2 + P_KSP + P_KSM + p2gfit;
              pddd = P_PIP2 + P_PIM2 + p4kstag + p2gfit;

              double pk0pipipi0 = pddd.rho();

              double temp1 = (ecms/2)*(ecms/2)-pk0pipipi0*pk0pipipi0 ;
              if(temp1<0) temp1 =0;
              double mass_bc_tem  = sqrt(temp1);
              if(mass_bc_tem <1.82 || mass_bc_tem > 1.89) continue;

              double 	delE_tag_tag = ecms/2-pddd.e();

              if(fabs(delE_tag_tag)<deltaE_tem) {
                deltaE_tem = fabs(delE_tag_tag);
                delE_tag_temp = delE_tag_tag;
                mass_bcgg = mass_bc_tem;

                pddd_temp = pddd;

                ipi1_temp=ipi1;
                ipi2_temp=ipi2;
                ipi3_temp=ipi3;
                ipi4_temp=ipi4;
                iGam1_temp = iGam[m];
                iGam2_temp = iGam[n];

                ncount1 = 1;

              }   
            }       
          }
        }
      }
    }
  }


  if(ncount1 == 1){  
    tagmode=17;
    if(m_chargetag<0)  tagmode=-17;
    tagmd=tagmode;
    mass_bc = mass_bcgg;
    delE_tag = delE_tag_temp;  
    cqtm     = 0;

    iGoodtag.push_back(ipi1_temp);
    iGoodtag.push_back(ipi2_temp);
    iGoodtag.push_back(ipi3_temp);
    iGoodtag.push_back(ipi4_temp);

    iGamtag.push_back(iGam1_temp);
    iGamtag.push_back(iGam2_temp);
    iGamtag.push_back(9999);
    iGamtag.push_back(9999);

    ptag = pddd_temp;

    k0pipipi0md = true;
  }

}



