//*****************************************************************************
//
// BABAYAGA.cxx
//
// Algorithm runs BHABHA precess 
//
// July 2007 Rong-Gang Ping to migrate BABAYAGA3.5 for BES3
// The accuracy is about 0.1% (see Nucl. Phys. B 758(2006) 227-253)
//*****************************************************************************

// Header for this module:-
#include "../Babayaga/Babayaga.h"
#include "../Babayaga/BabayagaRandom.h"
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

//COMMON/MOMSET/PM,PP,PFINEL,PFINPOS,QPH
//dimension PM(0:3),PP(0:3),PFINEL(0:3),PFINPOS(0:3),QPH(40,0:3)  //OUTPUT RESULTS PM(0:3),PP(0:3) for beam electron and positron 
//particle

 typedef struct { 
 double q1[4][160001]; 
 double p1[4][160001]; 
 double q2[4][160001]; 
 double p2[4][160001]; 
 double phot[4][10][160001]; 
 } MOMSET_DEF;
//MOMSET_DEF MOMSET; 
#define MOMSET COMMON_BLOCK(MOMSET_DEF, momset)
COMMON_BLOCK_DEF(MOMSET_DEF,MOMSET);
typedef struct { 
  double CutNgam,CutEgam;
} RADBB_DEF;
//RADBB_DEF RADBB; 
#define RADBB COMMON_BLOCK(RADBB_DEF, radbb)
COMMON_BLOCK_DEF(RADBB_DEF,RADBB);

//COMMON/ISRPHOTONS/NCQPH
typedef struct{
  int ncqph[160000];
}ISRPHOTONS_DEF; 
#define ISRPHOTONS COMMON_BLOCK(ISRPHOTONS_DEF, isrphotons)
COMMON_BLOCK_DEF(ISRPHOTONS_DEF,ISRPHOTONS);
 
//common/beamEnergy/ebeam
typedef struct {
double ebeam;
} BEAMENERGY_DEF;
#define BEAMENERGY COMMON_BLOCK(BEAMENERGY_DEF, beamenergy)
COMMON_BLOCK_DEF(BEAMENERGY_DEF,BEAMENERGY);
//BEAMENERGY_DEF,BEAMENERGY

//common/expcuts/thmin,thmax,emin,zmax,egmin,thgmin,thgmax
typedef struct {
double thmin,thmax,emin,zmax,egmin,thgmin,thgmax;
} EXPCUTS_DEF;
#define EXPCUTS COMMON_BLOCK(EXPCUTS_DEF, expcuts)
COMMON_BLOCK_DEF(EXPCUTS_DEF,EXPCUTS);
//EXPCUTS_DEF,EXPCUTS;

//common/switchFSR/ON,ILL
typedef struct{
int on,ill;
} SWITCH_DEF;
#define SWITCH 	COMMON_BLOCK(SWITCH_DEF,switchfsr)
COMMON_BLOCK_DEF(SWITCH_DEF,SWITCH);

//common/switcharun/IARUN
typedef struct{
int iarun;
} SWITCHARUN_DEF;
#define  SWITCHARUN  COMMON_BLOCK(SWITCHARUN_DEF,switcharun)
COMMON_BLOCK_DEF(SWITCHARUN_DEF,SWITCHARUN);

//COMMON/CHANNEL/ICH
typedef struct{
int ich;
} CHANNEL_DEF;
#define  CHANNEL  COMMON_BLOCK(CHANNEL_DEF,channel)
COMMON_BLOCK_DEF(CHANNEL_DEF,CHANNEL);

//common/resonances/IRES
typedef struct{
int ires;
} RESONANCES_DEF;
#define  RESONANCES  COMMON_BLOCK(RESONANCES_DEF,resonances)
COMMON_BLOCK_DEF(RESONANCES_DEF,RESONANCES); 

//COMMON/DECLAREINT/INT
typedef struct{
int seed;
} DECLAREINT_DEF;
#define  DECLAREINT  COMMON_BLOCK(DECLAREINT_DEF,declareint)
COMMON_BLOCK_DEF(DECLAREINT_DEF, DECLAREINT );

