#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <string>
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"

#include "DedxCalibAlg/DedxCalibEAng.h"

using namespace std;

const double StepEAng = (PhiMax-PhiMin)/NumSlices;

DedxCalibEAng::DedxCalibEAng(const std::string& name, ISvcLocator* pSvcLocator): DedxCalib(name, pSvcLocator){}

void DedxCalibEAng::BookHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibEAng::BookHists()"<<endreq;

    ReadRecFileList();

    m_eangle = new TH1F*[NumSlices];
    m_pos_eangle = new TH1F*[NumSlices];
    m_neg_eangle = new TH1F*[NumSlices];
    stringstream hlabel;
    for(int i=0;i<NumSlices;i++)
    {
        hlabel.str("");
        hlabel<<"dEdx"<<"_eang"<<i;
        m_eangle[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinHistValue, MaxHistValue);
        hlabel.str("");
        hlabel<<"pos_dEdx"<<"_eang"<<i;
        m_pos_eangle[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinHistValue, MaxHistValue);  
        hlabel.str("");
        hlabel<<"neg_dEdx"<<"_eang"<<i;
        m_neg_eangle[i] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinHistValue, MaxHistValue);
    }
    hlabel.str("");
    hlabel<<"dEdxVsEAng";
    m_EAngAverdE = new TH1F(hlabel.str().c_str(),"dEdx & EAng",NumSlices,PhiMin,PhiMax);
    hlabel.str("");
    hlabel<<"pos_dEdxVsEAng";
    m_pos_EAngAverdE = new TH1F(hlabel.str().c_str(),"pos_dEdx & EAng",NumSlices,PhiMin,PhiMax);
    hlabel.str("");
    hlabel<<"neg_dEdxVsEAng";
    m_neg_EAngAverdE = new TH1F(hlabel.str().c_str(),"neg_dEdx & EAng",NumSlices,PhiMin,PhiMax);

}

void DedxCalibEAng::FillHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibEAng::FillHists()"<<endreq;

    TFile* f;
    TTree* n102;
    string runlist;

    double dedx=0;
    float runNO=0,runFlag=0,pathlength=0,wid=0,layid=0,dd_in=0,driftdist=0,eangle=0,zhit=0,costheta=0,tes=0,ptrk=0,charge=0;
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
        n102->SetBranchAddress("charge",&charge);

        for(int j=0;j<n102->GetEntries();j++)
        {
            n102->GetEntry(j);
            if(eangle >0.25) eangle = eangle -0.5;
            else if(eangle <-0.25) eangle = eangle +0.5;
            if(eangle>PhiMin && eangle<PhiMax)
            {
                if(tes>1400) continue;
                if (ptrk<0.1) continue;
                if(layid <4) continue;
                else if(layid <8)
                {if(dedx<MinADCValuecut || dedx>MaxADCValuecut || pathlength>RPhi_PathMaxCut || pathlength<Iner_RPhi_PathMinCut || driftdist>Iner_DriftDistCut) continue;}
                else
                {if(dedx<MinADCValuecut || dedx>MaxADCValuecut || pathlength>RPhi_PathMaxCut || pathlength<Out_RPhi_PathMinCut || driftdist>Out_DriftDistCut) continue;}
                int iEAng = 0;
                iEAng = (int) ((eangle-PhiMin)/StepEAng);
                dedx = exsvc->StandardHitCorrec(0,runFlag,2,runNO,pathlength,wid,layid,dedx,dd_in,eangle,costheta);
                dedx = exsvc->StandardTrackCorrec(0, runFlag, 2, runNO, dedx, costheta, tes);
                m_eangle[iEAng]->Fill(dedx);
                if(charge>0) m_pos_eangle[iEAng]->Fill(dedx);
                if(charge<0) m_neg_eangle[iEAng]->Fill(dedx);
            }
        }
    }
}

