//    BesIII MDC dE/dx Reconstruction Module
//    Class: MdcDedxRecon    
//    Created by Dayong WANG 2003/11

#include "stdio.h"
#include "math.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogramFactory.h"
#include "GaudiKernel/INTupleSvc.h"
#include "EventModel/EventHeader.h"
#include "ReconEvent/ReconEvent.h"   
#include "EvTimeEvent/RecEsTime.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcKalHelixSeg.h"
#include "MdcRecEvent/RecMdcDedx.h"
#include "MdcRecEvent/RecMdcDedxHit.h"
#include "TTree.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"

#include "MdcDedxAlg/MdcDedxRecon.h"
#include "MdcDedxAlg/MdcDedxCorrection.h"
#include "MdcDedxAlg/MdcDedxTrk.h"
#include "MdcDedxAlg/MdcDedxParam.h"


using namespace std;
using CLHEP::HepVector;

extern "C" {float prob_ (float *, int*);}

int RunNO1 = 0;
int RunNO2;

int eventNo = 0;
int trackNO1 = 0;
int trackNO2 = 0;
int trackNO3 = 0;
MdcDedxRecon::MdcDedxRecon(const std::string& name, ISvcLocator* pSvcLocator) :  Algorithm(name, pSvcLocator)
{
    ex_calib=0;  // get MdcDedxCorrection
    calib_flag = 0x7F; //all calib on
    landau = 1;  //0: gauss distribution;  1:landau distribution;
    ntpFlag = 1;
    doNewGlobal = 0;
    recalg = 0; //reconstruction method: 0:RecMdcTrack; 1:RecMdcKalTrack; 
    //2:use RecMdcTrack when no RecMdcKalTrack
    ParticleFlag = -1;  //e:0,  mu:1,  pi:2,  K:3,  p:4,  cosmic:5
    m_alg = 1;  //calculation method of dE/dx of one track; 1: truncation and get average;
    truncate = 0.70; // truncation rate

    // Declare the properties
    declareProperty("CalibFlag",calib_flag);
    declareProperty("NTupleFlag",ntpFlag);
    declareProperty("RecAlg",recalg);
    declareProperty("ParticleType",ParticleFlag);
    declareProperty("dEdxMethod",m_alg);
    declareProperty("TruncRate",truncate);
    declareProperty("RootFile",m_rootfile = std::string("no rootfile")); 
}


StatusCode  MdcDedxRecon::initialize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "in initialize()" << endreq;

    log << MSG::INFO <<"--------------------<  MdcDedxRecon  V2.1   >---------------------"<< endreq;
    log << MSG::INFO <<"#######  correction for the wire current dependence   #######"<< endreq;
    log << MSG::INFO <<"#######      correction for the new z dependence      #######"<< endreq;
    log << MSG::INFO <<"-------------------------------------------------------------"<< endreq;
    log << MSG::INFO <<"++++++++++++++++++++[  initialization  ]+++++++++++++++++++++"<< endreq;
    log << MSG::INFO << "MdcDedxRecon has been initialized with calib_flag: "<< calib_flag <<endreq;
    log << MSG::INFO << "MdcDedxRecon has been initialized with landau: "<< landau <<  endreq;
    if( landau == 0 ) {truncate = 0.7;}
    log << MSG::INFO << "MdcDedxRecon has been initialized with ntpFlag: "<< ntpFlag <<  endreq;
    log << MSG::INFO << "MdcDedxRecon has been initialized with recalg: "<< recalg <<  endreq;
    log << MSG::INFO << "MdcDedxRecon has been initialized with dE/dx cal method m_alg: "<< m_alg << endreq;
    log << MSG::INFO << "MdcDedxRecon has been initialized with truncate: "<< truncate <<endreq;
    log << MSG::INFO << "MdcDedxRecon has been initialized with doNewGlobal: "<<doNewGlobal<<endreq;
    log << MSG::DEBUG  << "+++++++++++ MdcDedxRecon initialization end ++++++++++++ "<< endreq;
    if( truncate <= 0.0 || 1.0 < truncate )
    {
        log << MSG::FATAL <<" Initialization ERROR of truncate  = "<<truncate<< endreq;
        log << MSG::FATAL <<" truncate must be within 0.0 to 1.0 ! "<< endreq;
        log << MSG::FATAL <<" Please stop exec. "<< endreq;
    }
    log << MSG::DEBUG <<"-------------------------------------------------------------"<< endreq;
    log << MSG::DEBUG <<"MdcDedxRecon init 2nd part!!!"<< endreq;

    StatusCode scex = service("DedxCorrecSvc", exsvc);
    if (scex ==  StatusCode::SUCCESS) 
    {
        log << MSG::INFO <<"Hi, DedxCorrectSvc is running"<<endreq;
    } 
    else 
    {
        log << MSG::ERROR <<"DedxCorrectSvc is failed"<<endreq;
        return StatusCode::SUCCESS;
    }
    exsvc->set_flag( calib_flag );

    StatusCode cursvc = service("DedxCurSvc", dedxcursvc);
    if (cursvc ==  StatusCode::SUCCESS) 
    {
        log << MSG::INFO <<"DedxCurSvc is running"<<endreq;
    } 
    else 
    {
        log << MSG::ERROR <<"DedxCurSvc is failed"<<endreq;
        return StatusCode::SUCCESS;
    }


    if( !ex_calib ) ex_calib = new MdcDedxCorrection;
    //#ifdef DBCALIB
    //    ex_calib->getCalib();  //cleate MdcDedxWire and MdcDedxLayer.
    //#endif

    //------------------------------produce histogram root files-----------------------------//
    if(m_rootfile!="no rootfile")
    {
        const char* preDir = gDirectory->GetPath();
        m_hlist  = new TObjArray(0); 
        m_hitlevel = new TFolder("dedx_hitlevel","hitlevel");
        m_hlist -> Add(m_hitlevel);
        m_hitNo_wire = new TH1F("dedx_HitNo_wire","dedx hitNo vs wire",6797, -0.5, 6796.5);
        m_hitlevel -> Add(m_hitNo_wire);
        gDirectory->cd(preDir);
    }

    //------------------------------produce ntuple files-------------------------------------//
    if( ntpFlag ==2 )
    {
        StatusCode status;
        NTuplePtr  nt1(ntupleSvc(),"FILE103/n103");
        if ( nt1 )
          m_nt1 = nt1;
        else 
        {
            m_nt1= ntupleSvc()->book("FILE103/n103",CLID_ColumnWiseTuple,"dEdx vs momentum");
            if ( m_nt1 )
            {
                status = m_nt1->addItem("phtm",m_phtm);
                //status = m_nt1->addItem("median",m_median);
                //status = m_nt1->addItem("geom",m_geometric);
                //status = m_nt1->addItem("geom_tr",m_geometric_trunc);
                //status = m_nt1->addItem("harm",m_harmonic);
                //status = m_nt1->addItem("harm_tr",m_harmonic_trunc);
                //status = m_nt1->addItem("transf",m_transform);
                //status = m_nt1->addItem("logex",m_logtrunc);
                status = m_nt1->addItem("dEdx_meas", m_dEdx_meas);

                status = m_nt1->addItem("ptrk",m_ptrk);
                status = m_nt1->addItem("sintheta",m_sintheta);
                status = m_nt1->addItem("costheta",m_costheta);
                status = m_nt1->addItem("charge",m_charge);
                status = m_nt1->addItem("runNO",m_runNO);
                status = m_nt1->addItem("evtNO",m_evtNO);
                status = m_nt1->addItem("t0",m_t0);
                status = m_nt1->addItem("trackId",m_trackId); 
                status = m_nt1->addItem("poca_x",m_poca_x);
                status = m_nt1->addItem("poca_y",m_poca_y);
                status = m_nt1->addItem("poca_z",m_poca_z);
                status = m_nt1->addItem("recalg",m_recalg);

                status = m_nt1->addItem("nhit",m_nhit);
                status = m_nt1->addItem("usedhit",m_usedhit);
                status = m_nt1->addItem("ndedxhit",m_nphlisthit,0,100);
                status = m_nt1->addIndexedItem("dEdx_hit",m_nphlisthit,m_dEdx_hit);

                status = m_nt1->addItem("type",m_parttype);
                status = m_nt1->addItem("prob",m_prob);
                status = m_nt1->addItem("expect",m_expect);
                status = m_nt1->addItem("sigma",m_sigma);
                status = m_nt1->addItem("chidedx",m_chidedx);
                status = m_nt1->addItem("chidedx_e",m_chidedxe);
                status = m_nt1->addItem("chidedx_mu",m_chidedxmu);
                status = m_nt1->addItem("chidedx_pi",m_chidedxpi);
                status = m_nt1->addItem("chidedx_k",m_chidedxk);
                status = m_nt1->addItem("chidedx_p",m_chidedxp);
                status = m_nt1->addItem("partid",5,m_probpid);
                status = m_nt1->addItem("expectid",5,m_expectid);
                status = m_nt1->addItem("sigmaid",5,m_sigmaid);
            }
        }

        NTuplePtr  nt2(ntupleSvc(),"FILE103/n102");
        if ( nt2 )  m_nt2 = nt2;
        else 
        {
            m_nt2= ntupleSvc()->book("FILE103/n102",CLID_RowWiseTuple,"pulae height raw");
            if ( m_nt2 )
            {
                status = m_nt2->addItem("charge",m_charge1);
                status = m_nt2->addItem("adc_raw",m_phraw);
                status = m_nt2->addItem("exraw",m_exraw);
                status = m_nt2->addItem("runNO1",m_runNO1);
                status = m_nt2->addItem("nhit_hit", m_nhit_hit);
                status = m_nt2->addItem("wire",m_wire);
                //status = m_nt2->addItem("doca",m_doca);
                status = m_nt2->addItem("doca_in",m_doca_in);
                status = m_nt2->addItem("doca_ex",m_doca_ex);
                status = m_nt2->addItem("driftdist",m_driftdist);
                status = m_nt2->addItem("eangle",m_eangle);
                status = m_nt2->addItem("costheta1",m_costheta1);
                status = m_nt2->addItem("path_rphi",m_pathL);
                status = m_nt2->addItem("layer",m_layer);
                status = m_nt2->addItem("ptrk1",m_ptrk1);
                status = m_nt2->addItem("ptrk_hit",m_ptrk_hit);
                status = m_nt2->addItem("t01",m_t01); 
                status = m_nt2->addItem("tdc_raw",m_tdc_raw);  
                status = m_nt2->addItem("driftT",m_driftT);
                status = m_nt2->addItem("localwid",m_localwid);
                status = m_nt2->addItem("trackId1",m_trackId1);
            }
        }
    }

    return StatusCode::SUCCESS;
}


