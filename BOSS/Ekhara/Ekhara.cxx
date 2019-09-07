#include "HepMC/HEPEVT_Wrapper.h"
#include "HepMC/IO_HEPEVT.h"
//#include "HepMC/IO_Ascii.h"
//#include "CLHEP/HepMC/GenEvent.h"
#include "HepMC/GenEvent.h"


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "Ekhara/Ekhara.h"
#include "Ekhara/EkharaRandom.h"
#include "GeneratorObject/McGenEvent.h"
#include "BesKernel/IBesRndmGenSvc.h"
#include "Ekhara/cfortran.h"
#include <iostream>
#include <TLorentzVector.h>
#include <TVector3.h>

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h"

using std::cout;
using std::endl;
using namespace HepMC;
using namespace CLHEP;
using CLHEP::HepVector;
using CLHEP::HepLorentzVector;
using CLHEP::Hep3Vector;
using CLHEP::HepMatrix;
using CLHEP::HepSymMatrix;

// decay channel
typedef struct
{
	int channel_id;
} CHANNELSEL_DEF;
#define CHANNELSEL COMMON_BLOCK(CHANNELSEL_DEF, channelsel)

COMMON_BLOCK_DEF(CHANNELSEL_DEF, CHANNELSEL);

typedef struct
{
	int sw_2pi,sw_1pi;
} SWDIAG_DEF;
#define SWDIAG COMMON_BLOCK(SWDIAG_DEF, swdiag)

COMMON_BLOCK_DEF(SWDIAG_DEF, SWDIAG);

typedef struct
{
	int piggFFsw;
} PIONFFSW_DEF;
#define PIONFFSW COMMON_BLOCK(PIONFFSW_DEF, pionffsw)

COMMON_BLOCK_DEF(PIONFFSW_DEF, PIONFFSW);

PROTOCCALLSFSUB0(EKHARA_SET_ONE_EVENT_MODE,ekhara_set_one_event_mode)
#define EKHARA_SET_ONE_EVENT_MODE() CCALLSFSUB0(EKHARA_SET_ONE_EVENT_MODE,ekhara_set_one_event_mode)

PROTOCCALLSFSUB1(EKHARA,ekhara,INT)
#define EKHARA(i) CCALLSFSUB1(EKHARA,ekhara,INT,i)

PROTOCCALLSFSUB1(BOSS_INIT_READ,boss_init_read,DOUBLEV)
#define BOSS_INIT_READ(xpar) CCALLSFSUB1(BOSS_INIT_READ,boss_init_read,DOUBLEV,xpar)

PROTOCCALLSFSUB0(DIAGNOSE,diagnose)
#define DIAGNOSE() CCALLSFSUB0(DIAGNOSE,diagnose)

PROTOCCALLSFSUB0(EKHARA_SET_SILENT,ekhara_set_silent)
#define EKHARA_SET_SILENT() CCALLSFSUB0( EKHARA_SET_SILENT,ekhara_set_silent)

PROTOCCALLSFSUB7(GET_MOMENTUM,get_momentum,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV)
#define GET_MOMENTUM(p1,p2,q1,q2,pi1,pi2,qpion) CCALLSFSUB7(GET_MOMENTUM,get_momentum,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,p1,p2,q1,q2,pi1,pi2,qpion)

