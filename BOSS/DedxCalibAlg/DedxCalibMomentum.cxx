#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <string>
#include "cmath"
#include "TTree.h" 
#include "TH1F.h"
#include "TF1.h" 
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"

#include "DedxCalibAlg/DedxCalibMomentum.h"

#define Size  700000

using namespace std;

const double pMin=0.2;
const double pMax=2.1;
const int nbins = 50;
double bin_step = (pMax-pMin)/nbins;
const double chihist_min=-10;
const double chihist_max=10;
const double dedxhist_min=350;
const double dedxhist_max=750;
const double hits_min=0;
const double hits_max=35;

DedxCalibMomentum::DedxCalibMomentum(const std::string& name, ISvcLocator* pSvcLocator): DedxCalib(name, pSvcLocator){}

void DedxCalibMomentum::BookHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibMomentum::BookHists()"<<endreq;

    ReadRecFileList(); 

    m_chi = new TH1F*[nbins];
    m_pos_chi = new TH1F*[nbins];
    m_neg_chi = new TH1F*[nbins];
    m_dedx = new TH1F*[nbins];
    m_pos_dedx = new TH1F*[nbins];
    m_neg_dedx = new TH1F*[nbins];
    m_hits = new TH1F*[nbins];

    stringstream hlabel;
    for(int i=0;i<nbins;i++)
    {
        hlabel.str("");
        hlabel<<"chi"<<i;
        m_chi[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), nbins, chihist_min, chihist_max); 
        hlabel.str("");
        hlabel<<"pos_chi"<<i;
        m_pos_chi[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), nbins, chihist_min, chihist_max);
        hlabel.str("");
        hlabel<<"neg_chi"<<i;
        m_neg_chi[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), nbins, chihist_min, chihist_max);
        hlabel.str("");
        hlabel<<"dedx"<<i;
        m_dedx[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), nbins, dedxhist_min, dedxhist_max);
        hlabel.str(""); 
        hlabel<<"pos_dedx"<<i; 
        m_pos_dedx[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), nbins, dedxhist_min, dedxhist_max);
        hlabel.str("");
        hlabel<<"neg_dedx"<<i;  
        m_neg_dedx[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), nbins, dedxhist_min, dedxhist_max);
        hlabel.str("");               
        hlabel<<"hits"<<i; 
        m_hits[i] =  new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), nbins, hits_min, hits_max); 
    }

    Vec_dedx.clear();
    Vec_ptrk.clear();
}

void DedxCalibMomentum::FillHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibMomentum::FillHists()"<<endreq;

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
    for(int i=0; i<m_recFileVector.size(); i++)
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
            if(ptrk>pMax || ptrk<pMin) continue;
            if(tes>1400) continue;
            for(int k=0;k<ndedxhit;k++)
            {
                dEdx[k] = exsvc->StandardHitCorrec(0,runFlag,2,runNO,pathlength[k],wid[k],layid[k],dEdx[k],dd_in[k],eangle[k],costheta);
                phlist.push_back(dEdx[k]);
            }
            dedx = cal_dedx_bitrunc(truncate, phlist);
            dedx = exsvc->StandardTrackCorrec(0, runFlag, 2, runNO, dedx, costheta, tes);
            int ipBin = (Int_t)floor((ptrk-pMin)/bin_step);
            m_dedx[ipBin]->Fill(dedx);
            if(charge>0) m_pos_dedx[ipBin]->Fill(dedx);
            if(charge<0) m_neg_dedx[ipBin]->Fill(dedx);

            usedhit = ndedxhit*truncate;
            set_dEdx(1,dedx,recalg,runFlag,usedhit,ptrk,acos(costheta),1.5,vFlag,tes);
            double chi = m_chi_ex[ParticleFlag];
            //cout<<"chidedx= "<<chidedx<<" particleType= "<<ParticleFlag<<"  new chi= "<<chi<<endl;
            m_chi[ipBin]->Fill(chi);
            if(charge>0) m_pos_chi[ipBin]->Fill(chi);
            if(charge<0) m_neg_chi[ipBin]->Fill(chi);
            m_hits[ipBin]->Fill(usedhit);

            Vec_dedx.push_back(dedx);
            Vec_ptrk.push_back(ptrk);
        }
    }
}

