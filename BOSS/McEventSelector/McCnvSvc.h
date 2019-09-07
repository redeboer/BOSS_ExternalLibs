//	============================================================
//	McCnvSvc.h
//	------------------------------------------------------------
//
//	Package   : McEventSelector 
//
//	Author    : Marjorie Shapiro
//      Modified Nov 10 I.H.
//	============================================================
#ifndef MCEVENTSELECTOR_MCCNVSVC_H
#define MCEVENTSELECTOR_MCCNVSVC_H 1

#include "GaudiKernel/ConversionSvc.h"  

class McEvtSelector;

// Forward declarations
template <class TYPE> class SvcFactory;

static const InterfaceID IID_IMcCnvSvc("IMcCnvSvc", 1, 0);


class McCnvSvc : public ConversionSvc	{
  /// Allow the factory class access to the constructor
  friend class SvcFactory<McCnvSvc>;
public:
  /**@name McCnvSvc overrides     */
  //@{
  /// Initialize the service.
  virtual StatusCode initialize();

  virtual StatusCode finalize();

  /// Override inherited queryInterface due to enhanced interface
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  /// Update state of the service
  virtual StatusCode updateServiceState(IOpaqueAddress* pAddress);
  /// storage type
  static unsigned char storageType();

protected:
  /**@name: Object implementation     */
  //@{
  /// Standard Constructor
  McCnvSvc(const std::string& name, ISvcLocator* svc);

  /// Standard Destructor
  virtual ~McCnvSvc();
private:
  /// access to the EventSelector
  McEvtSelector* m_evtsel;
};

#endif // MCEVENTSELECTOR_MCCNVSVC_H
