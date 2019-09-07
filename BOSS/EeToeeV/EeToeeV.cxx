//*****************************************************************************
//
// EeToeeV.cxx
//
// Algorithm runs e+e- ->e+e- rho0, rho0->pi+pi-  precess 
//
// July 2016-4-29 Rong-Gang Ping to create package for BES3
// The original fortran code is generated with FDC, consult Prof. Wang Jianxiong
//*****************************************************************************

// Header for this module:-
#include "../EeToeeV/EeToeeV.h"
#include "../EeToeeV/EeToeeVRandom.h"
#include "BesKernel/IBesRndmGenSvc.h"

// Framework Related Headers:-
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "GeneratorObject/McGenEvent.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "cfortran/cfortran.h"

#include <stdlib.h>
#include <time.h> 

 typedef struct { 
   double q1[4][160001];  //e+ beam
   double p1[4][160001];  //e- beam
   double q2[4][160001];  // e-
   double p2[4][160001];  // e+
   double q3[4][160001];  // V 
 } MOMSET_DEF;
//MOMSET_DEF MOMSET; 
#define MOMSET COMMON_BLOCK(MOMSET_DEF, momset)
COMMON_BLOCK_DEF(MOMSET_DEF,MOMSET);

//common/beamEnergy/ebeam
typedef struct {
  double ecms;
} BEAMENERGY_DEF;
#define BEAMENERGY COMMON_BLOCK(BEAMENERGY_DEF, beamenergy)
COMMON_BLOCK_DEF(BEAMENERGY_DEF,BEAMENERGY);


//common/vector/vct
typedef struct {
  int vct;
} VECTOR_DEF;
#define VECTOR COMMON_BLOCK(VECTOR_DEF, vector)
COMMON_BLOCK_DEF(VECTOR_DEF,VECTOR);

//common/MCTRUTH/mccheck
typedef struct {
  int mccheck;
} MCTRUTH_DEF;
#define MCTRUTH COMMON_BLOCK(MCTRUTH_DEF, mctruth)
COMMON_BLOCK_DEF(MCTRUTH_DEF,MCTRUTH);


//--//Unify EeToeeV random engine with Bes random servive 
extern "C" {
  extern float flat_();
}
 
float flat_(){
  float rr;
  double rd=EeToeeVRandom::random();
//  std::cout<<"EeToeeVRandom::random()="<<rd<<endl;
  rr=(float)rd;
  return (float)EeToeeVRandom::random();
  //  return rr;
}
extern "C" {
  extern void intxs_();
}


PROTOCCALLSFSUB1(GEVENT,gevent,INT)
#define GEVENT(NEVENTS) CCALLSFSUB1(GEVENT,gevent,INT,NEVENTS)
                                                                                            

EeToeeV::EeToeeV(const string& name, ISvcLocator* pSvcLocator):Algorithm( name, pSvcLocator )
{
  declareProperty("Ecms", m_Ecms = 3.65); // Ecm = sqrt(s) [GeV]
  declareProperty("Vector", m_vect = "phi"); // Ecm = sqrt(s) [GeV]
  declareProperty("WriteMC", m_mctruth = 0); // Ecm = sqrt(s) [GeV]
}




StatusCode EeToeeV::initialize(){

  MsgStream log(messageService(), name());
  
  log << MSG::INFO << "EeToeeV initialize" << endreq;
  
  //set Bes unified random engine
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)
    {
      log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
      return RndmStatus;
    }
  CLHEP::HepRandomEngine* engine  = p_BesRndmGenSvc->GetEngine("EeToeeV");
  std::cout<<"==============================="<<engine<<endl;
  EeToeeVRandom::setRandomEngine(engine);
  //  *****************
  MCTRUTH.mccheck=m_mctruth;
  BEAMENERGY.ecms=m_Ecms;
  double mpar=10.;
  if(m_vect=="omega"){
    VECTOR.vct=1;
    mpar=0.782;
  }else if(m_vect=="phi"){
    VECTOR.vct=2;
    mpar=1.019;
  }else if(m_vect=="J/psi"){
    VECTOR.vct=3;
    mpar=3.097;
  }else if(m_vect=="psi(2S)"){
    VECTOR.vct=4;
    mpar=3.686;
  }else if(m_vect=="psi(3770)"){
    VECTOR.vct=5;
    mpar=3.773;
  }else if(m_vect=="psi(4040)"){
    VECTOR.vct=6;
    mpar=4.039;
  }else if(m_vect=="psi(4160)"){
    VECTOR.vct=7;
    mpar=4.153;
  }else if(m_vect=="psi(4415)"){
    VECTOR.vct=8;
    mpar=4.421;
  }else{
    std::cout<<"EeToeeV::initialize() Bad vector "<<std::endl; abort();
  }
  if(m_Ecms<mpar){std::cout<<"EeToeeV:initialize: the Ecms less than the vector mass"<<std::endl;abort();}
  //  std::cout<<"m_Ires= "<<m_Ires<<endl;

  getMaxEvent();
  std::cout<<"m_evtMax = "<<m_evtMax<<std::endl;
  intxs_();
  GEVENT(m_evtMax);
  
  return StatusCode::SUCCESS;
}

