#include "include/T0Calib.h"
#include "include/fun.h"

T0Calib::T0Calib(){
     cout << "Calibration type: T0Calib" << endl;
}

T0Calib::~T0Calib(){
}

void T0Calib::init(TObjArray* hlist, MdcCosGeom* pGeom){
     CalibBase::init(hlist, pGeom);

     m_pGeom = pGeom;

     m_fdT0 = new TFolder("fdt0","fdt0");
     hlist->Add(m_fdT0);

     m_fdResiWire = new TFolder("resiWire", "resiWire");
     hlist->Add(m_fdResiWire);

     char hname[200];
     for(int i=0; i<NWIRE; i++){
	  int lay = pGeom -> getWire(i) -> getLayerId();
	  int cel = pGeom -> getWire(i) -> getCellId();

	  sprintf(hname, "mResi%04d_Lay%02d_Cell%03d_L", i, lay, cel);
	  m_hleft[i] = new TH1F(hname, "", 400, -2.0, 2.0);
	  m_fdT0 -> Add(m_hleft[i]);

	  sprintf(hname, "mResi%04d_Lay%02d_Cell%03d_R", i, lay, cel);
	  m_hright[i] = new TH1F(hname, "", 400, -2.0, 2.0);
	  m_fdT0 -> Add(m_hright[i]);
     }
     m_hLrResiSum = new TH1F("mLrResiSum", "", 200, -0.5, 0.5);
     m_fdResiWire->Add(m_hLrResiSum);

     m_hLrResiSub = new TH1F("mLrResiSub", "", 200, -0.5, 0.5);
     m_fdResiWire->Add(m_hLrResiSub);
}

void T0Calib::mergeHist(TFile* fhist){
     CalibBase::mergeHist(fhist);

     char hname[200];
     TFolder* fd = (TFolder*)fhist->Get("fdT0");
     for(int i=0; i<NWIRE; i++){
	  int lay = m_pGeom -> getWire(i) -> getLayerId();
	  int cel = m_pGeom -> getWire(i) -> getCellId();

	  sprintf(hname, "Resi%04d_Lay%02d_Cell%03d_L", i, lay, cel);
	  TH1F* hl = (TH1F*)fd->FindObjectAny(hname);
	  m_hleft[i]->Add(hl);

	  sprintf(hname, "Resi%04d_Lay%02d_Cell%03d_R", i, lay, cel);
	  TH1F* hr = (TH1F*)fd->FindObjectAny(hname);
	  m_hright[i]->Add(hr);
     }
}

void T0Calib::calib(MdcCalibConst* calconst, TObjArray* newXtList, TObjArray* r2tList){
     CalibBase::calib(calconst, newXtList, r2tList);

     double vdr = 0.03;
     for(int i=0; i<NWIRE; i++){
	  Stat_t entry_l = m_hleft[i] -> GetEntries();
	  double mean_l = m_hleft[i] -> GetMean();
	  if(entry_l < 50) mean_l = 0.0;

	  Stat_t entry_r = m_hright[i] -> GetEntries();
	  double mean_r = m_hright[i] -> GetMean();
	  if(entry_r < 50) mean_r = 0.0;

	  double delt0 = 0.5 * (mean_l + mean_r) / vdr;
	  double resiLrSum = 0.5 * (mean_l + mean_r);
	  double resiLrSub = 0.5 * (mean_l - mean_r);
	  m_hLrResiSum->Fill(resiLrSum);
	  m_hLrResiSub->Fill(resiLrSub);

	  double t0 = calconst->getT0(i);
	  t0 += delt0;
	  calconst->resetT0(i, t0);
	  calconst->resetDelT0(i, delt0);
     }
     renameHist();
}

void T0Calib::renameHist(){
     char hname[200];
     m_fdT0->SetName("fdT0");
     for(int i=0; i<NWIRE; i++){
	  int lay = m_pGeom -> getWire(i) -> getLayerId();
	  int cel = m_pGeom -> getWire(i) -> getCellId();
	  sprintf(hname, "Resi%04d_Lay%02d_Cell%03d_L", i, lay, cel);
	  m_hleft[i]->SetName(hname);
	  sprintf(hname, "Resi%04d_Lay%02d_Cell%03d_R", i, lay, cel);
	  m_hright[i]->SetName(hname);
     }

     m_fdResiWire->SetName("ResiWire");
     m_hLrResiSum->SetName("LrResiSum");
     m_hLrResiSub->SetName("LrResiSub");
}
