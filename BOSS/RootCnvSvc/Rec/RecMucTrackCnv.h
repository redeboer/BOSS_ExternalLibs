#ifndef RecMucTrackCnv_H
#define RecMucTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecMucTrackCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecMucTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecMucTrackCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecMucTrackCol; 
  }

  virtual ~RecMucTrackCnv() { };

protected:
  RecMucTrackCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recMucTrackCol;
};



#endif // RecMucTrackCnv_H
