#ifndef ROOTEVTSELECTOR_H
#define ROOTEVTSELECTOR_H 1

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h"


class IAddressCreator;
class IDataProviderSvc;
class RootInterface;
class RootEventContext;
class TagFilterSvc;

static const InterfaceID IID_IRootEvtSelector("IRootEvtSelector", 1, 0);

/** @class RootEvtSelector
* @brief RootEvtSelector performs the function of controlling the 
* ApplicationMgr loop. 
*
* Adapted to RootCnvSvc from GlastSvc/EventSelector. 
* The input property can now be set either to NONE or to ROOT.
* If it is NONE, the loop 
* will run until it hits EvtMax. A new class variable was added :m_evtMax 
* which is a local copy of the AppMgrs property. 
*
* $Header: /bes/bes/BossCvs/Event/RootCnvSvc/RootCnvSvc/RootEvtSelector.h,v 1.14 2015/06/14 23:47:16 dengzy Exp $
*/
class RootEvtSelector : virtual public Service, 
                        virtual public IEvtSelector,
                        virtual public IProperty {
public:
  // constructor and destructor 
  RootEvtSelector(  const std::string& name, ISvcLocator* svcloc );
  ~RootEvtSelector();

  // IEvtSelector implementation
  virtual StatusCode initialize();

  // Implementation of the IEvtSelector interface methods. 
  virtual StatusCode createContext(Context*& it) const; 
  StatusCode next() const;
  virtual StatusCode next(Context& it) const; 
  virtual StatusCode next(Context& it, int jump) const; 
  virtual StatusCode previous(Context& it) const; 
  virtual StatusCode previous(Context& it, int jump) const; 
    
  virtual StatusCode last(Context& it) const; 
  virtual StatusCode rewind(Context& it) const; 
    
  virtual StatusCode createAddress(const Context& it,IOpaqueAddress*& iop) const; 
  virtual StatusCode releaseContext(Context*& it) const; 
  virtual StatusCode resetCriteria(const std::string& criteria, Context& context) const; 
        
  virtual StatusCode setCriteria( const std::string& criteria ); 
     
  // IInterface implementation
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface); 
    
  // int IEvtSelector::getrecId();
  int getRecId() const {  return (*m_recId); }

  void getEntryFromTag() const ;

    
private:
  enum CriteriaType { 
    ROOT,   //When the input is a ROOT file
    NONE    //When there is no input
  };
       
  // Used internally to get the MaxEvent from the ApplicationMgr
  StatusCode getMaxEvent();

  RootEventContext*    p_beginContext; 
  RootEventContext*    p_endContext; 

  std::string           m_criteria;
  StringProperty        m_jobInput;
  CriteriaType          m_criteriaType;
 
  // pointer to RootInterface
  RootInterface*              m_rootInterface;

  // pointer to AddressCreator
  IAddressCreator*      m_addrCreator;

  // CLID of root basic object
  CLID                  m_rootCLID;

  /// max nr of events to be treated
  int                   m_evtMax;

  /// counts number of event
  int*                   m_recId;
  
  TagFilterSvc* m_tagFilterSvc;
};

#endif  // RootEvtSelector_H
