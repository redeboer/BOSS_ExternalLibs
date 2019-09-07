#ifndef TOFCHECKDIGI_H
#define TOFCHECKDIGI_H
#include "EventModel/EventHeader.h"
#include "ExtEvent/RecExtTrack.h"
#include "TofRawEvent/TofDigi.h"
#include "RawDataProviderSvc/TofData.h"
#include "TofRec/TofTrack.h"
#include "GaudiKernel/NTuple.h"
#include "TrigEvent/TrigData.h"

#include "EventModel/EventModel.h"
#include "McTruth/McEvent.h"
#include "McTruth/TofMcHit.h"
#include "McTruth/McParticle.h"

#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "EmcRecEventModel/RecEmcShower.h"

using namespace Event;
class TofCheckDigi{
 public:
  TofCheckDigi( NTuple::Tuple*& digi, NTuple::Tuple*& barrel, NTuple::Tuple*& endcap, NTuple::Tuple*& mrpc, NTuple::Tuple*& ext, NTuple::Tuple*& tof, NTuple::Tuple*& bhabha );
  ~TofCheckDigi() {}

  void Fill_Barrel( Event::EventHeader&, TofData*&, double, int );
  void Fill_Endcap( Event::EventHeader&, TofData*&, double, int );
  void Fill_MRPC( Event::EventHeader&, TofData*&, double, int );
  void Fill( Event::EventHeader&, RecMdcTrack*&, RecMdcKalTrack*&, RecExtTrack*& );
  void Fill_TofTrack( Event::EventHeader&, TofTrack*&, double, int );
  void Fill_TofTrack( Event::EventHeader&, TofTrack*&, double, int, TrigData& );
  void Fill_TofTrack( Event::EventHeader&, TofTrack*&, double, int, RecMdcKalTrackCol& );
  void Fill_TofTrack( Event::EventHeader&, TofTrack*&, double, int, RecMdcKalTrackCol&, TofMcHitCol&, McParticleCol&, std::string );

  void FillCol( Event::EventHeader&, TofDigiCol&, double, int );
  void FillCol( Event::EventHeader&, TofDataMap&, double, int );
  void FillCol( Event::EventHeader&, TofDataVector&, double, int );
  void FillCol( Event::EventHeader&, RecMdcTrackCol&, RecMdcKalTrackCol&, RecExtTrackCol& );
  void FillCol( Event::EventHeader&, RecExtTrackCol&, RecMdcTrackCol&, RecEmcShowerCol&, RecMdcKalTrackCol& );

 private:
  NTuple::Tuple*&      digi_tuple;
  NTuple::Item<int>    digi_run;
  NTuple::Item<int>    digi_event;
  NTuple::Item<int>    digi_barrel;
  NTuple::Item<int>    digi_endcap;
  NTuple::Item<int>    digi_layer;
  NTuple::Item<int>    digi_tofId;
  NTuple::Item<int>    digi_strip;
  NTuple::Item<int>    digi_end;
  NTuple::Item<double> digi_adc;
  NTuple::Item<double> digi_tdc;
  NTuple::Item<int>    digi_qclock;
  NTuple::Item<int>    digi_tclock;
  NTuple::Item<int>    digi_overflow;
  NTuple::Item<int>    digi_noq;
  NTuple::Item<int>    digi_not;
  NTuple::Item<int>    digi_multiq;
  NTuple::Item<int>    digi_multit;
  NTuple::Item<int>    digi_overq;
  NTuple::Item<int>    digi_overt;
  NTuple::Item<double> digi_t0;
  NTuple::Item<int>    digi_t0stat;
  NTuple::Item<int>    digi_crate;
  NTuple::Item<int>    digi_geo;
  NTuple::Item<int>    digi_tdcnum;
  NTuple::Item<double> digi_err;

