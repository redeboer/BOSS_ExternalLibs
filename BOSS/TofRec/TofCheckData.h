#ifndef TOFCHECKDATA_H
#define TOFCHECKDATA_H
#include "EventModel/EventHeader.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "TofRecEvent/RecTofTrack.h"
#include "TofRecEvent/RecBTofCalHit.h"
#include "TofRecEvent/RecETofCalHit.h"
#include "TofRec/TofTrack.h"
#include "GaudiKernel/NTuple.h"

class TofCheckData{
 public:
  TofCheckData( NTuple::Tuple*& trk, NTuple::Tuple*& cbtrk, NTuple::Tuple*& cetrk,  NTuple::Tuple*& cetftrk );
  ~TofCheckData() {}

  void Fill( Event::EventHeader&, RecTofTrack*&, RecMdcKalTrack*&, bool );
  void FillBarrel( Event::EventHeader&, RecBTofCalHit*& );
  void Fill( Event::EventHeader&, RecETofCalHit*& );
  void FillETF( Event::EventHeader&, RecBTofCalHit*& );

  void FillCol( Event::EventHeader&, RecTofTrackCol&, RecMdcKalTrackCol& );
  void FillCol( Event::EventHeader&, RecBTofCalHitCol& );
  void FillCol( Event::EventHeader&, RecETofCalHitCol& );

 private:

  NTuple::Tuple*&      trk_tuple;
  NTuple::Item<int>    trk_run;
  NTuple::Item<int>    trk_event;
  NTuple::Item<int>    trk_toftrackid;
  NTuple::Item<int>    trk_trackid;
  NTuple::Item<int>    trk_charge;
  NTuple::Array<double> trk_p;
  NTuple::Item<int>    trk_tofid;
  NTuple::Item<int>    trk_strip;
  NTuple::Item<int>    trk_raw;
  NTuple::Item<int>    trk_readout;
  NTuple::Item<int>    trk_counter;
  NTuple::Item<int>    trk_cluster;
  NTuple::Item<int>    trk_barrel;
  NTuple::Item<int>    trk_east;
  NTuple::Item<int>    trk_layer;
  NTuple::Item<int>    trk_overflow;
  NTuple::Item<int>    trk_multihit;
  NTuple::Item<int>    trk_ncounter;
  NTuple::Item<int>    trk_neast;
  NTuple::Item<int>    trk_nwest;
  NTuple::Item<int>    trk_mrpc;
  NTuple::Item<double> trk_path;
  NTuple::Item<double> trk_zrhit;
  NTuple::Item<double> trk_ph;
  NTuple::Item<double> trk_tof;
  NTuple::Item<double> trk_errtof;
  NTuple::Item<double> trk_beta;
  NTuple::Item<double> trk_texpe;
  NTuple::Item<double> trk_texpmu;
  NTuple::Item<double> trk_texppi;
  NTuple::Item<double> trk_texpk;
  NTuple::Item<double> trk_texpp;
  NTuple::Item<double> trk_offe;
  NTuple::Item<double> trk_offmu;
  NTuple::Item<double> trk_offpi;
  NTuple::Item<double> trk_offk;
  NTuple::Item<double> trk_offp;
  NTuple::Item<double> trk_sige;
  NTuple::Item<double> trk_sigmu;
  NTuple::Item<double> trk_sigpi;
  NTuple::Item<double> trk_sigk;
  NTuple::Item<double> trk_sigp;
  NTuple::Item<int>    trk_quality;
  NTuple::Item<double> trk_t0;
  NTuple::Item<double> trk_errt0;
  NTuple::Item<double> trk_errz;
  NTuple::Item<double> trk_phi;
  NTuple::Item<double> trk_errphi;
  NTuple::Item<double> trk_energy;
  NTuple::Item<double> trk_errenergy;

  NTuple::Tuple*&      cbtrk_tuple;
  NTuple::Item<int>    cbtrk_run;
  NTuple::Item<int>    cbtrk_event;
  NTuple::Item<int>    cbtrk_tofid;
  NTuple::Item<int>    cbtrk_qual;
  NTuple::Item<double> cbtrk_texp;
  NTuple::Item<double> cbtrk_tdc1;
  NTuple::Item<double> cbtrk_tdc2;
  NTuple::Item<double> cbtrk_adc1;
  NTuple::Item<double> cbtrk_adc2;
  NTuple::Item<double> cbtrk_zhit;
  NTuple::Item<double> cbtrk_dzhit;
  NTuple::Item<double> cbtrk_sintheta;
  NTuple::Item<double> cbtrk_p;
  NTuple::Item<double> cbtrk_q;
  NTuple::Item<double> cbtrk_path;

  NTuple::Tuple*&      cetrk_tuple;
  NTuple::Item<int>    cetrk_run;
  NTuple::Item<int>    cetrk_event;
  NTuple::Item<int>    cetrk_tofid;
  NTuple::Item<int>    cetrk_qual;
  NTuple::Item<double> cetrk_texp;
  NTuple::Item<double> cetrk_tdc;
  NTuple::Item<double> cetrk_adc;
  NTuple::Item<double> cetrk_rhit;
  NTuple::Item<double> cetrk_drhit;
  NTuple::Item<double> cetrk_p;
  NTuple::Item<double> cetrk_q;
  NTuple::Item<double> cetrk_path;

  NTuple::Tuple*&      cetftrk_tuple;
  NTuple::Item<int>    cetftrk_run;
  NTuple::Item<int>    cetftrk_event;
  NTuple::Item<int>    cetftrk_tofid;
  NTuple::Item<int>    cetftrk_qual;
  NTuple::Item<double> cetftrk_texp;
  NTuple::Item<double> cetftrk_tdc1;
  NTuple::Item<double> cetftrk_tdc2;
  NTuple::Item<double> cetftrk_adc1;
  NTuple::Item<double> cetftrk_adc2;
  NTuple::Item<double> cetftrk_zhit;
  NTuple::Item<double> cetftrk_dzhit;
  NTuple::Item<int>    cetftrk_sintheta;
  NTuple::Item<double> cetftrk_p;
  NTuple::Item<double> cetftrk_q;
  NTuple::Item<double> cetftrk_path;

};
#endif
