#ifndef CalibTreeCnvSvc_h
#define CalibTreeCnvSvc_h

#include <string>

#include "CalibDataSvc/ICalibTreeSvc.h"
#include "GaudiKernel/ConversionSvc.h"

//The function of the CalibTreeCnvSvc is to read the trees' buffers from 
//CalibMYSQLCnvSvc and use the defined converters to intepret the buffers
//than register the calibdata to TCDS.
//It's a different way from the CalibRootCnv

/// Forward and external declarations
template <class TYPE> class SvcFactory;

class IDetDataSvc;
class IOpaqueAddress;

namespace CalibData {
  class CalibBase;
  class CalibBase1;
}

///---------------------------------------------------------------------------
/** @class CalibTreeCnvSvc

    A conversion service for GLAST calibration bulk data in ROOT format.

    @author J. Bogart
    @date July 2004
*///--------------------------------------------------------------------------

class CalibTreeCnvSvc : public ConversionSvc, virtual public ICalibTreeSvc
{
  /// Only factories can access protected constructors
  friend class SvcFactory<CalibTreeCnvSvc>;

 protected:

  CalibTreeCnvSvc(const std::string& name, ISvcLocator* svc );
  virtual ~CalibTreeCnvSvc() {}

 public:
  
  // Reimplemented from IInterface

  virtual StatusCode queryInterface( const InterfaceID& riid, 
				     void** ppvInterface);  

  // Reimplemented from ICalibTreeSvc
  /*virtual StatusCode writeToRoot(const std::string& outputFile, 
                                 const std::string& tdsPath);
  virtual StatusCode writeToRoot(const std::string& outputFile, 
                                 CalibData::CalibBase1 *calib);
*/

  virtual StatusCode updateObj     ( IOpaqueAddress* pAddress, 
				     DataObject* pObject );

 public:

  // Overloaded from ConversionSvc

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /**
   * Create a ROOT address using explicit arguments to identify a single object
   * @param svc_type the service type
   * @param CLID the CLID of the ROOT Element for which an address is created
   * @param par an array of three strings containing the format version,
   *        calibration type name and the flavor, in this order
   * @param ip has a single element, the serial number of the MySQL row
   *        which corresponds to this element
   * @param refpAddress the new address created
   * @return a StatusCode giving the status of the address creation
   */
  //  virtual StatusCode createAddress(unsigned char svc_type,
      virtual StatusCode createAddress(long svc_type,
				       const CLID& clid,
				       const std::string* par, 
				       const unsigned long* ip,
				       IOpaqueAddress*& refpAddress);
      std::string getrootfile()
	{ return m_rootfile[0];}

  /*  
      There are a pile of functions implemented in the Gaudi
      base class ConversionSvc which we can just let be, such
      as createObj, fillObjRefs,...

      The base implementation looks up the appropriate converter
      and invokes it, usually just what we want to do.
  */
 private:

  // With current functionality, there really is no need to keep
  // this as a member.  It's only used during initialize()
  /// Handle to the IConversionSvc interface of the DetectorPersistencySvc
  IConversionSvc*      m_detPersSvc;

  /// Handle to IDataProviderSvc interface of CalibDataSvc
  IDataProviderSvc*         m_detDataSvc;
   std::string              m_rootfile[4];
};
#endif   
