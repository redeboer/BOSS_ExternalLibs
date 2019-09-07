#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecPi0.h"
#include "VertexFit/KinematicFit.h"
#include "Pi0RecAlg/Pi0RecAlg.h"
#include "Pi0RecAlg/MakeGroupList.h"
const double xmpi0 =  0.13497;

//*******************************************************************************************
Pi0RecAlg::Pi0RecAlg(const std::string& name, ISvcLocator* pSvcLocator) :
	Algorithm(name, pSvcLocator)
{
	//Declare the properties
	declareProperty("EMinOfBarrel",       _pi0_cut.MinEnergyOfBarrelPhoton = 0.025);
	declareProperty("EMinOfEndcap",       _pi0_cut.MinEnergyOfEndcapPhoton = 0.045);
	declareProperty("Angle",       _pi0_cut.MinAngle = 20);
	declareProperty("TimeLimit",       _pi0_cut.TimeLimit = 20);

	declareProperty("Pi0MinMass",       _pi0_cut.MinMass = 0.10);
	declareProperty("Pi0MaxMass",       _pi0_cut.MaxMass = 0.18);
	declareProperty("ChisqCut",         _pi0_cut.Chisq = 50.0);
}

//******************************************************************************************
StatusCode Pi0RecAlg::initialize() {

	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in initialize()" <<endreq;

	return StatusCode::SUCCESS;
}


//*********************************************************************************************
StatusCode Pi0RecAlg::execute() {

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
	using namespace Pi0;
	//   std::cout<<"**********************************************"<<std::endl;
	static Criteria cri_inv(_pi0_cut.MinMass, _pi0_cut.MaxMass);
       
	//static KFitCriteria cri_kfit;

        UserPi0Cut::SetForTrack(recEvt, recTrkCol);

//	Pi0::make_gamma_list(recEvt, recTrkCol);
	Pi0::make_gamma_list(_pi0_cut);
	Pi0::make_pi0_list(Pi0::GetDefaultGammaList());
	//		Pi0::print_gamma_list(Pi0::GetDefaultGammaList());
	Pi0::apply_criteria(cri_inv);
	Pi0::GetCandidatePi0List().sort(high_momentum());
	//	Pi0::print_pi0_list(Pi0::GetCandidatePi0List());
	Pi0::Pi0ListToTDS(Pi0::GetCandidatePi0List(), recPi0Col);
	//   std::cout<<"**********************************************"<<std::endl;
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
StatusCode Pi0RecAlg::finalize() {

	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in finalize()" << endreq;

	return StatusCode::SUCCESS;
}

HepLorentzVector Pi0RecAlg::getP4(RecEmcShower* gTrk) {

	double eraw = gTrk->energy();
	double phi =  gTrk->phi();
	double the =  gTrk->theta();

	return HepLorentzVector( eraw * sin(the) * cos(phi),
			eraw * sin(the) * sin(phi),
			eraw * cos(the),
			eraw );
}

//   KinematicFit * kmfit = KinematicFit::instance();

//
//  Set up the pi0's list
//
/*   for(int i1 = recEvt->totalCharged(); i1 < (recEvt->totalTracks()-1); i1++) {
     EvtRecTrack* g1Trk = *(recTrkCol->begin() + i1);
     if ( !g1Trk->isPhoton() ) continue;
     RecEmcShower* g1Shower = g1Trk->emcShower();
     HepLorentzVector g1P4 = getP4(g1Shower);

     for(int i2 = i1+1; i2 < recEvt->totalTracks(); i2++) {
     EvtRecTrack* g2Trk = *(recTrkCol->begin() + i2);
     if ( !g2Trk->isPhoton() ) continue;
     RecEmcShower* g2Shower = g2Trk->emcShower();
     HepLorentzVector g2P4 = getP4(g2Shower);

     HepLorentzVector p2g = g1P4 + g2P4;
     if ( p2g.m() < m_minmass_cut ) continue;
     if ( p2g.m() > m_maxmass_cut ) continue;

     kmfit->init();
     kmfit->setIterNumber(1);
     kmfit->AddTrack(0, 0.0, g1Shower);
     kmfit->AddTrack(1, 0.0, g2Shower);
     kmfit->AddResonance(0, xmpi0, 0, 1);
     if ( !kmfit->Fit(0) ) continue;
     kmfit->BuildVirtualParticle(0);
     if ( kmfit->chisq(0) > m_chisq_cut ) continue;

     double e1 = (kmfit->pfit(0)).e();
     double e2 = (kmfit->pfit(1)).e();
     HepLorentzVector ppi0 = kmfit->pfit(0) + kmfit->pfit(1);
     double f = (e1-e2)/ppi0.rho();
     if ( fabs(f) > m_costheta_cut ) continue;

     EvtRecPi0* recPi0 = new EvtRecPi0();
     recPi0->setRawMass(p2g.restMass());
     recPi0->setChisq(kmfit->chisq(0));
     recPi0->setFcos(f);
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
     }
     }*/

//   std::cout<<"**********************************************"<<std::endl;
/*   std::cout<<"A(";
     for(int i1 = 0; i1 < (recEvt->totalTracks()); i1++) 
     {
     EvtRecTrack* g1Trk = *(recTrkCol->begin() + i1);
     std::cout<<g1Trk->trackId()<<", ";
     }
     std::cout<<")"<<std::endl;
     std::cout<<"N(";
     for(int i1 = recEvt->totalCharged(); i1 < (recEvt->totalTracks()); i1++) 
     {
     EvtRecTrack* g1Trk = *(recTrkCol->begin() + i1);
     std::cout<<g1Trk->trackId()<<", ";
     }
     std::cout<<")"<<std::endl;
     std::cout<<"C(";
     for(int i1 = 0; i1 < recEvt->totalCharged(); i1++) 
     {
     EvtRecTrack* g1Trk = *(recTrkCol->begin() + i1);
     std::cout<<g1Trk->trackId()<<", ";
     }
     std::cout<<")"<<std::endl;*/
