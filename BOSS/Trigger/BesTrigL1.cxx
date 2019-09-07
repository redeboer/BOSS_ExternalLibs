#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include <CLHEP/Geometry/Point3D.h>

#include "MdcRawEvent/MdcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "EmcRawEvent/EmcDigi.h"
#include "MucRawEvent/MucDigi.h"
#include "McTruth/MucMcHit.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "TrigEvent/TrigEvent.h"
#include "TrigEvent/TrigData.h"
#include "TrigEvent/TrigGTD.h"
#include "TrigEvent/TrigTOFT.h"
#include "TrigEvent/TrigEACC.h"
#include "RawDataProviderSvc/TofData.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "Identifier/TofID.h"
#include "Identifier/MucID.h"
#include "McTruth/McParticle.h"
#include "EvTimeEvent/RecEsTime.h"
#include "EmcWaveform/EmcWaveform.h"

#include "EmcCalibConstSvc/EmcCalibConstSvc.h"

#include "Trigger/IBesGlobalTrigSvc.h"
#include "Trigger/BesGlobalTrigSvc.h"
#include "Trigger/BesTrigL1.h"
#include "Trigger/AsciiData.h"
#include "Trigger/TrigPara.h"
#include <TGraph.h>
#include <TCanvas.h>
#include "CLHEP/Random/Random.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include <math.h>
#include <map>

using namespace CLHEP;
using namespace std;
using namespace Event;

