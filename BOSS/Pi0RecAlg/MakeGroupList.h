#ifndef MAKEGROUPLIST__H
#define MAKEGROUPLIST__H

#include <list>
#include <algorithm>
#include "Criteria.h"
#include "VertexFit/KinematicFit.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "Pi0Cut.h"
namespace Pi0{


	typedef std::list<Criteria> CriteriaList;
	typedef std::list<Criteria>::iterator CriteriaIterator;
	typedef std::list<Criteria>::const_iterator Const_CriteriaIteator;

	typedef std::list<GammaPair> Pi0List;
	typedef std::list<GammaPair>::iterator Pi0Iterator;
	typedef std::list<GammaPair>::const_iterator Const_Pi0Iterator;

        typedef std::list<EvtRecTrack*> GammaList;
        typedef std::list<EvtRecTrack*>::iterator GammaIterator;
        typedef std::list<EvtRecTrack*>::const_iterator Const_GammaIterator;

        CriteriaList default_criteria_list;
	Pi0List candidate_pi0_list;
	Pi0List filter_pi0_list;
	GammaList default_gamma_list;//one copy of all photons
//	GammaList update_gamma_list;//a subset of default_gamma_list 

        GammaList& GetDefaultGammaList() { return default_gamma_list;}
//        GammaList& GetUpdateGammaList() { return update_gamma_list;}
        Pi0List& GetCandidatePi0List() { return candidate_pi0_list;}
        Pi0List& GetFilterPi0List() { return filter_pi0_list;}

//	GammaList& make_gamma_list(EvtRecEvent* recEvt, EvtRecTrackCol* recTrkCol)
	GammaList& make_gamma_list(UserPi0Cut& cut)
	{
                EvtRecEvent* recEvt = UserPi0Cut::recEvt;
                EvtRecTrackCol* recTrkCol = UserPi0Cut::recTrkCol;

		default_gamma_list.clear();
//		update_gamma_list.clear();
		for(int i1 = recEvt->totalCharged(); i1 < (recEvt->totalTracks()); ++i1) 
		{

			EvtRecTrack* gTrk = *(recTrkCol->begin() + i1);
                        if(cut.isGoodPhoton(gTrk))
			{
				default_gamma_list.push_back(gTrk);
//				update_gamma_list.push_back(gTrk);
			}
		}
		return default_gamma_list;
	}
/*	GammaList& remake_gamma_list_by_remove(const GammaPair& gp)//remove two gamma ref by gp from list
	{
		update_gamma_list.remove(gp.First);
		update_gamma_list.remove(gp.Second);
		return update_gamma_list;
	}*/
	void print_gamma_list(const GammaList& list)
	{
		if(list.size() == 0) {std::cout<<"GammaList->()"<<std::endl; return;}
		std::cout<<"GammaList->(";
		for(Const_GammaIterator it = list.begin(); it!= list.end(); it++)
		{
			std::cout<<(*it)->trackId()<<", "; 
		}
		std::cout<<')'<<std::endl;
	}
	//=============
	Pi0List& make_pi0_list(const GammaList& gamma_list)
	{
		candidate_pi0_list.clear();
		Const_GammaIterator i_it = gamma_list.begin();
		Const_GammaIterator i_it_end = gamma_list.end();
		Const_GammaIterator j_it_end = gamma_list.end();

		--i_it_end;

		//		KinematicFit * kmfit = KinematicFit::instance();
		Const_GammaIterator j_it ;
		for(; i_it != i_it_end; ++i_it)
		{
			for( j_it = i_it, ++j_it ; j_it != j_it_end; ++j_it)
			{

				EvtRecTrack* g1Trk = *i_it;
				EvtRecTrack* g2Trk = *j_it;

				RecEmcShower* g1Shower = g1Trk->emcShower();
				RecEmcShower* g2Shower = g2Trk->emcShower();
				double inv_m = (getP4(g1Shower) + getP4(g2Shower)).m();

				/*				kmfit->init();
								kmfit->AddTrack(0, 0.0, g1Shower);
								kmfit->AddTrack(1, 0.0, g2Shower);
								kmfit->AddResonance(0, 0.1349766, 0, 1);
								kmfit->Fit(0);*/

				candidate_pi0_list.push_back(GammaPair(*i_it, *j_it, inv_m)); 
			}
		}
		return candidate_pi0_list;
	}
	void print_pi0_list(const Pi0List& list)
	{
		std::cout<<"OK Pi0List->{";
		for(Const_Pi0Iterator it = list.begin(); it!= list.end(); it++)
		{
			std::cout<<"("<<(*it).inv_m<<","<<(*it).First->trackId()<<","<<(*it).Second->trackId()<<"), ";       
		}
		std::cout<<'}'<<std::endl;
	}
	Pi0List& apply_criteria(const Criteria& cri)
	{
		Pi0Iterator it = candidate_pi0_list.begin();
		for(; it!=candidate_pi0_list.end();)
		{
			if( cri.check(*it))
				++it;
			else{
				filter_pi0_list.push_back(*it);
				it = candidate_pi0_list.erase(it);
			}

		}
		return candidate_pi0_list;
	}
	void Pi0ListToTDS(const Pi0List& pi0list, EvtRecPi0Col* recPi0Col)
	{
		assert(recPi0Col);
		static double xmpi0= 0.1349766;
		KinematicFit * kmfit = KinematicFit::instance();

		for(Const_Pi0Iterator it = pi0list.begin(); it != pi0list.end(); ++it)
		{
			EvtRecTrack* g1Trk = (*it).First;
			EvtRecTrack* g2Trk = (*it).Second;
			RecEmcShower* g1Shower = g1Trk->emcShower();
			RecEmcShower* g2Shower = g2Trk->emcShower();

			kmfit->init();
			kmfit->AddTrack(0, 0.0, g1Shower);
			kmfit->AddTrack(1, 0.0, g2Shower);
			kmfit->AddResonance(0, xmpi0, 0, 1);
			kmfit->Fit(0);

			HepLorentzVector g1P4 = getP4(g1Shower);
			HepLorentzVector g2P4 = getP4(g2Shower);
			HepLorentzVector p2g = g1P4 + g2P4;
			EvtRecPi0* recPi0 = new EvtRecPi0();

			recPi0->setUnconMass(p2g.restMass());
			recPi0->setChisq(kmfit->chisq(0));
			if ( g1P4.e() >= g2P4.e() ) {
				recPi0->setHiPfit(kmfit->pfit(0));
				recPi0->setLoPfit(kmfit->pfit(1));
				recPi0->setHiEnGamma(g1Trk);
				recPi0->setLoEnGamma(g2Trk);
			}
			else {
				recPi0->setHiPfit(kmfit->pfit(1));
				recPi0->setLoPfit(kmfit->pfit(0));
				recPi0->setHiEnGamma(g2Trk);
				recPi0->setLoEnGamma(g1Trk);
			}

			recPi0Col->push_back(recPi0);
		}
	}
	//pi0 reconstruction strategy
	/*  void priority_method(int nPi0, EvtRecPi0Col* pi0_col) //reconstruct pi0 one by one
	    {
	    assert(pi0_col);
	    int i = 0;
	    Pi0List& _this = make_pi0_list(update_gamma_list);
	    print_pi0_list(_this);
	    Pi0List ret = the_most_optimized(_this);
	    Pi0List2TDS(ret, pi0_col);
	    ++i;
	    while(i<nPi0){
	    remake_gamma_list_by_remove(*(ret.begin()));
	    Pi0List& _this = make_pi0_list(update_gamma_list);
	    print_pi0_list(_this);
	    Pi0List ret = the_most_optimized(_this);
	    Pi0List2TDS(ret, pi0_col);
	    };
	    }*/
	/*	void general_method(EvtRecPi0Col* pi0_col)
		{
		make_pi0_list(default_gamma_list);
		Criteria cri_inv;
		apply_criteria(cri_inv);
		BasicCriteria cri_kfit;
		apply_criteria(cri_kfit);
		Pi0ListToTDS(candidate_pi0_list, pi0_col);
		}*/
	void apply_criterias()
	{
		for(CriteriaIterator it = default_criteria_list.begin(); it != default_criteria_list.end(); ++it)
		{
			apply_criteria(*it);
		}
	}
	void add_cut(const Criteria& cri)
	{
		default_criteria_list.push_back(cri);
	}
	/*	void rec_pi0(EvtRecEvent* recEvt, EvtRecTrackCol* recTrkCol, EvtRecPi0Col* pi0_col, int nPi0=0)//nPi0, you want to reconstruct n Pi0 
		{
	//		assert(recEvt);assert(recTrkCol);
	GammaList& gamma_list = make_gamma_list(recEvt, recTrkCol);
	//		std::cout<<"gamma in total = "<<gamma_list.size()<<std::endl;
	//		std::cout<<"maximum pi0 N = "<<gamma_list.size()/2<<std::endl;
	//		if(gamma_list.size()/2 <nPi0) nPi0 = gamma_list.size()/2;
	//		print_gamma_list(gamma_list);
	//                priority_method(nPi0, pi0_col);
	make_pi0_list(gamma_list);
	apply_criterias();
	//		Criteria cri_inv;
	//		apply_criteria(cri_inv);
	//		KFitCriteria cri_kfit;
	//		apply_criteria(cri_kfit);
	candidate_pi0_list.sort(high_momentum());
	print_pi0_list(candidate_pi0_list);
	Pi0ListToTDS(candidate_pi0_list, pi0_col);
	}*/
}
#endif
