#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ITHistSvc.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"
#include "ParticleID/ParticleID.h"
#include "DQAEvent/DQAEvent.h"
#include "DQA_TOF/DQA_TOF.h"
#include <vector>
/////////////////////////////////////////////////////////////////////////////
DQA_TOF::DQA_TOF(const std::string& name, ISvcLocator* pSvcLocator) :
	Algorithm(name, pSvcLocator) {

		//Declare the properties  
	}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_TOF::initialize(){
	MsgStream log(msgSvc(), name());

	log << MSG::INFO << "in initialize()" << endmsg;
	StatusCode status;

	StatusCode sc=service("THistSvc", m_thsvc);
	if(sc.isFailure()) {
		log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
		return StatusCode::FAILURE;
	}
          
	h_path=new TH1F("h_path","barrel ",200,0,200);
        if(m_thsvc->regHist("/DQAHist/TOF/h_path",h_path).isFailure()){
            log << MSG::ERROR << "Couldn't register h_path" << endreq;
        }
	h_Bzrhit=new TH1F("h_Bzrhit","barrel z hitmap",240,-120,120);
        m_thsvc->regHist("/DQAHist/TOF/h_Bzrhit", h_Bzrhit);
	h_Ezrhit=new TH1F("h_Ezrhit","endcap z hitmap",50,40,90);
        m_thsvc->regHist("/DQAHist/TOF/h_Ezrhit", h_Ezrhit);
	h_ph=new TH1F("h_ph","barrel Q",900,0,9000);
        m_thsvc->regHist("/DQAHist/TOF/h_ph", h_ph);
	
	W_delT=new TH2F("W_delT","barrel west PMT delT",176,0,176,300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/W_delT", W_delT);
	E_delT=new TH2F("E_delT","barrel east PMT delT",176,0,176,300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/E_delT", E_delT);
	counter=new TH2F("counter","barrel counter delT",176,0,176,300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/counter", counter);
	cluster=new TH2F("cluster","barrel cluster delT",88,0,88,300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/cluster", cluster);
	EC_delT=new TH2F("EC_delT","endcap delT",96,0,96,300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/EC_delT", EC_delT);
	Bt_delT=new TH1F("Bt_delT","barrel delT",300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/Bt_delT", Bt_delT);
	Et_delT=new TH1F("Et_delT","endcap delT",300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/Et_delT", Et_delT);
        
	B_path=new TH2F("B_path","barrel flight distance vs z",240,-120,120,200,0.0,200.0);
        m_thsvc->regHist("/DQAHist/TOF/B_path", B_path);
	E_path=new TH2F("E_path","endcap path distance vs z",50,40,90,200,0.0,200.0);
        m_thsvc->regHist("/DQAHist/TOF/E_path", E_path);
	
	delT_z1=new TH2F("delT_z1","barrel east delT vs Z",240,-120,120,300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/delT_z1", delT_z1);
	delT_z2=new TH2F("delT_z2","barrel west delT vs Z",240,-120,120,300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/delT_z2", delT_z2);
	delT_z3=new TH2F("delT_z3","barrel counter delT vs Z",240,-120,120,300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/delT_z3", delT_z3);
	delT_z4=new TH2F("delT_z4","barrel cluster delT vs Z",240,-120,120,300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/delT_z4", delT_z4);
        
	W_delT_Q=new TH2F("W_delT_Q","west barrel delT vs Q",900,0,9000,300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/W_delT_Q", W_delT_Q);
	E_delT_Q=new TH2F("E_delT_Q","east barrel delT vs Q",900,0,9000,300,-1.5,1.5);
        m_thsvc->regHist("/DQAHist/TOF/E_delT_Q", E_delT_Q);
        
	delT_pp=new TH1F("delT_pp","proton delT",300,-1.5,1.5);
	m_thsvc->regHist("/DQAHist/TOF/delT_pp", delT_pp);
	delT_pm=new TH1F("delT_Pm","anti-proton delT",300,-1.5,1.5);
	m_thsvc->regHist("/DQAHist/TOF/delT_pm", delT_pm);
	delT_pi=new TH1F("delT_pi","pi delT",300,-1.5,1.5);
	m_thsvc->regHist("/DQAHist/TOF/delT_pi", delT_pi);
	delT_k=new TH1F("delT_k","k delT",300,-1.5,1.5);
	m_thsvc->regHist("/DQAHist/TOF/delT_k", delT_k);
	log << MSG::INFO << "DQA_TOF successfully return from initialize()" <<endmsg;
	return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_TOF::execute() {
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in execute()" << endreq;

	SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
	int runNo=eventHeader->runNumber();
	int event=eventHeader->eventNumber();
	log<<MSG::DEBUG<<"run,evtnum="<<runNo<<","<<event<<endreq;

	SmartDataPtr<DQAEvent::DQAEvent> dqaevt(eventSvc(), "/Event/DQATag");
	if ( !dqaevt ) {
		log << MSG::ERROR << "Error accessing DQAEvent" << endreq;
		return StatusCode::FAILURE;
	}

	log << MSG::DEBUG << "event tag = " << dqaevt->EventTag() << endreq;

	//if ( !dqaevt->Bhabha() )return StatusCode::SUCCESS; 
        SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);	
	SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);
	if(dqaevt->Bhabha()){
		for(int i = 0; i < evtRecEvent->totalCharged(); i++){
			EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
			//if ( (*itTrk)->partId() != 1 ) continue; // only e+, e-
			if(!(*itTrk)->isElectron())continue;
			if(!(*itTrk)->isTofTrackValid())continue;
			SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();
			SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();
			for(;iter_tof != tofTrkCol.end(); iter_tof++ ) {
				TofHitStatus *status = new TofHitStatus;
				status->setStatus((*iter_tof)->status());
				int tofid=(*iter_tof)->tofID();
				double tof=(*iter_tof)->tof();
				double texpE=(*iter_tof)->texpElectron();
				double path=(*iter_tof)->path();
				double Q=(*iter_tof)->ph();
				double zrhit=(*iter_tof)->zrhit();

				if(status->is_barrel()){
					h_Bzrhit->Fill(zrhit);
					h_path->Fill(path);
					//h_ph->Fill(Q);
					if(status->is_readout()){
						h_ph->Fill(Q);
						B_path->Fill(zrhit,path);
						if(status->is_east()){                    //barrel readout east
							E_delT->Fill(tofid,tof-texpE);
							delT_z1->Fill(zrhit,tof-texpE);
							E_delT_Q->Fill(Q,tof-texpE);
						}
						else {                                    //barrel readout west
							W_delT->Fill(tofid,tof-texpE);
							delT_z2->Fill(zrhit,tof-texpE);
							W_delT_Q->Fill(Q,tof-texpE);
						}
					}                                                 //end readout
					if(!status->is_readout()&&status->is_counter()){  //barrel counter
						counter->Fill(tofid,tof-texpE);
						delT_z3->Fill(zrhit,tof-texpE);
					}
					if(!status->is_counter()&&status->is_cluster()){  //barrel cluster
						cluster->Fill(tofid,tof-texpE);
						Bt_delT->Fill(tof-texpE);
						delT_z4->Fill(zrhit,tof-texpE);
					}
				}
				else{                                                     //endcap
					E_path->Fill(zrhit,path);
					h_Ezrhit->Fill(zrhit);
					EC_delT->Fill(tofid,tof-texpE);
					Et_delT->Fill(tof-texpE);
				}
			}                                                                 // loop 7 info fo tof
		}                                                                         // loop every track
	}
	else{
		for(int i = 0; i < evtRecEvent->totalCharged(); i++){
			EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
			//if ( (*itTrk)->partId() != 1 ) continue; // only e+, e-
			if(!(*itTrk)->isTofTrackValid())continue;
			SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();
			SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();
			for(;iter_tof != tofTrkCol.end(); iter_tof++ ) {
				TofHitStatus *status = new TofHitStatus;
				status->setStatus((*iter_tof)->status());
				if(!status->is_cluster())continue;
				double tof=(*iter_tof)->tof();
				double texpPi=(*iter_tof)->texpPion();
				double texpK=(*iter_tof)->texpKaon();
				double texpP=(*iter_tof)->texpProton();
				//if((*itTrk)->partId()==3){  //pi
				if((*itTrk)->isPion()){
				        double texpPi=(*iter_tof)->texpPion();
                                        delT_pi->Fill(tof-texpPi);
				}
				//else if((*itTrk)->partId()==4){ //k
				else if((*itTrk)->isKaon()){
				        double texpK=(*iter_tof)->texpKaon();
                                        delT_k->Fill(tof-texpK);
				}
				//else if((*itTrk)->partId()==5){ //p
				else if((*itTrk)->isProton()){
				        double texpP=(*iter_tof)->texpProton();
                                        if(!(*itTrk)->isMdcTrackValid())continue;
					RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
					if(mdcTrk->charge()>0)delT_pp->Fill(tof-texpP);
					else delT_pm->Fill(tof-texpP);
				}
			}
		}
	}
	return StatusCode::SUCCESS;

}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode DQA_TOF::finalize() {
	MsgStream log(msgSvc(), name());
	log << MSG::INFO << "in finalize()" << endmsg;

	return StatusCode::SUCCESS;
}