// Declaration of the Algorithm Factory
//static const  AlgFactory<BesTrigL1>          s_factory ;
//const        IAlgFactory& BesTrigL1Factory = s_factory ;
BesTrigL1::BesTrigL1(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), m_pIBGT(0),passNo(0)
{
  declareProperty("WriteFile", writeFile=0);
  declareProperty("IfOutEvtId",ifoutEvtId=0);
  declareProperty("Input",input="boost.dat");
  declareProperty("Output",output="boostOut.dat");
  declareProperty("OutEvtIdFile",outEvtId="evtId.dat");
  declareProperty("TrigRootFlag", mTrigRootFlag=false);
  declareProperty("RunMode", m_runMode=1);
  declareProperty("ClockShift", clock_shift=0);
  nEvent = 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode BesTrigL1::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
  
  //--------------------------------------
  // define a pointer of trigger service
  //--------------------------------------
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("BesGlobalTrigSvc", m_tmpSvc);
  m_pIBGT = dynamic_cast<BesGlobalTrigSvc* >(m_tmpSvc);
  if(sc!=StatusCode::SUCCESS) {
  log<<MSG::DEBUG<< "Unable to open trigger service"<<endreq;
  }

  // set run mode,  0: online, 1: offline
  m_pIBGT->setRunMode(m_runMode);

  //--------------------------------------------------------------
  // define a pointer of RawDataProviderSvc, used in tof trigger
  //--------------------------------------------------------------
  static const bool CREATEIFNOTTHERE(true);
  sc = service ("RawDataProviderSvc", m_rawDataProviderSvc, CREATEIFNOTTHERE);
  if ( !sc.isSuccess() ) {
    log<<MSG::ERROR  << "Could not load RawDataProviderSvc!" << m_rawDataProviderSvc << endreq;
    return sc;
  }

  //--------------------------------------------------------------
  // use realization service to get trigger configure parameters
  //--------------------------------------------------------------
  IRealizationSvc *tmpReal;
  sc = svcLocator->service("RealizationSvc",tmpReal);
  if (!sc.isSuccess())
  {
     cout << "FATAL:  Could not initialize Realization Service" << endl;
  } else {
    m_RealizationSvc=dynamic_cast<RealizationSvc*>(tmpReal);
  }

  //-----------------------------------------------------------------------
  // use EmcCalibConstSvc to convert crystal id(theta, phi) to global id.
  //-----------------------------------------------------------------------
  sc = svcLocator->service("EmcCalibConstSvc", emcCalibConstSvc);
  if(sc != StatusCode::SUCCESS) {
    cout << "EmcRecDigit2Hit Error: Can't get EmcCalibConstSvc." << endl;
  }

  if(mTrigRootFlag) {
    //-----------------------------------------
    // define ntuples for performance check
    //-----------------------------------------
    NTuplePtr nt(ntupleSvc(),"FILE302/trig1");
    if ( nt ) m_tuple = nt;
    else {
      m_tuple=ntupleSvc()->book("FILE302/trig1",CLID_ColumnWiseTuple,"TrigL1");
      if(m_tuple) {
	sc = m_tuple->addItem ("x",m_wire_x);
	sc = m_tuple->addItem ("y",m_wire_y);
	sc = m_tuple->addItem ("evtId",m_wire_evtId);
	sc = m_tuple->addItem ("delta_t",m_delta_tdc);
      }
      else {
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple) << endmsg;
        return StatusCode::FAILURE;
      }
    }

    NTuplePtr nt1(ntupleSvc(),"FILE302/trig2");
    if ( nt1 ) m_tuple1 = nt1;
    else {
      m_tuple1=ntupleSvc()->book("FILE302/trig2",CLID_ColumnWiseTuple,"TrigL1");
    if( m_tuple1 ) {
          sc = m_tuple1->addItem ("RunId", m_RunId);
          sc = m_tuple1->addItem ("EventId", m_EventId);
          sc = m_tuple1->addItem ("mc_totE_all", m_mc_totE_all);
          sc = m_tuple1->addItem ("data_totE_all", m_data_totE_all);
          sc = m_tuple1->addItem ("mc_wetotE", m_wetotE);
          sc = m_tuple1->addItem ("data_wetotE", m_data_wetotE);
          sc = m_tuple1->addItem ("mc_eetotE", m_eetotE);
          sc = m_tuple1->addItem ("data_eetotE", m_data_eetotE);
          sc = m_tuple1->addItem ("index_btc", m_index_btc, 0, 330); 
          sc = m_tuple1->addIndexedItem ("btc_e", m_index_btc, m_btc_e); 
          sc = m_tuple1->addIndexedItem ("data_btc", m_index_btc, m_data_btc); 
          sc = m_tuple1->addItem ("cond_id", m_cond_id, 0, 48);
          sc = m_tuple1->addIndexedItem ("mc_cond", m_cond_id, m_mc_cond);
          sc = m_tuple1->addIndexedItem ("data_cond", m_cond_id, m_data_cond);
          sc = m_tuple1->addItem ("block_id", m_block_id, 0, 12);
          sc = m_tuple1->addIndexedItem ("mc_blockE", m_block_id, m_mc_blockE);
          sc = m_tuple1->addIndexedItem ("data_blockE", m_block_id, m_data_blockE);
          sc = m_tuple1->addIndexedItem ("R_blockE", m_block_id, m_R_blockE);
        }
        else    {   // did not manage to book the N tuple....
	   log << MSG::ERROR <<"Cannot book N-tuple1:" << long(m_tuple1) << endmsg;
	   return StatusCode::FAILURE;
        }
    }


    NTuplePtr nt2(ntupleSvc(),"FILE302/muc");
    if ( nt2 ) m_tuple2 = nt2;
    else {
      m_tuple2=ntupleSvc()->book("FILE302/muc",CLID_ColumnWiseTuple,"TrigL1");
    if( m_tuple2 ) {
          sc = m_tuple2->addItem ("indexlayerSeg", m_index2, 0, 5000);
          sc = m_tuple2->addIndexedItem ("nlayerSeg", m_index2, m_fireLayer,0,5000);
          sc = m_tuple2->addItem ("indexhitLayer", m_index3, 0, 5000);
          sc = m_tuple2->addIndexedItem ("nhitLayer", m_index3, m_hitLayer, 0, 5000);
          sc = m_tuple2->addItem ("indexhitSeg", m_index4, 0, 5000);
          sc = m_tuple2->addIndexedItem ("nhitSeg", m_index4, m_hitSeg, 0, 5000);
          sc = m_tuple2->addItem ("indexpara", m_index5, 0, 5000);
          sc = m_tuple2->addIndexedItem ("costheta", m_index5, m_costheta, 0, 5000);
          sc = m_tuple2->addIndexedItem ("phi", m_index5, m_phi, 0, 5000);
          sc = m_tuple2->addIndexedItem ("p", m_index5, m_p, 0, 5000);
          sc = m_tuple2->addIndexedItem ("pdgcode", m_index5, m_pdgcode, 0, 5000);
          sc = m_tuple2->addItem ("indexhitphi", m_index6, 0, 5000);
          sc = m_tuple2->addIndexedItem ("hitphi", m_index6, m_hitphi, 0, 5000);

          sc = m_tuple2->addItem ("nlayerEE", m_nlayerEE);
          sc = m_tuple2->addItem ("nlayerBR", m_nlayerBR);
          sc = m_tuple2->addItem ("nlayerWE", m_nlayerWE); 
          sc = m_tuple2->addItem ("nlayerTotal", m_nlayerTotal);
          sc = m_tuple2->addItem ("nhitEE", m_nhitEE);
          sc = m_tuple2->addItem ("nhitBR", m_nhitBR);
          sc = m_tuple2->addItem ("nhitWE", m_nhitWE);
          sc = m_tuple2->addItem ("nhitTotal", m_nhitTotal); 
 
          sc = m_tuple2->addItem ("mumcostheta", m_mumcostheta);
          sc = m_tuple2->addItem ("mumphi", m_mumphi);

          sc = m_tuple2->addItem ("trackfindall", m_trackfindall);
          sc = m_tuple2->addItem ("trackfind3l", m_trackfind3l);
          sc = m_tuple2->addItem ("trackb", m_trackb);
          sc = m_tuple2->addItem ("tracke", m_tracke);
          sc = m_tuple2->addItem ("ntrack1", m_ntrack1);
          sc = m_tuple2->addItem ("ntrack2", m_ntrack2);
          sc = m_tuple2->addItem ("ntrack3", m_ntrack3); 
  
          sc = m_tuple2->addItem ("ngoodevent", m_ngoodevent);
          sc = m_tuple2->addItem ("ngoodtrack", m_ngoodtrack);
        }
        else    {   // did not manage to book the N tuple....
           log << MSG::ERROR <<"Cannot book N-tuple2:" << long(m_tuple2) << endmsg;
           return StatusCode::FAILURE;
        }  
    }
   
    NTuplePtr nt3(ntupleSvc(),"FILE302/trig3");
    if ( nt3 ) m_tuple3 = nt3;
    else {
      m_tuple3=ntupleSvc()->book("FILE302/trig3",CLID_ColumnWiseTuple,"TrigL1");
    if( m_tuple3 ) {
          sc = m_tuple3->addItem ("evtId", m_evtId);
          for(int index = 0; index < 48; index++) {
            std::ostringstream osname1;
            osname1 << "cond"<<index<<"_1";
            std::string name1 = osname1.str();

            std::ostringstream osname2;
            osname2 << "cond"<<index<<"_0";
            std::string name2 = osname2.str();
            m_tuple3->addItem(name1.c_str(), m_condNOne[index]);
            m_tuple3->addItem(name2.c_str(), m_condNZero[index]);
          }
          
        }
        else    {   // did not manage to book the N tuple....
           log << MSG::ERROR <<"Cannot book N-tuple3:" << long(m_tuple3) << endmsg;
           return StatusCode::FAILURE;
        }  
    }
  } 
 
  // pointer of mdc trigger
  m_MdcTSF = MdcTSF::get_Mdc();

  // pointer of tof trigger
  m_TofHitCount = TofHitCount::get_Tof();

  // pointer of emc trigger
  m_emcDigi = EmcTCFinder::get_Emc();

  // pointer of muc trigger
  m_mucDigi = MucTrigHit::get_Muc();

  //-------------------------------------
  // reset total track and event number
  //-------------------------------------
  totalEvent = 0;
  totalTracks = 0;

  if(mTrigRootFlag) {
    sc = service("THistSvc", m_thistsvc);
    if(sc.isFailure() ){
         log << MSG::INFO << "Unable to retrieve pointer to THistSvc" << endreq;
         return sc;
    }
    m_trigCondi_MC = new TH1F( "trgCond_MC", "trgCond_MC", 48, 0, 48 );
    sc = m_thistsvc->regHist("/TRG/trgCond_MC", m_trigCondi_MC);
    m_trigCondi_Data = new TH1F( "trgCond_Data", "trgCond_Data", 48, 0, 48 );
    sc = m_thistsvc->regHist("/TRG/trgCond_Data", m_trigCondi_Data);
  }

  //------------------------------------------------------------------
  // a pointer used to read emc trigger information from eacc board
  //------------------------------------------------------------------
  //eacctrig = new TrigEACC("eacc_trig");

  return StatusCode::SUCCESS;
}
StatusCode BesTrigL1::execute(){
  MsgStream log(msgSvc(),name());

  log<<MSG::DEBUG<< "in execute()" <<endreq;
  
  int event, run;
  ifpass = false; 
 
  //-------------------
  // get event header 
  //-------------------
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
      log << MSG::FATAL << "Could not find Event Header" << endreq;
      return( StatusCode::FAILURE);
  }
  run   = eventHeader->runNumber();
  event = eventHeader->eventNumber();

  if(mTrigRootFlag) {
    // fill run id and event id into ntuple
    m_RunId = run;
    m_EventId = event;
  }

  //-------------------------------------------------------------------
  // using ascii file for output, but an ascii input file is needed.
  //-------------------------------------------------------------------
  if(writeFile==1 && event == 0)
  { 
    readin.open(input.c_str(),ios_base::in);
    if(readin) cout<<"Input File is ok "<<input<<endl;
    readout.open(output.c_str(),ios_base::out);
    if(readout) cout<<"Output File is ok "<<output<<endl;
    VERSIONNUM version;
    readin >> version;
    readout << version;
  }

  //---------------------------------
  // define a map to store mdc hits
  //---------------------------------
  multimap<int,uint32_t,less<int> > mdc_hitmap;
  mdc_hitmap.clear();

  //---------------------------------
  // define a map to store tof hits
  //---------------------------------
  multimap<int,int,less<int> > tof_hitmap;
  tof_hitmap.clear();

  //----------------------------------------------------
  // get mdc digits from TDS and save them into a map
  //----------------------------------------------------
  SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc(),"/Event/Digi/MdcDigiCol");
   if (!mdcDigiCol) {
      log << MSG::FATAL << "Could not find MDC digi" << endreq;
      return( StatusCode::FAILURE);
  }
  for(MdcDigiCol::iterator iter3=mdcDigiCol->begin();iter3!=mdcDigiCol->end();iter3++)
  { 
    Identifier id= (*iter3)->identify();
    int layer = MdcID::layer(id);
    int wire  = MdcID::wire(id);
    int tdc = (*iter3)->getTimeChannel();
    if(tdc < 0x7FFFFFFF && tdc > 0) {
      if(layer<=19) { 
        typedef pair<int, uint32_t > vpair;
        uint32_t mdc_Id = (layer & 0xFFFF ) << 16;
        mdc_Id = mdc_Id | (wire & 0xFFFF);
        mdc_hitmap.insert(vpair(tdc,mdc_Id));
      }
      if(layer>=36&&layer<=39)
      { 
        typedef pair<int, uint32_t > vpair;
        uint32_t mdc_Id = (layer & 0xFFFF ) << 16;
        mdc_Id = mdc_Id | (wire & 0xFFFF);
        mdc_hitmap.insert(vpair(tdc,mdc_Id));
      }
    }
  }

  //------------------------------------------------------------------
  // get tof digits from rawDataProviderSvc ant save them into a map
  //------------------------------------------------------------------
  TofDataMap tofDigiMap = m_rawDataProviderSvc->tofDataMapEstime();
  for(TofDataMap::iterator iter3 = tofDigiMap.begin();iter3 != tofDigiMap.end(); iter3++) {
    Identifier idd = Identifier(iter3->first);
    TofData * p_tofDigi = iter3->second;
    int barrel_ec = TofID::barrel_ec(idd);
    int layer = TofID::layer(idd);
    int im = TofID::phi_module(idd);
    if(barrel_ec == 1) {
      if(((p_tofDigi->quality()) & 0x5) != 0x5) continue;
      double tdc1 = p_tofDigi->tdc1();
      double tdc2 = p_tofDigi->tdc2();
      int tdc;
      if(tdc1 > tdc2) tdc = (int) tdc1;
      else tdc = (int) tdc2;
      typedef pair<int, int > vpair;
      tdc = tdc;
      tof_hitmap.insert(vpair(tdc,10000*barrel_ec+1000*layer+im*10));
    }
    else {
      int tdc1 = (int)p_tofDigi->tdc1();
      typedef pair<int, int > vpair;
      tdc1 = tdc1;
      tof_hitmap.insert(vpair(tdc1,10000*barrel_ec+1000*layer+im*10));
    }
  }

  //--------------------------
  // get emc digits from TDS
  //-------------------------- 
  SmartDataPtr<EmcDigiCol> emcDigiCol(eventSvc(),"/Event/Digi/EmcDigiCol");
  if (!emcDigiCol) {
     log << MSG::FATAL << "Could not find EMC digi" << endreq;
     return( StatusCode::FAILURE);
  }
  
  //----------------------------------------------------------
  // define initialize waveform object for each energy block
  //----------------------------------------------------------
  EmcWaveform blockWave[16];
  
  //------------------------------------------------------------
  // define a map of trigger cell, contains time and id infor.
  //------------------------------------------------------------
  multimap<int,uint32_t,less<int> > emc_TC;
  emc_TC.clear();

  //---------------------------------------------------------------------------------
  // get emc analog signal, including trigger cell, energy block and cluster infor.
  //---------------------------------------------------------------------------------
  getEmcAnalogSig(emcDigiCol, blockWave, emc_TC);

  //-----------------------------------
  // find peak time of energy block
  //-----------------------------------
  double peak_time = 0;
  findEmcPeakTime(peak_time, blockWave);

  //--------------------------
  // get muc digits from TDS
  //--------------------------
  SmartDataPtr<MucDigiCol> mucDigiCol(eventSvc(),"/Event/Digi/MucDigiCol");
  if (!mucDigiCol) {
     log << MSG::FATAL << "Could not find MUC digi" << endreq;
     return( StatusCode::FAILURE);
  }
  if(m_mucDigi) m_mucDigi->getMucDigi(mucDigiCol);

  //----------------------------------------------
  // output for debugging and count event number
  //----------------------------------------------
  if(event%10000 == 0) std::cout << "---> EventNo is " << event << std::endl;
  nEvent++;

  //********************************************************************
  //                      start time clock
  //********************************************************************

  //--------------------------
  // find start and end time
  //--------------------------
  double stime = -1, etime = -1;
  findSETime(mdc_hitmap,tof_hitmap,emc_TC,stime,etime);

  // calculate total clock number
  int nclock = 0;
  if(stime >= 0) {
    nclock = int ((etime - stime)/24) + 1;
  }
  else {
    nclock = 0;
  }
  
  //------------------------------------------------------------
  // define an array to store trigger conditions in each clock
  //------------------------------------------------------------
  int** trgcond = new int*[48];
  for(int condId = 0; condId < 48; condId++) {
    trgcond[condId] = new int[nclock];
  }

  // used for tof status machine
  int idle_status = -1; 

  for(int iclock = 0; iclock < nclock; iclock++) {
    //---------------------------
    // start mdc trigger logic
    //---------------------------
    runAclock_mdc(iclock, stime, mdc_hitmap);

    //---------------------------
    // start tof trigger logic
    //---------------------------
    runAclock_tof(iclock, stime, idle_status, tof_hitmap);

    //--------------------------
    // start emc trigger logic
    //--------------------------
    runAclock_emc(iclock, stime, emc_TC, blockWave);

    //----------------------------------
    // start track match trigger logic
    //----------------------------------
    //m_pIBGT->startTMTrig();

    //--------------------------
    // set trigger conditions
    //--------------------------
    StatusCode status = m_pIBGT->setTrigCondition();
    if(status!=StatusCode::SUCCESS) {
      log<<MSG::FATAL<< "Could not set trigger condition index" <<endreq;
      return StatusCode::FAILURE;
    }

    //--------------------------------------------
    // get each trigger condition in each clock
    //--------------------------------------------
    for(int condId = 0; condId < 48; condId++) {
      trgcond[condId][iclock] = m_pIBGT->getTrigCond(condId);
    }
  }

  //------------------------------
  // stretch trigger conditions
  //------------------------------
  stretchTrgCond(nclock, trgcond); 

  //-------------------------
  // SAF delay
  //-------------------------
  //trgSAFDelay(nclock, trgcond);

  //-------------------------
  // GTL delay
  //-------------------------
  //trgGTLDelay(nclock, trgcond);


  //********************************************************************
  //                       end time clock
  //********************************************************************


  //-------------------------------------------------------------------------------------------------------------------
  // deal with emc trigger conditions, in principle, if NClus>=1 is true between peaktime - 1.6us and peak time, 
  // other emc conditions can be true, but not used now.
  //-------------------------------------------------------------------------------------------------------------------
  bool ifClus1 = false;
  for(int i = 0; i < nclock; i++) {
      if(trgcond[0][i] > 0) ifClus1 = true;
  }

  if(ifClus1 == false) {
    for(int i = 0; i < nclock; i++) {
      for(int j = 0; j < 16; j++) {
        trgcond[j][i] = 0;
      }
    }
  }
 
  //-----------------------------------------------------------
  // do logic 'or' for each trigger condition in all clocks.
  //-----------------------------------------------------------
  for(int i = 0; i < nclock; i++) {
    for(int j = 0; j < 48; j++) {
      if(trgcond[j][i]) m_pIBGT->setTrigCond(j,1);
    }
  }

  //----------------------------
  // match with trigger table
  //----------------------------
  m_pIBGT->GlobalTrig();

  //--------------------------------------
  // this event can pass trigger or not
  //--------------------------------------
  ifpass = m_pIBGT->getIfpass();
  if(ifpass) 
  {
    passNo++;
    log<<MSG::INFO<<"pass event number is "<<passNo<<endl;
  }

  //-------------------------------------------
  // write out events which can pass trigger.
  //-------------------------------------------
  if(writeFile == 2) {
    if(ifpass)
    {
      setFilterPassed(true);
    }
    else
    {
      setFilterPassed(false);
    }
  }

  if(mTrigRootFlag) {
    //--------------------------------------------
    // fill histograms, trigger conditions of MC
    //--------------------------------------------
    for(int i = 0; i < 48; i++) {
      bool edge = false;
      int NOne = 0;
      m_condNOne[i] = -9;
      m_condNZero[i] = -9;
      for(int j = 0; j < nclock; j++) {
        m_mc_cond[i] += trgcond[i][j];
        if(trgcond[i][j] != 0) {
          if (NOne == 0) {
            m_condNZero[i] = j;
            m_trigCondi_MC->Fill(i);

          }
          edge = true;
          NOne++;
        }
        else {
          edge = false;
        }
        if(edge == false && NOne != 0) break;
      }
      m_condNOne[i] = NOne;
    }
    m_cond_id = 48;

    //-----------------------------------------------
    // fill histograms, trigger conditions of data
    //-----------------------------------------------
    if(m_runMode == 0) {
      SmartDataPtr<TrigData> trigData(eventSvc(), "/Event/Trig/TrigData");
      if (!trigData) {
        log << MSG::FATAL << "Could not find Trigger Data for physics analysis" << endreq;
        return StatusCode::FAILURE;
      }

      for(int id = 0; id < 48; id++) {
        if(trigData->getTrigCondition(id) != 0) { m_trigCondi_Data->Fill(id); }
        m_data_cond[id] = trigData->getTrigCondition(id);
      }
      m_cond_id = 48;
    }
  }

  //----------------------------
  // release memory
  //----------------------------
  for(int condId = 0; condId < 48; condId++) {
    delete trgcond[condId];
  }
  delete trgcond;


  if(mTrigRootFlag) { 
    m_evtId = event;
    m_tuple3->write();

    m_mc_totE_all = m_pIBGT->getEmcTotE();
    m_wetotE = m_pIBGT->getEmcWETotE();
    m_eetotE = m_pIBGT->getEmcEETotE();
 
    map<int,vector<complex<int> >, greater<int> > mymap;
    mymap = m_pIBGT->getEmcClusId();
    log<<MSG::INFO<<"EMC test: "<<endreq;
    int emc_btc_id = 0;
    for(map<int,vector<complex<int> >, greater<int> >::iterator iter=mymap.begin(); iter!=mymap.end(); iter++) {
      if((iter->first)==1) {
        for(unsigned int i=0; i<(iter->second).size(); i++) {
          log<<MSG::INFO<<"barrel theta is "<<(iter->second[i]).real()<<" phi is "<<(iter->second[i]).imag()<<endreq;
          emc_btc_id++;
        } 
      }
      if((iter->first)==0) {
        for(unsigned int i=0; i<(iter->second).size(); i++)
          log<<MSG::INFO<<"east theta is "<<(iter->second[i]).real()<<" phi is "<<(iter->second[i]).imag()<<endreq;
      }
      if((iter->first)==2) {
        for(unsigned int i=0; i<(iter->second).size(); i++)
          log<<MSG::INFO<<"west theta is "<<(iter->second[i]).real()<<" phi is "<<(iter->second[i]).imag()<<endreq;
      }
    }

    //retrieve EMC trigger information from EACC
  /*  SmartDataPtr<TrigGTDCol> trigGTDCol(eventSvc(), "/Event/Trig/TrigGTDCol");
    if (!trigGTDCol) {
      log << MSG::FATAL << "Could not find Global Trigger Data" << endreq;
      return StatusCode::FAILURE;
    }
    eacctrig->initialize();
    TrigGTDCol::iterator iter5 = trigGTDCol->begin();
    for (; iter5 != trigGTDCol->end(); iter5++) {
      uint32_t size = (*iter5)->getDataSize();
      const uint32_t* ptr = (*iter5)->getDataPtr();
      //set EACC trigger data
      if((*iter5)->getId() == 0xD7) {
        eacctrig->setEACCTrigData((*iter5)->getId(), (*iter5)->getTimeWindow(), size, ptr);
      }
    }

    double bmean[12]  = {8.02,10.1,12.3,7.43,14.8,13.0,12.5,13.2,10.9,12.3,14.7,15.7};
    double bsigma[12] = {0.88,0.52,0.9,0.72,0.7,0.82,0.64,0.78,0.72,0.76,0.54,0.64};
    vector<double> vblockE = m_pIBGT->getEmcBlockE();
    for(int blockId = 0; blockId < vblockE.size(); blockId++) {
      //m_mc_blockE[blockId] = vblockE[blockId]; 
      int block_time;
      m_mc_blockE[blockId] = blockWave[blockId+2].max(block_time)*0.333 - 0xa + RandGauss::shoot(bmean[blockId],bsigma[blockId]); 
      m_data_blockE[blockId] = eacctrig->getBBLKCharge(blockId);
      float r_blockE;
      if((eacctrig->getBBLKCharge(blockId) - bmean[blockId]) == 0.) r_blockE = 0; 
      else r_blockE = vblockE[blockId]/(eacctrig->getBBLKCharge(blockId) - bmean[blockId]);
      if(!(r_blockE >=0. || r_blockE <= 0.)) r_blockE = 0;
      m_R_blockE[blockId] = r_blockE;
    }   
    m_block_id = vblockE.size();

    m_data_totE_all = eacctrig->getEMCTotalCharge();
    //endcap energy
    int ee_endcap = 0, we_endcap = 0;
    for(int i = 0; i < 2; i++) {
      ee_endcap += eacctrig->getEBLKCharge(i);
      we_endcap += eacctrig->getWBLKCharge(i);
    }
    m_data_wetotE = we_endcap;
    m_data_eetotE = ee_endcap;

    m_data_totE_all = eacctrig->getEMCTotalCharge();
  
    //fill trigger cell energy
    int window = eacctrig->getTimeWindow();
    int index_tc = 0;
    for(int i=0;i<TrigConf::TCTHETANO_B;i++)
      for(int j=0;j<TrigConf::TCPHINO_B;j++)
      {
        m_btc_e[index_tc] = m_pIBGT->getBTCEnergy(i,j);
        int if_clus = 0;
        for(int k = 0; k < window; k++) {
          if(eacctrig->getBTC(i,j,k) == 1) {
            if_clus = 1;
            break;
          }
        }
        m_data_btc[index_tc] = if_clus;
        index_tc++;
      }
    m_index_btc = index_tc;
    
    index_tc = 0;
    for(int i =0;i<TrigConf::TCTHETANO_E;i++)
      for(int j =0;j<TrigConf::TCPHINO_E;j++)
      { 
        //m_wetc_e[index_tc] = m_pIBGT->getWETCEnergy(i,j);
        //m_eetc_e[index_tc] = m_pIBGT->getEETCEnergy(i,j);
        index_tc++;
      }
    //m_index_etc = index_tc;
*/
    m_tuple1->write();

    //----------------------------------------------
    // check information of MDC, TOF, EMC output
    //----------------------------------------------
    vector<int> vstrkId;
    vector<int> vltrkId;
    vstrkId = m_pIBGT->getMdcStrkId();
    vltrkId = m_pIBGT->getMdcLtrkId();
    log<<MSG::INFO<<"Mdc test: "<<endreq;
    for(unsigned int i=0; i<vstrkId.size(); i++) log<<MSG::INFO<<"short is "<<vstrkId[i]<<endreq;
    for(unsigned int j=0; j<vltrkId.size(); j++) { log<<MSG::INFO<<"long is "<<vltrkId[j]<<endreq; }

    map<int,vector<int>,greater<int> > tofmap;
    tofmap = m_pIBGT->getTofHitPos();
    log<<MSG::INFO<<"TOF test: "<<endreq;
    for(map<int,vector<int>,greater<int> >::iterator iter=tofmap.begin(); iter!=tofmap.end(); iter++) {
      if(iter->first == 0) {
        for(unsigned int i=0; i<iter->second.size(); i++) { 
          log<<MSG::INFO<<"east tof Id is "<<iter->second[i]<<endreq; 
        }
      }
      if(iter->first == 1) {
        for(unsigned int i=0; i<iter->second.size(); i++) { log<<MSG::INFO<<" barrel tof Id is "<<iter->second[i]<<endreq; }
      }
      if(iter->first == 2) {
        for(unsigned int i=0; i<iter->second.size(); i++) { log<<MSG::INFO<<"west tof Id is "<<iter->second[i]<<endreq; }
      }
    }

    //Fill ntuple for MUC
    std::vector<int> vtmp;

    vtmp = m_pIBGT->getMuclayerSeg();
    m_index2 = 0;
    for(std::vector<int>::iterator iter = vtmp.begin(); iter != vtmp.end(); iter++) {
      m_fireLayer[m_index2] = (long) *iter;
      m_index2++;
      if(m_index2 > m_index2->range().distance()) { break; cerr<<"*********** too many index ************"<<endl; }
    }
    //find tracks by count the fired layer number
    long trackb3=0, tracke3=0, trackb2=0, tracke2=0, trackb1=0, tracke1=0;
    int trackwe = 0, trackee = 0;
    for(unsigned int i=0; i<vtmp.size(); i++) {
      if(0<=vtmp[i]&&vtmp[i]<100) {
        if((vtmp[i]%10)>=3) { tracke3++; trackee++; }
      }
      if(200<=vtmp[i]) {
        if(((vtmp[i]-200)%10)>=3) { tracke3++; trackwe++; }
      }
      if(100<=vtmp[i]&&vtmp[i]<200) {
        if(((vtmp[i]-100)%10)>=3) trackb3++;
      }
    }
    m_ntrack3 = trackb3 + tracke3;

    for(unsigned int i=0; i<vtmp.size(); i++) {
      if(0<=vtmp[i]&&vtmp[i]<100) {
        if((vtmp[i]%10)>=2) tracke2++;
      }
      if(200<=vtmp[i]) {
        if(((vtmp[i]-200)%10)>=2) tracke2++;
      }
      if(100<=vtmp[i]&&vtmp[i]<200) {
        if(((vtmp[i]-100)%10)>=2) trackb2++;
      }
    }
    m_ntrack2 = trackb2 + tracke2;

    for(unsigned int i=0; i<vtmp.size(); i++) {
      if(0<=vtmp[i]&&vtmp[i]<100) {
        if((vtmp[i]%10)>=1) tracke1++;
        }
        if(200<=vtmp[i]) {
        if(((vtmp[i]-200)%10)>=1) tracke1++;
      }
      if(100<=vtmp[i]&&vtmp[i]<200) {
        if(((vtmp[i]-100)%10)>=1) trackb1++;
      }
    }
    m_ntrack1 = trackb1 + tracke1;
    //end of finding tracks by count the fired layer number

    vtmp = m_pIBGT->getMuchitLayer();
    m_index3 = 0;
    for(std::vector<int>::iterator iter = vtmp.begin(); iter != vtmp.end(); iter++) {
      m_hitLayer[m_index3] = (long) *iter;
      m_index3++;
      if(m_index3 > m_index3->range().distance()) { break; cerr<<"*********** too many index ************"<<endl; }
    }
 
    vtmp = m_pIBGT->getMuchitSeg();
    m_index4 = 0;
    for(std::vector<int>::iterator iter = vtmp.begin(); iter != vtmp.end(); iter++) {
      m_hitSeg[m_index4] =  *(iter);
      m_index4++;
      if(m_index4 > m_index4->range().distance()) { break; cerr<<"*********** too many index ************"<<endl; }
    }
  } // end fill ntuple

  //---------------------------------------------------
  // write out event number which not passed trigger.
  //---------------------------------------------------
  if(ifoutEvtId==1)
  {
    ofstream eventnum(outEvtId.c_str(),ios_base::app);
    if(!ifpass)
      eventnum<<event<<endl;
    eventnum.close();
  }
  
  //-------------------------------------------------
  // write out event number which passed trigger.
  //-------------------------------------------------
  if(ifoutEvtId==2)
  {
    ofstream eventnum(outEvtId.c_str(),ios_base::app);
    if(ifpass)
      eventnum<<event<<endl;
    eventnum.close();
  }

  //--------------------------------------------------------
  // write out events (passed trigger) into an ascii file
  //--------------------------------------------------------
  if(writeFile==1) 
  {
    EVENT asciiEvt;
    readin >> asciiEvt;
    if(asciiEvt.header.eventNo == event)
    {
      if(ifpass==true)
      readout<<asciiEvt<<endl;
    }
    else 
    cout<<"********* Event No. from AsciiFile do not equal Event No. from TDS "
         <<asciiEvt.header.eventNo<<" "<<event<<endl;
  }

  //--------------------------------------------------------------
  // if it is offline mode, register trigger information into TDS
  //--------------------------------------------------------------
  if(m_runMode == 1) {
    const int* trigcond = m_pIBGT->getTrigCond();
    const int* trigchan = m_pIBGT->getTrigChan();
    int window = 0;
    int timing = 0;
    bool preScale = false;

    StatusCode sc = StatusCode::SUCCESS ;
    TrigEvent* aTrigEvent = new TrigEvent;
    sc = eventSvc()->registerObject("/Event/Trig",aTrigEvent);
    if(sc!=StatusCode::SUCCESS) {
      log<<MSG::DEBUG<< "Could not register TrigEvent, you can ignore." <<endreq;
    }

    TrigData* aTrigData = new TrigData(window, timing, trigcond, trigchan, preScale);
    sc = eventSvc()->registerObject("/Event/Trig/TrigData",aTrigData);
    if(sc!=StatusCode::SUCCESS) {
      log<<MSG::ERROR<< "Could not register TrigData!!!!!" <<endreq;
    }
  }

  return StatusCode::SUCCESS;  
}

