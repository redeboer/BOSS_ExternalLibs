#ifndef _EventModel_H_
#define _EventModel_H_

/* Definition of the event structure in the Transient Data Store.
 *  
 * Only two levels in the logical path are foreseen at present, 
 *   /event/<namespace>/<leave>  e.g. /Event/MC/McVertices
 * 
 * Convention:
 *  If the <leave> object is a
 *  DataObject    use name of corresponding class
 *  Container     use name of ContainedObject class in plural
 *                or append 'Vec' to the name, e.g. use
 *                McVertices or McVertexVec
 *                
 *
 * @author : adapted from LHCb EventModel
 */ 
// add TrkListCol   date 10/24/2005 by K.L. He

#include <string>

#if defined(_Event_EventModel_CPP_)
#define  _EXTERN_ 
#else
#define  _EXTERN_ extern
#endif
    
    namespace EventModel {
        _EXTERN_ std::string   EventH;
        _EXTERN_ std::string   EventHeader;
        _EXTERN_ std::string   Navigator;

       namespace Gen {
            _EXTERN_ std::string Event;
        }

        namespace MC {
            _EXTERN_ std::string Event;
            _EXTERN_ std::string McEventCol; 
            _EXTERN_ std::string McParticleCol;
            _EXTERN_ std::string DecayMode;
            _EXTERN_ std::string McPrimaryParticleCol;
            _EXTERN_ std::string MdcMcHitCol;
            _EXTERN_ std::string TofMcHitCol;
            _EXTERN_ std::string EmcMcHitCol;
            _EXTERN_ std::string MucMcHitCol;
            _EXTERN_ std::string McPartToMdcHitTab;
            _EXTERN_ std::string McPartToTofHitTab;
            _EXTERN_ std::string McPartToEmcHitTab;
            _EXTERN_ std::string McPartToMucHitTab;
        }


        namespace Digi {
	    _EXTERN_ std::string Event;
            _EXTERN_ std::string MdcDigiCol;
            _EXTERN_ std::string EmcDigiCol;
            _EXTERN_ std::string TofDigiCol;
            _EXTERN_ std::string MucDigiCol;
            _EXTERN_ std::string LumiDigiCol;
        }
       

	namespace Trig {
	    _EXTERN_ std::string Event;
	    _EXTERN_ std::string TrigGTDCol;
            _EXTERN_ std::string TrigData;
	}

	namespace Hlt {
	  _EXTERN_ std::string Event;
	  _EXTERN_ std::string HltRawCol;
	  _EXTERN_ std::string HltInf;
	  _EXTERN_ std::string DstHltInf;
	  
	}

	namespace Zdd {
	    _EXTERN_ std::string   ZddEvent;
	}

        namespace Recon {                                                       
	   _EXTERN_ std::string Event;
	   _EXTERN_ std::string RecMdcHitCol;
	   _EXTERN_ std::string RecMdcTrackCol;                                   
	   _EXTERN_ std::string MdcHOTCol;                                     
	   _EXTERN_ std::string RecMdcDedxCol;
	   _EXTERN_ std::string RecMdcDedxHitCol;
	   _EXTERN_ std::string RecMdcKalTrackCol;
           _EXTERN_ std::string RecMdcKalHelixSegCol;
           _EXTERN_ std::string RecEsTimeCol;
	   _EXTERN_ std::string RecExtTrackCol;	
	
	   _EXTERN_ std::string RecBTofHitCol;
	   _EXTERN_ std::string RecETofHitCol;
	   _EXTERN_ std::string RecTofTrackCol;
	   _EXTERN_ std::string RecBTofCalHitCol;
           _EXTERN_ std::string RecETofCalHitCol;
                
	   _EXTERN_ std::string RecEmcHitCol;
	   _EXTERN_ std::string RecEmcClusterCol;
	   _EXTERN_ std::string RecEmcShowerCol;
           
	   _EXTERN_ std::string MucRecHitCol;
           _EXTERN_ std::string RecMucTrackCol;

           _EXTERN_ std::string RecZddChannelCol;  // zoujh 2013
       } 
      
        namespace EvtRec {
	   //	   _EXTERN_ std::string TrkListCol;
	   //	   _EXTERN_ std::string EventList;
	   //	   _EXTERN_ std::string RecTrackListCol;//Wangll 2006.03.28
	  _EXTERN_ std::string  Event;
	   _EXTERN_ std::string EvtRecEvent;            //hekl 2008.03.18
	   _EXTERN_ std::string EvtRecTrackCol;         //hekl 2008.03.18
	   //	   _EXTERN_ std::string EvtRecVertexCol;        //hekl 2008.03.18
	   //	   _EXTERN_ std::string EvtRecPrimaryVertexCol;	 // remove Col	  
	   _EXTERN_ std::string EvtRecPrimaryVertex;	   
	   _EXTERN_ std::string EvtRecVeeVertexCol;
	   _EXTERN_ std::string EvtRecDTagCol;
	   _EXTERN_ std::string EvtRecPi0Col;      // petez 2009.03.02
           _EXTERN_ std::string EvtRecEtaToGGCol;  // petez 2009.03.02
	}

        namespace Dst {
	    _EXTERN_ std::string Event;
            _EXTERN_ std::string DstMdcTrackCol;
            _EXTERN_ std::string DstMdcKalTrackCol;
            _EXTERN_ std::string DstMdcKalHelixSegCol;
            _EXTERN_ std::string DstMdcDedxCol;
            _EXTERN_ std::string DstTofTrackCol;
	    _EXTERN_ std::string DstEmcShowerCol;
            _EXTERN_ std::string DstMucTrackCol;
	    _EXTERN_ std::string DstExtTrackCol;
	    _EXTERN_ std::string DstTrkListCol;
	    _EXTERN_ std::string EventList;                                         
        }                                   
	namespace Anal {
	  _EXTERN_  std::string Event;
	  _EXTERN_  std::string BParticleCol;
	  _EXTERN_  std::string BParticleIDCol;
	  _EXTERN_  std::string BVertexCol;
	  _EXTERN_  std::string BPhotonCol;
	  _EXTERN_  std::string BElectronCol;
	  _EXTERN_  std::string BMuonCol;
	  _EXTERN_  std::string BPionCol;
	  _EXTERN_  std::string BKaonCol;
	  _EXTERN_  std::string BProtonCol;
	  _EXTERN_  std::string BTrackCol;
	  _EXTERN_  std::string BKShortCol;
	  _EXTERN_  std::string BLambdaCol;
	  _EXTERN_  std::string BPi0Col;
	  _EXTERN_  std::string BEtaCol;
	}
	namespace Phys {
	  _EXTERN_  std::string Event;
	  _EXTERN_  std::string BCharmMesonCol;
	  _EXTERN_  std::string BJPsiCol;	  
	}
    }

#undef _EXTERN_
#endif 
