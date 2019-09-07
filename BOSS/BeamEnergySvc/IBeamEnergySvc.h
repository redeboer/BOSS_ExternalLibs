#ifndef IBEAM_ENERGY_SVC_H
#define IBEAM_ENERGY_SVC_H

#include "GaudiKernel/IService.h"

/* Decaration of the interface ID */
static const InterfaceID IID_IBeamEnergySvc("IBeamEnergySvc", 1, 0);

class EvtRecDTag;

class IBeamEnergySvc : virtual public IService
{
  public :
	virtual ~IBeamEnergySvc() {}

	static const InterfaceID& interfaceID() { return IID_IBeamEnergySvc; }

	virtual bool isRunValid() = 0;
	virtual double getbeamE() = 0;
	virtual void getBeamEnergyInfo() = 0;
};

#endif
