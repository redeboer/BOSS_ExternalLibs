//*****************************************************************************
//
// Bhlumi.cxx
//
// Algorithm runs small angle  Bhabha event generator BHLUMI 
// and stores output to transient store
//
// Jan 2006 A. Zhemchugov, initial version written for BES3
// Oct 2006 K.L. He, add initial seed interface  
//*****************************************************************************

// Header for this module:-
#include "Bhlumi/Bhlumi.h"
#include "Bhlumi/BhlumiRandom.h"

// Framework Related Headers:-
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "GeneratorObject/McGenEvent.h"
#include "BesKernel/IBesRndmGenSvc.h"

#include "cfortran/cfortran.h"

#include <stdlib.h>

using namespace std;

//      COMMON / momset / p1(4),q1(4),p2(4),q2(4),phot(100,4),nphot
//      * COMMON / wgtall / wtmod,wtcru1,wtcru2,wtset(300)

typedef struct { 
 double p1[4]; 
 double q1[4]; 
 double p2[4]; 
 double q2[4]; 
 double phot[4][100]; 
 int nphot; } MOMSET_DEF;

#define MOMSET COMMON_BLOCK(MOMSET_DEF, momset)
COMMON_BLOCK_DEF(MOMSET_DEF,MOMSET);
//MOMSET_DEF MOMSET; 

//-------------------------

PROTOCCALLSFSUB1(GLIMIT,glimit,INT)
#define GLIMIT(LENMX) CCALLSFSUB1(GLIMIT,glimit,INT,LENMX)
                                                                                            
PROTOCCALLSFSUB1(DUMPS,dumps,INT)
#define DUMPS(NOUT) CCALLSFSUB1(DUMPS,dumps,INT,NOUT)
                                                                                            
PROTOCCALLSFSUB3(BHLUMI,bhlumi,INT,DOUBLEV,INTV)
#define BHLUMI(MODE,XPAR,NPAR) CCALLSFSUB3(BHLUMI,bhlumi,INT,DOUBLEV,INTV,MODE,XPAR,NPAR)

PROTOCCALLSFSUB3(MARINI, marini, INT, INT, INT)
#define MARINI(IJKLIN, NTOTIN, NTOT2N) CCALLSFSUB3(MARINI, marini, INT, INT, INT, IJKLIN, NTOTIN, NTOT2N)

extern "C" {                                                                                           
  extern double ranmarr_();
  extern void marran_(float* rvec, int* nma);
  extern void ecuran_(double* rvec, int* nma);
  extern void carran_(double* rvec, int* nma);
}

double ranmarr_()
{
  return BhlumiRandom::random();
}

void marran_(float* rvec, int* nma)
{
  int nmax = *nma;
  assert(nmax<100);
  double rvecd[100];
  BhlumiRandom::FlatArray(rvecd, nmax);
  for(int i=0; i<nmax; i++) rvec[i]=rvecd[i];
}

void carran_(double* rvec, int* nma)
{
  int nmax = *nma;
  assert(nmax<100);
  double rvecd[100];
  BhlumiRandom::FlatArray(rvecd, nmax);
  for(int i=0; i<nmax; i++) rvec[i]=rvecd[i];
}

void ecuran_(double* rvec, int* nma)
{ 
  int nmax = *nma;
  assert(nmax<100);
  double rvecd[100];
  BhlumiRandom::FlatArray(rvecd, nmax);
  for(int i=0; i<nmax; i++) rvec[i]=rvecd[i];
}


Bhlumi::Bhlumi(const string& name, ISvcLocator* pSvcLocator):Algorithm( name, pSvcLocator )
{
  declareProperty("CMEnergy", m_cmEnergy = 3.097); // 2*Ebeam [GeV]
  declareProperty("AngleMode", m_angleMode = 0);   //0: rad; 1: degree; 2: cos(theta);
                                                   //if 1 or 2, angle will be controlled absolutely by user
  declareProperty("MinThetaAngle", m_minThetaAngle = 0.24); // [rad]
  declareProperty("MaxThetaAngle", m_maxThetaAngle = 0.58); // [rad]
  declareProperty("SoftPhotonCut", m_infraredCut   = 1E-4); // dimensionless, Ephoton > m_infraredCut*sqrt(s)/2
  m_initSeed.clear();
 // m_initSeed.push_back(54217137); m_initSeed.push_back(0); m_initSeed.push_back(0);
 // declareProperty("InitializedSeed", m_initSeed);
}

