#include "MdcCalibAlg/MdcCalib.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IService.h"

#include "EventModel/Event.h"
#include "MdcRawEvent/MdcDigi.h"
#include "McTruth/MdcMcHit.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "EventModel/EventHeader.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "TStyle.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>

using namespace Event;
using namespace std;

typedef map<int, int>::value_type valType3;
typedef std::vector<HepLorentzVector> Vp4;

MdcCalib::MdcCalib(){
     m_nEvt=0;
     m_cut1=0;
     m_cut2=0;
     m_cut3=0;
     m_cut4=0;
     m_cut5=0;
     m_cut6=0;
     m_nGrPoint = 0;
     fgReadWireEff = false;

     int lay;
     for(lay=0; lay<43; lay++){
	  if(lay < 15) m_nEntr[lay] = 1;
	  else m_nEntr[lay] = 2;
     }
     m_dwid = 0.5;		// mm
     m_fgIni = false;

     m_phiWid = PI2 / (double)NPhiBin;
     m_theWid = 2.0 / (double)NThetaBin;

     m_nEvtNtuple = 0;

     for(lay=0; lay<MdcCalNLayer; lay++){
	  if(lay < 8) m_nBin[lay] = 12;
	  else m_nBin[lay] = 16;
     }

     // setting boundary layer flags
     for(lay=0; lay<MdcCalNLayer; lay++){
	  if((0==lay) || (7==lay) || (8==lay) || (19==lay) || (20==lay) ||
	     (35==lay) || (36==lay) || (42==lay) ) m_layBound[lay] = true;
	  else m_layBound[lay] = false;
     }
}

MdcCalib::~MdcCalib(){
}

void MdcCalib::clear(){
     int lay;
     for(lay=0; lay<m_nlayer; lay++){
	  delete m_htraw[lay];
	  delete m_htdr[lay];
	  delete m_hresInc[lay];
	  delete m_hresExc[lay];
	  delete m_hresAve[lay];
	  delete m_hadc[lay];
	  for (int lr=0; lr<2; lr++){
	       delete m_htdrlr[lay][lr];
	       delete m_hreslrInc[lay][lr];
	       delete m_hreslrExc[lay][lr];
	       delete m_hreslrAve[lay][lr];
	  }
     }

     delete m_effNtrk;
     delete m_effNtrkRecHit;
     delete m_hresAllInc;
     delete m_hresAllExc;
     delete m_hresAllAve;
     for(int i=0; i<14; i++){
	  delete m_hresAveAllQ[i];
	  delete m_hresAveOutQ[i];
     }
     for(lay=0; lay<43; lay++){
	  for(int i=0; i<14; i++) delete m_hresAveLayQ[lay][i];
     }
     delete m_hresInnInc;
     delete m_hresInnExc;
     delete m_hresStpInc;
     delete m_hresStpExc;
     delete m_hresOutInc;
     delete m_hresOutExc;
     for(int iEs=0; iEs<m_param.nEsFlag; iEs++) delete m_hTes[iEs];
     delete m_hbbTrkFlg;
     delete m_hTesAll;
     delete m_hTesGood;
     delete m_hTesAllFlag;
     delete m_hTesRec;
     delete m_hTesCalFlag;
     delete m_hTesCalUse;
     delete m_hnRawHit;
     delete m_hpt;
     delete m_hpMax;
     delete m_hpMaxCms;
     delete m_hptPos;
     delete m_hptNeg;
     delete m_hp;
     delete m_hp_cms;
     delete m_hpPos;
     delete m_hpNeg;
     delete m_hpPoscms;
     delete m_hpNegcms;
     delete m_hp_cut;
     delete m_hchisq;
     delete m_hnTrk;
     delete m_hnTrkCal;
     delete m_hnhitsRec;
     delete m_hnhitsRecInn;
     delete m_hnhitsRecStp;
     delete m_hnhitsRecOut;
     delete m_hnhitsCal;
     delete m_hnhitsCalInn;
     delete m_hnhitsCalStp;
     delete m_hnhitsCalOut;
     delete m_wirehitmap;
     delete m_layerhitmap;
     delete m_hnoisephi;
     delete m_hnoiselay;
     delete m_hnoisenhits;
     delete m_hratio;
     delete m_hdr;
     delete m_hphi0;
     delete m_hkap;
     delete m_hdz;
     delete m_htanl;
     delete m_hcosthe;
     delete m_hcostheNeg;
     delete m_hcosthePos;
     delete m_hx0;
     delete m_hy0;
     delete m_hdelZ0;
     delete m_grX0Y0;
     delete m_hitEffAll;
     delete m_hitEffRaw;
     delete m_hitEffRec;
     int bin;
     int thbin;
     for(bin=0; bin<NPhiBin; bin++){
	  delete m_ppPhi[bin];
	  delete m_pnPhi[bin];
	  delete m_ppPhiCms[bin];
	  delete m_pnPhiCms[bin];
	  for(thbin=0; thbin<NThetaBin; thbin++){
	       delete m_ppThePhi[thbin][bin];
	       delete m_pnThePhi[thbin][bin];
	       delete m_ppThePhiCms[thbin][bin];
	       delete m_pnThePhiCms[thbin][bin];
	  }
     }
     for(thbin=0; thbin<NThetaBin; thbin++){
	  delete m_ppThe[thbin];
	  delete m_pnThe[thbin];
	  delete m_ppTheCms[thbin];
	  delete m_pnTheCms[thbin];
     }

     for(unsigned i=0; i<m_hr2dInc.size(); i++){
	  delete m_hr2dInc[i];
	  delete m_hr2dExc[i];
     }
     m_hr2dInc.clear();
     m_hr2dExc.clear();
     m_mapr2d.clear();

     delete m_fdTime;
     delete m_fdAdc;
     delete m_fdres;
     delete m_fdresAve;
     delete m_fdres2d;
     delete m_fdcom;
     delete m_fdResQ;
}

