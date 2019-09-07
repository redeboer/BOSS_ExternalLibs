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
#include "EvtRecEvent/EvtRecDTag.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"

#include "McTruth/McParticle.h"

#include "TH1F.h"
#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Geometry/Point3D.h"

using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "VertexFit/IVertexDbSvc.h"
#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/Helix.h"
#include "ParticleID/ParticleID.h"
#include <vector>

#include "DQADtagAlg/DQADtag.h"
#include "DQADtagAlg/util/MyUtil.h"
#include "DQADtagAlg/util/MyConst.h"

using namespace Event;
//const double mpi = 0.13957;
//const double mk  = 0.493677;
//const double mD0  = 1.86484;
//const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
////const double velc = 29.9792458;  tof_path unit in cm.
//const double velc = 299.792458;   // tof path unit in mm
//typedef std::vector<int> Vint;
//typedef std::vector<HepLorentzVector> Vp4;
const double mpsipp = 3.770;
//double beamE = mpsipp/2;
const HepLorentzVector ecms(0.011*mpsipp,0,0,mpsipp);
const Hep3Vector u_cms = -ecms.boostVector();

int Ncut_DQADTagAlg[10];

/////////////////////////////////////////////////////////////////////////////

DQADtag::DQADtag(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {

	//Declare the properties  
	declareProperty("Vr0cut", m_vr0cut=1.0);
	declareProperty("Vz0cut", m_vz0cut=10.0);
	declareProperty("Vctcut", m_cthcut=0.93);
	declareProperty("mpi0Max", m_mpi0Min =0.1);
	declareProperty("mpi0Min", m_mpi0Max =0.16);
	declareProperty("EnergyThreshold", 	m_energyThreshold=0.04);
	declareProperty("BarrelEmc_th",  		m_Barrel_th    = 0.82);
	declareProperty("EndcapEmc_th_1", 	m_Endcap_th_1  = 0.85);
	declareProperty("EndcapEmc_th_2", 	m_Endcap_th_2  = 0.93);
	declareProperty("EnergyThreshold_b",	m_energyThreshold_b=0.025);
	declareProperty("EnergyThreshold_e",	m_energyThreshold_e=0.05);
	declareProperty("GammaAngCut",		m_gammaAngCut=25.0);
	declareProperty("ReadBeamEFromDB", 	m_ReadBeamEFromDB = false );
	declareProperty("BeamE",		m_beamE = 1.885 );
	declareProperty("CheckDedx", m_checkDedx = 0);
	declareProperty("CheckTof",  m_checkTof = 0);
	declareProperty("CheckVtx", m_checkVtx= 0);
	declareProperty("CheckEvt",  m_checkEvt= 0);
	declareProperty("WriteTuple",  m_writeTuple = 1);
	declareProperty("Debug",     m_debug = 0);
	declareProperty("deltECut",     m_deltECut = 0.03);
	declareProperty("mpi0MaxCut", m_mpi0MinCut =0.117);
	declareProperty("mpi0MinCut", m_mpi0MaxCut =0.145);
	declareProperty("mKsMaxCut",     m_mKsMaxCut = 0.505);
	declareProperty("mKsMinCut",     m_mKsMinCut = 0.49);
	declareProperty("KsDLengthCut",     m_KsDLengthCut = 12.0);
	declareProperty("CheckDecayMode",   m_checkDecayMode = 2);
	declareProperty("ReadVertex",   m_readVertex = true);
	declareProperty("RegHist",   m_regHist = 1);
	m_vtxFit = VertexFit::instance();
	m_2ndVtxFit = SecondVertexFit::instance();
	HepPoint3D vx(0., 0., 0.);
	HepSymMatrix Evx(3, 0);
	double bx = 1E+6;
	double by = 1E+6;
	double bz = 1E+6;
	Evx[0][0] = bx*bx;
	Evx[1][1] = by*by;
	Evx[2][2] = bz*bz;
	m_vxpar.setVx(vx);
	m_vxpar.setEvx(Evx);
	m_privxpar.setVx(vx);
	m_privxpar.setEvx(Evx);
	//log = MsgStream(msgSvc(), name());
	TString strDecayModeName[6] = {"mBC","deltE","p","charm","mass","e"};
	m_decayModeHistName[0] = "mBC"; 
	m_decayModeHistName[1] = "deltE"; 
	m_decayModeHistName[2] = "mass"; 
	for (int i=0;i!=6;i++)
	{
	  m_DecayModeItemName[i] = strDecayModeName[i]; 
	}
	//for (int i=0;i!=6;i++)
	//{
	//cout<<"m_DecayModeItemName:	"<<m_DecayModeItemName[i]<<endl;
	//}
	TString strTrkName[10] = {"px","py","pz","pp","vx","vy","vz","vr0","cos","E"};
	m_TrackItemName[10] = "Id";
	for (int i=0;i!=10;i++)
	{
	  m_trackHistName[i] = strTrkName[i]; 
	  m_TrackItemName[i] = strTrkName[i]; 
	}
	TString strPhotonName[5] = {"numHits","secondmoment","x","y","z"};
	m_PhotonItemName[5] = "Id_Emc";
	for (int i=0;i!=5;i++)
	{
	  m_photonHistName[i] = strPhotonName[i];
	  m_PhotonItemName[i] = strPhotonName[i];
	}
	TString strTrkDedxName[6] = {"chik","chipi","ghit","thit","probPH","normPH"};
	for (int i=0;i!=6;i++)
	{
	  m_trackDedxHistName[i] = strTrkDedxName[i]; 
	  m_TrackDedxItemName[i] = strTrkDedxName[i]; 
	}
	m_TrackDedxItemName[6] = "Id_Dedx";
	TString strTrkTofName[6] = {"br_toff_mu","br_toff_pi","br_toff_k","ec_toff_mu","ec_toff_pi","ec_toff_k" };
	for (int i=0;i!=6;i++)
	{
	  m_trackTofHistName[i] = strTrkTofName[i];
	  m_TrackTofItemName[i] = strTrkTofName[i];
	}
	m_TrackTofItemName[6] = "Id_tof";
	m_TrackTofItemName[7] = "ec_toff_isEast";
	TString strKsName[4] = {"vfitmass","mass","dLength","dLengthErrorRatio"};
	for (int i=0;i!=4;i++)
	{
	  m_KsHistName[i] = strKsName[i]; 
	  m_KsItemName[i] = strKsName[i]; 
	}
	m_KsItemName[4] = "pvchi2";
	m_KsItemName[5] = "svchi2";
	TString strPi0Name[7] = {"pi0mass","photon1_E","photon1_cos","photon2_E","photon2_cos","photon1_Id","photon2_Id"};
	for (int i=0;i!=5;i++)
	{
	  m_Pi0HistName[i] = strPi0Name[i]; 
	  m_Pi0ItemName[i] = strPi0Name[i];
	}
	m_Pi0ItemName[5] = "photon1_Id";
	m_Pi0ItemName[6] = "photon2_Id";

  }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQADtag::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;

  m_irun=-100;

  StatusCode status;

  NTuplePtr nt2(ntupleSvc(), "DQAFILE/CheckVtx");
  if ( nt2 ) m_tuple2 = nt2;
  else {
	m_tuple2 = ntupleSvc()->book ("DQAFILE/CheckVtx", CLID_ColumnWiseTuple, "ks N-Tuple example");
	if ( m_tuple2 )    {
	  status = m_tuple2->addItem ("vtxTrkId",m_vtxTrkId);
	  status = m_tuple2->addItem ("vx0",   m_vx0);
	  status = m_tuple2->addItem ("vy0",   m_vy0);
	  status = m_tuple2->addItem ("vz0",   m_vz0);
	  status = m_tuple2->addItem ("vr0",   m_vr0);
	  status = m_tuple2->addItem ("vct",   m_Vct);
	}
	else    { 
	  log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple2) << endmsg;
	  return StatusCode::FAILURE;
	}
  }

  NTuplePtr nt4(ntupleSvc(), "DQAFILE/Dtag");
  if ( nt4 ) m_tuple4 = nt4;
  else {
	m_tuple4 = ntupleSvc()->book ("DQAFILE/Dtag", CLID_ColumnWiseTuple, "ks N-Tuple example");
	if ( m_tuple4 )    {
	  status = m_tuple4->addItem ("run",    m_run);
	  status = m_tuple4->addItem ("rec",    m_rec); 
	  status = m_tuple4->addItem ("nch",    m_nch);
	  status = m_tuple4->addItem ("nneu",   m_nneu);
	  status = m_tuple4->addItem ("numD0Mode",  m_numD0Mode);
	  status = m_tuple4->addItem ("D0_mBC",		m_D0_mBC);
	  status = m_tuple4->addItem ("D0_deltE",	m_D0_deltE);
	  status = m_tuple4->addItem ("D0_mass",		m_D0_mass);
	  status = m_tuple4->addItem ("numDCMode",  m_numDCMode);
	  status = m_tuple4->addItem ("DC_mBC",		m_DC_mBC);
	  status = m_tuple4->addItem ("DC_deltE",	m_DC_deltE);
	  status = m_tuple4->addItem ("DC_mass",		m_DC_mass);
	  status = m_tuple4->addItem ("numAllMode",   m_numAllMode);
	  status = m_tuple4->addItem ("DtagbeamE",		m_dtagbeamE);
	  status = m_tuple4->addItem ("mode", 			m_mode);
	  status = m_tuple4->addItem ("charge",			m_charge);
	  status = m_tuple4->addItem ("charm", 			m_charm);
	  status = m_tuple4->addItem ("numchildren",		m_numofchildren);
	  status = m_tuple4->addItem ("mass",			m_mass);
	  status = m_tuple4->addItem ("mBC",				m_mBC);
	  status = m_tuple4->addItem ("rflag",				m_rflag);
	  status = m_tuple4->addItem ("deltaE",			m_deltae);
	  status = m_tuple4->addItem ("E",				m_e);
	  status = m_tuple4->addItem ("p",				m_p);
	  status = m_tuple4->addItem ("p_CMS",				m_p_CMS);
	  status = m_tuple4->addItem ("p4_CMS",		4,		m_p4_CMS);
	  status = m_tuple4->addItem ("notherTrk",		m_nothertrk);
	  status = m_tuple4->addItem ("nTrk",			m_ntrk);
	  status = m_tuple4->addItem ("nothershower",	m_nothershower);
	  status = m_tuple4->addItem ("nshower", 		m_nshower);
	  status = m_tuple4->addItem ( "recNumD02KPi",   		m_recNumD02KPi);
	  status = m_tuple4->addItem ("D02KPi_e",   			m_D02KPi_e);
	  status = m_tuple4->addItem ("D02KPi_p",   			m_D02KPi_p);
	  status = m_tuple4->addItem ("D02KPi_mBC",   		m_D02KPi_mBC);
	  status = m_tuple4->addItem ("D02KPi_mass",   		m_D02KPi_mass);
	  status = m_tuple4->addItem ("D02KPi_deltE",   		m_D02KPi_deltE);
	  status = m_tuple4->addItem ("D02KPi_charm",   		m_D02KPi_charm);
	  status = m_tuple4->addItem ("D02KPi_kaon_pp",   	m_D02KPi_kaon_pp);
	  status = m_tuple4->addItem ("D02KPi_pion_pp",   	m_D02KPi_pion_pp);
	  status = m_tuple4->addItem ("D02KPi_kaon_vx",   	m_D02KPi_kaon_vx);
	  status = m_tuple4->addItem ("D02KPi_kaon_vy",   	m_D02KPi_kaon_vy);
	  status = m_tuple4->addItem ("D02KPi_kaon_vz",   	m_D02KPi_kaon_vz);
	  status = m_tuple4->addItem ("D02KPi_kaon_vr0",   	m_D02KPi_kaon_vr0);
	  status = m_tuple4->addItem ("D02KPi_kaon_px",   	m_D02KPi_kaon_px);
	  status = m_tuple4->addItem ("D02KPi_kaon_py",   	m_D02KPi_kaon_py);
	  status = m_tuple4->addItem ("D02KPi_kaon_pz",   	m_D02KPi_kaon_pz);
	  status = m_tuple4->addItem ("D02KPi_kaon_E",   	m_D02KPi_kaon_E);
	  status = m_tuple4->addItem ("D02KPi_kaon_cos",   	m_D02KPi_kaon_cos);
	  status = m_tuple4->addItem ("D02KPi_pion_vx",   	m_D02KPi_pion_vx);
	  status = m_tuple4->addItem ("D02KPi_pion_vy",   	m_D02KPi_pion_vy);
	  status = m_tuple4->addItem ("D02KPi_pion_vz",   	m_D02KPi_pion_vz);
	  status = m_tuple4->addItem ("D02KPi_pion_vr0",   	m_D02KPi_pion_vr0);
	  status = m_tuple4->addItem ("D02KPi_pion_px",   	m_D02KPi_pion_px);
	  status = m_tuple4->addItem ("D02KPi_pion_py",   	m_D02KPi_pion_py);
	  status = m_tuple4->addItem ("D02KPi_pion_pz",   	m_D02KPi_pion_pz);
	  status = m_tuple4->addItem ("D02KPi_pion_E",   	m_D02KPi_pion_E);
	  status = m_tuple4->addItem ("D02KPi_pion_cos",   	m_D02KPi_pion_cos);
	  status = m_tuple4->addItem ("D02KPi_kaon_Id",   	m_D02KPi_kaon_Id);
	  status = m_tuple4->addItem ("D02KPi_pion_Id",   	m_D02KPi_pion_Id);
	  status = m_tuple4->addItem ( "recNumD02KPiPi0",  		m_recNumD02KPiPi0);
	  status = m_tuple4->addItem ("D02KPiPi0_e",  		m_D02KPiPi0_e);
	  status = m_tuple4->addItem ("D02KPiPi0_p",   		m_D02KPiPi0_p);
	  status = m_tuple4->addItem ("D02KPiPi0_mBC",   	m_D02KPiPi0_mBC);
	  status = m_tuple4->addItem ("D02KPiPi0_mass",  	m_D02KPiPi0_mass);
	  status = m_tuple4->addItem ("D02KPiPi0_deltE", 	m_D02KPiPi0_deltE);
	  status = m_tuple4->addItem ("D02KPiPi0_charm", 	m_D02KPiPi0_charm);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_pp", 	m_D02KPiPi0_kaon_pp);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_pp", 	m_D02KPiPi0_pion_pp);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_Id", 	m_D02KPiPi0_kaon_Id);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_Id", 	m_D02KPiPi0_pion_Id);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_vx",  	m_D02KPiPi0_kaon_vx);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_vy",  	m_D02KPiPi0_kaon_vy);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_vz",  	m_D02KPiPi0_kaon_vz);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_vr0", 	m_D02KPiPi0_kaon_vr0);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_px",  	m_D02KPiPi0_kaon_px);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_py",  	m_D02KPiPi0_kaon_py);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_pz",  	m_D02KPiPi0_kaon_pz);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_E",   	m_D02KPiPi0_kaon_E);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_cos", 	m_D02KPiPi0_kaon_cos);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_vx",  	m_D02KPiPi0_pion_vx);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_vy",  	m_D02KPiPi0_pion_vy);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_vz",  	m_D02KPiPi0_pion_vz);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_vr0", 	m_D02KPiPi0_pion_vr0);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_px",  	m_D02KPiPi0_pion_px);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_py",  	m_D02KPiPi0_pion_py);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_pz",  	m_D02KPiPi0_pion_pz);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_E",   	m_D02KPiPi0_pion_E);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_cos",  m_D02KPiPi0_pion_cos);
	  status = m_tuple4->addItem ("D02KPiPi0_pi0mass", 	m_D02KPiPi0_pi0mass);
	  status = m_tuple4->addItem ("D02KPiPi0_photon1_Id", m_D02KPiPi0_photon1_Id);
	  status = m_tuple4->addItem ("D02KPiPi0_photon2_Id", m_D02KPiPi0_photon2_Id);
	  status = m_tuple4->addItem ("D02KPiPi0_photon1_E", 	m_D02KPiPi0_photon1_E);
	  status = m_tuple4->addItem ("D02KPiPi0_photon2_E", 	m_D02KPiPi0_photon2_E);
	  status = m_tuple4->addItem ("D02KPiPi0_photon1_cos", 	m_D02KPiPi0_photon1_cos);
	  status = m_tuple4->addItem ("D02KPiPi0_photon2_cos", 	m_D02KPiPi0_photon2_cos);
	  status = m_tuple4->addItem ( "recNumD02KPiPiPi",			m_recNumD02KPiPiPi);
	  status = m_tuple4->addItem ("D02KPiPiPi_e",			m_D02KPiPiPi_e);
	  status = m_tuple4->addItem ("D02KPiPiPi_p",			m_D02KPiPiPi_p);
	  status = m_tuple4->addItem ("D02KPiPiPi_mBC",			m_D02KPiPiPi_mBC);
	  status = m_tuple4->addItem ("D02KPiPiPi_mass",			m_D02KPiPiPi_mass);
	  status = m_tuple4->addItem ("D02KPiPiPi_deltE",		m_D02KPiPiPi_deltE);
	  status = m_tuple4->addItem ("D02KPiPiPi_charm",		m_D02KPiPiPi_charm);
	  status = m_tuple4->addItem ("D02KPiPiPi_kaon_pp",		m_D02KPiPiPi_kaon_pp);
	  status = m_tuple4->addItem ("D02KPiPiPi_pion1_pp",		m_D02KPiPiPi_pion1_pp);
	  status = m_tuple4->addItem ("D02KPiPiPi_pion2_pp",		m_D02KPiPiPi_pion2_pp);
	  status = m_tuple4->addItem ("D02KPiPiPi_pion3_pp",		m_D02KPiPiPi_pion3_pp);
	  status = m_tuple4->addItem ("D02KPiPiPi_kaon_Id",		m_D02KPiPiPi_kaon_Id);
	  status = m_tuple4->addItem ("D02KPiPiPi_pion1_Id",		m_D02KPiPiPi_pion1_Id);
	  status = m_tuple4->addItem ("D02KPiPiPi_pion2_Id",		m_D02KPiPiPi_pion2_Id);
	  status = m_tuple4->addItem ("D02KPiPiPi_pion3_Id",		m_D02KPiPiPi_pion3_Id);
	  status = m_tuple4->addItem ( "recNumD02KPiPiPiPi0",		m_recNumD02KPiPiPiPi0);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_e",				m_D02KPiPiPiPi0_e);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_p",				m_D02KPiPiPiPi0_p);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_mBC",   		m_D02KPiPiPiPi0_mBC);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_mass",  		m_D02KPiPiPiPi0_mass);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_deltE", 		m_D02KPiPiPiPi0_deltE);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_charm",			m_D02KPiPiPiPi0_charm);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_kaon_pp",		m_D02KPiPiPiPi0_kaon_pp);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_pion1_pp",		m_D02KPiPiPiPi0_pion1_pp);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_pion2_pp",		m_D02KPiPiPiPi0_pion2_pp);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_pion3_pp",		m_D02KPiPiPiPi0_pion3_pp);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_kaon_Id",		m_D02KPiPiPiPi0_kaon_Id);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_pion1_Id",		m_D02KPiPiPiPi0_pion1_Id);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_pion2_Id",		m_D02KPiPiPiPi0_pion2_Id);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_pion3_Id",		m_D02KPiPiPiPi0_pion3_Id);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_pi0mass",		m_D02KPiPiPiPi0_pi0mass);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_photon1_Id",	m_D02KPiPiPiPi0_photon1_Id);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_photon2_Id",	m_D02KPiPiPiPi0_photon2_Id);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_photon1_E",		m_D02KPiPiPiPi0_photon1_E);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_photon2_E",		m_D02KPiPiPiPi0_photon2_E);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_photon1_cos",	m_D02KPiPiPiPi0_photon1_cos);
	  status = m_tuple4->addItem ("D02KPiPiPiPi0_photon2_cos",	m_D02KPiPiPiPi0_photon2_cos);
	  status = m_tuple4->addItem ( "recNumD02KsPiPi",   	m_recNumD02KsPiPi);
	  status = m_tuple4->addItem ("D02KsPiPi_e",				m_D02KsPiPi_e);
	  status = m_tuple4->addItem ("D02KsPiPi_p",				m_D02KsPiPi_p);
	  status = m_tuple4->addItem ("D02KsPiPi_mBC",   		m_D02KsPiPi_mBC);
	  status = m_tuple4->addItem ("D02KsPiPi_mass",  		m_D02KsPiPi_mass);
	  status = m_tuple4->addItem ("D02KsPiPi_deltE", 		m_D02KsPiPi_deltE);
	  status = m_tuple4->addItem ("D02KsPiPi_charm", 		m_D02KsPiPi_charm);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_pp", 		m_D02KsPiPi_pion1_pp);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_pp", 		m_D02KsPiPi_pion2_pp);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_pp", 		m_D02KsPiPi_pion3_pp);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_pp", 		m_D02KsPiPi_pion4_pp);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_Id", 		m_D02KsPiPi_pion1_Id);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_Id", 		m_D02KsPiPi_pion2_Id);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_Id", 		m_D02KsPiPi_pion3_Id);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_Id", 		m_D02KsPiPi_pion4_Id);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_vx",  	m_D02KsPiPi_pion1_vx);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_vy",  	m_D02KsPiPi_pion1_vy);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_vz",  	m_D02KsPiPi_pion1_vz);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_vr0", 	m_D02KsPiPi_pion1_vr0);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_px",  	m_D02KsPiPi_pion1_px);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_py",  	m_D02KsPiPi_pion1_py);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_pz",  	m_D02KsPiPi_pion1_pz);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_pp",		m_D02KsPiPi_pion1_pp);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_E",   	m_D02KsPiPi_pion1_E);
	  status = m_tuple4->addItem ("D02KsPiPi_pion1_cos", 	m_D02KsPiPi_pion1_cos);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_vx",  	m_D02KsPiPi_pion2_vx);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_vy",  	m_D02KsPiPi_pion2_vy);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_vz",  	m_D02KsPiPi_pion2_vz);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_vr0", 	m_D02KsPiPi_pion2_vr0);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_px",  	m_D02KsPiPi_pion2_px);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_py",  	m_D02KsPiPi_pion2_py);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_pz",  	m_D02KsPiPi_pion2_pz);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_pp",		m_D02KsPiPi_pion2_pp);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_E",   	m_D02KsPiPi_pion2_E);
	  status = m_tuple4->addItem ("D02KsPiPi_pion2_cos", 	m_D02KsPiPi_pion2_cos);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_vx",  	m_D02KsPiPi_pion3_vx);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_vy",  	m_D02KsPiPi_pion3_vy);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_vz",  	m_D02KsPiPi_pion3_vz);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_vr0", 	m_D02KsPiPi_pion3_vr0);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_px",  	m_D02KsPiPi_pion3_px);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_py",  	m_D02KsPiPi_pion3_py);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_pz",  	m_D02KsPiPi_pion3_pz);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_pp",		m_D02KsPiPi_pion3_pp);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_E",   	m_D02KsPiPi_pion3_E);
	  status = m_tuple4->addItem ("D02KsPiPi_pion3_cos", 	m_D02KsPiPi_pion3_cos);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_vx",  	m_D02KsPiPi_pion4_vx);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_vy",  	m_D02KsPiPi_pion4_vy);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_vz",  	m_D02KsPiPi_pion4_vz);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_vr0", 	m_D02KsPiPi_pion4_vr0);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_px",  	m_D02KsPiPi_pion4_px);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_py",  	m_D02KsPiPi_pion4_py);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_pz",  	m_D02KsPiPi_pion4_pz);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_pp",		m_D02KsPiPi_pion4_pp);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_E",   	m_D02KsPiPi_pion4_E);
	  status = m_tuple4->addItem ("D02KsPiPi_pion4_cos", 	m_D02KsPiPi_pion4_cos);
	  status = m_tuple4->addItem ("D02KsPiPi_ks_mass", 		m_D02KsPiPi_ks_mass);
	  status = m_tuple4->addItem ("D02KsPiPi_ks_vfitmass", 	m_D02KsPiPi_ks_vfitmass);
	  status = m_tuple4->addItem ("D02KsPiPi_ks_pvchi2", 	m_D02KsPiPi_ks_pvchi2);
	  status = m_tuple4->addItem ("D02KsPiPi_ks_svchi2", 	m_D02KsPiPi_ks_svchi2);
	  status = m_tuple4->addItem ("D02KsPiPi_ks_dLength", 	m_D02KsPiPi_ks_dLength);
	  status = m_tuple4->addItem ("D02KsPiPi_ks_dLengthErrorRatio", 		m_D02KsPiPi_ks_dLengthErrorRatio);
	  status = m_tuple4->addItem ( "recNumD02KsPiPiPi0",		m_recNumD02KsPiPiPi0);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_e",				m_D02KsPiPiPi0_e);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_p",				m_D02KsPiPiPi0_p);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_mBC",			m_D02KsPiPiPi0_mBC);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_mass",			m_D02KsPiPiPi0_mass);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_deltE",			m_D02KsPiPiPi0_deltE);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_charm",			m_D02KsPiPiPi0_charm);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_pion1_pp",		m_D02KsPiPiPi0_pion1_pp);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_pion2_pp",		m_D02KsPiPiPi0_pion2_pp);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_pion3_pp",		m_D02KsPiPiPi0_pion3_pp);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_pion4_pp",		m_D02KsPiPiPi0_pion4_pp);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_pion1_Id",		m_D02KsPiPiPi0_pion1_Id);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_pion2_Id",		m_D02KsPiPiPi0_pion2_Id);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_pion3_Id",		m_D02KsPiPiPi0_pion3_Id);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_pion4_Id",		m_D02KsPiPiPi0_pion4_Id);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_pi0mass",		m_D02KsPiPiPi0_pi0mass);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_photon1_Id",		m_D02KsPiPiPi0_photon1_Id);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_photon2_Id",		m_D02KsPiPiPi0_photon2_Id);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_photon1_E",		m_D02KsPiPiPi0_photon1_E);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_photon2_E",		m_D02KsPiPiPi0_photon2_E);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_photon1_cos",	m_D02KsPiPiPi0_photon1_cos);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_photon2_cos",	m_D02KsPiPiPi0_photon2_cos);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_ks_mass", 		m_D02KsPiPiPi0_ks_mass);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_ks_vfitmass", 	m_D02KsPiPiPi0_ks_vfitmass);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_ks_pvchi2", 		m_D02KsPiPiPi0_ks_pvchi2);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_ks_svchi2", 		m_D02KsPiPiPi0_ks_svchi2);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_ks_dLength", 	m_D02KsPiPiPi0_ks_dLength);
	  status = m_tuple4->addItem ("D02KsPiPiPi0_ks_dLengthErrorRatio", 		m_D02KsPiPiPi0_ks_dLengthErrorRatio);
	  status = m_tuple4->addItem ( "recNumDC2KPiPi",			m_recNumDC2KPiPi);
	  status = m_tuple4->addItem ("DC2KPiPi_e",   			m_DC2KPiPi_e);
	  status = m_tuple4->addItem ("DC2KPiPi_p",   			m_DC2KPiPi_p);
	  status = m_tuple4->addItem ("DC2KPiPi_mBC",   			m_DC2KPiPi_mBC);
	  status = m_tuple4->addItem ("DC2KPiPi_mass",   		m_DC2KPiPi_mass);
	  status = m_tuple4->addItem ("DC2KPiPi_deltE",   		m_DC2KPiPi_deltE);
	  status = m_tuple4->addItem ("DC2KPiPi_charge",  		m_DC2KPiPi_charge);
	  status = m_tuple4->addItem ("DC2KPiPi_charm",   		m_DC2KPiPi_charm);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_pp",   		m_DC2KPiPi_kaon_pp);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_pp",   	m_DC2KPiPi_pion1_pp);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_pp",   	m_DC2KPiPi_pion2_pp);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_Id",   		m_DC2KPiPi_kaon_Id);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_Id",   	m_DC2KPiPi_pion1_Id);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_Id",   	m_DC2KPiPi_pion2_Id);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_vx",   		m_DC2KPiPi_kaon_vx);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_vy",   		m_DC2KPiPi_kaon_vy);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_vz",   		m_DC2KPiPi_kaon_vz);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_vr0",  		m_DC2KPiPi_kaon_vr0);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_px",   		m_DC2KPiPi_kaon_px);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_py",   		m_DC2KPiPi_kaon_py);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_pz",   		m_DC2KPiPi_kaon_pz);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_E",   		m_DC2KPiPi_kaon_E);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_cos",  		m_DC2KPiPi_kaon_cos);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_vx",  		m_DC2KPiPi_pion1_vx);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_vy",  		m_DC2KPiPi_pion1_vy);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_vz",  		m_DC2KPiPi_pion1_vz);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_vr0", 		m_DC2KPiPi_pion1_vr0);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_px",  		m_DC2KPiPi_pion1_px);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_py",  		m_DC2KPiPi_pion1_py);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_pz",  		m_DC2KPiPi_pion1_pz);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_E",   		m_DC2KPiPi_pion1_E);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_cos", 		m_DC2KPiPi_pion1_cos);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_vx",  		m_DC2KPiPi_pion2_vx);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_vy",  		m_DC2KPiPi_pion2_vy);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_vz",  		m_DC2KPiPi_pion2_vz);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_vr0", 		m_DC2KPiPi_pion2_vr0);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_px",  		m_DC2KPiPi_pion2_px);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_py",  		m_DC2KPiPi_pion2_py);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_pz",  		m_DC2KPiPi_pion2_pz);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_E",   		m_DC2KPiPi_pion2_E);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_cos", 		m_DC2KPiPi_pion2_cos);
	  status = m_tuple4->addItem ( "recNumDC2KPiPiPi0",			m_recNumDC2KPiPiPi0);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_e",   			m_DC2KPiPiPi0_e);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_p",   			m_DC2KPiPiPi0_p);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_mBC",   			m_DC2KPiPiPi0_mBC);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_mass",   			m_DC2KPiPiPi0_mass);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_deltE",  			m_DC2KPiPiPi0_deltE);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_charge", 			m_DC2KPiPiPi0_charge);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_charm",  			m_DC2KPiPiPi0_charm);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_kaon_pp",  		m_DC2KPiPiPi0_kaon_pp);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion1_pp",		m_DC2KPiPiPi0_pion1_pp);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion2_pp",		m_DC2KPiPiPi0_pion2_pp);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_kaon_Id", 		m_DC2KPiPiPi0_kaon_Id);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion1_Id",		m_DC2KPiPiPi0_pion1_Id);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion2_Id",		m_DC2KPiPiPi0_pion2_Id);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_kaon_vx",   		m_DC2KPiPiPi0_kaon_vx);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_kaon_vy",   		m_DC2KPiPiPi0_kaon_vy);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_kaon_vz",   		m_DC2KPiPiPi0_kaon_vz);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_kaon_vr0",  		m_DC2KPiPiPi0_kaon_vr0);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_kaon_px",   		m_DC2KPiPiPi0_kaon_px);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_kaon_py",   		m_DC2KPiPiPi0_kaon_py);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_kaon_pz",   		m_DC2KPiPiPi0_kaon_pz);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_kaon_E",   		m_DC2KPiPiPi0_kaon_E);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_kaon_cos",  		m_DC2KPiPiPi0_kaon_cos);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion1_vx",  		m_DC2KPiPiPi0_pion1_vx);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion1_vy",  		m_DC2KPiPiPi0_pion1_vy);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion1_vz",  		m_DC2KPiPiPi0_pion1_vz);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion1_vr0", 		m_DC2KPiPiPi0_pion1_vr0);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion1_px",  		m_DC2KPiPiPi0_pion1_px);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion1_py",  		m_DC2KPiPiPi0_pion1_py);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion1_pz",  		m_DC2KPiPiPi0_pion1_pz);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion1_E",   		m_DC2KPiPiPi0_pion1_E);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion1_cos", 		m_DC2KPiPiPi0_pion1_cos);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion2_vx",  		m_DC2KPiPiPi0_pion2_vx);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion2_vy",  		m_DC2KPiPiPi0_pion2_vy);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion2_vz",  		m_DC2KPiPiPi0_pion2_vz);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion2_vr0", 		m_DC2KPiPiPi0_pion2_vr0);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion2_px",  		m_DC2KPiPiPi0_pion2_px);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion2_py",  		m_DC2KPiPiPi0_pion2_py);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion2_pz",  		m_DC2KPiPiPi0_pion2_pz);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion2_E",   		m_DC2KPiPiPi0_pion2_E);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pion2_cos", 		m_DC2KPiPiPi0_pion2_cos);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_pi0mass",			m_DC2KPiPiPi0_pi0mass);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_photon1_Id",		m_DC2KPiPiPi0_photon1_Id);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_photon2_Id",		m_DC2KPiPiPi0_photon2_Id);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_photon1_E",		m_DC2KPiPiPi0_photon1_E);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_photon2_E",		m_DC2KPiPiPi0_photon2_E);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_photon1_cos",		m_DC2KPiPiPi0_photon1_cos);
	  status = m_tuple4->addItem ("DC2KPiPiPi0_photon2_cos",		m_DC2KPiPiPi0_photon2_cos);
	  status = m_tuple4->addItem ( "recNumDC2KsPiPi0",		m_recNumDC2KsPiPi0);
	  status = m_tuple4->addItem ("DC2KsPiPi0_e",				m_DC2KsPiPi0_e);
	  status = m_tuple4->addItem ("DC2KsPiPi0_p",				m_DC2KsPiPi0_p);
	  status = m_tuple4->addItem ("DC2KsPiPi0_mBC",				m_DC2KsPiPi0_mBC);
	  status = m_tuple4->addItem ("DC2KsPiPi0_mass",				m_DC2KsPiPi0_mass);
	  status = m_tuple4->addItem ("DC2KsPiPi0_deltE",			m_DC2KsPiPi0_deltE);
	  status = m_tuple4->addItem ("DC2KsPiPi0_charge",			m_DC2KsPiPi0_charge);
	  status = m_tuple4->addItem ("DC2KsPiPi0_charm",			m_DC2KsPiPi0_charm);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion1_pp",			m_DC2KsPiPi0_pion1_pp);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion2_pp",			m_DC2KsPiPi0_pion2_pp);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion3_pp",			m_DC2KsPiPi0_pion3_pp);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion1_Id",			m_DC2KsPiPi0_pion1_Id);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion2_Id",			m_DC2KsPiPi0_pion2_Id);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion3_Id",			m_DC2KsPiPi0_pion3_Id);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion1_vx",  		m_DC2KsPiPi0_pion1_vx);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion1_vy",  		m_DC2KsPiPi0_pion1_vy);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion1_vz",  		m_DC2KsPiPi0_pion1_vz);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion1_vr0",   		m_DC2KsPiPi0_pion1_vr0);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion1_px",  		m_DC2KsPiPi0_pion1_px);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion1_py",  		m_DC2KsPiPi0_pion1_py);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion1_pz",  		m_DC2KsPiPi0_pion1_pz);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion1_E",   		m_DC2KsPiPi0_pion1_E);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion1_cos", 		m_DC2KsPiPi0_pion1_cos);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion2_vx",  		m_DC2KsPiPi0_pion2_vx);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion2_vy",  		m_DC2KsPiPi0_pion2_vy);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion2_vz",  		m_DC2KsPiPi0_pion2_vz);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion2_vr0",   		m_DC2KsPiPi0_pion2_vr0);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion2_px",  		m_DC2KsPiPi0_pion2_px);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion2_py",  		m_DC2KsPiPi0_pion2_py);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion2_pz",  		m_DC2KsPiPi0_pion2_pz);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion2_E",   		m_DC2KsPiPi0_pion2_E);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion2_cos", 		m_DC2KsPiPi0_pion2_cos);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion3_vx",  		m_DC2KsPiPi0_pion3_vx);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion3_vy",  		m_DC2KsPiPi0_pion3_vy);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion3_vz",  		m_DC2KsPiPi0_pion3_vz);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion3_vr0",   		m_DC2KsPiPi0_pion3_vr0);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion3_px",  		m_DC2KsPiPi0_pion3_px);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion3_py",  		m_DC2KsPiPi0_pion3_py);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion3_pz",  		m_DC2KsPiPi0_pion3_pz);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion3_E",   		m_DC2KsPiPi0_pion3_E);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pion3_cos", 		m_DC2KsPiPi0_pion3_cos);
	  status = m_tuple4->addItem ("DC2KsPiPi0_pi0mass",			m_DC2KsPiPi0_pi0mass);
	  status = m_tuple4->addItem ("DC2KsPiPi0_photon1_Id",		m_DC2KsPiPi0_photon1_Id);
	  status = m_tuple4->addItem ("DC2KsPiPi0_photon2_Id",		m_DC2KsPiPi0_photon2_Id);
	  status = m_tuple4->addItem ("DC2KsPiPi0_photon1_E",		m_DC2KsPiPi0_photon1_E);
	  status = m_tuple4->addItem ("DC2KsPiPi0_photon2_E",		m_DC2KsPiPi0_photon2_E);
	  status = m_tuple4->addItem ("DC2KsPiPi0_photon1_cos",		m_DC2KsPiPi0_photon1_cos);
	  status = m_tuple4->addItem ("DC2KsPiPi0_photon2_cos",		m_DC2KsPiPi0_photon2_cos);
	  status = m_tuple4->addItem ("DC2KsPiPi0_ks_mass", 			m_DC2KsPiPi0_ks_mass);
	  status = m_tuple4->addItem ("DC2KsPiPi0_ks_vfitmass", 		m_DC2KsPiPi0_ks_vfitmass);
	  status = m_tuple4->addItem ("DC2KsPiPi0_ks_pvchi2",		m_DC2KsPiPi0_ks_pvchi2);
	  status = m_tuple4->addItem ("DC2KsPiPi0_ks_svchi2",		m_DC2KsPiPi0_ks_svchi2);
	  status = m_tuple4->addItem ("DC2KsPiPi0_ks_dLength", 		m_DC2KsPiPi0_ks_dLength);
	  status = m_tuple4->addItem ("DC2KsPiPi0_ks_dLengthErrorRatio", 	m_DC2KsPiPi0_ks_dLengthErrorRatio);
	  status = m_tuple4->addItem ( "recNumDC2KsPiPiPi",			m_recNumDC2KsPiPiPi);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_e",				m_DC2KsPiPiPi_e);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_p",				m_DC2KsPiPiPi_p);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_mBC",				m_DC2KsPiPiPi_mBC);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_mass",			m_DC2KsPiPiPi_mass);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_deltE",			m_DC2KsPiPiPi_deltE);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_charge",			m_DC2KsPiPiPi_charge);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_charm",			m_DC2KsPiPiPi_charm);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_pion1_pp",		m_DC2KsPiPiPi_pion1_pp);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_pion2_pp",		m_DC2KsPiPiPi_pion2_pp);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_pion3_pp",		m_DC2KsPiPiPi_pion3_pp);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_pion4_pp",		m_DC2KsPiPiPi_pion4_pp);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_pion5_pp",		m_DC2KsPiPiPi_pion5_pp);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_pion1_Id",		m_DC2KsPiPiPi_pion1_Id);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_pion2_Id",		m_DC2KsPiPiPi_pion2_Id);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_pion3_Id",		m_DC2KsPiPiPi_pion3_Id);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_pion4_Id",		m_DC2KsPiPiPi_pion4_Id);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_pion5_Id",		m_DC2KsPiPiPi_pion5_Id);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_ks_mass", 		m_DC2KsPiPiPi_ks_mass);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_ks_vfitmass", 	m_DC2KsPiPiPi_ks_vfitmass);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_ks_pvchi2",		m_DC2KsPiPiPi_ks_pvchi2);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_ks_svchi2",		m_DC2KsPiPiPi_ks_svchi2);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_ks_dLength", 		m_DC2KsPiPiPi_ks_dLength);
	  status = m_tuple4->addItem ("DC2KsPiPiPi_ks_dLengthErrorRatio", 	m_DC2KsPiPiPi_ks_dLengthErrorRatio);
	  status = m_tuple4->addItem ( "recNumDC2KKPi",   			m_recNumDC2KKPi);
	  status = m_tuple4->addItem ("DC2KKPi_e",   		m_DC2KKPi_e);
	  status = m_tuple4->addItem ("DC2KKPi_p",   		m_DC2KKPi_p);
	  status = m_tuple4->addItem ("DC2KKPi_mBC",   		m_DC2KKPi_mBC);
	  status = m_tuple4->addItem ("DC2KKPi_mass",   		m_DC2KKPi_mass);
	  status = m_tuple4->addItem ("DC2KKPi_deltE",   	m_DC2KKPi_deltE);
	  status = m_tuple4->addItem ("DC2KKPi_charge",  	m_DC2KKPi_charge);
	  status = m_tuple4->addItem ("DC2KKPi_charm",   	m_DC2KKPi_charm);
	  status = m_tuple4->addItem ("DC2KKPi_kaon1_pp",   	m_DC2KKPi_kaon1_pp);
	  status = m_tuple4->addItem ("DC2KKPi_kaon2_pp",   	m_DC2KKPi_kaon2_pp);
	  status = m_tuple4->addItem ("DC2KKPi_pion_pp",   	m_DC2KKPi_pion_pp);
	  status = m_tuple4->addItem ("DC2KKPi_kaon1_Id",   	m_DC2KKPi_kaon1_Id);
	  status = m_tuple4->addItem ("DC2KKPi_kaon2_Id",   	m_DC2KKPi_kaon2_Id);
	  status = m_tuple4->addItem ("DC2KKPi_pion_Id",   	m_DC2KKPi_pion_Id);
	  status = m_tuple4->addItem ("D02KPi_kaon_Id_Dedx",			m_D02KPi_kaon_Id_Dedx); 
	  status = m_tuple4->addItem ("D02KPi_pion_Id_Dedx",			m_D02KPi_pion_Id_Dedx); 
	  status = m_tuple4->addItem ("D02KPi_kaon_chipi",			m_D02KPi_kaon_chipi); 
	  status = m_tuple4->addItem ("D02KPi_kaon_chik",			m_D02KPi_kaon_chik); 
	  status = m_tuple4->addItem ("D02KPi_kaon_ghit",			m_D02KPi_kaon_ghit);
	  status = m_tuple4->addItem ("D02KPi_kaon_thit",			m_D02KPi_kaon_thit);
	  status = m_tuple4->addItem ("D02KPi_kaon_probPH",			m_D02KPi_kaon_probPH);
	  status = m_tuple4->addItem ("D02KPi_kaon_normPH",			m_D02KPi_kaon_normPH);
	  status = m_tuple4->addItem ("D02KPi_pion_chipi",			m_D02KPi_pion_chipi);
	  status = m_tuple4->addItem ("D02KPi_pion_chik",			m_D02KPi_pion_chik);
	  status = m_tuple4->addItem ("D02KPi_pion_ghit",			m_D02KPi_pion_ghit);
	  status = m_tuple4->addItem ("D02KPi_pion_thit",			m_D02KPi_pion_thit);
	  status = m_tuple4->addItem ("D02KPi_pion_probPH",			m_D02KPi_pion_probPH);
	  status = m_tuple4->addItem ("D02KPi_pion_normPH",			m_D02KPi_pion_normPH);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_Id_Dedx",		m_D02KPiPi0_kaon_Id_Dedx); 
	  status = m_tuple4->addItem ("D02KPiPi0_pion_Id_Dedx",		m_D02KPiPi0_pion_Id_Dedx); 
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_chipi",		m_D02KPiPi0_kaon_chipi); 
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_chik",			m_D02KPiPi0_kaon_chik); 
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_ghit",			m_D02KPiPi0_kaon_ghit);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_thit",			m_D02KPiPi0_kaon_thit);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_probPH",		m_D02KPiPi0_kaon_probPH);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_normPH",		m_D02KPiPi0_kaon_normPH);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_chipi",		m_D02KPiPi0_pion_chipi);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_chik",			m_D02KPiPi0_pion_chik);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_ghit",			m_D02KPiPi0_pion_ghit);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_thit",			m_D02KPiPi0_pion_thit);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_probPH",		m_D02KPiPi0_pion_probPH);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_normPH",		m_D02KPiPi0_pion_normPH);
	  status = m_tuple4->addItem ("D02KPi_kaon_Id_tof",			m_D02KPi_kaon_Id_tof); 
	  status = m_tuple4->addItem ("D02KPi_pion_Id_tof",			m_D02KPi_pion_Id_tof); 
	  status = m_tuple4->addItem ("D02KPi_kaon_ec_toff_mu",		m_D02KPi_kaon_ec_toff_mu); 
	  status = m_tuple4->addItem ("D02KPi_kaon_ec_toff_pi",		m_D02KPi_kaon_ec_toff_pi); 
	  status = m_tuple4->addItem ("D02KPi_kaon_ec_toff_k",		m_D02KPi_kaon_ec_toff_k); 
	  status = m_tuple4->addItem ("D02KPi_kaon_ec_toff_isEast",		m_D02KPi_kaon_ec_toff_isEast); 
	  status = m_tuple4->addItem ("D02KPi_kaon_br_toff_mu",		7,m_D02KPi_kaon_br_toff_mu);
	  status = m_tuple4->addItem ("D02KPi_kaon_br_toff_pi",		7,m_D02KPi_kaon_br_toff_pi);
	  status = m_tuple4->addItem ("D02KPi_kaon_br_toff_k",		7,m_D02KPi_kaon_br_toff_k);
	  status = m_tuple4->addItem ("D02KPi_pion_ec_toff_pi",		m_D02KPi_pion_ec_toff_mu); 
	  status = m_tuple4->addItem ("D02KPi_pion_ec_toff_mu",		m_D02KPi_pion_ec_toff_pi); 
	  status = m_tuple4->addItem ("D02KPi_pion_ec_toff_k",		m_D02KPi_pion_ec_toff_k); 
	  status = m_tuple4->addItem ("D02KPi_pion_ec_toff_isEast",		m_D02KPi_pion_ec_toff_isEast); 
	  status = m_tuple4->addItem ("D02KPi_pion_br_toff_mu",		7,m_D02KPi_pion_br_toff_mu);
	  status = m_tuple4->addItem ("D02KPi_pion_br_toff_pi",		7,m_D02KPi_pion_br_toff_pi);
	  status = m_tuple4->addItem ("D02KPi_pion_br_toff_k",		7,m_D02KPi_pion_br_toff_k);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_Id_tof",		m_D02KPiPi0_kaon_Id_tof); 
	  status = m_tuple4->addItem ("D02KPiPi0_pion_Id_tof",		m_D02KPiPi0_pion_Id_tof); 
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_ec_toff_mu",		m_D02KPiPi0_kaon_ec_toff_mu); 
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_ec_toff_pi",		m_D02KPiPi0_kaon_ec_toff_pi); 
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_ec_toff_k",		m_D02KPiPi0_kaon_ec_toff_k); 
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_ec_toff_isEast",		m_D02KPiPi0_kaon_ec_toff_isEast); 
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_br_toff_mu",		7,m_D02KPiPi0_kaon_br_toff_mu);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_br_toff_pi",		7,m_D02KPiPi0_kaon_br_toff_pi);
	  status = m_tuple4->addItem ("D02KPiPi0_kaon_br_toff_k",		7,m_D02KPiPi0_kaon_br_toff_k);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_ec_toff_mu",		m_D02KPiPi0_pion_ec_toff_mu); 
	  status = m_tuple4->addItem ("D02KPiPi0_pion_ec_toff_pi",		m_D02KPiPi0_pion_ec_toff_pi); 
	  status = m_tuple4->addItem ("D02KPiPi0_pion_ec_toff_k",		m_D02KPiPi0_pion_ec_toff_k); 
	  status = m_tuple4->addItem ("D02KPiPi0_pion_ec_toff_isEast",		m_D02KPiPi0_pion_ec_toff_isEast); 
	  status = m_tuple4->addItem ("D02KPiPi0_pion_br_toff_mu",		7,m_D02KPiPi0_pion_br_toff_mu);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_br_toff_pi",		7,m_D02KPiPi0_pion_br_toff_pi);
	  status = m_tuple4->addItem ("D02KPiPi0_pion_br_toff_k",		7,m_D02KPiPi0_pion_br_toff_k);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_Id_tof",				m_DC2KPiPi_kaon_Id_tof); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_Id_tof",				m_DC2KPiPi_pion1_Id_tof); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_Id_tof",				m_DC2KPiPi_pion2_Id_tof); 
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_ec_toff_mu",			m_DC2KPiPi_kaon_ec_toff_mu); 
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_ec_toff_pi",			m_DC2KPiPi_kaon_ec_toff_pi); 
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_ec_toff_k",			m_DC2KPiPi_kaon_ec_toff_k); 
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_ec_toff_isEast",		m_DC2KPiPi_kaon_ec_toff_isEast); 
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_br_toff_mu",		7,	m_DC2KPiPi_kaon_br_toff_mu);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_br_toff_pi",		7,	m_DC2KPiPi_kaon_br_toff_pi);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_br_toff_k",		7,	m_DC2KPiPi_kaon_br_toff_k);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_ec_toff_mu",			m_DC2KPiPi_pion1_ec_toff_mu); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_ec_toff_pi",			m_DC2KPiPi_pion1_ec_toff_pi); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_ec_toff_k",			m_DC2KPiPi_pion1_ec_toff_k); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_ec_toff_isEast",		m_DC2KPiPi_pion1_ec_toff_isEast); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_br_toff_mu",		7,	m_DC2KPiPi_pion1_br_toff_mu);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_br_toff_pi",		7,	m_DC2KPiPi_pion1_br_toff_pi);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_br_toff_k",		7,	m_DC2KPiPi_pion1_br_toff_k);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_ec_toff_mu",			m_DC2KPiPi_pion2_ec_toff_mu); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_ec_toff_pi",			m_DC2KPiPi_pion2_ec_toff_pi); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_ec_toff_k",			m_DC2KPiPi_pion2_ec_toff_k); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_ec_toff_isEast",		m_DC2KPiPi_pion2_ec_toff_isEast); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_br_toff_mu",		7,	m_DC2KPiPi_pion2_br_toff_mu);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_br_toff_pi",		7,	m_DC2KPiPi_pion2_br_toff_pi);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_br_toff_k",		7,	m_DC2KPiPi_pion2_br_toff_k);
	  status = m_tuple4->addItem ("D02KPiPi0_photon1_Id_Emc",		m_D02KPiPi0_photon1_Id_Emc);
	  status = m_tuple4->addItem ("D02KPiPi0_photon1_numHits",		m_D02KPiPi0_photon1_numHits);
	  status = m_tuple4->addItem ("D02KPiPi0_photon1_secondmoment",	m_D02KPiPi0_photon1_secondmoment);
	  status = m_tuple4->addItem ("D02KPiPi0_photon1_x",				m_D02KPiPi0_photon1_x);
	  status = m_tuple4->addItem ("D02KPiPi0_photon1_y",				m_D02KPiPi0_photon1_y);
	  status = m_tuple4->addItem ("D02KPiPi0_photon1_z",				m_D02KPiPi0_photon1_z);
	  status = m_tuple4->addItem ("D02KPiPi0_photon2_Id_Emc",		m_D02KPiPi0_photon2_Id_Emc);
	  status = m_tuple4->addItem ("D02KPiPi0_photon2_numHits",		m_D02KPiPi0_photon2_numHits);
	  status = m_tuple4->addItem ("D02KPiPi0_photon2_secondmoment",	m_D02KPiPi0_photon2_secondmoment);
	  status = m_tuple4->addItem ("D02KPiPi0_photon2_x",				m_D02KPiPi0_photon2_x);
	  status = m_tuple4->addItem ("D02KPiPi0_photon2_y",				m_D02KPiPi0_photon2_y);
	  status = m_tuple4->addItem ("D02KPiPi0_photon2_z",				m_D02KPiPi0_photon2_z);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_chipi",			m_DC2KPiPi_kaon_chipi); 
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_chik",				m_DC2KPiPi_kaon_chik); 
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_ghit",				m_DC2KPiPi_kaon_ghit);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_thit",				m_DC2KPiPi_kaon_thit);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_probPH",			m_DC2KPiPi_kaon_probPH);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_normPH",			m_DC2KPiPi_kaon_normPH);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_chipi",			m_DC2KPiPi_pion1_chipi);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_chik",			m_DC2KPiPi_pion1_chik);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_ghit",			m_DC2KPiPi_pion1_ghit);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_thit",			m_DC2KPiPi_pion1_thit);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_probPH",			m_DC2KPiPi_pion1_probPH);
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_normPH",			m_DC2KPiPi_pion1_normPH);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_chipi",			m_DC2KPiPi_pion2_chipi);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_chik",			m_DC2KPiPi_pion2_chik);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_ghit",			m_DC2KPiPi_pion2_ghit);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_thit",			m_DC2KPiPi_pion2_thit);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_probPH",			m_DC2KPiPi_pion2_probPH);
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_normPH",			m_DC2KPiPi_pion2_normPH);
	  status = m_tuple4->addItem ("DC2KPiPi_kaon_Id_Dedx",			m_DC2KPiPi_kaon_Id_Dedx); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion1_Id_Dedx",			m_DC2KPiPi_pion1_Id_Dedx); 
	  status = m_tuple4->addItem ("DC2KPiPi_pion2_Id_Dedx",			m_DC2KPiPi_pion2_Id_Dedx); 
	}
	else    { 
	  log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple4) << endmsg;
	  return StatusCode::FAILURE;
	}
  }

  NTuplePtr nt5(ntupleSvc(), "DQAFILE/Events");
  if ( nt5 ) m_tuple5 = nt5;
  else {
	m_tuple5 = ntupleSvc()->book ("DQAFILE/Events", CLID_ColumnWiseTuple, "ks N-Tuple example");
	if ( m_tuple5 )    {
	  status = m_tuple5->addItem ("evtrun",    m_evtrun);
	  status = m_tuple5->addItem ("evtrec",    m_evtrec); 
	  status = m_tuple5->addItem ("evtnch",    m_evtnch);
	  status = m_tuple5->addItem ("evtnneu",   m_evtnneu);
	  status = m_tuple5->addItem ("evtbeamE",   m_evtbeamE);
	}
	else    { 
	  log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple5) << endmsg;
	  return StatusCode::FAILURE;
	}
  }
  NTuplePtr nt11(ntupleSvc(), "DQAFILE/CheckDedxAll");
  if ( nt11 ) m_tuple11 = nt11;
  else {
	m_tuple11 = ntupleSvc()->book ("DQAFILE/CheckDedxAll", CLID_ColumnWiseTuple, "ks N-Tuple example");
	if ( m_tuple11 )    {
	  status = m_tuple11->addItem ("ngch",  m_ngch, 0, 10000);
	  status = m_tuple11->addIndexedItem ("trkiddedx",  m_ngch, m_trkiddedx);
	  status = m_tuple11->addIndexedItem ("mptrk",  m_ngch, m_ptrk);
	  status = m_tuple11->addIndexedItem ("chie",   m_ngch, m_chie);
	  status = m_tuple11->addIndexedItem ("chimu",  m_ngch,m_chimu);
	  status = m_tuple11->addIndexedItem ("chipi",  m_ngch,m_chipi);
	  status = m_tuple11->addIndexedItem ("chik",   m_ngch,m_chik);
	  status = m_tuple11->addIndexedItem ("chip",   m_ngch,m_chip);
	  status = m_tuple11->addIndexedItem ("probPH", m_ngch,m_probPH);
	  status = m_tuple11->addIndexedItem ("normPH", m_ngch,m_normPH);
	  status = m_tuple11->addIndexedItem ("ghit",   m_ngch,m_ghit);
	  status = m_tuple11->addIndexedItem ("thit",   m_ngch,m_thit);
	}
	else    { 
	  log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple11) << endmsg;
	  return StatusCode::FAILURE;
	}
  }

  NTuplePtr nt12(ntupleSvc(), "DQAFILE/CheckTofChAll");
  if ( nt12 ) m_tuple12 = nt12;
  else {
	m_tuple12 = ntupleSvc()->book ("DQAFILE/CheckTofChAll", CLID_ColumnWiseTuple, "ks N-Tuple example");
	if ( m_tuple12 )    {
	  status = m_tuple12->addItem ("ngch2",  m_ngch2, 0, 10000);
	  status = m_tuple12->addIndexedItem ("trkidtof",  m_ngch2,m_trkidtof);
	  status = m_tuple12->addIndexedItem ("ptot_etof", m_ngch2,m_ptot_etof);
	  status = m_tuple12->addIndexedItem ("cntr_etof", m_ngch2,m_cntr_etof);
	  status = m_tuple12->addIndexedItem ("te_etof",   m_ngch2,m_te_etof);
	  status = m_tuple12->addIndexedItem ("tmu_etof",  m_ngch2,m_tmu_etof);
	  status = m_tuple12->addIndexedItem ("tpi_etof",  m_ngch2,m_tpi_etof);
	  status = m_tuple12->addIndexedItem ("tk_etof",   m_ngch2,m_tk_etof);
	  status = m_tuple12->addIndexedItem ("tp_etof",   m_ngch2,m_tp_etof);
	  status = m_tuple12->addIndexedItem ("ph_etof",   m_ngch2,m_ph_etof);
	  status = m_tuple12->addIndexedItem ("rhit_etof", m_ngch2,m_rhit_etof);
	  status = m_tuple12->addIndexedItem ("qual_etof", m_ngch2,m_qual_etof);
	  status = m_tuple12->addIndexedItem ("ec_toff_e", m_ngch2,m_ec_toff_e);
	  status = m_tuple12->addIndexedItem ("ec_toff_mu",m_ngch2,m_ec_toff_mu);
	  status = m_tuple12->addIndexedItem ("ec_toff_pi",m_ngch2,m_ec_toff_pi);
	  status = m_tuple12->addIndexedItem ("ec_toff_k", m_ngch2,m_ec_toff_k);
	  status = m_tuple12->addIndexedItem ("ec_toff_p", m_ngch2,m_ec_toff_p);
	  status = m_tuple12->addIndexedItem ("ec_tsig_e", m_ngch2,m_ec_tsig_e);
	  status = m_tuple12->addIndexedItem ("ec_tsig_mu",m_ngch2,m_ec_tsig_mu);
	  status = m_tuple12->addIndexedItem ("ec_tsig_pi",m_ngch2,m_ec_tsig_pi);
	  status = m_tuple12->addIndexedItem ("ec_tsig_k", m_ngch2,m_ec_tsig_k);
	  status = m_tuple12->addIndexedItem ("ec_tsig_p", m_ngch2,m_ec_tsig_p);
	  status = m_tuple12->addIndexedItem ("ec_tof",    m_ngch2,m_ec_tof);
	  status = m_tuple12->addIndexedItem ("ptot_btof1",m_ngch2,m_ptot_btof1);
	  status = m_tuple12->addIndexedItem ("cntr_btof1",m_ngch2,m_cntr_btof1);
	  status = m_tuple12->addIndexedItem ("te_btof1",  m_ngch2,m_te_btof1);
	  status = m_tuple12->addIndexedItem ("tmu_btof1", m_ngch2,m_tmu_btof1);
	  status = m_tuple12->addIndexedItem ("tpi_btof1", m_ngch2,m_tpi_btof1);
	  status = m_tuple12->addIndexedItem ("tk_btof1",  m_ngch2,m_tk_btof1);
	  status = m_tuple12->addIndexedItem ("tp_btof1",  m_ngch2,m_tp_btof1);
	  status = m_tuple12->addIndexedItem ("ph_btof1",  m_ngch2,m_ph_btof1);
	  status = m_tuple12->addIndexedItem ("zhit_btof1",m_ngch2,m_zhit_btof1);
	  status = m_tuple12->addIndexedItem ("qual_btof1",m_ngch2,m_qual_btof1);
	  status = m_tuple12->addIndexedItem ("b1_toff_e", m_ngch2,m_b1_toff_e);
	  status = m_tuple12->addIndexedItem ("b1_toff_mu",m_ngch2,m_b1_toff_mu);
	  status = m_tuple12->addIndexedItem ("b1_toff_pi",m_ngch2,m_b1_toff_pi);
	  status = m_tuple12->addIndexedItem ("b1_toff_k", m_ngch2,m_b1_toff_k);
	  status = m_tuple12->addIndexedItem ("b1_toff_p", m_ngch2,m_b1_toff_p);
	  status = m_tuple12->addIndexedItem ("b1_tsig_e", m_ngch2,m_b1_tsig_e);
	  status = m_tuple12->addIndexedItem ("b1_tsig_mu",m_ngch2,m_b1_tsig_mu);
	  status = m_tuple12->addIndexedItem ("b1_tsig_pi",m_ngch2,m_b1_tsig_pi);
	  status = m_tuple12->addIndexedItem ("b1_tsig_k", m_ngch2,m_b1_tsig_k);
	  status = m_tuple12->addIndexedItem ("b1_tsig_p", m_ngch2,m_b1_tsig_p);
	  status = m_tuple12->addIndexedItem ("b1_tof",    m_ngch2,m_b1_tof);                                                    

	  status = m_tuple12->addIndexedItem ("mdedx_pid", m_ngch2,m_dedx_pid);
	  status = m_tuple12->addIndexedItem ("mtof1_pid", m_ngch2,m_tof1_pid);
	  status = m_tuple12->addIndexedItem ("mtof2_pid", m_ngch2,m_tof2_pid);
	  status = m_tuple12->addIndexedItem ("mprob_pid", m_ngch2,m_prob_pid);
	  status = m_tuple12->addIndexedItem ("mptrk_pid", m_ngch2,m_ptrk_pid);
	  status = m_tuple12->addIndexedItem ("mcost_pid", m_ngch2,m_cost_pid);
	}
	else    { 
	  log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple12) << endmsg;
	  return StatusCode::FAILURE;
	}
  }

  NTuplePtr nt13(ntupleSvc(), "DQAFILE/CheckTofNeAll");
  if ( nt13 ) m_tuple13 = nt13;
  else {
	m_tuple13 = ntupleSvc()->book ("DQAFILE/CheckTofNeAll", CLID_ColumnWiseTuple, "ks N-Tuple example");
	if ( m_tuple13 )    {
	  status = m_tuple13->addItem ("ngneu",  m_nggneu, 0, 10000);
	  status = m_tuple13->addIndexedItem ("shwidtof", 	m_nggneu,m_shwidtof);
	  status = m_tuple13->addIndexedItem ("numHits",      m_nggneu,m_numHits);    // Total number of hits
	  status = m_tuple13->addIndexedItem ("secondmoment", m_nggneu,m_secondmoment);
	  status = m_tuple13->addIndexedItem ("mx",           m_nggneu,m_x);       //  Shower coordinates and errors
	  status = m_tuple13->addIndexedItem ("my",           m_nggneu,m_y);
	  status = m_tuple13->addIndexedItem ("mz",           m_nggneu,m_z);
	  status = m_tuple13->addIndexedItem ("cosemc",       m_nggneu,m_cosemc);   // Shower Counter angles and errors
	  status = m_tuple13->addIndexedItem ("phiemc",       m_nggneu,m_phiemc);
	  status = m_tuple13->addIndexedItem ("energy",       m_nggneu,m_energy);  // Total energy observed in Emc
	  status = m_tuple13->addIndexedItem ("eseed",        m_nggneu,m_eSeed);
	  status = m_tuple13->addIndexedItem ("me9",          m_nggneu,m_e3x3); 
	  status = m_tuple13->addIndexedItem ("me25",         m_nggneu,m_e5x5); 
	  status = m_tuple13->addIndexedItem ("mlat",         m_nggneu,m_lat);
	  status = m_tuple13->addIndexedItem ("ma20",         m_nggneu,m_a20);
	  status = m_tuple13->addIndexedItem ("ma42",         m_nggneu,m_a42);  
	}
	else    { 
	  log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple13) << endmsg;
	  return StatusCode::FAILURE;
	}
  }

  if(service("THistSvc", m_thsvc).isFailure()) {
	log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
	return StatusCode::FAILURE;
  }

  string strdecaymode[11] = {"D02KPi","D02KPiPi0","D02KPiPiPi","D02KPiPiPiPi0","D02KsPiPi","D02KsPiPiPi0",\
	"DC2KPiPi","DC2KPiPiPi0","DC2KsPiPi0","DC2KsPiPiPi","DC2KKPi"};
  for (int i=0;i!=11;i++)
  {
	regDecayModeHistogram(m_thsvc,strdecaymode[i]);
  }
  string strDdecaymode[2] = {"D0","DC"};
  regDecayModeHistogram(m_thsvc,strDdecaymode[0]);
  regDecayModeHistogram(m_thsvc,strDdecaymode[1]);
  string strtrack[14] = {"D02KPi_kaon","D02KPi_pion","D02KPiPi0_kaon","D02KPiPi0_pion","D02KsPiPi_pion1",\
	"D02KsPiPi_pion2","D02KsPiPi_pion3","D02KsPiPi_pion4","DC2KPiPi_kaon","DC2KPiPi_pion1","DC2KPiPi_pion2",\
	  "DC2KsPiPi0_pion1","DC2KsPiPi0_pion2","DC2KsPiPi0_pion3" };
  for (int i=0;i!=14;i++)
  {
	regTrackHistogram(m_thsvc,strtrack[i]);
  }
  string strphoton[2] = {"D02KPiPi0_photon1","D02KPiPi0_photon2"};
  regPhotonHistogram(m_thsvc,strphoton[0]);
  regPhotonHistogram(m_thsvc,strphoton[1]);
	  string strtrackdedx[7] = {"D02KPi_kaon","D02KPi_pion","D02KPiPi0_kaon","D02KPiPi0_pion","DC2KPiPi_kaon","DC2KPiPi_pion1","DC2KPiPi_pion2"};
  for (int i=0;i!=7;i++)
  {
	regTrackDedxHistogram(m_thsvc,strtrackdedx[i]);
  }
  string strtracktof[7] = {"D02KPi_kaon","D02KPi_pion","D02KPiPi0_kaon","D02KPiPi0_pion", "DC2KPiPi_kaon", "DC2KPiPi_pion1", "DC2KPiPi_pion2"};
  for (int i=0;i!=7;i++)
  {
	regTrackTofHistogram(m_thsvc,strtracktof[i]);
  }
  string strpi0[2] = {"D02KPiPi0","DC2KsPiPi0"};
  for (int i=0;i!=2;i++)
  {
	regPi0Histogram(m_thsvc,strpi0[i]);
  }
  string strks[2] = {"D02KsPiPi","DC2KsPiPi0"};
  for (int i=0;i!=2;i++)
  {
	regKsHistogram(m_thsvc,strks[i]);
  }

  StatusCode sc = service("BestDTagSvc", m_bestDSvc);
  if ( sc.isFailure() ){
	log << MSG::FATAL << "Could not load BestDTagSvc!" << endreq;
	return sc;
  }
  //
  //--------end of book--------
  //

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQADtag::execute() {

  //  std::cout << "execute()" << std::endl;

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  setFilterPassed(false);

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  int run=eventHeader->runNumber();
  int event=eventHeader->eventNumber();
  log << MSG::DEBUG <<"run, evtnum = "
	<< run << " , "
	<< event <<endreq;

  m_run = eventHeader->runNumber();  
  m_rec = eventHeader->eventNumber();
  m_evtrun = eventHeader->runNumber();  
  m_evtrec = eventHeader->eventNumber();

  //////////////////
  // get beam energy
  //////////////////
  if(m_ReadBeamEFromDB&&m_irun!=run){ 
	m_irun=run;
	if(m_readDb.isRunValid(m_irun))   
	  m_beamE=m_readDb.getbeamE(m_irun);
  }

  double beamE=m_beamE;
  m_evtbeamE = m_beamE;

  if (m_debug) 
	cout<<"event "<<event<<endl;
  Ncut_DQADTagAlg[0]++;
  if (Ncut_DQADTagAlg[0]%10000 == 0)
  {
	cout<<"Ncut_DQADTagAlg[0]:	"<<Ncut_DQADTagAlg[0]<<endl;
  }
  //if (Ncut_DQADTagAlg[0]<120000)
  //return StatusCode::SUCCESS;
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  log << MSG::INFO << "get event tag OK" << endreq;
  log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
	<< evtRecEvent->totalCharged() << " , "
	<< evtRecEvent->totalNeutral() << " , "
	<< evtRecEvent->totalTracks() <<endreq;

  m_nch  = evtRecEvent->totalCharged();
  m_nneu = evtRecEvent->totalNeutral();
  m_evtnch  = evtRecEvent->totalCharged();
  m_evtnneu = evtRecEvent->totalNeutral();

  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);

  //get primay vetex from db
  Hep3Vector xorigin(0,0,0);
  IVertexDbSvc*  vtxsvc;
  HepPoint3D vx(0., 0., 0.);
  HepSymMatrix Evx(3, 0);
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if (vtxsvc->isVertexValid()) {
	double* vertex = vtxsvc->PrimaryVertex();     //vertex[0] = vx; vertex[1]= vy; vertex[2] = vz;
	double* vertexsigma = vtxsvc->SigmaPrimaryVertex();
	xorigin.setX(vertex[0]);
	xorigin.setY(vertex[1]);
	xorigin.setZ(vertex[2]);
	vx[0] = vertex[0];
	vx[1] = vertex[1];
	vx[2] = vertex[2];
	Evx[0][0] = vertexsigma[0]*vertexsigma[0];
	Evx[1][1] = vertexsigma[1]*vertexsigma[1];
	Evx[2][2] = vertexsigma[2]*vertexsigma[2];
	m_privxpar.setVx(vx);
	m_privxpar.setEvx(Evx);
  }

  //  Vp4 pDtagCand_cms;
  //  pDtagCand_cms.clear();
  //  int numDtagCand = 0;
  //  vector<double> vmBC_D0, vmDCand_D0, vdeltE_D0, vpDCand_D0, vmpi0, vvmks, vmksp4,\
  //	vvlks, vmBC_DC, vmDCand_DC, vdeltE_DC, vpDCand_DC;
  //  vmBC_D0.clear();
  //  vmDCand_D0.clear();
  //  vdeltE_D0.clear();
  //  vpDCand_D0.clear();
  //  vmBC_DC.clear();
  //  vmDCand_DC.clear();
  //  vdeltE_DC.clear();
  //  vpDCand_DC.clear();
  //  vmpi0.clear();
  //  vvmks.clear();
  //  vmksp4.clear();
  //  vvlks.clear();

  SmartDataPtr<EvtRecDTagCol> evtRecDTagCol(eventSvc(), EventModel::EvtRec::EvtRecDTagCol);
  if ( ! evtRecDTagCol ) {
	cout << "Could not find EvtRecDTagCol" << endl;
	return StatusCode::SUCCESS;
  }

  EvtRecDTag* bestDTag = NULL;
  bestDTag = m_bestDSvc->getSingleTag(-1);
  if (bestDTag == NULL )
	return StatusCode::SUCCESS;
  Ncut_DQADTagAlg[1]++;

  m_rflag = m_bestDSvc->isCosmicOrLepton();
  SmartRefVector<EvtRecTrack> tracks=(bestDTag)->tracks();
  SmartRefVector<EvtRecTrack> othertracks=(bestDTag)->otherTracks();
  SmartRefVector<EvtRecTrack> othershowers=(bestDTag)->otherShowers();
  SmartRefVector<EvtRecTrack> showers=(bestDTag)->showers();
  //  m_bestD_mode=(bestDTag)->decayMode();
  //  m_bestD_p=(bestDTag)->p4().rho();
  //  m_bestD_charm=(bestDTag)->charm();
  //  m_bestD_mass=(bestDTag)->mass();
  double g_mBC=(bestDTag)->mBC();
  double g_deltE=(bestDTag)->deltaE();
  //  int DMode =(int)(bestDTag)->decayMode();

  //use util tool to get dtaglist

  int nCharge = 0;

  const int numMode = 11;
  int indexD[numMode], indexTof[numMode], indexDedx[numMode],numDCand[numMode];
  for (int i=0;i!=numMode;i++) 
  {
	indexD[i] = 0;
	indexTof[i] = 0;
	indexDedx[i] = 0;
	numDCand[i] = 0;
  }
  int indexDAll = 0;
  int indexD0All = 0;
  int indexDCAll = 0;

  //  for (DTagToolIterator bestDTag=iter_begin; bestDTag != iter_end; bestDTag++)
  {
	bool iswritten = 1;
	//decay mode name

	// ***********************************************************************
	// m_checkDecayMode == 0 : allDecayMode		donot check pi0 Ks
	// m_checkDecayMode == 1 : singleDecayMode  D0 to KPi
	// m_checkDecayMode == 2 : fewDecayMode		10 decay modes  check pi0 Ks	
	// ***********************************************************************
	if (m_checkDecayMode < 3)		
	{
	  //	general variables
	  //HepLorentzVector p4=(bestDTag)->p4();
	  //p4.boost(-0.011,0,0);

	  //Hep3Vector p3=p4.v();
	  //double deltaEDCand = (bestDTag)->deltaE();
	  //double mBCDCand =(bestDTag)->mBC();  
	  //if(fabs(deltaEDCand)>=0.1) continue;
	  //if(mBCDCand<=1.8 || mBCDCand>=2.0) continue;
	  m_dtagbeamE  = m_beamE;
	  m_mode =(bestDTag)->decayMode();
	  m_charge =(bestDTag)->charge();
	  m_charm =(bestDTag)->charm();
	  m_numofchildren =(bestDTag)->numOfChildren();
	  m_mass =(bestDTag)->mass();
	  m_mBC =(bestDTag)->mBC();
	  m_e =(bestDTag)->beamE();
	  HepLorentzVector p4D =(bestDTag)->p4();
	  m_p = p4D.rho();
	  p4D.boost(-0.011,0,0);
	  m_p_CMS = p4D.rho();
	  fill(m_p4_CMS, p4D);
	  m_deltae =(bestDTag)->deltaE();
	  //	  pDtagCand_cms.push_back((bestDTag)->p4());	
	  SmartRefVector<EvtRecTrack> tracks=(bestDTag)->tracks();
	  SmartRefVector<EvtRecTrack> othertracks=(bestDTag)->otherTracks();
	  SmartRefVector<EvtRecTrack> othershowers=(bestDTag)->otherShowers();
	  SmartRefVector<EvtRecTrack> showers=(bestDTag)->showers();
	  //HepLorentzVector p4=(bestDTag)->p4();
	  //p4.boost(-0.011,0,0);
	  m_nshower  = showers.size();
	  m_ntrk =tracks.size();
	  m_nothertrk =othertracks.size();
	  m_nothershower  = othershowers.size();

	  //	  if (m_checkDecayMode == 0 ) continue;
	  //	end general variables
	  indexDAll++;
	  if((bestDTag)->decayMode()==EvtRecDTag::kD0toKPi) {
		const int MODE = 0;
		fillDecayModeItem(bestDTag,"D02KPi" );
		fillTrackItem(tracks[0],"D02KPi_kaon",3 ,xorigin);
		fillTrackItem(tracks[1],"D02KPi_pion",2 ,xorigin);
		indexD[MODE]++;
		if (m_checkDedx)
		{
		  //if (!tracks[0]->isMdcDedxValid()) continue;
		  //if (!tracks[1]->isMdcDedxValid()) continue;
		  fillTrackDedxItem(tracks[0],"D02KPi_kaon",3 );
		  fillTrackDedxItem(tracks[1],"D02KPi_pion",2 );
		}
		if (m_checkTof)
		{
		  //if(!tracks[0]->isTofTrackValid()) continue;
		  //if(!tracks[1]->isTofTrackValid()) continue;
		  fillTrackTofItem(tracks[0],"D02KPi_kaon",3 );
		  fillTrackTofItem(tracks[1],"D02KPi_pion",2 );
		}
	  } //end of D0 to K Pi mode
	  //	  if (m_checkDecayMode == 1 ) continue;
	  if((bestDTag)->decayMode()==EvtRecDTag::kD0toKPiPi0) {
		const int MODE = 1;
		//pi0 mass
		DQAPi0Info pi0info;
		pi0info.setchilds(showers[0],showers[1]);
		if (pi0info.calculate())
		{
		  if (pi0info.m() >m_mpi0Max || pi0info.m() < m_mpi0Min) 
			iswritten = 0;
		  //		vmpi0.push_back(pi0info.m());
		  fillPi0Item(showers,"D02KPiPi0" ,&pi0info );
		  fillDecayModeItem(bestDTag,"D02KPiPi0" );
		  fillTrackItem(tracks[0],"D02KPiPi0_kaon",3 ,xorigin);
		  fillTrackItem(tracks[1],"D02KPiPi0_pion",2 ,xorigin);
		  fillPhotonItem(showers[0],"D02KPiPi0_photon1" );
		  fillPhotonItem(showers[1],"D02KPiPi0_photon2" );
		  indexD[MODE]++;
		  if (m_checkDedx)
		  {
			//if (!tracks[0]->isMdcDedxValid()) continue;
			//if (!tracks[1]->isMdcDedxValid()) continue;
			fillTrackDedxItem(tracks[0],"D02KPiPi0_kaon",3 );
			fillTrackDedxItem(tracks[1],"D02KPiPi0_pion",2 );
		  }
		  if (m_checkTof)
		  {
			//if(!tracks[0]->isTofTrackValid()) continue;
			//if(!tracks[1]->isTofTrackValid()) continue;
			fillTrackTofItem(tracks[0],"D02KPiPi0_kaon",3 );
			fillTrackTofItem(tracks[1],"D02KPiPi0_pion",2 );
		  }
		}
	  }// end of D0 to KPiPi0

	  //****************************************************
	  //mode D0 to KPiPiPi only
	  //****************************************************
	  if((bestDTag)->decayMode()==EvtRecDTag::kD0toKPiPiPi) {
		const int MODE = 2;
		fillDecayModeItem(bestDTag,"D02KPiPiPi" );

		RecMdcKalTrack* mdcKalTrk1 = tracks[0]->mdcKalTrack();
		RecMdcKalTrack* mdcKalTrk2 = tracks[1]->mdcKalTrack();
		RecMdcKalTrack* mdcKalTrk3 = tracks[2]->mdcKalTrack();
		RecMdcKalTrack* mdcKalTrk4 = tracks[3]->mdcKalTrack();
		RecMdcKalTrack::setPidType  (RecMdcKalTrack::kaon);
		HepLorentzVector p4kaon = mdcKalTrk1->p4(xmass[3]);
		RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
		HepLorentzVector p4pion1 = mdcKalTrk2->p4(xmass[2]);
		HepLorentzVector p4pion2 = mdcKalTrk3->p4(xmass[2]);
		HepLorentzVector p4pion3 = mdcKalTrk4->p4(xmass[2]);
		m_D02KPiPiPi_kaon_pp  = p4kaon.rho();
		m_D02KPiPiPi_pion1_pp  = p4pion1.rho();
		m_D02KPiPiPi_pion2_pp  = p4pion2.rho();
		m_D02KPiPiPi_pion3_pp  = p4pion3.rho();
		m_D02KPiPiPi_kaon_Id  = tracks[0]->trackId(); 
		m_D02KPiPiPi_pion1_Id  = tracks[1]->trackId();
		m_D02KPiPiPi_pion2_Id  = tracks[2]->trackId();
		m_D02KPiPiPi_pion3_Id  = tracks[3]->trackId();
		indexD[MODE]++;
	  }
	  //****************************************************
	  //mode D0 to KPiPiPiPi0 only
	  //****************************************************
	  if((bestDTag)->decayMode()==EvtRecDTag::kD0toKPiPiPiPi0) {
		const int MODE = 3;
		DQAPi0Info pi0info;
		pi0info.setchilds(showers[0],showers[1]);
		if (pi0info.calculate()) {
		  if (pi0info.m() >m_mpi0Max || pi0info.m() < m_mpi0Min) 
			iswritten = 0;
		  fillPi0Item(showers,"D02KPiPiPiPi0" ,&pi0info );
		  //		vmpi0.push_back(pi0info.m());

		  fillDecayModeItem(bestDTag,"D02KPiPiPiPi0" );

		  RecMdcKalTrack* mdcKalTrk1 = tracks[0]->mdcKalTrack();
		  RecMdcKalTrack* mdcKalTrk2 = tracks[1]->mdcKalTrack();
		  RecMdcKalTrack* mdcKalTrk3 = tracks[2]->mdcKalTrack();
		  RecMdcKalTrack* mdcKalTrk4 = tracks[3]->mdcKalTrack();
		  RecMdcKalTrack::setPidType  (RecMdcKalTrack::kaon);
		  HepLorentzVector p4kaon = mdcKalTrk1->p4(xmass[3]);
		  RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
		  HepLorentzVector p4pion1 = mdcKalTrk2->p4(xmass[2]);
		  HepLorentzVector p4pion2 = mdcKalTrk3->p4(xmass[2]);
		  HepLorentzVector p4pion3 = mdcKalTrk4->p4(xmass[2]);
		  m_D02KPiPiPiPi0_kaon_pp   = p4kaon.rho();
		  m_D02KPiPiPiPi0_pion1_pp  = p4pion1.rho();
		  m_D02KPiPiPiPi0_pion2_pp  = p4pion2.rho();
		  m_D02KPiPiPiPi0_pion3_pp  = p4pion3.rho();
		  m_D02KPiPiPiPi0_kaon_Id  = tracks[0]->trackId(); 
		  m_D02KPiPiPiPi0_pion1_Id  = tracks[1]->trackId();
		  m_D02KPiPiPiPi0_pion2_Id  = tracks[2]->trackId();
		  m_D02KPiPiPiPi0_pion3_Id  = tracks[3]->trackId();
		  indexD[MODE]++;
		}
	  }
	  //****************************************************
	  //mode D0 to KsPiPi only
	  //****************************************************
	  if((bestDTag)->decayMode()==EvtRecDTag::kD0toKsPiPi) {
		const int MODE = 4;
		DQAKsInfo ksinfo;
		ksinfo.setchilds(tracks[0],tracks[1]);
		ksinfo.setVtxPar(m_vxpar);
		ksinfo.setPrimaryVtxPar(m_privxpar);
		if (ksinfo.calculate()) {

		  fillDecayModeItem(bestDTag,"D02KsPiPi" );
		  fillTrackItem(tracks[0],"D02KsPiPi_pion1",2 ,xorigin);
		  fillTrackItem(tracks[1],"D02KsPiPi_pion2",2 ,xorigin);
		  fillTrackItem(tracks[2],"D02KsPiPi_pion3",2 ,xorigin);
		  fillTrackItem(tracks[3],"D02KsPiPi_pion4",2 ,xorigin);
		  fillKsItem(tracks,"D02KsPiPi" ,&ksinfo);
		  indexD[MODE]++;

		  //		if (iswritten) vvmks.push_back(ksinfo.m());
		  //		if (iswritten) vmksp4.push_back(ksinfo.mksp4());
		  //		if (iswritten) vvlks.push_back(ksinfo.decayLengthRatio());
		}
	  }// end of D0 to KsPiPi
	  //****************************************************
	  //mode D0 to KsPiPiPi0 only
	  //****************************************************
	  if((bestDTag)->decayMode()==EvtRecDTag::kD0toKsPiPiPi0) {
		const int MODE = 5;
		DQAKsInfo ksinfo;
		ksinfo.setchilds(tracks[0],tracks[1]);
		ksinfo.setVtxPar(m_vxpar);
		ksinfo.setPrimaryVtxPar(m_privxpar);
		if (ksinfo.calculate()) {
		  DQAPi0Info pi0info;
		  pi0info.setchilds(showers[0],showers[1]);
		  if (pi0info.calculate()) {
			if (pi0info.m() >m_mpi0Max || pi0info.m() < m_mpi0Min) 
			  iswritten = 0;
			fillPi0Item(showers,"D02KsPiPiPi0" ,&pi0info );
			//		vmpi0.push_back(pi0info.m());
			fillKsItem(tracks,"D02KsPiPiPi0" ,&ksinfo);
			//		if (iswritten) vvmks.push_back(ksinfo.m());
			//		if (iswritten) vmksp4.push_back(ksinfo.mksp4());
			//		if (iswritten) vvlks.push_back(ksinfo.decayLengthRatio());
			fillDecayModeItem(bestDTag,"D02KsPiPiPi0" );

			RecMdcKalTrack* mdcKalTrk1 = tracks[0]->mdcKalTrack();
			RecMdcKalTrack* mdcKalTrk2 = tracks[1]->mdcKalTrack();
			RecMdcKalTrack* mdcKalTrk3 = tracks[2]->mdcKalTrack();
			RecMdcKalTrack* mdcKalTrk4 = tracks[3]->mdcKalTrack();
			RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
			HepLorentzVector p4pion1 = mdcKalTrk1->p4(xmass[2]);
			HepLorentzVector p4pion2 = mdcKalTrk2->p4(xmass[2]);
			HepLorentzVector p4pion3 = mdcKalTrk3->p4(xmass[2]);
			HepLorentzVector p4pion4 = mdcKalTrk4->p4(xmass[2]);
			m_D02KsPiPiPi0_pion1_pp  = p4pion1.rho();
			m_D02KsPiPiPi0_pion2_pp  = p4pion2.rho();
			m_D02KsPiPiPi0_pion3_pp  = p4pion3.rho();
			m_D02KsPiPiPi0_pion4_pp  = p4pion4.rho();
			m_D02KsPiPiPi0_pion1_Id  = tracks[0]->trackId();
			m_D02KsPiPiPi0_pion2_Id  = tracks[1]->trackId();
			m_D02KsPiPiPi0_pion3_Id  = tracks[2]->trackId();
			m_D02KsPiPiPi0_pion4_Id  = tracks[3]->trackId();
			indexD[MODE]++;
		  }
		}
	  }// end of D0 to KsPiPiPi0
	  //****************************************************
	  //mode Dp to KPiPi only
	  //****************************************************
	  if((bestDTag)->decayMode()==EvtRecDTag::kDptoKPiPi) {
		const int MODE = 6;
		fillDecayModeItem(bestDTag,"DC2KPiPi" );
		fillTrackItem(tracks[0],"DC2KPiPi_kaon",3 ,xorigin);
		fillTrackItem(tracks[1],"DC2KPiPi_pion1",2 ,xorigin);
		fillTrackItem(tracks[2],"DC2KPiPi_pion2",2 ,xorigin);
		if (m_checkTof)
		{
		  //if(!tracks[0]->isTofTrackValid()) continue;
		  //if(!tracks[1]->isTofTrackValid()) continue;
		  fillTrackTofItem(tracks[0],"DC2KPiPi_kaon",3 );
		  fillTrackTofItem(tracks[1],"DC2KPiPi_pion1",2 );
		  fillTrackTofItem(tracks[2],"DC2KPiPi_pion2",2 );
		}
		if (m_checkDedx)
		{
		  fillTrackDedxItem(tracks[0],"DC2KPiPi_kaon",3 );
		  fillTrackDedxItem(tracks[1],"DC2KPiPi_pion1",2 );
		  fillTrackDedxItem(tracks[2],"DC2KPiPi_pion2",2 );
		}
		indexD[MODE]++;
	  }

	  //****************************************************
	  //mode Dp to KPiPiPi0 only
	  //****************************************************
	  if((bestDTag)->decayMode()==EvtRecDTag::kDptoKPiPiPi0) {
		const int MODE = 7;
		DQAPi0Info pi0info;
		pi0info.setchilds(showers[0],showers[1]);
		if (pi0info.calculate()) {
		  if (pi0info.m() >m_mpi0Max || pi0info.m() < m_mpi0Min) 
			iswritten = 0;
		  fillPi0Item(showers,"DC2KPiPiPi0" ,&pi0info );
		  //		vmpi0.push_back(pi0info.m());
		  fillDecayModeItem(bestDTag,"DC2KPiPiPi0" );

		  fillTrackItem(tracks[0],"DC2KPiPiPi0_kaon",3 ,xorigin);
		  fillTrackItem(tracks[1],"DC2KPiPiPi0_pion1",2 ,xorigin);
		  fillTrackItem(tracks[2],"DC2KPiPiPi0_pion2",2 ,xorigin);
		  indexD[MODE]++;
		}
	  }
	  //****************************************************
	  //mode Dp to KsPiPi0 only
	  //****************************************************
	  if((bestDTag)->decayMode()==EvtRecDTag::kDptoKsPiPi0) {
		const int MODE = 8;
		DQAPi0Info pi0info;
		pi0info.setchilds(showers[0],showers[1]);
		if (pi0info.calculate()) {
		  DQAKsInfo ksinfo;
		  ksinfo.setchilds(tracks[0],tracks[1]);
		  ksinfo.setVtxPar(m_vxpar);
		  ksinfo.setPrimaryVtxPar(m_privxpar);
		  if (ksinfo.calculate()) {	
			if (pi0info.m() >m_mpi0Max || pi0info.m() < m_mpi0Min) 
			  iswritten = 0;
			fillPi0Item(showers,"DC2KsPiPi0" ,&pi0info );
			fillDecayModeItem(bestDTag,"DC2KsPiPi0" );
			fillTrackItem(tracks[0],"DC2KsPiPi0_pion1",2 ,xorigin);
			fillTrackItem(tracks[1],"DC2KsPiPi0_pion2",2 ,xorigin);
			fillTrackItem(tracks[2],"DC2KsPiPi0_pion3",2 ,xorigin);
			fillKsItem(tracks,"DC2KsPiPi0" ,&ksinfo);
			//		if (iswritten) vvmks.push_back(ksinfo.m());
			//		if (iswritten) vmksp4.push_back(ksinfo.mksp4());
			//		if (iswritten) vvlks.push_back(ksinfo.decayLengthRatio());
			indexD[MODE]++;
		  }
		}
	  }// end of Dp to Kspipi0
	  //****************************************************
	  //mode Dp to KKPi only
	  //****************************************************
	  if((bestDTag)->decayMode()==EvtRecDTag::kDptoKsPiPiPi) {
		const int MODE = 9;
		DQAKsInfo ksinfo;
		ksinfo.setchilds(tracks[0],tracks[1]);
		ksinfo.setVtxPar(m_vxpar);
		ksinfo.setPrimaryVtxPar(m_privxpar);
		if (ksinfo.calculate()) { 

		  fillKsItem(tracks,"DC2KsPiPiPi" ,&ksinfo);
		  //		if (iswritten) vvmks.push_back(ksinfo.m());
		  //		if (iswritten) vmksp4.push_back(ksinfo.mksp4());
		  //		if (iswritten) vvlks.push_back(ksinfo.decayLengthRatio());
		  fillDecayModeItem(bestDTag,"DC2KsPiPiPi" );

		  RecMdcKalTrack* mdcKalTrk1 = tracks[0]->mdcKalTrack();
		  RecMdcKalTrack* mdcKalTrk2 = tracks[1]->mdcKalTrack();
		  RecMdcKalTrack* mdcKalTrk3 = tracks[2]->mdcKalTrack();
		  RecMdcKalTrack* mdcKalTrk4 = tracks[3]->mdcKalTrack();
		  RecMdcKalTrack* mdcKalTrk5 = tracks[4]->mdcKalTrack();
		  RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
		  HepLorentzVector p4pion1 = mdcKalTrk1->p4(xmass[2]);
		  HepLorentzVector p4pion2 = mdcKalTrk2->p4(xmass[2]);
		  HepLorentzVector p4pion3 = mdcKalTrk3->p4(xmass[2]);
		  HepLorentzVector p4pion4 = mdcKalTrk4->p4(xmass[2]);
		  HepLorentzVector p4pion5 = mdcKalTrk5->p4(xmass[2]);
		  m_DC2KsPiPiPi_pion1_pp  = p4pion1.rho();
		  m_DC2KsPiPiPi_pion2_pp  = p4pion2.rho();
		  m_DC2KsPiPiPi_pion3_pp  = p4pion3.rho();
		  m_DC2KsPiPiPi_pion4_pp  = p4pion4.rho();
		  m_DC2KsPiPiPi_pion5_pp  = p4pion5.rho();
		  m_DC2KsPiPiPi_pion1_Id  = tracks[0]->trackId();
		  m_DC2KsPiPiPi_pion2_Id  = tracks[1]->trackId();
		  m_DC2KsPiPiPi_pion3_Id  = tracks[2]->trackId();
		  m_DC2KsPiPiPi_pion4_Id  = tracks[3]->trackId();
		  m_DC2KsPiPiPi_pion5_Id  = tracks[4]->trackId();
		  indexD[MODE]++;
		}
	  }//	end of kDptoKsPiPiPi
	  if((bestDTag)->decayMode()==EvtRecDTag::kDptoKKPi) {
		const int MODE = 10;
		fillDecayModeItem(bestDTag,"DC2KKPi" );

		RecMdcKalTrack* mdcKalTrk1 = tracks[0]->mdcKalTrack();
		RecMdcKalTrack* mdcKalTrk2 = tracks[1]->mdcKalTrack();
		RecMdcKalTrack* mdcKalTrk3 = tracks[2]->mdcKalTrack();
		RecMdcKalTrack::setPidType  (RecMdcKalTrack::kaon);
		HepLorentzVector p4kaon1 = mdcKalTrk1->p4(xmass[3]);
		HepLorentzVector p4kaon2 = mdcKalTrk2->p4(xmass[3]);
		RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
		HepLorentzVector p4pion = mdcKalTrk3->p4(xmass[2]);
		m_DC2KKPi_kaon1_pp  = p4kaon1.rho();
		m_DC2KKPi_kaon2_pp  = p4kaon2.rho();
		m_DC2KKPi_pion_pp  = p4pion.rho();
		m_DC2KKPi_kaon1_Id  = tracks[0]->trackId();
		m_DC2KKPi_kaon2_Id  = tracks[1]->trackId();
		m_DC2KKPi_pion_Id  = tracks[2]->trackId();
		indexD[MODE]++;
	  }//	end of kDptoKKPi
	  if ((bestDTag)->charge() == 0)
	  {
		m_D0_mBC = (bestDTag)->mBC();
		m_D0_deltE = (bestDTag)->deltaE();
		m_D0_mass = (bestDTag)->mass();
		indexD0All++;
	  }
	  if ((bestDTag)->charge() != 0)
	  {
		m_DC_mBC = (bestDTag)->mBC();
		m_DC_deltE = (bestDTag)->deltaE();
		m_DC_mass = (bestDTag)->mass();
		indexDCAll++;
	  }
	}
  }
  m_numAllMode = indexDAll;
  m_numD0Mode = indexD0All;
  m_numDCMode = indexDCAll;
  m_recNumD02KPi = indexD[0];
  m_recNumD02KPiPi0 = indexD[1]; 
  m_recNumD02KPiPiPi = indexD[2];
  m_recNumD02KPiPiPiPi0 = indexD[3];
  m_recNumD02KsPiPi = indexD[4];
  m_recNumD02KsPiPiPi0 = indexD[5];
  m_recNumDC2KPiPi = indexD[6];
  m_recNumDC2KPiPiPi0 = indexD[7];
  m_recNumDC2KsPiPi0 = indexD[8];
  m_recNumDC2KsPiPiPi = indexD[9];
  m_recNumDC2KKPi = indexD[10];
  int taggedMode = -1;	// 0 ~ 10
  for (int i=0; i!=11; i++)
  {
	if (indexD[i] == 1 )
	{
	  taggedMode = i; 
	}
  }

  Vint iGood;
  //  int nCharge = 0;
  for(int i = 0; i < evtRecEvent->totalCharged(); i++){
	EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
	if(!(*itTrk)->isMdcTrackValid()) continue;
	if (!(*itTrk)->isMdcKalTrackValid()) continue;
	RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
	RecMdcKalTrack *mdcKalTrk = (*itTrk)->mdcKalTrack();    

	double pch =mdcTrk->p();
	double x0  =mdcTrk->x();
	double y0  =mdcTrk->y();
	double z0  =mdcTrk->z();
	double phi0=mdcTrk->helix(1);
	double xv=xorigin.x();
	double yv=xorigin.y();
	double Rxy=fabs((x0-xv)*cos(phi0)+(y0-yv)*sin(phi0));
	m_vtxTrkId = (*itTrk)->trackId();
	m_vx0 = x0;
	m_vy0 = y0;
	m_vz0 = z0-xorigin.z();
	m_vr0 = Rxy;
	m_Vct=cos(mdcTrk->theta());
	if (m_writeTuple && m_checkVtx)
	  //	  m_tuple2->write();
	  if(fabs(m_vz0) >= m_vz0cut) continue;
	if(m_vr0 >= m_vr0cut) continue;
	if(fabs(m_Vct)>=m_cthcut) continue;

	iGood.push_back((*itTrk)->trackId());
	nCharge += mdcTrk->charge();
  }
  int nGood = iGood.size();

  ////
  //// Finish Good Charged Track Selection
  ////

  if ( m_writeTuple && (m_checkDedx || m_checkTof))
  {
	Vint iGam;
	iGam.clear();
	for(int i = evtRecEvent->totalCharged(); i< evtRecEvent->totalTracks(); i++) {
	  EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
	  if(!(*itTrk)->isEmcShowerValid()) continue;
	  RecEmcShower *emcTrk = (*itTrk)->emcShower();
	  Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());
	  // find the nearest charged track
	  double dthe = 200.;
	  double dphi = 200.;
	  double dang = 200.; 
	  for(int j = 0; j < evtRecEvent->totalCharged(); j++) {
		EvtRecTrackIterator jtTrk = evtRecTrkCol->begin() + j;
		if(!(*jtTrk)->isExtTrackValid()) continue;
		RecExtTrack *extTrk = (*jtTrk)->extTrack();
		if(extTrk->emcVolumeNumber() == -1) continue;
		Hep3Vector extpos = extTrk->emcPosition();
		//      double ctht = extpos.cosTheta(emcpos);
		double angd = extpos.angle(emcpos);
		double thed = extpos.theta() - emcpos.theta();
		double phid = extpos.deltaPhi(emcpos);
		thed = fmod(thed+CLHEP::twopi+CLHEP::twopi+pi, CLHEP::twopi) - CLHEP::pi;
		phid = fmod(phid+CLHEP::twopi+CLHEP::twopi+pi, CLHEP::twopi) - CLHEP::pi;

		if(fabs(thed) < fabs(dthe)) dthe = thed;
		if(fabs(phid) < fabs(dphi)) dphi = phid;
		if(angd < dang) dang = angd;
	  }
	  if(dang>=200) continue;
	  double eraw = emcTrk->energy();
	  dthe = dthe * 180 / (CLHEP::pi);
	  dphi = dphi * 180 / (CLHEP::pi);
	  dang = dang * 180 / (CLHEP::pi);
	  double m_dthe = dthe;
	  double m_dphi = dphi;
	  double m_dang = dang;
	  double m_eraw = eraw;
	  double theta=cos(emcpos.theta());
	  if ((fabs(theta) > m_Barrel_th && fabs(theta) < m_Endcap_th_1) || fabs(theta) > m_Endcap_th_2 ) continue;
	  //put different cut on barrel and endcap!
	  if((emcTrk->module()==0 || emcTrk->module()==2)&&eraw < m_energyThreshold_e) continue;
	  else if( emcTrk->module()==1 && eraw < m_energyThreshold_b) continue;
	  //if(eraw < m_energyThreshold) continue;
	  if(dang < m_gammaAngCut) continue;
	  //
	  // good photon cut will be set here
	  //
	  iGam.push_back((*itTrk)->trackId());
	}

	//
	// Finish Good Photon Selection
	//
	int nGam = iGam.size();

	//
	// Assign 4-momentum to each photon
	// 

	Vp4 pGam;
	pGam.clear();
	for(int i = 0; i < nGam; i++) {
	  EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGam[i]; 
	  RecEmcShower* emcTrk = (*itTrk)->emcShower();
	  double eraw = emcTrk->energy();
	  double phi = emcTrk->phi();
	  double the = emcTrk->theta();
	  HepLorentzVector ptrk;
	  ptrk.setPx(eraw*sin(the)*cos(phi));
	  ptrk.setPy(eraw*sin(the)*sin(phi));
	  ptrk.setPz(eraw*cos(the));
	  ptrk.setE(eraw);
	  //    ptrk = ptrk.boost(-0.011,0,0);// boost to cms
	  pGam.push_back(ptrk);
	}

	//
	// check dedx infomation
	//
	if (m_checkDedx)
	{
	  m_ngch = nGood;
	  for(int ii = 0; ii < nGood; ii++) {
		// dedx
		m_ptrk[ii] = 999.0;
		m_chie[ii] = 999.0;
		m_chimu[ii] = 999.0;
		m_chipi[ii] = 999.0;
		m_chik[ii] = 999.0;
		m_chip[ii] = 999.0;
		m_ghit[ii] = 999.0;
		m_thit[ii] = 999.0;
		m_probPH[ii] = 1999.0;
		m_normPH[ii] = 999.0;
	  }
	  int  indx0=0;
	  for(int i = 0; i < nGood; i++) {
		EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];
		if(!(*itTrk)->isMdcTrackValid()) continue;
		if(!(*itTrk)->isMdcDedxValid())continue;
		RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
		RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();
		m_trkiddedx[indx0] = (*itTrk)->trackId();
		m_ptrk[indx0] = mdcTrk->p();
		m_chie[indx0] = dedxTrk->chiE();
		m_chimu[indx0] = dedxTrk->chiMu();
		m_chipi[indx0] = dedxTrk->chiPi();
		m_chik[indx0] = dedxTrk->chiK();
		m_chip[indx0] = dedxTrk->chiP();
		m_ghit[indx0] = dedxTrk->numGoodHits();
		m_thit[indx0] = dedxTrk->numTotalHits();
		m_probPH[indx0] = dedxTrk->probPH();
		m_normPH[indx0] = dedxTrk->normPH();
		indx0++;
	  }
	  //	  m_tuple11->write();
	}  // end chechDedx
	//
	// check TOF infomation
	//
	if (m_checkTof)
	{
	  m_ngch2 = nGood;
	  for(int ii = 0; ii < nGood; ii++) {
		//endtof
		m_cntr_etof[ii] = 999.0;
		m_ptot_etof[ii] = 999.0;
		m_ph_etof[ii]   = 999.0;
		m_rhit_etof[ii] = 999.0;
		m_qual_etof[ii] = 999.0;
		m_te_etof[ii]   = 999.0;
		m_tmu_etof[ii]  = 999.0;
		m_tpi_etof[ii]  = 999.0;
		m_tk_etof[ii]   = 999.0;
		m_tp_etof[ii]   = 999.0;
		m_ec_tof[ii]    =  999.0;
		m_ec_toff_e[ii]   = 999.0;
		m_ec_toff_mu[ii]  = 999.0;
		m_ec_toff_pi[ii]  = 999.0;
		m_ec_toff_k[ii]   = 999.0;
		m_ec_toff_p[ii]   = 999.0;
		m_ec_tsig_e[ii]   = 999.0;
		m_ec_tsig_mu[ii]  = 999.0;
		m_ec_tsig_pi[ii]  = 999.0;
		m_ec_tsig_k[ii]   = 999.0;
		m_ec_tsig_p[ii]   = 999.0;

		// barrel tof
		m_cntr_btof1[ii]  = 999.0;
		m_ptot_btof1[ii]  = 999.0;
		m_ph_btof1[ii]    = 999.0;
		m_zhit_btof1[ii]  = 999.0;
		m_qual_btof1[ii]  = 999.0;
		m_te_btof1[ii]    = 999.0;
		m_tmu_btof1[ii]   = 999.0;
		m_tpi_btof1[ii]   = 999.0;
		m_tk_btof1[ii]    = 999.0;
		m_tp_btof1[ii]    = 999.0;
		m_b1_tof[ii]      = 999.0;
		m_b1_toff_e[ii]   = 999.0;
		m_b1_toff_mu[ii]  = 999.0;
		m_b1_toff_pi[ii]  = 999.0;
		m_b1_toff_k[ii]   = 999.0;
		m_b1_toff_p[ii]   = 999.0;
		m_b1_tsig_e[ii]   = 999.0;
		m_b1_tsig_mu[ii]  = 999.0;
		m_b1_tsig_pi[ii]  = 999.0;
		m_b1_tsig_k[ii]   = 999.0;
		m_b1_tsig_p[ii]   = 999.0;
		//pid
		//m_dedx_pid[ii] = 999.0;
		//m_tof1_pid[ii] = 999.0;
		//m_tof2_pid[ii] = 999.0;
		//m_prob_pid[ii] = 999.0; 
		//m_ptrk_pid[ii] = 999.0;  
		//m_cost_pid[ii] = 999.0;
	  }
	  int indx1=0;
	  for(int i = 0; i < nGood; i++) {
		EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];
		if(!(*itTrk)->isMdcTrackValid()) continue;
		if(!(*itTrk)->isTofTrackValid()) continue;

		RecMdcTrack * mdcTrk = (*itTrk)->mdcTrack();
		SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();

		double ptrk = mdcTrk->p();
		SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();
		for(;iter_tof != tofTrkCol.end(); iter_tof++ ) { 
		  TofHitStatus *status = new TofHitStatus; 
		  status->setStatus((*iter_tof)->status());
		  if(!(status->is_barrel())){//endcap
			if( !(status->is_counter()) ) continue; // ? 
			if( status->layer()!=1 ) continue;//layer1
			double path=(*iter_tof)->path(); // ? 
			double tof  = (*iter_tof)->tof();
			double ph   = (*iter_tof)->ph();
			double rhit = (*iter_tof)->zrhit();
			double qual = 0.0 + (*iter_tof)->quality();
			double cntr = 0.0 + (*iter_tof)->tofID();
			double texp[5];
			double tsig[5];
			for(int j = 0; j < 5; j++) {//0 e, 1 mu, 2 pi, 3 K, 4 p
			  texp[j] = (*iter_tof)->texp(j);
			  //           tsig[j] = (*iter_tof)->sigma(j);
			  //           toffset[j] = (*iter_tof)->offset(j);
			}
			m_trkidtof[indx1] = (*itTrk)->trackId();
			m_cntr_etof[indx1]  = cntr;
			m_ptot_etof[indx1] = ptrk;
			m_ph_etof[indx1]   = ph;
			m_rhit_etof[indx1]  = rhit;
			m_qual_etof[indx1]  = qual;
			m_te_etof[indx1]    = tof - texp[0];
			m_tmu_etof[indx1]   = tof - texp[1];
			m_tpi_etof[indx1]   = tof - texp[2];
			m_tk_etof[indx1]    = tof - texp[3];
			m_tp_etof[indx1]    = tof - texp[4];

			m_ec_tof[indx1]      =  tof;

			m_ec_toff_e[indx1]   = (*iter_tof)->toffset(0);
			m_ec_toff_mu[indx1]  = (*iter_tof)->toffset(1);
			m_ec_toff_pi[indx1]  = (*iter_tof)->toffset(2);
			m_ec_toff_k[indx1]   = (*iter_tof)->toffset(3);
			m_ec_toff_p[indx1]   = (*iter_tof)->toffset(4);

			m_ec_tsig_e[indx1]   = (*iter_tof)->sigma(0);
			m_ec_tsig_mu[indx1]  = (*iter_tof)->sigma(1);
			m_ec_tsig_pi[indx1]  = (*iter_tof)->sigma(2);
			m_ec_tsig_k[indx1]   = (*iter_tof)->sigma(3);
			m_ec_tsig_p[indx1]   = (*iter_tof)->sigma(4);

		  }
		  else {//barrel
			if( !(status->is_cluster()) ) continue; // ? 
			double path=(*iter_tof)->path(); // ? 
			double tof  = (*iter_tof)->tof();
			double ph   = (*iter_tof)->ph();
			double rhit = (*iter_tof)->zrhit();
			double qual = 0.0 + (*iter_tof)->quality();
			double cntr = 0.0 + (*iter_tof)->tofID();
			double texp[5];
			for(int j = 0; j < 5; j++) {
			  texp[j] = (*iter_tof)->texp(j);
			}
			m_cntr_btof1[indx1]  = cntr;
			m_ptot_btof1[indx1] = ptrk;
			m_ph_btof1[indx1]   = ph;
			m_zhit_btof1[indx1]  = rhit;
			m_qual_btof1[indx1]  = qual;
			m_te_btof1[indx1]    = tof - texp[0];
			m_tmu_btof1[indx1]   = tof - texp[1];
			m_tpi_btof1[indx1]   = tof - texp[2];
			m_tk_btof1[indx1]    = tof - texp[3];
			m_tp_btof1[indx1]    = tof - texp[4];

			m_b1_tof[indx1]      =  tof;

			m_b1_toff_e[indx1]   = (*iter_tof)->toffset(0);
			m_b1_toff_mu[indx1]  = (*iter_tof)->toffset(1);
			m_b1_toff_pi[indx1]  = (*iter_tof)->toffset(2);
			m_b1_toff_k[indx1]   = (*iter_tof)->toffset(3);
			m_b1_toff_p[indx1]   = (*iter_tof)->toffset(4);

			m_b1_tsig_e[indx1]   = (*iter_tof)->sigma(0);
			m_b1_tsig_mu[indx1]  = (*iter_tof)->sigma(1);
			m_b1_tsig_pi[indx1]  = (*iter_tof)->sigma(2);
			m_b1_tsig_k[indx1]   = (*iter_tof)->sigma(3);
			m_b1_tsig_p[indx1]   = (*iter_tof)->sigma(4);

		  }
		  delete status; 
		} 
		indx1++;
	  } // loop all charged track

	  //	  m_tuple12->write();
	  m_nggneu = nGam;
	  int iphoton = 0;
	  for (int i=0; i<nGam; i++)
	  {
		EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + iGam[i];
		if (!(*itTrk)->isEmcShowerValid()) continue; 
		RecEmcShower *emcTrk = (*itTrk)->emcShower();
		m_shwidtof[iphoton] = (*itTrk)->trackId();
		m_numHits[iphoton] = emcTrk->numHits();
		m_secondmoment[iphoton] = emcTrk->secondMoment();
		m_x[iphoton] = emcTrk->x();
		m_y[iphoton]= emcTrk->y();
		m_z[iphoton]= emcTrk->z();
		m_cosemc[iphoton]   = cos(emcTrk->theta());
		m_phiemc[iphoton]   = emcTrk->phi();      
		m_energy[iphoton]   = emcTrk->energy();
		m_eSeed[iphoton]    = emcTrk->eSeed(); 
		m_e3x3[iphoton]     = emcTrk->e3x3();  
		m_e5x5[iphoton]     = emcTrk->e5x5(); 
		m_lat[iphoton]      = emcTrk->latMoment();
		m_a20[iphoton]      = emcTrk->a20Moment();
		m_a42[iphoton]      = emcTrk->a42Moment();
		iphoton++;
	  }
	  //	  m_tuple13->write();
	}	//end checkTof
  }

  // DQAHist
  if (m_regHist)
  {
	if (taggedMode != -1 )
	{
	  string strdecaymode[11] = {"D02KPi","D02KPiPi0","D02KPiPiPi","D02KPiPiPiPi0","D02KsPiPi","D02KsPiPiPi0",\
		"DC2KPiPi","DC2KPiPiPi0","DC2KsPiPi0","DC2KsPiPiPi","DC2KKPi"};
	  int pi0cut[11] = {0,1,0,1,0,1,0,1,1,0,0};
	  int kscut[11] = {0,0,0,0,1,1,0,0,1,1,0};
	  string strtrack[14] = {"D02KPi_kaon","D02KPi_pion","D02KPiPi0_kaon","D02KPiPi0_pion","D02KsPiPi_pion1",\
		"D02KsPiPi_pion2","D02KsPiPi_pion3","D02KsPiPi_pion4","DC2KPiPi_kaon","DC2KPiPi_pion1","DC2KPiPi_pion2",\
		  "DC2KsPiPi0_pion1","DC2KsPiPi0_pion2","DC2KsPiPi0_pion3" };
	  //	for (int i=0;i!=11;i++)
	  //	{
	  //	  fillHistogram(m_thsvc,strdecaymode[i],3,m_decayModeHistName,m_tuple4,pi0cut[i],kscut[i]);
	  //	}
	  fillHistogram(m_thsvc,strdecaymode[taggedMode],3,m_decayModeHistName,m_tuple4,pi0cut[taggedMode],kscut[taggedMode]);
	  string strDdecaymode[2] = {"D0","DC"};
	  //	fillHistogram(m_thsvc,strDdecaymode[0],3,m_decayModeHistName,m_tuple4);
	  //	fillHistogram(m_thsvc,strDdecaymode[1],3,m_decayModeHistName,m_tuple4);
	  if (taggedMode < 6)
		fillHistogram(m_thsvc,strDdecaymode[0],3,m_decayModeHistName,m_tuple4);
	  else 
		fillHistogram(m_thsvc,strDdecaymode[1],3,m_decayModeHistName,m_tuple4);

	  string strtrackdedx[7] = {"D02KPi_kaon","D02KPi_pion","D02KPiPi0_kaon","D02KPiPi0_pion","DC2KPiPi_kaon","DC2KPiPi_pion1","DC2KPiPi_pion2"};
	  string strtracktof[7] = {"D02KPi_kaon","D02KPi_pion","D02KPiPi0_kaon","D02KPiPi0_pion", "DC2KPiPi_kaon", "DC2KPiPi_pion1", "DC2KPiPi_pion2"};
	  string strphoton[2] = {"D02KPiPi0_photon1","D02KPiPi0_photon2"};
	  string strpi0[2] = {"D02KPiPi0","DC2KsPiPi0"};
	  string strks[2] = {"D02KsPiPi_ks","DC2KsPiPi0_ks"};
	  if (taggedMode == 0)
	  {
		if (g_mBC > 1.86 && g_mBC < 1.87 && fabs(g_deltE) < 0.03)
		{
		  fillHistogram(m_thsvc,strtrack[0],10,m_trackHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrack[1],10,m_trackHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrackdedx[0],6,m_trackDedxHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrackdedx[1],6,m_trackDedxHistName,m_tuple4);
		  fillTofHistogram(m_thsvc,strtracktof[0]);
		  fillTofHistogram(m_thsvc,strtracktof[1]);
		}
	  }
	  if (taggedMode == 1)
	  {
		if (g_mBC > 1.86 && g_mBC < 1.87 && g_deltE > -0.0583 && g_deltE < 0.035 )
		{
		  fillHistogram(m_thsvc,strtrack[2],10,m_trackHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrack[3],10,m_trackHistName,m_tuple4);

		  fillHistogram(m_thsvc,strtrackdedx[2],6,m_trackDedxHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrackdedx[3],6,m_trackDedxHistName,m_tuple4);
		  fillTofHistogram(m_thsvc,strtracktof[2]);
		  fillTofHistogram(m_thsvc,strtracktof[3]);
		  fillHistogram(m_thsvc,strphoton[0],5,m_photonHistName,m_tuple4);
		  fillHistogram(m_thsvc,strphoton[1],5,m_photonHistName,m_tuple4);

		  fillHistogram(m_thsvc,strpi0[0],5,m_Pi0HistName,m_tuple4);
		}
	  }
	  if (taggedMode == 4)
	  {
		if (g_mBC > 1.86 && g_mBC < 1.87 && fabs(g_deltE) < 0.03 )
		{
		  fillHistogram(m_thsvc,strtrack[4],10,m_trackHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrack[5],10,m_trackHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrack[6],10,m_trackHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrack[7],10,m_trackHistName,m_tuple4);

		  fillHistogram(m_thsvc,strks[0],4,m_KsHistName,m_tuple4);
		}
	  }
	  if (taggedMode == 6)
	  {
		if (g_mBC > 1.86 && g_mBC < 1.87 && fabs(g_deltE) < 0.0218 )
		{
		  fillHistogram(m_thsvc,strtrack[8],10,m_trackHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrack[9],10,m_trackHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrack[10],10,m_trackHistName,m_tuple4);
		  fillTofHistogram(m_thsvc,strtracktof[4]);
		  fillTofHistogram(m_thsvc,strtracktof[5]);
		  fillTofHistogram(m_thsvc,strtracktof[6]);
		  fillHistogram(m_thsvc,strtrackdedx[4],6,m_trackDedxHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrackdedx[5],6,m_trackDedxHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrackdedx[6],6,m_trackDedxHistName,m_tuple4);
		}
	  }
	  if (taggedMode == 8)
	  {
		if (g_mBC > 1.86 && g_mBC < 1.87 && g_deltE < 0.0423 && g_deltE > -0.0455 )
		{
		  fillHistogram(m_thsvc,strtrack[11],10,m_trackHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrack[12],10,m_trackHistName,m_tuple4);
		  fillHistogram(m_thsvc,strtrack[13],10,m_trackHistName,m_tuple4);

		  fillHistogram(m_thsvc,strpi0[1],5,m_Pi0HistName,m_tuple4);
		  fillHistogram(m_thsvc,strks[1],4,m_KsHistName,m_tuple4);
		}
	  }
	}
  }
  if (m_writeTuple)
  {
	m_tuple4->write();
  }
  if (m_writeTuple && m_checkEvt)
  {
	//	m_tuple5->write();
  }
  //if (m_checkDedx)
  //m_tuple16->write();
  //if (m_checkTof)
  //m_tuple17->write();

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}


