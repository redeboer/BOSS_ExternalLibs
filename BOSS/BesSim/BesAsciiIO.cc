//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description:
////Author:   Dengzy
////Created:  Mar, 2004
////Modified:
////Comment:
//
#include "BesMdcHit.hh"
#include "BesMdcDigi.hh"
#include "BesTofHit.hh"
#include "BesTofDigi.hh"
#include "BesEmcHit.hh"
#include "BesEmcDigi.hh"
#include "BesMucHit.hh"
#include "BesMucDigi.hh"
#include "BesEventAction.hh"
#include "G4RunManager.hh"
#include "BesTruthTrack.hh"
#include "BesTruthVertex.hh"
#include "BesSensitiveManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"

#include "BesAsciiIO.hh"
#include "AsciiDmp/dmplib.hh"
#include "G4DigiManager.hh"
#include <iostream>
using namespace std;

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "McTruth/DecayMode.h"

BesAsciiIO::BesAsciiIO(
    G4int mdcTruFlag,G4int mdcDigiFlag,
    G4int tofTruFlag, G4int tofDigiFlag,
    G4int emcTruFlag, G4int emcDigiFlag,
    G4int mucTruFlag, G4int mucDigiFlag,
    G4String name)
:m_mdcTruFlag(mdcTruFlag),m_mdcDigiFlag(mdcDigiFlag),
 m_tofTruFlag(tofTruFlag),m_tofDigiFlag(tofDigiFlag),
 m_emcTruFlag(emcTruFlag),m_emcDigiFlag(emcDigiFlag),
 m_mucTruFlag(mucTruFlag),m_mucDigiFlag(mucDigiFlag), 
 m_asciiFile(name)
{
  m_DigiMan = G4DigiManager::GetDMpointer(); 
}

BesAsciiIO::~BesAsciiIO()
{
}

void BesAsciiIO::SaveAsciiEvents(G4int runId, const G4Event* evt)
{
  EVENT asciiEvt;

  asciiEvt.set_initialized();
  asciiEvt.header.set_initialized();
  asciiEvt.header.eventNo=evt->GetEventID();
  asciiEvt.header.runNo= runId;
  asciiEvt.decayMode.set_initialized();
  SaveDecayMode(asciiEvt);
  
  asciiEvt.trackTruth.set_initialized();
  asciiEvt.vertexTruth.set_initialized();
  SaveTrackTruth(asciiEvt);
  SaveVertexTruth(asciiEvt);
    
  if(m_mdcTruFlag)
  {
    asciiEvt.mdcTruth.set_initialized();
    SaveMdcTruth(asciiEvt);
  }
  
  if(m_mdcDigiFlag)
  {
    asciiEvt.mdcDigi.set_initialized();
    SaveMdcDigits(asciiEvt);
  }  
    
  if(m_tofTruFlag)
  {
    asciiEvt.tofTruth.set_initialized();
    SaveTofTruth(asciiEvt);
  }
  
  if(m_tofDigiFlag)
  {
    asciiEvt.tofDigi.set_initialized();
    SaveTofDigits(asciiEvt);
  }
  
  if(m_emcTruFlag)
  {
    asciiEvt.emcTruth.set_initialized();
    SaveEmcTruth(asciiEvt);
  }
   
  if(m_emcDigiFlag)
  {
    asciiEvt.emcDigi.set_initialized();
    SaveEmcDigits(asciiEvt);
  }
  
  if(m_mucTruFlag)
  {
    asciiEvt.mucTruth.set_initialized();
    SaveMucTruth(asciiEvt);
  }   
  
  if(m_mucDigiFlag)
  {
    asciiEvt.mucDigi.set_initialized();
    SaveMucDigits(asciiEvt);
  }
  
  ofstream os;
  if(evt->GetEventID()==0)
  {
    os.open(m_asciiFile);
    FRMTVERSION version;
    version.set_initialized();
    version.major = 1;
    version.minor = 0;
    os << version;
  }
  else
    os.open(m_asciiFile,ios::out|ios::app);

  try {
      os << asciiEvt;
  }
  catch (AsciiWrongTag& ex) {
    std::cerr << "wrong tag, got " << ex.got()
              << " expected: " << ex.expected()
              << std::endl;
  } catch (AsciiDumpException& ) {
      std::cerr << "AsciiDumpException was caught!" << std::endl;
  }
  os.close();
}

