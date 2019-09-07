#ifndef RecEmcClusterCnv_H
#define RecEmcClusterCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecEmcClusterCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecEmcClusterCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecEmcClusterCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecEmcClusterCol; 
  }

  virtual ~RecEmcClusterCnv() { };

protected:
  RecEmcClusterCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recEmcClusterCol;
};



#endif // RecEmcClusterCnv_H
