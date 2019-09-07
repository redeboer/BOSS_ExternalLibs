/***************************************************************************
 EventInfo Package
 -----------------------------------------
 Copyright (C) 2000 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: EventID.h,v 1.1.1.1 2007/04/25 11:46:57 zoujh Exp $
//<version>	$Name: HltDataTypes-01-01-03 $

#ifndef EVENTINFO_EVENTID_H
# define EVENTINFO_EVENTID_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

#include <iostream>

// 
//  class EventID
//
//  This class provides a unique identification for each event.
//
class EventID {
public:

  typedef unsigned int number_type;
  
  EventID();
  EventID(number_type run_number, 
	  number_type event_number);
  EventID(number_type run_number, 
	  number_type event_number,
	  number_type time_stamp);
  explicit EventID(const EventID& id);
  virtual ~EventID();
  
  number_type 	run_number	(void) const;
  number_type 	event_number	(void) const;
  number_type   time_stamp      (void) const; // posix time in seconds from 1970
  
  // Comparison operators
  friend bool operator<(const EventID& lhs, const EventID& rhs);
  friend bool operator>(const EventID& lhs, const EventID& rhs);
  friend bool operator==(const EventID& lhs, const EventID& rhs);
  friend bool operator!=(const EventID& lhs, const EventID& rhs);
  friend bool operator<=(const EventID& lhs, const EventID& rhs);
  friend bool operator>=(const EventID& lhs, const EventID& rhs);

  // Insertion and extraction operators
  friend std::istream& operator>>(std::istream& is, EventID& rhs);

  template <class STR>
  friend STR& operator<<(STR& os, const EventID& rhs);

private:
  number_type 	m_run_number;
  number_type 	m_event_number;

  // posix time in seconds since 1970/01/01
  number_type   m_time_stamp;
  
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>

inline bool operator<(const EventID& lhs, const EventID& rhs) {
  // We are assuming that ALL events will have run and event numbers,
  // and never just a time stamp.
  return lhs.m_run_number<rhs.m_run_number ||
         ( lhs.m_run_number==rhs.m_run_number && 
	   lhs.m_event_number<rhs.m_event_number) ;
}
inline bool operator==(const EventID& lhs, const EventID& rhs) {
  return lhs.m_run_number==rhs.m_run_number && 
         lhs.m_event_number==rhs.m_event_number ;
}
inline bool operator>(const EventID& lhs, const EventID& rhs) {
  return !( (lhs < rhs) || (lhs == rhs));
}
inline bool operator!=(const EventID& lhs, const EventID& rhs) {
  return !(lhs == rhs);
}
inline bool operator<=(const EventID& lhs, const EventID& rhs) {
  return !(lhs > rhs);
}
inline bool operator>=(const EventID& lhs, const EventID& rhs) {
  return !(lhs < rhs);
}

template <class STR>
inline STR& operator<<(STR& os, const EventID& rhs) {
  os << "[R,E] = [" << rhs.m_run_number << "," << rhs.m_event_number << "]";
  return os;
}

inline std::istream& operator>>(std::istream& is, EventID& rhs) {
  is >> rhs.m_run_number >> rhs.m_event_number;
  return is;
}

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // EVENTINFO_EVENTID_H






