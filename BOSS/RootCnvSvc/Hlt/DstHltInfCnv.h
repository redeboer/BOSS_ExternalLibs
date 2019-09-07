#ifndef DstHltInfCnv_H
#define DstHltInfCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootEventData/TDstHltInf.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_DstHltInf;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class DstHltInfCnv
 * @brief Concrete converter for the DstHltInf 
 *
 */ 

class DstHltInfCnv : public RootEventBaseCnv { 

  friend class CnvFactory<DstHltInfCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_DstHltInf; 
  }

  virtual ~DstHltInfCnv() { };

protected:
  DstHltInfCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:

  /// root object to be read
  TDstHltInf *m_dstHltInf;

  /// old version flag
  bool     m_old;
};

#endif // MdcMcCnv_H
