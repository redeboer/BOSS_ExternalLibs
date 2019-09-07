//*****************************************************************************
//
// Phokhara.cxx
//
// Algorithm runs event generator Phokhara (hep-ph/0710.4227v1)
// and stores output to transient store
//
// Nov. 2007 A. Zhemchugov, initial version written for BES3
//*****************************************************************************

// Header for this module:-
#include "Phokhara/Phokhara.h"
#include "Phokhara/PhokharaDef.h"

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
#include "CLHEP/Random/RandomEngine.h"

#include "cfortran/cfortran.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace CLHEP;

Phokhara::Phokhara(const string& name, ISvcLocator* pSvcLocator):Algorithm( name, pSvcLocator )
{
  m_initSeed = (0);

  m_tagged = 0;

  declareProperty( "InitialSeed", m_initSeed = 0);

  declareProperty( "InitialEvents",    m_nm = 50000 );       // # of events to determine the maximum   
  declareProperty( "NLO",              m_nlo = 1 );          // Born(0), NLO(1)
  declareProperty( "SoftPhotonCutoff", m_w = 0.0001 );       // soft photon cutoff                
  declareProperty( "Channel",          m_pion = 0 );         // mu+mu-(0),pi+pi-(1),2pi0pi+pi-(2),
                                                             // 2pi+2pi-(3),ppbar(4),nnbar(5),
                                                             // K+K-(6),K0K0bar(7),pi+pi-pi0(8), 
                                                             // Lamb Lambbar->pi-pi+ppbar(9) 
  declareProperty( "FSR",                m_fsr = 1 );        // ISR only(0), ISR+FSR(1), ISR+INT+FSR(2)
  declareProperty( "FSRNLO",             m_fsrnlo = 1 );     // yes(1), no(0)
  declareProperty( "NarrowRes",          m_NarrowRes = 1 );  // none(0) jpsi(1) psip(2)
  declareProperty( "KaonFormfactor",     m_FF_Kaon = 1 );    // constrained (0), unconstrained (1), Kuhn-Khodjamirian-Bruch (2)
  declareProperty( "VacuumPolarization", m_ivac = 0 );       // yes(1), no(0)
  //declareProperty( "TaggedPhotons",      m_tagged = 0 );     // tagged photons(0), untagged photons(1)
  declareProperty( "PionFormfactor",     m_FF_Pion = 0 );    // KS Pionformfactor(0), GS Pionformfactor old(1) and new(2)
  declareProperty( "F0model",            m_f0_model = 0 );   // f0+f0(600): KK model(0), no structure(1), 
                                                             // no f0+f0(600)(2), f0 KLOE(3)
  declareProperty( "Ecm",             m_E = 3.097 );         // CMS-energy (GeV)                       
  declareProperty( "CutTotalInvMass", m_q2min = 0.0 );       // minimal  hadrons(muons)-gamma-inv mass squared (GeV^2)
  declareProperty( "CutHadInvMass",   m_q2_min_c = 0.0447 ); // minimal inv. mass squared of the hadrons(muons)(GeV^2)
  declareProperty( "MaxHadInvMass",   m_q2_max_c = 1.06 );   // maximal inv. mass squared of the hadrons(muons)(GeV^2)
  declareProperty( "MinPhotonEnergy", m_gmin = 0.05 );       // minimal photon energy/missing energy   (GeV)          
  declareProperty( "MinPhotonAngle",  m_phot1cut = 0.0 );    // minimal photon angle/missing momentum angle (grad)
  declareProperty( "MaxPhotonAngle",  m_phot2cut = 180.0 );  // maximal photon angle/missing momentum angle (grad)
  declareProperty( "MinHadronsAngle", m_pi1cut = 0.0 );      // minimal hadrons(muons) angle (grad)
  declareProperty( "MaxHadronsAngle", m_pi2cut = 180.0 );    // maximal hadrons(muons) angle (grad)

  ievent = 0;
}

