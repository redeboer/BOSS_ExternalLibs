#ifndef RecMdcDedxHitCnv_H
#define RecMdcDedxHitCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecMdcDedxHitCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecMdcDedxHitCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecMdcDedxHitCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecMdcDedxHitCol; 
  }

  virtual ~RecMdcDedxHitCnv() { };

protected:
  RecMdcDedxHitCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recMdcDedxHitCol;
};



#endif // RecMdcDedxCnv_H
