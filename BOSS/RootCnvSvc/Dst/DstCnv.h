#ifndef DstCnv_H
#define DstCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"

#include "RootEventData/TDstEvent.h"
#include "RootCnvSvc/commonData.h"

extern const CLID& CLID_DstEvent;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class DstCnv
 * @brief Concrete converter for the Dst branch
 *
 * Based on the DstCnv of GLAST.
 */ 

class DstCnv : public RootEventBaseCnv { 

  friend class CnvFactory<DstCnv>;

public: 
  static const CLID& classID()   
  {
    return CLID_DstEvent;
  }

  virtual ~DstCnv() { };
  
  
  virtual StatusCode initialize();

  /// returns object to be written (maintained here for all DIGI-converters)
  static TDstEvent* getWriteObject() { return m_objWrite;}
 
 protected:
  DstCnv(ISvcLocator* svc);

  /// transformation from TDS object to ROOT
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// do the transformation from ROOT to TDS object
  virtual StatusCode TObjectToDataObject(DataObject*& refpObject);

private:
  /// Root object to be written
  static TDstEvent* m_objWrite;

  /// Run number
  Int_t m_runId;  
  /// Event Number 
  Int_t m_eventId;

  commonData  m_common;
};

#endif // DstCnv_H
