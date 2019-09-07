#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "RawDataCnvBase/RawDataBaseCnv.h"
#include "RawDataCnvBase/IRawDataCnvSvc.h"
#include <iostream>

static const InterfaceID IID_IRawDataBaseCnv("IRawDataBaseCnv", 1 , 0);

RawDataBaseCnv::RawDataBaseCnv(unsigned char storageType, const CLID& clid, ISvcLocator* svc)
  : Converter((long)storageType, clid, svc), m_CnvSvc(0)  
{}

  
RawDataBaseCnv::RawDataBaseCnv(ISvcLocator* svc)
: Converter(Test_StorageType, classID(), svc)
			      , m_CnvSvc(0)  
{}
  

StatusCode RawDataBaseCnv::createRep(DataObject* , 
                              IOpaqueAddress*& )   {
    // Purpose and Method: Convert the transient object to the requested 
    //     representation.  It is expected that derived classes will override
    //     this method.
    return StatusCode::SUCCESS;
}

StatusCode RawDataBaseCnv::fillRepRefs(IOpaqueAddress* ,
                                DataObject* )    {
    // Purpose and Method:  Resolve the references of the converted object.
    //     It is expected that derived classes will override this method.
    return StatusCode::SUCCESS;
}

StatusCode RawDataBaseCnv::updateRep(IOpaqueAddress* , 
                              DataObject* )   {
    // Purpose and Method:  Update the converted representation of a transient 
    //     object.  It is expected that derived classes will override this.
    return StatusCode::SUCCESS;
}

StatusCode RawDataBaseCnv::updateRepRefs(IOpaqueAddress* , 
                                  DataObject* ) {
    // Purpose and Method:  Update the references of an already converted object.
    //   It is expected that derived classes will override this method.
    return StatusCode::SUCCESS;
}

StatusCode RawDataBaseCnv::initialize()   {
    // Purpose and Method:  Perform standard converter initialization.
    //   Access the EventCnvSvc to create an association between converters 
    //   and paths within the TDS, using the vector of leaves and the
    //   declareObject methods available in each specific converter.
    StatusCode status = Converter::initialize();
    /*
    if ( status.isSuccess() )   {
        IService* isvc = 0;
        status = serviceLocator()->service(cnvSvcName, isvc, true);        
        if ( status.isSuccess() )   {
            status = isvc->queryInterface(IID_IRawDataBaseCnv, (void**)&m_CnvSvc);
            if ( status.isSuccess() )   {
                for ( std::vector<IRawDataCnvSvc::Leaf>::iterator i = m_leaves.begin(); i != m_leaves.end(); i++ )    {
                    m_CnvSvc->declareObject(*i);
                }
            }
        }
    }
    */
    return status;
}

StatusCode RawDataBaseCnv::finalize()   {
    if ( m_CnvSvc )     {
        m_CnvSvc->release();
        m_CnvSvc = 0;
    }
    return Converter::finalize();
}

void RawDataBaseCnv::declareObject(const std::string& /*path*/, const CLID& /*cl*/, 
                            const std::string& /*bank*/, long /*par*/)  {
    // Purpose and Method:  Save the path on the TDS, in the m_leaves vector, 
    //   corresponding to the DataObject that the converter handles.
    // m_leaves.push_back(IRawDataCnvSvc::Leaf(path, cl, bank, par));
}



