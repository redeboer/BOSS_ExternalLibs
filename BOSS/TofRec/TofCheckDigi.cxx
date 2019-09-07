#include "TofRec/TofCheckDigi.h"
#include "TofRec/TofConstants.h"
#include "TofCaliSvc/ITofCaliSvc.h"
// #include "TofQElecSvc/ITofQElecSvc.h"
#include "Identifier/Identifier.h"
#include "Identifier/TofID.h"
#include "McTruth/TofMcHit.h"
#include "McTruth/McParticle.h"
#include <iostream>

extern ITofCaliSvc*  tofCaliSvc;
// extern ITofQElecSvc* tofQElecSvc;

TofCheckDigi::TofCheckDigi( NTuple::Tuple*& digi, NTuple::Tuple*& barrel, NTuple::Tuple*& endcap, NTuple::Tuple*& mrpc, NTuple::Tuple*& ext, NTuple::Tuple*& tof, NTuple::Tuple*& bhabha ):digi_tuple(digi),barrel_tuple(barrel),endcap_tuple(endcap),mrpc_tuple(mrpc),ext_tuple(ext),tof_tuple(tof),bb_tuple(bhabha) {
  if(!digi_tuple) {
    std::cerr<<" Error: invalid pionter in TofCheckDigi(NTuple::Tuple*) for digi_tuple "<<std::endl;
  }
  else{
    digi_tuple->addItem("run",    digi_run      );
    digi_tuple->addItem("event",  digi_event    ); 
    digi_tuple->addItem("barrel", digi_barrel   );
    digi_tuple->addItem("endcap", digi_endcap   );
    digi_tuple->addItem("layer",  digi_layer    );
    digi_tuple->addItem("tofid",  digi_tofId    );
    digi_tuple->addItem("strip",  digi_strip    );
    digi_tuple->addItem("end",    digi_end      );
    digi_tuple->addItem("adc",    digi_adc      );
    digi_tuple->addItem("tdc",    digi_tdc      );
    digi_tuple->addItem("qclock", digi_qclock   );
    digi_tuple->addItem("tclock", digi_tclock   );
    digi_tuple->addItem("over",   digi_overflow );
    digi_tuple->addItem("noq",    digi_noq      );
    digi_tuple->addItem("not",    digi_not      );
    digi_tuple->addItem("multiq", digi_multiq   );
    digi_tuple->addItem("multit", digi_multit   );
    digi_tuple->addItem("overq",  digi_overq    );
    digi_tuple->addItem("overt",  digi_overt    );
    digi_tuple->addItem("t0",     digi_t0       );
    digi_tuple->addItem("t0stat", digi_t0stat   );
    digi_tuple->addItem("crate",  digi_crate    );
    digi_tuple->addItem("geo",    digi_geo      );
    digi_tuple->addItem("tdcnum", digi_tdcnum   );
    digi_tuple->addItem("err",    digi_err      );   
  }

  if(!barrel_tuple) {
    std::cerr<<" Error: invalid pionter in TofCheckDigi(NTuple::Tuple*) for barrel_tuple "<<std::endl;
  }
  else{
    barrel_tuple->addItem("run",      barrel_run        );
    barrel_tuple->addItem("event",    barrel_event      );
    barrel_tuple->addItem("tofid",    barrel_tofId      );
    barrel_tuple->addItem("icluster", barrel_tofTrackId );
    barrel_tuple->addItem("layer",    barrel_layer      );
    barrel_tuple->addItem("qch1",     barrel_qchannel1  );
    barrel_tuple->addItem("tch1",     barrel_tchannel1  );
    barrel_tuple->addItem("qch2",     barrel_qchannel2  );
    barrel_tuple->addItem("tch2",     barrel_tchannel2  );
    barrel_tuple->addItem("qtc1",     barrel_qtc1       );
    barrel_tuple->addItem("adc1",     barrel_adc1       );
    barrel_tuple->addItem("tdc1",     barrel_tdc1       );
    barrel_tuple->addItem("qtc2",     barrel_qtc2       );
    barrel_tuple->addItem("adc2",     barrel_adc2       );
    barrel_tuple->addItem("tdc2",     barrel_tdc2       );
    barrel_tuple->addItem("qclock1",  barrel_qclock1    );
    barrel_tuple->addItem("tclock1",  barrel_tclock1    );
    barrel_tuple->addItem("qclock2",  barrel_qclock2    );
    barrel_tuple->addItem("tclock2",  barrel_tclock2    );
    barrel_tuple->addItem("qual",     barrel_quality    );
    barrel_tuple->addItem("tmatched", barrel_tmatched   );
    barrel_tuple->addItem("qtimes1",  barrel_qtimes1    );
    barrel_tuple->addItem("ttimes1",  barrel_ttimes1    );
    barrel_tuple->addItem("qtimes2",  barrel_qtimes2    );
    barrel_tuple->addItem("ttimes2",  barrel_ttimes2    );
    barrel_tuple->addItem("times1",   barrel_times1     );
    barrel_tuple->addItem("times2",   barrel_times2     );
    barrel_tuple->addItem("times",    barrel_times      );
    barrel_tuple->addItem("qnum1",    barrel_qnum1      );
    barrel_tuple->addItem("tnum1",    barrel_tnum1      );
    barrel_tuple->addItem("qnum2",    barrel_qnum2      );
    barrel_tuple->addItem("tnum2",    barrel_tnum2      );
    barrel_tuple->addItem("tsum",     barrel_tsum       );
    barrel_tuple->addItem("tsub",     barrel_tsub       );
    barrel_tuple->addItem("ztdc",     barrel_ztdc       );
    barrel_tuple->addItem("zadc",     barrel_zadc       );
    barrel_tuple->addItem("t0",       barrel_t0         );
    barrel_tuple->addItem("t0stat",   barrel_t0stat     );
  }

  if(!endcap_tuple) {
    std::cerr<<" Error: invalid pionter in TofCheckDigi(NTuple::Tuple*) for endcap_tuple "<<std::endl;
  }
  else{
    endcap_tuple->addItem("run",      endcap_run        );
    endcap_tuple->addItem("event",    endcap_event      );
    endcap_tuple->addItem("tofid",    endcap_tofId      );
    endcap_tuple->addItem("icluster", endcap_tofTrackId );
    endcap_tuple->addItem("qch",      endcap_qchannel   );
    endcap_tuple->addItem("tch",      endcap_tchannel   );
    endcap_tuple->addItem("qtc",      endcap_qtc        );
    endcap_tuple->addItem("adc",      endcap_adc        );
    endcap_tuple->addItem("tdc",      endcap_tdc        );
    endcap_tuple->addItem("qual",     endcap_quality    );
    endcap_tuple->addItem("qclock",   endcap_qclock     );
    endcap_tuple->addItem("tclock",   endcap_tclock     );
    endcap_tuple->addItem("qtimes",   endcap_qtimes     );
    endcap_tuple->addItem("ttimes",   endcap_ttimes     );
    endcap_tuple->addItem("times",    endcap_times      );
    endcap_tuple->addItem("qnum",     endcap_qnum       );
    endcap_tuple->addItem("tnum",     endcap_tnum       );
    endcap_tuple->addItem("t0",       endcap_t0         );
    endcap_tuple->addItem("t0stat",   endcap_t0stat     );
  }

  if(!mrpc_tuple) {
    std::cerr<<" Error: invalid pionter in TofCheckDigi(NTuple::Tuple*) for mrpc_tuple "<<std::endl;
  }
  else{
    mrpc_tuple->addItem("run",      mrpc_run        );
    mrpc_tuple->addItem("event",    mrpc_event      );
    mrpc_tuple->addItem("tofid",    mrpc_tofId      );
    mrpc_tuple->addItem("strip",    mrpc_strip      );
    mrpc_tuple->addItem("icluster", mrpc_tofTrackId );
    mrpc_tuple->addItem("ttch1",    mrpc_ttch1      );
    mrpc_tuple->addItem("tlch1",    mrpc_tlch1      );
    mrpc_tuple->addItem("ttch2",    mrpc_ttch2      );
    mrpc_tuple->addItem("tlch2",    mrpc_tlch2      );
    mrpc_tuple->addItem("tt1",      mrpc_tt1        );
    mrpc_tuple->addItem("tl1",      mrpc_tl1        );
    mrpc_tuple->addItem("tt2",      mrpc_tt2        );
    mrpc_tuple->addItem("tl2",      mrpc_tl2        );
    mrpc_tuple->addItem("tot1",     mrpc_tot1       );
    mrpc_tuple->addItem("tot2",     mrpc_tot2       );
    mrpc_tuple->addItem("qual",     mrpc_quality    );
    mrpc_tuple->addItem("tmatched", mrpc_tmatched   );
    mrpc_tuple->addItem("tttimes1", mrpc_tttimes1   );
    mrpc_tuple->addItem("tltimes1", mrpc_tltimes1   );
    mrpc_tuple->addItem("tttimes2", mrpc_tttimes2   );
    mrpc_tuple->addItem("tltimes2", mrpc_tltimes2   );
    mrpc_tuple->addItem("times1",   mrpc_times1     );
    mrpc_tuple->addItem("times2",   mrpc_times2     );
    mrpc_tuple->addItem("times",    mrpc_times      );
    mrpc_tuple->addItem("ttnum1",   mrpc_ttnum1     );
    mrpc_tuple->addItem("tlnum1",   mrpc_tlnum1     );
    mrpc_tuple->addItem("ttnum2",   mrpc_ttnum2     );
    mrpc_tuple->addItem("tlnum2",   mrpc_tlnum2     );
    mrpc_tuple->addItem("tsum",     mrpc_tsum       );
    mrpc_tuple->addItem("tsub",     mrpc_tsub       );
    mrpc_tuple->addItem("ztdc",     mrpc_ztdc       );
    mrpc_tuple->addItem("zadc",     mrpc_zadc       );
    mrpc_tuple->addItem("t0",       mrpc_t0         );
    mrpc_tuple->addItem("t0stat",   mrpc_t0stat     );
  }

  if(!ext_tuple) {
    std::cerr<<" Error: invalid pionter in TofCheckDigi(NTuple::Tuple*) for ext_tple"<<std::endl;
  }
  else{
    ext_tuple->addItem("run",          ext_run        );
    ext_tuple->addItem("event",        ext_event      );
    ext_tuple->addItem("track",        ext_trackid    );
    ext_tuple->addItem("costheta",     ext_costheta   );
    ext_tuple->addItem("kal",      5,  ext_kalman     );
    ext_tuple->addItem("x1",       5,  ext_x1         );
    ext_tuple->addItem("x2",       5,  ext_x2         );
    ext_tuple->addItem("y1",       5,  ext_y1         );
    ext_tuple->addItem("y2",       5,  ext_y2         );
    ext_tuple->addItem("z1",       5,  ext_z1         );
    ext_tuple->addItem("z2",       5,  ext_z2         );
    ext_tuple->addItem("tofid1",   5,  ext_tofid1     );
    ext_tuple->addItem("tofid2",   5,  ext_tofid2     );
    ext_tuple->addItem("strip1",   5,  ext_strip1     );
    ext_tuple->addItem("strip2",   5,  ext_strip2     );
    ext_tuple->addItem("texp1",    5,  ext_texp1      );
    ext_tuple->addItem("texp2",    5,  ext_texp2      );
    ext_tuple->addItem("p",            ext_p          );
    ext_tuple->addItem("path1",    5,  ext_path1      );
    ext_tuple->addItem("path2",    5,  ext_path2      );
    ext_tuple->addItem("zrhit1",   5,  ext_zrhit1     );
    ext_tuple->addItem("zrhit2",   5,  ext_zrhit2     );
    ext_tuple->addItem("errzr1",   5,  ext_errzr1     );
    ext_tuple->addItem("errzr2",   5,  ext_errzr2     );
    ext_tuple->addItem("cost1",    5,  ext_theta1     );
    ext_tuple->addItem("cost2",    5,  ext_theta2     );
    ext_tuple->addItem("phi1",     5,  ext_phi1       );
    ext_tuple->addItem("phi2",     5,  ext_phi2       );
    ext_tuple->addItem("hitcase1", 5,  ext_hitcase1   );
    ext_tuple->addItem("hitcase2", 5,  ext_hitcase2   );
    ext_tuple->addItem("hitcase",  5,  ext_hitcase    );
    ext_tuple->addItem("qual",     5,  ext_quality    );
  }

  if(!tof_tuple) {
    std::cerr<<" Error: invalid pionter in TofCheckDigi(NTuple::Tuple*) for tof_tple"<<std::endl;
  }
  else{
    tof_tuple->addItem("run",        tof_run        );
    tof_tuple->addItem("event",      tof_event      );
    tof_tuple->addItem("toftrack",   tof_toftrackid );
    tof_tuple->addItem("track",      tof_trackid    );
    tof_tuple->addItem("charge",     tof_charge     );
    tof_tuple->addItem("pt",     5,  tof_pt         );
    tof_tuple->addItem("p",      5,  tof_p          );
    tof_tuple->addItem("id1",        tof_id1        );
    tof_tuple->addItem("id2",        tof_id2        );
    tof_tuple->addItem("istrip1",    tof_istrip1    );
    tof_tuple->addItem("istrip2",    tof_istrip2    );
    tof_tuple->addItem("dstrip1",    tof_dstrip1    );
    tof_tuple->addItem("dstrip2",    tof_dstrip2    );
    tof_tuple->addItem("barrel",     tof_barrel     );
    tof_tuple->addItem("hitcase",    tof_hitcase    );
    tof_tuple->addItem("mom",        tof_momentum   );
    tof_tuple->addItem("path1",      tof_path1      );
    tof_tuple->addItem("path2",      tof_path2      );
    tof_tuple->addItem("zrhit1",     tof_zrhit1     );
    tof_tuple->addItem("zrhit2",     tof_zrhit2     );
    tof_tuple->addItem("errzr1",     tof_errzr1     );
    tof_tuple->addItem("errzr2",     tof_errzr2     );
    tof_tuple->addItem("xhit1",      tof_xhit1      );
    tof_tuple->addItem("yhit1",      tof_yhit1      );
    tof_tuple->addItem("xhit2",      tof_xhit2      );
    tof_tuple->addItem("yhit2",      tof_yhit2      );
    tof_tuple->addItem("kal",    5,  tof_kal        );
    tof_tuple->addItem("zr1",    5,  tof_zr1        );
    tof_tuple->addItem("zr2",    5,  tof_zr2        );   
    tof_tuple->addItem("ztdc1",      tof_ztdc1      );
    tof_tuple->addItem("ztdc2",      tof_ztdc2      );
    tof_tuple->addItem("zadc1",      tof_zadc1      );
    tof_tuple->addItem("zadc2",      tof_zadc2      );
    tof_tuple->addItem("zt1",        tof_zt1        );
    tof_tuple->addItem("zt2",        tof_zt2        );
    tof_tuple->addItem("zt3",        tof_zt3        );
    tof_tuple->addItem("zt4",        tof_zt4        );
    tof_tuple->addItem("zq1",        tof_zq1        );
    tof_tuple->addItem("zq2",        tof_zq2        );
    tof_tuple->addItem("zq3",        tof_zq3        );
    tof_tuple->addItem("zq4",        tof_zq4        );
    tof_tuple->addItem("size1",      tof_size1      );
    tof_tuple->addItem("size2",      tof_size2      );
    tof_tuple->addItem("size3",      tof_size3      );
    tof_tuple->addItem("size4",      tof_size4      );
    tof_tuple->addItem("theta1",     tof_theta1     );
    tof_tuple->addItem("theta2",     tof_theta2     );
    tof_tuple->addItem("phi1",       tof_phi1       );
    tof_tuple->addItem("phi2",       tof_phi2       );
    tof_tuple->addItem("qual1",      tof_quality1   );
    tof_tuple->addItem("qual2",      tof_quality2   );
    tof_tuple->addItem("qual",       tof_quality    );
    tof_tuple->addItem("change",     tof_changed    );
    tof_tuple->addItem("tofid1",     tof_tofid1     );
    tof_tuple->addItem("tofid2",     tof_tofid2     );
    tof_tuple->addItem("strip1",     tof_strip1     );
    tof_tuple->addItem("strip2",     tof_strip2     );
    tof_tuple->addItem("board1",     tof_board1     );
    tof_tuple->addItem("board2",     tof_board2     );
    tof_tuple->addItem("crate1",     tof_crate1     );
    tof_tuple->addItem("crate2",     tof_crate2     );
    tof_tuple->addItem("fee1",       tof_fee1       );
    tof_tuple->addItem("fee2",       tof_fee2       );
    tof_tuple->addItem("channel1",   tof_channel1   );
    tof_tuple->addItem("channel2",   tof_channel2   );
    tof_tuple->addItem("channel3",   tof_channel3   );
    tof_tuple->addItem("channel4",   tof_channel4   );
    tof_tuple->addItem("ph11",       tof_ph11       );
    tof_tuple->addItem("ph12",       tof_ph12       );
    tof_tuple->addItem("ph21",       tof_ph21       );
    tof_tuple->addItem("ph22",       tof_ph22       );
    tof_tuple->addItem("ph1",        tof_ph1        );
    tof_tuple->addItem("ph2",        tof_ph2        );
    tof_tuple->addItem("ph",         tof_ph         );
    tof_tuple->addItem("tofe11",     tof_tofe11     );
    tof_tuple->addItem("tofe12",     tof_tofe12     );
    tof_tuple->addItem("tofe21",     tof_tofe21     );
    tof_tuple->addItem("tofe22",     tof_tofe22     );
    tof_tuple->addItem("tofe1",      tof_tofe1      );
    tof_tuple->addItem("tofe2",      tof_tofe2      );
    tof_tuple->addItem("tofe",       tof_tofe       );
    tof_tuple->addItem("tofmu11",    tof_tofmu11    );
    tof_tuple->addItem("tofmu12",    tof_tofmu12    );
    tof_tuple->addItem("tofmu21",    tof_tofmu21    );
    tof_tuple->addItem("tofmu22",    tof_tofmu22    );
    tof_tuple->addItem("tofmu1",     tof_tofmu1     );
    tof_tuple->addItem("tofmu2",     tof_tofmu2     );
    tof_tuple->addItem("tofmu",      tof_tofmu      );
    tof_tuple->addItem("tofpi11",    tof_tofpi11    );
    tof_tuple->addItem("tofpi12",    tof_tofpi12    );
    tof_tuple->addItem("tofpi21",    tof_tofpi21    );
    tof_tuple->addItem("tofpi22",    tof_tofpi22    );
    tof_tuple->addItem("tofpi1",     tof_tofpi1     );
    tof_tuple->addItem("tofpi2",     tof_tofpi2     );
    tof_tuple->addItem("tofpi",      tof_tofpi      );
    tof_tuple->addItem("tofk11",     tof_tofk11     );
    tof_tuple->addItem("tofk12",     tof_tofk12     );
    tof_tuple->addItem("tofk21",     tof_tofk21     );
    tof_tuple->addItem("tofk22",     tof_tofk22     );
    tof_tuple->addItem("tofk1",      tof_tofk1      );
    tof_tuple->addItem("tofk2",      tof_tofk2      );
    tof_tuple->addItem("tofk",       tof_tofk       );
    tof_tuple->addItem("tofp11",     tof_tofp11     );
    tof_tuple->addItem("tofp12",     tof_tofp12     );
    tof_tuple->addItem("tofp21",     tof_tofp21     );
    tof_tuple->addItem("tofp22",     tof_tofp22     );
    tof_tuple->addItem("tofp1",      tof_tofp1      );
    tof_tuple->addItem("tofp2",      tof_tofp2      );
    tof_tuple->addItem("tofp",       tof_tofp       );
    tof_tuple->addItem("qch1",       tof_qch1       );
    tof_tuple->addItem("qch2",       tof_qch2       );
    tof_tuple->addItem("qch3",       tof_qch3       );
    tof_tuple->addItem("qch4",       tof_qch4       );
    tof_tuple->addItem("adc1",       tof_adc1       );
    tof_tuple->addItem("adc2",       tof_adc2       );
    tof_tuple->addItem("adc3",       tof_adc3       );
    tof_tuple->addItem("adc4",       tof_adc4       );
    tof_tuple->addItem("tdc1",       tof_tdc1       );
    tof_tuple->addItem("tdc2",       tof_tdc2       );
    tof_tuple->addItem("tdc3",       tof_tdc3       );
    tof_tuple->addItem("tdc4",       tof_tdc4       );
    tof_tuple->addItem("texpe1",     tof_texpe1     );
    tof_tuple->addItem("texpmu1",    tof_texpmu1    );
    tof_tuple->addItem("texppi1",    tof_texppi1    );
    tof_tuple->addItem("texpk1",     tof_texpk1     );
    tof_tuple->addItem("texpp1",     tof_texpp1     );
    tof_tuple->addItem("texpe2",     tof_texpe2     );
    tof_tuple->addItem("texpmu2",    tof_texpmu2    );
    tof_tuple->addItem("texppi2",    tof_texppi2    );
    tof_tuple->addItem("texpk2",     tof_texpk2     );
    tof_tuple->addItem("texpp2",     tof_texpp2     );
    tof_tuple->addItem("texpe",      tof_texpe      );
    tof_tuple->addItem("texpmu",     tof_texpmu     );
    tof_tuple->addItem("texppi",     tof_texppi     );
    tof_tuple->addItem("texpk",      tof_texpk      );
    tof_tuple->addItem("texpp",      tof_texpp      );
    tof_tuple->addItem("tdiff1",     tof_tdiff1     );
    tof_tuple->addItem("tdiff2",     tof_tdiff2     );
    tof_tuple->addItem("trigcond",   tof_trigcond   );
    tof_tuple->addItem("trigchan",   tof_trigchan   );
    tof_tuple->addItem("trigwindow", tof_trigwindow );
    tof_tuple->addItem("trigtype",   tof_trigtype   );
    tof_tuple->addItem("t0",         tof_estime     );
    tof_tuple->addItem("t0stat",     tof_t0stat     );
    tof_tuple->addItem("mctrkid",    tof_mctrkid    );
    tof_tuple->addItem("mcp",        tof_mcp        );
    tof_tuple->addItem("mctofp",     tof_mctofp     );
    tof_tuple->addItem("mczrhit",    tof_mczrhit    );
    tof_tuple->addItem("mcpath",     tof_mcpath     );
    tof_tuple->addItem("mctexp",     tof_mctexp     );
  }

  if(!bb_tuple) {
    std::cerr<<" Error: invalid pionter in TofCheckDigi(NTuple::Tuple*) for bb_tple"<<std::endl;
  }
  else{
    bb_tuple->addItem("run",      bb_run        );
    bb_tuple->addItem("event",    bb_event      );
    bb_tuple->addItem("trksize",  bb_trksize    );
    bb_tuple->addItem("mdcsize",  bb_mdcsize    );
    bb_tuple->addItem("emcsize",  bb_emcsize    );
    bb_tuple->addItem("trk1",     bb_trk1       );
    bb_tuple->addItem("pmdc1",    bb_pmdc1      );
    bb_tuple->addItem("ptmdc1",   bb_ptmdc1     );
    bb_tuple->addItem("theta1",   bb_theta1     );
    bb_tuple->addItem("phi1",     bb_phi1       );
    bb_tuple->addItem("x1",       bb_x1         );
    bb_tuple->addItem("y1",       bb_y1         );
    bb_tuple->addItem("z1",       bb_z1         );
    bb_tuple->addItem("r1",       bb_r1         );
    bb_tuple->addItem("charge1",  bb_charge1    );
    bb_tuple->addItem("chi1",     bb_chi1       );
    bb_tuple->addItem("ndof1",    bb_ndof1      );
    bb_tuple->addItem("layer1",   bb_layer1     );
    bb_tuple->addItem("trk2",     bb_trk2       );
    bb_tuple->addItem("pmdc2",    bb_pmdc2      );
    bb_tuple->addItem("ptmdc2",   bb_ptmdc2     );
    bb_tuple->addItem("theta2",   bb_theta2     );
    bb_tuple->addItem("phi2",     bb_phi2       );
    bb_tuple->addItem("x2",       bb_x2         );
    bb_tuple->addItem("y2",       bb_y2         );
    bb_tuple->addItem("z2",       bb_z2         );
    bb_tuple->addItem("r2",       bb_r2         );
    bb_tuple->addItem("charge2",  bb_charge2    );
    bb_tuple->addItem("chi2",     bb_chi2       );
    bb_tuple->addItem("ndof2",    bb_ndof2      );
    bb_tuple->addItem("layer2",   bb_layer2     );
    bb_tuple->addItem("dang",     bb_dang       );
    bb_tuple->addItem("dphi",     bb_dphi       );
    bb_tuple->addItem("pe1",      bb_pe1        );
    bb_tuple->addItem("pte1",     bb_pte1       );
    bb_tuple->addItem("pe2",      bb_pe2        );
    bb_tuple->addItem("pte2",     bb_pte2       );
    bb_tuple->addItem("pmu1",     bb_pmu1       );
    bb_tuple->addItem("ptmu1",    bb_ptmu1      );
    bb_tuple->addItem("pmu2",     bb_pmu2       );
    bb_tuple->addItem("ptmu2",    bb_ptmu2      );
    bb_tuple->addItem("extx1",    bb_extx1      );
    bb_tuple->addItem("exty1",    bb_exty1      );
    bb_tuple->addItem("extz1",    bb_extz1      );
    bb_tuple->addItem("extx2",    bb_extx2      );
    bb_tuple->addItem("exty2",    bb_exty2      );
    bb_tuple->addItem("extz2",    bb_extz2      );
    bb_tuple->addItem("emctrk1",  bb_emctrk1    );
    bb_tuple->addItem("emcx1",    bb_emcx1      );
    bb_tuple->addItem("emcy1",    bb_emcy1      );
    bb_tuple->addItem("emcz1",    bb_emcz1      );
    bb_tuple->addItem("ep1",      bb_ep1        );
    bb_tuple->addItem("th1",      bb_th1        );
    bb_tuple->addItem("ph1",      bb_ph1        );
    bb_tuple->addItem("emctrk2",  bb_emctrk2    );
    bb_tuple->addItem("emcx2",    bb_emcx2      );
    bb_tuple->addItem("emcy2",    bb_emcy2      );
    bb_tuple->addItem("emcz2",    bb_emcz2      );
    bb_tuple->addItem("ep2",      bb_ep2        );
    bb_tuple->addItem("th2",      bb_th2        );
    bb_tuple->addItem("ph2",      bb_ph2        );
    bb_tuple->addItem("dr1",      bb_dr1        );
    bb_tuple->addItem("drxy1",    bb_drxy1      );
    bb_tuple->addItem("dz1",      bb_dz1        );
    bb_tuple->addItem("dr2",      bb_dr2        );
    bb_tuple->addItem("drxy2",    bb_drxy2      );
    bb_tuple->addItem("dz2",      bb_dz2        );
    bb_tuple->addItem("etot",     bb_etot       );
 }

  return;
}


