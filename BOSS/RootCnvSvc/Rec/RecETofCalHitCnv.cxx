#ifndef RecETofCalHitCnv_CXX
#define RecETofCalHitCnv_CXX  1

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
#include "RootCnvSvc/Rec/RecETofCalHitCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecETofCalHitCnv> s_factory;
//const ICnvFactory& RecETofCalHitCnvFactory = s_factory;

RecETofCalHitCnv::RecETofCalHitCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "RecETofCalHitCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    //m_rootTreename ="Rec";
    m_rootBranchname ="m_RecETofCalHitCol";
    //declareObject(EventModel::Recon::RecETofCalHitCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_mucTrackCol);
    m_mucTrackCol=0;
}

StatusCode RecETofCalHitCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object


    return StatusCode::SUCCESS;
}

StatusCode RecETofCalHitCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {
 
  return StatusCode::SUCCESS;
}
#endif
