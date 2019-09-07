#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecVeeVertex.h"

#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "DstEvent/TofHitStatus.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "FarmMonitorAlg/FarmMonitorAlg.h"

using namespace std;
using namespace Event;

double mPi     = 0.13957;
double mKs     = 0.49767;
double mLambda = 1.11568;


/////////////////////////////////////////////////////////////////////////////

FarmMonitorAlg::FarmMonitorAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties  
  declareProperty("PrintRunEventFreq", m_RunEventFreq = 10);

  declareProperty("Ecm", m_ecm = 3.686);
  declareProperty("Vr0cut", m_vr0cut = 1.0);
  declareProperty("Vz0cut", m_vz0cut = 5.0);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode FarmMonitorAlg::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  
  StatusCode status;

  //histograms

  /// Total energies
  h_eVisibleDivEcm = histoSvc()->book("eVisibleDivEcm", 1, "eVisible/Ecm", 150, 0.0, 1.5);
  h_eEMCDivEcm     = histoSvc()->book("eEMCDivEcm",     1, "eEMC/Ecm",     150, 0.0, 1.5);
  h_eNeutralDivEcm = histoSvc()->book("eNeutralDivEcm", 1, "eNeutral/Ecm", 150, 0.0, 1.5);
  h_eChargedDivEcm = histoSvc()->book("eChargedDivEcm", 1, "eCharged/Ecm", 150, 0.0, 1.5);

  /// Net momenta
  h_netMomentumDivEcm_AllChargedTracks = histoSvc()->book("netMomentumDivEcm_AllChargedTracks", 
							  1, "#Sigma p/Ecm (Charged)", 100, 0.0, 1.0);
  h_netTransMomentumDivEcm_AllChargedTracks = histoSvc()->book("netTransMomentumDivEcm_AllChargedTracks",
							  1, "#Sigma ptr/Ecm (Charged)", 100, 0.0, 1.0);
  h_cosTheta_AllChargedTracks = histoSvc()->book("cosTheta_AllChargedTracks",
							  1, "cos#theta (Charged)", 100, -1.0, 1.0);

  h_netMomentumDivEcm_AllNeutralTracks = histoSvc()->book("netMomentumDivEcm_AllNeutralTracks", 
							  1, "#Sigma p/Ecm (Neutral)", 100, 0.0, 1.0);
  h_netTransMomentumDivEcm_AllNeutralTracks = histoSvc()->book("netTransMomentumDivEcm_AllNeutralTracks",
							  1, "#Sigma ptr/Ecm (Neutral)", 100, 0.0, 1.0);
  h_cosTheta_AllNeutralTracks = histoSvc()->book("cosTheta_AllNeutralTracks",
							  1, "cos#theta (Neutral)", 100, -1.0, 1.0);

  h_netMomentumDivEcm_AllTracks = histoSvc()->book("netMomentumDivEcm_AllTracks", 
							  1, "#Sigma p/Ecm", 100, 0.0, 1.0);
  h_netTransMomentumDivEcm_AllTracks = histoSvc()->book("netTransMomentumDivEcm_AllTracks",
							  1, "#Sigma ptr/Ecm", 100, 0.0, 1.0);
  h_cosTheta_AllTracks = histoSvc()->book("cosTheta_AllTracks",
							  1, "cos#theta", 100, -1.0, 1.0);

  /// Charged Tracks
  h_trackR0 = histoSvc()->book("trackR0", 1, "R0 (cm)", 100, 0.0, 2.0);
  h_trackZ0 = histoSvc()->book("trackZ0", 1, "Z0 (cm)", 100, -20.0, 20.0);

  h_nChargedTracks   = histoSvc()->book("nChargedTracks",   1, "NChargedTracks", 17, -0.5, 16.5);
  h_nChargedTracksIP = histoSvc()->book("nChargedTracksIP", 1, "NChargedTracks from IP", 17, -0.5, 16.5);

  h_netCharge   = histoSvc()->book("netCharge",   1, "Net Charge", 11, -5.5, 5.5);
  h_netChargeIP = histoSvc()->book("netChargeIP", 1, "Net Charge with IP tracks", 11, -5.5, 5.5);

  /// 2 highest momentum charged tracks
  h_pIPTrack1DivEb = histoSvc()->book("pIPTrack1DivEb", 1, "pTrack1/Ebeam", 150, 0.0, 1.5);
  h_pIPTrack2DivEb = histoSvc()->book("pIPTrack2DivEb", 1, "pTrack2/Ebeam", 150, 0.0, 1.5);

  h_eEMCIPTrack1DivEb = histoSvc()->book("eEMCIPTrack1DivEb", 1, "eEMCTrack1/Ebeam", 150, 0.0, 1.5);
  h_eEMCIPTrack2DivEb = histoSvc()->book("eEMCIPTrack2DivEb", 1, "eEMCTrack2/Ebeam", 150, 0.0, 1.5);

  h_acoplanarity_2HighestPIPTracks = histoSvc()->book("acoplanarity_2HighestPIPTracks", 
						    1, "acoplanarity", 158, 0.0, 3.16);

  /// Neutral Tracks
  h_nNeutralTracks         = histoSvc()->book("nNeutralTracks",         1, "NNeutralTracks", 31, -0.5, 30.5);
  h_nNeutralTracksGT30MeV  = histoSvc()->book("nNeutralTracksGT30MeV",  1, "NNeutralTracksGT30MeV", 31, -0.5, 30.5);

  h_eEMCShower1DivEb = histoSvc()->book("eEMCShower1DivEb", 1, "eEMCShower1/Ebeam", 150, 0.0, 1.5);
  h_eEMCShower2DivEb = histoSvc()->book("eEMCShower2DivEb", 1, "eEMCShower2/Ebeam", 150, 0.0, 1.5);
  h_eEMCShower3DivEb = histoSvc()->book("eEMCShower3DivEb", 1, "eEMCShower3/Ebeam", 150, 0.0, 1.5);


  /// MUC information
  h_mucDepth = histoSvc()->book("mucDepth", 1, "mucDepth", 200, 0.0, 200.0);
  h_mucDepthVsCosTheta = histoSvc()->book("mucDepthVsCosTheta", 2, "mucDepthVsCosTheta", 
					  100, -1.0, 1.0, 200, 0.0, 200.0);
  h_mucDepthVsPhi = histoSvc()->book("mucDepthVsPhi", 2, "mucDepthVsPhi", 
				     180, -3.15, 3.15, 200, 0.0, 200.0);

  /// PID (dE/dx) information
  h_dedxTotalHitsIP = histoSvc()->book("dedxTotalHitsIP", 1, "dedxTotalHitsIP", 70, 0.0, 70.0);
  h_dedxGoodHitsIP  = histoSvc()->book("dedxGoodHitsIP",  1, "dedxGoodHitsIP", 70, 0.0, 70.0);
  h_dedxElecChiIP   = histoSvc()->book("dedxElecChiIP",   1, "dedxElecChiIP", 120, -6.0, 6.0);
  h_dedxMuonChiIP   = histoSvc()->book("dedxMuonChiIP",   1, "dedxMuonChiIP", 120, -6.0, 6.0);
  h_dedxPionChiIP   = histoSvc()->book("dedxPionChiIP",   1, "dedxPionChiIP", 120, -6.0, 6.0);
  h_dedxKaonChiIP   = histoSvc()->book("dedxKaonChiIP",   1, "dedxKaonChiIP", 120, -6.0, 6.0);
  h_dedxProtonChiIP = histoSvc()->book("dedxProtonChiIP", 1, "dedxProtonChiIP", 120, -6.0, 6.0);
  h_dedxProbPHIP = histoSvc()->book("dedxProbPHIP", 1, "dedxProbPHIP", 200, 0.0, 2000.0);
  h_dedxProbPHVsMomentumIP = histoSvc()->book("dedxProbPHVsMomentumIP", 2, "dedxProbPHVsMomentumIP", 
				     100, 0.0, 2.0, 200, 0.0, 2000.0);

  /// PID (TOF) information
  h_tofPHIP_BarrelLayer1 = histoSvc()->book("tofPHIP_BarrelLayer1", 1, "tofPHIP_BarrelLayer1", 250, 0.0, 5000.0);
  h_tofPHIP_BarrelLayer2 = histoSvc()->book("tofPHIP_BarrelLayer2", 1, "tofPHIP_BarrelLayer2", 250, 0.0, 5000.0);
  h_tofPHIP_EastEndcap = histoSvc()->book("tofPHIP_EastEndcap", 1, "tofPHIP_EastEndcap", 200, 0.0, 4000.0);
  h_tofPHIP_WestEndcap = histoSvc()->book("tofPHIP_WestEndcap", 1, "tofPHIP_WestEndcap", 200, 0.0, 4000.0);
  h_tofIP_BarrelLayer1 = histoSvc()->book("tofIP_BarrelLayer1", 1, "tofIP_BarrelLayer1", 100, 0.0, 20.0);
  h_tofIP_BarrelLayer2 = histoSvc()->book("tofIP_BarrelLayer2", 1, "tofIP_BarrelLayer1", 100, 0.0, 20.0);
  h_tofIP_EastEndcap = histoSvc()->book("tofIP_EastEndcap", 1, "tofIP_EastEndcap", 100, 0.0, 20.0);
  h_tofIP_WestEndcap = histoSvc()->book("tofIP_WestEndcap", 1, "tofIP_WestEndcap", 100, 0.0, 20.0);
  h_tofElecIP_Barrel   = histoSvc()->book("tofElecIP_Barrel",   1, "tofElecIP_Barrel", 120, -6.0, 6.0);
  h_tofMuonIP_Barrel   = histoSvc()->book("tofMuonIP_Barrel",   1, "tofMuonIP_Barrel", 120, -6.0, 6.0);
  h_tofPionIP_Barrel   = histoSvc()->book("tofPionIP_Barrel",   1, "tofPionIP_Barrel", 120, -6.0, 6.0);
  h_tofKaonIP_Barrel   = histoSvc()->book("tofKaonIP_Barrel",   1, "tofKaonIP_Barrel", 120, -6.0, 6.0);
  h_tofProtonIP_Barrel = histoSvc()->book("tofProtonIP_Barrel", 1, "tofProtonIP_Barrel", 120, -6.0, 6.0);
  h_tofElecIP_Endcap   = histoSvc()->book("tofElecIP_Endcap",   1, "tofElecIP_Endcap", 120, -6.0, 6.0);
  h_tofMuonIP_Endcap   = histoSvc()->book("tofMuonIP_Endcap",   1, "tofMuonIP_Endcap", 120, -6.0, 6.0);
  h_tofPionIP_Endcap   = histoSvc()->book("tofPionIP_Endcap",   1, "tofPionIP_Endcap", 120, -6.0, 6.0);
  h_tofKaonIP_Endcap   = histoSvc()->book("tofKaonIP_Endcap",   1, "tofKaonIP_Endcap", 120, -6.0, 6.0);
  h_tofProtonIP_Endcap = histoSvc()->book("tofProtonIP_Endcap", 1, "tofProtonIP_Endcap", 120, -6.0, 6.0);
  h_tofVsMomentumIP = histoSvc()->book("tofVsMomentumIP", 2, "tofVsMomentumIP", 
				       100, 0.0, 2.0, 100, 0.0, 20.0);

  /// VeeVertex information 
  h_nKs = histoSvc()->book("nKs",       1,    "nKs",  9, -0.5,   8.5);
  h_ksMass = histoSvc()->book("ksMass", 1, "ksMass", 70, 0.480, 0.515);

  h_nLambda = histoSvc()->book("nLambda",       1,    "nLambda",  9, -0.5,   8.5);
  h_lambdaMass = histoSvc()->book("lambdaMass", 1, "lambdaMass", 76, 1.100, 1.133);

  // Add Pi0 histograms when official pi0 list is ready


  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode FarmMonitorAlg::execute() {
  
  double eBeam = m_ecm/2;

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  int run=eventHeader->runNumber();
  int event=eventHeader->eventNumber();
  if( event%m_RunEventFreq == 0)  std::cout << "Run " << run << ", event " << event << std::endl;

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
    log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
      << evtRecEvent->totalCharged() << " , "
      << evtRecEvent->totalNeutral() << " , "
      << evtRecEvent->totalTracks() <<endreq;

  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);


  ////////////////////////////////////////
  /// Variables for charged track FOR loop
  int nChargedTracks = 0, nChargedTracksIP = 0;
  int nCharge = 0, nChargeIP = 0;
  double totalVisibleEnergy = 0, totalChargedEnergy = 0, totalEMCEnergy = 0;
  double totalChargedPX = 0, totalChargedPY = 0, totalChargedPZ = 0; 

  double highestIPTrackP = -1, secondHighestIPTrackP = -2;
  int highestPIPTrackId = -1, secondHighestPIPTrackId = -1;

  /// Charged track FOR loop
  for(int i = 0; i < evtRecEvent->totalCharged(); i++){
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isMdcTrackValid()) continue;
    RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();

    int trackId = mdcTrk->trackId();
    int charge = mdcTrk->charge();
    double r0 = mdcTrk->r();
    double z0 = mdcTrk->z();
    h_trackR0->fill(r0);
    h_trackZ0->fill(z0);

    nChargedTracks++;
    nCharge += charge;

    double pX = mdcTrk->px();
    double pY = mdcTrk->py();
    double pZ = mdcTrk->pz();
    double pMag = mdcTrk->p();
    double cosTheta = cos(mdcTrk->theta());
    double phi = mdcTrk->phi();

    double chargedEnergy = sqrt(pMag*pMag + mPi*mPi);
    totalVisibleEnergy += chargedEnergy;
    totalChargedEnergy += chargedEnergy;

    totalChargedPX += pX;    
    totalChargedPY += pY;    
    totalChargedPZ += pZ;    

    /// EMC energy associated with track
    if( (*itTrk)->isEmcShowerValid() ) {
      RecEmcShower* emcChargedTrk = (*itTrk)->emcShower();
      totalEMCEnergy += emcChargedTrk->energy();
    }

    /// MUC information
    if( (*itTrk)->isMucTrackValid() ) {
      RecMucTrack* mucTrk = (*itTrk)->mucTrack();
      double mucDepth = mucTrk->depth();
      if(mucDepth > 0) {
	h_mucDepth->fill(mucDepth);
	h_mucDepthVsCosTheta->fill(cosTheta,mucDepth);
	h_mucDepthVsPhi->fill(phi,mucDepth);
      }
    }  // End of "isMucShowerValid()" IF


    ////////////////////////
    /// Tracks after IP cuts
    if(fabs(z0) >= m_vz0cut) continue;
    if(r0 >= m_vr0cut) continue;

    nChargedTracksIP++;
    nChargeIP += charge;

    /// dE/dx information
    double dedxProbPH = -10;
    int dedxNumTotalHits = -10;
    int dedxNumGoodHits = -10;

    if( (*itTrk)->isMdcDedxValid() ) {
      RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();

      dedxNumTotalHits = dedxTrk->numTotalHits();
      dedxNumGoodHits = dedxTrk->numGoodHits();
      h_dedxTotalHitsIP->fill(dedxNumTotalHits);
      h_dedxGoodHitsIP->fill(dedxNumGoodHits);

      h_dedxElecChiIP->fill(dedxTrk->chiE());
      h_dedxMuonChiIP->fill(dedxTrk->chiMu());
      h_dedxPionChiIP->fill(dedxTrk->chiPi());
      h_dedxKaonChiIP->fill(dedxTrk->chiK());
      h_dedxProtonChiIP->fill(dedxTrk->chiP());

      dedxProbPH = dedxTrk->probPH();
      h_dedxProbPHIP->fill(dedxProbPH);
      h_dedxProbPHVsMomentumIP->fill(pMag,dedxProbPH);
      
    }  // End of "isMdcDedxValid()" IF
 
    /// TOF information
    if( (*itTrk)->isTofTrackValid() ) {
      SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();
      SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();

      for(;iter_tof != tofTrkCol.end(); iter_tof++ ) {
        TofHitStatus *status = new TofHitStatus;
        status->setStatus((*iter_tof)->status());

        if( status->is_barrel() ) {
          if( !(status->is_counter()) ) continue; // ?

	  double tofPH = (*iter_tof)->ph();
	  double tof = (*iter_tof)->tof();

	  h_tofElecIP_Barrel->fill(tof - (*iter_tof)->texpElectron());
	  h_tofMuonIP_Barrel->fill(tof - (*iter_tof)->texpMuon());
	  h_tofPionIP_Barrel->fill(tof - (*iter_tof)->texpPion());
	  h_tofKaonIP_Barrel->fill(tof - (*iter_tof)->texpKaon());
	  h_tofProtonIP_Barrel->fill(tof - (*iter_tof)->texpProton());
	  h_tofVsMomentumIP->fill(pMag,tof);
	  
	  if( status->layer() == 1 ) {
	    h_tofPHIP_BarrelLayer1->fill(tofPH);
	    h_tofIP_BarrelLayer1->fill(tof);
	  }
	  if( status->layer() == 2 ) {
	    h_tofPHIP_BarrelLayer2->fill(tofPH);
	    h_tofIP_BarrelLayer2->fill(tof);
	  }
	}  // End of TOF barrel IF

        else {
          if( !(status->is_counter()) ) continue; // ?

	  double tofPH = (*iter_tof)->ph();
	  double tof = (*iter_tof)->tof();

	  h_tofElecIP_Endcap->fill(tof - (*iter_tof)->texpElectron());
	  h_tofMuonIP_Endcap->fill(tof - (*iter_tof)->texpMuon());
	  h_tofPionIP_Endcap->fill(tof - (*iter_tof)->texpPion());
	  h_tofKaonIP_Endcap->fill(tof - (*iter_tof)->texpKaon());
	  h_tofProtonIP_Endcap->fill(tof - (*iter_tof)->texpProton());
	  h_tofVsMomentumIP->fill(pMag,tof);
	  
	  if( status->is_east() ) {
	    h_tofPHIP_EastEndcap->fill(tofPH);
	    h_tofIP_EastEndcap->fill(tof);
	  }
	  else {
	    h_tofPHIP_WestEndcap->fill(tofPH);
	    h_tofIP_WestEndcap->fill(tof);
	  }
	}  // End of TOF endcap IF

      }  // End of "iter_tof" FOR
    }  // End of "isTofTrackValid()" IF


    /// For the 2 highest momentum charged tracks
    if(pMag > highestIPTrackP) { 
      secondHighestPIPTrackId = highestPIPTrackId;
      secondHighestIPTrackP = highestIPTrackP;
      highestPIPTrackId = trackId;
      highestIPTrackP = pMag;
    }
    if((pMag > secondHighestIPTrackP)&&(pMag < highestIPTrackP)) { 
      secondHighestPIPTrackId = trackId;
      secondHighestIPTrackP = pMag;
    }

  }  // End of charged track FOR


  ///////////////////////////////////////
  /// If the event has 1 IP charged track 
  if(nChargedTracksIP > 0) {
      EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + highestPIPTrackId;
      RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
      double highestPPhi0 = mdcTrk->phi();
      h_pIPTrack1DivEb->fill(mdcTrk->p()/eBeam);

      if( (*itTrk)->isEmcShowerValid() ) {
	RecEmcShower* emcChargedTrk = (*itTrk)->emcShower();
	h_eEMCIPTrack1DivEb->fill(emcChargedTrk->energy()/eBeam);
      }

      /// If the event has 2 IP charged tracks 
      if(nChargedTracksIP > 1) {
	itTrk = evtRecTrkCol->begin() + secondHighestPIPTrackId;
	RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
	double secondHighestPPhi0 = mdcTrk->phi();
	h_pIPTrack2DivEb->fill(mdcTrk->p()/eBeam);

	if( (*itTrk)->isEmcShowerValid() ) {
	  RecEmcShower* emcChargedTrk = (*itTrk)->emcShower();
	  h_eEMCIPTrack2DivEb->fill(emcChargedTrk->energy()/eBeam);
	}

	h_acoplanarity_2HighestPIPTracks->fill(fabs(CLHEP::pi - fabs(highestPPhi0 - secondHighestPPhi0)));	
   }  // End of "nChargedTracksIP > 1" IF
  }  // End of "nChargedTracksIP > 0" IF
  

  ///////////////////////////////////////
  /// Shower (aka Neutral track) FOR loop
  int nNeutralTracks = 0, nNeutralTracksGT30MeV = 0;
  double totalNeutralEnergy = 0;
  double totalNeutralPX = 0, totalNeutralPY = 0, totalNeutralPZ = 0; 

  for(int i = evtRecEvent->totalCharged(); i< evtRecEvent->totalTracks(); i++) {
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid()) continue;
    RecEmcShower *emcTrk = (*itTrk)->emcShower();

    nNeutralTracks++;
    double eraw = emcTrk->energy();
    if(eraw > 0.030) nNeutralTracksGT30MeV++;

    totalVisibleEnergy += eraw;
    totalEMCEnergy += eraw;
    totalNeutralEnergy += eraw;

    double theta = emcTrk->theta();
    double phi = emcTrk->phi();
    
    double pX = eraw*cos(phi)*sin(theta);
    double pY = eraw*sin(phi)*sin(theta);
    double pZ = eraw*cos(theta);
    
    totalNeutralPX += pX;    
    totalNeutralPY += pY;    
    totalNeutralPZ += pZ;    

    /// Energy of most energetic showers
    if(nNeutralTracks == 1) h_eEMCShower1DivEb->fill(eraw/eBeam);
    if(nNeutralTracks == 2) h_eEMCShower2DivEb->fill(eraw/eBeam);
    if(nNeutralTracks == 3) h_eEMCShower3DivEb->fill(eraw/eBeam);

  } // End of neutral track FOR


  ///////////////////////////////
  /// Histograms filled per event

  h_nChargedTracks->fill(nChargedTracks);
  h_nChargedTracksIP->fill(nChargedTracksIP);

  h_netCharge->fill(nCharge);
  h_netChargeIP->fill(nChargeIP);

  h_nNeutralTracks->fill(nNeutralTracks);
  h_nNeutralTracksGT30MeV->fill(nNeutralTracksGT30MeV);


  /// Total Energy Histograms
  h_eVisibleDivEcm->fill(totalVisibleEnergy/m_ecm);
  h_eEMCDivEcm->fill(totalEMCEnergy/m_ecm);     
  h_eNeutralDivEcm ->fill(totalNeutralEnergy/m_ecm);
  h_eChargedDivEcm->fill(totalChargedEnergy/m_ecm);


  /// Total Charged Momentum Histograms
  double totalChargedPXY = sqrt(totalChargedPX*totalChargedPX + totalChargedPY*totalChargedPY);
  double totalChargedP = sqrt(totalChargedPXY*totalChargedPXY + totalChargedPZ*totalChargedPZ);

  h_netMomentumDivEcm_AllChargedTracks->fill(totalChargedP/m_ecm);
  h_netTransMomentumDivEcm_AllChargedTracks->fill(totalChargedPXY/m_ecm);
  if(totalChargedP > 0) {
    h_cosTheta_AllChargedTracks->fill(totalChargedPZ/totalChargedP);
  } else { 
    if(nChargedTracks > 0) {
    log << MSG::INFO << "Run " << run << ", event " << event 
	<< ": totalChargedP <= 0! " << endmsg;
    }
  }


  /// Total Neutral Momentum Histograms
  double totalNeutralPXY = sqrt(totalNeutralPX*totalNeutralPX + totalNeutralPY*totalNeutralPY);
  double totalNeutralP = sqrt(totalNeutralPXY*totalNeutralPXY + totalNeutralPZ*totalNeutralPZ);

  h_netMomentumDivEcm_AllNeutralTracks->fill(totalNeutralP/m_ecm);
  h_netTransMomentumDivEcm_AllNeutralTracks->fill(totalNeutralPXY/m_ecm);
  if(totalNeutralP > 0) {
    h_cosTheta_AllNeutralTracks->fill(totalNeutralPZ/totalNeutralP);
  } else { 
    if(nNeutralTracks > 0) {
    log << MSG::INFO << "Run " << run << ", event " << event 
	<< ": totalNeutralP <= 0! " << endmsg;
    }
  }

  /// Total Momentum Histograms
  double totalEventPX = totalChargedPX + totalNeutralPX;
  double totalEventPY = totalChargedPY + totalNeutralPY;
  double totalEventPZ = totalChargedPZ + totalNeutralPZ;

  double totalEventPXY = sqrt(totalEventPX*totalEventPX + totalEventPY*totalEventPY);
  double totalEventP = sqrt(totalEventPXY*totalEventPXY + totalEventPZ*totalEventPZ);

  h_netMomentumDivEcm_AllTracks->fill(totalEventP/m_ecm);
  h_netTransMomentumDivEcm_AllTracks->fill(totalEventPXY/m_ecm);
  if(totalEventP > 0) {
    h_cosTheta_AllTracks->fill(totalEventPZ/totalEventP);
  } else { 
    log << MSG::INFO << "Run " << run << ", event " << event 
	<< ": totalEventP <= 0! " << endmsg;
  }

  
  /// VeeVertex information
  SmartDataPtr<EvtRecVeeVertexCol> evtRecVeeVertexCol(eventSvc(), "/Event/EvtRec/EvtRecVeeVertexCol");
  if ( ! evtRecVeeVertexCol ) {
    log << MSG::FATAL << "Could not find EvtRecVeeVertexCol" << endreq;
    return StatusCode::FAILURE;
  }

  /// Loop over VeeVertex candidates
  int numKs = 0, numLambda = 0;
  for(EvtRecVeeVertexCol::iterator veeItr = evtRecVeeVertexCol->begin(); 
      veeItr < evtRecVeeVertexCol->end(); veeItr++){
     
    h_ksMass->fill((*veeItr)->mass()); 
    if(fabs((*veeItr)->mass() - mKs) < 0.010)  ++numKs;

    h_lambdaMass->fill((*veeItr)->mass()); 
    if(fabs((*veeItr)->mass() - mLambda) < 0.010)  ++numLambda;

  }  // End of "evtRecVeeVertexCol" FOR

  h_nKs->fill(numKs); 
  h_nLambda->fill(numLambda); 
  

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode FarmMonitorAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;

  return StatusCode::SUCCESS;
}