void BesAsciiIO::SaveDecayMode(EVENT& asciiEvt)
{
  asciiEvt.decayMode.size=10;
  //interface to event data service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IDataProviderSvc* evtSvc;
  StatusCode sc=svcLocator->service("EventDataSvc", evtSvc);
  if (sc.isFailure())
    G4cout<<"Could not accesss EventDataSvc!"<<G4endl;

  SmartDataPtr<DecayMode> decayMode(evtSvc,"/Event/MC/DecayMode");
  if(!decayMode)
  {
    for(int i=0;i<10;i++)
      asciiEvt.decayMode.data[i]=0;
  }
  else
  {
    int dm[10]={0,0,0,0,0,0,0,0,0,0};
    decayMode->getData(dm,10);
    for(int i=0;i<10;i++)
      asciiEvt.decayMode.data[i]=dm[i];
  }
} 

void BesAsciiIO::SaveTrackTruth(EVENT& asciiEvt)
{
  
  BesSensitiveManager* sensitiveManager = BesSensitiveManager::GetSensitiveManager();
  
  vector<BesTruthTrack*>* trackList =  sensitiveManager->GetTrackList();
  
  //arrange TruthTrack in trackList in order of trackIndex
  G4int nTrack = trackList->size();
  BesTruthTrack* track;
  for(int i=0;i<nTrack-1;i++)
    for(int j=i+1;j<nTrack;j++)
      if((*trackList)[i]->GetIndex()>(*trackList)[j]->GetIndex())
      {
        track=(*trackList)[i];
        (*trackList)[i]=(*trackList)[j];
        (*trackList)[j]=track;
      }

  for(int i=0;i<nTrack;i++)
  {
    TrackTruthType trackTruth;
    track = (*(sensitiveManager->GetTrackList())) [i];
    trackTruth.trackIndex = track->GetIndex();
    if(track->GetIndex()<0)
      G4cout<<"trackIndex<0!"<<G4endl; 
    trackTruth.PDGCode = track->GetPDGCode();
    trackTruth.PDGCharge = track->GetPDGCharge();
    trackTruth.v0Index = track->GetVertex()->GetIndex();
    if( track->GetTerminalVertex() )
      trackTruth.v1Index = track->GetTerminalVertex()->GetIndex();
    else
      trackTruth.v1Index = -99;
    trackTruth.px = track->GetP4().x()/1000.;
    trackTruth.py = track->GetP4().y()/1000.;
    trackTruth.pz = track->GetP4().z()/1000.;
    trackTruth.E = track->GetP4().t()/1000.;

    G4int size = track->GetDaughterIndexes().size();
    if(size>0)
    { 
      trackTruth.minDaughterIndex = (track->GetDaughterIndexes())[0];
      trackTruth.maxDaughterIndex = (track->GetDaughterIndexes())[size-1];
    }
    else 
    {
      trackTruth.minDaughterIndex = -99;
      trackTruth.maxDaughterIndex = -99;
    }
      
    
    asciiEvt.trackTruth.truthCol.push_back(trackTruth);
  }
    
  asciiEvt.trackTruth.nTruth=asciiEvt.trackTruth.truthCol.size();
}

void BesAsciiIO::SaveVertexTruth(EVENT& asciiEvt)
{
  BesSensitiveManager* sensitiveManager = BesSensitiveManager::GetSensitiveManager();

  vector<BesTruthVertex*>* vertexList =  sensitiveManager->GetVertexList();
  
  G4int nVertex = vertexList->size();
  BesTruthVertex* vertex;
  for(int i=0;i<nVertex;i++)
  {
    VertexTruthType vertexTruth;
    vertex = (*vertexList) [i];
    vertexTruth.vertexIndex = vertex->GetIndex();
    if(vertex->GetParentTrack())
      vertexTruth.parentTrackIndex = vertex->GetParentTrack()->GetIndex();
    else
      vertexTruth.parentTrackIndex = -99;

    vertexTruth.posX = vertex->GetPosition().x()/10.;
    vertexTruth.posY = vertex->GetPosition().y()/10.;
    vertexTruth.posZ = vertex->GetPosition().z()/10.;
    vertexTruth.time = vertex->GetTime();
    asciiEvt.vertexTruth.truthCol.push_back(vertexTruth);
  }
  asciiEvt.vertexTruth.nTruth = asciiEvt.vertexTruth.truthCol.size();
}

