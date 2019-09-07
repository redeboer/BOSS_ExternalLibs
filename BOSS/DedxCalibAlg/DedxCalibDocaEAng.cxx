#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <string>
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"

#include "DedxCalibAlg/DedxCalibDocaEAng.h"

using namespace std;
const double pMin=0.2;
const double pMax=2.1;
const double StepDoca = (DocaMax-DocaMin)/NumSlicesDoca;

DedxCalibDocaEAng::DedxCalibDocaEAng(const std::string& name, ISvcLocator* pSvcLocator): DedxCalib(name, pSvcLocator){
    declareProperty("Debug", m_debug=false);
    declareProperty("Idoca", m_idoca_test=1);
    declareProperty("Ieang", m_ieang_test=1);
}

void DedxCalibDocaEAng::BookHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibDocaEAng::BookHists()"<<endreq;

    ReadRecFileList();

    m_docaeangle = new TH1F**[NumSlicesDoca];
    stringstream hlabel;
    for(int i=0;i<NumSlicesDoca;i++)
    {
        m_docaeangle[i] = new TH1F*[NumSlicesEAng];
        for(int j=0;j<NumSlicesEAng;j++)
        {
            hlabel.str("");
            hlabel<<"dEdx_doca"<<i<<"_eang"<<j;
            m_docaeangle[i][j] = new TH1F(hlabel.str().c_str(), hlabel.str().c_str(), NumHistBins, MinHistValue, MaxHistValue);
        }
    }
    hlabel.str("");
    hlabel<<"dEdxVsDocaEAng";
    m_DocaEAngAverdE = new TH2F(hlabel.str().c_str(),"dEdx vs Doca & EAng",NumSlicesDoca,DocaMin,DocaMax,NumSlicesEAng,PhiMin,PhiMax);
}

void DedxCalibDocaEAng::FillHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibDocaEAng::FillHists()"<<endreq;

    TFile* f, *f_test(0);
    TTree* n103, *t_test(0);
    string runlist;

    double doca_test(0), eang_test(0);
    int ndedxhit=0;
    double dEdx[100]={0},pathlength[100]={0},wid[100]={0},layid[100]={0},dd_in[100]={0},eangle[100]={0},zhit[100]={0};
    float runNO=0,runFlag=0,costheta=0,tes=0,charge=0,recalg=0,ptrk=0,chidedx=0;
    cout<<"m_recFileVector.size()= "<<m_recFileVector.size()<<endl;
    if(m_debug){ f_test = new TFile("doca_eangle_test.root","recreate");
      t_test = new TTree("inftest", "information for test doca eangle calibration");
      t_test->Branch("idoca", &m_idoca_test, "idoca/I");
      t_test->Branch("ieang", &m_ieang_test, "ieang/I");
      t_test->Branch("doca", &doca_test, "doca/D");
      t_test->Branch("eang", &eang_test, "eang/D");
    }
	
    for(unsigned int i=0; i<m_recFileVector.size(); i++){
      runlist = m_recFileVector[i];
      cout<<"runlist: "<<runlist.c_str()<<endl;
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
      n103-> SetBranchAddress("ptrk",&ptrk);
      n103-> SetBranchAddress("chidedx_e",&chidedx);
      for(int j=0;j<n103->GetEntries();j++){
	n103->GetEntry(j);
	if(ptrk>pMax || ptrk<pMin) continue;
	if(tes>1400) continue;
	for(int k=0;k<ndedxhit;k++){
	  dd_in[k] = dd_in[k]/doca_norm[int(layid[k])];
	  if(eangle[k] >0.25) eangle[k] = eangle[k] -0.5;
	  else if(eangle[k] <-0.25) eangle[k] = eangle[k] +0.5;
	  if(eangle[k]>PhiMin && eangle[k]<PhiMax && dd_in[k]>DocaMin && dd_in[k]<DocaMax)
            {
	      //	      if(layid[k] <4) continue;
	      //	      else if(layid[k] <8)
	      //                {if(dEdx[k]<MinADCValuecut || dEdx[k]>MaxADCValuecut || pathlength[k]>RPhi_PathMaxCut || pathlength[k]<Iner_RPhi_PathMinCut || driftdist[k]>Iner_DriftDistCut) continue;}
	      //	      else
	      //                {if(dEdx[k]<MinADCValuecut || dEdx[k]>MaxADCValuecut || pathlength[k]>RPhi_PathMaxCut || pathlength[k]<Out_RPhi_PathMinCut || driftdist[k]>Out_DriftDistCut) continue;}
	      int iDoca =(Int_t)floor((dd_in[k]-DocaMin)/StepDoca);
	      int iEAng = 0;
	      for(int i =0; i<14; i++)
                {
		  if(eangle[k] < Eangle_value_cut[i] || eangle[k] >= Eangle_value_cut[i+1]) continue;
		  else if(eangle[k] >= Eangle_value_cut[i] && eangle[k] < Eangle_value_cut[i+1])
                    {
		      for(int m =0; m<i; m++)
                        {
			  iEAng += Eangle_cut_bin[m];
                        }
		      double eangle_bin_cut_step = (Eangle_value_cut[i+1] - Eangle_value_cut[i])/Eangle_cut_bin[i];     
		      int temp_bin = int((eangle[k]-Eangle_value_cut[i])/eangle_bin_cut_step);
		      iEAng += temp_bin;
                    }
                }
	      if(m_debug && (iDoca==m_idoca_test) && (iEAng==m_ieang_test)){
		doca_test = dd_in[k];
		eang_test = eangle[k];
		t_test->Fill();
		cout << "dedx before cor: " << dEdx[k]*1.5/pathlength[k] << endl;
	      }
	      dEdx[k] = exsvc->StandardHitCorrec(0,runFlag,2,runNO,pathlength[k],wid[k],layid[k],dEdx[k],(dd_in[k])*(doca_norm[int(layid[k])]),eangle[k],costheta);
	      dEdx[k] = exsvc->StandardTrackCorrec(0, runFlag, 2, runNO, dEdx[k], costheta, tes);
	      m_docaeangle[iDoca][iEAng]->Fill(dEdx[k]);
	      if(m_debug && (iDoca==m_idoca_test) && (iEAng==m_ieang_test)){
		cout << "dedx after cor: " << dEdx[k] << endl << endl;
	      }
	    } // end of if(eangle, doca) are normal
	} // end of loop in k, hit list 
      } // end of loop in j, entry list
    }// end of loop in i, filelist
    if(m_debug){ f_test->cd(); t_test->Write(); f_test->Close(); }
} // end of FillHists() 

