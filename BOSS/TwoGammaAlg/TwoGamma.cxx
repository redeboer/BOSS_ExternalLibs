#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"

#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"
//#include "EvTimeEvent/RecEsTime.h"

#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;
#include "CLHEP/Vector/LorentzVector.h"
using CLHEP::HepLorentzVector;
#include "CLHEP/Vector/TwoVector.h"
using CLHEP::Hep2Vector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/SecondVertexFit.h" 
#include "ParticleID/ParticleID.h"
#include "TwoGammaAlg/TwoGamma.h"
#include <vector>
#include <fstream>
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;
const double ecms = 3.686;
const double velc = 299.792458;
const double xmass[5] = {0.000511, 0.105658, 0.139570, 0.493677, 0.938272};
const double pai=3.1415926;

////////////////////////////////////////////////////////////////////
TwoGamma::TwoGamma(const std::string& name, ISvcLocator* pSvcLocator) :
	Algorithm(name, pSvcLocator) 
{  
	//  m_tuple1 = 0;
	for(int i = 0; i < 10; i++)
	{ 
		m_pass[i] = 0;
	}
	m_event = 0;

	Ndata1 = 0;
	Ndata2 = 0;
	m_runNo = 0;

	//Declare the properties 
	declareProperty("CmsEnergy", m_ecms = 3.686);

	declareProperty("max1", m_max1 = 1.2);
	declareProperty("max2", m_max2 = 0.8);
	declareProperty("costheta", m_costheta = 0.8);
	declareProperty("dphi1", m_dphi1 = 2.5);
	declareProperty("dphi2", m_dphi2 = 5);
	declareProperty("eff", m_eff = 0.07);
	declareProperty("sec", m_sec = 184.1);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode TwoGamma::initialize()
{
	MsgStream log(msgSvc(), name());

	log << MSG::INFO << "in initialize()" << endmsg;

	StatusCode status;

	NTuplePtr nt1(ntupleSvc(), "FILE1/DiGam");
	if ( nt1 ) m_tuple1 = nt1;
	else 
	{
		m_tuple1 = ntupleSvc()->book ("FILE1/DiGam", CLID_ColumnWiseTuple, "DiGam N-Tuple signal");
		if ( m_tuple1 )
		{
			status = m_tuple1->addItem ("run",  m_run );
			status = m_tuple1->addItem ("rec",  m_rec );
                        status = m_tuple1->addItem ("time",  m_time );

			status = m_tuple1->addItem ("ngood",   m_ngood);
			status = m_tuple1->addItem ("nchrg",    m_nchrg);

			status = m_tuple1->addItem ("Cms_e1",    m_e1);
			status = m_tuple1->addItem ("Cms_e2",    m_e2);
			status = m_tuple1->addItem ("Cms_e",    m_e);
			status = m_tuple1->addItem ("Cms_costheta1",    m_costheta1);
			status = m_tuple1->addItem ("Cms_costheta2",    m_costheta2);
			status = m_tuple1->addItem ("Cms_dltphi",    m_dltphi);
			status = m_tuple1->addItem ("Cms_dltphi_1",    m_dltphi_1);
			status = m_tuple1->addItem ("Cms_dlttheta",    m_dlttheta);
			status = m_tuple1->addItem ("Cms_phi1",    m_phi1);
			status = m_tuple1->addItem ("Cms_phi2",    m_phi2);

			status = m_tuple1->addItem ("Lab_e1",    m_e1_lab);
			status = m_tuple1->addItem ("Lab_e2",    m_e2_lab);
			status = m_tuple1->addItem ("Lab_e",    m_e_lab);
			status = m_tuple1->addItem ("Lab_costheta1",    m_costheta1_lab);
			status = m_tuple1->addItem ("Lab_costheta2",    m_costheta2_lab);
			status = m_tuple1->addItem ("Lab_dltphi",    m_dltphi_lab);
			status = m_tuple1->addItem ("Lab_dlttheta",    m_dlttheta_lab);
			status = m_tuple1->addItem ("Lab_phi1",    m_phi1_lab);
			status = m_tuple1->addItem ("Lab_phi2",    m_phi2_lab);

			status = m_tuple1->addItem ("xBoost",    m_xBoost);
			status = m_tuple1->addItem ("yBoost",    m_yBoost);
			status = m_tuple1->addItem ("zBoost",    m_zBoost);
		}
		else
		{
			log << MSG::ERROR << "Cannot book N-tuple1:"<<long(m_tuple1)<<endmsg;
			return StatusCode::FAILURE;
		}
	}
	Ndata1 = 0;
	Ndata2 = 0;
	m_runNo = 0;
        
        log << MSG::INFO << "successfully return from initialize()" <<endmsg;
        return StatusCode::SUCCESS;
}

//*********************************************************************************************************************
StatusCode TwoGamma::execute()
{
	StatusCode sc=StatusCode::SUCCESS;
	m_event++;

	MsgStream log(msgSvc(),name());
	log<<MSG::INFO<<"in execute()"<<endreq;
	SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
	SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(),EventModel::EvtRec::EvtRecEvent);
	SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),EventModel::EvtRec::EvtRecTrackCol);

	m_run = eventHeader->runNumber();
	m_rec = eventHeader->eventNumber();
	int runNo = m_run;
	int event = m_rec;
        int time = eventHeader->time();
	if(m_event > 1 && runNo != m_runNo) return sc;
	m_runNo = runNo;
        m_time = time;

	if(m_rec%1000==0)cout<<"Run   "<<m_run<<"     Event   "<<m_rec<<endl;

	HepLorentzVector p4psip(0.011*m_ecms, 0.0, 0.005, m_ecms);
	double psipBeta = (p4psip.vect()).mag()/(p4psip.e());

	m_pass[0]+=1;

	int NCharge =  evtRecEvent->totalCharged();
	if(NCharge != 0)return sc;

	m_pass[1]+=1;

	double Emax1=0.0;
	double Emax2=0.0;
	int Imax[2];

	if(((evtRecEvent->totalTracks() - evtRecEvent->totalCharged() )<2) || ((evtRecEvent->totalTracks() - evtRecEvent->totalCharged() )>15)) return sc;
	m_pass[2]+=1;

	HepLorentzVector p4Gam_1;
	HepLorentzVector p4Gam_2;

	for(int i=evtRecEvent->totalCharged(); i<evtRecEvent->totalTracks(); i++)
	{
		EvtRecTrackIterator  itTrk=evtRecTrkCol->begin() + i;
		if(!(*itTrk)->isEmcShowerValid()) continue;
		RecEmcShower *emcTrk = (*itTrk)->emcShower();

		HepLorentzVector p4Gam;
		double Phi = emcTrk->phi();
		double Theta = emcTrk->theta();
		double Ener=emcTrk->energy();

		p4Gam.setPx(Ener * sin(Theta) * cos(Phi));
		p4Gam.setPy(Ener * sin(Theta) * sin(Phi));
		p4Gam.setPz(Ener * cos(Theta));
		p4Gam.setE(Ener);
		p4Gam.boost(-0.011, 0 ,-0.005 * 1.0 / 3.686);

		if(Ener>Emax2)
		{
			Emax2=Ener;
			Imax[1]=i;
			p4Gam_2 = p4Gam;
		}
		if(Ener>Emax1)
		{
			Emax2=Emax1;
			p4Gam_2 = p4Gam_1;
			Imax[1]=Imax[0];
			Emax1=Ener;
			p4Gam_1 = p4Gam;
			Imax[0]=i;
		}
	}
	m_e1_lab = Emax1;
	m_e2_lab = Emax2;
	m_e_lab = Emax1 + Emax2;
	log << MSG::INFO << "Emax1 = " << Emax1 <<"Emax2= "<<Emax2<< endreq;
	if(Emax1 < m_max1 || Emax2 < m_max2) return sc;
	m_pass[3]+=1;

	//P4 in Lab
	double emcphi[2],emctht[2];
	for(int i = 0;i < 2; i++)
	{
		if(i>=evtRecTrkCol->size()) break;
		EvtRecTrackIterator  itTrk=evtRecTrkCol->begin() + Imax[i];
		if(!(*itTrk)->isEmcShowerValid()) continue;
		RecEmcShower *emcTrk = (*itTrk)->emcShower();
		emcphi[i]=emcTrk->phi();
		emctht[i]=emcTrk->theta();
	}
	double dltphi_lab = (fabs(emcphi[0] - emcphi[1]) - pai) * 180.0 / pai;
	double dlttheta_lab = (fabs(emctht[0] + emctht[1]) - pai) * 180.0 / pai;
	m_costheta1_lab = cos(emctht[0]);
	m_costheta2_lab = cos(emctht[1]);
	m_phi1_lab = emcphi[0] * 180.0 / pai;
	m_phi2_lab = emcphi[1] * 180.0 / pai;
	m_dlttheta_lab = dlttheta_lab;
	m_dltphi_lab = dltphi_lab;

	if(fabs(m_costheta1_lab) > m_costheta || fabs(m_costheta2_lab) > m_costheta) return sc;
	m_pass[4]+=1;
	//P4 in Lab

	//P4 in Cms
	double px1 = p4Gam_1.px();
	double py1 = p4Gam_1.py();
	double pz1 = p4Gam_1.pz();
	double pxy1 = sqrt(px1 * px1 + py1 * py1);
	double e1 = p4Gam_1.e();

	double px2 = p4Gam_2.px();
	double py2 = p4Gam_2.py();
	double pz2 = p4Gam_2.pz();
	double pxy2 = sqrt(px2 * px2 + py2 * py2);
	double e2 = p4Gam_2.e();


	double phi1 = 0;
	double phi2 = 0;
	if(atan(py1 * 1.0 / px1) > 0){
		if(px1 > 0)phi1 = atan(py1 * 1.0 / px1);
		else phi1 = -(pai - atan(py1 * 1.0 / px1));
	}
	else{
		if(px1 > 0)phi1 = atan(py1 * 1.0 / px1);
		else phi1 = pai + atan(py1 * 1.0 / px1);
	}

	if(atan(py2 * 1.0 / px2) > 0){
		if(px2 > 0)phi2 = atan(py2 * 1.0 / px2);
		else phi2 = -(pai - atan(py2 * 1.0 / px2));
	}
	else{
		if(px2 > 0)phi2 = atan(py2 * 1.0 / px2);
		else phi2 = pai + atan(py2 * 1.0 / px2);
	}

	double dltphi = (fabs(phi1 - phi2) - pai) * 180.0 / pai;

	double theta1 = 0;
	double theta2 = 0;

	if(pz1 > 0) theta1 = atan(pxy1 * 1.0 / pz1);
	else theta1 = (pai + atan(pxy1 * 1.0 / pz1));

	if(pz2 > 0) theta2 = atan(pxy2 * 1.0 / pz2);
	else theta2 = (pai + atan(pxy2 * 1.0 / pz2));

	double dlttheta = (theta1 + theta2 - pai) * 180.0 / pai;

	double xBoost = p4Gam_1.px() + p4Gam_2.px();
	double yBoost = p4Gam_1.py() + p4Gam_2.py();
	double zBoost = p4Gam_1.pz() + p4Gam_2.pz();
	m_xBoost = xBoost;
	m_yBoost = yBoost;
	m_zBoost = zBoost;

	m_costheta1 = cos(theta1);
	m_costheta2 = cos(theta2);
	m_phi1 = phi1 * 180.0 / pai;
	m_phi2 = phi2 * 180.0 / pai;
	m_dlttheta = dlttheta;
	m_dltphi = dltphi;
	m_e1 = e1;
	m_e2 = e2;
	m_e = e1 + e2;
	//P4 in Cms

	if(fabs(m_dltphi) < m_dphi1) Ndata1++;
	if(fabs(m_dltphi) > m_dphi1 && fabs(m_dltphi) < m_dphi2) Ndata2++;


	m_tuple1->write();
