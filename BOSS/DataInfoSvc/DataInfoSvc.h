#ifndef DATAINFOSVC_H_
#define DATAINFOSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IService.h"
#include "DataInfoSvc/IDataInfoSvc.h"

#include <string>
#include <vector>
using namespace std;

class DataInfoSvc: public Service, virtual public IDataInfoSvc{
  
  public:
    DataInfoSvc( const std::string& name, ISvcLocator* svcloc );
    ~DataInfoSvc();

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    string getDecayOptions() { return m_decayOptions; }
    std::vector<int> getTotEvtNo() { return m_totEvtNo; }
  
    void setDecayCard(string card); 
    void setTotEvtNo(std::vector<int> i) { m_totEvtNo = i; }

  private:
    string m_decayOptions;
    std::vector<int> m_totEvtNo;//run no, event no
};
#endif /* DATAINFOSVC_H_ */
