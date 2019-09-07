#ifndef RecTofTrackCnv_H
#define RecTofTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecTofTrackCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecTofTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecTofTrackCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecTofTrackCol; 
  }

  virtual ~RecTofTrackCnv() { };

protected:
  RecTofTrackCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recTofTrackCol;
};



#endif // RecTofTrackCnv_H