StatusCode BesTrigL1::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize BesTrigL1" << endreq;

  if(writeFile==1)
  {
    readin.close();
    readout.close();
  }
  cout<<"There are total "<< passNo<<" event pass trigger"<<endl;
  return StatusCode::SUCCESS;
}

void BesTrigL1::findSETime(multimap<int,uint32_t,less<int> > mdc_hitmap, multimap<int,int,less<int> > tof_hitmap, multimap<int,uint32_t,less<int> > emc_TC,
                           double& stime, double& etime) {
  std::multimap<int,uint32_t,less<int> >::iterator mdc_iter = mdc_hitmap.begin();
  double smdctime = -1, emdctime = -1;
  if(mdc_hitmap.size() != 0) {
    smdctime = (mdc_iter->first)*0.09375;
    mdc_iter = mdc_hitmap.end();
    mdc_iter--;
    emdctime = (mdc_iter->first)*0.09375;
  }

  std::multimap<int,int,less<int> >::iterator tof_iter = tof_hitmap.begin();
  double stoftime = -1, etoftime = -1;
  if(tof_hitmap.size() != 0) {
    stoftime = (tof_iter->first);
    tof_iter = tof_hitmap.end();
    tof_iter--;
    etoftime = (tof_iter->first);
  }

  std::multimap<int,uint32_t,less<int> >::iterator emc_iter = emc_TC.begin();
  double semctime = -1, eemctime = -1;
  if(emc_TC.size() != 0) {
    semctime = (emc_iter->first);
    emc_iter = emc_TC.end();
    emc_iter--;
    eemctime = (emc_iter->first);
  }

  stime = -1, etime = -1;
  if(smdctime >= 0 && stoftime >= 0) {
    if(smdctime > stoftime) stime = stoftime;
    else stime = smdctime;

    if((emdctime+800) > (etoftime + 24)) etime = emdctime+800;
    else etime = etoftime + 24;
  }
  else if(smdctime < 0 && stoftime >= 0) {
    stime = stoftime;
    etime = etoftime + 24;
  }
  else if(smdctime >= 0 && stoftime < 0) {
    stime = smdctime;
    etime = emdctime+800;
  }
  else {
    stime = -1;
    etime = -1;
  }
  //compare with emc time
  if(semctime >= 0 && stime >= 0) {
    if(semctime > stime) stime = stime;
    else stime = semctime;

    if((eemctime+16*24) > etime) etime = eemctime+16*24;
    else etime = etime;
  }
  else if(semctime < 0 && stime >= 0) {
    stime = stime;
    etime = etime;
  }
  else if(semctime >= 0 && stime < 0) {
    stime = semctime;
    etime = eemctime+16*24;
  }
  else {
    stime = -1;
    etime = -1;
  }
}

