//
//  Kpipi0pi0.cxx is the single D0 tag code to reconstruct D0 or anti-D0 through the final states of
//  kpipi0pi0 from D0 decays. Kpipi0pi0.cxx was transfered from the Fortran routine "Kpipi0pi0.f"
//  which was orignally used for study of the D0D0-bar production and D0 decays at the BES-II
//  experiment during the time period from 2002 to 2008.
//
//  The orignal Fortran routine "Kpipi0pi0.f" used at the BES-II experiment was coded by H.L. Ma
//  and G. Rong in 2003.
//
//  Kpipi0pi0.cxx was transfered by G. Rong and J. Liu in December, 2005.
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


#include "SD0TagAlg/Kpipi0pi0.h"
#include "SD0TagAlg/SingleBase.h"


Kpipi0pi0::Kpipi0pi0()
{}

Kpipi0pi0::~Kpipi0pi0()
{}


void Kpipi0pi0::MTotal(double event,SmartDataPtr<EvtRecTrackCol> evtRecTrkCol, Vint iGood,Vint
    iGam, double Ebeam, int PID_flag, int Charge_candidate_D)
{

  int nGood=iGood.size();
  int nGam=iGam.size();

  iGoodtag.clear();
  iGamtag.clear();
  
  double mass_bcgg,delE_tag_temp;
  int m_chargetag, m_chargek,m_chargepi;
  int ika_temp,ipi_temp, iGam1_temp, iGam2_temp, iGam3_temp, iGam4_temp;
  HepLorentzVector pddd;
  HepLorentzVector pddd_temp;

  int cqtm_temp;
  IDataProviderSvc* eventSvc = NULL;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc,EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");

  int runNo=eventHeader->runNumber();
  int rec=eventHeader->eventNumber();

  double xecm=2*Ebeam;

  kpipi0pi0md=false;
  double  tagmode=0;

  if((evtRecEvent->totalCharged() < 2||nGam<4)){     return;  }

  double ecms = xecm;


  ISimplePIDSvc* simple_pid;
  Gaudi::svcLocator()->service("SimplePIDSvc", simple_pid);
 
  double deltaE_tem = 0.20;
  int ncount1 = 0; 

  Hep3Vector xorigin(0,0,0);
  IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if(vtxsvc->isVertexValid())
  {
    double* dbv = vtxsvc->PrimaryVertex();
    double*  vv = vtxsvc->SigmaPrimaryVertex();
    xorigin.setX(dbv[0]);
    xorigin.setY(dbv[1]);
    xorigin.setZ(dbv[2]);
  }

  double xv=xorigin.x();
  double yv=xorigin.y();
  double zv=xorigin.z();

  HepPoint3D point0(0.,0.,0.);
  HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]);
  //////////////////////////////////////////////////////////////////
  HepLorentzVector p2gfit1, p2gfit2;
  HepLorentzVector p2gg;

  HepLorentzVector ptrk1_temp, ptrk2_temp, ptrk3_temp, ptrk4_temp, ptrk5_temp, ptrk6_temp, ptrk7_temp, ptrk8_temp;
  for(int i = 0; i < evtRecEvent->totalCharged(); i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + i;

    int ika= (*itTrk)->trackId();

    if(!(*itTrk)->isMdcKalTrackValid()) continue;
    RecMdcKalTrack*  mdcKalTrk1 = (*itTrk)->mdcKalTrack();
    RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon);
    /////////////////////////////////////////
    m_chargek=mdcKalTrk1->charge();
    if(Charge_candidate_D != 0) {
      if(m_chargek != -Charge_candidate_D) continue;
    }
    if(Charge_candidate_D == 0) {
      if(abs(m_chargek) != 1) continue;
    }
    /////////////////////////////////////////
    HepVector a1 = mdcKalTrk1->getZHelixK();
    HepSymMatrix Ea1 = mdcKalTrk1->getZErrorK();
    VFHelix helixip3_1(point0,a1,Ea1);
    helixip3_1.pivot(IP);
    HepVector  vecipa1 = helixip3_1.a();

    double dr1 = fabs(vecipa1[0]);
    double dz1 = fabs(vecipa1[3]);
    double costheta1 = cos(mdcKalTrk1->theta());
    if (  dr1 >= 1.0) continue;
    if (  dz1 >= 10.0) continue; 
    if ( fabs(costheta1) >= 0.93) continue; 
    /////////////////////////////////////////
    if(PID_flag == 5) {
      simple_pid->preparePID(*itTrk);
      if(simple_pid->probKaon() < 0.0 || simple_pid->probKaon() < simple_pid->probPion()) continue;  
    } 
    /////////////////////////////////////////


    WTrackParameter kam(xmass[3],mdcKalTrk1->getZHelixK(),mdcKalTrk1->getZErrorK() );

    //	
    // select pi
    //	
    for(int j = 0; j< evtRecEvent->totalCharged(); j++) {
      EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + j;

      int ipi= (*itTrk)->trackId();
      if(ipi==ika)  continue;

      if(!(*itTrk)->isMdcKalTrackValid()) continue;
      RecMdcKalTrack*  mdcKalTrk2 = (*itTrk)->mdcKalTrack();
      RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
      /////////////////////////////////////////
      m_chargepi=mdcKalTrk2->charge();
      if((m_chargek + m_chargepi) != 0) continue;
      /////////////////////////////////////////
      HepVector a2 = mdcKalTrk2->getZHelix();
      HepSymMatrix Ea2 = mdcKalTrk2->getZError();
      VFHelix helixip3_2(point0,a2,Ea2);
      helixip3_2.pivot(IP);
      HepVector  vecipa2 = helixip3_2.a();

      double dr2 = fabs(vecipa2[0]);
      double dz2 = fabs(vecipa2[3]);
      double costheta2 = cos(mdcKalTrk2->theta());
      if (  dr2 >= 1.0) continue;
      if (  dz2 >= 10.0) continue; 
      if ( fabs(costheta2) >= 0.93) continue; 
      /////////////////////////////////////////
      if(PID_flag == 5) {
        simple_pid->preparePID(*itTrk);
        if(simple_pid->probPion() < 0.0 || simple_pid->probPion() < simple_pid->probKaon()) continue;  
      } 
      /////////////////////////////////////////

      WTrackParameter pip(xmass[2],mdcKalTrk2->getZHelix(),mdcKalTrk2->getZError() );

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
          double m_xmpi0_tem = ptrkpi0.mag();
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
          kmfit->AddTrack(0, 0.0 , g1Trk);
          kmfit->AddTrack(1, 0.0 , g2Trk);
          kmfit->AddResonance(0, mpi0, 0, 1);
          kmfit->Fit(0);  // Perform fit
          kmfit->BuildVirtualParticle(0);

          double pi0_chisq = kmfit->chisq(0);
          if ( pi0_chisq >= 2500) continue;
          HepLorentzVector p2gfit1 = kmfit->pfit(0) + kmfit->pfit(1);
          p2gfit1.boost(-0.011,0,0);

          for(int s = 0; s < nGam-1; s++) {
            if(iGam[s]==-1) continue;
            RecEmcShower *g3Trk = (*(evtRecTrkCol->begin()+iGam[s]))->emcShower();
            if(iGam[s] == iGam[m] || iGam[s] == iGam[n] )   continue;
            double eraw3 = g3Trk->energy();
            double phi3 = g3Trk->phi();
            double the3 = g3Trk->theta();
            HepLorentzVector ptrkg3,ptrkg30,ptrkg32;
            ptrkg3.setPx(eraw3*sin(the3)*cos(phi3));
            ptrkg3.setPy(eraw3*sin(the3)*sin(phi3));
            ptrkg3.setPz(eraw3*cos(the3));
            ptrkg3.setE(eraw3);
            ptrkg30 = ptrkg3;
            ptrkg32 = ptrkg3.boost(-0.011,0,0);

            for(int r = s+1; r < nGam; r++) {
              if(iGam[r]==-1) continue;
              RecEmcShower *g4Trk = (*(evtRecTrkCol->begin()+iGam[r]))->emcShower();
              if(iGam[r] == iGam[m] || iGam[r] == iGam[n] )   continue;
              double eraw4 = g4Trk->energy();
              double phi4 = g4Trk->phi();
              double the4 = g4Trk->theta();
              HepLorentzVector ptrkg4,ptrkg40,ptrkg42;
              ptrkg4.setPx(eraw4*sin(the4)*cos(phi4));
              ptrkg4.setPy(eraw4*sin(the4)*sin(phi4));
              ptrkg4.setPz(eraw4*cos(the4));
              ptrkg4.setE(eraw4);
              ptrkg40 = ptrkg4;
              ptrkg42 = ptrkg4.boost(-0.011,0,0);

              /////////////////////////////////////////////////////////////
              HepLorentzVector  ptrkpi0_2;
              ptrkpi0_2 = ptrkg32+ptrkg42;
              double m_xmpi0_2_tem = ptrkpi0_2.mag();
              if(m_xmpi0_2_tem>0.150||m_xmpi0_2_tem<0.115)  continue;
              /////////////////////////////////////////////////////////////
              bool IsEndcap3 = false; bool IsEndcap4 = false;
              if(fabs(cos(the3)) > 0.86 && fabs(cos(the3)) < 0.92) IsEndcap3 = true;
              if(fabs(cos(the4)) > 0.86 && fabs(cos(the4)) < 0.92) IsEndcap4 = true;
              if(IsEndcap3 && IsEndcap4)  continue;
              /////////////////////////////////////////////////////////////

              KalmanKinematicFit * kmfit2 = KalmanKinematicFit::instance();
              kmfit2->init();
              kmfit2->setChisqCut(2500);
              kmfit2->AddTrack(0, 0.0 , g3Trk);
              kmfit2->AddTrack(1, 0.0 , g4Trk);
              kmfit2->AddResonance(0, mpi0, 0, 1);

              kmfit2->Fit(0);  // Perform fit
              kmfit2->BuildVirtualParticle(0);

              double pi0_2_chisq = kmfit2->chisq(0);
              if ( pi0_2_chisq >= 2500) continue;
              HepLorentzVector p2gfit2 = kmfit2->pfit(0) + kmfit2->pfit(1);
              p2gfit2.boost(-0.011,0,0);


              //////////////////////////////////////////////////////////////
              HepPoint3D vx(xorigin.x(), xorigin.y(), xorigin.z());
              HepSymMatrix Evx(3, 0);
              double bx = 1E+6; Evx[0][0] = bx*bx;
              double by = 1E+6; Evx[1][1] = by*by;
              double bz = 1E+6; Evx[2][2] = bz*bz;
              VertexParameter vxpar; vxpar.setVx(vx); vxpar.setEvx(Evx);
              //////////////////////////////////////////////////////////////

              VertexFit* vtxfit = VertexFit::instance();
              vtxfit->init();
              vtxfit->AddTrack(0,  kam);
              vtxfit->AddTrack(1,  pip);
              vtxfit->AddVertex(0, vxpar, 0, 1);
              if(!vtxfit->Fit(0))  continue;
              vtxfit->Swim(0);

              WTrackParameter wkam = vtxfit->wtrk(0);
              WTrackParameter wpip = vtxfit->wtrk(1);

              HepVector kam_val = HepVector(7,0);
              kam_val = wkam.w();
              HepVector pip_val = HepVector(7,0);
              pip_val = wpip.w();

              HepLorentzVector P_KAM(kam_val[0],kam_val[1],kam_val[2],kam_val[3]);
              HepLorentzVector P_PIP(pip_val[0],pip_val[1],pip_val[2],pip_val[3]);

              P_KAM.boost(-0.011,0,0);
              P_PIP.boost(-0.011,0,0);
              pddd = P_KAM + P_PIP + p2gfit1 + p2gfit2;

              double pkpipi0pi0=pddd.rho();

              double temp1 = (ecms/2)*(ecms/2)-pkpipi0pi0*pkpipi0pi0 ;
              if(temp1<0) temp1 =0;
              double mass_bc_tem  = sqrt(temp1);
              if(mass_bc_tem < 1.82 || mass_bc_tem > 1.89) continue;

              double  delE_tag_tag = ecms/2-pddd.e();

              if(fabs(delE_tag_tag)<deltaE_tem) {
                deltaE_tem = fabs(delE_tag_tag);
                delE_tag_temp = delE_tag_tag;
                mass_bcgg = mass_bc_tem;

                pddd_temp = pddd; 
                cqtm_temp = m_chargek;
                
                ika_temp=ika;
                ipi_temp=ipi;

                iGam1_temp = iGam[m];
                iGam2_temp = iGam[n];
                iGam3_temp = iGam[s];
                iGam4_temp = iGam[r];
                ncount1 = 1;

              }
            }
          }
        }
      }
    }
  }

  if(ncount1 == 1){
    tagmode=25;
    if(cqtm_temp<0) tagmode=-25;
    tagmd=tagmode;
    mass_bc = mass_bcgg;
    delE_tag = delE_tag_temp;

    cqtm     = -1.0*cqtm_temp;

    iGoodtag.push_back(ipi_temp);
    iGoodtag.push_back(ika_temp);

    iGamtag.push_back(iGam1_temp);
    iGamtag.push_back(iGam2_temp);
    iGamtag.push_back(iGam3_temp);
    iGamtag.push_back(iGam4_temp);
    
    ptag = pddd_temp;

    kpipi0pi0md = true;

  }


}