void DedxCalibDocaEAng::AnalyseHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibDocaEAng::AnalyseHists()"<<endreq;

    TF1* func;
    Double_t entry=0,mean=0,rms=0;
    Double_t binmax=0;
    Double_t lp[3]={0}; 
    gStyle->SetOptFit(1111);
    for(Int_t id=0;id<NumSlicesDoca;id++)
    {
        for(Int_t ip=0;ip<NumSlicesEAng;ip++)
        {   
            entry = m_docaeangle[id][ip]->GetEntries();
            if(entry<10) continue;
            mean = m_docaeangle[id][ip]->GetMean();
            rms = m_docaeangle[id][ip]->GetRMS();
            binmax = m_docaeangle[id][ip]->GetMaximumBin();
            lp[1] = m_docaeangle[id][ip]->GetBinCenter(binmax);
            lp[2] = 200;
            lp[0] = (m_docaeangle[id][ip]->GetBinContent(binmax)+m_docaeangle[id][ip]->GetBinContent(binmax-1)+m_docaeangle[id][ip]->GetBinContent(binmax+1))/3;

            if(m_phShapeFlag==0)
            {   
                func = new TF1("mylan",mylan,10,3000,4);
                func->SetParameters(entry, mean, rms, -0.8);
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

            m_docaeangle[id][ip]->Fit(func, "r");
        }
    }
}

