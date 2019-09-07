//$Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/CalibROOTCnv/CalibRootCnvSvc.h,v 1.11 2010/03/10 06:28:21 huangb Exp $
#ifndef CalibRootCnvSvc_h
//#define CalibRootCnvSvc_h  1
#define CalibRootCnvSvc_h

#include <string>

#include "CalibDataSvc/ICalibRootSvc.h"
#include "GaudiKernel/ConversionSvc.h"


//  Hi Heather.  For the XML conversion service I defined an
//  extra interface to do generic XML things that converters
//  might need.  For now there really is just one thing there:
//  a function which reads in the XML file, so that the converters
//  deal with the DOM representation (in-memory, but otherwise 
//  isomorphic to the XML description) rather than with the physical
//  file.  Putting that kind of functionality in the conversion service
//  is probably a good idea, but it doesn't have to be defined in
//  an abstract interface.

/// Forward and external declarations
template <class TYPE> class SvcFactory;

class IDetDataSvc;
class IOpaqueAddress;

namespace CalibData {
  class CalibBase;
  class CalibBase1;
}

///---------------------------------------------------------------------------
/** @class CalibRootCnvSvc

    A conversion service for GLAST calibration bulk data in ROOT format.

    @author J. Bogart
    @date July 2004
*///--------------------------------------------------------------------------

class CalibRootCnvSvc : public ConversionSvc, virtual public ICalibRootSvc
{
  /// Only factories can access protected constructors
  friend class SvcFactory<CalibRootCnvSvc>;

 protected:

  CalibRootCnvSvc(const std::string& name, ISvcLocator* svc );
  virtual ~CalibRootCnvSvc() {}

 public:
  
  // Reimplemented from IInterface

  virtual StatusCode queryInterface( const InterfaceID& riid, 
				     void** ppvInterface);  

  // Reimplemented from ICalibRootSvc
  virtual StatusCode writeToRoot(const std::string& outputFile, 
                                 const std::string& tdsPath);
  virtual StatusCode writeToRoot(const std::string& outputFile, 
                                 CalibData::CalibBase1 *calib);
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
     StatusCode  decodeDescription(const std::string& oldpath,
                                     std::string& realpath );


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
   std::string              m_rootfile[10];
};
#endif   
