#ifndef RecBTofCalHitCnv_CXX
#define RecBTofCalHitCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "ReconEvent/ReconEvent.h"
#include "MucRecEvent/RecMucTrack.h"

//#include "RootEventData/TMucTrack.h" // standard root object
#include "RootEventData/TRecMucTrack.h" // standard root object
//#include "RootEventData/TDstEvent.h"
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecBTofCalHitCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecBTofCalHitCnv> s_factory;
//const ICnvFactory& RecBTofCalHitCnvFactory = s_factory;

RecBTofCalHitCnv::RecBTofCalHitCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "RecBTofCalHitCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    //m_rootTreename ="Rec";
    m_rootBranchname ="m_RecBTofCalHitCol";
    //declareObject(EventModel::Recon::RecBTofCalHitCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_mucTrackCol);
    m_mucTrackCol=0;
}

StatusCode RecBTofCalHitCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object


    return StatusCode::SUCCESS;
}

StatusCode RecBTofCalHitCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {
 
  return StatusCode::SUCCESS;
}
#endif
