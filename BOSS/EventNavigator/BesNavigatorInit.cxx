#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "EventNavigator/EventNavigator.h"
#include "EventNavigator/BesNavigatorInit.h"

// Monte-Carlo data
#include "McTruth/McParticle.h"
#include "McTruth/EmcMcHit.h"
#include "McTruth/MdcMcHit.h"
#include "McTruth/MucMcHit.h"
#include "McTruth/TofMcHit.h"

// MDC reconstructed data
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcHit.h"

// EMC reconstructed data
#include "EmcRecEventModel/RecEmcShower.h"

// TOF reconstructed data
#include "TofRecEvent/RecTofTrack.h"
#include "TofRecEvent/RecBTofHit.h"
#include "TofRecEvent/RecETofHit.h"

// MUC reconstructed data
#include "MucRecEvent/RecMucTrack.h"
#include "MucRecEvent/MucRecHit.h"

// Digi information
#include "EmcRecEventModel/RecEmcDigit.h"
#include "MdcRawEvent/MdcDigi.h"

using namespace std;
using namespace EventModel;
using namespace Event;

/////////////////////////////////////////////////////////////////////////////

BesNavigatorInit::BesNavigatorInit(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator){
  // Subdetector flags
  declareProperty("FillMdcInfo", b_fillMdc=true);
  declareProperty("FillTofInfo", b_fillTof=true);
  declareProperty("FillEmcInfo", b_fillEmc=true);
  declareProperty("FillMucInfo", b_fillMuc=true);
  declareProperty("MdcCut", m_mdcCut=7);
  declareProperty("PrepareHitMaps", b_fillHitMaps = false);
  state = SIMU;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode BesNavigatorInit::initialize(){
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode BesNavigatorInit::execute() {
  MsgStream log(msgSvc(), name());

  state = SIMU;  
  
  SmartDataPtr<EventNavigator> nav(eventSvc(),"/Event/Navigator");
  if( ! nav ) 
    {
      log << MSG::DEBUG << "Create EventNavigator" << endreq;      
      m_navigator = new EventNavigator;
      m_navigator->setMdcCut(m_mdcCut);
    }
  else 
    {
      log << MSG::DEBUG << "Found EventNavigator (read from DST)" << endreq;
      m_navigator = nav;
      state = RECO;
      if( log.level() < 3 )
	m_navigator->Print();
    }
    
  SmartDataPtr<McParticleCol> mcParticles(eventSvc(),"/Event/MC/McParticleCol");
  if( ! mcParticles )
    {
      log << MSG::ERROR << "Unable to retrieve McParticleCol" << endreq;
      return StatusCode::FAILURE;
    }
  else
    log << MSG::DEBUG << "McParticleCol retrieved of size "<< mcParticles->size() << endreq;
  
  for(McParticleCol::const_iterator it = mcParticles->begin(); it != mcParticles->end(); it++)
    {
      m_navigator->addIdLink( (*it)->trackIndex(), *it );
    }

  // store MDC relations
  if( b_fillMdc )
    fillMdcInfo();
  
  // store EMC relations
  if( b_fillEmc )
    fillEmcInfo();
  
  // store TOF relations
  if( b_fillTof )
    fillTofInfo();
  
  // store MUC relations
  if( b_fillMuc )
    fillMucInfo();
  
  // Register EventNavigator to Gaudi TDS
  if (state == SIMU)
    {
      StatusCode sc = eventSvc()->registerObject("/Event/Navigator",m_navigator);
      if (sc != StatusCode::SUCCESS)
	{
	  log << MSG::ERROR << "Could not register EventNavigator" << endreq;
	  return StatusCode::FAILURE;
	}		 
      else
	log << MSG::INFO << "EventNavigator registered successfully in TDS" << endreq;
    }

   if( log.level() < 3 )
   {  
       log << MSG::DEBUG << "Write EventNavigator" << endreq;
       m_navigator->Print();
   }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode BesNavigatorInit::finalize() {
  return StatusCode::SUCCESS;
}

void BesNavigatorInit::fillMdcInfo() 
{
  MsgStream log(msgSvc(), name());

  //****** Process hits ******
  // If simulated hits are there, fill index map (read from DST if no hits in TDS)
  SmartDataPtr<MdcMcHitCol> mdcMcHits(eventSvc(),"/Event/MC/MdcMcHitCol");
  if( mdcMcHits )
    {
      log << MSG::DEBUG << "MdcMcHitsCol retrieved of size "<< mdcMcHits->size() << endreq;
      
      if (mdcMcHits->size() != 0) 
	m_navigator->getMcMdcMcHitsIdx().clear();
      
      for(MdcMcHitCol::const_iterator it = mdcMcHits->begin(); it != mdcMcHits->end(); it++)
	{
	  // fill relation MdcHit id <-> McParticle
	  m_navigator->getMcMdcMcHitsIdx().insert( pair<int,int>((*it)->identify().get_value(), (*it)->getTrackIndex()));
	  
	  //     m_navigator->addIdLink( (*it)->identify().get_value(), *it );
	}
    }
  else
    {
      log << MSG::DEBUG << "Unable to retrieve MdcMcHitCol" << endreq;
    }

  // If reconstructed hits are there, fill index map (read from DST if no hits in TDS) 
  SmartDataPtr<RecMdcHitCol> mdcRecHits(eventSvc(),"/Event/Recon/RecMdcHitCol");
  if( mdcRecHits )
    {
      log << MSG::DEBUG << "MdcRecHitCol retrieved of size "<< mdcRecHits->size() << endreq;

      if (mdcRecHits->size() != 0)
	m_navigator->getMcMdcTracksIdx().clear();
      
      IndexMap& mchits = m_navigator->getMcMdcMcHitsIdx();
      for(RecMdcHitCol::iterator it=mdcRecHits->begin(); it!=mdcRecHits->end(); it++)
	{
	  int mdcId = ((*it)->getMdcId()).get_value();
	  //	  m_navigator->addIdLink( mdcId, *it);
	  
	  const pair<IndexMap::const_iterator, IndexMap::const_iterator> range = mchits.equal_range(mdcId);
	  for(IndexMap::const_iterator jt = range.first; jt != range.second; jt++)
	    {
	      m_navigator->getMcMdcTracksIdx().insert( pair<int,int>((*jt).second, (*it)->getTrkId()));
	    }
	}
    }
  else
    log << MSG::DEBUG << "Unable to retrieve RecMdcHitCol" << endreq;
  
  
  // ****** Get MDC top level objects from TDS ******
  // MDC Reconstructed Tracks
  SmartDataPtr<RecMdcTrackCol> mdcTracks(eventSvc(),"/Event/Recon/RecMdcTrackCol");
  if( mdcTracks )
    {
      log << MSG::DEBUG << "MdcTrackCol retrieved of size "<< mdcTracks->size() << endreq;
      for(RecMdcTrackCol::const_iterator it = mdcTracks->begin(); it != mdcTracks->end(); it++)
	m_navigator->addIdLink( (*it)->trackId(), *it );
    }
  else 
    {
      log << MSG::DEBUG << "Unable to retrieve RecMdcTrackCol" << endreq;
    }
  
  // MDC Kalman Tracks
  SmartDataPtr<RecMdcKalTrackCol> mdcKalTracks(eventSvc(),"/Event/Recon/RecMdcKalTrackCol");
  if( mdcKalTracks )
    {
      log << MSG::DEBUG << "MdcKalTrackCol retrieved of size "<< mdcKalTracks->size() << endreq;
      for(RecMdcKalTrackCol::const_iterator it = mdcKalTracks->begin(); it != mdcKalTracks->end(); it++)
	m_navigator->addIdLink( (*it)->trackId(), *it );
    }
  else 
    {
      log << MSG::DEBUG << "Unable to retrieve RecMdcTrackCol" << endreq;
    }
  
  //****** Make main relations between top level objects ***
  IndexMap::const_iterator i;
  IndexMap& index = m_navigator->getMcMdcTracksIdx();
  for(i = index.begin(); i != index.end(); i++)
    {
      // i.first = McParticle id; i.second = MdcTrack id
      const McParticle* mcParticle = m_navigator->getMcParticle((*i).first);

      if (mcParticle == 0)
	continue;

      const RecMdcTrack* mdcTrack = m_navigator->getMdcTrack((*i).second);
      
      if( mdcTrack != 0 )
	{
	  m_navigator->addLink(mcParticle, mdcTrack);
	  m_navigator->addLink(mdcTrack, mcParticle);
	}

      const RecMdcKalTrack* mdcKalTrack = m_navigator->getMdcKalTrack((*i).second); // same trkid
      if( mdcKalTrack != 0 )
	{
	  m_navigator->addLink(mcParticle, mdcKalTrack);
	  m_navigator->addLink(mdcKalTrack, mcParticle);
	}
    }
}


void BesNavigatorInit::fillEmcInfo()
{   
  MsgStream log(msgSvc(), name());

  //****** Process hits ******
  // If simulated hits are there, fill index map (read from DST if no hits in TDS)
  SmartDataPtr<EmcMcHitCol> emcMcHits(eventSvc(),"/Event/MC/EmcMcHitCol");
  if( emcMcHits )
    {
      log << MSG::DEBUG << "EmcMcHitsCol retrieved of size "<< emcMcHits->size() << endreq;

      if (emcMcHits->size() != 0) 
	m_navigator->getMcEmcMcHitsIdx().clear();

      for(EmcMcHitCol::const_iterator it = emcMcHits->begin(); it != emcMcHits->end(); it++)
	{
	  //first put Id of crystal, which particle hits initially
	  int crystalId = (*it)->identify().get_value();
	  int McParticleID = 0;
	  if(crystalId != 0) // not gamma converted in TOF
	    {
	      McParticleID = (*it)->getTrackIndex();
	      m_navigator->getMcEmcMcHitsIdx().insert( pair<int,int>(crystalId, McParticleID));
	      //	      m_navigator->addIdLink( crystalId, *it );
	    }

	  //then add all other crystals touched by shower
	  const map<Identifier,double>& hitmap = (*it)->getHitMap();
	  map<Identifier,double>::const_iterator jt;
	  for (jt=hitmap.begin(); jt!=hitmap.end(); jt++)
	    {
	      int crystalId = (*jt).first.get_value();
		  
	      if(McParticleID !=0 )  // not gamma converted in TOF
		{
		  m_navigator->getMcEmcMcHitsIdx().insert( pair<int,int>(crystalId, McParticleID)); // CrystalId, McParticleID
		}
	      //m_navigator->addIdLink( (*jt).first.get_value(), *it );
	    }
	}	
    }
  else
    {
      log << MSG::DEBUG << "Unable to retrieve EmcMcHitCol" << endreq;
    }

  // ****** Get EMC top level objects from TDS ******
  // EMC Reconstructed Showers
  SmartDataPtr<RecEmcShowerCol> emcRecShowers(eventSvc(),"/Event/Recon/RecEmcShowerCol");
  if( emcRecShowers )
    {
      log << MSG::DEBUG << "RecEmcShowerCol retrieved of size "<< emcRecShowers->size() << endreq;
      
      IndexMap& mchits = m_navigator->getMcEmcMcHitsIdx();
      
      for(RecEmcShowerCol::const_iterator it = emcRecShowers->begin(); 
	  it != emcRecShowers->end(); 
	  it++)
	{ 
	  int showerId = (*it)->getShowerId().get_value();
	  m_navigator->addIdLink( showerId, *it );
 
	  // calculate relations only during reco and store them to DST. 
	  // EmcMcHit tables are removed then.
	  if(! mchits.empty()) 
	    {
	      const RecEmcFractionMap& fractionMap = (*it)->getFractionMap();
	      for(RecEmcFractionMap::const_iterator jt = fractionMap.begin(); 
		  jt != fractionMap.end(); 
		  jt++)
		{
		  int crystalId = (*jt).first.get_value();
		  const pair<IndexMap::const_iterator, IndexMap::const_iterator> range = mchits.equal_range(crystalId); // McParticles which hit given crystal
		  
		  for(IndexMap::const_iterator kt = range.first; kt != range.second; kt++)
		    {
		      m_navigator->getMcEmcRecShowersIdx().insert( pair<int,int>((*kt).second, showerId)); // McParticle id <-> RecShower ShowerId	  
		    }
		}
	    }
	}
    }
  else
    {
      log << MSG::DEBUG << "Unable to retrieve RecEmcShowerCol" << endreq;
    }
  

  //****** Make main relations between top level objects ***
  IndexMap::const_iterator i;
  IndexMap& index = m_navigator->getMcEmcRecShowersIdx();
  for(i = index.begin(); i != index.end(); i++)
    {
      // i.first = McParticle id; i.second = MdcTrack id
      const McParticle* mcParticle = m_navigator->getMcParticle((*i).first);

      if (mcParticle == 0)
	continue;

      const RecEmcShowerVector& emcShowers = m_navigator->getEmcRecShowers((*i).second);
           
      if( ! emcShowers.empty() )
	{
	  RecEmcShowerVector::const_iterator j;
	  for(j=emcShowers.begin(); j!=emcShowers.end(); j++)
	    {
	      m_navigator->addLink(mcParticle, *j);
	      m_navigator->addLink(*j, mcParticle);
	    }
	}
    }
}

void BesNavigatorInit::fillTofInfo()
{
}

void BesNavigatorInit::fillMucInfo()
{
}
