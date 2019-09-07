#ifndef EmcMcHitCnv_CXX
#define EmcMcHitCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"


#include "Identifier/Identifier.h"

#include "EventModel/EventModel.h"
#include "McTruth/McEvent.h"   //TDS object
#include "McTruth/EmcMcHit.h"   //TDS object
#include "RootEventData/TEmcMc.h" // standard root object
#include "RootEventData/TMcEvent.h"
#include "RootCnvSvc/Mc/McCnv.h"
#include "RootCnvSvc/Mc/EmcMcHitCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<EmcMcHitCnv> s_factory;
//const ICnvFactory& EmcMcHitCnvFactory = s_factory;

EmcMcHitCnv::EmcMcHitCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "EmcMcHitCnv");
    // log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_emcMcHitCol";
    //declareObject(EventModel::MC::EmcMcHitCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_emcMcHitCol);
    m_emcMcHitCol=0;
}

StatusCode EmcMcHitCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "EmcMcHitCnv");
    log << MSG::DEBUG << "EmcMcHitCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MdcMc Collection
    EmcMcHitCol* emcMcTdsCol = new EmcMcHitCol;
    refpObject=emcMcTdsCol;


    // now convert
    if (!m_emcMcHitCol) return sc;
    TIter emcMcIter(m_emcMcHitCol);
    TEmcMc *emcMcRoot = 0;
    while ((emcMcRoot = (TEmcMc*)emcMcIter.Next())) {
      // unsigned int id = emcMcRoot ->getId();
      Identifier id ;
         unsigned int id_root = emcMcRoot ->getId();
	 id = id_root;
		unsigned int trackIndex = emcMcRoot ->getTrackIndex();

    int   hitEmc = emcMcRoot ->getHitEmc();
    int   PDGCode = emcMcRoot ->getPDGCode();
    double PDGCharge = emcMcRoot ->getPDGCharge();
    double time = emcMcRoot ->getTime();

		double xPosition =   emcMcRoot ->getPositionX() ;
		double yPosition =   emcMcRoot ->getPositionY() ;
		double zPosition =   emcMcRoot ->getPositionZ() ;
		double px=   emcMcRoot ->getPx() ;
    double py =   emcMcRoot ->getPy() ;
    double pz =   emcMcRoot ->getPz() ;
    double depositEnergy = emcMcRoot ->getDepositEnergy() ;

    EmcMcHit *emcMcTds = new EmcMcHit();
    m_common.m_rootEmcMcHitMap[emcMcRoot] = emcMcTds;

    emcMcTds->setIdentifier(id);

    map<Int_t, Double_t>  hitMap = emcMcRoot ->getHitMap();
    map<Identifier, Double_t>  TdshitMap;
    map<Int_t, Double_t> HitMap = emcMcRoot->getHitMap();
    map<Int_t, Double_t>::iterator iHitMap;
    for(iHitMap=HitMap.begin();
            iHitMap!=HitMap.end();
            iHitMap++) {

        Identifier id(iHitMap->first);
        pair<Identifier, Double_t> TdsHit(id, iHitMap->second);
        TdshitMap.insert(TdsHit);

        //RecEmcHitCol::iterator iHit;
        //for(iHit=emcRecHitCol->begin();
        //        iHit!=emcRecHitCol->end();
        //        iHit++) {

        //    RecEmcID idHit((*iHit)->getCellId());

        //    if(id==idHit) {
        //        RecEmcFraction frac(*(*iHit));
        //        frac.Fraction(iHitMap->second);
        //        emcShowerTds->Insert(frac);
        //        break;
        //    }
        //} //RecEmcHitCol
    } //CellIdMap
    emcMcTds->setHitMap(TdshitMap);

    emcMcTds->setHitEmc(hitEmc);
    emcMcTds->setPDGCode(PDGCode);
    emcMcTds->setPDGCharge(PDGCharge);
    emcMcTds->setTime(time);

    emcMcTds->setTrackIndex(trackIndex);
    emcMcTds->setPositionX(xPosition);
    emcMcTds->setPositionY(yPosition);
    emcMcTds->setPositionZ(zPosition);
    emcMcTds->setPx(px);
    emcMcTds->setPy(py);
    emcMcTds->setPz(pz);
    emcMcTds->setDepositEnergy(depositEnergy);

    emcMcTdsCol->push_back(emcMcTds);
    }
    //m_emcMcHitCol->Delete();  // wensp add 2005/12/30
    delete m_emcMcHitCol;
    m_emcMcHitCol = 0;
    return StatusCode::SUCCESS;
}

