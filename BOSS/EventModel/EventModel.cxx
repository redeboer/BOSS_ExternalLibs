#define _Event_EventModel_CPP_

#include "EventModel/EventModel.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ClassID.h"

 /** @class EventModel
 *   @brief Event Model: Definition of logical paths and class identifiers
 *
 */
class EventDataModel {        
public:
  EventDataModel() {
    EventModel::EventH = "/Event";
    
    EventModel::Navigator = EventModel::EventH +  "/Navigator";

    EventModel::EventHeader = EventModel::EventH + "/EventHeader";
    
    // Generator 
    EventModel::Gen::Event                = EventModel::EventH  + "/Gen";

    // Simulation 
    EventModel::MC::Event                 = EventModel::EventH + "/MC";
    EventModel::MC::McParticleCol         = EventModel::MC::Event   + "/McParticleCol";
    EventModel::MC::DecayMode             = EventModel::MC::Event   + "/DecayMode";   
    EventModel::MC::MdcMcHitCol           = EventModel::MC::Event   + "/MdcMcHitCol";
    EventModel::MC::TofMcHitCol           = EventModel::MC::Event   + "/TofMcHitCol";
    EventModel::MC::EmcMcHitCol           = EventModel::MC::Event   + "/EmcMcHitCol";
    EventModel::MC::MucMcHitCol           = EventModel::MC::Event   + "/MucMcHitCol";
    EventModel::MC::McPrimaryParticleCol  = EventModel::MC::Event   + "/McPrimaryParticleCol";
    EventModel::MC::McPartToMdcHitTab     = EventModel::MC::Event  + "/McPartToMdcHitTab";
    EventModel::MC::McPartToTofHitTab     = EventModel::MC::Event  + "/McPartToTofHitTab";
    EventModel::MC::McPartToEmcHitTab     = EventModel::MC::Event  + "/McPartToEmcHitTab";
    EventModel::MC::McPartToMucHitTab     = EventModel::MC::Event  + "/McPartToMucHitTab";

    // Digi event
    EventModel::Digi::Event = EventModel::EventH + "/Digi";
    EventModel::Digi::MdcDigiCol  = EventModel::Digi::Event + "/MdcDigiCol";
    EventModel::Digi::EmcDigiCol  = EventModel::Digi::Event + "/EmcDigiCol";
    EventModel::Digi::TofDigiCol  = EventModel::Digi::Event + "/TofDigiCol";
    EventModel::Digi::MucDigiCol  = EventModel::Digi::Event + "/MucDigiCol";
    EventModel::Digi::LumiDigiCol = EventModel::Digi::Event + "/LumiDigiCol";

    // Trigger info
    EventModel::Trig::Event = EventModel::EventH + "/Trig";
    EventModel::Trig::TrigGTDCol = EventModel::Trig::Event + "/TrigGTDCol";
    EventModel::Trig::TrigData    = EventModel::Trig::Event + "/TrigData";

    // EventFilter info
    EventModel::Hlt::Event = EventModel::EventH + "/Hlt";
    EventModel::Hlt::HltRawCol = EventModel::Hlt::Event + "/HltRawCol";
    EventModel::Hlt::HltInf = EventModel::Hlt::Event + "/HltInf";
    EventModel::Hlt::DstHltInf = EventModel::Hlt::Event + "/DstHltInf";

    // for ZDD
    EventModel::Zdd::ZddEvent = EventModel::EventH + "/ZddEvent";

    //for test wsp 20050513
    EventModel::Dst::Event = EventModel::EventH + "/Dst";
    EventModel::Dst::DstMdcTrackCol  = EventModel::Dst::Event +    "/DstMdcTrackCol";
    EventModel::Dst::DstMdcKalTrackCol  = EventModel::Dst::Event +    "/DstMdcKalTrackCol";
    EventModel::Dst::DstMdcDedxCol      = EventModel::Dst::Event +    "/DstMdcDedxCol";
    EventModel::Dst::DstTofTrackCol  = EventModel::Dst::Event +    "/DstTofTrackCol";    
    EventModel::Dst::DstEmcShowerCol  = EventModel::Dst::Event +    "/DstEmcShowerCol";
    EventModel::Dst::DstMucTrackCol  = EventModel::Dst::Event +    "/DstMucTrackCol";    
    EventModel::Dst::DstExtTrackCol  = EventModel::Dst::Event +    "/DstExtTrackCol";
    EventModel::Dst::DstTrkListCol   = EventModel::Dst::Event +    "/DstTrkListCol";
    EventModel::Dst::EventList       = EventModel::Dst::Event +    "/EventList";
    
    
    // Recon event                                                            
     EventModel::Recon::Event = EventModel::EventH + "/Recon";
     EventModel::Recon::RecMdcHitCol   = EventModel::Recon::Event + "/RecMdcHitCol";
     EventModel::Recon::RecMdcTrackCol = EventModel::Recon::Event + "/RecMdcTrackCol";
     EventModel::Recon::MdcHOTCol  = EventModel::Recon::Event + "/MdcHOTCol";    
     EventModel::Recon::RecMdcDedxCol = EventModel::Recon::Event + "/RecMdcDedxCol";
     EventModel::Recon::RecMdcDedxHitCol = EventModel::Recon::Event + "/RecMdcDedxHitCol";
     EventModel::Recon::RecMdcKalTrackCol = EventModel::Recon::Event + "/RecMdcKalTrackCol";
     EventModel::Recon::RecMdcKalHelixSegCol = EventModel::Recon::Event + "/RecMdcKalHelixSegCol";
     EventModel::Recon::RecEsTimeCol = EventModel::Recon::Event + "/RecEsTimeCol";
     EventModel::Recon::RecExtTrackCol = EventModel::Recon::Event + "/RecExtTrackCol";
     EventModel::Recon::RecBTofHitCol = EventModel::Recon::Event + "/RecBTofHitCol";
     EventModel::Recon::RecETofHitCol = EventModel::Recon::Event + "/RecETofHitCol";
     EventModel::Recon::RecTofTrackCol = EventModel::Recon::Event + "/RecTofTrackCol";
     EventModel::Recon::RecBTofCalHitCol = EventModel::Recon::Event + "/RecBTofCalHitCol";
     EventModel::Recon::RecETofCalHitCol = EventModel::Recon::Event +"/RecETofCalHitCol";    
     EventModel::Recon::RecEmcHitCol = EventModel::Recon::Event + "/RecEmcHitCol";
     EventModel::Recon::RecEmcClusterCol = EventModel::Recon::Event + "/RecEmcClusterCol";
     EventModel::Recon::RecEmcShowerCol = EventModel::Recon::Event + "/RecEmcShowerCol";
     EventModel::Recon::MucRecHitCol   = EventModel::Recon::Event + "/MucRecHitCol";
     EventModel::Recon::RecMucTrackCol    = EventModel::Recon::Event + "/RecMucTrackCol";
     EventModel::Recon::RecZddChannelCol = EventModel::Recon::Event + "/RecZddChannelCol";

     // hekl 2008.03.18, add EvtRecEvent, EvtRecTrack, EvtRecVertex
     EventModel::EvtRec::Event = EventModel::EventH + "/EvtRec";
     EventModel::EvtRec::EvtRecEvent = EventModel::EvtRec::Event + "/EvtRecEvent";
     EventModel::EvtRec::EvtRecTrackCol = EventModel::EvtRec::Event + "/EvtRecTrackCol";
     // hekl 2008.11.14 split EvtRecVertex to EvtRecPrimaryVertex and EvtRecVeeVertex
     EventModel::EvtRec::EvtRecPrimaryVertex = EventModel::EvtRec::Event + "/EvtRecPrimaryVertex";   
     EventModel::EvtRec::EvtRecVeeVertexCol = EventModel::EvtRec::Event + "/EvtRecVeeVertexCol";
     // c.liu 2009.1.27 add EvtRecDTag
     EventModel::EvtRec::EvtRecDTagCol = EventModel::EvtRec::Event + "/EvtRecDTagCol";
     // petez 2009.03.02, add EvtRecPi0 and EvtRecEtaToGG (based off of the work by zoujh)
     EventModel::EvtRec::EvtRecPi0Col = EventModel::EvtRec::Event + "/EvtRecPi0Col";
     EventModel::EvtRec::EvtRecEtaToGGCol = EventModel::EvtRec::Event + "/EvtRecEtaToGGCol";


     //Analysis Event
    EventModel::Anal::Event = EventModel::EventH + "/Anal";
    EventModel::Anal::BParticleCol = EventModel::Anal::Event + "/BParticleCol";
    EventModel::Anal::BParticleIDCol = EventModel::Anal::Event + "/BParticleIDCol";
    EventModel::Anal::BVertexCol = EventModel::Anal::Event + "/BVertexCol";
    EventModel::Anal::BPhotonCol = EventModel::Anal::Event + "/BPhotonCol";
    EventModel::Anal::BElectronCol = EventModel::Anal::Event + "/BElectronCol";
    EventModel::Anal::BMuonCol = EventModel::Anal::Event + "/BMuonCol";
    EventModel::Anal::BPionCol = EventModel::Anal::Event + "/BPionCol";
    EventModel::Anal::BKaonCol = EventModel::Anal::Event + "/BKaonCol";
    EventModel::Anal::BProtonCol = EventModel::Anal::Event + "/BProtonCol";
    EventModel::Anal::BTrackCol = EventModel::Anal::Event + "/BTrackCol";
    EventModel::Anal::BPi0Col = EventModel::Anal::Event + "/BPi0Col";
    EventModel::Anal::BEtaCol = EventModel::Anal::Event + "/BEtaCol";
    EventModel::Anal::BKShortCol = EventModel::Anal::Event + "/BKShortCol";
    EventModel::Anal::BLambdaCol = EventModel::Anal::Event + "/BLambdaCol";
    //Physics Event
    EventModel::Phys::Event = EventModel::EventH + "/Phys";
    EventModel::Phys::BCharmMesonCol = EventModel::Phys::Event + "/BCharmMesonCol";
    EventModel::Phys::BJPsiCol = EventModel::Phys::Event + "/BJPsiCol";
  }
};
    
