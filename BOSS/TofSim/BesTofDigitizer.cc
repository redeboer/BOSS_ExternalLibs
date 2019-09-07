//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Chief control center for digitizing modes.Also define 'group hits'.
//Author: Dengzy
//Created: Mar, 2004
//Modified: Sun Shengsen based on Matthias Ullrich for MRPC Endcap TOF 2014.04.28.
//Comment: Double Side Readout Version
//---------------------------------------------------------------------------//
//$Id: BesTofDigitizer.cc

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"
#include "G4DigiManager.hh"
#include "ReadBoostRoot.hh"
#include "ScintSingle.hh"
#include "Randomize.hh"
#include "BesTofDigitizer.hh"


BesTofDigitizer::BesTofDigitizer(G4String modName):G4VDigitizerModule(modName) {

    collectionName.push_back("BesTofDigitsCollection");  
    m_besTofDigitsCollection=0;
    //m_digiMessenger = new BesTofDigitizerMessenger(this);

    tofDMBr1 = new BesTofDigitizerBrV1();
    tofDMEc1 = new BesTofDigitizerEcV1();
    tofDMBr2 = new BesTofDigitizerBrV2();
    tofDMEc2 = new BesTofDigitizerEcV2();
    tofDMEc3 = new BesTofDigitizerEcV3();     // Scintillator  
    tofDMEc4 = new BesTofDigitizerEcV4();     // MRPC Double End

    //retrieve G4Svc
    /*ISvcLocator* svcLocator = Gaudi::svcLocator();
      IG4Svc* tmpSvc;
      StatusCode sc=svcLocator->service("G4Svc", tmpSvc);
      G4Svc* g4Svc=dynamic_cast<G4Svc *>(tmpSvc);*/

    //retrieve RealizationSvc
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IRealizationSvc *tmpReal;
    StatusCode scReal = svcLocator->service("RealizationSvc",tmpReal);
    if (!scReal.isSuccess()) {
      std::cout << " Could not initialize Realization Service in BesTofDigitizerBrV2" << std::endl;
    } 
    else {
      m_RealizationSvc = dynamic_cast<RealizationSvc*>(tmpReal);
    }

    m_versionBr=2;

    if( ReadBoostRoot::GetTof()==3 || ReadBoostRoot::GetTof()==4 ) {  
        m_versionEc=4;
        std::cout << std::endl << "TofSim: BesTofDigitizer       You are using the new End Cap Tof (MRPC) Digitizer!" << std::endl;
    }
    else {
        m_versionEc=3;
        std::cout << std::endl << "TofSim: BesTofDigitizer       You are using the old End Cap Tof (Scintillator) Digitizer!" << std::endl;
    }  

}


BesTofDigitizer::~BesTofDigitizer() {
    //delete m_digiMessenger;
    delete tofDMBr1;
    delete tofDMEc1;
    delete tofDMBr2;
    delete tofDMEc2;
    delete tofDMEc3;
    delete tofDMEc4;
}


void BesTofDigitizer::Digitize() {

    m_besTofDigitsCollection = new BesTofDigitsCollection( moduleName, collectionName[0] );
    G4DigiManager* digiManager = G4DigiManager::GetDMpointer();
    G4int THCID = digiManager->GetHitsCollectionID("BesTofHitsCollection");
    m_THC = (BesTofHitsCollection*) (digiManager->GetHitsCollection(THCID));

    if( m_THC ) {
        m_scintGroup = new vector<ScintSingle*>;  //ScintSingle contains: partId, scintnb, edep <--only the basics   
        GroupHits();  //If one finds more hits in one scintillator, they are grouped together 
        G4int size = m_scintGroup->size();

        //    std::cout << "TofSim: BesTofDigitizer     m_scintGroup->size():  " << size << std::endl;

        ScintSingle* scint;
        G4int partId;
        for( G4int i=0; i<size; i++ ) {
            scint = (*m_scintGroup)[i];
            partId = scint->GetPartId();

            //      std::cout << "TofSim: BesTofDigitizer     scint->GetPartId():  " << partId << std::endl;

            if( partId==1 ) {  // for barrel part
                if( m_versionBr==1 )
                  tofDMBr1->Digitize(scint,m_besTofDigitsCollection);
                else if( m_versionBr==2 ) {
                    //std::cout << "TofSim:  tofDMBr2->Digitize" << std::endl;
                    tofDMBr2->Digitize(scint,m_besTofDigitsCollection);
                }
                else {
                    tofDMBr2->Digitize(scint,m_besTofDigitsCollection);
                }
            }
            else {  // for endcap part
                if( m_versionEc==1 ) {
                    tofDMEc1->Digitize(scint,m_besTofDigitsCollection);
                }
                else if( m_versionEc==2 ) {
                    tofDMEc2->Digitize(scint,m_besTofDigitsCollection);
                }
                else if( m_versionEc==3 ) {
                    tofDMEc3->Digitize(scint,m_besTofDigitsCollection);
                }
                else if( m_versionEc==4 ) {
                    tofDMEc4->Digitize(scint,m_besTofDigitsCollection);
                }
                else {
                    cout << "TofSim: Warning: BesTofDigitizerEcV2 is utilized otherwise!" << endl;
                    tofDMEc2->Digitize(scint,m_besTofDigitsCollection);
                }
            }  // Close endcap part
        }

        if( m_scintGroup ) {
            for( size_t i=0; i<m_scintGroup->size(); i++ ) {
                delete (*m_scintGroup)[i];
                //std::cout << "TofSim: i: " << i << std::endl;
                //std::cout << "TofSim: delete m_scintGroup[i] successfully " << std::endl;
            }
            m_scintGroup->clear();
            delete m_scintGroup;
        }
    }  //close if(m_THC)

    StoreDigiCollection(m_besTofDigitsCollection);
}