StatusCode Phokhara::initialize(){

  MsgStream log(messageService(), name());
  
  log << MSG::INFO  << "Phokhara initialize" << endreq;

  int i,s_seed[105];
  long int k,j;

  if( m_initSeed == 0) // if seed is not set in the jobptions file, set it using BesRndmGenSvc
    {
      IBesRndmGenSvc* p_BesRndmGenSvc;
      static const bool CREATEIFNOTTHERE(true);
      StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);
      if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)
	{
	  log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
	  return RndmStatus;
	}
      
      // Save the random number seeds in the event
      HepRandomEngine* engine  = p_BesRndmGenSvc->GetEngine("PHOKHARA");
      const long*  s       = engine->getSeeds();
      m_initSeed = s[0];
    }

  log << MSG::INFO << "Set initial seed " << m_initSeed << endreq;

  // --- initialize the seed ------
  /*   ifstream seeds("seed.dat");
  if( seeds.is_open() )
    {
      int ii=0;
      while( ! seeds.eof() )
	seeds >> s_seed[ii++];
    }
  else
    cerr << "Cannot open file seed.dat for reading" << endl;
  */

  RLXDINIT(1, m_initSeed);

  //RLXDRESETF(s_seed);
  //rlxd_reset(s_seed);
  
  // --- input parameters ----------------------------
  MAXIMA.iprint = 0;

  FLAGS.nlo = m_nlo;    
  FLAGS.pion = m_pion;   
  FLAGS.fsr = m_fsr;    
  FLAGS.fsrnlo = m_fsrnlo; 
  FLAGS.ivac = m_ivac;    
 // FLAGS.tagged = m_tagged;  
  FLAGS.FF_pion  = m_FF_Pion; 
  FLAGS.f0_model = m_f0_model;
  FLAGS.FF_kaon  = m_FF_Kaon;
  FLAGS.narr_res = m_NarrowRes; 
 
  CTES.Sp = m_E*m_E;        ;
  
  CUTS.w = m_w;
  CUTS.q2min = m_q2min;  
  CUTS.q2_min_c = m_q2_min_c; 
  CUTS.q2_max_c = m_q2_max_c;
  CUTS.gmin = m_gmin;    
  CUTS.phot1cut = m_phot1cut;
  CUTS.phot2cut = m_phot2cut;
  CUTS.pi1cut = m_pi1cut;  
  CUTS.pi2cut = m_pi2cut;  

  INPUT();
  // --- print run data ------------------------------
  cout << "-------------------------------------------------------------" << endl;
  if (FLAGS.pion == 0) 
    cout << "     PHOKHARA 7.0 : e^+ e^- -> mu^+ mu^- gamma" << endl;
  else if (FLAGS.pion == 1) 
    cout << "     PHOKHARA 7.0: e^+ e^- -> pi^+ pi^- gamma" << endl;    
  else if (FLAGS.pion == 2) 
    cout << "   PHOKHARA 7.0: e^+ e^- -> pi^+ pi^- 2pi^0 gamma" << endl;        
  else if (FLAGS.pion == 3) 
    cout << "   PHOKHARA 7.0: e^+ e^- -> 2pi^+ 2pi^- gamma" << endl; 
  else if (FLAGS.pion == 4) 
    cout << "   PHOKHARA 7.0: e^+ e^- -> p pbar gamma" << endl; 
  else if (FLAGS.pion == 5) 
    cout << "   PHOKHARA 7.0: e^+ e^- -> n nbar gamma" << endl;                
  else if (FLAGS.pion == 6) 
    cout << "   PHOKHARA 7.0: e^+ e^- -> K^+ K^- gamma" << endl;                
  else if (FLAGS.pion == 7) 
    cout << "   PHOKHARA 7.0: e^+ e^- -> K_0 K_0bar gamma" << endl;                
  else if (FLAGS.pion == 8) 
    cout << "   PHOKHARA 7.0: e^+ e^- -> pi^+ pi^- pi^0 gamma" << endl;                
  else if (FLAGS.pion == 9) {
    cout <<  "PHOKHARA 7.0 : e^+ e^- ->" << endl;
    cout << "  Lambda (-> pi^- p) Lambda bar (-> pi^+ pbar) gamma" << endl;
  } else 
    cout <<  "     PHOKHARA 7.0: not yet implemented" << endl;
  
  // --------------------------------
  cout <<  "--------------------------------------------------------------" << endl;
  printf(" %s %f %s\n","cms total energy                       = ",sqrt(CTES.Sp)," GeV  "); 
//  if (FLAGS.tagged == 0) { 
    if((CUTS.gmin/2.0/CTES.ebeam) < 0.0098){
      cerr << " minimal missing energy set to small" << endl;
      return 0;
    }
    printf(" %s %f %s\n","minimal tagged photon energy           = ",CUTS.gmin," GeV  ");	
    printf(" %s %f,%f\n","angular cuts on tagged photon          = ",CUTS.phot1cut,CUTS.phot2cut);
