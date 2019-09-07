#ifndef EventTagSvc_H
#define EventTagSvc_H

#include <vector>
#include <string>
#include <map>
#include <set>

#ifndef BEAN
#include "EventTag/IEventTagSvc.h"

#include "GaudiKernel/Service.h"
//#include "EventModel/EventList.h"
#else
#include "RootEventData/TMcEvent.h"
#include "RootEventData/TMcParticle.h"

typedef bool StatusCode;
#endif

typedef std::vector <int>  keyVector;
typedef std::map<keyVector,int> decayModeTab;

/////////////////////////////////////////////////////////////////////////////
#ifndef BEAN
class EventTagSvc: virtual public IEventTagSvc,
		virtual public Service{
public:
  EventTagSvc(const std::string& name, ISvcLocator* svc);

#else // BEAN
class EventTagSvc {
public:
  static EventTagSvc* instance() {
    return (m_evtag) ? m_evtag : (m_evtag=new(std::nothrow)EventTagSvc());
  }

private:
  EventTagSvc();
  virtual ~EventTagSvc(){};

public:
  void setMcEvent(TMcEvent* mcEvent)    { m_TMcEvent = mcEvent; }
#endif

  StatusCode initialize();

  std::string pdg2name(long int pdg);
  long int name2pdg(std::string pname);

#ifndef BEAN
  //  StatusCode execute();
  StatusCode finalize();

  int getDecayCode(Event::McParticle* part);
  unsigned long long int getChainCode(Event::McParticle* part);
  unsigned int getCharmDecayType(Event::McParticle* part); 

  virtual StatusCode queryInterface( const InterfaceID&, void** ppvInterface );
#else
  int getDecayCode(TMcParticle* part);
  unsigned long long int getChainCode(TMcParticle* part);
  unsigned int getCharmDecayType(TMcParticle* part); 

  unsigned int  getEventTag(unsigned int initialEventTag = 0);
  

  //declare property functions
  
  void setVerbose(int n_verbose)        { verbose = n_verbose;}
  void setPdtFile(std::string n_pdtFile){ m_pdtFile = n_pdtFile;}
  void setDecayTabsFile(std::string n_decayTabsFile) 
                                        { m_decayTabsFile = n_decayTabsFile;}
  void setUserDecayTabsFile(std::string n_userDecayTabsFile) 
                                { m_userDecayTabsFile = n_userDecayTabsFile;}
  void setBuildAntiTabs(bool n_buildAntiTabs) 
                                        { m_buildAntiTabs = n_buildAntiTabs;}
  void setIgnorePhotons(bool n_ignorePhotons) 
                                        { m_ignorePhotons = n_ignorePhotons;}
  void setIgnoreFSR(bool n_ignoreFSR)   { m_ignoreFSR = n_ignoreFSR;}
  void setDigK0(bool n_digK0)           { m_digK0 = n_digK0;}
  void setUserChainTrig(std::vector<string> n_userChainTrig) 
                                        { userChainTrig = n_userChainTrig;}
  void addUserChainTrig(std::string n_userChainTrig) 
                                { userChainTrig.push_back(n_userChainTrig);}
#endif

private:
  std::string m_decayTabsFile;
  std::string m_pdtFile;
  std::string m_userDecayTabsFile;
  bool m_buildAntiTabs;
  bool m_ignorePhotons;
  bool m_ignoreFSR;
  bool m_digK0;
  std::vector<std::string> userChainTrig;

  std::map<int,decayModeTab*>m_decayTabList;
  std::map<std::string,long int> name2pdgTab;
  std::map<long int,std::string> pdg2nameTab;
  std::set<long int> chainTrigParticles;
  //  int readPdtFile(string fname,MsgStream* log);
  int readPdtFile(std::string fname);
  int readDecayTabFile(std::string fname);
  int buildAntiPartTabs();

#ifdef BEAN
  static EventTagSvc* m_evtag;
  TMcEvent* m_TMcEvent;
  int verbose;
#endif
};

#endif
