#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "LumiDigi/LumiDigi.h"
#include "RootEventData/TLumiDigi.h"
#include "RootCnvSvc/Digi/DigiCnv.h"
#include "RootCnvSvc/Digi/LumiDigiCnv.h"
#include "RootCnvSvc/RootAddress.h"

LumiDigiCnv::LumiDigiCnv(ISvcLocator* svc)
   : RootEventBaseCnv(classID(), svc)
{
   m_rootBranchname = "m_lumiDigiCol";

   m_adresses.push_back(&m_lumiDigiCol);
   m_lumiDigiCol = 0;
}

StatusCode LumiDigiCnv::TObjectToDataObject(DataObject*& refpObject) {
   // creation of TDS object from root object

   MsgStream log(msgSvc(), "LumiDigiCnv");
   log << MSG::DEBUG << "LumiDigiCnv::TObjectToDataObject" << endreq;
   StatusCode sc = StatusCode::SUCCESS;

   LumiDigiCol* lumiDigiTdsCol = new LumiDigiCol;
   refpObject = lumiDigiTdsCol;

   if ( !m_lumiDigiCol ) return sc;
   TIter lumiDigiIter(m_lumiDigiCol);
   TLumiDigi* lumiDigiRoot = 0;
   while ((lumiDigiRoot = (TLumiDigi*)lumiDigiIter.Next())) {
      LumiDigi* lumiDigiTds = new LumiDigi( lumiDigiRoot->getIntId() );
      m_common.m_rootLumiDigiMap[lumiDigiRoot] = lumiDigiTds;

      lumiDigiTds->setTimeChannel( lumiDigiRoot->getTimeChannel() );
      lumiDigiTds->setChargeChannel( lumiDigiRoot->getChargeChannel() );
      lumiDigiTds->setOverflow( lumiDigiRoot->getOverflow() );

      lumiDigiTdsCol->push_back(lumiDigiTds);
   }

   delete m_lumiDigiCol;
   m_lumiDigiCol = 0;
   return sc;
}

StatusCode LumiDigiCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {

   MsgStream log(msgSvc(), "LumiDigiCnv");
   log << MSG::DEBUG << "LumiDigiCnv::DataObjectToTObject" << endreq;
   StatusCode sc = StatusCode::SUCCESS;

   LumiDigiCol* lumiDigiColTds = dynamic_cast<LumiDigiCol*>(obj);
   if ( !lumiDigiColTds ) {
      log << MSG::ERROR << "Could not downcast to LumiDigiCol" << endreq;
      return StatusCode::FAILURE;
   }

   DataObject* evt;
   m_eds->findObject(EventModel::Digi::Event, evt);
   if ( evt == NULL ) {
      log << MSG::ERROR << "Could not get DigiEvent in TDS" << endreq;
      return StatusCode::FAILURE;
   }
   DigiEvent* devtTds = dynamic_cast<DigiEvent*>(evt);
   if ( !devtTds ) {
      log << MSG::ERROR << "LumiDigiCnv: Could not downcast to TDS DigiEvent" << endreq;
   }
   IOpaqueAddress* addr;

   m_cnvSvc->getDigiCnv()->createRep(evt, addr);

   TDigiEvent* recEvt = m_cnvSvc->getDigiCnv()->getWriteObject();

   const TObjArray* m_lumiDigiCol = recEvt->getLumiDigiCol();
   if ( !m_lumiDigiCol ) return sc;
   recEvt->clearLumiDigiCol();
   LumiDigiCol::const_iterator lumiDigiTds;

   for (lumiDigiTds = lumiDigiColTds->begin(); lumiDigiTds != lumiDigiColTds->end(); ++lumiDigiTds) {
      TLumiDigi* lumiDigiRoot = new TLumiDigi();

      lumiDigiRoot->initialize((*lumiDigiTds)->getIntId(),
	    (*lumiDigiTds)->getTimeChannel(), (*lumiDigiTds)->getChargeChannel());
      lumiDigiRoot->setOverflow( (*lumiDigiTds)->getOverflow() );

      recEvt->addLumiDigi(lumiDigiRoot);
   }

   return sc;
}
