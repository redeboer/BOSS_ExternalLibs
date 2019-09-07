#ifndef TAGFILTERSVC_H_
#define TAGFILTERSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IService.h"
#include "TagFilterSvc/ITagFilterSvc.h"

#include <string>
#include <vector>
using namespace std;

class TagFilterSvc: public Service, virtual public ITagFilterSvc{
  
  public:
    TagFilterSvc( const std::string& name, ISvcLocator* svcloc );
    ~TagFilterSvc();

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    string getCriteria() {return m_criteria;}
    void setCriteria(string crt) {m_criteria = crt;}
    
    StatusCode getVEntry(string, vector<int>&);
    vector<string> getDstFiles() ;  
    vector<string> getTagFiles() {return m_tagFiles;}
  private:
    string m_criteria;
    vector<string> m_tagFiles;
    vector<string> m_dstFiles;
};
#endif /* TAGFILTERSVC_H_ */
