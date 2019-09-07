#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "TrigMakerAlg/TrigEventMaker.h"
#include "TrigEvent/TrigGTD.h"
#include "TrigEvent/TrigData.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

using namespace Event;

TrigEventMaker::TrigEventMaker(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{
  //Declare the properties
}

StatusCode TrigEventMaker::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "TrigEventMaker: in initialize()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode TrigEventMaker::execute(){
  MsgStream log(msgSvc(),name());
  log<<MSG::DEBUG<< "TrigEventMaker: in execute()" <<endreq;

  // Get the event header, print out event and run number
  
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return StatusCode::FAILURE;
  }

  int eventId = eventHeader->eventNumber() ;
  int runId = eventHeader->runNumber();
  log << MSG::INFO << "TrigEventMaker: retrieved event: " <<"  Event Number  "<< eventId << "  run: " <<runId  << endreq;

  //define elements in TrigData structure
  int trgCond[48];
  int trgChan[16];
  int window = 0;
  int timing = 0;
  bool preScale = false;

  //initialize the elements defined above
  for(int i = 0; i < 48; i++) {
    if(i < 16) trgChan[i] = 0;
    trgCond[i] = 0;
  }

  //Get trigger information from TDS
  SmartDataPtr<TrigGTDCol> trigGTDCol(eventSvc(),"/Event/Trig/TrigGTDCol");
  if (!trigGTDCol) {
    log << MSG::FATAL << "Could not find TrigGTDCol!" << endreq;
    return StatusCode::FAILURE;
  }
  
  TrigGTDCol::iterator iter = trigGTDCol->begin();
  for (;iter != trigGTDCol->end(); iter++ ) {
    const uint32_t  boardId = (*iter)->getId();   //The board Id 0xd3: GTL, 0xD2: SAF1, 0xD4: SAF2, 0xD6: SAF3
    const uint32_t  timeWindow = (*iter)->getTimeWindow(); //Time window, bit8 to bit13, total: 0--31
    //const uint32_t  dataType = (*iter)->getDataType(); //bit3 to bit7, total: 0--31
    const uint32_t  size = (*iter)->getDataSize();    //The size of trigger data, not include head
    const uint32_t* trigData = (*iter)->getDataPtr(); //Trigger data

    window = timeWindow;

    //Get data group 5 in GTL, including trigger channel, timing and prescale.
    if(boardId == 0xd3) {
      if(size%timeWindow != 0) {
        log << MSG::FATAL << "GTL data is NOT completed" << endreq;
        return StatusCode::FAILURE;
      }
      for(uint32_t j = 0; j < size; j++) {
        uint32_t dataId = ((trigData[j] >> 24) & 0x7);
        if(dataId != 5) continue; //find data group 5
        for(uint32_t i = 1, loop = 0; loop < 24; i <<= 1, loop++) {
          if(loop < 16) {
            if(trigData[j] & i) trgChan[loop] = 1;
          }
          if((loop == 16) && (trigData[j] & i)) timing = 1;
          if((loop == 17) && (trigData[j] & i) && (timing != 1)) timing = 2;
          if((loop == 18) && (trigData[j] & i) && (timing == 0)) timing = 3;
          if((loop == 21) && (trigData[j] & i)) preScale = true;
        }
      }
    }
    //Get "or 4" in SAF
    if(boardId == 0xd2 || boardId == 0xd4 || boardId == 0xd6) {
      for(uint32_t j = 0; j < size; j++) {
        uint32_t dataId = ((trigData[j] >> 16) & 0xFF);
        if(dataId != 4) continue; //find data "or 4" in SAF
        for(uint32_t i = 1, loop = 0; loop < 16; i <<= 1, loop++) {
          if((boardId == 0xd2) && (trigData[j] & i)) trgCond[32+loop] = 1;
          if((boardId == 0xd4) && (trigData[j] & i)) trgCond[16+loop] = 1;
          if((boardId == 0xd6) && (trigData[j] & i)) trgCond[loop] = 1;
        }
      }
    }
  }

  //Register related trigger information to TDS for physics analysis   
  TrigData* aTrigData = new TrigData(window, timing, trgCond, trgChan, preScale);

  StatusCode sc = StatusCode::SUCCESS ;
  sc = eventSvc()->registerObject(EventModel::Trig::TrigData,aTrigData);
  if(sc!=StatusCode::SUCCESS) {
    log<<MSG::DEBUG<< "Could not register TrigData" <<endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TrigEventMaker::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize TrigEventMaker" << endreq;
  return StatusCode::SUCCESS;
}