  NTuple::Tuple*&      barrel_tuple;
  NTuple::Item<int>    barrel_run;
  NTuple::Item<int>    barrel_event;
  NTuple::Item<int>    barrel_tofId;
  NTuple::Item<int>    barrel_tofTrackId;
  NTuple::Item<int>    barrel_layer;
  NTuple::Item<double> barrel_qchannel1;
  NTuple::Item<double> barrel_tchannel1;
  NTuple::Item<double> barrel_qchannel2;
  NTuple::Item<double> barrel_tchannel2;
  NTuple::Item<double> barrel_qtc1;
  NTuple::Item<double> barrel_adc1;
  NTuple::Item<double> barrel_tdc1;
  NTuple::Item<double> barrel_qtc2;
  NTuple::Item<double> barrel_adc2;
  NTuple::Item<double> barrel_tdc2;
  NTuple::Item<int>    barrel_qclock1;
  NTuple::Item<int>    barrel_tclock1;
  NTuple::Item<int>    barrel_qclock2;
  NTuple::Item<int>    barrel_tclock2;
  NTuple::Item<int>    barrel_quality;
  NTuple::Item<int>    barrel_tmatched;
  NTuple::Item<int>    barrel_qtimes1;
  NTuple::Item<int>    barrel_ttimes1;
  NTuple::Item<int>    barrel_qtimes2;
  NTuple::Item<int>    barrel_ttimes2;
  NTuple::Item<int>    barrel_times1;
  NTuple::Item<int>    barrel_times2;
  NTuple::Item<int>    barrel_times;
  NTuple::Item<int>    barrel_qnum1;
  NTuple::Item<int>    barrel_tnum1;
  NTuple::Item<int>    barrel_qnum2;
  NTuple::Item<int>    barrel_tnum2;
  NTuple::Item<double> barrel_tsum;
  NTuple::Item<double> barrel_tsub;
  NTuple::Item<double> barrel_ztdc;
  NTuple::Item<double> barrel_zadc;
  NTuple::Item<double> barrel_t0;
  NTuple::Item<int>    barrel_t0stat;

  NTuple::Tuple*&      endcap_tuple;
  NTuple::Item<int>    endcap_run;
  NTuple::Item<int>    endcap_event;
  NTuple::Item<int>    endcap_tofId;
  NTuple::Item<int>    endcap_tofTrackId;
  NTuple::Item<double> endcap_qchannel;
  NTuple::Item<double> endcap_tchannel;
  NTuple::Item<double> endcap_qtc;
  NTuple::Item<double> endcap_adc;
  NTuple::Item<double> endcap_tdc;
  NTuple::Item<int>    endcap_quality;
  NTuple::Item<int>    endcap_qclock;
  NTuple::Item<int>    endcap_tclock;
  NTuple::Item<int>    endcap_qtimes;
  NTuple::Item<int>    endcap_ttimes;
  NTuple::Item<int>    endcap_times;
  NTuple::Item<int>    endcap_qnum;
  NTuple::Item<int>    endcap_tnum;
  NTuple::Item<double> endcap_t0;
  NTuple::Item<int>    endcap_t0stat;

  NTuple::Tuple*&      mrpc_tuple;
  NTuple::Item<int>    mrpc_run;
  NTuple::Item<int>    mrpc_event;
  NTuple::Item<int>    mrpc_tofId;
  NTuple::Item<int>    mrpc_strip;
  NTuple::Item<int>    mrpc_tofTrackId;
  NTuple::Item<double> mrpc_ttch1;
  NTuple::Item<double> mrpc_tlch1;
  NTuple::Item<double> mrpc_ttch2;
  NTuple::Item<double> mrpc_tlch2;
  NTuple::Item<double> mrpc_tt1;
  NTuple::Item<double> mrpc_tl1;
  NTuple::Item<double> mrpc_tt2;
  NTuple::Item<double> mrpc_tl2;
  NTuple::Item<double> mrpc_tot1;
  NTuple::Item<double> mrpc_tot2;
  NTuple::Item<int>    mrpc_quality;
  NTuple::Item<int>    mrpc_tmatched;
  NTuple::Item<int>    mrpc_tttimes1;
  NTuple::Item<int>    mrpc_tltimes1;
  NTuple::Item<int>    mrpc_tttimes2;
  NTuple::Item<int>    mrpc_tltimes2;
  NTuple::Item<int>    mrpc_times1;
  NTuple::Item<int>    mrpc_times2;
  NTuple::Item<int>    mrpc_times;
  NTuple::Item<int>    mrpc_ttnum1;
  NTuple::Item<int>    mrpc_tlnum1;
  NTuple::Item<int>    mrpc_ttnum2;
  NTuple::Item<int>    mrpc_tlnum2;
  NTuple::Item<double> mrpc_tsum;
  NTuple::Item<double> mrpc_tsub;
  NTuple::Item<double> mrpc_ztdc;
  NTuple::Item<double> mrpc_zadc;
  NTuple::Item<double> mrpc_t0;
  NTuple::Item<int>    mrpc_t0stat;

