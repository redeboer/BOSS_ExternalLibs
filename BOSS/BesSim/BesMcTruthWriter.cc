//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description:
////Author:   Dengzy
////Created:  Mar, 2004
////Modified:
////Comment:
//
#include "G4DigiManager.hh"
#include "BesMdcHit.hh"
#include "BesTofHit.hh"
#include "BesEmcHit.hh"
#include "BesMucHit.hh"
#include "BesTruthTrack.hh"
#include "BesTruthVertex.hh"
#include "BesSensitiveManager.hh"
//#include "G4HCofThisEvent.hh"
//#include "G4SDManager.hh"
//#include "G4PrimaryVertex.hh"
//#include "G4PrimaryParticle.hh"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/MsgStream.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Point3D.h"

#include "McTruth/DecayMode.h"
#include "McTruth/MdcMcHit.h"
#include "McTruth/TofMcHit.h"
#include "McTruth/EmcMcHit.h"
#include "McTruth/MucMcHit.h"

#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "Identifier/TofID.h"
#include "Identifier/EmcID.h"
#include "Identifier/MucID.h"

#include "McTruth/McEvent.h"
#include "EventModel/EventModel.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "BesMcTruthWriter.hh"

BesMcTruthWriter::BesMcTruthWriter()
{
  m_DigiMan = G4DigiManager::GetDMpointer(); 
  //mdcGeoPointer=BesMdcGeoParameter::GetGeo(); 
}

BesMcTruthWriter::~BesMcTruthWriter()
{
}

void BesMcTruthWriter::SaveMcTruth()
{
  //interface to event data service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc=svcLocator->service("EventDataSvc", m_evtSvc);
  if (sc.isFailure())
    G4cout<<"Could not accesss EventDataSvc!"<<G4endl;
  
  DataObject *aMcEvent;
  m_evtSvc->findObject("/Event/MC",aMcEvent);
  if(aMcEvent==NULL) {       
    McEvent* aMcEvent = new McEvent;
    sc = m_evtSvc->registerObject("/Event/MC",aMcEvent);
    if(sc!=StatusCode::SUCCESS) 
      G4cout<< "Could not register McEvent" <<G4endl;
  }

  SaveMcParticle();
  SaveDecayMode();
  SaveMdcTruth();
  SaveTofTruth();
  SaveEmcTruth();
  SaveMucTruth();
}

