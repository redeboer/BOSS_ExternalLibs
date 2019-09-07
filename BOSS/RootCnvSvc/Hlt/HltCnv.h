#ifndef HltCnv_H
#define HltCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"

#include "RootEventData/THltEvent.h"
#include "RootCnvSvc/commonData.h"  //zoujh

extern const CLID& CLID_HltEvent;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class HltCnv
 * @brief Concrete converter for the Rec branch
 *
 * Based on the HltCnv of GLAST.
 */ 

class HltCnv : public RootEventBaseCnv { 

  friend class CnvFactory<HltCnv>;

public: 
  static const CLID& classID()   
  {
    return CLID_HltEvent;
  }

  virtual ~HltCnv() { };

  virtual StatusCode initialize();  

  /// returns object to be written (maintained here for all DIGI-converters)
  static THltEvent* getWriteObject() { return m_objWrite;};
 
 protected:
  HltCnv(ISvcLocator* svc);

  /// transformation from TDS object to ROOT
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// do the transformation from ROOT to TDS object
  virtual StatusCode TObjectToDataObject(DataObject*& refpObject);

private:
  /// Root object to be written
  static THltEvent* m_objWrite;

  /// Run number
  Int_t m_runId;  
  /// Event Number 
  Int_t m_eventId;  
    
  /// Denote whether or not this data was simulated
  Bool_t m_fromMc;

  commonData m_common;  //zoujh

};

#endif // HltCnv_H
