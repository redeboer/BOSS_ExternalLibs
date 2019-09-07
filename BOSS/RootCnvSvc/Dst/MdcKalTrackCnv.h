#ifndef MdcKalTrackCnv_H
#define MdcKalTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_DstMdcKalTrackCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class MdcKalTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<MdcKalTrackCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_DstMdcKalTrackCol; 
  }

  virtual ~MdcKalTrackCnv() { };

protected:
  MdcKalTrackCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_mdcKalTrackCol;
};



#endif // MdcKalTrackCnv_H
