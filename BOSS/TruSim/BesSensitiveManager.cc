//---------------------------------------------------------------------------//
////////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////////---------------------------------------------------------------------------//
////////Description:
// BesSensitiveManager is the magical piece of software that
// helps a class that inherits from BesSenstiveDetector
// complete all of its actions.
// In addition, BesSensitiveManager handles the BesTruthTrack,BesTruthVertex lists.

//Author : Dengzy
//Created:  Aug, 2004
//Modified:
//Comment:
//// //// $Id: BesSensitiveManager.cc


#include "BesSensitiveManager.hh"
#include "BesTruthTrack.hh"
#include "BesTruthVertex.hh"

#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4Event.hh"

#include "HepMC/GenEvent.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GeneratorObject/McGenEvent.h"
#include "GaudiKernel/SmartDataPtr.h"

BesSensitiveManager* BesSensitiveManager::m_sensitiveManager=0;

// BeginOfEvent
//
// Allocate lists, and tell clients
//
void BesSensitiveManager::BeginOfTruthEvent(const G4Event* evt)
{
  // Allocate our lists. We'll own these until EndOfEvent

  m_trackList  = new std::vector<BesTruthTrack*>;
  m_vertexList = new std::vector<BesTruthVertex*>;
  
  SetVertex0(evt);

  m_count = 0;
  SaveParticlesFromGenerator();
  m_count += m_trackList->size();

  // Tell clients
  iter=clients.begin();
  iter_end=clients.end();
  while( iter != iter_end ) 
  {
    (*iter)->BeginOfTruthEvent(evt);
    iter++;
  }
}

void BesSensitiveManager::SetVertex0(const G4Event* anEvent)
{
  //G4int n_vertex = anEvent->GetNumberOfPrimaryVertex();
  for( G4int i=0; i<1; i++ )
  {
    G4PrimaryVertex* primaryVertex = anEvent->GetPrimaryVertex(i);
    m_pos0 = primaryVertex->GetPosition();
    m_t0 = primaryVertex->GetT0();
  }
  if(m_logLevel<=4)
    G4cout<<"pos0:"<<m_pos0<<"   t0:"<<m_t0<<G4endl;
}

G4int BesSensitiveManager::CheckType(const HepMC::GenEvent* hepmcevt)
{
  G4int flag =0;
  for(HepMC::GenEvent::vertex_const_iterator vitr= hepmcevt->vertices_begin();
          vitr != hepmcevt->vertices_end(); ++vitr ) { // loop for vertex ...
    for (HepMC::GenVertex::particle_iterator
             pitr= (*vitr)->particles_begin(HepMC::children);
                pitr != (*vitr)->particles_end(HepMC::children); ++pitr) {
      if((*pitr)->end_vertex())
        {flag = 1; break;}
    }
    if(flag) break;
  }
  if(m_logLevel <= 4)
  {
    if(flag == 0)
      G4cout<<G4endl<<"generator is GENBES!"<<G4endl;
    else
      G4cout<<G4endl<<"generator is not GENBES!"<<G4endl;
  }
  return flag;
}

