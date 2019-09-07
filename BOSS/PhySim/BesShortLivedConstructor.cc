//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Descpirtion: Rewrite G4ShortLivedConstructor.hh, remove quark part
//Author: Liuhm
//Created: 2003/5/19
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//
#include "BesShortLivedConstructor.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ShortLivedTable.hh"
#include "G4PhaseSpaceDecayChannel.hh"
#include "G4VDecayChannel.hh"
#include "G4DecayTable.hh"

G4bool BesShortLivedConstructor::isConstructed = false;

BesShortLivedConstructor::BesShortLivedConstructor()
{
}

BesShortLivedConstructor::~BesShortLivedConstructor()
{
}


void BesShortLivedConstructor::ConstructParticle()
{
  if (!isConstructed){
    ConstructResonances();
    isConstructed = true;
  }
}


#include "G4ExcitedNucleonConstructor.hh"
#include "G4ExcitedDeltaConstructor.hh"
#include "G4ExcitedLambdaConstructor.hh"
#include "G4ExcitedSigmaConstructor.hh"
#include "G4ExcitedXiConstructor.hh"
#include "G4ExcitedMesonConstructor.hh"
void BesShortLivedConstructor::ConstructResonances()
{
  ConstructBaryons();
  ConstructMesons();

  // N*
  G4ExcitedNucleonConstructor nucleons;
  nucleons.Construct();

  // Delta*
  G4ExcitedDeltaConstructor deltas;
  deltas.Construct();

  // Lambda*
  G4ExcitedLambdaConstructor lamdas;
  lamdas.Construct();

  // Sigma*
  G4ExcitedSigmaConstructor sigmas;
  sigmas.Construct();

  // Xi*
  G4ExcitedXiConstructor xis;
  xis.Construct();
 
  // Mesons
  G4ExcitedMesonConstructor mesons;
  mesons.Construct();

}


