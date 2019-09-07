#ifndef RecMdcDedxCnv_CXX
#define RecMdcDedxCnv_CXX  1
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "TClonesArray.h"
#include "EventModel/EventModel.h"
#include "ReconEvent/ReconEvent.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcDedxHit.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcDedx.h"

#include "RootEventData/TRecMdcDedx.h" // standard root object
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecMdcDedxCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecMdcDedxCnv> s_factory;
//const ICnvFactory& RecMdcDedxCnvFactory = s_factory;

RecMdcDedxCnv::RecMdcDedxCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "RecMdcDedxCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    //m_rootTreename ="Rec";
    m_rootBranchname ="m_recMdcDedxCol";
    // declareObject(EventModel::Recon::RecMdcDedxCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_recMdcDedxCol);
    m_recMdcDedxCol=0;
}

StatusCode RecMdcDedxCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object
    MsgStream log(msgSvc(), "RecMdcDedxCnv");
    log << MSG::DEBUG << "RecMdcDedxCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the Dedx Collection
    RecMdcDedxCol* recMdcDedxCol = new RecMdcDedxCol;
    refpObject=recMdcDedxCol;

    if (!m_recMdcDedxCol) return sc;
    
    //*******************Dst to Rec convert in case of no Rec data in TDS****************************
    IDataProviderSvc* dataSvc = 0;
    sc = serviceLocator()->getService("EventDataSvc",
                IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
    if ( sc.isFailure() ) {
        log << MSG::FATAL << "Could not get EventDataSvc in RecMdcDedxCnv" << endreq;
        return sc;
    }

    //----------get hit smartRefVector--------
    SmartDataPtr<RecMdcDedxHitCol> recMdcDedxHitCol(dataSvc,"/Event/Recon/RecMdcDedxHitCol");
    if (!recMdcDedxHitCol) {
        log << MSG::FATAL << "Could not find RecMdcDedxHitCol" << endreq;
        return( StatusCode::FAILURE);
    }


    int trackID;
    // Retrieve Mdc Track     
    SmartDataPtr<RecMdcTrackCol> mdcTrackCol(dataSvc, EventModel::Recon::RecMdcTrackCol);
    if(!mdcTrackCol)
    {
        log << MSG::INFO << "Could not find RecMdcTrackCol" << endreq;
        SmartDataPtr<DstMdcTrackCol> dstMdcTrackCol(dataSvc,"/Event/Dst/DstMdcTrackCol");
        if (!dstMdcTrackCol) {
            log << MSG::INFO << "Could not find DstMdcTrackCol" << endreq;
        } 
        else {
            RecMdcTrackCol* mdcTrackCol = new RecMdcTrackCol();
            DstMdcTrackCol::iterator iter_mdc = dstMdcTrackCol->begin();
            trackID = 0;
            for (;iter_mdc != dstMdcTrackCol->end(); iter_mdc++, trackID++) {
                RecMdcTrack* recMdcTrack = new RecMdcTrack(); 
                *recMdcTrack = **iter_mdc; 
                (*mdcTrackCol).push_back(recMdcTrack);
                log << MSG::INFO 
                    << " Mdc Track ID = " << trackID
                    << " Mdc Track Nster = " << (*iter_mdc)->nster()
                    << endreq; 
            }       
            sc = dataSvc->registerObject(EventModel::Recon::RecMdcTrackCol,mdcTrackCol);
        }
    }

    // Retrieve MdcKal track 
    SmartDataPtr<RecMdcKalTrackCol> mdcKalTrackCol(dataSvc, EventModel::Recon::RecMdcKalTrackCol);
    if (!mdcKalTrackCol) {
        log << MSG::INFO << "Could not find RecMdcKalTrackCol" << endreq;
        SmartDataPtr<DstMdcKalTrackCol> dstMdcKalTrackCol(dataSvc,"/Event/Dst/DstMdcKalTrackCol");
        if(!dstMdcKalTrackCol) {
            log << MSG::INFO << "Could not find DstMdcKalTrackCol" << endreq;
        }
        else {
            RecMdcKalTrackCol* mdcKalTrackCol = new RecMdcKalTrackCol();
            DstMdcKalTrackCol::iterator iter_mdc = dstMdcKalTrackCol->begin();
            trackID = 0;
            for (;iter_mdc != dstMdcKalTrackCol->end(); iter_mdc++, trackID++) {
                RecMdcKalTrack* recMdcKalTrack = new RecMdcKalTrack();
                *recMdcKalTrack = **iter_mdc;
                (*mdcKalTrackCol).push_back(recMdcKalTrack);
                log << MSG::INFO
                    << " MdcKalTrack ID = " << trackID
                    << " MdcKalTrack Nster = " << (*iter_mdc)->nster()
                    << " MdcKalTrack poca = " << (*iter_mdc)->poca()
                    << endreq;
            }
            sc = dataSvc->registerObject(EventModel::Recon::RecMdcKalTrackCol, mdcKalTrackCol);
        }
    }


    //**********************************************************************
    // now convert
    TIter dedxIter(m_recMdcDedxCol);
    TRecMdcDedx *recMdcDedxRoot = 0;
    while ((recMdcDedxRoot = (TRecMdcDedx*)dedxIter.Next())) {
        double dedxHit = recMdcDedxRoot->dedxHit();
        double dedxEsat = recMdcDedxRoot->dedxEsat();
        double dedxNoRun = recMdcDedxRoot->dedxNoRun();
        double dedxMoment = recMdcDedxRoot->dedxMoment();        

        int trackId =  recMdcDedxRoot->trackId();	    
        int particleId = recMdcDedxRoot->particleId(); 
        int status = recMdcDedxRoot->status();
        int truncAlg = recMdcDedxRoot->truncAlg(); 
        // double pb[5];
        // for ( int i = 0; i < 5; i++) 
        //      pb[i] = dedxRoot->prob(i);

        // double numSigmaE  = dedxRoot->numSigmaE();   
        // double numSigmaMu = dedxRoot->numSigmaMu();  
        // double numSigmaPi = dedxRoot->numSigmaPi();  
        // double numSigmaK = dedxRoot->numSigmaK();   
        // double numSigmaP = dedxRoot->numSigmaP();   
        double chi[5];
        chi[0] = recMdcDedxRoot->chiE();
        chi[1] = recMdcDedxRoot->chiMu();
        chi[2] = recMdcDedxRoot->chiPi();
        chi[3] = recMdcDedxRoot->chiK();
        chi[4] = recMdcDedxRoot->chiP();
        int numGoodHits = recMdcDedxRoot->numGoodHits(); 
        int numTotalHits = recMdcDedxRoot->numTotalHits();

        double probPH = recMdcDedxRoot->probPH();      
        double normPH = recMdcDedxRoot->normPH();
        double errorPH = recMdcDedxRoot->errorPH();
        double twentyPH = recMdcDedxRoot->twentyPH(); 
        double dedxExpect[5],sigmaDedx[5],pidProb[5];

        for (int i=0; i<5; i++){
            dedxExpect[i] = recMdcDedxRoot->dedxExpect(i);
            sigmaDedx[i] = recMdcDedxRoot->sigmaDedx(i);
            pidProb[i] = recMdcDedxRoot->pidProb(i);}

            log << MSG::DEBUG<<"TObjectToDataObject: check Reconstrunction of dE/dx root::"<<" trackId: "<<trackId<<" particleId: "<<particleId<<" status: "<<status<<" truncAlg: "<<truncAlg<<" chi[2]: "<<chi[2]<<" numTotalHits: "<<numTotalHits<<" probPH: "<<probPH<<" errorPH: "<<errorPH<<" dedxExpect[2]: "<<dedxExpect[2]<<endreq;

            RecMdcDedx *recMdcDedx = new RecMdcDedx();
            m_common.m_rootRecMdcDedxMap[recMdcDedxRoot] = recMdcDedx;

            recMdcDedx->setTrackId(trackId);
            recMdcDedx->setParticleId(particleId);
            recMdcDedx->setStatus (status);
            recMdcDedx->setTruncAlg(truncAlg);
            // dedxTds->setProb(pb); 
            // dedxTds->setNumSigmaE(numSigmaE);       
            // dedxTds->setNumSigmaMu(numSigmaMu);     
            // dedxTds->setNumSigmaPi(numSigmaPi);     
            // dedxTds->setNumSigmaK(numSigmaK);       
            // dedxTds->setNumSigmaP(numSigmaP);       
            recMdcDedx->setChi(chi);
            recMdcDedx->setNumGoodHits( numGoodHits);     
            recMdcDedx->setNumTotalHits( numTotalHits); 

            recMdcDedx->setProbPH(probPH);             
            recMdcDedx->setNormPH(normPH);
            recMdcDedx->setErrorPH(errorPH);
            recMdcDedx->setTwentyPH(twentyPH);
            //for (int i=0; i<5; i++){
            recMdcDedx->setDedxExpect(dedxExpect);
            recMdcDedx->setSigmaDedx(sigmaDedx);
            recMdcDedx->setPidProb(pidProb);

            recMdcDedx->setDedxHit(dedxHit);   
            recMdcDedx->setDedxEsat(dedxEsat);  
            recMdcDedx->setDedxNoRun(dedxNoRun); 
            recMdcDedx->setDedxMoment(dedxMoment);     
            //}

            DedxHitRefVec theDedxHitRefVec;
            RecMdcDedxHitCol::iterator iter = recMdcDedxHitCol->begin();
            for (;iter != recMdcDedxHitCol->end(); iter++){
                if((*iter)->getTrkId() == trackId){
                    SmartRef<RecMdcDedxHit> refDedxHit(*iter);
                    theDedxHitRefVec.push_back(refDedxHit);
                }
            }
            recMdcDedx->setVecDedxHits(theDedxHitRefVec);
            int nhits = recMdcDedx->getVecDedxHits().size();
            //std::cout<<" mdc hits: "<<nhits<< std::endl;
            for(int ii=0; ii <nhits ; ii++){
                Identifier id(recMdcDedx->getVecDedxHits()[ii]->getMdcId());
                int layer = MdcID::layer(id);
                int wire = MdcID::wire(id);
                //cout<<"layer: "<<layer<<" wire: "<<wire<<endl;
            }

            int mdcTrackId = recMdcDedxRoot->mdcTrackId();
            //std::cout << __FILE__ << __LINE__ << " size: " << mdcTrackCol->size() << "  id: " << mdcTrackId << std::endl;
            if ( mdcTrackId >= 0 ) { 
                recMdcDedx->setMdcTrack(
                            dynamic_cast<RecMdcTrack*>(mdcTrackCol->containedObject(mdcTrackId)));
            }
            int mdcKalTrackId = recMdcDedxRoot->mdcKalTrackId();
            //std::cout << __FILE__ << __LINE__ << " size: " << mdcKalTrackCol->size() << "  id: " << mdcKalTrackId<< std::endl;
            if ( mdcKalTrackId >= 0 ) {
                recMdcDedx->setMdcKalTrack(
                            dynamic_cast<RecMdcKalTrack*>(mdcKalTrackCol->containedObject(mdcKalTrackId)));
            }

            recMdcDedxCol->push_back(recMdcDedx); 
            //delete dedxTds;
            // dedxTds = NULL;
    }
    //m_dedxCol->Delete();
    delete m_recMdcDedxCol;
    m_recMdcDedxCol = 0;

    return StatusCode::SUCCESS;
}


