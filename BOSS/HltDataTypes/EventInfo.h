#ifndef EVENTINFO_EVENTINFO_H
# define EVENTINFO_EVENTINFO_H 1
//<doc><file>	$Id: EventInfo.h,v 1.1.1.1 2007/04/25 11:46:57 zoujh Exp $
//<version>	$Name: HltDataTypes-01-01-03 $


//<<<<<< INCLUDES                                                       >>>>>>
#ifndef GAUDIKERNEL_CLASSID_H
# include "GaudiKernel/ClassID.h"
#endif

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class EventID;
class EventType;
class MixingEventSelector;
class TriggerInfo;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/** @class EventInfo
 * @brief This class provides general information about an event. 
 *  It extends EventInfo with a list of sub-evts (the original
 *  and the bkg ones) 
 * @author ATLAS Collaboration
 * $Id: EventInfo.h,v 1.1.1.1 2007/04/25 11:46:57 zoujh Exp $
 **/
extern const CLID& CLID_EventInfo;

class EventInfo {
public:

  /// \name structors
  //@{
  EventInfo();
  EventInfo(EventID* id, EventType* type);
  EventInfo(EventID* id, EventType* type, TriggerInfo* trig_info);
  EventInfo(const EventInfo& e);
  virtual ~EventInfo();
  //@}

  /// \name DataObject-like clid accessors
  //@{
  static const CLID& classID();
  const CLID& clID() const;
  //@}
  
  /// \name Event information accessors
  //@{
  ///the unique identification of the event.
  EventID* 		event_ID	() const;
  ///the type of the event, e.g. simulation, testbeam, etc
  EventType*		event_type	() const;
  /// trigger information (ptr may be NULL)
  TriggerInfo*          trigger_info    () const;
  //@}
  void setTriggerInfo(TriggerInfo*);
  
protected:
  friend class MixingEventSelector;
  void setEventID(EventID* pid);

private:
  EventID* 	m_event_ID;
  EventType* 	m_event_type;
  TriggerInfo*  m_trigger_info;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline EventID*
EventInfo::event_ID	() const
{
    return (m_event_ID);
}

inline EventType*
EventInfo::event_type	() const
{
    return (m_event_type);
}

inline TriggerInfo*   
EventInfo::trigger_info    () const
{
    return (m_trigger_info);
}


inline const CLID& 
EventInfo::clID() const
{ 
  return classID(); 
}

/* hxt 
#ifndef CLIDSVC_CLASSDEF_H
# include "CLIDSvc/CLASS_DEF.h"
#endif
CLASS_DEF( EventInfo , 2101 , 1 )
*/  


inline const CLID& 
EventInfo::classID() 
{ 
  return CLID_EventInfo; //hxt ClassID_traits<EventInfo>::ID(); 
}

#endif // EVENTINFO_EVENTINFO_H