  NTuple::Tuple*&       ext_tuple;
  NTuple::Item<int>     ext_run;
  NTuple::Item<int>     ext_event;
  NTuple::Item<int>     ext_trackid;
  NTuple::Item<double>  ext_costheta;
  NTuple::Array<int>    ext_kalman;
  NTuple::Array<double> ext_x1;
  NTuple::Array<double> ext_x2;
  NTuple::Array<double> ext_y1;
  NTuple::Array<double> ext_y2;
  NTuple::Array<double> ext_z1;
  NTuple::Array<double> ext_z2;
  NTuple::Array<int>    ext_tofid1;
  NTuple::Array<int>    ext_tofid2;
  NTuple::Array<int>    ext_strip1;
  NTuple::Array<int>    ext_strip2;
  NTuple::Item<double>  ext_p;
  NTuple::Array<double> ext_texp1;
  NTuple::Array<double> ext_texp2;
  NTuple::Array<double> ext_path1;
  NTuple::Array<double> ext_path2;
  NTuple::Array<double> ext_zrhit1;
  NTuple::Array<double> ext_zrhit2;
  NTuple::Array<double> ext_errzr1;
  NTuple::Array<double> ext_errzr2;
  NTuple::Array<double> ext_theta1;
  NTuple::Array<double> ext_theta2;
  NTuple::Array<double> ext_phi1;
  NTuple::Array<double> ext_phi2;
  NTuple::Array<int>    ext_hitcase1;
  NTuple::Array<int>    ext_hitcase2;
  NTuple::Array<int>    ext_hitcase;
  NTuple::Array<int>    ext_quality;

