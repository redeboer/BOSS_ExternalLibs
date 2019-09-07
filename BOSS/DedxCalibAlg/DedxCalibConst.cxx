#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include <TROOT.h>

using namespace std;

void DedxCalibConst()
{
    Int_t runno;
    Double_t runmean, rungain, runresol;

    TFile fdedx("DedxConst_before.root","recreate"); // output
    
    TTree* runbyrun = new TTree("runcalib", "runcalib");
    runbyrun -> Branch("runno", &runno, "runno/I");
    runbyrun -> Branch("runmean", &runmean, "runmean/D");
    runbyrun -> Branch("rungain", &rungain, "rungain/D");
    runbyrun -> Branch("runresol", &runresol, "runresol/D");
    //TFile fconst0("/ihepbatch/besd26/anff/662/Calib/draw/calib/rungain/rungain.root"); //input 
    TFile fconst0("rungain.root");
    TTree *tree_rung = (TTree*)fconst0.Get("runcalib");
    tree_rung -> SetBranchAddress("rungain", &rungain);
    tree_rung -> SetBranchAddress("runmean", &runmean);
    tree_rung -> SetBranchAddress("runno", &runno);
    tree_rung -> SetBranchAddress("runresol", &runresol);

    long NRUN = tree_rung -> GetEntries();
    cout<<"Run Number == "<<NRUN<<endl;
    for(long runid =0; runid<NRUN; runid++) 
    {
        tree_rung->GetEntry(runid);
        if(runno<12739) runbyrun ->Fill();
    }

    TTree *tree_resg = (TTree*)fconst0.Get("resolcalib");
    Double_t resolpar[1];
    tree_resg -> SetBranchAddress("resol", resolpar);
    tree_resg -> GetEntry(0);

    TTree *tree_gaing = (TTree*)fconst0.Get("gaincalib");
    Double_t gainpar[1];
    tree_gaing -> SetBranchAddress("gain", gainpar);
    tree_gaing -> GetEntry(0);

    fconst0.Close();
    cout<<"runbyrun const OK!!!"<<endl;

    // wire gain const  
   TFile fconst1("/ihepbatch/besd26/anff/662/Calib/const_file/WireGain/wiregain_before_new.root"); //for adding runs in 11414-14604 only
    //TFile fconst1("/ihepbatch/besd26/anff/655/Calib/root_file/WireGain/total_655/WireRoot_20448-23454.root"); // produced in total calib and will be used in psi4040
    //TFile fconst1("/ihepbatch/besd26/anff/655/Calib/root_file/WireGain/WireRoot_20683-20981_20000_byday_more.root");  //use in 20448-23454 usual calibration
   // TFile fconst1("/panfs/panfs.ihep.ac.cn/home/data/anff/dEdxCal/662_cal/WireGain/constfile/wireg_24897-25123.root"); //produced after high voltage changed on 2011.Dec and will be used after 24897 usual calibration
    Double_t gwire[6796];
    TTree *tree_wireg = (TTree*)fconst1.Get("wiregcalib");
    tree_wireg -> SetBranchAddress("wireg", gwire);
    tree_wireg -> GetEntry(0);
    fconst1.Close();
    cout<<"wiregcalib const OK!!!"<<endl;

    // satuaration const
    TFile fconst2("../Eangle1D/DedxConst_test.root");
    Double_t ggs0[43], ggs1[43], ggs2[43], ggs3[43];
    TTree *tree_ggs = (TTree*)fconst2.Get("ggscalib");
    tree_ggs -> SetBranchAddress("ggs0", ggs0);
    tree_ggs -> SetBranchAddress("ggs1", ggs1);
    tree_ggs -> SetBranchAddress("ggs2", ggs2);
    tree_ggs -> SetBranchAddress("ggs3", ggs3);
    tree_ggs->GetEntry(0);
    fconst2.Close();
    cout<<"ggscalib const OK!!!"<<endl;

    // driftdist const
    TFile fconst3("../Eangle1D/DedxConst_test.root");
    Double_t ddg0[43], ddg1[43], ddg2[43], ddg3[43];
    TTree *tree_ddg = (TTree*)fconst3.Get("ddgcalib");
    tree_ddg -> SetBranchAddress("ddg0", ddg0);
    tree_ddg -> SetBranchAddress("ddg1", ddg1);
    tree_ddg -> SetBranchAddress("ddg2", ddg2);
    tree_ddg -> SetBranchAddress("ddg3", ddg3);
    tree_ddg -> GetEntry(0);
    fconst3.Close();
    cout<<"ddgcalib const OK!!!"<<endl;

    // entrance angle const
    TFile fconst4("../Eangle1D/DedxConst_test.root");
    Double_t entra0[43], entra1[43], entra2[43], entra3[43];
    TTree *tree_entra = (TTree*)fconst4.Get("entracalib");
    tree_entra -> SetBranchAddress("entra0", entra0);
    tree_entra -> SetBranchAddress("entra1", entra1);
    tree_entra -> SetBranchAddress("entra2", entra2);
    tree_entra -> SetBranchAddress("entra3", entra3);

    //int entry[1]; double denangle[100];
    //tree_entra -> SetBranchAddress("1denangle_entry", entry);
    //tree_entra -> SetBranchAddress("1denangle", denangle);
    tree_entra->GetEntry(0);
    fconst4.Close();

    TFile fconst4_1("/ihepbatch/besd26/anff/662/Calib/draw/calib/eangle/EAngle.root");
    //TFile fconst4_1("../Eangle1D/DedxConst_test.root");
    int entry[1]; double denangle[100];
    TTree *tree_entra1 = (TTree*)fconst4_1.Get("entracalib");
    tree_entra1 -> SetBranchAddress("1denangle_entry", entry);
    tree_entra1 -> SetBranchAddress("1denangle", denangle);
    tree_entra1 ->GetEntry(0);
    fconst4_1.Close();
    cout<<"entrance angle const OK!!!"<<endl;

    // zhit const
    TFile fconst5("../Eangle1D/DedxConst_test.root");
    Double_t zdep0[43], zdep1[43], zdep2[43], zdep3[43];
    TTree *tree_zdep = (TTree*)fconst5.Get("zdepcalib");
    tree_zdep -> SetBranchAddress("zdep0", zdep0);
    tree_zdep -> SetBranchAddress("zdep1", zdep1);
    tree_zdep -> SetBranchAddress("zdep2", zdep2);
    tree_zdep -> SetBranchAddress("zdep3", zdep3);
    tree_zdep->GetEntry(0);
    fconst5.Close();
    cout<<"zdepcalib const OK!!!"<<endl;

    // layer gain const
    TFile fconst6("../Eangle1D/DedxConst_test.root");
    Double_t glayer[43];
    TTree *tree_layerg = (TTree*)fconst6.Get("layergcalib");
    tree_layerg -> SetBranchAddress("layerg", glayer);
    tree_layerg->GetEntry(0);
    fconst6.Close();    
    cout<<"layergcalib const OK!!!"<<endl;

    // costhetacalib const
    //    TFile fconst7("/ihepbatch/besd26/anff/655/Calib/root_file/Costheta/total_655/CosthetaRoot_20448-23454_new.root");
    //  TTree *tree_costheta = (TTree*)fconst7.Get("costhetacalib");
    TFile fconst7("/ihepbatch/besd26/anff/662/Calib/const_file/Costheta/costheta_new.root");
    TTree *tree_costheta = (TTree*)fconst7.Get("costhetacalib");
    double gcostheta[80];
    tree_costheta ->SetBranchAddress("costheta",gcostheta);
    tree_costheta->GetEntry(0);
    fconst7.Close();
    cout<<"costhetacalib const OK!!!"<<endl;

    // t0calib const
    TFile fconst8("/afs/ihep.ac.cn/bes3/offline/CalibConst/dedx/6.5.3/Jpsi/t0calib.root");
    //    TFile fconst8("../DocaEangle/DedxConst_test.root");
    TTree *tree_t0=(TTree*)fconst8.Get("t0calib");
    double t0, dedx;
    double gt0[35];
    double gdedx[35];
    tree_t0->SetBranchAddress("t0",&t0);
    tree_t0->SetBranchAddress("dedx",&dedx);
    for(int i=0; i<tree_t0->GetEntries(); i++)
    {
        tree_t0->GetEntry(i);
        gt0[i] = t0;
        gdedx[i] = dedx;
    }
    fconst8.Close();
    cout<<"t0calib const OK!!!"<<endl;

    // docaeangle const
    //TFile fconst9("/afs/ihep.ac.cn/bes3/offline/CalibConst/dedx/6.5.4/DedxConst_before_docaeangle.root");    
    TFile fconst9("/ihepbatch/besd26/anff/662/Calib/const_file/DocaEAngle/docaeangle_new.root");
    const int n = 1600;
    double Iner_gain[n], Iner_chi[n], Iner_hits[n];
    double Out_gain[n], Out_chi[n], Out_hits[n];
    double Id_doca[n], Ip_eangle[n];

    TTree *tree_docasin = (TTree*)fconst9.Get("ddgcalib");
    tree_docasin -> SetBranchAddress("Iner_gain", Iner_gain);
    tree_docasin -> SetBranchAddress("Iner_chi",  Iner_chi);
    tree_docasin -> SetBranchAddress("Iner_hits", Iner_hits);
    tree_docasin -> SetBranchAddress("Out_gain",  Out_gain);
    tree_docasin -> SetBranchAddress("Out_chi",   Out_chi);
    tree_docasin -> SetBranchAddress("Out_hits",  Out_hits);
    tree_docasin -> SetBranchAddress("Id_doca",   Id_doca);
    tree_docasin -> SetBranchAddress("Ip_eangle", Ip_eangle);
    tree_docasin -> GetEntry(0);
    fconst9.Close();

    cout<<"ddgcalib const OK!!!"<<endl;

    TFile fconst10("/panfs/panfs.ihep.ac.cn/home/data/anff/dEdxCal/662_cal/Hadron/constfile/Hadron_11414-14604.root");
    // TFile fconst10("/ihepbatch/besd26/anff/655/Calib/root_file/Hadron/total_655/Hadron_20448-23454.root");
    // TFile fconst10("/panfs/panfs.ihep.ac.cn/home/data/anff/dEdxCal/662_cal/Hadron/constfile/Hadron_20333-23454.root");
    int hadronNo;
    double ghadron[5];
    TTree *tree_hadron = (TTree*)fconst10.Get("ggscalib");
    tree_hadron -> SetBranchAddress("hadron", ghadron);
    tree_hadron -> SetBranchAddress("hadronNo",&hadronNo);
    tree_hadron->GetEntry(0);
    fconst10.Close();
    cout<<"hadron const OK!!!"<<endl;

    //output of const file
    fdedx.cd();
    TTree* gain = new TTree("gaincalib", "gaincalib");
    gain -> Branch("gain", gainpar, "gain[1]/D");
    gain -> Branch("t0",   gt0,      "t0[35]/D");
    gain -> Branch("dedx", gdedx,    "dedx[35]/D");

    TTree* resol = new TTree("resolcalib", "resolcalib");
    resol -> Branch("resol", resolpar, "resol[1]/D");

    TTree* wireg = new TTree("wiregcalib", "wiregcalib");
    wireg -> Branch("wireg", gwire, "wireg[6796]/D");
    wireg-> SetBasketSize("wireg",2000000); //not need in 661,but have to be done in 655

    TTree* ggs = new TTree("ggscalib", "ggscalib");
    ggs -> Branch("ggs0", ggs0, "ggs[43]/D");
    ggs -> Branch("ggs1", ggs1, "ggs[43]/D");
    ggs -> Branch("ggs2", ggs2, "ggs[43]/D");
    ggs -> Branch("ggs3", ggs3, "ggs[43]/D");
    ggs -> Branch("costheta", gcostheta, "costheta[80]/D");
    ggs -> Branch("hadron",ghadron,"hadron[5]/D");
    ggs -> Branch("hadronNo",&hadronNo,"hadronNo/I");

    TTree* ddg = new TTree("ddgcalib", "ddgcalib");
    ddg -> Branch("ddg0", ddg0, "ddg[43]/D");
    ddg -> Branch("ddg1", ddg1, "ddg[43]/D");
    ddg -> Branch("ddg2", ddg2, "ddg[43]/D");
    ddg -> Branch("ddg3", ddg3, "ddg[43]/D");
    ddg -> Branch("Iner_gain",  Iner_gain,  "Iner_gain[1600]/D");
    ddg -> Branch("Out_gain",   Out_gain,   "Out_gain[1600]/D");
    ddg -> Branch("Iner_chi",   Iner_chi,   "Iner_chi[1600]/D");
    ddg -> Branch("Out_chi",    Out_chi,    "Out_chi[1600]/D");
    ddg -> Branch("Iner_hits",  Iner_hits,  "Iner_hits[1600]/D");
    ddg -> Branch("Out_hits",   Out_hits,   "Out_hits[1600]/D");
    ddg -> Branch("Id_doca",    Id_doca,    "Id_doca[1600]/D");
    ddg -> Branch("Ip_eangle",  Ip_eangle,  "Ip_eangle[1600]/D");

    TTree* entra = new TTree("entracalib", "entracalib");
    entra -> Branch("entra0", entra0, "entra[43]/D");
    entra -> Branch("entra1", entra1, "entra[43]/D");
    entra -> Branch("entra2", entra2, "entra[43]/D");
    entra -> Branch("entra3", entra3, "entra[43]/D");
    entra -> Branch("1denangle_entry", entry, "1denangle_entry[1]/I");
    entra -> Branch("1denangle", denangle, "1denangle[100]/D");

    TTree* zdep = new TTree("zdepcalib", "zdepcalib");
    zdep -> Branch("zdep0", zdep0, "zdep[43]/D");
    zdep -> Branch("zdep1", zdep1, "zdep[43]/D");
    zdep -> Branch("zdep2", zdep2, "zdep[43]/D");
    zdep -> Branch("zdep3", zdep3, "zdep[43]/D");

    TTree* layerg = new TTree("layergcalib", "layergcalib");
    layerg -> Branch("layerg", glayer, "layerg[43]/D");

    //Fill constroot file!!!
    gain->Fill();
    resol->Fill();
    wireg -> Fill();
    ggs -> Fill();
    ddg -> Fill();
    entra -> Fill();
    zdep -> Fill();
    layerg -> Fill();
    ///////////////////////////////////////////////////
    //wireg->Write();
    //ggs->Write();
    //ddg->Write();
    //entra->Write();
    //zdep->Write();
    //layerg->Write();
    //gain->Write();
    //resol->Write();
    //runbyrun ->Write();
    fdedx.Write();
    fdedx.Close();
    cout<<"All const OK!!!"<<endl;
}