StatusCode MdcDedxRecon::beginRun()
{
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG <<"in MdcDedxRecon::beginrun()!!!"<< endreq;

    StatusCode gesc = service("MdcGeomSvc", geosvc);
    if (gesc ==  StatusCode::SUCCESS) 
    {
        log << MSG::INFO <<"MdcGeomSvc is running"<<endreq;
    } 
    else 
    {
        log << MSG::ERROR <<"MdcGeomSvc is failed"<<endreq;
        return StatusCode::SUCCESS;
    }

    return StatusCode::SUCCESS;
}


StatusCode MdcDedxRecon::finalize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "in finalize()" << endreq;

    ex_trks.clear();
    m_trkalgs.clear();

    if(m_rootfile != "no rootfile")
    {
        TFile *f1 = new TFile(m_rootfile.c_str(),"recreate");
        m_hlist->Write();
        f1->Close();
        delete f1;
        delete m_hitNo_wire;
        delete m_hitlevel;
        delete m_hlist;
    }
    delete ex_calib;

    cout<<"total event number is : "<<eventNo<<endl;
    cout<<"total track number is : "<<trackNO1
        <<"     RecMdcTrack number is : "<<trackNO2
        <<"     RecMdcKalTrack number is :"<<trackNO3<<endl;
    log << MSG::DEBUG <<"MdcDedxRecon terminated!!!"<< endreq;
    return StatusCode::SUCCESS;
}


