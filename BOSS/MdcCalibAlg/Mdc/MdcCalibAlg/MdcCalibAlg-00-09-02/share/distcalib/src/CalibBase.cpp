#include "include/CalibBase.h"
#include "include/fun.h"

#include "TF1.h"
#include "TTree.h"
#include <cmath>

CalibBase::CalibBase(){
}

CalibBase::~CalibBase(){
}

void CalibBase::init(TObjArray* hlist, MdcCosGeom* pGeom){
     char hname[200];

     m_fdcom = new TFolder("mcommon", "common");
     hlist -> Add(m_fdcom);

     m_effNtrk = new TH1F("meffNtrk", "", 43, -0.5, 42.5);
     m_fdcom->Add(m_effNtrk);

     m_effNtrkRecHit = new TH1F("meffNtrkRecHit", "", 43, -0.5, 42.5);
     m_fdcom->Add(m_effNtrkRecHit);

     m_hresAllInc = new TH1F("mHResAllInc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresAllInc);

     m_hresAllExc = new TH1F("mHResAllExc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresAllExc);

     m_hresAllAve = new TH1F("mHResAllAve", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresAllAve);

     m_hresInnInc = new TH1F("mHResInnInc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresInnInc);

     m_hresInnExc = new TH1F("mHResInnExc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresInnExc);

     m_hresStpInc = new TH1F("mHResStpInc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresStpInc);

     m_hresStpExc = new TH1F("mHResStpExc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresStpExc);

     m_hresOutInc = new TH1F("mHResOutInc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresOutInc);

     m_hresOutExc = new TH1F("mHResOutExc", "", 200, -1.0, 1.0);
     m_fdcom -> Add(m_hresOutExc);

     m_fdResQ = new TFolder("mResQ", "ResQ");
     hlist->Add(m_fdResQ);
     for(int i=0; i<14; i++){
	  sprintf(hname, "mresoAll_qbin%02d", i);
	  m_hresAveAllQ[i] = new TH1F(hname, "", 200, -1, 1);
	  m_fdResQ->Add(m_hresAveAllQ[i]);

	  sprintf(hname, "mresoOut_qbin%02d", i);
	  m_hresAveOutQ[i] = new TH1F(hname, "", 200, -1, 1);
	  m_fdResQ->Add(m_hresAveOutQ[i]);
     }
     for(int lay=0; lay<43; lay++){
	  for(int i=0; i<14; i++){
	       sprintf(hname, "mresoLay%02d_qbin%02d", lay, i);
	       m_hresAveLayQ[lay][i] = new TH1F(hname, "", 200, -1, 1);
	       m_fdResQ->Add(m_hresAveLayQ[lay][i]);
	  }
     }

     m_hbbTrkFlg = new TH1F("mBbTrkFlg", "", 100, 0, 6);
     m_fdcom -> Add(m_hbbTrkFlg);

     m_hTesAll = new TH1F("mTesAll", "", 1000, 0, 2000);
     m_fdcom -> Add(m_hTesAll);

     m_hTesGood = new TH1F("mTesGood", "", 1000, 0, 2000);
     m_fdcom -> Add(m_hTesGood);

     m_hTesAllFlag = new TH1F("mTesAllFlag", "", 300, -0.5, 299.5);
     m_fdcom -> Add(m_hTesAllFlag);

     m_hTesRec = new TH1F("mTesRec", "", 1000, 0, 2000);
     m_fdcom -> Add(m_hTesRec);

     m_hTesCalFlag = new TH1F("mTesCalFlag", "", 1000, 0, 2000);
     m_fdcom -> Add(m_hTesCalFlag);

     m_hTesCalUse = new TH1F("mTesCalUse", "", 1000, 0, 2000);
     m_fdcom -> Add(m_hTesCalUse);

     m_hnRawHit = new TH1F("mNRawHit", "", 6797, -0.5, 6796.5);
     m_fdcom -> Add(m_hnRawHit);

     m_hpt = new TH1F("mHPt", "", 800, 0, 3);
     m_fdcom -> Add(m_hpt);

     m_hptPos = new TH1F("mHPtPos", "", 800, 0, 3);
     m_fdcom -> Add(m_hptPos);

     m_hptNeg = new TH1F("mHPtNeg", "", 800, 0, 3);
     m_fdcom -> Add(m_hptNeg);

     m_hp = new TH1F("mHP", "", 800, 0, 3);
     m_fdcom -> Add(m_hp);

     m_hp_cms = new TH1F("mHPCMS", "", 800, 0, 3);
     m_fdcom -> Add(m_hp_cms);

     m_hpMax = new TH1F("mHPMax", "", 800, 0, 3);
     m_fdcom -> Add(m_hpMax);

     m_hpMaxCms = new TH1F("mHPMax_Cms", "", 800, 0, 3);
     m_fdcom -> Add(m_hpMaxCms);

     m_hpPos = new TH1F("mHP_Pos", "", 800, 0, 3);
     m_fdcom -> Add(m_hpPos);

     m_hpNeg = new TH1F("mHP_Neg", "", 800, 0, 3);
     m_fdcom -> Add(m_hpNeg);

     m_hpPoscms = new TH1F("mHP_Pos_cms", "", 800, 0, 3);
     m_fdcom -> Add(m_hpPoscms);

     m_hpNegcms = new TH1F("mHP_Neg_cms", "", 800, 0, 3);
     m_fdcom -> Add(m_hpNegcms);

     m_hp_cut = new TH1F("mHPCut", "", 800, 0, 3);
     m_fdcom -> Add(m_hp_cut);

     m_hchisq = new TH1F("mChisq", "", 10, 0, 100);
     m_fdcom -> Add(m_hchisq);

     m_hnTrk = new TH1F("mHNtrack", "HNtrack", 10, -0.5, 9.5);
     m_fdcom -> Add(m_hnTrk);

     m_hnTrkCal = new TH1F("mHNtrackCal", "HNtrackCal", 10, -0.5, 9.5);
     m_fdcom -> Add(m_hnTrkCal);

     m_hnhitsRec = new TH1F("mHNhitsRec", "", 100, -0.5, 99.5);
     m_fdcom -> Add(m_hnhitsRec);

     m_hnhitsRecInn = new TH1F("mHNhitsInnRec", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsRecInn);

     m_hnhitsRecStp = new TH1F("mHNhitsStpRec", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsRecStp);

     m_hnhitsRecOut = new TH1F("mHNhitsOutRec", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsRecOut);

     m_hnhitsCal = new TH1F("mHNhitsCal", "", 100, -0.5, 99.5);
     m_fdcom -> Add(m_hnhitsCal);

     m_hnhitsCalInn = new TH1F("mHNhitsCalInn", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsCalInn);

     m_hnhitsCalStp = new TH1F("mHNhitsCalStp", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsCalStp);

     m_hnhitsCalOut = new TH1F("mHNhitsCalOut", "", 60, 0.5, 60.5);
     m_fdcom -> Add(m_hnhitsCalOut);

     m_wirehitmap = new TH1F("mWire_HitMap", "Wire_HitMap", 6796, -0.5, 6795.5);
     m_fdcom -> Add(m_wirehitmap);

     m_layerhitmap = new TH1F("mLayer_HitMap", "Layer_HitMap", 43, -0.5, 42.5);
     m_fdcom -> Add(m_layerhitmap);

     m_hnoisephi = new TH1F("mphi_noise", "", 100, 0, 6.284);
     m_fdcom -> Add(m_hnoisephi);

     m_hnoiselay = new TH1F("mLayer_noise", "Layer_noise", 43, -0.5, 42.5);
     m_fdcom -> Add(m_hnoiselay);

     m_hnoisenhits = new TH1F("mnhits_noise", "nhits_noise", 6796, -0.5, 6795.5);
     m_fdcom -> Add(m_hnoisenhits);

     m_hratio = new TH1F("mratio", "", 100, 0, 1);
     m_fdcom -> Add(m_hratio);

     m_hdr = new TH1F("mdr", "", 500, -500, 500);
     m_fdcom -> Add(m_hdr);

     m_hphi0 = new TH1F("mphi0", "", 100, 0, 6.284);
     m_fdcom -> Add(m_hphi0);

     m_hkap = new TH1F("mkappa", "", 400, -50, 50);
     m_fdcom -> Add(m_hkap);

     m_hdz = new TH1F("mdz", "", 500, -1000, 1000);
     m_fdcom -> Add(m_hdz);

     m_htanl = new TH1F("mtanl", "", 200, -5, 5);
     m_fdcom -> Add(m_htanl);

     m_hcosthe = new TH1F("mcostheta", "", 200, -1, 1);
     m_fdcom -> Add(m_hcosthe);

     m_hcostheNeg = new TH1F("mcosthetaNeg", "", 200, -1, 1);
     m_fdcom -> Add(m_hcostheNeg);

     m_hcosthePos = new TH1F("mcosthetaPos", "", 200, -1, 1);
     m_fdcom -> Add(m_hcosthePos);

     m_hx0 = new TH1F("mx0", "", 100, -10, 10);
     m_fdcom -> Add(m_hx0);

     m_hy0 = new TH1F("my0", "", 100, -10, 10);
     m_fdcom -> Add(m_hy0);

     m_hdelZ0 = new TH1F("mdelta_z0", "", 100, -50, 50);
     m_fdcom -> Add(m_hdelZ0);

     m_nGrPoint = 0;
     m_grX0Y0 = new TGraph();
     m_grX0Y0->SetName("mx0y0");
     m_fdcom -> Add(m_grX0Y0);

     m_hitEffAll = new TH1F("mhitEffAll", "", 6800, -0.5, 6799.5);
     m_fdcom -> Add(m_hitEffAll);

     m_hitEffRaw = new TH1F("mhitEffRaw", "", 6800, -0.5, 6799.5);
     m_fdcom -> Add(m_hitEffRaw);

     m_hitEffRec = new TH1F("mhitEffRec", "", 6800, -0.5, 6799.5);
     m_fdcom -> Add(m_hitEffRec);

     // histograms for drift time
     m_fdTime = new TFolder("mtime", "time");
     hlist -> Add(m_fdTime);

     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "mTraw%02d", lay);
	  m_htraw[lay] = new TH1F(hname, "", 1000, 0, 2000);
	  m_fdTime -> Add(m_htraw[lay]);

	  sprintf(hname, "mTdr%02d", lay);
	  m_htdr[lay] = new TH1F(hname, "", 510, -10, 500);
	  m_fdTime -> Add(m_htdr[lay]);

	  for(int lr=0; lr<2; lr++){
	       sprintf(hname, "mTdr%02d_lr%01d", lay, lr);
	       m_htdrlr[lay][lr] = new TH1F(hname, "", 510, -10, 500);
	       m_fdTime -> Add(m_htdrlr[lay][lr]);
	  }
     }

     // histograms for resolution
     m_fdres = new TFolder("mresolution", "resolution");
     hlist -> Add(m_fdres);

     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "mReso%02dInc", lay);
	  m_hresInc[lay] = new TH1F(hname, "", 1000, -5, 5);
	  m_fdres -> Add(m_hresInc[lay]);

	  sprintf(hname, "mReso%02dExc", lay);
	  m_hresExc[lay] = new TH1F(hname, "", 1000, -5, 5);
	  m_fdres -> Add(m_hresExc[lay]);

	  for (int lr=0; lr<2; lr++){
	       sprintf(hname, "mReso%02dInc_lr%01d", lay, lr);
	       m_hreslrInc[lay][lr] = new TH1F(hname, "", 1000, -5, 5);
	       m_fdres->Add(m_hreslrInc[lay][lr]);

	       sprintf(hname, "mReso%02dExc_lr%01d", lay, lr);
	       m_hreslrExc[lay][lr] = new TH1F(hname, "", 1000, -5, 5);
	       m_fdres->Add(m_hreslrExc[lay][lr]);
	  }
     }

     /* histograms for momentum vs phi */
     m_fdmomPhi = new TFolder("mmomPhi", "momPhi");
     hlist -> Add(m_fdmomPhi);

     int thbin;
     for(int bin=0; bin<NPhiBin; bin++){
	  sprintf(hname, "mhPpos_phi%02d", bin);
	  m_ppPhi[bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_ppPhi[bin]);

	  sprintf(hname, "mhPneg_phi%02d", bin);
	  m_pnPhi[bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_pnPhi[bin]);

	  sprintf(hname, "mhPpos_phi_cms%02d", bin);
	  m_ppPhiCms[bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_ppPhiCms[bin]);

	  sprintf(hname, "mhPneg_phi_cms%02d", bin);
	  m_pnPhiCms[bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_pnPhiCms[bin]);

	  for(thbin=0; thbin<NThetaBin; thbin++){
	       sprintf(hname, "mhPpos_theta%02d_phi%02d", thbin, bin);
	       m_ppThePhi[thbin][bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	       m_fdmomPhi->Add(m_ppThePhi[thbin][bin]);

	       sprintf(hname, "mhPneg_theta%02d_phi%02d", thbin, bin);
	       m_pnThePhi[thbin][bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	       m_fdmomPhi->Add(m_pnThePhi[thbin][bin]);

	       sprintf(hname, "mhPposCms_theta%02d_phi%02d", thbin, bin);
	       m_ppThePhiCms[thbin][bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	       m_fdmomPhi->Add(m_ppThePhiCms[thbin][bin]);

	       sprintf(hname, "mhPnegCms_theta%02d_phi%02d", thbin, bin);
	       m_pnThePhiCms[thbin][bin] = new TH1F(hname, "", 400, 1.0, 2.5);
	       m_fdmomPhi->Add(m_pnThePhiCms[thbin][bin]);
	  }
     }
     for(thbin=0; thbin<NThetaBin; thbin++){
	  sprintf(hname, "mhPpos_the%02d", thbin);
	  m_ppThe[thbin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_ppThe[thbin]);

	  sprintf(hname, "mhPneg_the%02d", thbin);
	  m_pnThe[thbin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_pnThe[thbin]);

	  sprintf(hname, "mhPposCms_the%02d", thbin);
	  m_ppTheCms[thbin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_ppTheCms[thbin]);

	  sprintf(hname, "mhPnegCms_the%02d", thbin);
	  m_pnTheCms[thbin] = new TH1F(hname, "", 400, 1.0, 2.5);
	  m_fdmomPhi->Add(m_pnTheCms[thbin]);
     }

     m_fdr2d = new TFolder("mres2d", "res2d");
     hlist -> Add(m_fdr2d);
     for(int lay=0; lay<43; lay++){
	  for(int iEntr=0; iEntr<gNEntr[lay]; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    for(int bin=0; bin<NSDBIN; bin++){
			 sprintf(hname, "mR2d%02d_%02d_%01d_%02dInc", lay, iEntr, lr, bin);
			 m_hr2dInc[lay][iEntr][lr][bin] = new TH1F(hname, "", 200, -1, 1);
			 m_fdr2d -> Add(m_hr2dInc[lay][iEntr][lr][bin]);

			 sprintf(hname, "mR2d%02d_%02d_%01d_%02dExc", lay, iEntr, lr, bin);
			 m_hr2dExc[lay][iEntr][lr][bin] = new TH1F(hname, "", 200, -1, 1);
			 m_fdr2d -> Add(m_hr2dExc[lay][iEntr][lr][bin]);
		    }
	       }
	  }
     }

//      m_fdr2t = new TFolder("mres2t", "res2t");
//      hlist -> Add(m_fdr2t);
//      for(int lay=0; lay<43; lay++){
// 	  for(int iEntr=0; iEntr<18; iEntr++){
// 	       for(int lr=0; lr<2; lr++){
// 		    for(int bin=0; bin<45; bin++){
// 			 sprintf(hname, "mr2t%02d_%02d_%01d_%02d", lay, iEntr, lr, bin);
// 			 m_hr2t[lay][iEntr][lr][bin] = new TH1F(hname, "", 600, -3, 3);
// 			 m_fdr2t -> Add(m_hr2t[lay][iEntr][lr][bin]);
// 		    }
// 	       }
// 	  }
//      }
}

void CalibBase::mergeHist(TFile* fhist){
     char hname[200];
     TH1F* hist;
     TFolder* fdcom = (TFolder*)fhist->Get("common");
     TFolder* fdResQ = (TFolder*)fhist->Get("ResQ");
     TFolder* fdTime = (TFolder*)fhist->Get("time");
     TFolder* fdres = (TFolder*)fhist->Get("resolution");
     TFolder* fdmomPhi = (TFolder*)fhist->Get("momPhi");
     TFolder* fdres2d = (TFolder*)fhist->Get("res2d");

     hist = (TH1F*)fdcom->FindObjectAny("effNtrk");
     m_effNtrk->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("effNtrkRecHit");
     m_effNtrkRecHit->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HResAllInc");
     m_hresAllInc->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HResAllExc");
     m_hresAllExc->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HResAllAve");
     m_hresAllAve->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HResInnInc");
     m_hresInnInc->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HResInnExc");
     m_hresInnExc->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HResStpInc");
     m_hresStpInc->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HResStpExc");
     m_hresStpExc->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HResOutInc");
     m_hresOutInc->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HResOutExc");
     m_hresOutExc->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("BbTrkFlg");
     m_hbbTrkFlg->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("TesAll");
     m_hTesAll->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("TesGood");
     m_hTesGood->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("TesAllFlag");
     m_hTesAllFlag->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("TesRec");
     m_hTesRec->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("TesCalFlag");
     m_hTesCalFlag->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("TesCalUse");
     m_hTesCalUse->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("nRawHit");
     m_hnRawHit->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HPt");
     m_hpt->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HPtPos");
     m_hptPos->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HPtNeg");
     m_hptNeg->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HP");
     m_hp->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HPCMS");
     m_hp_cms->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HPMax");
     m_hpMax->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HPMax_Cms");
     m_hpMaxCms->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HP_Pos");
     m_hpPos->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HP_Neg");
     m_hpNeg->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HP_Pos_cms");
     m_hpPoscms->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HP_Neg_cms");
     m_hpNegcms->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HPCut");
     m_hp_cut->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("Chisq");
     m_hchisq->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HNtrack");
     m_hnTrk->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HNtrackCal");
     m_hnTrkCal->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HNhitsRec");
     m_hnhitsRec->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HNhitsInnRec");
     m_hnhitsRecInn->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HNhitsStpRec");
     m_hnhitsRecStp->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HNhitsOutRec");
     m_hnhitsRecOut->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HNhitsCal");
     m_hnhitsCal->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HNhitsCalInn");
     m_hnhitsCalInn->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HNhitsCalStp");
     m_hnhitsCalStp->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("HNhitsCalOut");
     m_hnhitsCalOut->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("Wire_HitMap");
     m_wirehitmap->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("Layer_HitMap");
     m_layerhitmap->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("phi_noise");
     m_hnoisephi->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("Layer_noise");
     m_hnoiselay->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("nhits_noise");
     m_hnoisenhits->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("ratio");
     m_hratio->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("dr");
     m_hdr->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("phi0");
     m_hphi0->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("kappa");
     m_hkap->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("dz");
     m_hdz->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("tanl");
     m_htanl->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("costheta");
     m_hcosthe->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("costhetaNeg");
     m_hcostheNeg->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("costhetaPos");
     m_hcosthePos->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("x0");
     m_hx0->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("y0");
     m_hy0->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("delta_z0");
     m_hdelZ0->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("hitEffAll");
     m_hitEffAll->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("hitEffRaw");
     m_hitEffRaw->Add(hist);

     hist = (TH1F*)fdcom->FindObjectAny("hitEffRec");
     m_hitEffRec->Add(hist);

     TGraph* gr = (TGraph*)fdcom->FindObjectAny("x0y0");
     int np = gr->GetN();
     double xx;
     double yy;
     for(int i=0; i<np; i++){
	  gr->GetPoint(i, xx, yy);
	  m_grX0Y0->SetPoint(m_nGrPoint, xx, yy);
	  m_nGrPoint++;
     }


     for(int i=0; i<14; i++){
	  sprintf(hname, "resoAll_qbin%02d", i);
	  hist = (TH1F*)fdResQ->FindObjectAny(hname);
	  m_hresAveAllQ[i]->Add(hist);

	  sprintf(hname, "resoOut_qbin%02d", i);
	  hist = (TH1F*)fdResQ->FindObjectAny(hname);
	  m_hresAveOutQ[i]->Add(hist);
     }

     for(int lay=0; lay<43; lay++){
	  for(int i=0; i<14; i++){
	       sprintf(hname, "resoLay%02d_qbin%02d", lay, i);
	       hist = (TH1F*)fdResQ->FindObjectAny(hname);
	       m_hresAveLayQ[lay][i]->Add(hist);
	  }
     }

     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "Traw%02d", lay);
	  hist = (TH1F*)fdTime->FindObjectAny(hname);
	  m_htraw[lay]->Add(hist);

	  sprintf(hname, "Tdr%02d", lay);
	  hist = (TH1F*)fdTime->FindObjectAny(hname);
	  m_htdr[lay]->Add(hist);

	  for(int lr=0; lr<2; lr++){
	       sprintf(hname, "Tdr%02d_lr%01d", lay, lr);
	       hist = (TH1F*)fdTime->FindObjectAny(hname);
	       m_htdrlr[lay][lr]->Add(hist);
	  }
     }

     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "Reso%02dInc", lay);
	  hist = (TH1F*)fdres->FindObjectAny(hname);
	  m_hresInc[lay]->Add(hist);

	  sprintf(hname, "Reso%02dExc", lay);
	  hist = (TH1F*)fdres->FindObjectAny(hname);
	  m_hresExc[lay]->Add(hist);

	  for (int lr=0; lr<2; lr++){
	       sprintf(hname, "Reso%02dInc_lr%01d", lay, lr);
	       hist = (TH1F*)fdres->FindObjectAny(hname);
	       m_hreslrInc[lay][lr]->Add(hist);

	       sprintf(hname, "Reso%02dExc_lr%01d", lay, lr);
	       hist = (TH1F*)fdres->FindObjectAny(hname);
	       m_hreslrExc[lay][lr]->Add(hist);
	  }
     }

     int thbin;
     for(int bin=0; bin<NPhiBin; bin++){
	  sprintf(hname, "hPpos_phi%02d", bin);
	  hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	  m_ppPhi[bin]->Add(hist);

	  sprintf(hname, "hPneg_phi%02d", bin);
	  hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	  m_pnPhi[bin]->Add(hist);

	  sprintf(hname, "hPpos_phi_cms%02d", bin);
	  hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	  m_ppPhiCms[bin]->Add(hist);

	  sprintf(hname, "hPneg_phi_cms%02d", bin);
	  hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	  m_pnPhiCms[bin]->Add(hist);

	  for(thbin=0; thbin<NThetaBin; thbin++){
	       sprintf(hname, "hPpos_theta%02d_phi%02d", thbin, bin);
	       hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	       m_ppThePhi[thbin][bin]->Add(hist);

	       sprintf(hname, "hPneg_theta%02d_phi%02d", thbin, bin);
	       hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	       m_pnThePhi[thbin][bin]->Add(hist);

	       sprintf(hname, "hPposCms_theta%02d_phi%02d", thbin, bin);
	       hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	       m_ppThePhiCms[thbin][bin]->Add(hist);

	       sprintf(hname, "hPnegCms_theta%02d_phi%02d", thbin, bin);
	       hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	       m_pnThePhiCms[thbin][bin]->Add(hist);
	  }
     }

     for(thbin=0; thbin<NThetaBin; thbin++){
	  sprintf(hname, "hPpos_the%02d", thbin);
	  hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	  m_ppThe[thbin]->Add(hist);

	  sprintf(hname, "hPneg_the%02d", thbin);
	  hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	  m_ppThe[thbin]->Add(hist);

	  sprintf(hname, "hPposCms_the%02d", thbin);
	  hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	  m_ppTheCms[thbin]->Add(hist);

	  sprintf(hname, "hPnegCms_the%02d", thbin);
	  hist = (TH1F*)fdmomPhi->FindObjectAny(hname);
	  m_pnTheCms[thbin]->Add(hist);
     }

     for(int lay=0; lay<43; lay++){
	  for(int iEntr=0; iEntr<gNEntr[lay]; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    for(int bin=0; bin<NSDBIN; bin++){
			 sprintf(hname, "r2d%02d_%02d_%01d_%02dInc", lay, iEntr, lr, bin);
			 hist = (TH1F*)fdres2d->FindObjectAny(hname);
			 m_hr2dInc[lay][iEntr][lr][bin]->Add(hist);

			 sprintf(hname, "r2d%02d_%02d_%01d_%02dExc", lay, iEntr, lr, bin);
			 hist = (TH1F*)fdres2d->FindObjectAny(hname);
			 m_hr2dExc[lay][iEntr][lr][bin]->Add(hist);
		    }
	       }
	  }
     }

//      TFolder* fdres2t = (TFolder*)fhist->Get("res2t");
//      for(int lay=0; lay<43; lay++){
// 	  for(int iEntr=0; iEntr<18; iEntr++){
// 	       for(int lr=0; lr<2; lr++){
// 		    for(int bin=0; bin<45; bin++){
// 			 sprintf(hname, "r2t%02d_%02d_%01d_%02d", lay, iEntr, lr, bin);
// 			 hist = (TH1F*)fdres2t->FindObjectAny(hname);
// 			 m_hr2t[lay][iEntr][lr][bin]->Add(hist);
// 		    }
// 	       }
// 	  }
//      }
}

