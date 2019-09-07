#ifndef RecMdcDedxCnv_H
#define RecMdcDedxCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecMdcDedxCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecMdcDedxCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecMdcDedxCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecMdcDedxCol; 
  }

  virtual ~RecMdcDedxCnv() { };

protected:
  RecMdcDedxCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recMdcDedxCol;
};



#endif // RecMdcDedxCnv_H