  NTuple::Tuple*&      tof_tuple;
  NTuple::Item<int>    tof_run;
  NTuple::Item<int>    tof_event;
  NTuple::Item<int>    tof_toftrackid;
  NTuple::Item<int>    tof_trackid;
  NTuple::Item<int>    tof_charge;
  NTuple::Array<double> tof_pt;
  NTuple::Array<double> tof_p;
  NTuple::Item<int>    tof_id1;
  NTuple::Item<int>    tof_id2;
  NTuple::Item<int>    tof_istrip1;
  NTuple::Item<int>    tof_istrip2;
  NTuple::Item<int>    tof_dstrip1;
  NTuple::Item<int>    tof_dstrip2;
  NTuple::Item<int>    tof_barrel;
  NTuple::Item<int>    tof_hitcase;
  NTuple::Item<double> tof_momentum;
  NTuple::Item<double> tof_path1;
  NTuple::Item<double> tof_path2;
  NTuple::Item<double> tof_zrhit1;
  NTuple::Item<double> tof_zrhit2;
  NTuple::Item<double> tof_errzr1;
  NTuple::Item<double> tof_errzr2;
  NTuple::Item<double> tof_xhit1;
  NTuple::Item<double> tof_yhit1;
  NTuple::Item<double> tof_xhit2;
  NTuple::Item<double> tof_yhit2;
  NTuple::Array<double> tof_kal;
  NTuple::Array<double> tof_zr1;
  NTuple::Array<double> tof_zr2;
  NTuple::Item<double> tof_ztdc1;
  NTuple::Item<double> tof_ztdc2;
  NTuple::Item<double> tof_zadc1;
  NTuple::Item<double> tof_zadc2;
  NTuple::Item<double> tof_zt1;
  NTuple::Item<double> tof_zt2;
  NTuple::Item<double> tof_zt3;
  NTuple::Item<double> tof_zt4;
  NTuple::Item<double> tof_zq1;
  NTuple::Item<double> tof_zq2;
  NTuple::Item<double> tof_zq3;
  NTuple::Item<double> tof_zq4;
  NTuple::Item<int>    tof_size1;
  NTuple::Item<int>    tof_size2;
  NTuple::Item<int>    tof_size3;
  NTuple::Item<int>    tof_size4;
  NTuple::Item<double> tof_theta1;
  NTuple::Item<double> tof_theta2;
  NTuple::Item<double> tof_phi1;
  NTuple::Item<double> tof_phi2;
  NTuple::Item<int>    tof_quality1;
  NTuple::Item<int>    tof_quality2;
  NTuple::Item<int>    tof_quality;
  NTuple::Item<int>    tof_changed;
  NTuple::Item<int>    tof_tofid1;
  NTuple::Item<int>    tof_tofid2;
  NTuple::Item<int>    tof_strip1;
  NTuple::Item<int>    tof_strip2;
  NTuple::Item<int>    tof_board1;
  NTuple::Item<int>    tof_board2;
  NTuple::Item<int>    tof_crate1;
  NTuple::Item<int>    tof_crate2;
  NTuple::Item<int>    tof_fee1;
  NTuple::Item<int>    tof_fee2;
  NTuple::Item<double> tof_channel1;
  NTuple::Item<double> tof_channel2;
  NTuple::Item<double> tof_channel3;
  NTuple::Item<double> tof_channel4;
  NTuple::Item<double> tof_ph11;
  NTuple::Item<double> tof_ph12;
  NTuple::Item<double> tof_ph21;
  NTuple::Item<double> tof_ph22;
  NTuple::Item<double> tof_ph1;
  NTuple::Item<double> tof_ph2;
  NTuple::Item<double> tof_ph;
  NTuple::Item<double> tof_tofe11;
  NTuple::Item<double> tof_tofe12;
  NTuple::Item<double> tof_tofe21;
  NTuple::Item<double> tof_tofe22;
  NTuple::Item<double> tof_tofe1;
  NTuple::Item<double> tof_tofe2;
  NTuple::Item<double> tof_tofe;
  NTuple::Item<double> tof_tofmu11;
  NTuple::Item<double> tof_tofmu12;
  NTuple::Item<double> tof_tofmu21;
  NTuple::Item<double> tof_tofmu22;
  NTuple::Item<double> tof_tofmu1;
  NTuple::Item<double> tof_tofmu2;
  NTuple::Item<double> tof_tofmu;
  NTuple::Item<double> tof_tofpi11;
  NTuple::Item<double> tof_tofpi12;
  NTuple::Item<double> tof_tofpi21;
  NTuple::Item<double> tof_tofpi22;
  NTuple::Item<double> tof_tofpi1;
  NTuple::Item<double> tof_tofpi2;
  NTuple::Item<double> tof_tofpi;
  NTuple::Item<double> tof_tofk11;
  NTuple::Item<double> tof_tofk12;
  NTuple::Item<double> tof_tofk21;
  NTuple::Item<double> tof_tofk22;
  NTuple::Item<double> tof_tofk1;
  NTuple::Item<double> tof_tofk2;
  NTuple::Item<double> tof_tofk;
  NTuple::Item<double> tof_tofp11;
  NTuple::Item<double> tof_tofp12;
  NTuple::Item<double> tof_tofp21;
  NTuple::Item<double> tof_tofp22;
  NTuple::Item<double> tof_tofp1;
  NTuple::Item<double> tof_tofp2;
  NTuple::Item<double> tof_tofp;
  NTuple::Item<double> tof_qch1;
  NTuple::Item<double> tof_qch2;
  NTuple::Item<double> tof_qch3;
  NTuple::Item<double> tof_qch4;
  NTuple::Item<double> tof_adc1;
  NTuple::Item<double> tof_adc2;
  NTuple::Item<double> tof_adc3;
  NTuple::Item<double> tof_adc4;
  NTuple::Item<double> tof_tdc1;
  NTuple::Item<double> tof_tdc2;
  NTuple::Item<double> tof_tdc3;
  NTuple::Item<double> tof_tdc4;
  NTuple::Item<double> tof_texpe1;
  NTuple::Item<double> tof_texpmu1;
  NTuple::Item<double> tof_texppi1;
  NTuple::Item<double> tof_texpk1;
  NTuple::Item<double> tof_texpp1;
  NTuple::Item<double> tof_texpe2;
  NTuple::Item<double> tof_texpmu2;
  NTuple::Item<double> tof_texppi2;
  NTuple::Item<double> tof_texpk2;
  NTuple::Item<double> tof_texpp2;
  NTuple::Item<double> tof_texpe;
  NTuple::Item<double> tof_texpmu;
  NTuple::Item<double> tof_texppi;
  NTuple::Item<double> tof_texpk;
  NTuple::Item<double> tof_texpp;
  NTuple::Item<double> tof_tdiff1;
  NTuple::Item<double> tof_tdiff2;
  NTuple::Item<double> tof_trigcond;
  NTuple::Item<double> tof_trigchan;
  NTuple::Item<double> tof_trigwindow;
  NTuple::Item<double> tof_trigtype;
  NTuple::Item<double> tof_estime;
  NTuple::Item<int>    tof_t0stat;
  NTuple::Item<int>    tof_mctrkid;
  NTuple::Item<double> tof_mcp;
  NTuple::Item<double> tof_mctofp;
  NTuple::Item<double> tof_mczrhit;
  NTuple::Item<double> tof_mcpath;
  NTuple::Item<double> tof_mctexp;

