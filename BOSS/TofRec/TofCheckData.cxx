#include "TofRec/TofCheckData.h"
#include "DstEvent/TofHitStatus.h"
#include <iostream>

TofCheckData::TofCheckData( NTuple::Tuple*& trk, NTuple::Tuple*& cbtrk, NTuple::Tuple*& cetrk, NTuple::Tuple*& cetftrk ):trk_tuple(trk),cbtrk_tuple(cbtrk),cetrk_tuple(cetrk),cetftrk_tuple(cetftrk) {

  if(!trk_tuple) {
    std::cerr<<" Error: invalid pionter in TofCheckData(NTuple::Tuple*) for trk_tple"<<std::endl;
  }
  else{
    trk_tuple->addItem("run",        trk_run        );
    trk_tuple->addItem("event",      trk_event      );
    trk_tuple->addItem("toftrackid", trk_toftrackid );
    trk_tuple->addItem("trackid",    trk_trackid    );
    trk_tuple->addItem("charge",     trk_charge     );
    trk_tuple->addItem("p",       5, trk_p          );
    trk_tuple->addItem("tofid",      trk_tofid      );
    trk_tuple->addItem("strip",      trk_strip      );
    trk_tuple->addItem("raw",        trk_raw        );
    trk_tuple->addItem("readout",    trk_readout    );
    trk_tuple->addItem("counter",    trk_counter    );
    trk_tuple->addItem("cluster",    trk_cluster    );
    trk_tuple->addItem("barrel",     trk_barrel     );
    trk_tuple->addItem("east",       trk_east       );
    trk_tuple->addItem("layer",      trk_layer      );
    trk_tuple->addItem("over",       trk_overflow   );
    trk_tuple->addItem("cluster",    trk_cluster    );
    trk_tuple->addItem("multihit",   trk_multihit   );
    trk_tuple->addItem("ncounter",   trk_ncounter   );
    trk_tuple->addItem("neast",      trk_neast      );
    trk_tuple->addItem("nwest",      trk_nwest      );
    trk_tuple->addItem("mrpc",       trk_mrpc       );
    trk_tuple->addItem("path",       trk_path       );
    trk_tuple->addItem("zrhit",      trk_zrhit      );
    trk_tuple->addItem("ph",         trk_ph         );
    trk_tuple->addItem("tof",        trk_tof        );
    trk_tuple->addItem("errtof",     trk_errtof     );
    trk_tuple->addItem("beta",       trk_beta       );
    trk_tuple->addItem("texpe",      trk_texpe      );
    trk_tuple->addItem("texpmu",     trk_texpmu     );
    trk_tuple->addItem("texppi",     trk_texppi     );
    trk_tuple->addItem("texpk",      trk_texpk      );
    trk_tuple->addItem("texpp",      trk_texpp      );
    trk_tuple->addItem("offe",       trk_offe       );
    trk_tuple->addItem("offmu",      trk_offmu      );
    trk_tuple->addItem("offpi",      trk_offpi      );
    trk_tuple->addItem("offk",       trk_offk       );
    trk_tuple->addItem("offp",       trk_offp       );
    trk_tuple->addItem("sige",       trk_sige       );
    trk_tuple->addItem("sigmu",      trk_sigmu      );
    trk_tuple->addItem("sigpi",      trk_sigpi      );
    trk_tuple->addItem("sigk",       trk_sigk       );
    trk_tuple->addItem("sigp",       trk_sigp       );
    trk_tuple->addItem("qual",       trk_quality    );
    trk_tuple->addItem("t0",         trk_t0         );
    trk_tuple->addItem("errt0",      trk_errt0      );
    trk_tuple->addItem("errz",       trk_errz       );
    trk_tuple->addItem("phi",        trk_phi        );
    trk_tuple->addItem("errphi",     trk_errphi     );
    trk_tuple->addItem("e",          trk_energy     );
    trk_tuple->addItem("erre",       trk_errenergy  );
  }

  if(!cbtrk_tuple) {
    std::cerr<<" Error: invalid pionter in TofCheckData(NTuple::Tuple*) for cbtrk_tple"<<std::endl;
  }
  else{
    cbtrk_tuple->addItem("run",        cbtrk_run        );
    cbtrk_tuple->addItem("event",      cbtrk_event      );
    cbtrk_tuple->addItem("tofid",      cbtrk_tofid      );
    cbtrk_tuple->addItem("qual",       cbtrk_qual       );
    cbtrk_tuple->addItem("texp",       cbtrk_texp       );
    cbtrk_tuple->addItem("tdc1",       cbtrk_tdc1       );
    cbtrk_tuple->addItem("tdc2",       cbtrk_tdc2       );
    cbtrk_tuple->addItem("adc1",       cbtrk_adc1       );
    cbtrk_tuple->addItem("adc2",       cbtrk_adc2       );
    cbtrk_tuple->addItem("zhit",       cbtrk_zhit       );
    cbtrk_tuple->addItem("dzhit",      cbtrk_dzhit      );
    cbtrk_tuple->addItem("sint",       cbtrk_sintheta   );
    cbtrk_tuple->addItem("p",          cbtrk_p          );
    cbtrk_tuple->addItem("q",          cbtrk_q          );
    cbtrk_tuple->addItem("path",       cbtrk_path       );
  }

  if(!cetrk_tuple) {
    std::cerr<<" Error: invalid pionter in TofCheckData(NTuple::Tuple*) for cetrk_tple"<<std::endl;
  }
  else{
    cetrk_tuple->addItem("run",        cetrk_run        );
    cetrk_tuple->addItem("event",      cetrk_event      );
    cetrk_tuple->addItem("tofid",      cetrk_tofid      );
    cetrk_tuple->addItem("qual",       cetrk_qual       );
    cetrk_tuple->addItem("texp",       cetrk_texp       );
    cetrk_tuple->addItem("tdc",        cetrk_tdc        );
    cetrk_tuple->addItem("adc",        cetrk_adc        );
    cetrk_tuple->addItem("rhit",       cetrk_rhit       );
    cetrk_tuple->addItem("drhit",      cetrk_drhit      );
    //    cetrk_tuple->addItem("sint",       cetrk_sintheta   );
    cetrk_tuple->addItem("p",          cetrk_p          );
    cetrk_tuple->addItem("q",          cetrk_q          );
    cetrk_tuple->addItem("path",       cetrk_path       );
  }

  if(!cetftrk_tuple) {
    std::cerr<<" Error: invalid pionter in TofCheckData(NTuple::Tuple*) for cetftrk_tple"<<std::endl;
  }
  else{
    cetftrk_tuple->addItem("run",        cetftrk_run        );
    cetftrk_tuple->addItem("event",      cetftrk_event      );
    cetftrk_tuple->addItem("tofid",      cetftrk_tofid      );
    cetftrk_tuple->addItem("qual",       cetftrk_qual       );
    cetftrk_tuple->addItem("texp",       cetftrk_texp       );
    cetftrk_tuple->addItem("tdc1",       cetftrk_tdc1       );
    cetftrk_tuple->addItem("tdc2",       cetftrk_tdc2       );
    cetftrk_tuple->addItem("adc1",       cetftrk_adc1       );
    cetftrk_tuple->addItem("adc2",       cetftrk_adc2       );
    cetftrk_tuple->addItem("zhit",       cetftrk_zhit       );
    cetftrk_tuple->addItem("dzhit",      cetftrk_dzhit      );
    cetftrk_tuple->addItem("sint",       cetftrk_sintheta   );
    cetftrk_tuple->addItem("p",          cetftrk_p          );
    cetftrk_tuple->addItem("q",          cetftrk_q          );
    cetftrk_tuple->addItem("path",       cetftrk_path       );
  }
}