void BesAsciiIO::SaveMdcTruth(EVENT& asciiEvt)
{
  G4int HCID = -1;
  HCID = m_DigiMan->GetHitsCollectionID("BesMdcTruthCollection");
  if(HCID>0)
  {
    BesMdcHitsCollection* HC = 0;
    HC = (BesMdcHitsCollection*) (m_DigiMan->GetHitsCollection(HCID));
    G4int n_hit = HC->entries();
    if(n_hit>0)
    {
      //arrange hits in hits collection in order of trackIndex
      BesMdcHit* hit;
      vector<BesMdcHit*>* vecHC = HC->GetVector();
      for(int i=0;i<n_hit-1;i++)
       for(int j=i+1;j<n_hit;j++)
         if((*vecHC)[i]->GetTrackID()>(*vecHC)[j]->GetTrackID())
         {
           hit = (*vecHC)[i];
           (*vecHC)[i] = (*vecHC)[j];
           (*vecHC)[j] = hit;
         }

      for(G4int i=0;i<n_hit;i++)
      {
        hit = (*HC)[i];
        MdcTruthType  mdcTruth;
        mdcTruth.trackIndex = hit->GetTrackID();
        mdcTruth.layerNo    = hit->GetLayerNo();
        mdcTruth.cellNo     = hit->GetCellNo();
        mdcTruth.edep       = hit->GetEdep();
        mdcTruth.driftD     = hit->GetDriftD();
        mdcTruth.posX       = hit->GetPos().x();
        mdcTruth.posY       = hit->GetPos().y();
        mdcTruth.posZ       = hit->GetPos().z();
        mdcTruth.posFlag    = hit->GetPosFlag();

        asciiEvt.mdcTruth.truthCol.push_back(mdcTruth);
      }
    }
  }
  asciiEvt.mdcTruth.nTruth = asciiEvt.mdcTruth.truthCol.size();
}

void BesAsciiIO::SaveTofTruth(EVENT& asciiEvt)
{
  G4int HCID = -1;
  HCID = m_DigiMan->GetHitsCollectionID("BesTofHitsList");
  if(HCID>0)
  {
    BesTofHitsCollection* HC = 0;
    HC = (BesTofHitsCollection*) (m_DigiMan->GetHitsCollection(HCID));
    G4int n_hit = HC->entries();
    if(n_hit>0)
    {
      //arrange hits in hits collection in order of trackIndex
      BesTofHit* hit;
      vector<BesTofHit*>* vecHC = HC->GetVector();
      for(int i=0;i<n_hit-1;i++)
       for(int j=i+1;j<n_hit;j++)
         if((*vecHC)[i]->GetTrackIndex()>(*vecHC)[j]->GetTrackIndex())
         {
           hit = (*vecHC)[i];
           (*vecHC)[i] = (*vecHC)[j];
           (*vecHC)[j] = hit;
         }

      for(G4int i=0;i<n_hit;i++)
      {
        hit = (*HC)[i];
        TofTruthType  tofTruth;
        tofTruth.trackIndex = hit->GetTrackIndex();
        tofTruth.partId = hit->GetPartId();
        tofTruth.scinNb = hit->GetScinNb();
        tofTruth.posX = hit->GetPos().x();
        tofTruth.posY = hit->GetPos().y();
        tofTruth.posZ = hit->GetPos().z();
        tofTruth.px = hit->GetMomentum().x();
        tofTruth.py = hit->GetMomentum().y();
        tofTruth.pz = hit->GetMomentum().z();
        tofTruth.trackL = hit->GetTrackL();
        tofTruth.time = hit->GetTime();
        
        asciiEvt.tofTruth.truthCol.push_back(tofTruth);
      }
    }
  }
  asciiEvt.tofTruth.nTruth=asciiEvt.tofTruth.truthCol.size();  
}

