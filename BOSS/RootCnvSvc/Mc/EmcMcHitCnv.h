#ifndef EmcMcHitCnv_H
#define EmcMcHitCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_EmcMcHitCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class EmcMcCnv
 * @brief Concrete converter for the EmcMc collection
 *
 */ 

class EmcMcHitCnv : public RootEventBaseCnv { 

  friend class CnvFactory<EmcMcHitCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_EmcMcHitCol; 
  }

  virtual ~EmcMcHitCnv() { };

protected:
  EmcMcHitCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_emcMcHitCol;
};



#endif // EmcMcCnv_H









