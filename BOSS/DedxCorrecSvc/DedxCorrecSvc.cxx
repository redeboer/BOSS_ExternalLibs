#include "DedxCorrecSvc/DedxCorrecSvc.h"
#include "DedxCorrecSvc/DedxCorrParameters.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"

#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CalibData/Dedx/DedxCalibData.h"
#include "CalibData/CalibModel.h"
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TLeafD.h"

using namespace std;
using CLHEP::Hep3Vector;

//static SvcFactory<DedxCorrecSvc> s_factory;
//const ISvcFactory& DedxCorrecSvcFactory = s_factory;
//double Iner_Stepdoca = (Iner_DocaMax-Iner_DocaMin)/NumSlicesDoca;
//double Out_Stepdoca = (Out_DocaMax-Out_DocaMin)/NumSlicesDoca;

DedxCorrecSvc::DedxCorrecSvc( const string& name, ISvcLocator* svcloc) :
    geosvc(0),Service (name, svcloc) {
        declareProperty("Run",m_run=1);
        declareProperty("init",m_init=1);
        declareProperty("par_flag",m_par_flag=0);
        declareProperty("Debug",m_debug=false);
        declareProperty("DebugI",m_debug_i=1);
        declareProperty("DebugJ",m_debug_j=1);
        m_initConstFlg = false;
        // cout<<"DedxCorrecSvc::DedxCorrecSvc"<<endl;
    }

DedxCorrecSvc::~DedxCorrecSvc(){
}

StatusCode DedxCorrecSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
    //cout<<"DedxCorrecSvc::queryInterface"<<endl;
    if( IID_IDedxCorrecSvc.versionMatch(riid) ){
        *ppvInterface = static_cast<IDedxCorrecSvc*> (this);
    } else{
        return Service::queryInterface(riid, ppvInterface);
    }
    return StatusCode::SUCCESS;
}

StatusCode DedxCorrecSvc::initialize(){
    // cout<<"DedxCorrecSvc::initialize"<<endl;
    MsgStream log(messageService(), name());
    log << MSG::INFO << name() << "DedxCorrecSvc::initialize()" << endreq;

    StatusCode sc = Service::initialize();
    if( sc.isFailure() ) return sc;

    IIncidentSvc* incsvc;
    sc = service("IncidentSvc", incsvc);
    int priority = 100;
    if( sc.isSuccess() ){
        //incsvc -> addListener(this, "BeginEvent", priority);
        incsvc -> addListener(this, "NewRun", priority);
    }

    StatusCode scgeo = service("MdcGeomSvc", geosvc);
    if(scgeo.isFailure() ) {
        log << MSG::ERROR << "GeoSvc failing!!!!!!!" << endreq;
        return scgeo;
    }

    StatusCode scmgn = service ("MagneticFieldSvc",m_pIMF);
    if(scmgn!=StatusCode::SUCCESS) { 
        log << MSG::ERROR << "Unable to open Magnetic field service"<<endreq;
    }

    return StatusCode::SUCCESS;
}

StatusCode DedxCorrecSvc::finalize(){
    // cout<<"DedxCorrecSvc::finalize()"<<endl;
    MsgStream log(messageService(), name());
    log << MSG::INFO << name() << "DedxCorrecSvc::finalize()" << endreq;
    return StatusCode::SUCCESS;
}

void DedxCorrecSvc::handle(const Incident& inc){
    // cout<<"DedxCorrecSvc::handle"<<endl;
    MsgStream log( messageService(), name() );
    log << MSG::DEBUG << "handle: " << inc.type() << endreq;

    if ( inc.type() == "NewRun" ){
        log << MSG::DEBUG << "New Run" << endreq;

        if( ! m_initConstFlg  ) 
        {
            if( m_init == 0 ) { init_param(); }
            else if( m_init ==1 ) { init_param_svc(); }
            /* if( ! init_param() ){
               log << MSG::ERROR 
               << "can not initilize Mdc Calib Constants" << endreq;
               }*/
        }
    } 
}

double DedxCorrecSvc::f_larcos(double x, int nbin) {
  if(nbin!=80) return x;  // temporally for only nbin = 80
  double m_absx(fabs(x)); 
  double m_charge(x/m_absx);
  if(m_absx>0.925 && m_absx<=0.950) return 0.9283*m_charge; // these numbers are from the mean values 
  if(m_absx>0.900 && m_absx<=0.925) return 0.9115*m_charge; // of each bin in cos(theta) distribution
  if(m_absx>0.875 && m_absx<=0.900) return 0.8877*m_charge;
  if(m_absx>0.850 && m_absx<=0.875) return 0.8634*m_charge;
  if(m_absx>0.825 && m_absx<=0.850) return 0.8460*m_charge;
  if(m_absx>0.800 && m_absx<=0.825) return 0.8089*m_charge;
}