void TofCheckData::Fill(Event::EventHeader& eventHeader, RecTofTrack*& recTof, RecMdcKalTrack*& kalTrack, bool fillTrk ) {

  int charge = 0;
  double p[5]= {0.};
  if( fillTrk ) {
    kalTrack->setPidType( RecMdcKalTrack::electron );
    double kappa = kalTrack->kappa();
    if( kappa>0 ) { charge = 1; }
    else { charge = -1; }
    if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
    p[0]  = abs(sqrt( 1.0 + kalTrack->tanl()*kalTrack->tanl() ) / kappa);
    kalTrack->setPidType( RecMdcKalTrack::muon );
    kappa = kalTrack->kappa();
    if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
    p[1]  = abs(sqrt( 1.0 + kalTrack->tanl()*kalTrack->tanl() ) / kappa );
    kalTrack->setPidType( RecMdcKalTrack::pion );
    kappa = kalTrack->kappa();
    if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
    p[2]  = abs(sqrt( 1.0 + kalTrack->tanl()*kalTrack->tanl() ) / kappa );
    kalTrack->setPidType( RecMdcKalTrack::kaon );
    kappa = kalTrack->kappa();
    if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
    p[3]  = abs(sqrt( 1.0 + kalTrack->tanl()*kalTrack->tanl() ) / kappa );
    kalTrack->setPidType( RecMdcKalTrack::proton );
    kappa = kalTrack->kappa();
    if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
    p[4]  = abs(sqrt( 1.0 + kalTrack->tanl()*kalTrack->tanl() ) / kappa );
  }
  trk_run     = eventHeader.runNumber();
  trk_event   = eventHeader.eventNumber();
  trk_toftrackid = recTof->tofTrackID();
  trk_trackid    = recTof->trackID();
  trk_charge     = charge;
  for( unsigned int i=0; i<5; i++ ) {
    trk_p[i]    = p[i];
  }
  trk_tofid      = recTof->tofID();
  trk_strip      = recTof->strip();
  TofHitStatus* hitStatus = new TofHitStatus;
  hitStatus->setStatus( recTof->status() );
  trk_raw       = hitStatus->is_raw();
  trk_readout   = hitStatus->is_readout();
  trk_counter   = hitStatus->is_counter();
  trk_cluster   = hitStatus->is_cluster();
  trk_barrel    = hitStatus->is_barrel();
  trk_east      = hitStatus->is_east();
  trk_layer     = hitStatus->layer();
  trk_overflow  = hitStatus->is_overflow();
  trk_multihit  = hitStatus->is_multihit();
  trk_ncounter  = hitStatus->ncounter();
  trk_neast     = hitStatus->neast();
  trk_nwest     = hitStatus->nwest();
  trk_mrpc      = hitStatus->is_mrpc();
  delete hitStatus;
  /*
  if( hitStatus->is_mrpc() ) {
    if( recTof->tofID() >= 0 ) {
      trk_tofid      = static_cast<int>(recTof->tofID()/12);
      trk_strip      = static_cast<int>(recTof->tofID()%12);
    }
  }
  */
  trk_path    = recTof->path();
  trk_zrhit   = recTof->zrhit();
  trk_ph      = recTof->ph();
  trk_tof     = recTof->tof();
  trk_errtof  = recTof->errtof();
  trk_beta    = recTof->beta();
  trk_texpe   = recTof->texpElectron();
  trk_texpmu  = recTof->texpMuon();
  trk_texppi  = recTof->texpPion();
  trk_texpk   = recTof->texpKaon();
  trk_texpp   = recTof->texpProton();
  trk_offe    = recTof->toffsetElectron();
  trk_offmu   = recTof->toffsetMuon();
  trk_offpi   = recTof->toffsetPion();
  trk_offk    = recTof->toffsetKaon();
  trk_offp    = recTof->toffsetProton();
  trk_sige    = recTof->sigmaElectron();
  trk_sigmu   = recTof->sigmaMuon();
  trk_sigpi   = recTof->sigmaPion();
  trk_sigk    = recTof->sigmaKaon();
  trk_sigp    = recTof->sigmaProton();
  trk_quality = recTof->quality();
  trk_t0      = recTof->t0();
  trk_errt0   = recTof->errt0();
  trk_errz    = recTof->errz();
  trk_phi     = recTof->phi();
  trk_errphi  = recTof->errphi();
  trk_energy  = recTof->energy();
  trk_errenergy = recTof->errenergy();
  trk_tuple->write();
  return;
}


