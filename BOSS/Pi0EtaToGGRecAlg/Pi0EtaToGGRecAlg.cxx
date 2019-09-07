#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecPi0.h"
#include "EvtRecEvent/EvtRecEtaToGG.h"
#include "VertexFit/KalmanKinematicFit.h"

#include "Pi0EtaToGGRecAlg/Pi0EtaToGGRecAlg.h"

const double xmpi0 =  0.134976;  // BOSS 6.4.1 pdt.table value 
const double xmeta =  0.54784;   // PDG08 = 0.547853, BOSS 6.4.1 pdt.table = 0.54784

//*************************************************************************************
Pi0EtaToGGRecAlg::Pi0EtaToGGRecAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   Algorithm(name, pSvcLocator)
{
   //Declare the properties

   declareProperty("RejectBothInEndcap",  m_rejectEndEnd = false);

   /// pi0 properties
   declareProperty("Pi0MinMass",  m_pi0minmass_cut = 0.08);
   declareProperty("Pi0MaxMass",  m_pi0maxmass_cut = 0.18);
   declareProperty("Pi0ChisqCut", m_pi0chisq_cut   = 2500);

   /// eta properties
   declareProperty("EtaMinMass",  m_etaminmass_cut = 0.4);
   declareProperty("EtaMaxMass",  m_etamaxmass_cut = 0.7);
   declareProperty("EtaChisqCut", m_etachisq_cut   = 2500);


   /// Individual photons properties
   declareProperty("PhotonMinEnergy", m_minEnergy = 0.025);

   declareProperty("PhotonInBarrelOrEndcap",  m_useBarrelEndcap   = true);
   declareProperty("PhotonMaxCosThetaBarrel", m_maxCosThetaBarrel = 0.8);
   declareProperty("PhotonMinCosThetaEndcap", m_minCosThetaEndcap = 0.86);
   declareProperty("PhotonMaxCosThetaEndcap", m_maxCosThetaEndcap = 0.92);
   declareProperty("PhotonMinEndcapEnergy",   m_minEndcapEnergy   = 0.050);

   declareProperty("PhotonApplyTimeCut", m_applyTimeCut = true);
   declareProperty("PhotonMinTime",      m_minTime      = 0.);
   declareProperty("PhotonMaxTime",      m_maxTime      = 14.);
   declareProperty("PhotonDeltaTime",    m_deltaTime    = 10.);

   declareProperty("PhotonApplyDangCut", m_applyDangCut = false);
   declareProperty("PhotonMinDang",      m_minDang      = 20.0);

   /// Limits for number of photons, pi0s and etas
   declareProperty("MaxNGoodPhoton",     m_maxNGoodPhoton  =  50);
   declareProperty("MaxNPi0",            m_maxNPi0         =  100);
   //declareProperty("MaxNEta",            m_maxNEta         =  100);

   declareProperty("DoClear",            m_doClear         =  false);
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

   SmartDataPtr<EvtRecPi0Col> recPi0Col(eventSvc(), EventModel::EvtRec::EvtRecPi0Col);
   if ( !recPi0Col ) {
      recPi0Col = new EvtRecPi0Col;
      StatusCode sc = eventSvc()->registerObject(EventModel::EvtRec::EvtRecPi0Col, recPi0Col);
      if ( sc.isFailure() ) {
	 log << MSG::ERROR << "could not register EvtRecPi0Col in TDS" <<endreq;
	 return StatusCode::FAILURE;
      }
   }

   SmartDataPtr<EvtRecEtaToGGCol> recEtaToGGCol(eventSvc(), EventModel::EvtRec::EvtRecEtaToGGCol);
   if ( !recEtaToGGCol ) {
      recEtaToGGCol = new EvtRecEtaToGGCol;
      StatusCode sc = eventSvc()->registerObject(EventModel::EvtRec::EvtRecEtaToGGCol, recEtaToGGCol);
      if ( sc.isFailure() ) {
	 log << MSG::ERROR << "could not register EvtRecEtaToGGCol in TDS" <<endreq;
	 return StatusCode::FAILURE;
      }
   }

   if ( m_doClear ) {
       SmartIF<IDataManagerSvc> dataManSvc(eventSvc());
       dataManSvc->clearSubTree(EventModel::EvtRec::EvtRecPi0Col);
       dataManSvc->clearSubTree(EventModel::EvtRec::EvtRecEtaToGGCol);
   }

   // select good photons
   std::vector<EvtRecTrack*> vGoodPhotons;
   for ( int i = recEvt->totalCharged(); i < recEvt->totalTracks(); i++ ) {
      EvtRecTrack* gTrk = *(recTrkCol->begin() + i);
      if ( !validPhoton(gTrk,recEvt,recTrkCol) ) continue;
      vGoodPhotons.push_back(gTrk);
   }

   int nGoodPhoton = vGoodPhotons.size();
   if ( nGoodPhoton > m_maxNGoodPhoton ) return StatusCode::SUCCESS;

   /// Needed for 1C mass fits for pi0 and eta
   KalmanKinematicFit * kmfit = KalmanKinematicFit::instance();

   //
   //  Form two-photon combinations
   //
   for(int i1 = 0; i1 < (nGoodPhoton-1); i1++) {
      EvtRecTrack* g1Trk = vGoodPhotons[i1];
      RecEmcShower* g1Shower = g1Trk->emcShower();
      HepLorentzVector g1P4 = getP4(g1Shower);

      for(int i2 = i1+1; i2 < nGoodPhoton; i2++) {
	 EvtRecTrack* g2Trk = vGoodPhotons[i2];
	 RecEmcShower* g2Shower = g2Trk->emcShower();
	 HepLorentzVector g2P4 = getP4(g2Shower);

	 HepLorentzVector p2g = g1P4 + g2P4;

	 /// If RejectBothInEndcap=true, reject candidate if both photons are in endcaps
	 if(m_rejectEndEnd&&bothInEndcap(g1P4,g2P4)) continue;

	 /// Select pi0 candidates 
	 if ((p2g.m() > m_pi0minmass_cut) && (p2g.m() < m_pi0maxmass_cut )) 
	 {

	   /// Perform pi0 1C KinematicFit 
	   kmfit->init();
	   kmfit->setIterNumber(5);
	   kmfit->AddTrack(0, 0.0, g1Shower);
	   kmfit->AddTrack(1, 0.0, g2Shower);
	   kmfit->AddResonance(0, xmpi0, 0, 1);

	   kmfit->Fit(0);  // Perform fit
	   kmfit->BuildVirtualParticle(0);

	   double pi0_chisq = kmfit->chisq(0);
	   if ( pi0_chisq >= m_pi0chisq_cut ) continue;


	   /// Fill EvtRecPi0
	   EvtRecPi0* recPi0 = new EvtRecPi0();
	   recPi0->setUnconMass(p2g.restMass());
	   recPi0->setChisq(pi0_chisq);

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
	   recPi0Col->push_back(recPi0);

	 }  // End of pi0 mass window IF


	 /// Select eta candidates 
	 if ((p2g.m() > m_etaminmass_cut) && (p2g.m() < m_etamaxmass_cut )) 
	 {

	   /// Perform eta 1C KinematicFit 
	   kmfit->init();
	   kmfit->setIterNumber(5);
	   kmfit->AddTrack(0, 0.0, g1Shower);
	   kmfit->AddTrack(1, 0.0, g2Shower);
	   kmfit->AddResonance(0, xmeta, 0, 1);

	   kmfit->Fit(0);  // Perform fit
	   kmfit->BuildVirtualParticle(0);

	   double eta_chisq = kmfit->chisq(0);
	   if ( eta_chisq >= m_etachisq_cut ) continue;


	   /// Fill EvtRecEtaToGG
	   EvtRecEtaToGG* recEtaToGG = new EvtRecEtaToGG();
	   recEtaToGG->setUnconMass(p2g.restMass());
	   recEtaToGG->setChisq(eta_chisq);

	   if ( g1P4.e() >= g2P4.e() ) {
	     recEtaToGG->setHiPfit(kmfit->pfit(0));
	     recEtaToGG->setLoPfit(kmfit->pfit(1));
	     recEtaToGG->setHiEnGamma(g1Trk);
	     recEtaToGG->setLoEnGamma(g2Trk);
	   }
	   else {
	     recEtaToGG->setHiPfit(kmfit->pfit(1));
	     recEtaToGG->setLoPfit(kmfit->pfit(0));
	     recEtaToGG->setHiEnGamma(g2Trk);
	     recEtaToGG->setLoEnGamma(g1Trk);
	   }
	   recEtaToGGCol->push_back(recEtaToGG);

	 }  // End of etaToGG mass window IF

      }  // End of g2Trk evtRec FOR 
   }  // End of g1Trk evtRec FOR 

   // If there are too many pi0s, it's possibly a bad event and all pi0s are abandoned
   if ( recPi0Col->size() > m_maxNPi0 ) {
      recPi0Col->clear();
   }

   return StatusCode::SUCCESS;
}