  NTuple::Tuple*&      bb_tuple;
  NTuple::Item<int>    bb_run;
  NTuple::Item<int>    bb_event;
  NTuple::Item<int>    bb_trksize;
  NTuple::Item<int>    bb_mdcsize;
  NTuple::Item<int>    bb_emcsize;
  NTuple::Item<int>    bb_trk1;
  NTuple::Item<double> bb_pmdc1;
  NTuple::Item<double> bb_ptmdc1;
  NTuple::Item<double> bb_theta1;
  NTuple::Item<double> bb_phi1;
  NTuple::Item<double> bb_x1;
  NTuple::Item<double> bb_y1;
  NTuple::Item<double> bb_z1;
  NTuple::Item<double> bb_r1;
  NTuple::Item<int>    bb_charge1;
  NTuple::Item<double> bb_chi1;
  NTuple::Item<double> bb_ndof1;
  NTuple::Item<int>    bb_layer1;
  NTuple::Item<int>    bb_trk2;
  NTuple::Item<double> bb_pmdc2;
  NTuple::Item<double> bb_ptmdc2;
  NTuple::Item<double> bb_theta2;
  NTuple::Item<double> bb_phi2;
  NTuple::Item<double> bb_x2;
  NTuple::Item<double> bb_y2;
  NTuple::Item<double> bb_z2;
  NTuple::Item<double> bb_r2;
  NTuple::Item<int>    bb_charge2;
  NTuple::Item<double> bb_chi2;
  NTuple::Item<double> bb_ndof2;
  NTuple::Item<int>    bb_layer2;
  NTuple::Item<double> bb_dang;
  NTuple::Item<double> bb_dphi;
  NTuple::Item<double> bb_pe1;
  NTuple::Item<double> bb_pte1;
  NTuple::Item<double> bb_pe2;
  NTuple::Item<double> bb_pte2;  
  NTuple::Item<double> bb_pmu1;
  NTuple::Item<double> bb_ptmu1;
  NTuple::Item<double> bb_pmu2;
  NTuple::Item<double> bb_ptmu2;  
  NTuple::Item<double> bb_extx1;
  NTuple::Item<double> bb_exty1;
  NTuple::Item<double> bb_extz1;
  NTuple::Item<double> bb_extx2;
  NTuple::Item<double> bb_exty2;
  NTuple::Item<double> bb_extz2;
  NTuple::Item<double> bb_emctrk1;
  NTuple::Item<double> bb_emcx1;
  NTuple::Item<double> bb_emcy1;
  NTuple::Item<double> bb_emcz1;
  NTuple::Item<double> bb_ep1;
  NTuple::Item<double> bb_th1;
  NTuple::Item<double> bb_ph1;
  NTuple::Item<double> bb_emctrk2;
  NTuple::Item<double> bb_emcx2;
  NTuple::Item<double> bb_emcy2;
  NTuple::Item<double> bb_emcz2;
  NTuple::Item<double> bb_ep2;
  NTuple::Item<double> bb_th2;
  NTuple::Item<double> bb_ph2;
  NTuple::Item<double> bb_dr1;
  NTuple::Item<double> bb_drxy1;
  NTuple::Item<double> bb_dz1;
  NTuple::Item<double> bb_dr2;
  NTuple::Item<double> bb_drxy2;
  NTuple::Item<double> bb_dz2;
  NTuple::Item<double> bb_etot;

};
#endif
