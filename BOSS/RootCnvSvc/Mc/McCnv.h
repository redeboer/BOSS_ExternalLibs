#ifndef McCnv_H
#define McCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"

#include "RootEventData/TMcEvent.h"
#include "RootCnvSvc/commonData.h"   //zoujh

extern const CLID& CLID_McEvent;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class McCnv
 * @brief Concrete converter for the Mc branch
 *
 * Based on the McCnv of GLAST.
 */ 

class McCnv : public RootEventBaseCnv { 

  friend class CnvFactory<McCnv>;

public: 
  static const CLID& classID()   
  {
    return CLID_McEvent;
  }

  virtual ~McCnv() { };

  virtual StatusCode initialize();

  /// returns object to be written (maintained here for all DIGI-converters)
  static TMcEvent* getWriteObject() { return m_objWrite;}
 
 protected:
  McCnv(ISvcLocator* svc);

  /// transformation from TDS object to ROOT
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// do the transformation from ROOT to TDS object
  virtual StatusCode TObjectToDataObject(DataObject*& refpObject);

private:
   /// Root object to be written
  static TMcEvent* m_objWrite;

  /// Run number
  Int_t m_runId;  
  /// Event Number 
  Int_t m_eventId;

    
  commonData m_common;
};

#endif // McCnv_H
