//*****************************************************************************
//
// Bhwide.cxx
//
// Algorithm runs large angle Bhabha event generator Bhwide 
// and stores output to transient store
//
// Aug 2007 A. Zhemchugov, initial version written for BES3
//*****************************************************************************

// Header for this module:-
#include "Bhwide/Bhwide.h"
#include "Bhwide/BhwideRandom.h"

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
                                                                                            
PROTOCCALLSFSUB3(BHWIDE,bhwide,INT,DOUBLEV,INTV)
#define BHWIDE(MODE,XPAR,NPAR) CCALLSFSUB3(BHWIDE,bhwide,INT,DOUBLEV,INTV,MODE,XPAR,NPAR)

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
  return BhwideRandom::random();
}

void marran_(float* rvec, int* nma)
{
  int nmax = *nma;
  assert(nmax<100);
  double rvecd[100];
  BhwideRandom::FlatArray(rvecd, nmax);
  for(int i=0; i<nmax; i++) rvec[i]=rvecd[i];
}

void carran_(double* rvec, int* nma)
{
  int nmax = *nma;
  assert(nmax<100);
  double rvecd[100];
  BhwideRandom::FlatArray(rvecd, nmax);
  for(int i=0; i<nmax; i++) rvec[i]=rvecd[i];
}

void ecuran_(double* rvec, int* nma)
{
  int nmax = *nma;
  assert(nmax<100);
  double rvecd[100];
  BhwideRandom::FlatArray(rvecd, nmax);
  for(int i=0; i<nmax; i++) rvec[i]=rvecd[i];
}




//float ranmar_(){
//    return BhwideRandom::random();                                                              
//}                                                                                                      
                                                                                                       
Bhwide::Bhwide(const string& name, ISvcLocator* pSvcLocator):Algorithm( name, pSvcLocator )
{
  declareProperty("CMEnergy", m_cmEnergy = 3.097); // 2*Ebeam [GeV]
  declareProperty("EleMinThetaAngle", m_ThMine = 22); // [deg]
  declareProperty("EleMaxThetaAngle", m_ThMaxe = 158); // [deg]
  declareProperty("PosMinThetaAngle", m_ThMinp = 22); // [deg]
  declareProperty("PosMaxThetaAngle", m_ThMaxp = 158); // [deg]
  declareProperty("EleMinEnergy", m_EnMine = 0.01); // [GeV]
  declareProperty("PosMinEnergy", m_EnMinp = 0.01); // [GeV]
  declareProperty("Acollinearity", m_Acolli = 10);   // [deg]
  declareProperty("SoftPhotonCut", m_infraredCut   = 1E-5); // dimensionless, Ephoton > m_infraredCut*sqrt(s)/2
  declareProperty("VacuumPolarization", m_keyPia   = 3); // 0 - OFF, 1 - ON, Burkhardt et.al. 1989, as in BHLUMI 2.0x, 2 - ON, S. Eidelman, F. Jegerlehner, Z.Phys.C(1995), 3 - ON, Burkhardt and Pietrzyk 1995 (Moriond).
  declareProperty("KeyMod", m_keyMod   = 2); // type of MODEL subprogram and QED matrix element for hard bremsstrahlung: 1 - obtained by the authors (helicity amplitudes), 2 - from CALKUL, Nucl. Phys. B206 (1982) 61. Checked to be in a very good agreement!
  declareProperty("KeyLib", m_keyLib   = 2); // option for ElectroWeak Corrections Library: 1 - ElectroWeak Corr. from BABAMC (obsolete), 2 - ElectroWeak Corr. from ALIBABA, RECOMMENDED 
  declareProperty("EWC", m_keyEwc   = 1); // switching ON/OFF weak corrections: 0 - only QED corrections included (here both KeyLib =1,2 should be equivalent), 1 - all ElectroWeak Corrections included 
  m_initSeed.clear();
//  m_initSeed.push_back(54217137); m_initSeed.push_back(0); m_initSeed.push_back(0);
//  declareProperty("InitializedSeed", m_initSeed);
}