void BesMcTruthWriter::SaveMcParticle()
{
  BesSensitiveManager* sensitiveManager = BesSensitiveManager::GetSensitiveManager(); 
  vector<BesTruthTrack*>* trackList =  sensitiveManager->GetTrackList();
  G4int nTrack = trackList->size();
  BesTruthTrack* track;

  vector<BesTruthVertex*>* vertexList =  sensitiveManager->GetVertexList();
  G4int nVertex = vertexList->size();
  BesTruthVertex* vertex;
  
  //arrange TruthTrack in trackList in order of trackIndex
  for(int i=0;i<nTrack-1;i++)
  for(int j=i+1;j<nTrack;j++)
    if((*trackList)[i]->GetIndex()>(*trackList)[j]->GetIndex())
    {
      track=(*trackList)[i];
      (*trackList)[i]=(*trackList)[j];
      (*trackList)[j]=track;
    }
  
  Event::McParticleCol *particleCol = new Event::McParticleCol;
  
  //loop over tracks
  for(int i=0;i<nTrack;i++)
  {
    track = (*trackList) [i];
    
    // find out the start point
    bool isPrimary = false;
    bool startPointFound = false;
    BesTruthVertex* startPoint;
    
    for (int i=0;i<nVertex;i++) 
    {
      vertex = (*vertexList) [i];
      if ( track->GetVertex()->GetIndex() == vertex->GetIndex() ) 
      {
        if (!vertex->GetParentTrack()) isPrimary = true;
        startPointFound = true;
        startPoint = vertex;
        break;
      }
    }

    if (!startPointFound)  
      std::cout << "error in finding the start point of a track" <<std::endl;


    bool endPointFound = false;
    // find out the end point
    for (int i=0;i<nVertex;i++)
    {
      vertex = (*vertexList) [i];
      if( track->GetTerminalVertex() )
      {
        if (track->GetTerminalVertex()->GetIndex() == vertex->GetIndex() ) 
        {
	        //create the mc particle
      	  Event::McParticle* mcParticle = new Event::McParticle;
        
	        // initialization 
      	  HepLorentzVector initialMomentum(track->GetP4().x()/1000., track->GetP4().y()/1000., track->GetP4().z()/1000., track->GetP4().t()/1000.);
                 
      	  HepLorentzVector  initialPosition(startPoint->GetPosition().x()/10., startPoint->GetPosition().y()/10., startPoint->GetPosition().z()/10.,startPoint->GetTime());
            
	        mcParticle->initialize(track->GetPDGCode(), 0, initialMomentum, initialPosition); 
          
          // Set track index
          mcParticle->setTrackIndex( track->GetIndex() );
          
      	  // Adding status flag  
	        if (isPrimary ) mcParticle->addStatusFlag(Event::McParticle::PRIMARY);

          if (track->GetDaughterIndexes().size()==0)
            mcParticle->addStatusFlag(Event::McParticle::LEAF);
        
          //std::cout<<"pdg:"<<track->GetPDGCode()<<" "
          //         <<"source:"<<track->GetSource()<<" "
          //         <<std::endl;
          if (track->GetSource()=="FromGenerator")
            mcParticle->addStatusFlag(Event::McParticle::DECAYED);
          else if(track->GetSource()=="FromG4")
            mcParticle->addStatusFlag(Event::McParticle::DECAYFLT); 
          else
            mcParticle->addStatusFlag(Event::McParticle::ERROR);

          //std::cout<<"status:"<<mcParticle->statusFlags()<<" "
          //         <<"FGN:"<<mcParticle->decayFromGenerator()<<" "
          //         <<"FG4:"<<mcParticle->decayInFlight()<<std::endl;
	        // Adding vertex index 
	        mcParticle->setVertexIndex0(startPoint->GetIndex());
          mcParticle->setVertexIndex1(vertex->GetIndex());
          
          // Set the final position 
          HepLorentzVector  finalPosition(vertex->GetPosition().x()/10., vertex->GetPosition().y()/10., vertex->GetPosition().z()/10., vertex->GetTime());
          mcParticle->finalize(finalPosition);

          // push back 
	        particleCol->push_back(mcParticle);

          endPointFound = true; 
        }
      }
    }
   
    if (!endPointFound)
    {
	    //std::cout << "warning in finding the end point of a track" <<std::endl;
	    if (track->GetDaughterIndexes().size()==0) 
      {
	      // create the mc particle
	      Event::McParticle* mcParticle = new Event::McParticle;
	      // initialization 
	      HepLorentzVector initialMomentum( track->GetP4().x()/1000., track->GetP4().y()/1000., track->GetP4().z()/1000., track->GetP4().t()/1000.);
	      HepLorentzVector  initialPosition(startPoint->GetPosition().x()/10., startPoint->GetPosition().y()/10., startPoint->GetPosition().z()/10., startPoint->GetTime());

	      mcParticle->initialize(track->GetPDGCode(), 0, initialMomentum, initialPosition); 

        // Set track index
	      mcParticle->setTrackIndex( track->GetIndex() );

   	    // Adding status flag  
	      mcParticle->addStatusFlag(Event::McParticle::LEAF);
	      if (isPrimary ) mcParticle->addStatusFlag(Event::McParticle::PRIMARY);

    	  // Adding vertex index 
	      mcParticle->setVertexIndex0( startPoint->GetIndex() );
	      mcParticle->setVertexIndex1( -99 );

        //std::cout<<"pdg:"<<track->GetPDGCode()<<" "
        //           <<"source:"<<track->GetSource()<<" "
        //           <<std::endl;
        if (track->GetSource()=="FromGenerator")
          mcParticle->addStatusFlag(Event::McParticle::DECAYED);
        else if(track->GetSource()=="FromG4")
          mcParticle->addStatusFlag(Event::McParticle::DECAYFLT); 
        else
          mcParticle->addStatusFlag(Event::McParticle::ERROR);

        //std::cout<<"status:"<<mcParticle->statusFlags()<<" "
        //         <<"FGN:"<<mcParticle->decayFromGenerator()<<" "
        //         <<"FG4:"<<mcParticle->decayInFlight()<<std::endl;

    	  // push back 
	      particleCol->push_back(mcParticle);
	      continue;
      }
    }
	}

  // Get primary McParticles 
  SmartRefVector<Event::McParticle> primaryParticleCol;
  Event::McParticleCol::iterator iter_particle;
  for ( iter_particle = particleCol->begin(); 
        iter_particle != particleCol->end(); iter_particle++) {

    if ((*iter_particle)->primaryParticle()) {
       Event::McParticle* mcPart =  (Event::McParticle*)(*iter_particle); 
       primaryParticleCol.push_back(mcPart);
     }
  }

  if (primaryParticleCol.empty())
    std::cout << "no primary particle found!" << std::endl;
    
  // Add mother particle recursively 
  SmartRefVector<Event::McParticle>::iterator iter_primary; 
  for ( iter_primary = primaryParticleCol.begin(); 
        iter_primary != primaryParticleCol.end(); iter_primary++) {
    if ( !((*iter_primary)->leafParticle()) ) 
      AddMother((*iter_primary), particleCol);
  }

  //register McParticle collection to TDS
  StatusCode sc = m_evtSvc->registerObject("/Event/MC/McParticleCol", particleCol);
  if(sc!=StatusCode::SUCCESS) 
    G4cout<< "Could not register McParticle collection" <<G4endl;
  
  //retrive McParticle from TDS
  /*SmartDataPtr<Event::McParticleCol> mcParticleCol(m_evtSvc,"/Event/MC/McParticleCol");
  if(!mcParticleCol)
    G4cout<<"Could not retrieve McParticelCol"<<G4endl;  
  Event::McParticleCol::iterator iter_mc = mcParticleCol->begin();
  for (;iter_mc != mcParticleCol->end(); iter_mc++)
  {
    G4cout<<(*iter_mc)->getTrackIndex();
    G4cout<<" "<<(*iter_mc)->particleProperty();
    G4cout<<" "<<(*iter_mc)->getVertexIndex0();
    G4cout<<" "<<(*iter_mc)->getVertexIndex1();
    G4cout<<" "<<(*iter_mc)->initialFourMomentum().x();
    G4cout<<" "<<(*iter_mc)->initialFourMomentum().y();
    G4cout<<" "<<(*iter_mc)->initialFourMomentum().z();
    G4cout<<" "<<(*iter_mc)->initialFourMomentum().t();
    G4cout<<" "<<(*iter_mc)->initialPosition().x();
    G4cout<<" "<<(*iter_mc)->initialPosition().y();
    G4cout<<" "<<(*iter_mc)->initialPosition().z();
    G4cout<<G4endl;
  }
  G4cout<<"end of retrieve McParticle from TDS"<<G4endl;
  */

}

