#ifndef McParticleCnv_H
#define McParticleCnv_H 1

#include "RootCnvSvc/RootEventBaseCnv.h"
#include "RootCnvSvc/commonData.h"

class RootAddress;

extern const CLID& CLID_McParticleCol;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class McParticleCnv
 * @brief Concrete converter for the McParticle collection
 *
 */ 

class McParticleCnv : public RootEventBaseCnv { 

  friend class CnvFactory<McParticleCnv>;

public: 
  static const CLID& classID()   
  {
    return CLID_McParticleCol; 
  }

  virtual ~McParticleCnv() { };

protected:
  McParticleCnv(ISvcLocator* svc);

  /// transformation to root
  virtual StatusCode DataObjectToTObject(DataObject* obj, RootAddress* addr);

  /// transformation from root
  virtual StatusCode TObjectToDataObject(DataObject*& obj);

private:
  /// relational maps
  commonData m_common;

  /// root object to be read
  TObjArray *m_mcParticleCol;
};



#endif // MdcMcCnv_H