void MdcCalib::initialize(TObjArray* hlist, IMdcGeomSvc* mdcGeomSvc,
			  IMdcCalibFunSvc* mdcFunSvc, IMdcUtilitySvc* mdcUtilitySvc) {
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalib");
     log << MSG::INFO << "MdcCalib::initialize()" << endreq;

     m_hlist = hlist;
     m_mdcGeomSvc = mdcGeomSvc;
     m_mdcFunSvc = mdcFunSvc;
     m_mdcUtilitySvc = mdcUtilitySvc;

     int lay;
     int iEntr;
     int lr;
     int bin;
     char hname[200];

     m_nlayer = m_mdcGeomSvc -> getLayerSize();

     for(lay=0; lay<m_nlayer; lay++){
	  m_radii[lay] = m_mdcGeomSvc->Layer(lay)->Radius();
     }
     ofstream fwpc("wirelog.txt");
     for(int wir=0; wir<MdcCalTotCell; wir++){
	  m_xe[wir] = m_mdcGeomSvc->Wire(wir)->Backward().x();
	  m_ye[wir] = m_mdcGeomSvc->Wire(wir)->Backward().y();
	  m_ze[wir] = m_mdcGeomSvc->Wire(wir)->Backward().z();
	  m_xw[wir] = m_mdcGeomSvc->Wire(wir)->Forward().x();
	  m_yw[wir] = m_mdcGeomSvc->Wire(wir)->Forward().y();
	  m_zw[wir] = m_mdcGeomSvc->Wire(wir)->Forward().z();
	  fwpc << setw(6) << wir << setw(15) << m_xe[wir] << setw(15) << m_ye[wir]
	       << setw(15) << m_ze[wir] << setw(15) << m_xw[wir]
	       << setw(15) << m_yw[wir] << setw(15) << m_zw[wir] << endl;
     }
     fwpc.close();

     m_fdcom = new TFolder("common", "common");
     m_hlist -> Add(m_fdcom);

     m_effNtrk = new TH1F("effNtrk", "", 43, -0.5, 42.5);
     m_fdcom->Add(m_effNtrk);

     m_effNtrkRecHit = new TH1F("effNtrkRecHit", "", 43, -0.5, 42.5);
     m_fdcom->Add(m_effNtrkRecHit);

     m_hresAllInc = new TH1F("HResAllInc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresAllInc);

     m_hresAllExc = new TH1F("HResAllExc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresAllExc);

     m_hresAllAve = new TH1F("HResAllAve", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresAllAve);

     m_hresInnInc = new TH1F("HResInnInc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresInnInc);

     m_hresInnExc = new TH1F("HResInnExc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresInnExc);

     m_hresStpInc = new TH1F("HResStpInc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresStpInc);

     m_hresStpExc = new TH1F("HResStpExc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresStpExc);

     m_hresOutInc = new TH1F("HResOutInc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresOutInc);

     m_hresOutExc = new TH1F("HResOutExc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresOutExc);

     m_fdResQ = new TFolder("ResQ", "ResQ");
     m_hlist->Add(m_fdResQ);
     for(int i=0; i<14; i++){
	  sprintf(hname, "resoAll_qbin%02d", i);
	  m_hresAveAllQ[i] = new TH1F(hname, "", 200, -1, 1);
	  m_fdResQ->Add(m_hresAveAllQ[i]);

	  sprintf(hname, "resoOut_qbin%02d", i);
	  m_hresAveOutQ[i] = new TH1F(hname, "", 200, -1, 1);
	  m_fdResQ->Add(m_hresAveOutQ[i]);
     }
     for(lay=0; lay<43; lay++){
	  for(int i=0; i<14; i++){
	       sprintf(hname, "resoLay%02d_qbin%02d", lay, i);
	       m_hresAveLayQ[lay][i] = new TH1F(hname, "", 200, -1, 1);
	       m_fdResQ->Add(m_hresAveLayQ[lay][i]);
	  }
     }

     for(int iEs=0; iEs<m_param.nEsFlag; iEs++){
	  sprintf(hname, "Tes_%d", m_param.esFlag[iEs]);
	  m_hTes[iEs] = new TH1F(hname, "", 750, 0, 1500);
	  m_fdcom->Add(m_hTes[iEs]);
     }

     m_hbbTrkFlg = new TH1F("BbTrkFlg", "", 100, 0, 6);
     m_fdcom -> Add(m_hbbTrkFlg);

     m_hTesAll = new TH1F("TesAll", "", 1000, 0, 2000);
     m_fdcom -> Add(m_hTesAll);

     m_hTesGood = new TH1F("TesGood", "", 1000, 0, 2000);
     m_fdcom -> Add(m_hTesGood);

     m_hTesAllFlag = new TH1F("TesAllFlag", "", 300, -0.5, 299.5);
     m_fdcom -> Add(m_hTesAllFlag);

     m_hTesRec = new TH1F("TesRec", "", 1000, 0, 2000);
     m_fdcom -> Add(m_hTesRec);

     m_hTesCalFlag = new TH1F("TesCalFlag", "", 1000, 0, 2000);
     m_fdcom -> Add(m_hTesCalFlag);

     m_hTesCalUse = new TH1F("TesCalUse", "", 1000, 0, 2000);
     m_fdcom -> Add(m_hTesCalUse);

     m_hnRawHit = new TH1F("nRawHit", "", 6797, -0.5, 6796.5);
     m_fdcom -> Add(m_hnRawHit);

     m_hpt = new TH1F("HPt", "", 800, 0, 3);
     m_fdcom -> Add(m_hpt);

     m_hptPos = new TH1F("HPtPos", "", 800, 0, 3);
     m_fdcom -> Add(m_hptPos);

     m_hptNeg = new TH1F("HPtNeg", "", 800, 0, 3);
     m_fdcom -> Add(m_hptNeg);

     m_hp = new TH1F("HP", "", 800, 0, 3);
     m_fdcom -> Add(m_hp);

     m_hp_cms = new TH1F("HPCMS", "", 800, 0, 3);
     m_fdcom -> Add(m_hp_cms);

     m_hpMax = new TH1F("HPMax", "", 800, 0, 3);
     m_fdcom -> Add(m_hpMax);

     m_hpMaxCms = new TH1F("HPMax_Cms", "", 800, 0, 3);
     m_fdcom -> Add(m_hpMaxCms);

     m_hpPos = new TH1F("HP_Pos", "", 800, 0, 3);
     m_fdcom -> Add(m_hpPos);

     m_hpNeg = new TH1F("HP_Neg", "", 800, 0, 3);
     m_fdcom -> Add(m_hpNeg);

     m_hpPoscms = new TH1F("HP_Pos_cms", "", 800, 0, 3);
     m_fdcom -> Add(m_hpPoscms);

     m_hpNegcms = new TH1F("HP_Neg_cms", "", 800, 0, 3);
     m_fdcom -> Add(m_hpNegcms);

     m_hp_cut = new TH1F("HPCut", "", 800, 0, 3);
     m_fdcom -> Add(m_hp_cut);

     m_hchisq = new TH1F("Chisq", "", 10, 0, 100);
     m_fdcom -> Add(m_hchisq);

     m_hnTrk = new TH1F("HNtrack", "HNtrack", 10, -0.5, 9.5);
     m_fdcom -> Add(m_hnTrk);

     m_hnTrkCal = new TH1F("HNtrackCal", "HNtrackCal", 10, -0.5, 9.5);
     m_fdcom -> Add(m_hnTrkCal);

     m_hnhitsRec = new TH1F("HNhitsRec", "", 100, -0.5, 99.5);
     m_fdcom -> Add(m_hnhitsRec);

     m_hnhitsRecInn = new TH1F("HNhitsInnRec", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsRecInn);

     m_hnhitsRecStp = new TH1F("HNhitsStpRec", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsRecStp);

     m_hnhitsRecOut = new TH1F("HNhitsOutRec", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsRecOut);

     m_hnhitsCal = new TH1F("HNhitsCal", "", 100, -0.5, 99.5);
     m_fdcom -> Add(m_hnhitsCal);

     m_hnhitsCalInn = new TH1F("HNhitsCalInn", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsCalInn);

     m_hnhitsCalStp = new TH1F("HNhitsCalStp", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsCalStp);

     m_hnhitsCalOut = new TH1F("HNhitsCalOut", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsCalOut);

     m_wirehitmap = new TH1F("Wire_HitMap", "Wire_HitMap", 6796, -0.5, 6795.5);
     m_fdcom -> Add(m_wirehitmap);

     m_layerhitmap = new TH1F("Layer_HitMap", "Layer_HitMap", 43, -0.5, 42.5);
     m_fdcom -> Add(m_layerhitmap);

     m_hnoisephi = new TH1F("phi_noise", "", 100, 0, 6.284);
     m_fdcom -> Add(m_hnoisephi);

     m_hnoiselay = new TH1F("Layer_noise", "Layer_noise", 43, -0.5, 42.5);
     m_fdcom -> Add(m_hnoiselay);

     m_hnoisenhits = new TH1F("nhits_noise", "nhits_noise", 6796, -0.5, 6795.5);
     m_fdcom -> Add(m_hnoisenhits);

     m_hratio = new TH1F("ratio", "", 100, 0, 1);
     m_fdcom -> Add(m_hratio);

     m_hdr = new TH1F("dr", "", 500, -500, 500);
     m_fdcom -> Add(m_hdr);

     m_hphi0 = new TH1F("phi0", "", 100, 0, 6.284);
     m_fdcom -> Add(m_hphi0);

     m_hkap = new TH1F("kappa", "", 400, -50, 50);
     m_fdcom -> Add(m_hkap);

     m_hdz = new TH1F("dz", "", 500, -1000, 1000);
     m_fdcom -> Add(m_hdz);

     m_htanl = new TH1F("tanl", "", 200, -5, 5);
     m_fdcom -> Add(m_htanl);

     m_hcosthe = new TH1F("costheta", "", 200, -1, 1);
     m_fdcom -> Add(m_hcosthe);

     m_hcostheNeg = new TH1F("costhetaNeg", "", 200, -1, 1);
     m_fdcom -> Add(m_hcostheNeg);

     m_hcosthePos = new TH1F("costhetaPos", "", 200, -1, 1);
     m_fdcom -> Add(m_hcosthePos);

     m_hx0 = new TH1F("x0", "", 100, -10, 10);
     m_fdcom -> Add(m_hx0);

     m_hy0 = new TH1F("y0", "", 100, -10, 10);
     m_fdcom -> Add(m_hy0);

     m_hdelZ0 = new TH1F("delta_z0", "", 100, -50, 50);
     m_fdcom -> Add(m_hdelZ0);

     m_grX0Y0 = new TGraph();
     m_grX0Y0->SetName("x0y0");
     m_fdcom -> Add(m_grX0Y0);

     m_hitEffAll = new TH1F("hitEffAll", "", 6800, -0.5, 6799.5);
     m_fdcom -> Add(m_hitEffAll);

     m_hitEffRaw = new TH1F("hitEffRaw", "", 6800, -0.5, 6799.5);
     m_fdcom -> Add(m_hitEffRaw);

     m_hitEffRec = new TH1F("hitEffRec", "", 6800, -0.5, 6799.5);
     m_fdcom -> Add(m_hitEffRec);

     // histograms for drift time
     m_fdTime = new TFolder("time", "time");
     m_hlist -> Add(m_fdTime);

     for(lay=0; lay<m_nlayer; lay++){
	  sprintf(hname, "Traw%02d", lay);
	  m_htraw[lay] = new TH1F(hname, "", 1000, 0, 2000);
	  m_fdTime -> Add(m_htraw[lay]);

	  sprintf(hname, "Tdr%02d", lay);
	  m_htdr[lay] = new TH1F(hname, "", 510, -10, 500);
	  m_fdTime -> Add(m_htdr[lay]);

	  for (lr=0; lr<2; lr++){
	       sprintf(hname, "Tdr%02d_lr%01d", lay, lr);
	       m_htdrlr[lay][lr] = new TH1F(hname, "", 510, -10, 500);
	       m_fdTime -> Add(m_htdrlr[lay][lr]);
	  }
     }

     // histograms of adc
     m_fdAdc = new TFolder("adc", "adc");
     m_hlist -> Add(m_fdAdc);

     for(lay=0; lay<m_nlayer; lay++){
	  sprintf(hname, "adc%02d", lay);
	  m_hadc[lay] = new TH1F(hname, "", 1500, 0, 3000);
	  m_fdAdc -> Add(m_hadc[lay]);
     }
     // histograms for resolution
     m_fdres = new TFolder("resolution", "resolution");
     m_hlist -> Add(m_fdres);

     m_fdresAve = new TFolder("resAve", "resAve");
     m_hlist -> Add(m_fdresAve);

     for(lay=0; lay<m_nlayer; lay++){
	  sprintf(hname, "Reso%02dInc", lay);
	  m_hresInc[lay] = new TH1F(hname, "", 1000, -5, 5);
	  m_fdres -> Add(m_hresInc[lay]);

	  sprintf(hname, "Reso%02dExc", lay);
	  m_hresExc[lay] = new TH1F(hname, "", 1000, -5, 5);
	  m_fdres -> Add(m_hresExc[lay]);

	  sprintf(hname, "Reso%02d", lay);
	  m_hresAve[lay] = new TH1F(hname, "", 1000, -5, 5);
	  m_fdresAve -> Add(m_hresAve[lay]);

	  for (lr=0; lr<2; lr++){
	       sprintf(hname, "Reso%02dInc_lr%01d", lay, lr);
// 	       m_hreslrInc[lay][lr] = new TH1F(hname, "", 200, -1, 1);
	       m_hreslrInc[lay][lr] = new TH1F(hname, "", 1000, -5, 5);
	       m_fdres->Add(m_hreslrInc[lay][lr]);

	       sprintf(hname, "Reso%02dExc_lr%01d", lay, lr);
// 	       m_hreslrExc[lay][lr] = new TH1F(hname, "", 200, -1, 1);
	       m_hreslrExc[lay][lr] = new TH1F(hname, "", 1000, -5, 5);
	       m_fdres->Add(m_hreslrExc[lay][lr]);

	       sprintf(hname, "Reso%02d_lr%01d", lay, lr);
// 	       m_hreslrAve[lay][lr] = new TH1F(hname, "", 200, -1, 1);
	       m_hreslrAve[lay][lr] = new TH1F(hname, "", 1000, -5, 5);
	       m_fdresAve->Add(m_hreslrAve[lay][lr]);
	  }
	  for(int phi=0; phi<20; phi++){
	       sprintf(hname, "ResoPhi%02d_phi%02d", lay, phi);
	       m_hresphi[lay][phi] = new TH1F(hname, "", 200, -1, 1);
	       m_fdres->Add(m_hresphi[lay][phi]);
	  }
     }

     /* histograms for momentum vs phi */
     m_fdmomPhi = new TFolder("momPhi", "momPhi");
     m_hlist -> Add(m_fdmomPhi);

     int thbin;
     for(bin=0; bin<NPhiBin; bin++){
	  sprintf(hname, "hPpos_phi%02d", bin);
	  m_ppPhi[bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_ppPhi[bin]);

	  sprintf(hname, "hPneg_phi%02d", bin);
	  m_pnPhi[bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_pnPhi[bin]);

	  sprintf(hname, "hPpos_phi_cms%02d", bin);
	  m_ppPhiCms[bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_ppPhiCms[bin]);

	  sprintf(hname, "hPneg_phi_cms%02d", bin);
	  m_pnPhiCms[bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_pnPhiCms[bin]);

	  for(thbin=0; thbin<NThetaBin; thbin++){
	       sprintf(hname, "hPpos_theta%02d_phi%02d", thbin, bin);
	       m_ppThePhi[thbin][bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	       m_fdmomPhi->Add(m_ppThePhi[thbin][bin]);

	       sprintf(hname, "hPneg_theta%02d_phi%02d", thbin, bin);
	       m_pnThePhi[thbin][bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	       m_fdmomPhi->Add(m_pnThePhi[thbin][bin]);

	       sprintf(hname, "hPposCms_theta%02d_phi%02d", thbin, bin);
	       m_ppThePhiCms[thbin][bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	       m_fdmomPhi->Add(m_ppThePhiCms[thbin][bin]);

	       sprintf(hname, "hPnegCms_theta%02d_phi%02d", thbin, bin);
	       m_pnThePhiCms[thbin][bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	       m_fdmomPhi->Add(m_pnThePhiCms[thbin][bin]);
	  }
     }
     for(thbin=0; thbin<NThetaBin; thbin++){
	  sprintf(hname, "hPpos_the%02d", thbin);
	  m_ppThe[thbin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_ppThe[thbin]);

	  sprintf(hname, "hPneg_the%02d", thbin);
	  m_pnThe[thbin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_pnThe[thbin]);

	  sprintf(hname, "hPposCms_the%02d", thbin);
	  m_ppTheCms[thbin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_ppTheCms[thbin]);

	  sprintf(hname, "hPnegCms_the%02d", thbin);
	  m_pnTheCms[thbin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_pnTheCms[thbin]);
     }

     // histograms for resolution vs distance
     m_fdres2d = new TFolder("res2d", "res2d");
     m_hlist -> Add(m_fdres2d);

     int hid = 0;
     int key;
     TH1F* hist;
     for(lay=0; lay<m_nlayer; lay++){
	  for(iEntr=0; iEntr<MdcCalNENTRSD; iEntr++){
	       for(lr=0; lr<2; lr++){
		    for(bin=0; bin<MdcCalSdNBIN; bin++){
			 sprintf(hname, "r2d%02d_%02d_%01d_%02dInc", lay, iEntr, lr, bin);
			 hist = new TH1F(hname, "", 200, -1, 1);
			 m_hr2dInc.push_back(hist);
			 m_fdres2d -> Add(hist);

			 sprintf(hname, "r2d%02d_%02d_%01d_%02dExc", lay, iEntr, lr, bin);
			 hist = new TH1F(hname, "", 200, -1, 1);
			 m_hr2dExc.push_back(hist);
			 m_fdres2d -> Add(hist);

			 key = getHresId(lay, iEntr, lr, bin);
			 m_mapr2d.insert( valType3(key, hid) );
			 hid++;
		    }
	       }
	  }
     } // end of layer loop

     m_fdres2t = new TFolder("res2t", "res2t");
//      m_hlist -> Add(m_fdres2t);

     for(lay=0; lay<m_nlayer; lay++){
	  for(iEntr=0; iEntr<MdcCalNENTRXT; iEntr++){
	       for(lr=0; lr<2; lr++){
		    for(bin=0; bin<45; bin++){
			 sprintf(hname, "r2t%02d_%02d_%01d_%02d", lay, iEntr, lr, bin);
			 m_hr2t[lay][iEntr][lr][bin] = new TH1F(hname, "", 600, -3, 3);
			 m_fdres2t -> Add(m_hr2t[lay][iEntr][lr][bin]);
		    }
	       }
	  }
     }

     INTupleSvc* ntupleSvc;
     Gaudi::svcLocator() -> service("NTupleSvc", ntupleSvc);
     for(lay=0; lay<m_nlayer; lay++){
	  sprintf(hname, "FILE136/xt%02d", lay);
	  NTuplePtr nt(ntupleSvc, hname);
	  if ( nt ) m_xtTuple[lay] = nt;
	  else{
	       m_xtTuple[lay] = ntupleSvc->book(hname, CLID_ColumnWiseTuple, "MdcXtNtuple");
	       if( m_xtTuple[lay] ){
		    m_xtTuple[lay]->addItem("cel", m_cel[lay]);
		    m_xtTuple[lay]->addItem("lr", m_lr[lay]);
		    m_xtTuple[lay]->addItem("run", m_run[lay]);
		    m_xtTuple[lay]->addItem("evt", m_evt[lay]);
		    m_xtTuple[lay]->addItem("doca", m_doca[lay]);
		    m_xtTuple[lay]->addItem("dm", m_dm[lay]);
		    m_xtTuple[lay]->addItem("tdr", m_tdr[lay]);
		    m_xtTuple[lay]->addItem("tdc", m_tdc[lay]);
		    m_xtTuple[lay]->addItem("entr", m_entr[lay]);
		    m_xtTuple[lay]->addItem("zhit", m_zhit[lay]);
		    m_xtTuple[lay]->addItem("qhit", m_qhit[lay]);
		    m_xtTuple[lay]->addItem("p", m_p[lay]);
		    m_xtTuple[lay]->addItem("pt", m_pt[lay]);
		    m_xtTuple[lay]->addItem("phi0", m_phi0[lay]);
		    m_xtTuple[lay]->addItem("tanl", m_tanl[lay]);
		    m_xtTuple[lay]->addItem("hitphi", m_hitphi[lay]);
	       } else{
		    log << MSG::FATAL << "Cannot book Xt N-tuple:"
			<< long(m_xtTuple[lay]) << endreq;
	       }
	  }
     }

     if(5==m_param.particle){
	  sprintf(hname, "FILE136/cosmic");
	  NTuplePtr nt(ntupleSvc, hname);
	  if ( nt ) m_cosmic = nt;
	  else{
	       m_cosmic = ntupleSvc->book(hname, CLID_ColumnWiseTuple, "MdcXtNtuple");
	       if( m_cosmic ){
		    m_cosmic->addItem("pUp", m_pUpcos);
		    m_cosmic->addItem("pDw", m_pDwcos);
		    m_cosmic->addItem("ptUp", m_ptUpcos);
		    m_cosmic->addItem("ptDw", m_ptDwcos);
		    m_cosmic->addItem("phiUp", m_phiUpcos);
		    m_cosmic->addItem("phiDw", m_phiDwcos);
		    m_cosmic->addItem("drUp", m_drUpcos);
		    m_cosmic->addItem("drDw", m_drDwcos);
		    m_cosmic->addItem("dzUp", m_dzUpcos);
		    m_cosmic->addItem("dzDw", m_dzDwcos);
		    m_cosmic->addItem("ctheUp", m_ctheUpcos);
		    m_cosmic->addItem("ctheDw", m_ctheDwcos);
		    m_cosmic->addItem("nhitUp", m_nhitUpcos);
		    m_cosmic->addItem("nhitDw", m_nhitDwcos);
		    m_cosmic->addItem("char", m_chargecos);
		    m_cosmic->addItem("tesfg", m_tesFlagcos);
		    m_cosmic->addItem("tes", m_tescos);
	       }
	  }
     }
}

int MdcCalib::fillHist(MdcCalEvent* event){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalib");
     log << MSG::DEBUG << "MdcCalib::fillHist()" << endreq;

     int i;
     int k;
     int lay;
     int cel;
     int wir;
     int lr;
     int stat;

     int hid;
     int key;
     int iEntr;
     int bin;

     int phiBin;
     int phiBinCms;
     int theBin;
     int theBinCms;
     double lamda;
     double theta;

     double qhit;
     double traw;
     double tdr;
     double doca;
     double resiInc;
     double resiExc;
     double entr;
     double pt;
     double p;
     double p_cms;
     double chisq;
     double ecm = m_param.ecm;
     double xboost = m_param.boostPar[0] * ecm;
     double yboost = m_param.boostPar[1];
     double zboost = m_param.boostPar[2];
     HepLorentzVector p4;

     double dr;
     double phi0;
     double dz;
     double kap;
     double tanl;

     double x0;
     double y0;
     double zminus = 9999.0;
     double zplus = -9999.0;

     double hitphi;
     double philab;
     double phicms;
     double thetacms;
     double costheCMS;

     double dphi;
     double wphi;
     double xx;
     double yy;
     double rr;

     int nhitlay;
     bool fgHitLay[MdcCalNLayer];
     bool fgTrk;

     int ntrk = event -> getNTrk();
     int nhitRec;
     int nhitRecInn;
     int nhitRecStp;
     int nhitRecOut;
     int nhitCal;
     int nhitCalInn;
     int nhitCalStp;
     int nhitCalOut;
     MdcCalRecTrk* rectrk;
     MdcCalRecHit* rechit;

     int fgAdd[43];		// for calculating layer efficiency

     // read dead wire
     if(!fgReadWireEff){
	  for(lay=0; lay<MdcCalNLayer; lay++){
	       int ncel = m_mdcGeomSvc->Layer(lay)->NCell();
	       for(cel=0; cel<ncel; cel++){
		    double eff = m_mdcFunSvc->getWireEff(lay, cel);
		    if(eff > 0.5) m_fgGoodWire[lay][cel] = true;
		    else m_fgGoodWire[lay][cel] = false;
		    if(eff<0.9) cout << "dead channel: " << setw(5) << lay << setw(5) << cel << endl;
	       }
	  }
	  fgReadWireEff = true;
     }

     int nRawHit = event->getNRawHitTQ();
     m_hnRawHit->Fill(nRawHit);

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

     SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");
     if (!eventHeader) {
	  log << MSG::FATAL << "Could not find Event Header" << endreq;
	  return( StatusCode::FAILURE);
     }
     int iRun = eventHeader->runNumber();
     int iEvt = eventHeader->eventNumber();

     int esTimeflag = event->getEsFlag();
     double tes = event->getTes();
     bool esCutFg = event->getEsCutFlag();
     int iEs = event->getNesCutFlag();
     //calculate the efficiency of Bhabha event
     if (-1 != esTimeflag) {
	  SmartDataPtr<RecMdcTrackCol> newtrkCol(eventSvc, "/Event/Recon/RecMdcTrackCol");
	  if(!newtrkCol){
	       log << MSG::ERROR << "Could not find RecMdcTrackCol" << endreq;
	       return ( StatusCode::FAILURE );
	  }
	  int nGoodTrk = 0;
	  int icharge = 0;
	  Vp4 p4p;
	  Vp4 p4m;
	  RecMdcTrackCol::iterator it_trk = newtrkCol->begin();
	  for(; it_trk != newtrkCol->end(); it_trk++){
	       double mass = 0.000511; // only for eletron
	       HepLorentzVector p4 = (*it_trk)->p4(mass);
	       icharge = (*it_trk)->charge();
	       if (icharge > 0) p4p.push_back(p4);
	       else p4m.push_back(p4);
	  }
	  if (1 == p4p.size() * p4m.size()){
	       double dang = p4p[0].vect().angle(p4m[0].vect());
	       m_hbbTrkFlg->Fill(1);
	       if (dang > 2.94) {
		    m_hbbTrkFlg->Fill(2);
	       }
	  }

     }
     m_hTesAll->Fill(tes);
//      cout << "tes " << tes << endl;
     if (-1 != esTimeflag) m_hTesGood->Fill(tes);
     m_hTesAllFlag->Fill(esTimeflag);
     if(ntrk > 0) m_hTesRec->Fill(tes);
     if( (iEs >= 0) && (iEs < m_param.nEsFlag) ) m_hTes[iEs]->Fill(tes);
     if( esCutFg ) m_hTesCalFlag->Fill(tes);
     else return -1;

     if(! m_fgIni){
	  for(lay=0; lay<MdcCalNLayer; lay++){
	       if(lay < 8) m_docaMax[lay] = m_param.maxDocaInner;
	       else m_docaMax[lay] = m_param.maxDocaOuter;
	  }
	  m_fgIni = true;
     }

     bool trkFlag[200];		// for calculating hit efficiency without impact of track fitting
     for(i=0; i<200; i++) trkFlag[i] = false;

     int ntrkCal = 0;
     double pTrk[2];
     double pTrkcms[2];
     double hitphiplus = 9999.0;
     double hitthetaplus = 9999.0;
     double hitphiminus = -9999.0;
     double hitthetaminus = -9999.0;
     Vp4 pp;
     Vp4 pm;
     pp.clear();
     pm.clear();
     int phibinp;
     int phibinm;

     m_hnTrk->Fill(ntrk);
     if((ntrk < m_param.nTrkCut[0]) || (ntrk > m_param.nTrkCut[1])){
	  m_cut1++;
	  return -1;
     }
//      if(ntrk > 2) return -1;
     for(i=0; i<ntrk; i++){
	  fgTrk = true;
	  rectrk = event -> getRecTrk(i);
	  nhitRec = rectrk -> getNHits();
	  m_hnhitsRec -> Fill( nhitRec );

	  for(lay=0; lay<MdcCalNLayer; lay++){
	       fgHitLay[lay] = false;
	  }

	  nhitRecInn = 0;
	  nhitRecStp = 0;
	  nhitRecOut = 0;
	  for(k=0; k<nhitRec; k++){
	       rechit = rectrk -> getRecHit(k);
	       lay = rechit -> getLayid();
	       doca = rechit -> getDocaExc();
	       resiExc = rechit -> getResiExc();
	       fgHitLay[lay] = true;

	       if(lay < 8) nhitRecInn++;
	       else if(lay < 20) nhitRecStp++;
	       else nhitRecOut++;
	  }
	  m_hnhitsRecInn->Fill(nhitRecInn);
	  m_hnhitsRecStp->Fill(nhitRecStp);
	  m_hnhitsRecOut->Fill(nhitRecOut);

	  // get momentum
	  pt = rectrk -> getPt();
	  p = rectrk -> getP();

	  // boost P to the cms
	  p4 = rectrk->getP4();
	  HepLorentzVector psip(xboost, yboost, zboost, ecm);
	  Hep3Vector boostv = psip.boostVector();
	  p4.boost(- boostv);
	  p_cms = p4.rho();
	  phicms = p4.phi();
	  if(phicms < 0) phicms += PI2;
	  thetacms = p4.theta();
	  costheCMS = cos(thetacms);
	  if (pt < 0) p_cms *= -1.0;
	  p4.boost(boostv);
// 	  cout << setw(15) << p << setw(15) << p_cms << setw(15) << costheCMS << endl;

	  // cos(theta) cut
	  if( (costheCMS < m_param.costheCut[0]) || (costheCMS > m_param.costheCut[1]) ){
	       m_cut2++;
	       continue;
	  }

	  // dr cut
	  dr = rectrk->getDr();
	  if(fabs(dr) > m_param.drCut){
	       m_cut3++;
	       continue;
	  }

	  // dz cut
	  dz = rectrk->getDz();
	  if(fabs(dz) > m_param.dzCut){
	       m_cut4++;
	       continue;
	  }

// 	  if(! fgTrk) continue;

	  // hit layer cut
	  nhitlay = 0;
	  for(lay=0; lay<MdcCalNLayer; lay++){
	       if(fgHitLay[lay]) nhitlay++;
	  }
	  if(nhitlay < m_param.nHitLayCut){
	       m_cut5++;
	       continue;
	  }

	  // nhit cut
	  if(nhitRec < m_param.nHitCut){
	       m_cut6++;
	       continue;
	  }

// 	  bool fgNoise = rectrk->getFgNoiseRatio();
// 	  if(m_param.noiseCut && (!fgNoise)) continue;
// 	  cout << setw(10) << iRun << setw(15) << iEvt << setw(5) << fgNoise << endl;

// 	  if(! ((fgHitLay[0]||fgHitLay[1]) && (fgHitLay[41]||fgHitLay[42])) ){
// 	       continue;
// 	  }

	  // calculate cell on the track
	  int cellTrkPass[43];
	  bool fgPass = getCellTrkPass(event, i, cellTrkPass);
	  for(lay=0; lay<m_nlayer; lay++){
	       fgAdd[lay] = 0;
// 	       if((16==lay) || (18==lay) || (19==lay) || (41==lay)){  // hv2200 2009-3
	       if((15==lay) || (16==lay) || (18==lay) || (19==lay) || (40==lay) || (41==lay) || (42==lay)){
		    int iCell = cellTrkPass[lay];
		    if(fgPass && (iCell >= 0) && m_fgGoodWire[lay][iCell]) m_effNtrk->Fill(lay);
		    else fgAdd[lay] = 1;
	       } else{
		    m_effNtrk->Fill(lay);
	       }
	  }

	  chisq = rectrk -> getChisq();
	  m_hchisq -> Fill( chisq );

	  if(pt > 0){
	       m_hpt -> Fill(pt);
	       m_hptPos -> Fill(pt);
	       m_hp -> Fill(p);
	       m_hp_cms -> Fill(p_cms);
	       m_hpPos -> Fill(p);
	       m_hpPoscms -> Fill(p_cms);
	  } else{
	       m_hpt -> Fill(-1.0*pt);
	       m_hptNeg -> Fill(-1.0*pt);
	       m_hp -> Fill(-1.0*p);
	       m_hp_cms -> Fill(-1.0*p_cms);
	       m_hpNeg -> Fill(-1.0*p);
	       m_hpNegcms -> Fill(-1.0*p_cms);
	  }
	  if(2 == ntrk){
	       pTrk[i] = fabs(p);
	       pTrkcms[i] = fabs(p_cms);
	  }

	  dr = rectrk -> getDr();
	  phi0 = rectrk -> getPhi0();
	  kap = rectrk -> getKappa();
	  dz = rectrk -> getDz();
	  tanl = rectrk -> getTanLamda();
	  lamda = atan(tanl);
	  theta = HFPI - lamda;

	  m_hdr -> Fill(dr);
	  m_hphi0 -> Fill(phi0);
	  m_hkap -> Fill(kap);
	  m_hdz -> Fill(dz);
	  m_htanl -> Fill(tanl);
	  m_hcosthe -> Fill(cos(theta));
	  if(pt > 0) m_hcosthePos->Fill(cos(theta));
	  else m_hcostheNeg->Fill(cos(theta));

	  philab = phi0 + HFPI;
	  if(philab > PI2) philab -= PI2;
// 	  cout << setw(15) << phi0 << setw(15) << philab << setw(15) << phicms << endl;

	  phiBin = (int)(philab / m_phiWid);
	  phiBinCms = (int)(phicms / m_phiWid);
	  theBin = (int)((cos(theta) + 1.0) / m_theWid);
	  theBinCms = (int)((cos(thetacms) + 1.0) / m_theWid);
	  if(phiBin < 0) phiBin = 0;
	  if(phiBin >= NPhiBin) phiBin = NPhiBin-1;
	  if(phiBinCms < 0) phiBinCms = 0;
	  if(phiBinCms >= NPhiBin) phiBinCms = NPhiBin-1;
	  if(theBin < 0) theBin = 0;
	  if(theBin >= NThetaBin) theBin = NThetaBin-1;
	  if(theBinCms < 0) theBinCms = 0;
	  if(theBinCms >= NThetaBin) theBinCms = NThetaBin-1;

	  if(pt > 0){
	       m_ppPhi[phiBin]->Fill(p);
	       m_ppPhiCms[phiBinCms]->Fill(p_cms);
	       m_ppThe[theBin]->Fill(p);
	       m_ppTheCms[theBinCms]->Fill(p_cms);
	       m_ppThePhi[theBin][phiBin]->Fill(p);
	       m_ppThePhiCms[theBinCms][phiBinCms]->Fill(p_cms);
	  } else{
	       m_pnPhi[phiBin]->Fill(-1.0*p);
	       m_pnPhiCms[phiBinCms]->Fill(-1.0*p_cms);
	       m_pnThe[theBin]->Fill(-1.0*p);
	       m_pnTheCms[theBinCms]->Fill(-1.0*p_cms);
	       m_pnThePhi[theBin][phiBin]->Fill(-1.0*p);
	       m_pnThePhiCms[theBinCms][phiBinCms]->Fill(-1.0*p_cms);
	  }

	  x0 = dr * cos(phi0);
	  y0 = dr * sin(phi0);
	  m_hx0 -> Fill(x0);
	  m_hy0 -> Fill(y0);
	  if(m_nGrPoint < 10000){
	       m_grX0Y0->SetPoint(m_nGrPoint, x0, y0);
	       m_nGrPoint++;
	  }

	  if(kap < 0) {
	       zminus = dz;
	       pm.push_back(p4);
	       phibinm = phiBinCms;
	  } else {
	       zplus = dz;
	       pp.push_back(p4);
	       phibinp = phiBinCms;
	  }

// 	  cout << "phi = " << setw(15) << philab << setw(15) << philab*180./3.14159 << setw(15) << p << endl;
	  ntrkCal++;
	  trkFlag[i] = true;
	  nhitCal = 0;
	  nhitCalInn = 0;
	  nhitCalStp = 0;
	  nhitCalOut = 0;
	  for(k=0; k<nhitRec; k++){
	       rechit    = rectrk -> getRecHit(k);

	       lay    = rechit -> getLayid();
	       cel    = rechit -> getCellid();
	       lr     = rechit -> getLR();
	       stat   = rechit -> getStat();
	       doca   = rechit -> getDocaExc();
	       resiInc = rechit -> getResiIncLR();
	       resiExc = rechit -> getResiExcLR();
	       entr    = rechit -> getEntra();
	       tdr     = rechit -> getTdrift();
	       traw    = (rechit -> getTdc()) * MdcCalTdcCnv;
	       wir     = m_mdcGeomSvc -> Wire(lay, cel) -> Id();

	       m_cel[lay] = (long)cel;
	       m_lr[lay] = (long)lr;
	       m_run[lay] = iRun;
	       m_evt[lay] = iEvt;
	       m_doca[lay] = doca;
	       m_dm[lay] = rechit -> getDmeas();
	       m_tdr[lay] = tdr;
	       m_tdc[lay] = traw;
	       m_entr[lay] = entr*180.0/3.14;
	       m_zhit[lay] = rechit -> getZhit();
	       m_qhit[lay] = rechit -> getQhit();
	       m_p[lay] = p;
	       m_pt[lay] = pt;
	       m_phi0[lay] = phi0;
	       m_tanl[lay] = tanl;
	       qhit = rechit -> getQhit();

	       // calculating hitphi
	       xx = (m_zhit[lay] - m_zw[wir]) * (m_xe[wir] - m_xw[wir]) / 
		    (m_ze[wir] - m_zw[wir]) + m_xw[wir];
	       yy = (m_zhit[lay] - m_zw[wir]) * (m_ye[wir] - m_yw[wir]) / 
		    (m_ze[wir] - m_zw[wir]) + m_yw[wir];
	       rr = sqrt( (xx * xx) + (yy * yy) );
	       dphi = fabs(doca) / m_radii[lay];

	       if( yy >= 0 )  wphi = acos(xx / rr);
	       else           wphi = PI2 - acos(xx / rr);
	       if(1 == lr)    hitphi = wphi + dphi; // mention
	       else hitphi = wphi - dphi;
	       if(hitphi < 0) hitphi += PI2;
	       else if(hitphi > PI2) hitphi -= PI2;

	       m_hitphi[lay] = hitphi;

	       if( (fabs(doca) > m_docaMax[lay]) || 
		   (fabs(resiExc) > m_param.resiCut[lay]) ){
		    continue;
	       }

	       if(m_param.fgAdjacLayerCut){
		    if(0 == lay){
			 if( ! fgHitLay[1] ) continue;
		    } else if(42 == lay){
			 if( ! fgHitLay[41] ) continue;
		    } else{
			 if( (!fgHitLay[lay-1]) && (!fgHitLay[lay+1]) ) continue;

			 // for boundary layers
			 if( m_param.fgBoundLayerCut && m_layBound[lay] && 
			     ((!fgHitLay[lay-1]) || (!fgHitLay[lay+1])) ) continue;
		    }
	       }

	       if((1 == m_param.hitStatCut) && (1 != stat)) continue;

	       // fill xtplot tree
	       if((1 == m_param.fillNtuple) && (m_nEvtNtuple < m_param.nEvtNtuple)){
		    m_xtTuple[lay] -> write();
	       }

	       if(1 == m_param.hitStatCut){
		    if( (0 == fgAdd[lay]) && (1 == stat) ){
			 m_effNtrkRecHit->Fill(lay);
			 fgAdd[lay] = 1;
		    }
	       } else{
		    if(0 == fgAdd[lay]){
			 m_effNtrkRecHit->Fill(lay);
			 fgAdd[lay] = 1;
		    }
	       }

	       nhitCal++;
	       if(lay < 8) nhitCalInn++;
	       else if(lay < 20) nhitCalStp++;
	       else nhitCalOut++;

	       m_wirehitmap -> Fill(wir);
	       m_layerhitmap -> Fill(lay);

	       m_htraw[lay] -> Fill(traw);
	       m_htdr[lay] -> Fill(tdr);
	       m_htdrlr[lay][lr]->Fill(tdr);
	       m_hadc[lay] -> Fill(qhit);

	       m_hresAllInc -> Fill(resiInc);
	       m_hresAllExc -> Fill(resiExc);
	       double resiAve = 0.5 * (resiInc + resiExc);
	       m_hresAllAve -> Fill(resiAve);

	       if(lay < 8){
		    m_hresInnInc -> Fill(resiInc);
		    m_hresInnExc -> Fill(resiExc);
	       } else if(lay < 20){
		    m_hresStpInc -> Fill(resiInc);
		    m_hresStpExc -> Fill(resiExc);
	       } else{
		    m_hresOutInc -> Fill(resiInc);
		    m_hresOutExc -> Fill(resiExc);
	       }

	       int qbin = (int)((qhit-100.0)/100.0);
	       if(qbin>=0 && qbin<14){
		    m_hresAveAllQ[qbin]->Fill(resiAve);
		    m_hresAveLayQ[lay][qbin]->Fill(resiAve);
		    if(lay > 7) m_hresAveOutQ[qbin]->Fill(resiAve);
	       }

	       m_hresInc[lay] -> Fill(resiInc);
	       m_hreslrInc[lay][lr]->Fill(resiInc);
	       m_hresExc[lay] -> Fill(resiExc);
	       m_hreslrExc[lay][lr]->Fill(resiExc);
	       m_hresAve[lay] -> Fill(resiAve);
	       m_hreslrAve[lay][lr]->Fill(resiAve);

	       int iPhi = (int)(hitphi*20.0/PI2);
	       if(iPhi>=20) iPhi = 19;
	       m_hresphi[lay][iPhi]->Fill((resiInc+resiExc)*0.5);

// 	       bin = (int)(fabs(doca) / m_dwid);
	       bin = (int)(fabs(rechit->getDmeas()) / m_dwid);
	       iEntr = m_mdcFunSvc -> getSdEntrIndex(entr);
	       if(1 == m_nEntr[lay]){
		    iEntr = 0;
	       } else if(2 == m_nEntr[lay]){
		    if(entr > 0.0) iEntr = 1;
		    else iEntr = 0;
	       }
	       if((iEntr < MdcCalNENTRSD) && (bin < MdcCalSdNBIN)){
		    key = getHresId(lay, iEntr, lr, bin);
		    if( 1 == m_mapr2d.count(key) ){
			 hid = m_mapr2d[key];
			 m_hr2dInc[hid] -> Fill(resiInc);
			 m_hr2dExc[hid] -> Fill(resiExc);
		    }
	       }

	       if((tdr>0) && (tdr<750)){
		    if(tdr<300) bin = (int)(tdr/10.0);
		    else bin = (int)((tdr-300.0)/30.0) + 29;
		    m_hr2t[lay][iEntr][lr][bin]->Fill(resiExc);
	       }
	  } // loop of nhits
	  m_nEvtNtuple++;
	  m_hnhitsCal->Fill(nhitCal);
	  m_hnhitsCalInn->Fill(nhitCalInn);
	  m_hnhitsCalStp->Fill(nhitCalStp);
	  m_hnhitsCalOut->Fill(nhitCalOut);
     } // end of track loop
     m_hnTrkCal->Fill(ntrkCal);
     if(2 == ntrkCal){
	  if(pTrk[0] > pTrk[1]) m_hpMax->Fill(pTrk[0]);
	  else m_hpMax->Fill(pTrk[1]);

	  if(pTrkcms[0] > pTrkcms[1]) m_hpMaxCms->Fill(pTrkcms[0]);
	  else m_hpMaxCms->Fill(pTrkcms[1]);
     }
     if(ntrkCal > 0) m_hTesCalUse->Fill(tes);

     double delZ0;
     if((fabs(zminus) < 9000.0) && (fabs(zplus) < 9000.0)) delZ0 = zplus - zminus;
     m_hdelZ0 -> Fill(delZ0);

     if (1 == pp.size() * pm.size()){
	  HepLorentzVector ptot = pp[0] + pm[0];
	  bool fourmomcut = false;
// 	  fourmomcut = (ptot.x()>0.02 && ptot.x()<0.06) && (fabs(ptot.y()) < 0.02)
// 	       && (ptot.z()>-0.01 && ptot.z()<0.03) && (ptot.e()>3.4 && ptot.e()<4.0);
          fourmomcut = (fabs(ptot.x()-0.04)<0.026) && (fabs(ptot.y()) < 0.026)
               && (fabs(ptot.z()-0.005)<0.036) && (fabs(ptot.e()-ecm)<0.058);
	  //cout << "x = " << ptot.x() << ", y = " << ptot.y() << ", z = " << ptot.z() << ", e = " << ptot.e() << endl;
	  if (fourmomcut) {
	       HepLorentzVector psip(xboost, yboost, zboost, ecm);
	       Hep3Vector boostv = psip.boostVector();
	       pp[0].boost(- boostv);
	       pm[0].boost(- boostv);
	       m_hp_cut->Fill(pp[0].rho());
	       m_hp_cut->Fill(pm[0].rho());
	  }
     }

     if(2==ntrk) for(i=0; i<ntrk; i++) pTrk[i] = (event -> getRecTrk(i)) -> getP();
     if((5==m_param.particle) && (2==ntrk) && (fabs(pTrk[0])<5) && (fabs(pTrk[1])<5)){
//      if(1==ntrk) p = (event->getRecTrk(0)) -> getP();
//      if((5==m_param.particle) && (1==ntrk) && (fabs(p)<5)){
	  m_tescos = tes;
	  m_tesFlagcos = esTimeflag;
	  for(i=0; i<ntrk; i++){
	       rectrk = event -> getRecTrk(i);
	       phi0 = rectrk -> getPhi0();
	       phi0 = ((phi0+HFPI) > PI2) ? (phi0+HFPI-PI2) : (phi0+HFPI);

	       tanl = rectrk -> getTanLamda();
	       lamda = atan(tanl);
	       theta = HFPI - lamda;

	       if(phi0 < (2.0*HFPI)){
		    m_nhitUpcos = rectrk -> getNHits();
		    m_pUpcos = rectrk -> getP();
		    m_ptUpcos = rectrk -> getPt();
		    m_phiUpcos = phi0;
		    m_drUpcos = rectrk->getDr();
		    m_dzUpcos = rectrk->getDz();
		    m_ctheUpcos = cos(theta);
	       } else{
		    m_nhitDwcos = rectrk -> getNHits();
		    m_pDwcos = rectrk -> getP();
		    m_ptDwcos = rectrk -> getPt();
		    m_phiDwcos = phi0;
		    m_drDwcos = rectrk->getDr();
		    m_dzDwcos = rectrk->getDz();
		    m_ctheDwcos = cos(theta);

		    if(m_pDwcos > 0) m_chargecos = 1;
		    else m_chargecos = 0;
	       }
	  }
	  m_cosmic->write();
     }

     if(1 == m_param.fgCalDetEffi) calDetEffi();

     return 1;
}

int MdcCalib::updateConst(MdcCalibConst* calconst){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalib");
     log << MSG::DEBUG << "MdcCalib::updateConst()" << endreq;

     cout << "Tot " << m_hnTrk->GetEntries()
	  << ",  nTrkCut " << m_cut1 << ",  cos(theta)_cut " << m_cut2 << ",  drCut " << m_cut3
	  << ",  dzCut " << m_cut4 << ",  nHitLayer_cut " << m_cut5 << ",  nHit_cut " << m_cut6 << endl;

     int lay;
     double effi;
     double effErr;

     int nGoodAll = 0;
     int nGoodInn = 0;
     int nGoodStp = 0;
     int nGoodOut = 0;
     int nTotAll = 0;
     int nTotInn = 0;
     int nTotStp = 0;
     int nTotOut = 0;
     ofstream feffi("MdcLayerEffi.dat");
     for(lay=0; lay<m_nlayer; lay++){
	  double effNtrk = m_effNtrk->GetBinContent(lay+1);
	  double effGoodHit = m_effNtrkRecHit->GetBinContent(lay+1);
	  nGoodAll += effGoodHit;
	  if(lay < 8) nGoodInn += effGoodHit;
	  else if (lay < 20) nGoodStp += effGoodHit;
	  else nGoodOut += effGoodHit;

	  nTotAll += effNtrk;
	  if(lay < 8) nTotInn += effNtrk;
	  else if (lay < 20) nTotStp += effNtrk;
	  else nTotOut += effNtrk;

	  effi = (double)effGoodHit / (double)effNtrk;
	  effErr = sqrt(effi * (1-effi) / (double)effNtrk);
	  feffi << setw(5) << lay << setw(15) << effi << setw(15) << effErr
		<< setw(15) << effGoodHit << setw(15) << effNtrk << endl;
     }
     double effiAll = (double)nGoodAll / (double)(nTotAll);
     double errAll = sqrt(effiAll * (1-effiAll) / (double)(nTotAll));
     double effiInn = (double)nGoodInn / (double)(nTotInn);
     double errInn = sqrt(effiInn * (1-effiInn) / (double)(nTotInn));
     double effiStp = (double)nGoodStp / (double)(nTotStp);
     double errStp = sqrt(effiStp * (1-effiStp) / (double)(nTotStp));
     double effiOut = (double)nGoodOut / (double)(nTotOut);
     double errOut = sqrt(effiOut * (1-effiOut) / (double)(nTotOut));
     feffi << endl << "EffiAll: " << setw(15) << effiAll << setw(15) << errAll
	   << setw(15) << nGoodAll << setw(15) << nTotAll << endl;
     feffi << endl << "EffiInn: " << setw(15) << effiInn << setw(15) << errInn
	   << setw(15) << nGoodInn << setw(15) << nTotInn << endl;
     feffi << endl << "EffiStp: " << setw(15) << effiStp << setw(15) << errStp
	   << setw(15) << nGoodStp << setw(15) << nTotStp << endl;
     feffi << endl << "EffiOut: " << setw(15) << effiOut << setw(15) << errOut
	   << setw(15) << nGoodOut << setw(15) << nTotOut << endl;
     feffi.close();

     // calculate efficiency without the impact of track fitting
     if(0 != m_param.fgCalDetEffi){
	  int nHitAll[] = {0, 0};
	  int nHitInn[] = {0, 0};
	  int nHitStp[] = {0, 0};
	  int nHitOut[] = {0, 0};
	  ofstream feff2("MdcHitEffi.dat");
	  for(lay=0; lay<m_nlayer; lay++){
	       nHitAll[0] += m_hitNum[lay][0];
	       nHitAll[1] += m_hitNum[lay][1];
	       if(lay < 8){
		    nHitInn[0] += m_hitNum[lay][0];
		    nHitInn[1] += m_hitNum[lay][1];
	       } else if (lay < 20){
		    nHitStp[0] += m_hitNum[lay][0];
		    nHitStp[1] += m_hitNum[lay][1];
	       } else{
		    nHitOut[0] += m_hitNum[lay][0];
		    nHitOut[1] += m_hitNum[lay][1];
	       }

	       effi = (double)m_hitNum[lay][1] / (double)m_hitNum[lay][0];
	       effErr = sqrt(effi * (1-effi) / (double)m_hitNum[lay][0]);
	       feff2 << setw(5) << lay << setw(15) << effi << setw(15) << effErr
		     << setw(15) << m_hitNum[lay][1] << setw(15) << m_hitNum[lay][0] << endl;
	  }
	  effiAll = (double)nHitAll[1] / (double)nHitAll[0];
	  errAll = sqrt(effiAll * (1-effiAll)) / (double)nHitAll[0];
	  effiInn = (double)nHitInn[1] / (double)nHitInn[0];
	  errInn = sqrt(effiInn * (1-effiInn)) / (double)nHitInn[0];
	  effiStp = (double)nHitStp[1] / (double)nHitStp[0];
	  errStp = sqrt(effiStp * (1-effiStp)) / (double)nHitStp[0];
	  effiOut = (double)nHitOut[1] / (double)nHitOut[0];
	  errOut = sqrt(effiOut * (1-effiOut)) / (double)nHitOut[0];
	  feff2 << endl << "EffiAll: " << setw(15) << effiAll << setw(15) << errAll
		<< setw(15) << nHitAll[1] << setw(15) << nHitAll[0] << endl;
	  feff2 << endl << "EffiInn: " << setw(15) << effiInn << setw(15) << errInn
		<< setw(15) << nHitInn[1] << setw(15) << nHitInn[0] << endl;
	  feff2 << endl << "EffiStp: " << setw(15) << effiStp << setw(15) << errStp
		<< setw(15) << nHitStp[1] << setw(15) << nHitStp[0] << endl;
	  feff2 << endl << "EffiOut: " << setw(15) << effiOut << setw(15) << errOut
		<< setw(15) << nHitOut[1] << setw(15) << nHitOut[0] << endl;
	  feff2.close();
     }

     // get resolution
     int i;
     int iEntr;
     int lr;
     int bin;
     int key;
     int hid;

     Stat_t entry;
     double sigm[MdcCalSdNBIN];
     if(m_param.calSigma){
	  ofstream fr2d("logr2d.dat");
	  for(lay=0; lay<m_nlayer; lay++){
	       for(iEntr=0; iEntr<m_nEntr[lay]; iEntr++){
		    for(lr=0; lr<2; lr++){
			 fr2d << setw(3) << lay << setw(3) << iEntr << setw(3) << lr << endl;
			 for(bin=0; bin<m_nBin[lay]; bin++){
			      key = getHresId(lay, iEntr, lr, bin);
			      hid = m_mapr2d[key];

			      if(1 == m_param.resiType){
				   entry = m_hr2dExc[hid] -> GetEntries();
				   if(entry > 500){
					m_hr2dExc[hid] -> Fit("gaus", "Q");
					sigm[bin] = m_hr2dExc[hid]->GetFunction("gaus")->GetParameter(2);
				   } else if(entry > 100){
					sigm[bin] = m_hr2dExc[hid]->GetRMS();
				   } else{
					sigm[bin] = 0.2;
				   }
			      } else{
				   entry = m_hr2dInc[hid] -> GetEntries();
				   if(entry > 500){
					m_hr2dInc[hid] -> Fit("gaus", "Q");
					sigm[bin] = m_hr2dInc[hid]->GetFunction("gaus")->GetParameter(2);
				   } else if(entry > 100){
					sigm[bin] = m_hr2dInc[hid]->GetRMS();
				   } else{
					sigm[bin] = 0.2;
				   }
			      }
			      if(sigm[bin] < 0.05) sigm[bin] = 0.05;	// for boundary layers
			 } // end of bin loop

			 for(bin=m_nBin[lay]; bin<MdcCalSdNBIN; bin++){
			      sigm[bin] = sigm[m_nBin[lay]-1];
			 }

			 for(bin=0; bin<MdcCalSdNBIN; bin++){
			      if(1 == m_param.fgCalib[lay]){
// 			      calconst -> resetSdpar(lay, iEntr, lr, bin, sigm[bin]);
				   if(1 == m_nEntr[lay]){
					for(i=0; i<6; i++) calconst -> resetSdpar(lay, i, lr, bin, sigm[bin]);
				   } else if(2 == m_nEntr[lay]){
					if(0 == iEntr){
					     for(i=0; i<3; i++){ // entr<0
						  calconst -> resetSdpar(lay, i, lr, bin, sigm[bin]);
					     }
					} else{
					     for(i=3; i<6; i++){ // entr>0
						  calconst -> resetSdpar(lay, i, lr, bin, sigm[bin]);
					     }
					}
				   }
			      } else{
				   sigm[bin] = calconst->getSdpar(lay, iEntr, lr, bin);
			      }
			      fr2d << setw(5) << bin << setw(15) << sigm[bin] << endl;
			 } // end of bin loop
		    }
	       } // end of entr loop
	  }
	  fr2d.close();
     }

     return 1;
}

int MdcCalib::getHresId(int lay, int entr, int lr, int bin) const{
     int index = ( (lay << HRESLAYER_INDEX) & HRESLAYER_MASK ) |
	  ( (entr << HRESENTRA_INDEX) & HRESENTRA_MASK ) |
	  ( (lr << HRESLR_INDEX) & HRESLR_MASK ) |
	  ( (bin << HRESBIN_INDEX) & HRESBIN_MASK );
     return index;
}

int MdcCalib::calDetEffi(){
     IMessageSvc* msgSvc;
     Gaudi::svcLocator() -> service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalib");

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
     SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc,"/Event/Digi/MdcDigiCol");
     if (!mdcDigiCol) {
          log << MSG::FATAL << "Could not find event" << endreq;
     }

     int lay;
     int cel;
     bool hitCel[43][288];
     int hitCel2[43][288];
     for(lay=0; lay<43; lay++){
	  for(cel=0; cel<288; cel++){
	       hitCel[lay][cel] = false;
	       hitCel2[lay][cel] = 0;
	  }
     }

     MdcDigiCol::iterator iter = mdcDigiCol->begin();
     unsigned fgOverFlow;
     int digiId = 0;
     Identifier id; 
     for(; iter != mdcDigiCol->end(); iter++, digiId++) {
          MdcDigi *aDigi = (*iter);
          id = (aDigi)->identify();

          lay = MdcID::layer(id);
          cel = MdcID::wire(id);
	  fgOverFlow = (aDigi) -> getOverflow();

// 	  if ( !((fgOverFlow == 0)||(fgOverFlow ==12)) ||
// 	       (aDigi->getTimeChannel() == 0x7FFFFFFF) ||
// 	       (aDigi->getChargeChannel() == 0x7FFFFFFF) ) continue;
	  if ( ((fgOverFlow & 3) !=0 ) || ((fgOverFlow & 12) != 0) ||
	       (aDigi->getTimeChannel() == 0x7FFFFFFF) ||
	       (aDigi->getChargeChannel() == 0x7FFFFFFF) ) continue;
	  hitCel[lay][cel] = true;
	  hitCel2[lay][cel] = 1;
     }

     SmartDataPtr<RecMdcTrackCol> newtrkCol(eventSvc, "/Event/Recon/RecMdcTrackCol");
     if(!newtrkCol){
	  log << MSG::ERROR << "Could not find RecMdcTrackCol" << endreq;
	  return -1;
     }

     double dphi = 1.0;
     Identifier identifier;
     MdcID mdcid;
     RecMdcTrackCol::iterator it_trk = newtrkCol->begin();
     for(it_trk = newtrkCol->begin(); it_trk != newtrkCol->end(); it_trk++){
	  HitRefVec gothits = (*it_trk) -> getVecHits();
	  HitRefVec::iterator it_hit = gothits.begin();
	  for(; it_hit != gothits.end(); it_hit++){
	       identifier = (*it_hit)->getMdcId();
	       lay = mdcid.layer(identifier);
	       cel = mdcid.wire(identifier);
	       hitCel2[lay][cel] = 2;
	  }
     }
     for(it_trk = newtrkCol->begin(); it_trk != newtrkCol->end(); it_trk++){
	  HepVector helix = (*it_trk)->helix();
	  HepSymMatrix helixErr = (*it_trk)->err();
	  double phi0 = (*it_trk)->helix(1);
	  double phiTrk = phi0 + HFPI;
	  if(phiTrk > PI2) phiTrk -= PI2;

	  for(lay=0; lay<43; lay++){
	       double docamin = 0.9; // cm
	       if(lay<8) docamin = 0.6;	// cm
	       int celmin = -1;
	       int ncel = m_mdcGeomSvc->Layer(lay)->NCell();
	       for(cel=0; cel<ncel; cel++){
		    double wphi;
		    const MdcGeoWire* pWire = m_mdcGeomSvc -> Wire(lay, cel);
		    double xx = pWire->Backward().x();
		    double yy = pWire->Backward().y();
		    double rr = sqrt( (xx * xx) + (yy * yy) );
		    if( yy >= 0 )  wphi = acos(xx / rr);
		    else           wphi = CLHEP::twopi - acos(xx / rr);

		    if( !( (fabs(wphi-phiTrk) < dphi) || (fabs(wphi+PI2-phiTrk) < dphi) ||
			   (fabs(wphi-PI2-phiTrk) < dphi) ) ){
			 continue;
		    }

		    double doca = m_mdcUtilitySvc->doca(lay, cel, helix, helixErr);
// 		    cout << setw(5) << lay << setw(5) << cel << setw(15) << doca << endl;
		    if(fabs(doca) < fabs(docamin)){
			 docamin = doca;
			 celmin = cel;
		    }
	       }
	       if(celmin > -1){
		    int wir = m_mdcGeomSvc -> Wire(lay, celmin) -> Id();
		    m_hitEffAll->Fill(wir);
		    m_hitEffAll->Fill(6799);
		    if(lay<8) m_hitEffAll->Fill(6796);
		    else if(lay<20) m_hitEffAll->Fill(6797);
		    else m_hitEffAll->Fill(6798);

		    if(hitCel[lay][celmin]){
			 m_hitEffRaw->Fill(wir);
			 m_hitEffRaw->Fill(6799);
			 if(lay<8) m_hitEffRaw->Fill(6796);
			 else if(lay<20) m_hitEffRaw->Fill(6797);
			 else m_hitEffRaw->Fill(6798);
		    }
		    if(2==hitCel2[lay][celmin]){
			 m_hitEffRec->Fill(wir);
			 m_hitEffRec->Fill(6799);
			 if(lay<8) m_hitEffRec->Fill(6796);
			 else if(lay<20) m_hitEffRec->Fill(6797);
			 else m_hitEffRec->Fill(6798);
		    }
	       }
	  }
     }



//      int ncel;
//      int fgLayHit[43];
//      int celHit[43];
//      int celDocaMin;
//      double docaMin;
//      double trkpar[5];

//      for(i=0; i<ntrk; i++){
// 	  if(!trkFlag[i]) continue;
// 	  rectrk = event -> getRecTrk(i);
// 	  nhitRec = rectrk -> getNHits();

// 	  HepVector helix = rectrk->getHelix();
// 	  HepSymMatrix helixErr = rectrk->getHelixErr();

// 	  for(lay=0; lay<43; lay++){
// // 	       m_hitNum[lay][0]++;
// 	       fgLayHit[lay] = false;
// 	       celHit[lay] = -1;
// 	  }
// 	  for(k=0; k<nhitRec; k++){
// 	       rechit = rectrk -> getRecHit(k);
// 	       lay = rechit -> getLayid();
// 	       cel = rechit -> getCellid();
// 	       fgLayHit[lay] = true;
// 	       celHit[lay] = cel;
// 	       hitCel2[lay][cel] = 2;

// // 	       HepVector helix = rechit->getHelix();
// // 	       HepSymMatrix helixErr = rechit->getHelixErr();
//             double doca_rec = m_mdcUtilitySvc->doca(lay, cel, helix, helixErr);
// 	       cout << setw(15) << doca_rec*10. << setw(15) << rechit -> getDocaInc() << endl;
// 	  }

// // 	  for(lay=0; lay<43; lay++){
// // 	       if(fgLayHit[lay]){
// // 		    m_hitNum[lay][0]++;
// // 		    m_hitNum[lay][1]++;
// // 	       } else{
// // 		    if(lay<8) docaMin = 8.0;
// // 		    else docaMin = 10.5;
// // 		    celDocaMin = -1;

// // 		    ncel = m_mdcGeomSvc->Layer(lay)->NCell();
// // 		    for(cel=0; cel<ncel; cel++){
// //                    double doca_rec = m_mdcUtilitySvc->doca(lay, cel, helix, helixErr);
// // 			 if(fabs(docaCal) < docaMin){
// // 			      docaMin = fabs(docaCal);
// // 			      celDocaMin = cel;
// // 			 }
// // 		    }
// // 		    if(celDocaMin > -1){
// // 			 m_hitNum[lay][0]++;
// // 			 if(hitCel[lay][celDocaMin]) m_hitNum[lay][1]++;
// // 		    }
// // 	       }
// // 	  }
//      }

//      int nraw = 0;
//      int nrec = 0;
//      for(lay=0; lay<43; lay++){
// 	  for(cel=0; cel<288; cel++){
// 	       if(hitCel2[lay][cel] > 0) nraw++;
// 	       if(hitCel2[lay][cel] > 1) nrec++;
// 	  }
//      }
//      double ratio = (double)nrec / (double)nraw;
//      m_hratio->Fill(ratio);
     return 1;
}

bool MdcCalib::getCellTrkPass(MdcCalEvent* event, int iTrk, int cellTrkPass[]){
     MdcCalRecTrk* rectrk = event -> getRecTrk(iTrk);
     int nHits = rectrk -> getNHits();
     int hitInTrk[100];
     for(int k=0; k<nHits; k++){
	  MdcCalRecHit* rechit = rectrk->getRecHit(k);
	  int lay = rechit->getLayid();
	  int cel = rechit->getCellid();
	  int wir = m_mdcGeomSvc -> Wire(lay, cel) -> Id();
	  hitInTrk[k] = wir;
     }

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

     SmartDataPtr<RecMdcTrackCol> newtrkCol(eventSvc, "/Event/Recon/RecMdcTrackCol");
     if(!newtrkCol){
// 	  log << MSG::ERROR << "Could not find RecMdcTrackCol" << endreq;
	  return false;
     }
     MdcID mdcid;
     Identifier identifier;
     double dphi = 1.0;
     RecMdcTrackCol::iterator it_trk = newtrkCol->begin();
     for(it_trk = newtrkCol->begin(); it_trk != newtrkCol->end(); it_trk++){
	  int nRecHits = (*it_trk)->getNhits();
	  if(nRecHits < nHits) continue;

	  int hitInRecTrk[100];
	  int iRecHit = 0;
	  HitRefVec gothits = (*it_trk) -> getVecHits();
	  HitRefVec::iterator it_hit = gothits.begin();
	  for(; it_hit != gothits.end(); it_hit++){
	       identifier = (*it_hit)->getMdcId();
	       int lay = mdcid.layer(identifier);
	       int cel = mdcid.wire(identifier);
	       int wir = m_mdcGeomSvc -> Wire(lay, cel) -> Id();
	       hitInRecTrk[iRecHit] = wir;
	       iRecHit++;
	  }

	  // match the track
	  bool matchSuccess = true;
	  for(int i=0; i<nHits; i++){
	       bool findHit = false;
	       for(int k=0; k<nRecHits; k++){
		    if(hitInTrk[i] == hitInRecTrk[k]){
			 findHit = true;
			 break;
		    }
	       }
	       if(!findHit){
		    matchSuccess = false;
		    break;
	       }
	  }
	  if(!matchSuccess) continue;

	  HepVector helix = (*it_trk)->helix();
	  HepSymMatrix helixErr = (*it_trk)->err();
	  double phi0 = (*it_trk)->helix(1);
	  double phiTrk = phi0 + HFPI;
	  if(phiTrk > PI2) phiTrk -= PI2;

	  for(int lay=0; lay<43; lay++){
	       double docamin = 0.9; // cm
	       if(lay<8) docamin = 0.6;	// cm
	       int celmin = -1;
	       int ncel = m_mdcGeomSvc->Layer(lay)->NCell();
	       for(int cel=0; cel<ncel; cel++){
		    double wphi;
		    const MdcGeoWire* pWire = m_mdcGeomSvc -> Wire(lay, cel);
		    double xx = pWire->Backward().x();
		    double yy = pWire->Backward().y();
		    double rr = sqrt( (xx * xx) + (yy * yy) );
		    if( yy >= 0 )  wphi = acos(xx / rr);
		    else           wphi = CLHEP::twopi - acos(xx / rr);

		    if( !( (fabs(wphi-phiTrk) < dphi) || (fabs(wphi+PI2-phiTrk) < dphi) ||
			   (fabs(wphi-PI2-phiTrk) < dphi) ) ){
			 continue;
		    }

		    double doca = m_mdcUtilitySvc->doca(lay, cel, helix, helixErr);
// 		    cout << setw(5) << lay << setw(5) << cel << setw(15) << doca << endl;
		    if(fabs(doca) < fabs(docamin)){
			 docamin = doca;
			 celmin = cel;
		    }
	       }
	       if(celmin > -1){
		    cellTrkPass[lay] = celmin;
	       } else{
		    cellTrkPass[lay] = -1;
	       }
	  }
	  return true;
     }
     return false;
}