StatusCode MdcDedxRecon::execute()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "in execute()" << endreq;

    vector<double> Curve_Para, Sigma_Para;
    int vFlag[3];//vFlag[0]:dedx curve version;  vFlag[1]:dedx sigma version;  vFlag[2]: 0:data; 1:MC

    for(int i=0; i< dedxcursvc->getCurveSize(); i++) // get the dedx curve parameters
    {
        if(i==0)  vFlag[0] = (int) dedxcursvc->getCurve(i);  //first element is dedx curve version
        else  Curve_Para.push_back( dedxcursvc->getCurve(i) );  //dedx curve parameters
    }
    for(int i=0; i< dedxcursvc->getSigmaSize(); i++)
    {
        if(i==0) vFlag[1] = (int) dedxcursvc->getSigma(i);  //dedx sigma version: 2: psip; 3:jpsi
        else  Sigma_Para.push_back( dedxcursvc->getSigma(i) );  //dedx sigma parameters
    }

    //check if size of parameters is right
    if(vFlag[0] ==1 && Curve_Para.size() != 5)    //version 1: 5 parameters 652a psip data 
      cout<<" size of dedx curve paramters for version 1 is not right!"<<endl;
    if(vFlag[0] ==2 && Curve_Para.size() != 16)   //version 2: increase from 5 parameters of 652 to 16
      cout<<" size of dedx curve paramters for version 2 is not right!"<<endl;

    if(vFlag[1] ==1 && Sigma_Para.size() != 14)   //vesion 1: 14 parameters 652a psip data (old way)
      cout<<" size of dedx sigma paramters for version 1 is not right!"<<endl;
    if(vFlag[1] ==2 && Sigma_Para.size() != 21)   //version 2: include t0 correction (for psip09 data)
      cout<<" size of dedx sigma paramters for version 2 is not right!"<<endl;
    if(vFlag[1] ==3 && Sigma_Para.size() != 18)   //version 3: no t0 correction (for jpsi09 data and beyond)
      cout<<" size of dedx sigma paramters for version 3 is not right!"<<endl;
    if(vFlag[1] ==4 && Sigma_Para.size() != 19)  //version 4: data with mdc track defaulted when kal track not ok(no t0 correction) 
      cout<<" size of dedx sigma paramters for version 4 is not right!"<<endl;
    if(vFlag[1] ==5 && Sigma_Para.size() != 22)  //version 5: data with mdc track defaulted when kal track not ok(with t0 correction) 
      cout<<" size of dedx sigma paramters for version 5 is not right!"<<endl;


    //---------- register RecMdcDedxCol and RecMdcDedxHitCol to tds---------//
    DataObject *aReconEvent;
    eventSvc()->findObject("/Event/Recon",aReconEvent);
    if(aReconEvent==NULL) 
    {        
        aReconEvent = new ReconEvent();
        StatusCode sc = eventSvc()->registerObject("/Event/Recon",aReconEvent);
        if(sc!=StatusCode::SUCCESS) 
        {
            log << MSG::FATAL << "Could not register ReconEvent" <<endreq;            
            return( StatusCode::FAILURE);
        } 
    }

    SmartIF<IDataManagerSvc> dataManSvc(eventSvc());

    DataObject *aDedxcol;
    eventSvc()->findObject("/Event/Recon/RecMdcDedxCol",aDedxcol);    
    if(aDedxcol != NULL) 
    {
        dataManSvc->clearSubTree("/Event/Recon/RecMdcDedxCol");
        eventSvc()->unregisterObject("/Event/Recon/RecMdcDedxCol");
    }
    dedxList = new RecMdcDedxCol;
    StatusCode dedxsc;
    dedxsc =  eventSvc()->registerObject(EventModel::Recon::RecMdcDedxCol, dedxList);
    if(!dedxsc.isSuccess()) 
    {
        log << MSG::FATAL << " Could not register Mdc dedx collection" <<endreq;
        return ( StatusCode::FAILURE);
    }

    DataObject *aDedxhitcol;
    eventSvc()->findObject("/Event/Recon/RecMdcDedxHitCol",aDedxhitcol);
    if(aDedxhitcol != NULL) 
    {
        dataManSvc->clearSubTree("/Event/Recon/RecMdcDedxHitCol");
        eventSvc()->unregisterObject("/Event/Recon/RecMdcDedxHitCol");
    }
    dedxhitList = new RecMdcDedxHitCol;
    StatusCode dedxhitsc;
    dedxhitsc =  eventSvc()->registerObject(EventModel::Recon::RecMdcDedxHitCol, dedxhitList);
    if(!dedxhitsc.isSuccess()) 
    {
        log << MSG::FATAL << " Could not register Mdc dedx collection" <<endreq;
        return ( StatusCode::FAILURE);
    }    

    SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
    if (!eventHeader) 
    {
        log << MSG::INFO << "Could not find Event Header" << endreq;
        return( StatusCode::FAILURE);
    }
    int eventNO = eventHeader->eventNumber();
    int runNO = eventHeader->runNumber();
    log << MSG::INFO << "now begin the event:  runNO= "<<runNO<<" eventNO= "<< eventNO<< endreq;
    RunNO2= runNO; 
    if(RunNO1==0) RunNO1=runNO;
    if(RunNO2 != RunNO1)
    {
        cout<<"RunNO2 = "<<RunNO2 <<"      RunNO1 = "<<RunNO1<<endl;
    }
    RunNO1 = runNO;
    int runFlag; //data type flag
    if(runNO<MdcDedxParam::RUN0) runFlag =0;   //MC: less than 0
    else if(runNO>=MdcDedxParam::RUN1 && runNO<MdcDedxParam::RUN2) runFlag =1;
    else if(runNO>=MdcDedxParam::RUN2 && runNO<MdcDedxParam::RUN3) runFlag =2;
    else if(runNO>=MdcDedxParam::RUN3 && runNO<MdcDedxParam::RUN4) runFlag =3;
    else  runFlag =4;

    //vFlag[2] identify MC or data: 1:Mc;  0:data
    if(runNO < 0)   vFlag[2]=1;
    else            vFlag[2]=0;

    double tes = -999.0;
    int esTimeflag = -1;
    SmartDataPtr<RecEsTimeCol> estimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
    if( ! estimeCol) 
    {
        log << MSG::WARNING << "Could not find EvTimeCol" << endreq;
    }
    else
    {
        RecEsTimeCol::iterator iter_evt = estimeCol->begin();
        for(; iter_evt!=estimeCol->end(); iter_evt++)
        {
            tes =  (*iter_evt)->getTest();
            esTimeflag = (*iter_evt)->getStat();
        }
    }   
    //cout<<"estime : "<<tes<<endl;


    Identifier mdcid; 
    int ntrk;
    ex_trks.clear();  // clear the vector of MdcDedxTrk,when read a new event  
    m_trkalgs.clear();
    if( !doNewGlobal )
    {
        log << MSG::DEBUG << "recalg:  "<<recalg<<endreq;

        //---------using kal algorithm by default, switch to seek mdc track if no kaltrack hits //
        if(recalg ==2 )
        {
            //retrieve  MdcKalTrackCol from TDS
            SmartDataPtr<RecMdcKalTrackCol> newtrkCol(eventSvc(),"/Event/Recon/RecMdcKalTrackCol");
            if (!newtrkCol) 
            {
                log << MSG::WARNING << "Could not find RecMdcKalTrackCol" << endreq;
                return StatusCode::SUCCESS;
            }
            if(ntpFlag>0) eventNo++;
            ntrk = newtrkCol->size();
            log << MSG::DEBUG << "track collection size:  " << newtrkCol->size() <<endreq;

            RecMdcKalTrackCol::iterator trk = newtrkCol->begin();
            //cout<<"MdcDedxRecon  newtrkCol.size()  "<<newtrkCol->size()<<endl;
            for( ; trk != newtrkCol->end(); trk++) 
            {
                if(ntpFlag>0) trackNO1++;

                HelixSegRefVec gothits= (*trk)->getVecHelixSegs(ParticleFlag);  
                //if not set ParticleFlag, we will get the last succefully hypothesis;
                if(gothits.size()==0)
                {
                    m_trkalg=0;
                    int trkid=(*trk)->trackId();
                    switchtomdctrack(trkid, mdcid, tes, runNO, runFlag, log);
                }
                else
                {
                    m_trkalg=1;
                    if(gothits.size()<2) continue;
                    kaltrackrec(trk, mdcid, tes, runNO, runFlag, log );
                }
            }//end of track loop
        }//end of recalg==2

        //------------------------ Use information of MDC track rec --------------------------//
        else if(recalg ==0 )
        {
            m_trkalg=0; 

            //retrieve  MdcTrackCol from TDS     
            SmartDataPtr<RecMdcTrackCol> newtrkCol(eventSvc(),"/Event/Recon/RecMdcTrackCol");
            if (!newtrkCol) 
            {
                log << MSG::WARNING << "Could not find RecMdcTrackCol" << endreq;
                return StatusCode::SUCCESS;
            }
            if(ntpFlag>0) eventNo++;
            ntrk = newtrkCol->size();
            log << MSG::DEBUG << "track collection size:  " << newtrkCol->size() <<endreq;

            vector<double> phlist;  //dE/dx only after hit level correction
            vector<double> phlist_hit;  //dE/dx after hit and track level correction
            double m_d0E=0,m_phi0E=0,m_cpaE=0,m_z0E=0,phi0=0,costheta=0,sintheta=0,m_Pt=0,m_P=0;
            int charge=0,layid=0,localwid=0,w0id=0,wid=0,lr=-2;
            double adc_raw=0,tdc_raw=0,zhit=0,driftd=0,driftD=0,driftT=0,dd=0,eangle=0;
            int Nhits=0;
            double ph=0,ph_hit=0,pathlength=0,rphi_path=0;

            RecMdcTrackCol::iterator trk = newtrkCol->begin(); 
            for( ; trk != newtrkCol->end(); trk++)
            {
                if(ntpFlag>0) trackNO1++;

                MdcDedxTrk trk_ex( **trk);
                log << MSG::DEBUG <<" %%%%% trk id = "<<trk_ex.trk_id() <<endreq;
                log << MSG::DEBUG <<" %%%%% initial id = "<<(*trk)->trackId() <<endreq;

                HepVector a = (*trk)->helix();
                HepSymMatrix tkErrM = (*trk)->err();
                m_d0E   = tkErrM[0][0];
                m_phi0E = tkErrM[1][1];
                m_cpaE  = tkErrM[2][2];
                m_z0E   = tkErrM[3][3];

                HepPoint3D IP(0,0,0);
                Dedx_Helix exhel(IP, a);  //initialize class Dedx_Helix for DedxCorrecSvc
                log << MSG::DEBUG <<"MDC Helix 5 parameters: "<<a[0]<<"     "<<a[1]<<"      "<<a[2]<<"      "<<a[3]<<"     "<<a[4]<<endreq;
                //cout<<"MDC Helix 5 parameters: "<<a[0]<<"     "<<a[1]<<"      "<<a[2]<<"      "<<a[3]<<"     "<<a[4]<<endl;
                phi0= a[1];
                costheta = cos(M_PI_2-atan(a[4]));
                //cout<<"track theta: "<<M_PI_2-atan(a[4])<<" extrack costheta: "<<trk_ex.theta()<<endl;
                sintheta = sin(M_PI_2-atan(a[4]));
                m_Pt = 1.0/fabs( a[2] );
                m_P = m_Pt*sqrt(1 + a[4]*a[4]);
                charge = ( a[2] > 0 )? 1 : -1;
                //cout<<"track charge: "<<charge<<" extrack charge: "<<trk_ex.charge()<<endl;
                dedxhitrefvec = new DedxHitRefVec;


                HitRefVec gothits= (*trk)->getVecHits();
                Nhits = gothits.size();
                log << MSG::DEBUG << "hits size  = " <<gothits.size()<<endreq;
                if(gothits.size()<2) 
                {
                    delete dedxhitrefvec;
                    continue;
                }
                //if(gothits.size()<15) cout<<"eventNO : "<<eventNO<<"  hits : "<<gothits.size()<<endl;

                RecMdcKalHelixSeg* mdcKalHelixSeg = 0;
                HitRefVec::iterator it_gothit = gothits.begin();
                for( ; it_gothit != gothits.end(); it_gothit++) 
                {
                    mdcid = (*it_gothit)->getMdcId();
                    layid =  MdcID::layer(mdcid);
                    localwid = MdcID::wire(mdcid); 
                    w0id = geosvc->Layer(layid)->Wirst();
                    wid = w0id + localwid;
                    adc_raw = (*it_gothit)->getAdc();
                    tdc_raw = (*it_gothit)->getTdc();
                    log << MSG::INFO <<"hit layer id  " <<layid<< " wire id: " <<localwid<< " adc_raw: " <<adc_raw<<" tdc_raw: "<<tdc_raw<<endreq;
                    zhit = (*it_gothit)->getZhit();
                    lr = (*it_gothit)->getFlagLR();
                    if(lr == 2) cout<<"lr = "<<lr<<endl;
                    if(lr == 0 || lr == 2) driftD = (*it_gothit)->getDriftDistLeft();
                    else  driftD = (*it_gothit)->getDriftDistRight();   
                    //cout<<"lr: "<<lr<<"  driftD: "<<driftD<<endl;
                    driftd = abs(driftD);
                    dd = (*it_gothit)->getDoca();
                    if(lr == 0 || lr == 2 ) dd = -abs(dd);
                    if(lr == 1 ) dd = abs(dd);
                    driftT = (*it_gothit)->getDriftT();

                    eangle = (*it_gothit)->getEntra();
                    eangle = eangle/M_PI;
                    pathlength=exsvc->PathL( ntpFlag, exhel, layid, localwid, zhit);
                    rphi_path=pathlength * sintheta;

                    //cout<<"ph para check: "<<"adc_raw: "<<adc_raw<<"  dd: "<<dd<<" eangle: "<<eangle<<" zhit: "<<zhit<<" costheta: "<<costheta<<endl;
                    ph=exsvc->StandardHitCorrec(0,runFlag,ntpFlag,runNO,pathlength,wid,layid,adc_raw,dd,eangle,costheta);
                    ph_hit = exsvc->StandardTrackCorrec(0,runFlag, ntpFlag, runNO, ph, costheta,tes );
                    //if(pathlength == -1)
                    //cout<<"parameter0: "<<"eventNO: "<<eventNO<<" layid: "<<layid<<" localwid: "<<localwid<<" driftd: "<<driftd<<" rphi_path: "<<rphi_path<<" pathlength: "<<pathlength<<" ph: "<<ph<<" ph_hit: "<<ph_hit<<endl;

                    if(runNO<=5911 && runNO>=5854 && layid ==14) continue;
                    if(runNO<0)
                    {
                        if (layid<8)
                        {  
                            if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Iner_RPhi_PathMinCut || driftd>Iner_DriftDistCut) continue;
                        }
                        else if(layid >= 8)
                        {
                            if(adc_raw<MinHistValue || adc_raw>MaxHistValue ||rphi_path>RPhi_PathMaxCut || rphi_path<Out_RPhi_PathMinCut || driftd>Out_DriftDistCut ) continue;                     
                        }
                    }
                    else if(runNO>=0)
                    {
                        if(layid <8)
                        {  
                            if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Iner_RPhi_PathMinCut || driftd>Iner_DriftDistCut) continue;
                        }
                        else if(layid >= 8)
                        {
                            if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Out_RPhi_PathMinCut || driftd>Out_DriftDistCut ) continue;
                        }
                    }
                    //cout<<"recAlg=0 para mdc:  m_phraw: "<<adc_raw<<" m_exraw: "<<ph_hit<<" m_doca: "<<dd<<" m_eangle: "<<eangle<<" m_costheta: "<<costheta<<endl;

                    if (ph<0.0||ph==0) continue;
                    else
                    {
                        //-----------------------put data into TDS-----------------------------//
                        dedxhit = new RecMdcDedxHit;
                        dedxhit->setMdcHit(*it_gothit);
                        dedxhit->setMdcKalHelixSeg(mdcKalHelixSeg );
                        dedxhit->setMdcId(mdcid);
                        dedxhit->setFlagLR(lr);
                        dedxhit->setTrkId(trk_ex.trk_id());
                        dedxhit->setDedx(ph_hit);
                        dedxhit->setPathLength(pathlength);

                        //---------------------------Fill root file----------------------------//
                        if(m_rootfile!= "no rootfile")
                        {
                            m_hitNo_wire->Fill(wid);
                        }

                        //-------------------------Fill ntuple n102---------------------------//
                        if ( ntpFlag ==2 )
                        {
                            m_charge1 = (*trk)->charge();
                            m_t01 = tes;
                            m_driftT = driftT;
                            m_tdc_raw = tdc_raw;
                            m_phraw = adc_raw;
                            m_exraw = ph_hit;
                            m_localwid = localwid;
                            m_wire = wid;
                            m_runNO1 = runNO;
                            m_nhit_hit = Nhits;
                            m_doca_in = dd;
                            m_doca_ex = dd;
                            m_driftdist = driftD;
                            m_eangle = eangle;
                            m_costheta1 = costheta;
                            m_pathL = pathlength;
                            m_layer = layid;
                            m_ptrk1 = m_P;
                            //cout<<"adc_raw : "<<adc_raw<<"     "<<ph_hit<<"     "<<dd_in<<"  "<<dd_ex<<"  "<<eangle<<endl;
                            m_trackId1 = trk_ex.trk_id();
                            StatusCode status2 = m_nt2->write();
                            if ( status2.isFailure() )
                              log << MSG::ERROR << "Cannot fill Ntuple n102" << endreq;
                        }
                        if(layid>3)   
                        {
                            phlist.push_back(ph);
                            phlist_hit.push_back(ph_hit);
                        }
                    }
                    dedxhitList->push_back( dedxhit );
                    dedxhitrefvec->push_back( dedxhit );
                }//end of hit loop
                trk_ex.set_phlist( phlist );
                trk_ex.set_phlist_hit( phlist_hit );
                trk_ex.setVecDedxHits( *dedxhitrefvec );
                ex_trks.push_back(trk_ex );
                m_trkalgs.push_back(m_trkalg);

                delete dedxhitrefvec;
                phlist.clear();
                phlist_hit.clear();
                if(ntpFlag>0) trackNO2++;
            }//end of track loop
            log << MSG::DEBUG << "size in processing:  " << ex_trks.size() <<endreq; 
        }//end of recalg==0

        //------------------------ Use information of MDC KAL track rec -----------------------//
        else if(recalg ==1 )
        {
            m_trkalg=1;

            //retrieve  MdcKalTrackCol from TDS
            SmartDataPtr<RecMdcKalTrackCol> newtrkCol(eventSvc(),"/Event/Recon/RecMdcKalTrackCol");
            if (!newtrkCol) 
            {
                log << MSG::WARNING << "Could not find RecMdcKalTrackCol" << endreq;
                return  StatusCode::SUCCESS;
            }
            if(ntpFlag>0) eventNo++; 
            ntrk = newtrkCol->size();
            log << MSG::DEBUG << "track collection size:  " << newtrkCol->size() <<endreq;

            vector<double> phlist; //dE/dx only after hit level correction
            vector<double> phlist_hit;  //dE/dx after hit and track level correction
            double m_d0E=0,m_phi0E=0,m_cpaE=0,m_z0E=0,phi0=0,costheta=0,sintheta=0,m_Pt=0,m_P=0;
            int charge=0,layid=0,localwid=0,w0id=0,wid=0,lr=-2;
            double p_hit=0,adc_raw=0,tdc_raw=0,zhit=0,driftd=0,driftD=0,driftT=0,dd_in=0,dd_ex=0,eangle=0;
            int Nhits=0;
            double ph=0,ph_hit=0,pathlength=0,rphi_path=0;


            RecMdcKalTrackCol::iterator trk = newtrkCol->begin();
            for( ; trk != newtrkCol->end(); trk++)
            {
                if(ntpFlag>0) trackNO1++;

                MdcDedxTrk trk_ex( **trk, ParticleFlag);
                log << MSG::DEBUG <<" %%%%% trk id = "<<trk_ex.trk_id() <<endreq;
                log << MSG::DEBUG <<" %%%%% initial id = "<<(*trk)->trackId() <<endreq;

                HepVector a;
                HepSymMatrix tkErrM;
                if(ParticleFlag>-1&&ParticleFlag<5) 
                {
                    DstMdcKalTrack* dstTrk = *trk;
                    a = dstTrk->getFHelix(ParticleFlag);
                    tkErrM = dstTrk->getFError(ParticleFlag);
                }
                else 
                {
                    a = (*trk)->getFHelix();
                    tkErrM = (*trk)->getFError();
                }
                //cout<<"FHelix 5 parameters: "<<a[0]<<"     "<<a[1] <<"      "<<a[2]<<"      "<<a[3]<<"     "<<a[4]<<endl;  //getFHelix is first layer of MdcKalTrack;
                log << MSG::DEBUG <<"FHelix 5 parameters: "<<a[0]<<"     "<<a[1] <<"      "<<a[2]<<"      "<<a[3]<<"     "<<a[4]<<endreq;

                m_d0E   = tkErrM[0][0];
                m_phi0E = tkErrM[1][1];
                m_cpaE  = tkErrM[2][2];
                m_z0E   = tkErrM[3][3];

                phi0= a[1];
                costheta = cos(M_PI_2-atan(a[4]));
                //cout<<"track theta: "<<M_PI_2-atan(a[4])<<" extrack theta: "<<trk_ex.theta()<<endl;  //theta in trk_ex is got by getFHelix();
                sintheta = sin(M_PI_2-atan(a[4]));
                m_Pt = 1.0/fabs( a[2] );
                m_P = m_Pt*sqrt(1 + a[4]*a[4]);
                //cout<<"track ptrk: "<<m_P<<" extrack ptrk: "<<trk_ex.P()<<endl;
                charge = ( a[2] > 0 )? 1 : -1;
                //cout<<"track charge: "<<charge<<" extrack charge: "<<trk_ex.charge()<<endl;
                dedxhitrefvec = new DedxHitRefVec;


                HelixSegRefVec gothits= (*trk)->getVecHelixSegs(ParticleFlag);
                //HelixSegRefVec gothits= (*trk)->getVecHelixSegs();
                //if not set ParticleFlag, we will get the last succefully hypothesis;
                Nhits = gothits.size();
                log << MSG::DEBUG << "hits size  = " <<gothits.size()<<endreq;
                if(gothits.size()<2) 
                {
                    delete dedxhitrefvec;
                    continue;
                }
                //if(gothits.size()<15) cout<<"eventNO : "<<eventNO<<"  hits : "<<gothits.size()<<endl;

                RecMdcHit* mdcHit = 0;
                HelixSegRefVec::iterator it_gothit = gothits.begin();
                for( ; it_gothit != gothits.end(); it_gothit++)
                {
                    HepVector a_hit_in = (*it_gothit)->getHelixIncl();
                    //HepVector a_hit_ex = (*it_gothit)->getHelixExcl(); //same with getHelixIncl()
                    HepPoint3D IP(0,0,0);
                    Dedx_Helix exhel_hit_in(IP, a_hit_in);
                    //Dedx_Helix exhel_hit_ex(IP, a_hit_ex);
                    p_hit = 1.0/fabs(a_hit_in(3))*sqrt(1+a_hit_in(5)*a_hit_in(5));   
                    //cout<<"getHelixIncl 5 para: "<<a_hit_in[0]<<"     "<<a_hit_in[1] <<"      "<<a_hit_in[2]<<"      "<<a_hit_in[3]<<"     "<<a_hit_in[4]<<endl;

                    mdcid = (*it_gothit)->getMdcId();
                    layid =  MdcID::layer(mdcid);
                    localwid = MdcID::wire(mdcid);
                    w0id = geosvc->Layer(layid)->Wirst();
                    wid = w0id + localwid;
                    adc_raw = (*it_gothit)->getAdc();
                    tdc_raw = (*it_gothit)->getTdc();
                    log << MSG::INFO <<"hit layer id  " <<layid<< " wire id: " <<localwid<< " adc_raw: " <<adc_raw<<" tdc_raw: "<<tdc_raw<<endreq;
                    zhit = (*it_gothit)->getZhit();
                    lr = (*it_gothit)->getFlagLR();
                    if(lr == 2) cout<<"lr = "<<lr<<endl;
                    driftD = (*it_gothit)->getDD();
                    driftd = abs(driftD);
                    driftT = (*it_gothit)->getDT();
                    dd_in = (*it_gothit)->getDocaIncl(); //getDocaIncl() include fit unused hit
                    dd_ex = (*it_gothit)->getDocaExcl(); //getDocaExcl() exclude fit unused hit

                    if(lr==-1 || lr == 2) {dd_in = dd_in; dd_ex = dd_ex;}
                    else if(lr ==1) {dd_in = -dd_in; dd_ex = -dd_ex;}
                    //dd = dd/doca_norm[layid];
                    //cout<<"lr "<<lr<<" dd_in: "<<dd_in<<" dd_ex: "<<dd_ex<<endl;

                    eangle = (*it_gothit)->getEntra();
                    eangle = eangle/M_PI;
                    pathlength=exsvc->PathL( ntpFlag, exhel_hit_in, layid, localwid, zhit);
                    rphi_path=pathlength * sintheta;
                    //cout<<"ph para check: "<<"adc_raw: "<<adc_raw<<"  dd_in: "<<dd_in<<" eangle: "<<eangle<<" zhit: "<<zhit<<" costheta: "<<costheta<<endl;
                    ph=exsvc->StandardHitCorrec(0,runFlag,ntpFlag,runNO,pathlength,wid,layid,adc_raw,dd_in,eangle,costheta); 
                    ph_hit = exsvc->StandardTrackCorrec(0,runFlag, ntpFlag, runNO, ph, costheta,tes );
                    //if(pathlength == -1)  
                    //cout<<"parameter1: "<<" layid: "<<layid<<" localwid: "<<localwid<<" driftd: "<<driftd<<" rphi_path: "<<rphi_path<<" pathlength: "<<pathlength<<" ph: "<<ph<<"  ph_hit: "<<ph_hit<<endl;

                    if(runNO<=5911 && runNO>=5854 && layid ==14) continue;
                    if(runNO<0)
                    {
                        if (layid<8)
                        {  
                            if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Iner_RPhi_PathMinCut || driftd>Iner_DriftDistCut) continue;                         
                        }
                        else if(layid >= 8)
                        {  
                            if(adc_raw<MinHistValue || adc_raw>MaxHistValue ||rphi_path>RPhi_PathMaxCut || rphi_path<Out_RPhi_PathMinCut || driftd>Out_DriftDistCut ) continue;  
                        }
                    }    
                    else if(runNO>=0)
                    {
                        if(layid <8)
                        {
                            if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Iner_RPhi_PathMinCut || driftd>Iner_DriftDistCut) continue;
                        }
                        else if(layid >= 8)
                        {  
                            if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Out_RPhi_PathMinCut || driftd>Out_DriftDistCut ) continue;
                        }
                    }
                    //cout<<"recAlg=1 para kal:  m_phraw: "<<adc_raw<<" m_exraw: "<<ph_hit<<" m_doca: "<<dd_in<<" m_eangle: "<<eangle<<" m_costheta: "<<costheta<<endl;

                    if (ph<0.0||ph==0) continue;
                    else
                    {
                        //-----------------------put data into TDS-----------------------------//
                        dedxhit = new RecMdcDedxHit;
                        dedxhit->setMdcHit(mdcHit);
                        dedxhit->setMdcKalHelixSeg(*it_gothit);
                        dedxhit->setMdcId(mdcid);
                        dedxhit->setFlagLR(lr);
                        dedxhit->setTrkId(trk_ex.trk_id());
                        dedxhit->setDedx(ph_hit);
                        dedxhit->setPathLength(pathlength); 

                        //---------------------------Fill root file----------------------------//
                        if(m_rootfile!= "no rootfile")                    
                        {
                            m_hitNo_wire->Fill(wid);
                        }

                        //-------------------------Fill ntuple n102---------------------------//
                        if ( ntpFlag ==2 )
                        {
                            m_charge1 = (*trk)->charge();
                            m_t01 = tes;
                            m_driftT = driftT;
                            m_tdc_raw = tdc_raw;
                            m_phraw = adc_raw;
                            m_exraw = ph_hit;
                            m_localwid = localwid;
                            m_wire = wid;
                            m_runNO1 = runNO;
                            m_nhit_hit = Nhits;
                            m_doca_in = dd_in;
                            m_doca_ex = dd_ex;
                            m_driftdist = driftD;
                            m_eangle = eangle;
                            m_costheta1 = costheta;
                            m_pathL = pathlength;
                            m_layer = layid;
                            m_ptrk1 = m_P;
                            m_ptrk_hit = p_hit;
                            //cout<<"adc_raw : "<<adc_raw<<"     "<<ph_hit<<"     "<<dd_in<<"  "<<dd_ex<<"  "<<eangle<<endl;
                            m_trackId1 = trk_ex.trk_id();
                            StatusCode status2 = m_nt2->write();
                            if ( status2.isFailure() )  
                              log << MSG::ERROR << "Cannot fill Ntuple n102" << endreq;
                        }
                        if(layid>3)   
                        {
                            phlist.push_back(ph);
                            phlist_hit.push_back(ph_hit);
                        }
                    }
                    dedxhitList->push_back( dedxhit );
                    dedxhitrefvec->push_back( dedxhit );
                }//end of hit loop
                trk_ex.set_phlist( phlist );
                trk_ex.set_phlist_hit( phlist_hit );
                trk_ex.setVecDedxHits( *dedxhitrefvec );
                ex_trks.push_back(trk_ex );
                m_trkalgs.push_back(m_trkalg); 

                delete dedxhitrefvec;
                phlist.clear();
                phlist_hit.clear();
                if(ntpFlag>0) trackNO3++;
            }//end of track loop
            log << MSG::DEBUG << "size in processing:  " << ex_trks.size() <<endreq;
        }//end of recalg==1
        //--------------------------------- Hit level finished ---------------------------------//

        //-------------------------------- Track level begin --------------------------------//
        if( ntrk != ex_trks.size())
          log << MSG::DEBUG <<"ntrkCol size: "<<ntrk<<" dedxtrk size: "<<ex_trks.size()<< endreq;

        double poca_x=0,poca_y=0,poca_z=0;
        float sintheta=0,costheta=0,ptrk=0,charge=0;
        int Nhit=0,Nphlisthit=0;
        int usedhit = 0;
        double phtm=0,median=0,geometric=0,geometric_trunc=0,harmonic=0,harmonic_trunc=0,transform=0,logtrunc=0;

        enum pid_dedx parType_temp(electron);
        float probpid_temp=-0.01,expect_temp=-0.01,sigma_temp=-0.01,chidedx_temp=-0.01;

        double dEdx_meas_hit=0;
        double dEdx_meas=0,dEdx_meas_esat=0,dEdx_meas_norun=0; 
        int trk_recalg=-1;

        int idedxid = 0;
        for(std::vector<MdcDedxTrk>::iterator it = ex_trks.begin(); it != ex_trks.end(); it++, idedxid++ )
        {
            log << MSG::DEBUG <<"-------------------------------------------------------"<< endreq;
            log << MSG::DEBUG <<" trk id ="<< it->trk_id()<<" : P ="<<it->P() <<"  Pt ="<<it->Pt()<<" : theta ="<<it->theta() <<" : phi ="<<it->phi()<< " : charge = "<<it->charge()<<endreq;
            log << MSG::DEBUG <<"all hits on track: "<<it->nsample()<<"  phlist size: "<<it->get_phlist().size()<<endreq;

            if(it->trk_ptr_kal()!=0)
            {
                poca_x = it->trk_ptr_kal()->x();  //get poca, default pid is pion; change pid using setPidType();
                poca_y = it->trk_ptr_kal()->y();
                poca_z = it->trk_ptr_kal()->z();
            }
            else if(it->trk_ptr()!=0)
            {
                poca_x = it->trk_ptr()->x();
                poca_y = it->trk_ptr()->y();
                poca_z = it->trk_ptr()->z();
            }
            //cout<<"poca_x: "<<poca_x<<" poca_y: "<<poca_y<<" poca_z: "<<poca_z<<endl;

            sintheta = sin(it->theta());
            costheta = cos(it->theta());
            ptrk = it->P();
            charge = it->charge();
            Nhit = it->nsample();   //total hits on track used as sample; 
            Nphlisthit = it->get_phlist_hit().size();  //hits in phlist_hit, exclude first 4 layers;
            //usedhit: hits after truncting phlist and used in cal dE/dx value;

            //--------------------------extrk truncation--------------------------------// 
            phtm = it->cal_dedx( truncate );
            //cout<<"phtm:  "<<phtm<<endl; 
            //median = it->cal_dedx_median( truncate );
            //geometric = it->cal_dedx_geometric( truncate );
            //geometric_trunc = it->cal_dedx_geometric_trunc( truncate );
            //harmonic = it->cal_dedx_harmonic( truncate );
            //harmonic_trunc = it->cal_dedx_harmonic_trunc( truncate );
            //transform = it->cal_dedx_transform( 0 );
            //logtrunc = it->cal_dedx_log( 1.0, 0);

            if(m_alg==1) dEdx_meas_hit = it->cal_dedx_bitrunc(truncate, 0, usedhit);
            else if(m_alg==2) dEdx_meas_hit = it-> cal_dedx_transform(1);
            else if(m_alg==3) dEdx_meas_hit = it->cal_dedx_log(1.0, 1);
            else cout<<"-------------Truncate Algorithm Error!!!------------------------"<<endl;
            if(m_alg==1 && usedhit==0)
            {
                cout<<"***************bad extrk with no hits!!!!!******************"<<endl;
                continue;
            }
	    // force to use the same definition of usedhit in TDS and what used in setDedx() function
	    usedhit = (int)(Nphlisthit*truncate);  

            //--------------------track level correction of extrk dE/dx Value---------------// 
            dEdx_meas = exsvc->StandardTrackCorrec(0,runFlag, ntpFlag, runNO, dEdx_meas_hit,  cos(it->theta()),  tes );
            dEdx_meas_esat = exsvc->StandardTrackCorrec(1,runFlag, ntpFlag, runNO, dEdx_meas_hit,  cos(it->theta()),  tes );
            dEdx_meas_norun = exsvc->StandardTrackCorrec(2,runFlag, ntpFlag, runNO, dEdx_meas_hit,  cos(it->theta()),  tes );
	    log << MSG::INFO << "===================== " << endreq << endreq;
            log << MSG::DEBUG <<"dEdx_meas_hit: "<<dEdx_meas_hit<<" dEdx_meas: "<<dEdx_meas<<" dEdx_meas_esat: "<<dEdx_meas_esat<<" dEdx_meas_norun: "<<dEdx_meas_norun<<" ptrk: "<<it->P()<<endreq;
	    log << MSG::INFO  << "ptrk " << ptrk << "  costhe " << costheta << "  runno " << runNO << "  evtno " << eventNO << endreq;
	    //cout<<"dEdx_meas_hit: "<<dEdx_meas_hit<<" dEdx_meas: "<<dEdx_meas<<" dEdx_meas_esat: "<<dEdx_meas_esat<<" dEdx_meas_norun: "<<dEdx_meas_norun<<" ptrk: "<<it->P()<<endl;

            trk_recalg = m_trkalgs[idedxid];

            if(dEdx_meas<0 || dEdx_meas==0)  continue;
            it->set_dEdx( landau , dEdx_meas, trk_recalg, runFlag, vFlag , tes, Curve_Para, Sigma_Para, ex_calib); // calculate expect
            parType_temp = electron;
            probpid_temp=-0.01; 
            expect_temp=-0.01;
            sigma_temp=-0.01;
            chidedx_temp=-0.01;
            for(int i=0;i<5;i++)
            {   
                if(it->pprob()[i]>probpid_temp)
                {                        
		    parType_temp = (enum pid_dedx) i;  //e:0,  mu:1,  pi:2,  K:3,  p:4
                    probpid_temp = it->pprob()[i];
                    expect_temp = it->pexpect()[i];
                    sigma_temp = it->pexp_sigma()[i];
                    chidedx_temp = it->pchi_dedx()[i];
                }
            }
            log<< MSG::INFO<<"expect dE/dx: type: "<<parType_temp<<" probpid: "<<probpid_temp<<" expect: "<<expect_temp<<" sigma: "<<sigma_temp<<" chidedx: "<<chidedx_temp<<endreq;
            //cout<<"dEdx_meas: "<<dEdx_meas<<endl;

            //-----------------------put data into TDS-----------------------------//
            resdedx = new RecMdcDedx;
            resdedx->setDedxHit(dEdx_meas_hit);
            resdedx->setDedxEsat(dEdx_meas_esat);
            resdedx->setDedxNoRun(dEdx_meas_norun);
            resdedx->setDedxMoment(it->P());
            resdedx->setTrackId( it->trk_id() );
            resdedx->setProbPH( dEdx_meas );
            resdedx->setNormPH( dEdx_meas/550.0 );
            resdedx->setDedxExpect( it->pexpect() );
            resdedx->setSigmaDedx( it->pexp_sigma() );
            resdedx->setPidProb( it->pprob() );
            resdedx->setChi( it->pchi_dedx() );
	    //cout<<"recdedx: "<<"  "<<resdedx->getPidProb(parType_temp)<<"  "<<resdedx->getDedxExpect(parType_temp)<<"  "<<resdedx->getSigmaDedx(parType_temp)<<"  "<<resdedx->chi(parType_temp)<<endl;
            resdedx->setNumTotalHits(it->nsample() );  //all hits on track;
            resdedx->setNumGoodHits(usedhit);   //hits after truncing the phlist
            //phlist are all hits on track excluding first 4 layers; 
            //resdedx->setStatus( it->quality() );
            resdedx->setStatus(trk_recalg );
            //cout<<"trk_recalg: "<<trk_recalg<<"  trk stat: "<<it->quality()<<endl;
            resdedx->setTruncAlg( m_alg );
            resdedx->setParticleId(parType_temp);
            //cout<<"ParticleType: "<<parType_temp<<"  "<<resdedx->particleType()<<endl;
            resdedx->setVecDedxHits(it->getVecDedxHits());
            resdedx->setMdcTrack(it->trk_ptr());
            resdedx->setMdcKalTrack(it->trk_ptr_kal());

            //-------------------------Fill ntuple n103---------------------------//
            if(ntpFlag ==2)
            {
                m_phtm=phtm;
                //m_median=median;
                //m_geometric=geometric;
                //m_geometric_trunc=geometric_trunc;
                //m_harmonic=harmonic;
                //m_harmonic_trunc=harmonic_trunc;
                //m_transform=transform;
                //m_logtrunc=logtrunc;
                m_dEdx_meas = dEdx_meas;

                m_poca_x = poca_x;
                m_poca_y = poca_y;
                m_poca_z = poca_z;
                m_ptrk=ptrk;
                m_sintheta=sintheta;
                m_costheta=costheta;
                m_charge=charge;
                m_runNO = runNO;
                m_evtNO = eventNO;

                m_t0 = tes;
                m_trackId = it->trk_id();
                m_recalg = trk_recalg;

                m_nhit=Nhit;
                m_nphlisthit=Nphlisthit; 
                m_usedhit=usedhit;
                for(int i=0; i<Nphlisthit; i++)  m_dEdx_hit[i] = it->get_phlist_hit()[i]; 

                m_parttype = parType_temp;
                m_prob = probpid_temp;
                m_expect = expect_temp;
                m_sigma = sigma_temp;
                m_chidedx = chidedx_temp;
                m_chidedxe=it->pchi_dedx()[0];
                m_chidedxmu=it->pchi_dedx()[1];
                m_chidedxpi=it->pchi_dedx()[2];
                m_chidedxk=it->pchi_dedx()[3];
                m_chidedxp=it->pchi_dedx()[4];
                for(int i=0;i<5;i++)
                {
                    m_probpid[i]= it->pprob()[i];
                    m_expectid[i]= it->pexpect()[i];
                    m_sigmaid[i]= it->pexp_sigma()[i];
                }

                StatusCode status = m_nt1->write();
                if ( status.isFailure() )  
                {
                    log << MSG::ERROR << "Cannot fill Ntuple n103" << endreq;
                } 
            }

            log<< MSG::INFO<<"-----------------Summary of this ExTrack----------------"<<endreq
                <<"dEdx_mean: "<<dEdx_meas<<"  type: "<<parType_temp<<" probpid: "<<probpid_temp
                <<" expect: "<<expect_temp<<" sigma: "<<sigma_temp<<" chidedx: "<<chidedx_temp<<endreq<<endreq;

            dedxList->push_back( resdedx );            
        }//ExTrk loop end
    } //doNewGlobal==0 END . . .


    // check MdcDedxCol registered
    SmartDataPtr<RecMdcDedxCol> newexCol(eventSvc(),"/Event/Recon/RecMdcDedxCol");
    if (!newexCol) 
    { 
        log << MSG::FATAL << "Could not find RecMdcDedxCol" << endreq;
        return( StatusCode::SUCCESS);
    }       
    log << MSG::DEBUG << "----------------Begin to check RecMdcDedxCol-----------------"<<endreq; 
    RecMdcDedxCol::iterator it_dedx = newexCol->begin();
    for( ; it_dedx != newexCol->end(); it_dedx++) 
    {
        log << MSG::INFO << "retrieved MDC dE/dx:" << endreq
            << "dEdx Id: " << (*it_dedx)->trackId()
            << "   part Id: " << (*it_dedx)->particleType()
            << "   measured dEdx:  " << (*it_dedx)->probPH()
            << "   dEdx std:  " << (*it_dedx)->normPH() << endreq
            << "hits on track:   "<<(*it_dedx)->numTotalHits()
            << "   usedhits:  " << (*it_dedx)->numGoodHits() << endreq
            << "Electron: expect: " << (*it_dedx)->getDedxExpect(0)
            << "   sigma: " << (*it_dedx)->getSigmaDedx(0)
            << "   chi: " << (*it_dedx)->chi(0)
            << "   prob: " << (*it_dedx)->getPidProb(0) << endreq
            << "Muon: expect: " << (*it_dedx)->getDedxExpect(1)
            << "   sigma: " << (*it_dedx)->getSigmaDedx(1)  
            << "   chi: " << (*it_dedx)->chi(1)
            << "   prob: " << (*it_dedx)->getPidProb(1) << endreq
            << "Pion: expect: " << (*it_dedx)->getDedxExpect(2)
            << "   sigma: " << (*it_dedx)->getSigmaDedx(2)
            << "   chi: " << (*it_dedx)->chi(2)
            << "   prob: " << (*it_dedx)->getPidProb(2) << endreq
            << "Kaon: expect: " << (*it_dedx)->getDedxExpect(3)
            << "   sigma: " << (*it_dedx)->getSigmaDedx(3)
            << "   chi: " << (*it_dedx)->chi(3)
            << "   prob: " << (*it_dedx)->getPidProb(3) << endreq
            << "Proton: expect: " << (*it_dedx)->getDedxExpect(4)
            << "   sigma: " << (*it_dedx)->getSigmaDedx(4)
            << "   chi: " << (*it_dedx)->chi(4)
            << "   prob: " << (*it_dedx)->getPidProb(4) << endreq;
    }
    return StatusCode::SUCCESS;
}

