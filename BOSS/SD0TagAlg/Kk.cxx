//
//  Kk.cxx is the single D0 tag code to reconstruct D0 or anti-D0 through the final states of
//  KK from D0 decays. Kk.cxx was transfered from the Fortran routine "Kk.f" which was 
//  orignally used for study of the D0D0-bar production and D0 decays at the BES-II
//  experiment during the time period from 2002 to 2008.
//
//  The orignal Fortran routine "Kk.f" used at the BES-II experiment was coded by G. Rong in 2002.
//
//  Kk.cxx was transfered by G. Rong and J. Liu in December, 2005.
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
#include "SD0TagAlg/Kk.h"
#include "SD0TagAlg/SingleBase.h"

Kk::Kk()
{}

Kk::~Kk()
{}

void Kk::MTotal(double event,SmartDataPtr<EvtRecTrackCol> evtRecTrkCol, Vint iGood,Vint
    iGam, double Ebeam, int PID_flag, int Charge_candidate_D)
{

  int nGood=iGood.size();
  int nGam=iGam.size();

  iGoodtag.clear();
  iGamtag.clear();
  
  double mass_bcgg, delE_tag_temp;
  int m_chargetag, m_chargek1,m_chargek2;
  int ika1_temp,ika2_temp,ipi2_temp,ipi3_temp;
  HepLorentzVector kmfit1,kmfit2,kmfit3,kmfit4,pddd;

  HepLorentzVector pddd_temp;
  IDataProviderSvc* eventSvc = NULL;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc,EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");

  int runNo=eventHeader->runNumber();
  int rec=eventHeader->eventNumber();

  double xecm=2*Ebeam;

  kkmd=false;
  double  tagmode=0;

  if((evtRecEvent->totalCharged() < 2)){    return;  }

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

  for(int i = 0; i < evtRecEvent->totalCharged(); i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + i;

    int ika1= (*itTrk)->trackId();

    if(!(*itTrk)->isMdcKalTrackValid()) continue;
    RecMdcKalTrack*  mdcKalTrk1 = (*itTrk)->mdcKalTrack();
    RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon);

    m_chargek1=mdcKalTrk1->charge();
    if(m_chargek1 != 1) continue;

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
    WTrackParameter kap(xmass[3],mdcKalTrk1->getZHelixK(),mdcKalTrk1->getZErrorK() );

    //	
    // select K2
    //	
    for(int j = 0; j< evtRecEvent->totalCharged();j++) {
      EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + j;

      int ika2= (*itTrk)->trackId();
      if(ika1==ika2)  continue;

      if(!(*itTrk)->isMdcKalTrackValid()) continue;
      RecMdcKalTrack*  mdcKalTrk2 = (*itTrk)->mdcKalTrack();
      RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon);

      m_chargek2=mdcKalTrk2->charge();
      if((m_chargek1 + m_chargek2) != 0) continue;

      /////////////////////////////////////////
      HepVector a2 = mdcKalTrk2->getZHelixK();
      HepSymMatrix Ea2 = mdcKalTrk2->getZErrorK();
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
        if(simple_pid->probKaon() < 0.0 || simple_pid->probKaon() < simple_pid->probPion()) continue;  
      } 
      /////////////////////////////////////////
      WTrackParameter kam(xmass[3],mdcKalTrk2->getZHelixK(),mdcKalTrk2->getZErrorK() );

      //  all loop for the charged tracks overed here !!!!!!!

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
      vtxfit->AddTrack(0,  kap);
      vtxfit->AddTrack(1,  kam);
      vtxfit->AddVertex(0, vxpar, 0, 1);
      if(!vtxfit->Fit(0))  continue;
      vtxfit->Swim(0);

      WTrackParameter wkap = vtxfit->wtrk(0);
      WTrackParameter wkam = vtxfit->wtrk(1);

      HepVector kap_val = HepVector(7,0);
      kap_val = wkap.w();
      HepVector kam_val = HepVector(7,0);
      kam_val = wkam.w();

      HepLorentzVector P_KAP(kap_val[0],kap_val[1],kap_val[2],kap_val[3]);
      HepLorentzVector P_KAM(kam_val[0],kam_val[1],kam_val[2],kam_val[3]);

      P_KAP.boost(-0.011,0,0);
      P_KAM.boost(-0.011,0,0);
      pddd = P_KAP + P_KAM;

      double pkk=pddd.rho();

      double temp1 = (ecms/2)*(ecms/2)-pkk*pkk;
      if(temp1<0) temp1 =0;
      double mass_bc_tem  = sqrt(temp1);
      if(mass_bc_tem < 1.82 || mass_bc_tem > 1.89) continue;
      
      double  delE_tag_tag = ecms/2-pddd.e();

      if(fabs(delE_tag_tag)<deltaE_tem){
        deltaE_tem = fabs(delE_tag_tag);
        delE_tag_temp = delE_tag_tag;
        mass_bcgg = mass_bc_tem;

        pddd_temp = pddd;
        ika1_temp=ika1;
        ika2_temp=ika2;
        ncount1 = 1;
      }   
    }
  } 

  if(ncount1 == 1){  
    tagmode=15;
    if(m_chargetag<0)  tagmode=-15;
    tagmd=tagmode;
    mass_bc  = mass_bcgg;
    delE_tag = delE_tag_temp;
    cqtm     = -0.0; 

    iGoodtag.push_back(ika1_temp);
    iGoodtag.push_back(ika2_temp);
    iGamtag.push_back(9999);
    iGamtag.push_back(9999);
    iGamtag.push_back(9999);
    iGamtag.push_back(9999);
    
    ptag = pddd_temp;

    kkmd = true;
  }
}



