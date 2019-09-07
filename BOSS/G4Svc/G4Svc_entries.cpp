#include "GaudiKernel/DeclareFactoryEntries.h"

#include "G4Svc/G4Svc.h"

DECLARE_FACTORY_ENTRIES(G4Svc) {
  DECLARE_SERVICE( G4Svc );
}

DECLARE_SERVICE_FACTORY( G4Svc )

