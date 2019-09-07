#include "ZddEvent/RecZddChannel.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecVeeVertex.h"
#include "EvtRecEvent/EvtRecPi0.h"
#include "EvtRecEvent/EvtRecEtaToGG.h"
#include "EvtRecEvent/EvtRecDTag.h"
#include "RootCnvSvc/commonData.h"
 // common data for the 3 different trees. This data is needed 
 // when writing/reading relations which cross the boundaries of
 // Monte Carlo, Digitization, and Reconstruction data.


std::map<const MdcDigi*, TRef> commonData::m_mdcDigiMap;
std::map<const TObject*, const MdcDigi*>          commonData::m_rootMdcDigiMap;

std::map<const EmcDigi*, TRef>  commonData::m_emcDigiMap;
std::map<const TObject*, const  EmcDigi*>    commonData::m_rootEmcDigiMap;

std::map<const TofDigi*, TRef>  commonData::m_tofDigiMap;
std::map<const TObject*, const  TofDigi*>    commonData::m_rootTofDigiMap;

std::map<const MucDigi*, TRef>  commonData::m_mucDigiMap;
std::map<const TObject*, const  MucDigi*>    commonData::m_rootMucDigiMap;

std::map<const LumiDigi*, TRef>  commonData::m_lumiDigiMap;
std::map<const TObject*, const  LumiDigi*>    commonData::m_rootLumiDigiMap;

std::map<const DstEmcShower*, TRef>  commonData::m_emcShowerMap;
std::map<const TObject*, const  DstEmcShower*>    commonData::m_rootEmcShowerMap;

std::map<const DstMdcTrack*, TRef>  commonData::m_mdcTrackMap;
std::map<const TObject*, const  DstMdcTrack*>    commonData::m_rootMdcTrackMap; 

std::map<const DstMucTrack*, TRef>  commonData::m_mucTrackMap;
std::map<const TObject*, const  DstMucTrack*>    commonData::m_rootMucTrackMap; 

std::map<const DstTofTrack*, TRef>  commonData::m_tofTrackMap;
std::map<const TObject*, const  DstTofTrack*>    commonData::m_rootTofTrackMap; 

std::map<const DstMdcDedx*, TRef>  commonData::m_mdcDedxMap;
std::map<const TObject*, const  DstMdcDedx*>    commonData::m_rootMdcDedxMap; 

std::map<const DstExtTrack*, TRef>  commonData::m_extTrackMap;
std::map<const TObject*, const  DstExtTrack*>    commonData::m_rootExtTrackMap;

std::map<const DstMdcKalTrack*, TRef>  commonData::m_mdcKalTrackMap;          // Add on Oct.10
std::map<const TObject*, const  DstMdcKalTrack*>    commonData::m_rootMdcKalTrackMap; // Add on Oct.10



//
std::map<const RecEmcHit*, TRef>  commonData::m_recEmcHitMap;
std::map<const TObject*, const  RecEmcHit*>    commonData::m_rootRecEmcHitMap;

std::map<const RecMdcTrack*, TRef>  commonData::m_recMdcTrackMap;
std::map<const TObject*, const  RecMdcTrack*>    commonData::m_rootRecMdcTrackMap; 

std::map<const RecMdcHit*, TRef>  commonData::m_recMdcHitMap;
std::map<const TObject*, const  RecMdcHit*>    commonData::m_rootRecMdcHitMap; 

std::map<const RecEmcCluster*, TRef>  commonData::m_recEmcClusterMap;
std::map<const TObject*, const  RecEmcCluster*>    commonData::m_rootRecEmcClusterMap;

std::map<const RecEmcShower*, TRef>  commonData::m_recEmcShowerMap;
std::map<const TObject*, const  RecEmcShower*>    commonData::m_rootRecEmcShowerMap;

std::map<const RecTofTrack*, TRef>  commonData::m_RecTofTrackMap;
std::map<const TObject*, const  RecTofTrack*>    commonData::m_rootRecTofTrackMap; 

std::map<const RecMucTrack*, TRef>  commonData::m_RecMucTrackMap;
std::map<const TObject*, const  RecMucTrack*>    commonData::m_rootRecMucTrackMap; 

std::map<const RecMdcDedx*, TRef>  commonData::m_recMdcDedxMap;
std::map<const TObject*, const  RecMdcDedx*>    commonData::m_rootRecMdcDedxMap;