const std::vector<MdcDedxTrk>&MdcDedxRecon::tracks(void) const
{
    return ex_trks;
}

void MdcDedxRecon::clearTables() {}

void MdcDedxRecon::mdctrackrec(RecMdcTrackCol::iterator trk, Identifier mdcid, double tes, int runNO,int runFlag, MsgStream log )
{
    vector<double> phlist;  //dE/dx only after hit level correction
    vector<double> phlist_hit; //dE/dx after hit and track level correction
    double m_d0E=0,m_phi0E=0,m_cpaE=0,m_z0E=0,phi0=0,costheta=0,sintheta=0,m_Pt=0,m_P=0;
    int charge=0,layid=0,localwid=0,w0id=0,wid=0,lr=-2;
    double adc_raw=0,tdc_raw=0,zhit=0,driftd=0,driftD=0,driftT=0,dd=0,eangle=0;
    int Nhits=0;
    double ph=0,ph_hit=0,pathlength=0,rphi_path=0;

    MdcDedxTrk trk_ex( **trk);
    log << MSG::DEBUG <<" %%%%% trk id = "<<trk_ex.trk_id() <<endreq;
    log << MSG::DEBUG <<" %%%%% initial id = "<<(*trk)->trackId() <<endreq;

    HepVector a = (*trk)->helix();
    HepSymMatrix tkErrM = (*trk)->err();
    m_d0E   = tkErrM[0][0];
    m_phi0E = tkErrM[1][1];
    m_cpaE  = tkErrM[2][2];
    m_z0E   = tkErrM[3][3];

    HepPoint3D IP(0,0,0);
    Dedx_Helix exhel(IP, a);  //initialize class Dedx_Helix for DedxCorrecSvc
    log << MSG::DEBUG <<"MDC Helix 5 parameters: "<<a[0]<<"     "<<a[1]<<"      "<<a[2]<<"      "<<a[3]<<"     "<<a[4]<<endreq;
    //cout<<"MDC Helix 5 parameters: "<<a[0]<<"     "<<a[1]<<"      "<<a[2]<<"      "<<a[3]<<"     "<<a[4]<<endl;
    phi0= a[1];
    costheta = cos(M_PI_2-atan(a[4]));
    //cout<<"track theta: "<<M_PI_2-atan(a[4])<<" extrack theta: "<<trk_ex.theta()<<endl;
    sintheta = sin(M_PI_2-atan(a[4]));
    m_Pt = 1.0/fabs( a[2] );
    m_P = m_Pt*sqrt(1 + a[4]*a[4]);
    charge = ( a[2] > 0 )? 1 : -1;
    //cout<<"track charge: "<<charge<<" extrack charge: "<<trk_ex.charge()<<endl;
    dedxhitrefvec = new DedxHitRefVec;


    HitRefVec gothits= (*trk)->getVecHits();
    Nhits = gothits.size();
    log << MSG::DEBUG << "hits size  = " <<gothits.size()<<endreq;
    //if(gothits.size()<15) cout<<"eventNO : "<<eventNO<<"  hits : "<<gothits.size()<<endl;

    RecMdcKalHelixSeg* mdcKalHelixSeg = 0;
    HitRefVec::iterator it_gothit = gothits.begin();
    for( ; it_gothit != gothits.end(); it_gothit++)
    {
        mdcid = (*it_gothit)->getMdcId();
        layid =  MdcID::layer(mdcid);
        localwid = MdcID::wire(mdcid);
        w0id = geosvc->Layer(layid)->Wirst();
        wid = w0id + localwid;
        adc_raw = (*it_gothit)->getAdc();
        tdc_raw = (*it_gothit)->getTdc();
        log << MSG::INFO <<"hit layer id  " <<layid<< " wire id: " <<localwid<< " adc_raw: " <<adc_raw<<" tdc_raw: "<<tdc_raw<<endreq;
        zhit = (*it_gothit)->getZhit();
        lr = (*it_gothit)->getFlagLR();
        if(lr == 2) cout<<"lr = "<<lr<<endl;
        if(lr == 0 || lr == 2) driftD = (*it_gothit)->getDriftDistLeft();
        else  driftD = (*it_gothit)->getDriftDistRight();
        //cout<<"lr: "<<lr<<"  driftD: "<<driftD<<endl;
        driftd = abs(driftD);
        dd = (*it_gothit)->getDoca();
        if(lr == 0 || lr == 2 ) dd = -abs(dd);
        if(lr == 1 ) dd = abs(dd);
        driftT = (*it_gothit)->getDriftT();

        eangle = (*it_gothit)->getEntra();
        eangle = eangle/M_PI;
        pathlength=exsvc->PathL( ntpFlag, exhel, layid, localwid, zhit);
        rphi_path=pathlength * sintheta;

        //cout<<"ph para check: "<<"adc_raw: "<<adc_raw<<"  dd: "<<dd<<" eangle: "<<eangle<<" zhit: "<<zhit<<" costheta: "<<costheta<<endl;
        ph=exsvc->StandardHitCorrec(0,runFlag,ntpFlag,runNO,pathlength,wid,layid,adc_raw,dd,eangle,costheta);
        ph_hit = exsvc->StandardTrackCorrec(0,runFlag, ntpFlag, runNO, ph, costheta,tes );
        //if(pathlength == -1)
        //cout<<"parameter0: "<<"eventNO: "<<eventNO<<" layid: "<<layid<<" localwid: "<<localwid<<" driftd: "<<driftd<<" rphi_path: "<<rphi_path<<" pathlength: "<<pathlength<<" ph: "<<ph<<" ph_hit: "<<ph_hit<<endl;

        if(runNO<=5911 && runNO>=5854 && layid ==14) continue;
        if(runNO<0)
        {
            if (layid<8)
            {
                if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Iner_RPhi_PathMinCut || driftd>Iner_DriftDistCut) continue;
            }
            else if(layid >= 8)
            {
                if(adc_raw<MinHistValue || adc_raw>MaxHistValue ||rphi_path>RPhi_PathMaxCut || rphi_path<Out_RPhi_PathMinCut || driftd>Out_DriftDistCut ) continue; 
            }
        }
        else if(runNO>=0)
        {
            if(layid <8)
            {
                if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Iner_RPhi_PathMinCut || driftd>Iner_DriftDistCut) continue;
            }
            else if(layid >= 8)
            {
                if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Out_RPhi_PathMinCut || driftd>Out_DriftDistCut ) continue;
            }
        }
        //cout<<"recAlg=2 para mdc:  m_phraw: "<<adc_raw<<" m_exraw: "<<ph_hit<<" m_doca: "<<dd<<" m_eangle: "<<eangle<<" m_costheta: "<<costheta<<endl;

        if (ph<0.0||ph==0) continue;
        else
        {
            //-----------------------put data into TDS-----------------------------//
            dedxhit = new RecMdcDedxHit;
            dedxhit->setMdcHit(*it_gothit);
            dedxhit->setMdcKalHelixSeg(mdcKalHelixSeg);
            dedxhit->setMdcId(mdcid);
            dedxhit->setFlagLR(lr);
            dedxhit->setTrkId(trk_ex.trk_id());
            dedxhit->setDedx(ph_hit);
            dedxhit->setPathLength(pathlength);

            //---------------------------Fill root file----------------------------//
            if(m_rootfile!= "no rootfile")
            {
                m_hitNo_wire->Fill(wid);
            }

            //-------------------------Fill ntuple n102---------------------------//
            if ( ntpFlag ==2 )
            {
                m_charge1 = (*trk)->charge();
                m_t01 = tes;
                m_driftT = driftT;
                m_tdc_raw = tdc_raw;
                m_phraw = adc_raw;
                m_exraw = ph_hit;
                m_localwid = localwid;
                m_wire = wid;
                m_runNO1 = runNO;
                m_nhit_hit = Nhits;
                m_doca_in = dd;
                m_doca_ex = dd;
                m_driftdist = driftD;
                m_eangle = eangle;
                m_costheta1 = costheta;
                m_pathL = pathlength;
                m_layer = layid;
                m_ptrk1 = m_P;
                //cout<<"adc_raw : "<<adc_raw<<"     "<<ph_hit<<"     "<<dd_in<<"  "<<dd_ex<<"  "<<eangle<<endl;
                m_trackId1 = trk_ex.trk_id();
                StatusCode status2 = m_nt2->write();
                if ( status2.isFailure() )
                  log << MSG::ERROR << "Cannot fill Ntuple n102" << endreq;
            }
            if(layid>3)   
            {
                phlist.push_back(ph);
                phlist_hit.push_back(ph_hit);
            }
        }
        dedxhitList->push_back( dedxhit );
        dedxhitrefvec->push_back( dedxhit );
    }//end of hit loop
    trk_ex.set_phlist( phlist );
    trk_ex.set_phlist_hit( phlist_hit );
    trk_ex.setVecDedxHits( *dedxhitrefvec );
    ex_trks.push_back(trk_ex );
    m_trkalgs.push_back(m_trkalg);

    delete dedxhitrefvec;
    phlist.clear();
    phlist_hit.clear();
    if(ntpFlag>0) trackNO2++;
}


