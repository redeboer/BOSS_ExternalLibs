#include "Phokhara/PhokharaDef.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#include "Phokhara/ranlxd.h" 

using namespace std;

PROTOCCALLSFSUB1(RLXDRESETF,rlxdresetf,INTV);
#define RLXDRESETF(SEED) CCALLSFSUB1(RLXDRESETF,rlxdresetf,INTV, SEED)

PROTOCCALLSFSUB3(INPUT,input,PLONG,PINT,PSTRING);
#define INPUT(NGES,NM,OUTFILE) CCALLSFSUB3(INPUT,input,PLONG,PINT,PSTRING,NGES,NM,OUTFILE)

PROTOCCALLSFSUB0(INITHISTO,inithisto);
#define INITHISTO() CCALLSFSUB0(INITHISTO,inithisto)

PROTOCCALLSFSUB0(ENDHISTO,endhisto);
#define ENDHISTO() CCALLSFSUB0(ENDHISTO,endhisto)

PROTOCCALLSFSUB0(WRITEEVENT,writeevent);
#define WRITEEVENT() CCALLSFSUB0(WRITEEVENT,writeevent)

PROTOCCALLSFSUB2(RANLXDF,ranlxdf,DOUBLEV,INT);
#define RANLXDF(AR, VAL) CCALLSFSUB2(RANLXDF,ranlxdf,DOUBLEV, INT, AR, VAL)

PROTOCCALLSFSUB7(GEN_1PH,gen_1ph_,INT, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE);
#define GEN_1PH(I,QQMIN,QQMAX,COS1MIN,COS1MAX,COS3MIN,COS3MAX) CCALLSFSUB7(GEN_1PH,gen_1ph_,INT, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE,I,QQMIN,QQMAX,COS1MIN,COS1MAX,COS3MIN,COS3MAX)

PROTOCCALLSFSUB8(GEN_2PH,gen_2ph_,INT, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE);
#define GEN_2PH(I,QQMIN,COS1MIN,COS1MAX,COS2MIN,COS2MAX,COS3MIN,COS3MAX) CCALLSFSUB8(GEN_2PH,gen_2ph_,INT, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE, PDOUBLE,I,QQMIN,COS1MIN,COS1MAX,COS2MIN,COS2MAX,COS3MIN,COS3MAX)

int main(){
  double qqmin,qqmax;
  double cos1min,cos1max,cos2min,cos2max,cos3min,cos3max;
  double dsigm1,dsigm2,sigma1,sigma2,sigma,dsigm,Ar[14],Ar_r[14];
  int nm,i,s_seed[105];
  long int nges,k,j;
  char outfile[20];

  // --- reads the seed ------
  ifstream seeds("seed.dat");
  if( seeds.is_open() )
    {
      int ii=0;
      while( ! seeds.eof() )
	seeds >> s_seed[ii++];
    }
  else
    cerr << "Cannot open file seed.dat for reading" << endl;
  
  RLXDRESETF(s_seed);
  //rlxd_reset(s_seed);
  
  // --- input parameters ----------------------------
  
  INPUT(nges, nm, outfile);

  // --- open output file for generated momenta ------
  //    if(iprint.ne.0) open (10,file=outfile,type="new")
  
  // --- print run data ------------------------------
  cout << "----------------------------------------------------" << FLAGS.pion<< endl;
  if (FLAGS.pion == 0) 
    cout << "     PHOKHARA 6.0 : e^+ e^- -> mu^+ mu^- gamma" << endl;
  else if (FLAGS.pion == 1) 
    cout << "     PHOKHARA 6.0: e^+ e^- -> pi^+ pi^- gamma" << endl;    
  else if (FLAGS.pion == 2) 
    cout << "   PHOKHARA 6.0: e^+ e^- -> pi^+ pi^- 2pi^0 gamma" << endl;        
  else if (FLAGS.pion == 3) 
    cout << "   PHOKHARA 6.0: e^+ e^- -> 2pi^+ 2pi^- gamma" << endl; 
  else if (FLAGS.pion == 4) 
    cout << "   PHOKHARA 6.0: e^+ e^- -> p pbar gamma" << endl; 
  else if (FLAGS.pion == 5) 
    cout << "   PHOKHARA 6.0: e^+ e^- -> n nbar gamma" << endl;                
  else if (FLAGS.pion == 6) 
    cout << "   PHOKHARA 6.0: e^+ e^- -> K^+ K^- gamma" << endl;                
  else if (FLAGS.pion == 7) 
    cout << "   PHOKHARA 6.0: e^+ e^- -> K_0 K_0bar gamma" << endl;                
  else if (FLAGS.pion == 8) 
    cout << "   PHOKHARA 6.0: e^+ e^- -> pi^+ pi^- pi^0 gamma" << endl;                
  else if (FLAGS.pion == 9) {
    cout <<  "PHOKHARA 6.0 : e^+ e^- ->" << endl;
    cout << "  Lambda (-> pi^- p) Lambda bar (-> pi^+ pbar) gamma" << endl;
  } else 
    cout <<  "     PHOKHARA 6.0: not yet implemented" << endl;
  
  // --------------------------------
  cout <<  "----------------------------------------------------" << endl;
  printf(" %s %f %s\n","cms total energy                       = ",sqrt(CTES.Sp)," GeV  "); 
  if (FLAGS.tagged == 0) { 
    if((CUTS.gmin/2.0/CTES.ebeam) < 0.0098){
      cerr << " minimal missing energy set to small" << endl;
      return 0;
    }
    printf(" %s %f %s\n","minimal tagged photon energy           = ",CUTS.gmin," GeV  ");	
    printf(" %s %f,%f\n","angular cuts on tagged photon          = ",CUTS.phot1cut,CUTS.phot2cut);
  }
  else {
    if((CUTS.gmin/2.0/CTES.ebeam) < 0.0098){
      cerr << " minimal missing energy set to small" << endl;
      return 0;
    }
    printf(" %s %f %s\n","minimal missing energy                 = ",CUTS.gmin," GeV  ");
    printf(" %s %f,%f\n","angular cuts on missing momentum       = ",CUTS.phot1cut, CUTS.phot2cut);
  }

 
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
    
  if (FLAGS.tagged == 0) {
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
  }
	
// --- book histograms -----------------------------

     INITHISTO();

// --- set cuts ------------------------------------
      if (FLAGS.tagged == 0) { 
	cos1min = cos(CUTS.phot2cut*CTES.pi/180.0);     // photon1 angle cuts in the 
	cos1max = cos(CUTS.phot1cut*CTES.pi/180.0);     // LAB rest frame            
      } else {
	cos1min = -1.0;                         
	cos1max =  1.0;
	CUTS.gmin = CUTS.gmin/2.0;
      }
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
	cerr << " Q^2_min CHANGE BY PHOKHARA = " << qqmin << " GeV^2" << endl;
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
        cout << "Vacuum polarization is NOT included" << endl;
      } else if(FLAGS.ivac == 1){
        cout << "Vacuum polarization is included"  << endl;
      } else {
        cout << "WRONG vacuum polarization switch" << endl;
	return 0;
      }