void BesSensitiveManager::SaveParticlesFromGenerator()
{
  IDataProviderSvc* p_evtSvc=0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc=svcLocator->service("EventDataSvc", p_evtSvc);
  if (sc.isFailure())
      std::cout<<"BesHepMCInterface could not access EventDataSvc!!"<<std::endl;

  SmartDataPtr<McGenEventCol> mcCollptr( p_evtSvc, "/Event/Gen");

  if ( mcCollptr == 0 )
    std::cout << "no McGenEventCollection found."  << std::endl;

  else 
  {
    McGenEventCol::const_iterator it = mcCollptr->begin();
    McGenEvent* mcEvent = (McGenEvent* ) (*it);
    m_hepmcevt = mcEvent->getGenEvt();
    G4int typeFlag = CheckType(m_hepmcevt); 
    

    for(HepMC::GenEvent::vertex_const_iterator vitr= m_hepmcevt->vertices_begin();
              vitr != m_hepmcevt->vertices_end(); ++vitr ) 
    {
      G4int barcodeVtx = (*vitr)->barcode();
      if(m_logLevel<=4)
        G4cout<<G4endl<<"barcodeVtx:"<<barcodeVtx<<" ";
  
      G4LorentzVector xvtx((*vitr)->position().x(),(*vitr)->position().y(),(*vitr)->position().z(),(*vitr)->position().t());
      G4ThreeVector v(xvtx.x(), xvtx.y(), xvtx.z());
      BesTruthVertex* newVertex = new BesTruthVertex();
      newVertex->SetPosition(v);
      newVertex->SetTime(xvtx.t()*mm/c_light);
      if(m_logLevel<=4)
        G4cout<<"xyzt:"<<xvtx.x()<<" "<<xvtx.y()<<" "
            <<xvtx.z()<<" "<<xvtx.t()*mm/c_light<<" ";
      G4int nTrack = m_trackList->size();
      BesTruthTrack* parentTrack=0;
      G4int parentTrackIndex= -99;
      for(int i=0;i<nTrack;i++)
      {
        parentTrack = (*m_trackList)[i];
        if(parentTrack->GetBarcodeEndVtx() == barcodeVtx)
        {
          newVertex->SetParentTrack(parentTrack);
          parentTrackIndex = parentTrack->GetIndex();
          if(m_logLevel<=4)
            G4cout<<"parentTrack:"<<parentTrackIndex<<" ";
          parentTrack->SetTerminalVertex(newVertex);
          //break;
        }
      }
  
      G4int vtxFlag=0;
      G4int pOut = (*vitr)->particles_out_size();
      HepMC::GenVertex::particle_iterator pitr = (*vitr)->particles_begin(HepMC::children);
      G4int pdgcode= (*pitr)-> pdg_id();
      if(pOut == 1 && pdgcode == -11 && typeFlag==1)
        vtxFlag=1;
          
      if(vtxFlag==0)
      {
        m_vertexList->push_back(newVertex);
        newVertex->SetIndex(m_vertexList->size()-1);
        if(m_logLevel<=4)
          G4cout<<"vindex:"<<m_vertexList->size()-1<<G4endl;
  
        for (HepMC::GenVertex::particle_iterator pitr= (*vitr)->particles_begin(HepMC::children); pitr != (*vitr)->particles_end(HepMC::children); ++pitr)
        {
          G4LorentzVector p((*pitr)->momentum().x(), (*pitr)->momentum().y(),(*pitr)->momentum().z(),(*pitr)->momentum().t());
          G4LorentzVector pGeV(p.x()*GeV,p.y()*GeV,p.z()*GeV,p.t()*GeV);
          G4int pdgcode = (*pitr)->pdg_id();
  
          BesTruthTrack* newTrack = new BesTruthTrack;
          newTrack->SetP4(pGeV);
          newTrack->SetPDGCode(pdgcode);
          if(m_logLevel<=4)
          {  
            G4cout<<"pdg:"<<pdgcode<<" ";
            G4cout<<"p:"<<p.x()<<" "<<p.y()<<" "<<p.z()<<" "<<p.t()<<" ";
            G4double mass = sqrt(p.t()*p.t()-p.x()*p.x()-p.y()*p.y()-p.z()*p.z());
            G4cout<<mass<<" ";
          }
          newTrack->SetPDGCharge(99);
          newTrack->SetParticleName("unknown");
          newTrack->SetVertex(newVertex);
          newTrack->SetTerminalVertex(0);
          newTrack->SetSource("FromGenerator");
        
          G4int barcodeEndVtx=0;
          if((*pitr)->end_vertex())
          {
            barcodeEndVtx = (*pitr)->end_vertex()->barcode();
            if(m_logLevel<=4)
              G4cout<<"endVtx:"<<barcodeEndVtx<<" ";
          }
          newTrack->SetBarcodeEndVtx(barcodeEndVtx);
  
          m_trackList->push_back(newTrack);
          newTrack->SetIndex(m_trackList->size()-1);
          if(m_logLevel<=4)
            G4cout<<"index:"<<m_trackList->size()-1<<" ";
          //here, parentTrack->GetIndex can't be used,
          //use parentTrackIndex instead
          if(parentTrackIndex>=0)
          {
            if(m_logLevel<=4)
              G4cout<<"mother:"<<parentTrackIndex; 
            (*m_trackList)[parentTrackIndex]->AddDaughterIndex(m_trackList->size()-1);
          }
          if(m_logLevel<=4)
            G4cout<<G4endl;
        }  
      }
    } 
  } 
}

// EndOfEvent
// Give lists to Event, and tell clients
void BesSensitiveManager::EndOfTruthEvent(const G4Event* evt)
{
  // Tell clients
  iter=clients.begin();
  iter_end=clients.end();
  while( iter != iter_end ) 
  {
    (*iter)->EndOfTruthEvent(evt);
    iter++;
  }
}