void BesAsciiIO::SaveEmcTruth(EVENT& asciiEvt)
{
  G4int HCID = -1;
  HCID = m_DigiMan->GetHitsCollectionID("BesEmcHitsList");
  if(HCID>0)
  {
    BesEmcHitsCollection* HC = 0;
    HC = (BesEmcHitsCollection*) (m_DigiMan->GetHitsCollection(HCID));
    G4int n_hit = HC->entries();
    if(n_hit>0)
    {
      //arrange hits in hits collection in order of trackIndex
      BesEmcHit* hit;
      vector<BesEmcHit*>* vecHC = HC->GetVector();
      for(int i=0;i<n_hit-1;i++)
        for(int j=i+1;j<n_hit;j++)
          if((*vecHC)[i]->GetTrackIndex()>(*vecHC)[j]->GetTrackIndex())
          {
            hit = (*vecHC)[i];
            (*vecHC)[i] = (*vecHC)[j];
            (*vecHC)[j] = hit;
          }
      
      for(G4int i=0;i<n_hit;i++)
      {
        hit = (*HC)[i];
        EmcTruthType emcTruth;
        emcTruth.trackIndex = hit->GetTrackIndex();
        emcTruth.partId = hit->GetPartId();
        emcTruth.numTheta = hit->GetNumThetaCrystal();
        emcTruth.numPhi = hit->GetNumPhiCrystal();
        emcTruth.posX = hit->GetPosCrystal().x();
        emcTruth.posY = hit->GetPosCrystal().y();
        emcTruth.posZ = hit->GetPosCrystal().z();
        emcTruth.px = hit->GetMomentum().x();
        emcTruth.py = hit->GetMomentum().y();
        emcTruth.pz = hit->GetMomentum().z();
        emcTruth.totalEdep = hit->GetEdepCrystal();

        asciiEvt.emcTruth.truthCol.push_back(emcTruth);
      }
    }
  }
  asciiEvt.emcTruth.nTruth=asciiEvt.emcTruth.truthCol.size();
}

void BesAsciiIO::SaveMucTruth(EVENT& asciiEvt)
{
  G4int HCID = -1;
  HCID = m_DigiMan->GetHitsCollectionID("BesMucHitsList");
  if(HCID>0)
  {
    BesMucHitsCollection* HC = 0;
    HC = (BesMucHitsCollection*) (m_DigiMan->GetHitsCollection(HCID));
    G4int n_hit = HC->entries();
    if(n_hit>0)
    {
      //arrange hits in hits collection in order of trackIndex
      BesMucHit* hit;
      vector<BesMucHit*>* vecHC = HC->GetVector();
      for(int i=0;i<n_hit-1;i++)
        for(int j=i+1;j<n_hit;j++)
          if((*vecHC)[i]->GetTrackIndex()>(*vecHC)[j]->GetTrackIndex())
          {
            hit = (*vecHC)[i];
            (*vecHC)[i] = (*vecHC)[j];
            (*vecHC)[j] = hit;
          }

      for(G4int i=0;i<n_hit;i++)
      {
        hit = (*HC)[i];
        MucTruthType mucTruth;
        mucTruth.trackIndex = hit->GetTrackIndex();
        mucTruth.partId = hit->GetPart();
        mucTruth.segId = hit->GetSeg();
        mucTruth.gapId = hit->GetGap();
        mucTruth.stripId = hit->GetStrip();
        mucTruth.posX = hit->GetPos().x();
        mucTruth.posY = hit->GetPos().y();
        mucTruth.posZ = hit->GetPos().z();
        mucTruth.px = hit->GetMomentum().x();
        mucTruth.py = hit->GetMomentum().y();
        mucTruth.pz = hit->GetMomentum().z();
        asciiEvt.mucTruth.truthCol.push_back(mucTruth);
      }
    }
  } 
  asciiEvt.mucTruth.nTruth=asciiEvt.mucTruth.truthCol.size();
}

