#ifndef HltRawCnv_H
#define HltRawCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_HltRawCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class HltRawCnv
 * @brief Concrete converter for the HltRaw collection
 *
 */ 

class HltRawCnv : public RootEventBaseCnv { 

  friend class CnvFactory<HltRawCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_HltRawCol; 
  }

  virtual ~HltRawCnv() { };

protected:
  HltRawCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_hltRawCol;
};



#endif // HltRawCnv_H









