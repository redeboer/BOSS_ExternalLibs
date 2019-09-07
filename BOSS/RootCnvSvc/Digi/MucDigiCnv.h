#ifndef MucDigiCnv_H
#define MucDigiCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_MucDigiCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class MucDigiCnv : public RootEventBaseCnv { 

  friend class CnvFactory<MucDigiCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_MucDigiCol; 
  }

  virtual ~MucDigiCnv() { };

protected:
  MucDigiCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_mucDigiCol;
};



#endif // MucDigiCnv_H









