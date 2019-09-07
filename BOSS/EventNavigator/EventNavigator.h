#ifndef EventNavigator_H
#define EventNavigator_H 

#include <map>

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/DataObject.h"

#include "EventModel/EventModel.h"

// Monte-Carlo data
#include "McTruth/McParticle.h"
#include "McTruth/MdcMcHit.h"
#include "McTruth/EmcMcHit.h"
#include "McTruth/TofMcHit.h"
#include "McTruth/MucMcHit.h"

// MDC reconstructed data
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcHit.h"

// EMC reconstructed data
#include "EmcRecEventModel/RecEmcShower.h"

// TOF reconstructed data
class RecBTofTrack;
class RecBTofHit;
class RecETofTrack;
class RecETofHit;

// MUC reconstructed data
class RecMucTrack;
class RecMucHit;

// Digi information
class MdcDigi;

class BesNavigatorInit;

//using namespace Event;

typedef std::multimap <int, int> IndexMap;
typedef std::vector < pair<const Event::McParticle*,int> > McParticleVectorP;
typedef std::vector < pair<const RecMdcTrack*,int> >    RecMdcTrackVectorP;
typedef std::vector < pair<const RecMdcKalTrack*,int> > RecMdcKalTrackVectorP;
typedef std::vector < pair<const RecEmcShower*,int> > RecEmcShowerVectorP;
typedef std::vector <const Event::McParticle*>       McParticleVector;
typedef std::vector <const RecMdcTrack*>      RecMdcTrackVector;
typedef std::vector <const RecMdcKalTrack*>   RecMdcKalTrackVector;
typedef std::vector <const Event::MdcMcHit*>  MdcMcHitVector;
typedef std::vector <const RecMdcHit*>        RecMdcHitVector;
typedef std::vector <const MdcDigi*>          MdcDigiVector;
typedef std::vector <const Event::EmcMcHit*>  EmcMcHitVector;
typedef std::vector <const RecEmcShower*>     RecEmcShowerVector;

static const CLID CLID_EventNavigator = 5101;

// Class EventNavigator

// Provides relation tables to make correspondence between BES3 
// reconstructed data and Monte-Carlo truth

// History: A.Zhemchugov. Created with MDC navigation, 20 Jan 2006 
//	    A.Zhemchugov. EMC navigation  added,       23 Nov 2006
//          A.Zhemchugov. Migrated to new EventModel,  03 Apr 2008
//          A.Zhemchugov. Cut on number of common hits in MDC introduced, 13 Jun 2009
//          A.Zhemchugov. Store Event Navigator to DST, 17 Aug 2011 
//          A.Zhemchugov. Add methods to access McParticle relevance, 07 Aug 2012

class EventNavigator : public DataObject                                                {
    
 public:
    
  EventNavigator() : DataObject() 
    {
	m_mdcCut = 7;
    }
    
  virtual ~EventNavigator() {}
    
  /// Retrieve reference to class definition structure
  virtual const CLID& clID() const { return EventNavigator::classID(); }
  static const CLID& classID() { return CLID_EventNavigator; }

  void Print();

  // Set cut on the minimum number of common hits of 
  // the MDC tracks and McParticles to make correspondence (default is 2)
  void setMdcCut( int cut ) { m_mdcCut = cut; };

  //******** Methods to retrieve objPtr via id *******
  const Event::McParticle* getMcParticle ( int id ) { return m_mcParticleId[id]; };
  // MDC
  const RecMdcTrack*   getMdcTrack   ( int id ) { return m_mdcTrackId[id]; };
  const RecMdcKalTrack*   getMdcKalTrack   ( int id ) { return m_mdcKalTrackId[id]; };

  // return MdcMcHits corresponding given detector Id
  MdcMcHitVector    getMdcMcHits   ( int id );

  const RecMdcHit*  getMdcRecHit  ( int id ) { return m_mdcRecHitId[id]; };
  const MdcDigi*    getMdcDigi    ( int id ) { return m_mdcDigiId[id]; };
  // EMC
  RecEmcShowerVector getEmcRecShowers ( int id );
  EmcMcHitVector     getEmcMcHits     ( int id );
  
  //******** Methods to handle persistency ************
  IndexMap& getMcMdcMcHitsIdx(){return m_mcMdcMcHitsIdx;}
  IndexMap& getMcMdcTracksIdx(){return m_mcMdcTracksIdx;}
  IndexMap& getMcEmcMcHitsIdx(){return m_mcEmcMcHitsIdx;}
  IndexMap& getMcEmcRecShowersIdx(){return m_mcEmcRecShowersIdx;}
  void setMcMdcMcHitsIdx(IndexMap& map){m_mcMdcMcHitsIdx=map;}
  void setMcMdcTracksIdx(IndexMap& map){m_mcMdcTracksIdx=map;}
  void setMcEmcMcHitsIdx(IndexMap& map){m_mcEmcMcHitsIdx=map;}
  void setMcEmcRecShowersIdx(IndexMap& map){m_mcEmcRecShowersIdx=map;}

  //******** Main methods to retrieve relations *******
  // MC particle
  McParticleVector& getMcParticles  ( const RecMdcTrack* key ); 
  McParticleVector& getMcParticles  ( const RecMdcKalTrack* key ); 
  McParticleVector& getMcParticles  ( const RecEmcShower* key );

