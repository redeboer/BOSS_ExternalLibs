#ifndef RawDataCnvSvc_H
#define RawDataCnvSvc_H 

#include <map>
#include "GaudiKernel/ConversionSvc.h"
#include "RawDataCnvBase/IRawDataCnvSvc.h"

template <class TYPE> class SvcFactory;

class IOpaqueAddress;

/**class RawDataCnvSvc
 * Conversion Service which coordinates all of converters.
 *
 * When a component requests an item not yet available on the TDS, 
 * the RawDataCnvSvc is called to find the appropriate converter to gain
 * access to the data and put it on the TDS.
 *
 * Based on GLAST's EfbCnvSvc
 *
 */

class RawDataCnvSvc  : public ConversionSvc, virtual public IRawDataCnvSvc	{

  friend class SvcFactory<RawDataCnvSvc>;

  /// Map with leaf entries containing TDS paths
  typedef std::map<std::string, Leaf*> LeafMap;
  LeafMap m_leaves;

public:

  virtual StatusCode initialize();

  virtual StatusCode finalize();

  /// Associates a path on TDS with a particular converter
  virtual StatusCode declareObject(const IRawDataCnvSvc::Leaf& leaf);

  /// Override inherited queryInterface due to enhanced interface
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  // Address creation.
  virtual StatusCode createAddress( long  svc_type,
                                    const CLID& clid,
                                    const std::string* par, 
				    const unsigned long* ip,
                                    IOpaqueAddress*& refpAddress);
  virtual StatusCode createAddress(long storageType,
                                   const CLID& clid,
				   const std::string&,
				   IOpaqueAddress*& refpAddress);

protected:
  virtual StatusCode addConverters () = 0;

  StatusCode updateServiceState(IOpaqueAddress* pAddress);

  RawDataCnvSvc(const std::string& name, ISvcLocator* svc, long storageTyp);

  virtual ~RawDataCnvSvc() { };
};


#endif // RawDataCnvSvc_H