void MdcDedxRecon::kaltrackrec(RecMdcKalTrackCol::iterator trk, Identifier mdcid, double tes, int runNO,int runFlag, MsgStream log )
{
    vector<double> phlist;  //dE/dx only after hit level correction
    vector<double> phlist_hit; //dE/dx after hit and track level correction
    double m_d0E=0,m_phi0E=0,m_cpaE=0,m_z0E=0,phi0=0,costheta=0,sintheta=0,m_Pt=0,m_P=0;
    int charge=0,layid=0,localwid=0,w0id=0,wid=0,lr=-2;
    double p_hit=0,adc_raw=0,tdc_raw=0,zhit=0,driftd=0,driftD=0,driftT=0,dd_in=0,dd_ex=0,eangle=0;
    int Nhits=0;
    double ph=0,ph_hit=0,pathlength=0,rphi_path=0;

    MdcDedxTrk trk_ex( **trk, ParticleFlag);
    log << MSG::DEBUG <<" %%%%% trk id = "<<trk_ex.trk_id() <<endreq;
    log << MSG::DEBUG <<" %%%%% initial id = "<<(*trk)->trackId() <<endreq;

    HepVector a;
    HepSymMatrix tkErrM;
    if(ParticleFlag>-1&&ParticleFlag<5)
    {
        DstMdcKalTrack* dstTrk = *trk;
        a = dstTrk->getFHelix(ParticleFlag);
        tkErrM = dstTrk->getFError(ParticleFlag);
    }
    else
    {
        a = (*trk)->getFHelix();
        tkErrM = (*trk)->getFError();
    }
    log << MSG::DEBUG <<"FHelix 5 parameters: "<<a[0]<<"     "<<a[1] <<"      "<<a[2]<<"      "<<a[3]<<"     "<<a[4]<<endreq;
    //cout<<"FHelix 5 parameters: "<<a[0]<<"     "<<a[1] <<"      "<<a[2]<<"      "<<a[3]<<"     "<<a[4]<<endl;  //getFHelix is first layer of MdcKalTrack;

    m_d0E   = tkErrM[0][0];
    m_phi0E = tkErrM[1][1];
    m_cpaE  = tkErrM[2][2];
    m_z0E   = tkErrM[3][3];

    phi0= a[1];
    costheta = cos(M_PI_2-atan(a[4]));
    //cout<<"track theta: "<<M_PI_2-atan(a[4])<<" extrack theta: "<<trk_ex.theta()<<endl;  //theta in trk_ex is got by getFHelix();
    sintheta = sin(M_PI_2-atan(a[4]));
    m_Pt = 1.0/fabs( a[2] );
    m_P = m_Pt*sqrt(1 + a[4]*a[4]);
    //cout<<"track ptrk: "<<m_P<<" extrack ptrk: "<<trk_ex.P()<<endl;
    charge = ( a[2] > 0 )? 1 : -1;
    //cout<<"track charge: "<<charge<<" extrack charge: "<<(*trk)->charge()<<endl;
    dedxhitrefvec = new DedxHitRefVec;


    HelixSegRefVec gothits= (*trk)->getVecHelixSegs(ParticleFlag);
    //HelixSegRefVec gothits= (*trk)->getVecHelixSegs();
    //if not set ParticleFlag, we will get the last succefully hypothesis;
    Nhits = gothits.size();
    log << MSG::DEBUG << "hits size  = " <<gothits.size()<<endreq;
    //if(gothits.size()<15) cout<<"eventNO : "<<eventNO<<"  hits : "<<gothits.size()<<endl;

    RecMdcHit* mdcHit = 0;
    HelixSegRefVec::iterator it_gothit = gothits.begin();
    for( ; it_gothit != gothits.end(); it_gothit++)
    {
        HepVector a_hit_in = (*it_gothit)->getHelixIncl();
        //HepVector a_hit_ex = (*it_gothit)->getHelixExcl(); //same with getHelixIncl()
        HepPoint3D IP(0,0,0);
        Dedx_Helix exhel_hit_in(IP, a_hit_in);
        //Dedx_Helix exhel_hit_ex(IP, a_hit_ex);
        p_hit = 1.0/fabs(a_hit_in(3))*sqrt(1+a_hit_in(5)*a_hit_in(5));
        //cout<<"getHelixIncl 5 para: "<<a_hit_in[0]<<"     "<<a_hit_in[1] <<"      "<<a_hit_in[2]<<"      "<<a_hit_in[3]<<"     "<<a_hit_in[4]<<endl;

        mdcid = (*it_gothit)->getMdcId();
        layid =  MdcID::layer(mdcid);
        localwid = MdcID::wire(mdcid);                    
        w0id = geosvc->Layer(layid)->Wirst();
        wid = w0id + localwid;
        adc_raw = (*it_gothit)->getAdc();
        tdc_raw = (*it_gothit)->getTdc();
        log << MSG::INFO <<"hit layer id  " <<layid<< " wire id: " <<localwid<< " adc_raw: " <<adc_raw<<" tdc_raw: "<<tdc_raw<<endreq;
        zhit = (*it_gothit)->getZhit();
        lr = (*it_gothit)->getFlagLR();
        if(lr == 2) cout<<"lr = "<<lr<<endl;
        driftD = (*it_gothit)->getDD();
        driftd = abs(driftD);
        driftT = (*it_gothit)->getDT();
        dd_in = (*it_gothit)->getDocaIncl(); //getDocaIncl() include fit unused hit
        dd_ex = (*it_gothit)->getDocaExcl(); //getDocaExcl() exclude fit unused hit

        if(lr==-1 || lr == 2) {dd_in = dd_in; dd_ex = dd_ex;}
        else if(lr ==1) {dd_in = -dd_in; dd_ex = -dd_ex;}
        //dd = dd/doca_norm[layid];
        //cout<<"lr: "<<lr<<" dd_in: "<<dd_in<<" dd_ex: "<<dd_ex<<endl;

        eangle = (*it_gothit)->getEntra();
        eangle = eangle/M_PI;
        pathlength=exsvc->PathL( ntpFlag, exhel_hit_in, layid, localwid, zhit);
        rphi_path=pathlength * sintheta;

        //cout<<"ph para check: "<<"runFlag: "<<runFlag<<" runNO: "<<runNO<<" pathlength: "<<pathlength<<" wid: "<<wid<<" layid: "<<layid<<" adc_raw: "<<adc_raw<<" dd_in: "<<dd_in<<" eangle: "<<eangle<<" zhit: "<<zhit<<" costheta: "<<costheta<<endl;
        ph=exsvc->StandardHitCorrec(0,runFlag,ntpFlag,runNO,pathlength,wid,layid,adc_raw,dd_in,eangle,costheta);
        //cout<<"tes= "<<tes<<endl;
        ph_hit = exsvc->StandardTrackCorrec(0,runFlag, ntpFlag, runNO, ph, costheta,tes );
        //cout<<"adc_raw=  "<<adc_raw<<" ph= "<<ph<<endl;
        //cout<<"adc_raw = "<<adc_raw<<" ph_hit= "<<ph_hit<<endl;
        //if(pathlength == -1)  
        //cout<<"parameter1: "<<" layid: "<<layid<<" localwid: "<<localwid<<" driftd: "<<driftd<<" rphi_path: "<<rphi_path<<" pathlength: "<<pathlength<<" ph: "<<ph<<"  ph_hit: "<<ph_hit<<endl;

        if(runNO<=5911 && runNO>=5854 && layid ==14) continue;
        if(runNO<0)
        {
            if (layid<8)
            {
                if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Iner_RPhi_PathMinCut || driftd>Iner_DriftDistCut) continue;
            }
            else if(layid >= 8)
            {
                if(adc_raw<MinHistValue || adc_raw>MaxHistValue ||rphi_path>RPhi_PathMaxCut || rphi_path<Out_RPhi_PathMinCut || driftd>Out_DriftDistCut ) continue;
            }
        }
        else if(runNO>=0)
        {
            if(layid <8)
            {
                if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Iner_RPhi_PathMinCut || driftd>Iner_DriftDistCut) continue;
            }
            else if(layid >= 8)
            {
                if(adc_raw<MinHistValue || adc_raw>MaxHistValue || rphi_path>RPhi_PathMaxCut || rphi_path<Out_RPhi_PathMinCut || driftd>Out_DriftDistCut ) continue;
            }
        }
        //cout<<"recAlg=2 para kal:  m_phraw: "<<adc_raw<<" m_exraw: "<<ph_hit<<" m_doca: "<<dd_in<<" m_eangle: "<<eangle<<" m_costheta: "<<costheta<<endl;   

        if (ph<0.0||ph==0) continue;
        else
        {
            //-----------------------put data into TDS-----------------------------//
            dedxhit = new RecMdcDedxHit;
            dedxhit->setMdcHit(mdcHit);
            dedxhit->setMdcKalHelixSeg(*it_gothit);
            dedxhit->setMdcId(mdcid);
            dedxhit->setFlagLR(lr);
            dedxhit->setTrkId(trk_ex.trk_id());
            dedxhit->setDedx(ph_hit);
            dedxhit->setPathLength(pathlength);

            //---------------------------Fill root file----------------------------//
            if(m_rootfile!= "no rootfile")
            {
                m_hitNo_wire->Fill(wid);
            }

            //-------------------------Fill ntuple n102---------------------------//
            if ( ntpFlag ==2 )
            {
                m_charge1 = (*trk)->charge();
                m_t01 = tes;
                m_driftT = driftT;
                m_tdc_raw = tdc_raw;
                m_phraw = adc_raw;
                m_exraw = ph_hit;
                m_localwid = localwid;
                m_wire = wid;
                m_runNO1 = runNO;
                m_nhit_hit = Nhits;
                m_doca_in = dd_in;
                m_doca_ex = dd_ex;
                m_driftdist = driftD;
                m_eangle = eangle;
                m_costheta1 = costheta;
                m_pathL = pathlength;
                m_layer = layid;
                m_ptrk1 = m_P;
                m_ptrk_hit = p_hit;
                //cout<<"adc_raw : "<<adc_raw<<"     "<<ph_hit<<"     "<<dd_in<<"  "<<dd_ex<<"  "<<eangle<<endl;
                m_trackId1 = trk_ex.trk_id();
                StatusCode status2 = m_nt2->write();
                if ( status2.isFailure() )
                  log << MSG::ERROR << "Cannot fill Ntuple n102" << endreq;
            }
            if(layid>3)   
            {
                phlist.push_back(ph);
                phlist_hit.push_back(ph_hit);
            }
        }
        dedxhitList->push_back( dedxhit );
        dedxhitrefvec->push_back( dedxhit );
    }//end of hit loop
    trk_ex.set_phlist( phlist );
    trk_ex.set_phlist_hit( phlist_hit );
    trk_ex.setVecDedxHits( *dedxhitrefvec );
    ex_trks.push_back(trk_ex );
    m_trkalgs.push_back(m_trkalg);

    delete dedxhitrefvec;
    phlist.clear();
    phlist_hit.clear();
    if(ntpFlag>0) trackNO3++;
}

void MdcDedxRecon::switchtomdctrack(int trkid,Identifier mdcid, double tes, int runNO,int runFlag, MsgStream log)
{
    //retrieve  MdcTrackCol from TDS
    SmartDataPtr<RecMdcTrackCol> newtrkCol(eventSvc(),"/Event/Recon/RecMdcTrackCol");
    if (!newtrkCol) 
    {
        log << MSG::WARNING << "Could not find RecMdcTrackCol in switchtomdctrack" << endreq;
        return ;
    }

    RecMdcTrackCol::iterator trk = newtrkCol->begin();
    for( ; trk != newtrkCol->end(); trk++) 
    {
        if( (*trk)->trackId()==trkid)
        {
            HitRefVec gothits= (*trk)->getVecHits();
            if(gothits.size()>=2)
              mdctrackrec(trk,mdcid,tes,runNO,runFlag,log);
        }
    }
}
