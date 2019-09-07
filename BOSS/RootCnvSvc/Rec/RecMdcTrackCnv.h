#ifndef RecMdcTrackCnv_H
#define RecMdcTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecMdcTrackCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecMdcTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecMdcTrackCnv>;

public: 
  static const CLID& classID()   
  {
    return CLID_RecMdcTrackCol; 
  }

  virtual ~RecMdcTrackCnv() { };

protected:
  RecMdcTrackCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recMdcTrackCol;
};



#endif // RecMdcTrackCnv_H
