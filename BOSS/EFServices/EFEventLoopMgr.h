//====================================================================
//	EFEventLoopMgr.h
//====================================================================
#ifndef STOREGATE_EFEVENTLOOPMGR_H
#define STOREGATE_EFEVENTLOOPMGR_H

#define ONLINE_DATA_HEAD 36
#define ONLINE_DATA_TAIL 4
// Framework include files
#ifndef GAUDIKERNEL_IEVTSELECTOR_H
#include "GaudiKernel/IEvtSelector.h"
#endif
#include "GaudiKernel/SvcFactory.h"
#ifndef GAUDISVC_MINIMALEVENTLOOPMGR_H
//#include "ApplicationMgr/MinimalEventLoopMgr.h"
#include "GaudiKernel/MinimalEventLoopMgr.h"
#endif

#include "HltDataTypes/EventID.h"  /* number_type */
#include "HltDataTypes/EFResult.h"
#include "HltStore/HltStoreSvc.h"
#include "EFServices/IEFEventLoopMgr.h"

#include "RawDataCnv/IRawDataInputSvc.h"
#include "RawDataCnv/RawDataCnv.h"
#include <vector>
#include <stdint.h>

// Forward declarations
class IIncidentSvc;
class IDataManagerSvc;
class IConversionSvc;
class IDataProviderSvc;
class RAWEVENT;

/** Class definition of EFEventLoopMgr.
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
class EFEventLoopMgr : public MinimalEventLoopMgr,
	         virtual public IEFEventLoopMgr {
public:
  /// Creator friend class
  friend class SvcFactory<EFEventLoopMgr>;

protected:
  /// Reference to the indicent service
  IIncidentSvc*     m_incidentSvc;
  /// Reference to HltStoreSvc;
  HltStoreSvc*  m_HltStoreSvc;
  /// Reference to the Event Selector
  IEvtSelector*     m_evtSelector;
  /// Event Context
  IEvtSelector::Context*      m_evtContext;
  /// Event selector
  std::string       m_evtsel;

  /// Reference to the Event Data Service's IDataManagerSvc interface
  IDataManagerSvc*  m_evtDataMgrSvc;
  /// Reference to the Event Data Service's IDataProviderSvc interface
  IDataProviderSvc* m_evtDataSvc;

  /// Reference to the Histogram Data Service
  IDataManagerSvc*  m_histoDataMgrSvc;
  /// Reference to the Histogram Persistency Service
  IConversionSvc*   m_histoPersSvc;
  /// Name of the Hist Pers type
  std::string       m_histPersName;

  /// current run number
  EventID::number_type m_currentRun;
  bool m_firstRun;

protected:
  /// Standard Constructor
  EFEventLoopMgr(const std::string& nam, ISvcLocator* svcLoc);
  /// Standard Destructor
  virtual ~EFEventLoopMgr();

public:
  /// implementation of IAppMgrUI::initalize
  virtual StatusCode initialize();
  /// implementation of IAppMgrUI::finalize
  virtual StatusCode finalize();
 
  /// implementation of IEventProcessor::executeEvent(void* par)
  virtual StatusCode executeEvent(void* par);
  /// Create event address using event selector
  StatusCode getEventRoot(IOpaqueAddress*& refpAddr);
  /// implementation of IInterface: queryInterface
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  /// prepare for run step
  virtual StatusCode prepareForRun(int ef_run_number);

  /// whether it is a RrandomTrg
  virtual bool getTrigChn(uint32_t* data, uint32_t ndata);
  /// return data from EF decision
  virtual std::string  EF_Decision();
  /// return version number
  virtual uint32_t EF_Version();
  /// return algorithm tag
  virtual uint32_t EF_Algorithm_Tag();
  /// return total energy
  virtual uint32_t EF_Total_Energy();
  ///Return a pointer to the local memory containing the EF subdetector fragment
  virtual uint32_t* EF_Fragment_Address();

private:
  int                       m_total_nevt ;
  IRawDataInputSvc*         m_inputSvc;
  bool                      m_disableEFResult ;
  bool                      m_okexecuteEvent ;
  int                       m_timeTest;
  EFResult*                 m_efResult;
  EFResult*                 m_evtType;
//hxt   IByteStreamEventAccess* m_ByteStreamEventAccess;
  char* m_serializedEFAddress; ///< Pointer to the local memory containing the EF Fragment
  uint32_t                  m_trgType;
  bool                      m_firstEvent; 
  RAWEVENT*                 m_re;
  //hxt typedef OFFLINE_FRAGMENTS_NAMESPACE::SubDetectorFragment DETF;
  //hxt typedef std::vector<DETF*, Hlt_ALLOC> VDETF; 
};
#endif // STOREGATE_EFEVENTLOOPMGR_H
