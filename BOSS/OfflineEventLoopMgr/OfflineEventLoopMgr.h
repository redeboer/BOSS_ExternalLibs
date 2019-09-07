// $Id: OfflineEventLoopMgr.h,v 1.6 2011/02/15 06:57:12 dengzy Exp $
#ifndef OFFLINE_EVENTLOOPMGR_H
#define OFFLINE_EVENTLOOPMGR_H

// Framework include files
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/MinimalEventLoopMgr.h"
#include "GaudiKernel/SvcFactory.h"  

// Forward declarations
class IIncidentSvc;
class IDataManagerSvc;
class IDataProviderSvc;
class IConversionSvc;
/** Class definition of OfflineEventLoopMgr.
    This is the default processing manager of the application manager.
    This object handles the minimal requirements needed by the 
    application manager. It also handles the default user 
    configuration setup for standard event processing.

    <UL>
    <LI> handling of the incidents when starting event processing
    <LI> handling of the event loop
    </UL>

    <B>History:</B>
    <PRE>
    +---------+----------------------------------------------+---------+
    |    Date |                 Comment                      | Who     |
    +---------+----------------------------------------------+---------+
    |13/12/00 | Initial version                              | M.Frank |
    +---------+----------------------------------------------+---------+
    </PRE>
   @author Markus Frank
   @version 1.0
*/
class OfflineEventLoopMgr : public MinimalEventLoopMgr   {
public:
  /// Creator friend class
  friend class SvcFactory<OfflineEventLoopMgr>;

protected:
  /// Reference to the indicent service
  IIncidentSvc*     m_incidentSvc;
  /// Reference to the Event Data Service's IDataManagerSvc interface
  IDataManagerSvc*  m_evtDataMgrSvc;
  /// Reference to the Event Data Service's IDataProviderSvc interface
  IDataProviderSvc* m_evtDataSvc;
  /// Reference to the Event Selector
  IEvtSelector*     m_evtSelector;
  /// Event Iterator
  IEvtSelector::Context*      m_evtContext;
  /// Event selector
  std::string       m_evtsel;
  /// Reference to the Histogram Data Service
  IDataManagerSvc*  m_histoDataMgrSvc;
  /// Reference to the Histogram Persistency Service
  IConversionSvc*   m_histoPersSvc;
  /// Name of the Hist Pers type
  std::string       m_histPersName;
  /// Property interface of ApplicationMgr
  IProperty*        m_appMgrProperty;

protected:
  /// Standard Constructor
  OfflineEventLoopMgr(const std::string& nam, ISvcLocator* svcLoc);
  /// Standard Destructor
  virtual ~OfflineEventLoopMgr();
  /// Create event address using event selector
  StatusCode getEventRoot(IOpaqueAddress*& refpAddr);

public:
  /// implementation of IService::initalize
  virtual StatusCode initialize();
  /// implementation of IService::reinitalize
  virtual StatusCode reinitialize();
  /// implementation of IService::finalize
  virtual StatusCode finalize();
  /// implementation of IService::nextEvent
  virtual StatusCode nextEvent(int maxevt);
  /// implementation of IEventProcessor::executeEvent(void* par)
  virtual StatusCode executeEvent(void* par);
  /// implementation of IEventProcessor::executeRun()
  virtual StatusCode executeRun(int maxevt);

 private:
   double  m_runNo;
};
#endif // OFFLINE_EVENTLOOPMGR_H 
