#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <string>
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"

#include "DedxCalibAlg/DedxCalibLayerGain.h"

using namespace std;

DedxCalibLayerGain::DedxCalibLayerGain(const std::string& name, ISvcLocator* pSvcLocator): DedxCalib(name, pSvcLocator){}

const int layNo = 43;
double m_mean,m_rms;
void calculate(vector<double> phlist)
{
    double mean=0,rms=0;
    for(int i=0;i<phlist.size();i++)
    {
        mean+=phlist[i];
    }
    mean/=phlist.size();
    for(int i=0;i<phlist.size();i++)
    {
        rms += pow((phlist[i]-mean),2);
    }
    //cout<<"phlist.size()= "<<phlist.size()<<"  rms= "<<rms<<endl; 
    rms = sqrt(rms/phlist.size());
    //cout<<"mean = "<<mean<<"  rms= "<<rms<<endl;
    m_mean = mean;
    m_rms = rms;
}
double getMean() {return m_mean;}
double getRms() {return m_rms;}

void DedxCalibLayerGain::BookHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibLayerGain::BookHists()"<<endreq;

    ReadRecFileList();

    m_laygain = new TH1F*[layNo];
    m_laygain_gaus = new TH1F*[layNo];
    stringstream hlabel;
    for(int i=0; i<layNo; i++)
    {
        hlabel.str("");
        hlabel<<"dEdx_Lay_"<<i;
        m_laygain[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinHistValue, MaxHistValue);
        hlabel.str("");
        hlabel<<"dEdx_gaus_Lay_"<<i;
        m_laygain_gaus[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinHistValue1, MaxHistValue1);         
    }
}

void DedxCalibLayerGain::FillHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibLayerGain::FillHists()"<<endreq;

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
            //if(wid>=6731 && wid<=6739) continue;
            if(layid <8)
            {if(dedx<MinADCValuecut || dedx>MaxADCValuecut || pathlength>RPhi_PathMaxCut || pathlength<Iner_RPhi_PathMinCut || driftdist>Iner_DriftDistCut) continue;}
            else
            {if(dedx<MinADCValuecut || dedx>MaxADCValuecut || pathlength>RPhi_PathMaxCut || pathlength<Out_RPhi_PathMinCut || driftdist>Out_DriftDistCut) continue;}
            dedx = exsvc->StandardHitCorrec(0,runFlag,2,runNO,pathlength,wid,layid,dedx,dd_in,eangle,costheta);
            dedx = exsvc->StandardTrackCorrec(0, runFlag, 2, runNO, dedx, costheta, tes);
            m_laygain[(int)layid]->Fill(dedx);

            m_vector[(int)layid].push_back(dedx);
        }
    }

}

void DedxCalibLayerGain::AnalyseHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibLayerGain::AnalyseHists()"<<endreq;

    TF1* func;
    Double_t entry=0,mean=0,rms=0;
    Double_t binmax=0;
    Double_t lp[3]={0};
    gStyle->SetOptFit(1111);

    stringstream hlabel;
    double dedxt;
    vector<double> phlist;
    vector<double> phlist_gaus;
    for(int i=0; i<layNo; i++)
    {
        entry = m_laygain[i]->GetEntries();
        if(entry<10) continue;
        mean = m_laygain[i]->GetMean();
        rms = m_laygain[i]->GetRMS();
        binmax = m_laygain[i]->GetMaximumBin();
        lp[1] = m_laygain[i]->GetBinCenter(binmax);
        lp[2] = 200;
        lp[0] = (m_laygain[i]->GetBinContent(binmax)+m_laygain[i]->GetBinContent(binmax-1)+m_laygain[i]->GetBinContent(binmax+1))/3;

        if(m_phShapeFlag==0)
        {
            func = new TF1("mylan",mylan,10,3000,4);
            func->SetParameters(entry, mean, rms, -0.9);
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

        m_laygain[i]->Fit(func, "r" );


        //******* begin truncated dedx fitting **************************//
        for(int j=0;j<m_vector[i].size();j+=100)
        {
            for(int k=0;k<100;k++) phlist.push_back(m_vector[i][j+k]);
            dedxt = cal_dedx_bitrunc(truncate, phlist);
            phlist_gaus.push_back(dedxt);
            phlist.clear();
        }

        hlabel.str("");
        hlabel<<"dEdx_gaus_Lay_"<<i;
        calculate(phlist_gaus);
        //cout<<getMean()-10*getRms()<<"   "<<getMean()+10*getRms()<<endl;
        m_laygain_gaus[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, getMean()-10*getRms(), getMean()+10*getRms());
        for(int j=0;j<phlist_gaus.size();j++)  m_laygain_gaus[i]->Fill(phlist_gaus[j]);
        phlist_gaus.clear();
        m_laygain_gaus[i]->Fit("gaus","Q");


    }
}

