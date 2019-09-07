#ifndef RecEvTimeCnv_H
#define RecEvTimeCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecEsTimeCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecEvTimeCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecEvTimeCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecEsTimeCol; 
  }

  virtual ~RecEvTimeCnv() { };

protected:
  RecEvTimeCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recEvTimeCol;
};



#endif // RecEvTimeCnv_H
