#ifndef EmcDigiCnv_H
#define EmcDigiCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_EmcDigiCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class EmcDigiCnv
 * @brief Concrete converter for the EmcDigi collection
 *
 */ 

class EmcDigiCnv : public RootEventBaseCnv { 

  friend class CnvFactory<EmcDigiCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_EmcDigiCol; 
  }

  virtual ~EmcDigiCnv() { };

protected:
  EmcDigiCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_emcDigiCol;
};



#endif // EmcDigiCnv_H









