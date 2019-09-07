#ifndef EvtHeaderCnv_H
#define EvtHeaderCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"

#include "RootEventData/TEvtHeader.h"
#include "RootCnvSvc/commonData.h"

//extern const CLID& CLID_Event;
extern const CLID& CLID_EventHeader;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class EvtHeaderCnv
 * @brief Concrete converter for the EvtHeader branch
 *
 * Based on the EvtHeaderCnv of GLAST.
 */ 

class EvtHeaderCnv : public RootEventBaseCnv { 

  friend class CnvFactory<EvtHeaderCnv>;

public: 
  static const CLID& classID()   
  {
    return CLID_EventHeader;
  }

  virtual ~EvtHeaderCnv() { };
  
  
  virtual StatusCode initialize();

  /// returns object to be written (maintained here for all DIGI-converters)
  static TEvtHeader* getWriteObject() { return m_objWrite;}
 
 protected:
  EvtHeaderCnv(ISvcLocator* svc);

  /// transformation from TDS object to ROOT
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// do the transformation from ROOT to TDS object
  virtual StatusCode TObjectToDataObject(DataObject*& refpObject);

private:
  /// Root object to be written
  static TEvtHeader* m_objWrite;

  /// Run number
  Int_t m_runId;  
  /// Event Number 
  Int_t m_eventId;

  UInt_t m_time;

  /// EventTag
  UInt_t m_eventTag;

  /// Flags
  UInt_t m_flag1;
  UInt_t m_flag2;

  // Int_t* m_data;
  Int_t m_data[10];  

  commonData  m_common;
};

#endif // EvtHeaderCnv_H
