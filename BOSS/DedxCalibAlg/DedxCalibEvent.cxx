#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "EventModel/EventHeader.h"
#include "EvTimeEvent/RecEsTime.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcKalHelixSeg.h"
#include "MdcRecEvent/RecMdcDedx.h"
#include "MdcRecEvent/RecMdcDedxHit.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"

#include <TMath.h>

#include "DedxCalibAlg/DedxCalibEvent.h"

typedef std::vector<int> Vint;

using namespace std;
using CLHEP::HepVector;


DedxCalibEvent::DedxCalibEvent(const std::string& name, ISvcLocator* pSvcLocator): DedxCalib(name, pSvcLocator) {}


void DedxCalibEvent::initializing()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "DedxCalibEvent::initializing()" << endreq;

    StatusCode status;
    NTuplePtr  nt1(ntupleSvc(),"FILE100/n103");
    if ( nt1 )
      m_nt1 = nt1;
    else 
    {
        m_nt1= ntupleSvc()->book("FILE100/n103",CLID_ColumnWiseTuple,"dEdx per track");
        if ( m_nt1 )
        {
            status = m_nt1->addItem("ptrk",m_ptrk);
            status = m_nt1->addItem("ptrk_t",m_ptrk_t);
            status = m_nt1->addItem("sintheta",m_sintheta);
            status = m_nt1->addItem("costheta",m_costheta);
            status = m_nt1->addItem("charge",m_charge);
            status = m_nt1->addItem("runNO",m_runNO);
            status = m_nt1->addItem("runFlag",m_runFlag);
            status = m_nt1->addItem("evtNO",m_evtNO);
            status = m_nt1->addItem("t0",m_t0);
            status = m_nt1->addItem("trackId",m_trackId);
            status = m_nt1->addItem("poca_x",m_poca_x);
            status = m_nt1->addItem("poca_y",m_poca_y);
            status = m_nt1->addItem("poca_z",m_poca_z);
            status = m_nt1->addItem("recalg",m_recalg);
            status = m_nt1->addItem("nhit",m_nhit);
            status = m_nt1->addItem("nhits",m_nhits);
            status = m_nt1->addItem("usedhit",m_usedhit);

            status = m_nt1->addItem("ndedxhit",m_nphlisthit,0,100);
            status = m_nt1->addIndexedItem("dEdx_hit",m_nphlisthit,m_dEdx_hit); 
            status = m_nt1->addIndexedItem("pathlength_hit",m_nphlisthit,m_pathlength_hit);
            status = m_nt1->addIndexedItem("wid_hit",m_nphlisthit,m_wid_hit);
            status = m_nt1->addIndexedItem("layid_hit",m_nphlisthit,m_layid_hit);
            status = m_nt1->addIndexedItem("dd_in_hit",m_nphlisthit,m_dd_in_hit);
            status = m_nt1->addIndexedItem("eangle_hit",m_nphlisthit,m_eangle_hit);
            status = m_nt1->addIndexedItem("zhit_hit",m_nphlisthit,m_zhit_hit);

            //status = m_nt1->addItem("dEdx_meas_hit", m_dEdx_meas_hit);
            status = m_nt1->addItem("dEdx_meas", m_dEdx_meas);
            //status = m_nt1->addItem("dEdx_meas_esat", m_dEdx_meas_esat);
            //status = m_nt1->addItem("dEdx_meas_norun", m_dEdx_meas_norun);

            status = m_nt1->addItem("type",m_parttype);
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

    NTuplePtr  nt2(ntupleSvc(),"FILE100/n102");
    if ( nt2 )  m_nt2 = nt2;
    else
    {
        m_nt2= ntupleSvc()->book("FILE100/n102",CLID_RowWiseTuple,"dE/dx per hit");
        if ( m_nt2 )
        {
            status = m_nt2->addItem("charge",m_charge1);
            status = m_nt2->addItem("adc_raw",m_phraw);
            status = m_nt2->addItem("exraw",m_exraw);
            status = m_nt2->addItem("runNO",m_runNO1);
            status = m_nt2->addItem("runFlag",m_runFlag1);
            status = m_nt2->addItem("wire",m_wire);
            status = m_nt2->addItem("doca_in",m_doca_in);
            status = m_nt2->addItem("doca_ex",m_doca_ex);
            status = m_nt2->addItem("driftdist",m_driftdist);
            status = m_nt2->addItem("eangle",m_eangle);
            status = m_nt2->addItem("zhit",m_zhit);
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

void DedxCalibEvent::genNtuple()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "DedxCalibEvent::genNtuple()" << endreq;

    SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
    if (!eventHeader)
    {
        log << MSG::INFO << "Could not find Event Header" << endreq;
        return;
    }
    int eventNO = eventHeader->eventNumber();
    int runNO = eventHeader->runNumber();
    log << MSG::INFO << "now begin the event:  runNO= "<<runNO<<" eventNO= "<< eventNO<< endreq;

    int runFlag=0;  //data type flag
    if(runNO<RUN0) runFlag =0;
    if(runNO>=RUN1 && runNO<RUN2) runFlag =1;
    if(runNO>=RUN2 && runNO<RUN3) runFlag =2;
    if(runNO>=RUN3 && runNO<RUN4) runFlag =3;   
    if(runNO>=RUN4 && runNO<RUN5) runFlag =4;   //jpsi
    if(runNO>=RUN5 && runNO<RUN6) runFlag =5;   //psipp
    if(runNO>=RUN6 && runNO<RUN7) runFlag =6;   //psi4040, psip, jpsi...
    if(runNO>=RUN7) runFlag =7;  //psip

    double tes = -999.0;
    int esTimeflag = -1; 
    SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
    if( (!aevtimeCol) || (aevtimeCol->size()==0) ){
        tes = -9999.0;
        esTimeflag = -1;
    }else{
        RecEsTimeCol::iterator iter_evt = aevtimeCol->begin();
        for(; iter_evt!=aevtimeCol->end(); iter_evt++){
            tes = (*iter_evt)->getTest();
            esTimeflag = (*iter_evt)->getStat();
        }
    }
    if(runFlag ==2) {if( tes>1000 ) return;}
    else if(runFlag ==3 ){if (tes>700 ) return;}
    else {if (tes>1400 ) return;}

    SmartDataPtr<RecMdcDedxCol> newexCol(eventSvc(),"/Event/Recon/RecMdcDedxCol");
    if (!newexCol)
    {
        log << MSG::FATAL << "Could not find RecMdcDedxCol" << endreq;
        return;
    }

    Vint iGood;
    iGood.clear();
    int nCharge = 0;
    double db=0,dz=0,pt0=0,charge0=0;
    for(RecMdcDedxCol::iterator it = newexCol->begin(); it != newexCol->end(); it++)
    {
        HepVector a;
        if((*it)->getMdcKalTrack())
        {
            RecMdcKalTrack* trk = (*it)->getMdcKalTrack();
            if(ParticleFlag>-1&&ParticleFlag<5)
            {
                DstMdcKalTrack* dstTrk = trk;
                a = dstTrk->getZHelix(ParticleFlag);
            }
            else
              a = trk->getZHelix();
        }
        else if((*it)->getMdcTrack())
        {
            RecMdcTrack* trk = (*it)->getMdcTrack();
            a = trk->helix();
        }
        else continue;
        db = a[0];
        dz = a[3];
        pt0 = fabs(1.0/a[2]);
        charge0 = ( a[2] > 0 )? 1 : -1;

        //cout<<"db: "<<db<<" dz: "<<dz<<" pt0: "<<pt0<<" charge0: "<<charge0<<endl;
        if(fabs(dz) >= VZ0CUT) continue;
        if(db >= VR0CUT) continue;
        if(pt0 >= PT0HighCut) continue;
        if(pt0 <= PT0LowCut) continue;
        iGood.push_back((*it)->trackId());
        nCharge += charge0;
    }
    //cout<<"iGood.size()= "<<iGood.size()<<" nCharge= "<<nCharge<<endl;
    if((m_eventType == "isBhabha")||(m_eventType == "isRadBhabha"))
    {
        if(iGood.size()!=2 || nCharge!=0 )  return;
    }


    //cout<<"begin to read RecMdcDedxCol!!!!"<<endl;
    double poca_x=0,poca_y=0,poca_z=0;
    float sintheta=0,costheta=0,ptrk=0,ptrk_t=0,charge=0,trackId=0;
    int Nhit=0,usedhit=0,Nhits=0,Nphlisthit=0;
    double dEdx_meas_hit=0, dEdx_meas=0,dEdx_meas_esat=0,dEdx_meas_norun=0;
    double dEdx_hit[100]={0},pathlength_hit[100]={0},wid_hit[100]={0},layid_hit[100]={0},dd_in_hit[100]={0},eangle_hit[100]={0},zhit_hit[100]={0};
    int trk_recalg = -1;
    Identifier mdcid;

    for(RecMdcDedxCol::iterator it = newexCol->begin(); it != newexCol->end(); it++)
    {
        //cout<<"in track iteration!!!!!!!!"<<endl;
        bool flag = false;
        for(int i = 0; i < iGood.size(); i++)
        {
            if((*it)->trackId()==iGood[i]) flag=true;
        }
        if(flag==false) continue;

        HepVector a;
        HepSymMatrix tkErrM;
        if((*it)->getMdcKalTrack())
        {
            //cout<<"can get MdcKalTrack!!!!!!!!!"<<endl;
            poca_x = (*it)->getMdcKalTrack()->x();  //get poca, default pid is pion; change pid using setPidType();
            poca_y = (*it)->getMdcKalTrack()->y();
            poca_z = (*it)->getMdcKalTrack()->z();

            RecMdcKalTrack* trk = (*it)->getMdcKalTrack();
            //cout<<"ParticleFlag= "<<ParticleFlag<<endl;
            if(ParticleFlag>-1&&ParticleFlag<5)
            {
                DstMdcKalTrack* dstTrk = trk;
                a = dstTrk->getFHelix(ParticleFlag);
                tkErrM = dstTrk->getFError(ParticleFlag);
            }
            else
            {
                a = trk->getFHelix();
                tkErrM = trk->getFError();
            }
        }
        else if((*it)->getMdcTrack())
        {
            poca_x = (*it)->getMdcTrack()->x();
            poca_y = (*it)->getMdcTrack()->y();
            poca_z = (*it)->getMdcTrack()->z();

            RecMdcTrack* trk = (*it)->getMdcTrack();
            a = trk->helix();
            tkErrM = trk->err();
        }
        else continue;

        sintheta = sin(M_PI_2 - atan(a[4]));
        costheta = cos(M_PI_2 - atan(a[4]));
        ptrk_t = 1.0/fabs( a[2] );
        ptrk = ptrk_t*sqrt(1 + a[4]*a[4]);
        charge = ( a[2] > 0 )? 1 : -1;

        Nhit = (*it)->numTotalHits(); //total hits on track used as sample; 
        Nhits = ((*it)->getVecDedxHits()).size(); //dedx hits on this track, they are put in phlist if layid>3 
        usedhit = (*it)->numGoodHits(); //hits after truncting phlist and used in cal dE/dx value;
        trk_recalg = (*it)->status();
        trackId = (*it)->trackId();

        if(m_eventType == "isBhabha")
        {
            if(runFlag ==3 &&(ptrk>1.88 || ptrk<1.80)) continue;
            if(runFlag ==4 &&(ptrk>1.72 || ptrk<1.45)) continue;
            if(runFlag ==5 &&(ptrk>2.00 || ptrk<1.70))  continue;
            if(runFlag ==6 &&(ptrk>1.90 || ptrk<1.00)) continue;
            if(runFlag ==7 &&(ptrk>1.90 || ptrk<1.40)) continue;
            if(abs(costheta)>0.9) continue;

            if(Nhit<20) continue;
            if(usedhit<6) continue;
        }


        int layid=0,localwid=0,w0id=0,wid=0,lr=0;
        double p_hit=0,adc_raw=0,tdc_raw=0,zhit=0,driftd=0,driftD=0,driftT=0,dd_in=0,dd_ex=0,eangle=0;
        double ph=0,pathlength=0,rphi_path=0;
        long k=0;

        DedxHitRefVec gothits = (*it)->getVecDedxHits();
        DedxHitRefVec::iterator it_gothit = gothits.begin();
        for(;it_gothit!=gothits.end(); it_gothit++)
        {
            //cout<<"in hit iteration!!!!!!!!!!!!!!!!!!"<<endl;
            if((*it_gothit)->isMdcHitValid())
            { 
                RecMdcHit* itor = (*it_gothit)->getMdcHit(); 
                mdcid = itor->getMdcId();
                layid =  MdcID::layer(mdcid);
                localwid = MdcID::wire(mdcid);
                w0id = geosvc->Layer(layid)->Wirst();
                wid = w0id + localwid;
                adc_raw = itor->getAdc();
                tdc_raw = itor->getTdc();
                zhit = itor->getZhit();

                lr = itor->getFlagLR();
                if(lr == 2) cout<<"lr = "<<lr<<endl;
                if(lr == 0 || lr == 2) driftD = itor->getDriftDistLeft();
                else  driftD = itor->getDriftDistRight();
                driftd = abs(driftD);
                dd_in = itor->getDoca();
                dd_ex = itor->getDoca();
                if(lr == 0 || lr == 2 ) {dd_in = -abs(dd_in);dd_ex = -abs(dd_ex);}
                if(lr == 1 ) {dd_in = abs(dd_in);dd_ex = abs(dd_ex);}
                driftT = itor->getDriftT();
                eangle = itor->getEntra();
                eangle = eangle/M_PI;
            }
            else if((*it_gothit)->isMdcKalHelixSegValid()) 
            {
                RecMdcKalHelixSeg* itor = (*it_gothit)->getMdcKalHelixSeg();
                HepVector a_hit_in = itor->getHelixIncl();
                p_hit = 1.0/fabs(a_hit_in(3))*sqrt(1+a_hit_in(5)*a_hit_in(5)); 

                mdcid = itor->getMdcId();
                layid =  MdcID::layer(mdcid);
                localwid = MdcID::wire(mdcid);
                w0id = geosvc->Layer(layid)->Wirst();
                wid = w0id + localwid;
                adc_raw = itor->getAdc();
                tdc_raw = itor->getTdc();
                zhit = itor->getZhit();

                lr = itor->getFlagLR();
                if(lr == 2) cout<<"lr = "<<lr<<endl;
                driftD = itor->getDD();
                driftd = abs(driftD);
                driftT = itor->getDT();
                dd_in = itor->getDocaIncl(); //getDocaIncl() include fit unused hit
                dd_ex = itor->getDocaExcl(); //getDocaExcl() exclude fit unused hit
                if(lr==-1 || lr == 2) {dd_in = dd_in; dd_ex = dd_ex;}
                else if(lr ==1) {dd_in = -dd_in; dd_ex = -dd_ex;}
                eangle = itor->getEntra();
                eangle = eangle/M_PI;
            }
            else continue;

            pathlength=(*it_gothit)->getPathLength();
            rphi_path=pathlength*sintheta;
            ph = (*it_gothit)->getDedx();
            if(layid>3)
            {
                dEdx_hit[k]=adc_raw;
                pathlength_hit[k]=pathlength;
                wid_hit[k]=wid;
                layid_hit[k]=layid;
                dd_in_hit[k]=dd_in;
                eangle_hit[k]=eangle;
                zhit_hit[k]=zhit;

                k++;
            }

            //cout<<"begin to Fill Ntuple n102!!!!!!!!!"<<endl;
            m_charge1 = charge;
            m_t01 = tes;
            m_driftT = driftT;
            m_tdc_raw = tdc_raw;
            m_phraw = adc_raw;
            m_exraw = ph;
            m_localwid = localwid;
            m_wire = wid;
            m_runNO1 = runNO;
            m_runFlag1 = runFlag;
            m_doca_in = dd_in;
            m_doca_ex = dd_ex;
            m_driftdist = driftD;
            m_eangle = eangle;
            m_zhit = zhit;
            m_costheta1 = costheta;
            m_pathL = pathlength;
            m_layer = layid;
            m_ptrk1 = ptrk;
            m_ptrk_hit = p_hit;
            m_trackId1 = trackId;

            StatusCode status = m_nt2->write();
            if ( status.isFailure() )
              log << MSG::ERROR << "Cannot fill Ntuple n102" << endreq;
        }

        Nphlisthit = k;  //dedx hits on this track, exclude the first 3 layers
        dEdx_meas = (*it)->probPH();
        //cout<<"dEdx_meas in reconstruction: "<<dEdx_meas<<endl;
        dEdx_meas_esat = (*it)->getDedxEsat();
        dEdx_meas_norun = (*it)->getDedxNoRun();
        dEdx_meas_hit = (*it)->getDedxHit(); 
        //cout<<"dEdx_meas in calibration: "<<dEdx_meas<<endl;

        //cout<<"begin to Fill Ntuple n103!!!!!!!"<<endl;
        m_poca_x = poca_x;
        m_poca_y = poca_y;
        m_poca_z = poca_z;
        m_ptrk_t=ptrk_t;
        m_ptrk=ptrk;
        m_sintheta=sintheta;
        m_costheta=costheta;
        m_charge=charge;
        m_runNO = runNO;
        m_runFlag = runFlag;
        m_evtNO = eventNO;
        m_t0 = tes;
        m_trackId = trackId;
        m_recalg = trk_recalg;

        m_nhit=Nhit;
        m_nhits=Nhits;
        m_nphlisthit=Nphlisthit;
        m_usedhit=usedhit;
        for(int j=0; j<Nphlisthit; j++)
        {
            m_dEdx_hit[j]=dEdx_hit[j];
            m_pathlength_hit[j]=pathlength_hit[j];
            m_wid_hit[j]=wid_hit[j];
            m_layid_hit[j]=layid_hit[j];
            m_dd_in_hit[j]=dd_in_hit[j];
            m_eangle_hit[j]=eangle_hit[j];
            m_zhit_hit[j]=zhit_hit[j];
        }

        //m_dEdx_meas_hit = dEdx_meas_hit;
        m_dEdx_meas = dEdx_meas;
        //m_dEdx_meas_esat = dEdx_meas_esat;
        //m_dEdx_meas_norun = dEdx_meas_norun;

        m_parttype = (*it)->particleId();
        m_chidedxe=(*it)->chiE();
        m_chidedxmu=(*it)->chiMu();
        m_chidedxpi=(*it)->chiPi();
        m_chidedxk=(*it)->chiK();
        m_chidedxp=(*it)->chiP();
        for(int i=0;i<5;i++)
        {
            m_probpid[i]= (*it)->getPidProb(i);
            m_expectid[i]= (*it)->getDedxExpect(i);
            m_sigmaid[i]= (*it)->getSigmaDedx(i);
        }
        StatusCode status = m_nt1->write();
        if ( status.isFailure() )
        {
            log << MSG::ERROR << "Cannot fill Ntuple n103" << endreq;
        } 
    }
    //cout<<"track iteration ended!!!!!!!!!!"<<endl;
}