void BesMcTruthWriter::AddMother(Event::McParticle* currentParticle, Event::McParticleCol *particleCol)
{
  if (currentParticle->leafParticle()) {
    return;
  }
  Event::McParticleCol::iterator iter;
  bool found = false;
  for ( iter = particleCol->begin(); iter != particleCol->end(); iter++) {
    if (currentParticle->vertexIndex1() == (*iter)->vertexIndex0()) {
      found = true;
      (*iter)->setMother(currentParticle);
      currentParticle->addDaughter(*iter);
      AddMother((*iter), particleCol);
    }
  }
  if (!found) std::cout << "AddMother: inconsistency was found!" << std::endl;

}

void BesMcTruthWriter::SaveDecayMode()
{
  SmartDataPtr<DecayMode> decayMode(m_evtSvc,"/Event/MC/DecayMode");
  if(!decayMode)
  {
    //G4cout<<"Could not retrieve DecayMode"<<G4endl;
    DecayMode*  aDecayMode = new DecayMode;
    //register Decay Mode to TDS
    int dm[10]={0,0,0,0,0,0,0,0,0,0};
    aDecayMode->putData(dm,10);
    StatusCode scDM = m_evtSvc->registerObject("/Event/MC/DecayMode", aDecayMode);
    if(scDM!=StatusCode::SUCCESS)
      G4cout<< "Could not register DecayMode" <<G4endl;
  }
}
  
