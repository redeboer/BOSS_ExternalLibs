#ifndef EVENTINFO_EVENTINCIDENT_H
# define EVENTINFO_EVENTINCIDENT_H 1


//<<<<<< INCLUDES                                                       >>>>>>
#include <string>
#ifndef GAUDIKERNEL_INCIDENT_H
# include "GaudiKernel/Incident.h"
#endif

//<<<<<< FORWARD DECLS                                                  >>>>>>

class EventInfo;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/** @class EventIncident
 * @brief an incident that carries the EventInfo object
 * @author pcalafiura@lbl.gov - ATLAS Collaboration
 * $Id: EventIncident.h,v 1.1.1.1 2007/04/25 11:46:57 zoujh Exp $
 **/

class EventIncident : public Incident {
public:

  /// \name structors
  //@{
  /// standard constructor
  ///@param event    the EventInfo of the event in question
  ///@param source   the name of the service/algorithm firing
  ///@param type     e.g. "BeginEvent"
  EventIncident(const EventInfo& event, 
		const std::string& source,
		const std::string& type=std::string("BeginEvent"));
  EventIncident(const EventIncident& e);
  virtual ~EventIncident();
  //@}

  /// \name Event information accessors
  //@{
  ///the unique identification of the event.
  const EventInfo& eventInfo () const;
  //@}

private:
  const EventInfo& m_eventInfo;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
inline const EventInfo&
EventIncident::eventInfo () const
{
    return m_eventInfo;
}
#endif // EVENTINFO_EVENTINCIDENT_H


