#ifndef RecMdcKalTrackCnv_H
#define RecMdcKalTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecMdcKalTrackCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecMdcKalTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecMdcKalTrackCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecMdcKalTrackCol; 
  }

  virtual ~RecMdcKalTrackCnv() { };

protected:
  RecMdcKalTrackCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recMdcKalTrackCol;
};



#endif // RecMdcKalTrackCnv_H