void BesTrigL1::runAclock_mdc(int iclock, double stime, multimap<int,uint32_t,less<int> > mdc_hitmap) {
  std::vector<int> vmdcHit;
  vmdcHit.clear();
 
  std::multimap<int,uint32_t,less<int> >::iterator mdc_iter = mdc_hitmap.begin();
  //int beginclock = int ((mdc_iter->first)*0.09375/24);

  //--------------------------
  // consider mdc noise
  //--------------------------
  /*
  if((iclock - beginclock) >= 0 && (iclock - beginclock) <= 33) {
    for(int i = 0; i < 16; i++) {
      for(int hit_id = 0; hit_id < 256; hit_id++) {
        int layer, wire;
        double ratio = -1;
        if(i == 0) layer = 8;
        if(i == 1) layer = 9;
        if(i == 2) layer = 10;
        if(i == 3) layer = 11;
        if(i == 4) layer = 12;
        if(i == 5) layer = 13;
        if(i == 6) layer = 14;
        if(i == 7) layer = 15;
        if(i == 8) layer = 16;
        if(i == 9) layer = 17;
        if(i == 10) layer = 18;
        if(i == 11) layer = 19;
        if(i == 12) layer = 36;
        if(i == 13) layer = 37;
        if(i == 14) layer = 38;
        if(i == 15) layer = 39;
        
        if(hit_id < 76) {
          if(i == 0) ratio = hit9[hit_id];
          if(i == 1) ratio = hit10[hit_id];
        }
        if(hit_id < 88) {
          if(i == 2) ratio = hit11[hit_id];
          if(i == 3) ratio = hit12[hit_id];
        }
        if(hit_id < 100) { 
          if(i == 4) ratio = hit13[hit_id];
          if(i == 5) ratio = hit14[hit_id];
        }
        if(hit_id < 112) { 
          if(i == 6) ratio = hit15[hit_id];
          if(i == 7) ratio = hit16[hit_id];
        }
        if(hit_id < 128) { 
          if(i == 8) ratio = hit17[hit_id];
          if(i == 9) ratio = hit18[hit_id];
        }
        if(hit_id < 140) {
          if(i == 10) ratio = hit19[hit_id];
          if(i == 11) ratio = hit20[hit_id];
        }
        if(i == 12) ratio = hit37[hit_id];
        if(i == 13) ratio = hit38[hit_id];
        if(i == 14) ratio = hit39[hit_id];
        if(i == 15) ratio = hit40[hit_id];
       
        wire = hit_id;
          
        if(RandFlat::shoot() < ratio*(33 - iclock)*24/2000.) {
          vmdcHit.push_back(layer);
          vmdcHit.push_back(wire);
        }
      }
    }
  }
  */

  for(std::multimap<int,uint32_t,less<int> >::iterator mdc_iter = mdc_hitmap.begin(); mdc_iter != mdc_hitmap.end(); mdc_iter++)
  {
    double time = (mdc_iter->first)*0.09375;
    if((time < (stime + (iclock + 1)*24.)) && (time + 800.) > (stime + iclock*24.)) {
      uint32_t mdcId = mdc_iter->second;
      int layer =  (mdcId & 0xFFFF0000 ) >> 16;
      int cell = mdcId & 0xFFFF;
      bool firstdc = true;
      //for(std::multimap<int,int,less<int> >::iterator tmp_mdc = mdc_hitmap.begin(); tmp_mdc != mdc_iter; tmp_mdc++) {
      //  if(mdcId == (tmp_mdc->second)) firstdc = false;
      //} 
      if(firstdc == true) {
        vmdcHit.push_back(layer);
        vmdcHit.push_back(cell);
      }
    }
  }

  //set mdc vector hit
  m_MdcTSF->setMdcDigi(vmdcHit);
  m_pIBGT->startMdcTrig();
}

