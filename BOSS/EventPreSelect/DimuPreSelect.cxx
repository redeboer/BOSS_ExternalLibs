#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"

#include "MdcRecEvent/RecMdcTrack.h"
#include "EmcRecEventModel/RecEmcEventModel.h"
#include "TofRecEvent/RecTofTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "MucRawEvent/MucDigi.h"
#include "MucRecEvent/RecMucTrack.h"

#include "EventPreSelect/DimuPreSelect.h"

DimuPreSelect::DimuPreSelect()
{
  m_output=false;
  m_totevent = m_currun = m_curevent = 0;
  for(int i = 0; i < 20; i++)  m_cutpass[i] = 0;
  m_subpass[0] = m_subpass[1] = m_subpass[2] = m_subpass[3] = 0;
  m_totalpass = 0;

  m_propMgr.declareProperty("CmsEnergy",  m_ecms      = 3.686);
  m_propMgr.declareProperty("Vr0Cut",     m_vr0cut    = 1.0);
  m_propMgr.declareProperty("Vz0Cut",     m_vz0cut    = 5.0);
  m_propMgr.declareProperty("PUpCut",     m_pcut_up   = 2.0);
  m_propMgr.declareProperty("PDownCut",   m_pcut_down = 0.5);
  m_propMgr.declareProperty("PSymCut",    m_psymcut   = 0.5);
  m_propMgr.declareProperty("TCut",       m_tcut      = 4);
  m_propMgr.declareProperty("EUpCut",     m_ecut_up   = 1.0);
  m_propMgr.declareProperty("EDownCut",   m_ecut_down = 0.1);
  m_propMgr.declareProperty("DThetaCut",  m_dthetacut = 0.05);
  m_propMgr.declareProperty("DPhiCut",    m_dphicut   = 0.4);
  m_propMgr.declareProperty("PartSelect", m_partselect= 0);
  m_propMgr.declareProperty("MuDigiCut",  m_mudigicut = 6);
  m_propMgr.declareProperty("MuTrkCut",   m_mutrkcut  = 1);

  IJobOptionsSvc* jobSvc;
  Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);
  jobSvc->setMyProperties("DimuSelect", &m_propMgr);
}

void DimuPreSelect::BookNtuple(NTuple::Tuple*& tuple)
{
  m_output=true;
  m_passtuple=tuple;
  if(!m_passtuple) {
    std::cerr << "invalid ntuple in DimuPreSelect!" << std::endl;
  } else {
    m_passtuple->addItem ("run",   m_run);
    m_passtuple->addItem ("event", m_event);
    m_passtuple->addItem ("part",  m_part);
    m_passtuple->addItem ("c1",    m_c1);
    m_passtuple->addItem ("c2",    m_c2);
    m_passtuple->addItem ("r1",    m_r1);
    m_passtuple->addItem ("r2",    m_r2);
    m_passtuple->addItem ("z1",    m_z1);
    m_passtuple->addItem ("z2",    m_z2);
    m_passtuple->addItem ("p1",    m_p1);
    m_passtuple->addItem ("p2",    m_p2);
    m_passtuple->addItem ("t1",    m_t1);
    m_passtuple->addItem ("t2",    m_t2);
    m_passtuple->addItem ("e1",    m_e1);
    m_passtuple->addItem ("e2",    m_e2);
    m_passtuple->addItem ("theta1",m_theta1);
    m_passtuple->addItem ("theta2",m_theta2);
    m_passtuple->addItem ("phi1",  m_phi1);
    m_passtuple->addItem ("phi2",  m_phi2);
    m_passtuple->addItem ("mudigi",m_mudigi);
    m_passtuple->addItem ("mutrk", m_mutrk);

    m_passtuple->addItem ("zeroC",  m_zeroCFlag);
    m_passtuple->addItem ("vtRZ",   m_vtRZFlag);
    m_passtuple->addItem ("pLim",   m_pLimFlag);
    m_passtuple->addItem ("pSym",   m_pSymFlag);
    m_passtuple->addItem ("tLim",   m_tLimFlag);
    m_passtuple->addItem ("eLim",   m_eLimFlag);
    m_passtuple->addItem ("eBB",    m_eBBFlag);
    m_passtuple->addItem ("partSlt",m_partFlag);
    m_passtuple->addItem ("muDigiN",m_mudigiFlag);
    m_passtuple->addItem ("muTrkN", m_mutrkFlag);

    m_passtuple->addItem ("mdc",    m_mdcFlag);
    m_passtuple->addItem ("tof",    m_tofFlag);
    m_passtuple->addItem ("emc",    m_emcFlag);
    m_passtuple->addItem ("muc",    m_mucFlag);
  }
}