StatusCode Bhlumi::initialize(){

  MsgStream log(messageService(), name());
  
  log << MSG::INFO << "Bhlumi initialize" << endreq;

  static const bool CREATEIFNOTTHERE(true);                                                            
  StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);                 
  if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)                                                 
  {                                                                                                    
    log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;                      
    return RndmStatus;                                                                                 
  }                                                                                                    
  CLHEP::HepRandomEngine* engine  = p_BesRndmGenSvc->GetEngine("Bhlumi");                            
  engine->showStatus();
  BhlumiRandom::setRandomEngine(engine);      

  GLIMIT(50000);
                                                                                            
/*!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
! User should cross-check the folowing two output cross sections
! which are calculated and printed at the very end of the output:
! Workshop95, Table14, BARE1 WW for zmin=0.5:  KeyGen=3, KeyPia=0,
KeyZet=0
! Workshop95, Table18, CALO2 WW for zmin=0.5:  KeyGen=3, KeyPia=2,
KeyZet=1
!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
*/
//!---------------------------------------------------------------------------
//!         Input parameters for  Bhlumi
//!----------------------------------------------------------------------
//! Technical parameters, nothing should depend on them:
  int KeyGen =   3;  // ! Multiphoton Bhlumi
  int KeyRem =   1;  // ! No remooval of photons below epsCM
  KeyRem =   0;  // ! Remooval of photons below epsCM, Necessary for Z!!!
 //! Try both options for KeyWgt, result should be the same
  int KeyWgt =   2;  // ! weighted events, with t generation down to zero
  KeyWgt =   0;  // ! WT=1 events, for detector simulation
  int KeyRnd =   1;  // ! RANMAR random numbers
  int KeyOpt =1000*KeyGen +100*KeyRem +10*KeyWgt +KeyRnd;
 //!--------------------------------------------------
 //! Physics parameters:
  int KeyPia =   0;  // ! Vacuum polarization OFF
  int KeyMod =   2;  //   ! Matrix element default version 4.x
  KeyPia =   2;  //   ! Vacuum polarization ON
  int  KeyZet =   0;  //   ! Z contribution OFF
  KeyZet =   1;  //   ! Z contribution ON
  int KeyRad =1000*KeyZet +10*KeyMod +KeyPia;
 //!--------------------------------------
  npar[0]=   KeyOpt;
  npar[1]=   KeyRad;
  double CmsEne =  m_cmEnergy;  //! 2*Ebeam [GeV], as in Workshop95
  xpar[0]=   CmsEne;
  double th1,th2,thmin,thmax;
  if(m_angleMode==0){
    th1    =  m_minThetaAngle;  //       ! Detector range ThetaMin [rad]
    th2    =  m_maxThetaAngle;  //       ! Detector range ThetaMax [rad]
    thmin    = 0.7*th1;  //   ! thmin has to be lower  than th1
    thmax    = 2.0*th2;  //   ! thmax has to be higher than th2
    if(thmin<0.||thmax>3.1415926) {
      log << MSG::WARNING << "input angles exceed range (0~pi), so effect will be unprospectable" << endreq;
      return StatusCode::FAILURE;
    }
  }
  else if(m_angleMode==1){
    th1 = m_minThetaAngle*3.1415926/180.;
    th2 = m_maxThetaAngle*3.1415926/180.;
    // not multiply  0.7(2.0) coefficient while large angle
    thmin    = th1;
    thmax    = th2;
  }
  else if(m_angleMode==2){
    th1 = acos(max(m_minThetaAngle,m_maxThetaAngle));
    th2 = acos(min(m_minThetaAngle,m_maxThetaAngle));
    // not multiply  0.7(2.0) coefficient while large angle
    thmin    = th1;
    thmax    = th2;
  }
  else{
    log << MSG::FATAL << "unknown angle mode!" << endreq;
    return StatusCode::FAILURE;
  }
  if(thmin<0.||thmax>3.1415926) {
    log << MSG::FATAL << "input angles exceed range (0~pi), unprospectable" << endreq;
    return StatusCode::FAILURE;
  }
  else if(thmin>thmax) {
    log << MSG::FATAL << "thmin>thmax, unprospectable" << endreq;
    return StatusCode::FAILURE;
  }
  if(KeyWgt == 2) thmin=th1;  //  ! Because generation below th1 is on!!!
  xpar[1]=   CmsEne*CmsEne*(1-cos(thmin))/2;  //  ! TransMin [GeV**2]
  xpar[2]=   CmsEne*CmsEne*(1-cos(thmax))/2;  //  ! TransMax [GeV**2]
  xpar[3]=   m_infraredCut;  //        ! Infrared cut on photon energy

 // MARINI(m_initSeed[0], m_initSeed[1], m_initSeed[2]);

  BHLUMI(-1,xpar,npar);

  return StatusCode::SUCCESS;
}