void BesSensitiveManager::ClearEvent()
{
  if(m_trackList)
  {
    for(size_t i=0;i<m_trackList->size();i++)
      delete (*m_trackList)[i];
    m_trackList->clear();
    delete m_trackList;
  }
  if(m_vertexList)
  {
    for(size_t i=0;i<m_vertexList->size();i++)
      delete (*m_vertexList)[i];
    m_vertexList->clear();
    delete m_vertexList;
  }
}

void BesSensitiveManager::BeginOfTrack( const G4Track *track )
{
  m_trackFlag = 0; //if m_trackFlag=1, this track is from generator

  // Append our track in the parentage history
  chain.push_back(FollowTrack(track));

  // Get TruthTrack index
  //int i = chain.size();

  /*for(;;) 
  {
    if(--i<0) G4cerr<<"::parents are corrupted"<<G4endl;
    m_trackIndex = chain[i].trackIndex;
    if (m_trackIndex != -1) break;
  }*/
  
  // Invoke clients
  //
  iter=clients.begin();
  iter_end=clients.end();
  while(iter!=iter_end) 
  {
    (*iter)->BeginOfTrack( track );
    iter++;
  }
}
void BesSensitiveManager::EndOfTrack( const G4Track *track , G4TrackingManager* trackingManager )
{
  if(chain.back().savedByDefault==true)
  { 
    BesTStats &stat = chain.back();
    G4int endVtxFlag = 0;
    if(m_trackFlag==1)
    {
      BesTruthTrack* truthTrack = (*m_trackList)[m_trackIndex];
      if(truthTrack->GetTerminalVertex())
      {
        UpdateVertex(stat,track); endVtxFlag = 1;}
    } 
    if(endVtxFlag == 0)
    {
      // Tracks saved to BesTruthTrack require additional work
      G4StepPoint *finalStep = track->GetStep()->GetPostStepPoint();
      G4StepStatus stepStatus = finalStep->GetStepStatus();
      if (track->GetNextVolume() != 0 ||
  	      (stepStatus != fGeomBoundary && stepStatus != fWorldBoundary) )
      {
        if(m_logLevel<=4)
          G4cout<<"Particle died. make a terminal vertex: ";
        // Particle died. We always make a terminal vertex
        const G4ThreeVector pos(finalStep->GetPosition());
        G4int vindex = m_vertexList->size();
        if(m_logLevel<=4)
          G4cout<<vindex<<G4endl;
        stat.vertices.push_back(vindex);
        BesTruthVertex *newVertex = new BesTruthVertex();
        newVertex->SetPosition(pos);
        newVertex->SetTime( finalStep->GetGlobalTime());
        if(m_trackFlag==1)
          newVertex->SetParentTrack( (*m_trackList)[m_trackIndex] );
        else
          newVertex->SetParentTrack( m_trackList->back() );
        newVertex->SetTerminal( true );
        newVertex->SetIndex( vindex );
        
        //set minDaughter index equal to m_count
        newVertex->SetMinDau(m_count);  
        
        //Get how many decayed daughters this track has, if no, 0 is set.
        G4TrackVector* trackVector = trackingManager->GimmeSecondaries();
        G4int nSecon = trackVector->size();
        G4Track* seconTrack;
        G4int nDau=0;
        if(nSecon>0)
        {
          for(G4int i=0;i<nSecon;i++)
          {
            seconTrack = (*trackVector)[i];
            G4String processName = seconTrack->GetCreatorProcess()->GetProcessName();
            if(processName == "Decay")
              nDau++;
          }
        }
        m_vertexList->push_back( newVertex );
  
        m_count += nDau;
        
        // Give this terminal vertex to the track
        //(*m_trackList)[stat.trackIndex]->SetTerminalVertex( m_vertexList->back() );
        // for particles from generator, 
        // m_trackList->back() may not be current track.

        if(m_trackFlag==1)
          (*m_trackList)[m_trackIndex]->SetTerminalVertex( m_vertexList->back() );
        else
          m_trackList->back()->SetTerminalVertex( m_vertexList->back() );
      }
    }
  } 
  // Invoke clients
  iter=clients.begin();
  iter_end=clients.end();
  while( iter!=iter_end) {
    (*iter)->EndOfTrack( track );
    iter++;
  }
}

