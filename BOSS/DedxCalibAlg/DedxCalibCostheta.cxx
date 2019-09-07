#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <string>
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"

#include "DedxCalibAlg/DedxCalibCostheta.h"

#define Size  700000

using namespace std;

const double StepCostheta = (CosthetaMax-CosthetaMin)/NumBinCostheta;

DedxCalibCostheta::DedxCalibCostheta(const std::string& name, ISvcLocator* pSvcLocator): DedxCalib(name, pSvcLocator){
  declareProperty("Debug", m_debug=false);
  declareProperty("DebugMin", m_debug_min=2);
  declareProperty("DebugMax", m_debug_max=2);
}

void DedxCalibCostheta::BookHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibCostheta::BookHists()"<<endreq;

    ReadRecFileList();

    m_costheta = new TH1F*[NumBinCostheta];
    m_pos_costheta = new TH1F*[NumBinCostheta];
    m_neg_costheta = new TH1F*[NumBinCostheta];
    m_chi  = new TH1F*[NumBinCostheta];
    m_pos_chi = new TH1F*[NumBinCostheta];
    m_neg_chi = new TH1F*[NumBinCostheta];
    stringstream hlabel;
    for(int i=0;i<NumBinCostheta;i++)
    {
        hlabel.str("");
        hlabel<<"dEdx_costheta"<<i;
        m_costheta[i]=new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinHistValue1, MaxHistValue1);
        hlabel.str("");
        hlabel<<"pos_dEdx_costheta"<<i;
        m_pos_costheta[i]=new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinHistValue1, MaxHistValue1);
        hlabel.str("");
        hlabel<<"neg_dEdx_costheta"<<i;
        m_neg_costheta[i]=new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinHistValue1, MaxHistValue1);
        hlabel.str("");
        hlabel<<"chi_costheta"<<i;
        m_chi[i]=new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinChiValue, MaxChiValue);
        hlabel.str("");
        hlabel<<"pos_chi_costheta"<<i;
        m_pos_chi[i]=new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinChiValue, MaxChiValue);
        hlabel.str("");
        hlabel<<"neg_chi_costheta"<<i;
        m_neg_chi[i]=new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinChiValue, MaxChiValue);
    }
    hlabel.str("");
    hlabel<<"dEdxVsCostheta";
    m_dEdxVsCostheta = new TH1F(hlabel.str().c_str(),"dEdx vs costheta",NumBinCostheta, CosthetaMin, CosthetaMax);
    m_dEdxVsCostheta->GetXaxis()->SetTitle("cos#theta");
    m_dEdxVsCostheta->GetYaxis()->SetTitle("dE/dx");
    hlabel.str("");
    hlabel<<"pos_dEdxVsCostheta";
    m_pos_dEdxVsCostheta = new TH1F(hlabel.str().c_str(),"positron dEdx vs costheta",NumBinCostheta, CosthetaMin, CosthetaMax);
    m_pos_dEdxVsCostheta->GetXaxis()->SetTitle("cos#theta");
    m_pos_dEdxVsCostheta->GetYaxis()->SetTitle("dE/dx");
    hlabel.str("");
    hlabel<<"neg_dEdxVsCostheta";
    m_neg_dEdxVsCostheta = new TH1F(hlabel.str().c_str(),"electron dEdx vs costheta",NumBinCostheta, CosthetaMin, CosthetaMax);
    m_neg_dEdxVsCostheta->GetXaxis()->SetTitle("cos#theta");
    m_neg_dEdxVsCostheta->GetYaxis()->SetTitle("dE/dx");

    Vec_dedx.clear();
    Vec_costheta.clear();
}