void BesMcTruthWriter::SaveMdcTruth()
{
  //Mdc McHit collection defined in BOSS
  Event::MdcMcHitCol*  aMdcMcHitCol = new Event::MdcMcHitCol;

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
      
      //push back MdcMcHits to MdcMcHitCol in TDS
      for(G4int i=0;i<n_hit;i++)
      {
        hit = (*HC)[i];
        const Identifier ident =  MdcID::wire_id ( hit->GetLayerNo(), hit->GetCellNo() );
        Event::MdcMcHit* mdcMcHit = new Event::MdcMcHit(ident, hit->GetTrackID(), 
            hit->GetPos().x(),  hit->GetPos().y(), hit->GetPos().z(), 
            hit->GetDriftD(), hit->GetEdep(),hit->GetPosFlag() );
        aMdcMcHitCol->push_back(mdcMcHit);
      }      
   
    }
  }

  //register MDC McTruth collection to TDS
  StatusCode scMdc = m_evtSvc->registerObject("/Event/MC/MdcMcHitCol", aMdcMcHitCol);
  if(scMdc!=StatusCode::SUCCESS) 
    G4cout<< "Could not register MDC McTruth collection" <<G4endl;

  //retrieve MDC McTruth from TDS
  /*SmartDataPtr<Event::MdcMcHitCol> aMcHitCol(m_evtSvc,"/Event/MC/MdcMcHitCol");
  if(!aMcHitCol)
    G4cout<<"Could not retrieve MDC McTruth collection"<<G4endl;

  Event::MdcMcHitCol::iterator iMcHitCol;
  for(iMcHitCol=aMcHitCol->begin(); iMcHitCol!=aMcHitCol->end(); iMcHitCol++)
  {
    const Identifier ident = (*iMcHitCol)->identify();
    G4cout<<(*iMcHitCol)->getTrackIndex();
    G4cout<<" "<<MdcID::layer(ident);
    G4cout<<" "<<MdcID::wire(ident);
    G4cout<<" "<<(*iMcHitCol)->getDepositEnergy();
    G4cout<<" "<<(*iMcHitCol)->getDriftDistance();
    G4cout<<" "<<(*iMcHitCol)->getPositionX();
    G4cout<<" "<<(*iMcHitCol)->getPositionY();
    G4cout<<" "<<(*iMcHitCol)->getPositionZ();
    G4cout<<G4endl;
  }
  G4cout<<"end of retrieve MDC McTruth collection"<<G4endl;
  */
}

void BesMcTruthWriter::SaveTofTruth()
{ 
  //Tof McHit collection defined in BOSS
  Event::TofMcHitCol*  aTofMcHitCol = new Event::TofMcHitCol;

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
      for( int i=0; i<n_hit-1; i++ ) {
	for( int j=i+1; j<n_hit; j++ ) {
	  if((*vecHC)[i]->GetTrackIndex()>(*vecHC)[j]->GetTrackIndex()) {
	    hit = (*vecHC)[i];
	    (*vecHC)[i] = (*vecHC)[j];
	    (*vecHC)[j] = hit;
	  }
	}
      }

      //push back TofMcHit collection to TDS
      for( G4int i=0; i<n_hit; i++ ) {
        hit = (*HC)[i];
	Identifier ident;
        unsigned int barrel_ec = hit->GetPartId();
	// for Scintillator
	if( TofID::is_scin( barrel_ec ) ) {
	  unsigned int scinNum = hit->GetScinNb();
	  unsigned int layer = 0;
	  if( TofID::is_barrel(barrel_ec) && scinNum>TofID::getPHI_BARREL_MAX()) {
	    layer = 1;
	    scinNum = scinNum -  TofID::getPHI_BARREL_MAX() - 1;
	  }
	  ident =  TofID::cell_id ( barrel_ec, layer, scinNum, 0);
	}
	// for ETF(MRPC)
	else { // for ETF(MRPC)
	  if( barrel_ec==3 || barrel_ec==4 ) {
	    unsigned int endcap = 0;
	    unsigned int module = hit->GetModule();
	    unsigned int strip  = hit->GetStrip();
	    if( barrel_ec==4 ) { // west end cap
	      endcap = 1;
	    }
	    ident = TofID::cell_id( 3, endcap, module, strip, 0 ); 
	  }
	}
	if( barrel_ec>=0 && barrel_ec<=4 ) {
	  Event::TofMcHit* tofMcHit = new Event::TofMcHit( ident, hit->GetTrackIndex(),
							   hit->GetPos().x(), hit->GetPos().y(), hit->GetPos().z(), hit->GetMomentum().x(),
							   hit->GetMomentum().y(), hit->GetMomentum().z(), hit->GetTrackL(), hit->GetTime() );
	  aTofMcHitCol->push_back(tofMcHit);
	}
      }
    }
  }

  //register TOF McTruth collection to TDS
  StatusCode scTof = m_evtSvc->registerObject("/Event/MC/TofMcHitCol", aTofMcHitCol);
  if(scTof!=StatusCode::SUCCESS) 
    G4cout<< "Could not register TOF McTruth collection" <<G4endl;

  //retrieve TOF McTruth from TDS
  /*SmartDataPtr<Event::TofMcHitCol> aMcHitCol(m_evtSvc,"/Event/MC/TofMcHitCol");
  if(!aMcHitCol)
    G4cout<<"Could not retrieve TOF McTruth collection"<<G4endl;

  Event::TofMcHitCol::iterator iMcHitCol;
  for(iMcHitCol=aMcHitCol->begin(); iMcHitCol!=aMcHitCol->end(); iMcHitCol++)
  {
    const Identifier ident = (*iMcHitCol)->identify();
    G4cout<<(*iMcHitCol)->getTrackIndex();
    G4cout<<" "<<TofID::barrel_ec(ident);;
    G4cout<<" "<<TofID::layer(ident);
    G4cout<<" "<<TofID::phi_module(ident);
    G4cout<<" "<<(*iMcHitCol)->getPositionX();
    G4cout<<" "<<(*iMcHitCol)->getPositionY();
    G4cout<<" "<<(*iMcHitCol)->getPositionZ();
    G4cout<<" "<<(*iMcHitCol)->getPx();
    G4cout<<" "<<(*iMcHitCol)->getPy();
    G4cout<<" "<<(*iMcHitCol)->getPz();
    G4cout<<" "<<(*iMcHitCol)->getTrackLength();
    G4cout<<" "<<(*iMcHitCol)->getFlightTime();
    G4cout<<G4endl;
  }
  G4cout<<"end of retrieve TOF McTruth"<<G4endl;
  */
}  

