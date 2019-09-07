#ifndef Event_EVENTHeader_H
#define Event_EVENTHeader_H 

#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/DataObject.h"
#include "EventModel/EventModel.h"

extern const CLID& CLID_EventHeader;

/** class EventHeader
*   brief Essential header information of the event.
*   It can be identified by "/Event" on the TDS.
*
*   It contains:
*   - run number
*   - event number
**/

namespace Event{

class EventHeader : public DataObject                                                {
    
public:
    
    EventHeader()
        : DataObject(), m_event(-1), m_run(-1) {}
    
    virtual ~EventHeader() {}
    
    /// Retrieve reference to class definition structure
    virtual const CLID& clID() const { return EventHeader::classID(); }
    static const CLID& classID() { return CLID_EventHeader; }
    
    /// Retrieve event number
    int eventNumber () const { return m_event; }
    /// Update event number
    void setEventNumber (int value) { m_event = value; }
    
    /// Retrieve run number
    int runNumber () const { return m_run; }
    /// Update run number
    void setRunNumber (int value) { m_run = value; }

    unsigned int time() const { return m_time; }

    void setTime(int value) { m_time = value; }

/// Retrieve EventTag
    unsigned int eventTag(){return m_EventTag; }
   /// Update  EventTag 
    void  setEventTag(unsigned int value){ m_EventTag=value;} 

    /// Retrieve flags
    unsigned int flag1() { return m_flag1; };
    unsigned int flag2() { return m_flag2; };
    /// Update flags
    void  setFlag1(unsigned int value) { m_flag1 = value; }
    void  setFlag2(unsigned int value) { m_flag2 = value; }

private:
    /// Event number
    int m_event;

    /// Run number
    int m_run;

    unsigned int m_time;

    // EventTag
    unsigned int m_EventTag ;

    // flag1-HV, bit(11):has-MDC-HV, bit(8-10):MDC-HV-status
    //           bit(7):has-TOF-HV,  bit(4-6):TOF-HV-status
    //           bit(3):has-MUC-HV,  bit(0-2):MUC-HV-status
    unsigned int m_flag1;

    // flag2
    unsigned int m_flag2;
};

} // namespace Event
#endif    // Event_EVENTHeader_H

