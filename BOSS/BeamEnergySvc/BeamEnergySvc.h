#ifndef BEAM_ENERGY_SVC_H
#define BEAM_ENERGY_SVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "BeamEnergySvc/IBeamEnergySvc.h"
#include "DatabaseSvc/IDatabaseSvc.h"
#include "GaudiKernel/Service.h"
//#include <mysql/mysql.h>
#include <vector>

template <class TYPE> class CnvFactory;

class BeamEnergySvc : public Service, virtual public IBeamEnergySvc, virtual public IIncidentListener
{
  friend class CnvFactory<BeamEnergySvc>;

  public :
  BeamEnergySvc(const std::string& name, ISvcLocator* svcLoc);
  virtual ~BeamEnergySvc();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

  void handle(const Incident&);
  void getBeamEnergyInfo();

  bool isRunValid();
  double getbeamE();

  private :
  std::string m_host;
  IDataProviderSvc* m_eventSvc;
  IDatabaseSvc* m_dbsvc;
  int m_run;
  bool m_isRunValid;
  double m_beamE;
};

#endif
