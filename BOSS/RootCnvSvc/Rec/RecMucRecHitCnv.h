#ifndef RecMucRecHitCnv_H
#define RecMucRecHitCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_MucRecHitCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecMucRecHitCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecMucRecHitCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_MucRecHitCol; 
  }

  virtual ~RecMucRecHitCnv() { };

protected:
  RecMucRecHitCnv(ISvcLocator* svc);

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



#endif // RecMucRecHitCnv_H
