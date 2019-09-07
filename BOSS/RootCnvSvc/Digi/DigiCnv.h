#ifndef DigiCnv_H
#define DigiCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"

#include "RootEventData/TDigiEvent.h"
#include "RootCnvSvc/commonData.h"  //zoujh

extern const CLID& CLID_DigiEvent;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class DigiCnv
 * @brief Concrete converter for the Rec branch
 *
 * Based on the DigiCnv of GLAST.
 */ 

class DigiCnv : public RootEventBaseCnv { 

  friend class CnvFactory<DigiCnv>;

public: 
  static const CLID& classID()   
  {
    return CLID_DigiEvent;
  }

  virtual ~DigiCnv() { };

  virtual StatusCode initialize();  

  /// returns object to be written (maintained here for all DIGI-converters)
  static TDigiEvent* getWriteObject() { return m_objWrite;}
 
 protected:
  DigiCnv(ISvcLocator* svc);

  /// transformation from TDS object to ROOT
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// do the transformation from ROOT to TDS object
  virtual StatusCode TObjectToDataObject(DataObject*& refpObject);

private:
  /// Root object to be written
  static TDigiEvent* m_objWrite;

  /// Run number
  Int_t m_runId;  
  /// Event Number 
  Int_t m_eventId;  
    
  /// Denote whether or not this data was simulated
  Bool_t m_fromMc;

  commonData m_common;  //zoujh

};

#endif // DigiCnv_H
