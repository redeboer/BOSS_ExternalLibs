/***************************************************************************
 Event Info Package
 -----------------------------------------
 Copyright (C) 2000 by ATLAS Collaboration
 ***************************************************************************/


//<<<<<< INCLUDES                                                       >>>>>>

#include "HltDataTypes/EventInfo.h"
#include "HltDataTypes/EventID.h"
#include "HltDataTypes/EventType.h"
#include "HltDataTypes/TriggerInfo.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

EventInfo::EventInfo()
    :
    m_event_ID	  (new EventID(0,0)),
    m_event_type  (new EventType()),
    m_trigger_info(new TriggerInfo())
{}

EventInfo::EventInfo(EventID* id, EventType* type)
    :
    m_event_ID	(id),
    m_event_type(type),
    m_trigger_info(0)
{}

EventInfo::EventInfo(EventID* id, EventType* type, TriggerInfo* trig_info)
    :
    m_event_ID	(id),
    m_event_type(type),
    m_trigger_info(trig_info)
{}


EventInfo::EventInfo(const EventInfo& e)
    :
    m_event_ID( e.m_event_ID ?
		new EventID( *(e.m_event_ID) ) :
		new EventID(0,0) ),
    m_event_type( e.m_event_type ?
		  new EventType( *(e.m_event_type) ):
		  new EventType() ),
    m_trigger_info( e.m_trigger_info?
		    new TriggerInfo(*(e.m_trigger_info) ) :
		    new TriggerInfo() )
{}


EventInfo::~EventInfo()
{
    delete m_event_ID;
    delete m_event_type;
    delete m_trigger_info;
}

void 
EventInfo::setEventID(EventID* pid) { 
  delete m_event_ID; m_event_ID=pid; 
}


void
EventInfo::setTriggerInfo(TriggerInfo* pTrig) {
  delete m_trigger_info; m_trigger_info = pTrig;
}

