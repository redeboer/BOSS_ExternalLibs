#include "RootCnvSvc/Rec/RecZddChannelCnv.h"
#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "ZddEvent/RecZddChannel.h"
#include "RootEventData/TRecTrackEvent.h"
#include "RootEventData/TRecZddChannel.h"

RecZddChannelCnv::RecZddChannelCnv(ISvcLocator* svc)
    : RootEventBaseCnv(classID(), svc)
{
    m_rootBranchname = "m_recZddChannelCol";
    m_adresses.push_back(&m_recZddChannelCol);
    m_recZddChannelCol = 0;
}

StatusCode RecZddChannelCnv::TObjectToDataObject(DataObject*& refpObject)
{
    MsgStream log(msgSvc(), "RecZddChannelCnv");
    log << MSG::DEBUG << "RecZddChannelCnv::TObjectToDataObject" << endreq;

    RecZddChannelCol* zddCol = new RecZddChannelCol;
    refpObject = zddCol;

    if ( ! m_recZddChannelCol ) return StatusCode::SUCCESS;

    // convert
    TIter zddIter(m_recZddChannelCol);
    TRecZddChannel* zddRoot = 0;
    while ( (zddRoot = (TRecZddChannel*)zddIter.Next()) ) {
	RecZddChannel* zddTDS = new RecZddChannel;
	m_common.m_rootRecZddChannelMap[zddRoot] = zddTDS;

	// settings
	zddTDS->setChannelId(zddRoot->channelId());
	zddTDS->setScanCode(zddRoot->scanCode());
	zddTDS->setBaseLine(zddRoot->baseLine());
	zddTDS->setPhase(zddRoot->phase());
	zddTDS->setFragments(zddRoot->fragments());

	zddCol->push_back(zddTDS);
    }

    delete m_recZddChannelCol;
    m_recZddChannelCol = 0;

    return StatusCode::SUCCESS;
}

StatusCode RecZddChannelCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr)
{
    MsgStream log(msgSvc(), "RecZddChannelCnv");
    log << MSG::DEBUG << "RecZddChannelCnv::DataObjectToTObject" << endreq;

    RecZddChannelCol* zddCol = dynamic_cast<RecZddChannelCol*>(obj);
    if ( ! zddCol ) {
	log << MSG::ERROR << "Could not downcast to RecZddChannelCol" << endreq;
	return StatusCode::FAILURE;
    }

    DataObject* evt;
    m_eds->findObject(EventModel::Recon::Event, evt);
    if ( evt == NULL ) {
	log << MSG::ERROR << "Could not get ReconEvent in TDS" << endreq;
	return StatusCode::FAILURE;
    }
    IOpaqueAddress *addr;
    m_cnvSvc->getRecTrackCnv()->createRep(evt, addr);
    TRecTrackEvent* recEvt = m_cnvSvc->getRecTrackCnv()->getWriteObject();

    const TObjArray* m_recZddChannelCol = recEvt->getRecZddChannelCol();
    if ( ! m_recZddChannelCol ) return StatusCode::SUCCESS;

    //convert
    recEvt->clearRecZddChannelCol();
    RecZddChannelCol::const_iterator zdd = zddCol->begin();

    for ( ; zdd != zddCol->end(); ++zdd ) {
	RecZddChannel* pzdd = *zdd;
	TRecZddChannel* zddRoot = new TRecZddChannel();

	zddRoot->setChannelId(pzdd->channelId());
	zddRoot->setBaseLine(pzdd->baseLine());
	zddRoot->setPhase(pzdd->phase());
	zddRoot->setFragments(pzdd->fragments());

	recEvt->addRecZddChannel(zddRoot);
    }

    return StatusCode::SUCCESS;
}
