//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Sep 4, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#include "BesEmcSD.hh"

#include "BesEmcHit.hh"
#include "BesEmcConstruction.hh"
#include "BesEmcGeometry.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4Event.hh"
#include "G4ios.hh"
#include "BesSensitiveManager.hh"
#include "BesTruthTrack.hh"
#include <sstream>
#include "Identifier/EmcID.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesEmcSD::BesEmcSD(G4String name,
		   BesEmcConstruction* det,
		   BesEmcGeometry*     besEMCGeometry)
  :BesSensitiveDetector(name)
  ,Detector(det),fBesEmcGeometry(besEMCGeometry)
{
  collectionName.insert("BesEmcHitsCollection");    //for digitization
  collectionName.insert("BesEmcHitsList");          //for MC truth
  collectionName.insert("BesEmcTruthHitsList");
  HitID = new G4int[20000];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesEmcSD::~BesEmcSD()
{
  delete [] HitID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcSD::Initialize(G4HCofThisEvent*)
{
  CalCollection = new BesEmcHitsCollection
                      (SensitiveDetectorName,collectionName[0]); 
  for (G4int j=0;j<20000;j++) 
    {
      HitID[j] = -1;
    }
  nHit=0;
}

void BesEmcSD::BeginOfTruthEvent(const G4Event* )
{
  CalList =  new BesEmcHitsCollection
                 (SensitiveDetectorName,collectionName[1]);
  CalTruthList =  new BesEmcTruthHitsCollection
                 (SensitiveDetectorName,collectionName[2]);
  m_trackIndex = -99;
}

void BesEmcSD::EndOfTruthEvent(const G4Event* evt)
{
  static G4int HLID=-1;
  if(HLID<0)
    HLID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[1]);
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  HCE->AddHitsCollection(HLID,CalList); 

  static G4int HTID=-1;
  if(HTID<0)
    HTID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[2]);
  HCE->AddHitsCollection(HTID,CalTruthList); 
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool BesEmcSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4double stepl = aStep->GetStepLength();
  if ((edep==0.)&&(stepl==0.)) return false;      

  G4TouchableHistory* theTouchable
    = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  G4VPhysicalVolume* physVol = theTouchable->GetVolume();
  
  if(physVol->GetName().contains("physicalCrystal"))  //barrel code
  {
    PartId=1;
    CryNumberPhi=theTouchable->GetReplicaNumber(2);
    CryNumberTheta=theTouchable->GetReplicaNumber(1);
  }
  else if(physVol->GetName().contains("logicalCrystal"))  //barrel gdml
  {
    PartId=1;
    std::istringstream thetaBuf((theTouchable->GetVolume(1)->GetName()).substr(16,2));
    thetaBuf  >> CryNumberTheta ;
    CryNumberPhi = 308-theTouchable->GetCopyNumber(2);
  }
  else if(physVol->GetName().contains("physicalEndCrystal"))  //endcap code
  {
      PartId=theTouchable->GetReplicaNumber(3);
      G4int endSector=theTouchable->GetReplicaNumber(2);
      G4int endNb=theTouchable->GetReplicaNumber(0);
      ComputeThetaPhi(PartId,endSector,ComputeEndCopyNb(endNb));
  }
  else if(physVol->GetName().contains("logicalEndCrystal"))   //endcap gdml
  {
    PartId=2-2*theTouchable->GetCopyNumber(3);
    G4int endSector=theTouchable->GetCopyNumber(2);
    G4int endNb,endNbGDML;
    std::istringstream thetaBuf((theTouchable->GetVolume(0)->GetName()).substr(20,2));
    thetaBuf  >> endNb ;
    endNbGDML=ComputeEndCopyNb(endNb);
    G4int endSectorGDML=EndPhiNumberForGDML(endSector);
    ComputeThetaPhi(PartId,endSectorGDML,endNbGDML);
  }
  else if(physVol->GetName().contains("physicalPD")) //barrel PD code
  {
    edep*=50.;
    PartId=1;
    CryNumberPhi=theTouchable->GetReplicaNumber(2);
    CryNumberTheta=theTouchable->GetReplicaNumber(1);
  }
  else if(physVol->GetName().contains("logicalPD")) //barrel PD gdml
  {
    edep*=50.;
    PartId=1;
    G4int nb=theTouchable->GetCopyNumber(1);
    if(nb==216) {
      CryNumberTheta=0;
    } else {
      CryNumberTheta = 43-(nb-2)/5;
    }
    CryNumberPhi = 308-theTouchable->GetCopyNumber(2);
  }
  
  if (verboseLevel>1)
    G4cout << "(Check ID)New EMC Hit on crystal: "
	   << CryNumberPhi << "(phi)"
	   << CryNumberTheta << "(theta)"
	   << " and the volume is " << physVol->GetName()
	   << G4endl;

  //-----------------------------------------------------  
  G4int trackId = aStep->GetTrack()->GetTrackID();
  
  BesEmcHit* calHit = new BesEmcHit();
  calHit->SetTrackIndex(-99);
  calHit->SetG4Index(trackId);
  calHit->SetEdepCrystal(edep);
  calHit->SetTrakCrystal(stepl);
  calHit->SetPosCrystal(aStep->GetPreStepPoint()->GetPosition());
  calHit->SetTimeCrystal(aStep->GetPreStepPoint()->GetGlobalTime());
  calHit->SetMomentum(aStep->GetPreStepPoint()->GetMomentum());
  calHit->SetNumCrystal(PartId,CryNumberTheta,CryNumberPhi);
  //calHit->Print(1);
  if(edep>0&&nHit<20000)
    HitID[nHit]=CalCollection->insert(calHit)-1;
  else 
  {
    delete calHit;
    //if(nHit==20000)
      //G4cout << "Hits number exceed store space!!!" << G4endl;
  }
  nHit++;
  
  //for MC Truth
  if(CalList)
  {
    G4int trackIndex, g4TrackId;
    GetCurrentTrackIndex(trackIndex, g4TrackId);
    calHit->SetTrackIndex(trackIndex);
    
    if(m_trackIndex != trackIndex)    //find a new track
    {
      m_trackIndex = trackIndex;
      
      G4int flag=1;
      G4int pdg = abs(aStep->GetTrack()->GetDefinition()->GetPDGEncoding());
      if(pdg==12 || pdg==14 || pdg==16) {   //neutrino
        flag=0;
      }
      
      if(flag && aStep->GetTrack()->GetTrackID()==g4TrackId )   //is primary particle
      {
        BesEmcHit* truHit = new BesEmcHit();
        truHit->SetTrackIndex(trackIndex);
        truHit->SetG4Index(trackId);
        truHit->SetEdepCrystal(edep);
        truHit->SetTrakCrystal(stepl);
        truHit->SetPosCrystal(aStep->GetPreStepPoint()->GetPosition());
        truHit->SetTimeCrystal(aStep->GetPreStepPoint()->GetGlobalTime());
        truHit->SetMomentum(aStep->GetPreStepPoint()->GetMomentum());
        truHit->SetNumCrystal(PartId,CryNumberTheta,CryNumberPhi);
        CalList->insert(truHit);
      }
    }
    
    else if(m_trackIndex == trackIndex)
    {
      G4int length = CalList->entries();
      if(length>=1)
      {
        for(G4int i=0;i<length;i++)
        {
          BesEmcHit* oldHit =(*CalList)[i];
          if(oldHit->GetTrackIndex()==trackIndex)   //find the same trackIndex in CalList
          {
            oldHit->SetEdepCrystal(oldHit->GetEdepCrystal() +edep);
            break;
          }
        }
      }
    }
    
  }

  //for full Mc Truth
  if(CalTruthList)
  {
    G4int trackIndex, g4TrackId;
    GetCurrentTrackIndex(trackIndex, g4TrackId);
    Identifier id = EmcID::crystal_id(PartId,CryNumberTheta,CryNumberPhi);

    G4int flag=1;
    if(CalTruthList->entries()>0) {
      for(G4int i=0;i<CalTruthList->entries();i++) {
        BesEmcTruthHit* oldHit=(*CalTruthList)[i];
        if(oldHit->GetTrackIndex()==trackIndex) {  //find the same trackIndex in CalList
          flag=0;
          oldHit->SetEDep(oldHit->GetEDep()+edep);  //total energy
          if(oldHit->Find(id)!=oldHit->End()) {
            oldHit->AddEHit(id,edep);
          } else {
            oldHit->Insert(id,edep);
          }
          break;
        }
      }
    }

    if(flag==1) {   //new track
      G4int pdg = abs(aStep->GetTrack()->GetDefinition()->GetPDGEncoding());
      if(!(pdg==12 || pdg==14 || pdg==16)) {   //not neutrino
        BesEmcTruthHit* truHit=new BesEmcTruthHit;
        truHit->SetTrackIndex(trackIndex);
        truHit->SetG4TrackId(g4TrackId);
        truHit->SetEDep(edep);
        truHit->SetIdentify(id);
        truHit->Insert(id,edep);

        if(aStep->GetTrack()->GetTrackID()==g4TrackId) {    //is primary particle
          truHit->SetHitEmc(1);
          truHit->SetPDGCode(aStep->GetTrack()->GetDefinition()->GetPDGEncoding());
          truHit->SetPDGCharge(aStep->GetTrack()->GetDefinition()->GetPDGCharge());
          truHit->SetParticleName(aStep->GetTrack()->GetDefinition()->GetParticleName());
          truHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
          truHit->SetPosition(aStep->GetPreStepPoint()->GetPosition());
          truHit->SetMomentum(aStep->GetPreStepPoint()->GetMomentum());
          
        } else {      // the primary particle doesn't hit emc
          
          BesSensitiveManager *sensitiveManager =  BesSensitiveManager::GetSensitiveManager();
          std::vector<BesTruthTrack*> *trackList = sensitiveManager->GetTrackList();
          
          for(unsigned i=0;i<trackList->size();i++) {
            BesTruthTrack *truthTrack=(*trackList)[i];
            
            if(trackIndex==truthTrack->GetIndex()) {      //find primary particle
              truHit->SetHitEmc(0);
              truHit->SetPDGCode(truthTrack->GetPDGCode());
              truHit->SetPDGCharge(truthTrack->GetPDGCharge());
              truHit->SetParticleName(truthTrack->GetParticleName());
              
              if(truthTrack->GetTerminalVertex()) {
                truHit->SetTime(truthTrack->GetTerminalVertex()->GetTime());
                truHit->SetPosition(truthTrack->GetTerminalVertex()->GetPosition());
              } else {    //have not found terminal vertex 
                truHit->SetTime(-99);
                truHit->SetPosition(G4ThreeVector(-99,-99,-99));
              }
              
              break;
            }   //end if find primary particle
            
          }
        }

        CalTruthList->insert(truHit);
      }
    }
  }
        
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcSD::ComputeThetaPhi(G4int id, G4int sector, G4int nb)
{
  if((sector>=0)&&(sector<3))
    sector+=16;
  //if((sector!=7)&&(sector!=15))
  {
    if((nb>=0)&&(nb<4))
    {
      CryNumberTheta = 0;
      CryNumberPhi = (sector-3)*4+nb;
    }
    else if((nb>=4)&&(nb<8))
    {
      CryNumberTheta = 1;
      CryNumberPhi = (sector-3)*4+nb-4;
    }
    else if((nb>=8)&&(nb<13))
    {
      CryNumberTheta = 2;
      CryNumberPhi = (sector-3)*5+nb-8;
    }
    else if((nb>=13)&&(nb<18))
    {
      CryNumberTheta = 3;
      CryNumberPhi = (sector-3)*5+nb-13;
    }
    else if((nb>=18)&&(nb<24))
    {
      CryNumberTheta = 4;
      CryNumberPhi = (sector-3)*6+nb-18;
    }
    else if((nb>=24)&&(nb<30))
    {
      CryNumberTheta = 5;
      CryNumberPhi = (sector-3)*6+nb-24;
    }
  }
  /*else// if((sector=7)||(sector==15))
  {
    if((nb>=0)&&(nb<4))
    {
      CryNumberTheta = 0;
      CryNumberPhi = (sector-3)*4+3-nb;
    }
    else if((nb>=4)&&(nb<8))
    {
      CryNumberTheta = 1;
      CryNumberPhi = (sector-3)*4+7-nb;
    }
    else if((nb>=8)&&(nb<13))
    {
      CryNumberTheta = 2;
      CryNumberPhi = (sector-3)*5+12-nb;
    }
    else if((nb>=13)&&(nb<18))
    {
      CryNumberTheta = 3;
      CryNumberPhi = (sector-3)*5+17-nb;
    }
    else if((nb>=18)&&(nb<24))
    {
      CryNumberTheta = 4;
      CryNumberPhi = (sector-3)*6+23-nb;
    }
    else if((nb>=24)&&(nb<30))
    {
      CryNumberTheta = 5;
      CryNumberPhi = (sector-3)*6+29-nb;
    }
  }*/
  
  if(id==2)
  {
    switch(CryNumberTheta)
    {
      case 0:
        if(CryNumberPhi<32)
          CryNumberPhi = 31-CryNumberPhi;
        else
          CryNumberPhi = 95-CryNumberPhi;
        break;
      case 1:
        if(CryNumberPhi<32)
          CryNumberPhi = 31-CryNumberPhi;
        else
          CryNumberPhi = 95-CryNumberPhi;
        break;
      case 2:
        if(CryNumberPhi<40)
          CryNumberPhi = 39-CryNumberPhi;
        else
          CryNumberPhi = 119-CryNumberPhi;
        break;
      case 3:
        if(CryNumberPhi<40)
          CryNumberPhi = 39-CryNumberPhi;
        else
          CryNumberPhi = 119-CryNumberPhi;
        break;
      case 4:
        if(CryNumberPhi<48)
          CryNumberPhi = 47-CryNumberPhi;
        else
          CryNumberPhi = 143-CryNumberPhi;
        break;
      case 5:
        if(CryNumberPhi<48)
          CryNumberPhi = 47-CryNumberPhi;
        else
          CryNumberPhi = 143-CryNumberPhi;
      default:
        ;
    }
  }
}

G4int BesEmcSD::EndPhiNumberForGDML(G4int copyNb)
{
  if(copyNb<0||copyNb>15) {
    G4Exception("Wrong copy number of EMC Endcap Phi Volume!");
  } 
  
  if(copyNb==0||copyNb==1) {
    return 15-copyNb*8;
  } else if(copyNb==2||copyNb==3) {
    return 30-copyNb*8;
  } else if(copyNb<=9) {
    return 17-copyNb;
  } else {
    return 15-copyNb;
  }
}

G4int BesEmcSD::ComputeEndCopyNb(G4int num)
{
  G4int copyNb;
  switch(num){
    case 30:
      copyNb = 5;
      break;
    case 31:
      copyNb = 6;
      break;
    case 32:
      copyNb = 14;
      break;
    case 33:
      copyNb = 15;
      break;
    case 34:
      copyNb = 16;
      break;
    default:
      copyNb = num;
      break;
  }
  return copyNb;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  static G4int HCID = -1;
  if(HCID<0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection(HCID,CalCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcSD::clear()
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcSD::DrawAll()
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcSD::PrintAll()
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


