void TofCheckDigi::FillCol(Event::EventHeader& eventHeader, TofDigiCol& tofDigiCol, double estime, int t0stat ) {
  if( tofDigiCol.size() <= 0 ) return;
  TofDigiCol::iterator iter_digi = tofDigiCol.begin();
  for( ; iter_digi!=tofDigiCol.end(); iter_digi++ ) {
    unsigned int overflow = (*iter_digi)->getOverflow();
    if( ( overflow & 0xfe000000 ) == 0xfe000000 ) {
      if( digi_tuple ) {
	digi_run    = eventHeader.runNumber();
	digi_event  = eventHeader.eventNumber();
	digi_barrel = -1;
	digi_endcap = -1;
	digi_layer  = -1;
	digi_tofId  = -1;
	digi_strip  = -1;
	digi_end    = -1;
	digi_adc    = 0x7fffffff;              // adc 13bits
	digi_tdc    = 0x7fffffff;
	digi_qclock = 1000.0;
	digi_tclock = 1000.0;
	digi_overflow = overflow;
	digi_noq    = -1;
	digi_not    = -1;
	digi_multiq = -1;
	digi_multit = -1;
	digi_overq  = -1;
	digi_overt  = -1;
	digi_t0     = -1;
	digi_t0stat = -1;
	digi_crate  = ( ( overflow &0x1000000 ) >> 24 );
	digi_geo    = ( ( overflow &0x0f80000 ) >> 19 );
	digi_tdcnum = ( ( overflow &0x0078000 ) >> 15 );
	digi_err    = ( overflow &0x0007fff );
	digi_tuple->write();
      }
    }
    else {
      Identifier iden  = (*iter_digi)->identify();
      unsigned int adc = (*iter_digi)->getChargeChannel();
      unsigned int tdc = (*iter_digi)->getTimeChannel();
      if( digi_tuple ) {
	digi_run    = eventHeader.runNumber();
	digi_event  = eventHeader.eventNumber();
	digi_barrel = TofID::barrel_ec(iden);
	digi_endcap = TofID::endcap(iden);
	digi_layer  = TofID::layer(iden);
	digi_strip  = TofID::strip(iden);
	digi_end    = TofID::end(iden);
	if( TofID::is_scin(iden) ) {
	  digi_tofId  = TofID::phi_module(iden);
	  digi_adc    = ( adc & 0x1fff );              // adc 13bits
	}
	if( TofID::is_mrpc(iden) ) {
	  digi_tofId  = TofID::module(iden);
	  digi_adc    = adc;
	}
	if( adc == 0x7fffffff ) digi_adc = adc;
	digi_tdc    = tdc;
	digi_qclock = ( ( adc & 0x7e000 ) >> 13 );   // adc high 6 bits, time clock
	if( adc == 0x7fffffff ) digi_qclock = 1000.;
	digi_tclock = ( ( tdc & 0x7e000 ) >> 13 );   // tdc high 6 bits, time clock
	if( tdc == 0x7fffffff ) digi_tclock = 1000.;
	digi_overflow = overflow;
	digi_noq      = ( ( overflow & 0x20 ) >> 5 );
	digi_not      = ( ( overflow & 0x10 ) >> 4 );
	digi_multiq   = ( ( overflow & 0x08 ) >> 3 );
	digi_multit   = ( ( overflow & 0x04 ) >> 2 );
	digi_overq    = ( ( overflow & 0x02 ) >> 1 );
	digi_overt    =   ( overflow & 0x01 );
	digi_t0       = estime;
	digi_t0stat   = t0stat;
	digi_crate    = -1;
	digi_geo      = -1;
	digi_tdcnum   = -1;
	digi_tdcnum   = -1;
	digi_err      = -1;
	digi_tuple->write();
      }
    }
  }

  return;
}


