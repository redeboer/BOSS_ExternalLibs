#ifndef MdcDigiCnv_H
#define MdcDigiCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_MdcDigiCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class MdcDigiCnv
 * @brief Concrete converter for the MdcDigi collection
 *
 */ 

class MdcDigiCnv : public RootEventBaseCnv { 

  friend class CnvFactory<MdcDigiCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_MdcDigiCol; 
  }

  virtual ~MdcDigiCnv() { };

protected:
  MdcDigiCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_mdcDigiCol;
};



#endif // MdcDigiCnv_H