void BesAsciiIO::SaveMdcDigits(EVENT& asciiEvt)
{
  G4int mdcDigiCollID = -1;
  mdcDigiCollID = m_DigiMan->GetDigiCollectionID("BesMdcDigisCollection");
  if(mdcDigiCollID>=0){
    BesMdcDigisCollection* mdcDC = (BesMdcDigisCollection*)m_DigiMan->GetDigiCollection(mdcDigiCollID);
    G4int nDigi = mdcDC->entries();
    if(nDigi>0)
    {
      BesMdcDigi* mdcDigi;
      for(int i=0;i<nDigi;i++)
      {
        MdcDigiType mdcData;
        mdcDigi=(*mdcDC)[i];
        mdcData.trackIndex    = mdcDigi->GetTrackID();
        mdcData.layerNo       = mdcDigi->GetLayerNo();
        mdcData.cellNo        = mdcDigi->GetCellNo();
        mdcData.energyDeposit = mdcDigi->GetEdep();
        mdcData.driftTime     = mdcDigi->GetDriftT();
        asciiEvt.mdcDigi.digiCol.push_back(mdcData);
      }
    }
  }
  asciiEvt.mdcDigi.nDigi=asciiEvt.mdcDigi.digiCol.size();
}


void BesAsciiIO::SaveTofDigits(EVENT& asciiEvt)
{
  
  G4int tofDigiCollID = -1;
  tofDigiCollID = m_DigiMan->GetDigiCollectionID("BesTofDigitsCollection");
  
  if(tofDigiCollID>=0)
  {
    BesTofDigitsCollection* tofDC = (BesTofDigitsCollection*)m_DigiMan->GetDigiCollection(tofDigiCollID);
    G4int nDigi = tofDC->entries();
    if(nDigi>0)
    {
      //arrange digis in digitsCollection in order of trackIndex
      BesTofDigi* digi;
      vector<BesTofDigi*>* vecDC = tofDC->GetVector();
      for(int i=0;i<nDigi-1;i++)
       for(int j=i+1;j<nDigi;j++)
         if((*vecDC)[i]->GetTrackIndex()>(*vecDC)[j]->GetTrackIndex())
         {
           digi = (*vecDC)[i];
           (*vecDC)[i] = (*vecDC)[j];
           (*vecDC)[j] = digi;
         }
      
      for(int i=0;i<nDigi;i++)
      {
        TofDigiType tofData;
        digi = (*tofDC)[i];
        tofData.trackIndex = digi->GetTrackIndex();
        tofData.partId = digi->GetPartId();
        tofData.scinNb = digi->GetScinNb();
        tofData.forwADC = digi->GetForwADC();
        tofData.forwTDC = digi->GetForwTDC();
        tofData.backADC = digi->GetBackADC();
        tofData.backTDC = digi->GetBackTDC();
	
        asciiEvt.tofDigi.digiCol.push_back(tofData);
      }
    }
  }
  asciiEvt.tofDigi.nDigi=asciiEvt.tofDigi.digiCol.size();
}  

void BesAsciiIO::SaveEmcDigits(EVENT& asciiEvt)
{
  G4int emcDigiCollID = -1;
  emcDigiCollID = m_DigiMan->GetDigiCollectionID("BesEmcDigitsCollection");
  if(emcDigiCollID>=0)
  {
    BesEmcDigitsCollection* emcDC = (BesEmcDigitsCollection*)m_DigiMan->GetDigiCollection(emcDigiCollID);
    G4int nDigi = emcDC->entries();
    if(nDigi>0)
    {
      //arrange digis in digitsCollection in order of trackIndex
      BesEmcDigi* digi;
      vector<BesEmcDigi*>* vecDC = emcDC->GetVector();
      for(int i=0;i<nDigi-1;i++)
       for(int j=i+1;j<nDigi;j++)
         if((*vecDC)[i]->GetTrackIndex()>(*vecDC)[j]->GetTrackIndex())
         {
           digi = (*vecDC)[i];
           (*vecDC)[i] = (*vecDC)[j];
           (*vecDC)[j] = digi;
         }
      for(int i=0;i<nDigi;i++)
      {
      	EmcDigiType emcData;
      	digi = (*emcDC)[i];
        emcData.trackIndex = digi->GetTrackIndex();
      	emcData.partId = digi->GetPartId();
        emcData.numTheta = digi->GetThetaNb();
      	emcData.numPhi = digi->GetPhiNb();
      	emcData.energyDeposit = digi->GetEnergy();
      	emcData.hitTime = (G4double)digi->GetTime();
      	asciiEvt.emcDigi.digiCol.push_back(emcData);
      }
    }
  }
  asciiEvt.emcDigi.nDigi=asciiEvt.emcDigi.digiCol.size();
}
 
