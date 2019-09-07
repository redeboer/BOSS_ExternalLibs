#ifndef TrigDataCnv_H
#define TrigDataCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootEventData/TTrigData.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_TrigData;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class TrigDataCnv
 * @brief Concrete converter for the TrigData 
 *
 */ 

class TrigDataCnv : public RootEventBaseCnv { 

  friend class CnvFactory<TrigDataCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_TrigData; 
  }

  virtual ~TrigDataCnv() { };

protected:
  TrigDataCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:

  /// root object to be read
  TTrigData *m_trigData;
};

#endif // MdcMcCnv_H
