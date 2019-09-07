#ifndef _RootEventBaseCnv_H
#define _RootEventBaseCnv_H 1

#include <vector>
#include <string>
#include <map>
#include "TArrayS.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "RootInterface.h"
#include "RootCnvSvc.h"

#include <iostream>

class RootAddress;
class DataObject;
class TObject;
class RootEvtSelector;

extern const long int ROOT_StorageType;

/** @class RootEventBaseCnv
* @brief Base class for all  Root Converters.
*
* Based on SICb's SicbBaseCnv
* all specific converters inherit from this base class.
* The basic functions they have to implement are the pure conversions
* to and from DataObjects/Root Objects.
* In case there is more to do ( calling of upper converters, creation of branches for writing,
* reading of subbranches) they may also override the methods
* createObj/createRep.
* Based on the RootEventBaseCnv of Glast.
*/
class RootEventBaseCnv : public Converter      {
protected:
	
    RootCnvSvc*  m_cnvSvc;
    std::vector<RootCnvSvc::Leaf>   m_leaves;

    /// pointer to the RootInterface
    RootInterface* m_rootInterface;
    
    /// pointer to eventdataservice
    IDataProviderSvc* m_eds;

    /// the branchNr of this converter for writing
    int m_branchNr; 
    int m_branchNrDst;
    int m_branchNrMc;
    int m_branchNrRecon;
    int m_branchNrEvtRec;
    int m_branchNrEvtHeader;
    int m_branchNrEvtNavigator;
        
    /// the object that was read
    TObject *m_objRead;
    
    /// the CLID of the upper converter if any
    CLID CLID_top;
    
    /// array with number of branches for reading
    TArrayS *m_branchNumbers;

    /// root branchname (may be concatenated of severals)
    std::string m_rootBranchname;
    /// each converter knows it's treename
    std::string m_rootTreename;
   
    std::string m_currentFileName; 
    /// each converter knows the corresponding adresses
    std::vector<void *> m_adresses;

    RootEvtSelector* m_evtsel;
    
public:
    
    RootEventBaseCnv(const CLID& clid, ISvcLocator* svc);
    
    virtual ~RootEventBaseCnv() { };
    
    /// Storage type and class ID
    static const unsigned char storageType()      {
      return ROOT_StorageType;
    }

    virtual long repSvcType() const { 
      return ROOT_StorageType; 
    } 

    virtual StatusCode initialize();
    
    virtual StatusCode finalize();
    
    /// Store TDS path to link a particular converter to an object on the TDS
    void declareObject(const std::string& fullPath, const CLID& clid, 
		                                 const std::string& treename, 
						 const std::string& branchname );

    /// Convert the persistent object to transient
    virtual StatusCode createObj(IOpaqueAddress* addr,
		                     DataObject*& dat );   

    /// Convert the transient object to the requested representation.
    virtual StatusCode createRep(DataObject* pObject, 
		                IOpaqueAddress*& refpAddress);
    
    /// Resolve the references of the converted object. 
    virtual StatusCode fillRepRefs(IOpaqueAddress* pAddress,
		                       DataObject* pObject);
    
    /// Resolve the references of the converted object. 
    virtual StatusCode fillObjRefs(IOpaqueAddress* pAddress,
		                       DataObject* pObject);
    
    /// Do the concrete conversion from TDS to ROOT
    virtual StatusCode DataObjectToTObject(DataObject* dat, 
		                          RootAddress* addr) =0;
    
    /// Do the concrete conversion  from ROOT to TDS
    virtual StatusCode TObjectToDataObject(DataObject*& dat) =0;

    /// get the object to be read
    TObject* getReadObject() const  { return m_objRead;} 

};


#endif // _RootEventBaseCnv_H