#include "G4ExcitedBaryons.hh"
void BesShortLivedConstructor::ConstructBaryons()
{
  G4DecayTable*   decayTable;
  G4VDecayChannel* mode;
  G4ExcitedBaryons* particle;

  //    Construct Resonace particles as dynamic object
  //    Arguments for constructor are as follows
  //               name             mass          width         charge
  //             2*spin           parity  C-conjugation
  //          2*Isospin       2*Isospin3       G-parity
  //               type    lepton number  baryon number   PDG encoding
  //             stable         lifetime    decay table 

  // delta baryons
  //  delta(1232)++
  particle = new G4ExcitedBaryons(            
	    "delta++",       1.232*GeV,     115.0*MeV,    +2.0*eplus, 
                    3,              +1,             0,          
                    3,              +3,             0,             
             "baryon",               0,            +1,          2224,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("delta");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of delta++ -> proton + pi+
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("delta++",1.000, 2,
				                        "proton","pi+");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  //  delta(1232)+
  particle = new G4ExcitedBaryons(            
	     "delta+",       1.232*GeV,     115.0*MeV,    +1.0*eplus, 
                    3,              +1,             0,          
                    3,              +1,             0,             
             "baryon",               0,            +1,          2214,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("delta(1232)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of delta+  -> proton + Gamma
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("delta+", 0.01, 2,
				                        "proton","gamma");
  decayTable->Insert(mode);
  // create decay channel of delta+  -> neutron + pi+
  //                                   parent    BR     #daughters
  // create decay channel of delta+  -> proton + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("delta+", 0.495, 2,
				                        "proton","pi0");
  decayTable->Insert(mode);
  // create decay channel of delta+  -> neutron + pi+
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("delta+", 0.495, 2,
				                        "neutron","pi+");
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  //  delta(1232)0
  particle = new G4ExcitedBaryons(            
	     "delta0",       1.232*GeV,     115.0*MeV,    +0.0*eplus, 
                    3,              +1,             0,          
                    3,              -1,             0,             
             "baryon",               0,            +1,          2114,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("delta(1232)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of delta+  -> neutron + gamma
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("delta0", 0.01, 2,
				                        "neutron","gamma");
  decayTable->Insert(mode);
  // create decay channel of delta+  -> proton + pi-
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("delta0", 0.495, 2,
				                        "proton","pi-");
  decayTable->Insert(mode);
  // create decay channel of delta+  -> neutron + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("delta0", 0.495, 2,
				                        "neutron","pi0");
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  //  delta(1232)-
  particle = new G4ExcitedBaryons(            
	     "delta-",       1.232*GeV,     115.0*MeV,    -1.0*eplus, 
                    3,              +1,             0,          
                    3,              -3,             0,             
             "baryon",               0,            +1,          1114,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("delta(1232)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of delta+  -> neutron + pi-
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("delta-", 1.000, 2,
				                        "neutron","pi-");
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);


  ////////////////////////////
  // anti_delta baryons
  //  anti_delta(1232)++
  particle = new G4ExcitedBaryons(            
       "anti_delta++",       1.232*GeV,     115.0*MeV,    -2.0*eplus, 
                    3,              +1,             0,          
                    3,              -3,             0,             
             "baryon",               0,            -1,         -2224,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("delta(1232)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of delta++ -> anti_proton + pi-
  //                                        parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("anti_delta++",1.000, 2,
				                   "anti_proton","pi-");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  //  anti_delta(1232)+
  particle = new G4ExcitedBaryons(            
	"anti_delta+",       1.232*GeV,     115.0*MeV,    -1.0*eplus, 
                    3,              +1,             0,          
                    3,              -1,             0,             
             "baryon",               0,            -1,         -2214,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("delta(1232)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of anti_delta+  -> anti_proton + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("anti_delta+", 0.500, 2,
				                   "anti_proton","pi0");
  decayTable->Insert(mode);
  // create decay channel of anti_delta+  -> anti_neutron + pi-
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("anti_delta+", 0.500, 2,
				                  "anti_neutron","pi-");
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  //  anti_delta(1232)0
  particle = new G4ExcitedBaryons(            
	"anti_delta0",       1.232*GeV,     115.0*MeV,    +0.0*eplus, 
                    3,              +1,             0,          
                    3,              +1,             0,             
             "baryon",               0,            -1,         -2114,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("delta(1232)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of anti_delta+  -> anti_proton + pi+
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("anti_delta0", 0.500, 2,
				                        "anti_proton","pi+");
  decayTable->Insert(mode);
  // create decay channel of delta+  -> neutron + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("anti_delta0", 0.500, 2,
				                        "anti_neutron","pi0");
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  //  anti_delta(1232)-
  particle = new G4ExcitedBaryons(            
	"anti_delta-",       1.232*GeV,     115.0*MeV,    +1.0*eplus, 
                    3,              +1,             0,          
                    3,              +3,             0,             
             "baryon",               0,            -1,         -1114,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("delta(1232)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of delta-  -> neutron + pi+
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("anti_delta-", 1.000, 2,
				                        "anti_neutron","pi+");
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);



}
#include "G4ExcitedMesons.hh"
void BesShortLivedConstructor::ConstructMesons()
{
  G4DecayTable*   decayTable;
  G4VDecayChannel* mode;
  G4ExcitedMesons* particle;

  //    Construct Resonace particles as dynamic object
  //    Arguments for constructor are as follows
  //               name             mass          width         charge
  //             2*spin           parity  C-conjugation
  //          2*Isospin       2*Isospin3       G-parity
  //               type    lepton number  baryon number   PDG encoding
  //             stable         lifetime    decay table 

  // vector mesons
  // omega
  particle = new G4ExcitedMesons(            
	      "omega",      781.94*MeV,      8.41*MeV,    +0.0*eplus, 
                    2,              -1,            -1,          
                    0,              +0,            -1,             
              "meson",               0,             0,           223,
		false,             0.0,          NULL);
  particle->SetAntiPDGEncoding(223);
  // set sub type
  particle->SetMultipletName("omega");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of omega -> pi+ + pi- + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("omega",0.888, 3,
				                    "pi+","pi-","pi0");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  // phi
  particle = new G4ExcitedMesons(            
	        "phi",      1019.4*MeV,      4.43*MeV,    +0.0*eplus, 
                    2,              -1,            -1,          
                    0,              +0,            -1,             
              "meson",               0,             0,           333,
		false,             0.0,          NULL);
  particle->SetAntiPDGEncoding(333);
  // set sub type
  particle->SetMultipletName("phi");
   // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of phi -> kaon+ + kaon-
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("phi",0.491, 2,
				                    "kaon+","kaon-");
  decayTable->Insert(mode);
   // create decay channel of phi -> kaon0S + kaon0L
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("phi",0.343, 2,
				                   "kaon0S","kaon0L");
  // add decay table
  decayTable->Insert(mode);
  // create decay channel of phi -> rho0 + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("phi",0.129, 2,
				                   "rho0","pi0");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);
 
  // rho+
  particle = new G4ExcitedMesons(            
	       "rho+",       769.9*MeV,     151.2*MeV,    +1.0*eplus, 
                    2,              -1,            -1,          
                    2,              +2,            +1,             
              "meson",               0,             0,           213,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("rho");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of rho+ -> pi+ + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("rho+",1.000, 2,
				                    "pi+","pi0");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  // rho-
  particle = new G4ExcitedMesons(            
	       "rho-",       769.9*MeV,     151.2*MeV,    -1.0*eplus, 
                    2,              -1,            -1,          
                    2,              -2,            +1,             
              "meson",               0,             0,          -213,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("rho");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of rho- -> pi- + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("rho-",1.000, 2,
				                    "pi-","pi0");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);
  
  // rho0
  particle = new G4ExcitedMesons(            
               "rho0",       770.0*MeV,     150.7*MeV,         0.0, 
                    2,              -1,            -1,          
                    2,               0,            +1,             
              "meson",               0,             0,         113,
                false,          0.0*ns,          NULL );
  particle->SetAntiPDGEncoding(113);
  // set sub type
  particle->SetMultipletName("rho");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of rho0 -> pi+ + pi-
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("rho0",1.000, 2,
				                    "pi+","pi-");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  // a0(980)+
  particle = new G4ExcitedMesons(            
	   "a0(980)+",       984.8*MeV,      60.0*MeV,    +1.0*eplus, 
                    0,              +1,            +1,          
                    2,              +2,            -1,             
              "meson",               0,             0,       9000211,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("a0(980)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of a0(980)+ -> eta + pi+
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("a0(980)+",1.000, 2,
				                    "pi+","eta");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  // a0(980)-
  particle = new G4ExcitedMesons(            
	   "a0(980)-",       984.8*MeV,      60.0*MeV,    -1.0*eplus, 
                    0,              +1,            +1,          
                    2,              -2,            -1,             
              "meson",               0,             0,      -9000211,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("a0(980)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of a0(980)- -> eta + pi-
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("a0(980)-",1.000, 2,
				                    "pi-","eta");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  // a0(980)0
  particle = new G4ExcitedMesons(            
	   "a0(980)0",       984.8*MeV,      60.0*MeV,           0.0,
                    0,              +1,            +1,          
                    2,               0,            -1,             
              "meson",               0,             0,       9000111,
		false,             0.0,          NULL);
  particle->SetAntiPDGEncoding(9000111);
  // set sub type
  particle->SetMultipletName("a0(980)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of a0(980)0 -> eta + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("a0(980)0",1.000, 2,
				                    "pi0","eta");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  // f0(400-1200)
  particle = new G4ExcitedMesons(            
       "f0(400-1200)",       470.0*MeV,     400.0*MeV,           0.0,
                    0,              +1,            +1,          
                    0,               0,            +1,             
              "meson",               0,             0,       9000221,
		false,             0.0,          NULL);
  particle->SetAntiPDGEncoding(9000221);
  // set sub type
  particle->SetMultipletName("f0(400-1200)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of f0(400-1200) -> pi + pi
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("f0(400-1200)",1.000, 2,
				                    "pi+","pi-");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);


  // f0(980)
  particle = new G4ExcitedMesons(            
            "f0(980)",       980.0*MeV,      40.0*MeV,           0.0,
                    0,              +1,            +1,          
                    0,               0,            +1,             
              "meson",               0,             0,       9010221,
		false,             0.0,          NULL);
  particle->SetAntiPDGEncoding(9010221);
  // set sub type
  particle->SetMultipletName("f0(980)");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of f0(980) -> pi + pi
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("f0(980)",1.000, 2,
				                    "pi+","pi-");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);


  // k_star+
  particle = new G4ExcitedMesons(            
	    "k_star+",       891.6*MeV,      50.8*MeV,    +1.0*eplus, 
                    2,              -1,             0,          
                    1,              +1,             0,             
              "meson",               0,             0,           323,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("k_star");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of k_star+ -> kaon+ + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("k_star+",0.500, 2,
				                    "kaon+","pi0");
  // add decay table
  decayTable->Insert(mode);
   // create decay channel of k_star+ -> kaon+ + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("k_star+",0.500, 2,
				                    "kaon0","pi+");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);
  
  // k_star0
  particle = new G4ExcitedMesons(            
	    "k_star0",       896.1*MeV,      50.5*MeV,     0.0*eplus, 
                    2,              -1,             0,          
                    1,              -1,             0,             
              "meson",               0,             0,           313,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("k_star");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of k_star0 -> kaon+ + pi-
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("k_star0",0.500, 2,
				                    "kaon+","pi-");
  // add decay table
  decayTable->Insert(mode);
   // create decay channel of k_star0 -> kaon0 + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("k_star0",0.500, 2,
				                    "kaon0","pi0");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);

  // k_star-
  particle = new G4ExcitedMesons(            
            "k_star-",       891.6*MeV,      50.8*MeV,    -1.0*eplus, 
                    2,              -1,             0,          
                    1,              +1,             0,             
              "meson",               0,             0,          -323,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("k_star");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of k_star- -> kaon- + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("k_star-",0.500, 2,
				                    "kaon-","pi0");
  // add decay table
  decayTable->Insert(mode);
   // create decay channel of k_star- -> anti_kaon0 + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("k_star-",0.500, 2,
				                    "anti_kaon0","pi+");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);
  
  
  // anti_k_star0
  particle = new G4ExcitedMesons(            
       "anti_k_star0",       896.1*MeV,      50.5*MeV,     0.0*eplus, 
                    2,              -1,             0,          
                    1,              -1,             0,             
              "meson",               0,             0,          -313,
		false,             0.0,          NULL);
  // set sub type
  particle->SetMultipletName("k_star");
  // create decay table
  decayTable =  new G4DecayTable();
  // create decay channel of anti_k_star0 -> kaon- + pi+
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("anti_k_star0",0.500, 2,
				                    "kaon-","pi+");
  // add decay table
  decayTable->Insert(mode);
   // create decay channel of anti_k_star0 -> anti_kaon0 + pi0
  //                                   parent    BR     #daughters
  mode  = new G4PhaseSpaceDecayChannel("anti_k_star0",0.500, 2,
				                    "anti_kaon0","pi0");
  // add decay table
  decayTable->Insert(mode);
  particle->SetDecayTable(decayTable);
  
}



