void BesMcTruthWriter::SaveEmcTruth()
{
  //Emc McHit collection defined in BOSS
  Event::EmcMcHitCol*  aEmcMcHitCol = new Event::EmcMcHitCol;

  G4int fullMc = 1;
  if(fullMc==1) {     //full Mc Truth
    G4int HCID = -1;
    HCID = m_DigiMan->GetHitsCollectionID("BesEmcTruthHitsList");
    if(HCID>0)
    {
      BesEmcTruthHitsCollection* HC = 0;
      HC = (BesEmcTruthHitsCollection*) (m_DigiMan->GetHitsCollection(HCID));
      G4int n_hit = HC->entries();
      if(n_hit>0)
      {
        //arrange hits in hits collection in order of trackIndex
        BesEmcTruthHit* hit;
        vector<BesEmcTruthHit*>* vecHC = HC->GetVector();
        for(int i=0;i<n_hit-1;i++) {
          for(int j=i+1;j<n_hit;j++) {
            if((*vecHC)[i]->GetTrackIndex()>(*vecHC)[j]->GetTrackIndex()) {
              hit = (*vecHC)[i];
              (*vecHC)[i] = (*vecHC)[j];
              (*vecHC)[j] = hit;
            }
          }
        }

        for(G4int i=0;i<n_hit;i++)
        {
          BesEmcTruthHit* hit;
          hit = (*HC)[i];

          std::map<Identifier,double> hitMap;
          std::map<Identifier,double>::const_iterator iHitMap;
          hitMap.clear();
          for(iHitMap=hit->Begin();iHitMap!=hit->End();iHitMap++) {
            hitMap[iHitMap->first]=iHitMap->second;
          }
          
          Event::EmcMcHit* emcMcHit = new Event::EmcMcHit(hit->GetIdentify(), hit->GetTrackIndex(),
              hit->GetPosition().x(), hit->GetPosition().y(), hit->GetPosition().z(),
              hit->GetMomentum().x(), hit->GetMomentum().y(), hit->GetMomentum().z(),
              hit->GetEDep() );

          emcMcHit->setHitEmc(hit->GetHitEmc());
          emcMcHit->setPDGCode(hit->GetPDGCode());
          emcMcHit->setPDGCharge(hit->GetPDGCharge());
          emcMcHit->setTime(hit->GetTime());
          emcMcHit->setHitMap(hitMap);

          aEmcMcHitCol->push_back(emcMcHit);
        }
      }
    }
  } else {    //simple Mc Truth
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
            
        //Emc McHit collection defined in BOSS
        Event::EmcMcHitCol*  aEmcMcHitCol = new Event::EmcMcHitCol;
        
        for(G4int i=0;i<n_hit;i++)
        {
          hit = (*HC)[i];
          Identifier ident =  EmcID::crystal_id ( hit->GetPartId(), hit->GetNumThetaCrystal(), hit->GetNumPhiCrystal() );
          
          std::map<Identifier,double> hitMap;
          Event::EmcMcHit* emcMcHit = new Event::EmcMcHit(ident, hit->GetTrackIndex(),
              hit->GetPosCrystal().x(), hit->GetPosCrystal().y(), hit->GetPosCrystal().z(),
              hit->GetMomentum().x(), hit->GetMomentum().y(), hit->GetMomentum().z(),
              hit->GetEdepCrystal() );
          
          aEmcMcHitCol->push_back(emcMcHit);
        }
      }
    }           
  }

  //register EMC McTruth collection to TDS
  StatusCode scEmc = m_evtSvc->registerObject("/Event/MC/EmcMcHitCol", aEmcMcHitCol);
  if(scEmc!=StatusCode::SUCCESS)
    G4cout<< "Could not register EMC McTruth collection" <<G4endl;

  //retrieve EMC McTruth from TDS
  /*SmartDataPtr<Event::EmcMcHitCol> aMcHitCol(m_evtSvc,"/Event/MC/EmcMcHitCol");
  if(!aMcHitCol)
    G4cout<<"Could not retrieve EMC McTruth collection"<<G4endl;

  Event::EmcMcHitCol::iterator iMcHitCol;
  for(iMcHitCol=aMcHitCol->begin(); iMcHitCol!=aMcHitCol->end(); iMcHitCol++)
  {
    const Identifier ident = (*iMcHitCol)->identify();
    G4cout<<(*iMcHitCol)->getTrackIndex();
    G4cout<<" "<<EmcID::barrel_ec(ident);
    G4cout<<" "<<EmcID::theta_module(ident);
    G4cout<<" "<<EmcID::phi_module(ident);
    G4cout<<" "<<(*iMcHitCol)->getPositionX();
    G4cout<<" "<<(*iMcHitCol)->getPositionY();
    G4cout<<" "<<(*iMcHitCol)->getPositionZ();
    G4cout<<" "<<(*iMcHitCol)->getPx();
    G4cout<<" "<<(*iMcHitCol)->getPy();
    G4cout<<" "<<(*iMcHitCol)->getPz();
    G4cout<<" "<<(*iMcHitCol)->getDepositEnergy();
    G4cout<<G4endl;
  }
  G4cout<<"end of retrieve EMC McTruth"<<G4endl;
  */
}
 
