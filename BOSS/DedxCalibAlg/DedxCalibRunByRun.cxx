#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <string>
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TTree.h"

#include "DedxCalibAlg/DedxCalibRunByRun.h"

using namespace std;

int runNo = 0;

DedxCalibRunByRun::DedxCalibRunByRun(const std::string& name, ISvcLocator* pSvcLocator): DedxCalib(name, pSvcLocator){}


void DedxCalibRunByRun::BookHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibRunByRun::BookHists()"<<endreq;

    ReadRecFileList();
    runNo = m_recFileVector.size();

    m_hist = new TH1F("dEdx_allRun","dEdx_allRun",NumHistBins,MinHistValue1,MaxHistValue1);
    m_hist->GetXaxis()->SetTitle("dE/dx");
    m_hist->GetYaxis()->SetTitle("entries");
    m_rungain = new TH1F*[runNo];
    stringstream hlabel;
    for(int i=0; i<runNo; i++)
    {
        hlabel.str("");
        hlabel<<"dEdx_Run_"<<i;
        m_rungain[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(),NumHistBins,MinHistValue1,MaxHistValue1);
    }
}

void DedxCalibRunByRun::FillHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibRunByRun::FillHists()"<<endreq;

    TFile* f;
    TTree* n103;
    string runlist;

    int ndedxhit=0;
    double dEdx[100]={0},pathlength[100]={0},wid[100]={0},layid[100]={0},dd_in[100]={0},eangle[100]={0},zhit[100]={0};
    double dedx=0;
    float runNO=0,runFlag=0,costheta=0,tes=0;
    vector<double> phlist; 
    for(int i=0; i<runNo; i++)
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
        for(int j=0;j<n103->GetEntries();j++)
        {
            phlist.clear();
            n103->GetEntry(j);
            for(int k=0;k<ndedxhit;k++) 
            {
                dEdx[k] = exsvc->StandardHitCorrec(0,runFlag,2,runNO,pathlength[k],wid[k],layid[k],dEdx[k],dd_in[k],eangle[k],costheta);
                phlist.push_back(dEdx[k]);
            }
            dedx = cal_dedx_bitrunc(truncate, phlist);
            dedx = exsvc->StandardTrackCorrec(0, runFlag, 2, runNO, dedx, costheta, tes);
            m_rungain[i]->Fill(dedx);
            m_hist->Fill(dedx);
        }
        cout<<"runNO = "<<runNO<<endl;
        m_runNOVector.push_back(runNO);
    }
}

void DedxCalibRunByRun::AnalyseHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibRunByRun::AnalyseHists()"<<endreq;

    m_hist->Fit("gaus", "Q" );
    for(int i=0; i<runNo; i++)
    {
        m_rungain[i]->Fit("gaus", "Q" );
    }
}

void DedxCalibRunByRun::WriteHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibRunByRun::WriteHists()"<<endreq;

    TFile* f = new TFile(m_rootfile.c_str(), "recreate");

    Double_t gainpar=0, resolpar=0;
    gainpar = m_hist -> GetFunction("gaus") -> GetParameter(1);
    resolpar = m_hist -> GetFunction("gaus") -> GetParameter(2);

    TTree* gain = new TTree("gaincalib", "gaincalib");
    gain -> Branch("gain", &gainpar, "gain[1]/D");
    gain->Fill();
    TTree* resol = new TTree("resolcalib", "resolcalib");
    resol -> Branch("resol", &resolpar, "resol[1]/D");
    resol->Fill();

    Int_t runno=0;
    Double_t runmean=0, rungain=0, runresol=0;
    TTree* runbyrun = new TTree("runcalib", "runcalib");
    runbyrun -> Branch("runno", &runno, "runno/I");
    runbyrun -> Branch("runmean", &runmean, "runmean/D");
    runbyrun -> Branch("rungain", &rungain, "rungain/D");
    runbyrun -> Branch("runresol", &runresol, "runresol/D");
    for(int i=0; i<runNo; i++)
    {
        runno = m_runNOVector[i];
        runmean = m_rungain[i] -> GetFunction("gaus") -> GetParameter(1);
        runresol = m_rungain[i] -> GetFunction("gaus") -> GetParameter(2);
        rungain = runmean/NormalMean;
        cout<<"i = "<<i<<"   runno = "<<runno <<"  @  runmean = "<<runmean<<"  @  rungain = "<<rungain<<"  @  runresol =  "<<runresol<<endl;
        runbyrun -> Fill();
    }

    m_hist->Write();
    for(int i=0; i<runNo; i++) m_rungain[i]->Write();
    gain->Write();
    resol -> Write();
    runbyrun -> Write();

    delete m_hist;
    for(int i=0; i<runNo; i++)  delete m_rungain[i];
    delete gain;
    delete resol;
    delete runbyrun;
    f->Close(); 
}