/*  }
  else {
    if((CUTS.gmin/2.0/CTES.ebeam) < 0.0098){
      cerr << " minimal missing energy set to small" << endl;
      return 0;
    }
    printf(" %s %f %s\n","minimal missing energy                 = ",CUTS.gmin," GeV  ");
    printf(" %s %f,%f\n","angular cuts on missing momentum       = ",CUTS.phot1cut, CUTS.phot2cut);
  }
*/
 
  // --------------------------------
  if (FLAGS.pion == 0) 
    printf(" %s %f,%f\n","angular cuts on muons                  = ",CUTS.pi1cut,CUTS.pi2cut);
  else if (FLAGS.pion == 4)  
    printf(" %s %f,%f\n","angular cuts on protons                = ",CUTS.pi1cut,CUTS.pi2cut);
  else if (FLAGS.pion == 5) 
    printf(" %s %f,%f\n","angular cuts on neutrons               = ", CUTS.pi1cut,CUTS.pi2cut);
  else if ((FLAGS.pion == 6)||(FLAGS.pion == 7)) 
    printf(" %s %f,%f\n","angular cuts on kaons                  = ", CUTS.pi1cut,CUTS.pi2cut);
  else if (FLAGS.pion == 9) 
    printf(" %s %f,%f\n","angular cuts on pions and protons      = ", CUTS.pi1cut,CUTS.pi2cut);
  else
    printf(" %s %f,%f\n","angular cuts on pions                  = ", CUTS.pi1cut,CUTS.pi2cut);
    
//  if (FLAGS.tagged == 0) {
    if (FLAGS.pion == 0) 
      printf(" %s %f %s\n","min. muons-tagged photon inv.mass^2    = ", CUTS.q2min," GeV^2");
    else if (FLAGS.pion == 4) 
      printf(" %s %f %s\n","min. protons-tagged photon inv.mass^2  = ", CUTS.q2min," GeV^2" );
    else if (FLAGS.pion == 5) 
      printf(" %s %f %s\n","min. neutrons-tagged photon inv.mass^2 = ", CUTS.q2min," GeV^2" );
    else if ((FLAGS.pion == 6)||(FLAGS.pion == 7)) 
      printf(" %s %f %s\n","min. kaons-tagged photon inv.mass^2    = ", CUTS.q2min," GeV^2" );
    else if (FLAGS.pion == 9) 
      printf(" %s %f %s\n"," min. lambdas-tagged photon inv.mass^2 = ", CUTS.q2min," GeV^2" );
    else
      printf(" %s %f %s\n","min. pions-tagged photon inv.mass^2    = ",	CUTS.q2min," GeV^2" );
//  }
// --- set cuts ------------------------------------
//      if (FLAGS.tagged == 0) { 
	cos1min = cos(CUTS.phot2cut*CTES.pi/180.0);     // photon1 angle cuts in the 
	cos1max = cos(CUTS.phot1cut*CTES.pi/180.0);     // LAB rest frame            