void BesSensitiveManager::UpdateVertex(BesTStats stat, const G4Track *track)
{
  BesTruthTrack* truthTrack = (*m_trackList)[m_trackIndex];
  G4int terminalIndex = truthTrack->GetTerminalVertex()->GetIndex();
  if(m_logLevel<=4)  
    G4cout<<"updateVertex:"<<terminalIndex<<" ";
  BesTruthVertex* vertex = (*m_vertexList)[terminalIndex];
  G4StepPoint *finalStep = track->GetStep()->GetPostStepPoint();
  const G4ThreeVector pos(finalStep->GetPosition());
  G4double time = finalStep->GetGlobalTime();
  if(m_logLevel<=4)
    G4cout<<"newPos:"<<pos<<" "<<"newTime:"<<time<<G4endl;
  vertex->SetPosition(pos);
  vertex->SetTime(time);
  vertex->SetTerminal( true );
  //G4int vindex = m_vertexList->size();
  //stat.vertices.push_back(terminalIndex);

}

void BesSensitiveManager::MakeNewTrack( BesTStats &stat, const G4Track *track)
{
  if (stat.originVertex < 0 && chain.size() > 0) 
  {
    if(m_logLevel<=4)
      G4cout<<"MakeNewTrack for decayed particles,";
    //for decayed particles, there may already be a BesTruthVertex that is suitable for this track.
    //If so, it's always the terminal vertex of its immediate parent( chain.back() ).

    G4int parentVstat = chain.back().vertices.size();
    while( --parentVstat >= 0)
    {
      G4int vindex = chain.back().vertices[parentVstat];

      G4ThreeVector diff((*m_vertexList)[vindex]->GetPosition());
      diff = diff - track->GetPosition();
      if (diff.mag2() < 1E-9) 
      {
	      stat.originVertex = vindex;
        if(m_logLevel<=4)
          G4cout<<"find a vertex:"<<vindex;
        (*m_vertexList)[vindex]->AddCurrentDau();
      	break;
      }
    }
    
  }
  
  if(stat.originVertex < 0 && chain.size() == 0)
  {
    //for primary track, check if there is already a vertex suitable for it.
    G4int nVertex = m_vertexList->size();	  
    for(G4int i=0;i<nVertex;i++)
    {
      G4ThreeVector diff((*m_vertexList)[i]->GetPosition());
      diff = diff -track->GetPosition();
      if(diff.mag2() < 1E-9)
      {
        stat.originVertex = i; 
        if(m_logLevel<=4)
          G4cout<<"MakeNewTrack for primary particles,find a vertex:"
                <<i;
       	break;
      }
    }
  }
  
  if (stat.originVertex < 0)
  {
    if(m_logLevel<=4) 
      G4cout<<"MakeNewTrack for primary particles,make new vertex:"
            <<m_vertexList->size();
    // it's a primary track(there's no vertex suitable for it)
    // Make a new BesTruthVertex
    const G4ThreeVector v(track->GetPosition());
		
    stat.originVertex = m_vertexList->size();

    BesTruthVertex *newVertex = new BesTruthVertex();
    newVertex->SetPosition(v);
    newVertex->SetTime( track->GetGlobalTime());
    
    //if (chain.size() > 0) {
    //  G4int trackIndex = -1;
    //  G4int i = chain.size();
    //  while(--i>=0 && trackIndex < 0) trackIndex = chain[i].trackIndex;
    //  newVertex->SetParentTrack( trackIndex < 0 ? 0 : (*m_trackList)[trackIndex] );
    //}
    
    newVertex->SetParentTrack( 0 );
    newVertex->SetTerminal( false );
    newVertex->SetIndex( stat.originVertex );
    if(track->GetCreatorProcess())
      newVertex->SetProcessName(track->GetCreatorProcess()->GetProcessName());
    else
      newVertex->SetProcessName("generator");    
    
    m_vertexList->push_back( newVertex );
  }
	
  // Now, finally make our new BesTruthTrack
  // We'll leave the assignment of terminalVertex until
  // we know what happens to this track		
  BesTruthTrack *newTrack = new BesTruthTrack();
  newTrack->SetP4( stat.p4 );
  newTrack->SetPDGCode(track->GetDefinition()->GetPDGEncoding());
  newTrack->SetPDGCharge(track->GetDefinition()->GetPDGCharge());
  newTrack->SetParticleName(track->GetDefinition()->GetParticleName());
  newTrack->SetSource("FromG4");
  BesTruthVertex* vertex = (*m_vertexList)[stat.originVertex];
  newTrack->SetVertex(vertex);
  
  if(track->GetParentID()==0)
  {
    stat.trackIndex = m_count;
    m_count++;
  }
  else
    stat.trackIndex = vertex->GetMinDau() + vertex->GetCurrentDau()-1;
  
  newTrack->SetIndex( stat.trackIndex );
  m_trackIndex = stat.trackIndex;
  if(m_logLevel<=4)
    G4cout<<"  m_trackIndex:"<<m_trackIndex<<G4endl;
  
  newTrack->SetG4TrackId( track->GetTrackID()); 
  m_trackList->push_back( newTrack );
  //tell the parent track its daughter track index
  BesTruthTrack* parent = newTrack->GetVertex()->GetParentTrack();
  if(parent)
  {
    parent->AddDaughterIndex(stat.trackIndex);
    if(m_logLevel<=4)
      G4cout<<"add this daughter to:"<<parent->GetIndex()<<G4endl;
  }
}

