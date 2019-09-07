#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <string>
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"

#include "DedxCalibAlg/DedxCalibWireGain.h"

using namespace std;

DedxCalibWireGain::DedxCalibWireGain(const std::string& name, ISvcLocator* pSvcLocator): DedxCalib(name, pSvcLocator){}

const int wireNo = 6796;

void DedxCalibWireGain::BookHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibWireGain::BookHists()"<<endreq;

    ReadRecFileList();

    m_wiregain = new TH1F*[wireNo];
    stringstream hlabel;
    for(int i=0; i<wireNo; i++)
    {
        hlabel.str("");
        hlabel<<"dEdx_Wire_"<<i;
        m_wiregain[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinHistValue, MaxHistValue);
    }
}

void DedxCalibWireGain::FillHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibWireGain::FillHists()"<<endreq;

    TFile* f;
    TTree* n102;
    string runlist;

    double dedx=0;
    float runNO=0,runFlag=0,pathlength=0,wid=0,layid=0,dd_in=0,driftdist=0,eangle=0,zhit=0,costheta=0,tes=0,ptrk=0;
    cout<<"m_recFileVector.size()= "<<m_recFileVector.size()<<endl;
    for(int i=0; i<m_recFileVector.size(); i++)
    {
        runlist = m_recFileVector[i];
        cout<<"runlist: "<<runlist.c_str()<<endl;
        f = new TFile(runlist.c_str());
        n102 = (TTree*)f->Get("n102");
        n102-> SetBranchAddress("adc_raw",&dedx);
        n102-> SetBranchAddress("path_rphi",&pathlength);
        n102-> SetBranchAddress("wire",&wid);
        n102-> SetBranchAddress("layer",&layid);
        n102-> SetBranchAddress("doca_in",&dd_in);
        n102-> SetBranchAddress("driftdist",&driftdist);
        n102-> SetBranchAddress("eangle",&eangle);
        n102-> SetBranchAddress("zhit",&zhit);
        n102-> SetBranchAddress("runNO",&runNO);
        n102-> SetBranchAddress("runFlag",&runFlag);
        n102-> SetBranchAddress("costheta1",&costheta);
        n102-> SetBranchAddress("t01",&tes);
        n102->SetBranchAddress("ptrk1",&ptrk);

        for(int j=0;j<n102->GetEntries();j++)
        {
            n102->GetEntry(j);
            if(tes>1400) continue;
            if (ptrk<0.1) continue;
            if(layid <8)
            {if(dedx<MinADCValuecut || dedx>MaxADCValuecut || pathlength>RPhi_PathMaxCut || pathlength<Iner_RPhi_PathMinCut || driftdist>Iner_DriftDistCut) continue;}
            else
            {if(dedx<MinADCValuecut || dedx>MaxADCValuecut || pathlength>RPhi_PathMaxCut || pathlength<Out_RPhi_PathMinCut || driftdist>Out_DriftDistCut) continue;}
            dedx = exsvc->StandardHitCorrec(0,runFlag,2,runNO,pathlength,wid,layid,dedx,dd_in,eangle,costheta);
            dedx = exsvc->StandardTrackCorrec(0, runFlag, 2, runNO, dedx, costheta, tes);
            m_wiregain[(int)wid]->Fill(dedx);
        }
    }    
}

void DedxCalibWireGain::AnalyseHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibWireGain::AnalyseHists()"<<endreq;

    TF1* func;
    Double_t entry=0,mean=0,rms=0;
    Double_t binmax=0;
    Double_t lp[3]={0};
    gStyle->SetOptFit(1111);
    for(int wire=0; wire<wireNo; wire++)
    {
        entry = m_wiregain[wire]->Integral();
        if(entry<10) continue;
        mean = m_wiregain[wire]->GetMean();
        rms = m_wiregain[wire]->GetRMS();
        binmax = m_wiregain[wire]->GetMaximumBin();
        lp[1] = m_wiregain[wire]->GetBinCenter(binmax);
        lp[2] = 200;
        lp[0] = (m_wiregain[wire]->GetBinContent(binmax)+m_wiregain[wire]->GetBinContent(binmax-1)+m_wiregain[wire]->GetBinContent(binmax+1))/3;

        if(m_phShapeFlag==0)
        {
            func = new TF1("mylan",mylan,10,3000,4);
            func->SetParameters(entry, mean, rms, -0.8);
	    func->SetParLimits(0, 0, entry);
	    func->SetParLimits(1, 0, mean);
	    func->SetParLimits(2, 10, rms);
	    func->SetParLimits(3, -3, 3);
        }
        else if(m_phShapeFlag==1)   
        {
            func = new TF1("landaun",landaun,10,3000,3);
            func->SetParameters(lp[0], lp[1], lp[2] );
        }
        else if(m_phShapeFlag==2)  
        {
            func = new TF1("Landau",Landau,10,3000,2);
            func->SetParameters(0.7*mean, rms );
        }
        else if(m_phShapeFlag==3)  
        {
            func = new TF1("Vavilov",Vavilov,10,3000,2);
            func->SetParameters(0.05, 0.6);
        }
        else if(m_phShapeFlag==4) 
        {
            func = new TF1("AsymGauss",AsymGauss,10,3000,4);
            func->SetParameters(entry, mean, rms, 1.0 );
        }
        func->SetLineWidth(2.1);
        func->SetLineColor(2);

        m_wiregain[wire]->Fit(func, "rq", "", mean-rms, mean+rms/2 );
    }
}

