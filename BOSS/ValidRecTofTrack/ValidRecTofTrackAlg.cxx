#include "ValidRecTofTrack/ValidRecTofTrackAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "TofRecEvent/RecTofTrack.h"
#include "DstEvent/TofHitStatus.h"


ValidRecTofTrackAlg::ValidRecTofTrackAlg(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator) { }

StatusCode ValidRecTofTrackAlg::initialize(){
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "ValidRecTofTrackAlg initialize !" << endreq;

  NTuplePtr nt1(ntupleSvc(),"FILE201/tof");
  if ( nt1 ) m_tuple_tof = nt1;
  else {
    m_tuple_tof=ntupleSvc()->book("FILE201/tof",CLID_ColumnWiseTuple,"Validation of TOF" );
    if( m_tuple_tof ) {
      m_tuple_tof->addItem("run",     m_run        );
      m_tuple_tof->addItem("evt",     m_event      );
      m_tuple_tof->addItem("toftrk",  m_tofTrackID );
      m_tuple_tof->addItem("trk",     m_trackID    );
      m_tuple_tof->addItem("tofid",   m_tofID      );
      m_tuple_tof->addItem("status",  m_status     );
      m_tuple_tof->addItem("raw",     m_raw        );
      m_tuple_tof->addItem("readout", m_readout    );
      m_tuple_tof->addItem("counter", m_counter    );
      m_tuple_tof->addItem("cluster", m_cluster    );
      m_tuple_tof->addItem("barrel",  m_barrel     );
      m_tuple_tof->addItem("east",    m_east       );
      m_tuple_tof->addItem("layer",   m_layer      );
      m_tuple_tof->addItem("ncounter",m_ncounter   );
      m_tuple_tof->addItem("neast",   m_neast      );
      m_tuple_tof->addItem("nwest",   m_nwest      );
      m_tuple_tof->addItem("path",    m_path       );
      m_tuple_tof->addItem("zrhit",   m_zrhit      );
      m_tuple_tof->addItem("ph",      m_ph         );
      m_tuple_tof->addItem("tof",     m_tof        );
      m_tuple_tof->addItem("etof",    m_errtof     );
      m_tuple_tof->addItem("beta",    m_beta       );
      m_tuple_tof->addItem("texpe",   m_texpe      );
      m_tuple_tof->addItem("texpmu",  m_texpmu     );
      m_tuple_tof->addItem("texppi",  m_texppi     );
      m_tuple_tof->addItem("texpk",   m_texpk      );
      m_tuple_tof->addItem("texpp",   m_texpp      );
      m_tuple_tof->addItem("toffe",   m_toffe      );
      m_tuple_tof->addItem("toffmu",  m_toffmu     );
      m_tuple_tof->addItem("toffpi",  m_toffpi     );
      m_tuple_tof->addItem("toffk",   m_toffk      );
      m_tuple_tof->addItem("toffp",   m_toffp      );
      m_tuple_tof->addItem("toffpb",  m_toffpb     );
      m_tuple_tof->addItem("sige",    m_sigmae     );
      m_tuple_tof->addItem("sigmu",   m_sigmamu    );
      m_tuple_tof->addItem("sigpi",   m_sigmapi    );
      m_tuple_tof->addItem("sigk",    m_sigmak     );
      m_tuple_tof->addItem("sigp",    m_sigmap     );
      m_tuple_tof->addItem("sigpb",   m_sigmapb    );
      m_tuple_tof->addItem("qual",    m_quality    );
      m_tuple_tof->addItem("t0",      m_t0         );
      m_tuple_tof->addItem("et0",     m_errt0      );
      m_tuple_tof->addItem("ez",      m_errz       );
      m_tuple_tof->addItem("phi",     m_phi        );
      m_tuple_tof->addItem("ephi",    m_errphi     );
      m_tuple_tof->addItem("e",       m_energy     );
      m_tuple_tof->addItem("ee",      m_errenergy  );
    }
  }

  log << MSG::INFO << "Finish ValidRecTofTrackAlg initialize!" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode ValidRecTofTrackAlg::execute() {
   MsgStream log(msgSvc(), name());
   SmartDataPtr<RecTofTrackCol> tofTracks(eventSvc(),"/Event/Recon/RecTofTrackCol");
   if( ! tofTracks ) {
         log << MSG::ERROR << "Unable to retrieve RecTofTrackCol" << endreq;
         return  StatusCode::FAILURE;
   } 
   else {
     log << MSG::DEBUG << "RecTofTrackCol retrieved of size "<< tofTracks->size() << endreq;
     for(RecTofTrackCol::iterator it=tofTracks->begin(); it!=tofTracks->end(); it++) {
       m_run   = 0.0;
       m_event = 0.0;
       m_tofTrackID = (*it)->tofTrackID();
       m_trackID    = (*it)->trackID();
       m_tofID      = (*it)->tofID();
       unsigned int status = (*it)->status();
       TofHitStatus* tofhits = new TofHitStatus;
       tofhits->setStatus( status );
       m_status     = (*it)->status();
       m_raw        = tofhits->is_raw();
       m_readout    = tofhits->is_readout();
       m_counter    = tofhits->is_counter();
       m_cluster    = tofhits->is_cluster();
       m_barrel     = tofhits->is_barrel();
       m_east       = tofhits->is_east();
       m_layer      = tofhits->layer();
       m_ncounter   = tofhits->ncounter();
       m_neast      = tofhits->neast();
       m_nwest      = tofhits->nwest();
       delete tofhits;
       m_path       = (*it)->path();
       m_zrhit      = (*it)->zrhit();
       m_ph         = (*it)->ph();
       m_tof        = (*it)->tof();
       m_errtof     = (*it)->errtof();
       m_beta       = (*it)->beta();
       m_texpe      = (*it)->texpElectron();
       m_texpmu     = (*it)->texpMuon();
       m_texppi     = (*it)->texpPion();
       m_texpk      = (*it)->texpKaon();
       m_texpp      = (*it)->texpProton();
       m_toffe      = (*it)->toffsetElectron();
       m_toffmu     = (*it)->toffsetMuon();
       m_toffpi     = (*it)->toffsetPion();
       m_toffk      = (*it)->toffsetKaon();
       m_toffp      = (*it)->toffsetProton();
       m_toffpb     = (*it)->toffsetAntiProton();
       m_sigmae    = (*it)->sigmaElectron();
       m_sigmamu   = (*it)->sigmaMuon();
       m_sigmapi   = (*it)->sigmaPion();
       m_sigmak    = (*it)->sigmaKaon();
       m_sigmap    = (*it)->sigmaProton();
       m_sigmapb   = (*it)->sigmaAntiProton();
       m_quality    = (*it)->quality();
       m_t0         = (*it)->t0();
       m_errt0      = (*it)->errt0();
       m_errz       = (*it)->errz();
       m_phi        = (*it)->phi();
       m_errphi     = (*it)->errphi();
       m_energy     = (*it)->energy();
       m_errenergy     = (*it)->errenergy();

       m_tuple_tof->write();
     }
   }
   return StatusCode::SUCCESS;
}


StatusCode ValidRecTofTrackAlg::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalizing..." << endreq;
  return StatusCode::SUCCESS;
}
