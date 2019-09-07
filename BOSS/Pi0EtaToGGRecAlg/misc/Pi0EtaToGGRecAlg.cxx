#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecPi0.h"
#include "VertexFit/KinematicFit.h"

#include "Pi0EtaToGGRecAlg/Pi0EtaToGGRecAlg.h"

const double xmpi0 =  0.13497;

//*******************************************************************************************
Pi0EtaToGGRecAlg::Pi0EtaToGGRecAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   Algorithm(name, pSvcLocator)
{
   //Declare the properties
   declareProperty("Pi0MinMass",       m_minmass_cut  = 0.07);
   declareProperty("Pi0MaxMass",       m_maxmass_cut  = 0.18);
   declareProperty("Pi0ChisqCut",      m_chisq_cut    = 20.0);

   /// Individual photons
   declareProperty("PhotonUseBarrelEmc",    m_useBarrel        = true);
   declareProperty("PhotonUseEndcapEmc",    m_useEndcap        = true);
   declareProperty("PhotonMinEnergy",       m_minEnergy        = 0.02);
   declareProperty("PhotonDeltaAngleCut",   m_angled_cut       = 20.0);
   declareProperty("PhotonDeltaThetaCut",   m_thed_cut         = 20.0);
   declareProperty("PhotonDeltaPhiCut",     m_phid_cut         = 20.0);

}

//******************************************************************************************
StatusCode Pi0EtaToGGRecAlg::initialize() {

   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" <<endreq;

   return StatusCode::SUCCESS;
}


//*********************************************************************************************
StatusCode Pi0EtaToGGRecAlg::execute() {

   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in execute()" <<endreq;

   // get event header, eventlist and tracklist from TDS
   SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
   SmartDataPtr<EvtRecEvent> recEvt(eventSvc(), EventModel::EvtRec::EvtRecEvent);
   SmartDataPtr<EvtRecTrackCol> recTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);

   bool save2TDS = false;
   SmartDataPtr<EvtRecPi0Col> recPi0Col(eventSvc(), EventModel::EvtRec::EvtRecPi0Col);
   if ( !recPi0Col ) {
      recPi0Col = new EvtRecPi0Col;
      save2TDS = true;
   }

   KinematicFit * kmfit = KinematicFit::instance();

   //
   //  Set up the pi0's list
   //
   for(int i1 = recEvt->totalCharged(); i1 < (recEvt->totalTracks()-1); i1++) {
      EvtRecTrack* g1Trk = *(recTrkCol->begin() + i1);
      if ( !validPhoton(g1Trk,recEvt,recTrkCol) ) continue;
      RecEmcShower* g1Shower = g1Trk->emcShower();
      HepLorentzVector g1P4 = getP4(g1Shower);

      for(int i2 = i1+1; i2 < recEvt->totalTracks(); i2++) {
	 EvtRecTrack* g2Trk = *(recTrkCol->begin() + i2);
	 if ( !validPhoton(g2Trk,recEvt,recTrkCol) ) continue;
	 RecEmcShower* g2Shower = g2Trk->emcShower();
	 HepLorentzVector g2P4 = getP4(g2Shower);

	 HepLorentzVector p2g = g1P4 + g2P4;
	 if ( p2g.m() < m_minmass_cut ) continue;
	 if ( p2g.m() > m_maxmass_cut ) continue;

	 /// Perform pi0 1C kinfit 
	 kmfit->init();
	 kmfit->setIterNumber(1);
	 kmfit->AddTrack(0, 0.0, g1Shower);
	 kmfit->AddTrack(1, 0.0, g2Shower);
	 kmfit->AddResonance(0, xmpi0, 0, 1);
	 if ( !kmfit->Fit(0) ) continue;
	 kmfit->BuildVirtualParticle(0);

	 double pi0_chisq = kmfit->chisq(0);
	 if ( pi0_chisq > m_chisq_cut ) continue;

	 double e1 = (kmfit->pfit(0)).e();
	 double e2 = (kmfit->pfit(1)).e();
	 HepLorentzVector ppi0 = kmfit->pfit(0) + kmfit->pfit(1);

	 EvtRecPi0* recPi0 = new EvtRecPi0();
	 recPi0->setRawMass(p2g.restMass());
	 recPi0->setChisq(pi0_chisq);
	 recPi0->setFcos((e1-e2)/ppi0.rho());

	 if ( g1P4.e() >= g2P4.e() ) {
	    recPi0->setHiPfit(kmfit->pfit(0));
	    recPi0->setLoPfit(kmfit->pfit(1));
	    recPi0->setHiEnGamma(g1Trk);
	    recPi0->setLoEnGamma(g2Trk);
	 }
	 else {
	    recPi0->setHiPfit(kmfit->pfit(1));
	    recPi0->setLoPfit(kmfit->pfit(0));
	    recPi0->setHiEnGamma(g2Trk);
	    recPi0->setLoEnGamma(g1Trk);
	 }

	 std::cout << "p(g1) = " <<  g1P4 << std::endl; 
	 std::cout << "p(g2) = " <<  g2P4 << std::endl; 
	 std::cout << "chi2(pi0) = " << pi0_chisq << std::endl; 
	 std::cout << "m(gg) before fill = " << p2g.restMass() << std::endl << std::endl;

	 recPi0Col->push_back(recPi0);
      }
   }

   /// Fill number of pi0s in EvtRecEvent
   recEvt->setNumberOfPi0(recPi0Col->size());

   if ( save2TDS ) {
      StatusCode sc = eventSvc()->registerObject(EventModel::EvtRec::EvtRecPi0Col, recPi0Col);
      if ( sc.isFailure() ) {
	 log << MSG::ERROR << "could not register EvtRecPi0Col in TDS" <<endreq;
	 return StatusCode::FAILURE;
      }
   }

   return StatusCode::SUCCESS;
}