//
// FollowTrack (protected)
//
// Build a new track geneology stat record, and update
// the trackList, and vertexList appropriately.
//

BesTStats  BesSensitiveManager::FollowTrack( const G4Track *track )
{
  // Some default stats for this track
  BesTStats stat( track->GetTrackID(),
		  HepLorentzVector( track->GetMomentum(), track->GetTotalEnergy() ),
		  track->GetGlobalTime());

  // Check immediate parent
  G4int parentId = track->GetParentID();
  G4int pdg = track->GetDefinition()->GetPDGEncoding();
  G4String particleName = track->GetDefinition()->GetParticleName();
  G4ThreeVector trackPos = track->GetPosition();
  G4double diffT = m_t0-track->GetGlobalTime();
  G4ThreeVector diffPos = (m_pos0-trackPos).mag2();

  if (parentId == 0) 
  {
    if(m_logLevel<=4)
    {
      G4cout<<G4endl;
      G4cout<<"trackId:"<<track->GetTrackID()<<" ";
      G4cout<<"parentId:"<<parentId<<" ";
      G4cout<<"pdg:"<<pdg<<" ";
      G4cout<<"name:"<<particleName<<" ";
      G4cout<<"timeDiff:"<<diffT<<" ";
      G4cout<<"posDiff:"<<diffPos<<G4endl;
    }

    // Primary particle: wipe decay chain
    chain.clear();
    // Always save
    stat.savedByDefault = true;
    // Make TruthTrack
    if(m_hepmcevt==0)
      MakeNewTrack( stat, track );  
    else
    {
      UpdatePrimaryTrack(track);
      m_trackFlag = 1;
    }
    return stat;
  }
	
  // Not primary particle: trim down chain until
  // we uncover the parent. The parent must be there!
  for(;;) 
  {
    if (chain.back().G4index == parentId) break;
    chain.pop_back();
  }

  // This track is a candidate for saving by default
  // only if its parent was saved by default
  if (chain.back().savedByDefault) 
  {
    // There are three ways particles are saved by default:
    //   1. if they are a primary particle
    //   2. if they are the result of a decay
    //
    G4String processName = track->GetCreatorProcess()->GetProcessName();
    if (processName=="Decay") 
    {
      if(m_logLevel<=4)
      {
        G4cout<<G4endl;
        G4cout<<"trackId: "<<track->GetTrackID()<<" ";
        G4cout<<"parentId: "<<parentId<<" ";
        G4cout<<"pdg: "<<pdg<<" ";
        G4cout<<"pname: "<<particleName<<G4endl;
      }
      stat.savedByDefault = true;

      if(CheckDecayTrack(track))
        m_trackFlag = 1;
      else
        MakeNewTrack( stat, track);
      return stat;
    }
  }
  
  //now this track will not be saved as BesTruthTrack
  stat.savedByDefault = false;
  return stat;
}

