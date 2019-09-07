//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$Id: BesTofSD.cc

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "BesTofSD.hh"
#include "ReadBoostRoot.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "G4Event.hh"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "G4LossTableManager.hh"
#include "G4ElectronIonPair.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesTofSD::BesTofSD( G4String name ) : BesSensitiveDetector(name), m_besTofList(0) {
    collectionName.insert("BesTofHitsCollection");
    collectionName.insert("BesTofHitsList");
    elIonPair = new G4ElectronIonPair();

    PropertyMgr m_propMgr;
    m_propMgr.declareProperty("FanoFactor",         m_fanoFactor = 0.2);
    m_propMgr.declareProperty("nionOff",                m_nionOff = 0.5);

    IJobOptionsSvc* jobSvc;
    Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);
    jobSvc->setMyProperties("BesTofSD", &m_propMgr);

    cout<<"BesTofSD Property:"<<endl
        <<"  FanoFactor= "<<m_fanoFactor
        <<"  nionOff= "<<m_nionOff
        <<endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesTofSD::~BesTofSD() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesTofSD::Initialize( G4HCofThisEvent* ) {
    m_besTofCollection = new BesTofHitsCollection( SensitiveDetectorName, collectionName[0] );
    //elIonPair = G4LossTableManager::Instance()->ElectronIonPair();
}

//for MC Truth
void BesTofSD::BeginOfTruthEvent( const G4Event* evt) {
    m_event = evt->GetEventID();
    m_besTofList = new BesTofHitsCollection( SensitiveDetectorName, collectionName[1] );
    m_trackIndexes.clear();
    m_trackIndex = -99;
}