// -----------------
      if(FLAGS.pion == 1){
        if(FLAGS.FF_pion == 0)
          cout << "Kuhn-Santamaria PionFormFactor" << endl;
        else if(FLAGS.FF_pion == 1)
          cout << "Gounaris-Sakurai PionFormFactor" << endl;
        else {
	  cout << "WRONG PionFormFactor switch" << endl;
	  return 0;
	}
// ------
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
//
// =================================================
// --- finding the maximum -------------------------
      k = nm;
      for( i = 0; i<2; i++)
	{
	  MAXIMA.Mmax[i] = 1.0;
	  MAXIMA.gross[i] = 0.0;
	  MAXIMA.klein[i] = 0.0;      
	}
      if (FLAGS.nlo == 0) 
	MAXIMA.Mmax[1]=0.0;   // only 1 photon events generated
      
      for( i = 1; i<=2; i++)    {    // initializing the MC loop
	MAXIMA.tr[0] = 0.0;      
	MAXIMA.tr[1] = 0.0;
	MAXIMA.count[0] = 0.0;
	MAXIMA.count[1] = 0.0;
	
	// =================================================
	// --- beginning the MC loop event generation ------
	for(j = 1; j <= k; j++)
	  {
	    RANLXDF(Ar_r,1);
	    Ar[1] = Ar_r[0];
	    
	    if (Ar[1] <= (MAXIMA.Mmax[0]/(MAXIMA.Mmax[0]+MAXIMA.Mmax[1]))) { 
	      MAXIMA.count[0] = MAXIMA.count[0]+1.0;
	      GEN_1PH(i,qqmin,qqmax,cos1min,cos1max,cos3min,cos3max);
	    } 
	    else {
	      MAXIMA.count[1] = MAXIMA.count[1]+1.0;
	      GEN_2PH(i,qqmin,cos1min,cos1max,cos2min,cos2max,cos3min,cos3max);
	    }
	  }
	// --- end of the MC loop --------------------------
	// =================================================
	// --- for the second run ---
	k = nges;
	if (i == 1) {
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
	}
      }    
      // --- end of the second run -----------------------
      
      // =================================================
      if(FLAGS.pion == 9)
	MAXIMA.Mmax[1] = MAXIMA.Mmax[1] * (1.0 + LAMBDA_PAR.alpha_lamb)*(1.0 + LAMBDA_PAR.alpha_lamb) 
	  * LAMBDA_PAR.ratio_lamb*LAMBDA_PAR.ratio_lamb;
      
      // --- save histograms -----------------------------
      
      ENDHISTO();

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
      cout <<  "----------------------------------------------------"  << endl;
      cout <<  int(MAXIMA.tr[0]+MAXIMA.tr[1]) << " total events accepted of " << endl;
      cout <<  int(nges)            <<  " total events generated" << endl;
      cout <<  int(MAXIMA.tr[0])    <<  " one photon events accepted of " << endl;
      cout <<  int(MAXIMA.count[0]) <<  " events generated" << endl;
      cout <<  int(MAXIMA.tr[1])    <<  " two photon events accepted of " << endl;
      cout <<  int(MAXIMA.count[1]) <<  " events generated" << endl;
      cout << endl;
      if (FLAGS.nlo != 0) cout <<  "sigma1(nbarn) = " << sigma1 << " +- " << dsigm1 << endl;
      if (FLAGS.nlo != 0) cout <<  "sigma2(nbarn) = " << sigma2 << " +- " << dsigm2 << endl;
      cout <<  "sigma (nbarn) = " << sigma << " +- " <<dsigm << endl;
      cout << endl;
      cout <<  "maximum1 = " << MAXIMA.gross[0] << "  minimum1 = " << MAXIMA.klein[0] << endl;
      cout <<  "Mmax1    = " << MAXIMA.Mmax[0]  << endl;        
      cout <<  "maximum2 = " << MAXIMA.gross[1] << "  minimum2 = " << MAXIMA.klein[1] << endl;
      cout <<  "Mmax2    = " << MAXIMA.Mmax[1]  << endl;         
      cout <<  "----------------------------------------------------" << endl;
	
// --- saves the new seed --------------------------
/*
      close (9,DISP="delete")
      open(9,file="seed.dat",type="new")

      call rlxdgetf(s_seed)
      write(9,*)s_seed

      end
    */

return 0;	
}
