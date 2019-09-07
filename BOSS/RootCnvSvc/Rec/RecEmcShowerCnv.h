#ifndef RecEmcShowerCnv_H
#define RecEmcShowerCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecEmcShowerCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecEmcShowerCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecEmcShowerCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecEmcShowerCol; 
  }

  virtual ~RecEmcShowerCnv() { };

protected:
  RecEmcShowerCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recEmcShowerCol;
};



#endif // RecEmcShowerCnv_H
