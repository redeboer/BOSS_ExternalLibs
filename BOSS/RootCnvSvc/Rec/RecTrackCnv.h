#ifndef RecTrackCnv_H
#define RecTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"

#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/commonData.h"

extern const CLID& CLID_ReconEvent;   //CLID_ReconEvent 3000   liangyt -->6000//?

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class DstCnv
 * @brief Concrete converter for the Dst branch
 *
 * Based on the DstCnv of GLAST.
 */ 

class RecTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecTrackCnv>;

public: 
  static const CLID& classID()   
  {
    return CLID_ReconEvent;
  }

  virtual ~RecTrackCnv() { };

  virtual StatusCode initialize();

  /// returns object to be written (maintained here for all DIGI-converters)
  static TRecTrackEvent* getWriteObject() { return m_objWrite;}
 
 protected:
  RecTrackCnv(ISvcLocator* svc);

  /// transformation from TDS object to ROOT
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// do the transformation from ROOT to TDS object
  virtual StatusCode TObjectToDataObject(DataObject*& refpObject);

private:
  /// Root object to be written
  static TRecTrackEvent* m_objWrite;
  //TDstEvent* m_objWrite;   //use TDstEvent temp

  /// Run number
  Int_t m_runId;  
  /// Event Number 
  Int_t m_eventId;

    
  commonData  m_common;
};

#endif // RecTrackCnv_H