void DedxCalibMomentum::AnalyseHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibMomentum::AnalyseHists()"<<endreq;

    gStyle->SetOptFit(1111);
    for(int i=0; i<nbins; i++)
    {
        m_dedx[i]->Fit("gaus", "Q" );
        m_pos_dedx[i]->Fit("gaus", "Q" );
        m_neg_dedx[i]->Fit("gaus", "Q" );
        m_chi[i]->Fit("gaus", "Q" );
        m_pos_chi[i]->Fit("gaus", "Q" );
        m_neg_chi[i]->Fit("gaus", "Q" );
    }
}

void DedxCalibMomentum::WriteHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibMomentum::WriteHists()"<< endreq;

    double chientryNo[nbins]={0},chimean[nbins]={0},chimeanerr[nbins]={0},chisigma[nbins]={0},chisq[nbins]={0};
    double pos_chientryNo[nbins]={0},pos_chimean[nbins]={0},pos_chimeanerr[nbins]={0},pos_chisigma[nbins]={0},pos_chisq[nbins]={0};
    double neg_chientryNo[nbins]={0},neg_chimean[nbins]={0},neg_chimeanerr[nbins]={0},neg_chisigma[nbins]={0},neg_chisq[nbins]={0};
    double fitentryNo[nbins]={0},fitmean[nbins]={0},fitmeanerr[nbins]={0},fitsigma[nbins]={0},fitchisq[nbins]={0};
    double pos_fitentryNo[nbins]={0},pos_fitmean[nbins]={0},pos_fitmeanerr[nbins]={0},pos_fitsigma[nbins]={0},pos_fitchisq[nbins]={0};
    double neg_fitentryNo[nbins]={0},neg_fitmean[nbins]={0},neg_fitmeanerr[nbins]={0},neg_fitsigma[nbins]={0},neg_fitchisq[nbins]={0};
    double hits_mean[nbins]={0},hits_sigma[nbins]={0};
    double pBin[nbins]={0};

    for(int i=0;i<nbins;i++)
    {
        pBin[i] = (i+0.5)*bin_step + pMin;

        chientryNo[i] = m_chi[i]->GetEntries();
        if(chientryNo[i]>100)
        {
            chimean[i] = m_chi[i]->GetFunction("gaus")->GetParameter(1);
            chimeanerr[i] = m_chi[i]->GetFunction("gaus")->GetParError(1);
            chisigma[i] = m_chi[i]->GetFunction("gaus")->GetParameter(2);
            chisq[i] = (m_chi[i]->GetFunction("gaus")->GetChisquare())/(m_chi[i]->GetFunction("gaus")->GetNDF());
        }
        pos_chientryNo[i] = m_pos_chi[i]->GetEntries();
        if(pos_chientryNo[i]>100)
        {    
            pos_chimean[i] = m_pos_chi[i]->GetFunction("gaus")->GetParameter(1);
            pos_chimeanerr[i] = m_pos_chi[i]->GetFunction("gaus")->GetParError(1);
            pos_chisigma[i] = m_pos_chi[i]->GetFunction("gaus")->GetParameter(2);
            pos_chisq[i] = (m_pos_chi[i]->GetFunction("gaus")->GetChisquare())/(m_pos_chi[i]->GetFunction("gaus")->GetNDF());
        }
        neg_chientryNo[i] = m_neg_chi[i]->GetEntries();
        if(neg_chientryNo[i]>100)
        {
            neg_chimean[i] = m_neg_chi[i]->GetFunction("gaus")->GetParameter(1);
            neg_chimeanerr[i] = m_neg_chi[i]->GetFunction("gaus")->GetParError(1);
            neg_chisigma[i] = m_neg_chi[i]->GetFunction("gaus")->GetParameter(2);
            neg_chisq[i] = (m_neg_chi[i]->GetFunction("gaus")->GetChisquare())/(m_neg_chi[i]->GetFunction("gaus")->GetNDF()); 
        }

        fitentryNo[i] = m_dedx[i]->GetEntries();
        if(fitentryNo[i]>100)
        {
            fitmean[i] = m_dedx[i]->GetFunction("gaus")->GetParameter(1);
            fitmeanerr[i] = m_dedx[i]->GetFunction("gaus")->GetParError(1);
            fitsigma[i] = m_dedx[i]->GetFunction("gaus")->GetParameter(2);
            fitchisq[i] = (m_dedx[i]->GetFunction("gaus")->GetChisquare())/(m_dedx[i]->GetFunction("gaus")->GetNDF());
        }
        pos_fitentryNo[i] = m_pos_dedx[i]->GetEntries(); 
        if(pos_fitentryNo[i]>100)
        {
            pos_fitmean[i] = m_pos_dedx[i]->GetFunction("gaus")->GetParameter(1);
            pos_fitmeanerr[i] = m_pos_dedx[i]->GetFunction("gaus")->GetParError(1);
            pos_fitsigma[i] = m_pos_dedx[i]->GetFunction("gaus")->GetParameter(2);     
            pos_fitchisq[i] = (m_pos_dedx[i]->GetFunction("gaus")->GetChisquare())/(m_pos_dedx[i]->GetFunction("gaus")->GetNDF());
        }   
        neg_fitentryNo[i] = m_neg_dedx[i]->GetEntries();
        if(neg_fitentryNo[i]>100)
        {
            neg_fitmean[i] = m_neg_dedx[i]->GetFunction("gaus")->GetParameter(1);
            neg_fitmeanerr[i] = m_neg_dedx[i]->GetFunction("gaus")->GetParError(1);
            neg_fitsigma[i] = m_neg_dedx[i]->GetFunction("gaus")->GetParameter(2);
            neg_fitchisq[i] = (m_neg_dedx[i]->GetFunction("gaus")->GetChisquare())/(m_neg_dedx[i]->GetFunction("gaus")->GetNDF());
        } 

        hits_mean[i] = m_hits[i]->GetMean();
        hits_sigma[i] = m_hits[i]->GetRMS();
    }

    double dedx1[Size] = {0};
    double ptrk1[Size] = {0};
    cout << "Vec_dedx.size() = " << Vec_dedx.size() << endl;
    for(int i=0;i<Size && i< Vec_dedx.size();i++)
    {       
        dedx1[i] = Vec_dedx[i];
        ptrk1[i] = Vec_ptrk[i];
        //cout<<"i= "<<i<<"dedx= "<<dedx1[i]<<" ptrk= "<<ptrk1[i]<<endl;
    } 

    log<<MSG::INFO<<"begin generating root file!!! "<<endreq;    
    TFile* f = new TFile(m_rootfile.c_str(), "RECREATE");
    for(int i=0;i<nbins;i++)
    {
        m_chi[i]->Write();
        m_pos_chi[i]->Write();
        m_neg_chi[i]->Write();
        m_dedx[i]->Write();
        m_pos_dedx[i]->Write();
        m_neg_dedx[i]->Write();
        m_hits[i]->Write();
    }

    TTree* momInfor = new TTree("momInfor","momInfor");
    momInfor->Branch("chientryNo",chientryNo,"chientryNo[50]/D");
    momInfor->Branch("chimean",chimean,"chimean[50]/D");
    momInfor->Branch("chimeanerr",chimeanerr,"chimeanerr[50]/D");
    momInfor->Branch("chisigma",chisigma,"chisigma[50]/D");
    momInfor->Branch("chisq",chisq,"chisq[50]/D");
    momInfor->Branch("pos_chientryNo",pos_chientryNo,"pos_chientryNo[50]/D");
    momInfor->Branch("pos_chimean",pos_chimean,"pos_chimean[50]/D");
    momInfor->Branch("pos_chimeanerr",pos_chimeanerr,"pos_chimeanerr[50]/D");
    momInfor->Branch("pos_chisigma",pos_chisigma,"pos_chisigma[50]/D");
    momInfor->Branch("pos_chisq",pos_chisq,"pos_chisq[50]/D");
    momInfor->Branch("neg_chientryNo",neg_chientryNo,"neg_chientryNo[50]/D");
    momInfor->Branch("neg_chimean",neg_chimean,"neg_chimean[50]/D");
    momInfor->Branch("neg_chimeanerr",neg_chimeanerr,"neg_chimeanerr[50]/D");
    momInfor->Branch("neg_chisigma",neg_chisigma,"neg_chisigma[50]/D");
    momInfor->Branch("neg_chisq",neg_chisq,"neg_chisq[50]/D");
    momInfor->Branch("fitentryNo",fitentryNo,"fitentryNo[50]/D");
    momInfor->Branch("fitmean",fitmean,"fitmean[50]/D");
    momInfor->Branch("fitmeanerr",fitmeanerr,"fitmeanerr[50]/D");
    momInfor->Branch("fitsigma",fitsigma,"fitsigma[50]/D");
    momInfor->Branch("fitchisq",fitchisq,"fitchisq[50]/D");
    momInfor->Branch("pos_fitentryNo",pos_fitentryNo,"pos_fitentryNo[50]/D");
    momInfor->Branch("pos_fitmean",pos_fitmean,"pos_fitmean[50]/D");
    momInfor->Branch("pos_fitmeanerr",pos_fitmeanerr,"pos_fitmeanerr[50]/D");
    momInfor->Branch("pos_fitsigma",pos_fitsigma,"pos_fitsigma[50]/D");
    momInfor->Branch("pos_fitchisq",pos_fitchisq,"pos_fitchisq[50]/D");
    momInfor->Branch("neg_fitentryNo",neg_fitentryNo,"neg_fitentryNo[50]/D");
    momInfor->Branch("neg_fitmean",neg_fitmean,"neg_fitmean[50]/D");
    momInfor->Branch("neg_fitmeanerr",neg_fitmeanerr,"neg_fitmeanerr[50]/D");
    momInfor->Branch("neg_fitsigma",neg_fitsigma,"neg_fitsigma[50]/D");
    momInfor->Branch("neg_fitchisq",neg_fitchisq,"neg_fitchisq[50]/D");
    momInfor->Branch("hits_mean",hits_mean,"hits_mean[50]/D");
    momInfor->Branch("hits_sigma",hits_sigma,"hits_sigma[50]/D");
    momInfor->Branch("pBin",pBin,"pBin[50]/D");
    momInfor-> Branch("ptrk1",ptrk1,"ptrk1[700000]/D");
    momInfor-> Branch("dedx1",dedx1,"dedx1[700000]/D");
    momInfor->Fill();
    momInfor->Write();

    TCanvas c1("c1", "canvas", 500, 400);
    c1.Print((m_rootfile+".ps[").c_str());
    momInfor -> Draw("dedx1:ptrk1","dedx1>200 && dedx1<1200");
    c1.Print((m_rootfile+".ps").c_str());
    for(Int_t i=0;i<nbins;i++)
    {
        m_chi[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    for(Int_t i=0;i<nbins;i++)
    {
        m_pos_chi[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    for(Int_t i=0;i<nbins;i++)
    {
        m_neg_chi[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    for(Int_t i=0;i<nbins;i++)
    {
        m_dedx[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    for(Int_t i=0;i<nbins;i++)
    {
        m_pos_dedx[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    for(Int_t i=0;i<nbins;i++)
    {
        m_neg_dedx[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    c1.Print((m_rootfile+".ps]").c_str()); 
    f->Close();

    for(int i=0;i<nbins;i++)
    {
        delete m_chi[i];
        delete m_pos_chi[i];
        delete m_neg_chi[i];
        delete m_dedx[i];
        delete m_pos_dedx[i];
        delete m_neg_dedx[i];
        delete m_hits[i];
    }
}