void CalibBase::calib(MdcCalibConst* calconst, TObjArray* newXtList, TObjArray* r2tList){
     int nGoodAll = 0;
     int nGoodInn = 0;
     int nGoodStp = 0;
     int nGoodOut = 0;
     int nTotAll = 0;
     int nTotInn = 0;
     int nTotStp = 0;
     int nTotOut = 0;
     ofstream feffi("MdcLayerEffi.dat");
     for(int lay=0; lay<NLAYER; lay++){
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

	  double effi = (double)effGoodHit / (double)effNtrk;
	  double effErr = sqrt(effi * (1-effi) / (double)effNtrk);
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

     // update resolution function
     Stat_t histEntry;
     int nSdBin[43];
     for(int lay=0; lay<8; lay++) nSdBin[lay] = 12; // 14
     for(int lay=8; lay<43; lay++) nSdBin[lay] = 16; // 18
     double sigm[NSDBIN];
     if(1 == gCalSigma){
	  ofstream fr2d("logr2d.dat");
	  for(int lay=0; lay<43; lay++){
	       int nbinLim = 13;
	       if(lay<8) nbinLim = 9;
	       if(0 == gFgCalib[lay]) continue;
	       for(int iEntr=0; iEntr<gNEntr[lay]; iEntr++){
		    for(int lr=0; lr<2; lr++){
			 fr2d << setw(3) << lay << setw(3) << iEntr << setw(3) << lr << endl;
			 for(int bin=0; bin<nSdBin[lay]; bin++){
			      if(1 == gResiType){
				   histEntry = m_hr2dExc[lay][iEntr][lr][bin]->GetEntries();
				   double rms = m_hr2dExc[lay][iEntr][lr][bin]->GetRMS();
				   m_hr2dExc[lay][iEntr][lr][bin]->Fit("gaus", "Q");
				   double gausSigm = m_hr2dExc[lay][iEntr][lr][bin]->GetFunction("gaus")->GetParameter(2);
// 				   if(histEntry<50){
// 					sigm[bin] = calconst->getSdpar(lay, iEntr, lr, bin);
// 				   } else if((lay>2) && (histEntry>500) && (fabs(rms-gausSigm)<0.02)){
// 					sigm[bin] = gausSigm;
// 				   } else{
// 					sigm[bin] = rms;
// 				   }

				   if(histEntry > 500){
					sigm[bin] = gausSigm;
				   } else if(histEntry>100){
					sigm[bin] = rms;
				   } else{
					sigm[bin] = calconst->getSdpar(lay, iEntr, lr, bin);
				   }
				   if(bin>=nbinLim){
					if(sigm[bin] < sigm[bin-1]) sigm[bin] = sigm[bin-1];
				   }
			      } else{
				   histEntry = m_hr2dInc[lay][iEntr][lr][bin]->GetEntries();
				   double rms = m_hr2dInc[lay][iEntr][lr][bin]->GetRMS();
				   m_hr2dInc[lay][iEntr][lr][bin]->Fit("gaus", "Q");
				   double gausSigm = m_hr2dInc[lay][iEntr][lr][bin]->GetFunction("gaus")->GetParameter(2);
				   if(histEntry<50){
					sigm[bin] = calconst->getSdpar(lay, iEntr, lr, bin);
				   } else if((lay>2) && (histEntry>500) && (fabs(rms-gausSigm)<0.02)){
					sigm[bin] = gausSigm;
				   } else{
					sigm[bin] = rms;
				   }
				   if(bin>=nbinLim){
					if(sigm[bin] < sigm[bin-1]) sigm[bin] = sigm[bin-1];
				   }
// 				   if(histEntry > 500){
// 					m_hr2dInc[lay][iEntr][lr][bin]->Fit("gaus", "Q");
// 					sigm[bin] = m_hr2dInc[lay][iEntr][lr][bin]->GetFunction("gaus")->GetParameter(2);
// 				   } else if(histEntry>100){
// 					sigm[bin] = m_hr2dInc[lay][iEntr][lr][bin]->GetRMS();
// 				   } else{
// 					sigm[bin] = 0.2;
// 				   }
			      }
			      if(sigm[bin] < 0.05) sigm[bin] = 0.05;	// for boundary layers
			 } // end of bin loop
			 for(int bin=nSdBin[lay]; bin<NSDBIN; bin++){
			      sigm[bin] = sigm[bin-1];
			 }

			 for(int bin=0; bin<NSDBIN; bin++){
			      if(1 == gNEntr[lay]){
				   for(int i=0; i<6; i++) calconst->resetSdpar(lay, i, lr, bin, sigm[bin]);
			      } else if(2 == gNEntr[lay]){
				   if(0 == iEntr){
					for(int i=0; i<3; i++){ // entr<0
					     calconst->resetSdpar(lay, i, lr, bin, sigm[bin]);
					}
				   } else{
					for(int i=3; i<6; i++){ // entr>0
					     calconst->resetSdpar(lay, i, lr, bin, sigm[bin]);
					}
				   }
			      }
			      fr2d << setw(5) << bin << setw(15) << sigm[bin] << endl;
			 }
		    } // lr loop
	       } // entr loop
	  } // layer loop
	  fr2d.close();
     }

     int entrId, lrId;
     double tdr, sigma;
     char hname[200];
     r2tList->Clear();
     TTree* r2ttr[NLAYER];
     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "r2t%02d", lay);
	  r2ttr[lay] = new TTree(hname, hname);
	  r2ttr[lay]->Branch("iEntr", &entrId, "iEntr/I");
	  r2ttr[lay]->Branch("lr", &lrId, "lr/I");
	  r2ttr[lay]->Branch("t", &tdr, "t/D");
	  r2ttr[lay]->Branch("s", &sigma, "s/D");
	  for(int iEntr=0; iEntr<NENTRXT; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    for(int bin=0; bin<45; bin++){
			 entrId = iEntr;
			 lrId = lr;
			 tdr = (double)bin * 10.0 + 5.0;
			 sigma = 0.13;
			 r2ttr[lay]->Fill();
		    }
	       }
	  }
	  r2tList->Add(r2ttr[lay]);
     }

     renameHist();
}