StatusCode Bhlumi::execute() 
{ 
  MsgStream log(messageService(), name());
  log << MSG::INFO << "Bhlumi executing" << endreq;


  BHLUMI( 0,xpar,npar);

  if( log.level() < MSG::INFO )
  {
    DUMPS(6);
   // dump output to file
   //  DUMPS(16);
  }

  int npart = 0;
  
  // Fill event information
  GenEvent* evt = new GenEvent(1,1);

  GenVertex* prod_vtx = new GenVertex();
//  prod_vtx->add_particle_out( p );
  evt->add_vertex( prod_vtx );
			  
  // incoming beam e+
  GenParticle* p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.p1[0],  MOMSET.p1[1], 
				                      MOMSET.p1[2],  MOMSET.p1[3]),
						      -11, 3); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_in(p);

  // incoming beam e-
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.q1[0],  MOMSET.q1[1], MOMSET.q1[2],  MOMSET.q1[3]),
		       11, 3); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_in(p);
  
  // scattered e+
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.p2[0],  MOMSET.p2[1], 
   		                         MOMSET.p2[2],  MOMSET.p2[3]),
					 -11, 1); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_out(p);

  // scattered e-
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.q2[0],  MOMSET.q2[1], MOMSET.q2[2],  MOMSET.q2[3]),
		       11, 1); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_out(p);

  int iphot=0;
  for (iphot=0; iphot<MOMSET.nphot; iphot++)
  {
    // gamma
    p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.phot[0][iphot],  MOMSET.phot[1][iphot], 
  					   MOMSET.phot[2][iphot],  MOMSET.phot[3][iphot]),
		                           22, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
  }
  
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
      log << MSG::INFO << "McGenEventCol created and " << npart <<" particles stored in McGenEvent" << endreq;
    }
  }

  // retrieve event from Transient Store (Storegate)
/*  SmartDataPtr<McGenEventCol> McEvtColl(eventSvc(), "/Event/Gen");	
  if ( McEvtColl == 0 ) 
  {
    log << MSG::ERROR << "Could not retrieve McGenEventCollection" << endreq;
    return StatusCode::FAILURE;
  };
  
  McGenEventCol::iterator mcItr;
  for( mcItr = McEvtColl->begin(); mcItr != McEvtColl->end(); mcItr++ )   
  {
     HepMC::GenEvent* hEvt = (*mcItr)->getGenEvt();
    // MeVToGeV( hEvt );
//     callEvtGen( hEvt );
    // GeVToMeV( hEvt );
  };
*/
     
  return StatusCode::SUCCESS; 
}

StatusCode Bhlumi::finalize() 
{
  MsgStream log(messageService(), name());

  BHLUMI( 2,xpar,npar);

  log << MSG::INFO << "Bhlumi finalized" << endreq;

  return StatusCode::SUCCESS;
}
