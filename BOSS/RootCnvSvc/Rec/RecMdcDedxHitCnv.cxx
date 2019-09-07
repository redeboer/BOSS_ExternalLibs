#ifndef RecMdcDedxHitCnv_CXX
#define RecMdcDedxHitCnv_CXX  1
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "TClonesArray.h"
#include "EventModel/EventModel.h"
#include "ReconEvent/ReconEvent.h"
#include "MdcRecEvent/RecMdcDedxHit.h"
#include "Identifier/Identifier.h"

#include "RootEventData/TRecMdcDedxHit.h" // standard root object
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecMdcDedxHitCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecMdcDedxCnv> s_factory;
//const ICnvFactory& RecMdcDedxCnvFactory = s_factory;

RecMdcDedxHitCnv::RecMdcDedxHitCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "RecMdcDedxHitCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    //m_rootTreename ="Rec";
    m_rootBranchname ="m_recMdcDedxHitCol";
    // declareObject(EventModel::Recon::RecMdcDedxCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_recMdcDedxHitCol);
    m_recMdcDedxHitCol=0;
}

StatusCode RecMdcDedxHitCnv::TObjectToDataObject(DataObject*& refpObject) {
    // creation of TDS object from root object
    MsgStream log(msgSvc(), "RecMdcDedxHitCnv");
    log << MSG::DEBUG << "RecMdcDedxHitCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;
    // create the TDS location for the Dedx Collection
    RecMdcDedxHitCol* recMdcDedxHitCol = new RecMdcDedxHitCol;
    refpObject=recMdcDedxHitCol;

    //***************************register rec hit data in TDS****************************
    IDataProviderSvc* dataSvc = 0;
    sc = serviceLocator()->getService("EventDataSvc", 
                IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
    if ( sc.isFailure() ) {
        log << MSG::FATAL << "Could not get EventDataSvc in RecMdcDedxHitCnv" << endreq;
        return sc;
    }

    // Retrieve Mdc Hit     
    SmartDataPtr<RecMdcHitCol> mdcHitCol(dataSvc, EventModel::Recon::RecMdcHitCol);
    if(!mdcHitCol)
    {
        log << MSG::INFO << "Could not find RecMdcHitCol" << endreq;
    }

    // Retrieve MdcKal Helix Segment
    SmartDataPtr<RecMdcKalHelixSegCol> mdcKalHelixSegCol(dataSvc, EventModel::Recon::RecMdcKalHelixSegCol);
    if (!mdcKalHelixSegCol) {
        log << MSG::INFO << "Could not find RecMdcKalHelixSegCol" << endreq;
    }
    //**********************************************************************


    // now convert
    if (!m_recMdcDedxHitCol) return sc;
    TIter dedxIter(m_recMdcDedxHitCol);
    TRecMdcDedxHit *recMdcDedxHitRoot = 0;
    while ((recMdcDedxHitRoot = (TRecMdcDedxHit*)dedxIter.Next())) {
        int trackId = recMdcDedxHitRoot->trkId();
        //cout<<"trackId: "<<trackId<<endl;
        int flagLR = recMdcDedxHitRoot->flagLR(); 
        //cout<<"flagLR: "<<flagLR<<endl;
        long int mdcId(recMdcDedxHitRoot->mdcId());
        //cout<<"mdcID: "<<mdcId<<endl;
        double pathlength =  recMdcDedxHitRoot->pathLength();
        //cout<<"pathlength: " <<pathlength<<endl;
        double dedx = recMdcDedxHitRoot->getDedx();
        //cout<<"dedx: "<<dedx<<endl;
        
        log << MSG::DEBUG<<"TObjectToDataObject: check Reconstrunction of dE/dx Hit root::"<<" trackId: "<<trackId<<" flagLR: "<<flagLR<<" pathlength: "<<pathlength<<" dedx: "<<dedx<<endreq;
        RecMdcDedxHit *recMdcDedxHit = new RecMdcDedxHit();
        m_common.m_rootRecMdcDedxHitMap[recMdcDedxHitRoot] = recMdcDedxHit;
        recMdcDedxHit->setTrkId(trackId);
        recMdcDedxHit->setFlagLR(flagLR);
        recMdcDedxHit->setMdcId( Identifier(mdcId) );
        recMdcDedxHit->setPathLength( pathlength);
        recMdcDedxHit->setDedx(dedx);
        //   log << MSG::DEBUG<<"check Reconstrunction of dE/dx Hit root"<<" pathlength 2 is : "<<(*recdedxhit)->getPathLength()<<endreq; 

        int mdcHitId = recMdcDedxHitRoot->mdcHitId();
        //std::cout << __FILE__ << __LINE__ << " size: " << mdcHitCol->size() << "  id: " << mdcHitId << std::endl;
        if ( mdcHitId >= 0 ) { 
            recMdcDedxHit->setMdcHit( dynamic_cast<RecMdcHit*>(mdcHitCol->containedObject(mdcHitId)));
        }
        int mdcKalHelixSegId = recMdcDedxHitRoot->mdcKalHelixSegId();
         //std::cout << __FILE__ << __LINE__ << " size: " << mdcKalHelixSegCol->size() << "  id: " << mdcKalHelixSegId << std::endl;
        if ( mdcKalHelixSegId >= 0 ) {
            recMdcDedxHit->setMdcKalHelixSeg( dynamic_cast<RecMdcKalHelixSeg*>(mdcKalHelixSegCol->containedObject(mdcKalHelixSegId)));
        }

        recMdcDedxHitCol->push_back(recMdcDedxHit); 
        //delete dedxTds;
        // dedxTds = NULL;
    }
    //m_dedxCol->Delete();
    delete m_recMdcDedxHitCol;
    m_recMdcDedxHitCol = 0;

    return StatusCode::SUCCESS;
}


StatusCode RecMdcDedxHitCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {
    MsgStream log(msgSvc(), "RecMdcDedxHitCnv");
    log << MSG::DEBUG << "RecMdcDedxHitCnv::DataObjectToTObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;
    
    RecMdcDedxHitCol * recMdcDedxHitCol=dynamic_cast<RecMdcDedxHitCol *> (obj);
    if (!recMdcDedxHitCol) {
        log << MSG::ERROR << "Could not downcast to RecMdcDedxHitCol" << endreq;
        return StatusCode::FAILURE;
    }
    
    DataObject *evt;
    m_eds->findObject(EventModel::Recon::Event,evt);
    if (evt==NULL) {
        log << MSG::ERROR << "Could not get RecEvent in TDS "  << endreq;
        return StatusCode::FAILURE;
    }
    ReconEvent * devtTds=dynamic_cast<ReconEvent *> (evt);
    if (!devtTds) {
        log << MSG::ERROR << "RecMdcDedxHitCnv:Could not downcast to TDS RecEvent" << endreq;
    }
    IOpaqueAddress *addr;
    m_cnvSvc->getRecTrackCnv()->createRep(evt,addr); 
    TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();
    const TObjArray *m_recMdcDedxHitCol = recEvt->getRecMdcDedxHitCol();
    if (!m_recMdcDedxHitCol) return sc;
    recEvt->clearRecMdcDedxHitCol(); //necessary in case there is I/O at the same time since array is static

    //******************** get according mdcHIt and mdcKalHelixSeg of MdcDedxHit  ****************

    RecMdcHitCol::iterator    recMdcHitColbegin,    recMdcHitColend;
    RecMdcKalHelixSegCol::iterator recMdcKalHelixSegColbegin, recMdcKalHelixSegColend;

    IDataProviderSvc* dataSvc = 0;
    sc = serviceLocator()->getService("EventDataSvc", 
                IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
    if ( sc.isFailure() ) {
        log << MSG::FATAL << "Could not get EventDataSvc in RecMdcDedxHitCnv" << endreq;
        return sc;
    }

    // Retrieve Mdc Hit     
    SmartDataPtr<RecMdcHitCol> recMdcHitCol(dataSvc, EventModel::Recon::RecMdcHitCol);
    if (!recMdcHitCol) {
        log << MSG::ERROR << "Could not downcast to RecMdcHitCol" << endreq;
        return StatusCode::FAILURE;
    }
    else {
        recMdcHitColbegin = recMdcHitCol->begin();
        recMdcHitColend   = recMdcHitCol->end();
    }

    SmartDataPtr<RecMdcKalHelixSegCol> recMdcKalHelixSegCol(dataSvc, EventModel::Recon::RecMdcKalHelixSegCol);
    if (!recMdcKalHelixSegCol) {
        log << MSG::ERROR << "Could not downcast to RecMdcKalHelixSegCol" << endreq;
        return StatusCode::FAILURE;
    }
    else {
        recMdcKalHelixSegColbegin = recMdcKalHelixSegCol->begin();
        recMdcKalHelixSegColend   = recMdcKalHelixSegCol->end();
    }
    //******************************************************************************


    RecMdcDedxHitCol::const_iterator recMdcDedxHit;
    for (recMdcDedxHit = recMdcDedxHitCol->begin(); recMdcDedxHit != recMdcDedxHitCol->end(); recMdcDedxHit++) {
        int trackId = (*recMdcDedxHit)->getTrkId();
        int flagLR = (*recMdcDedxHit)->getFlagLR();
        UInt_t  mdcId =(*recMdcDedxHit)->getMdcId().get_value();
        double pathlength = (*recMdcDedxHit)->getPathLength();
        double dedx = (*recMdcDedxHit)->getDedx();

        log << MSG::DEBUG<<"DataObjectToTObject: check Reconstrunction of dE/dx Hit::"<<" trackId: "<<trackId<<" flagLR: "<<flagLR<<" pathlength: "<<pathlength<<" dedx: "<<dedx<<endreq;
        TRecMdcDedxHit *recMdcDedxHitRoot = new TRecMdcDedxHit();
        //m_common.m_recMdcDedxHitMap[(*recMdcDedxHit)] = recMdcDedxHitRoot;
        recMdcDedxHitRoot->setTrkId(trackId);
        recMdcDedxHitRoot->setFlagLR(flagLR);
        recMdcDedxHitRoot->setMdcId( mdcId );
        recMdcDedxHitRoot->setPathLength( pathlength);
        recMdcDedxHitRoot->setDedx(dedx);
      
        //int trackId1 = recMdcDedxHitRoot->trkId();
        //cout<<"trackId: "<<trackId1<<endl;
        //int flagLR1 = recMdcDedxHitRoot->flagLR(); 
        //cout<<"flagLR: "<<flagLR1<<endl;
        //long int mdcId1(recMdcDedxHitRoot->mdcId());
        //cout<<"mdcID: "<<mdcId1<<endl;
        //double pathlength1 =  recMdcDedxHitRoot->pathLength();
        //cout<<"pathlength: " <<pathlength1<<endl;
        //double dedx1 = recMdcDedxHitRoot->getDedx();
        //cout<<"dedx: "<<dedx1<<endl;


        if ( (*recMdcDedxHit)->isMdcHitValid() ) {
            RecMdcHitCol::iterator it = find(recMdcHitColbegin, recMdcHitColend, (*recMdcDedxHit)->getMdcHit());
            recMdcDedxHitRoot->setMdcHitId( it - recMdcHitColbegin );
        }
        else{
            recMdcDedxHitRoot->setMdcHitId(-1);
        }

        if ( (*recMdcDedxHit)->isMdcKalHelixSegValid() ) {
            RecMdcKalHelixSegCol::iterator it = find(recMdcKalHelixSegColbegin, recMdcKalHelixSegColend, (*recMdcDedxHit)->getMdcKalHelixSeg());
            recMdcDedxHitRoot->setMdcKalHelixSegId( it - recMdcKalHelixSegColbegin );
        }
        else{
            recMdcDedxHitRoot->setMdcKalHelixSegId(-1);
        }

        //log << MSG::DEBUG<<"check Reconstrunction of dE/dx Hit root"<<" pathlength 2 is : "<<(*recMdcDedxHit)->getPathLength()<<endreq;
        recEvt->addRecMdcDedxHit(recMdcDedxHitRoot);
    }

    return StatusCode::SUCCESS;
}

#endif
