//--------------------------------------------------------------------------
//
//------------------------------------------------------------------------
// Header for this module:-
#include "BesGenModule/BesGenModule.h"
#include "GeneratorModule/GeneratorName.h"

// Framework Related Headers:-
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// Other classes used by this class:-
//#include "StoreGate/StoreGateSvc.h"
//#include "StoreGate/DataHandle.h"

//#include "GeneratorObject/McEventCollection.h"
#include "GeneratorObject/McGenEvent.h"
#include "McTruth/DecayMode.h"

//#include "HepMC/GenEvent.h"
#include "HepMC/GenEvent.h"

//#include "HepMC/ParticleDataTable.h"
//#include "HepMC/ParticleData.h"
#include "HepMC/IO_HEPEVT.h"
//#include "HepMC/IO_HERWIG.h"
#include "HepMC/HEPEVT_Wrapper.h"
#include "McTruth/McEvent.h"


#include "GeneratorObject/McGenEvent.h"
#include "GeneratorUtil/StringParse.h"
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>
//for random seed of genbes 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "BesKernel/IBesRndmGenSvc.h"
//-------------------------------

// Pointer On AtRndmGenSvc
#include "CLHEP/Random/RandFlat.h"

std::string BesGenModule::besMcGen_stream="PHOTOS_INIT";

// calls to fortran routines


extern "C" {
  void mainini_(int *,const char *);
//  void mainini_(int*);
  void mainexe_(int*,int* mvec);
  void mainfin_(int*);
  void runi_seed_(int*);
}
//-----------------------------------
//using HepMC::ParticleData;
//using HepMC::IO_HERWIG;
using HepMC::IO_HEPEVT;
using HepMC::HEPEVT_Wrapper;


// File scope declarations:-

// set pointer to zero at start
// Bes_HEPEVT*  BesMcGen::bes_HEPEVT = new Bes_HEPEVT();

//--------------------------------------------------------------------------
BesGenModule::BesGenModule(const std::string& name, 
      ISvcLocator* pSvcLocator): GenModule(name,pSvcLocator)
{
//--------------------------------------------------------------------------  
  besMcGen_stream	=	"PHOTOS_INIT";
//  declareProperty("BesMcGenCommand", m_BesMcGenCommandVector);

  declareProperty("Runcards",  runname = "run.cards");
  declareProperty("Ratio",     Nr = 1);  

}

BesGenModule::~BesGenModule()
{}

//-------------------------------------------------------------
//--------------
// Operations --
//--------------

/*
Ffphot& BesGenModule::ffphot(){
  return m_ffphot;
}

Ffpholun& BesGenModule::ffpholun(){
  return m_ffpholun;
}
*/

//---------------------------------------------------------------------------
StatusCode BesGenModule::genInitialize() {
//---------------------------------------------------------------------------

  MsgStream log(messageService(), name());
  log << MSG::INFO << " BES PHOTOS INITIALISING.  \n"  << endreq;
//  log << MSG::INFO << " BES PHOTOS 2.  \n"  << endreq;

  //caogf add for random seed of genbes
  StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, true);
  if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)
  {
     log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
     return RndmStatus;
  }

  BesGenModule::besMcGen_stream	=	"PHOTOS_INIT";

  BesGenModule::besMcGen_stream	=	"PHOTOS111";

  m_events = 0;
  n=0;
  cardname = runname.c_str();


  mainini_(&n,cardname);

  int theseed;
  runi_seed_(&theseed);
  p_BesRndmGenSvc->setGenseed(theseed+1);

  HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(4);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(2000);

 
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode BesGenModule::callGenerator() {
//---------------------------------------------------------------------------
  MsgStream log(messageService(), name());
  log << MSG::INFO << " BesGenModule generating.  \n"  << endreq;

  StatusCode status = StatusCode::SUCCESS;
  int iq;  

//  n=1;


  mainexe_(&Nr,mvec);

/*
    for (int iq = 1; iq < 11; iq++) {
       nvec[iq-1]=mvec[iq];
    }
*/

    McEvent* aMcEvent = new McEvent;
    StatusCode sc = eventSvc()->registerObject("/Event/MC",aMcEvent);


    SmartDataPtr<DecayMode> anMcCol(eventSvc(), "/Event/MC/DecayMode");
    if (anMcCol!=0) {
//     log << MSG::INFO << "Add McEvent to existing collection" << endreq;
    }
    else {
//     log << MSG::INFO << "No decay mode now" << endreq;
	DecayMode *decayMode = new DecayMode;
	decayMode->putData(mvec, 10);
    StatusCode sc = eventSvc()->registerObject("/Event/MC/DecayMode",decayMode);   
    }


    SmartDataPtr<DecayMode> aMcCol(eventSvc(), "/Event/MC/DecayMode");
    if (aMcCol!=0) {   
//     log << MSG::INFO << "Haha signal exist!!!!" << endreq;
    }




//  HEPEVT_Wrapper::print_hepevt();

  ++m_events;
  


  log << MSG::INFO << " BESGenModule generating done.  \n"  << endreq;
  return status;

}

//---------------------------------------------------------------------------
StatusCode BesGenModule::genFinalize() {
  //---------------------------------------------------------------------------
  MsgStream log(messageService(), name());
 mainfin_(&n);
  log << MSG:: INFO << " BESGenModule Ending...  \n"  << endreq;
  return StatusCode::SUCCESS;
}
//---------------------------------------------------------------------------
StatusCode BesGenModule::fillEvt(GenEvent* evt) {
  //---------------------------------------------------------------------------
  MsgStream log(messageService(), name());
  log << MSG:: INFO << " BESGenModule Filling...  \n"  << endreq;

      evt->set_event_number(m_events);
      HepMC::IO_HEPEVT hepio;
      hepio.fill_next_event(evt);
//   cmTomm(evt);
//   GeVToMeV(evt);


 return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