static int evtgen=1;
StatusCode EeToeeV::execute() 
{ 
  MsgStream log(messageService(), name());


  int npart = 0;
  int pid1,pid2,pid3,pid4,pst1,pst2;
  pid1 = 11;
  pid2 =-11;
  pst1 = 1;
  pst2 = 1; //1: supose the Vector will decay in the BesEvtGen

  if(m_vect=="omega"){
    pid3=223;
  }else if(m_vect=="phi"){
    pid3=333;
  }else if(m_vect=="J/psi"){
    pid3=443;
  }else if(m_vect=="psi(2S)"){
    pid3=100443;
  }else if(m_vect=="psi(3770)"){
    pid3=30443;
  }else if(m_vect=="psi(4040)"){
    pid3=9000443;
  }else if(m_vect=="psi(4160)"){
    pid3=9010443;
  }else if(m_vect=="psi(4415)"){
    pid3=9020443;
  }
 
  // Fill event information
  GenEvent* evt = new GenEvent(1,1);

  GenVertex* prod_vtx = new GenVertex();
//  prod_vtx->add_particle_out( p );
  evt->add_vertex( prod_vtx );
			  
  // incoming beam e- HepLorentzVector(px,py,pz,energy) required!
  GenParticle* p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.p1[1][evtgen],  MOMSET.p1[2][evtgen], 
				                      MOMSET.p1[3][evtgen],  MOMSET.p1[0][evtgen]),
						      11, 3); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_in(p);

//  std::cout<<"incoming beam e+"<<endl;
  // incoming beam e+, e+ moving along the z-direction
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.q1[1][evtgen],  MOMSET.q1[2][evtgen],
                                         MOMSET.q1[3][evtgen],  MOMSET.q1[4][evtgen]),
		                          -11, 3); 

  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_in(p);
  
  // scattered lepton -
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.p2[1][evtgen],  MOMSET.p2[2][evtgen], 
   		                         MOMSET.p2[3][evtgen],  MOMSET.p2[0][evtgen]),
					 pid1,pst1); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_out(p);
 
  // scattered lepton +
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.q2[1][evtgen],  MOMSET.q2[2][evtgen],
                                         MOMSET.q2[3][evtgen],  MOMSET.q2[0][evtgen]),
		                        pid2, pst1); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_out(p);

  // outgoing Vector
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.q3[1][evtgen],  MOMSET.q3[2][evtgen], 
   		                         MOMSET.q3[3][evtgen],  MOMSET.q3[0][evtgen]),
		                        pid3,pst2); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_out(p);

  evtgen++;

  if( log.level() < MSG::INFO )
  {
    evt->print();  
  }

  // Check if the McCollection already exists
  SmartDataPtr<McGenEventCol> anMcCol(eventSvc(), "/Event/Gen");
  if (anMcCol!=0) 
  {
    // Add event to existing collection
    MsgStream log(messageService(), name());
    log << MSG::INFO << "Add McGenEvent to existing collection" << endreq;
    McGenEvent* mcEvent = new McGenEvent(evt);
    anMcCol->push_back(mcEvent);
  }
  else 
  {
    // Create Collection and add  to the transient store
    // std::cout<<"I created McCollection "<<std::endl;
    McGenEventCol *mcColl = new McGenEventCol;
    McGenEvent* mcEvent = new McGenEvent(evt);
    mcColl->push_back(mcEvent);
    StatusCode sc = eventSvc()->registerObject("/Event/Gen",mcColl);
    if (sc != StatusCode::SUCCESS) 
    {
      log << MSG::ERROR << "Could not register McGenEvent" << endreq;
      delete mcColl;
      delete evt;
      delete mcEvent;
      return StatusCode::FAILURE;
    }
    else 
    {
      //   log << MSG::INFO << "McGenEventCol created and " << npart <<" particles stored in McGenEvent" << endreq;
    }
  }
     
  return StatusCode::SUCCESS; 
}

StatusCode EeToeeV::finalize() 
{
  MsgStream log(messageService(), name());
   char delcmd[300];
   strcpy(delcmd,"cat ");
   strcat(delcmd,"fresult.dat");
   system(delcmd);

  std::cout<< "EeToeeV finalized" << endl;
  return StatusCode::SUCCESS;
}

StatusCode EeToeeV::getMaxEvent() {
  IProperty* appPropMgr=0;
  StatusCode status = 
    serviceLocator()->getService("ApplicationMgr", IProperty::interfaceID(),
				 reinterpret_cast<IInterface*&>( appPropMgr ));
  if( status.isFailure() ) return status;
    
  IntegerProperty evtMax("EvtMax",0);
  status = appPropMgr->getProperty( &evtMax );
  if (status.isFailure()) return status;
  
  m_evtMax = evtMax.value();
  return status;
}

