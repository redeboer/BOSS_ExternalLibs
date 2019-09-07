// TofQElecSvc
// Sun Shengsen 2009-9-17
//
#ifndef TOFQELECSVC_H
#define TOFQELECSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "TofQElecSvc/ITofQElecSvc.h"
#include "CalibData/Tof/TofElecData.h"
#include "CalibData/Tof/BarrelElec.h"
#include "CalibData/Tof/EndcapElec.h"

class TofQElecSvc : public Service, virtual public ITofQElecSvc,virtual public IIncidentListener
{
public:
  // Constructors and destructors
  TofQElecSvc( const std::string& name, ISvcLocator* svcloc );
  ~TofQElecSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize ( );
  virtual StatusCode finalize ( );

  void handle(const Incident&);

  const unsigned int Board( bool barrel, int id, bool eastEnd );
  const unsigned int Crate( bool barrel, int id, bool eastEnd );
  const unsigned int Fee( bool barrel, int id, bool eastEnd );
  const unsigned int Channel( bool barrel, int id, bool eastEnd );
  const double BQTC1(int id, double q);
  const double BQTC2(int id, double q);
  const double EQTC(int id, double q);
  const double BQChannel1(int id, double qtc);
  const double BQChannel2(int id, double qtc);
  const double EQChannel(int id, double qtc);

private:
  StatusCode FillfromDatabase();
  bool m_hasbeeninitialized;

private:
  IDataProviderSvc* m_pCalibDataSvc;
  IntegerProperty  m_run;

  CalibData::TofElecData* m_data;

  //  std::vector<CalibData::BarrelElec*>  fBarrelElec;
  //  std::vector<CalibData::EndcapElec*>  fEndcapElec;

};

#endif // TOFQELECSVC_H
