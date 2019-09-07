//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author:  Youzy      Peking University      mail: youzy@hep.pku.cn
//Created: Nov, 2003
//Modified:
//Comment:
//  2006.10.17 update to new gdml, structure changed
//  new gdml structure: gap<---aluminumbox<---gaschamber  
//  but still use the former simulation structure  
//---------------------------------------------------------------------------//

//
// $Id: BesMucSD.cc,v 1.13 2009/08/25 13:33:54 xieyg Exp $
// GEANT4 tag $Name: MucSim-00-01-03 $
//

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "G4Svc/G4Svc.h"

#include "BesMucSD.hh"
#include "BesMucDigit.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "BesMucEfficiency.hh"
#include "BesMucNoise.hh"
#include "ReadBoostRoot.hh"
#include "Randomize.hh"
#include "strstream"
#include "G4Box.hh"
#include "stdlib.h"

  BesMucSD::BesMucSD(G4String name,BesMucConstruction* Det)
:BesSensitiveDetector(name),detector(Det)
{
  collectionName.insert("BesMucHitsCollection");
  collectionName.insert("BesMucHitsList");
  HitID = new G4int[500];

  //liangyt 2006.3.1 initialize muc-efficiency;
  G4String GeometryPath = ReadBoostRoot::GetBoostRoot(); 
  G4String GeometryPath2 = GeometryPath;
  if(!GeometryPath){ 
    G4Exception("BOOST environment not set!");
  }
  GeometryPath += "/dat/muc-effi.dat";
  GeometryPath2 += "/dat/muc-noise.dat";

  m_effi=BesMucEfficiency::Instance();
  //  m_effi->Initialize(GeometryPath);  
  
  //retrieve G4Svc
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IG4Svc* iG4Svc;
  StatusCode sc=svcLocator->service("G4Svc", iG4Svc);
  m_G4Svc = dynamic_cast<G4Svc *>(iG4Svc);
  m_noiseMode = m_G4Svc->MucNoiseMode();  
  G4cout << "MucNoiseMode:\t"<<m_noiseMode<<G4endl;
  
  if( m_noiseMode != 0 ) 
  {
    m_noise=BesMucNoise::Instance();
    G4LogicalVolume* logicalMuc = detector->GetPhysicalMuc()->GetLogicalVolume();
    m_noise->Initialize(GeometryPath2,logicalMuc);
    //m_noise->Initialize(GeometryPath2,logicalMuc,"ROOT");
  }

  m_PreviousPrimaryTrackG4Id = 0;
  m_CurEvent = 0;
  m_TrackCon = 0;

}

BesMucSD::~BesMucSD(){ 
  delete [] HitID;
}

void BesMucSD::Initialize(G4HCofThisEvent*)
{
  MucHitCollection = new BesMucHitsCollection(SensitiveDetectorName, collectionName[0]); 
  // for (G4int j=0;j<detector->GetBesMucNbOfTraps();j++) {HitID[j] = -1;};  
  TotEneDeposit = 0;
  // G4cout<<"----------in SD::Init()---  "<<detector->GetPhysicalMuc()->GetLogicalVolume()->GetName()<<G4endl;  
}

void BesMucSD::BeginOfTruthEvent(const G4Event* )
{
  MucHitList = new BesMucHitsCollection(SensitiveDetectorName, collectionName[1]);
  m_trackIndex = -99;
  m_trackIndexes.clear();
  m_prePart  = -99;
  m_preSeg   = -99;
  m_preGap   = -99;
  m_preStrip = -99;
  //G4cout<<"---in BesMucSD::BeginOfTruthEvent()---"<<MucHitCollection->entries()<<" "<<MucHitList->entries()<<G4endl;
  
  //add oise
  if(m_noiseMode != 0) m_noise->AddNoise(m_noiseMode, MucHitCollection, MucHitList);  //commen out 2006.10.21
}

void BesMucSD::EndOfTruthEvent(const G4Event* evt)
{
  static G4int HLID=-1;
  if(HLID<0)
    HLID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[1]);
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  HCE->AddHitsCollection(HLID,MucHitList);
}