void BesAsciiIO::SaveMucDigits(EVENT& asciiEvt)
{
  G4int mucDigiCollID =-1;
  mucDigiCollID = m_DigiMan->GetDigiCollectionID("BesMucDigisCollection");
  if(mucDigiCollID>=0)
  {
    BesMucDigisCollection* mucDC = (BesMucDigisCollection*)m_DigiMan->GetDigiCollection(mucDigiCollID);
    G4int nDigi = mucDC->entries();
    if(nDigi > 0) {
      BesMucDigi* mucDigi;
      for(int i = 0; i < nDigi; i++)
      {
        MucDigiType mucData;
        mucDigi = (*mucDC)[i];
        mucData.trackIndex = mucDigi->GetTrackIndex();
        mucData.partNo  = mucDigi->GetPartId();
        mucData.segNo   = mucDigi->GetSegId();
        mucData.gapNo   = mucDigi->GetGapId();
        mucData.stripNo = mucDigi->GetStripId();
      
        asciiEvt.mucDigi.digiCol.push_back(mucData);
      }
    }
  }
  asciiEvt.mucDigi.nDigi=asciiEvt.mucDigi.digiCol.size();
}  

//Below used when output hits not digis
void BesAsciiIO::SaveHitAsciiEvents(G4int runId, const G4Event* evt){
  HitEVENT asciiEvt;

  asciiEvt.set_initialized();
  asciiEvt.header.set_initialized();
  asciiEvt.header.eventNo=evt->GetEventID();
  asciiEvt.header.runNo= runId;
  asciiEvt.decayMode.set_initialized();
  SaveDecayMode(asciiEvt);

  asciiEvt.trackTruth.set_initialized();
  asciiEvt.vertexTruth.set_initialized();
  SaveTrackTruth(asciiEvt);
  SaveVertexTruth(asciiEvt);

  if(m_mdcTruFlag)
  {
    asciiEvt.mdcTruth.set_initialized();
    SaveMdcTruth(asciiEvt);
  }

  if(m_mdcDigiFlag)
  {
    asciiEvt.mdcHit.set_initialized();
    SaveMdcHits(asciiEvt);
  }  
  /*
  if(m_tofTruFlag)
  {
    asciiEvt.tofTruth.set_initialized();
    SaveTofTruth(asciiEvt);
  }

  if(m_tofDigiFlag)
  {
    asciiEvt.tofHit.set_initialized();
    SaveTofHits(asciiEvt);
  }

  if(m_emcTruFlag)
  {
    asciiEvt.emcTruth.set_initialized();
    SaveEmcTruth(asciiEvt);
  }

  if(m_emcDigiFlag)
  {
    asciiEvt.emcHit.set_initialized();
    SaveEmcHits(asciiEvt);
  }

  if(m_mucTruFlag)
  {
    asciiEvt.mucTruth.set_initialized();
    SaveMucTruth(asciiEvt);
  }   

  if(m_mucDigiFlag)
  {
    asciiEvt.mucHit.set_initialized();
    SaveMucHits(asciiEvt);
  }
  */
  ofstream os;
  if(evt->GetEventID()==0){
    os.open(m_asciiFile);
    FRMTVERSION  version;
    version.set_initialized();
    version.major = 1;
    version.minor = 0;
    os << version;
  }
  else
    os.open(m_asciiFile,ios::out|ios::app);

  try {
      os << asciiEvt;
  }catch (AsciiWrongTag& ex) {
    std::cerr << "wrong tag, got " << ex.got()
              << " expected: " << ex.expected()
              << std::endl;
  } catch (AsciiDumpException& ) {
      std::cerr << "AsciiDumpException was caught!" << std::endl;
  }

  os.close();
}

