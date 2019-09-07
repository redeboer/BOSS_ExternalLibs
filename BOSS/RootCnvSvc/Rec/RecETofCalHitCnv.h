#ifndef RecETofCalHitCnv_H
#define RecETofCalHitCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecETofCalHitCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecETofCalHitCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecETofCalHitCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecETofCalHitCol; 
  }

  virtual ~RecETofCalHitCnv() { };

protected:
  RecETofCalHitCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_mucTrackCol;
};



#endif // RecETofCalHitCnv_H
