#ifndef IEF_ALGORITHM_H
#define IEF_ALGORITHM_H
#include "GaudiKernel/Algorithm.h"

#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "HltStore/HltStoreSvc.h"
#include "HltDataTypes/EFResult.h"

class IEFAlgorithm : public Algorithm {
  public:
    IEFAlgorithm(const std::string& name,ISvcLocator* pSvcLocator);
    virtual ~IEFAlgorithm() {};
    virtual StatusCode initialize();
    virtual void reset() = 0;
  protected:
    int m_output;
    float m_beam;
    PropertyMgr m_propMgr;
    bool m_run;
    HltStoreSvc* m_HltStoreSvc;
    IRawDataProviderSvc* m_rawDigiSvc;
    EFResult*          m_ef;
};
#endif
