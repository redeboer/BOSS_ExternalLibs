#ifndef EFtoTDS_H
#define EFtoTDS_H

#include "GaudiKernel/Algorithm.h"
//#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"
#include "HltStore/HltStoreSvc.h"

class EFtoTDS : public Algorithm{
 public:
  EFtoTDS(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EFtoTDS();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  HltStoreSvc*   m_HltStoreSvc;
  int            m_outputLevel;
};
#endif //EFtoTDS_H
