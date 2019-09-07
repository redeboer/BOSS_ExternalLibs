#ifndef Event_EVENTH_H
#define Event_EVENTH_H 

#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/DataObject.h"
#include "EventModel/EventModel.h"

extern const CLID& CLID_Event;

/** class EventHeader
*   brief Essential header information of the event.
*   It can be identified by "/Event" on the TDS.
*
*   It contains:
*   - run number
*   - event number
**/

namespace Event{

class EventH : public DataObject                                                {
    
public:
    
    EventH()
        : DataObject() {}
    
    virtual ~EventH() {}
    
    /// Retrieve reference to class definition structure
    virtual const CLID& clID() const { return EventH::classID(); }
    static const CLID& classID() { return CLID_Event; }
    
private:
};

} // namespace EventH
#endif    // Event_EVENTH_H

