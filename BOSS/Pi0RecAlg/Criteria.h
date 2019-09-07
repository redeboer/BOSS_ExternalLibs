#ifndef CRITERIA__H
#define CRITERAI__H

#include "GaudiKernel/SmartDataPtr.h"

#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"

HepLorentzVector getP4(RecEmcShower* gTrk)
{

   double eraw = gTrk->energy();
   double phi =  gTrk->phi();
   double the =  gTrk->theta();

   return HepLorentzVector( eraw * sin(the) * cos(phi),
                            eraw * sin(the) * sin(phi),
                            eraw * cos(the),
                            eraw );
}
bool isGoodGamma(EvtRecTrack* gtrk)
{

}
struct GammaPair : public std::pair<EvtRecTrack*,EvtRecTrack*>
{
	GammaPair(EvtRecTrack* g1, EvtRecTrack* g2, double m/*, double s, double c*/)
		:First(g1), Second(g2), inv_m(m)/*, chisq(s), cos_theta(c)*/{;}
	EvtRecTrack* First;
	EvtRecTrack* Second;
	double inv_m;
//	double chisq;
//	double cos_theta;
};
/*struct less_chisq: public binary_function <GammaPair, GammaPair, bool>
{
	bool operator()(const GammaPair& lgp, const GammaPair& rgp)
	{
		return lgp.chisq < rgp.chisq;
	}
};*/
struct high_momentum: public binary_function <GammaPair, GammaPair, bool>
{
	bool operator()(const GammaPair& lgp, const GammaPair& rgp)
	{
		double p1 = (getP4(lgp.First->emcShower())+ getP4(lgp.Second->emcShower())).rho();
		double p2 = (getP4(rgp.First->emcShower())+ getP4(rgp.Second->emcShower())).rho();
                return p1 > p2;
	}
};
struct Criteria
{
	Criteria(double inv_min = 0.10, double inv_max = 0.18):_inv_min(inv_min), _inv_max(inv_max){;}
	double _inv_min;
	double _inv_max;
	virtual bool check(const GammaPair& gp) const 
	{
		//                std::cout<<"gp.inv_m"<<gp.inv_m<<" inv_min"<<_inv_min<<std::endl;
		if( gp.inv_m> _inv_min && gp.inv_m < _inv_max) return true;
		else return false;
	}
};
/*struct KFitCriteria : public Criteria
{
	KFitCriteria(double chisq_cut = 20., double costheta_cut = 0.98 ): _chisq_cut(chisq_cut),_costheta_cut(costheta_cut){;}
	double _chisq_cut;
	double _costheta_cut;
	virtual bool check(const GammaPair& gp) const 
	{
		//                std::cout<<"gp.chisq"<<gp.chisq<<" chisq_cut"<<_chisq_cut<<std::endl;
		if(gp.chisq > _chisq_cut) return false;
		if(fabs(gp.cos_theta) > _costheta_cut) return false;
		return true;
	}
};*/
/*struct helper : public Criteria
{
	helper(){;}
	double mass(const GammaPair& gp) const{
		EvtRecTrack* g1Trk = gp.First;
		EvtRecTrack* g2Trk = gp.Second;

		RecEmcShower* g1Shower = g1Trk->emcShower();
		RecEmcShower* g2Shower = g2Trk->emcShower();
		return (getP4(g1Shower)+getP4(g2Shower)).m();
	}
	double angle(const GammaPair& gp) const{
		EvtRecTrack* g1Trk =  gp.First;
		EvtRecTrack* g2Trk =  gp.Second;

		RecEmcShower* g1Shower = g1Trk->emcShower();
		RecEmcShower* g2Shower = g2Trk->emcShower();
		return getP4(g1Shower).angle(getP4(g2Shower).vect());;
	}
};*/
#endif
