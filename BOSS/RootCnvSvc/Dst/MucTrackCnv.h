#ifndef MucTrackCnv_H
#define MucTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_DstMucTrackCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class MucTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<MucTrackCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_DstMucTrackCol; 
  }

  virtual ~MucTrackCnv() { };

protected:
  MucTrackCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_mucTrackCol;
};



#endif // MucTrackCnv_H