void
DedxCorrecSvc::init_param_svc() {
    // cout<<"DedxCorrecSvc::init_param_svc()"<<endl;
    MsgStream log(messageService(), name());
    IDataProviderSvc* pCalibDataSvc;
    StatusCode sc = service("CalibDataSvc", pCalibDataSvc, true);
    if ( !sc.isSuccess() ) {
        log << MSG::ERROR 
            << "Could not get IDataProviderSvc interface of CalibXmlCnvSvc" 
            << endreq;
        return ;
    } else {
        log << MSG::DEBUG 
            << "Retrieved IDataProviderSvc interface of CalibXmlCnvSvc" 
            << endreq;
    }

    Iner_Stepdoca = (Iner_DocaMax-Iner_DocaMin)/NumSlicesDoca;
    Out_Stepdoca = (Out_DocaMax-Out_DocaMin)/NumSlicesDoca; 
    //cout<<"Out_DocaMax:   "<<Out_DocaMax<<"         Out_DocaMin:  "<<Out_DocaMin<<"        Out_Stepdoca : "<<Out_Stepdoca<<"        NumSlicesDoca : "<<NumSlicesDoca<<endl;
    std::string fullPath = "/Calib/DedxCal";

    SmartDataPtr<CalibData::DedxCalibData> test(pCalibDataSvc, fullPath);

    //rung par----------------------------------------------------------
    N_run = test->getrunNO();
    int run_temp = 0;
    cout<<"N_run = "<<N_run<<endl; 
    for(int j=0;j<10000;j++)
    {
        if(j<N_run)
        {
            for(int i=0;i<4;i++)
            {
                m_rung[i][j] = test->getrung(i,j);
                if(i==2 && m_rung[2][j]>run_temp) run_temp = m_rung[2][j];
            }
        }
        else
        {
            run_temp++;
            m_rung[0][j] = 1;
            m_rung[1][j] = 550;
            m_rung[2][j] = run_temp;
            m_rung[3][j] = 26.8;
        }
    }

    //for(int i=0;i<4;i++) 
    //{
    //    for(int j=0;j<10000;j++)
    //    {
    //        std::cout<<"rung["<<i<<"]["<<j<<"]=  "<<m_rung[i][j]<<std::endl;
    //    }
    //}

    //TFile* fruncalib=new TFile("/home/bes/xcao/besd26/Calib/651_Calib/Psai_cal/rung_calib/first_calib/DedxConst.root");
    /*TFile* fruncalib=new TFile("/home/bes/xcao/besd26/Calib/651_Calib/Psai_cal/rung_calib/fourth_calib/temp/DedxConst.root");

      double rungain, runmean, runresol;
      int runno;
      TTree *rungtree= (TTree*) fruncalib-> Get("runcalib");
      rungtree -> SetBranchAddress("rungain", &rungain);
      rungtree -> SetBranchAddress("runmean", &runmean);
      rungtree -> SetBranchAddress("runno", &runno);
      rungtree -> SetBranchAddress("runresol", &runresol);
      rungtree -> GetEntry(0);
      int NRUN = rungtree -> GetEntries();
      N_run = rungtree -> GetEntries();
    //cout<<"N_run = "<<N_run<<endl;
    //cout<<"NRUN = "<<NRUN<<endl;
    for(int runid =0; runid<NRUN; runid++) {
    rungtree->GetEntry(runid);
    m_rung[0][runid] = rungain;
    m_rung[1][runid] = runmean;
    m_rung[2][runid] = runno;
    m_rung[3][runid] = runresol;
    //cout<<"runid :"<<runid<<"     run_no =  "<<m_rung[2][runid]<<"      run_mean = "<<m_rung[1][runid]<<"      run_gain = "<<m_rung[0][runid]<<"      runresol = "<<m_rung[3][runid]<<endl;
    }

    //cout<<"run by run const ------------------------------------------------- "<<endl;
    for(int i=0;i<4;i++) {
    for(int j=0;j<NRUN;j++) {
    //std::cout<<"rung["<<i<<"]["<<j<<"]="<<m_rung[i][j]<<endl;
    //std::cout<<"\n";
    }
    }*/

    for(int i=0;i<4;i++) {
        for(int j=0;j<43;j++) {
            m_ddg[i][j] = test->getddg(i,j);
            m_ggs[i][j] = test->getggs(i,j);
            m_zdep[i][j] = test->getzdep(i,j);
            //      m_enta[i][j] = test->getenta(i,j);
            //std::cout<<"ddg["<<i<<"]["<<j<<"]="<<m_ddg[i][j];
            //std::cout<<"         ggs["<<i<<"]["<<j<<"]="<<m_ggs[i][j];
            //std::cout<<"        zdep["<<i<<"]["<<j<<"]="<<m_zdep[i][j];
            //std::cout<<"\n";
        }
    }

    m_dedx_gain = test->getgain();
    std::cout<<"gain="<<m_dedx_gain<<"\n";

    m_dedx_resol = test->getresol();
    std::cout<<"resol="<<m_dedx_resol<<"\n";

    for(int i=0;i<43;i++) { 
        m_layer_g[i] = test -> getlayerg(i);
        if(m_layer_g[i]>2.0 || m_layer_g[i]<0.5) m_layer_g[i] =1;
        //std::cout<<"layerg["<<i<<"]="<<m_layer_g[i]<<endl;
    }

    /*const int n = 6796;
      double wireg[n];
      TFile* fwiregcalib=new TFile("/home/bes/xcao/besd26/Calib/651_Calib/Psai_cal/wireg_calib/wiregcalib.root");
      TTree *wiregtree= (TTree*)fwiregcalib -> Get("wiregcalib");
      wiregtree -> SetBranchAddress("wireg", &wireg);
      wiregtree -> GetEntry(0);
    //cout<<"wire gain ------------------------------------------------- "<<endl;
    for(int i=0; i<n; i++){
    m_wire_g[i] = wireg[i];
    //std::cout<<"wireg["<<i<<"] = "<<m_wire_g[i]<<"\n";
    }*/

    for(int i=0;i<6796;i++) { 
        m_wire_g[i] = test -> getwireg(i);
        //std::cout<<"wireg["<<i<<"] = "<<m_wire_g[i]<<"\n";
    } 

    //int kk=0;
    //if(N_run<11397) kk=0;
    //else if(N_run<12739) kk=1;
    //else if(N_run<14395) kk=2;
    //else kk=3;
    int kk=3;
    for(int i=0;i<6796;i++) {
        m_valid[i] = 1;
        /*if(i<=483) {m_valid[i] =0;}*/
        for(int j=0; j<25; j++){
            if( i == dead_wire[kk][j] ) 
            {m_valid[i] = 0;
                //cout<<"N_run: "<<N_run<<" kk: "<<kk<<endl;
                //cout<<"m_valid["<<i<<"]:  "<<m_valid[i]<<endl;
            }
        }
        //std::cout<<"valid["<<i<<"]="<<m_valid[i]<<"\n";
    } 

    //tempoary way to get costheta constants
    cos_k.clear();
    cos_b.clear();
    //double cos1[80];
    if(true){
        const int nbin=80;
        vector<double> cos;
        /*TFile* fcosthecalib=new TFile("/home/bes/xcao/besd26/Calib/651_Calib/Psai_cal/costhe_calib/costhetacalib2.root");
          TTree *costhetree= (TTree*)fcosthecalib -> Get("costhetacalib");
          TBranch* b_costheta;
          double costheta;
          costhetree->SetBranchAddress("costheta",&costheta,&b_costheta);
        //const int nbin=tc->GetEntries();
        //cout<<"costheta gain ------------------------------------------------- "<<endl;
        for(int i=0; i<nbin; i++){
        costhetree->GetEntry(i);
        cos.push_back(costheta);
        //cout<<"i : "<<i<<"      costheta gain : "<<costheta<<endl; 
        }*/

        for(int i=0; i<nbin; i++){
            cos.push_back(test -> get_costheta(i));
        }
        for(int i=0;i<nbin-1;i++){
            double k=0;
            double b=0;
            if(cos[i]>0.0001){ // not empty bin corresponding to large angle
                if(cos[i+1]>0.0001){
                    double x1=-1.00+(0.5+i)*(2.00/nbin);
                    double y1=cos[i];
                    double x2=-1.00+(0.5+i+1)*(2.00/nbin);
                    double y2=cos[i+1];
		    // correct around absolute large cos(theta)  angle
		    if(fabs(x1)>0.8) x1 = f_larcos(x1, nbin);
		    if(fabs(x2)>0.8) x2 = f_larcos(x2, nbin);
                    k=(y2-y1)/(x2-x1); // k is the slope
		    b=y1-k*x1; 
		    //cout << "x1 " << x1 << "  x2 " << x2 << "  y1 " <<  y1 << "  y2 " << y2 << endl;
                }
                else if(i>0){
                    if(cos[i-1]>0.0001){
                        double x1=-1.00+(0.5+i-1)*(2.00/nbin);
                        double y1=cos[i-1];
                        double x2=-1.00+(0.5+i)*(2.00/nbin);
                        double y2=cos[i];
			// correct around absolute large cos(theta)  angle
			if(fabs(x1)>0.8) x1 = f_larcos(x1, nbin);
			if(fabs(x2)>0.8) x2 = f_larcos(x2, nbin);
                        k=(y2-y1)/(x2-x1);
                        b=y1-k*x1;
			//cout << "x1 " << x1 << "  x2 " << x2 << "  y1 " <<  y1 << "  y2 " << y2 << endl;
                    }
                }
            }
            else {
                if(i>0){
                    if(cos[i-1]>0.0001 && cos[i+1]>0.0001){
                        double x1=-1.00+(0.5+i-1)*(2.00/nbin);
                        double y1=cos[i-1];
                        double x2=-1.00+(0.5+i+1)*(2.00/nbin);
                        double y2=cos[i+1];
			// correct around absolute large cos(theta)  angle
			if(fabs(x1)>0.8) x1 = f_larcos(x1, nbin);
			if(fabs(x2)>0.8) x2 = f_larcos(x2, nbin);
                        k=(y2-y1)/(x2-x1);
                        b=y1-k*x1;
			//cout << "x1 " << x1 << "  x2 " << x2 << "  y1 " <<  y1 << "  y2 " << y2 << endl;
                    }
                }
            }
	    //cout<<"i : "<<i<<"      costheta gain : "<<cos[i] << "  k=" << k << "  b=" << b <<endl; 
            cos_k.push_back(k);
            cos_b.push_back(b);
        }
    }

    t0_k.clear();
    t0_b.clear();
    if(true){
        //const int nbin=35;
        /*vector<double> xbin;
          vector<double> ybin;
          TFile* ft0calib=new TFile("/ihepbatch/besd13/chunlei/calib/t0calib.root");
          TTree *tree_t0=(TTree*)ft0calib->Get("t0calib");
          TBranch* b_t0;
          TBranch* b_dedx;
          double t0, dedx;
          tree_t0->SetBranchAddress("t0",&t0,&b_t0);
          tree_t0->SetBranchAddress("dedx",&dedx,&b_dedx);
          const int nbin=tree_t0->GetEntries();
        //cout<<"costheta t0 ------------------------------------------------- "<<endl;
        for(int i=0; i<tree_t0->GetEntries(); i++){
        tree_t0->GetEntry(i);
        xbin.push_back(t0);
        ybin.push_back(dedx);
        //cout<<"xbin = "<<t0<<"      ybin = "<<dedx<<endl;
        }*/

        const int nbin=35;
        vector<double> xbin;
        vector<double> ybin;
        for(int i=0; i<35; i++){
            xbin.push_back(test ->get_t0(i));
            ybin.push_back(test ->get_dedx(i));
            //cout<<"xbin = "<<test ->get_t0(i)<<"      ybin = "<<test ->get_dedx(i)<<endl;
        }
        for(int i=0;i<nbin-1;i++){
            double k=0;
            double b=0;
            if(ybin[i]>0){
                if(ybin[i+1]>0){
                    double x1=xbin[i];
                    double y1=ybin[i];
                    double x2=xbin[i+1];
                    double y2=ybin[i+1];
                    k=(y2-y1)/(x2-x1);
                    b=(y1*x2-y2*x1)/(x2-x1);
                }
                else if(i>0){
                    if(ybin[i-1]>0){
                        double x1=xbin[i-1];
                        double y1=ybin[i-1];
                        double x2=xbin[i];
                        double y2=ybin[i];
                        k=(y2-y1)/(x2-x1);
                        b=(y1*x2-y2*x1)/(x2-x1);
                    }
                }
            }
            else {
                if(i>0){
                    if(ybin[i-1]>0 && ybin[i+1]>0){
                        double x1=xbin[i-1];
                        double y1=ybin[i-1];
                        double x2=xbin[i+1];
                        double y2=ybin[i+1];
                        k=(y2-y1)/(x2-x1);
                        b=(y1*x2-y2*x1)/(x2-x1);
                    }
                }
            }
            t0_k.push_back(k);
            t0_b.push_back(b);
        }
    }

    if(true){
        /*TFile fconst9("/home/bes/xcao/besd26/Calib/651_Calib/Psai_cal/doca_eangle_calib/check/doca_eangle_kal_doca110.root");
          const int n = 1600;
          double Iner_gain[n], Iner_chi[n], Iner_hits[n];
          double Out_gain[n], Out_chi[n], Out_hits[n];
          double Id_doca[n], Ip_eangle[n];

          TTree *tree_docasin= (TTree*)fconst9.Get("docasincalib");
          tree_docasin -> SetBranchAddress("Iner_gain", &Iner_gain);
          tree_docasin -> SetBranchAddress("Iner_chi",  &Iner_chi);
          tree_docasin -> SetBranchAddress("Iner_hits", &Iner_hits);
          tree_docasin -> SetBranchAddress("Out_gain",  &Out_gain);
          tree_docasin -> SetBranchAddress("Out_chi",   &Out_chi);
          tree_docasin -> SetBranchAddress("Out_hits",  &Out_hits);
          tree_docasin -> SetBranchAddress("Id_doca",   &Id_doca);
          tree_docasin -> SetBranchAddress("Ip_eangle", &Ip_eangle);
          tree_docasin -> GetEntry(0);
          double docaeangle_gain[n];
          double docaeangle_chisq[n];
          double docaeangle_entries[n];
          double cell[n];
        //cout<<"doca eangle  gain ------------------------------------------------- "<<endl;
        for(int i=0; i<n; i++){
        tree_docasin->GetEntry(i);
        cell[i] = i;
        docaeangle_gain[i] = Out_gain[i];
        docaeangle_chisq[i] = Out_chi[i];
        docaeangle_entries[i] = Out_hits[i]; 
        int Id_Doca_temp = Id_doca[i];
        int Ip_Eangle_temp = Ip_eangle[i];
        m_docaeangle[Id_Doca_temp][Ip_Eangle_temp] = Out_gain[i];
        //cout<<i<<"       "<<Id_Doca_temp<<"      "<<Ip_Eangle_temp<<"        "<<docaeangle_gain[i]<<endl; 
        //m_docaeangle[Id_Doca_temp][Ip_Eangle_temp] = docaeangle_gain[i];   
        //cout<<i<<"       "<<Id_doca[i]<<"       "<<Ip_eangle[i]<<"     Entries : "<<docaeangle_entries[i]<<"      Gain value : "<<docaeangle_gain[i]<<"     chisq : "<<docaeangle_chisq[i] <<endl;

        } 
        m_docaeangle[38][28]=0.72805;*/ 

        const int n = 1600;
        double docaeangle_gain[n];
        double docaeangle_chisq[n];
        double docaeangle_entries[n];
        double cell[n];
        for(int i=0; i<n; i++){
            cell[i] = i;
            docaeangle_gain[i] = test -> get_out_gain(i);
            docaeangle_chisq[i] = test -> get_out_chi(i);
            docaeangle_entries[i] = test -> get_out_hits(i);
            int Id_Doca_temp = test -> get_id_doca(i);
            int Ip_Eangle_temp = test -> get_ip_eangle(i);
            m_docaeangle[Id_Doca_temp][Ip_Eangle_temp] = docaeangle_gain[i];
	    if(m_debug && (Id_Doca_temp==m_debug_i) && (Ip_Eangle_temp==m_debug_j)) std::cout<<"docaeangle_gain["<<Id_Doca_temp<<"]["<<Ip_Eangle_temp<<"]="<<m_docaeangle[m_debug_i][m_debug_j] << std::endl;
            //cout<<i<<"       "<<Id_Doca_temp<<"      "<<Ip_Eangle_temp<<"        "<<docaeangle_gain[i]<<endl; 
        }
    }  


    //get 1d entrance angle constants
    int onedsize=test->get_enanglesize();
    m_venangle.clear();
    for(int i=0; i< onedsize; i++){
        m_venangle.push_back(test->get_enangle(i));
    }


    //temporary way to get hadron saturation constants
    //for Psai hadron saturation

    //only valide for jpsi data now !!!!!
    //have to find ways pass constans for different data 
    /*  m_alpha= 1.35630e-02;
        m_gamma= -6.78907e-04;
        m_delta= 1.18037e-02;
        m_power= 1.66268e+00;
        m_ratio= 9.94775e-01;*/

    const int hadronNo=test -> get_hadronNo();
    // cout<<"@@@hadronNO===="<<hadronNo<<endl;
    m_alpha=test -> get_hadron(0);
    // cout<<"@@@@m_alpha===="<<m_alpha<<endl;
    m_gamma=test -> get_hadron(1);
    // cout<<"@@@@m_gamma===="<<m_gamma<<endl;
    m_delta=test -> get_hadron(2);
    // cout<<"@@@@m_delta====="<<m_delta<<endl;
    m_power=test -> get_hadron(3);
    // cout<<"@@@@m_power====="<<m_power<<endl;
    m_ratio=test -> get_hadron(4);
    // cout<<"@@@m_ratio======"<<m_ratio<<endl;



    /*
       m_delta=0.1;
       m_alpha=0.0282;
       m_gamma=-0.030;
       m_power=1.0;
       m_ratio=1.0;
    //m_delta=0.1;
    //m_alpha=0.0382;
    //m_gamma=-0.0438;
     */

    //m_initConstFlg =true;
}

double DedxCorrecSvc::WireGainCorrec(int wireid, double ex) const{
    if( m_wire_g[wireid] > 0 ){
        double ch_dedx = (ex/m_wire_g[wireid])*m_valid[wireid];
        return ch_dedx;
    }     
    else if( m_wire_g[wireid] == 0 ){
        return ex;
    }
    else return 0;
}

double
DedxCorrecSvc::SaturCorrec( int layer, double costheta,  double dedx ) const {
    //cout<<"DedxCorrecSvc::SaturCorrec"<<endl;
    double dedx_ggs;
    //cout<<"costheta vaule is = "<<costheta<<endl;
    costheta = fabs(costheta);   
    if(m_par_flag == 1) {
        dedx_ggs = m_ggs[0][layer] + m_ggs[1][layer]*costheta +
            m_ggs[2][layer]*pow(costheta,2) + m_ggs[3][layer]*pow(costheta,3);
    } else if(m_par_flag == 0) {
        dedx_ggs = m_ggs[0][layer] + m_ggs[1][layer]*T1(costheta) +
            m_ggs[2][layer]*T2(costheta) + m_ggs[3][layer]*T3(costheta);
    }
    //cout<<"dedx_ggs is = "<<dedx_ggs<<endl;
    dedx_ggs = dedx/dedx_ggs;
    if(dedx_ggs>0.0) return dedx_ggs;
    else return dedx;
}


double 
DedxCorrecSvc::CosthetaCorrec( double costheta,  double dedx ) const {
  //cout<<"DedxCorrecSvc::CosthetaCorrec"<<endl;
  double dedx_cos;
  //cout<<"costheta vaule is = "<<costheta<< " dedx = " << dedx << endl;
  if(fabs(costheta)>1.0)  return dedx;

  const int nbin=cos_k.size()+1;
  const double step=2.00/nbin;
  int n= (int)((costheta+1.00-0.5*step)/step);
  if(costheta>(1.00-0.5*step))
    n=nbin-2;

  if(costheta>-0.5*step && costheta<=0)
    dedx_cos=cos_k[n-1]*costheta+cos_b[n-1];
  else if(costheta>0 && costheta<0.5*step)
    dedx_cos=cos_k[n+1]*costheta+cos_b[n+1];
  else
    dedx_cos=cos_k[n]*costheta+cos_b[n];

  //cout << "cos_k[n]="<<cos_k[n] << "  cos_b[n]=" << cos_b[n] << 
  //  "  dedx_cos=" << dedx_cos << "  final dedx=" << dedx/dedx_cos << endl;

  //conside physical edge around 0.93
  if(nbin==80){ // temporally for only nbin = 80 
    if(costheta>0.80 && costheta<0.95){
      n = 77;
      if(costheta<0.9282) n--;
      if(costheta<0.9115) n--;
      if(costheta<0.8877) n--;
      if(costheta<0.8634) n--;
      if(costheta<0.8460) n--;
      if(costheta<0.8089) n--;
      dedx_cos=cos_k[n]*costheta+cos_b[n];
    }
    else if(costheta<-0.80 && costheta>-0.95){
      n = 2;
      if(costheta>-0.9115) n++;
      if(costheta>-0.8877) n++;
      if(costheta>-0.8634) n++;
      if(costheta>-0.8460) n++;
      if(costheta>-0.8089) n++;
      dedx_cos=cos_k[n]*costheta+cos_b[n];
    }
  }

  if(dedx_cos>0){
    dedx_cos = dedx/dedx_cos;
    return dedx_cos;
  }
  else return dedx;
}


