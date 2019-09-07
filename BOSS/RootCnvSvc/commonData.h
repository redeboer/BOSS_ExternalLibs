#ifndef ROOTIO_COMMONDATA_H
#define ROOTIO_COMMONDATA_H 1


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Algorithm.h"

#include "EventModel/Event.h"
#include "EventModel/EventModel.h"


#include "RawEvent/DigiEvent.h"

#include "TFile.h"
#include "TTree.h"
#include "TRef.h"

//Digi
#include "RootEventData/TDigiEvent.h"
#include "MdcRawEvent/MdcDigi.h"
#include "EmcRawEvent/EmcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "MucRawEvent/MucDigi.h"
#include "LumiDigi/LumiDigi.h"

//Dst
#include "RootEventData/TDstEvent.h"
#include "DstEvent/DstMdcTrack.h"
#include "DstEvent/DstEmcShower.h"
#include "DstEvent/DstTofTrack.h"
#include "DstEvent/DstMucTrack.h"
#include "DstEvent/DstMdcDedx.h"
#include "DstEvent/DstExtTrack.h"
#include "DstEvent/DstMdcKalTrack.h" // Add On Oct.10 wensp

// McTruth information
#include "RootEventData/TMcEvent.h"
#include "McTruth/MdcMcHit.h"
#include "McTruth/EmcMcHit.h"
#include "McTruth/TofMcHit.h"
#include "McTruth/MucMcHit.h"
#include "McTruth/McParticle.h"
//#include "McTruth/DecayMode.h"

//trigger information
#include "RootEventData/TTrigEvent.h"
#include "TrigEvent/TrigData.h"

//event filter information
#include "RootEventData/THltEvent.h"
#include "HltEvent/HltRaw.h"
#include "HltEvent/HltInf.h"
#include "HltEvent/DstHltInf.h"

//------------liangyt
#include "RootEventData/TRecTrackEvent.h"
#include "RootEventData/TRecTofTrack.h"
#include "RootEventData/TRecMdcTrack.h"
#include "RootEventData/TRecMdcHit.h"
#include "RootEventData/TRecEvTime.h"
#include "RootEventData/TRecEmcHit.h"
#include "RootEventData/TRecEmcCluster.h"
#include "RootEventData/TRecEmcShower.h"
#include "RootEventData/TRecMucTrack.h"
#include "RootEventData/TRecMdcDedx.h"
#include "RootEventData/TRecMdcDedxHit.h"
#include "RootEventData/TEvtHeader.h"
#include "RootEventData/TRecMdcKalTrack.h"
#include "RootEventData/TRecMdcKalHelixSeg.h"
#include "RootEventData/TEvtNavigator.h"

#include "RootEventData/TEvtRecObject.h"

#include "TofRecEvent/RecTofTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcKalHelixSeg.h"
#include "MdcRecEvent/RecMdcDedx.h"
#include "MdcRecEvent/RecMdcDedxHit.h"
#include "EmcRecEventModel/RecEmcShower.h"
#include "MucRecEvent/RecMucTrack.h"
#include "MucRecEvent/RecMucTrack.h"
#include "EvTimeEvent/RecEsTime.h"

#include "EventNavigator/EventNavigator.h"

#include "Util.h"

#include <map>

using namespace Event;

class EvtRecTrack;
class EvtRecVeeVertex;
class EvtRecPi0;
class EvtRecEtaToGG;
class EvtRecDTag;
class RecZddChannel;

class commonData 
{	
public:
    
    commonData() {  }; 
    ~commonData() {  };

    void clear();

    static TEvtHeader *m_EvtHeader;

    static TEvtNavigator *m_EvtNavigator;

    /// Create a set of maps between Digi data in the TDS and the TRefs in the ROOT file
    static std::map<const MdcDigi*, TRef> m_mdcDigiMap;

    /// Create a set of maps between ROOT Digi objects and TDS Digi data
    static std::map<const TObject*, const MdcDigi*>          m_rootMdcDigiMap;


    /// Provide access to the ROOT event pointers
    static TDigiEvent *m_recEvt;

    static  std::map<const  EmcDigi* , TRef >  m_emcDigiMap;
    static  std::map<const TObject*, const EmcDigi*>    m_rootEmcDigiMap;    

    static  std::map<const  TofDigi*, TRef >   m_tofDigiMap;
    static  std::map<const  TObject*,  const TofDigi*>  m_rootTofDigiMap;

