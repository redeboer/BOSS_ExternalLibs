#ifndef ServiceAccessor_H
#define ServiceAccessor_H


#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/IHistogramSvc.h"

// access the message service
inline IMessageSvc* msgSvc() 
{
	static IMessageSvc* mS=0;
	if (!mS)
	{
		ISvcLocator* svcLocator = Gaudi::svcLocator();
		svcLocator->service("MessageSvc", mS);
	}
	return mS;	
}

// access the histogram service
inline IHistogramSvc* histoSvc() 
{
	static IHistogramSvc* hS=0;
	StatusCode status;
	if (!hS)
	{
		ISvcLocator* svcLocator = Gaudi::svcLocator();
		status=svcLocator->service("HistogramDataSvc",hS);
	}
	return hS;
}

// access store gate

#include "StoreGate/StoreGateSvc.h"

inline StoreGateSvc* storeGateSvc()
{
	static StoreGateSvc* sG=0;
	StatusCode status;
	if (!sG)
	{
     		ISvcLocator* svcLocator = Gaudi::svcLocator();
		status = svcLocator->service("StoreGateSvc", sG);
	}
	return sG;
}

#include "GaudiKernel/INTupleSvc.h"

inline INTupleSvc* ntupleSvc()
{
	static INTupleSvc* nS=0;
	StatusCode status;
	if (!nS)
	{
		ISvcLocator* svcLocator = Gaudi::svcLocator();
		status = svcLocator->service("NTupleSvc",nS);
	}
	return nS;
}

#endif