void CalibBase::renameHist(){
     char hname[200];
     m_fdcom->SetName("common");
     m_effNtrk->SetName("effNtrk");
     m_effNtrkRecHit->SetName("effNtrkRecHit");
     m_hresAllInc->SetName("HResAllInc");
     m_hresAllExc->SetName("HResAllExc");
     m_hresAllAve->SetName("HResAllAve");
     m_hresInnInc->SetName("HResInnInc");
     m_hresInnExc->SetName("HResInnExc");
     m_hresStpInc->SetName("HResStpInc");
     m_hresStpExc->SetName("HResStpExc");
     m_hresOutInc->SetName("HResOutInc");
     m_hresOutExc->SetName("HResOutExc");
     m_fdResQ->SetName("ResQ");
     m_hbbTrkFlg->SetName("BbTrkFlg");
     m_hTesAll->SetName("TesAll");
     m_hTesGood->SetName("TesGood");
     m_hTesAllFlag->SetName("TesAllFlag");
     m_hTesRec->SetName("TesRec");
     m_hTesCalFlag->SetName("TesCalFlag");
     m_hTesCalUse->SetName("TesCalUse");
     m_hnRawHit->SetName("nRawHit");
     m_hpt->SetName("HPt");
     m_hptPos->SetName("HPtPos");
     m_hptNeg->SetName("HPtNeg");
     m_hp->SetName("HP");
     m_hp_cms->SetName("HPCMS");
     m_hpMax->SetName("HPMax");
     m_hpMaxCms->SetName("HPMax_Cms");
     m_hpPos->SetName("HP_Pos");
     m_hpNeg->SetName("HP_Neg");
     m_hpPoscms->SetName("HP_Pos_cms");
     m_hpNegcms->SetName("HP_Neg_cms");
     m_hp_cut->SetName("HPCut");
     m_hchisq->SetName("Chisq");
     m_hnTrk->SetName("HNtrack");
     m_hnTrkCal->SetName("HNtrackCal");
     m_hnhitsRec->SetName("HNhitsRec");
     m_hnhitsRecInn->SetName("HNhitsInnRec");
     m_hnhitsRecStp->SetName("HNhitsStpRec");
     m_hnhitsRecOut->SetName("HNhitsOutRec");
     m_hnhitsCal->SetName("HNhitsCal");
     m_hnhitsCalInn->SetName("HNhitsCalInn");
     m_hnhitsCalStp->SetName("HNhitsCalStp");
     m_hnhitsCalOut->SetName("HNhitsCalOut");
     m_wirehitmap->SetName("Wire_HitMap");
     m_layerhitmap->SetName("Layer_HitMap");
     m_hnoisephi->SetName("phi_noise");
     m_hnoiselay->SetName("Layer_noise");
     m_hnoisenhits->SetName("nhits_noise");
     m_hratio->SetName("ratio");
     m_hdr->SetName("dr");
     m_hphi0->SetName("phi0");
     m_hkap->SetName("kappa");
     m_hdz->SetName("dz");
     m_htanl->SetName("tanl");
     m_hcosthe->SetName("costheta");
     m_hcostheNeg->SetName("costhetaNeg");
     m_hcosthePos->SetName("costhetaPos");
     m_hx0->SetName("x0");
     m_hy0->SetName("y0");
     m_hdelZ0->SetName("delta_z0");
     m_hitEffAll->SetName("hitEffAll");
     m_hitEffRaw->SetName("hitEffRaw");
     m_hitEffRec->SetName("hitEffRec");
     m_grX0Y0->SetName("x0y0");
     m_fdTime->SetName("time");

     for(int i=0; i<14; i++){
	  sprintf(hname, "resoAll_qbin%02d", i);
	  m_hresAveAllQ[i]->SetName(hname);

	  sprintf(hname, "resoOut_qbin%02d", i);
	  m_hresAveOutQ[i]->SetName(hname);
     }
     for(int lay=0; lay<43; lay++){
	  for(int i=0; i<14; i++){
	       sprintf(hname, "resoLay%02d_qbin%02d", lay, i);
	       m_hresAveLayQ[lay][i]->SetName(hname);
	  }
     }

     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "Traw%02d", lay);
	  m_htraw[lay]->SetName(hname);

	  sprintf(hname, "Tdr%02d", lay);
	  m_htdr[lay]->SetName(hname);

	  for(int lr=0; lr<2; lr++){
	       sprintf(hname, "Tdr%02d_lr%01d", lay, lr);
	       m_htdrlr[lay][lr]->SetName(hname);
	  }
     }

     // histograms for resolution
     m_fdres->SetName("resolution");
     for(int lay=0; lay<NLAYER; lay++){
	  sprintf(hname, "Reso%02dInc", lay);
	  m_hresInc[lay]->SetName(hname);

	  sprintf(hname, "Reso%02dExc", lay);
	  m_hresExc[lay]->SetName(hname);

	  for (int lr=0; lr<2; lr++){
	       sprintf(hname, "Reso%02dInc_lr%01d", lay, lr);
	       m_hreslrInc[lay][lr]->SetName(hname);

	       sprintf(hname, "Reso%02dExc_lr%01d", lay, lr);
	       m_hreslrExc[lay][lr]->SetName(hname);
	  }
     }

     /* histograms for momentum vs phi */
     m_fdmomPhi->SetName("momPhi");
     int thbin;
     for(int bin=0; bin<NPhiBin; bin++){
	  sprintf(hname, "hPpos_phi%02d", bin);
	  m_ppPhi[bin]->SetName(hname);

	  sprintf(hname, "hPneg_phi%02d", bin);
	  m_pnPhi[bin]->SetName(hname);

	  sprintf(hname, "hPpos_phi_cms%02d", bin);
	  m_ppPhiCms[bin]->SetName(hname);

	  sprintf(hname, "hPneg_phi_cms%02d", bin);
	  m_pnPhiCms[bin]->SetName(hname);

	  for(thbin=0; thbin<NThetaBin; thbin++){
	       sprintf(hname, "hPpos_theta%02d_phi%02d", thbin, bin);
	       m_ppThePhi[thbin][bin]->SetName(hname);

	       sprintf(hname, "hPneg_theta%02d_phi%02d", thbin, bin);
	       m_pnThePhi[thbin][bin]->SetName(hname);

	       sprintf(hname, "hPposCms_theta%02d_phi%02d", thbin, bin);
	       m_ppThePhiCms[thbin][bin]->SetName(hname);

	       sprintf(hname, "hPnegCms_theta%02d_phi%02d", thbin, bin);
	       m_pnThePhiCms[thbin][bin]->SetName(hname);
	  }
     }
     for(thbin=0; thbin<NThetaBin; thbin++){
	  sprintf(hname, "hPpos_the%02d", thbin);
	  m_ppThe[thbin]->SetName(hname);

	  sprintf(hname, "hPneg_the%02d", thbin);
	  m_pnThe[thbin]->SetName(hname);

	  sprintf(hname, "hPposCms_the%02d", thbin);
	  m_ppTheCms[thbin]->SetName(hname);

	  sprintf(hname, "hPnegCms_the%02d", thbin);
	  m_pnTheCms[thbin]->SetName(hname);
     }

     m_fdr2d->SetName("res2d");
     for(int lay=0; lay<43; lay++){
	  for(int iEntr=0; iEntr<gNEntr[lay]; iEntr++){
	       for(int lr=0; lr<2; lr++){
		    for(int bin=0; bin<NSDBIN; bin++){
			 sprintf(hname, "r2d%02d_%02d_%01d_%02dInc", lay, iEntr, lr, bin);
			 m_hr2dInc[lay][iEntr][lr][bin]->SetName(hname);
			 sprintf(hname, "r2d%02d_%02d_%01d_%02dExc", lay, iEntr, lr, bin);
			 m_hr2dExc[lay][iEntr][lr][bin]->SetName(hname);
		    }
	       }
	  }
     }

//      m_fdr2d->SetName("res2t");
//      for(int lay=0; lay<43; lay++){
// 	  for(int iEntr=0; iEntr<18; iEntr++){
// 	       for(int lr=0; lr<2; lr++){
// 		    for(int bin=0; bin<45; bin++){
// 			 sprintf(hname, "r2t%02d_%02d_%01d_%02d", lay, iEntr, lr, bin);
// 			 m_hr2t[lay][iEntr][lr][bin]->SetName(hname);
// 		    }
// 	       }
// 	  }
//      }
}
