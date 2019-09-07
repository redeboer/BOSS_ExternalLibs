#ifndef TofTrackCnv_H
#define TofTrackCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_DstTofTrackCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class TofTrackCnv : public RootEventBaseCnv { 

  friend class CnvFactory<TofTrackCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_DstTofTrackCol; 
  }

  virtual ~TofTrackCnv() { };

protected:
  TofTrackCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_tofTrackCol;
};



#endif // TofTrackCnv_H
