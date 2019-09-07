#include <iostream>

#include "EventNavigator/EventNavigator.h"

using namespace std;
using namespace Event;

// methods to retrieve physics objects corresponding given detector id

RecEmcShowerVector EventNavigator::getEmcRecShowers ( int id )
{ 
  RecEmcShowerVector tmp;
  multimap <unsigned int, const RecEmcShower*>::iterator it;
  for (it=m_emcRecShowerId.equal_range(id).first; 
       it != m_emcRecShowerId.equal_range(id).second; 
       it++)
    tmp.push_back((*it).second);
  return tmp;
}

void EventNavigator::Print() 
{ 
  cout << "EventNavigator object:" << std::endl;
  cout << "  Mdc hits:    " << m_mcMdcMcHitsIdx.size() << endl;
  cout << "  Mdc tracks:  " << m_mcMdcTracksIdx.size() << endl;
  cout << "  Emc hits:    " << m_mcEmcMcHitsIdx.size() << endl;
  cout << "  Emc showers: " << m_mcEmcRecShowersIdx.size() << endl;
  cout << "  TABLES:" << endl;
  cout << "   m_mdcTracksWrtMcParticle " 
       << m_mdcTracksWrtMcParticle.size() << endl;
  cout << "   m_mcParticlesWrtMdcTrack " 
       << m_mcParticlesWrtMdcTrack.size() << endl;
  cout << "   m_mdcKalTracksWrtMcParticle " 
       << m_mdcKalTracksWrtMcParticle.size() << endl;
  cout << "   m_mcParticlesWrtMdcKalTrack " 
       << m_mcParticlesWrtMdcKalTrack.size() << endl;
  cout << "   m_mdcTracksWrtMcParticleP " 
       << m_mdcTracksWrtMcParticleP.size() << endl;
  cout << "   m_mcParticlesWrtMdcTrackP " 
       << m_mcParticlesWrtMdcTrackP.size() << endl;
  cout << "   m_mdcKalTracksWrtMcParticle " 
       << m_mdcKalTracksWrtMcParticleP.size() << endl;
  cout << "   m_mcParticlesWrtMdcKalTrack " 
       << m_mcParticlesWrtMdcKalTrackP.size() << endl;
  cout << "   m_mcParticlesWrtRecShower " 
       << m_mcParticlesWrtRecShower.size() << endl;
  cout << "   m_emcRecShowersWrtMcParticle " 
       << m_emcRecShowersWrtMcParticle.size() << endl; 
  cout << "   m_mcParticlesWrtEmcShowerP " 
       << m_mcParticlesWrtRecEmcShowerP.size() << endl;
  cout << "   m_emcRecShowersWrtMcParticle " 
       << m_emcRecShowersWrtMcParticleP.size() << endl;
}


//=====================================
// MDC

McParticleVector& EventNavigator::getMcParticles ( const RecMdcTrack* key )  
{
  return m_mcParticlesWrtMdcTrack[key->trackId()];
}

McParticleVector& EventNavigator::getMcParticles ( const RecMdcKalTrack* key )  
{
  return m_mcParticlesWrtMdcKalTrack[key->trackId()];
}

McParticleVector& EventNavigator::getMcParticles (const RecEmcShower* key ) 
{
  return m_mcParticlesWrtRecShower[key->getShowerId().get_value()];
}


RecMdcTrackVector& EventNavigator::getMdcTracks  ( const McParticle* key ) 
{
  return m_mdcTracksWrtMcParticle[key->trackIndex()];
}

RecMdcKalTrackVector& EventNavigator::getMdcKalTracks  ( const McParticle* key ) 
{
  return m_mdcKalTracksWrtMcParticle[key->trackIndex()];
}

// Return number of hits contributed by the McParticle to given MdcTrack
int EventNavigator::getMcParticleRelevance( const RecMdcTrack* key, const McParticle* value )
{
   McParticleVectorP& vpair = m_mcParticlesWrtMdcTrackP[key->trackId()];                                
   McParticleVectorP::iterator it;                                                                      
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {                                                                                                   
     if ( (*it).first == value )                                                                       
     {                                                                                               
       return (*it).second;
     }
   }                                                                                               
}

int EventNavigator::getMcParticleRelevance( const RecMdcKalTrack* key, const McParticle* value )     
{
   McParticleVectorP& vpair = m_mcParticlesWrtMdcKalTrackP[key->trackId()];                                
   McParticleVectorP::iterator it;                                                                      
   for (it=vpair.begin(); it!=vpair.end(); it++)
   {                                                                                                   
     if ( (*it).first == value )                                                                       
     {                                                                                               
       return (*it).second;
     }
   }                                                                                               
}   

int EventNavigator::getMcParticleRelevance( const RecEmcShower* key, const McParticle* value )     
{
   McParticleVectorP& vpair = m_mcParticlesWrtRecEmcShowerP[key->getShowerId().get_value()];                                
   McParticleVectorP::iterator it;                                                                      
   for (it=vpair.begin(); it!=vpair.end(); it++)
   {                                                                                                   
     if ( (*it).first == value )                                                                       
     {                                                                                               
       return (*it).second;
     }
   }                                                                                               
}   

RecEmcShowerVector& EventNavigator::getEmcRecShowers ( const McParticle* key ) 
{ 
  return m_emcRecShowersWrtMcParticle[key->trackIndex()];
}

