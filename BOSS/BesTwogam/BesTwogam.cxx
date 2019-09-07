//*****************************************************************************
//
// BesTwogam.cxx
//
//*****************************************************************************

// Header for this module:-
#include "BesTwogam/BesTwogam.h"

// Framework Related Headers:-
#include "HepMC/GenEvent.h"
//#include "CLHEP/HepMC/GenVertex.h"
//#include "CLHEP/HepMC/GenParticle.h"

#include "HepMC/HEPEVT_Wrapper.h"
#include "HepMC/IO_HEPEVT.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "GeneratorObject/McGenEvent.h"

#include "cfortran/cfortran.h"

#include <stdlib.h>

typedef struct{
  int ISEED;
}ISEEDC_DEF;
#define ISEEDC COMMON_BLOCK(ISEEDC_DEF,iseedc)
COMMON_BLOCK_DEF(ISEEDC_DEF,ISEEDC);

typedef struct{
  int  i_cardName;
  char cardName[255];
}FNAMES_DEF;
#define FNAMES COMMON_BLOCK(FNAMES_DEF,fnames)
COMMON_BLOCK_DEF(FNAMES_DEF,FNAMES);

typedef struct{
  int NEVTOT,NTUNW,NSMALL,IPROC1,LUNOUT,IDEBUG,LUN,NTRY;
}INPARC_DEF;
#define INPARC  COMMON_BLOCK(INPARC_DEF,inparc) 
COMMON_BLOCK_DEF(INPARC_DEF,INPARC);
typedef struct{
  float RLUMI;
  float CST;
}LUMINO_DEF;
#define LUMINO COMMON_BLOCK( LUMINO_DEF,lumino) 
COMMON_BLOCK_DEF(LUMINO_DEF,LUMINO);
typedef struct{
  int NEVENT,NEVCUT,NEVUNW;
}EVENUM_DEF;
#define EVENUM  COMMON_BLOCK(EVENUM_DEF,evenum) 
COMMON_BLOCK_DEF(EVENUM_DEF,EVENUM);
typedef struct{
  double  MAXESW,SUMW[5],MEANW[5],SUMW2[5],MAXW;
  double  SUMWC[5],MEANWC[5],SUMW2C[5];
}UNWEIT_DEF;
#define UNWEIT COMMON_BLOCK(UNWEIT_DEF,unweit) 
COMMON_BLOCK_DEF(UNWEIT_DEF,UNWEIT);
typedef struct{
  double WT,SUMWT,SUMWTC;
}WEIGHTED_DEF;
#define WEIGHTED COMMON_BLOCK(WEIGHTED_DEF,weighted) 
COMMON_BLOCK_DEF(WEIGHTED_DEF,WEIGHTED);
typedef struct{
  int GOODEV;
}CFLAGS_DEF;
#define CFLAGS COMMON_BLOCK(CFLAGS_DEF,cflags) 
COMMON_BLOCK_DEF(CFLAGS_DEF,CFLAGS);

typedef struct{
  float OM1MNE, OM1MXE, OM2MNE, OM2MXE,
    T1MINE, T1MAXE, T2MINE, T2MAXE,
    Q12MNE, Q12MXE, Q22MNE, Q22MXE,
    W2MINE, W2MAXE, TKMINE, TKMAXE, EBEAME, MAXESW;
    }EXCUTS_DEF;
#define EXCUTS COMMON_BLOCK(EXCUTS_DEF, excuts)
COMMON_BLOCK_DEF(EXCUTS_DEF,EXCUTS);

typedef struct{
  int N;
  int K[5][4000];
  float P[5][4000],V[5][4000];
}LUJETS_DEF;
#define LUJETS COMMON_BLOCK(LUJETS_DEF,lujets)
COMMON_BLOCK_DEF(LUJETS_DEF,LUJETS);

PROTOCCALLSFSUB0(PRINTEVT,printevt)
#define PRINTEVT() CCALLSFSUB0(PRINTEVT,printevt)

PROTOCCALLSFSUB0(INIRUN,inirun)
#define INIRUN() CCALLSFSUB0(INIRUN,inirun) 
  
PROTOCCALLSFSUB0(FINISH,finish)
#define FINISH() CCALLSFSUB0(FINISH,finish) 

PROTOCCALLSFSUB0(GENEVT,genevt)
#define GENEVT() CCALLSFSUB0(GENEVT,genevt) 

PROTOCCALLSFSUB0(GEN1EVT,gen1evt)
#define GEN1EVT() CCALLSFSUB0(GEN1EVT,gen1evt) 

PROTOCCALLSFSUB0( RDCUTS , rdcuts)
#define RDCUTS() CCALLSFSUB0( RDCUTS,rdcuts)

PROTOCCALLSFSUB0( SETDEF , setdef)
#define SETDEF() CCALLSFSUB0( SETDEF,setdef)

