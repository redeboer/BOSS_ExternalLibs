#ifndef EmcTrackCnv_H
#define EmcTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_DstEmcShowerCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class EmcTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<EmcTrackCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_DstEmcShowerCol; 
  }

  virtual ~EmcTrackCnv() { };

protected:
  EmcTrackCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_emcTrackCol;
};



#endif // EmcTrackCnv_H
