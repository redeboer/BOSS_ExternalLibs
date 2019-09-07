#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include <iostream>

AthenaHitsCollectionHelper::AthenaHitsCollectionHelper()
{
	std::cout<<" new collection helper being created "<<std::endl;
	ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
      
      	IMessageSvc*	m_msgSvc;
      	StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
      
      	// MsgStream log(m_msgSvc, "AthenaHitsCollectionHelper");
      	// status = svcLocator->service("StoreGateSvc", m_storeGate);
}
AthenaHitsCollectionHelper::~AthenaHitsCollectionHelper()
{
}