StatusCode EmcMcHitCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

    MsgStream log(msgSvc(), "EmcMcHitCnv");
    log << MSG::DEBUG << "EmcMcHitCnv::DataObjectToTObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    EmcMcHitCol * emcMcHitCnvTds=dynamic_cast<EmcMcHitCol *> (obj);
    if (!emcMcHitCnvTds) {
        log << MSG::ERROR << "Could not downcast to EmcMcHitCol" << endreq;
        return StatusCode::FAILURE;
    }

    DataObject *evt;
    m_eds->findObject(EventModel::MC::Event,evt);
    if (evt==NULL) {
        log << MSG::ERROR << "Could not get McEvent in TDS "  << endreq;
        return StatusCode::FAILURE;
    }
    McEvent * devtTds=dynamic_cast<McEvent *> (evt);
    if (!devtTds) {
        log << MSG::ERROR << "EmcMcHitCnv:Could not downcast to TDS McEvent" << endreq;
    }
    IOpaqueAddress *addr;

    m_cnvSvc->getMcCnv()->createRep(evt,addr); 
    TMcEvent *McEvt=m_cnvSvc->getMcCnv()->getWriteObject();

    const TObjArray *m_emcMcHitCol = McEvt->getEmcMcHitCol();
    if (!m_emcMcHitCol) return sc;
    McEvt->clearEmcMcHitCol(); //necessary in case there is I/O at the same time since array is static
    EmcMcHitCol::const_iterator emcMcTds;

    for (emcMcTds = emcMcHitCnvTds->begin(); emcMcTds != emcMcHitCnvTds->end(); emcMcTds++) {
        UInt_t id  = ((*emcMcTds)->identify()).get_value() ;
        UInt_t trackIndex = (*emcMcTds) ->getTrackIndex();

        int   hitEmc = (*emcMcTds) ->getHitEmc();
        int   PDGCode = (*emcMcTds) ->getPDGCode();
        double PDGCharge = (*emcMcTds) ->getPDGCharge();
        double time = (*emcMcTds) ->getTime();
        std::map<Identifier, Double_t>  TdshitMap = (*emcMcTds)->getHitMap();

        map<Int_t, Double_t>  HitMap;
        map<Identifier, Double_t>::iterator iHitMap;
        for(iHitMap=TdshitMap.begin();
                iHitMap!=TdshitMap.end();
                iHitMap++) {

            Int_t id(iHitMap->first.get_value());
            pair<Int_t, Double_t> Hit(id, iHitMap->second);
            HitMap.insert(Hit);

            //RecEmcHitCol::iterator iHit;
            //for(iHit=emcRecHitCol->begin();
            //        iHit!=emcRecHitCol->end();
            //        iHit++) {

            //    RecEmcID idHit((*iHit)->getCellId());

            //    if(id==idHit) {
            //        RecEmcFraction frac(*(*iHit));
            //        frac.Fraction(iHitMap->second);
            //        emcShowerTds->Insert(frac);
            //        break;
            //    }
            //} //RecEmcHitCol
        } //CellIdMap

        Double_t xPosition =   (*emcMcTds) ->getPositionX() ;
        Double_t yPosition =   (*emcMcTds) ->getPositionY() ;
        Double_t zPosition =   (*emcMcTds) ->getPositionZ() ;
        Double_t px=   (*emcMcTds) ->getPx() ;
        Double_t py =   (*emcMcTds) ->getPy() ;
        Double_t pz =   (*emcMcTds) ->getPz() ;
        Double_t depositEnergy = (*emcMcTds) ->getDepositEnergy() ;

        TEmcMc *emcMcRoot = new TEmcMc();
        //m_common.m_emcMcHitMap[(*emcMcTds)] = emcMcRoot;

        emcMcRoot->setId(id);
        emcMcRoot->setHitMap(HitMap);

        emcMcRoot->setHitEmc(hitEmc);
        emcMcRoot->setPDGCode(PDGCode);
        emcMcRoot->setPDGCharge(PDGCharge);
        emcMcRoot->setTime(time);
        //emcMcRoot->setHitMap(hitMap);

        emcMcRoot->setTrackIndex(trackIndex);
        emcMcRoot->setPositionX(xPosition);
        emcMcRoot->setPositionY(yPosition);
        emcMcRoot->setPositionZ(zPosition);
        emcMcRoot->setPx(px);
        emcMcRoot->setPy(py);
        emcMcRoot->setPz(pz);
        emcMcRoot->setDepositEnergy(depositEnergy);

        McEvt->addEmcMc(emcMcRoot);
    }

    return StatusCode::SUCCESS;
}
#endif










