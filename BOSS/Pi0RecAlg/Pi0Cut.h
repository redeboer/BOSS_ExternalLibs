#ifndef __PI0SELECTOR__H
#define __PI0SELECTOR__H

#include "DecayChain/Function/DCSelectionFunction.h"
#include "BesDChain/CDPi0.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecPi0.h"

struct UserPi0Cut : public DCSelectionFunction<CDPi0>
{
	UserPi0Cut(double MEBP = 0.025, double MEEP = 0.045,
			double MA = 20.,
			double TL = 25.,
			double MinM = 0.12, double MaxM = 0.15,
			double c = 1e+10)
		: MinEnergyOfBarrelPhoton(MEBP), MinEnergyOfEndcapPhoton(MEEP),
		MinAngle(MA), 
		TimeLimit(TL),
		Chisq(c),
		MinMass(MinM), MaxMass(MaxM)
		{}

	double MinEnergyOfBarrelPhoton;
	double MinEnergyOfEndcapPhoton;
	double MinAngle;
	double TimeLimit;
	double MinMass;
	double MaxMass;
	double Chisq;

	bool operator()(CDPi0& _this)
	{
		EvtRecTrack* g1 = const_cast<EvtRecTrack*>(_this.navPi0()->hiEnGamma());
		EvtRecTrack* g2 = const_cast<EvtRecTrack*>(_this.navPi0()->loEnGamma());
		if(!isGoodPhoton(g1)) return false;
		if(!isGoodPhoton(g2)) return false;
		if(_this.navPi0()->chisq() > Chisq) return false;
		if(_this.navPi0()->unconMass() < MinMass || _this.navPi0()->unconMass() > MaxMass) return false;
		return true;
	}

	bool isGoodPhoton(EvtRecTrack* gTrk)
	{
		if ( !gTrk->isEmcShowerValid() ) return false;

		RecEmcShower *emcTrk = gTrk->emcShower();
		//check time
		if(emcTrk->time() > TimeLimit) return false;
		//check energy
		int flag =(emcTrk->cellId() & 0x000F0000) >> 16;
		if ((flag == 1) && emcTrk->energy() < MinEnergyOfBarrelPhoton) return false;
		if ((flag == 0 || flag == 2) && emcTrk->energy() < MinEnergyOfEndcapPhoton) return false;
		//check angle
		Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());
		if (recEvt->totalCharged() > 0)
		{
			double dthe = 200.;
			double dphi = 200.;
			double dang1 = 200.;
			for (int j = 0; j < recEvt->totalCharged(); j++) {
				EvtRecTrackIterator jtTrk = recTrkCol->begin() + j;
				if ( !(*jtTrk)->isExtTrackValid() ) continue;
				RecExtTrack* extTrk = (*jtTrk)->extTrack();
				if ( extTrk->emcVolumeNumber() == -1 ) continue;
				Hep3Vector extpos = extTrk->emcPosition();
				double  angd1 = extpos.angle(emcpos);
				double  thed = extpos.theta() - emcpos.theta();
				double  phid = extpos.deltaPhi(emcpos);
				thed = fmod(thed+CLHEP::twopi+CLHEP::twopi+CLHEP::pi, CLHEP::twopi) - CLHEP::pi;
				phid = fmod(phid+CLHEP::twopi+CLHEP::twopi+CLHEP::pi, CLHEP::twopi) - CLHEP::pi;

				if ( fabs(thed) < fabs(dthe) ) dthe = thed;
				if ( fabs(phid) < fabs(dphi) ) dphi = phid;
				if ( angd1 < dang1 ) dang1 = angd1;
			}
			if ( dang1 < 200 ) {
				dthe = dthe * 180 / (CLHEP::pi);
				dphi = dphi * 180 / (CLHEP::pi);
				dang1 = dang1 * 180 / (CLHEP::pi);
				if ( dang1 < MinAngle) return false;


			}  // End of "dang1 < 200" IF 

		}  // End of "recEv
		return true;
	}
	public:
	static void SetForTrack(EvtRecEvent* _recEvt, EvtRecTrackCol* _recTrkCol){ recEvt =_recEvt; recTrkCol=_recTrkCol;}
	static EvtRecEvent* recEvt;
	static EvtRecTrackCol* recTrkCol;
};
namespace Pi0{
	extern UserPi0Cut VeryLoosePi0Cut;
	extern UserPi0Cut DefaultPi0Cut;
}
#endif