std::map<const RecMdcDedxHit*, TRef>  commonData::m_recMdcDedxHitMap;
std::map<const TObject*, const  RecMdcDedxHit*>    commonData::m_rootRecMdcDedxHitMap;

std::map<const RecMdcKalTrack*, TRef>  commonData::m_recMdcKalTrackMap;          // Add on Oct.10
std::map<const TObject*, const  RecMdcKalTrack*>    commonData::m_rootRecMdcKalTrackMap; // Add on Oct.10

std::map<const RecMdcKalHelixSeg*, TRef>  commonData::m_recMdcKalHelixSegMap;          
std::map<const TObject*, const  RecMdcKalHelixSeg*>    commonData::m_rootRecMdcKalHelixSegMap; 

std::map<const RecEsTime*, TRef>  commonData::m_RecEvTimeMap;
std::map<const TObject*, const  RecEsTime*>    commonData::m_rootRecEvTimeMap;

// Event after EventAssembly
//
std::map<const EvtRecTrack*, TRef>  commonData::m_EvtRecTrackMap;
std::map<const TObject*, const EvtRecTrack*> commonData::m_rootEvtRecTrackMap;

std::map<const EvtRecVeeVertex*, TRef>  commonData::m_evtRecVeeVertexMap;
std::map<const TObject*, const  EvtRecVeeVertex*>    commonData::m_rootEvtRecVeeVertexMap;


std::map<const EvtRecPi0*, TRef>  commonData::m_evtRecPi0Map;
std::map<const TObject*, const  EvtRecPi0*>    commonData::m_rootEvtRecPi0Map;


std::map<const EvtRecEtaToGG*, TRef>  commonData::m_evtRecEtaToGGMap;
std::map<const TObject*, const  EvtRecEtaToGG*>    commonData::m_rootEvtRecEtaToGGMap;

std::map<const EvtRecDTag*, TRef>  commonData::m_evtRecDTagMap;
std::map<const TObject*, const  EvtRecDTag*>    commonData::m_rootEvtRecDTagMap;

std::map<const RecZddChannel*, TRef>  commonData::m_recZddChannelMap;
std::map<const TObject*, const  RecZddChannel*>    commonData::m_rootRecZddChannelMap;


//for McTruth
std::map<const MdcMcHit*, TRef> commonData::m_mdcMcHitMap;
std::map<const TObject*, const MdcMcHit*>          commonData::m_rootMdcMcHitMap;

std::map<const EmcMcHit*, TRef>  commonData::m_emcMcHitMap;
std::map<const TObject*, const  EmcMcHit*>    commonData::m_rootEmcMcHitMap;

std::map<const TofMcHit*, TRef>  commonData::m_tofMcHitMap;
std::map<const TObject*, const  TofMcHit*>    commonData::m_rootTofMcHitMap;

std::map<const MucMcHit*, TRef>  commonData::m_mucMcHitMap;
std::map<const TObject*, const  MucMcHit*>    commonData::m_rootMucMcHitMap;

std::map<const McParticle*, TRef>  commonData::m_mcParticleMap;
std::map<const TObject*, const  McParticle*>    commonData::m_rootMcParticleMap;

//std::map<const DecayMode*, TRef>  commonData::m_decayModeMap;
//std::map<const TObject*, const  DecayMode*>    commonData::m_rootDecayModeMap;

std::map<const TrigData*, TRef>  commonData::m_trigDataMap; //caogf
std::map<const TObject*, const  TrigData*>    commonData::m_rootTrigDataMap; //caogf

std::map<const HltRaw*, TRef>  commonData::m_hltRawMap; //fucd
std::map<const TObject*, const  HltRaw*>    commonData::m_rootHltRawMap; //fucd
std::map<const HltInf*, TRef>  commonData::m_hltInfMap; //fucd
std::map<const TObject*, const  HltInf*>    commonData::m_rootHltInfMap; //fucd
std::map<const DstHltInf*, TRef>  commonData::m_dstHltInfMap; //fucd
std::map<const TObject*, const  DstHltInf*>    commonData::m_rootDstHltInfMap; //fucd

TDigiEvent* commonData::m_recEvt = NULL;
TDstEvent* commonData::m_dstEvt = NULL;
TMcEvent* commonData::m_mcEvt = NULL;
TRecTrackEvent* commonData::m_rectrackEvt = NULL;
TEvtHeader* commonData::m_EvtHeader = NULL;
TEvtNavigator* commonData::m_EvtNavigator = NULL;
TTrigEvent* commonData::m_trigEvt = NULL;
THltEvent* commonData::m_hltEvt = NULL;
TEvtRecObject* commonData::m_evtRecObject = NULL;