static EventDataModel evtModel;        

/*  Class ID definitions for the Event Model
    Maximum CLID is 65536 = 2^16 - 1

    Category           ID range      Comment
    ---------          --------      -------
    Gaudi Kernel          0 -  99    see Gaudi/Kernel/Kernel.cpp
                        100 - 199    general classes (Run, Event, EventTag, ContainedObject)
                                     see Gaudi/Kernel/Kernel.cpp
                                     Some of these classes may be removed in the next 
                                     Gaudi release

    class Categories and ID ranges 
    
    Category           ID range      Comment
    ---------          --------      -------
    EventSelection     200  -  299   High level event information used for event selection
                              
    Utilities          300  -  399   used at several places of the EventModel
    
    MonteCarlo         1000
      Generator        1001 - 1099   generator related classes
      Kine+Hit         1100 - 1199   Kinematics, Hits and associations

    Digi data          1300 - 1399   Digi

    Dst Data           1400 - 1499   Dst 

    Trigger (simulation)
      Trigger          2000 - 2099   Trigger summary 
      LVL1             2100 - 2199   LVL1 classes
      EventFilter      2200 - 2299   Event Filter classes

    Reconstruction
      Recon            3000 - 3099   
      MdcRecon         3100 - 3199   Tracker recon
      CalRecon         3200 - 3299   Calorimeter recon
      TofRecon         3300 - 3399   TOF recon
      MuonRecon        3400 - 3499   Muon Counter recon
      TrkExt           3500 - 3599   Track extrapoation
      EvTime           3600 - 3699   Event Time recon

    
    EvtRec             4000 - 4999   Global link and reconstruction

    Analysis           5000 - 5999   Event interpretation beyond reconstruction, e.g.
                                     information for event classification, diagnosis, ... 
                                     Typically not written to persistent store.

    Physics            6000 - 6999

    Note:
    Detector description and calibrations have their own trees, not connected
    to the EventModel tree.  
 */
    