int DimuPreSelect::IsDimu()
{
  IMessageSvc* msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "DimuPreSelect");

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

  m_mdcPass = false;
  m_tofPass = false;
  m_emcPass = false;
  m_mucPass = false;

  // Select by MDC Info
  SmartDataPtr<RecMdcTrackCol> mdcTrackCol(eventSvc,"/Event/Recon/RecMdcTrackCol");
  if(!mdcTrackCol)  {
    log << MSG::ERROR << "Could not find RecMdcTrackCol!" << endreq;
    return false;
  }
  log << MSG::INFO << "MDC tracks:\t" << mdcTrackCol->size() << endreq;

  if( mdcTrackCol->size() != 2 ) return 0;

  m_cutpass[0] += 1;

  double c1, c2, r1, r2, z1, z2, p1, p2;
  c1 = c2 = r1 = r2 = z1 = z2 = p1 = p2 = 0.;

  c1 = (*mdcTrackCol)[0]->charge(); c2 = (*mdcTrackCol)[1]->charge();
  r1 = (*mdcTrackCol)[0]->r();      r2 = (*mdcTrackCol)[1]->r();
  z1 = (*mdcTrackCol)[0]->z();      z2 = (*mdcTrackCol)[1]->z();
  p1 = (*mdcTrackCol)[0]->p();      p2 = (*mdcTrackCol)[1]->p();


  bool mdcflag1 = c1 + c2 == 0;
  bool mdcflag2 = fabs(r1)<=m_vr0cut && fabs(z1)<m_vz0cut;
  bool mdcflag3 = fabs(r2)<=m_vr0cut && fabs(z2)<m_vz0cut;
  bool mdcflag4 = p1<m_pcut_up && p2<m_pcut_up;
  //bool mdcflag4 = p1<2 && p1>1;
  //bool mdcflag5 = p2<2 && p2>1;
  bool mdcflag5 = fabs( p1-p2 )/( p1+p2 ) < m_psymcut;

  log << MSG::INFO << "r1:\t" << r1 << "\tz1:" << z1 << endreq;
  log << MSG::INFO << "r2:\t" << r2 << "\tz2:" << z2 << endreq;
  log << MSG::INFO << "p1:\t" << p1 << "\tp2:" << p2 << endreq;

  double zeroCFlag,vtRZFlag,pLimFlag,pSymFlag;
  if( mdcflag1 )             { m_cutpass[1] += 1; zeroCFlag = 1; }
  if( mdcflag2 && mdcflag3 ) { m_cutpass[2] += 1; vtRZFlag = 1; }
  if( mdcflag4 )             { m_cutpass[3] += 1; pLimFlag = 1; }
  if( mdcflag5 )             { m_cutpass[4] += 1; pSymFlag = 1; }

  if( mdcflag1 && mdcflag2 && mdcflag3 && mdcflag4 && mdcflag5 ) { m_mdcPass = true; m_subpass[0] += 1; }
  log << MSG::INFO << "MDC selection done!" << endreq;

  // Select by TOF Info
  SmartDataPtr<RecTofTrackCol> tofTrackCol(eventSvc,"/Event/Recon/RecTofTrackCol");
  if(!tofTrackCol)  {
    log << MSG::ERROR << "Could not find RecTofTrackCol!" << endreq;
    return 0;
  }
  log << MSG::INFO << "TOF tracks:\t" << tofTrackCol->size() << endreq;

  double t1, t2; 
  t1 = 0., t2 = 1000;
  // if(tofTrackCol->size() < 8 && tofTrackCol->size() > 20) 
  if(tofTrackCol->size() > 7 && tofTrackCol->size() < 21) 
  {
    int goodtof = 0;
    for(int itof = 0; itof < tofTrackCol->size(); itof++)
    {
      TofHitStatus *status = new TofHitStatus;  
      status->setStatus((*tofTrackCol)[itof]->status());

      if( !(status->is_cluster()) ) { delete status; continue; }      
      if(goodtof==0) t1 = (*tofTrackCol)[itof]->tof();
      if(goodtof==1) t2 = (*tofTrackCol)[itof]->tof();

      goodtof++;
      delete status;
    }
  }

  double tLimFlag;
  bool tofflag1 = fabs( t1-t2 ) < m_tcut; // ns
  log << MSG::INFO << "t1:\t" << t1 << "\tt2:" << t2 << "dt:\t" << fabs(t1-t2) << endreq;
  if( tofflag1 ) { m_cutpass[5] += 1; tLimFlag = 1; m_tofPass = true; m_subpass[1] += 1; }
  log << MSG::INFO << "TOF selection done!" << endreq;

  // Select by EMC Info
  SmartDataPtr<RecEmcShowerCol> emcShowerCol(eventSvc,"/Event/Recon/RecEmcShowerCol");
  if (!emcShowerCol) { 
    log << MSG::ERROR << "Could not find RecEmcShowerCol!" << endreq;
    return 0;
  }
  log << MSG::INFO << "EMC showers:\t" << emcShowerCol->size() << endreq;

  if( emcShowerCol->size() < 2 ) return 0;

  double e1, e2, theta1, theta2, phi1, phi2;
  int part;

  e1      = (*emcShowerCol)[0]->energy();   e2      = (*emcShowerCol)[1]->energy();
  theta1  = (*emcShowerCol)[0]->theta();    theta2  = (*emcShowerCol)[1]->theta();
  phi1    = (*emcShowerCol)[0]->phi();      phi2    = (*emcShowerCol)[1]->phi();
  part    = (*emcShowerCol)[0]->module(); 

  bool emcFlag1 = e1 < m_ecut_up && e1 > m_ecut_down;
  bool emcFlag2 = e2 < m_ecut_up && e2 > m_ecut_down; 
  bool emcFlag3 = fabs(theta1 + theta2 - CLHEP::pi) < m_dthetacut;
  bool emcFlag4 = fabs(phi1 - phi2) - CLHEP::pi < m_dphicut;
  bool emcFlag5 = !m_partselect || (m_partselect==1&&part==1) || (m_partselect==2&&part!=1);

  log << MSG::INFO << "e1:\t" << e1 << "\te2:\t" << e2 << endreq;
  log << MSG::INFO << "theta1:\t" << theta1 << "\ttheta2:\t" << theta2 << endreq;  
  log << MSG::INFO << "phi1:\t" << phi1 << "\tphi2:\t" << phi2 << endreq;  
  log << MSG::INFO << "part:\t" << part << "\tpartFlag:\t" << emcFlag5 << endreq;  

  double eLimFlag,eBBFlag,partFlag;
  if( emcFlag1 && emcFlag2 ) { m_cutpass[6] += 1; eLimFlag = 1; }
  if( emcFlag3 && emcFlag4 ) { m_cutpass[7] += 1; eBBFlag  = 1; }
  if( emcFlag5 )             { m_cutpass[8] += 1; partFlag = 1; }
  if( emcFlag1 && emcFlag2 && emcFlag3 && emcFlag4 && emcFlag5 ) { m_emcPass = true; m_subpass[2] += 1; }
  log << MSG::INFO << "EMC selection done!" << endreq;

  // Select by MUC Info
  SmartDataPtr<MucDigiCol> mucDigiCol(eventSvc,"/Event/Digi/MucDigiCol");
  if(!mucDigiCol)  {
    log << MSG::ERROR << "Could not find MucDigiCol!" << endreq;
    return 0;
  }
  SmartDataPtr<RecMucTrackCol> mucTrackCol(eventSvc,"/Event/Recon/RecMucTrackCol");
  if (!mucTrackCol) {
    log << MSG::ERROR << "Could not find RecMucTrackCol" << endreq;
    return 0;
  }  

  int mudigiNum, mutrkNum;
  mudigiNum = mutrkNum = 0;
  mudigiNum = mucDigiCol->size(); mutrkNum  = mucTrackCol->size();

  bool mucflag1 = mudigiNum >=  m_mudigicut;
  bool mucflag2 = mutrkNum  >=  m_mutrkcut;

  log << MSG::INFO << "MUC digi:\t" << mudigiNum << "\tMUC track:\t" << mutrkNum << endreq;

  double mudigiFlag,mutrkFlag;
  if( mucflag1 ) { m_cutpass[9]   += 1; mudigiFlag = 1; }
  if( mucflag2 ) { m_cutpass[10]  += 1; mutrkFlag  = 1; }  
  if( mucflag1 && mucflag2 ) { m_mucPass = true; m_subpass[3] += 1; }
  log << MSG::INFO << "MUC selection done!" << endreq;

  if(m_output) {
    m_c1 = c1; m_c2 = c2; m_r1 = r1; m_r2 = r2; m_z1 = z1; m_z2 = z2; m_p1 = p1; m_p2 = p2;
    m_t1 = t1; m_t2 = t2;
    m_e1 = e1; m_e2 = e2; m_theta1 = theta1; m_theta2 = theta2; m_phi1 = phi1; m_phi2 = phi2;
    m_part = part;
    m_mudigi  = mudigiNum;          m_mutrk   = mutrkNum; 
    m_zeroCFlag = zeroCFlag; m_vtRZFlag = vtRZFlag; m_pLimFlag = pLimFlag; m_pSymFlag = pSymFlag;
    m_tLimFlag = tLimFlag; m_eLimFlag = eLimFlag; m_eBBFlag = eBBFlag; m_partFlag = partFlag;
    m_mudigiFlag = mudigiFlag; m_mutrkFlag = mutrkFlag;

    m_mdcFlag = m_mdcPass; m_tofFlag = m_tofPass; m_emcFlag = m_emcPass; m_mucFlag = m_mucPass;

    m_passtuple->write();
  }

  // All selections 
  if( m_mdcPass && m_tofPass && m_emcPass && m_mucPass )
  {    
    m_totalpass += 1;    
    if( part==1 ) return 1; //barrel
    else return 2;          //endcap
    //setFilterPassed(true);
  }

  return 0;

}

