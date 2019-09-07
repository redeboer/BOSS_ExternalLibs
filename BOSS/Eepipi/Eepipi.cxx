//*****************************************************************************
//
// Eepipi.cxx
//
// Algorithm runs e+e- ->e+e- rho0, rho0->pi+pi-  precess 
//
// July 2016-4-29 Rong-Gang Ping to create package for BES3
// The original fortran code is generated with FDC, consult Prof. Wang Jianxiong
//*****************************************************************************

// Header for this module:-
#include "../Eepipi/Eepipi.h"
#include "../Eepipi/EepipiRandom.h"
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
   double q3[4][160001];  // pi+ 
   double p3[4][160001];  // pi-
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
//BEAMENERGY_DEF,BEAMENERGY

//common/cosee/setcos
typedef struct {
  double setcos;
} COSEE_DEF;
#define COSEE COMMON_BLOCK(COSEE_DEF, cosee)
COMMON_BLOCK_DEF(COSEE_DEF,COSEE);

//common/MCTRUTH/mccheck
typedef struct {
  int mccheck;
} MCTRUTH_DEF;
#define MCTRUTH COMMON_BLOCK(MCTRUTH_DEF, mctruth)
COMMON_BLOCK_DEF(MCTRUTH_DEF,MCTRUTH);

//--//Unify Eepipi random engine with Bes random servive 
extern "C" {
  extern float flat_();
}
 
float flat_(){
  float rr;
  double rd=EepipiRandom::random();
//  std::cout<<"EepipiRandom::random()="<<rd<<endl;
  rr=(float)rd;
  return (float)EepipiRandom::random();
  //  return rr;
}
extern "C" {
  extern void intxs_();
}


PROTOCCALLSFSUB1(GEVENT,gevent,INT)
#define GEVENT(NEVENTS) CCALLSFSUB1(GEVENT,gevent,INT,NEVENTS)
                                                                                            

Eepipi::Eepipi(const string& name, ISvcLocator* pSvcLocator):Algorithm( name, pSvcLocator )
{
  declareProperty("Ecms", m_Ecms = 3.65); // Ecm = sqrt(s) [GeV]
//  declareProperty("cosee", m_cosee = 0.99); // set costheta for e+ e- in final state 
  declareProperty("WriteMC", m_mctruth = 0); //write out the MC truth of final state to "pdata1.dat"
}

StatusCode Eepipi::initialize(){

  MsgStream log(messageService(), name());
  
  log << MSG::INFO << "Eepipi initialize" << endreq;
  
  //set Bes unified random engine
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)
    {
      log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
      return RndmStatus;
    }
  CLHEP::HepRandomEngine* engine  = p_BesRndmGenSvc->GetEngine("Eepipi");
  std::cout<<"==============================="<<engine<<endl;
  EepipiRandom::setRandomEngine(engine);
  //  *****************
  MCTRUTH.mccheck=m_mctruth;
  BEAMENERGY.ecms=m_Ecms;
  COSEE.setcos=m_cosee; 	
  //  std::cout<<"m_Ires= "<<m_Ires<<endl;

  getMaxEvent();
  std::cout<<"m_evtMax = "<<m_evtMax<<std::endl;
  intxs_();
  GEVENT(m_evtMax);
  
  return StatusCode::SUCCESS;
}

static int evtgen=1;
StatusCode Eepipi::execute() 
{ 
  MsgStream log(messageService(), name());


  int npart = 0;
  int pid1,pid2,pid3,pid4,pst1,pst2;
  pid1 = 11;
  pid2 =-11;
  pid3 = 211;
  pid4 =-211;
  pst1 = 1;
  pst2 = 1;



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
  
  // scattered lepton +
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.p2[1][evtgen],  MOMSET.p2[2][evtgen], 
   		                         MOMSET.p2[3][evtgen],  MOMSET.p2[0][evtgen]),
					 pid1,pst1); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_out(p);
 
  // scattered lepton -
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.q2[1][evtgen],  MOMSET.q2[2][evtgen],
                                         MOMSET.q2[3][evtgen],  MOMSET.q2[0][evtgen]),
		       pid2, pst2); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_out(p);

  // outgoing pi+
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.p3[1][evtgen],  MOMSET.p3[2][evtgen], 
   		                         MOMSET.p3[3][evtgen],  MOMSET.p3[0][evtgen]),
		      pid3,pst1); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_out(p);

  //outgoing pi-
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.q3[1][evtgen],  MOMSET.q3[2][evtgen],
                                         MOMSET.q3[3][evtgen],  MOMSET.q3[0][evtgen]),
		       pid4, pst2); 
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

StatusCode Eepipi::finalize() 
{
  MsgStream log(messageService(), name());
   char delcmd[300];
   strcpy(delcmd,"cat ");
   strcat(delcmd,"fresult.dat");
   system(delcmd);

  std::cout<< "Eepipi finalized" << endl;
  return StatusCode::SUCCESS;
}

StatusCode Eepipi::getMaxEvent() {
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