void BesTofSD::EndOfTruthEvent( const G4Event* evt ) {
    static G4int HLID=-1;
    if( HLID<0 ) {
        HLID = G4SDManager::GetSDMpointer()->GetCollectionID( collectionName[1] );
    }
    G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
    HCE->AddHitsCollection( HLID, m_besTofList );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool BesTofSD::ProcessHits( G4Step* aStep, G4TouchableHistory* ) {

    G4double chg=aStep->GetTrack()->GetDefinition()->GetPDGCharge();
    G4double edep = aStep->GetTotalEnergyDeposit();
    G4double stepL=aStep->GetStepLength();
    G4double deltaT=aStep->GetDeltaTime();
    G4StepPoint* preStep = aStep->GetPreStepPoint();
    G4ThreeVector pDirection=preStep->GetMomentumDirection();
    G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();
    G4Material* scinMaterial = aStep->GetTrack()->GetMaterial();
    G4double charge = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
    G4int pdgcode = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

    if( chg==0 && edep==0 && stepL==0 ) { return false; }

    BesTofHit *newHit = new BesTofHit();
    G4int trackId = aStep->GetTrack()->GetTrackID();

    newHit->SetEvent(m_event);
    newHit->SetTrackIndex(-99);
    newHit->SetG4Index(aStep->GetTrack()->GetTrackID());
    newHit->SetEdep(edep);
    newHit->SetStepL(stepL);
    //newHit->SetPName(particleName);
    newHit->SetTrackL(aStep->GetTrack()->GetTrackLength());
    G4ThreeVector pos=preStep->GetPosition();
    newHit->SetPos(pos);
    G4double globalTime=preStep->GetGlobalTime();
    newHit->SetTime(globalTime);
    newHit->SetDeltaT(deltaT);
    newHit->SetPDirection(pDirection);
    newHit->SetMomentum(preStep->GetMomentum());
    //newHit->SetMaterial(scinMaterial);
    newHit->SetCharge(charge);
    newHit->SetPDGcode(pdgcode);

    //Get local position in sensitive detector, add by anff
    G4ThreeVector locPos(0,0,0);
    G4TouchableHistory* theTouchable = (G4TouchableHistory*)( preStep->GetTouchable() );
    //partId: barrel(1), east endcap(0), west endcap(2);
    //scinNb: barrel(0-175),east endcap(0-47), west endcap(0-47)
    //partId: mrpc: east(3), west(4), 

    G4String name;
    //for(G4int i=0; i<theTouchable->GetHistoryDepth(); i++)
    //{
    //    name = theTouchable->GetVolume(i)->GetName();    
    //    G4cout<<"********* name of the "<<i<<" volume:  "<<name<<" ***************"<<G4endl;
    //}
    name = theTouchable->GetVolume(0)->GetName();

    G4int partId=-1, scinNb=-1, gapNb=-1, number=-1;
    G4int strip = -1;
    gapNb = theTouchable->GetReplicaNumber(0);
    number = theTouchable->GetReplicaNumber(2); //This is valid only for scintillator

    //MRPC ENDCAPS construct by geant4 code
    if(name.contains("physical_gasLayer"))
    {
        locPos = theTouchable->GetHistory()->GetTopTransform().TransformPoint(pos);
        number = theTouchable->GetReplicaNumber(3);
        scinNb = number;

        G4String name1 = theTouchable->GetVolume(4)->GetName();
        if(name1 == "physicalEcTofEast") partId=3;
        else if(name1 == "physicalEcTofWest") partId=4;
        //G4cout<<"scinNb= "<<scinNb<<"  gapNb= "<<gapNb<<"  partId="<<partId<<G4endl;
    }
    //MRPC ENDCAPS construct by GDML
    else if(name=="logical_gasLayer")
    {
        locPos = theTouchable->GetHistory()->GetTopTransform().TransformPoint(pos);
        number = theTouchable->GetReplicaNumber(3);
        scinNb = 35-number;

        G4String name1 = theTouchable->GetVolume(4)->GetName();
        if(name1 == "logicalEcTofEast") partId=3;
        else if(name1 == "logicalEcTofWest") partId=4;
        //G4cout<<"scinNb= "<<scinNb<<"  gapNb= "<<gapNb<<"  partId="<<partId<<G4endl;
    }


    // Scintillator
    else if( name=="physicalScinBr1" ) {
        partId = 1;
        scinNb = number;
        //G4cout<<"scinNb= "<<scinNb<<"  partId="<<partId<<G4endl;
    }
    else if( name=="physicalScinBr2" ) {
        partId = 1;
        scinNb = number+88;
        //G4cout<<"scinNb= "<<scinNb<<"  partId="<<partId<<G4endl;
    }
    else if( name=="physicalScinEcWest" ) {
        partId = 2;
        scinNb = number;
        //G4cout<<"scinNb= "<<scinNb<<"  partId="<<partId<<G4endl;
    }
    else if( name=="physicalScinEcEast" ) {
        partId = 0;
        scinNb = number;
        //G4cout<<"scinNb= "<<scinNb<<"  partId="<<partId<<G4endl;
    }


    //if construct by gdml files
    else if( name=="logicalScinBr1" || name=="logicalScinBr2" ) {
        partId = 1;
        scinNb = (527-number)/3;
    }
    //copy number: east:1-95(scinNb:47-0), west:1-95(scinNb:47-0)
    else if( name=="logicalScinEcEast" ) {
        partId = 0;
        scinNb = (95-number)/2;
    }
    else if( name=="logicalScinEcWest" ) {
        partId  = 2;
        scinNb = (95-number)/2;
    }
    else { return false; }


    if(name.contains("physical_gasLayer") || name.contains("logical_gasLayer"))
    {
        G4double zz = locPos.z()-0.5*mm+(24+3)*mm*6; //0.5mm is the offset between strip and gasLayer
        if(zz<=0)
        {
            strip=0;
        }
        else if(zz>0 && zz<12*27*mm)
        {
            for(G4int i=0; i<12; i++)
            {
                if(zz>i*27*mm && zz<=(i+1)*27*mm)
                {
                    strip = i;
                    //G4cout<<"Local z is "<<locPos.z()<<"  strip number is: "<<strip<<" !!!"<<G4endl;        
                    break;
                }
            }
        }
        else
        {
            strip=11;
        }
        if(strip<0) strip=0;
        if(strip>11) strip=11;
    }


    newHit->SetPartId(partId);
    newHit->SetScinNb(scinNb);
    newHit->SetGapNb(gapNb);
    newHit->SetLocPos(locPos);
    newHit->SetModule(scinNb);
    newHit->SetStrip(strip);
    //newHit->SetIons(elIonPair->SampleNumberOfIonsAlongStep(aStep));
    newHit->SetIons(SampleNumberOfIonsAlongStep(aStep,elIonPair));
    //newHit->Draw();

    G4int trackIndex, g4TrackId;
    GetCurrentTrackIndex( trackIndex, g4TrackId );
    newHit->SetTrackIndex( trackIndex );
    //newHit->Print();
    if( edep>0 ) {
        m_besTofCollection->insert( newHit );
    }

    //for mc truth
    if( m_besTofList ) {
        G4int trackIndex, g4TrackId;
        GetCurrentTrackIndex(trackIndex, g4TrackId);
        newHit->SetTrackIndex(trackIndex);
        if( m_trackIndex != trackIndex ) {
            m_trackIndex = trackIndex;
            //G4int size = m_trackIndexes.size();
            //G4int flag=1;
            //if(size>0)
            //{
            //  for(G4int i=0;i<size;i++)
            //    if(m_trackIndexes[i] == trackIndex )
            //    {flag =0; break;}
            //}
            //if(flag && aStep->GetTrack()->GetTrackID()==g4TrackId )
            G4int flag = 1;
            G4int pdg = abs(aStep->GetTrack()->GetDefinition()->GetPDGEncoding());
            if( pdg==12 || pdg==14 || pdg==16 ) { flag=0; }
            if( flag && aStep->GetTrack()->GetTrackID()==g4TrackId ) {
                m_trackIndexes.push_back(trackIndex);
                BesTofHit* truHit = new BesTofHit();
                *truHit = *newHit;
                m_besTofList->insert(truHit);
            }
        }
    }
    if( edep<=0 ) { delete newHit; }

    return true;

}//Close Process Hits

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesTofSD::EndOfEvent( G4HCofThisEvent* HCE ) {
    static G4int HCID=-1;
    if( HCID<0 ) {
        HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    }
    HCE->AddHitsCollection(HCID,m_besTofCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int BesTofSD::SampleNumberOfIonsAlongStep( const G4Step* step, G4ElectronIonPair* eipair ) {
    G4double FanoFactor = m_fanoFactor; //0.2;  //Is this value appropiate?
    G4double meanion = eipair->MeanNumberOfIonsAlongStep(step);
    G4double sig = std::sqrt(FanoFactor*meanion);  //Be carefull : In originally Geant4 code this calculatin is wrong! 
    G4int nion = G4int(G4RandGauss::shoot(meanion,sig) + m_nionOff); //0.5);
    return nion;
}


