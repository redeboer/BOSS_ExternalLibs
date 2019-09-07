#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DummyLoadOldROOTAlg.h"

DECLARE_ALGORITHM_FACTORY( DummyLoadOldROOTAlg )

DECLARE_FACTORY_ENTRIES( DummyLoadOldROOT ) {
	DECLARE_ALGORITHM( DummyLoadOldROOTAlg );
}