double
DedxCorrecSvc::T0Correc( double t0,  double dedx ) const {
    //  cout<<"DedxCorrecSvc::T0Correc"<<endl;
    double dedx_t0;
    const int nbin=t0_k.size()+1;
    if(nbin!=35)
      cout<<"there should be 35 bins for t0 correction, check it!"<<endl;

    int n=0;
    if(t0<575)
      n=0;
    else if(t0<610)
      n=1;
    else if(t0>=610 && t0<1190){
        n=(int)( (t0-610.0)/20.0 ) + 2;
    }
    else if(t0>=1190 && t0<1225)
      n=31;
    else if(t0>=1225 && t0<1275)
      n=32;
    else if(t0>=1275)
      n=33;

    dedx_t0=t0_k[n]*t0+t0_b[n];

    if(dedx_t0>0){
        dedx_t0 = dedx/dedx_t0;
        return dedx_t0;
    }
    else return dedx;
}

double
DedxCorrecSvc::HadronCorrec( double costheta,  double dedx ) const {
    //  cout<<"DedxCorrecSvc::HadronCorrec"<<endl;
    //constant 1.00 in the following function is the mean dedx of normalized electrons.
  dedx=dedx/550.00;
  return D2I(costheta, I2D(costheta,1.00)/1.00*dedx)*550;
}
double
DedxCorrecSvc::LayerGainCorrec( int layid, double dedx ) const {
    //  cout<<"DedxCorrecSvc::LayerGainCorrec"<<endl;
    if( m_layer_g[layid] > 0 ){
        double ch_dedx = dedx/m_layer_g[layid];
        return ch_dedx;
    }     
    else if( m_layer_g[layid] == 0 ){
        return dedx;
    }
    else return 0;
}


double 
DedxCorrecSvc::RungCorrec( int runNO, double dedx ) const{
    //  cout<<"DedxCorrecSvc::RungCorrec"<<endl;
    double dedx_rung;  
    int run_ture =0;
    //cout<<"N_run : "<<N_run<<"       runNO : "<<runNO<<endl;

    for(int j=0;j<10000;j++) {
        if(runNO == m_rung[2][j]) {
            dedx_rung = dedx/m_rung[0][j];
            run_ture =1;
            return dedx_rung;     
        }    
    }
    if(run_ture ==0)
    {
        cout<<"Warning!!!  in DedxCorrecSvc::RungCorrec(): no rungain to "<<runNO<<endl;
        exit(1);
    }
}


double
DedxCorrecSvc::DriftDistCorrec( int layer, double dd,  double dedx ) const {
    //  cout<<"DedxCorrecSvc::DriftDistCorrec"<<endl;
    double dedx_ddg;
    //cout<<"m_par_flag = "<<m_par_flag<<endl;
    //cout<<"dd vaule is = "<<dd<<endl;
    dd = fabs(dd); 
    if(m_par_flag == 1) {
        dedx_ddg = m_ddg[0][layer] + m_ddg[1][layer]*dd + 
            m_ddg[2][layer]*dd*dd + m_ddg[3][layer]*pow(dd,3);
    } else if(m_par_flag == 0) {
        dedx_ddg = m_ddg[0][layer] + m_ddg[1][layer]*T1(dd) + 
            m_ddg[2][layer]*T2(dd) + m_ddg[3][layer]*T3(dd);
    }
    //cout<<"dedx_ddg is = "<<dedx_ddg<<endl;
    dedx_ddg = dedx/dedx_ddg;
    if (dedx_ddg>0.0)  return dedx_ddg;
    else return dedx;
}


double
DedxCorrecSvc::EntaCorrec( int layer, double eangle,  double dedx ) const {
    // cout<<"DedxCorrecSvc::EntaCorrec"<<endl;
//    double dedx_enta;
//    if(m_par_flag == 1) {
//        dedx_enta = m_enta[0][layer] + m_enta[1][layer]*sinenta + 
//            m_enta[2][layer]*sinenta*sinenta + m_enta[3][layer]*pow(sinenta,3);
//    } else if(m_par_flag == 0) {
//        dedx_enta = m_enta[0][layer] + m_enta[1][layer]*T1(sinenta) + 
//            m_enta[2][layer]*T2(sinenta) + m_enta[3][layer]*T3(sinenta);
//    }
//    dedx_enta = dedx/dedx_enta;
//    if (dedx_enta>0.0)  return dedx_enta;
//    else return dedx;

    if(eangle>-0.25 && eangle<0.25){
        double stepsize= 0.5/m_venangle.size();
        int nsize= m_venangle.size();
        double slope=0;
        double offset=1;
        double y1=0,y2=0,x1=0,x2=0;

        if(eangle>=-0.25+0.5*stepsize && eangle<=0.25-0.5*stepsize){
            int bin = (int)( (eangle-(-0.25+0.5*stepsize))/stepsize);
            y1=m_venangle[bin];
            x1=-0.25+0.5*stepsize+bin*stepsize;
            y2=m_venangle[bin+1];
            x2=-0.25+1.5*stepsize+bin*stepsize;
        }
        else if(eangle<=-0.25+0.5*stepsize){
            y1=m_venangle[0];
            x1=-0.25+0.5*stepsize;
            y2=m_venangle[1];
            x2=-0.25+1.5*stepsize;
        }
        else if( eangle>=0.25-0.5*stepsize){
            y1=m_venangle[nsize-2];
            x1=0.25-1.5*stepsize;
            y2=m_venangle[nsize-1];
            x2=0.25-0.5*stepsize;
        }
        double kcof= (y2-y1)/(x2-x1);
        double bcof= y1-kcof*x1; 
        double ratio = kcof*eangle+bcof;
        dedx=dedx/ratio;
    }

    return dedx;
}


double
DedxCorrecSvc::ZdepCorrec( int layer, double z,  double dedx ) const {
    // cout<<"DedxCorrecSvc::ZdepCorrec"<<endl;
    double dedx_zdep;
    if(m_par_flag == 1) {
        dedx_zdep = m_zdep[0][layer] + m_zdep[1][layer]*z + 
            m_zdep[2][layer]*z*z + m_zdep[3][layer]*pow(z,3);
    } else if(m_par_flag == 0) {
        dedx_zdep = m_zdep[0][layer] + m_zdep[1][layer]*T1(z) + 
            m_zdep[2][layer]*T2(z) + m_zdep[3][layer]*T3(z);
    }
    dedx_zdep = dedx/dedx_zdep;
    if (dedx_zdep>0.0)  return dedx_zdep;
    else return dedx;

    //return dedx;
}


double
DedxCorrecSvc::DocaSinCorrec( int layer,double doca, double eangle, double dedx ) const {
  if(m_debug) cout<<"DedxCorrecSvc::DocaSinCorrec"<<endl;
  // cout<<"doca = "<<doca<<"       eangle = "<<eangle<<endl;

  if(eangle>0.25) eangle = eangle -0.5;
  else if(eangle < -0.25) eangle = eangle +0.5;
  int iDoca;
  int iEAng = 0;
  doca = doca/doca_norm[layer];
  iDoca =(Int_t)floor((doca-Out_DocaMin)/Out_Stepdoca);
  if(doca<Out_DocaMin) iDoca=0;
  if(doca>Out_DocaMax) iDoca=NumSlicesDoca-1;
  if(iDoca >=(Int_t)floor((Out_DocaMax-Out_DocaMin)/Out_Stepdoca) ) 
    iDoca = (Int_t)floor((Out_DocaMax-Out_DocaMin)/Out_Stepdoca)-1;
  if(iDoca<0) iDoca = 0;
  if(m_debug) cout<<"iDoca : "<<iDoca<<"      doca : "<<doca<<"       Out_DocaMin : "<<Out_DocaMin<<"      Out_Stepdoca : "<<Out_Stepdoca<<endl;   

  //iEAng = (Int_t)floor((eangle-Phi_Min)/IEangle_step);
  for(int i =0; i<14; i++){
    //iEAng =0;
    if(eangle <Eangle_value_cut[i] || eangle > Eangle_value_cut[i+1]) continue;
    else if(eangle>= Eangle_value_cut[i] && eangle < Eangle_value_cut[i+1]) {
      for(int k =0; k<i; k++){
	iEAng+= Eangle_cut_bin[k];
      }
      double eangle_bin_cut_step = (Eangle_value_cut[i+1] - Eangle_value_cut[i])/Eangle_cut_bin[i];
      int temp_bin = int((eangle-Eangle_value_cut[i])/eangle_bin_cut_step);
      iEAng+= temp_bin;
    }
  }
  //cout<<iDoca <<"         "<<iEAng<<endl;
  if(m_docaeangle[iDoca][iEAng]>0) {
    if(m_debug && (iDoca==m_debug_i) && (iEAng==m_debug_j)) 
      cout << "doca: " << doca << "  eang"  << eangle << "  dedx before: " << dedx << endl;
    dedx = dedx/m_docaeangle[iDoca][iEAng];
    if(m_debug && (iDoca==m_debug_i) && (iEAng==m_debug_j))
      cout << "gain: " << m_docaeangle[iDoca][iEAng] << "  dedx after: " << dedx << endl;
  } 
  return dedx;
}


double
DedxCorrecSvc::DipAngCorrec(int layer, double costheta, double dedx) const {
}

double
DedxCorrecSvc::GlobalCorrec( double dedx) const{
    if( m_mdcg_flag == 0 ) return dedx;
    double calib_ex = dedx;
    if( m_dedx_gain > 0 ) calib_ex /= m_dedx_gain;
    return calib_ex;
}



double 
DedxCorrecSvc::CellCorrec( int ser, double adc, double dd, double sinenta,
            double z, double costheta ) const {

    if( m_rung_flag == 0 && m_wireg_flag == 0 && m_ddg_flag == 0 
                && m_layerg_flag == 0 && m_zdep_flag == 0 && 
                m_ggs_flag == 0) return adc;
    adc = m_valid[ser]*m_wire_g[ser]*adc;
    //  int lyr = Wire2Lyr(ser); 
    int lyr = ser; 
    double ex = adc;
    double correct1_ex , correct2_ex, correct3_ex, correct4_ex, correct5_ex;

    if( m_ggs_flag) {
        correct1_ex =  SaturCorrec( lyr, costheta, adc );
        ex = correct1_ex;
    } else {
        correct1_ex = adc;
    }

    if( m_ddg_flag)  {
        correct2_ex = DriftDistCorrec( lyr, dd, correct1_ex );
        ex = correct2_ex;
    } else {
        correct2_ex =correct1_ex;
    }
    if( m_enta_flag)  {
        correct3_ex = DriftDistCorrec( lyr, sinenta, correct2_ex );
        ex = correct3_ex;
    } else {
        correct3_ex =correct2_ex;
    }

    if( m_zdep_flag) {
        correct4_ex = ZdepCorrec( lyr, z, correct3_ex );
        ex = correct4_ex;
    } else {
        correct4_ex = correct3_ex; 
    }

    if( m_layerg_flag)  {
        correct5_ex = LayerGainCorrec( lyr, correct4_ex );
        ex = correct5_ex;
    } else {
        correct5_ex = correct4_ex;
    }
    return ex;

}

double
DedxCorrecSvc::LayerCorrec( int layer, double z, double costheta, double ex ) const{
    //cout<<"DedxCorrecSvc::LayerCorrec"<<endl;

    if( m_zdep_flag == 0 && m_ggs_flag == 0 ) return ex;

    double calib_ex = ZdepCorrec( layer, z, ex );
    if( m_ggs_flag != 0 ) calib_ex =  SaturCorrec( layer, costheta, calib_ex );
    return calib_ex;

}

double 
DedxCorrecSvc::TrkCorrec( double costheta, double dedx ) const{
    //  cout<<"DedxCorrecSvc::TrkCorrec"<<endl;
    if( m_mdcg_flag == 0 ) return dedx;
    ///dEdx calib. for each track
    double calib_ex = dedx;

    double run_const = m_dedx_gain;
    if( run_const > 0 && m_mdcg_flag != 0 ) calib_ex /= run_const;

    return calib_ex;

}


double 
DedxCorrecSvc::StandardCorrec( int runFlag, int ntpFlag, int runNO, double pathl, int wid, int layid, double adc, double dd, double eangle, double z, double costheta ) const {
    // cout<<"DedxCorrecSvc::StandardCorrec"<<endl;
    //int layid =  MdcID::layer(mdcid);
    //int localwid = MdcID::wire(mdcid);
    //int w0id = geosvc->Layer(layid)->Wirst();
    //int wid = w0id + localwid;
    //double pathl = PathL(ntpFlag, hel, layid, localwid, z); 
    ////pathl= PathLCosmic(hel, layid, localwid, z, sigmaz );
    double ex = adc;
    if( runNO<0 ) return ex;
    ex = ex*1.5/pathl;
    //double ex = adc/pathl;
    //if( runNO<0 ) return ex;
    if( ntpFlag ==0) return ex;
    //double ex = adc/pathl;
    if( m_rung_flag == 0 && m_wireg_flag == 0 &&   m_dcasin_flag==0 && m_ddg_flag == 0 
                && m_layerg_flag == 0 && m_zdep_flag == 0 && 
                m_ggs_flag == 0) return ex;

    if(m_rung_flag) {
        ex = RungCorrec( runNO, ex );
    }

    if( m_wireg_flag)  {
        ex = WireGainCorrec(wid, ex);
    }

    if( m_dcasin_flag) {
      if(runFlag<3) {
	ex = DriftDistCorrec( layid, dd, ex );
      }
      else{ ex = DocaSinCorrec(layid, dd, eangle, ex);}
    }

    if(m_enta_flag && runFlag>=3){
      ex = EntaCorrec(layid, eangle, ex);
    }

    // ddg is not use anymore, it's replaced by DocaSin
    if(m_ddg_flag)  {
        ex = DriftDistCorrec( layid, dd, ex );
    }

    if(m_ggs_flag) {
      if(runFlag <3 ){
	ex =  SaturCorrec( layid, costheta, ex );
      }
      else{ ex = CosthetaCorrec( costheta, ex );}
      // Staur is OLD, Costheta is NEW.
    }

    if( m_sat_flag) {
        ex =  HadronCorrec( costheta, ex );
    }  

    if( m_zdep_flag) {
        ex = ZdepCorrec( layid, z, ex );
    } 

    if( m_layerg_flag)  {
        ex = LayerGainCorrec( layid, ex );
    } 

    if( m_dip_flag){
        ex = DipAngCorrec(layid, costheta, ex);
    } 

    if( m_mdcg_flag)  {
        ex = GlobalCorrec( ex );
    } 
    return ex;
}