void DedxCalibCostheta::FillHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibCostheta::FillHists()"<<endreq;

    TFile* f;
    TTree* n103;
    string runlist;

    int ndedxhit=0;
    double dEdx[100]={0},pathlength[100]={0},wid[100]={0},layid[100]={0},dd_in[100]={0},eangle[100]={0},zhit[100]={0};
    double dedx=0;
    float runNO=0,runFlag=0,costheta=0,tes=0,charge=0,recalg=0,ptrk=0,chidedx=0;
    int usedhit=0;
    vector<double> phlist; 
    cout<<"m_recFileVector.size()= "<<m_recFileVector.size()<<endl;
    for(unsigned int i=0; i<m_recFileVector.size(); i++)
    {
      runlist = m_recFileVector[i];
      f = new TFile(runlist.c_str());
      n103 = (TTree*)f->Get("n103");
      n103-> SetBranchAddress("ndedxhit", &ndedxhit);
      n103-> SetBranchAddress("dEdx_hit",dEdx);
      n103-> SetBranchAddress("pathlength_hit",pathlength);
      n103-> SetBranchAddress("wid_hit",wid);
      n103-> SetBranchAddress("layid_hit",layid);
      n103-> SetBranchAddress("dd_in_hit",dd_in);
      n103-> SetBranchAddress("eangle_hit",eangle);
      n103-> SetBranchAddress("zhit_hit",zhit);
      n103-> SetBranchAddress("runNO",&runNO);
      n103-> SetBranchAddress("runFlag",&runFlag);
      n103-> SetBranchAddress("costheta",&costheta);
      n103-> SetBranchAddress("t0",&tes);
      n103-> SetBranchAddress("charge",&charge);
      n103-> SetBranchAddress("recalg",&recalg);
      n103-> SetBranchAddress("ndedxhit",&ndedxhit);
      n103-> SetBranchAddress("ptrk",&ptrk);
      n103-> SetBranchAddress("chidedx_e",&chidedx);
      for(int j=0;j<n103->GetEntries();j++)
        {
	  phlist.clear();
	  n103->GetEntry(j);
	  if(costheta>CosthetaMax || costheta<CosthetaMin) continue;
	  if(tes>1400) continue; 
	  for(int k=0;k<ndedxhit;k++)
            {
	      dEdx[k] = exsvc->StandardHitCorrec(0,runFlag,2,runNO,pathlength[k],wid[k],layid[k],dEdx[k],dd_in[k],eangle[k],costheta);
	      phlist.push_back(dEdx[k]);
            }
	  dedx = cal_dedx_bitrunc(truncate, phlist);
	  int iCos = (Int_t)floor((costheta-CosthetaMin)/StepCostheta);
	  double pre_dedx = dedx;
	  if(m_debug && iCos>=m_debug_min && iCos<=m_debug_max) 
	    cout << "before cor, dedx " << pre_dedx << "  with cos(theta) " << costheta << endl;
	  dedx = exsvc->StandardTrackCorrec(0, runFlag, 2, runNO, dedx, costheta, tes);
	  if(m_debug && iCos>=m_debug_min && iCos<=m_debug_max) 
	    cout << "after cor, dedx " << dedx << " with gain " << pre_dedx/dedx << endl;
	  m_costheta[iCos]->Fill(dedx);
	  if(charge>0) m_pos_costheta[iCos]->Fill(dedx);
	  if(charge<0) m_neg_costheta[iCos]->Fill(dedx);

	  usedhit = ndedxhit*truncate;
	  set_dEdx(1,dedx,recalg,runFlag,usedhit,ptrk,acos(costheta),1.5,vFlag,tes);
	  double chi = m_chi_ex[ParticleFlag];
	  //cout<<"chidedx= "<<chidedx<<" particleType= "<<ParticleFlag<<"  new chi= "<<chi<<endl;
	  m_chi[iCos]->Fill(chi);
	  if(charge>0) m_pos_chi[iCos]->Fill(chi);
	  if(charge<0) m_neg_chi[iCos]->Fill(chi);

	  Vec_dedx.push_back(dedx);
	  Vec_costheta.push_back(costheta);
        }
    }
}

void DedxCalibCostheta::AnalyseHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibCostheta::AnalyseHists()"<<endreq;

    gStyle->SetOptFit(1111);
    for(int i=0; i<NumBinCostheta; i++)
    {
      if(m_debug) cout << "num of bin " << i << endl;
      if(m_costheta[i]->GetEntries()>100) m_costheta[i]->Fit("gaus", "Q" );
      if(m_pos_costheta[i]->GetEntries()>100) m_pos_costheta[i]->Fit("gaus", "Q" );
      if(m_neg_costheta[i]->GetEntries()>100) m_neg_costheta[i]->Fit("gaus", "Q" );
      if(m_chi[i]->GetEntries()>100) m_chi[i]->Fit("gaus", "Q" );
      if(m_pos_chi[i]->GetEntries()>100) m_pos_chi[i]->Fit("gaus", "Q" );
      if(m_neg_chi[i]->GetEntries()>100) m_neg_chi[i]->Fit("gaus", "Q" );
    }
}