// Declaration of Identifiers
// The order is: General, MDC, Calorimeter, TOF, MuonCounter

// No longer defined in Gaudi - it is left to the user to define
const CLID& CLID_Event              =  110;
const CLID& CLID_EventList          =  111;

const CLID& CLID_EventHeader        =  112;

// Generator
const CLID& CLID_GenEvent           = 1100;
const CLID& CLID_GenEventCol        = CLID_GenEvent + CLID_ObjectList;
    
//Simulation IDs
const CLID& CLID_McEvent            = 1200;
const CLID& CLID_McEventCol         = CLID_McEvent + CLID_ObjectList;

const CLID& CLID_McParticle         = 1201;
const CLID& CLID_McParticleCol      = CLID_McParticle + CLID_ObjectList;

const CLID& CLID_DecayMode          = 1202; // DecayMode

const CLID& CLID_McPrimaryParticle         = 1203;
const CLID& CLID_McPrimaryParticleCol      = CLID_McParticle + CLID_ObjectVector;

const CLID& CLID_MdcMcHit           = 1211;
const CLID& CLID_MdcMcHitCol        = CLID_MdcMcHit + CLID_ObjectVector;

const CLID& CLID_TofMcHit           = 1212;
const CLID& CLID_TofMcHitCol        = CLID_TofMcHit + CLID_ObjectVector;