//      } else {
//	cos1min = -1.0;                         
//	cos1max =  1.0;
//	CUTS.gmin = CUTS.gmin/2.0;
//      }
      cos2min = -1.0;                        // photon2 angle limits      
      cos2max =  1.0;                        //                           
      cos3min = -1.0;                        // hadrons/muons angle limits    
      cos3max =  1.0;                        // in their rest frame            
      if (FLAGS.pion == 0)                   // virtual photon energy cut 
	qqmin = 4.0*CTES.mmu*CTES.mmu;
      else if (FLAGS.pion == 1) 
	qqmin = 4.0*CTES.mpi*CTES.mpi;
      else if (FLAGS.pion == 2)  
	qqmin = 4.0*(CTES.mpi+CTES.mpi0)*(CTES.mpi+CTES.mpi0);
      else if (FLAGS.pion == 3)  
	qqmin = 16.0*CTES.mpi*CTES.mpi;
      else if (FLAGS.pion == 4)  
	qqmin = 4.0*CTES.mp*CTES.mp;
      else if (FLAGS.pion == 5)  
	qqmin = 4.0*CTES.mnt*CTES.mnt;
      else if (FLAGS.pion == 6)  
	qqmin = 4.0*CTES.mKp*CTES.mKp;
      else if (FLAGS.pion == 7)  
	qqmin = 4.0*CTES.mKn*CTES.mKn;
      else if (FLAGS.pion == 8) 
	qqmin = (2.0*CTES.mpi+CTES.mpi0)*(2.0*CTES.mpi+CTES.mpi0);
      else if (FLAGS.pion == 9) 
	qqmin = 4.0*CTES.mlamb*CTES.mlamb;
      //      else
      //	continue;

      qqmax = CTES.Sp-2.0*sqrt(CTES.Sp)*CUTS.gmin;         // if only one photon 
      if (CUTS.q2_max_c < qqmax) 
	qqmax=CUTS.q2_max_c;      // external cuts      

      // -------------------
      if ( (CUTS.q2_min_c > qqmin) && (CUTS.q2_min_c < (CTES.Sp*(1.0-2.0*(CUTS.gmin/sqrt(CTES.Sp)+CUTS.w))) ) )
	qqmin = CUTS.q2_min_c;
      else {
	cerr << "------------------------------" << endl;
	cerr << " Q^2_min TOO SMALL" << endl;
	cerr << " Q^2_min CHANGED BY PHOKHARA = " << qqmin << " GeV^2" << endl;
	cerr << "------------------------------" << endl;
      }
      // -------------------
      if(qqmax <= qqmin){
	cerr << " Q^2_max to small " << endl;
	cerr << " Q^2_max = " << qqmax << endl;
	cerr << " Q^2_min = " << qqmin << endl;
	return 0;
      }

      // -------------------
      if (FLAGS.pion == 0) {
	printf(" %s %f %s\n", "minimal muon-pair invariant mass^2     = ", qqmin," GeV^2");
        printf(" %s %f %s\n", "maximal muon-pair invariant mass^2     = ", qqmax," GeV^2");
      } else if (FLAGS.pion == 1) {
	printf(" %s %f %s\n", "minimal pion-pair invariant mass^2     = ", qqmin," GeV^2");
	printf(" %s %f %s\n", "maximal pion-pair invariant mass^2     = ", qqmax," GeV^2");
      } else if (FLAGS.pion == 4) {
	printf(" %s %f %s\n", "minimal proton-pair invariant mass^2   = ", qqmin," GeV^2");
	printf(" %s %f %s\n", "maximal proton-pair invariant mass^2   = ", qqmax," GeV^2");
      } else if (FLAGS.pion == 5) {
	printf(" %s %f %s\n", "minimal neutron-pair invariant mass^2  = ", qqmin," GeV^2");
	printf(" %s %f %s\n", "maximal neutron-pair invariant mass^2  = ", qqmax," GeV^2");
      } else if ((FLAGS.pion == 6) || (FLAGS.pion == 7)) {
	printf(" %s %f %s\n", "minimal kaon-pair invariant mass^2     = ", qqmin," GeV^2");
	printf(" %s %f %s\n", "maximal kaon-pair invariant mass^2     = ", qqmax," GeV^2");
      } else if(FLAGS.pion == 8){
	printf(" %s %f %s\n", "minimal three-pion invariant mass^2    = ", qqmin," GeV^2");
	printf(" %s %f %s\n", "maximal three-pion invariant mass^2    = ", qqmax," GeV^2");
      } else if(FLAGS.pion == 9){
	printf(" %s %f %s\n", "minimal lambda-pair invariant mass^2  = ", qqmin," GeV^2");
	printf(" %s %f %s\n", "maximal lambda-pair invariant mass^2  = ", qqmax," GeV^2");
      } else {
	printf(" %s %f %s\n", "minimal four-pion invariant mass^2     = ", qqmin," GeV^2" );
	printf(" %s %f %s\n", "maximal four-pion invariant mass^2     = ", qqmax," GeV^2");
      }
      // -------------------
      if (FLAGS.nlo == 0) { 
	cout <<  "Born" << endl;
         if(FLAGS.fsrnlo != 0){
           cerr << "WRONG FSRNLO flag - only fsrnlo=0 allowed for Born" << endl;
           return 0;
	 }
      }
      // -------------------
      if((FLAGS.pion == 9) && (FLAGS.nlo != 0)) {
        cerr << "WRONG NLO flag - only Born allowed for Lambdas"<< endl;
	cerr << "If you feel that you need NLO"<< endl;
	cerr << "please contact the authors"<< endl;
	return 0;
      }
      // -------------------
      if (FLAGS.nlo == 1) printf(" %s %f\n", "NLO:    soft photon cutoff w           = ",CUTS.w);
      if ((FLAGS.pion <= 1) || (FLAGS.pion == 6)) 
	{
	  
	  if( ! ((FLAGS.fsr == 1) || (FLAGS.fsr == 2) || (FLAGS.fsrnlo == 0) 
	      || (FLAGS.fsr == 1) || (FLAGS.fsrnlo == 1)
		 || (FLAGS.fsr == 0) || (FLAGS.fsrnlo == 0))) {
	    cerr << "WRONG combination of FSR, FSRNLO flags" <<endl;
	    return 0;
	  }
	  
	  // ------------------
	  if (FLAGS.fsr == 0) 
	    cout << "ISR only" << endl;
	  else if (FLAGS.fsr == 1) 
	    cout << "ISR+FSR" << endl;
	  else if (FLAGS.fsr == 2) {
	    if (FLAGS.nlo == 0) 
	      cout << "ISR+INT+FSR" << endl;
	    else {
	      cerr << "WRONG FSR flag: interference is included only for nlo=0" << endl;
	      return 0;
	    }
	  }
	  else {
	    cerr << "WRONG FSR flag" << FLAGS.fsr << endl;
	    return 0;
	  }
      
	  if(FLAGS.fsrnlo == 1) 
	    cout << "IFSNLO included" << endl;
	}
      else
	{
	  if((FLAGS.fsr == 0) && (FLAGS.fsrnlo == 0))
	    cout << "ISR only" << endl;
	  else {
	    cerr << "FSR is implemented only for pi+pi-, mu+mu- and K+K- modes" << endl;
	    return 0;
	  }
	}

      // ------------------
      if(FLAGS.ivac == 0){
        cout << "Vacuum polarization is NOT included" << endl;}
        else if(FLAGS.ivac == 1){
        cout << "Vacuum polarization  by Fred Jegerlehner (http://www-com.physik.hu-berlin.de/fjeger/alphaQEDn.uu)"  << endl;}
        else if(FLAGS.ivac == 2){
	cout << "Vacuum polarization (VP_HLMNT_v1_3nonr) by Daisuke Nomura and Thomas Teubner" << endl;}
	else {
        cout << "WRONG vacuum polarization switch" << endl;
	return 0;
        }
      