//	runNo = 0;

	return StatusCode::SUCCESS;
}

//*************************************************************************************************************
StatusCode TwoGamma::finalize() 
{
	char head[100];
	char foot[100] = ".txt";
	sprintf(head,"OffLineLum_%04d",m_runNo);
	strcat(head,foot);
	ofstream fout(head,ios::out);
	fout<<"==============================================="<<endl;
	fout<<"DESIGNED For OffLine Luminosity BY 2Gam Events"<<endl<<endl;
	fout<<"MANY THANKS to Prof. C.Z Yuan & Z.Y Wang"<<endl<<endl;
	fout<<"                                        2009/05"<<endl;
	//	fout<<"                               Charmonium Group"<<endl;
	fout<<"==============================================="<<endl<<endl<<endl;

	double lum = (Ndata1 - Ndata2) * 1.0 / (m_eff * m_sec);
	fout<<"                  Table  List                  "<<endl<<endl;
	fout<<"-----------------------------------------------"<<endl;
	fout<<"Firstly Energy Gamma	"<<m_max1<<" Gev"<<endl;
	fout<<"Secondly Energy Gamma	"<<m_max2<<" Gev"<<endl;
	fout<<"Solid Angle Acceptance	"<<m_costheta<<endl;
	fout<<"QED XSection		"<<m_sec<<" NB"<<endl;
	fout<<"Monte Carto Efficiency	"<<m_eff * 100<<"%"<<endl<<endl;
	fout<<"runNo                      Luminosity          "<<endl<<endl;
	fout<<m_runNo<<"                     "<<lum<<" nb^(-1)"<<endl;
	fout<<"-----------------------------------------------"<<endl;
	fout.close();

	MsgStream log(msgSvc(), name());
	cout<< "in finalize()" <<endl;
	cout<< "all events	" <<m_pass[0]<<endl;
	cout<< "NCharged==0	" <<m_pass[1]<<endl;
	cout<< "Number of Gam	" <<m_pass[2]<<endl;
	cout<< "N events	" <<m_pass[3]<<endl;
	cout<< "N events 0.8	" <<m_pass[4]<<endl;
	return StatusCode::SUCCESS;
}

