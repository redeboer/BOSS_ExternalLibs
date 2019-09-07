//
//  SD0Tag.cxx is the Single D0 tag code transfered from the Fortran routine "SD0Tag.f"
//  which was used for study of the D0D0-bar production and D0 decays at the BES-II experiment.
//  The orignal routine "SD0Tag.f" used at the BES-II experiment was coded by G. Rong in 2001.
//
//  SD0Tag.cxx was transfered by G. Rong and J. Liu in December, 2005.
//  Since 2008, G. Rong and L.L. Jiang have been working on developing this
//  code to analyze of the data taken at 3.773 GeV with the BES-III detector
//  at the BEPC-II collider.
//
//  During developing this code, many People made significant contributions to this code. These are
//          G. Rong, L.L. Jiang, J. Liu, H.L. Ma, J.C. Chen, D.H. Zhang,
//          M.G. Zhao, B. Zheng, L. Li, Y. Fang, Z.Y. Yi, H.H. Liu, Z.Q. Liu et al.
//
//                                       By G. Rong and L.L. Jiang
//                                       March, 2009
//
//
//  We updated the Single D0 Tag Software Package for the BES-III collaborators use in their studies
//  of the D0 semileptonic decays as well as other decays. Acctually, the Referee committee for reviewing
//  these analysis of the D0-->K-e+v, pi-e+v decays and the BES-III Physics Analysis Coordinators required 
//  the analysis authors for these decays to use the common analysis cuts for selection of the events 
//  of anti-D0 tags VS the D0-->K-e+v, pi-e+v for preparing the analysis MEMO and paper to be published.
//  They also recommended the analysis authors to use the IHEP SD0Tag Software to select the anti-D0 tags in
//  their analyses. To response the Analysis Committee Members and Physics Coordinator requirements for these
//  analyses, we updated the Single anti-D0 Tag Software Package SD0Tag (D0TagAlg-00-00-02) with the common 
//  event selection cuts set by the Referee Committe and the Physics Analysis Coordinators. The updated version
//  of the SD0Tag is SD0TagAlg-00-00-03. In this releasion of the software, we use this common event selection cuts.
//  The details of these common event selection cuts can be found on the webpage of 
//
//                                http://hnbes3.ihep.ac.cn/HyperNews/get/paper71/75.html
//  
//  In an e-mail message from the Referee Committee and Physics Analysis Coordinators about these analyses, 
//  the Referee Committee and Physics Analysis Coordinators like to recommend for analysis authors to use 
//  the SDTS to do anti-D0 reconstruction. They also required the IHEP authors to supply the run-dependent 
//  Ebeam that have been used in the original IHEP analysis.
//
//  In the updated releasion of SD0Tag (D0TagAlg-00-00-03), we supply all of these.
//
//                                       
//                                       G. Rong,  L.L. Jiang, Y. Fang and H.L. Ma
//                                       1 Dec, 2013
//
//  ==========================================================================================
//
#include "SD0TagAlg/SD0Tag.h"
#include "SD0TagAlg/Sing.h"
#include "SD0TagAlg/SingleBase.h"
#include "DTagTool/DTagTool.h"
//#include "Ecmset/Ecmset.h"
#include <iostream>
#include <fstream>
int nD0 = 0;
int n1 = 0;
int n2 = 0;
int ND0 = 0;
int NDp = 0;
//

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
/////////////////////////////////////////////////////////////////////////////

SD0Tag::SD0Tag(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
    //Declare the properties
    declareProperty("PID_FLAG",            PID_flag = 1);
    declareProperty("MC_sample",           m_MC_sample = 1);
 
    declareProperty("m_Seperate_Charge",   Seperate_Charge = 1);
    declareProperty("m_Charge_default",    Charge_default = 1);
             

  }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode SD0Tag::initialize(){


  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;

  StatusCode status;

  ifstream readrunEcm;
//
// Read in the energy calibration constant for run-by-run data
  readrunEcm.open("../share/psipp_ecms_calrunbyrun_runno_3648runs.dat");
  cout<<"readrunEcm = "<<readrunEcm.is_open()<<endl;
  for(int i = 0; i < 3467; i++) {
    readrunEcm>>p_run[i]; readrunEcm>>p_Ecm[i];
  }


  NTuplePtr nt1(ntupleSvc(), "FILE1/tag");
  if ( nt1 ) m_tuple1 = nt1;
  else {
    m_tuple1 = ntupleSvc()->book ("FILE1/tag", CLID_ColumnWiseTuple, "tag N-Tuple example");
    if ( m_tuple1 )  {
      status = m_tuple1->addItem ("tagmode",          m_tagmode);
      status = m_tuple1->addItem ("mass_bc",          m_mass_bc);
      status = m_tuple1->addItem ("delE_tag",         m_delE_tag);

      status = m_tuple1->addItem ("cosmic_ok",        m_cosmic_ok);    
      status = m_tuple1->addItem ("EGam_max_0",       m_EGam_max_0);
      status = m_tuple1->addItem ("nGood",            m_nGood);      
      status = m_tuple1->addItem ("nGam",             m_nGam);      
      status = m_tuple1->addItem ("runNo",            m_runNo);      
      status = m_tuple1->addItem ("event",            m_event);      
    }
    else  {
      log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) <<endmsg;
      return StatusCode::FAILURE;
    }
  }  

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;
}


