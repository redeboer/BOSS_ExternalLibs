//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description:
////Author:   Dengzy
////Created:  Mar, 2004
////Modified:
////Comment:
//

#include "BesTDSWriter.hh"
#include "BesRawDataWriter.hh"
#include "BesMcTruthWriter.hh"
#include "G4Event.hh"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "EventModel/EventHeader.h"

BesTDSWriter::BesTDSWriter()
{
  m_rawWriter = new BesRawDataWriter;
  m_mcWriter = new BesMcTruthWriter;
}

BesTDSWriter::~BesTDSWriter()
{
  if(m_rawWriter)
    delete m_rawWriter;
  if(m_mcWriter)
    delete m_mcWriter;
}

void BesTDSWriter::SaveAll(const G4Event* evt, int runNumber)
{
  //interface to event data service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IDataProviderSvc* evtSvc;
  StatusCode sc=svcLocator->service("EventDataSvc", evtSvc);
  if (sc.isFailure())
    G4cout<<"Could not accesss EventDataSvc!"<<G4endl;
  
  //save event head
/*
  SmartDataPtr<Event::EventHeader> eventHeader(evtSvc,"/Event/EventHeader");
  if (!eventHeader) {
    //G4cout<< "Could not find Event Header" << G4endl;
    Event::EventHeader *eventHeader = new Event::EventHeader;
    StatusCode sc = evtSvc->registerObject("/Event/EventHeader",eventHeader);    
  }
  eventHeader->setEventNumber( evt->GetEventID() ); 
  eventHeader->setRunNumber( runNumber );
*/
  //retrieve
  SmartDataPtr<Event::EventHeader> aEventHeader(evtSvc,"/Event/EventHeader");
  G4int run=aEventHeader->runNumber();
  G4int event=aEventHeader->eventNumber();
  //G4cout<<"BesTDSWriter:  runNumber: "<<run<<"  eventNumber:"<<event<<G4endl;
  

  //save raw data
  m_rawWriter->SaveDigits();
  
  //save MC Truth
  m_mcWriter->SaveMcTruth();

}