void TofCheckDigi::Fill_Barrel(Event::EventHeader& eventHeader, TofData*& bTof, double estime, int t0stat ) {
  barrel_run        = eventHeader.runNumber();
  barrel_event      = eventHeader.eventNumber();
  barrel_tofId      = bTof->tofId();
  barrel_tofTrackId = bTof->tofTrackId();
  barrel_layer      = bTof->layer();
  barrel_qchannel1  = bTof->adcChannelEast();
  barrel_tchannel1  = bTof->tdcChannelEast();
  barrel_qchannel2  = bTof->adcChannelWest();
  barrel_tchannel2  = bTof->tdcChannelWest();
  barrel_qtc1       = bTof->qtc1();
  barrel_adc1       = bTof->adc1();
  barrel_tdc1       = bTof->tdc1();
  barrel_qtc2       = bTof->qtc2();
  barrel_adc2       = bTof->adc2();
  barrel_tdc2       = bTof->tdc2();
  barrel_qclock1    = bTof->qclock1();
  barrel_tclock1    = bTof->tclock1();
  barrel_qclock2    = bTof->qclock2();
  barrel_tclock2    = bTof->tclock2();
  barrel_quality    = bTof->quality();
  barrel_tmatched   = bTof->tmatched();
  barrel_qtimes1    = bTof->qtimes1();
  barrel_ttimes1    = bTof->ttimes1();
  barrel_qtimes2    = bTof->qtimes2();
  barrel_ttimes2    = bTof->ttimes2();
  barrel_times1     = bTof->eastTimes();
  barrel_times2     = bTof->westTimes();
  barrel_times      = bTof->times();
  barrel_qnum1      = bTof->qnumber1();
  barrel_tnum1      = bTof->tnumber1();
  barrel_qnum2      = bTof->qnumber2();
  barrel_tnum2      = bTof->tnumber2();
  barrel_tsum       = ( bTof->tdc1() + bTof->tdc2() )/2.0;
  barrel_tsub       = ( bTof->tdc1() - bTof->tdc2() )/2.0;
  barrel_ztdc       = tofCaliSvc->ZTDC( bTof->tdc1(), bTof->tdc2(), bTof->tofId() );
  barrel_zadc       = tofCaliSvc->ZADC( bTof->adc1(), bTof->adc2(), bTof->tofId() );
  barrel_t0         = estime;
  barrel_t0stat     = t0stat;
  barrel_tuple->write();
  return;
}


void TofCheckDigi::Fill_Endcap(Event::EventHeader& eventHeader, TofData*& eTof, double estime, int t0stat) {
  endcap_run        = eventHeader.runNumber();
  endcap_event      = eventHeader.eventNumber();
  endcap_tofId      = eTof->tofId();
  endcap_tofTrackId = eTof->tofTrackId();
  endcap_qchannel   = eTof->adcChannel();
  endcap_tchannel   = eTof->tdcChannel();
  endcap_qtc        = eTof->qtc();
  endcap_adc        = eTof->adc();
  endcap_tdc        = eTof->tdc();
  endcap_quality    = eTof->quality();
  endcap_qclock     = eTof->qclock();
  endcap_tclock     = eTof->tclock();
  endcap_qtimes     = eTof->qtimes1();
  endcap_ttimes     = eTof->ttimes1();
  endcap_times      = eTof->times();
  endcap_qnum       = eTof->qnumber1();
  endcap_tnum       = eTof->tnumber1();
  endcap_t0         = estime;
  endcap_t0stat     = t0stat;
  endcap_tuple->write();
  return;
}


void TofCheckDigi::Fill_MRPC(Event::EventHeader& eventHeader, TofData*& mrpcTof, double estime, int t0stat) {
  mrpc_run        = eventHeader.runNumber();
  mrpc_event      = eventHeader.eventNumber();
  mrpc_tofId      = mrpcTof->tofId();
  mrpc_strip      = mrpcTof->strip();
  mrpc_tofTrackId = mrpcTof->tofTrackId();
  mrpc_ttch1      = mrpcTof->adcChannelEast();
  mrpc_tlch1      = mrpcTof->tdcChannelEast();
  mrpc_ttch2      = mrpcTof->adcChannelWest();
  mrpc_tlch2      = mrpcTof->tdcChannelWest();
  mrpc_tt1        = mrpcTof->qtc1();
  mrpc_tl1        = mrpcTof->tdc1();
  mrpc_tt2        = mrpcTof->qtc2();
  mrpc_tl2        = mrpcTof->tdc2();
  mrpc_tot1       = mrpcTof->adc1();
  mrpc_tot2       = mrpcTof->adc2();
  mrpc_quality    = mrpcTof->quality();
  mrpc_tmatched   = mrpcTof->tmatched();
  mrpc_tttimes1   = mrpcTof->qtimes1();
  mrpc_tltimes1   = mrpcTof->ttimes1();
  mrpc_tttimes2   = mrpcTof->qtimes2();
  mrpc_tltimes2   = mrpcTof->ttimes2();
  mrpc_times1     = mrpcTof->eastTimes();
  mrpc_times2     = mrpcTof->westTimes();
  mrpc_times      = mrpcTof->times();
  mrpc_ttnum1      = mrpcTof->qnumber1();
  mrpc_tlnum1      = mrpcTof->tnumber1();
  mrpc_ttnum2      = mrpcTof->qnumber2();
  mrpc_tlnum2      = mrpcTof->tnumber2();
  mrpc_tsum       = ( mrpcTof->tdc1() + mrpcTof->tdc2() )/2.0;
  mrpc_tsub       = ( mrpcTof->tdc1() - mrpcTof->tdc2() )/2.0;
  mrpc_ztdc       = tofCaliSvc->ZTDC( mrpcTof->tdc1(), mrpcTof->tdc2(), mrpcTof->tofId() );
  mrpc_zadc       = tofCaliSvc->ZADC( mrpcTof->adc1(), mrpcTof->adc2(), mrpcTof->tofId() );
  mrpc_t0         = estime;
  mrpc_t0stat     = t0stat;
  mrpc_tuple->write();
  return;
}