void BesTrigL1::runAclock_tof(int iclock, double stime, int& idle_status, std::multimap<int,int,less<int> > tof_hitmap) {
  std::vector<int> vtofHit;
  vtofHit.clear();

  //tof trigger
  if(idle_status != -1 && (iclock - idle_status) == 3) idle_status = -1;
  for(std::multimap<int,int,less<int> >::iterator tof_iter = tof_hitmap.begin(); tof_iter != tof_hitmap.end(); tof_iter++)
  {
    double time = (tof_iter->first); //ns
    if(idle_status == -1) {
      if(time < (stime + (iclock + 1)*24) && time >= (stime + iclock*24)) {
      //if(time < (stime + (iclock + 1)*24) && (time + 24) > (stime + iclock*24)) { //stretch signal
        vtofHit.push_back(tof_iter->second);
      }
    }
    else {
      if((iclock - idle_status) == 1) {
        if((time < (stime + (iclock + 1)*24) && time >= (stime + iclock*24)) ||
           (time < (stime + iclock*24) && time >= (stime + (iclock - 1)*24))
          ) {
          vtofHit.push_back(tof_iter->second);
        }
      }
      if((iclock - idle_status) == 2) {
        if((time < (stime + (iclock + 1)*24) && time >= (stime + iclock*24)) ||
           (time < (stime + iclock*24) && time >= (stime + (iclock - 1)*24)) ||
           (time < (stime + (iclock - 1)*24) && time >= (stime + (iclock - 2)*24))
          ) {
          vtofHit.push_back(tof_iter->second);
        }
      }
    }
  }
  if(idle_status == -1 && vtofHit.size() != 0) idle_status = iclock;

  //set tof vector hit
  m_TofHitCount->setTofDigi(vtofHit);
  m_pIBGT->startTofTrig();
}

