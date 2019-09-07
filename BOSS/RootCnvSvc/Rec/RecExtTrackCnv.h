#ifndef RecExtTrackCnv_H
#define RecExtTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecExtTrackCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecExtTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecExtTrackCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecExtTrackCol; 
  }

  virtual ~RecExtTrackCnv() { };

protected:
  RecExtTrackCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

 
 
 /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_extTrackCol;
};



#endif // RecExtTrackCnv_H
