#ifndef ExtTrackCnv_H
#define ExtTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_DstExtTrackCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class ExtTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<ExtTrackCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_DstExtTrackCol; 
  }

  virtual ~ExtTrackCnv() { };

protected:
  ExtTrackCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_extTrackCol;
};



#endif // ExtTrackCnv_H