void BesTofDigitizer::GroupHits() {

    G4int partId, scinNb,size,flag,trackId;
    G4double edep;
    BesTofHit* hit;
    G4int nHits = m_THC->entries();

    // sunss 20140508 add to suppress the efficiency of TOF signal
    vector<G4int> deadTrackVec;
    deadTrackVec.clear();
    G4int runId = m_RealizationSvc->getRunId();
    if( ( runId>=-11396 && runId<=-8093 ) || ( runId>-80000 && runId<=-23463 ) ) {
      G4int previousTrack = -999;
      vector<G4int> trackVec;
      trackVec.clear();
      for( G4int i=0; i<nHits; i++ ) {
	hit    = (*m_THC)[i];
	trackId = hit->GetTrackIndex();
	if( trackId != previousTrack ) {
	  previousTrack = trackId;
	  bool same = false;
 	  vector<G4int>::iterator iter = trackVec.begin();
	  for( ; iter!=trackVec.end(); iter++ ) {
	    if( (*iter) == trackId ) {
	      same = true;
	      break;
	    }
	  }
	  if( !same ) {
	    trackVec.push_back( trackId );
	    G4double ranSignal = G4UniformRand();
	    if( ranSignal>0.988 ) { // hhliu 20140613
	      deadTrackVec.push_back( trackId );
	    }
	  }
	}
      }
    }
    // sunss 20140508

    //group the hits which are in the same scintillator
    for (G4int i=0;i<nHits;i++) {
        hit=(*m_THC)[i];
	// sunss 20140508
	if( ( runId>=-11396 && runId<=-8093 ) || ( runId>-80000 && runId<=-23463 ) ) {
	  trackId = hit->GetTrackIndex();
	  partId = hit->GetPartId();
	  bool aband = false;
	  vector<G4int>::iterator iter = deadTrackVec.begin();
	  for( ; iter!=deadTrackVec.end(); iter++ ) {
	    if( ( trackId == (*iter) ) && ( partId!=3 && partId!=4 ) ) { aband = true; }
	  }
	  if( aband ) continue;
	}
	// sunss 20140508
        partId = hit->GetPartId();
        scinNb = hit->GetScinNb();
        edep = hit->GetEdep();
        size = m_scintGroup->size();
        flag = 0;
        if( size>0 ) {
            ScintSingle* oldScint;
            for( G4int j=0; j<size; j++ ) {
                oldScint = (*m_scintGroup)[j];
                if( oldScint->GetPartId()==partId && oldScint->GetScinNb()==scinNb ) {
		  //std::cout << "BesTodDigitizer:       GroupHits: 'Oldhit:'  partID |  scinnb :  "   << partId   << "  |  "  << scinNb << std::endl;
                    oldScint->GetHitIndexes()->push_back(i);
                    // ATTENTION!!! sum over all deposited energy!
		    oldScint->AddEdep(edep);
                    flag=1;
                    break;
                }
            }
        }

        if( flag==0 ) {
            ScintSingle* newScint = new ScintSingle;
            newScint->SetPartId(partId);
            newScint->SetScinNb(scinNb);
	    newScint->SetEdep(edep);
            newScint->GetHitIndexes()->push_back(i);
            m_scintGroup->push_back(newScint);

            //      std::cout << "BesTofDigitizer:        GroupHits:  NewHitGroup:   " << partId  << "   "  << scinNb << std::endl;  
        }
    }

}