//COMMON/DECLARESTR/INTUPLE,PHCUT,CUTG
typedef struct{
char tuple,phcut,cutg;
} DECLARESTR_DEF;   
#define  DECLARESTR  COMMON_BLOCK(DECLARESTR_DEF,declarestr)
COMMON_BLOCK_DEF(DECLARESTR_DEF,DECLARESTR);



//--//Unify Babayaga random engine with Bes random servive 
extern "C" {
  extern float flat_();
}
 
float flat_(){
  float rr;
  double rd=BabayagaRandom::random();
//  std::cout<<"BabayagaRandom::random()="<<rd<<endl;
  rr=(float)rd;
  return (float)BabayagaRandom::random();
  //  return rr;
}



PROTOCCALLSFSUB1(BABAYAGA,babayaga,INT)
#define BABAYAGA(NEVENTS) CCALLSFSUB1(BABAYAGA,babayaga,INT,NEVENTS)
                                                                                            

Babayaga::Babayaga(const string& name, ISvcLocator* pSvcLocator):Algorithm( name, pSvcLocator )
{
  //  declareProperty("Seed", m_Int = 62341342); // seed for random number generator
  declareProperty("Channel", m_Ich = 1); // 1: e+e-->e+e-;2:e+e_->mu+mu-;3:e+e-->gamma gamma;4:e+e--->pi+pi-
  declareProperty("Ebeam", m_Ebeam = 1.548); // Ecm = 2*Ebeam [GeV]
  declareProperty("MinThetaAngle", m_Thmin = 70); // [degree]
  declareProperty("MaxThetaAngle", m_Thmax = 178); // [degree]
  declareProperty("MinimumEnergy", m_Emin   = 0.4); // Minimum Energy(GeV)
  declareProperty("MaximumAcollinearity", m_Zmax   = 10); //Maximum acollinearity (degree)
  declareProperty("RunningAlpha", m_Iarun   = 1); //running alpha, 0=off, 1=on
  declareProperty("HadronicResonance", m_Ires   = 0); //hadronic resoances for ICH=1 or 2
  declareProperty("FSR_swich", m_on   = 0); // FSR switch for ICH=2  ( 0=off, 1=on) 
  declareProperty("MinEnerCutG", m_Egmin   = 0.01); // minimum energy for CUTG=Y (GeV)
  declareProperty("MinAngCutG", m_Thgmin   = 5); // minimum angle for cuts =Y (deg.)
  declareProperty("MaxAngCutG", m_Thgmax   = 21); //maximum angle for CUTG=Y (deg.)
  declareProperty("HBOOK",    HN= 1); //INTUPLE:  if events have to be stored (1/0)
  declareProperty("PHCUT",   m_PHCUT   = 0); //PHCUT:    to avoid double counting when ICH = 3  (1/0), for other channels, it set as 0 by default
  declareProperty("CUTG",    m_CUTG   = 0); //CUTG:	 explicit cuts on the generated photons (1/0)
  declareProperty("CutNgam",    m_CutNgam   = 0); //mininum number of ISR photon cut on Radiative Bhabha events   
  declareProperty("CutEgam",    m_CutEgam   = 0); //in GeV, mininum energy of ISR photon cut on Radiative Bhabha events 
}

StatusCode Babayaga::initialize(){

  MsgStream log(messageService(), name());
  
  log << MSG::INFO << "Babayaga initialize" << endreq;

  //set Bes unified random engine
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)
  {
    log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
    return RndmStatus;
  }
  CLHEP::HepRandomEngine* engine  = p_BesRndmGenSvc->GetEngine("Babayaga");
  std::cout<<"==============================="<<engine<<endl;
  BabayagaRandom::setRandomEngine(engine);
  //  *****************

  if(HN==1) {DECLARESTR.tuple='Y';} else DECLARESTR.tuple='N';
  if(m_PHCUT==1){ DECLARESTR.phcut='Y';} else  DECLARESTR.cutg='N';
  if(m_CUTG ==1){ DECLARESTR.cutg='Y';} else  DECLARESTR.cutg='N';
  CHANNEL.ich = m_Ich;
  BEAMENERGY.ebeam=m_Ebeam;
  EXPCUTS.thmin=m_Thmin;
  EXPCUTS.thmax=m_Thmax;
  EXPCUTS.emin =m_Emin;
  EXPCUTS.zmax=m_Zmax;
  SWITCHARUN.iarun=m_Iarun;
  RESONANCES.ires =m_Ires;
  SWITCH.on   =m_on;
  EXPCUTS.egmin=m_Egmin;
  EXPCUTS.thgmin=m_Thgmin;
  EXPCUTS.thgmax=m_Thgmax;

  //  std::cout<<"m_Ires= "<<m_Ires<<endl;

  getMaxEvent();
  std::cout<<"m_evtMax = "<<m_evtMax<<std::endl;
  DECLAREINT.seed=m_Int;
  BABAYAGA(m_evtMax);

  return StatusCode::SUCCESS;
}