void TofCheckData::FillBarrel(Event::EventHeader& eventHeader, RecBTofCalHit*& recBTof ) {

  cbtrk_run     = eventHeader.runNumber();
  cbtrk_event   = eventHeader.eventNumber();
  cbtrk_tofid   = recBTof->mod();
  cbtrk_qual    = recBTof->qual();
  cbtrk_texp    = recBTof->tpred(1);
  cbtrk_tdc1    = recBTof->tdc1();
  cbtrk_tdc2    = recBTof->tdc2();
  cbtrk_adc1    = recBTof->adc1();
  cbtrk_adc2    = recBTof->adc2();
  cbtrk_zhit    = recBTof->zHit();
  cbtrk_dzhit   = recBTof->dzHit();
  cbtrk_sintheta = recBTof->sinTheta();
  cbtrk_p       = recBTof->p();
  cbtrk_q       = recBTof->Q();
  cbtrk_path    = recBTof->path();
  cbtrk_tuple->write();
  return;
}


void TofCheckData::Fill(Event::EventHeader& eventHeader, RecETofCalHit*& recETof ) {

  cetrk_run     = eventHeader.runNumber();
  cetrk_event   = eventHeader.eventNumber();
  cetrk_tofid   = recETof->mod();
  cetrk_qual    = recETof->qual();
  cetrk_texp    = recETof->tpred(1);
  cetrk_tdc     = recETof->tdc();
  cetrk_adc     = recETof->adc();
  cetrk_rhit    = recETof->rHit();
  cetrk_drhit   = recETof->drHit();
  //  cetrk_sintheta = recETof->sinTheta();
  cetrk_p       = recETof->p();
  cetrk_q       = recETof->Q();
  cetrk_path    = recETof->path();
  cetrk_tuple->write();
  return;
}

