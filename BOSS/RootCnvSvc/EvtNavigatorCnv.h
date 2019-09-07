#ifndef EvtNavigatorCnv_H
#define EvtNavigatorCnv_H 1

#include <iostream>
#include <map>
#include "RootCnvSvc/RootEventBaseCnv.h"

#include "RootEventData/TEvtNavigator.h"
#include "RootCnvSvc/commonData.h"

//extern const CLID& CLID_Event;
extern const CLID CLID_EventNavigator;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class EvtNavigatorCnv
 * @brief Concrete converter for the EvtNavigator branch
 */ 

class EvtNavigatorCnv : public RootEventBaseCnv { 

  friend class CnvFactory<EvtNavigatorCnv>;

public: 
  static const CLID& classID()   
  {
    return CLID_EventNavigator;
  }

  virtual ~EvtNavigatorCnv() { };
  
  
  virtual StatusCode initialize();

  /// returns object to be written (maintained here for all DIGI-converters)
  static TEvtNavigator* getWriteObject() { return m_evtNavigatorW;}
 
 protected:
  EvtNavigatorCnv(ISvcLocator* svc);

  /// transformation from TDS object to ROOT
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// do the transformation from ROOT to TDS object
  virtual StatusCode TObjectToDataObject(DataObject*& refpObject);

private:
  /// Root objects` to be read/written
  static TEvtNavigator* m_evtNavigatorR;
  static TEvtNavigator* m_evtNavigatorW;

  std::multimap<int, int> m_mcMdcMcHits;
  std::multimap<int, int> m_mcMdcTracks;
  std::multimap<int, int> m_mcEmcMcHits;
  std::multimap<int, int> m_mcEmcRecShowers;

  commonData  m_common;
};

#endif // EvtNavigatorCnv_H