//********************************************************************************************
StatusCode Pi0EtaToGGRecAlg::finalize() {

   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;

   return StatusCode::SUCCESS;
}

HepLorentzVector Pi0EtaToGGRecAlg::getP4(RecEmcShower* gTrk){

   double eraw = gTrk->energy();
   double phi =  gTrk->phi();
   double the =  gTrk->theta();

   return HepLorentzVector( eraw * sin(the) * cos(phi),
	                    eraw * sin(the) * sin(phi),
			    eraw * cos(the),
			    eraw );
}


bool Pi0EtaToGGRecAlg::validPhoton(EvtRecTrack* recTrk, 
				   SmartDataPtr<EvtRecEvent> recEvt, 
				   SmartDataPtr<EvtRecTrackCol> recTrkCol) 
{

  if ( !recTrk->isEmcShowerValid() ) return( false );

  RecEmcShower *emcTrk = recTrk->emcShower();


  // flag = 1: Barrel = 0,2 : Endcap
  int flag =(emcTrk->cellId() & 0x000F0000) >> 16;
  if ((flag == 1) && !m_useBarrel) return( false );
  if ((flag == 0 || flag == 2) && !m_useEndcap) return( false );

  if ( emcTrk->energy() < m_minEnergy ) return( false );

  /// find the nearest charged track
  Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());
  if (recEvt->totalCharged() > 0) 
  {      
    double dthe = 200.;
    double dphi = 200.;
    double dang1 = 200.; 
    for (int j = 0; j < recEvt->totalCharged(); j++) {
      EvtRecTrackIterator jtTrk = recTrkCol->begin() + j;
      if ( !(*jtTrk)->isExtTrackValid() ) continue;
      RecExtTrack* extTrk = (*jtTrk)->extTrack();
      if ( extTrk->emcVolumeNumber() == -1 ) continue;
      Hep3Vector extpos = extTrk->emcPosition();
      double  angd1 = extpos.angle(emcpos);
      double  thed = extpos.theta() - emcpos.theta();
      double  phid = extpos.deltaPhi(emcpos);
      thed = fmod(thed+CLHEP::twopi+CLHEP::twopi+CLHEP::pi, CLHEP::twopi) - CLHEP::pi;
      phid = fmod(phid+CLHEP::twopi+CLHEP::twopi+CLHEP::pi, CLHEP::twopi) - CLHEP::pi;

      if ( fabs(thed) < fabs(dthe) ) dthe = thed;
      if ( fabs(phid) < fabs(dphi) ) dphi = phid;
      if ( angd1 < dang1 ) dang1 = angd1;
    }
    if ( dang1 < 200 ) {
      dthe = dthe * 180 / (CLHEP::pi);
      dphi = dphi * 180 / (CLHEP::pi);
      dang1 = dang1 * 180 / (CLHEP::pi);
      if ( m_angled_cut > 0 ) {
	if ( dang1 < m_angled_cut ) return( false );
      } else {
	if ( (fabs(dthe) < m_thed_cut) && (fabs(dphi)<m_phid_cut) ) return( false );
      }
    }  // End of "dang1 < 200" IF 

  }  // End of "recEvt->totalCharged() > 0" IF

  return true;
}
