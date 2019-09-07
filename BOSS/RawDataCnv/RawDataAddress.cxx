//====================================================================
//	RawDataAddress.cxx
//====================================================================
//
// Description: An address class used to access Event Filter packed
//              raw event persistent data from within the Athena 
//              framework.
//
//--------------------------------------------------------------------

// Include files.
#include "GaudiKernel/GenericAddress.h"
#include "RawDataCnv/ClassID_temp.h"
#include "RawDataCnv/RawDataAddress.h"


class EVENT;

//  Constructor.
RawDataAddress::RawDataAddress(const CLID& clid, const string& fname, 
    const string& cname) 
     : GenericAddress(PACKEDRAWEVENT_StorageType, clid, fname, cname),
       m_Event(NULL),
       m_clID(clid)
{
}

// Destructor.
RawDataAddress::~RawDataAddress()
{
}

void RawDataAddress::setEvent(const EVENT* evt) 
{
   m_Event = evt;
}

/// Access : Retrieve class ID of the link
const CLID& RawDataAddress::clID()  const   {
    return m_clID;
}