void DQADtag::regTrackHistogram(ITHistSvc*& , string &str)
{
  MsgStream log(msgSvc(), name());
  const int NUM = 10;
  TString hisname[NUM];
  TString strHisReg[NUM];
  TString strHisLog[NUM];
  for (int i=0;i!=NUM;i++)
  {
	hisname[i] = str + "_" + m_trackHistName[i]; 		// D02KPi_k_px ...
	strHisLog[i] = "Couldn't register " + hisname[i];
	strHisReg[i] = "/DQAHist/Dtag/" + hisname[i];
	if (m_debug) cout<<"hisname:	"<<hisname[i]<<endl;
	if (m_debug) cout<<"strHisReg:	"<<strHisReg[i]<<endl;
  }
  TH1F* hist[10];
  hist[0]  = new TH1F(hisname[0] ,  hisname[0], 100,-1.1,1.1);
  hist[1]  = new TH1F(hisname[1] ,  hisname[1], 100,-1.1,1.1);
  hist[2]  = new TH1F(hisname[2] ,  hisname[2], 100,-1.1,1.1);
  hist[3]  = new TH1F(hisname[3] ,  hisname[3], 200,0.,1.2);
  hist[4]  = new TH1F(hisname[4] ,  hisname[4], 100,-1.,1.);
  hist[5]  = new TH1F(hisname[5] ,  hisname[5], 100,-1.,1.);
  hist[6]  = new TH1F(hisname[6] ,  hisname[6], 100,-10.,10.);
  hist[7]  = new TH1F(hisname[7] ,  hisname[7], 100,-1.,1.);
  hist[8]  = new TH1F(hisname[8] ,  hisname[8], 100,-1.,1.);
  hist[9]  = new TH1F(hisname[9] ,  hisname[9], 200,0.,1.4);
  for (int i=0;i!=NUM;i++)
  {
	if(m_thsvc->regHist((string)strHisReg[i], hist[i]).isFailure()) {
	  log << MSG::ERROR << strHisLog[i] << endreq;
	}
  }
}
void DQADtag::regPhotonHistogram(ITHistSvc*& , string &str)
{
  MsgStream log(msgSvc(), name());
  const int NUM = 5;
  TString hisname[NUM];
  TString strHisReg[NUM];
  TString strHisLog[NUM];
  for (int i=0;i!=NUM;i++)
  {
	hisname[i] = str + "_" + m_photonHistName[i]; 		// D02KPi_k_px ...
	strHisLog[i] = "Couldn't register " + hisname[i];
	strHisReg[i] = "/DQAHist/Dtag/" + hisname[i];
	if (m_debug) cout<<"hisname:	"<<hisname[i]<<endl;
	if (m_debug) cout<<"strHisReg:	"<<strHisReg[i]<<endl;
  }
  TH1F* hist[5];
  hist[0]  = new TH1F(hisname[0] ,  hisname[0], 80,0,40);
  hist[1]  = new TH1F(hisname[1] ,  hisname[1], 160,0,80);
  hist[2]  = new TH1F(hisname[2] ,  hisname[2], 100,-100.,100.);
  hist[3]  = new TH1F(hisname[3] ,  hisname[3], 100,-100.,100.);
  hist[4]  = new TH1F(hisname[4] ,  hisname[4], 100,-150.,150.);
  for (int i=0;i!=NUM;i++)
  {
	if(m_thsvc->regHist((string)strHisReg[i], hist[i]).isFailure()) {
	  log << MSG::ERROR << strHisLog[i] << endreq;
	}
  }
}