// -----------------
      if(FLAGS.pion == 1){
        if(FLAGS.FF_pion == 0)
          cout << "Kuhn-Santamaria PionFormFactor" << endl;
        else if(FLAGS.FF_pion == 1)
          cout << "Gounaris-Sakurai PionFormFactor old" << endl;
	else if(FLAGS.FF_pion == 2)
	  cout << "Gounaris-Sakurai PionFormFactor new" << endl;
        else {
	  cout << "WRONG PionFormFactor switch" << endl;
	  return 0;
	}
        
         if(FLAGS.fsr != 0){
         if(FLAGS.f0_model == 0)
           cout << "f0+f0(600): K+K- model" << endl;
         else if(FLAGS.f0_model == 1)
           cout << "f0+f0(600): \"no structure\" model" << endl;
         else if(FLAGS.f0_model == 2)
           cout << "NO f0+f0(600)" << endl;
         else if(FLAGS.f0_model == 3)
           cout << "only f0, KLOE: Cesare Bini-private communication" << endl;
         else {
           cout << "WRONG f0+f0(600) switch" << endl;
           return 0;
         }
        }
      }

//-------
      if((FLAGS.pion == 6) || (FLAGS.pion==7)){
        if(FLAGS.FF_kaon == 0)
          cout << "constrained KaonFormFactor" << endl;
        else if(FLAGS.FF_kaon == 1) {
          cout << "unconstrained KaonFormFactor" << endl;}
        else if(FLAGS.FF_kaon == 2) {
          cout << "Kuhn-Khodjamirian-Bruch KaonFormFactor" << endl;}
        else{
          cout << "WRONG KaonFormFactor switch" << endl;
          return 0; 
        }
      }
// --------------------
      if((FLAGS.pion == 0) || (FLAGS.pion==1) || (FLAGS.pion == 6) || (FLAGS.pion == 7)){
        if(FLAGS.narr_res == 1){
          cout << "THE NARROW RESONANCE J/PSI INCLUDED" << endl;}
        else if(FLAGS.narr_res == 2){
          cout << "THE NARROW RESONANCE PSI(2S) INCLUDED" << endl;}
        else if(FLAGS.narr_res != 0){
          cout << "wrong flag narr_res: only 0, 1 or 2 allowed" << endl;
          return 0;
        }
       }
// ------

  cout << "--------------------------------------------------------------" << endl;
//
 // =================================================