void BesAsciiIO::SaveDecayMode(HitEVENT& asciiEvt)
{
  asciiEvt.decayMode.size=10;
  //interface to event data service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IDataProviderSvc* evtSvc;
  StatusCode sc=svcLocator->service("EventDataSvc", evtSvc);
  if (sc.isFailure())
    G4cout<<"Could not accesss EventDataSvc!"<<G4endl;

  SmartDataPtr<DecayMode> decayMode(evtSvc,"/Event/MC/DecayMode");
  if(!decayMode)
  {
    for(int i=0;i<10;i++)
      asciiEvt.decayMode.data[i]=0;
  }
  else
  {
    int dm[10]={0,0,0,0,0,0,0,0,0,0};
    decayMode->getData(dm,10);
    for(int i=0;i<10;i++)
      asciiEvt.decayMode.data[i]=dm[i];
  }
} 

void BesAsciiIO::SaveTrackTruth(HitEVENT& asciiEvt)
{
  
  BesSensitiveManager* sensitiveManager = BesSensitiveManager::GetSensitiveManager();
  
  vector<BesTruthTrack*>* trackList =  sensitiveManager->GetTrackList();
  
  //arrange TruthTrack in trackList in order of trackIndex
  G4int nTrack = trackList->size();
  BesTruthTrack* track;
  for(int i=0;i<nTrack-1;i++)
    for(int j=i+1;j<nTrack;j++)
      if((*trackList)[i]->GetIndex()>(*trackList)[j]->GetIndex())
      {
        track=(*trackList)[i];
        (*trackList)[i]=(*trackList)[j];
        (*trackList)[j]=track;
      }

  for(int i=0;i<nTrack;i++)
  {
    TrackTruthType trackTruth;
    track = (*(sensitiveManager->GetTrackList())) [i];
    trackTruth.trackIndex = track->GetIndex();
    trackTruth.PDGCode = track->GetPDGCode();
    trackTruth.PDGCharge = track->GetPDGCharge();
    trackTruth.v0Index = track->GetVertex()->GetIndex();
    if( track->GetTerminalVertex() )
      trackTruth.v1Index = track->GetTerminalVertex()->GetIndex();
    else
      trackTruth.v1Index = -99;
    trackTruth.px = track->GetP4().x();
    trackTruth.py = track->GetP4().y();
    trackTruth.pz = track->GetP4().z();
    trackTruth.E = track->GetP4().t();

    G4int size = track->GetDaughterIndexes().size();
    if(size>0)
    { 
      trackTruth.minDaughterIndex = (track->GetDaughterIndexes())[0];
      trackTruth.maxDaughterIndex = (track->GetDaughterIndexes())[size-1];
    }
    else 
    {
      trackTruth.minDaughterIndex = -99;
      trackTruth.maxDaughterIndex = -99;
    }
      
    
    asciiEvt.trackTruth.truthCol.push_back(trackTruth);
  }
    
  asciiEvt.trackTruth.nTruth=asciiEvt.trackTruth.truthCol.size();
}

void BesAsciiIO::SaveVertexTruth(HitEVENT& asciiEvt)
{
  BesSensitiveManager* sensitiveManager = BesSensitiveManager::GetSensitiveManager();

  vector<BesTruthVertex*>* vertexList =  sensitiveManager->GetVertexList();
  
  G4int nVertex = vertexList->size();
  BesTruthVertex* vertex;
  for(int i=0;i<nVertex;i++)
  {
    VertexTruthType vertexTruth;
    vertex = (*vertexList) [i];
    vertexTruth.vertexIndex = vertex->GetIndex();
    if(vertex->GetParentTrack())
      vertexTruth.parentTrackIndex = vertex->GetParentTrack()->GetIndex();
    else
      vertexTruth.parentTrackIndex = -99;

    vertexTruth.posX = vertex->GetPosition().x();
    vertexTruth.posY = vertex->GetPosition().y();
    vertexTruth.posZ = vertex->GetPosition().z();
    vertexTruth.time = vertex->GetTime();
    asciiEvt.vertexTruth.truthCol.push_back(vertexTruth);
  }
  asciiEvt.vertexTruth.nTruth = asciiEvt.vertexTruth.truthCol.size();
}