  int getMcParticleRelevance( const RecMdcTrack* key, const Event::McParticle* value );
  int getMcParticleRelevance( const RecMdcKalTrack* key, const Event::McParticle* value );
  int getMcParticleRelevance( const RecEmcShower* key, const Event::McParticle* value );

  // MDC Tracks
  RecMdcTrackVector&    getMdcTracks     ( const Event::McParticle* key );
  RecMdcKalTrackVector& getMdcKalTracks  ( const Event::McParticle* key );

  // Reconstructed EMC showers
  RecEmcShowerVector&   getEmcRecShowers ( const Event::McParticle* key );

  //******** Auxillary methods to retrieve relations **
  /*
  RecMdcHitVector&  getMdcRecHits   ( const McParticle* key );
  MdcMcHitVector&   getMdcMcHits    ( const RecMdcTrack* key );
  RecMdcHitVector&  getMdcRecHits   ( const MdcMcHit* key );
  MdcMcHitVector&   getMdcMcHits    ( const RecMdcHit* key );
  
  EmcMcHitVector& getEmcMcHits     ( const RecEmcShower* key ); 
  */


  friend class BesNavigatorInit;

 protected:
  
  // ==== Methods to store id->objPtr lookup maps ====
  void addIdLink ( int id, Event::McParticle* ptr ) { m_mcParticleId[id] = ptr; };
  
  // MDC info
  void addIdLink ( int id, RecMdcTrack* ptr ) { 
    m_mdcTrackId[id] = ptr; 
  };
  
  void addIdLink ( int id, RecMdcKalTrack* ptr ) { 
    m_mdcKalTrackId[id] = ptr; 
  };

  void addIdLink ( unsigned int id, RecEmcShower* ptr ) { 
    m_emcRecShowerId.insert(pair<int, RecEmcShower*>(id,ptr)); 
  };
  
  // ==== Methods to store relations ====
  // store list of MdcTracks corresponding given McParticle
  void addLink( const Event::McParticle* key, const RecMdcTrack* value );
  
  // store list of McParticles corresponding given MdcTrack
  void addLink( const RecMdcTrack* key, const Event::McParticle* value );
  
  // store list of MdcKalTracks corresponding given McParticle
  void addLink( const Event::McParticle* key, const RecMdcKalTrack* value );
  
  // store list of McParticles corresponding given MdcKalTrack
  void addLink( const RecMdcKalTrack* key, const Event::McParticle* value );

  // store list of McParticles corresponding given EmcRecShower
  void addLink( const RecEmcShower* key, const Event::McParticle* value );
  
  // store list of EmcShowers corresponding given McParticle
  void addLink( const Event::McParticle* key, const RecEmcShower* value );
  
 private:
  // === lookup maps of id->objPtr relations ===
  std::map <int, const Event::McParticle*> m_mcParticleId;

  // MDC info
  std::map <int, const RecMdcTrack*>  m_mdcTrackId;
  std::map <int, const RecMdcKalTrack*>  m_mdcKalTrackId;
  std::map <int, const MdcDigi*>   m_mdcDigiId;
  std::multimap <unsigned int, const Event::MdcMcHit*>  m_mdcMcHitId;
  std::map <unsigned int, const RecMdcHit*> m_mdcRecHitId;

  // ===== relations =====
  // relation tables to be returned by query
  std::map <int, RecMdcTrackVector>  m_mdcTracksWrtMcParticle;
  std::map <int, McParticleVector>   m_mcParticlesWrtMdcTrack;
  std::map <int, RecMdcKalTrackVector>  m_mdcKalTracksWrtMcParticle;
  std::map <int, McParticleVector>   m_mcParticlesWrtMdcKalTrack;

  // internal version of relation tables to keep number of the same hits for each pair McParticle-MdcTrack
  std::map <int, RecMdcTrackVectorP>    m_mdcTracksWrtMcParticleP;
  std::map <int, McParticleVectorP>     m_mcParticlesWrtMdcTrackP;
  std::map <int, RecMdcKalTrackVectorP> m_mdcKalTracksWrtMcParticleP;
  std::map <int, McParticleVectorP>     m_mcParticlesWrtMdcKalTrackP;
 
  // EMC info
  std::multimap <unsigned int, const RecEmcShower*>  m_emcRecShowerId;
  std::multimap <unsigned int, const Event::EmcMcHit*>      m_emcMcHitId;

  // ===== relations =====
  std::map <int, McParticleVector>      m_mcParticlesWrtRecShower;
  std::map <int, RecEmcShowerVector>    m_emcRecShowersWrtMcHit;
  std::map <int, RecEmcShowerVector>    m_emcRecShowersWrtMcParticle;
  std::map <int, EmcMcHitVector>        m_emcMcHitsWrtRecShower;
  // internal version of relation tables to keep number of the same hits for each pair McParticle-EmcRecShower
  std::map <int, RecEmcShowerVectorP>   m_emcRecShowersWrtMcParticleP;
  std::map <int, McParticleVectorP>     m_mcParticlesWrtRecEmcShowerP;

  // persistency
  IndexMap m_mcMdcTracksIdx; // id is the same for RecMdcTrack and RecMdcKalTrack
  IndexMap m_mcMdcMcHitsIdx;
  IndexMap m_mcEmcRecShowersIdx;
  IndexMap m_mcEmcMcHitsIdx;

  int m_mdcCut;
};

#endif    // EventNavigator_H