Ekhara::Ekhara(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{
	declareProperty( "Ecm",                 m_E = 1.02 );               // CMS-energy (GeV) 
	declareProperty( "InitialSeed",         m_initSeed = 100);
	declareProperty( "InitialEvents",       m_ngUpperBound = 50000 );  // # of events to determine the maximum           
	declareProperty( "Channel",             m_channel_id = 2 );         // pi+pi-(1),pi0(2),, eta(3), eta' (4)
	declareProperty( "Thetaminpositron",    cut_th1min = 0.00 );   //minimum angle e+
	declareProperty( "Thetamaxpositron",    cut_th1max = 180.00 ); //maximum angle e+
	declareProperty( "Energyminpositron",   cut_E1min = 0.00 );   //minimum Energy e+
	declareProperty( "Energymaxpositron",   cut_E1max = 110.00 ); //maximum Energy e+
	declareProperty( "Thetaminelectron",    cut_th2min = 0.00 );   //minimum angle e-
	declareProperty( "Thetamaxelectron",    cut_th2max = 180.00 ); //maximum angle e-
	declareProperty( "Energyminelectron",   cut_E2min = 0.00 );   //minimum Energy e-
	declareProperty( "Energymaxelectron",   cut_E2max = 110.00 ); //maximum Energy e-
	declareProperty( "Thetaminpion",        cut_thpionmin = 0.00 );   //minimum angle pi0
	declareProperty( "Thetamaxpion",        cut_thpionmax = 180.00 ); //maximum angle pi0
	declareProperty( "Eminpion",            cut_Epionmin = 0.00 );   //minimum Energy pi0
	declareProperty( "Emaxpion",            cut_Epionmax = 100.00 ); //maximum Energy pi0
	declareProperty( "sw_silent",           m_sw_silent = 1 ); //suppress output to stdout
	declareProperty( "sw_2pi",                 m_sw_2pi = 2 );           // s (1); t (2); s+t (3); s = signal
	declareProperty( "sw_1pi",                 m_sw_1pi = 2 );           // s (1); t (2); s+t (3); s = signal
	declareProperty( "sw",                 m_sw = 2 ); 
	declareProperty( "Formfactor",             m_piggFFsw = 5 );
	//
}

StatusCode Ekhara::initialize() {
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "Ekhara in initialize()" << endreq;


	hMCPosiMom   = histoSvc()->book("MonteCarlo",1,"PosiMom",250,0.,5.);
	hMCPosiThe   = histoSvc()->book("MonteCarlo",2,"PosiThe",45,0.,180.);
	hMCPosiPhi   = histoSvc()->book("MonteCarlo",3,"PosiPhi",90,-180.,180.);
	hMCElecMom   = histoSvc()->book("MonteCarlo",4,"ElecMom",250,0.,5.);
	hMCElecThe   = histoSvc()->book("MonteCarlo",5,"ElecThe",45,0.,180.);
	hMCElecPhi   = histoSvc()->book("MonteCarlo",6,"ElecPhi",90,-180.,180.);
	hMCEtaPMom   = histoSvc()->book("MonteCarlo",7,"EtaPMom",250,0.,5.);
	hMCEtaPThe   = histoSvc()->book("MonteCarlo",8,"EtaPThe",45,0.,180.);
	hMCEtaPPhi   = histoSvc()->book("MonteCarlo",9,"EtaPPhi",90,-180.,180.);


	//set Bes unified random engine
	static const bool CREATEIFNOTTHERE(true);
	StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);
	if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)
	{
		log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
		return RndmStatus;
	}
	HepRandomEngine* engine  = p_BesRndmGenSvc->GetEngine("Ekhara");
	EkharaRandom::setRandomEngine(engine);

	CHANNELSEL.channel_id = m_channel_id;
	SWDIAG.sw_2pi=m_sw_2pi;
	SWDIAG.sw_1pi=m_sw_1pi;
	PIONFFSW.piggFFsw=m_piggFFsw;
	/*
	if(m_channel_id==1){
	  m_sw=m_sw_2pi;}
	else if(m_channel_id==2||m_channel_id==3||m_channel_id==4){
	  m_sw=m_sw_1pi;}
	*/
	
// --- print run data ------------------------------
	cout << "-------------------------------------------------------------" << endl;
	if (CHANNELSEL.channel_id == 2) 
	  cout << "     EKHARA 2.1 : e^+ e^- -> e^+ e^- pi^0" << endl;
	else if (CHANNELSEL.channel_id == 1) 
	  cout << "     EKHARA 2.1: e^+ e^- -> e^+ e^- pi^+ pi^-" << endl;
	else if (CHANNELSEL.channel_id == 3) 
	  cout << "     EKHARA 2.1: e^+ e^- -> e^+ e^- eta" << endl;
	else if (CHANNELSEL.channel_id == 4) 
	  cout << "     EKHARA 2.1: e^+ e^- -> e^+ e^- eta'" << endl;
	
	
	if(m_channel_id ==2||m_channel_id ==3||m_channel_id ==4)
	  printf(" %s %f,%f\n","angular cuts on e+                 = ",cut_th1min,cut_th1max);
	  printf(" %s %f,%f\n","angular cuts on e-                 = ",cut_th2min,cut_th2max);
	  printf(" %s %f,%f\n","Energy  cuts on e+                 = ",cut_E1min,cut_E1max);
	  printf(" %s %f,%f\n","Energy  cuts on e-                 = ",cut_E2min,cut_E2max);
	//---------------------------------
	
	double xpar[100];
	xpar[0]=m_E;
	
	xpar[1]=m_ngUpperBound;
	xpar[2]=m_channel_id;
	xpar[3]=cut_th1min;
	xpar[4]=cut_th1max;
	xpar[5]=cut_E1min;
	xpar[6]=cut_E1max;
	xpar[7]=cut_th2min;
	xpar[8]=cut_th2max;
	xpar[9]=cut_E2min;
	xpar[10]=cut_E2max;
	xpar[11]=cut_thpionmin;
	xpar[12]=cut_thpionmax;
	xpar[13]=cut_Epionmin;
	xpar[14]=cut_Epionmax;

	log << MSG::DEBUG << "Options filled in array" << endreq;
	BOSS_INIT_READ(xpar);
	log << MSG::DEBUG << "Options read by FORTRAN routine" << endreq;
	DIAGNOSE();
	log << MSG::DEBUG << "FORTRAN diagnose passed" << endreq;
	EKHARA(-1);
	log << MSG::DEBUG << "Ekhara Fortran routines initialized" << endreq;
	//EKHARA_SET_ONE_EVENT_MODE();
	//log << MSG::DEBUG << "Ekhara set to single event mode" << endreq;
	//	EKHARA_SET_SILENT();
	//log << MSG::DEBUG << "Ekhara set to silent operation" << endreq;

	return StatusCode::SUCCESS;
}

