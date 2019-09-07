#ifndef RecBTofCalHitCnv_H
#define RecBTofCalHitCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecBTofCalHitCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecBTofCalHitCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecBTofCalHitCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecBTofCalHitCol; 
  }

  virtual ~RecBTofCalHitCnv() { };

protected:
  RecBTofCalHitCnv(ISvcLocator* svc);

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



#endif // RecBTofCalHitCnv_H