void DedxCalibDocaEAng::WriteHists()
{
    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalibDocaEAng::WriteHists()"<<endreq;

    const int totalNo = NumSlicesDoca*NumSlicesEAng;
    double Out_entryNo[totalNo]={0},Out_mean[totalNo]={0},Out_sigma[totalNo]={0},Out_fitmean[totalNo]={0},Out_fitmeanerr[totalNo]={0},Out_fitsigma[totalNo]={0},Out_gain[totalNo]={0};
    double Iner_entryNo[totalNo]={0},Iner_mean[totalNo]={0},Iner_sigma[totalNo]={0},Iner_fitmean[totalNo]={0},Iner_fitmeanerr[totalNo]={0},Iner_fitsigma[totalNo]={0},Iner_gain[totalNo]={0}; 
    double Id_doca[totalNo]={0},Ip_eangle[totalNo]={0},doca[totalNo]={0},eangle[totalNo]={0},chisq[totalNo]={0};

    double Norm(0), count(0);
    for(Int_t id=0;id<NumSlicesDoca;id++)
    {
        for(Int_t ip=0;ip<NumSlicesEAng;ip++)
        {
            Id_doca[id*NumSlicesEAng+ip] = id;
            Ip_eangle[id*NumSlicesEAng+ip] = ip;
            doca[id*NumSlicesEAng+ip] = (id+0.5)*StepDoca + DocaMin;
            int iE=0,i=0; 
            for(;i<14;i++)
            {
                if(ip>=iE && ip<iE+Eangle_cut_bin[i]) break;
                iE=iE+Eangle_cut_bin[i];
            }
            //cout<<"iE= "<<iE<<" Eangle_cut_bin["<<i<<"]= "<<Eangle_cut_bin[i]<<endl;
            eangle[id*NumSlicesEAng+ip] = Eangle_value_cut[i] + (ip-iE+0.5)*(Eangle_value_cut[i+1]-Eangle_value_cut[i])/Eangle_cut_bin[i];
            //cout<<"eangle["<<id<<"]["<<ip<<"]=  "<<eangle[id*NumSlicesEAng+ip]<<endl;

            Out_entryNo[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetEntries();
            Out_mean[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetMean();
            Out_sigma[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetRMS();
            //cout<<"Out_entryNo["<<id<<"]["<<ip<<"]= "<<Out_entryNo[id*NumSlicesEAng+ip]<<"  "<<Out_mean[id*NumSlicesEAng+ip]<<"  "<<Out_sigma[id*NumSlicesEAng+ip]<<endl; 
            if(Out_entryNo[id*NumSlicesEAng+ip]<10) continue;

            if(m_phShapeFlag==0)
            {
                Out_fitmean[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("mylan")->GetParameter(1);
                Out_fitmeanerr[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("mylan")->GetParError(1);
                Out_fitsigma[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("mylan")->GetParameter(2);
                //cout<<Out_fitmean[id*NumSlicesEAng+ip]<<"  "<<Out_fitmeanerr[id*NumSlicesEAng+ip]<<"  "<<Out_fitsigma[id*NumSlicesEAng+ip]<<endl;  
                chisq[id*NumSlicesEAng+ip] = (m_docaeangle[id][ip]->GetFunction("mylan")-> GetChisquare())/(m_docaeangle[id][ip]->GetFunction("mylan")->  GetNDF());
            }
            if(m_phShapeFlag==1)
            {
                Out_fitmean[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("landaun")->GetParameter(1);
                Out_fitmeanerr[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("landaun")->GetParError(1);
                Out_fitsigma[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("landaun")->GetParameter(2);
                chisq[id*NumSlicesEAng+ip] = (m_docaeangle[id][ip]->GetFunction("landaun")-> GetChisquare())/(m_docaeangle[id][ip]->GetFunction("landaun")->  GetNDF());
            }
            if(m_phShapeFlag==2)
            {
                Out_fitmean[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("Landau")->GetParameter(1);
                Out_fitmeanerr[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("Landau")->GetParError(1);
                Out_fitsigma[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("Landau")->GetParameter(2);
                chisq[id*NumSlicesEAng+ip] = (m_docaeangle[id][ip]->GetFunction("Landau")-> GetChisquare())/(m_docaeangle[id][ip]->GetFunction("Landau")->  GetNDF());
            }
            if(m_phShapeFlag==3)
            {
                Out_fitmean[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("Vavilov")->GetParameter(1);
                Out_fitmeanerr[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("Vavilov")->GetParError(1);
                Out_fitsigma[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("Vavilov")->GetParameter(2);
                chisq[id*NumSlicesEAng+ip] = (m_docaeangle[id][ip]->GetFunction("Vavilov")-> GetChisquare())/(m_docaeangle[id][ip]->GetFunction("Vavilov")->  GetNDF());
            }
            if(m_phShapeFlag==4)
            {
                Out_fitmean[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("AsymGauss")->GetParameter(1);
                Out_fitmeanerr[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("AsymGauss")->GetParError(1);
                Out_fitsigma[id*NumSlicesEAng+ip] = m_docaeangle[id][ip]->GetFunction("AsymGauss")->GetParameter(2);
                chisq[id*NumSlicesEAng+ip] = (m_docaeangle[id][ip]->GetFunction("AsymGauss")-> GetChisquare())/(m_docaeangle[id][ip]->GetFunction("AsymGauss")->  GetNDF());
            }

            m_DocaEAngAverdE -> SetBinContent(id+1, ip+1, Out_fitmean[id*NumSlicesEAng+ip]);
            if(Out_fitmean[id*NumSlicesEAng+ip]>0 && Out_fitmeanerr[id*NumSlicesEAng+ip]>0 && Out_fitmeanerr[id*NumSlicesEAng+ip]<10 && Out_fitsigma[id*NumSlicesEAng+ip]>0 && Out_fitsigma[id*NumSlicesEAng+ip]<200){
	      Norm += (Out_fitmean[id*NumSlicesEAng+ip])*(Out_entryNo[id*NumSlicesEAng+ip]);
	      count += Out_entryNo[id*NumSlicesEAng+ip];
            }
        } // end of loop in ip
    } // end of loop in id
    Norm = Norm/count;
    cout<<"count=  "<<count<<"  average value of doca eangle bins: "<<Norm<<endl;
    for(Int_t i=0;i<totalNo;i++)
    {
        if(Out_entryNo[i]>10 && Out_fitmean[i]>0 && Out_fitsigma[i]>0)  Out_gain[i]=Out_fitmean[i]/Norm;
        else Out_gain[i] = 0;
    }

    log<<MSG::INFO<<"begin generating root file!!! "<<endreq;
    TFile* f = new TFile(m_rootfile.c_str(), "recreate");
    for(Int_t id=0;id<NumSlicesDoca;id++)
    {
        for(Int_t ip=0;ip<NumSlicesEAng;ip++)
        {
            m_docaeangle[id][ip]->Write();
        }
    }
    m_DocaEAngAverdE->Write();

    TTree* ddgcalib = new TTree("ddgcalib", "ddgcalib");
    ddgcalib -> Branch("Out_hits", Out_entryNo, "Out_entryNo[1600]/D");
    ddgcalib -> Branch("Out_mean", Out_mean, "Out_mean[1600]/D");
    ddgcalib -> Branch("Out_sigma", Out_sigma, "Out_sigma[1600]/D");
    ddgcalib -> Branch("Out_fitmean", Out_fitmean, "Out_fitmean[1600]/D");
    ddgcalib -> Branch("Out_fitmeanerr", Out_fitmeanerr, "Out_fitmeanerr[1600]/D");
    ddgcalib -> Branch("Out_chi", Out_fitsigma, "Out_fitsigma[1600]/D");
    ddgcalib -> Branch("Out_gain", Out_gain, "Out_gain[1600]/D");
    ddgcalib -> Branch("Iner_hits", Iner_entryNo, "Iner_entryNo[1600]/D");
    ddgcalib -> Branch("Iner_mean", Iner_mean, "Iner_mean[1600]/D");    
    ddgcalib -> Branch("Iner_sigma", Iner_sigma, "Iner_sigma[1600]/D");
    ddgcalib -> Branch("Iner_fitmean", Iner_fitmean, "Iner_fitmean[1600]/D");
    ddgcalib -> Branch("Iner_fitmeanerr", Iner_fitmeanerr, "Iner_fitmeanerr[1600]/D");
    ddgcalib -> Branch("Iner_chi", Iner_fitsigma, "Iner_fitsigma[1600]/D");
    ddgcalib -> Branch("Iner_gain", Iner_gain, "Iner_gain[1600]/D");
    ddgcalib -> Branch("Id_doca", Id_doca, "Id_doca[1600]/D");
    ddgcalib -> Branch("Ip_eangle", Ip_eangle, "Ip_eangle[1600]/D");
    ddgcalib -> Branch("chisq", chisq, "chisq[1600]/D");
    ddgcalib -> Branch("doca", doca, "doca[1600]/D");
    ddgcalib -> Branch("eangle", eangle, "eangle[1600]/D");
    ddgcalib -> Fill();
    ddgcalib -> Write();
    f->Close();

    TCanvas c1("c1", "canvas", 500, 400);
    c1.Print((m_rootfile+".ps[").c_str());
    for(Int_t id=0;id<NumSlicesDoca;id++)
    {
        for(Int_t ip=0;ip<NumSlicesEAng;ip++)
        {
            m_docaeangle[id][ip]->Draw();
            c1.Print((m_rootfile+".ps").c_str());
        }
    }
    c1.Print((m_rootfile+".ps]").c_str());

    for(Int_t id=0;id<NumSlicesDoca;id++)
    {   
        for(Int_t ip=0;ip<NumSlicesEAng;ip++)
        {   
            delete m_docaeangle[id][ip];
        }
    }
    delete m_DocaEAngAverdE;
}