StatusCode Ekhara::execute() {
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "Ekhara in execute()" << endreq;

	EKHARA(0);

	//std::ofstream file1;
	//file1.open("file1.txt");


	double p1[4],p2[4],q1[4],q2[4];
	double pi1[4],pi2[4],qpion[4];

	for (int i=0;i<4;i++) {
		p1[i]=0.0;
		p2[i]=0.0;
		q1[i]=0.0;
		q2[i]=0.0;
		pi1[i]=0.0;
		pi2[i]=0.0;
		qpion[i]=0.0;
	}

	GET_MOMENTUM(p1,p2,q1,q2,pi1,pi2,qpion);

	double PosiMom = sqrt(q1[1]*q1[1]+q1[2]*q1[2]+q1[3]*q1[3]);
	double PosiThe = acos(q1[3]/PosiMom);
	double PosiPhi = atan2(q1[2],q1[1]);

	double ElecMom = sqrt(q2[1]*q2[1]+q2[2]*q2[2]+q2[3]*q2[3]);
	double ElecThe = acos(q2[3]/ElecMom);
	double ElecPhi = atan2(q2[2],q2[1]);

	double EtaPMom = sqrt(qpion[1]*qpion[1]+qpion[2]*qpion[2]+qpion[3]*qpion[3]);
	double EtaPThe = acos(qpion[3]/EtaPMom);
	double EtaPPhi = atan2(qpion[2],qpion[1]);

	hMCPosiMom->fill(PosiMom);
	hMCElecMom->fill(ElecMom);
	hMCEtaPMom->fill(EtaPMom);

	hMCPosiPhi->fill(PosiPhi*TMath::RadToDeg());
	hMCElecPhi->fill(ElecPhi*TMath::RadToDeg());
	hMCEtaPPhi->fill(EtaPPhi*TMath::RadToDeg());

	hMCPosiThe->fill(PosiThe*TMath::RadToDeg());
	hMCElecThe->fill(ElecThe*TMath::RadToDeg());
	hMCEtaPThe->fill(EtaPThe*TMath::RadToDeg());



	// Fill event information
	GenEvent* evt = new GenEvent(1,1);

	GenVertex* prod_vtx = new GenVertex();
	evt->add_vertex( prod_vtx );

	// incoming beam e+
	GenParticle* p = new GenParticle( HepLorentzVector(p1[1],p1[2],p1[3],p1[0]), -11, 3); 
	p->suggest_barcode(1 );
	prod_vtx->add_particle_in(p);

	// incoming beam e-
	p = new GenParticle(HepLorentzVector(p2[1],p2[2],p2[3],p2[0]), 11, 3); 
	p->suggest_barcode( 2 );
	prod_vtx->add_particle_in(p);


	// outcoming beam e+
	p = new GenParticle(HepLorentzVector(q1[1],q1[2],q1[3],q1[0]), -11, 1); 
	p->suggest_barcode(3 );
	prod_vtx->add_particle_out(p);

	// outcoming beam e-
	p = new GenParticle( HepLorentzVector(q2[1],q2[2],q2[3],q2[0] ), 11, 1); 
	p->suggest_barcode( 4 );
	prod_vtx->add_particle_out(p);

	if (m_channel_id == 2){ //e+e- pi0
		p = new GenParticle( HepLorentzVector( qpion[1],qpion[2],qpion[3],qpion[0]), 111, 1); 
		p->suggest_barcode( 5 );
		prod_vtx->add_particle_out(p);
	}
	else if (m_channel_id == 3){ //e+e- eta
		p = new GenParticle( HepLorentzVector( qpion[1],qpion[2],qpion[3],qpion[0]), 221, 1); 
		p->suggest_barcode( 5 );
		prod_vtx->add_particle_out(p);
	}
	else if (m_channel_id == 4){ //e+e- eta'
		p = new GenParticle( HepLorentzVector( qpion[1],qpion[2],qpion[3],qpion[0]), 331, 1); 
		p->suggest_barcode( 5 );
		prod_vtx->add_particle_out(p);
	}
	else if (m_channel_id == 1) { // e+e-pi+ pi-
		// pi+
		p = new GenParticle( HepLorentzVector( pi1[1],pi1[2],pi1[3],pi1[0]),211, 1); 
		p->suggest_barcode( 5 );
		prod_vtx->add_particle_out(p);
		// pi-
		p = new GenParticle( HepLorentzVector(pi2[1],pi2[2],pi2[3],pi2[0]), -211, 1); 
		p->suggest_barcode( 6 );
		prod_vtx->add_particle_out(p);
	} 

	if( log.level() <= MSG::INFO )
	{
	  
	  evt->print();
	  
	  //  file1<<qpion[1]<<endl;
	  
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
			log << MSG::INFO << "McGenEventCol created " << endreq;
		}
		//file1.close();
	}

	log<<MSG::INFO<< "before execute() return"<<endreq;
	return StatusCode::SUCCESS;

}

StatusCode Ekhara::finalize() {
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "Ekhara in finalize()" << endreq;
	log << MSG::INFO << "Ekhara is terminated now successfully" << endreq;
	DIAGNOSE();
	EKHARA(1);
	return StatusCode::SUCCESS;
}