void TofCheckDigi::FillCol(Event::EventHeader& eventHeader, TofDataMap& tofDataMap, double estime, int t0stat) {
  if( tofDataMap.empty() ) return;
  IterTofDataMap iter = tofDataMap.begin();
  for( ; iter != tofDataMap.end(); iter++ ) {
    Identifier iden = TofID::cell_id( (*iter).first );
    if( TofID::is_scin( iden ) ) {
      if( TofID::is_barrel( iden ) ) {
	Fill_Barrel( eventHeader, (*iter).second, estime, t0stat );
      }
      else {
	Fill_Endcap( eventHeader, (*iter).second, estime, t0stat );
      }
    }
    if( TofID::is_mrpc( iden ) ) {
      Fill_MRPC( eventHeader, (*iter).second, estime, t0stat );
    }
  }
  return;
}


void TofCheckDigi::FillCol(Event::EventHeader& eventHeader, std::vector<TofData*>& tofDataVec, double estime, int t0stat) {
  if( tofDataVec.size() <= 0 ) return;
  std::vector<TofData*>::iterator iter = tofDataVec.begin();
  for( ; iter != tofDataVec.end(); iter++ ) {
    if( (*iter)->barrel() ) {
      Fill_Barrel( eventHeader, (*iter), estime, t0stat );
    }
    else {
      Fill_Endcap( eventHeader, (*iter), estime, t0stat );
    }
  }
  return;
}


void TofCheckDigi::Fill(Event::EventHeader& eventHeader, RecMdcTrack*& mdcTrack, RecMdcKalTrack*& mdcKalTrack, RecExtTrack*& extTrack ) {

  ext_run     = eventHeader.runNumber();
  ext_event   = eventHeader.eventNumber();
  ext_trackid = extTrack->trackId();
  ext_p       = mdcTrack->p();
  ext_costheta= cos( mdcTrack->theta() );

  int hitcase1[5], hitcase2[5], hitcase[5];
  int tofId1[5], tofId2[5];

  for( unsigned int i=0; i<5; i++ ) {

    ext_tofid1[i] = -9;
    ext_tofid2[i] = -9;
    ext_strip1[i] = -9;
    ext_strip2[i] = -9;
    ext_zrhit1[i] = -99.;
    ext_zrhit2[i] = -99.;
    ext_x1[i]     = -999.;
    ext_x2[i]     = -999.;
    ext_y1[i]     = -999.;
    ext_y2[i]     = -999.;
    ext_z1[i]     = -999.;
    ext_z2[i]     = -999.;

    hitcase1[i] = 7;
    hitcase2[i] = 7;
    hitcase[i]  = 7;

    ext_kalman[i] = mdcKalTrack->getStat( 1, i );
    ext_x1[i] = (extTrack->tof1Position(i)).x();
    ext_x2[i] = (extTrack->tof2Position(i)).x();
    ext_y1[i] = (extTrack->tof1Position(i)).y();
    ext_y2[i] = (extTrack->tof2Position(i)).y();
    ext_z1[i] = (extTrack->tof1Position(i)).z();
    ext_z2[i] = (extTrack->tof2Position(i)).z();

    tofId1[i] = extTrack->tof1VolumeNumber( i );
    tofId2[i] = extTrack->tof2VolumeNumber( i );

    if( tofId1[i]>=0 && tofId1[i]<=87 ) {
      ext_tofid1[i]  = tofId1[i];
      hitcase1[i] = 0;
    }
    else if( tofId1[i]>=176 && tofId1[i]<=223 ) {
      ext_tofid1[i]  = tofId1[i] - 176 + 48;
      hitcase1[i] = 4;
    }
    else if( tofId1[i]>=224 && tofId1[i]<=271 ) { 
      ext_tofid1[i] = tofId1[i] - 176 - 48;
      hitcase1[i] = 3;
    }
    else if( tofId1[i]>=272 && tofId1[i]<=1135 ) { 
      int module = tofId1[i] - 176 - 96;
      ext_strip1[i] = module%12;
      ext_tofid1[i] = module/12;
      if( tofId1[i]>=272 && tofId1[i]<=703 ) { 
	hitcase1[i] = 5;
      }
      else if( tofId1[i]>=704 && tofId1[i]<=1135 ) { 
	hitcase1[i] = 6;
      }
    }

    if( tofId2[i]>=88 && tofId2[i]<=175 ) {
      ext_tofid2[i] = tofId2[i];
      hitcase2[i] = 1;
    }
    else if( tofId2[i]>=272 && tofId2[i]<=1135 ) { 
      int module = tofId2[i] - 176 - 96;
      ext_strip2[i] = module%12;
      ext_tofid2[i] = module/12;
      if( tofId2[i]>=272 && tofId2[i]<=703 ) { 
	hitcase2[i] = 5;
      }
      else if( tofId2[i]>=704 && tofId2[i]<=1135 ) { 
	hitcase2[i] = 6;
      }
    }

    /*
    ext_p[i] = extTrack->tof1Momentum( i ).r();
    if( tofId1[i]<0 ) {
      ext_p[i] = extTrack->tof2Momentum( i ).r();
    }
    */

    if( ( hitcase1[i] == 0 ) || ( hitcase1[i] == 2 ) ) {
      ext_texp1[i]   = extTrack->tof1( i );
      ext_path1[i]   = extTrack->tof1Path( i );
      ext_zrhit1[i]  = extTrack->tof1Position( i ).z();
      ext_errzr1[i]  = extTrack->tof1PosSigmaAlongZ( i );
      ext_theta1[i]  = extTrack->tof1Momentum( i ).rho()/extTrack->tof1Momentum( i ).r();
      ext_phi1[i]    = extTrack->tof1Position( i ).phi();
    }
    else if( ( hitcase1[i] == 3 ) || ( hitcase1[i] == 4 ) ) {
      ext_texp1[i]   = extTrack->tof1( i );
      ext_path1[i]   = extTrack->tof1Path( i );
      ext_zrhit1[i]  = extTrack->tof1Position( i ).rho();
      ext_errzr1[i]  = sqrt( extTrack->tof1PosSigmaAlongX( i )*extTrack->tof1PosSigmaAlongX( i ) + extTrack->tof1PosSigmaAlongY( i )*extTrack->tof1PosSigmaAlongY( i ) );
      ext_theta1[i]  = extTrack->tof1Momentum( i ).z()/extTrack->tof1Momentum( i ).r();
      ext_phi1[i]    = extTrack->tof1Position( i ).phi();
    }
    else if( ( hitcase1[i] == 5 ) || ( hitcase1[i] == 6 ) ) {
      ext_texp1[i]   = extTrack->tof1( i );
      ext_path1[i]   = extTrack->tof1Path( i );
      ext_zrhit1[i]  = extTrack->tof1Position( i ).x();
      ext_errzr1[i]  = extTrack->tof1PosSigmaAlongX( i );
      ext_theta1[i]  = extTrack->tof1Momentum( i ).z()/extTrack->tof1Momentum( i ).r();
      ext_phi1[i]    = extTrack->tof1Position( i ).phi();
    }

    if( ( hitcase2[i] == 5 ) || ( hitcase2[i] == 6 ) ) {
      ext_texp2[i]   = extTrack->tof2( i );
      ext_path2[i]   = extTrack->tof2Path( i );
      ext_zrhit2[i]  = extTrack->tof2Position( i ).x();
      ext_errzr2[i]  = extTrack->tof2PosSigmaAlongX( i );
      ext_theta1[i]  = extTrack->tof2Momentum( i ).z()/extTrack->tof1Momentum( i ).r();
      ext_phi2[i]    = extTrack->tof2Position( i ).phi();
    }
    else if( hitcase2[i] == 1 ) {
      ext_texp2[i]   = extTrack->tof2( i );
      ext_path2[i]   = extTrack->tof2Path( i );
      ext_zrhit2[i]  = extTrack->tof2Position( i ).z();
      ext_errzr2[i]  = extTrack->tof2PosSigmaAlongZ( i );
      ext_theta2[i]  = extTrack->tof2Momentum( i ).rho()/extTrack->tof2Momentum( i ).r();
      ext_phi2[i]    = extTrack->tof2Position( i ).phi();
    }

    if( ( hitcase1[i] == 7 ) && ( hitcase2[i] == 7 ) ) { ext_quality[i] = 3; }
    else { ext_quality[i] = 0; }

    if( hitcase1[i] == 0 ) {
      if( hitcase2[i] == 1 )      { hitcase[i] = 2; }
      else if( hitcase2[i] == 7 ) { hitcase[i] = 0; }
    }
    else if( ( hitcase1[i] == 3 ) || ( hitcase1[i] == 4 ) ) {
      if( hitcase2[i] == 7 ) { hitcase[i] == hitcase1[i]; }
    }
    else if( ( hitcase1[i] == 5 ) || ( hitcase1[i] == 6 ) ) {
      if( hitcase2[i] == 7 ) { hitcase[i] == hitcase1[i]; }
    }

    if( hitcase2[i] == 1 ) {
      if( hitcase1[i] == 7 ) { hitcase[i] = 1; }
    }
    else if( ( hitcase2[i] == 5 ) || ( hitcase2[i] == 6 ) ) {
      if( hitcase1[i] == 7 )                { hitcase[i] == hitcase2[i]; }
      else if( hitcase1[i] == hitcase2[i] ) { hitcase[i] == hitcase2[i]; }
    }

    ext_hitcase1[i] = hitcase1[i];
    ext_hitcase2[i] = hitcase2[i];
    ext_hitcase[i]  = hitcase[i];
  }

  ext_tuple->write();

  return;
}