void DedxCalibWireGain::WriteHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibWireGain::WriteHists()"<<endreq;


    double entryNo[wireNo]={0},mean[wireNo]={0},sigma[wireNo]={0},proper[wireNo]={0},fitmean[wireNo]={0},fitmeanerr[wireNo]={0},fitsigma[wireNo]={0},wireg[wireNo]={0},wire[wireNo]={0},chisq[wireNo]={0};
    for(int i=0; i<wireNo; i++)
    {
        entryNo[i] = m_wiregain[i]->Integral();
        mean[i] = m_wiregain[i]->GetMean();
        sigma[i] = m_wiregain[i]->GetRMS();
        proper[i] = m_wiregain[i]->GetBinCenter(m_wiregain[i]->GetMaximumBin());
        if(entryNo[i]<10) continue;

        if(m_phShapeFlag==0)
        {
            fitmean[i] = m_wiregain[i]->GetFunction("mylan")->GetParameter(1);
            fitmeanerr[i] = m_wiregain[i]->GetFunction("mylan")->GetParError(1);
            fitsigma[i] = m_wiregain[i]->GetFunction("mylan")->GetParameter(2);
            chisq[i] = (m_wiregain[i]->GetFunction("mylan")-> GetChisquare())/(m_wiregain[i]->GetFunction("mylan")->  GetNDF());
        }
        else if(m_phShapeFlag==1)
        {
            fitmean[i] = m_wiregain[i]->GetFunction("landaun")->GetParameter(1);
            fitmeanerr[i] = m_wiregain[i]->GetFunction("landaun")->GetParError(1);
            fitsigma[i] = m_wiregain[i]->GetFunction("landaun")->GetParameter(2);
            chisq[i] = (m_wiregain[i]->GetFunction("landaun")-> GetChisquare())/(m_wiregain[i]->GetFunction("landaun")->  GetNDF());
        }
        else if(m_phShapeFlag==2)
        {
            fitmean[i] = m_wiregain[i]->GetFunction("Landau")->GetParameter(1);
            fitmeanerr[i] = m_wiregain[i]->GetFunction("Landau")->GetParError(1);
            fitsigma[i] = m_wiregain[i]->GetFunction("Landau")->GetParameter(2);
            chisq[i] = (m_wiregain[i]->GetFunction("Landau")-> GetChisquare())/(m_wiregain[i]->GetFunction("Landau")->  GetNDF());
        }
        else if(m_phShapeFlag==3)
        {
            fitmean[i] = m_wiregain[i]->GetFunction("Vavilov")->GetParameter(1);
            fitmeanerr[i] = m_wiregain[i]->GetFunction("Vavilov")->GetParError(1);
            fitsigma[i] = m_wiregain[i]->GetFunction("Vavilov")->GetParameter(2);
            chisq[i] = (m_wiregain[i]->GetFunction("Vavilov")-> GetChisquare())/(m_wiregain[i]->GetFunction("Vavilov")->  GetNDF());
        }
        else if(m_phShapeFlag==4)
        {
            fitmean[i] = m_wiregain[i]->GetFunction("AsymGauss")->GetParameter(1);
            fitmeanerr[i] = m_wiregain[i]->GetFunction("AsymGauss")->GetParError(1);
            fitsigma[i] = m_wiregain[i]->GetFunction("AsymGauss")->GetParameter(2);
            chisq[i] = (m_wiregain[i]->GetFunction("AsymGauss")-> GetChisquare())/(m_wiregain[i]->GetFunction("AsymGauss")->  GetNDF());
        }
        //cout<<"fitmean[i]= "<<fitmean[i]<<" fitmeanerr[i]= "<<fitmeanerr[i]<<" fitsigma[i]= "<<fitsigma[i]<<endl;
    }

    double Norm=0, count=0;
    for(int i=188; i<wireNo; i++)
    {
        if(fitmean[i]>0 && fitmeanerr[i]>0 && fitmeanerr[i]<10 && fitsigma[i]>0 && fitsigma[i]<200 && entryNo[i]>1500)
        {
            Norm += fitmean[i];
            count++;
        }    
    }
    Norm/=count;
    cout<<"count=  "<<count<<"   average value of good wire: "<<Norm<<endl;
    //  double Norm(550); // use 550 instead of average because we will handle wireg later outside of the program

    for(int i=0;i<wireNo;i++)
    {
        wireg[i] = fitmean[i]/Norm;
        wire[i] = i;
    }


    log<<MSG::INFO<<"begin generating root file!!! "<<endreq;
    TFile* f = new TFile(m_rootfile.c_str(), "recreate");
    for(int i=0;i<wireNo;i++) m_wiregain[i]->Write();

    TTree* wiregcalib = new TTree("wiregcalib", "wiregcalib");
    wiregcalib->Branch("wireg", wireg, "wireg[6796]/D");
    wiregcalib->Branch("wire", wire, "wire[6796]/D");
    wiregcalib->Branch("entryNo", entryNo, "entryNo[6796]/D");
    wiregcalib->Branch("proper", proper, "proper[6796]/D");
    wiregcalib->Branch("mean", mean, "mean[6796]/D");
    wiregcalib->Branch("sigma", sigma, "sigma[6796]/D");           
    wiregcalib->Branch("fitmean", fitmean, "fitmean[6796]/D");
    wiregcalib->Branch("fitmeanerr", fitmeanerr, "fitmeanerr[6796]/D");
    wiregcalib->Branch("fitsigma", fitsigma, "fitsigma[6796]/D");
    wiregcalib->Branch("chisq", chisq, "chisq[6796]/D");
    wiregcalib->Fill();
    wiregcalib->Write();

    f->Close();

    TCanvas c1("c1", "canvas", 500, 400);
    c1.Print((m_rootfile+".ps[").c_str());
    for(int i=0;i<wireNo;i++)
    {
        m_wiregain[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    c1.Print((m_rootfile+".ps]").c_str());

    for(int i=0;i<wireNo;i++) delete m_wiregain[i];

}