void DQADtag::regTrackDedxHistogram(ITHistSvc*& , string& str)
{
  MsgStream log(msgSvc(), name());
  const int NUM = 6;
  TString hisname[NUM];
  TString strHisReg[NUM];
  TString strHisLog[NUM];
  for (int i=0;i!=NUM;i++)
  {
	hisname[i] = str + "_" + m_trackDedxHistName[i]; 	// D02KPi_dedx_chik
	strHisLog[i] = "Couldn't register " + hisname[i];
	strHisReg[i] = "/DQAHist/Dtag/" + hisname[i];
	if (m_debug) cout<<"hisname:	"<<hisname[i]<<endl;
	if (m_debug) cout<<"strHisReg:	"<<strHisReg[i]<<endl;
  }
  TH1F* hist[6];
  hist[0] = new TH1F(hisname[0] , hisname[0], 200,-3.,5.);
  hist[1] = new TH1F(hisname[1] , hisname[1], 200,-2.,15.);
  hist[2] = new TH1F(hisname[2] , hisname[2], 80,0,40);
  hist[3] = new TH1F(hisname[3] , hisname[3], 120,0,60);
  hist[4] = new TH1F(hisname[4] , hisname[4], 100,200,1400);
  hist[5] = new TH1F(hisname[5] , hisname[5], 100,0.6,1.5);
  for (int i=0;i!=NUM;i++)
  {
	if(m_thsvc->regHist((string)strHisReg[i], hist[i]).isFailure()) {
	  log << MSG::ERROR << strHisLog[i] << endreq;
	}
  }
}
void DQADtag::regTrackTofHistogram(ITHistSvc*& , string& str)
{
  MsgStream log(msgSvc(), name());
  const int NUM = 24;
  TString hisname[NUM];
  TString strHisReg[NUM];
  TString strHisLog[NUM];
  string tofReadOutID[7] = {"1","2","3","4","12","34","1234"};
  int num(0);
  for (int i=0;i!=3;i++)
  {
	for (int j=0;j!=7;j++)
	{
	  num = i*7+j;
	  hisname[num] = str + "_" + m_trackTofHistName[i] + "_" + tofReadOutID[j]; 	// D02KPi_dedx_chik
	  strHisLog[num] = "Couldn't register " + hisname[num];
	  strHisReg[num] = "/DQAHist/Dtag/" + hisname[num];
	  //strHisLog[num] = "Couldn't register " + hisname[num] + "_" + sprintf("%d",j);
	  //strHisReg[num] = "/DQAHist/Dtag/" + hisname[num] + "_" + sprintf("%d",j);
	  if (m_debug) cout<<"hisname:	"<<hisname[num]<<endl;
	  if (m_debug) cout<<"strHisReg:	"<<strHisReg[num]<<endl;
	}
  }
  for (int i=3;i!=6;i++)
  {
	num = 3*6+i;
	hisname[num] = str + "_" + m_trackTofHistName[i]; 	// D02KPi_dedx_chik
	strHisLog[num] = "Couldn't register " + hisname[num];
	strHisReg[num] = "/DQAHist/Dtag/" + hisname[num];
  }
  TH1F* hist[24];
  for (int i=0;i!=24;i++)
  {
	hist[i] = new TH1F(hisname[i] , hisname[i], 100,-4.0,4.);
  }
  for (int i=0;i!=NUM;i++)
  {
	if(m_thsvc->regHist((string)strHisReg[i], hist[i]).isFailure()) {
	  log << MSG::ERROR << strHisLog[i] << endreq;
	}
  }
}
void DQADtag::regDecayModeHistogram(ITHistSvc*& , string& str)
{
  MsgStream log(msgSvc(), name());
  const int NUM = 3;
  TString hisname[NUM];
  TString strHisReg[NUM];
  TString strHisLog[NUM];
  for (int i=0;i!=NUM;i++)
  {
	hisname[i] = str + "_" + m_decayModeHistName[i]; 	// D02KPi_dedx_chik
	strHisLog[i] = "Couldn't register " + hisname[i];
	strHisReg[i] = "/DQAHist/Dtag/" + hisname[i];
	if (m_debug) cout<<"hisname:	"<<hisname[i]<<endl;
	if (m_debug) cout<<"strHisReg:	"<<strHisReg[i]<<endl;
  }
  TH1F* hist[3];
  hist[0] = new TH1F(hisname[0] , hisname[0], 100,1.83,1.89);
  hist[1] = new TH1F(hisname[1] , hisname[1], 100,-0.1,0.1);
  hist[2] = new TH1F(hisname[2] , hisname[2], 100,1.83,1.89);
  for (int i=0;i!=NUM;i++)
  {
	if(m_thsvc->regHist((string)strHisReg[i], hist[i]).isFailure()) {
	  log << MSG::ERROR << strHisLog[i] << endreq;
	}
  }
}
void DQADtag::regKsHistogram(ITHistSvc*& , string& str)
{
  MsgStream log(msgSvc(), name());
  const int NUM = 4;
  TString hisname[NUM];
  TString strHisReg[NUM];
  TString strHisLog[NUM];
  TH1F* hist[4];
  for (int i=0;i!=NUM;i++)
  {
	hisname[i] = str + "_ks_" + m_KsHistName[i]; 	// D02KPi_dedx_chik
	strHisLog[i] = "Couldn't register " + hisname[i];
	strHisReg[i] = "/DQAHist/Dtag/" + hisname[i];
	if (m_debug) cout<<"hisname:	"<<hisname[i]<<endl;
	if (m_debug) cout<<"strHisReg:	"<<strHisReg[i]<<endl;
  } 
  hist[0] = new TH1F(hisname[0] , hisname[0], 100,0.46,0.54);
  hist[1] = new TH1F(hisname[1] , hisname[1], 100,0.46,0.54);
  hist[2] = new TH1F(hisname[2] , hisname[2], 100,0,30);
  hist[3] = new TH1F(hisname[3] , hisname[3], 100,0,30);
  for (int i=0;i!=NUM;i++)
  {
	if(m_thsvc->regHist((string)strHisReg[i], hist[i]).isFailure()) {
	  log << MSG::ERROR << strHisLog[i] << endreq;
	}
  }
}
void DQADtag::regPi0Histogram(ITHistSvc*& , string& str)
{
  MsgStream log(msgSvc(), name());
  const int NUM = 5;
  TString hisname[NUM];
  TString strHisReg[NUM];
  TString strHisLog[NUM];
  TH1F* hist[5];
  for (int i=0;i!=NUM;i++)
  {
	hisname[i] = str + "_" + m_Pi0HistName[i]; 	// D02KPi_dedx_chik
	strHisLog[i] = "Couldn't register " + hisname[i];
	strHisReg[i] = "/DQAHist/Dtag/" + hisname[i];
	if (m_debug) cout<<"hisname:	"<<hisname[i]<<endl;
	if (m_debug) cout<<"strHisReg:	"<<strHisReg[i]<<endl;
  }  
  hist[0] = new TH1F(hisname[0] , hisname[0], 100,0.08,0.18);
  hist[1] = new TH1F(hisname[1] , hisname[1], 100,0.0,1.0);
  hist[2] = new TH1F(hisname[2] , hisname[2], 100,-1.0,1.0);
  hist[3] = new TH1F(hisname[3] , hisname[3], 100,0.0,1.0);
  hist[4] = new TH1F(hisname[4] , hisname[4], 100,-1.0,1.0);
  for (int i=0;i!=NUM;i++)
  {
	if(m_thsvc->regHist((string)strHisReg[i], hist[i]).isFailure()) {
	  log << MSG::ERROR << strHisLog[i] << endreq;
	}
  }
}
void DQADtag::fillHistogram(ITHistSvc*& , string str,  int NUM, string* fillHistName, NTuple::Tuple*& m_tuple)
{
  MsgStream log(msgSvc(), name());
  string* strVarName = new string[NUM];
  string* strHisGet = new string[NUM];
  string* strHisLog = new string[NUM];
  TH1* hist;
  for (int i=0;i!=NUM;i++)
  {
	strVarName[i] = str + "_" + fillHistName[i]; 		// 
	strHisLog[i] = "Couldn't retrieve " + strVarName[i];
	strHisGet[i] = "/DQAHist/Dtag/" + strVarName[i];
	//cout<<"strVarName:	"<<strVarName[i]<<endl;
	//cout<<"strHisGet:	"<<strHisGet[i]<<endl;
	NTuple::Item<double> m_Variable;
	m_tuple->item(strVarName[i],m_Variable);
	if (m_thsvc->getHist(strHisGet[i], hist).isSuccess()) {
	  hist->Fill(m_Variable);
	}
	else {
	  log << MSG::ERROR << strHisLog[i] << endreq;
	}
  }
  //delete [] hist;
  delete [] strVarName;
  delete [] strHisGet;
  delete [] strHisLog;
}
void DQADtag::fillHistogram(ITHistSvc*& , string str,  int NUM, string* fillHistName, NTuple::Tuple*& m_tuple, int pi0cut, int kscut)
{
  MsgStream log(msgSvc(), name());
  string* strVarName = new string[NUM];
  string* strHisGet = new string[NUM];
  string* strHisLog = new string[NUM];
  TH1* hist;
  NTuple::Item<double> m_Variable0;
  NTuple::Item<double> m_Variable1;
  NTuple::Item<double> m_Variable2;
  NTuple::Item<double> m_Variable3;
  if (pi0cut)
	m_tuple->item(str+"_pi0mass",m_Variable0);
  if (kscut)
	m_tuple->item(str+"_ks_vfitmass",m_Variable1);
  m_tuple->item(str+"_deltE",m_Variable2);
  m_tuple->item(str+"_mBC",m_Variable3);
  for (int i=0;i!=NUM;i++)
  {
	strVarName[i] = str + "_" + fillHistName[i]; 		// D02KPi_kaon_px ...
	strHisLog[i] = "Couldn't retrieve " + strVarName[i];
	strHisGet[i] = "/DQAHist/Dtag/" + strVarName[i];
	//cout<<"strVarName:	"<<strVarName[i]<<endl;
	//cout<<"strHisGet:	"<<strHisGet[i]<<endl;
	NTuple::Item<double> m_Variable;
	m_tuple->item(strVarName[i],m_Variable);
	if (m_thsvc->getHist(strHisGet[i], hist).isSuccess()) {
	  bool isfill = 1;
	  if (pi0cut) 
	  {
		if (m_Variable0>m_mpi0MaxCut||m_Variable0<m_mpi0MinCut) isfill = 0;
	  }
	  if (kscut) 
	  {
		if (m_Variable1>m_mKsMaxCut||m_Variable1<m_mKsMinCut) isfill = 0;
	  }
	  if (i==0)
	  {
		if (fabs(m_Variable2)>0.03) isfill = 0;
	  }
	  if (i==1)
	  {
		if (m_Variable3 > 1.87 || m_Variable3 < 1.86 ) isfill = 0;
	  }
	  if (i==2)
	  {
		if (m_Variable3 > 1.87 || m_Variable3 < 1.86  || fabs(m_Variable2)>0.03 ) isfill = 0;
	  }
	  if (isfill == 1)
	  {
		hist->Fill(m_Variable);
	  }
	}
	else {
	  log << MSG::ERROR << strHisLog[i] << endreq;
	}
  }
  //delete [] hist;
  delete [] strVarName;
  delete [] strHisGet;
  delete [] strHisLog;
}
void DQADtag::fillTofHistogram(ITHistSvc*& , string str )
{
  MsgStream log(msgSvc(), name());
  string strVarName[6];
  string strHisGet[24];
  string strHisLog[24];
  TH1* hist;
  string tofReadOutID[7] = {"1","2","3","4","12","34","1234"};
  for (int i=0;i!=3;i++)
  {
	strVarName[i] = str + "_" + m_trackTofHistName[i]; 		// D02KPi_kaon_px ...
	//cout<<"strVarName:	"<<strVarName[i]<<endl;
	//cout<<"strHisGet:	"<<strHisGet[i]<<endl;
	NTuple::Array<double> m_Variable;
	m_tuple4->item(strVarName[i],m_Variable);
	for (int j=0;j!=7;j++)
	{
	  int num = i*7+j;
	  strHisLog[num] = "Couldn't retrieve " + strVarName[i] + "_" + tofReadOutID[j];
	  strHisGet[num] = "/DQAHist/Dtag/" + strVarName[i] + "_" + tofReadOutID[j];
	  if (m_thsvc->getHist(strHisGet[num], hist).isSuccess()) {
		hist->Fill(m_Variable[j]);
	  }
	  else {
		log << MSG::ERROR << strHisLog[num] << endreq;
	  }
	}
  }
  for (int i=3;i!=6;i++)
  {
	int num = 3*6+ i;
	strVarName[i] = str + "_" + m_trackTofHistName[i]; 		// D02KPi_kaon_px ...
	strHisLog[num] = "Couldn't retrieve " + strVarName[i];
	strHisGet[num] = "/DQAHist/Dtag/" + strVarName[i];
	//cout<<"strVarName:	"<<strVarName[i]<<endl;
	//cout<<"strHisGet:	"<<strHisGet[num]<<endl;
	NTuple::Item<double> m_Variable;
	m_tuple4->item(strVarName[i],m_Variable);
	if (m_thsvc->getHist(strHisGet[num], hist).isSuccess()) {
	  hist->Fill(m_Variable);
	}
	else {
	  log << MSG::ERROR << strHisLog[num] << endreq;
	}
  }
  //delete [] hist;
}
void DQADtag::fillHistogram(ITHistSvc*& , string str, int NUM, string* fillHistName, int tuplenum, bool cut)
{
  MsgStream log(msgSvc(), name());
  string* strVarName = new string[NUM];
  string* strHisGet = new string[NUM];
  string* strHisLog = new string[NUM];
  TH1* hist;
  string pi0VarName = str + "_pi0mass";
  NTuple::Item<double> m_pi0Variable;
  m_tuple4->item(pi0VarName,m_pi0Variable);
  for (int i=0;i!=NUM;i++)
  {
	strVarName[i] = str + "_" + fillHistName[i]; 		// D02KPi_k_px ...
	strHisLog[i] = "Couldn't retrieve " + strVarName[i];
	strHisGet[i] = "/DQAHist/Dtag/" + strVarName[i];
	//cout<<"strVarName:	"<<strVarName[i]<<endl;
	//cout<<"strHisGet:	"<<strHisGet[i]<<endl;
	NTuple::Item<double> m_Variable;
	m_tuple4->item(strVarName[i],m_Variable);
	if (m_thsvc->getHist(strHisGet[i], hist).isSuccess()) {
	  if(! (m_pi0Variable >m_mpi0Max || m_pi0Variable < m_mpi0Min))
	  {
		hist->Fill(m_Variable);
	  }
	}
	else {
	  log << MSG::ERROR << strHisLog[i] << endreq;
	}
  }
  delete [] strVarName;
  delete [] strHisGet;
  delete [] strHisLog;
}
void DQADtag::fillDecayModeItem(EvtRecDTag* bestDTag, string str)
{
  string strVarName[6];
  //for (int i=0;i!=6;i++)
  //{
  ////cout<<"m_DecayModeItemName:	"<<m_DecayModeItemName[i]<<endl;
  //}
  for (int i=0;i!=6;i++ )
  {
	strVarName[i] = str + "_" + m_DecayModeItemName[i];
	//cout<<"	"<<strVarName[i]<<endl;
  }
  NTuple::Item<double> m_Variable0;
  NTuple::Item<double> m_Variable1;
  NTuple::Item<double> m_Variable2;
  NTuple::Item<double> m_Variable3;
  NTuple::Item<double> m_Variable4;
  NTuple::Item<double> m_Variable5;
  m_tuple4->item(strVarName[0],m_Variable0);
  m_tuple4->item(strVarName[1],m_Variable1);
  m_tuple4->item(strVarName[2],m_Variable2);
  m_tuple4->item(strVarName[3],m_Variable3);
  m_tuple4->item(strVarName[4],m_Variable4);
  m_tuple4->item(strVarName[5],m_Variable5);
  m_Variable0=(bestDTag)->mBC();
  m_Variable1=(bestDTag)->deltaE();
  m_Variable2=(bestDTag)->p4().rho();
  m_Variable3=(bestDTag)->charm();
  m_Variable4=(bestDTag)->mass();
  m_Variable5=(bestDTag)->beamE();
}
void DQADtag::fillTrackItem(EvtRecTrack* track, string str, int type,  Hep3Vector& xorigin)
{
  string strVarName[11];
  for (int i=0;i!=11;i++ )
  {
	strVarName[i] = str + "_" + m_TrackItemName[i];
	//cout<<"strVarName:	"<<strVarName[i]<<endl;
  }
  NTuple::Item<double> m_Variable0;
  NTuple::Item<double> m_Variable1;
  NTuple::Item<double> m_Variable2;
  NTuple::Item<double> m_Variable3;
  NTuple::Item<double> m_Variable4;
  NTuple::Item<double> m_Variable5;
  NTuple::Item<double> m_Variable6;
  NTuple::Item<double> m_Variable7;
  NTuple::Item<double> m_Variable8;
  NTuple::Item<double> m_Variable9;
  NTuple::Item<long> m_Variable10;
  m_tuple4->item(strVarName[0],m_Variable0);
  m_tuple4->item(strVarName[1],m_Variable1);
  m_tuple4->item(strVarName[2],m_Variable2);
  m_tuple4->item(strVarName[3],m_Variable3);
  m_tuple4->item(strVarName[4],m_Variable4);
  m_tuple4->item(strVarName[5],m_Variable5);
  m_tuple4->item(strVarName[6],m_Variable6);
  m_tuple4->item(strVarName[7],m_Variable7);
  m_tuple4->item(strVarName[8],m_Variable8);
  m_tuple4->item(strVarName[9],m_Variable9);
  m_tuple4->item(strVarName[10],m_Variable10);
  RecMdcKalTrack* mdcKalTrk = track->mdcKalTrack();
  if (type==2)
	RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
  else if (type==3)
	RecMdcKalTrack::setPidType  (RecMdcKalTrack::kaon);
  HepLorentzVector p4trk = mdcKalTrk->p4(xmass[type]);

  HepVector a = mdcKalTrk->helix();
  HepSymMatrix Ea = mdcKalTrk->err();
  HepPoint3D point0(0., 0., 0.);   // the initial point for MDC recosntruction
  HepPoint3D IP(xorigin[0], xorigin[1], xorigin[2]);
  VFHelix helixip(point0, a, Ea);
  helixip.pivot(IP);
  HepVector vecipa = helixip.a();

  m_Variable0 = p4trk.px();
  m_Variable1 = p4trk.py();
  m_Variable2 = p4trk.pz();
  m_Variable3 = p4trk.rho();
  m_Variable4 = mdcKalTrk->dr() * cos(mdcKalTrk->fi0());
  m_Variable5 = mdcKalTrk->dr() * sin(mdcKalTrk->fi0());
  m_Variable6 = mdcKalTrk->dz();
  m_Variable7 = fabs(vecipa[0]);  //the nearest distance to IP in xy plane
  m_Variable8 = p4trk.cosTheta();
  m_Variable9 = p4trk.e();
  m_Variable10 = track->trackId();
}
void DQADtag::fillPhotonItem(EvtRecTrack* shower, string str)
{
  string strVarName[6];
  for (int i=0;i!=6;i++ )
  {
	strVarName[i] = str + "_" + m_PhotonItemName[i];
	//cout<<"strVarName:	"<<strVarName[i]<<endl;
  }
  NTuple::Item<double> m_Variable0;
  NTuple::Item<double> m_Variable1;
  NTuple::Item<double> m_Variable2;
  NTuple::Item<double> m_Variable3;
  NTuple::Item<double> m_Variable4;
  NTuple::Item<long> m_Variable5;
  m_tuple4->item(strVarName[0],m_Variable0);
  m_tuple4->item(strVarName[1],m_Variable1);
  m_tuple4->item(strVarName[2],m_Variable2);
  m_tuple4->item(strVarName[3],m_Variable3);
  m_tuple4->item(strVarName[4],m_Variable4);
  m_tuple4->item(strVarName[5],m_Variable5);
  RecEmcShower* emcTrk = shower->emcShower();
  m_Variable0 = emcTrk->numHits();
  m_Variable1 = emcTrk->secondMoment();
  m_Variable2 = emcTrk->x();
  m_Variable3 = emcTrk->y();
  m_Variable4 = emcTrk->z();
  m_Variable5 = shower->trackId();
}
void DQADtag::fillPi0Item(SmartRefVector<EvtRecTrack> showers, string str,  DQAPi0Info* pi0info)
{
  string strVarName[7];
  for (int i=0;i!=7;i++ )
  {
	strVarName[i] = str + "_" + m_Pi0ItemName[i];
	//cout<<"strVarName:	"<<strVarName[i]<<endl;
  }
  //DQAPi0Info pi0info;
  //pi0info.setchilds(showers[0],showers[1]);
  //if (!pi0info.calculate()) return false;
  //if (pi0info.m() >m_mpi0Max || pi0info.m() < m_mpi0Min) 
  //iswritten = 0;
  NTuple::Item<double> m_Variable0;
  NTuple::Item<double> m_Variable1;
  NTuple::Item<double> m_Variable2;
  NTuple::Item<double> m_Variable3;
  NTuple::Item<double> m_Variable4;
  NTuple::Item<long> m_Variable5;
  NTuple::Item<long> m_Variable6;
  m_tuple4->item(strVarName[0],m_Variable0);
  m_tuple4->item(strVarName[1],m_Variable1);
  m_tuple4->item(strVarName[2],m_Variable2);
  m_tuple4->item(strVarName[3],m_Variable3);
  m_tuple4->item(strVarName[4],m_Variable4);
  m_tuple4->item(strVarName[5],m_Variable5);
  m_tuple4->item(strVarName[6],m_Variable6);
  m_Variable0 = pi0info->m();
  m_Variable1 = showers[0]->emcShower()->energy();
  m_Variable2 = cos(showers[0]->emcShower()->theta());
  m_Variable3 = showers[1]->emcShower()->energy();
  m_Variable4 = cos(showers[1]->emcShower()->theta());
  m_Variable5 = showers[0]->trackId();
  m_Variable6 = showers[1]->trackId();
}
void DQADtag::fillKsItem(SmartRefVector<EvtRecTrack> tracks, string str,  DQAKsInfo* ksinfo)
{
  string strVarName[6];
  for (int i=0;i!=6;i++ )
  {
	strVarName[i] = str + "_ks_" + m_KsItemName[i];
	//cout<<"strVarName:	"<<strVarName[i]<<endl;
  }
  //DQAKsInfo ksinfo;
  //ksinfo.setchilds(tracks[0],tracks[1]);
  //ksinfo.setVtxPar(m_vxpar);
  //ksinfo.setPrimaryVtxPar(m_privxpar);
  //if (!ksinfo.calculate()) return false;
  NTuple::Item<double> m_Variable0;
  NTuple::Item<double> m_Variable1;
  NTuple::Item<double> m_Variable2;
  NTuple::Item<double> m_Variable3;
  NTuple::Item<double> m_Variable4;
  NTuple::Item<double> m_Variable5;
  m_tuple4->item(strVarName[0],m_Variable0);
  m_tuple4->item(strVarName[1],m_Variable1);
  m_tuple4->item(strVarName[2],m_Variable2);
  m_tuple4->item(strVarName[3],m_Variable3);
  m_tuple4->item(strVarName[4],m_Variable4);
  m_tuple4->item(strVarName[5],m_Variable5);
  m_Variable0 = ksinfo->m();
  m_Variable1 = ksinfo->mksp4();
  m_Variable2 = ksinfo->vtxChi2();
  m_Variable3 = ksinfo->chi2(); 
  m_Variable4 = ksinfo->decayLength();
  m_Variable5 = ksinfo->decayLengthRatio();
}
int DQADtag::fillTrackTofItem(EvtRecTrack* track, string str, int type)
{
  string strVarName[8];
  int numtag(0);
  for (int i=0;i!=8;i++ )
  {
	strVarName[i] = str + "_" + m_TrackTofItemName[i];
	//cout<<"strVarName:	"<<strVarName[i]<<endl;
  }
  NTuple::Array<double> m_Variable0;
  NTuple::Array<double> m_Variable1;
  NTuple::Array<double> m_Variable2;
  NTuple::Item<double> m_Variable3;
  NTuple::Item<double> m_Variable4;
  NTuple::Item<double> m_Variable5;
  NTuple::Item<long> m_Variable6;
  NTuple::Item<long> m_Variable7;
  m_tuple4->item(strVarName[0],m_Variable0);
  m_tuple4->item(strVarName[1],m_Variable1);
  m_tuple4->item(strVarName[2],m_Variable2);
  m_tuple4->item(strVarName[3],m_Variable3);
  m_tuple4->item(strVarName[4],m_Variable4);
  m_tuple4->item(strVarName[5],m_Variable5);
  m_tuple4->item(strVarName[6],m_Variable6);
  m_tuple4->item(strVarName[7],m_Variable7);
  m_Variable6 = track->trackId();
  m_Variable7 = 10;
  for (int i=0;i!=7;i++)
  {
	m_Variable0[i] = 999.9;  
	m_Variable1[i] = 999.9;
	m_Variable2[i] = 999.9;
  }
  m_Variable3 = 999.9; 
  m_Variable4 = 999.9;
  m_Variable5 = 999.9;
  if(!track->isTofTrackValid()) return 0;
  SmartRefVector<RecTofTrack> tofTrkCol = track->tofTrack();
  SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();
  for(;iter_tof != tofTrkCol.end(); iter_tof++ ) {
	TofHitStatus *status = new TofHitStatus; 
	status->setStatus((*iter_tof)->status());
	if(!(status->is_barrel())){//endcap
	  if( !(status->is_counter()) ) continue; // ? 
	  if( status->layer()!=1 ) continue;//layer1
	  m_Variable3 = (*iter_tof)->tof() - (*iter_tof)->texp(1);
	  m_Variable4 = (*iter_tof)->tof() - (*iter_tof)->texp(2);
	  m_Variable5 = (*iter_tof)->tof() - (*iter_tof)->texp(3);
	  m_Variable7 = (status->is_east() + 1);
	}
	else {//barrel
	  if (status->is_cluster()) numtag = 6; 
	  else if (status->is_counter() && status->layer()!=1) numtag = 5;
	  else if (status->is_counter() && status->layer()==1) numtag = 4;
	  else if (status->is_readout() && status->layer()==1 && status->is_east()==1) numtag = 0;
	  else if (status->is_readout() && status->layer()==1 && status->is_east()!=1) numtag = 1;
	  else if (status->is_readout() && status->layer()!=1 && status->is_east()==1) numtag = 2;
	  else if (status->is_readout() && status->layer()!=1 && status->is_east()!=1) numtag = 3;
	  m_Variable0[numtag] = (*iter_tof)->tof() - (*iter_tof)->texp(1);
	  m_Variable1[numtag] = (*iter_tof)->tof() - (*iter_tof)->texp(2);
	  m_Variable2[numtag] = (*iter_tof)->tof() - (*iter_tof)->texp(3);
	}
	delete status;
  }
}
int DQADtag::fillTrackDedxItem(EvtRecTrack* track, string str, int type)
{
  string strVarName[7];
  for (int i=0;i!=7;i++ )
  {
	strVarName[i] = str + "_" + m_TrackDedxItemName[i];
	//cout<<"strVarName:	"<<strVarName[i]<<endl;
  }
  NTuple::Item<double> m_Variable0;
  NTuple::Item<double> m_Variable1;
  NTuple::Item<double> m_Variable2;
  NTuple::Item<double> m_Variable3;
  NTuple::Item<double> m_Variable4;
  NTuple::Item<double> m_Variable5;
  NTuple::Item<long> m_Variable6;
  m_tuple4->item(strVarName[0],m_Variable0);
  m_tuple4->item(strVarName[1],m_Variable1);
  m_tuple4->item(strVarName[2],m_Variable2);
  m_tuple4->item(strVarName[3],m_Variable3);
  m_tuple4->item(strVarName[4],m_Variable4);
  m_tuple4->item(strVarName[5],m_Variable5);
  m_tuple4->item(strVarName[6],m_Variable6);
  m_Variable0 = 999.9;
  m_Variable1 = 999.9;
  m_Variable2 = 1000;
  m_Variable3 = 1000;
  m_Variable4 = 9999.9;
  m_Variable5 = 999.9;
  m_Variable6 = 999.9;
  if (!track->isMdcDedxValid()) return 0;
  RecMdcDedx* dedxTrk = track->mdcDedx();
  m_Variable0 = dedxTrk->chiK();
  m_Variable1 = dedxTrk->chiPi();
  m_Variable2 = dedxTrk->numGoodHits();
  m_Variable3 = dedxTrk->numTotalHits();
  m_Variable4 = dedxTrk->probPH();
  m_Variable5 = dedxTrk->normPH();
  m_Variable6 = track->trackId();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQADtag::finalize() {
  cout<<"* * * * * * * 	Begin DQADTag	* * * * * * * *"<<endl;
  cout<<"total number:         "<<Ncut_DQADTagAlg[0]<<endl;
  cout<<"Has DTag:         "<<Ncut_DQADTagAlg[1]<<endl;
  cout<<"* * * * * * * 	End DQADTag	* * * * * * * *"<<endl;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

