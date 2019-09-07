#ifndef LumiDigiCnv_H
#define LumiDigiCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_LumiDigiCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class LumiDigiCnv : public RootEventBaseCnv { 

  friend class CnvFactory<LumiDigiCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_LumiDigiCol; 
  }

  virtual ~LumiDigiCnv() { };

protected:
  LumiDigiCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_lumiDigiCol;
};

#endif // LumiDigiCnv_H