void BesTrigL1::runAclock_emc(int iclock, double stime, std::multimap<int,uint32_t,less<int> > emc_TC, EmcWaveform* blockWave) {
  std::vector<uint32_t> vemcClus;
  std::vector<double> vemcBlkE;

  vemcClus.clear();
  vemcBlkE.clear();
  //std::cout << "iclock, emc_TC size: " << iclock << ", " << emc_TC.size() << std::endl;
  //cluster finding in emc trigger
  for(std::multimap<int,uint32_t,less<int> >::iterator emc_iter = emc_TC.begin(); emc_iter != emc_TC.end(); emc_iter++)
  {
    double time = (emc_iter->first);
    if((time < (stime + (iclock + 1)*24.)) && (time + 16*24) > (stime + iclock*24.)) {
      vemcClus.push_back(emc_iter->second);
    } 
  }
  
  //energy adding in emc trigger
  for(int blockId = 0; blockId < 16; blockId++) {
    double block_ADC = (blockWave[blockId]).getADCTrg((int)stime+iclock*24);
    vemcBlkE.push_back(block_ADC);
   // std::cout << " block_ADC: " << block_ADC << std::endl;
  } 
  //std::cout << "iclock,stime,vemcClus size: " << iclock << "," << stime << ", " << vemcClus.size() << std::endl;  
  m_emcDigi->setEmcTC(vemcClus);
  m_emcDigi->setEmcBE(vemcBlkE); //set block energy
  //start EMC trigger logic
  m_pIBGT->startEmcTrig(); 
}