    static  std::map<const  MucDigi*, TRef >   m_mucDigiMap;
    static  std::map<const  TObject*,  const MucDigi*>  m_rootMucDigiMap;

    static  std::map<const  LumiDigi*, TRef >   m_lumiDigiMap;
    static  std::map<const  TObject*,  const LumiDigi*>  m_rootLumiDigiMap;

     /// Provide access to the ROOT event pointers
    static TDstEvent *m_dstEvt;
    
    static  std::map<const  DstEmcShower*, TRef >   m_emcShowerMap;
    static  std::map<const  TObject*,  const DstEmcShower*>  m_rootEmcShowerMap;

    static  std::map<const  DstMdcTrack*, TRef >   m_mdcTrackMap;
    static  std::map<const  TObject*,  const DstMdcTrack*>  m_rootMdcTrackMap;
   
    static  std::map<const  DstMucTrack*, TRef >   m_mucTrackMap;
    static  std::map<const  TObject*,  const DstMucTrack*>  m_rootMucTrackMap; 

    static  std::map<const  DstTofTrack*, TRef >   m_tofTrackMap;
    static  std::map<const  TObject*,  const DstTofTrack*>  m_rootTofTrackMap;
   
    static  std::map<const  DstMdcDedx*, TRef >   m_mdcDedxMap;
    static  std::map<const  TObject*,  const DstMdcDedx*>  m_rootMdcDedxMap; 

    static  std::map<const  DstExtTrack*, TRef >   m_extTrackMap;
    static  std::map<const  TObject*,  const DstExtTrack*>  m_rootExtTrackMap;

    static  std::map<const  DstMdcKalTrack*, TRef >   m_mdcKalTrackMap;
    static  std::map<const  TObject*,  const DstMdcKalTrack*>  m_rootMdcKalTrackMap;

 /// Provide access to the ROOT event pointers
    static TMcEvent *m_mcEvt;

    static  std::map<const  EmcMcHit*, TRef >   m_emcMcHitMap;
    static  std::map<const  TObject*,  const EmcMcHit*>  m_rootEmcMcHitMap;

    static  std::map<const MdcMcHit*, TRef >   m_mdcMcHitMap;
    static  std::map<const  TObject*,  const MdcMcHit*>  m_rootMdcMcHitMap;
   
    static  std::map<const  TofMcHit*, TRef >   m_tofMcHitMap;
    static  std::map<const  TObject*,  const TofMcHit*>  m_rootTofMcHitMap;

    static  std::map<const  MucMcHit*, TRef >   m_mucMcHitMap;
    static  std::map<const  TObject*,  const MucMcHit*>  m_rootMucMcHitMap;

    static  std::map<const  McParticle*, TRef >   m_mcParticleMap;
    static  std::map<const  TObject*,  const McParticle*>  m_rootMcParticleMap;

    //  static  std::map<const  DecayMode*, TRef >   m_decayModeMap;
    // static  std::map<const  TObject*,  const DecayMode*>  m_rootDecayModeMap;
 
    /// Provide access to the ROOT event pointers
    static TTrigEvent *m_trigEvt;

    static  std::map<const  TrigData*, TRef >   m_trigDataMap;
    static  std::map<const  TObject*,  const TrigData*>  m_rootTrigDataMap;

    /// Provide access to the ROOT event pointers
    static THltEvent  *m_hltEvt;
    static  std::map<const  HltRaw*, TRef >   m_hltRawMap;
    static  std::map<const  TObject*,  const HltRaw*>  m_rootHltRawMap;
    static  std::map<const  HltInf*, TRef >   m_hltInfMap;
    static  std::map<const  TObject*,  const HltInf*>  m_rootHltInfMap;
    static  std::map<const  DstHltInf*, TRef >   m_dstHltInfMap;
    static  std::map<const  TObject*,  const DstHltInf*>  m_rootDstHltInfMap;
	
    /// Provide access to the ROOT event pointers
    static TRecTrackEvent *m_rectrackEvt; 

    static  std::map<const  RecMdcTrack*, TRef >   m_recMdcTrackMap;
    static  std::map<const  TObject*,  const RecMdcTrack*>  m_rootRecMdcTrackMap;