void DedxCalibCostheta::WriteHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibCostheta::WriteHists()"<<endreq;

    double chientryNo[NumBinCostheta]={0},chimean[NumBinCostheta]={0},chimeanerr[NumBinCostheta]={0},chisigma[NumBinCostheta]={0},chisq[NumBinCostheta]={0};
    double pos_chientryNo[NumBinCostheta]={0},pos_chimean[NumBinCostheta]={0},pos_chimeanerr[NumBinCostheta]={0},pos_chisigma[NumBinCostheta]={0},pos_chisq[NumBinCostheta]={0};
    double neg_chientryNo[NumBinCostheta]={0},neg_chimean[NumBinCostheta]={0},neg_chimeanerr[NumBinCostheta]={0},neg_chisigma[NumBinCostheta]={0},neg_chisq[NumBinCostheta]={0};
    double fitentryNo[NumBinCostheta]={0},fitmean[NumBinCostheta]={0},fitmeanerr[NumBinCostheta]={0},fitsigma[NumBinCostheta]={0},gain[NumBinCostheta]={0},fitchisq[NumBinCostheta]={0};
    double pos_fitentryNo[NumBinCostheta]={0},pos_fitmean[NumBinCostheta]={0},pos_fitmeanerr[NumBinCostheta]={0},pos_fitsigma[NumBinCostheta]={0},pos_gain[NumBinCostheta]={0},pos_fitchisq[NumBinCostheta]={0};
    double neg_fitentryNo[NumBinCostheta]={0},neg_fitmean[NumBinCostheta]={0},neg_fitmeanerr[NumBinCostheta]={0},neg_fitsigma[NumBinCostheta]={0},neg_gain[NumBinCostheta]={0},neg_fitchisq[NumBinCostheta]={0};
    double cosBin[NumBinCostheta]={0};

    for(int i=0;i<NumBinCostheta;i++)
    {
        cosBin[i] = (i+0.5)*StepCostheta + CosthetaMin;

        chientryNo[i] = m_chi[i]->GetEntries();
	if(m_debug) cout << "get results at " << i << " bin with chi entries " << chientryNo[i] << endl;
        if(m_chi[i]->GetFunction("gaus"))
        {
            chimean[i] = m_chi[i]->GetFunction("gaus")->GetParameter(1);
            chimeanerr[i] = m_chi[i]->GetFunction("gaus")->GetParError(1);
            chisigma[i] = m_chi[i]->GetFunction("gaus")->GetParameter(2);
            chisq[i] = (m_chi[i]->GetFunction("gaus")->GetChisquare())/(m_chi[i]->GetFunction("gaus")->GetNDF());
        }
        pos_chientryNo[i] = m_pos_chi[i]->GetEntries();
	if(m_debug) cout << "get results at " << i << " bin with pos_chi entries " << pos_chientryNo[i] << endl;
        if(m_pos_chi[i]->GetFunction("gaus"))
        {
            pos_chimean[i] = m_pos_chi[i]->GetFunction("gaus")->GetParameter(1);
            pos_chimeanerr[i] = m_pos_chi[i]->GetFunction("gaus")->GetParError(1);
            pos_chisigma[i] = m_pos_chi[i]->GetFunction("gaus")->GetParameter(2);
            pos_chisq[i] = (m_pos_chi[i]->GetFunction("gaus")->GetChisquare())/(m_pos_chi[i]->GetFunction("gaus")->GetNDF());
        }
        neg_chientryNo[i] = m_neg_chi[i]->GetEntries();
	if(m_debug) cout << "get results at " << i << " bin with neg_chi entries " << neg_chientryNo[i] << endl;
        if(m_neg_chi[i]->GetFunction("gaus"))
        {
            neg_chimean[i] = m_neg_chi[i]->GetFunction("gaus")->GetParameter(1);
            neg_chimeanerr[i] = m_neg_chi[i]->GetFunction("gaus")->GetParError(1);
            neg_chisigma[i] = m_neg_chi[i]->GetFunction("gaus")->GetParameter(2);
            neg_chisq[i] = (m_neg_chi[i]->GetFunction("gaus")->GetChisquare())/(m_neg_chi[i]->GetFunction("gaus")->GetNDF());
        }

        fitentryNo[i] = m_costheta[i]->GetEntries();
	if(m_debug) cout << "get results at " << i << " bin with fit entries " << fitentryNo[i] << endl;
        if(m_costheta[i]->GetFunction("gaus"))
        {
            fitmean[i] = m_costheta[i]->GetFunction("gaus")->GetParameter(1);
            fitmeanerr[i] = m_costheta[i]->GetFunction("gaus")->GetParError(1);
            fitsigma[i] = m_costheta[i]->GetFunction("gaus")->GetParameter(2);
            gain[i] = fitmean[i]/NormalMean;
            fitchisq[i] = (m_costheta[i]->GetFunction("gaus")->GetChisquare())/(m_costheta[i]->GetFunction("gaus")->GetNDF());
        }
        pos_fitentryNo[i] = m_pos_costheta[i]->GetEntries();
	if(m_debug) cout << "get results at " << i << " bin with pos_fit entries " << pos_fitentryNo[i] << endl;
        if(m_pos_costheta[i]->GetFunction("gaus"))
        {
            pos_fitmean[i] = m_pos_costheta[i]->GetFunction("gaus")->GetParameter(1);
            pos_fitmeanerr[i] = m_pos_costheta[i]->GetFunction("gaus")->GetParError(1);
            pos_fitsigma[i] = m_pos_costheta[i]->GetFunction("gaus")->GetParameter(2);
            pos_gain[i] = pos_fitmean[i]/NormalMean;
            pos_fitchisq[i] = (m_pos_costheta[i]->GetFunction("gaus")->GetChisquare())/(m_pos_costheta[i]->GetFunction("gaus")->GetNDF());
        }
        neg_fitentryNo[i] = m_neg_costheta[i]->GetEntries();
	if(m_debug) cout << "get results at " << i << " bin with neg_fit entries " << neg_fitentryNo[i] << endl;
        if(m_neg_costheta[i]->GetFunction("gaus"))
        {
            neg_fitmean[i] = m_neg_costheta[i]->GetFunction("gaus")->GetParameter(1);
            neg_fitmeanerr[i] = m_neg_costheta[i]->GetFunction("gaus")->GetParError(1);
            neg_fitsigma[i] = m_neg_costheta[i]->GetFunction("gaus")->GetParameter(2);
            neg_gain[i] = neg_fitmean[i]/NormalMean;
            neg_fitchisq[i] = (m_neg_costheta[i]->GetFunction("gaus")->GetChisquare())/(m_neg_costheta[i]->GetFunction("gaus")->GetNDF());
        }

        if(fitentryNo[i]>100) m_dEdxVsCostheta -> SetBinContent(i+1,fitmean[i]);
        if(pos_fitentryNo[i]>100) m_pos_dEdxVsCostheta -> SetBinContent(i+1,pos_fitmean[i]);
        if(neg_fitentryNo[i]>100) m_neg_dEdxVsCostheta -> SetBinContent(i+1,neg_fitmean[i]);
    }

    double dedx1[Size] = {0};
    double costheta1[Size] = {0};
    cout << "Vec_dedx.size() = " << Vec_dedx.size() << endl;
    for(unsigned int i=0;i<Size && i< Vec_dedx.size();i++)
    {
        dedx1[i] = Vec_dedx[i];
        costheta1[i] = Vec_costheta[i];
        //cout<<"i= "<<i<<"dedx= "<<dedx1[i]<<" costheta= "<<costheta1[i]<<endl;
    }

    log<<MSG::INFO<<"begin generating root file!!! "<<endreq;
    TFile* f = new TFile(m_rootfile.c_str(), "RECREATE");
    for(int i=0;i<NumBinCostheta;i++)
    {
        m_chi[i]->Write();
        m_pos_chi[i]->Write();
        m_neg_chi[i]->Write();
        m_costheta[i]->Write();
        m_pos_costheta[i]->Write();
        m_neg_costheta[i]->Write();
    }
    m_dEdxVsCostheta->Write();
    m_pos_dEdxVsCostheta->Write();
    m_neg_dEdxVsCostheta->Write();

    TTree *costhetacalib = new TTree("costhetacalib","costhetacalib");
    costhetacalib -> Branch("chientryNo",chientryNo,"chientryNo[80]/D");
    costhetacalib -> Branch("chimean",chimean,"chimean[80]/D");
    costhetacalib -> Branch("chimeanerr",chimeanerr,"chimeanerr[80]/D");
    costhetacalib -> Branch("chisigma",chisigma,"chisigma[80]/D");
    costhetacalib -> Branch("chisq",chisq,"chisq[80]/D");
    costhetacalib -> Branch("pos_chientryNo",pos_chientryNo,"pos_chientryNo[80]/D");
    costhetacalib -> Branch("pos_chimean",pos_chimean,"pos_chimean[80]/D");
    costhetacalib -> Branch("pos_chimeanerr",pos_chimeanerr,"pos_chimeanerr[80]/D");
    costhetacalib -> Branch("pos_chisigma",pos_chisigma,"pos_chisigma[80]/D");
    costhetacalib -> Branch("pos_chisq",pos_chisq,"pos_chisq[80]/D");
    costhetacalib -> Branch("neg_chientryNo",neg_chientryNo,"neg_chientryNo[80]/D");
    costhetacalib -> Branch("neg_chimean",neg_chimean,"neg_chimean[80]/D");
    costhetacalib -> Branch("neg_chimeanerr",neg_chimeanerr,"neg_chimeanerr[80]/D");
    costhetacalib -> Branch("neg_chisigma",neg_chisigma,"neg_chisigma[80]/D");
    costhetacalib -> Branch("neg_chisq",neg_chisq,"neg_chisq[80]/D");
    costhetacalib -> Branch("fitentryNo", fitentryNo, "fitentryNo[80]/D");
    costhetacalib -> Branch("fitmean", fitmean, "fitmean[80]/D");
    costhetacalib -> Branch("fitmeanerr", fitmeanerr, "fitmeanerr[80]/D"); 
    costhetacalib -> Branch("fitsigma", fitsigma, "fitsigma[80]/D");
    costhetacalib -> Branch("costheta", gain, "gain[80]/D");
    costhetacalib -> Branch("fitchisq", fitchisq, "fitchisq[80]/D");
    costhetacalib -> Branch("pos_fitentryNo", pos_fitentryNo, "pos_fitentryNo[80]/D");
    costhetacalib -> Branch("pos_fitmean", pos_fitmean, "pos_fitmean[80]/D");
    costhetacalib -> Branch("pos_fitmeanerr", pos_fitmeanerr, "pos_fitmeanerr[80]/D");
    costhetacalib -> Branch("pos_fitsigma", pos_fitsigma, "pos_fitsigma[80]/D");
    costhetacalib -> Branch("pos_gain", pos_gain, "pos_gain[80]/D");
    costhetacalib -> Branch("pos_fitchisq", pos_fitchisq, "pos_fitchisq[80]/D");
    costhetacalib -> Branch("neg_fitentryNo", neg_fitentryNo, "neg_fitentryNo[80]/D");
    costhetacalib -> Branch("neg_fitmean", neg_fitmean, "neg_fitmean[80]/D");
    costhetacalib -> Branch("neg_fitmeanerr", neg_fitmeanerr, "neg_fitmeanerr[80]/D");
    costhetacalib -> Branch("neg_fitsigma", neg_fitsigma, "neg_fitsigma[80]/D");
    costhetacalib -> Branch("neg_gain", neg_gain, "neg_gain[80]/D");
    costhetacalib -> Branch("neg_fitchisq", neg_fitchisq, "neg_fitchisq[80]/D");
    costhetacalib -> Branch("cosBin", cosBin, "cosBin[80]/D");
    costhetacalib -> Branch("costheta1",costheta1,"costheta1[700000]/D");
    costhetacalib -> Branch("dedx1",dedx1,"dedx1[700000]/D");
    costhetacalib -> Fill();
    costhetacalib -> Write();

    TCanvas c1("c1", "canvas", 500, 400);
    c1.Print((m_rootfile+".ps[").c_str());
    costhetacalib -> Draw("dedx1:costheta1","dedx1>200 && dedx1<1000");
    c1.Print((m_rootfile+".ps").c_str());
    m_dEdxVsCostheta->Draw();
    c1.Print((m_rootfile+".ps").c_str());
    m_pos_dEdxVsCostheta->Draw();
    c1.Print((m_rootfile+".ps").c_str());
    m_neg_dEdxVsCostheta->Draw();
    c1.Print((m_rootfile+".ps").c_str());
    for(Int_t i=0;i<NumBinCostheta;i++)
    {   
        m_chi[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    for(Int_t i=0;i<NumBinCostheta;i++)
    {
        m_pos_chi[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    for(Int_t i=0;i<NumBinCostheta;i++)
    {
        m_neg_chi[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    for(Int_t i=0;i<NumBinCostheta;i++)
    {
        m_costheta[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    for(Int_t i=0;i<NumBinCostheta;i++)
    {
        m_pos_costheta[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    for(Int_t i=0;i<NumBinCostheta;i++)
    {
        m_neg_costheta[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    c1.Print((m_rootfile+".ps]").c_str());
    f->Close();

    for(Int_t i=0;i<NumBinCostheta;i++)
    {
        delete m_chi[i];
        delete m_pos_chi[i];
        delete m_neg_chi[i];
        delete m_costheta[i];
        delete m_pos_costheta[i];
        delete m_neg_costheta[i];
    }
    delete m_dEdxVsCostheta;
    delete m_pos_dEdxVsCostheta;
    delete m_neg_dEdxVsCostheta;
}
