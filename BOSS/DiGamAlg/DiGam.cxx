#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ITHistSvc.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "McTruth/McParticle.h"
#include "TMath.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "CLHEP/Geometry/Point3D.h"
#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "ParticleID/ParticleID.h"
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
const double velc = 299.792458;
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;
#include "DiGamAlg/DiGam.h"
#include <vector>
#include "DiGamAlg/Parameter.h"

const double pai=3.1415926;
int N0;int N1;int N2;int N3;int N4;
DiGam::DiGam(const std::string& name, ISvcLocator* pSvcLocator) :
	Algorithm(name, pSvcLocator) {
		declareProperty("jpsiCrossSection",jpsiCrossSection=249.7);
		declareProperty("jpsiMCEff",jpsiMCEff=0.07145);
		declareProperty("jpsiMCEffBoost",jpsiMCEffBoost=0.07099);
		
		declareProperty("psi2sCrossSection",psi2sCrossSection=180.8);
		declareProperty("psi2sMCEff",psi2sMCEff=0.07159);
		declareProperty("psi2sMCEffBoost",psi2sMCEffBoost=0.07123);

		declareProperty("psi3770CrossSection",psi3770CrossSection=173.4);
		declareProperty("psi3770MCEff",psi3770MCEff=0.071725);
		declareProperty("psi3770MCEffBoost",psi3770MCEffBoost=0.0713125);

		declareProperty("CrossSection",CrossSection);
		declareProperty("MCEff",MCEff);
		declareProperty("MCEffBoost",MCEffBoost);
		
		declareProperty("boostPhimin", boostPhimin=2.5);
		declareProperty("boostPhimax",boostPhimax=5);
		declareProperty("boostMinEmin",boostMinEmin);
		declareProperty("boostMinEmax",boostMinEmax);

		declareProperty("RunModel",RunModel=2000);
		declareProperty("dPhiMin",dPhiMin=-7);
		declareProperty("dPhiMax",dPhiMax=5);
		declareProperty("dPhiMinSig",dPhiMinSig=-4);
		declareProperty("dPhiMaxSig",dPhiMaxSig=2);
	
		declareProperty("flag",flag=true);
		declareProperty("E_cms",E_cms);
	}