void TofCheckDigi::Fill_TofTrack(Event::EventHeader& eventHeader, TofTrack*& tof, double estime, int t0stat ) {

  tof_run     = eventHeader.runNumber();
  tof_event   = eventHeader.eventNumber();
  tof_toftrackid = tof->tofTrackId();
  tof_trackid    = tof->trackId();
  tof_id1        = tof->id1();
  tof_id2        = tof->id2();
  tof_istrip1    = tof->istrip1();
  tof_istrip2    = tof->istrip2();
  tof_barrel     = tof->barrel();
  tof_hitcase    = tof->hitCase();
  tof_momentum   = tof->p();
  tof_path1      = tof->path1();
  tof_path2      = tof->path2();
  tof_zrhit1     = tof->zrhit1();
  tof_zrhit2     = tof->zrhit2();
  tof_errzr1     = tof->errzrhit1();
  tof_errzr2     = tof->errzrhit2();
  tof_xhit1      = tof->xhit1();
  tof_yhit1      = tof->yhit1();
  tof_xhit2      = tof->xhit2();
  tof_yhit2      = tof->yhit2();
  for( unsigned int i=0; i<5; i++ ) {
    tof_kal[i]   = tof->kal(i);
    tof_zr1[i]   = tof->zr1(i);
    tof_zr2[i]   = tof->zr2(i);
  }
  tof_ztdc1      = tof->ztdc1();
  tof_ztdc2      = tof->ztdc2();
  tof_zadc1      = tof->zadc1();
  tof_zadc2      = tof->zadc2();
  tof_zt1        = -999.0;
  tof_zt2        = -999.0;
  tof_zt3        = -999.0;
  tof_zt4        = -999.0;
  tof_zq1        = -999.0;
  tof_zq2        = -999.0;
  tof_zq3        = -999.0;
  tof_zq4        = -999.0;
  if( tof->hitCase() == 0 || tof->hitCase() == 2 ) {
    tof_zt1 = tofCaliSvc->ZTDC1( tof->tdc2(), tof->tofId1(), tof->zrhit1() );
    tof_zt2 = tofCaliSvc->ZTDC2( tof->tdc1(), tof->tofId1(), tof->zrhit1() );
    tof_zq1 = tofCaliSvc->ZADC1( tof->adc2(), tof->tofId1(), tof->zrhit1() );
    tof_zq2 = tofCaliSvc->ZADC2( tof->adc1(), tof->tofId1(), tof->zrhit1() );
  }
  if( tof->hitCase() == 1 || tof->hitCase() == 2 ) {
    tof_zt3 = tofCaliSvc->ZTDC1( tof->tdc4(), tof->tofId2(), tof->zrhit2() );
    tof_zt4 = tofCaliSvc->ZTDC2( tof->tdc3(), tof->tofId2(), tof->zrhit2() );
    tof_zq3 = tofCaliSvc->ZADC1( tof->adc4(), tof->tofId2(), tof->zrhit2() );
    tof_zq4 = tofCaliSvc->ZADC2( tof->adc3(), tof->tofId2(), tof->zrhit2() );
  }
  tof_size1      = tof->size1();
  tof_size2      = tof->size2();
  tof_size3      = tof->size3();
  tof_size4      = tof->size4();
  tof_theta1     = tof->theta1();
  tof_theta2     = tof->theta2();
  tof_phi1       = tof->phi1();
  tof_phi2       = tof->phi2();
  tof_quality1   = tof->quality1();
  tof_quality2   = tof->quality2();
  tof_quality    = tof->quality();
  tof_tofid1     = tof->tofId1();
  tof_tofid2     = tof->tofId2();
  tof_strip1     = tof->strip1();
  tof_strip2     = tof->strip2();
  /*
  tof_board1     = tofQElecSvc->Board( tof->hitCase()<3, tof->tofId1(), true );
  tof_board2     = tofQElecSvc->Board( tof->hitCase()<3, tof->tofId2(), true );
  tof_crate1     = tofQElecSvc->Crate( tof->hitCase()<3, tof->tofId1(), true );
  tof_crate2     = tofQElecSvc->Crate( tof->hitCase()<3, tof->tofId2(), true );
  tof_fee1       = tofQElecSvc->Fee( tof->hitCase()<3, tof->tofId1(), true );
  tof_fee2       = tofQElecSvc->Fee( tof->hitCase()<3, tof->tofId2(), true );
  tof_channel1   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId1(),true );
  tof_channel2   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId1(),false );
  tof_channel3   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId2(),true );
  tof_channel4   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId2(),false );
  */
  tof_ph11       = tof->ph11();
  tof_ph12       = tof->ph12();
  tof_ph21       = tof->ph21();
  tof_ph22       = tof->ph22();
  tof_ph1        = tof->ph1();
  tof_ph2        = tof->ph2();
  tof_ph         = tof->ph();
  tof_tofe11     = tof->tof11(0);
  tof_tofe12     = tof->tof12(0);
  tof_tofe21     = tof->tof21(0);
  tof_tofe22     = tof->tof22(0);
  tof_tofe1      = tof->tof1(0);
  tof_tofe2      = tof->tof2(0);
  tof_tofe       = tof->tof(0);
  tof_tofmu11    = tof->tof11(1);
  tof_tofmu12    = tof->tof12(1);
  tof_tofmu21    = tof->tof21(1);
  tof_tofmu22    = tof->tof22(1);
  tof_tofmu1     = tof->tof1(1);
  tof_tofmu2     = tof->tof2(1);
  tof_tofmu      = tof->tof(1);
  tof_tofpi11    = tof->tof11(2);
  tof_tofpi12    = tof->tof12(2);
  tof_tofpi21    = tof->tof21(2);
  tof_tofpi22    = tof->tof22(2);
  tof_tofpi1     = tof->tof1(2);
  tof_tofpi2     = tof->tof2(2);
  tof_tofpi      = tof->tof(2);
  tof_tofk11     = tof->tof11(3);
  tof_tofk12     = tof->tof12(3);
  tof_tofk21     = tof->tof21(3);
  tof_tofk22     = tof->tof22(3);
  tof_tofk1      = tof->tof1(3);
  tof_tofk2      = tof->tof2(3);
  tof_tofk       = tof->tof(3);
  tof_tofp11     = tof->tof11(4);
  tof_tofp12     = tof->tof12(4);
  tof_tofp21     = tof->tof21(4);
  tof_tofp22     = tof->tof22(4);
  tof_tofp1      = tof->tof1(4);
  tof_tofp2      = tof->tof2(4);
  tof_tofp       = tof->tof(4);
  tof_qch1       = tof->qch1();
  tof_qch2       = tof->qch2();
  tof_qch3       = tof->qch3();
  tof_qch4       = tof->qch4();
  tof_adc1       = tof->adc1();
  tof_adc2       = tof->adc2();
  tof_adc3       = tof->adc3();
  tof_adc4       = tof->adc4();
  tof_tdc1       = tof->tdc1();
  tof_tdc2       = tof->tdc2();
  tof_tdc3       = tof->tdc3();
  tof_tdc4       = tof->tdc4();
  tof_texpe1     = tof->texpInner(0);
  tof_texpmu1    = tof->texpInner(1);
  tof_texppi1    = tof->texpInner(2);
  tof_texpk1     = tof->texpInner(3);
  tof_texpp1     = tof->texpInner(4);
  tof_texpe2     = tof->texpOuter(0);
  tof_texpmu2    = tof->texpOuter(1);
  tof_texppi2    = tof->texpOuter(2);
  tof_texpk2     = tof->texpOuter(3);
  tof_texpp2     = tof->texpOuter(4);
  tof_texpe      = tof->texp(0);
  tof_texpmu     = tof->texp(1);
  tof_texppi     = tof->texp(2);
  tof_texpk      = tof->texp(3);
  tof_texpp      = tof->texp(4);
  tof_tdiff1     = tof->tdiff1();
  tof_tdiff2     = tof->tdiff2();
  tof_estime     = estime;
  tof_t0stat     = t0stat;
  tof_tuple->write();

  return;
}


void TofCheckDigi::Fill_TofTrack(Event::EventHeader& eventHeader, TofTrack*& tof, double estime, int t0stat, TrigData& trigData ) {

  tof_run     = eventHeader.runNumber();
  tof_event   = eventHeader.eventNumber();
  tof_toftrackid = tof->tofTrackId();
  tof_trackid    = tof->trackId();
  tof_id1        = tof->id1();
  tof_id2        = tof->id2();
  tof_istrip1    = tof->istrip1();
  tof_istrip2    = tof->istrip2();
  tof_barrel     = tof->barrel();
  tof_hitcase    = tof->hitCase();
  tof_momentum          = tof->p();
  tof_path1      = tof->path1();
  tof_path2      = tof->path2();
  tof_zrhit1     = tof->zrhit1();
  tof_zrhit2     = tof->zrhit2();
  tof_errzr1     = tof->errzrhit1();
  tof_errzr2     = tof->errzrhit2();
  tof_xhit1      = tof->xhit1();
  tof_yhit1      = tof->yhit1();
  tof_xhit2      = tof->xhit2();
  tof_yhit2      = tof->yhit2();
  for( unsigned int i=0; i<5; i++ ) {
    tof_kal[i]   = tof->kal(i);
    tof_zr1[i]   = tof->zr1(i);
    tof_zr2[i]   = tof->zr2(i);
  }
  tof_ztdc1      = tof->ztdc1();
  tof_ztdc2      = tof->ztdc2();
  tof_zadc1      = tof->zadc1();
  tof_zadc2      = tof->zadc2();
  tof_zt1        = -999.0;
  tof_zt2        = -999.0;
  tof_zt3        = -999.0;
  tof_zt4        = -999.0;
  tof_zq1        = -999.0;
  tof_zq2        = -999.0;
  tof_zq3        = -999.0;
  tof_zq4        = -999.0;
  if( tof->hitCase() == 0 || tof->hitCase() == 2 ) {
    tof_zt1 = tofCaliSvc->ZTDC1( tof->tdc2(), tof->tofId1(), tof->zrhit1() );
    tof_zt2 = tofCaliSvc->ZTDC2( tof->tdc1(), tof->tofId1(), tof->zrhit1() );
    tof_zq1 = tofCaliSvc->ZADC1( tof->adc2(), tof->tofId1(), tof->zrhit1() );
    tof_zq2 = tofCaliSvc->ZADC2( tof->adc1(), tof->tofId1(), tof->zrhit1() );
  }
  if( tof->hitCase() == 1 || tof->hitCase() == 2 ) {
    tof_zt3 = tofCaliSvc->ZTDC1( tof->tdc4(), tof->tofId2(), tof->zrhit2() );
    tof_zt4 = tofCaliSvc->ZTDC2( tof->tdc3(), tof->tofId2(), tof->zrhit2() );
    tof_zq3 = tofCaliSvc->ZADC1( tof->adc4(), tof->tofId2(), tof->zrhit2() );
    tof_zq4 = tofCaliSvc->ZADC2( tof->adc3(), tof->tofId2(), tof->zrhit2() );
  }
  tof_size1      = tof->size1();
  tof_size2      = tof->size2();
  tof_size3      = tof->size3();
  tof_size4      = tof->size4();
  tof_theta1     = tof->theta1();
  tof_theta2     = tof->theta2();
  tof_phi1       = tof->phi1();
  tof_phi2       = tof->phi2();
  tof_quality1   = tof->quality1();
  tof_quality2   = tof->quality2();
  tof_quality    = tof->quality();
  tof_tofid1     = tof->tofId1();
  tof_tofid2     = tof->tofId2();
  tof_strip1     = tof->strip1();
  tof_strip2     = tof->strip2();
  /*
  tof_board1     = tofQElecSvc->Board( tof->hitCase()<3, tof->tofId1(), true );
  tof_board2     = tofQElecSvc->Board( tof->hitCase()<3, tof->tofId2(), true );
  tof_crate1     = tofQElecSvc->Crate( tof->hitCase()<3, tof->tofId1(), true );
  tof_crate2     = tofQElecSvc->Crate( tof->hitCase()<3, tof->tofId2(), true );
  tof_fee1       = tofQElecSvc->Fee( tof->hitCase()<3, tof->tofId1(), true );
  tof_fee2       = tofQElecSvc->Fee( tof->hitCase()<3, tof->tofId2(), true );
  tof_channel1   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId1(),true );
  tof_channel2   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId1(),false );
  tof_channel3   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId2(),true );
  tof_channel4   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId2(),false );
  */
  tof_ph11       = tof->ph11();
  tof_ph12       = tof->ph12();
  tof_ph21       = tof->ph21();
  tof_ph22       = tof->ph22();
  tof_ph1        = tof->ph1();
  tof_ph2        = tof->ph2();
  tof_ph         = tof->ph();
  tof_tofe11     = tof->tof11(0);
  tof_tofe12     = tof->tof12(0);
  tof_tofe21     = tof->tof21(0);
  tof_tofe22     = tof->tof22(0);
  tof_tofe1      = tof->tof1(0);
  tof_tofe2      = tof->tof2(0);
  tof_tofe       = tof->tof(0);
  tof_tofmu11    = tof->tof11(1);
  tof_tofmu12    = tof->tof12(1);
  tof_tofmu21    = tof->tof21(1);
  tof_tofmu22    = tof->tof22(1);
  tof_tofmu1     = tof->tof1(1);
  tof_tofmu2     = tof->tof2(1);
  tof_tofmu      = tof->tof(1);
  tof_tofpi11    = tof->tof11(2);
  tof_tofpi12    = tof->tof12(2);
  tof_tofpi21    = tof->tof21(2);
  tof_tofpi22    = tof->tof22(2);
  tof_tofpi1     = tof->tof1(2);
  tof_tofpi2     = tof->tof2(2);
  tof_tofpi      = tof->tof(2);
  tof_tofk11     = tof->tof11(3);
  tof_tofk12     = tof->tof12(3);
  tof_tofk21     = tof->tof21(3);
  tof_tofk22     = tof->tof22(3);
  tof_tofk1      = tof->tof1(3);
  tof_tofk2      = tof->tof2(3);
  tof_tofk       = tof->tof(3);
  tof_tofp11     = tof->tof11(4);
  tof_tofp12     = tof->tof12(4);
  tof_tofp21     = tof->tof21(4);
  tof_tofp22     = tof->tof22(4);
  tof_tofp1      = tof->tof1(4);
  tof_tofp2      = tof->tof2(4);
  tof_tofp       = tof->tof(4);
  tof_qch1       = tof->qch1();
  tof_qch2       = tof->qch2();
  tof_qch3       = tof->qch3();
  tof_qch4       = tof->qch4();
  tof_adc1       = tof->adc1();
  tof_adc2       = tof->adc2();
  tof_adc3       = tof->adc3();
  tof_adc4       = tof->adc4();
  tof_tdc1       = tof->tdc1();
  tof_tdc2       = tof->tdc2();
  tof_tdc3       = tof->tdc3();
  tof_tdc4       = tof->tdc4();
  tof_texpe1     = tof->texpInner(0);
  tof_texpmu1    = tof->texpInner(1);
  tof_texppi1    = tof->texpInner(2);
  tof_texpk1     = tof->texpInner(3);
  tof_texpp1     = tof->texpInner(4);
  tof_texpe2     = tof->texpOuter(0);
  tof_texpmu2    = tof->texpOuter(1);
  tof_texppi2    = tof->texpOuter(2);
  tof_texpk2     = tof->texpOuter(3);
  tof_texpp2     = tof->texpOuter(4);
  tof_texpe      = tof->texp(0);
  tof_texpmu     = tof->texp(1);
  tof_texppi     = tof->texp(2);
  tof_texpk      = tof->texp(3);
  tof_texpp      = tof->texp(4);
  tof_tdiff1     = tof->tdiff1();
  tof_tdiff2     = tof->tdiff2();
  unsigned int trigCondition = 0;
  for( int i=0 ; i < 48; i++ ) {
    trigCondition = ( trigCondition | ( ( trigData.getTrigCondition(i) ) << i ) );
  }
  tof_trigcond   = trigCondition;
  unsigned int trigChannel = 0;
  for( int i=0 ; i < 16; i++ ) {
    trigChannel = ( trigChannel | ( ( trigData.getTrigChannel(i) ) << i ) );
  }
  tof_trigchan   = trigChannel;
  tof_trigwindow = trigData.getTimeWindow();
  tof_trigtype   = trigData.getTimingType();
  tof_estime     = estime;
  tof_t0stat     = t0stat;
  tof_tuple->write();

  return;
}


