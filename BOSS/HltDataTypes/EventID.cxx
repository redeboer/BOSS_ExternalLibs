/***************************************************************************
 Event Info Package
 -----------------------------------------
 Copyright (C) 2000 by ATLAS Collaboration
 ***************************************************************************/


//<<<<<< INCLUDES                                                       >>>>>>

#include "HltDataTypes/EventID.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

EventID::EventID() :
  m_run_number(0),
  m_event_number(0),
  m_time_stamp(0)
{}

EventID::EventID(number_type run_number, 
		 number_type event_number)
  :
  m_run_number(run_number),
  m_event_number(event_number),
  m_time_stamp(0)
{}

EventID::EventID(number_type run_number, 
		 number_type event_number,
		 number_type time_stamp)
  :
  m_run_number(run_number),
  m_event_number(event_number),
  m_time_stamp(time_stamp)
{}

EventID::EventID(const EventID& id) :
  m_run_number(id.m_run_number),
  m_event_number(id.m_event_number),
  m_time_stamp(id.m_time_stamp)
{}

EventID::~EventID()
{}

EventID::number_type 	
EventID::run_number	(void) const
{
    return m_run_number;
}

EventID::number_type 	
EventID::event_number	(void) const
{
    return m_event_number;
}

EventID::number_type 	
EventID::time_stamp	(void) const
{
    return m_time_stamp;
}