G4bool BesSensitiveManager::CheckDecayTrack(const G4Track* track)
{ 
  G4int flag = 0;
  G4int trackID = track->GetTrackID();
  G4int parentID = track->GetParentID();
  G4int pdgcode = track->GetDefinition()->GetPDGEncoding();
  G4ThreeVector p3 = track->GetMomentum();
  if(m_logLevel<=4)
    G4cout<<"CheckDecayTrack p3: "<<p3<<"  "<<track->GetTotalEnergy()<<G4endl;

  //HepLorentzVector  p4(track->GetMomentum(), track->GetTotalEnergy());
  
  BesTruthTrack* truthTrack=0;
  G4int nTrack = m_trackList->size();
  G4int parentTrackIndex=-99;
  G4int terminalVertexIndex=-99;
  BesTruthTrack* pTrack;
  for(int i=0;i<nTrack;i++)
  {
    truthTrack = (*m_trackList)[i];
    if(truthTrack->GetG4TrackId() == parentID)
    {
      //parentTrackIndex = i;
      parentTrackIndex = truthTrack->GetIndex();
      if(truthTrack->GetTerminalVertex())
      {
        terminalVertexIndex = truthTrack->GetTerminalVertex()->GetIndex();
        pTrack = truthTrack;
        if(m_logLevel<=4)
        {
          G4cout<<"parentTrackIndex:"<<parentTrackIndex<<" "
                <<"parent terminate at vertex: "<<terminalVertexIndex<<G4endl;
          if(parentTrackIndex != i)
            G4cout<<"i: "<<i<<std::endl;
        }
        break;
      }
    }
  }
  if(parentTrackIndex==-99 || terminalVertexIndex==-99)
  {
    G4cout<<"MatchDecayError!"<<G4endl;
    //return false;
    exit(1);
  }
 
  //match decayed track with p3
  /*G4bool matchDauFlag=false; 
  if(terminalVertexIndex>0)
    matchDauFlag = MatchDaughterTrack(track);
  if(matchDauFlag)
    return true;*/

  //match decayed track with vertex index
  G4double minDiffP=1e99;
  G4int truthI=-9;
  /*for(int i=0;i<nTrack;i++)
  {
    truthTrack = (*m_trackList)[i];
    G4String source = truthTrack->GetSource();
    G4int pdgcode_tru = truthTrack->GetPDGCode();
    if(pdgcode_tru==-22)  pdgcode_tru = 22;
    if(truthTrack->GetVertex()->GetIndex() == terminalVertexIndex &&
       pdgcode_tru == pdgcode &&source=="FromGenerator"&&truthTrack->NotFound())
    {
      HepLorentzVector tp4 = truthTrack->GetP4();
      G4ThreeVector tp3(tp4.x(), tp4.y(), tp4.z());
      G4double diffP = (p3-tp3).mag2();
      if(m_logLevel<=4)
        G4cout<<"diffP: "<<diffP<<G4endl;
      if(diffP<minDiffP )
      {  minDiffP = diffP;  truthI = i; }
    }
  }*/
  
  /*if(truthI!=-9)
  {
    if(m_logLevel<=4)
      G4cout<<"MatchDecayedTrackWithVertexIndex, trackIndex:"<<truthI<<G4endl;
    m_trackIndex = truthI;
    truthTrack=(*m_trackList)[truthI];
    //truthTrack->SetP4(p4);
    truthTrack->SetPDGCharge(track->GetDefinition()->GetPDGCharge());
    truthTrack->SetParticleName(track->GetDefinition()->GetParticleName());
    truthTrack->SetG4TrackId(track->GetTrackID());
    truthTrack->Found();
    G4int size = truthTrack->GetDaughterIndexes().size();
    if(size>0)
    {
      G4int minDau = (truthTrack->GetDaughterIndexes())[0];
      G4int maxDau = (truthTrack->GetDaughterIndexes())[size-1];
      if(m_logLevel<=4)
        G4cout<<"daughters: "<<minDau<<" "<<maxDau<<G4endl;
    }
    else
    {
      if(m_logLevel<=4)
      G4cout<<G4endl;
    }
    return true; 
  }*/ //match decay track with vertex index

  //match decay track
  
  if(m_logLevel<=4)
    std::cout<<"chain.back().vertices.size(): "<<chain.back().vertices.size()<<std::endl;
  //if particle is from generator, chain.back().vertices.size()=0;
  //if particle is not from generator, but decayed during flight in Geant4, chain.back().vertices.size()=1;
  if(chain.back().vertices.size()<1)
  {
    if(m_logLevel<=4)
      std::cout<<"trackList size: "<<m_trackList->size()<<std::endl;
    std::vector<int>* vDau = new std::vector<int>;
    GetDaughterVertexes(pTrack, vDau);
    G4int sizev = vDau->size();
    if(sizev>0)
    {
      for(int i=0;i<nTrack;i++)
      {
        truthTrack = (*m_trackList)[i];
        G4int vIndex = truthTrack->GetVertex()->GetIndex();
        G4int pdgT = truthTrack->GetPDGCode();
        if(pdgT==-22)  pdgT = 22;
        if(pdgT == pdgcode)
        {
          G4bool matchFlag = MatchVertex(vIndex, vDau);
          if(matchFlag)
          {
            HepLorentzVector tp4 = truthTrack->GetP4();
            G4ThreeVector tp3(tp4.x(), tp4.y(), tp4.z());
            G4double diffP = (p3-tp3).mag2();
            if(m_logLevel<=4)
            {
              G4cout<<"index: "<<truthTrack->GetIndex()<<"tp3: "<<tp3<<G4endl;
              G4cout<<"diffP: "<<diffP<<G4endl;
            }
            if(diffP<minDiffP)
            {  
              minDiffP = diffP;  truthI = i; 
              if(m_logLevel<=4)
                G4cout<<"truthI: "<<i<<G4endl;
            }
          }
        }
      }
      if(vDau)
        delete vDau;
    }
  }

  if(truthI!=-9)
  {
    m_trackIndex = truthI;
    if(m_logLevel<=4)
    {
      G4cout<<"MatchDecayedTrackWithTrueMother"<<G4endl;
      G4cout<<"MatchWithTrueMother m_trackIndex: "<<m_trackIndex<<G4endl;
      if(minDiffP>1e-9)
        G4cout<<"large minDiffP: "<<minDiffP<<G4endl;
    }
    truthTrack=(*m_trackList)[truthI];
    G4double mass = truthTrack->GetP4().m();
    G4double E = sqrt(mass*mass+p3.x()*p3.x()+p3.y()*p3.y()+p3.z()*p3.z());    
    truthTrack->GetP4().setX(p3.x());
    truthTrack->GetP4().setY(p3.y());
    truthTrack->GetP4().setZ(p3.z());
    truthTrack->GetP4().setE(E);
    HepLorentzVector  p4 = truthTrack->GetP4();
    if(m_logLevel<=4)
    {
      G4cout<<"primary p: "<<p4.x()<<" "<<p4.y()<<" "<<p4.z()<<" "<<p4.m()<<G4endl;
    }
    truthTrack->SetPDGCharge(track->GetDefinition()->GetPDGCharge());
    truthTrack->SetParticleName(track->GetDefinition()->GetParticleName());
    truthTrack->SetG4TrackId(track->GetTrackID());
    truthTrack->Found();
    //truthTrack->GetVertex()->SetPosition(track->GetPosition());
    //truthTrack->GetVertex()->SetTime(track->GetGlobalTime());
    return true;
  }
  return false;   
}

