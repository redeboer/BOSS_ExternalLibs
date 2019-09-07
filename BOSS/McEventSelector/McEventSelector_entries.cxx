//====================================================================
//  McEventSelector_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : Generators/McEventSelector
//
//  Description: Implementation of <Package>_load routine.
//               This routine is needed for forcing the linker
//               to load all the components of the library. 
//
//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "McEventSelector/McEventCnv.h"
#include "McEventSelector/McCnvSvc.h"
#include "McEventSelector/McEventSelector.h"

DECLARE_CONVERTER_FACTORY(McEventCnv)
DECLARE_SERVICE_FACTORY(McEventSelector)
DECLARE_SERVICE_FACTORY(McCnvSvc)

DECLARE_FACTORY_ENTRIES( McEventSelector ) {
    DECLARE_CONVERTER( McEventCnv )
    DECLARE_SERVICE  ( McEventSelector )
    DECLARE_SERVICE  ( McCnvSvc )
}