StatusCode RecMdcDedxCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {
    MsgStream log(msgSvc(), "RecMdcDedxCnv");
    log << MSG::DEBUG << "RecMdcDedxCnv::DataObjectToTObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    RecMdcDedxCol * recMdcDedxCol=dynamic_cast<RecMdcDedxCol *> (obj);
    if (!recMdcDedxCol) {
        log << MSG::ERROR << "Could not downcast to RecMdcDedxCol" << endreq;
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
        log << MSG::ERROR << "RecMdcDedxCnv:Could not downcast to TDS RecEvent" << endreq;
    }

    IOpaqueAddress *addr;
    m_cnvSvc->getRecTrackCnv()->createRep(evt,addr); 
    TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();

    const TObjArray *m_recMdcDedxCol = recEvt->getRecMdcDedxCol();
    if (!m_recMdcDedxCol) return sc;
    recEvt->clearRecMdcDedxCol(); //necessary in case there is I/O at the same time since array is static

    //******************** get according mdcTrack and MdcKalmanTrack of MdcDedx  ****************

    RecMdcTrackCol::iterator    recMdcTrackColbegin,    recMdcTrackColend;
    RecMdcKalTrackCol::iterator recMdcKalTrackColbegin, recMdcKalTrackColend;

    IDataProviderSvc* dataSvc = 0;
    sc = serviceLocator()->getService("EventDataSvc", 
                IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
    if ( sc.isFailure() ) {
        log << MSG::FATAL << "Could not get EventDataSvc in RecMdcDedxCnv" << endreq;
        return sc;
    }
    SmartDataPtr<RecMdcTrackCol> recMdcTrackCol(dataSvc, EventModel::Recon::RecMdcTrackCol);
    if (!recMdcTrackCol) {
        log << MSG::ERROR << "Could not downcast to RecMdcTrackCol" << endreq;
        return StatusCode::FAILURE;
    }
    else {
        recMdcTrackColbegin = recMdcTrackCol->begin();
        recMdcTrackColend   = recMdcTrackCol->end();
    }

    SmartDataPtr<RecMdcKalTrackCol> recMdcKalTrackCol(dataSvc, EventModel::Recon::RecMdcKalTrackCol);
    if (!recMdcKalTrackCol) {
        log << MSG::ERROR << "Could not downcast to RecMdcKalTrackCol" << endreq;
        return StatusCode::FAILURE;
    }
    else {
        recMdcKalTrackColbegin = recMdcKalTrackCol->begin();
        recMdcKalTrackColend   = recMdcKalTrackCol->end();
    }
    //******************************************************************************

    RecMdcDedxCol::const_iterator recMdcDedx;

    for (recMdcDedx = recMdcDedxCol->begin(); recMdcDedx != recMdcDedxCol->end(); recMdcDedx++) {
        Int_t trackId = (*recMdcDedx)->trackId();
        Int_t particleId = (*recMdcDedx)->particleId(); 
        Int_t status = (*recMdcDedx)->status();     
        Int_t truncAlg = (*recMdcDedx)->truncAlg();
        // Double_t pb[5];
        // for (Int_t i = 0; i < 5; i++) 
        // pb[i] = (*dedxTds)->prob(i);
        Double_t dedxHit    = (*recMdcDedx)->getDedxHit();
        Double_t dedxEsat   = (*recMdcDedx)->getDedxEsat();
        Double_t dedxNoRun  = (*recMdcDedx)->getDedxNoRun();
        Double_t dedxMoment = (*recMdcDedx)->getDedxMoment();    


        Double_t chiE  = (*recMdcDedx)->chi(0);   
        Double_t chiMu = (*recMdcDedx)->chi(1);  
        Double_t chiPi = (*recMdcDedx)->chi(2);  
        Double_t chiK = (*recMdcDedx)->chi(3);   
        Double_t chiP = (*recMdcDedx)->chi(4);   

        Int_t numGoodHits = (*recMdcDedx)->numGoodHits(); 
        Int_t numTotalHits = (*recMdcDedx)->numTotalHits();

        Double_t probPH = (*recMdcDedx)->probPH();
        Double_t normPH = (*recMdcDedx)->normPH();
        Double_t errorPH = (*recMdcDedx)->errorPH();
        Double_t twentyPH = (*recMdcDedx)->twentyPH();
        //Double_t fracErrPH = (*dedxTds)-> fracErrPH();  
        //Double_t minIronPH = (*dedxTds)->minIronPH();   
        //Double_t corrPH = (*dedxTds)->corrPH();      
        double dedxExpect[5],sigmaDedx[5],pidProb[5],chi[5];
        for (int i=0; i<5; i++){
            chi[i] = (*recMdcDedx)->chi(i);
            dedxExpect[i] = (*recMdcDedx)->getDedxExpect(i);
            sigmaDedx[i] = (*recMdcDedx)->getSigmaDedx(i);
            pidProb[i] = (*recMdcDedx)->getPidProb(i);
        }

        log << MSG::DEBUG<<"DataObjectToTObject: check Reconstrunction of dE/dx::"<<" trackId: "<<trackId<<" particleId: "<<particleId<<" status: "<<status<<" truncAlg: "<<truncAlg<<" chiPi: "<<chiPi<<" numTotalHits: "<<numTotalHits<<" probPH: "<<probPH<<" errorPH: "<<errorPH<<" dedxExpect[2]: "<<dedxExpect[2]<<endreq;


        TRecMdcDedx *recMdcDedxRoot = new TRecMdcDedx();
        //m_common.m_recMdcDedxMap[(*recMdcDedx)] = recMdcDedxRoot;
        //std::cout<<"check write to Reconstrunction root particle Id is "<<dedxRoot->particleId()<<endl; 
        recMdcDedxRoot->setTrackId(trackId);
        recMdcDedxRoot->setParticleId(particleId);  
        recMdcDedxRoot->setStatus (status); 
        recMdcDedxRoot->setTruncAlg(truncAlg);
        
        //dedxRoot->setProb(pb); 
        recMdcDedxRoot->setChiE(chiE);       
        recMdcDedxRoot->setChiMu(chiMu);     
        recMdcDedxRoot->setChiPi(chiPi);     
        recMdcDedxRoot->setChiK(chiK);       
        recMdcDedxRoot->setChiP(chiP);       

        recMdcDedxRoot->setNumGoodHits( numGoodHits);     
        recMdcDedxRoot->setNumTotalHits(   numTotalHits); 

        recMdcDedxRoot->setProbPH(probPH);             
        recMdcDedxRoot->setNormPH(normPH);
        recMdcDedxRoot->setErrorPH(errorPH);
        recMdcDedxRoot->setTwentyPH(twentyPH);
        //  for (int i=0; i<5; i++){
        recMdcDedxRoot->setChi(chi);
        recMdcDedxRoot->setDedxExpect(dedxExpect);
        recMdcDedxRoot->setSigmaDedx(sigmaDedx);
        recMdcDedxRoot->setPidProb(pidProb);

        recMdcDedxRoot->setDedxHit(dedxHit);   
        recMdcDedxRoot->setDedxEsat(dedxEsat);
        recMdcDedxRoot->setDedxNoRun(dedxNoRun);
        recMdcDedxRoot->setDedxMoment(dedxMoment);    
        //}

        if ( (*recMdcDedx)->isMdcTrackValid() ) {
            RecMdcTrackCol::iterator it = find(recMdcTrackColbegin, recMdcTrackColend, (*recMdcDedx)->getMdcTrack());
            recMdcDedxRoot->setMdcTrackId( it - recMdcTrackColbegin );
        }
        else {
            recMdcDedxRoot->setMdcTrackId( -1 );
        }

        if ( (*recMdcDedx)->isMdcKalTrackValid() ) {  
            RecMdcKalTrackCol::iterator it = find(recMdcKalTrackColbegin, recMdcKalTrackColend, (*recMdcDedx)->getMdcKalTrack());
            recMdcDedxRoot->setMdcKalTrackId( it - recMdcKalTrackColbegin );
        }
        else {
            recMdcDedxRoot->setMdcKalTrackId( -1 );
        }

        log << MSG::INFO<<"check Reconstrunction root"<<" particle Id is : "<<particleId
            <<"track id is : "<<trackId
            <<" and status is : "<<status<<endreq;

        // dedxRoot->setFracErrPH(fracErrPH);       
        //dedxRoot->setMinIronPH(minIronPH);       
        //dedxRoot->setCorrPH(corrPH);             
        recEvt->addRecMdcDedx(recMdcDedxRoot);
    }

    return StatusCode::SUCCESS;
}

#endif
