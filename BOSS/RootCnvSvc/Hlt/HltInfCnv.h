#ifndef HltInfCnv_H
#define HltInfCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootEventData/THltInf.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_HltInf;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class HltInfCnv
 * @brief Concrete converter for the HltInf 
 *
 */ 

class HltInfCnv : public RootEventBaseCnv { 

  friend class CnvFactory<HltInfCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_HltInf; 
  }

  virtual ~HltInfCnv() { };

protected:
  HltInfCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:

  /// root object to be read
  THltInf *m_hltInf;

  /// old version flag
  bool    m_old;
};

#endif // MdcMcCnv_H