double 
DedxCorrecSvc::StandardHitCorrec(int calib_rec_Flag, int runFlag, int ntpFlag, int runNO, double pathl,  int wid, int layid, double adc, double dd, double eangle, double costheta ) const {
  if(m_debug) cout<<"DedxCorrecSvc::StandardHitCorrec"<<endl;
  //int layid =  MdcID::layer(mdcid);
  //int localwid = MdcID::wire(mdcid);
  //int w0id = geosvc->Layer(layid)->Wirst();
  //int wid = w0id + localwid;
  //double pathl = PathL(ntpFlag, hel, layid, localwid, z); 
  //cout<<"DedxCorrecSvc::StandardHitCorrec pathl= "<<pathl<<endl;
  //pathl= PathLCosmic(hel, layid, localwid, z, sigmaz );
  double ex = adc;
  if( runNO<0 ) return ex;
  ex = ex*1.5/pathl;
  //if( runNO<0 ) return ex;
  //double ex = adc/pathl;
  if( ntpFlag ==0) return ex;
  //if(ntpFlag>0) cout<<"dE/dx value after path correction: "<<ex<<endl; 
  //double ex = adc/pathl;
  //cout<<m_rung_flag << "    "<<m_wireg_flag<<"    "<<m_ddg_flag<<"     "<<m_ggs_flag<<endl;
  if( m_rung_flag == 0 && m_wireg_flag == 0 && m_ddg_flag == 0 
      && m_layerg_flag == 0 && m_zdep_flag == 0 &&   m_dcasin_flag==0 
      && m_ggs_flag == 0 && m_enta_flag==0) return ex;

  if(m_rung_flag) {
    ex = RungCorrec( runNO, ex );
  }
  //if(ntpFlag>0) cout<<"dE/dx value after run by run correction: "<<ex<<endl;

  if( m_wireg_flag)  {
    ex = WireGainCorrec(wid, ex);
  }
  //if(ntpFlag>0) cout<<"dE/dx value after wire gain correction: "<<ex<<endl;

  if( m_dcasin_flag) {
    if(runFlag<3){
      ex = DriftDistCorrec( layid, dd, ex );
    }
    else{ 
      //cout<<layid<<"        "<<dd<<"       "<<eangle<<"         "<<ex<<endl;    
      ex = DocaSinCorrec(layid, dd, eangle, ex);
    }
  } 

  // 1D entrance angle correction
  if(m_enta_flag && runFlag>=3){
    ex = EntaCorrec(layid, eangle, ex);
  }

  // ddg is not used anymore
  if( m_ddg_flag)  {
    ex = DriftDistCorrec( layid, dd, ex );
  }
  //if(ntpFlag>0) cout<<"dE/dx value after ddg correction: "<<ex<<endl;

  if(m_ggs_flag) {
    if(runFlag <3 ){
      ex =  SaturCorrec( layid, costheta, ex );
    }
    else{ ex = ex;} // do not do the cos(theta) correction at hit level
  } 
  //if(ntpFlag>0) cout<<"dE/dx value after costheta correction: "<<ex<<endl; 
  return ex;
}


double 
DedxCorrecSvc::StandardTrackCorrec(int calib_rec_Flag, int runFlag, int ntpFlag, int runNO, double ex, double costheta, double t0 ) const {
  if(m_debug)  cout<<"DedxCorrecSvc::StandardTrackCorrec"<<endl;
  if( runNO<0 ) return ex;
  if( ntpFlag ==0) return ex;
  if( runFlag <3) return ex;
  if( calib_rec_Flag ==1){
    ex =  CosthetaCorrec( costheta, ex );   
    if(m_t0_flag) ex= T0Correc(t0, ex);
    return ex;                        
  }

  //if(ntpFlag>0) cout<<"trcak value before costheta correction: "<<ex<<endl;
  if( m_ggs_flag) {
    ex =  CosthetaCorrec( costheta, ex );
  }  
  //if(ntpFlag>0) cout<<"trcak value after costheta correction: "<<ex<<endl;
  if(m_t0_flag){
    if(runFlag==3) {ex= T0Correc(t0, ex);}
    else if(runFlag>3) {ex=ex;}
  }
  //if(ntpFlag>0) cout<<"trcak value after all correction: "<<ex<<endl;
  if( m_sat_flag) {
    ex =  HadronCorrec( costheta, ex );
  }  

  if(m_rung_flag && calib_rec_Flag ==2){
    double rungain_temp =RungCorrec( runNO, ex )/ex;
    ex = ex/rungain_temp;
  }  

  //if(ntpFlag>0) cout<<"trcak value no run gain correction: "<<ex<<endl;
  return ex;

}



void
DedxCorrecSvc::init_param() {

    //cout<<"DedxCorrecSvc::init_param()"<<endl;

    for( int i = 0; i<6796 ; i++) {
        m_valid[i] = 1.0;
        m_wire_g[i] = 1.0;
    }
    m_dedx_gain = 1.0;
    m_dedx_resol = 1.0;
    for(int j = 0; j<100; j++){
        m_rung[0][j] =1;
    }
    for(int j = 0; j<43; j++) {
        m_layer_g[j] = 1.0;
        m_ggs[0][j] = 1.0;
        m_ddg[0][j] = 1.0;
        m_enta[0][j] = 1.0;
        m_zdep[0][j] = 1.0;
        for(int k = 1; k<4; k++ ) {
            m_ggs[k][j] = 0.0;
            m_ddg[k][j] = 0.0;
            m_enta[k][j] = 0.0;
            m_zdep[k][j] = 0.0;
        }
    }

    std::cout<<"DedxCorrecSvc::init_param()!!!"<<std::endl;
    std::cout<<"hello,init_param"<<endl;
    m_initConstFlg =true;

}


void
DedxCorrecSvc::set_flag( int calib_F ) {
  // cout<<"DedxCorrecSvc::set_flag"<<endl;
  cout<<"calib_F is = "<<calib_F<<endl;
  if(calib_F<0){ m_enta_flag = 0; calib_F = abs(calib_F); }
  else m_enta_flag = 1;
  m_rung_flag    = ( calib_F &  1 )? 1 : 0;
  m_wireg_flag   = ( calib_F &  2 )? 1 : 0;
  m_dcasin_flag  = ( calib_F &  4 )? 1 : 0;
  m_ggs_flag     = ( calib_F &  8 )? 1 : 0;
  m_ddg_flag = 0;
  //m_ddg_flag     = ( calib_F &  8 )? 1 : 0;
  m_t0_flag      = ( calib_F & 16 )? 1 : 0;
  m_sat_flag     = ( calib_F & 32 )? 1 : 0;
  m_layerg_flag  = ( calib_F & 64 )? 1 : 0;
  //m_dcasin_flag  = ( calib_F & 128 )? 1 : 0;
  m_dip_flag     = ( calib_F & 256 )? 1 : 0;
  m_mdcg_flag    = ( calib_F & 512 )? 1 : 0;
}



