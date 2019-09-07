#ifndef TofDigiCnv_H
#define TofDigiCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_TofDigiCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class TofDigiCnv : public RootEventBaseCnv { 

  friend class CnvFactory<TofDigiCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_TofDigiCol; 
  }

  virtual ~TofDigiCnv() { };

protected:
  TofDigiCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_tofDigiCol;
};



#endif // TofDigiCnv_H