void DimuPreSelect::Print()
{
  const string str[3] = {"all","barrel","endcap"};

  cout << "pass 0 - 2 MDC tracks            : " << m_cutpass[0] << endl;
  cout << "pass 1 - 0 charge                : " << m_cutpass[1] << endl;
  cout << "pass 2 - |r|<"<<m_vr0cut<<" && |z|<"<<m_vz0cut<<"          : " << m_cutpass[2] << endl;
  cout << "pass 3 - p < "<<m_pcut_up<<" GeV/c             : " << m_cutpass[3] << endl;
  cout << "pass 4 - p_sym < "<<m_psymcut<<"             : " << m_cutpass[4] << endl;
  cout << "pass 5 - |t1-t1| < "<<m_tcut<<" ns          : " << m_cutpass[5] << endl;
  cout << "pass 6 - "<<m_ecut_down<<" < e < "<<m_ecut_up<<"             : " << m_cutpass[6] << endl;
  cout << "pass 7 - |dth|<"<<m_dthetacut<<" && |dphi|<"<<m_dphicut<<": " << m_cutpass[7] << endl;
  cout << "pass 8 - "<< str[(int)m_partselect] <<" part is selected    : " << m_cutpass[8] << endl;
  cout << "pass 9 - mudigi >= "<<m_mudigicut<<"             : " << m_cutpass[9] << endl;
  cout << "pass 10- mutrk  >= "<<m_mutrkcut<<"             : " << m_cutpass[10]<< endl;

  cout << "pass MDC : " << m_subpass[0] << endl;
  cout << "pass TOF : " << m_subpass[1] << endl;
  cout << "pass EMC : " << m_subpass[2] << endl;
  cout << "pass MUC : " << m_subpass[3] << endl;

  cout<<"Total event: " << m_totevent   <<endl;
  cout<<"Dimu event: "  << m_totalpass  <<endl;
}