// --- finding the maximum -------------------------
      for( i = 0; i<2; i++)
	{
	  MAXIMA.Mmax[i] = 1.0;
	  MAXIMA.gross[i] = 0.0;
	  MAXIMA.klein[i] = 0.0;      
	}
      
      if (FLAGS.nlo == 0) 
	MAXIMA.Mmax[1]=0.0;   // only 1 photon events generated
      
      MAXIMA.tr[0] = 0.0;      
      MAXIMA.tr[1] = 0.0;
      MAXIMA.count[0] = 0.0;
      MAXIMA.count[1] = 0.0;
      
      // =================================================
      // --- beginning the MC loop event generation ------
      for(j = 1; j <= m_nm; j++)
	{
	  RANLXDF(Ar_r,1);
	  Ar[1] = Ar_r[0];
	  if (Ar[1] <= (MAXIMA.Mmax[0]/(MAXIMA.Mmax[0]+MAXIMA.Mmax[1]))) { 
	    MAXIMA.count[0] = MAXIMA.count[0]+1.0;
	    GEN_1PH(1,qqmin,qqmax,cos1min,cos1max,cos3min,cos3max);
	  } 
	  else {
	    MAXIMA.count[1] = MAXIMA.count[1]+1.0;
	    GEN_2PH(1,qqmin,cos1min,cos1max,cos2min,cos2max,cos3min,cos3max);
	  }
	}
      // --- end of the MC loop --------------------------
      // =================================================
      // --- for the second run ---
      MAXIMA.Mmax[0] = MAXIMA.gross[0]+.05*sqrt(MAXIMA.gross[0]*MAXIMA.gross[0]);
      MAXIMA.Mmax[1] = MAXIMA.gross[1]+(.03+.02*CTES.Sp)*sqrt(MAXIMA.gross[1]*MAXIMA.gross[1]); 
      if((FLAGS.pion == 1) && (FLAGS.fsrnlo == 1)) 
	MAXIMA.Mmax[1]=MAXIMA.Mmax[1]*1.5;
      if((FLAGS.pion == 0) && (FLAGS.fsrnlo == 1)) 
	MAXIMA.Mmax[1]=MAXIMA.Mmax[1]*1.5;
      
      if((FLAGS.pion == 0) && (FLAGS.fsr == 1) && (FLAGS.fsrnlo == 0)) 
	MAXIMA.Mmax[1]=MAXIMA.Mmax[1]*1.2;
      
      if((FLAGS.pion == 2) || (FLAGS.pion == 3)){
	MAXIMA.Mmax[0]=MAXIMA.Mmax[0]*1.1;
	MAXIMA.Mmax[1]=MAXIMA.Mmax[1]*1.1;
      }
      
      if(FLAGS.pion == 8){
	MAXIMA.Mmax[0]=MAXIMA.Mmax[0]*1.08;
	MAXIMA.Mmax[1]=MAXIMA.Mmax[1]*1.1;
      }
// --- end of the second run -----------------------
      
      MAXIMA.tr[0] = 0.0;      
      MAXIMA.tr[1] = 0.0;
      MAXIMA.count[0] = 0.0;
      MAXIMA.count[1] = 0.0;
      
  return StatusCode::SUCCESS;
}

StatusCode Phokhara::execute() 
{ 
  MsgStream log(messageService(), name());

  int ntrials = 0;
  int tr_old[2];
  tr_old[0] = (int)MAXIMA.tr[0];
  tr_old[1] = (int)MAXIMA.tr[1];

  while( ntrials < 10000)
    {
      ievent++;
      RANLXDF(Ar_r,1);
      Ar[1] = Ar_r[0];
      
      if (Ar[1] <= (MAXIMA.Mmax[0]/(MAXIMA.Mmax[0]+MAXIMA.Mmax[1]))) { 
	MAXIMA.count[0] = MAXIMA.count[0]+1.0;
	GEN_1PH(2,qqmin,qqmax,cos1min,cos1max,cos3min,cos3max);
      } 
      else {
	MAXIMA.count[1] = MAXIMA.count[1]+1.0;
	GEN_2PH(2,qqmin,cos1min,cos1max,cos2min,cos2max,cos3min,cos3max);
      }
      
      if( ((int)MAXIMA.tr[0]+(int)MAXIMA.tr[1]) > (tr_old[0]+tr_old[1]) ) // event accepted after cuts
	{
	  storeParticles();
	  return StatusCode::SUCCESS; 
	}
      ntrials ++;
    }
 
  log << MSG::FATAL << "Could not satisfy cuts after " << ntrials << "trials. Terminate." << endreq;
  
  return StatusCode::FAILURE; 
}


