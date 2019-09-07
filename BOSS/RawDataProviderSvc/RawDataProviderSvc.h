#ifndef RAWDATAPROVIDERSVC_H
#define RAWDATAPROVIDERSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"
#include "MdcRawEvent/MdcDigi.h"
#include "EventModel/Event.h"
#include <vector>
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "RawDataProviderSvc/TofData.h"

class RawDataProviderSvc: public Service, virtual public IRawDataProviderSvc,
			  virtual public IIncidentListener{

public:
  RawDataProviderSvc( const std::string& name, ISvcLocator* svcloc );
  ~RawDataProviderSvc();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  // Incident handler
  void handle(const Incident&); 

  MdcDigiVec& getMdcDigiVec(uint32_t control = 0) { return m_mdcProvider->getMdcDigiVec(control); }

  TofDataMap& tofDataMapOnlineMode(uint32_t control = 1) { return m_tofProvider->tofDataMapOnlineMode(control); }
  TofDataVector& tofDataVectorOnlineMode(uint32_t control = 1) { return m_tofProvider->tofDataVectorOnlineMode(control); }
  TofDataMap& tofDataMapEstime() { return m_tofProvider->tofDataMapEstime(); }
  TofDataVector& tofDataVectorEstime() { return m_tofProvider->tofDataVectorEstime(); }
  TofDataMap& tofDataMapTof(double estime = 0.0) { return m_tofProvider->tofDataMapTof(estime); }
  TofDataVector& tofDataVectorTof(double estime = 0.0) { return m_tofProvider->tofDataVectorTof(estime); }
  TofDataMap& tofDataMapEmc(double estime = 0.0) { return m_tofProvider->tofDataMapEmc(estime); }
  TofDataVector& tofDataVectorEmc(double estime = 0.0) { return m_tofProvider->tofDataVectorEmc(estime); }

  EmcDigiCol& getEmcDigiVec(uint32_t control) {return m_emcProvider->getEmcDigiVec(control); }

  MdcRawDataProvider* getMdcProvider() { return m_mdcProvider; }
  TofRawDataProvider* getTofProvider() { return m_tofProvider; }
  EmcRawDataProvider* getEmcProvider() { return m_emcProvider; }
  bool                isOnlineMode()   { return m_onlineMode;  }

private:

  MdcRawDataProvider*  m_mdcProvider;
  TofRawDataProvider*  m_tofProvider;
  EmcRawDataProvider*  m_emcProvider;

  std::vector<int> m_skipLayers;    // Mdc skip layer flag
  float            m_effThreshold;  // Mdc wire efficiency threshold
  float            m_elow;          // Emc low threshold of energy for single crystal (MeV)
  uint32_t         m_tlow;          // Emc low threshold of time
  uint32_t         m_thigh;         // Emc high threshold of time
  std::vector<uint32_t>  m_hotVec;  // Emc hot channels' id
  std::vector<uint32_t>  m_deadVec; // Emc dead channels' id 
  bool             m_onlineMode;    // if online mode, true
};

#endif