void TofCheckDigi::Fill_TofTrack(Event::EventHeader& eventHeader, TofTrack*& tof, double estime, int t0stat, RecMdcKalTrackCol& mdcKalTrackCol ) {

  tof_run     = eventHeader.runNumber();
  tof_event   = eventHeader.eventNumber();
  tof_toftrackid = tof->tofTrackId();
  tof_trackid    = tof->trackId();
  RecMdcKalTrackCol::iterator iter = mdcKalTrackCol.begin();
  for( ; iter != mdcKalTrackCol.end(); iter++ ) {
    if( (*iter)->trackId() == tof->trackId() ) {
      double kappa = 10000.0;
      double tanl = -10000.0;

      (*iter)->setPidType( RecMdcKalTrack::electron );
      kappa = (*iter)->kappa();
      tanl  = (*iter)->tanl();
      if( kappa>0 ) { tof_charge = 1; }
      else { tof_charge = -1; }
      if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
      tof_pt[0] = 1.0/kappa;
      tof_p[0]  = abs(sqrt(1.0+tanl*tanl)*tof_pt[0]);

      (*iter)->setPidType( RecMdcKalTrack::muon );
      kappa = (*iter)->kappa();
      tanl  = (*iter)->tanl();
      if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
      tof_pt[1] = 1.0/kappa;
      tof_p[1]  = abs(sqrt(1.0+tanl*tanl)*tof_pt[1]);

      (*iter)->setPidType( RecMdcKalTrack::pion );
      kappa = (*iter)->kappa();
      tanl  = (*iter)->tanl();
      if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
      tof_pt[2] = 1.0/kappa;
      tof_p[2]  = abs(sqrt(1.0+tanl*tanl)*tof_pt[2]);

      (*iter)->setPidType( RecMdcKalTrack::kaon );
      kappa = (*iter)->kappa();
      tanl  = (*iter)->tanl();
      if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
      tof_pt[3] = 1.0/kappa;
      tof_p[3]  = abs(sqrt(1.0+tanl*tanl)*tof_pt[3]);

      (*iter)->setPidType( RecMdcKalTrack::proton );
      kappa = (*iter)->kappa();
      tanl  = (*iter)->tanl();
      if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
      tof_pt[4] = 1.0/kappa;
      tof_p[4]  = abs(sqrt(1.0+tanl*tanl)*tof_pt[4]);

      break;
    }
  }
  tof_id1        = tof->id1();
  tof_id2        = tof->id2();
  tof_istrip1    = tof->istrip1();
  tof_istrip2    = tof->istrip2();
  tof_dstrip1    = tof->dstrip1();
  tof_dstrip2    = tof->dstrip2();
  tof_barrel     = tof->barrel();
  tof_hitcase    = tof->hitCase();
  tof_momentum   = tof->p();
  tof_path1      = tof->path1();
  tof_path2      = tof->path2();
  tof_zrhit1     = tof->zrhit1();
  tof_zrhit2     = tof->zrhit2();
  tof_errzr1     = tof->errzrhit1();
  tof_errzr2     = tof->errzrhit2();
  tof_xhit1      = tof->xhit1();
  tof_yhit1      = tof->yhit1();
  tof_xhit2      = tof->xhit2();
  tof_yhit2      = tof->yhit2();
  for( unsigned int i=0; i<5; i++ ) {
    tof_kal[i]   = tof->kal(i);
    tof_zr1[i]   = tof->zr1(i);
    tof_zr2[i]   = tof->zr2(i);
  }
  tof_ztdc1      = tof->ztdc1();
  tof_ztdc2      = tof->ztdc2();
  tof_zadc1      = tof->zadc1();
  tof_zadc2      = tof->zadc2();
  tof_zt1        = -999.0;
  tof_zt2        = -999.0;
  tof_zt3        = -999.0;
  tof_zt4        = -999.0;
  tof_zq1        = -999.0;
  tof_zq2        = -999.0;
  tof_zq3        = -999.0;
  tof_zq4        = -999.0;
  if( tof->hitCase() == 0 || tof->hitCase() == 2 ) {
    tof_zt1 = tofCaliSvc->ZTDC1( tof->tdc2(), tof->tofId1(), tof->zrhit1() );
    tof_zt2 = tofCaliSvc->ZTDC2( tof->tdc1(), tof->tofId1(), tof->zrhit1() );
    tof_zq1 = tofCaliSvc->ZADC1( tof->adc2(), tof->tofId1(), tof->zrhit1() );
    tof_zq2 = tofCaliSvc->ZADC2( tof->adc1(), tof->tofId1(), tof->zrhit1() );
  }
  if( tof->hitCase() == 1 || tof->hitCase() == 2 ) {
    tof_zt3 = tofCaliSvc->ZTDC1( tof->tdc4(), tof->tofId2(), tof->zrhit2() );
    tof_zt4 = tofCaliSvc->ZTDC2( tof->tdc3(), tof->tofId2(), tof->zrhit2() );
    tof_zq3 = tofCaliSvc->ZADC1( tof->adc4(), tof->tofId2(), tof->zrhit2() );
    tof_zq4 = tofCaliSvc->ZADC2( tof->adc3(), tof->tofId2(), tof->zrhit2() );
  }
  tof_size1      = tof->size1();
  tof_size2      = tof->size2();
  tof_size3      = tof->size3();
  tof_size4      = tof->size4();
  tof_theta1     = tof->theta1();
  tof_theta2     = tof->theta2();
  tof_phi1       = tof->phi1();
  tof_phi2       = tof->phi2();
  tof_quality1   = tof->quality1();
  tof_quality2   = tof->quality2();
  tof_quality    = tof->quality();
  tof_tofid1     = tof->tofId1();
  tof_tofid2     = tof->tofId2();
  tof_strip1     = tof->strip1();
  tof_strip2     = tof->strip2();
  /*
  tof_board1     = tofQElecSvc->Board( tof->hitCase()<3, tof->tofId1(), true );
  tof_board2     = tofQElecSvc->Board( tof->hitCase()<3, tof->tofId2(), true );
  tof_crate1     = tofQElecSvc->Crate( tof->hitCase()<3, tof->tofId1(), true );
  tof_crate2     = tofQElecSvc->Crate( tof->hitCase()<3, tof->tofId2(), true );
  tof_fee1       = tofQElecSvc->Fee( tof->hitCase()<3, tof->tofId1(), true );
  tof_fee2       = tofQElecSvc->Fee( tof->hitCase()<3, tof->tofId2(), true );
  tof_channel1   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId1(),true );
  tof_channel2   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId1(),false );
  tof_channel3   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId2(),true );
  tof_channel4   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId2(),false );
  */
  tof_ph11       = tof->ph11();
  tof_ph12       = tof->ph12();
  tof_ph21       = tof->ph21();
  tof_ph22       = tof->ph22();
  tof_ph1        = tof->ph1();
  tof_ph2        = tof->ph2();
  tof_ph         = tof->ph();
  tof_tofe11     = tof->tof11(0);
  tof_tofe12     = tof->tof12(0);
  tof_tofe21     = tof->tof21(0);
  tof_tofe22     = tof->tof22(0);
  tof_tofe1      = tof->tof1(0);
  tof_tofe2      = tof->tof2(0);
  tof_tofe       = tof->tof(0);
  tof_tofmu11    = tof->tof11(1);
  tof_tofmu12    = tof->tof12(1);
  tof_tofmu21    = tof->tof21(1);
  tof_tofmu22    = tof->tof22(1);
  tof_tofmu1     = tof->tof1(1);
  tof_tofmu2     = tof->tof2(1);
  tof_tofmu      = tof->tof(1);
  tof_tofpi11    = tof->tof11(2);
  tof_tofpi12    = tof->tof12(2);
  tof_tofpi21    = tof->tof21(2);
  tof_tofpi22    = tof->tof22(2);
  tof_tofpi1     = tof->tof1(2);
  tof_tofpi2     = tof->tof2(2);
  tof_tofpi      = tof->tof(2);
  tof_tofk11     = tof->tof11(3);
  tof_tofk12     = tof->tof12(3);
  tof_tofk21     = tof->tof21(3);
  tof_tofk22     = tof->tof22(3);
  tof_tofk1      = tof->tof1(3);
  tof_tofk2      = tof->tof2(3);
  tof_tofk       = tof->tof(3);
  tof_tofp11     = tof->tof11(4);
  tof_tofp12     = tof->tof12(4);
  tof_tofp21     = tof->tof21(4);
  tof_tofp22     = tof->tof22(4);
  tof_tofp1      = tof->tof1(4);
  tof_tofp2      = tof->tof2(4);
  tof_tofp       = tof->tof(4);
  tof_qch1       = tof->qch1();
  tof_qch2       = tof->qch2();
  tof_qch3       = tof->qch3();
  tof_qch4       = tof->qch4();
  tof_adc1       = tof->adc1();
  tof_adc2       = tof->adc2();
  tof_adc3       = tof->adc3();
  tof_adc4       = tof->adc4();
  tof_tdc1       = tof->tdc1();
  tof_tdc2       = tof->tdc2();
  tof_tdc3       = tof->tdc3();
  tof_tdc4       = tof->tdc4();
  tof_texpe1     = tof->texpInner(0);
  tof_texpmu1    = tof->texpInner(1);
  tof_texppi1    = tof->texpInner(2);
  tof_texpk1     = tof->texpInner(3);
  tof_texpp1     = tof->texpInner(4);
  tof_texpe2     = tof->texpOuter(0);
  tof_texpmu2    = tof->texpOuter(1);
  tof_texppi2    = tof->texpOuter(2);
  tof_texpk2     = tof->texpOuter(3);
  tof_texpp2     = tof->texpOuter(4);
  tof_texpe      = tof->texp(0);
  tof_texpmu     = tof->texp(1);
  tof_texppi     = tof->texp(2);
  tof_texpk      = tof->texp(3);
  tof_texpp      = tof->texp(4);
  tof_tdiff1     = tof->tdiff1();
  tof_tdiff2     = tof->tdiff2();
  tof_estime     = estime;
  tof_t0stat     = t0stat;
  tof_tuple->write();

  return;
}


