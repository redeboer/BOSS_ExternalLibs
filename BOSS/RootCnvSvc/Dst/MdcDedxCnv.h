#ifndef MdcDedxCnv_H
#define MdcDedxCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_DstMdcDedxCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class MdcDedxCnv : public RootEventBaseCnv { 

  friend class CnvFactory<MdcDedxCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_DstMdcDedxCol; 
  }

  virtual ~MdcDedxCnv() { };

protected:
  MdcDedxCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_mdcDedxCol;
};



#endif // MdcDedxCnv_H