const CLID& CLID_EmcMcHit           = 1213;
const CLID& CLID_EmcMcHitCol        = CLID_EmcMcHit + CLID_ObjectVector;

const CLID& CLID_MucMcHit           = 1214;
const CLID& CLID_MucMcHitCol        = CLID_MucMcHit + CLID_ObjectVector;

const CLID& CLID_McHitMcPartToMdcHitTab    = 1221;
const CLID& CLID_McHitMcPartToTofHitTab    = 1222;
const CLID& CLID_McHitMcPartToEmcHitTab    = 1223;
const CLID& CLID_McHitMcPartToMucHitTab    = 1224;

//! Raw event and Digi IDs
const CLID& CLID_DigiEvent          = 1300; 

const CLID& CLID_MdcDigi            = 1310;  // indicative only
const CLID& CLID_MdcDigiCol         = CLID_MdcDigi+CLID_ObjectVector;

const CLID& CLID_EmcDigi            = 1320;  // indicative only
const CLID& CLID_EmcDigiCol         = CLID_EmcDigi+CLID_ObjectVector;

const CLID& CLID_TofDigi            = 1330;
const CLID& CLID_TofDigiCol         = CLID_TofDigi+CLID_ObjectVector;

const CLID& CLID_MucDigi            = 1340;
const CLID& CLID_MucDigiCol         = CLID_MucDigi+CLID_ObjectVector;

const CLID& CLID_LumiDigi     = 1350;
const CLID& CLID_LumiDigiCol  = CLID_LumiDigi + CLID_ObjectVector;

//Dst event 
const CLID& CLID_DstEvent           = 1400;

const CLID& CLID_DstMdcTrack        = 1410;                                     
const CLID& CLID_DstMdcTrackCol     = CLID_DstMdcTrack + CLID_ObjectVector;

const CLID& CLID_DstMdcKalTrack     = 1415;                                     
const CLID& CLID_DstMdcKalTrackCol  = CLID_DstMdcKalTrack + CLID_ObjectVector;

const CLID& CLID_DstMdcDedx            = 1420;                                      
const CLID& CLID_DstMdcDedxCol         = CLID_DstMdcDedx + CLID_ObjectVector;       

const CLID& CLID_DstTofTrack        = 1430;                                     
const CLID& CLID_DstTofTrackCol     = CLID_DstTofTrack + CLID_ObjectVector; 

const CLID& CLID_DstEmcShower        = 1440;                                     
const CLID& CLID_DstEmcShowerCol        = CLID_DstEmcShower + CLID_ObjectVector;

const CLID& CLID_DstMucTrack        = 1450;                                     
const CLID& CLID_DstMucTrackCol        = CLID_DstMucTrack + CLID_ObjectVector;    
   
const CLID& CLID_DstExtTrack        = 1460;
const CLID& CLID_DstExtTrackCol     = CLID_DstExtTrack + CLID_ObjectVector;

const CLID& CLID_DstTrkList         = 1470;
const CLID& CLID_DstTrkListCol      = CLID_DstTrkList + CLID_ObjectVector;

// Trigger
const CLID& CLID_TrigEvent        =  2100;
const CLID& CLID_TrigGTD          =  2110;
const CLID& CLID_TrigGTDCol       =  CLID_TrigGTD + CLID_ObjectVector;
const CLID& CLID_TrigData         =  2120;