void TofCheckDigi::Fill_TofTrack(Event::EventHeader& eventHeader, TofTrack*& tof, double estime, int t0stat, RecMdcKalTrackCol& mdcKalTrackCol, TofMcHitCol& tofMcCol, McParticleCol& mcParticleCol, std::string calibData ) {

  tof_run     = eventHeader.runNumber();
  tof_event   = eventHeader.eventNumber();
  tof_toftrackid = tof->tofTrackId();
  tof_trackid    = tof->trackId();
  RecMdcKalTrackCol::iterator iter = mdcKalTrackCol.begin();
  for( ; iter != mdcKalTrackCol.end(); iter++ ) {
    if( (*iter)->trackId() == tof->trackId() ) {
      double kappa = 10000.0;
      double tanl = -10000.0;

      (*iter)->setPidType( RecMdcKalTrack::electron );
      kappa = (*iter)->kappa();
      tanl  = (*iter)->tanl();
      if( kappa>0 ) { tof_charge = 1; }
      else { tof_charge = -1; }
      if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
      tof_pt[0] = 1.0/kappa;
      tof_p[0]  = abs(sqrt(1.0+tanl*tanl)*tof_pt[0]);

      (*iter)->setPidType( RecMdcKalTrack::muon );
      kappa = (*iter)->kappa();
      tanl  = (*iter)->tanl();
      if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
      tof_pt[1] = 1.0/kappa;
      tof_p[1]  = abs(sqrt(1.0+tanl*tanl)*tof_pt[1]);

      (*iter)->setPidType( RecMdcKalTrack::pion );
      kappa = (*iter)->kappa();
      tanl  = (*iter)->tanl();
      if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
      tof_pt[2] = 1.0/kappa;
      tof_p[2]  = abs(sqrt(1.0+tanl*tanl)*tof_pt[2]);

      (*iter)->setPidType( RecMdcKalTrack::kaon );
      kappa = (*iter)->kappa();
      tanl  = (*iter)->tanl();
      if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
      tof_pt[3] = 1.0/kappa;
      tof_p[3]  = abs(sqrt(1.0+tanl*tanl)*tof_pt[3]);

      (*iter)->setPidType( RecMdcKalTrack::proton );
      kappa = (*iter)->kappa();
      tanl  = (*iter)->tanl();
      if( abs(kappa)<1.0e-6 ) { kappa = 1.0e-6; }
      tof_pt[4] = 1.0/kappa;
      tof_p[4]  = abs(sqrt(1.0+tanl*tanl)*tof_pt[4]);

      break;
    }
  }
  tof_id1        = tof->id1();
  tof_id2        = tof->id2();
  tof_istrip1    = tof->istrip1();
  tof_istrip2    = tof->istrip2();
  tof_barrel     = tof->barrel();
  tof_hitcase    = tof->hitCase();
  tof_momentum   = tof->p();
  tof_path1      = tof->path1();
  tof_path2      = tof->path2();
  tof_zrhit1     = tof->zrhit1();
  tof_zrhit2     = tof->zrhit2();
  tof_errzr1     = tof->errzrhit1();
  tof_errzr2     = tof->errzrhit2();
  tof_xhit1      = tof->xhit1();
  tof_yhit1      = tof->yhit1();
  tof_xhit2      = tof->xhit2();
  tof_yhit2      = tof->yhit2();
  for( unsigned int i=0; i<5; i++ ) {
    tof_kal[i]   = tof->kal(i);
    tof_zr1[i]   = tof->zr1(i);
    tof_zr2[i]   = tof->zr2(i);
  }
  tof_ztdc1      = tof->ztdc1();
  tof_ztdc2      = tof->ztdc2();
  tof_zadc1      = tof->zadc1();
  tof_zadc2      = tof->zadc2();
  tof_zt1        = -999.0;
  tof_zt2        = -999.0;
  tof_zt3        = -999.0;
  tof_zt4        = -999.0;
  tof_zq1        = -999.0;
  tof_zq2        = -999.0;
  tof_zq3        = -999.0;
  tof_zq4        = -999.0;
  if( tof->hitCase() == 0 || tof->hitCase() == 2 ) {
    tof_zt1 = tofCaliSvc->ZTDC1( tof->tdc2(), tof->tofId1(), tof->zrhit1() );
    tof_zt2 = tofCaliSvc->ZTDC2( tof->tdc1(), tof->tofId1(), tof->zrhit1() );
    tof_zq1 = tofCaliSvc->ZADC1( tof->adc2(), tof->tofId1(), tof->zrhit1() );
    tof_zq2 = tofCaliSvc->ZADC2( tof->adc1(), tof->tofId1(), tof->zrhit1() );
  }
  if( tof->hitCase() == 1 || tof->hitCase() == 2 ) {
    tof_zt3 = tofCaliSvc->ZTDC1( tof->tdc4(), tof->tofId2(), tof->zrhit2() );
    tof_zt4 = tofCaliSvc->ZTDC2( tof->tdc3(), tof->tofId2(), tof->zrhit2() );
    tof_zq3 = tofCaliSvc->ZADC1( tof->adc4(), tof->tofId2(), tof->zrhit2() );
    tof_zq4 = tofCaliSvc->ZADC2( tof->adc3(), tof->tofId2(), tof->zrhit2() );
  }
  tof_size1      = tof->size1();
  tof_size2      = tof->size2();
  tof_size3      = tof->size3();
  tof_size4      = tof->size4();
  tof_theta1     = tof->theta1();
  tof_theta2     = tof->theta2();
  tof_phi1       = tof->phi1();
  tof_phi2       = tof->phi2();
  tof_quality1   = tof->quality1();
  tof_quality2   = tof->quality2();
  tof_quality    = tof->quality();
  tof_tofid1     = tof->tofId1();
  tof_tofid2     = tof->tofId2();
  tof_strip1     = tof->strip1();
  tof_strip2     = tof->strip2();
  /*
  tof_board1     = tofQElecSvc->Board( tof->hitCase()<3, tof->tofId1(), true );
  tof_board2     = tofQElecSvc->Board( tof->hitCase()<3, tof->tofId2(), true );
  tof_crate1     = tofQElecSvc->Crate( tof->hitCase()<3, tof->tofId1(), true );
  tof_crate2     = tofQElecSvc->Crate( tof->hitCase()<3, tof->tofId2(), true );
  tof_fee1       = tofQElecSvc->Fee( tof->hitCase()<3, tof->tofId1(), true );
  tof_fee2       = tofQElecSvc->Fee( tof->hitCase()<3, tof->tofId2(), true );
  tof_channel1   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId1(),true );
  tof_channel2   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId1(),false );
  tof_channel3   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId2(),true );
  tof_channel4   = tofQElecSvc->Channel(tof->hitCase()<3,tof->tofId2(),false );
  */
  tof_ph11       = tof->ph11();
  tof_ph12       = tof->ph12();
  tof_ph21       = tof->ph21();
  tof_ph22       = tof->ph22();
  tof_ph1        = tof->ph1();
  tof_ph2        = tof->ph2();
  tof_ph         = tof->ph();
  tof_tofe11     = tof->tof11(0);
  tof_tofe12     = tof->tof12(0);
  tof_tofe21     = tof->tof21(0);
  tof_tofe22     = tof->tof22(0);
  tof_tofe1      = tof->tof1(0);
  tof_tofe2      = tof->tof2(0);
  tof_tofe       = tof->tof(0);
  tof_tofmu11    = tof->tof11(1);
  tof_tofmu12    = tof->tof12(1);
  tof_tofmu21    = tof->tof21(1);
  tof_tofmu22    = tof->tof22(1);
  tof_tofmu1     = tof->tof1(1);
  tof_tofmu2     = tof->tof2(1);
  tof_tofmu      = tof->tof(1);
  tof_tofpi11    = tof->tof11(2);
  tof_tofpi12    = tof->tof12(2);
  tof_tofpi21    = tof->tof21(2);
  tof_tofpi22    = tof->tof22(2);
  tof_tofpi1     = tof->tof1(2);
  tof_tofpi2     = tof->tof2(2);
  tof_tofpi      = tof->tof(2);
  tof_tofk11     = tof->tof11(3);
  tof_tofk12     = tof->tof12(3);
  tof_tofk21     = tof->tof21(3);
  tof_tofk22     = tof->tof22(3);
  tof_tofk1      = tof->tof1(3);
  tof_tofk2      = tof->tof2(3);
  tof_tofk       = tof->tof(3);
  tof_tofp11     = tof->tof11(4);
  tof_tofp12     = tof->tof12(4);
  tof_tofp21     = tof->tof21(4);
  tof_tofp22     = tof->tof22(4);
  tof_tofp1      = tof->tof1(4);
  tof_tofp2      = tof->tof2(4);
  tof_tofp       = tof->tof(4);
  tof_qch1       = tof->qch1();
  tof_qch2       = tof->qch2();
  tof_qch3       = tof->qch3();
  tof_qch4       = tof->qch4();
  tof_adc1       = tof->adc1();
  tof_adc2       = tof->adc2();
  tof_adc3       = tof->adc3();
  tof_adc4       = tof->adc4();
  tof_tdc1       = tof->tdc1();
  tof_tdc2       = tof->tdc2();
  tof_tdc3       = tof->tdc3();
  tof_tdc4       = tof->tdc4();
  tof_texpe1     = tof->texpInner(0);
  tof_texpmu1    = tof->texpInner(1);
  tof_texppi1    = tof->texpInner(2);
  tof_texpk1     = tof->texpInner(3);
  tof_texpp1     = tof->texpInner(4);
  tof_texpe2     = tof->texpOuter(0);
  tof_texpmu2    = tof->texpOuter(1);
  tof_texppi2    = tof->texpOuter(2);
  tof_texpk2     = tof->texpOuter(3);
  tof_texpp2     = tof->texpOuter(4);
  tof_texpe      = tof->texp(0);
  tof_texpmu     = tof->texp(1);
  tof_texppi     = tof->texp(2);
  tof_texpk      = tof->texp(3);
  tof_texpp      = tof->texp(4);
  tof_tdiff1     = tof->tdiff1();
  tof_tdiff2     = tof->tdiff2();
  tof_estime     = estime;
  tof_t0stat     = t0stat;

  int particleId = 0;
  if( calibData == "Bhabha" )      { particleId =   11; }
  else if( calibData == "Dimu" )   { particleId =   13; }
  else if( calibData == "pion" )   { particleId =  211; }
  else if( calibData == "kaon" )   { particleId =  321; }
  else if( calibData == "proton" ) { particleId = 2212; }
  else {
    cout << "Reconstruction::TofRec::TofCheckDigi::Fill_TofTrack: For MC, Wrong Input Particle ID!" << endl;
  }

  tof_mcp     = -100.0;
  tof_mctofp  = -100.0;
  tof_mctrkid = -100.0;
  tof_mcpath  = -100.0;
  tof_mctexp  = -100.0;
  tof_mczrhit = -1000.0;

  McParticleCol::iterator iter_mc = mcParticleCol.begin();
  for( ; iter_mc != mcParticleCol.end(); iter_mc++ ) {
    int  pid = (*iter_mc)->particleProperty();
    if( abs(pid) != particleId ) continue;
    if( ( particleId==11 || particleId==13 ) && ( ( pid>0 && tof_charge>0 ) || ( pid<0 && tof_charge<0 ) ) ) continue;
    if( ( particleId!=11 && particleId!=13 ) && ( ( pid>0 && tof_charge<0 ) || ( pid<0 && tof_charge>0 ) ) ) continue;

    int trkIndex = (*iter_mc)->trackIndex();
    tof_mcp = (*iter_mc)->initialFourMomentum().rho();

    TofMcHitCol::iterator iter_mc_tof = tofMcCol.begin();
    for( ; iter_mc_tof != tofMcCol.end(); iter_mc_tof++ ) {
      if( trkIndex != (*iter_mc_tof)->getTrackIndex() ) continue;
      double mcposx  = (*iter_mc_tof)->getPositionX();
      double mcposy  = (*iter_mc_tof)->getPositionY();
      double mcposr  = 0.1*sqrt(mcposx*mcposx+mcposy*mcposy);
      double mcposz  = 0.1*(*iter_mc_tof)->getPositionZ();
      double mcpx    = (*iter_mc_tof)->getPx();
      double mcpy    = (*iter_mc_tof)->getPy();
      double mcpz    = (*iter_mc_tof)->getPz();
      double mctofp  = 0.001*sqrt(mcpx*mcpx+mcpy*mcpy+mcpz*mcpz);
      tof_mctofp     = mctofp;
      tof_mctrkid    = (*iter_mc_tof)->getTrackIndex();
      tof_mcpath     = 0.1*(*iter_mc_tof)->getTrackLength();
      tof_mctexp     = (*iter_mc_tof)->getFlightTime();
      if( tof->hitCase()==0 || tof->hitCase()==1 || tof->hitCase()==2 ) {
	tof_mczrhit    = mcposz;
      }
      else if( tof->hitCase()==3 || tof->hitCase()==4 ) {
	tof_mczrhit    = mcposr;
      }
    }
  }

  tof_tuple->write();

  return;
}