StatusCode Bhwide::initialize(){

  MsgStream log(messageService(), name());
  
  log << MSG::INFO << "Bhwide initialize" << endreq;

  static const bool CREATEIFNOTTHERE(true);                                                            
  StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);                 
  if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)                                                 
  {                                                                                                    
    log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;                      
    return RndmStatus;                                                                                 
  }                                                                                                    
  CLHEP::HepRandomEngine* engine  = p_BesRndmGenSvc->GetEngine("Bhwide");                            
  engine->showStatus();
  BhwideRandom::setRandomEngine(engine);                                                             

  GLIMIT(50000);

//!---------------------------------------------------------------------------
//!         Input parameters for  Bhwide
//!----------------------------------------------------------------------
  double WTMAX  =  3.0; //    ! Maximum Weight for rejection
  double AMAZ   =  91.1882; // ! Z mass
  double GAMMZ  =  2.4952; //  ! Z width (may be recalculated by EW library)
  double SINW2  =  0.22225; // ! sin^2(theta_W) (may be recalculated by EW library)   
  double AMTOP  = 174.3; //    ! top quark mass
  double AMHIG  = 115.0; //    ! Higgs mass      
  //! Try both options for KeyWgt, result should be the same
  int KeyWgt =   0;     // ! unweighted (WT=1) events, for detector simulation
  //!#      KeyWgt = 1  // ! weighted events
  int    KeyRnd =   1;  // ! RANMAR random numbers
  int    KeyCha =   0;  // ! Channel choice: all/s-only/t-only: =0/1/2
  int    KeyZof =   0;  // ! Z-contribution ON/OFF: =0/1
  int    KeyOpt = 1000*KeyZof +100*KeyCha +10*KeyWgt + KeyRnd;
  // !#      KeyEWC = 0   ! QED corrections only
  int    KeyEWC =   m_keyEwc;  // ! Total O(alpha) ElectroWeak corr. included
  // !#      KeyLib = 1   ! ElectroWeak corrections from BABAMC (obsolete!)
  int   KeyLib =  m_keyLib;  // ! ElectroWeak corrections from ALIBABA
  // !#      KeyMod = 1   ! Hard bremsstr. matrix element from MODEL1
  int    KeyMod =   m_keyMod;  // ! Hard bremsstr. matrix alement from MODEL2
  int    KeyPia =   m_keyPia;  // ! Vacuum polarization option (0/1/2/3)
  int    KeyRad = 1000*KeyEWC + 100*KeyLib + 10*KeyMod + KeyPia;
  
 //!--------------------------------------
  npar[0]=   KeyOpt;
  npar[1]=   KeyRad;

  xpar[0] = m_cmEnergy;  //! 2*Ebeam [GeV]
  xpar[1] = m_ThMinp;
  xpar[2] = m_ThMaxp;
  xpar[3] = m_ThMine;
  xpar[4] = m_ThMaxe;
  xpar[5] = m_EnMinp;
  xpar[6] = m_EnMine;
  xpar[7] = m_Acolli;
  xpar[8] = m_infraredCut;  //        ! Infrared cut on photon energy
  xpar[9]  = WTMAX;
  xpar[10] = AMAZ;
  xpar[11] = GAMMZ;
  xpar[12] = SINW2;
  xpar[13] = AMTOP;
  xpar[14] = AMHIG;
  
  //MARINI(m_initSeed[0], m_initSeed[1], m_initSeed[2]);
  
  BHWIDE(-1,xpar,npar);
  
  return StatusCode::SUCCESS;
}

StatusCode Bhwide::execute() 
{ 
  MsgStream log(messageService(), name());
  log << MSG::INFO << "Bhwide executing" << endreq;


  BHWIDE( 0,xpar,npar);

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

  return StatusCode::SUCCESS; 
}

StatusCode Bhwide::finalize() 
{
  MsgStream log(messageService(), name());

  BHWIDE( 2,xpar,npar);

  log << MSG::INFO << "Bhwide finalized" << endreq;

  return StatusCode::SUCCESS;
}