StatusCode Phokhara::storeParticles()
{
  MsgStream log(messageService(), name());
  int npart = 0;
  
  // Fill event information
  GenEvent* evt = new GenEvent(1,1);

  GenVertex* prod_vtx = new GenVertex();
//  prod_vtx->add_particle_out( p );
  evt->add_vertex( prod_vtx );

  // incoming beam e+
  GenParticle* p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][0], CTES.momenta[2][0], CTES.momenta[3][0], CTES.momenta[0][0] ), -11, 3); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_in(p);

  // incoming beam e-
  p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][1], CTES.momenta[2][1], CTES.momenta[3][1], CTES.momenta[0][1] ), 11, 3); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_in(p);


  // gamma
  p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][2], CTES.momenta[2][2], CTES.momenta[3][2], CTES.momenta[0][2] ), 22, 1); 
  p->suggest_barcode( ++npart );
  prod_vtx->add_particle_out(p);
  
  if( CTES.momenta[0][3] != 0 ) // second photon exists
    {
      p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][3], CTES.momenta[2][3], CTES.momenta[3][3], CTES.momenta[0][3] ), 22, 1); 
      p->suggest_barcode( ++npart );
      prod_vtx->add_particle_out(p);
    }
  
  // other products depending on channel
  if (FLAGS.pion == 0) { // mu+ mu-
    // mu+
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][5], CTES.momenta[2][5], CTES.momenta[3][5], CTES.momenta[0][5] ), -13, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
    // mu -
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][6], CTES.momenta[2][6], CTES.momenta[3][6], CTES.momenta[0][6] ), 13, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
  }    
    
  if (FLAGS.pion == 1) { // pi+ pi-
    // pi+
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][5], CTES.momenta[2][5], CTES.momenta[3][5], CTES.momenta[0][5] ),211, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
    // pi-
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][6], CTES.momenta[2][6], CTES.momenta[3][6], CTES.momenta[0][6] ), -211, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
  } 
  
  if (FLAGS.pion == 2) { // pi+ pi- pi0 pi0
    // pi0
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][5], CTES.momenta[2][5], CTES.momenta[3][5], CTES.momenta[0][5] ),111, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
    // pi0
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][6], CTES.momenta[2][6], CTES.momenta[3][6], CTES.momenta[0][6] ), 111, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
    // pi-
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][7], CTES.momenta[2][7], CTES.momenta[3][7], CTES.momenta[0][7] ),-211, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
    // pi+
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][8], CTES.momenta[2][8], CTES.momenta[3][8], CTES.momenta[0][8] ), 211, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
  } 
  
  if (FLAGS.pion == 3) { // pi+ pi- pi+ pi-
    // pi+
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][5], CTES.momenta[2][5], CTES.momenta[3][5], CTES.momenta[0][5] ),211, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
    // pi-
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][6], CTES.momenta[2][6], CTES.momenta[3][6], CTES.momenta[0][6] ), -211, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
    // pi-
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][7], CTES.momenta[2][7], CTES.momenta[3][7], CTES.momenta[0][7] ),-211, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
    // pi+
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][8], CTES.momenta[2][8], CTES.momenta[3][8], CTES.momenta[0][8] ), 211, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
  } 

  if (FLAGS.pion == 4) { // p pbar
    // pbar
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][5], CTES.momenta[2][5], CTES.momenta[3][5], CTES.momenta[0][5] ),-2212, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
    // p
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][6], CTES.momenta[2][6], CTES.momenta[3][6], CTES.momenta[0][6] ), 2212, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
  } 

  if (FLAGS.pion == 5) { // n nbar
    // nbar
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][5], CTES.momenta[2][5], CTES.momenta[3][5], CTES.momenta[0][5] ),-2112, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
    // n
    p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][6], CTES.momenta[2][6], CTES.momenta[3][6], CTES.momenta[0][6] ), 2112, 1); 
    p->suggest_barcode( ++npart );
    prod_vtx->add_particle_out(p);
  } 
  
     if (FLAGS.pion == 6) { // K+ K-
       // pbar
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][5], CTES.momenta[2][5], CTES.momenta[3][5], CTES.momenta[0][5] ),321, 1); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
       // p
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][6], CTES.momenta[2][6], CTES.momenta[3][6], CTES.momenta[0][6] ), -321, 1); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
     } 
     
     if (FLAGS.pion == 7) { // K0 K0bar
       // pbar
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][5], CTES.momenta[2][5], CTES.momenta[3][5], CTES.momenta[0][5] ), 311, 1); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
       // p
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][6], CTES.momenta[2][6], CTES.momenta[3][6], CTES.momenta[0][6] ), -311, 1); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
     } 

     if (FLAGS.pion == 8) { // pi+ pi- pi0
       // pi+
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][5], CTES.momenta[2][5], CTES.momenta[3][5], CTES.momenta[0][5] ), 211, 1); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
       // pi-
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][6], CTES.momenta[2][6], CTES.momenta[3][6], CTES.momenta[0][6] ), -211, 1); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
       // pi0
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][7], CTES.momenta[2][7], CTES.momenta[3][7], CTES.momenta[0][7] ), 111, 1); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
     } 

     if (FLAGS.pion == 9) { // Lambda Lambdabar Pi+ Pi- P Pbar
       // Lambdabar
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][5], CTES.momenta[2][5], CTES.momenta[3][5], CTES.momenta[0][5] ), -3122, 2); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
       // Lambda
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][6], CTES.momenta[2][6], CTES.momenta[3][6], CTES.momenta[0][6] ), 3122, 2); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
       // pi+
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][7], CTES.momenta[2][7], CTES.momenta[3][7], CTES.momenta[0][7] ), 211, 1); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
       // pbar
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][8], CTES.momenta[2][8], CTES.momenta[3][8], CTES.momenta[0][8] ), -2212, 1); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
       // pi-
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][9], CTES.momenta[2][9], CTES.momenta[3][9], CTES.momenta[0][9] ), -211, 1); 
       p->suggest_barcode( ++npart );
       prod_vtx->add_particle_out(p);
       // p
       p = new GenParticle( CLHEP::HepLorentzVector( CTES.momenta[1][10], CTES.momenta[2][10], CTES.momenta[3][10], CTES.momenta[0][10] ), 2212, 1); 
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

