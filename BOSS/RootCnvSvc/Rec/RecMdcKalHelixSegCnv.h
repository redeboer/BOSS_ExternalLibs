#ifndef RecMdcKalHelixSegCnv_H
#define RecMdcKalHelixSegCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_RecMdcKalHelixSegCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


class RecMdcKalHelixSegCnv : public RootEventBaseCnv { 

  friend class CnvFactory<RecMdcKalHelixSegCnv>;

public: 
  static const CLID& classID()   
  {
       return CLID_RecMdcKalHelixSegCol; 
  }

  virtual ~RecMdcKalHelixSegCnv() { };

protected:
  RecMdcKalHelixSegCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_recMdcKalHelixSegCol;
};



#endif // RecMdcKalHelixSegCnv_H
