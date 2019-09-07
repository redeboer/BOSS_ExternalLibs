#ifndef RecMdcHitCnv_H
#define RecMdcHitCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecMdcHitCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecMdcHitCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecMdcHitCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecMdcHitCol; 
  }

  virtual ~RecMdcHitCnv() { };

protected:
  RecMdcHitCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recMdcHitCol;
};



#endif // RecMdcHitCnv_H
