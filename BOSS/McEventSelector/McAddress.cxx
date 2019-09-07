//====================================================================
//	McAddress implementation
//--------------------------------------------------------------------
//
//	Package    : McEventSelector
//
//  Description: Event TDR address implementation
//
//====================================================================

//own 
#include "McEventSelector/McAddress.h"
#include "McEventSelector/McCnvSvc.h"

// Framework include files
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/MsgStream.h"

// Event model
#include <iostream>

// Externals

/// Standard Constructor
McAddress::McAddress(const CLID& clid, const std::string& fname, const std::string& cname )
: GenericAddress(McCnvSvc::storageType(), clid, fname, cname),
  m_runNo(0), m_eventNo(0)
{ }
McAddress::McAddress(const CLID& clid, const std::string& fname, 
	   const int& run, const int& event, const std::string& cname)
: GenericAddress(McCnvSvc::storageType(), clid, fname, cname),
  m_runNo(run), m_eventNo(event)
{ }

