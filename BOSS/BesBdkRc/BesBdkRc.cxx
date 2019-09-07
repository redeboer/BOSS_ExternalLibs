//*****************************************************************************
//
//
//
//*****************************************************************************

// Header for this module:-
#include "BesBdkRc/BesBdkRc.h"
#include "BesBdkRc/BesBdkRcRandom.h"

// Framework Related Headers:-
#include "HepMC/GenEvent.h"

#include "HepMC/HEPEVT_Wrapper.h"
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/GenEvent.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "BesKernel/IBesRndmGenSvc.h"

#include "GeneratorObject/McGenEvent.h"
#include "cfortran/cfortran.h"
#include <cstdlib>
#include <cassert>

using namespace std;

typedef struct{
  double  EB,W2MIN,K0,KS,EWE;
}INPUT_DEF;
#define INPUT COMMON_BLOCK(INPUT_DEF,input)
COMMON_BLOCK_DEF(INPUT_DEF,INPUT);

typedef struct{
  double          ME,MU,PI,ALFA,BARN;
} PCONST_DEF;
#define PCONST COMMON_BLOCK(PCONST_DEF, pconst)
COMMON_BLOCK_DEF(PCONST_DEF,PCONST);

typedef struct{
  float   TCHMIN, PCHMIN;
} FINCUT_DEF;
#define FINCUT COMMON_BLOCK(FINCUT_DEF, fincut)
COMMON_BLOCK_DEF(FINCUT_DEF,FINCUT);

typedef struct{
  float W2MINR, EBEAM, MAXESW, KZERO;
  int ISEED;
  float QMASS[6];
}LOCALC_DEF;
#define LOCALC COMMON_BLOCK(LOCALC_DEF, localc)
COMMON_BLOCK_DEF(LOCALC_DEF,LOCALC);

typedef struct{
  double COLCHA;
  int IFINAL;
}COLCHC_DEF;
#define COLCHC COMMON_BLOCK(COLCHC_DEF, colchc)
COMMON_BLOCK_DEF(COLCHC_DEF,COLCHC);

typedef struct{
  int         NEVCUT, NEVTOT, MAXNTRY;
}DELPHC_DEF;
#define DELPHC COMMON_BLOCK(DELPHC_DEF, delphc)
COMMON_BLOCK_DEF(DELPHC_DEF,DELPHC);

typedef struct{
  int ITOT, NEVUNW;
  double CSTOT,CSERR;
  float  CSCUT, CSCUTERR;
}XSECTC_DEF;
#define XSECTC COMMON_BLOCK(XSECTC_DEF, xsectc)
COMMON_BLOCK_DEF(XSECTC_DEF,XSECTC);

typedef struct{
  int         GOODEVT;
}FLAGS_DEF;
#define FLAGS COMMON_BLOCK(FLAGS_DEF, flags)
COMMON_BLOCK_DEF(FLAGS_DEF,FLAGS);

typedef struct{
  int N;
  int K[5][4000];
  float P[5][4000],V[5][4000];
}LUJETS_DEF;
#define LUJETS COMMON_BLOCK(LUJETS_DEF,lujets)
COMMON_BLOCK_DEF(LUJETS_DEF,LUJETS);

PROTOCCALLSFSUB1(LUHEPC, luhepc, INT)
#define LUHEPC(ICONV) CCALLSFSUB1(LUHEPC, luhepc, INT, ICONV)

PROTOCCALLSFSUB1(LULIST, lulist, INT)
#define LULIST(ICONV) CCALLSFSUB1(LULIST, lulist, INT, ICONV)

PROTOCCALLSFSUB0(HEPEVT_CLEAN, hepevt_clean)
#define HEPEVT_CLEAN() CCALLSFSUB0(HEPEVT_CLEAN, hepevt_clean)

PROTOCCALLSFSUB0(HEPEVT_PRINT, hepevt_print)
#define HEPEVT_PRINT() CCALLSFSUB0(HEPEVT_PRINT, hepevt_print)

PROTOCCALLSFSUB1(BDKRC,bdkrc,INT)
#define BDKRC(IDUMP) CCALLSFSUB1(BDKRC, bdkrc, INT, IDUMP)

PROTOCCALLSFSUB3(FINISH,finish,PINT,PDOUBLE,PDOUBLE)
#define FINISH(IN,SIGT,ER) CCALLSFSUB3(FINISH,finish,PINT,PDOUBLE,PDOUBLE,IN,SIGT,ER)

PROTOCCALLSFSUB4(RLUXGO,rluxgo,INT,INT,INT,INT)
#define RLUXGO(lux,ISEED,K1,K2) CCALLSFSUB4(RLUXGO,rluxgo,INT,INT,INT,INT,lux,ISEED,K1,K2)

PROTOCCALLSFSUB0(GEN_INIT, gen_init)
#define GEN_INIT() CCALLSFSUB0(GEN_INIT, gen_init)

PROTOCCALLSFSUB0(GEN1EVT, gen1evt)
#define GEN1EVT() CCALLSFSUB0(GEN1EVT, gen1evt)

extern "C" {                                                                                      
  extern float flat_();                                                                       
}                                                                                                 
                                                                                                  
float flat_()
{ 
  return (float)BesBdkRcRandom::random();
}                                                                                                 
  
