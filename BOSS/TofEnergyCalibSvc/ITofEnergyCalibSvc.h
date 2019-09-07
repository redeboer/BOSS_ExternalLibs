#ifndef ITOFENERGYCALIBSVC_H_
#define ITOFENERGYCALIBSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include <string>
#include "CLHEP/Matrix/Vector.h"
using CLHEP::HepVector;

/* Declaration of the interface ID */
static const InterfaceID IID_ITofEnergyCalibSvc("ITofEnergyCalibSvc",1,0);

class ITofEnergyCalibSvc: virtual public IInterface{
public:
	static const InterfaceID& interfaceID() { return IID_ITofEnergyCalibSvc; }

	virtual  void          SetIsData(const std::string& _IsData) =0;
	virtual  std::string   GetIsData() const=0;
	virtual  void          SetBossRelease(const std::string& _bossrelease)=0;
	virtual  std::string   GetBossRelease() const=0; 

	virtual bool getTofEnergyCalibSvcInfo()=0;
	virtual double getCalibConst() const = 0;
	virtual double getPara1() const = 0;
	virtual double getPara2() const = 0;
	virtual double getPara3() const = 0;
	virtual double getPara4() const = 0;
	virtual double getPara5() const = 0;

};

#endif /* ITOFENERGYCALIBSVC_H_ */