static int evtgen=1;
StatusCode Babayaga::execute() 
{ 
  MsgStream log(messageService(), name());
// log << MSG::INFO << "BABAYAGA executing" << endreq;
//  std::cout<<"BABAYAGA begin executing"<<std::endl;  

  int npart = 0;
  int pid1,pid2,pst1,pst2;
  if(m_Ich==1) {pid1=11;   pid2=-11;  pst1=1;pst2=1;}
  if(m_Ich==2) {pid1=13;   pid2=-13;  pst1=1;pst2=1;}
  if(m_Ich==3) {pid1=22;   pid2= 22;  pst1=1;pst2=1;}
  if(m_Ich==4) {pid1=-211; pid2= 211; pst1=1;pst2=1;}

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
 
  // debuging pingrg
  //  std::cout<<"evtgen= "<<evtgen<<std::endl;
  //std::cout<<"b:e-: "<<MOMSET.p1[0][evtgen]<<"; "<< MOMSET.p1[1][evtgen]<<"; "<<MOMSET.p1[2][evtgen]<<"; "<< MOMSET.p1[3][evtgen]<<std::endl;
  //std::cout<<"b:e+: "<<MOMSET.q1[0][evtgen]<<"; "<< MOMSET.q1[1][evtgen]<<"; "<<MOMSET.q1[2][evtgen]<<"; "<< MOMSET.q1[3][evtgen]<<std::endl;
  //std::cout<<"e-: "<<MOMSET.p2[0][evtgen]<<"; "<< MOMSET.p2[1][evtgen]<<"; "<<MOMSET.p2[2][evtgen]<<"; "<< MOMSET.p2[3][evtgen]<<std::endl;
  //std::cout<<"e+: "<<MOMSET.q2[0][evtgen]<<"; "<< MOMSET.q2[1][evtgen]<<"; "<<MOMSET.q2[2][evtgen]<<"; "<< MOMSET.q2[3][evtgen]<<std::endl;

  // scattered lepton -
  p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.q2[1][evtgen],  MOMSET.q2[2][evtgen],
                                         MOMSET.q2[3][evtgen],  MOMSET.q2[0][evtgen]),
		       pid2, pst2); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_out(p);

  
  int iphot=0;
  // std::cout<<"evtgen, ncqph[events#] "<<evtgen<<"; "<<ISRPHOTONS.ncqph[evtgen-1]<<std::endl;
  for (iphot=0; iphot<ISRPHOTONS.ncqph[evtgen-1]; iphot++)
  {
    // debuging, pingrg
    //    std::cout<<"evtgen, iphot= "<<evtgen<<"; "<<iphot<<std::endl;
    //std::cout<<MOMSET.phot[0][iphot][evtgen]<<", "<<MOMSET.phot[1][iphot][evtgen]<<", "<<MOMSET.phot[2][iphot][evtgen]<<", "<<MOMSET.phot[3][iphot][evtgen]<<std::endl;
     
    p = new GenParticle( CLHEP::HepLorentzVector( MOMSET.phot[1][iphot][evtgen],  MOMSET.phot[2][iphot][evtgen], 
  					   MOMSET.phot[3][iphot][evtgen],  MOMSET.phot[0][iphot][evtgen]),
		                           22, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
    
  }


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

StatusCode Babayaga::finalize() 
{
  MsgStream log(messageService(), name());
   char delcmd[300];
   strcpy(delcmd,"cat ");
   strcat(delcmd,"CrossSection.txt");
   system(delcmd);

  std::cout<< "BABAYAGA finalized" << endl;
  return StatusCode::SUCCESS;
}

StatusCode Babayaga::getMaxEvent() {
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