void DedxCalibLayerGain::WriteHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibLayerGain::WriteHists()"<<endreq;


    double entryNo[layNo]={0},mean[layNo]={0},sigma[layNo]={0},proper[layNo]={0},fitmean[layNo]={0},fitmeanerr[layNo]={0},fitsigma[layNo]={0},layerg[layNo]={0},layer[layNo]={0},chisq[layNo]={0};
    double fitmean_gaus[layNo]={0},fitmeanerr_gaus[layNo]={0},fitsigma_gaus[layNo]={0},layerg_gaus[layNo]={0};
    for(int i=0; i<layNo; i++)
    {
        fitmean_gaus[i] = m_laygain_gaus[i]->GetFunction("gaus")->GetParameter(1);
        fitmeanerr_gaus[i] = m_laygain_gaus[i]->GetFunction("gaus")->GetParError(1);
        fitsigma_gaus[i] = m_laygain_gaus[i]->GetFunction("gaus")->GetParameter(2);

        entryNo[i] = m_laygain[i]->GetEntries();
        mean[i] = m_laygain[i]->GetMean();
        sigma[i] = m_laygain[i]->GetRMS();
        proper[i] = m_laygain[i]->GetBinCenter(m_laygain[i]->GetMaximumBin());
        if(entryNo[i]<10) continue;

        if(m_phShapeFlag==0)
        {
            fitmean[i] = m_laygain[i]->GetFunction("mylan")->GetParameter(1);
            fitmeanerr[i] = m_laygain[i]->GetFunction("mylan")->GetParError(1);
            fitsigma[i] = m_laygain[i]->GetFunction("mylan")->GetParameter(2);
            chisq[i] = (m_laygain[i]->GetFunction("mylan")-> GetChisquare())/(m_laygain[i]->GetFunction("mylan")->  GetNDF());
        }
        else if(m_phShapeFlag==1)
        {
            fitmean[i] = m_laygain[i]->GetFunction("landaun")->GetParameter(1);
            fitmeanerr[i] = m_laygain[i]->GetFunction("landaun")->GetParError(1);
            fitsigma[i] = m_laygain[i]->GetFunction("landaun")->GetParameter(2);
            chisq[i] = (m_laygain[i]->GetFunction("landaun")-> GetChisquare())/(m_laygain[i]->GetFunction("landaun")->  GetNDF());
        }
        else if(m_phShapeFlag==2)
        {
            fitmean[i] = m_laygain[i]->GetFunction("Landau")->GetParameter(1);
            fitmeanerr[i] = m_laygain[i]->GetFunction("Landau")->GetParError(1);
            fitsigma[i] = m_laygain[i]->GetFunction("Landau")->GetParameter(2);
            chisq[i] = (m_laygain[i]->GetFunction("Landau")-> GetChisquare())/(m_laygain[i]->GetFunction("Landau")->  GetNDF());
        }
        else if(m_phShapeFlag==3)
        {
            fitmean[i] = m_laygain[i]->GetFunction("Vavilov")->GetParameter(1);
            fitmeanerr[i] = m_laygain[i]->GetFunction("Vavilov")->GetParError(1);
            fitsigma[i] = m_laygain[i]->GetFunction("Vavilov")->GetParameter(2);
            chisq[i] = (m_laygain[i]->GetFunction("Vavilov")-> GetChisquare())/(m_laygain[i]->GetFunction("Vavilov")->  GetNDF());
        }
        else if(m_phShapeFlag==4)
        {
            fitmean[i] = m_laygain[i]->GetFunction("AsymGauss")->GetParameter(1);
            fitmeanerr[i] = m_laygain[i]->GetFunction("AsymGauss")->GetParError(1);
            fitsigma[i] = m_laygain[i]->GetFunction("AsymGauss")->GetParameter(2);
            chisq[i] = (m_laygain[i]->GetFunction("AsymGauss")-> GetChisquare())/(m_laygain[i]->GetFunction("AsymGauss")->  GetNDF());
        }
        //cout<<"fitmean[i]= "<<fitmean[i]<<" fitmeanerr[i]= "<<fitmeanerr[i]<<" fitsigma[i]= "<<fitsigma[i]<<endl;
    }

    double sum=0, sum_gaus=0, n=0;
    for(int i=4; i<layNo; i++)
    {
        if(fitmean[i]>0 && fitmeanerr[i]>0 && fitmeanerr[i]<10 && fitsigma[i]>0 && fitsigma[i]<200 && entryNo[i]>1500)
        {
            sum += fitmean[i];
            sum_gaus+= fitmean_gaus[i];
            n++;
        }    
    }
    sum/=n;
    sum_gaus/=n;
    cout<<"average value of good layer: "<<sum<<endl;

    for(int i=0;i<layNo;i++)
    {
        layerg[i] = fitmean[i]/sum;
        layerg_gaus[i] = fitmean_gaus[i]/sum_gaus;
        layer[i] = i;
    }


    log<<MSG::INFO<<"begin generating root file!!! "<<endreq;
    TFile* f = new TFile(m_rootfile.c_str(), "recreate");
    for(int i=0;i<layNo;i++)
    {
        m_laygain[i]->Write();
        m_laygain_gaus[i]->Write();
    }

    TTree* layergcalib = new TTree("layergcalib", "layergcalib");
    layergcalib->Branch("layerg_gaus", layerg_gaus, "layerg_gaus[43]/D");
    layergcalib->Branch("layerg", layerg, "layerg[43]/D");
    layergcalib->Branch("layer", layer, "layer[43]/D");
    layergcalib->Branch("entryNo", entryNo, "entryNo[43]/D");
    layergcalib->Branch("mean", mean, "mean[43]/D");
    layergcalib->Branch("sigma", sigma, "sigma[43]/D");           
    layergcalib->Branch("fitmean", fitmean, "fitmean[43]/D");
    layergcalib->Branch("fitmeanerr", fitmeanerr, "fitmeanerr[43]/D");
    layergcalib->Branch("fitsigma", fitsigma, "fitsigma[43]/D");
    layergcalib->Branch("chisq", chisq, "chisq[43]/D");
    layergcalib->Branch("fitmean_gaus", fitmean_gaus, "fitmean_gaus[43]/D");
    layergcalib->Branch("fitmeanerr_gaus", fitmeanerr_gaus, "fitmeanerr_gaus[43]/D");
    layergcalib->Branch("fitsigma_gaus", fitsigma_gaus, "fitsigma_gaus[43]/D");
    layergcalib->Fill();
    layergcalib->Write();

    f->Close();

    TCanvas c1("c1", "canvas", 500, 400);
    c1.Print((m_rootfile+".ps[").c_str());
    for(int i=0;i<layNo;i++)
    {
        m_laygain[i]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    c1.Print((m_rootfile+".ps]").c_str());

    for(int i=0;i<layNo;i++)
    {
        delete m_laygain[i];
        delete m_laygain_gaus[i];
    }
}