void BesMcTruthWriter::SaveMucTruth()
{
  //Muc McHit collection defined in BOSS
  Event::MucMcHitCol*  aMucMcHitCol = new Event::MucMcHitCol;

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
        Identifier ident =  MucID::channel_id ( hit->GetPart(), hit->GetSeg(), hit->GetGap(),
          hit->GetStrip() );
        Event::MucMcHit* mucMcHit = new Event::MucMcHit(ident, hit->GetTrackIndex(), 
            hit->GetPos().x(), hit->GetPos().y(), hit->GetPos().z(), hit->GetMomentum().x(),
            hit->GetMomentum().y(), hit->GetMomentum().z() );
        aMucMcHitCol->push_back(mucMcHit);
      }
      
    }
  }

  //register MUC McTruth collection to TDS
  StatusCode scMuc = m_evtSvc->registerObject("/Event/MC/MucMcHitCol", aMucMcHitCol);
  if(scMuc!=StatusCode::SUCCESS) 
    G4cout<< "Could not register MUC McTruth collection" <<G4endl;

  //retrieve MUC McTruth from TDS
  /*SmartDataPtr<Event::MucMcHitCol> aMcHitCol(m_evtSvc,"/Event/MC/MucMcHitCol");
  if(!aMcHitCol)
    G4cout<<"Could not retrieve MUC McTruth collection"<<G4endl;

  Event::MucMcHitCol::iterator iMcHitCol;
  for(iMcHitCol=aMcHitCol->begin(); iMcHitCol!=aMcHitCol->end(); iMcHitCol++)
  {
    const Identifier ident = (*iMcHitCol)->identify();
    G4cout<<(*iMcHitCol)->getTrackIndex();
    G4cout<<" "<<MucID::part(ident);
    G4cout<<" "<<MucID::seg(ident);
    G4cout<<" "<<MucID::gap(ident);
    G4cout<<" "<<MucID::strip(ident);
    G4cout<<" "<<(*iMcHitCol)->getPositionX();
    G4cout<<" "<<(*iMcHitCol)->getPositionY();
    G4cout<<" "<<(*iMcHitCol)->getPositionZ();
    G4cout<<" "<<(*iMcHitCol)->getPx();
    G4cout<<" "<<(*iMcHitCol)->getPy();
    G4cout<<" "<<(*iMcHitCol)->getPz();
    G4cout<<G4endl;
  }
  G4cout<<"end of retrieve MUC McTruth"<<G4endl;
  */
}