void BesAsciiIO::SaveMdcTruth(HitEVENT& asciiEvt)
{
  G4int HCID = -1;
  HCID = m_DigiMan->GetHitsCollectionID("BesMdcTruthCollection");
  if(HCID>0)
  {
    BesMdcHitsCollection* HC = 0;
    HC = (BesMdcHitsCollection*) (m_DigiMan->GetHitsCollection(HCID));
    G4int n_hit = HC->entries();
    if(n_hit>0)
    {
      //arrange hits in hits collection in order of trackIndex
      BesMdcHit* hit;
      vector<BesMdcHit*>* vecHC = HC->GetVector();
      for(int i=0;i<n_hit-1;i++)
       for(int j=i+1;j<n_hit;j++)
         if((*vecHC)[i]->GetTrackID()>(*vecHC)[j]->GetTrackID())
         {
           hit = (*vecHC)[i];
           (*vecHC)[i] = (*vecHC)[j];
           (*vecHC)[j] = hit;
         }

      for(G4int i=0;i<n_hit;i++)
      {
        hit = (*HC)[i];
        MdcTruthType  mdcTruth;
        mdcTruth.trackIndex = hit->GetTrackID();
        mdcTruth.layerNo    = hit->GetLayerNo();
        mdcTruth.cellNo     = hit->GetCellNo();
        mdcTruth.edep       = hit->GetEdep();
        mdcTruth.driftD     = hit->GetDriftD();
        mdcTruth.posX       = hit->GetPos().x();
        mdcTruth.posY       = hit->GetPos().y();
        mdcTruth.posZ       = hit->GetPos().z();
        mdcTruth.posFlag    = hit->GetPosFlag();

        asciiEvt.mdcTruth.truthCol.push_back(mdcTruth);
      }
    }
  }
  asciiEvt.mdcTruth.nTruth = asciiEvt.mdcTruth.truthCol.size();
}

void BesAsciiIO::SaveMdcHits(HitEVENT& asciiEvt)
{
  G4int mdcHitCollID = -1;
  mdcHitCollID = m_DigiMan->GetHitsCollectionID("BesMdcHitsCollection");
  if(mdcHitCollID>=0)
  {
    BesMdcHitsCollection* mdcDC = (BesMdcHitsCollection*)m_DigiMan->GetHitsCollection(mdcHitCollID);
    G4int nHit = mdcDC->entries();
    if(nHit>0)
    {
      BesMdcHit* mdcHit;
      for(int i=0;i<nHit;i++)
      {
        MdcHitType mdcData;
        mdcHit=(*mdcDC)[i];
        mdcData.trackIndex    = mdcHit->GetTrackID();
        mdcData.layerNo       = mdcHit->GetLayerNo();
        mdcData.cellNo        = mdcHit->GetCellNo();
        mdcData.posX          = mdcHit->GetPos().x();
        mdcData.posY          = mdcHit->GetPos().y();
        mdcData.posZ          = mdcHit->GetPos().z();
        mdcData.energyDeposit = mdcHit->GetEdep();
        mdcData.driftDistance = mdcHit->GetDriftD();
        mdcData.globalT       = mdcHit->GetGlobalT();
        mdcData.theta         = mdcHit->GetTheta();
        mdcData.enterAngle    = mdcHit->GetEnterAngle();
        mdcData.posFlag       = mdcHit->GetPosFlag();

        asciiEvt.mdcHit.hitCol.push_back(mdcData);
      }
    }
  }
  asciiEvt.mdcHit.nHit=asciiEvt.mdcHit.hitCol.size();
}
 
