#ifndef ISIMPLE_PID_SVC_H
#define ISIMPLE_PID_SVC_H

#include "GaudiKernel/IService.h"
#include "EvtRecEvent/EvtRecTrack.h"

/* Decaration of the interface ID */
static const InterfaceID IID_ISimplePIDSvc("ISimplePIDSvc", 1, 0);

class EvtRecDTag;

class ISimplePIDSvc : virtual public IService
{
	public :
		static const   InterfaceID& interfaceID() { return IID_ISimplePIDSvc; }
		virtual        ~ISimplePIDSvc() {}
		virtual void   setdedxminchi(double x) = 0;
		virtual void   settofminchi(double x) = 0;
		virtual void   seteidratio(double x) = 0;
		virtual void   preparePID(EvtRecTrack* track) = 0;
		virtual bool   iselectron(bool emc = false) = 0;
		virtual bool   ispion() = 0;
		virtual bool   iskaon() = 0;
		virtual double probElectron() = 0;
		virtual double probMuon() = 0;
		virtual double probPion() = 0;
		virtual double probKaon() = 0;
		virtual double probProton() = 0;
		virtual double getdEdxChi(int i) = 0;
		virtual double getTOFChi(int i) = 0;
		virtual double getChi2(int i) = 0;
		virtual double getEmcLikelihood(int i) = 0;
		virtual double getTofDt(int m, int n) = 0;
		virtual double getTofPh(int n) = 0;
		virtual double getTofZR(int n) = 0;
		virtual int    getTofCounter(int n) = 0;
		virtual int    getTofBarrel() = 0;
		virtual double getEmcE() = 0;
		virtual double getEmcE13() = 0;
		virtual double getEmcE35() = 0;
		virtual double getEmcLatMoment() = 0;
		virtual double getEmcSecMoment() = 0;
		virtual double getElectronLikelihood() = 0;

};

#endif