void BesTrigL1::getEmcAnalogSig(EmcDigiCol* emcDigiCol, EmcWaveform (&blockWave)[16], multimap<int,uint32_t,less<int> >& emc_TC) {
  EmcWaveform eewave[32];
  EmcWaveform wewave[32];
  EmcWaveform bwave[11][30];

  for(int i = 0; i < 11; i++) {
    for(int j = 0; j < 30; j++) {
      bwave[i][j].makeWaveformTrg(0,0);
    } 
  } 
  for(int i = 0; i < 32; i++) {
    if(i < 16) blockWave[i].makeWaveformTrg(0,0);
    eewave[i].makeWaveformTrg(0,0);
    wewave[i].makeWaveformTrg(0,0);
  }

  for (EmcDigiCol::iterator iter3=emcDigiCol->begin();iter3!= emcDigiCol->end();iter3++) {
    Identifier id=(*iter3)->identify();
    unsigned int module = EmcID::barrel_ec(id);
    unsigned int theta = EmcID::theta_module(id);
    unsigned int phi = EmcID::phi_module(id);

    int index = emcCalibConstSvc->getIndex(module,theta,phi);
    double trgGain = m_RealizationSvc->getTrgGain(index);
    double adc = (double) (*iter3)->getChargeChannel();
    double mv = RandGauss::shoot(978.,14.);
    
    if((*iter3)->getMeasure()==0) adc = adc*2*mv*2/65535.*(trgGain);
    else if((*iter3)->getMeasure()==1) adc = adc*16*mv*2/65535*(trgGain);
    else adc = adc*64*mv*2/65535*(trgGain);

    unsigned int tdc = (*iter3)->getTimeChannel();
    int theTC = m_emcDigi->getTCThetaId(module,theta,phi);
    int phiTC = m_emcDigi->getTCPhiId(module,theta,phi);
    EmcWaveform wave1;
    if(module == 0) { wave1.makeWaveformTrg(adc,tdc+80); eewave[phiTC] += wave1; }
    if(module == 1) { wave1.makeWaveformTrg(adc,tdc+80); bwave[theTC][phiTC] += wave1; }
    if(module == 2) { wave1.makeWaveformTrg(adc,tdc+80); wewave[phiTC] += wave1; }
  }

  //find barrel cluster
  for(int i = 0; i < 11; i++) {
    for(int j = 0; j < 30; j++) {
      int time_low = bwave[i][j].frontEdgeTrg(m_pIBGT->getL1TC_GATE());
      int time_high = bwave[i][j].frontEdgeTrg(m_pIBGT->getL1TC_THRESH());
      int time = -1;

      if(time_high >= 0) {
        if(time_low*50+1500 > time_high*50) time = time_low*50 + 1500;
        else time = time_high*50;
        uint32_t TCID = (1 & 0xFF) << 16;
        TCID = TCID | ((i & 0xFF) << 8);
        TCID = TCID | (j & 0xFF);
        typedef pair<int, uint32_t > vpair;
        emc_TC.insert(vpair(time,TCID));
        //std::cout <<"i, j: " << i << ", " << j << "  time: " << time << std::endl;
      }
      if(time_low >= 0) {
        int blockId = m_emcDigi->getBLKId(i,j);
        blockWave[blockId+2] += bwave[i][j];
      }
    }
  }
  //find end cap cluster
  for(int i = 0; i < 32; i++) {
    //east end cap
    int time_low1 = eewave[i].frontEdgeTrg(m_pIBGT->getL1TC_GATE());
    int time_high1 = eewave[i].frontEdgeTrg(m_pIBGT->getL1TC_THRESH());
    int time1 = -1;
    if(time_high1 >= 0) {
      if(time_low1*50+1500 > time_high1*50) time1 = time_low1*50 + 1500;
      else time1 = time_high1*50;
      uint32_t TCID1 = (0 & 0xFF) << 16;
      TCID1 = TCID1 | ((0 & 0xFF) << 8);
      TCID1 = TCID1 | (i & 0xFF);
      typedef pair<int, uint32_t > vpair;
      emc_TC.insert(vpair(time1,TCID1));
    }
    if(time_low1 >= 0) {
      if(i<16) blockWave[0] += eewave[i];
      else blockWave[1] += eewave[i];
    }
    //west end cap
    int time_low2 = wewave[i].frontEdgeTrg(m_pIBGT->getL1TC_GATE());
    int time_high2 = wewave[i].frontEdgeTrg(m_pIBGT->getL1TC_THRESH());
    int time2 = -1;
    if(time_high2 >= 0) {
      if(time_low2*50+1500 > time_high2*50) time2 = time_low2*50 + 1500;
      else time2 = time_high2*50;
      uint32_t TCID2 = (2 & 0xFF) << 16;
      TCID2 = TCID2 | ((0 & 0xFF) << 8);
      TCID2 = TCID2 | (i & 0xFF);
      typedef pair<int, uint32_t > vpair;
      emc_TC.insert(vpair(time2,TCID2));
    }
    if(time_low2 >= 0) {
      if(i<16) blockWave[14] += wewave[i];
      else blockWave[15] += wewave[i];
    }
  }
}