//funcation to calculate the path pength of Cosmic data in the layer 
/*double
  DedxCorrecSvc::PathLCosmic(const Dedx_Helix& hel, int layer, int cellid, double z,double sigmaz ) const {
////// calculate the cooridate of P0 
HepPoint3D piv(hel.pivot()); 
double dr  = hel.a()[0];
double phi0  = hel.a()[1];
double tanl  = hel.a()[4];
double dz = hel.a()[3];  

double dr  = -19.1901;
double phi0  = 3.07309;
double tanl  = -0.466654;
double dz = 64.8542;

double csf0 = cos(phi0);
double snf0 = sin(phi0);
double x_c = dr*csf0;
double y_c = dr*snf0;
double z_c = hel.a()[3];

////// calculate the cooridate of hits
////calculate the track length in r_phi plane

double m_crio[2];
double m_zb, m_zf, Calpha;
//  double sintheta = sin(M_PI_2-atan(hel.a()[4]));
double sintheta = sin(M_PI_2-atan(tanl));
// retrieve Mdc  geometry information
double rcsiz1= geosvc->Layer(layer)->RCSiz1();
double rcsiz2= geosvc->Layer(layer)->RCSiz2();
double rlay= geosvc->Layer(layer)->Radius();
int ncell= geosvc->Layer(layer)->NCell();
double phioffset= geosvc->Layer(layer)->Offset();
float shift= geosvc->Layer(layer)->Shift();
double slant= geosvc->Layer(layer)->Slant();
double length = geosvc->Layer(layer)->Length();
int type = geosvc->Layer(layer)->Sup()->Type();
//  shift = shift*type;

//conversion of the units(mm=>cm)
length = 0.1*length;
rcsiz1 = 0.1*rcsiz1;
rcsiz2 = 0.1*rcsiz2;
rlay = 0.1*rlay;
m_zb = 0.5*length;
m_zf = -0.5*length;
m_crio[0] = rlay - rcsiz1;
m_crio[1] = rlay + rcsiz2;

if(z == 0)
{ if(rlay<= fabs(dr)) rlay = rlay + rcsiz2;
if(rlay<fabs(dr)) return -1;
double t_digi = sqrt(rlay*rlay - dr*dr);
double x_t_digi = x_c - t_digi*snf0;
double y_t_digi = y_c + t_digi*csf0;
double z_t_digi = z_c + t_digi*tanl; 
double x__t_digi = x_c + t_digi*snf0;
double y__t_digi = y_c - t_digi*csf0;
double z__t_digi = z_c - t_digi*tanl; 
double phi_t_digi = atan2(y_t_digi,x_t_digi);
double phi__t_digi = atan2(y__t_digi,x__t_digi);
phi_t_digi = fmod( phi_t_digi+4*M_PI,2*M_PI );
phi__t_digi = fmod( phi__t_digi+4*M_PI,2*M_PI );
double phibin_digi = 2.0*M_PI/ncell;
double phi_cellid_digi = phioffset + shift*phibin_digi*0.5 + cellid *phibin_digi;
if(fabs(phi_cellid_digi - phi_t_digi)<fabs(phi_cellid_digi - phi__t_digi))
z = z_t_digi;
else if (fabs(phi_cellid_digi - phi_t_digi)>fabs(phi_cellid_digi - phi__t_digi))
z = z__t_digi;
else  z = z_t_digi;
}

int sign = 1;  ///assumed the first half circle
int epflag[2];
Hep3Vector iocand;
Hep3Vector cell_IO[2];
double dphi, downin;
Hep3Vector zvector;
if( type ) {
    downin = (z*z-m_zb*m_zb)*pow(tan(slant),2);
    m_crio[0] = sqrt(m_crio[0]*m_crio[0]+downin);
    m_crio[1] = sqrt(m_crio[1]*m_crio[1]+downin);
}

// calculate t value
double t[2];
if(m_crio[1]<fabs(dr)) return -1;
else if(m_crio[0]<fabs(dr)) {
    t[0] = sqrt(m_crio[1]*m_crio[1] - dr*dr);
    t[1] = -t[0];
}
else{
    for( int i = 0; i < 2; i++ ) {
        if(m_crio[i]<fabs(dr)) return -1;
        t[i] = sqrt(m_crio[i]*m_crio[i] - dr*dr);
    }  
}

// calculate the cooridate of hits
double x_t[2],y_t[2],z_t[2];
double x__t[2],y__t[2],z__t[2];
double x_p[2],y_p[2],z_p[2];
for( int i = 0; i < 2; i++){
    x_t[i] = x_c - t[i]*snf0;
    y_t[i] = y_c + t[i]*csf0;
    z_t[i] = z_c + t[i]*tanl; 
    x__t[i] = x_c + t[i]*snf0;
    y__t[i] = y_c - t[i]*csf0;
    z__t[i] = z_c - t[i]*tanl; 
}

double  phi_in_t,phi_in__t, phi_out_t,phi_out__t,phi_t,phi__t; 
double  phibin = 2.0*M_PI/ncell;
double  phi_cellid = phioffset + shift*phibin*(0.5-z/length) + cellid *phibin;
phi_in_t = atan2(y_t[0],x_t[0]);
phi_out_t = atan2(y_t[1],x_t[1]);
phi_in__t = atan2(y__t[0],x__t[0]);
phi_out__t = atan2(y__t[1],x__t[1]);
phi_t = atan2(((y_t[0]+y_t[1])/2),((x_t[0]+x_t[1])/2));
phi__t = atan2(((y__t[0]+y__t[1])/2),((x__t[0]+x__t[1])/2));

phi_in_t = fmod( phi_in_t+4*M_PI,2*M_PI );
phi_out_t = fmod( phi_out_t+4*M_PI,2*M_PI );
phi_in__t = fmod( phi_in__t+4*M_PI,2*M_PI );
phi_out__t = fmod( phi_out__t+4*M_PI,2*M_PI );
phi_t = fmod( phi_t+4*M_PI,2*M_PI );
phi__t = fmod( phi__t+4*M_PI,2*M_PI );
phi_cellid = fmod( phi_cellid+4*M_PI,2*M_PI );

if(fabs(phi_cellid - phi_t)<fabs(phi_cellid - phi__t))
{
    for(int i =0; i<2; i++ )
    { x_p[i] = x_t[i];
        y_p[i] = y_t[i];
        z_p[i] = z_t[i];}
}
else if (fabs(phi_cellid - phi_t)>fabs(phi_cellid - phi__t))
{ 
    for(int i =0; i<2; i++ )
    { x_p[i] = x__t[i];
        y_p[i] = y__t[i];
        z_p[i] = z__t[i];}
}
else  
{
    for(int i =0; i<2; i++ )
    { x_p[i] = x_t[i];
        y_p[i] = y_t[i];
        z_p[i] = z_t[i];}
}

//calculate path length in r_phi plane and all length in this layer
double ch_ltrk_rp, ch_ltrk;
ch_ltrk_rp = sqrt((x_p[0]-x_p[1])*(x_p[0]-x_p[1])+(y_p[0]-y_p[1])*(y_p[0]-y_p[1]));
ch_ltrk = ch_ltrk_rp/sintheta;
//give cellid of in and out of this layer
double phi_in,phi_out;
phi_in = atan2(y_p[0],x_p[0]);
phi_out = atan2(y_p[1],x_p[1]);
phi_in = fmod( phi_in+4*M_PI,2*M_PI );
phi_out = fmod( phi_out+4*M_PI,2*M_PI ); 

//determine the in/out cellid
double  inlow, inup, outlow, outup, gap, phi1, phi2, phi_mid, phi_midin, phi_midout;
int cid_in, cid_out;
//  int cid_in_t,cid_in__t,cid_out_t,cid_out__t;
//cache sampl in each cell for this layer
std::vector<double> phi_entrance; 
std::vector<double> sampl;  
sampl.resize(ncell);
phi_entrance.resize(ncell);
for(int k=0; k<ncell; k++) {
    sampl[k] = -1.0;
    phi_entrance[k] = 0;
}     

cid_in = cid_out = -1;
phibin = 2.0*M_PI/ncell;
//determine the in/out cell id
double stphi[2], phioff[2];
stphi[0] = shift*phibin*(0.5-z_p[0]/length);
stphi[1] = shift*phibin*(0.5-z_p[1]/length);
phioff[0] = phioffset+stphi[0];
phioff[1] = phioffset+stphi[1];
for(int i=0; i<ncell; i++) {
    // for stereo layer
    //    phi[i] = phioff[0]+phibin*i;
    inlow = phioff[0]+phibin*i-phibin*0.5;
    inup = phioff[0]+phibin*i+phibin*0.5;
    outlow = phioff[1]+phibin*i-phibin*0.5;
    outup = phioff[1]+phibin*i+phibin*0.5;
    inlow = fmod( inlow+4*M_PI,2*M_PI );
    inup = fmod( inup+4*M_PI,2*M_PI );
    outlow = fmod( outlow+4*M_PI,2*M_PI );
    outup = fmod( outup+4*M_PI,2*M_PI );
#ifdef YDEBUG
    cout << "shift " << shift<< " cellid " << i
        <<" phi_in " << phi_in << " phi_out " << phi_out 
        << " inup "<< inup << " inlow " << inlow 
        << " outup "<< outup << " outlow " << outlow << endl;
#endif
    if(phi_in>=inlow&&phi_in<inup) cid_in = i;
    if(phi_out>=outlow&&phi_out<outup) cid_out = i;
    if ( inlow>inup) {
        if((phi_in>=inlow&&phi_in<2.0*M_PI)||(phi_in>=0.0&&phi_in<inup)) cid_in = i;
    } 
    if ( outlow>outup) {
        if((phi_out>=outlow&&phi_out<2.0*M_PI)||(phi_out>=0.0&&phi_out<outup)) cid_out = i;
    }
}			    

// judge how many cells are traversed and deal with different situations
phi_midin = phi_midout = phi1 = phi2 = -999.0;
gap = -999.0; 
//only one cell traversed
if( cid_in == cid_out) {
    sampl[cid_in] = ch_ltrk;
    //   return ch_ltrk;

} else  if(cid_in < cid_out) {
    //in cell id less than out cell id
    //deal with the special case crossing x axis
    if( cid_out-cid_in>ncell/2  ) {
        //      if(gap>=M_PI) gap = 2.0*M_PI-gap;
        phi_midin = phioff[0]+phibin*cid_in-phibin*0.5;
        phi_midout = phioff[1]+phibin*cid_out+phibin*0.5;
        phi_midin = fmod( phi_midin+4*M_PI,2*M_PI );
        phi_midout = fmod( phi_midout+4*M_PI,2*M_PI );
        phi1 = phi_midout-phi_out;
        phi2 = phi_in-phi_midin;
        phi1 = fmod(phi1+2.0*M_PI,2.0*M_PI);
        phi2 = fmod(phi2+2.0*M_PI,2.0*M_PI);
        gap = phi1+phi2+(ncell-1-cid_out+cid_in)*phibin;
        gap = fmod(gap+2.0*M_PI,2.0*M_PI);
        sampl[cid_in]=phi2/gap*ch_ltrk;
        sampl[cid_out]=phi1/gap*ch_ltrk;

        for( int jj = cid_out+1; jj<ncell; jj++) {
            sampl[jj]=phibin/gap*ch_ltrk;
        }
        for( int jj = 0; jj<cid_in; jj++) {
            sampl[jj]=phibin/gap*ch_ltrk;
        }	    

    } else {
        //normal case
        phi_midin = phioff[0]+phibin*cid_in+phibin*0.5;
        phi_midout = phioff[1]+phibin*cid_out-phibin*0.5;
        phi_midin = fmod( phi_midin+4*M_PI,2*M_PI );
        phi_midout = fmod( phi_midout+4*M_PI,2*M_PI );
        phi1 = phi_midin-phi_in;
        phi2 = phi_out-phi_midout;
        phi1 = fmod(phi1+2.0*M_PI,2.0*M_PI);
        phi2 = fmod(phi2+2.0*M_PI,2.0*M_PI);
        gap = phi1+phi2+(cid_out-cid_in-1)*phibin;
        gap = fmod(gap+2.0*M_PI,2.0*M_PI);
        sampl[cid_in]=phi1/gap*ch_ltrk;
        sampl[cid_out]=phi2/gap*ch_ltrk;
        phi_entrance[cid_in] = phi_in;
        phi_entrance[cid_out] = phi_midout;
        for( int jj = cid_in+1; jj<cid_out; jj++) {
            sampl[jj]=phibin/gap*ch_ltrk;
        }
    }
} else  if(cid_in > cid_out) {
    //in cell id greater than out cell id
    //deal with the special case crossing x axis
    if( cid_in-cid_out>ncell/2  ) {
        //      if(gap>=M_PI) gap = 2.0*M_PI-gap;
        phi_midin = phioff[0]+phibin*cid_in+phibin*0.5;
        phi_midout = phioff[1]+phibin*cid_out-phibin*0.5;
        phi_midin = fmod( phi_midin+4*M_PI,2*M_PI );
        phi_midout = fmod( phi_midout+4*M_PI,2*M_PI );
        phi1 = phi_midin-phi_in;
        phi2 = phi_out-phi_midout;
        phi1 = fmod(phi1+2.0*M_PI,2.0*M_PI);
        phi2 = fmod(phi2+2.0*M_PI,2.0*M_PI);
        gap = phi1+phi2+(ncell-1-cid_in+cid_out)*phibin;
        gap = fmod(gap+2.0*M_PI,2.0*M_PI);
        sampl[cid_out]=phi2/gap*ch_ltrk;
        sampl[cid_in]=phi1/gap*ch_ltrk; 

        for( int jj = cid_in+1; jj<ncell; jj++) {
            sampl[jj]=phibin/gap*ch_ltrk;
        }
        for( int jj = 0; jj<cid_out; jj++) {
            sampl[jj]=phibin/gap*ch_ltrk;
        }
    } else {
        //normal case
        phi_midin = phioff[0]+phibin*cid_in-phibin*0.5;
        phi_midout = phioff[1]+phibin*cid_out+phibin*0.5;
        phi_midin = fmod( phi_midin+4*M_PI,2*M_PI );
        phi_midout = fmod( phi_midout+4*M_PI,2*M_PI );
        phi1 = phi_midout-phi_out;
        phi2 = phi_in-phi_midin;
        phi1 = fmod(phi1+2.0*M_PI,2.0*M_PI);
        phi2 = fmod(phi2+2.0*M_PI,2.0*M_PI);
        gap = phi1+phi2+(cid_in-cid_out-1)*phibin;
        gap = fmod(gap+2.0*M_PI,2.0*M_PI);
        sampl[cid_in]=phi2/gap*ch_ltrk;
        sampl[cid_out]=phi1/gap*ch_ltrk;
        for( int jj = cid_out+1; jj<cid_in; jj++) {
            sampl[jj]=phibin/gap*ch_ltrk;
        }
    }
}

#ifdef YDEBUG
if(sampl[cellid]<0.0) {
    if(cid_in!=cid_out) cout<<"?????????"<<endl;
    cout<< "layerId  " << layer <<"  cell id  "<< cellid <<"  shift  " << shift 
        << "  cid_in  " << cid_in << "  cid_out  " << cid_out << endl;

    cout <<" phi_in " << phi_in <<" phi_midin " << phi_midin << " phi_out " 
        << phi_out << " phi_midout " << phi_midout <<endl;
    cout<<"total sampl " << ch_ltrk << " gap "<< gap << " phi1 " 
        << phi1 << " phi2 " << phi2 << " phibin " <<  phibin << endl; 


    for(int l=0; l<ncell; l++) {
        if(sampl[l]>=0.0) 
          cout<<"picked cellid  " << l << "  sampling length  "<< sampl[l]<< endl;
    }
}
#endif
return sampl[cellid];

}

*/


// function to calculate the path length in the layer