void TofCheckData::FillETF(Event::EventHeader& eventHeader, RecBTofCalHit*& recBTof ) {

  cetftrk_run     = eventHeader.runNumber();
  cetftrk_event   = eventHeader.eventNumber();
  cetftrk_tofid   = recBTof->mod();
  cetftrk_qual    = recBTof->qual();
  cetftrk_texp    = recBTof->tpred(1);
  cetftrk_tdc1    = recBTof->tdc1();
  cetftrk_tdc2    = recBTof->tdc2();
  cetftrk_adc1    = recBTof->adc1();
  cetftrk_adc2    = recBTof->adc2();
  cetftrk_zhit    = recBTof->zHit();
  cetftrk_dzhit   = recBTof->dzHit();
  cetftrk_sintheta = recBTof->sinTheta();
  cetftrk_p       = recBTof->p();
  cetftrk_q       = recBTof->Q();
  cetftrk_path    = recBTof->path();
  cetftrk_tuple->write();
  return;
}


void TofCheckData::FillCol(Event::EventHeader& eventHeader, RecTofTrackCol& recTofTrackCol, RecMdcKalTrackCol& kalTrackCol) {
  RecTofTrackCol::iterator iter_tof = recTofTrackCol.begin();
  for( ; iter_tof!=recTofTrackCol.end(); iter_tof++ ) {
    RecMdcKalTrackCol::iterator iter_kal = kalTrackCol.begin();
    for( ; iter_kal!=kalTrackCol.end(); iter_kal++ ) {
      if( (*iter_tof)->trackID() == (*iter_kal)->trackId() ) break;
    }
    Fill( eventHeader, *iter_tof, *iter_kal, iter_kal!=kalTrackCol.end() );
  }
  return;
}

void TofCheckData::FillCol(Event::EventHeader& eventHeader, RecBTofCalHitCol& recBTofCalHitCol) {
  RecBTofCalHitCol::iterator iter_btof = recBTofCalHitCol.begin();
  for( ; iter_btof!=recBTofCalHitCol.end(); iter_btof++ ) {
    if( fabs( (*iter_btof)->dzHit() - 1.0 )<1.0e-6 ) {
      FillBarrel( eventHeader, *iter_btof );
    }
    else {
      FillETF( eventHeader, *iter_btof );
    }
  }
  return;
}

void TofCheckData::FillCol(Event::EventHeader& eventHeader, RecETofCalHitCol& recETofCalHitCol) {
  RecETofCalHitCol::iterator iter_etof = recETofCalHitCol.begin();
  for( ; iter_etof!=recETofCalHitCol.end(); iter_etof++ ) {
    Fill( eventHeader, *iter_etof );
  }
  return;
}