//***************************************************************************************
StatusCode DiGam::initialize(){
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in initialize()" << endmsg;
        
	if(RunModel==1){//jpsi
	   CrossSection=jpsiCrossSection;
	   MCEff=jpsiMCEff;
	   MCEffBoost=jpsiMCEffBoost;
	   boostPhimin=2.5;
	   boostPhimax=5;
	   boostMinEmin=1.2;
	   boostMinEmax=1.6;
	}
	else if(RunModel==2){//psi2s
	   CrossSection=psi2sCrossSection;
	   MCEff=psi2sMCEff;
	   MCEffBoost=psi2sMCEffBoost;
	   boostPhimin=2.5;
	   boostPhimax=5;
	   boostMinEmin=1.4;
	   boostMinEmax=1.9;
	}
        else if(RunModel==3){//psi3770
	   CrossSection=psi3770CrossSection;
	   MCEff=psi3770MCEff;
	   MCEffBoost=psi3770MCEffBoost;
	   boostPhimin=2.5;
	   boostPhimax=5;
	   boostMinEmin=1.4;
	   boostMinEmax=2;
	}
	
        tot=0;
	signal=0;
	boost_signal=0;
	boost_tot=0;
	StatusCode ssc=service("THistSvc", m_thsvc);
	if(ssc.isFailure()) {
		log << MSG::FATAL << "DiGamAlg:Couldn't get THistSvc" << endreq;
	        return ssc;
	}
        h_lum=new TH1F("lum","lum",4,0.5,4.5);

        StatusCode scBeamEnergy=service("BeamEnergySvc", m_BeamEnergySvc); 

        if( scBeamEnergy.isFailure() ){
           log << MSG::FATAL << "can not use BeamEnergySvc" << endreq;
	   return scBeamEnergy;
        }
 
	StatusCode status;

	NTuplePtr nt2(ntupleSvc(),"DQAFILE/zhsDiGam");
	if(nt2)m_tuple2=nt2;
	else{
		m_tuple2=ntupleSvc()->book("DQAFILE/zhsDiGam",CLID_ColumnWiseTuple,"ks N-Tuple example");
		if(m_tuple2){
			status=m_tuple2->addItem("ETol",m_tot);
			status=m_tuple2->addItem("maxE",m_maxE);
			status=m_tuple2->addItem("minE",m_minE);
			status=m_tuple2->addItem("maxTheta",m_maxTheta);
			status=m_tuple2->addItem("minTheta",m_minTheta);
			status=m_tuple2->addItem("maxPhi",m_maxPhi);
			status=m_tuple2->addItem("minPhi",m_minPhi);
			status=m_tuple2->addItem("delTheta",m_delTheta);
			status=m_tuple2->addItem("delPhi",m_delPhi);
			status=m_tuple2->addItem("angle",m_angle);
			status=m_tuple2->addItem("boostAngle",m_boostAngle);
			status=m_tuple2->addItem("boostMaxE",m_boostMaxE);
			status=m_tuple2->addItem("boostMinE",m_boostMinE);
			status=m_tuple2->addItem("boostDelPhi",m_boostDelPhi);
			status=m_tuple2->addItem("boostDelTheta",m_boostDelTheta);
			status=m_tuple2->addItem("boostM",m_boostM);
			status=m_tuple2->addItem("boostIM",m_boostIM);
			status=m_tuple2->addItem("m",m_m);
			status=m_tuple2->addItem("IM",m_IM);
			
			status=m_tuple2->addItem ("run",m_run );
			status=m_tuple2->addItem ("rec",m_rec );
			status=m_tuple2->addItem("indexmc",          m_idxmc, 0, 100);
			status=m_tuple2->addIndexedItem("pdgid",     m_idxmc, m_pdgid);
			status=m_tuple2->addIndexedItem("motheridx", m_idxmc, m_motheridx);

		}
		else {
			log<<MSG::FATAL<<"Cannot book N-tuple:"<<long(m_tuple2)<<endmsg;
			return StatusCode::FAILURE;
		}
	}
	
	return StatusCode::SUCCESS;
}
//******************************************************************************
StatusCode DiGam::execute() {
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in execute()" << endreq;
	N0++;
	SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
	runNo=eventHeader->runNumber();
	int event=eventHeader->eventNumber();
	log<<MSG::INFO<<"run,evtnum="<<runNo<<","<<event<<endreq;
	m_run = eventHeader->runNumber();
	m_rec = eventHeader->eventNumber();

	if(flag == true)
	{
		flag = false;
		log << MSG::DEBUG << "setting parameter" << endreq;
		
		m_BeamEnergySvc->getBeamEnergyInfo();
		if ( ! m_BeamEnergySvc->isRunValid() ) return StatusCode::FAILURE;
		E_cms = 2*m_BeamEnergySvc->getbeamE();
		log << MSG::DEBUG << "cms energy is " << E_cms << endreq;
		
		Parameter *func = new Parameter();
		func->parameters(E_cms);
		
		CrossSection=func->m_CrossSection;
		MCEff=func->m_MCEff;
		MCEffBoost=func->m_MCEffBoost;
		boostMinEmin=func->m_boostMinEmin;
		boostMinEmax=func->m_boostMinEmax;
	}

	SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
	log<<MSG::INFO<<"ncharg,nneu,tottks="
		<<evtRecEvent->totalCharged()<<","
		<<evtRecEvent->totalNeutral()<<","
		<<evtRecEvent->totalTracks()<<endreq;
	SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);
	std::vector<int> iGam;
	iGam.clear();
	N1++;
	for(int i=evtRecEvent->totalCharged();i< evtRecEvent->totalTracks();i++){
		EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
		if(!(*itTrk)->isEmcShowerValid()) continue;
		RecEmcShower *emcTrk = (*itTrk)->emcShower();
		if(emcTrk->energy()<0.6)continue;
		if(fabs(cos(emcTrk->theta()))>0.83)continue;
		iGam.push_back((*itTrk)->trackId());
	}
	if(iGam.size()<2)return StatusCode::SUCCESS;
	N2++;
	double energy1=0.5;
	double energy2=0.5;
	int gam1=-9999;
	int gam2=-9999;
	for(int i=0;i<iGam.size();i++){
	        EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + iGam[i];
		RecEmcShower *emcTrk = (*itTrk)->emcShower();
		//std::cout<<emcTrk->energy()<<std::endl;
		if(emcTrk->energy()>energy1){                       //max emc
			energy2=energy1;
			gam2=gam1;
			energy1=emcTrk->energy();
			gam1=iGam[i];
		}
		else if(emcTrk->energy()>energy2){                     //second max emc
			energy2=emcTrk->energy();
			gam2=iGam[i];
		}
	}
	if(gam1==-9999 || gam2==-9999)return StatusCode::SUCCESS;
        N3++;
	m_tot=energy1+energy2;
        RecEmcShower* maxEmc=(*(evtRecTrkCol->begin()+gam1))->emcShower();
	RecEmcShower* minEmc=(*(evtRecTrkCol->begin()+gam2))->emcShower();
	m_maxE=maxEmc->energy();
	m_minE=minEmc->energy();
	m_maxTheta=maxEmc->theta();
	m_minTheta=minEmc->theta();
	m_maxPhi=maxEmc->phi();
	m_minPhi=minEmc->phi();
	m_delPhi=(fabs(m_maxPhi-m_minPhi)-pai)*180./pai;
	m_delTheta=(fabs(m_maxTheta+m_minTheta)-pai)*180./pai;
	
      HepLorentzVector max,min;
	max.setPx(m_maxE*sin(m_maxTheta)*cos(m_maxPhi));
	max.setPy(m_maxE*sin(m_maxTheta)*sin(m_maxPhi));
	max.setPz(m_maxE*cos(m_maxTheta));
	max.setE(m_maxE);
	min.setPx(m_minE*sin(m_minTheta)*cos(m_minPhi));
	min.setPy(m_minE*sin(m_minTheta)*sin(m_minPhi));
	min.setPz(m_minE*cos(m_minTheta));
	min.setE(m_minE);
	m_angle=max.angle(min.vect())*180./pai;
	m_m=(max+min).m();
	m_IM=max.invariantMass(min);
	//select signal and sideband to get lum;
	if(m_minE>=boostMinEmin && m_delPhi>dPhiMin && m_delPhi<dPhiMax && m_minE<=boostMinEmax)tot++;
	if(m_minE>=boostMinEmin && m_delPhi>dPhiMinSig && m_delPhi<dPhiMaxSig && m_minE<=boostMinEmax)signal++;

	//boost
	//HepLorentzVector p_cms(0.034067,0.0,0.0,3.097);
	HepLorentzVector boost1=max.boost(-0.011,0,0);
	HepLorentzVector boost2=min.boost(-0.011,0,0);
	m_boostAngle=boost1.angle(boost2.vect())*180./pai;
	m_boostMaxE=boost1.e();
	m_boostMinE=boost2.e();
	m_boostDelPhi=(fabs(boost1.phi()-boost2.phi())-pai)*180./pai;
	m_boostDelTheta=(fabs(boost1.theta()+boost2.theta())-pai)*180./pai;
	m_boostM=(boost1+boost2).m();
	m_boostIM=boost1.invariantMass(boost2);
	log << MSG::INFO << "num Good Photon " << iGam.size()<<endreq;
        if(m_boostMinE>=boostMinEmin && m_boostMinE<=boostMinEmax && fabs(m_boostDelPhi)<=boostPhimin)boost_signal++;
	if(m_boostMinE>=boostMinEmin && m_boostMinE<=boostMinEmax && fabs(m_boostDelPhi)<=boostPhimax)boost_tot++;
	//mcTruth
	if (eventHeader->runNumber()<0)
	{
		SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), "/Event/MC/McParticleCol");
		int m_numParticle = 0;
		if (!mcParticleCol)
		{
			//std::cout << "Could not retrieve McParticelCol" << std::endl;
			return StatusCode::FAILURE;
		}
		else
		{
			bool jpsipDecay = false;
			int rootIndex = -1;
			Event::McParticleCol::iterator iter_mc = mcParticleCol->begin();
			for (; iter_mc != mcParticleCol->end(); iter_mc++)
			{
				if ((*iter_mc)->primaryParticle()) continue;
				if (!(*iter_mc)->decayFromGenerator()) continue;
				
				if ((*iter_mc)->particleProperty()==443) 
				{
					jpsipDecay = true;
					rootIndex = (*iter_mc)->trackIndex();
				}
				if (!jpsipDecay) continue;
				int mcidx = ((*iter_mc)->mother()).trackIndex() - rootIndex;
				int pdgid = (*iter_mc)->particleProperty();
				m_pdgid[m_numParticle] = pdgid;
				m_motheridx[m_numParticle] = mcidx;
				m_numParticle += 1;    
			}
		}
		m_idxmc = m_numParticle;
	}
	N4++;
	m_tuple2->write();
	return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DiGam::finalize() {
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in finalize()" << endmsg;
	//get intLumEndcapEE
	double ssg=0.;
	double lum=0.;
	double boost_ssg=0.;
	double boost_lum=0.;
	if(CrossSection*MCEff==0||CrossSection*MCEffBoost==0){
	   log << MSG::FATAL<<"DiGam Error: cross_section or MC_eff is not correct!"<<endreq;
	   exit(1);
	}
	ssg=(signal-(tot-signal))*1.0;
	//boss 650 mc:7.135% 
	lum=(ssg)/(CrossSection*MCEff);
        //boost LUM
	boost_ssg=(boost_signal-(boost_tot-boost_signal))*1.0;
	//boost 650 mc:7.097%
	boost_lum=(boost_ssg)/(CrossSection*MCEffBoost);

	//std::cout<<"flag = "<<flag<<std::endl;
	std::cout<<"E_cms = "<<E_cms<<std::endl;

	std::cout<<"N0 = "<<N0<<std::endl;
        std::cout<<"minE, phi in:"<<boostMinEmin<<" ~ "<<boostMinEmax<<",   "<<boostPhimin<<" ~ "<<boostPhimax<<std::endl;
	std::cout<<"sigma, eff = "<<CrossSection<<","<<MCEff<<std::endl;
	std::cout<<"sigma, effBoost = "<<CrossSection<<", "<<MCEffBoost<<std::endl;
	std::cout<<"Nsignal, lum, Nsig_boost, boost_lum = "<<ssg<<", "<<lum<<", "<<boost_ssg<<", "<<boost_lum<<std::endl;
	h_lum->SetBinContent(1,lum);
	h_lum->SetBinContent(2,ssg);
	h_lum->SetBinContent(3,boost_lum);
	h_lum->SetBinContent(4,boost_ssg);
	if(m_thsvc->regHist("/DQAHist/zhsLUM/lum", h_lum).isFailure()){
	   log << MSG::FATAL<<"DiGam Error:can't write data to LUM!"<<endreq;
	   exit(1);
	}
	return StatusCode::SUCCESS;
}