// EventFilter
const CLID& CLID_HltEvent         =  2200;
const CLID& CLID_HltRaw           =  2210;
const CLID& CLID_HltRawCol        =  CLID_HltRaw + CLID_ObjectVector;
const CLID& CLID_HltInf           =  2220;
const CLID& CLID_DstHltInf        =  2230;

// ZDD
const CLID& CLID_ZddEvent         =  2300;

//! Reconstruction
const CLID& CLID_ReconEvent       = 3000;
  
const CLID& CLID_RecMdcTrack      = 3110;                                     
const CLID& CLID_RecMdcTrackCol   = CLID_RecMdcTrack + CLID_ObjectVector;        
                                                                                
const CLID& CLID_RecMdcHit        = 3120;                                    
const CLID& CLID_RecMdcHitCol     = CLID_RecMdcHit + CLID_ObjectVector;      
                                                                                
const CLID& CLID_MdcHitOnTrack    = 3130;                                       
const CLID& CLID_MdcHOTCol        = CLID_MdcHitOnTrack + CLID_ObjectVector;     
                                                                                
const CLID& CLID_RecMdcDedx          = 3140;                                      
const CLID& CLID_RecMdcDedxCol       = CLID_RecMdcDedx + CLID_ObjectVector;          

const CLID& CLID_RecMdcDedxHit          = 3145;
const CLID& CLID_RecMdcDedxHitCol       = CLID_RecMdcDedxHit + CLID_ObjectVector;


const CLID& CLID_RecMdcKalTrack      = 3150;
const CLID& CLID_RecMdcKalTrackCol   = CLID_RecMdcKalTrack + CLID_ObjectVector;

const CLID& CLID_RecMdcKalHelixSeg      = 3160;
const CLID& CLID_RecMdcKalHelixSegCol   = CLID_RecMdcKalHelixSeg + CLID_ObjectVector;

const CLID& CLID_RecEmcShower     = 3210;
const CLID& CLID_RecEmcShowerCol  = CLID_RecEmcShower + CLID_ObjectVector;

const CLID& CLID_RecEmcHit        = 3220;
const CLID& CLID_RecEmcHitCol     = CLID_RecEmcHit + CLID_ObjectVector;

const CLID& CLID_RecEmcCluster    = 3230;
const CLID& CLID_RecEmcClusterCol = CLID_RecEmcCluster + CLID_ObjectVector;

const CLID& CLID_RecBTofHit          = 3310;                                     
const CLID& CLID_RecBTofHitCol       = CLID_RecBTofHit + CLID_ObjectVector; 

const CLID& CLID_RecETofHit          = 3320;                                     
const CLID& CLID_RecETofHitCol       = CLID_RecETofHit + CLID_ObjectVector; 

const CLID& CLID_RecTofTrack        = 3330;                                     
const CLID& CLID_RecTofTrackCol     = CLID_RecTofTrack + CLID_ObjectVector; 

//const CLID& CLID_RecETofTrack        = 3340;                                     
//const CLID& CLID_RecETofTrackCol     = CLID_RecETofTrack + CLID_ObjectVector; 

const CLID& CLID_RecBTofCalHit        = 3350;
const CLID& CLID_RecBTofCalHitCol     = CLID_RecBTofCalHit + CLID_ObjectVector;

const CLID& CLID_RecETofCalHit        = 3360;
const CLID& CLID_RecETofCalHitCol     = CLID_RecETofCalHit + CLID_ObjectVector;

const CLID& CLID_RecMucTrack         = 3410;                                     
const CLID& CLID_RecMucTrackCol      = CLID_RecMucTrack + CLID_ObjectVector;        
                                                                                
const CLID& CLID_MucRecHit        = 3420;                                    
const CLID& CLID_MucRecHitCol     = CLID_MucRecHit + CLID_ObjectVector;      

//const CLID& CLID_TrkList          = 3430;                                     
//const CLID& CLID_TrkListCol      = CLID_TrkList + CLID_ObjectVector;        
  


//------ Wangll 2006.03.28 ------
//const CLID& CLID_RecTrackList     = 3710;
//const CLID& CLID_RecTrackListCol  = CLID_RecTrackList + CLID_ObjectVector;
//------ end ------