    static  std::map<const  RecMdcHit*, TRef >   m_recMdcHitMap;
    static  std::map<const  TObject*,  const RecMdcHit*>  m_rootRecMdcHitMap;


     //static  std::map<const  DstMdcTrack*, TRef >   m_RecmdcTrackMap;
     //static  std::map<const  TObject*,  const DstMdcTrack*>  m_rootRecMdcTrackMap;
   
    static  std::map<const  RecTofTrack*, TRef >   m_RecTofTrackMap;
    static  std::map<const  TObject*,  const RecTofTrack*>  m_rootRecTofTrackMap;

    static  std::map<const  RecEmcHit*, TRef >   m_recEmcHitMap;
    static  std::map<const  TObject*,  const RecEmcHit*>  m_rootRecEmcHitMap;

    static  std::map<const  RecEmcCluster*, TRef >   m_recEmcClusterMap;
    static  std::map<const  TObject*,  const RecEmcCluster*>  m_rootRecEmcClusterMap;

    static  std::map<const  RecEmcShower*, TRef >   m_recEmcShowerMap;
    static  std::map<const  TObject*,  const RecEmcShower*>  m_rootRecEmcShowerMap;

    static  std::map<const  RecMucTrack*, TRef >   m_RecMucTrackMap;
    static  std::map<const  TObject*,  const RecMucTrack*>  m_rootRecMucTrackMap;

    static  std::map<const  RecMdcDedx*, TRef >   m_recMdcDedxMap; 
    static  std::map<const  TObject*,  const RecMdcDedx*>  m_rootRecMdcDedxMap;
    
    static  std::map<const  RecMdcDedxHit*, TRef >   m_recMdcDedxHitMap;
    static  std::map<const  TObject*,  const RecMdcDedxHit*>  m_rootRecMdcDedxHitMap;
    /* static  std::map<const  DstTofTrack*, TRef >   m_RectofTrackMap; */
    /*     static  std::map<const  TObject*,  const DstTofTrack*>  m_rootRecTofTrackMap; */
   
    /*     static  std::map<const  DstDedx*, TRef >   m_RecdedxMap; */
    /*     static  std::map<const  TObject*,  const DstDedx*>  m_rootRecDedxMap; */

    /*     static  std::map<const  DstExtTrack*, TRef >   m_RecextTrackMap; */
    /*     static  std::map<const  TObject*,  const DstExtTrack*>  m_rootRecExtTrackMap; */

    static  std::map<const  RecMdcKalTrack*, TRef >   m_recMdcKalTrackMap;
    static  std::map<const  TObject*,  const RecMdcKalTrack*>  m_rootRecMdcKalTrackMap;

    static  std::map<const  RecMdcKalHelixSeg*, TRef >   m_recMdcKalHelixSegMap;
    static  std::map<const  TObject*,  const RecMdcKalHelixSeg*>  m_rootRecMdcKalHelixSegMap;
    
    static  std::map<const  RecEsTime*, TRef >   m_RecEvTimeMap;
    static  std::map<const  TObject*,  const RecEsTime*>  m_rootRecEvTimeMap;
    
    static  std::map<const  RecZddChannel*, TRef >   m_recZddChannelMap;
    static  std::map<const  TObject*,  const RecZddChannel*>  m_rootRecZddChannelMap;

    static  std::map<const  EvtRecVeeVertex*, TRef >   m_evtRecVeeVertexMap;
    static  std::map<const  TObject*,  const EvtRecVeeVertex*>  m_rootEvtRecVeeVertexMap;

    static  std::map<const  EvtRecPi0*, TRef >   m_evtRecPi0Map;
    static  std::map<const  TObject*,  const EvtRecPi0*>  m_rootEvtRecPi0Map;    

    static  std::map<const  EvtRecEtaToGG*, TRef >   m_evtRecEtaToGGMap;
    static  std::map<const  TObject*,  const EvtRecEtaToGG*>  m_rootEvtRecEtaToGGMap;    

    static  std::map<const  EvtRecDTag*, TRef >   m_evtRecDTagMap;
    static  std::map<const  TObject*,  const EvtRecDTag*>  m_rootEvtRecDTagMap;
    

    static  TEvtRecObject*   m_evtRecObject;

    static  std::map<const  EvtRecTrack*, TRef>  m_EvtRecTrackMap;
    static  std::map<const  TObject*,  const EvtRecTrack*>  m_rootEvtRecTrackMap;

};


#endif