//********************************************************************************************
StatusCode Pi0EtaToGGRecAlg::finalize() {

   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;

   return StatusCode::SUCCESS;
}



/// ************************** FUNCTIONS *****************************************************
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
   if ( !recTrk->isEmcShowerValid() ) return false;

   RecEmcShower *emcTrk = recTrk->emcShower();

   HepLorentzVector shP4 = getP4(emcTrk);
   double cosThetaSh = shP4.vect().cosTheta();


   /// Minimum energy
   if (shP4.e() <= m_minEnergy) return false;


   /// Barrel/Endcap
   if ( m_useBarrelEndcap ) {
      bool inBarrelEndcap = false;

      if(fabs(cosThetaSh) < m_maxCosThetaBarrel) inBarrelEndcap = true;

      if((fabs(cosThetaSh) > m_minCosThetaEndcap)
	    &&(fabs(cosThetaSh) < m_maxCosThetaEndcap)
	    &&(shP4.e() > m_minEndcapEnergy)) inBarrelEndcap = true;

      if ( !inBarrelEndcap ) return false;
   }


   /// Time, only apply timing cuts if "recEvt->totalCharged() > 0"
   if ( m_applyTimeCut ) {
      double time = emcTrk->time();
      if ( recEvt->totalCharged() > 0 ) {
	 if ( time < m_minTime || time > m_maxTime ) return false;
      }
      else {
	 RecEmcShower* firstG = (*(recTrkCol->begin()))->emcShower();
	 double deltaTime = fabs(time - firstG->time());
	 if ( deltaTime > 10 ) return false;
      }
   }


   /// Dang
   if (m_applyDangCut && recEvt->totalCharged() > 0) 
   {      
      Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());
      double dang = 200.; 

      for (int j = 0; j < recEvt->totalCharged(); j++) {
	 EvtRecTrackIterator jtTrk = recTrkCol->begin() + j;
	 if ( !(*jtTrk)->isExtTrackValid() ) continue;
	 RecExtTrack* extTrk = (*jtTrk)->extTrack();
	 if ( extTrk->emcVolumeNumber() == -1 ) continue;
	 Hep3Vector extpos = extTrk->emcPosition();
	 double  angd1 = extpos.angle(emcpos);
	 if ( angd1 < dang ) dang = angd1;
      }

      if ( dang < 200 ) {
	 dang = dang * 180 / (CLHEP::pi);
	 if ( dang <= m_minDang ) return false;
      }
   }  // End of "recEvt->totalCharged() > 0" IF

   return true;
}


bool Pi0EtaToGGRecAlg::bothInEndcap(HepLorentzVector g1_P4, HepLorentzVector g2_P4){

  double g1_CosTheta = g1_P4.vect().cosTheta();
  double g2_CosTheta = g2_P4.vect().cosTheta();

  bool g1InEndcap = false;
  bool g2InEndcap = false;

  if((fabs(g1_CosTheta) > m_minCosThetaEndcap)
     &&(fabs(g1_CosTheta) < m_maxCosThetaEndcap)) g1InEndcap = true;

  if((fabs(g2_CosTheta) > m_minCosThetaEndcap)
     &&(fabs(g2_CosTheta) < m_maxCosThetaEndcap)) g2InEndcap = true;

  if(g1InEndcap&&g2InEndcap) return( true );

  return( false );
}