void DedxCalibEAng::AnalyseHists()
{           
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibEAng::AnalyseHists()"<<endreq;

    TF1* func;
    Double_t entry=0,mean=0,rms=0;
    Double_t binmax=0;
    Double_t lp[3]={0};
    gStyle->SetOptFit(1111);
    for(Int_t ip=0;ip<NumSlices;ip++)
    {
        entry = m_eangle[ip]->Integral();
        if(entry<10) continue;
        mean = m_eangle[ip]->GetMean();
        rms = m_eangle[ip]->GetRMS();
        binmax = m_eangle[ip]->GetMaximumBin();
        lp[1] = m_eangle[ip]->GetBinCenter(binmax);
        lp[2] = 200;
        lp[0] = (m_eangle[ip]->GetBinContent(binmax)+m_eangle[ip]->GetBinContent(binmax-1)+m_eangle[ip]->GetBinContent(binmax+1))/3;

        if(m_phShapeFlag==0)
        {
            func = new TF1("mylan",mylan,10,3000,4);
	    func->SetParLimits(0, 0, entry);
	    func->SetParLimits(1, 0, mean);
	    func->SetParLimits(2, 10, rms);
	    func->SetParLimits(3, -1, 0);
	    func->SetParameters(entry/36., mean-200, rms/2.3, -0.5);
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

        m_eangle[ip]->Fit(func, "rq"    );//    ,     "", mean-rms, mean+rms/2);
        m_pos_eangle[ip]->Fit(func, "rq");//   ,  "", mean-rms, mean+rms/2);
        m_neg_eangle[ip]->Fit(func, "rq");//   ,  "", mean-rms, mean+rms/2);
    }
}

void DedxCalibEAng::WriteHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibEAng::WriteHists()"<<endreq;

    double entryNo[NumSlices]={0},mean[NumSlices]={0},sigma[NumSlices]={0},fitmean[NumSlices]={0},fitmeanerr[NumSlices]={0},fitsigma[NumSlices]={0},gain[NumSlices]={0},chisq[NumSlices]={0};
    double pos_entryNo[NumSlices]={0},pos_mean[NumSlices]={0},pos_sigma[NumSlices]={0},pos_fitmean[NumSlices]={0},pos_fitmeanerr[NumSlices]={0},pos_fitsigma[NumSlices]={0},pos_gain[NumSlices]={0},pos_chisq[NumSlices]={0};
    double neg_entryNo[NumSlices]={0},neg_mean[NumSlices]={0},neg_sigma[NumSlices]={0},neg_fitmean[NumSlices]={0},neg_fitmeanerr[NumSlices]={0},neg_fitsigma[NumSlices]={0},neg_gain[NumSlices]={0},neg_chisq[NumSlices]={0};
    double Ip_eangle[NumSlices]={0},eangle[NumSlices]={0};

    double Norm=0;
    int count=0;
    for(Int_t ip=0;ip<NumSlices;ip++)
    {
        Ip_eangle[ip] = ip;
        eangle[ip] = (ip+0.5)*StepEAng + PhiMin;

        entryNo[ip]= m_eangle[ip]->Integral();
        mean[ip] = m_eangle[ip]->GetMean();
        sigma[ip] = m_eangle[ip]->GetRMS();
        pos_entryNo[ip]= m_pos_eangle[ip]->Integral();
        pos_mean[ip] = m_pos_eangle[ip]->GetMean();
        pos_sigma[ip] = m_pos_eangle[ip]->GetRMS();
        neg_entryNo[ip]= m_neg_eangle[ip]->Integral();
        neg_mean[ip] = m_neg_eangle[ip]->GetMean();
        neg_sigma[ip] = m_neg_eangle[ip]->GetRMS();
        if(entryNo[ip]<10 || pos_entryNo[ip]<10 || neg_entryNo[ip]<10) continue;

        if(m_phShapeFlag==0)
        {
            fitmean[ip] = m_eangle[ip]->GetFunction("mylan")->GetParameter(1);
            fitmeanerr[ip] = m_eangle[ip]->GetFunction("mylan")->GetParError(1);
            fitsigma[ip] = m_eangle[ip]->GetFunction("mylan")->GetParameter(2);
            chisq[ip] = (m_eangle[ip]->GetFunction("mylan")-> GetChisquare())/(m_eangle[ip]->GetFunction("mylan")->  GetNDF());

            pos_fitmean[ip] = m_pos_eangle[ip]->GetFunction("mylan")->GetParameter(1);
            pos_fitmeanerr[ip] = m_pos_eangle[ip]->GetFunction("mylan")->GetParError(1);
            pos_fitsigma[ip] = m_pos_eangle[ip]->GetFunction("mylan")->GetParameter(2);
            pos_chisq[ip] = (m_pos_eangle[ip]->GetFunction("mylan")-> GetChisquare())/(m_pos_eangle[ip]->GetFunction("mylan")->  GetNDF());

            neg_fitmean[ip] = m_neg_eangle[ip]->GetFunction("mylan")->GetParameter(1);
            neg_fitmeanerr[ip] = m_neg_eangle[ip]->GetFunction("mylan")->GetParError(1);
            neg_fitsigma[ip] = m_neg_eangle[ip]->GetFunction("mylan")->GetParameter(2);
            neg_chisq[ip] = (m_neg_eangle[ip]->GetFunction("mylan")-> GetChisquare())/(m_neg_eangle[ip]->GetFunction("mylan")->  GetNDF());
        }
        if(m_phShapeFlag==1)
        {
            fitmean[ip] = m_eangle[ip]->GetFunction("landaun")->GetParameter(1);
            fitmeanerr[ip] = m_eangle[ip]->GetFunction("landaun")->GetParError(1);
            fitsigma[ip] = m_eangle[ip]->GetFunction("landaun")->GetParameter(2);
            chisq[ip] = (m_eangle[ip]->GetFunction("landaun")-> GetChisquare())/(m_eangle[ip]->GetFunction("mylan")->  GetNDF());

            pos_fitmean[ip] = m_pos_eangle[ip]->GetFunction("landaun")->GetParameter(1);
            pos_fitmeanerr[ip] = m_pos_eangle[ip]->GetFunction("landaun")->GetParError(1);
            pos_fitsigma[ip] = m_pos_eangle[ip]->GetFunction("landaun")->GetParameter(2);
            pos_chisq[ip] = (m_pos_eangle[ip]->GetFunction("landaun")-> GetChisquare())/(m_pos_eangle[ip]->GetFunction("landaun")->  GetNDF());

            neg_fitmean[ip] = m_neg_eangle[ip]->GetFunction("landaun")->GetParameter(1);
            neg_fitmeanerr[ip] = m_neg_eangle[ip]->GetFunction("landaun")->GetParError(1);
            neg_fitsigma[ip] = m_neg_eangle[ip]->GetFunction("landaun")->GetParameter(2);
            neg_chisq[ip] = (m_neg_eangle[ip]->GetFunction("landaun")-> GetChisquare())/(m_neg_eangle[ip]->GetFunction("landaun")->  GetNDF());
        }
        if(m_phShapeFlag==2)
        {
            fitmean[ip] = m_eangle[ip]->GetFunction("Landau")->GetParameter(1);
            fitmeanerr[ip] = m_eangle[ip]->GetFunction("Landau")->GetParError(1);
            fitsigma[ip] = m_eangle[ip]->GetFunction("Landau")->GetParameter(2);
            chisq[ip] = (m_eangle[ip]->GetFunction("Landau")-> GetChisquare())/(m_eangle[ip]->GetFunction("Landau")->  GetNDF());

            pos_fitmean[ip] = m_pos_eangle[ip]->GetFunction("Landau")->GetParameter(1);
            pos_fitmeanerr[ip] = m_pos_eangle[ip]->GetFunction("Landau")->GetParError(1);
            pos_fitsigma[ip] = m_pos_eangle[ip]->GetFunction("Landau")->GetParameter(2);
            pos_chisq[ip] = (m_pos_eangle[ip]->GetFunction("Landau")-> GetChisquare())/(m_pos_eangle[ip]->GetFunction("Landau")->  GetNDF());

            neg_fitmean[ip] = m_neg_eangle[ip]->GetFunction("Landau")->GetParameter(1);
            neg_fitmeanerr[ip] = m_neg_eangle[ip]->GetFunction("Landau")->GetParError(1);
            neg_fitsigma[ip] = m_neg_eangle[ip]->GetFunction("Landau")->GetParameter(2);
            neg_chisq[ip] = (m_neg_eangle[ip]->GetFunction("Landau")-> GetChisquare())/(m_neg_eangle[ip]->GetFunction("Landau")->  GetNDF());
        }
        if(m_phShapeFlag==3)
        {
            fitmean[ip] = m_eangle[ip]->GetFunction("Vavilov")->GetParameter(1);
            fitmeanerr[ip] = m_eangle[ip]->GetFunction("Vavilov")->GetParError(1);
            fitsigma[ip] = m_eangle[ip]->GetFunction("Vavilov")->GetParameter(2);
            chisq[ip] = (m_eangle[ip]->GetFunction("Vavilov")-> GetChisquare())/(m_eangle[ip]->GetFunction("Vavilov")->  GetNDF());

            pos_fitmean[ip] = m_pos_eangle[ip]->GetFunction("Vavilov")->GetParameter(1);
            pos_fitmeanerr[ip] = m_pos_eangle[ip]->GetFunction("Vavilov")->GetParError(1);
            pos_fitsigma[ip] = m_pos_eangle[ip]->GetFunction("Vavilov")->GetParameter(2);
            pos_chisq[ip] = (m_pos_eangle[ip]->GetFunction("Vavilov")-> GetChisquare())/(m_pos_eangle[ip]->GetFunction("Vavilov")->  GetNDF());

            neg_fitmean[ip] = m_neg_eangle[ip]->GetFunction("Vavilov")->GetParameter(1);
            neg_fitmeanerr[ip] = m_neg_eangle[ip]->GetFunction("Vavilov")->GetParError(1);
            neg_fitsigma[ip] = m_neg_eangle[ip]->GetFunction("Vavilov")->GetParameter(2);
            neg_chisq[ip] = (m_neg_eangle[ip]->GetFunction("Vavilov")-> GetChisquare())/(m_neg_eangle[ip]->GetFunction("Vavilov")->  GetNDF());
        }
        if(m_phShapeFlag==4)
        {
            fitmean[ip] = m_eangle[ip]->GetFunction("AsymGauss")->GetParameter(1);
            fitmeanerr[ip] = m_eangle[ip]->GetFunction("AsymGauss")->GetParError(1);
            fitsigma[ip] = m_eangle[ip]->GetFunction("AsymGauss")->GetParameter(2);
            chisq[ip] = (m_eangle[ip]->GetFunction("AsymGauss")-> GetChisquare())/(m_eangle[ip]->GetFunction("AsymGauss")->  GetNDF());

            pos_fitmean[ip] = m_pos_eangle[ip]->GetFunction("AsymGauss")->GetParameter(1);
            pos_fitmeanerr[ip] = m_pos_eangle[ip]->GetFunction("AsymGauss")->GetParError(1);
            pos_fitsigma[ip] = m_pos_eangle[ip]->GetFunction("AsymGauss")->GetParameter(2);
            pos_chisq[ip] = (m_pos_eangle[ip]->GetFunction("AsymGauss")-> GetChisquare())/(m_pos_eangle[ip]->GetFunction("AsymGauss")->  GetNDF());

            neg_fitmean[ip] = m_neg_eangle[ip]->GetFunction("AsymGauss")->GetParameter(1);
            neg_fitmeanerr[ip] = m_neg_eangle[ip]->GetFunction("AsymGauss")->GetParError(1);
            neg_fitsigma[ip] = m_neg_eangle[ip]->GetFunction("AsymGauss")->GetParameter(2);
            neg_chisq[ip] = (m_neg_eangle[ip]->GetFunction("AsymGauss")-> GetChisquare())/(m_neg_eangle[ip]->GetFunction("AsymGauss")->  GetNDF());
        }
        if(entryNo[ip]>1500 && fitmean[ip]>0 && fitmeanerr[ip]>0 && fitmeanerr[ip]<10 && fitsigma[ip]>0 && fitsigma[ip]<200)
        {   
            Norm+=fitmean[ip];
            count++;
        }
    }
    Norm = Norm/count;
    //    Norm = 550; // use an absolute normalization
    cout<<"count=  "<<count<<"  average value of eangle bins: "<<Norm<<endl;
    for(Int_t i=0;i<NumSlices;i++)
    {       
        if(entryNo[i]>10 && fitmean[i]>0 && fitsigma[i]>0)  gain[i]=fitmean[i]/Norm;
        else{
	  gain[i] = 1;
	  fitmeanerr[i]=10000;
	}
        if(pos_entryNo[i]>10 && pos_fitmean[i]>0 && pos_fitsigma[i]>0)  pos_gain[i]=pos_fitmean[i]/Norm;
        else pos_gain[i] = 0;
        if(neg_entryNo[i]>10 && neg_fitmean[i]>0 && neg_fitsigma[i]>0)  neg_gain[i]=neg_fitmean[i]/Norm;
        else neg_gain[i] = 0;
        if(gain[i] != 0){
	  m_EAngAverdE -> SetBinContent(i+1, gain[i]);
	  m_EAngAverdE->SetBinError(i+1, fitmeanerr[i]/Norm);
	}
        if(pos_gain[i] != 0){
	  m_pos_EAngAverdE -> SetBinContent(i+1, pos_gain[i]);
	  m_pos_EAngAverdE->SetBinError(i+1, pos_fitmeanerr[i]/Norm);
	}
        if(neg_gain[i] != 0){
	  m_neg_EAngAverdE -> SetBinContent(i+1, neg_gain[i]);
	  m_neg_EAngAverdE->SetBinError(i+1, neg_fitmeanerr[i]/Norm);
	}
    }

    log<<MSG::INFO<<"begin getting calibration constant!!! "<<endreq;
    double denangle[NumSlices]={0};
    int denangle_entry[1] = {100};
    m_EAngAverdE->Fit("pol1","r","",-0.25,0.25);
    double par0 = m_EAngAverdE->GetFunction("pol1")->GetParameter(0);
    double par1 = m_EAngAverdE->GetFunction("pol1")->GetParameter(1);
    //    double par2 = m_EAngAverdE->GetFunction("pol2")->GetParameter(2);
    // par1 = -par1; // a trick is used here, because we don't really understand
    // for(int i=0;i<NumSlices;i++)   denangle[i] = (par0+par1*eangle[i])/par0;//+par2*pow(eangle[i],2))/par0;
    for(int i=0;i<NumSlices;i++) denangle[i] = gain[i]; // use raw values instead of fit

    log<<MSG::INFO<<"begin generating root file!!! "<<endreq;
    TFile* f = new TFile(m_rootfile.c_str(), "recreate");
    for(Int_t ip=0;ip<NumSlices;ip++)   
    {
        m_eangle[ip]->Write();
        m_pos_eangle[ip]->Write();
        m_neg_eangle[ip]->Write();
    }
    m_EAngAverdE->Write();
    m_pos_EAngAverdE->Write();
    m_neg_EAngAverdE->Write();

    TTree* entracalib = new TTree("entracalib","entracalib");
    entracalib->Branch("1denangle", denangle, "denangle[100]/D");
    entracalib->Branch("1denangle_entry", denangle_entry, "denangle_entry[1]/I");
    entracalib->Fill();
    entracalib-> Write();

    TTree* ddgcalib = new TTree("ddgcalib", "ddgcalib");
    ddgcalib -> Branch("hits", entryNo, "entryNo[100]/D");
    ddgcalib -> Branch("mean", mean, "mean[100]/D");
    ddgcalib -> Branch("sigma", sigma, "sigma[100]/D");
    ddgcalib -> Branch("fitmean", fitmean, "fitmean[100]/D");
    ddgcalib -> Branch("fitmeanerr", fitmeanerr, "fitmeanerr[100]/D");
    ddgcalib -> Branch("chi", fitsigma, "fitsigma[100]/D");
    ddgcalib -> Branch("gain", gain, "gain[100]/D");
    ddgcalib -> Branch("chisq", chisq, "chisq[100]/D");
    ddgcalib -> Branch("pos_hits", pos_entryNo, "pos_entryNo[100]/D");
    ddgcalib -> Branch("pos_mean", pos_mean, "pos_mean[100]/D");
    ddgcalib -> Branch("pos_sigma", pos_sigma, "pos_sigma[100]/D");
    ddgcalib -> Branch("pos_fitmean", pos_fitmean, "pos_fitmean[100]/D");
    ddgcalib -> Branch("pos_fitmeanerr", pos_fitmeanerr, "pos_fitmeanerr[100]/D");
    ddgcalib -> Branch("pos_chi", pos_fitsigma, "pos_fitsigma[100]/D");
    ddgcalib -> Branch("pos_gain", pos_gain, "pos_gain[100]/D");
    ddgcalib -> Branch("pos_chisq", pos_chisq, "pos_chisq[100]/D");
    ddgcalib -> Branch("neg_hits", neg_entryNo, "neg_entryNo[100]/D");
    ddgcalib -> Branch("neg_mean", neg_mean, "neg_mean[100]/D");
    ddgcalib -> Branch("neg_sigma", neg_sigma, "neg_sigma[100]/D");
    ddgcalib -> Branch("neg_fitmean", neg_fitmean, "neg_fitmean[100]/D");   
    ddgcalib -> Branch("neg_fitmeanerr", neg_fitmeanerr, "neg_fitmeanerr[100]/D");
    ddgcalib -> Branch("neg_chi", neg_fitsigma, "neg_fitsigma[100]/D");
    ddgcalib -> Branch("neg_gain", neg_gain, "neg_gain[100]/D");
    ddgcalib -> Branch("neg_chisq", neg_chisq, "neg_chisq[100]/D"); 
    ddgcalib -> Branch("Ip_eangle", Ip_eangle, "Ip_eangle[100]/D");
    ddgcalib -> Branch("eangle", eangle, "eangle[100]/D");
    ddgcalib -> Fill();
    ddgcalib -> Write();
    f->Close();

    TCanvas c1("c1", "canvas", 500, 400);
    c1.Print((m_rootfile+".ps[").c_str());
    m_EAngAverdE->Draw();
    c1.Print((m_rootfile+".ps").c_str());
    m_pos_EAngAverdE->Draw();
    c1.Print((m_rootfile+".ps").c_str());
    m_neg_EAngAverdE->Draw();
    c1.Print((m_rootfile+".ps").c_str());
    for(Int_t ip=0;ip<NumSlices;ip++)
    {
        m_eangle[ip]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }
    for(Int_t ip=0;ip<NumSlices;ip++)
    {
        m_pos_eangle[ip]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }  
    for(Int_t ip=0;ip<NumSlices;ip++)
    {
        m_neg_eangle[ip]->Draw();
        c1.Print((m_rootfile+".ps").c_str());
    }  
    c1.Print((m_rootfile+".ps]").c_str());

    for(Int_t ip=0;ip<NumSlices;ip++)
    {
        delete m_eangle[ip];
        delete m_pos_eangle[ip];
        delete m_neg_eangle[ip];
    }
    delete m_EAngAverdE;
    delete m_pos_EAngAverdE;
    delete m_neg_EAngAverdE;

}
