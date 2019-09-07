#ifndef Event_DIGIEVENT_H
#define Event_DIGIEVENT_H 
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/DataObject.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_DigiEvent;


/** class DigiEvent
*   brief Defines the top level object for digitization data.
*   It can be identified by "/Event/Digi" on the TDS
* 
* It contains:
* - m_fromMc, if coming from Monte Carlo
* 
* based GLAST DigiEvent
*/

class DigiEvent : public DataObject {
    
public:
    
    DigiEvent()
        : DataObject(), m_fromMc(true) { }
    
    virtual ~DigiEvent() { }
    
    void initialize(bool fromMc) { m_fromMc = fromMc; };

    /// Retrieve reference to class definition structure
    virtual const CLID& clID() const  { return DigiEvent::classID(); }
    static const CLID& classID() { return CLID_DigiEvent; }
    
    ///  Retrieve flag of origin
    bool fromMc () const {
        return m_fromMc;
    }
    
    
private: 
    /// Flag of origin
    bool m_fromMc;
};

#endif