void BesTrigL1::findEmcPeakTime(double& peak_time, EmcWaveform* blockWave) {
  double tot_block_max = 0;
  for(int i = 0; i < 16; i++) {
    int block_time;
    double block_max = blockWave[i].max(block_time);
    tot_block_max += block_max;
  }

  for(int i = 0; i < 16; i++) {
    if(tot_block_max == 0) break;
    int block_time;
    double block_max = blockWave[i].max(block_time);
    block_time = block_time*50;
    peak_time += block_max/tot_block_max*block_time;
  }
}

void BesTrigL1::stretchTrgCond(int nclock, int** & trgcond) {
  int emc_clus = 34;
  int emc_ener = 50;
  int mdc = 34;
  int mdc_n = 68;
  int tof = 4;
  for(int icond = 0; icond < 48; icond++) {
    int sclock = -1;
    bool retrig = false;
    for(int iclock = 0; iclock < nclock; iclock++) {
      if(icond < 16) { //stretch emc trigger conditions
        if(icond < 7 || icond == 12 || icond == 13) { // stretch cluster trigger conditions
          if(sclock != -1 && iclock - sclock == emc_clus) sclock = -1;
          if(sclock == -1 && trgcond[icond][iclock] > 0) {
            if(iclock == 0) sclock = iclock;
            else {
              if(trgcond[icond][iclock]*trgcond[icond][iclock-1] == 0) sclock = iclock;
            }
          } 
          if(sclock != -1 && iclock - sclock < emc_clus) trgcond[icond][iclock] = 1;  
        }
        else { //stretch emc energy trigger conditions, re-triggering is available
          if(sclock != -1 && iclock - sclock == emc_ener) sclock = -1;
          if(sclock == -1 && trgcond[icond][iclock] > 0) {
            if(iclock == 0) sclock = iclock;
            else {
              if(trgcond[icond][iclock]*trgcond[icond][iclock-1] == 0) sclock = iclock;
            }
          }
          if(sclock != -1 && iclock - sclock < emc_ener && trgcond[icond][iclock] == 0) retrig = true;
          if(retrig == true) {
            if(trgcond[icond][iclock] > 0) {
              sclock = iclock;
              retrig = false;
            }
          }
          if(sclock != -1 && iclock - sclock < emc_ener) trgcond[icond][iclock] = 1;
        } 
      }
      else if(icond >= 16 && icond < 23) { //stretch tof trigger conditions
        if(sclock != -1 && iclock - sclock == tof) sclock = -1;
          if(sclock == -1 && trgcond[icond][iclock] > 0) {
            if(iclock == 0) sclock = iclock;
            else {
              if(trgcond[icond][iclock]*trgcond[icond][iclock-1] == 0) sclock = iclock;
            }
          }
          if(sclock != -1 && iclock - sclock < tof) trgcond[icond][iclock] = 1;
      }
      else if(icond >= 38) { //stretch mdc trigger conditions
        if(icond == 39|| icond == 43) {
          if(sclock != -1 && iclock - sclock == mdc_n) sclock = -1;
          if(sclock == -1 && trgcond[icond][iclock] > 0) {
            if(iclock == 0) sclock = iclock;
            else {
              if(trgcond[icond][iclock]*trgcond[icond][iclock-1] == 0) sclock = iclock;
            }
          }
          if(sclock != -1 && iclock - sclock < mdc_n) trgcond[icond][iclock] = 1;
        }
        else {
          if(sclock != -1 && iclock - sclock == mdc) sclock = -1;
          if(sclock == -1 && trgcond[icond][iclock] > 0) {
            if(iclock == 0) sclock = iclock;
            else {
              if(trgcond[icond][iclock]*trgcond[icond][iclock-1] == 0) sclock = iclock;
            }
          }
          if(sclock != -1 && iclock - sclock < mdc) trgcond[icond][iclock] = 1;
        }
      }
      else { //stretch other trigger conditions, including track match and muc
      }
    }
  }
}

void BesTrigL1::trgSAFDelay(int nclock, int** & trgcond) {
  //SAF delay time
//  int delay[48] = {31,31,31,31,31,31,31,7,7,7,7,7,31,31,7,7,
//                   135,135,135,135,135,135,135,83,83,83,6,6,6,83,83,83,
//                   97,97,97,97,97,97,86,87,85,87,83,85,83,85,122,122};
  int delay[48] = {24,24,24,24,24,24,24,7,7,7,7,7,24,24,7,7,
                   0,0,0,0,0,0,0,83,83,83,6,6,6,83,83,83,
                   97,97,97,97,97,97,0,0,0,0,0,0,0,0,122,122};

  for(int icond = 0; icond < 48; icond++) {
    for(int iclock = nclock-1; iclock >= 0; iclock--) {
      if(iclock < delay[icond]) trgcond[icond][iclock] = 0;
      else {
        trgcond[icond][iclock] = trgcond[icond][iclock-delay[icond]];
      }
    }
  }
}

void BesTrigL1::trgGTLDelay(int nclock, int** & trgcond) {
  //GTL delay time
  int delay[48] = {1,1,1,1,1,1,1,18,18,18,18,18,1,1,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,14,14,14,14,14,14,10,10,10,10,10,10,10,10,10,10};

  for(int icond = 0; icond < 48; icond++) {
    for(int iclock = nclock-1; iclock >= 0; iclock--) {
      if(iclock < delay[icond]) trgcond[icond][iclock] = 0;
      else trgcond[icond][iclock] = trgcond[icond][iclock-delay[icond]];
    }
  }
}
