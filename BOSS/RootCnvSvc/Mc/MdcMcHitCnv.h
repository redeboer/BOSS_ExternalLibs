#ifndef MdcMcHitCnv_H
#define MdcMcHitCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_MdcMcHitCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class MdcMcCnv
 * @brief Concrete converter for the MdcMc collection
 *
 */ 

class MdcMcHitCnv : public RootEventBaseCnv { 

  friend class CnvFactory<MdcMcHitCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_MdcMcHitCol; 
  }

  virtual ~MdcMcHitCnv() { };

protected:
  MdcMcHitCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_mdcMcHitCol;
};



#endif // MdcMcCnv_H