void commonData::clear() { 
//for digi

  if (m_recEvt != NULL) {
    delete m_recEvt;
    m_recEvt = NULL;
  }
  if (m_dstEvt != NULL) {
    delete m_dstEvt;
    m_dstEvt = NULL;
  }
  if (m_mcEvt != NULL) {
    delete m_mcEvt;
    m_mcEvt = NULL;
  }
  if (m_rectrackEvt != NULL) {  //2007-10-04
    delete m_rectrackEvt;
    m_rectrackEvt = NULL;
  }
  if (m_EvtHeader != NULL){
    delete m_EvtHeader;
    m_EvtHeader = NULL;
  }
  if (m_EvtNavigator != NULL){
    delete m_EvtNavigator;
    m_EvtNavigator = NULL;
  }
  if(m_trigEvt != NULL){
    delete m_trigEvt;
    m_trigEvt = NULL;
  }
  if(m_hltEvt != NULL){
    delete m_hltEvt;
    m_hltEvt = NULL;
  } 
  if ( m_evtRecObject != NULL ) {
     delete m_evtRecObject;
     m_evtRecObject = NULL;
  }
  
  for (std::map<const MdcDigi*, TRef>::iterator iter = m_mdcDigiMap.begin();
      iter != m_mdcDigiMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_mdcDigiMap.clear();
  for (std::map<const TObject*, const MdcDigi*>::iterator iter = m_rootMdcDigiMap.begin();
      iter != m_rootMdcDigiMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootMdcDigiMap.clear();
  
  for (std::map<const EmcDigi*, TRef>::iterator iter = m_emcDigiMap.begin();
      iter != m_emcDigiMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_emcDigiMap.clear();
  for (std::map<const TObject*, const  EmcDigi*>::iterator iter = m_rootEmcDigiMap.begin();
      iter != m_rootEmcDigiMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootEmcDigiMap.clear();
  
  for (std::map<const TofDigi*, TRef>::iterator iter = m_tofDigiMap.begin();
      iter != m_tofDigiMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_tofDigiMap.clear();
  for (std::map<const TObject*, const  TofDigi*>::iterator iter = m_rootTofDigiMap.begin();
      iter != m_rootTofDigiMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootTofDigiMap.clear();
  
  for (std::map<const MucDigi*, TRef>::iterator iter = m_mucDigiMap.begin();
      iter != m_mucDigiMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_mucDigiMap.clear();
  for (std::map<const TObject*, const  MucDigi*>::iterator iter = m_rootMucDigiMap.begin();
      iter != m_rootMucDigiMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootMucDigiMap.clear();

  for ( std::map<const LumiDigi*, TRef>::iterator iter = m_lumiDigiMap.begin();
	iter != m_lumiDigiMap.end(); ++iter) {
     delete (*iter).second.GetObject();
     delete (*iter).first;
  }
  m_lumiDigiMap.clear();
  for ( std::map<const TObject*, const LumiDigi*>::iterator iter = m_rootLumiDigiMap.begin();
	iter != m_rootLumiDigiMap.end(); ++iter) {
     delete (*iter).first;
  }
  m_rootLumiDigiMap.clear();
  
  //for dst
  for (std::map<const DstEmcShower*, TRef>::iterator iter = m_emcShowerMap.begin();
      iter != m_emcShowerMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_emcShowerMap.clear();
  for (std::map<const TObject*, const  DstEmcShower*>::iterator iter = m_rootEmcShowerMap.begin();
      iter != m_rootEmcShowerMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootEmcShowerMap.clear();
  /*
  for (std::map<const DstMdcTrack*, TRef>::iterator p = m_mdcTrackMap.begin(); p != m_mdcTrackMap.end();p++){
    std::cout << "********The first is ****************" << (*p).first <<endl;
    //std::cout << "*********The second is " << (*p).second <<endl;
    }
  */
  for (std::map<const DstMdcTrack*, TRef>::iterator iter = m_mdcTrackMap.begin();
      iter != m_mdcTrackMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_mdcTrackMap.clear();
  for (std::map<const TObject*, const  DstMdcTrack*>::iterator iter = m_rootMdcTrackMap.begin();
      iter != m_rootMdcTrackMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootMdcTrackMap.clear();
  
  for (std::map<const DstMucTrack*, TRef>::iterator iter = m_mucTrackMap.begin();
      iter != m_mucTrackMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_mucTrackMap.clear();
  for (std::map<const TObject*, const  DstMucTrack*>::iterator iter = m_rootMucTrackMap.begin();
      iter != m_rootMucTrackMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootMucTrackMap.clear(); 
  
  for (std::map<const DstTofTrack*, TRef>::iterator iter = m_tofTrackMap.begin();
      iter != m_tofTrackMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_tofTrackMap.clear();
  for (std::map<const TObject*, const  DstTofTrack*>::iterator iter = m_rootTofTrackMap.begin();
      iter != m_rootTofTrackMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootTofTrackMap.clear(); 
  
  for (std::map<const DstMdcDedx*, TRef>::iterator iter = m_mdcDedxMap.begin();
      iter != m_mdcDedxMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_mdcDedxMap.clear();
  for (std::map<const TObject*, const  DstMdcDedx*>::iterator iter = m_rootMdcDedxMap.begin();
      iter != m_rootMdcDedxMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootMdcDedxMap.clear(); 
  
  for (std::map<const DstExtTrack*, TRef>::iterator iter = m_extTrackMap.begin();
      iter != m_extTrackMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_extTrackMap.clear();
  for (std::map<const TObject*, const  DstExtTrack*>::iterator iter = m_rootExtTrackMap.begin();
      iter != m_rootExtTrackMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootExtTrackMap.clear();
  
  for (std::map<const DstMdcKalTrack*, TRef>::iterator iter = m_mdcKalTrackMap.begin();
      iter != m_mdcKalTrackMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_mdcKalTrackMap.clear();
  for (std::map<const TObject*, const  DstMdcKalTrack*>::iterator iter = m_rootMdcKalTrackMap.begin();
      iter != m_rootMdcKalTrackMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootMdcKalTrackMap.clear();
  // for McTruth
  for (std::map<const MdcMcHit*, TRef>::iterator iter = m_mdcMcHitMap.begin();
      iter != m_mdcMcHitMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_mdcMcHitMap.clear();
  for (std::map<const TObject*, const MdcMcHit*>::iterator iter = m_rootMdcMcHitMap.begin();
      iter != m_rootMdcMcHitMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootMdcMcHitMap.clear();
  
  for (std::map<const EmcMcHit*, TRef>::iterator iter = m_emcMcHitMap.begin();
      iter != m_emcMcHitMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_emcMcHitMap.clear();
  for (std::map<const TObject*, const  EmcMcHit*>::iterator iter = m_rootEmcMcHitMap.begin();
      iter != m_rootEmcMcHitMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootEmcMcHitMap.clear();
  
  for (std::map<const TofMcHit*, TRef>::iterator iter = m_tofMcHitMap.begin();
      iter != m_tofMcHitMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_tofMcHitMap.clear();
  for (std::map<const TObject*, const  TofMcHit*>::iterator iter = m_rootTofMcHitMap.begin();
      iter != m_rootTofMcHitMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootTofMcHitMap.clear();
  
  for (std::map<const MucMcHit*, TRef>::iterator iter = m_mucMcHitMap.begin();
      iter != m_mucMcHitMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_mucMcHitMap.clear();
  for (std::map<const TObject*, const  MucMcHit*>::iterator iter = m_rootMucMcHitMap.begin();
      iter != m_rootMucMcHitMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootMucMcHitMap.clear();
  
  for (std::map<const McParticle*, TRef>::iterator iter = m_mcParticleMap.begin();
      iter != m_mcParticleMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_mcParticleMap.clear();
  for (std::map<const TObject*, const  McParticle*>::iterator iter = m_rootMcParticleMap.begin();
      iter != m_rootMcParticleMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootMcParticleMap.clear();

  //m_decayModeMap.clear();
  //m_rootDecayModeMap.clear();

//m_decayModeMap.clear();
//m_rootDecayModeMap.clear();

  //for Trig
  m_trigDataMap.clear();
  m_rootTrigDataMap.clear();

  //for event filter
  for (std::map<const HltRaw*, TRef>::iterator iter = m_hltRawMap.begin();
       iter != m_hltRawMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_hltRawMap.clear();
  for (std::map<const TObject*, const  HltRaw*>::iterator iter = m_rootHltRawMap.begin();
       iter != m_rootHltRawMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootHltRawMap.clear();
  m_hltInfMap.clear();
  m_rootHltInfMap.clear();
  m_dstHltInfMap.clear();
  m_rootDstHltInfMap.clear();
  //for Rec
  //Mdc
  for (std::map<const RecMdcTrack*, TRef>::iterator iter = m_recMdcTrackMap.begin();
      iter != m_recMdcTrackMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_recMdcTrackMap.clear();
  for (std::map<const TObject*, const  RecMdcTrack*>::iterator iter = m_rootRecMdcTrackMap.begin();
      iter != m_rootRecMdcTrackMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootRecMdcTrackMap.clear(); 
  for (std::map<const RecMdcHit*, TRef>::iterator iter = m_recMdcHitMap.begin();
      iter != m_recMdcHitMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_recMdcHitMap.clear();
  for (std::map<const TObject*, const  RecMdcHit*>::iterator iter = m_rootRecMdcHitMap.begin();
      iter != m_rootRecMdcHitMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootRecMdcHitMap.clear(); 

//for Rec
  for (std::map<const RecEmcHit*, TRef>::iterator iter = m_recEmcHitMap.begin();
      iter != m_recEmcHitMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_recEmcHitMap.clear();
  for (std::map<const TObject*, const  RecEmcHit*>::iterator iter = m_rootRecEmcHitMap.begin();
      iter != m_rootRecEmcHitMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootRecEmcHitMap.clear();

  for (std::map<const RecEmcCluster*, TRef>::iterator iter = m_recEmcClusterMap.begin();
      iter != m_recEmcClusterMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_recEmcClusterMap.clear();
  for (std::map<const TObject*, const  RecEmcCluster*>::iterator iter = m_rootRecEmcClusterMap.begin();
      iter != m_rootRecEmcClusterMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootRecEmcClusterMap.clear();

  for (std::map<const RecEmcShower*, TRef>::iterator iter = m_recEmcShowerMap.begin();
      iter != m_recEmcShowerMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_recEmcShowerMap.clear();
  for (std::map<const TObject*, const  RecEmcShower*>::iterator iter = m_rootRecEmcShowerMap.begin();
      iter != m_rootRecEmcShowerMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootRecEmcShowerMap.clear();

  for (std::map<const RecTofTrack*, TRef>::iterator iter = m_RecTofTrackMap.begin();
       iter != m_RecTofTrackMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_RecTofTrackMap.clear();
  for (std::map<const TObject*, const  RecTofTrack*>::iterator iter = m_rootRecTofTrackMap.begin();
      iter != m_rootRecTofTrackMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootRecTofTrackMap.clear(); 

  for (std::map<const RecMucTrack*, TRef>::iterator iter = m_RecMucTrackMap.begin();
      iter != m_RecMucTrackMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_RecMucTrackMap.clear();
  for (std::map<const TObject*, const  RecMucTrack*>::iterator iter = m_rootRecMucTrackMap.begin();
      iter != m_rootRecMucTrackMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootRecMucTrackMap.clear(); 
  
  for (std::map<const RecMdcDedx*, TRef>::iterator iter = m_recMdcDedxMap.begin();
      iter != m_recMdcDedxMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_recMdcDedxMap.clear();
  for (std::map<const TObject*, const  RecMdcDedx*>::iterator iter =  m_rootRecMdcDedxMap.begin();
      iter !=  m_rootRecMdcDedxMap.end(); iter++) {
    delete (*iter).first;
  }
   m_rootRecMdcDedxMap.clear();


  for (std::map<const RecMdcDedxHit*, TRef>::iterator iter = m_recMdcDedxHitMap.begin();
      iter != m_recMdcDedxHitMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_recMdcDedxHitMap.clear();
  for (std::map<const TObject*, const  RecMdcDedxHit*>::iterator iter =  m_rootRecMdcDedxHitMap.begin();
      iter !=  m_rootRecMdcDedxHitMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootRecMdcDedxHitMap.clear();

  for (std::map<const RecMdcKalTrack*, TRef>::iterator iter = m_recMdcKalTrackMap.begin();
      iter != m_recMdcKalTrackMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_recMdcKalTrackMap.clear();
  for (std::map<const TObject*, const  RecMdcKalTrack*>::iterator iter = m_rootRecMdcKalTrackMap.begin();
      iter != m_rootRecMdcKalTrackMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootRecMdcKalTrackMap.clear(); 


  for (std::map<const RecMdcKalHelixSeg*, TRef>::iterator iter = m_recMdcKalHelixSegMap.begin();
      iter != m_recMdcKalHelixSegMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_recMdcKalHelixSegMap.clear();
  for (std::map<const TObject*, const  RecMdcKalHelixSeg*>::iterator iter = m_rootRecMdcKalHelixSegMap.begin();
      iter != m_rootRecMdcKalHelixSegMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootRecMdcKalHelixSegMap.clear(); 

  // RecZddChannel
  for (std::map<const RecZddChannel*, TRef>::iterator iter = m_recZddChannelMap.begin();
      iter != m_recZddChannelMap.end(); ++iter) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_recZddChannelMap.clear();
  for (std::map<const TObject*, const  RecZddChannel*>::iterator iter = m_rootRecZddChannelMap.begin();
      iter != m_rootRecZddChannelMap.end(); ++iter) {
    delete (*iter).first;
  }
  m_rootRecZddChannelMap.clear();

//Estime
    
  for (std::map<const RecEsTime*, TRef>::iterator iter = m_RecEvTimeMap.begin();
      iter != m_RecEvTimeMap.end(); iter++) {
    delete (*iter).second.GetObject(); 
    delete (*iter).first;
  } 
  m_RecEvTimeMap.clear();
  for (std::map<const TObject*, const  RecEsTime*>::iterator iter = m_rootRecEvTimeMap.begin();
      iter != m_rootRecEvTimeMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootRecEvTimeMap.clear();

  // for event after EventAssembly
  for (std::map<const EvtRecTrack*, TRef>::iterator iter = m_EvtRecTrackMap.begin();
	iter != m_EvtRecTrackMap.end(); iter++ ) {
     delete (*iter).second.GetObject();
     delete (*iter).first;
  }
  m_EvtRecTrackMap.clear();
  for (std::map<const TObject*, const EvtRecTrack*>::iterator iter = m_rootEvtRecTrackMap.begin();
	iter != m_rootEvtRecTrackMap.end(); iter++) {
     delete iter->first;
  }
  m_rootEvtRecTrackMap.clear();

  // EvtRecVeeVertexCol
  for (std::map<const EvtRecVeeVertex*, TRef>::iterator iter = m_evtRecVeeVertexMap.begin();
      iter != m_evtRecVeeVertexMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_evtRecVeeVertexMap.clear();
  for (std::map<const TObject*, const  EvtRecVeeVertex*>::iterator iter = m_rootEvtRecVeeVertexMap.begin();
      iter != m_rootEvtRecVeeVertexMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootEvtRecVeeVertexMap.clear();

  //EvtRecPi0Col
  for (std::map<const EvtRecPi0*, TRef>::iterator iter = m_evtRecPi0Map.begin();
      iter != m_evtRecPi0Map.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_evtRecPi0Map.clear();
  for (std::map<const TObject*, const  EvtRecPi0*>::iterator iter = m_rootEvtRecPi0Map.begin();
      iter != m_rootEvtRecPi0Map.end(); iter++) {
    delete (*iter).first;
  }
  m_rootEvtRecPi0Map.clear();


  //EvtRecEtaToGGCol
  for (std::map<const EvtRecEtaToGG*, TRef>::iterator iter = m_evtRecEtaToGGMap.begin();
      iter != m_evtRecEtaToGGMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_evtRecEtaToGGMap.clear();
  for (std::map<const TObject*, const  EvtRecEtaToGG*>::iterator iter = m_rootEvtRecEtaToGGMap.begin();
      iter != m_rootEvtRecEtaToGGMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootEvtRecEtaToGGMap.clear();

  

  // EvtRecDTagCol
  for (std::map<const EvtRecDTag*, TRef>::iterator iter = m_evtRecDTagMap.begin();
      iter != m_evtRecDTagMap.end(); iter++) {
    delete (*iter).second.GetObject();
    delete (*iter).first;
  }
  m_evtRecDTagMap.clear();
  for (std::map<const TObject*, const  EvtRecDTag*>::iterator iter = m_rootEvtRecDTagMap.begin();
      iter != m_rootEvtRecDTagMap.end(); iter++) {
    delete (*iter).first;
  }
  m_rootEvtRecDTagMap.clear();

}