StatusCode Phokhara::finalize() 
{
  MsgStream log(messageService(), name());
  // =================================================
  if(FLAGS.pion == 9)
    MAXIMA.Mmax[1] = MAXIMA.Mmax[1] * (1.0 + LAMBDA_PAR.alpha_lamb)*(1.0 + LAMBDA_PAR.alpha_lamb) 
      * LAMBDA_PAR.ratio_lamb*LAMBDA_PAR.ratio_lamb;
  
  // --- value of the cross section ------------------
  if (FLAGS.nlo == 0) { 
    sigma = MAXIMA.Mmax[0]/MAXIMA.count[0]*MAXIMA.tr[0];
    dsigm = MAXIMA.Mmax[0]*sqrt((MAXIMA.tr[0]/MAXIMA.count[0]-(MAXIMA.tr[0]/MAXIMA.count[0])*(MAXIMA.tr[0]/MAXIMA.count[0]))/MAXIMA.count[0]);
  } else {
    sigma1 = MAXIMA.Mmax[0]/MAXIMA.count[0]*MAXIMA.tr[0];
    sigma2 = MAXIMA.Mmax[1]/MAXIMA.count[1]*MAXIMA.tr[1];
    dsigm1 = MAXIMA.Mmax[0]*sqrt((MAXIMA.tr[0]/MAXIMA.count[0]-(MAXIMA.tr[0]/MAXIMA.count[0])*(MAXIMA.tr[0]/MAXIMA.count[0]))/MAXIMA.count[0]);
    dsigm2 = MAXIMA.Mmax[1]*sqrt((MAXIMA.tr[1]/MAXIMA.count[1]-(MAXIMA.tr[1]/MAXIMA.count[1])*(MAXIMA.tr[1]/MAXIMA.count[1]))/MAXIMA.count[1]);
    
    sigma = sigma1+sigma2;
    dsigm = sqrt(dsigm1*dsigm1 + dsigm2*dsigm2);
  }
  
  // --- output --------------------------------------
  cout << "-------------------------------------------------------------" << endl;
  cout << "     PHOKHARA 7.0 Final Statistics                            " << endl;
  cout << "-------------------------------------------------------------"  << endl;
  cout <<  int(MAXIMA.tr[0]+MAXIMA.tr[1]) << " total events accepted of " << endl;
  cout <<  int(ievent)            <<  " total events generated" << endl;
  cout <<  int(MAXIMA.tr[0])    <<  " one photon events accepted of " << endl;
  cout <<  int(MAXIMA.count[0]) <<  " events generated" << endl;
  cout <<  int(MAXIMA.tr[1])    <<  " two photon events accepted of " << endl;
  cout <<  int(MAXIMA.count[1]) <<  " events generated" << endl;
  cout << endl;
  if (FLAGS.nlo != 0) cout <<  "sigma1(nbarn) = " << sigma1 << " +- " << dsigm1 << endl;
  if (FLAGS.nlo != 0) cout <<  "sigma2(nbarn) = " << sigma2 << " +- " << dsigm2 << endl;
  cout <<  "sigma (nbarn) = " << sigma << " +- " << dsigm << endl;
  cout << endl;
  cout <<  "maximum1 = " << MAXIMA.gross[0] << "  minimum1 = " << MAXIMA.klein[0] << endl;
  cout <<  "Mmax1    = " << MAXIMA.Mmax[0]  << endl;        
  cout <<  "maximum2 = " << MAXIMA.gross[1] << "  minimum2 = " << MAXIMA.klein[1] << endl;
  cout <<  "Mmax2    = " << MAXIMA.Mmax[1]  << endl;         
  cout <<  "-------------------------------------------------------------" << endl;
    
  log << MSG::INFO << "Phokhara finalized" << endreq;

  return StatusCode::SUCCESS;
}
