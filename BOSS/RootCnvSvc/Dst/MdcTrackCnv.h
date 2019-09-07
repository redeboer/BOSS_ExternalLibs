#ifndef MdcTrackCnv_H
#define MdcTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_DstMdcTrackCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class MdcTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<MdcTrackCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_DstMdcTrackCol; 
  }

  virtual ~MdcTrackCnv() { };

protected:
  MdcTrackCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_mdcTrackCol;
};



#endif // MdcTrackCnv_H
