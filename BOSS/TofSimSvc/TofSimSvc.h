#ifndef TOFSIMSVC_H
#define TOFSIMSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "TofSimSvc/ITofSimSvc.h"
#include "TofSimSvc/TofConst.h"
#include "TofSimSvc/BTofSim.h"
#include "TofSimSvc/ETofSim.h"

class TofSimSvc : public Service, virtual public ITofSimSvc,virtual public IIncidentListener
{
public:
  // Constructors and destructors
  TofSimSvc( const std::string& name, ISvcLocator* svcloc );
  ~TofSimSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize ( );
  virtual StatusCode finalize ( );
  void handle(const Incident&);

  const double BarLowThres();
  const double BarHighThres();
  const double EndLowThres();
  const double EndHighThres();
  const double BarPMTGain();
  const double EndPMTGain();
  const double BarConstant();
  const double EndConstant();
  const double EndNoiseSwitch();

  const double BarGain1(unsigned int id);
  const double BarGain2(unsigned int id);
  const double EndGain(unsigned int id);
  const double BarAttenLength(unsigned int id);
  const double EndAttenLength(unsigned int id);
  const double EndNoiseSmear(unsigned int id);

  void  Dump();

 private:
  StatusCode FillfromDatabase();
  bool m_hasbeeninitialized;

private:
  IDataProviderSvc* m_pSimDataSvc;
  IntegerProperty  m_run;

  std::vector<TofConst*> fTofConst;
  std::vector<BTofSim*>  fBTofSim;
  std::vector<ETofSim*>  fETofSim;

};

#endif // TOFSIMSVC