// ==== Methods to store relations ====
// store list of MdcTracks corresponding given McParticle
void EventNavigator::addLink( const McParticle* key, const RecMdcTrack* value )
{
  RecMdcTrackVectorP& vpair = m_mdcTracksWrtMcParticleP[key->trackIndex()];
  RecMdcTrackVectorP::iterator it;
  bool found = false;
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
     if ( (*it).first == value )
       {
         (*it).second++;
         found = true;
         break;
       }
   } 

   if( ! found )
     vpair.push_back( pair<const RecMdcTrack*,int>(value,1));

  RecMdcTrackVector& tmp = m_mdcTracksWrtMcParticle[key->trackIndex()];
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
     if((*it).second > m_mdcCut)
     {
       if (find(tmp.begin(),tmp.end(),(*it).first) == tmp.end() ) 
	{
    	   tmp.push_back((*it).first);
    	}
     }
   }
}

// store list of MdcKalTracks corresponding given McParticle
void EventNavigator::addLink( const McParticle* key, const RecMdcKalTrack* value )
{
  RecMdcKalTrackVectorP& vpair = m_mdcKalTracksWrtMcParticleP[key->trackIndex()];
  RecMdcKalTrackVectorP::iterator it;
  bool found = false;
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
     if ( (*it).first == value )
       {
         (*it).second++;
         found = true;
         break;
       }
   } 

   if( ! found )
     vpair.push_back( pair<const RecMdcKalTrack*,int>(value,1));

  RecMdcKalTrackVector& tmp = m_mdcKalTracksWrtMcParticle[key->trackIndex()];
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
     if((*it).second > m_mdcCut)
     {
       if (find(tmp.begin(),tmp.end(),(*it).first) == tmp.end() ) 
	{
    	   tmp.push_back((*it).first);
    	}
     }
   }
}

// store list of McParticles corresponding given MdcTrack
void EventNavigator::addLink( const RecMdcTrack* key, const McParticle* value )
{
  McParticleVectorP& vpair = m_mcParticlesWrtMdcTrackP[key->trackId()];
  McParticleVectorP::iterator it;
  bool found = false;
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
     if ( (*it).first == value )
       {
         (*it).second++;
         found = true;
         break;
       }
   } 

   if( ! found )
     vpair.push_back( pair<const McParticle*,int>(value,1));

  McParticleVector& tmp = m_mcParticlesWrtMdcTrack[key->trackId()];
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
     if((*it).second > m_mdcCut)
     {
       if (find(tmp.begin(),tmp.end(),(*it).first) == tmp.end() ) 
	{
    	   tmp.push_back((*it).first);
    	}
     }
   }
}


// store list of McParticles corresponding given MdcKalTrack
void EventNavigator::addLink( const RecMdcKalTrack* key, const McParticle* value )
{
  McParticleVectorP& vpair = m_mcParticlesWrtMdcKalTrackP[key->trackId()];
  McParticleVectorP::iterator it;
  bool found = false;
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
     if ( (*it).first == value )
       {
         (*it).second++;
         found = true;
         break;
       }
   } 

   if( ! found )
     vpair.push_back( pair<const McParticle*,int>(value,1));

  McParticleVector& tmp = m_mcParticlesWrtMdcKalTrack[key->trackId()];
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
     if((*it).second > m_mdcCut)
     {
       if (find(tmp.begin(),tmp.end(),(*it).first) == tmp.end() ) 
	{
    	   tmp.push_back((*it).first);
    	}
     }
   }
}

// store list of McParticles corresponding given EmcRecShower
void EventNavigator::addLink( const RecEmcShower* key, const McParticle* value )
{
//  McParticleVector& tmp = m_mcParticlesWrtRecShower[((RecEmcShower*)key)->getShowerId().get_value()];
//  if (find(tmp.begin(),tmp.end(),value) == tmp.end() ) 
//    tmp.push_back(value);

  McParticleVectorP& vpair = m_mcParticlesWrtRecEmcShowerP[((RecEmcShower*)key)->getShowerId().get_value()];
  McParticleVectorP::iterator it;
  bool found = false;
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
     if ( (*it).first == value )
       {
         (*it).second++;
         found = true;
         break;
       }
   } 

   if( ! found )
     vpair.push_back( pair<const McParticle*,int>(value,1));

  McParticleVector& tmp = m_mcParticlesWrtRecShower[((RecEmcShower*)key)->getShowerId().get_value()];
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
       if (find(tmp.begin(),tmp.end(),(*it).first) == tmp.end() ) 
	{
    	   tmp.push_back((*it).first);
    	}
   }
}

// store list of EmcRecShowers corresponding given McParticle
void EventNavigator::addLink( const McParticle* key, const RecEmcShower* value )
{
//  RecEmcShowerVector& tmp = m_emcRecShowersWrtMcParticle[key->trackIndex()];
//  if (find(tmp.begin(),tmp.end(),value) == tmp.end() ) 
//    tmp.push_back(value);
  RecEmcShowerVectorP& vpair = m_emcRecShowersWrtMcParticleP[key->trackIndex()];
  RecEmcShowerVectorP::iterator it;
  bool found = false;
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
     if ( (*it).first == value )
       {
         (*it).second++;
         found = true;
         break;
       }
   } 

   if( ! found )
     vpair.push_back( pair<const RecEmcShower*,int>(value,1));

  RecEmcShowerVector& tmp = m_emcRecShowersWrtMcParticle[key->trackIndex()];
  for (it=vpair.begin(); it!=vpair.end(); it++)
   {
       if (find(tmp.begin(),tmp.end(),(*it).first) == tmp.end() ) 
	{
    	   tmp.push_back((*it).first);
    	}
   }
}

