#ifndef TofMcHitCnv_H
#define TofMcHitCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_TofMcHitCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class TofMcCnv
 * @brief Concrete converter for the TofMc collection
 *
 */ 

class TofMcHitCnv : public RootEventBaseCnv { 

  friend class CnvFactory<TofMcHitCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_TofMcHitCol; 
  }

  virtual ~TofMcHitCnv() { };

protected:
  TofMcHitCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_tofMcHitCol;
};



#endif // TofMcCnv_H