G4bool BesMucSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  G4Track *curTrack = aStep->GetTrack();

  m_CurEvent = G4EventManager::GetEventManager()->GetConstCurrentEvent();
  //  if(m_CurEvent->GetEventID()<19565)return true;  // ---del
  if (m_CurEvent) {
    m_TrackCon = m_CurEvent->GetTrajectoryContainer();

    //  if (!m_TrackCon) G4cout << "m_TrackCon does not exist " << G4endl;
    //  else {
    //  G4cout << "m_TrackCon size " << m_TrackCon->size() << G4endl;
    //  for (int i = 0; i < (int)m_TrackCon->size(); i++) {
    //  G4cout << "track " << i << " g4TrackId " << (*m_TrackCon)[i]->GetTrackID() << " parent ID " << (*m_TrackCon)[i]->GetParentID() << G4endl;
    //  }
    //  }
  }


  if (curTrack->GetDefinition()->GetPDGCharge() == 0.) return false;

  G4double edep = aStep->GetTotalEnergyDeposit();
  //if(edep == 0.) return false;

  //TotEneDeposit+=edep;
  G4int trackIndex = -99, g4TrackId = -99;
  GetCurrentTrackIndex(trackIndex, g4TrackId);

  G4TouchableHistory* theTouchable
    = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());

  BesMucHit *newHit = new BesMucHit();

  G4int trackID = curTrack->GetTrackID(); //G4 track ID of current track.
  G4int parentID = curTrack->GetParentID(); //G4 track ID of parent track.
  newHit->SetTrackID(trackID);
  newHit->SetTrackIndex(trackIndex); // MC truth track index

  G4int pdg = curTrack->GetDefinition()->GetPDGEncoding();
  newHit->SetPDGCode(pdg);

  newHit->SetEdep(edep);

  G4ThreeVector pos = 0.5*( aStep->GetPostStepPoint()->GetPosition()
      + aStep->GetPreStepPoint()->GetPosition() );
  newHit->SetPos(pos);

  G4ThreeVector posInGas = theTouchable->GetHistory()->GetTopTransform().TransformPoint(pos);
  G4int       stackDepth = theTouchable->GetHistory()->GetDepth();
  G4ThreeVector posInBox = theTouchable->GetHistory()->GetTransform(stackDepth-1).TransformPoint(pos);
  newHit->SetPosLocal(posInBox);

  G4ThreeVector posInGap = theTouchable->GetHistory()->GetTransform(stackDepth-2).TransformPoint(pos);
  // cout<<"  pos  "<<pos.x()<<" "<<pos.y()<<"  "<<pos.z()<<"depth= "<<stackDepth<<endl
  //       <<" posInBox(3)"<<posInBox.x()<<"  "<<posInBox.y()<<" "<<posInBox.z()<<endl
  //       <<" posInGap(2)"<<posInGap.x()<<"  "<<posInGap.y()<<" "<<posInGap.z()<<endl;

  G4double energy = aStep->GetPreStepPoint()->GetKineticEnergy();
  newHit->SetEnergy(energy);

  G4ThreeVector dir = aStep->GetPreStepPoint()->GetMomentumDirection();
  newHit->SetDir(dir);

  G4ThreeVector momentum = aStep->GetPreStepPoint()->GetMomentum();
  newHit->SetMomentum(momentum);

  G4double GlobalTime = aStep->GetPostStepPoint()->GetGlobalTime();
  newHit->SetTime(GlobalTime);

  G4VPhysicalVolume* vl = theTouchable->GetVolume(0);
  newHit->SetVolume(vl);

  //G4cout<<"in SD "<<newHit->GetPart()<<" "<<newHit->GetSeg()<<" "<<newHit->GetGap()<<" "<<newHit->GetGasChamber()<<" "<<newHit->GetPanel()<<endl;

  newHit->Draw();
  //newHit->Print();
  //MucHitCollection->insert(newHit);
  m_PreviousPrimaryTrackG4Id = g4TrackId;

  //-----------add by liangyt for efficiency

  //for MC Truth
  if (MucHitList) {

    // cout <<"g4TrackId " << g4TrackId << " trackID " << trackID << " parentID " << parentID << "trackIndex " << trackIndex << endl; 
    // g4TrackId:  G4 track ID of previous primary track in trackList,
    // trackID:    G4 track ID of current track.
    // parentID:   G4 track ID of parent track of current track.
    // trackIndex: track ID in MC truth

    // no longer necessary to find the track's ancestor primary track,
    // it is kept in trackIndex untill next primary track generated. So IsChildof is uncessary
    //if ( g4TrackId == trackID ) { //|| IsChildOf(curTrack, g4TrackId) ) {     // This track is the primary track & will appear in MC truth
    G4int newTrackFlag = 0;
    newHit->SetTrackIndex(trackIndex);
    if(m_trackIndex != trackIndex) {
      m_trackIndex = trackIndex;
      G4int size = m_trackIndexes.size();
      newTrackFlag = 1;
      if (size > 0) {
        for(G4int i=0;i<size;i++)
          if(m_trackIndexes[i] == trackIndex ) {
            newTrackFlag = 0; 
            break;
          }
      }
    }

    if (newTrackFlag) {
      m_trackIndexes.push_back(trackIndex);
      m_prePart  = -99;
      m_preSeg   = -99;
      m_preGap   = -99;
      m_preStrip = -99;
    }
    BesMucHit* truHit = new BesMucHit();
    *truHit = *newHit;
    if (g4TrackId != trackID) {
      //	trackIndex += 1000; // a sencondary track
      trackIndex += 0; //2006.12.22 do not indicate secondary track now
      truHit->SetTrackIndex(trackIndex);
    }
    //cout << "trackIndex " << trackIndex << endl;

    BesMucDigit aDigit;
    aDigit.SetHit(truHit);
    G4int curPart, curSeg, curGap, curStrip;
    curPart  = aDigit.GetPart();
    curSeg   = aDigit.GetSeg();
    curGap   = aDigit.GetGap();
    curStrip = aDigit.GetNearestStripNo();

    m_effi->CheckCalibSvc();
    m_effi->SetHit(truHit);
    G4double need_eff = m_effi->GetEfficiency();

    //G4cout<<"in SD effi= "<<need_eff<<endl;
    //need_eff = 1.0;  //2006.12.28
    if (curPart == m_prePart && curSeg == m_preSeg &&
        curGap  == m_preGap && curStrip == m_preStrip) {
      //cout<<MucHitList->entries()<<" "<<MucHitCollection->entries()<<" "<<need_eff<<"---if----curPart "<<curPart<<"curSeg "<<curSeg<<"curGap "<<curGap<<"curStrip "<<curStrip<<" momentum "<<momentum.x()<<" "<<momentum.y()<<" "<<momentum.z()<<" "<<endl;
      delete truHit;delete newHit;
    }
    else {
      //cout<<MucHitList->entries()<<"----else--Part "<<curPart<<" Seg "<<curSeg<<" Gap "<<curGap<<" Strip "<<curStrip<<endl;
      truHit->SetPart(curPart);
      truHit->SetSeg(curSeg);
      truHit->SetGap(curGap);
      truHit->SetStrip(curStrip);

      // if a truHit with the same id(part, seg, gap, strip) and trackIndex(%1000) exist,
      // they belong to the same primary track,(maybe one is primary, the other is secondary), dont add.
      bool truHitExist = false;
      G4int n_hit = MucHitList->entries();
      for(G4int iTru=0;iTru<n_hit;iTru++) {
        BesMucHit* aTruHit = (*MucHitList)[iTru];
        if ( aTruHit->GetTrackIndex()%1000 == truHit->GetTrackIndex()%1000 &&
            aTruHit->GetPart()  == truHit->GetPart() &&
            aTruHit->GetSeg()   == truHit->GetSeg()  &&
            aTruHit->GetGap()   == truHit->GetGap()  &&
            aTruHit->GetStrip() == truHit->GetStrip() ) 
        {
          truHitExist = true;
          break;
        }
      }
      G4float random=G4UniformRand();   //*** use other random
      // G4float random=(rand()%100000)/100000.0;
      // G4cout<<"---in SD---"<<random<<endl;
      if (random<=need_eff){ MucHitCollection->insert(newHit);}
      else delete newHit;
      if (!truHitExist&&random<=need_eff)
      { MucHitList->insert(truHit);}
      else delete truHit;
      m_prePart  = curPart;
      m_preSeg   = curSeg;
      m_preGap   = curGap;
      m_preStrip = curStrip;
    }
    //}
  }

  return true;
  }

  bool BesMucSD::IsChildOf(G4Track *curTrack, G4int primaryG4TrackID) 
  {
    G4cout << "IsChildof " << "curTrackID " <<  curTrack->GetTrackID() << G4endl;

    G4VTrajectory* aTraj = GetTrajFromID(curTrack->GetParentID());
    G4cout << "IsChildof " << "parentTrackID " <<  aTraj->GetTrackID() << G4endl;

    while ( aTraj->GetTrackID() != 0 && aTraj->GetTrackID() != primaryG4TrackID ) {
      //G4cout << "loop: parentID " << aTraj->GetParentID() << G4endl;
      aTraj = GetTrajFromID(aTraj->GetParentID());
    }

    if (aTraj->GetTrackID() == primaryG4TrackID) return true;
    else return false;
  }

  G4VTrajectory* BesMucSD::GetTrajFromID(G4int id) 
  {
    // TrajContainer does not contain current track
    G4cout << "begin of GetTrajFromID, id : " << id << G4endl;

    if (!m_TrackCon) G4cout << "Trajectory not saved? Set /tracking/storeTrajectory 1" << G4endl;

    if (m_TrackCon->size() == 0) {
      G4cout << "BesMucSD::GetTrajFromID, TrackCon size 0" << G4endl;
      return 0;
    }

    int k = 0;
    while( k < (int)m_TrackCon->size() && (*m_TrackCon)[k]->GetTrackID() != id ) {
      G4cout << "GetTrajFromID " << k << " : ID " << (*m_TrackCon)[k]->GetTrackID() << G4endl;
      k++;
      if (!(*m_TrackCon)[k]) {
        G4cout << "G4Track ID " << (*m_TrackCon)[k]->GetTrackID() << " doesnt exist in TrajContainer of this event! " << G4endl; 
        return 0;
      }
    }

    if ( k == (int)m_TrackCon->size() ) {
      G4cout << "BesMucSD::GetTrajFromID, track with ID " << id << " not found" << G4endl;
      return 0;
    }

    return (*m_TrackCon)[k];
  }

  void BesMucSD::EndOfEvent(G4HCofThisEvent* HCE)
  {
    static G4int HCID=-1;
    if(HCID<0)
    {HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
    HCE->AddHitsCollection(HCID, MucHitCollection);
  }