BesBdkRc::BesBdkRc(const string& name, ISvcLocator* pSvcLocator):Algorithm( name, pSvcLocator )
{
//  declareProperty("InitialSeed",m_iseed=1001);
  declareProperty("CMEnergy", m_CMEnergy = 3.097); // 2*Ebeam [GeV]
  declareProperty("W2min",m_w2min=0.02); // Cut on invariant gamma-gamma mass
  declareProperty("EstimatedMaxWeight", m_ewe=2.5);
  declareProperty("SoftPhotonMaxEnergy", m_kzero=0.002);
  qmass[0] = 0.2;
  qmass[1] = 0.2;
  qmass[2] = 0.5;
  qmass[3] = 1.5;
  qmass[4] = 4.5;
  qmass[5] = 180.;
  declareProperty("MaxNTry", m_maxNTry);
  declareProperty("FinalState", m_ifinal);
  declareProperty("MinTheta",m_tcmin);
  declareProperty("MinMomentum",m_pcmin);

  m_numberEvent=0;
  toRad=M_PI/180.0;
  toDeg=180.0/M_PI;
}

StatusCode BesBdkRc::initialize(){
  MsgStream log(messageService(), name());
  log << MSG::WARNING << "BesBdkRc initialize" << endreq;

  static const bool CREATEIFNOTTHERE(true);                                                            
  StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);                 
  if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)                                                 
  {                                                                                                    
    log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;                      
    return RndmStatus;                                                                                 
  }                                                                                                    
  CLHEP::HepRandomEngine* engine  = p_BesRndmGenSvc->GetEngine("BesBdkRc");                            
  engine->showStatus();
  BesBdkRcRandom::setRandomEngine(engine); 

  //"INIT" subroutine action is replaced by c++ code
  // for gen_init  fortran code is called   

  FINCUT.TCHMIN=m_tcmin*toRad;
  FINCUT.PCHMIN=m_pcmin;
//  RLUXGO(3,m_iseed,0,0);

  DELPHC.MAXNTRY=m_maxNTry;

  COLCHC.IFINAL=m_ifinal;

  INPUT.EB=0.5*m_CMEnergy;

  LOCALC.W2MINR=m_w2min;
  LOCALC.EBEAM=0.5*m_CMEnergy;
  LOCALC.MAXESW=m_ewe;
  LOCALC.KZERO=m_kzero;
//  LOCALC.ISEED=m_iseed;
  for(int i=0; i<6;i++)LOCALC.QMASS[i]=qmass[i];
  GEN_INIT();
  XSECTC.NEVUNW=0;

  return StatusCode::SUCCESS;
}

StatusCode BesBdkRc::execute() 
{ 
  MsgStream log(messageService(), name());
  log << MSG::INFO << "BesBdkRc executing" << endreq;
  HepMC::HEPEVT_Wrapper::set_max_number_entries(2000);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::IO_HEPEVT HepEvtIO;

  HEPEVT_CLEAN();
  GEN1EVT();

  if(FLAGS.GOODEVT!=1){
    log << MSG::ERROR<<" BesBdkRc: fail to generate good event"<<endl;
    return StatusCode::FAILURE;
  }

  m_numberEvent++;
  if( log.level() < MSG::INFO )LULIST(1);
  //  LULIST(1);
  LUHEPC(1);
  HepMC::GenEvent* evt = HepEvtIO.read_next_event();
  evt->set_event_number(m_numberEvent);
  evt->set_signal_process_id(1);
  //  evt->print();
  //Check if the McCollection already exists                                                 
  SmartDataPtr<McGenEventCol> anMcCol(eventSvc(), "/Event/Gen");
  if (anMcCol!=0) {
    // Add event to existing collection                                                       
    MsgStream log(messageService(), name());
    log << MSG::INFO << "Add McGenEvent to existing collection" << endreq;
    McGenEvent* mcEvent = new McGenEvent(evt);
    anMcCol->push_back(mcEvent);
  }  else {
    // Create Collection and add  to the transient store                                      
    McGenEventCol *mcColl = new McGenEventCol;
    McGenEvent* mcEvent = new McGenEvent(evt);
    mcColl->push_back(mcEvent);
    StatusCode sc = eventSvc()->registerObject("/Event/Gen",mcColl);
    if (sc != StatusCode::SUCCESS) {
      log << MSG::ERROR << "Could not register McGenEvent" << endreq;
      delete mcColl;
      delete evt;
      delete mcEvent;
      return StatusCode::FAILURE;
    }
  }
  // string s;
  // getline(cin,s);

  return StatusCode::SUCCESS; 
}

StatusCode BesBdkRc::finalize() 
{
  MsgStream log(messageService(), name());
  log << MSG::INFO << "BesBdkRc finalized" << endreq;
  int itot;
  itot=XSECTC.ITOT;
  double cstot;
  double cserr;
  FINISH(itot,cstot,cserr);
  float effcut=0;
  float cscut=0;
  float efferr=0;
  float cscuterr=0;
  if(XSECTC.NEVUNW){
    effcut=float(DELPHC.NEVCUT)/float(XSECTC.NEVUNW);
    cscut=effcut*cstot;
    efferr=sqrt(effcut*(1.0-effcut)/float(XSECTC.NEVUNW));
    cscuterr = sqrt(cstot*efferr*cstot*efferr + effcut*effcut*cserr*cserr);
  }
  printf("BDKRC SUMMARY: Cross section after user cuts= %G +- %G nb\n",cscut,cscuterr);
  printf("               Cut acceptance              = %G +- %G \n",effcut,efferr);
  return StatusCode::SUCCESS;
}
