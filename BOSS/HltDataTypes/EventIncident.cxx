#include "HltDataTypes/EventIncident.h"

EventIncident::EventIncident(const EventInfo& event,
			     const std::string& source,
			     const std::string& type) :
  Incident(source, type), m_eventInfo(event) {}

EventIncident::EventIncident(const EventIncident& e) :
  Incident(e), m_eventInfo(e.m_eventInfo) {}

EventIncident::~EventIncident() {}