void BesSensitiveManager::GetDaughterVertexes(BesTruthTrack* pTrack, std::vector<int>* vDau)
{
  G4int size = pTrack->GetDaughterIndexes().size();
  if(size>0)
  {
    G4int minDau = (pTrack->GetDaughterIndexes())[0];
    G4int maxDau = (pTrack->GetDaughterIndexes())[size-1];
    //note! here, dau<=maxDau, not dau<maxDau
    for(G4int dau=minDau;dau<=maxDau;dau++)
    {
      if(dau<m_trackList->size())
      {
        BesTruthTrack* truthTrack = (*m_trackList)[dau];
        if(truthTrack->GetVertex())
        {
          vDau->push_back(truthTrack->GetVertex()->GetIndex());
          GetDaughterVertexes(truthTrack,vDau);
        }
      } 
    }
  }
}

G4bool BesSensitiveManager::MatchVertex(G4int vIndex, std::vector<int>* vDau)
{
  G4int size = vDau->size();
  if(size>0)
  {
    for(G4int i=0;i<size;i++)
    {
      if(vIndex==(*vDau)[i])
        return true;
    }
  }
  return false;
}

G4bool BesSensitiveManager::MatchDaughterTrack(const G4Track* track)
{
  /*G4int flag=0;
  G4int pdgcode = track->GetDefinition()->GetPDGEncoding();
  G4ThreeVector p = track->GetMomentum();
  HepLorentzVector  p4(track->GetMomentum(), track->GetTotalEnergy());
  G4ThreeVector pos = track->GetPosition();
  G4double time = track->GetGlobalTime();
  BesTruthTrack* truthTrack=0;
  G4int size = m_trackList->size();
  if(size>0)
  { 
    for(G4int i=0;i<size;i++)
    {
      truthTrack=(*m_trackList)[i];
      G4String source = truthTrack->GetSource(); 
      G4int pdgcode_tru = truthTrack->GetPDGCode();
      if(pdgcode_tru==-22)  pdgcode_tru = 22;
      HepLorentzVector tp4 = truthTrack->GetP4();
      G4ThreeVector tp3(tp4.x(), tp4.y(), tp4.z());
      G4double diffP = (p-tp3).mag2();
      if(source=="FromGenerator"&&pdgcode_tru==pdgcode&&diffP<1e-9&&truthTrack->NotFound())
      {
        m_trackIndex = i;
        //truthTrack->SetP4(p4);
        truthTrack->SetPDGCharge(track->GetDefinition()->GetPDGCharge());
        truthTrack->SetParticleName(track->GetDefinition()->GetParticleName());
        truthTrack->SetG4TrackId(track->GetTrackID());
        truthTrack->Found();
        //truthTrack->GetVertex()->SetPosition(pos);
        //truthTrack->GetVertex()->SetTime(time);
        flag=1;
        if(m_logLevel<=4)
        {
          G4cout<<"MatchDecayedTrackWithP3!"<<"trackIndex:"<<m_trackIndex
                <<" pdgcode:"<<pdgcode<<G4endl;
          G4cout<<"parentIndex:"<<truthTrack->GetVertex()->GetParentTrack()->GetIndex()
                <<"  PDGCode:"<<truthTrack->GetVertex()->GetParentTrack()->GetPDGCode()<<G4endl; 
        }
        return true;
      }
    }
  }
  if(flag!=1)
    return false;*/
  return true;
}
  