PROTOCCALLSFSUB0( QCDSTA , qcdsta)
#define QCDSTA() CCALLSFSUB0( QCDSTA,qcdsta)

PROTOCCALLSFSUB0( DOCUTS , docuts)
#define DOCUTS() CCALLSFSUB0( DOCUTS,docuts)

PROTOCCALLSFSUB0( ESTMAXCS , estmaxcs)
#define ESTMAXCS() CCALLSFSUB0( ESTMAXCS,estmaxcs)

PROTOCCALLSFSUB0( PRTCUT , prtcut)
#define  PRTCUT() CCALLSFSUB0( PRTCUT,prtcut)

PROTOCCALLSFSUB0( USRINI , usrini)
#define USRINI() CCALLSFSUB0( USRINI,usrini)

PROTOCCALLSFSUB0( USRHIS , usrhis)
#define USRHIS() CCALLSFSUB0( USRHIS,usrhis)

PROTOCCALLSFSUB0( USREND , usrend)
#define USREND() CCALLSFSUB0( USREND,usrend)
//
PROTOCCALLSFSUB1(LUHEPC, luhepc, INT)
#define LUHEPC(ICONV) CCALLSFSUB1(LUHEPC, luhepc, INT, ICONV)

PROTOCCALLSFSUB0(HEPEVT_CLEAN, hepevt_clean)
#define HEPEVT_CLEAN() CCALLSFSUB0(HEPEVT_CLEAN, hepevt_clean)

PROTOCCALLSFSUB1(LULIST, lulist, INT)
#define LULIST(ICONV) CCALLSFSUB1(LULIST, lulist, INT, ICONV)

PROTOCCALLSFSUB0(HEPEVT_PRINT, hepevt_print)
#define HEPEVT_PRINT() CCALLSFSUB0(HEPEVT_PRINT, hepevt_print)

extern "C" {
  extern void pygive_(const char *cnfgstr,int length);
}
 

BesTwogam::BesTwogam(const string& name, ISvcLocator* pSvcLocator):Algorithm( name, pSvcLocator )
{
  declareProperty("InitialSeed",m_seed=1001);
  declareProperty("CMEnergy", m_cmEnergy = 3.097); // 2*Ebeam [GeV]
  declareProperty("MinimumW2",m_m2min=0.02);
  declareProperty("Unweighted",m_unw=1);
  declareProperty("CardFileName", m_fcard="BesTwogam.conf");
  //interface set pythia pars, pingrg
  m_pypars.clear();
  declareProperty("setPythiaPars", m_pypars);
}

StatusCode BesTwogam::initialize(){
  MsgStream log(messageService(), name());
  log << MSG::WARNING << "BesTwogam initialize" << endreq;

  if(m_fcard.size()>254){
  log << MSG::ERROR << "Too big name of BesTwogam card file" << endreq;
  return StatusCode::FAILURE;
  }
  //  FNAMES.dummy=61777;
  strcpy(FNAMES.cardName,m_fcard.c_str());
  FNAMES.i_cardName=m_fcard.size();

  RDCUTS();
  ISEEDC.ISEED=m_seed;
  EXCUTS.EBEAME=0.5*m_cmEnergy;
  EXCUTS.W2MINE=m_m2min;
  INPARC.NTUNW=m_unw;

  SETDEF();
  QCDSTA();
  DOCUTS();
  ESTMAXCS();
  PRTCUT();
  //  USRINI();
  m_numberEvent=0;
  //for pythia parameter setting,pingrg-2013-12-11
  for(int i=0;i<m_pypars.size();i++){
    pygive_(m_pypars[i].c_str(),strlen(m_pypars[i].c_str()));
  }
  return StatusCode::SUCCESS;
}

StatusCode BesTwogam::execute() 
{ 
  MsgStream log(messageService(), name());
  log << MSG::INFO << "BesTwogam executing" << endreq;
  HepMC::HEPEVT_Wrapper::set_max_number_entries(2000);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::IO_HEPEVT HepEvtIO;
  HEPEVT_CLEAN();
  GEN1EVT();
  //PRINTEVT();
  if(CFLAGS.GOODEV!=1){
    log << MSG::ERROR<<" BesTwogam: fail to generate good event"<<endl;
    return StatusCode::FAILURE;
  }
  m_numberEvent++;
  if( log.level() < MSG::INFO )LULIST(1);
  LUHEPC(1);
  //  HEPEVT_PRINT();
  HepMC::GenEvent* evt = HepEvtIO.read_next_event();
  evt->set_event_number(m_numberEvent);
  evt->set_signal_process_id(1);
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
  return StatusCode::SUCCESS; 
}

StatusCode BesTwogam::finalize() 
{
  MsgStream log(messageService(), name());
  log << MSG::INFO << "BesTwogam finalized" << endreq;
  FINISH();
  return StatusCode::SUCCESS;
}