void TofCheckDigi::FillCol(Event::EventHeader& eventHeader, RecMdcTrackCol& mdcTrackCol, RecMdcKalTrackCol& mdcKalTrackCol, RecExtTrackCol& recExtTrackCol) {
  if( mdcTrackCol.size()<=0 || mdcKalTrackCol.size() <= 0 || recExtTrackCol.size() <= 0 ) return;
  RecExtTrackCol::iterator iter_ext = recExtTrackCol.begin();
  for( ; iter_ext!=recExtTrackCol.end(); iter_ext++ ) {
    RecMdcKalTrackCol::iterator iter_kal = mdcKalTrackCol.begin();
    for( ; iter_kal!=mdcKalTrackCol.end(); iter_kal++ ) {
      RecMdcTrackCol::iterator iter_mdc = mdcTrackCol.begin();
      for( ; iter_mdc!=mdcTrackCol.end(); iter_mdc++ ) {
	if( (*iter_kal)->getTrackId() != (*iter_mdc)->trackId() ) continue;
	if( (*iter_kal)->getTrackId() == (*iter_ext)->trackId() ) break;
      }
      if( iter_mdc != mdcTrackCol.end() ) {
	Fill( eventHeader, *iter_mdc, *iter_kal, *iter_ext );
      }
    }
  }
  return;
}


void TofCheckDigi::FillCol(Event::EventHeader& eventHeader, RecExtTrackCol& extTrackCol, RecMdcTrackCol& mdcTrackCol, RecEmcShowerCol& emcShowerCol, RecMdcKalTrackCol& mdcKalTrackCol) {
  bb_run     = eventHeader.runNumber();
  bb_event   = eventHeader.eventNumber();
  bb_trksize = extTrackCol.size();
  bb_mdcsize = mdcTrackCol.size();
  bb_emcsize = emcShowerCol.size();

  RecMdcTrackCol::iterator iter_mdc1 = mdcTrackCol.begin();
  RecMdcTrackCol::iterator iter_mdc2 = mdcTrackCol.begin() + 1;

  Hep3Vector p1 = (*iter_mdc1)->p3();
  Hep3Vector p2 = (*iter_mdc2)->p3();

  bb_trk1    = (*iter_mdc1)->trackId();
  bb_pmdc1   = (*iter_mdc1)->p();
  bb_ptmdc1  = (*iter_mdc1)->pxy();
  bb_theta1  = (*iter_mdc1)->theta();
  bb_phi1    = (*iter_mdc1)->phi();
  bb_x1      = (*iter_mdc1)->x();
  bb_y1      = (*iter_mdc1)->y();
  bb_z1      = (*iter_mdc1)->z();
  bb_r1      = (*iter_mdc1)->r();
  bb_charge1 = (*iter_mdc1)->charge();
  bb_chi1    = (*iter_mdc1)->chi2();
  bb_ndof1   = (*iter_mdc1)->ndof();
  bb_layer1  = (*iter_mdc1)->lastLayer();
  bb_trk2    = (*iter_mdc2)->trackId();
  bb_pmdc2   = (*iter_mdc2)->p();
  bb_ptmdc2  = (*iter_mdc2)->pxy();
  bb_theta2  = (*iter_mdc2)->theta();
  bb_phi2    = (*iter_mdc2)->phi();
  bb_x2      = (*iter_mdc2)->x();
  bb_y2      = (*iter_mdc2)->y();
  bb_z2      = (*iter_mdc2)->z();
  bb_r2      = (*iter_mdc2)->r();
  bb_charge2 = (*iter_mdc2)->charge();
  bb_chi2    = (*iter_mdc2)->chi2();
  bb_ndof2   = (*iter_mdc2)->ndof();
  bb_layer2  = (*iter_mdc2)->lastLayer();

  bb_dang = 180.0 - p1.angle( p2.unit() )*180.0/pi;
  bb_dphi = abs( (*iter_mdc1)->phi() - (*iter_mdc2)->phi() )*180.0/pi;

  RecMdcKalTrackCol::iterator iter_kal1 = mdcKalTrackCol.begin();
  RecMdcKalTrackCol::iterator iter_kal2 = mdcKalTrackCol.begin() + 1;

  if( ( (*iter_kal1)->trackId() == (*iter_mdc2)->trackId() ) && ( (*iter_kal2)->trackId() == (*iter_mdc1)->trackId() ) ) {
    RecMdcKalTrackCol::iterator iter_tmp = iter_kal1;
    iter_kal1 = iter_kal2;
    iter_kal2 = iter_tmp;
  }

  //  RecMdcKalTrack::setPidType  (RecMdcKalTrack::electron);
  (*iter_kal1)->setPidType( RecMdcKalTrack::electron );
  (*iter_kal2)->setPidType( RecMdcKalTrack::electron );
  double tanl1  = (*iter_kal1)->tanl();
  double kappa1 = (*iter_kal1)->kappa();
  double tanl2  = (*iter_kal2)->tanl();
  double kappa2 = (*iter_kal2)->kappa();
  bb_pe1  = sqrt(1.0+tanl1*tanl1)/kappa1;
  bb_pte1 = 1.0/kappa1;
  bb_pe2  = sqrt(1.0+tanl2*tanl2)/kappa2;
  bb_pte2 = 1.0/kappa2;
  //  RecMdcKalTrack::setPidType  (RecMdcKalTrack::muon);
  (*iter_kal1)->setPidType( RecMdcKalTrack::muon );
  (*iter_kal2)->setPidType( RecMdcKalTrack::muon );
  tanl1  = (*iter_kal1)->tanl();
  kappa1 = (*iter_kal1)->kappa();
  tanl2  = (*iter_kal2)->tanl();
  kappa2 = (*iter_kal2)->kappa();
  bb_pmu1  = sqrt(1.0+tanl1*tanl1)/kappa1;
  bb_ptmu1 = 1.0/kappa1;
  bb_pmu2  = sqrt(1.0+tanl2*tanl2)/kappa2;
  bb_ptmu2 = 1.0/kappa2;

  RecExtTrackCol::iterator iter_ext1 = extTrackCol.begin();
  RecExtTrackCol::iterator iter_ext2 = extTrackCol.begin() + 1;

  Hep3Vector extPos1 = (*iter_ext1)->emcPosition();
  Hep3Vector extPos2 = (*iter_ext2)->emcPosition();

  bb_extx1   = extPos1.x();
  bb_exty1   = extPos1.y();
  bb_extz1   = extPos1.z();
  bb_extx2   = extPos2.x();
  bb_exty2   = extPos2.y();
  bb_extz2   = extPos2.z();

  RecEmcShowerCol::iterator iter_emc1 = emcShowerCol.begin();
  RecEmcShowerCol::iterator iter_emc2 = emcShowerCol.begin() + 1;

  Hep3Vector emcPos1((*iter_emc1)->x(),(*iter_emc1)->y(),(*iter_emc1)->z());
  Hep3Vector emcPos2((*iter_emc2)->x(),(*iter_emc2)->y(),(*iter_emc2)->z());

  //  cout << "ext 1 --- " << extPos1 << " ext 2 --- " << extPos2 << 
  //    "emc 1 --- " << emcPos1 << " emc 2 --- " << emcPos2 << endl;

  Hep3Vector dis1 = extPos1 - emcPos1;
  Hep3Vector dis2 = extPos2 - emcPos1;

  //  cout << " r1 = " << dis1.r() << " r2 = " << dis2.r() << endl;

  if( dis1.r() > dis2.r() ) {
    RecEmcShowerCol::iterator iter_tmp = iter_emc1;
    iter_emc1 = iter_emc2;
    iter_emc2 = iter_tmp;
    Hep3Vector emc_tmp = emcPos1;
    emcPos1 = emcPos2;
    emcPos2 = emc_tmp;
  }
  dis1 = extPos1 - emcPos1;
  dis2 = extPos2 - emcPos2;

  bb_emctrk1 = (*iter_emc1)->trackId();
  bb_emcx1   = (*iter_emc1)->x();
  bb_emcy1   = (*iter_emc1)->y();
  bb_emcz1   = (*iter_emc1)->z();
  bb_ep1     = (*iter_emc1)->energy()/(*iter_mdc1)->p();;
  bb_th1     = (*iter_emc1)->theta();
  bb_ph1     = (*iter_emc1)->phi();
  bb_emctrk2 = (*iter_emc2)->trackId();
  bb_emcx2   = (*iter_emc2)->x();
  bb_emcy2   = (*iter_emc2)->y();
  bb_emcz2   = (*iter_emc2)->z();
  bb_ep2     = (*iter_emc2)->energy()/(*iter_mdc2)->p();;
  bb_th2     = (*iter_emc2)->theta();
  bb_ph2     = (*iter_emc2)->phi();
  bb_dr1     = dis1.r();
  bb_drxy1   = dis1.rho();
  bb_dz1     = dis1.z();
  bb_dr2     = dis2.r();
  bb_drxy2   = dis2.rho();
  bb_dz2     = dis2.z();

  double etot = 0.0;
  RecEmcShowerCol::iterator iter_emc = emcShowerCol.begin();
  for( ; iter_emc != emcShowerCol.end(); iter_emc++ ) {
    etot += (*iter_emc)->energy();
  }
  bb_etot = etot - (*iter_emc1)->energy() - (*iter_emc2)->energy();

  bb_tuple->write();

  return;
}