void BesSensitiveManager::UpdatePrimaryTrack(const G4Track* track)
{
  if(m_logLevel<=4)
  {
    G4cout<<"UpdatePrimaryTrack! ";
    G4cout<<"position: "<<track->GetPosition()<<G4endl;
    G4cout<<"time: "<<track->GetGlobalTime()<<G4endl;
    G4cout<<"momentum: "<<track->GetMomentum()<<"  "<<track->GetTotalEnergy()<<G4endl;
  }
  G4int pdgcode = track->GetDefinition()->GetPDGEncoding();
  G4ThreeVector p = track->GetMomentum();
  //HepLorentzVector  p4(track->GetMomentum(), track->GetTotalEnergy());
  G4int nTrack = m_trackList->size();
  BesTruthTrack* truthTrack;
  G4int flag = 0;
  G4double minDiffP=1e99;
  for(int i=0;i<nTrack;i++)
  {
    truthTrack = (*m_trackList)[i];
    HepLorentzVector tp4 = truthTrack->GetP4();
    G4ThreeVector tp3(tp4.x(), tp4.y(), tp4.z());
    G4double diffP=(p-tp3).mag2();
    G4int pdgT = truthTrack->GetPDGCode();
    if(pdgT==-22) pdgT=22;
    if(pdgcode == pdgT && diffP<minDiffP)
      minDiffP = diffP;
    if(pdgcode == pdgT && diffP < 1E-9 && 
       truthTrack->NotFound())
    {
      m_trackIndex = truthTrack->GetIndex();
      if(m_logLevel<=4)
        G4cout<<"m_trackIndex: "<<m_trackIndex<<" "<<G4endl;
      truthTrack->SetPDGCharge(track->GetDefinition()->GetPDGCharge());
      truthTrack->SetParticleName(track->GetDefinition()->GetParticleName());
      truthTrack->SetG4TrackId(track->GetTrackID());
      truthTrack->Found();
      G4double mass = truthTrack->GetP4().m();
      G4double E = sqrt(mass*mass+p.x()*p.x()+p.y()*p.y()+p.z()*p.z());
      truthTrack->GetP4().setX(p.x());
      truthTrack->GetP4().setY(p.y());
      truthTrack->GetP4().setZ(p.z());
      truthTrack->GetP4().setT(E);
      if(m_logLevel<=4)
      {
        HepLorentzVector  p4 = truthTrack->GetP4();
        G4cout<<"primary p: "<<p4.x()<<" "<<p4.y()<<" "<<p4.z()<<" "<<p4.m()<<G4endl;
      }
      G4int size = truthTrack->GetDaughterIndexes().size();
      if(size>0)
      {
        G4double minDau = (truthTrack->GetDaughterIndexes())[0];
        G4double maxDau = (truthTrack->GetDaughterIndexes())[size-1];
        if(m_logLevel<=4)
          G4cout<<"daughters: "<<minDau<<" "<<maxDau<<G4endl;
      }
      else
      {
        if(m_logLevel<=4)
          G4cout<<G4endl;
      }
      flag = 1; break;
    }
  }
  if(flag==0)
  {
    G4cout<<"MatchError! parentID=0, but match no track in trackList"<<G4endl;
    G4cout<<"pdgcode: "<<pdgcode<<" min p diff: "<<minDiffP<<G4endl;
    exit(1);
  }
}