//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//

StatusCode SD0Tag::execute() {

  MsgStream log(msgSvc(), name());

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  int runNo=eventHeader->runNumber();
  int event=eventHeader->eventNumber();

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);
  //cout<<"//////////////////////////////////////"<<endl;
  nD0++;

  //
  // The default energy is 3.773 GeV for psi(3770) data. 
  // Alayizer can add calibrated energy here.
  double Ebeam = 3.773/2.0;

  if(runNo >=11414 && runNo <= 23500) {
    for(int i = 0; i < 3467; i++) {
      if(runNo == p_run[i]) {Ebeam = p_Ecm[i]/2.0;}
    }
  }

  if(runNo < 0)
  {
    int irun = abs(runNo);
//    Ecmset*  ECMSET = Ecmset::instance();
//    ECMSET->EcmSet(irun);
//    Ebeam =  ECMSET->ECM()/2.0;
  }

  if(nD0%1000==0) cout<<"SD0TagAlg-13-11-15pp = "<<nD0<<"  "<<runNo<<"   "<<event<<"   "<<Ebeam*2<<endl;

  Hep3Vector xorigin(0,0,0);
  IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if(vtxsvc->isVertexValid()){
    double* dbv = vtxsvc->PrimaryVertex();
    double*  vv = vtxsvc->SigmaPrimaryVertex();
    xorigin.setX(dbv[0]);
    xorigin.setY(dbv[1]);
    xorigin.setZ(dbv[2]);
  }

  /////////////////////////////////////
  Vint iCharge_good; iCharge_good.clear();
  for(int i = 0; i <  evtRecEvent->totalCharged(); i++){
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isMdcTrackValid()) continue;
    RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
    //%%%%%%%%%%%%%%%%%%%%%%%%
    HepVector a = mdcTrk->helix();
    HepSymMatrix Ea = mdcTrk->err();
    HepPoint3D point0(0.,0.,0.);   // the initial point for MDC recosntruction
    HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]);
    VFHelix helixip(point0,a,Ea);
    helixip.pivot(IP);
    HepVector vecipa = helixip.a();
    double  Rvxy0=fabs(vecipa[0]);  //the nearest distance to IP in xy plane
    double  Rvz0=vecipa[3];         //the nearest distance to IP in z direction
    double  costheta = cos(mdcTrk->theta());
    //%%%%%%%%%%%%%%%%%%%%%%%%
    if(fabs(Rvxy0) >= 1.0) continue;
    if(fabs(Rvz0)  >= 10.0) continue;
    if(fabs(costheta) >= 0.930) continue;
    iCharge_good.push_back(i);
  }

  ///////////////////////////////////////////////////////
  Vint iGam; iGam.clear();
  for(int i =  evtRecEvent->totalCharged(); i<  evtRecEvent->totalTracks(); i++) {
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid()) continue;
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    //
    //   We here remove the hot channels of EMC 
    //
    if(abs(runNo)>=11615&&abs(runNo)<=11617&&emcTrk->cellId()==805438481)  continue;
    if(abs(runNo)>=11615&&abs(runNo)<=11655&&emcTrk->cellId()==805376008)  continue;
    if(abs(runNo)>=13629&&abs(runNo)<=13669&&emcTrk->cellId()==805374754)  continue;
    if(abs(runNo)>=21190&&abs(runNo)<=21231&&emcTrk->cellId()==805375262)  continue;

    int emctdc = emcTrk->time();
    if(emctdc>14||emctdc<0) continue;

    double eraw = emcTrk->energy();
    double theta = emcTrk->theta();
    int Module = 0;
    if(fabs(cos(theta)) < 0.80 && eraw > 0.025){   Module = 1;   }
    if(fabs(cos(theta)) > 0.86 && fabs(cos(theta)) < 0.92 && eraw > 0.05) {   Module = 2;      }
    if(Module == 0)  continue; 
    iGam.push_back((*itTrk)->trackId());
  }  
  /////////////////////////////////////////////////////////////////
  DTagTool trk;
  bool cosmic_ok = trk.cosmicandleptonVeto();
  /////////////////////////////////////////////////////////////////

  int   ntk;
  double tagmass,ebeam,tagmode,ksmass,dlte;

  Vint tagtrk;   tagtrk.clear();
  Vint tagGam;   tagGam.clear();

  HepLorentzVector  tagp;

  double mass_bc_temp, mass_kf_temp, kf_chi2_temp, mks_temp, mpi0_temp, ptag_temp; 
  int Charge_candidate_D = 0;
  double  EGam_max_0 = 0;

  for(int i1 = 0; i1 < 2; i1++) {
    if(Seperate_Charge == 2 ) {  Charge_candidate_D = Charge_default; i1 = 2;}
    if(Seperate_Charge == 1 ) {  Charge_candidate_D = pow(-1,i1); }
    if(Seperate_Charge == 0 ) {  Charge_candidate_D = 0; i1 = 2; }

    for(int i = 0; i < 15; i++) {
      EGam_max_0 = 0;   
      int  mdslct=pow(2.,i);
      Sing sing;
      sing.Mdset(event,evtRecTrkCol,iCharge_good,iGam,mdslct,Ebeam, PID_flag,Charge_candidate_D);
      bool oktag=sing.Getoktg();
      if(oktag) {
        //
        //  Here analysizer should pick up variables from anti-D0 tags
        //  to do physics analysis
        //
        tagtrk=sing.Gettagtrk1();
        tagmode = sing.Gettagmd();
        //==========================================================================
        if((abs(tagmode) == 11 || abs(tagmode) == 15 || abs(tagmode) == 19)) {
          //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
          for(int i =  evtRecEvent->totalCharged(); i <  evtRecEvent->totalTracks(); i++) {
            EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
            int itrk = (*itTrk)->trackId();
            if(!(*itTrk)->isEmcShowerValid()) continue;
            RecEmcShower *emcTrk = (*itTrk)->emcShower();

            int emctdc = emcTrk->time();
            if(emctdc>14||emctdc<0) continue;

            Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());
            double dang = 200.;
            for(int j = 0; j <  tagtrk.size(); j++) {
              EvtRecTrackIterator jtTrk = evtRecTrkCol->begin() + tagtrk[j];
              if(!(*jtTrk)->isExtTrackValid()) continue;
              RecExtTrack *extTrk = (*jtTrk)->extTrack();
              if(extTrk->emcVolumeNumber() == -1) continue;
              Hep3Vector extpos = extTrk->emcPosition();
              double angd = extpos.angle(emcpos);
              if(angd < dang) dang = angd;
            } 
            dang = dang * 180 / (CLHEP::pi);
            if(fabs(dang)<20.) continue;

            double eraw = emcTrk->energy();
            if(eraw > EGam_max_0) EGam_max_0 = eraw;
          }          
        }     
        //==========================================================================
        m_cosmic_ok  = cosmic_ok;
        m_EGam_max_0 = EGam_max_0;
        m_nGood = iCharge_good.size();
        m_nGam = iGam.size(); 
        m_runNo = runNo;
        m_event = event;

        m_tagmode = sing.Gettagmd();
        m_mass_bc = sing.Getmass_bc();
        m_delE_tag = sing.GetdelE_tag();

        m_tuple1->write();

        //      Here one can do double tag analysis based on the variables
        //      from single anti-D0 tag analysis.
        //      To do so, analyzer should make his/her codes
        //
      }

    }
  }  // The end of loopping over mdslct

  return StatusCode::SUCCESS;

}


//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//

StatusCode SD0Tag::finalize() {
  MsgStream log(msgSvc(), name());
  cout<<"nD0         =    "<<nD0<<endl;
  cout<<"n1         =    "<<n1<<endl;
  cout<<"n2         =    "<<n2<<endl;
  cout<<"ND0     ====   "<<ND0<<endl;
  cout<<"NDp     ====   "<<NDp<<endl;
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

