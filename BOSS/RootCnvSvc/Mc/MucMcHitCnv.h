#ifndef MucMcHitCnv_H
#define MucMcHitCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_MucMcHitCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class MucMcCnv
 * @brief Concrete converter for the MucMc collection
 *
 */ 

class MucMcHitCnv : public RootEventBaseCnv { 

  friend class CnvFactory<MucMcHitCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_MucMcHitCol; 
  }

  virtual ~MucMcHitCnv() { };

protected:
  MucMcHitCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_mucMcHitCol;
};



#endif // MucMcCnv_H









