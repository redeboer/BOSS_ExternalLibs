#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/GenericAddress.h"

#include "RawDataCnvBase/RawDataBaseCnv.h"
#include "RawDataCnvBase/RawDataCnvSvc.h"
#include "EventModel/EventModel.h"
#include <iostream>

static const InterfaceID IID_IRawDataCnvSvc("IRawDataCnvSvc", 1 , 0); 

// RCS Id for identification of object version
// static const char* rcsid = "$Id: RawDataCnvSvc.cxx,v 1.12 2011/02/17 01:14:23 maqm Exp $";

RawDataCnvSvc::RawDataCnvSvc(const std::string& name, ISvcLocator* svc, long storageType)
: ConversionSvc(name, svc, storageType)              
{
}

StatusCode RawDataCnvSvc::initialize()     {
    // Purpose and Method:  Setup Converter Service.
    //   Associate RawDataCnvSvc with the EventDataSvc
    //   Associate the list of known converters with this RawDataCnvSvc.

    MsgStream log(messageService(), name());
 
    StatusCode iret;
    StatusCode status = ConversionSvc::initialize();
    if ( status.isSuccess() )   {

        IDataProviderSvc *pIDP = 0;
        // Set event data service
        status = service("EventDataSvc", pIDP, true);
        if ( status.isSuccess() )   {
            status = setDataProvider ( pIDP );
        }
        else    {
            return status;
        }

        // Add converters to the service 
        status = addConverters();
        if ( !status.isSuccess() )   {
          log << MSG::ERROR << "Unable to add converters to the service" << endreq;
	  return status;
	}

        // Now we have to configure the map of leaves
        // Which should contain the association of converters with 
        // paths on the TDS
        for (LeafMap::iterator k = m_leaves.begin(); k != m_leaves.end(); k++ )   {
            std::string path = (*k).first;
            for (LeafMap::iterator j = m_leaves.begin(); j != m_leaves.end(); j++ )   {
                std::string pp = (*j).first.substr(0, (*j).first.rfind("/"));
                if ( path == pp && path != (*j).first )   {
                    (*k).second->push_back((*j).second);
                }
            }
        }
        
    }

    return status;
}

StatusCode RawDataCnvSvc::finalize()     {
    StatusCode status = ConversionSvc::finalize();
    for (LeafMap::iterator k = m_leaves.begin(); k != m_leaves.end(); k++ )   {
        delete (*k).second;
    }
    m_leaves.erase(m_leaves.begin(), m_leaves.end());
    return status;
}

StatusCode RawDataCnvSvc::declareObject(const IRawDataCnvSvc::Leaf& leaf)   {
    // Purpose and Method:  Callback from each of the individual converters that allows
    //  association of TDS path and converter.
    Leaf* ll = new Leaf(leaf);
    std::pair<LeafMap::iterator, bool> p = m_leaves.insert(LeafMap::value_type( leaf.path, ll) );
    if( p.second )    {
        return StatusCode::SUCCESS;
    }
    delete ll;
    return StatusCode::FAILURE;
}

/// Update state of the service
StatusCode RawDataCnvSvc::updateServiceState(IOpaqueAddress* pAddress)    {
    
    // not sure about the use of recid or bank...
    MsgStream log(msgSvc(), name());
    
    StatusCode status = INVALID_ADDRESS;
    IRegistry* ent = pAddress->registry();

    if ( 0 != ent )   {
      SmartIF<IDataManagerSvc> iaddrReg( dataProvider());
 //       if ( 0 != iaddrReg )   {
      status = StatusCode::SUCCESS;
      std::string path = ent->identifier();

      LeafMap::iterator itm = m_leaves.find(path);
      if ( itm != m_leaves.end() ) {
	IRawDataCnvSvc::Leaf* leaf = (*itm).second;
	if ( 0 != leaf )    {
	  //ipar[0] = leaf->userParameter;
	  for ( Leaf::iterator il = leaf->begin(); il != leaf->end(); il++ )   {
	    IOpaqueAddress* newAddr = 0;
	    unsigned long ipars[2] = {0, 0}; //{(*il)->userParameter, new_rid};
	    const std::string spars[2] = {"", ""}; //{par[0], (*il)->bank};
            /* fix for Gaidi V16r4  -- Weidong Li 2005.10.25 
	    StatusCode ir =
	      addressCreator()->createAddress(storageType, 
					      (*il)->clid, 
					      spars, 
					      ipars,
					      newAddr);
	    */
            StatusCode ir = createAddress(repSvcType(), 
					      (*il)->clid, 
					      spars, 
					      ipars,
					      newAddr);
	    if ( ir.isSuccess() )   {
	      ir = iaddrReg->registerAddress((*il)->path, newAddr);            
	      if ( !ir.isSuccess() )    {
		newAddr->release();
		status = ir;
	      }
	    }
	  }
	}
      }
    }
    else  {
       status = NO_INTERFACE;
    }
    
    return status;
}

StatusCode RawDataCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)  {
    if ( IID_IRawDataCnvSvc == riid )  {
        *ppvInterface = (IRawDataCnvSvc*)this;
    }
    else  {
        // Interface is not directly availible: try out a base class
        return ConversionSvc::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
}

StatusCode RawDataCnvSvc::createAddress(long storageType,
					const CLID& clid,
					const std::string*, 
					const unsigned long*,
					IOpaqueAddress*& refpAddress) 
{
    MsgStream log( msgSvc(), name() );
    
    if (storageType != repSvcType() ) {
        log << MSG::ERROR << "bad storage type" << storageType << endreq;
        return StatusCode::FAILURE;
    }

    refpAddress = new GenericAddress(storageType,
				     clid,
				     "",  
				     "",
				     0);

    return StatusCode::SUCCESS;
}

StatusCode RawDataCnvSvc::createAddress(long storageType,
                                        const CLID& clid,
					const std::string&,
					IOpaqueAddress*& refpAddress)
{
  return createAddress(storageType, clid, NULL, NULL, refpAddress);
}
