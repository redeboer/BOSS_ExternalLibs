#ifndef TrigCnv_H
#define TrigCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"

#include "RootEventData/TTrigEvent.h"
#include "RootCnvSvc/commonData.h"  //zoujh

extern const CLID& CLID_TrigEvent;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class TrigCnv
 * @brief Concrete converter for the Rec branch
 *
 * Based on the TrigCnv of GLAST.
 */ 

class TrigCnv : public RootEventBaseCnv { 

  friend class CnvFactory<TrigCnv>;

public: 
  static const CLID& classID()   
  {
    return CLID_TrigEvent;
  }

  virtual ~TrigCnv() { };

  virtual StatusCode initialize();  

  /// returns object to be written (maintained here for all DIGI-converters)
  static TTrigEvent* getWriteObject() { return m_objWrite;}
 
 protected:
  TrigCnv(ISvcLocator* svc);

  /// transformation from TDS object to ROOT
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// do the transformation from ROOT to TDS object
  virtual StatusCode TObjectToDataObject(DataObject*& refpObject);

private:
  /// Root object to be written
  static TTrigEvent* m_objWrite;

  /// Run number
  Int_t m_runId;  
  /// Event Number 
  Int_t m_eventId;  
    
  /// Denote whether or not this data was simulated
  Bool_t m_fromMc;

  commonData m_common;  //zoujh

};

#endif // TrigCnv_H
