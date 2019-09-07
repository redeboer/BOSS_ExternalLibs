#ifndef RecEmcHitCnv_H
#define RecEmcHitCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecEmcHitCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecEmcHitCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecEmcHitCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecEmcHitCol; 
  }

  virtual ~RecEmcHitCnv() { };

protected:
  RecEmcHitCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recEmcHitCol;
};



#endif // RecEmcHitCnv_H