double
DedxCorrecSvc::PathL( int ntpFlag, const Dedx_Helix& hel, int layer, int cellid, double z ) const {

    double length = geosvc->Layer(layer)->Length();
    int genlay = geosvc->Layer(layer)->Gen();
    double East_lay_X = geosvc->GeneralLayer(genlay)->SxEast();
    double East_lay_Y = geosvc->GeneralLayer(genlay)->SyEast();
    double East_lay_Z = geosvc->GeneralLayer(genlay)->SzEast();
    double West_lay_X = geosvc->GeneralLayer(genlay)->SxWest();
    double West_lay_Y = geosvc->GeneralLayer(genlay)->SyWest();
    double West_lay_Z = geosvc->GeneralLayer(genlay)->SzWest();

    HepPoint3D East_origin(East_lay_X, East_lay_Y, East_lay_Z);
    HepPoint3D West_origin(West_lay_X, West_lay_Y, West_lay_Z); 
    Hep3Vector wire = (CLHEP::Hep3Vector)East_origin - (CLHEP::Hep3Vector)West_origin;
    HepPoint3D piovt_z =(z*10+length/2 )/length * wire + West_origin;
    piovt_z = piovt_z*0.1; // conversion of the units(mm=>cm)


    //-------------------------------temp -------------------------------//
    HepPoint3D piv(hel.pivot());
    //-------------------------------temp -------------------------------//

    double dr0   = hel.a()[0];
    double phi0  = hel.a()[1];
    double kappa = hel.a()[2];
    double dz0   = hel.a()[3];
    double tanl  = hel.a()[4];

    // Choose the local field !!
    double Bz = 1000*(m_pIMF->getReferField());
    double ALPHA_loc = 1000/(2.99792458*Bz);

    // Choose the local field !!
    //double Bz = 1.0; // will be obtained from magnetic field service
    //double ALPHA_loc = 1000/(2.99792458*Bz);
    //double ALPHA_loc = 333.564095;
    int charge = ( kappa >= 0 )? 1 : -1;
    double rho = ALPHA_loc/kappa;
    double pt = fabs( 1.0/kappa );
    double lambda = atan( tanl );
    double theta = M_PI_2- lambda;
    double sintheta = sin(M_PI_2-atan(tanl));
    //  theta = 2.0*M_PI*theta/360.;
    double phi = fmod(phi0 + M_PI*4, M_PI*2);
    double csf0 = cos(phi);
    double snf0 = (1. - csf0) * (1. + csf0);
    snf0 = sqrt((snf0 > 0.) ? snf0 : 0.);
    if(phi > M_PI) snf0 = - snf0;
    //if(ntpFlag>0)
    //cout<<"rho = "<<rho<<"           hel.dr() + rho = "<<hel.dr() + rho<<endl; 

    //-------------------------------temp -------------------------------//
    double x_c = piv.x() + ( hel.dr() + rho )*csf0;
    double y_c = piv.y() + ( hel.dr() + rho )*snf0;
    double z_c = piv.z() + hel.dz();
    HepPoint3D ccenter(x_c, y_c, 0.0);
    double m_c_perp(ccenter.perp());
    Hep3Vector m_c_unit((HepPoint3D)ccenter.unit());
    //-------------------------------temp -------------------------------//

    ////change to boost coordinate system   
    double x_c_boost = x_c - piovt_z.x();
    double y_c_boost = y_c - piovt_z.y();
    double z_c_boost = z_c - piovt_z.z();
    HepPoint3D ccenter_boost(x_c_boost, y_c_boost, 0.0);
    double m_c_perp_boost(ccenter_boost.perp());
    //if(ntpFlag>0)
    //cout<<" ccenter = "<<ccenter<<"         m_c_perp ="<<m_c_perp<<endl;
    Hep3Vector m_c_unit_boost((HepPoint3D)ccenter_boost.unit());

    //phi region estimation
    double phi_io[2];
    HepPoint3D IO = (-1)*charge*m_c_unit;
    double dphi0 = fmod( IO.phi()+4*M_PI,2*M_PI ) - phi;
    double IO_phi = fmod( IO.phi()+4*M_PI,2*M_PI );
    //double dphi0_bak = IO_phi - phi;
    //if(dphi0 != 0)
    //cout<<"dphi value is : "<<dphi0<<"     phi:"<<phi<<"     IO.phi:"<<IO_phi<<endl;
    if( dphi0 > M_PI ) dphi0 -= 2*M_PI;
    else if( dphi0 < -M_PI ) dphi0 += 2*M_PI;
    //cout<<"dphi value is : "<<dphi0<<"     phi:"<<phi<<"     IO.phi:"<<IO_phi<<endl;
    //cout<<"charge is = "<<charge<<endl;
    phi_io[0] = -(1+charge)*M_PI_2 - charge*dphi0;
    //phi_io[0] = -(1+charge)*M_PI_2 + charge*dphi0;
    phi_io[1] = phi_io[0]+1.5*M_PI;
    //cout<<"phi_io[0] is : "<<phi_io[0]<<"           phi_io[1]:"<<phi_io[1]<<endl;
    double m_crio[2];
    double m_zb, m_zf, Calpha;

    // retrieve Mdc  geometry information
    double rcsiz1= geosvc->Layer(layer)->RCSiz1();
    double rcsiz2= geosvc->Layer(layer)->RCSiz2();
    double rlay= geosvc->Layer(layer)->Radius();
    int ncell= geosvc->Layer(layer)->NCell();
    double phioffset= geosvc->Layer(layer)->Offset();
    float shift= geosvc->Layer(layer)->Shift();
    double slant= geosvc->Layer(layer)->Slant();
    //double length = geosvc->Layer(layer)->Length();
    int type = geosvc->Layer(layer)->Sup()->Type();

    ///***********-------------------***************------------------****************//
    //check the value if same //
    ///***********-------------------***************------------------****************// 
    int w0id = geosvc->Layer(layer)->Wirst();
    int wid = w0id + cellid;
    HepPoint3D backkward = geosvc->Wire(wid)->BWirePos(); 
    HepPoint3D forward = geosvc->Wire(wid)->FWirePos();
    double x_lay_backward = geosvc->Wire(layer, cellid)->Backward().x();
    double y_lay_backward = geosvc->Wire(layer, cellid)->Backward().y();
    double x_lay_forward = geosvc->Wire(layer, cellid)->Forward().x();
    double y_lay_forward = geosvc->Wire(layer, cellid)->Forward().y();
    double r_lay_backward = sqrt(x_lay_backward*x_lay_backward+y_lay_backward*y_lay_backward);
    double r_lay_forward = sqrt(x_lay_forward*x_lay_forward+y_lay_forward*y_lay_forward);
    double r_lay_use = ((z*10+length/2)/length)*(r_lay_backward-r_lay_forward) + r_lay_forward;
    /*for(int i=0; i<43; i++){  
      double r_up= geosvc->Layer(i)->RCSiz1(); 
      double r_down= geosvc->Layer(i)->RCSiz2(); 
      cout<<i<<"       "<<r_up<<"        "<<r_down<<endl; 
      }*/ 
    //  shift = shift*type;
    //  cout<< "type "<< type <<endl;
    r_lay_use = 0.1*r_lay_use; 
    rcsiz1 = 0.1*rcsiz1;
    rcsiz2 = 0.1*rcsiz2;
    rlay = 0.1*rlay;
    length = 0.1*length; 
    m_zb = 0.5*length;
    m_zf = -0.5*length;
    m_crio[0] = rlay - rcsiz1;
    m_crio[1] = rlay + rcsiz2;
    //conversion of the units(mm=>cm)
    int sign = -1;  ///assumed the first half circle
    int epflag[2];
    Hep3Vector iocand[2];
    Hep3Vector cell_IO[2];
    double dphi, downin;
    Hep3Vector zvector;
    //if (ntpFlag>0) cout<<"z = "<<z<<" , m_zb = "<<m_zb<<" , m_zf = "<<m_zf<<endl;
    if( type ) {
        downin = (z*z-m_zb*m_zb)*pow(tan(slant),2);
        m_crio[0] = sqrt(m_crio[0]*m_crio[0]+downin);
        m_crio[1] = sqrt(m_crio[1]*m_crio[1]+downin);
    }

    //int stced[2];

    for( int i = 0; i < 2; i++ ) {
        double cos_alpha = m_c_perp_boost*m_c_perp_boost + m_crio[i]*m_crio[i] - rho*rho;
        cos_alpha = 0.5*cos_alpha/( m_c_perp_boost*m_crio[i] );
        if(fabs(cos_alpha)>1&&i==0) return(-1.0);
        if(fabs(cos_alpha)>1&&i==1) {
            cos_alpha = m_c_perp_boost*m_c_perp_boost + m_crio[0]*m_crio[0] - rho*rho;
            cos_alpha = 0.5*cos_alpha/( m_c_perp_boost*m_crio[0] );
            Calpha = 2.0*M_PI-acos( cos_alpha );
        } else {
            Calpha =  acos( cos_alpha );
        }
        epflag[i] = 0;
        iocand[i] = m_c_unit_boost;
        iocand[i].rotateZ( charge*sign*Calpha );
        iocand[i]*= m_crio[i];
        //if (ntpFlag>0) cout<<"iocand corridate is : "<<iocand[i]<<endl;

        ///***********-------------------***************------------------****************//
        //compare with standard coordinate system results //
        ///***********-------------------***************------------------****************//    
        //-------------------------------temp-------------------------//
        iocand[i] = iocand[i]+ piovt_z; //change to standard coordinate system      
        //iocand[i].y() = iocand[i].y() + piovt_z.y();
        //if (ntpFlag>0) cout<<i<<setw(10)<<iocand[i].x()<<setw(10)<<iocand[i].y()<<endl;
        //------------------------------temp -------------------------//    

        double xx = iocand[i].x() - x_c;
        double yy = iocand[i].y() - y_c;
        //dphi = atan2(yy, xx) - phi0 - M_PI_2*(1+charge);
        dphi = atan2(yy, xx) - phi0 - M_PI_2*(1-charge);
        dphi = fmod( dphi + 8.0*M_PI, 2*M_PI );

        if( dphi < phi_io[0] ) {
            dphi += 2*M_PI;
        }
        else if( phi_io[1] < dphi ) {
            dphi -= 2*M_PI;
        }
        ///in the Local Helix case, phi must be small

        //Hep3Vector zvector( 0., 0., z_c-rho*dphi*tanl);
        Hep3Vector zvector( 0., 0., z_c-rho*dphi*tanl-piovt_z.z());
        //if (ntpFlag>0) cout<<"z_c-rho*dphi*tanl : "<<z_c-rho*dphi*tanl<<endl;
        cell_IO[i] = iocand[i];
        cell_IO[i] += zvector;
        //---------------------------------temp ---------------------------------//
        //cell_IO[i] = hel.x(dphi);//compare with above results 
        //---------------------------------temp ---------------------------------//     

        double xcio, ycio, phip;
        ///// z region check active volume is between zb+2. and zf-2. [cm]
        /*
           float delrin = 2.0;
           if( m_zf-delrin > zvector.z() ){
           phip = z_c - m_zb + delrin;
           phip = phip/( rho*tanl );
           phip = phip + phi0;
           xcio = x_c - rho*cos( phip );
           ycio = y_c - rho*sin( phip );
           cell_IO[i].setX( xcio );
           cell_IO[i].setY( ycio );
           cell_IO[i].setZ( m_zb+delrin );
           epflag[i] = 1;
           }
           else if( m_zb+delrin < zvector.z() ){
           phip = z_c - m_zf-delrin;
           phip = phip/( rho*tanl );
           phip = phip + phi0;
           xcio = x_c - rho*cos( phip );
           ycio = y_c - rho*sin( phip );
           cell_IO[i].setX( xcio );
           cell_IO[i].setY( ycio );
           cell_IO[i].setZ( m_zf-delrin );
           epflag[i] = 1;
           }
           else{
         */
        //      cell_IO[i] = iocand;
        //      cell_IO[i] += zvector;
        //    }
        //dphi = dphi -M_PI;  
        cell_IO[i] = hel.x(dphi);
        //if (ntpFlag>0)  { cout<<"cell_IO corridate : "<<cell_IO[i]<<endl;}
        // if(i==0) cout<<"zhit value is = "<<z<<endl; 
    }

    // path length estimation
    // phi calculation
    Hep3Vector cl = cell_IO[1] - cell_IO[0];

    //float ch_tphi, ch_tdphi;
    double ch_theta;
    double ch_dphi;
    double ch_ltrk = 0;
    double ch_ltrk_rp = 0;
    ch_dphi = cl.perp()*0.5/(ALPHA_loc*pt);
    ch_dphi = 2.0 * asin( ch_dphi );
    ch_ltrk_rp = ch_dphi*ALPHA_loc*pt;
    double rpi_path = sqrt(cl.x()*cl.x()+cl.y()*cl.y());
    ch_ltrk = sqrt( ch_ltrk_rp*ch_ltrk_rp + cl.z()*cl.z() );
    double path = ch_ltrk_rp/ sintheta;
    ch_theta = cl.theta();
    /*   if (ntpFlag>0)
         { 
    // if((ch_ltrk_rp-rpi_path)>0.001 || (ch_ltrk-path)>0.001)
    cout<<"ch_ltrk_rp : "<<ch_ltrk_rp<<"       rpi_path: "<<rpi_path<<endl;
    cout<<"ch_ltrk : "<<ch_ltrk<<"       path:"<<path<<endl; 
    }
     */
    /*
       if( ch_theta < theta*0.85 || 1.15*theta < ch_theta)
       ch_ltrk *= -1; /// miss calculation

       if( epflag[0] == 1 || epflag [1] == 1 )
       ch_ltrk *= -1;  /// invalid region for dE/dx or outside of Mdc
     */
    // judge how many cells are traversed and deal with different situations
    double phibin, phi_in, phi_out, inlow, inup, outlow, outup, gap, phi1, phi2, phi_mid, phi_midin, phi_midout;
    int cid_in, cid_out;
    double inlow_z, inup_z, outlow_z, outup_z, gap_z, phi1_z, phi2_z, phi_mid_z, phi_midin_z, phi_midout_z;
    //cache sampl in each cell for this layer

    std::vector<double> sampl;  
    sampl.resize(ncell);
    for(int k=0; k<ncell; k++) {
        sampl[k] = -1.0;
    }

    cid_in = cid_out = -1;
    phi_in = cell_IO[0].phi();
    phi_out = cell_IO[1].phi();
    //phi_in = iocand[0].phi();
    //phi_out = iocand[1].phi();
    phi_in = fmod( phi_in+4*M_PI,2*M_PI );
    phi_out = fmod( phi_out+4*M_PI,2*M_PI );
    phibin = 2.0*M_PI/ncell;
    //  gap = fabs(phi_out-phi_in);

    //determine the in/out cell id
    Hep3Vector cell_mid=0.5*(cell_IO[0]+cell_IO[1]);
    //Hep3Vector cell_mid=0.5*(iocand[0]+iocand[0]);
    //cout<<cell_mid<<endl;
    //double stereophi = shift*phibin*(0.5-cell_mid.z()/length);
    //phioffset = phioffset+stereophi;
    double stphi[2], phioff[2];
    stphi[0] = shift*phibin*(0.5-cell_IO[0].z()/length);
    stphi[1] = shift*phibin*(0.5-cell_IO[1].z()/length);
    //stphi[0] = shift*phibin*(0.5-z/length);
    //stphi[1] = shift*phibin*(0.5-z/length);
    phioff[0] = phioffset+stphi[0];
    phioff[1] = phioffset+stphi[1];

    for(int i=0; i<ncell; i++) {

        double x_lay_backward_cell = geosvc->Wire(layer, i)->Backward().x()*0.1;
        double y_lay_backward_cell = geosvc->Wire(layer, i)->Backward().y()*0.1;
        double x_lay_forward_cell = geosvc->Wire(layer, i)->Forward().x()*0.1;
        double y_lay_forward_cell = geosvc->Wire(layer, i)->Forward().y()*0.1;
        //if(ntpFlag>0) cout<<layer<<setw(10)<<i<<setw(10)<<x_lay_forward<<setw(10)<<y_lay_forward<<setw(10)<<x_lay_backward<<setw(10)<<y_lay_backward<<setw(10)<<r_lay_forward<<setw(10)<<endl;
        //Hep3Vector lay_backward, lay_forward;  
        Hep3Vector lay_backward(x_lay_backward_cell, y_lay_backward_cell, 0);
        Hep3Vector lay_forward(x_lay_forward_cell, y_lay_forward_cell, 0);
        Hep3Vector Cell_z[2];
        Cell_z[0] = ((cell_IO[0].z()+length/2)/length)*(lay_backward - lay_forward) + lay_forward;
        Cell_z[1] = ((cell_IO[1].z()+length/2)/length)*(lay_backward - lay_forward) + lay_forward;
        double z_phi[2];
        z_phi[0] = Cell_z[0].phi();
        z_phi[1] = Cell_z[1].phi();
        z_phi[0] = fmod( z_phi[0]+4*M_PI,2*M_PI );
        z_phi[1] = fmod( z_phi[1]+4*M_PI,2*M_PI );
        //double backward_phi = lay_backward.phi();
        //double forward_phi = lay_forward.phi();
        //backward_phi = fmod( backward_phi+4*M_PI,2*M_PI );
        //forward_phi = fmod( forward_phi+4*M_PI,2*M_PI );
        //if(ntpFlag>0) cout<<"backward_phi cal : "<<atan2(y_lay_backward,x_lay_backward)<<"       forward_phi : "<<atan2(y_lay_forward,x_lay_forward)<<endl;
        //if(ntpFlag>0) cout<<layer<<"       backward_phi : "<<backward_phi<<"       forward_phi : "<<forward_phi<<endl;

        //double z_phi[2];
        //z_phi[0] = ((cell_IO[0].z()+length/2)/length)*(backward_phi - forward_phi) + forward_phi; 
        //z_phi[1] = ((cell_IO[1].z()+length/2)/length)*(backward_phi - forward_phi) + forward_phi;
        //if(ntpFlag>0) cout<<"phi z : "<<z_phi[0]<<"          "<<z_phi[1]<<endl;
        inlow_z = z_phi[0] - phibin*0.5;
        inup_z = z_phi[0] + phibin*0.5;
        outlow_z = z_phi[1] - phibin*0.5;
        outup_z = z_phi[1] + phibin*0.5;
        inlow_z = fmod( inlow_z+4*M_PI,2*M_PI );
        inup_z = fmod( inup_z+4*M_PI,2*M_PI );
        outlow_z = fmod( outlow_z+4*M_PI,2*M_PI );
        outup_z = fmod( outup_z+4*M_PI,2*M_PI );


        // for stereo layer
        inlow = phioff[0]+phibin*i-phibin*0.5;
        inup = phioff[0]+phibin*i+phibin*0.5;
        outlow = phioff[1]+phibin*i-phibin*0.5;
        outup = phioff[1]+phibin*i+phibin*0.5;
        inlow = fmod( inlow+4*M_PI,2*M_PI );
        inup = fmod( inup+4*M_PI,2*M_PI );
        outlow = fmod( outlow+4*M_PI,2*M_PI );
        outup = fmod( outup+4*M_PI,2*M_PI );

#ifdef YDEBUG
        if(ntpFlag > 0)  cout << "shift " << shift 
          <<" phi_in " << phi_in << " phi_out " << phi_out 
              << " inup "<< inup << " inlow " << inlow 
              << " outup "<< outup << " outlow " << outlow << endl;
#endif

        /*if(phi_in>=inlow&&phi_in<inup) cid_in = i;
          if(phi_out>=outlow&&phi_out<outup) cid_out = i;
          if ( inlow>inup) {
          if((phi_in>=inlow&&phi_in<2.0*M_PI)||(phi_in>=0.0&&phi_in<inup)) cid_in = i;
          } 
          if ( outlow>outup) {
          if((phi_out>=outlow&&phi_out<2.0*M_PI)||(phi_out>=0.0&&phi_out<outup)) cid_out = i;
          }*/

        if(phi_in>=inlow_z&&phi_in<inup_z) cid_in = i;
        if(phi_out>=outlow_z&&phi_out<outup_z) cid_out = i;
        if ( inlow_z>inup_z) {
            if((phi_in>=inlow_z&&phi_in<2.0*M_PI)||(phi_in>=0.0&&phi_in<inup_z)) cid_in = i;
        }
        if ( outlow_z>outup_z) {
            if((phi_out>=outlow_z&&phi_out<2.0*M_PI)||(phi_out>=0.0&&phi_out<outup_z)) cid_out = i;
        }
    }

    phi_midin = phi_midout = phi1 = phi2 = -999.0;
    gap = -999.0; 
    //only one cell traversed
    //if(ntpFlag > 0) cout<<"cid_in =  "<<cid_in<<"        cid_out = "<<cid_out<<endl;
    if(cid_in == -1 || cid_out == -1) return -1;

    if( cid_in == cid_out) {
        sampl[cid_in]= ch_ltrk;
        //return ch_ltrk;
        return sampl[cellid];
    } else  if(cid_in < cid_out) {
        //in cell id less than out cell id
        //deal with the special case crossing x axis
        if( cid_out-cid_in>ncell/2  ) {

            //      if(gap>=M_PI) gap = 2.0*M_PI-gap;
            double x_lay_backward_cin = geosvc->Wire(layer, cid_in)->Backward().x()*0.1;
            double y_lay_backward_cin = geosvc->Wire(layer, cid_in)->Backward().y()*0.1;
            double x_lay_forward_cin = geosvc->Wire(layer, cid_in)->Forward().x()*0.1;
            double y_lay_forward_cin = geosvc->Wire(layer, cid_in)->Forward().y()*0.1;
            Hep3Vector lay_backward_cin(x_lay_backward_cin, y_lay_backward_cin, 0);
            Hep3Vector lay_forward_cin(x_lay_forward_cin, y_lay_forward_cin, 0);
            Hep3Vector Cell_z[2];
            Cell_z[0]=((cell_IO[0].z()+length/2)/length)*(lay_backward_cin-lay_forward_cin)+lay_forward_cin;
            double phi_cin_z = Cell_z[0].phi();
            phi_cin_z = fmod( phi_cin_z+4*M_PI,2*M_PI );
            double x_lay_backward_cout = geosvc->Wire(layer, cid_out)->Backward().x()*0.1;
            double y_lay_backward_cout = geosvc->Wire(layer, cid_out)->Backward().y()*0.1;
            double x_lay_forward_cout = geosvc->Wire(layer, cid_out)->Forward().x()*0.1;
            double y_lay_forward_cout = geosvc->Wire(layer, cid_out)->Forward().y()*0.1;
            Hep3Vector lay_backward_cout(x_lay_backward_cout, y_lay_backward_cout, 0);
            Hep3Vector lay_forward_cout(x_lay_forward_cout, y_lay_forward_cout, 0);
            Cell_z[1]=((cell_IO[1].z()+length/2)/length)*(lay_backward_cout-lay_forward_cout)+lay_forward_cout;
            double phi_cout_z = Cell_z[1].phi();
            phi_cout_z = fmod( phi_cout_z+4*M_PI,2*M_PI );

            phi_midin_z = phi_cin_z-phibin*0.5;
            phi_midout_z = phi_cout_z+phibin*0.5;
            phi_midin_z = fmod( phi_midin_z+4*M_PI,2*M_PI );
            phi_midout_z = fmod( phi_midout_z+4*M_PI,2*M_PI );
            phi1_z = phi_midout_z-phi_out;
            phi2_z = phi_in-phi_midin_z;
            phi1_z = fmod(phi1_z+2.0*M_PI,2.0*M_PI);
            phi2_z = fmod(phi2_z+2.0*M_PI,2.0*M_PI);
            gap_z = phi1_z+phi2_z+(ncell-1-cid_out+cid_in)*phibin;
            gap_z = fmod(gap_z+2.0*M_PI,2.0*M_PI);
            sampl[cid_in]=phi2_z/gap_z*ch_ltrk;
            sampl[cid_out]=phi1_z/gap_z*ch_ltrk;
            for( int jj = cid_out+1; jj<ncell; jj++) {
                sampl[jj]=phibin/gap_z*ch_ltrk;
            }
            for( int jj = 0; jj<cid_in; jj++) {
                sampl[jj]=phibin/gap_z*ch_ltrk;
            }

            /*
            //      if(gap>=M_PI) gap = 2.0*M_PI-gap;
            phi_midin = phioff[0]+phibin*cid_in-phibin*0.5;
            phi_midout = phioff[1]+phibin*cid_out+phibin*0.5;
            phi_midin = fmod( phi_midin+4*M_PI,2*M_PI );
            phi_midout = fmod( phi_midout+4*M_PI,2*M_PI );
            phi1 = phi_midout-phi_out;
            phi2 = phi_in-phi_midin;
            phi1 = fmod(phi1+2.0*M_PI,2.0*M_PI);
            phi2 = fmod(phi2+2.0*M_PI,2.0*M_PI);
            gap = phi1+phi2+(ncell-1-cid_out+cid_in)*phibin;
            gap = fmod(gap+2.0*M_PI,2.0*M_PI);
            sampl[cid_in]=phi2/gap*ch_ltrk;
            sampl[cid_out]=phi1/gap*ch_ltrk;
            for( int jj = cid_out+1; jj<ncell; jj++) {
            sampl[jj]=phibin/gap*ch_ltrk;
            }
            for( int jj = 0; jj<cid_in; jj++) {
            sampl[jj]=phibin/gap*ch_ltrk;
            }*/
            /*
               cout<<"LLLLLLLLLLLLL"<<endl;
               cout<< "layerId  " << layer <<"  cell id  "<< cellid <<"  shift  " << shift 
               << "  cid_in  " << cid_in << "  cid_out  " << cid_out << endl;
               cout <<" phi_in " << phi_in <<" phi_midin " << phi_midin << " phi_out " 
               << phi_out << " phi_midout " << phi_midout <<endl;
               cout<<"total sampl " << ch_ltrk << " gap "<< gap << " phi1 " 
               << phi1 << " phi2 " << phi2 << " phibin " <<  phibin << endl; 
             */
        } else {
            //normal case
            double x_lay_backward_cin = geosvc->Wire(layer, cid_in)->Backward().x()*0.1;
            double y_lay_backward_cin = geosvc->Wire(layer, cid_in)->Backward().y()*0.1;
            double x_lay_forward_cin = geosvc->Wire(layer, cid_in)->Forward().x()*0.1;
            double y_lay_forward_cin = geosvc->Wire(layer, cid_in)->Forward().y()*0.1;
            Hep3Vector lay_backward_cin(x_lay_backward_cin, y_lay_backward_cin, 0);
            Hep3Vector lay_forward_cin(x_lay_forward_cin, y_lay_forward_cin, 0);
            Hep3Vector Cell_z[2];
            Cell_z[0]=((cell_IO[0].z()+length/2)/length)*(lay_backward_cin-lay_forward_cin)+lay_forward_cin;
            double phi_cin_z = Cell_z[0].phi();
            phi_cin_z = fmod( phi_cin_z+4*M_PI,2*M_PI );
            double x_lay_backward_cout = geosvc->Wire(layer, cid_out)->Backward().x()*0.1;
            double y_lay_backward_cout = geosvc->Wire(layer, cid_out)->Backward().y()*0.1;
            double x_lay_forward_cout = geosvc->Wire(layer, cid_out)->Forward().x()*0.1;
            double y_lay_forward_cout = geosvc->Wire(layer, cid_out)->Forward().y()*0.1;
            Hep3Vector lay_backward_cout(x_lay_backward_cout, y_lay_backward_cout, 0);
            Hep3Vector lay_forward_cout(x_lay_forward_cout, y_lay_forward_cout, 0);
            Cell_z[1]=((cell_IO[1].z()+length/2)/length)*(lay_backward_cout-lay_forward_cout)+lay_forward_cout;
            double phi_cout_z = Cell_z[1].phi();
            phi_cout_z = fmod( phi_cout_z+4*M_PI,2*M_PI );

            phi_midin_z = phi_cin_z+phibin*0.5;
            phi_midout_z = phi_cout_z-phibin*0.5;
            phi_midin_z = fmod( phi_midin_z+4*M_PI,2*M_PI );
            phi_midout_z = fmod( phi_midout_z+4*M_PI,2*M_PI );
            phi1_z = phi_midin_z-phi_in;
            phi2_z = phi_out-phi_midout_z;
            phi1_z = fmod(phi1_z+2.0*M_PI,2.0*M_PI);
            phi2_z = fmod(phi2_z+2.0*M_PI,2.0*M_PI);
            gap_z = phi1_z+phi2_z+(cid_out-cid_in-1)*phibin;
            gap_z = fmod(gap_z+2.0*M_PI,2.0*M_PI);
            sampl[cid_in]=phi1_z/gap_z*ch_ltrk;
            sampl[cid_out]=phi2_z/gap_z*ch_ltrk;
            for( int jj = cid_in+1; jj<cid_out; jj++) {
                sampl[jj]=phibin/gap_z*ch_ltrk;
            }
            //normal case
            /*phi_midin = phioff[0]+phibin*cid_in+phibin*0.5;
              phi_midout = phioff[1]+phibin*cid_out-phibin*0.5;
              phi_midin = fmod( phi_midin+4*M_PI,2*M_PI );
              phi_midout = fmod( phi_midout+4*M_PI,2*M_PI );
              phi1 = phi_midin-phi_in;
              phi2 = phi_out-phi_midout;
              phi1 = fmod(phi1+2.0*M_PI,2.0*M_PI);
              phi2 = fmod(phi2+2.0*M_PI,2.0*M_PI);
              gap = phi1+phi2+(cid_out-cid_in-1)*phibin;
              gap = fmod(gap+2.0*M_PI,2.0*M_PI);
              sampl[cid_in]=phi1/gap*ch_ltrk;
              sampl[cid_out]=phi2/gap*ch_ltrk;
              for( int jj = cid_in+1; jj<cid_out; jj++) {
              sampl[jj]=phibin/gap*ch_ltrk;
              }*/
        }

    } else  if(cid_in > cid_out) {
        //in cell id greater than out cell id
        //deal with the special case crossing x axis
        if( cid_in-cid_out>ncell/2  ) {
            double x_lay_backward_cin = geosvc->Wire(layer, cid_in)->Backward().x()*0.1;
            double y_lay_backward_cin = geosvc->Wire(layer, cid_in)->Backward().y()*0.1;
            double x_lay_forward_cin = geosvc->Wire(layer, cid_in)->Forward().x()*0.1;
            double y_lay_forward_cin = geosvc->Wire(layer, cid_in)->Forward().y()*0.1;
            Hep3Vector lay_backward_cin(x_lay_backward_cin, y_lay_backward_cin, 0);
            Hep3Vector lay_forward_cin(x_lay_forward_cin, y_lay_forward_cin, 0);
            Hep3Vector Cell_z[2];
            Cell_z[0]=((cell_IO[0].z()+length/2)/length)*(lay_backward_cin-lay_forward_cin)+lay_forward_cin;
            double phi_cin_z = Cell_z[0].phi();
            phi_cin_z = fmod( phi_cin_z+4*M_PI,2*M_PI );
            double x_lay_backward_cout = geosvc->Wire(layer, cid_out)->Backward().x()*0.1;
            double y_lay_backward_cout = geosvc->Wire(layer, cid_out)->Backward().y()*0.1;
            double x_lay_forward_cout = geosvc->Wire(layer, cid_out)->Forward().x()*0.1;
            double y_lay_forward_cout = geosvc->Wire(layer, cid_out)->Forward().y()*0.1;
            Hep3Vector lay_backward_cout(x_lay_backward_cout, y_lay_backward_cout, 0);
            Hep3Vector lay_forward_cout(x_lay_forward_cout, y_lay_forward_cout, 0);
            Cell_z[1]=((cell_IO[1].z()+length/2)/length)*(lay_backward_cout-lay_forward_cout)+lay_forward_cout;
            double phi_cout_z = Cell_z[1].phi();
            phi_cout_z = fmod( phi_cout_z+4*M_PI,2*M_PI );

            phi_midin_z = phi_cin_z+phibin*0.5;
            phi_midout_z = phi_cout_z-phibin*0.5;
            phi_midin_z = fmod( phi_midin_z+4*M_PI,2*M_PI );
            phi_midout_z = fmod( phi_midout_z+4*M_PI,2*M_PI );
            phi1_z = phi_midin_z-phi_in;
            phi2_z = phi_out-phi_midout_z;
            phi1_z = fmod(phi1_z+2.0*M_PI,2.0*M_PI);
            phi2_z = fmod(phi2_z+2.0*M_PI,2.0*M_PI);
            gap_z = phi1_z+phi2_z+(ncell-1-cid_in+cid_out)*phibin;
            gap_z = fmod(gap_z+2.0*M_PI,2.0*M_PI);
            sampl[cid_out]=phi2_z/gap_z*ch_ltrk;
            sampl[cid_in]=phi1_z/gap_z*ch_ltrk;
            for( int jj = cid_in+1; jj<ncell; jj++) {
                sampl[jj]=phibin/gap_z*ch_ltrk;
            }
            for( int jj = 0; jj<cid_out; jj++) {
                sampl[jj]=phibin/gap_z*ch_ltrk;
            }

            //      if(gap>=M_PI) gap = 2.0*M_PI-gap;
            /*phi_midin = phioff[0]+phibin*cid_in+phibin*0.5;
              phi_midout = phioff[1]+phibin*cid_out-phibin*0.5;
              phi_midin = fmod( phi_midin+4*M_PI,2*M_PI );
              phi_midout = fmod( phi_midout+4*M_PI,2*M_PI );
              phi1 = phi_midin-phi_in;
              phi2 = phi_out-phi_midout;
              phi1 = fmod(phi1+2.0*M_PI,2.0*M_PI);
              phi2 = fmod(phi2+2.0*M_PI,2.0*M_PI);
              gap = phi1+phi2+(ncell-1-cid_in+cid_out)*phibin;
              gap = fmod(gap+2.0*M_PI,2.0*M_PI);
              sampl[cid_out]=phi2/gap*ch_ltrk;
              sampl[cid_in]=phi1/gap*ch_ltrk;
              for( int jj = cid_in+1; jj<ncell; jj++) {
              sampl[jj]=phibin/gap*ch_ltrk;
              }
              for( int jj = 0; jj<cid_out; jj++) {
              sampl[jj]=phibin/gap*ch_ltrk;
              }*/
            /*
               cout<<"LLLLLLLLLLLLL"<<endl;
               cout<< "layerId  " << layer <<"  cell id  "<< cellid <<"  shift  " << shift 
               << "  cid_in  " << cid_in << "  cid_out  " << cid_out << endl;
               cout <<" phi_in " << phi_in <<" phi_midin " << phi_midin << " phi_out " 
               << phi_out << " phi_midout " << phi_midout <<endl;
               cout<<"total sampl " << ch_ltrk << " gap "<< gap << " phi1 " 
               << phi1 << " phi2 " << phi2 << " phibin " <<  phibin << endl; 
             */
        } else {
            double x_lay_backward_cin = geosvc->Wire(layer, cid_in)->Backward().x()*0.1;
            double y_lay_backward_cin = geosvc->Wire(layer, cid_in)->Backward().y()*0.1;
            double x_lay_forward_cin = geosvc->Wire(layer, cid_in)->Forward().x()*0.1;
            double y_lay_forward_cin = geosvc->Wire(layer, cid_in)->Forward().y()*0.1;
            Hep3Vector lay_backward_cin(x_lay_backward_cin, y_lay_backward_cin, 0);
            Hep3Vector lay_forward_cin(x_lay_forward_cin, y_lay_forward_cin, 0);
            Hep3Vector Cell_z[2];
            Cell_z[0]=((cell_IO[0].z()+length/2)/length)*(lay_backward_cin-lay_forward_cin)+lay_forward_cin;
            double phi_cin_z = Cell_z[0].phi();
            phi_cin_z = fmod( phi_cin_z+4*M_PI,2*M_PI );
            double x_lay_backward_cout = geosvc->Wire(layer, cid_out)->Backward().x()*0.1;
            double y_lay_backward_cout = geosvc->Wire(layer, cid_out)->Backward().y()*0.1;
            double x_lay_forward_cout = geosvc->Wire(layer, cid_out)->Forward().x()*0.1;
            double y_lay_forward_cout = geosvc->Wire(layer, cid_out)->Forward().y()*0.1;
            Hep3Vector lay_backward_cout(x_lay_backward_cout, y_lay_backward_cout, 0);
            Hep3Vector lay_forward_cout(x_lay_forward_cout, y_lay_forward_cout, 0);
            Cell_z[1]=((cell_IO[1].z()+length/2)/length)*(lay_backward_cout-lay_forward_cout)+lay_forward_cout;
            double phi_cout_z = Cell_z[1].phi();
            phi_cout_z = fmod( phi_cout_z+4*M_PI,2*M_PI );

            phi_midin_z = phi_cin_z-phibin*0.5;
            phi_midout_z = phi_cout_z+phibin*0.5;
            phi_midin_z = fmod( phi_midin_z+4*M_PI,2*M_PI );
            phi_midout_z = fmod( phi_midout_z+4*M_PI,2*M_PI );
            phi1_z = phi_midout_z-phi_out;
            phi2_z = phi_in-phi_midin_z;
            phi1_z = fmod(phi1_z+2.0*M_PI,2.0*M_PI);
            phi2_z = fmod(phi2_z+2.0*M_PI,2.0*M_PI);
            gap_z = phi1_z+phi2_z+(cid_in-cid_out-1)*phibin;
            gap_z = fmod(gap_z+2.0*M_PI,2.0*M_PI);
            sampl[cid_in]=phi2_z/gap_z*ch_ltrk;
            sampl[cid_out]=phi1_z/gap_z*ch_ltrk;
            for( int jj = cid_out+1; jj<cid_in; jj++) {
                sampl[jj]=phibin/gap_z*ch_ltrk;
            }

            //normal case
            /*phi_midin = phioff[0]+phibin*cid_in-phibin*0.5;
              phi_midout = phioff[1]+phibin*cid_out+phibin*0.5;
              phi_midin = fmod( phi_midin+4*M_PI,2*M_PI );
              phi_midout = fmod( phi_midout+4*M_PI,2*M_PI );
              phi1 = phi_midout-phi_out;
              phi2 = phi_in-phi_midin;
              phi1 = fmod(phi1+2.0*M_PI,2.0*M_PI);
              phi2 = fmod(phi2+2.0*M_PI,2.0*M_PI);
              gap = phi1+phi2+(cid_in-cid_out-1)*phibin;
              gap = fmod(gap+2.0*M_PI,2.0*M_PI);
              sampl[cid_in]=phi2/gap*ch_ltrk;
              sampl[cid_out]=phi1/gap*ch_ltrk;
              for( int jj = cid_out+1; jj<cid_in; jj++) {
              sampl[jj]=phibin/gap*ch_ltrk;
              }*/
        }
    }

#ifdef YDEBUG
    if(sampl[cellid]<0.0) {
        if(cid_in!=cid_out) cout<<"?????????"<<endl;
        cout<< "layerId  " << layer <<"  cell id  "<< cellid <<"  shift  " << shift 
            << "  cid_in  " << cid_in << "  cid_out  " << cid_out << endl;

        cout <<" phi_in " << phi_in <<" phi_midin " << phi_midin << " phi_out " 
            << phi_out << " phi_midout " << phi_midout <<endl;
        cout<<"total sampl " << ch_ltrk << " gap "<< gap << " phi1 " 
            << phi1 << " phi2 " << phi2 << " phibin " <<  phibin << endl; 


        for(int l=0; l<ncell; l++) {
            if(sampl[l]>=0.0) 
              cout<<"picked cellid  " << l << "  sampling length  "<< sampl[l]<< endl;
        }
    }
#endif
    return sampl[cellid];
}