const CLID& CLID_RecExtTrack         = 3510;
const CLID& CLID_RecExtTrackCol      = CLID_RecExtTrack + CLID_ObjectVector;
const CLID& CLID_RecEsTime           = 3610;
const CLID& CLID_RecEsTimeCol        = CLID_RecEsTime + CLID_ObjectVector;

const CLID& CLID_RecZddChannel       = 3810;
const CLID& CLID_RecZddChannelCol    = CLID_RecZddChannel + CLID_ObjectVector;


///! Global link and reconstruction
const CLID& CLID_EvtRecObject        = 4000;
const CLID& CLID_EvtRecEvent         = 4010;
const CLID& CLID_EvtRecPrimaryVertex = 4020;

const CLID& CLID_EvtRecTrack         = 4100;
const CLID& CLID_EvtRecTrackCol      = CLID_EvtRecTrack + CLID_ObjectVector;
const CLID& CLID_EvtRecVeeVertex     = 4110;
const CLID& CLID_EvtRecVeeVertexCol  = CLID_EvtRecVeeVertex + CLID_ObjectVector;
const CLID& CLID_EvtRecDTag          = 4120;
const CLID& CLID_EvtRecDTagCol       = CLID_EvtRecDTag + CLID_ObjectVector;
const CLID& CLID_EvtRecPi0           = 4130;
const CLID& CLID_EvtRecPi0Col        = CLID_EvtRecPi0 + CLID_ObjectVector;
const CLID& CLID_EvtRecEtaToGG       = 4140;
const CLID& CLID_EvtRecEtaToGGCol    = CLID_EvtRecEtaToGG + CLID_ObjectVector;


//! Analysis
const CLID& CLID_AnalEvent          = 5000;

const CLID& CLID_BParticle          = 5010;
const CLID& CLID_BParticleCol       = CLID_BParticle + CLID_ObjectVector;

const CLID& CLID_BParticleID        = 5020;
const CLID& CLID_BParticleIDCol     = CLID_BParticleID + CLID_ObjectVector;

const CLID& CLID_BVertex            = 5030;
const CLID& CLID_BVertexCol         = CLID_BVertex + CLID_ObjectVector;

const CLID& CLID_BPhoton             = 5040;
const CLID& CLID_BPhotonCol          = CLID_BPhoton + CLID_ObjectVector;

const CLID& CLID_BElectron          = 5050;
const CLID& CLID_BElectronCol       = CLID_BElectron + CLID_ObjectVector;

const CLID& CLID_BMuon              = 5060;
const CLID& CLID_BMuonCol           = CLID_BMuon + CLID_ObjectVector;

const CLID& CLID_BPion              = 5070;
const CLID& CLID_BPionCol           = CLID_BPion + CLID_ObjectVector;

const CLID& CLID_BKaon              = 5080;
const CLID& CLID_BKaonCol           = CLID_BKaon + CLID_ObjectVector;

const CLID& CLID_BProton            = 5090;
const CLID& CLID_BProtonCol         = CLID_BProton + CLID_ObjectVector;

const CLID& CLID_BTrack		    = 5100;
const CLID& CLID_BTrackCol	    = CLID_BTrack + CLID_ObjectVector;

const CLID& CLID_BPi0               = 5110;
const CLID& CLID_BPi0Col            = CLID_BPi0  + CLID_ObjectVector;

const CLID& CLID_BEta               = 5120;
const CLID& CLID_BEtaCol            = CLID_BEta + CLID_ObjectVector;

const CLID& CLID_BKShort             = 5130;
const CLID& CLID_BKShortCol          = CLID_BKShort + CLID_ObjectVector;

const CLID& CLID_BLambda             = 5140;
const CLID& CLID_BLambdaCol          = CLID_BLambda + CLID_ObjectVector;

// ! Physics

const CLID& CLID_PhysEvent           = 6000;
const CLID& CLID_BCharmMeson         = 6010;
const CLID& CLID_BCharmMesonCol      = CLID_BCharmMeson + CLID_ObjectVector;

const CLID& CLID_BJPsi               = 6020;
const CLID& CLID_BJPsiCol            = CLID_BJPsi + CLID_ObjectVector;   