// function to convert measured ionization (D) to actural ionization (I)
double DedxCorrecSvc::D2I(const double& cosTheta, const double& D) const
{
    //cout<<"DedxCorrecSvc::D2I"<<endl;
    double absCosTheta   = fabs(cosTheta);
    double projection    = pow(absCosTheta,m_power) + m_delta;   // Projected length on wire
    double chargeDensity = D/projection;
    double numerator     = 1 + m_alpha*chargeDensity;
    double denominator   = 1 + m_gamma*chargeDensity;
    double I             = D;

    //if(denominator > 0.1)

    I = D * m_ratio* numerator/denominator;
//    cout << "m_alpha " << m_alpha << endl;
//    cout << "m_gamma " << m_gamma << endl;
//    cout << "m_delta " << m_delta << endl;
//    cout << "m_power " << m_power << endl;
//    cout << "m_ratio " << m_ratio << endl;
    return I;
}

// Convert actural ionization (I) to measured ionization (D)
double DedxCorrecSvc::I2D(const double& cosTheta, const double& I) const
{
    // cout<<" DedxCorrecSvc::I2D"<<endl;
    double absCosTheta = fabs(cosTheta);
    double projection  = pow(absCosTheta,m_power) + m_delta;   // Projected length on wire

    // Do the quadratic to compute d of the electron
    double a =  m_alpha / projection;
    double b =  1 - m_gamma / projection*(I/m_ratio);
    double c = -I/m_ratio;

    if(b==0 && a==0){
        cout<<"both a and b coefficiants for hadron correction are 0"<<endl;
        return I;
    }

    double D = a != 0? (-b + sqrt(b*b - 4.0*a*c))/(2.0*a) : -c/b;
    if(D<0){	
        cout<<"D is less 0! will try another solution"<<endl;
        D=a != 0? (-b - sqrt(b*b + 4.0*a*c))/(2.0*a) : -c/b;
        if(D<0){
            cout<<"D is still less 0! just return uncorrectecd value"<<endl;
            return I;
        } 		
    }

    return D;
}


