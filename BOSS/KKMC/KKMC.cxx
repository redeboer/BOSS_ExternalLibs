//#include "HepMC/HEPEVT_Wrapper.h"
#include "HepMC/IO_HEPEVT.h"
//#include "HepMC/IO_Ascii.h"
#include "HepMC/GenEvent.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "KKMC/KKMC.h"
#include "KKMC/KKMCRandom.h"
#include "GeneratorObject/McGenEvent.h"
#include "BesKernel/IBesRndmGenSvc.h"
#include "cfortran/cfortran.h"

#include "EventModel/EventHeader.h"

PROTOCCALLSFSUB3(PSEUMAR_INITIALIZE, pseumar_initialize, INT, INT, INT)
#define PSEUMAR_INITIALIZE(ijklin, ntot1n, ntot2n) CCALLSFSUB3(PSEUMAR_INITIALIZE, pseumar_initialize, INT, INT, INT, ijklin, ntot1n, ntot2n);

PROTOCCALLSFSUB1(WHYM_SETDEF, whym_setdef, DOUBLEV)
#define WHYM_SETDEF(XPAR) CCALLSFSUB1(WHYM_SETDEF, whym_setdef, DOUBLEV, XPAR);
PROTOCCALLSFSUB0(MY_PYUPD, my_pyupd)
#define MY_PYUPD() CCALLSFSUB0(MY_PYUPD, my_pyupd);

PROTOCCALLSFSUB1(KK2F_INITIALIZE, kk2f_initialize, DOUBLEV)
#define KK2F_INITIALIZE(XPAR) CCALLSFSUB1(KK2F_INITIALIZE, kk2f_initialize, DOUBLEV, XPAR);

PROTOCCALLSFSUB0(HEPEVT_CLEAN, hepevt_clean)
#define HEPEVT_CLEAN() CCALLSFSUB0(HEPEVT_CLEAN, hepevt_clean);
PROTOCCALLSFSUB0(KK2F_MAKE, kk2f_make)
#define KK2F_MAKE() CCALLSFSUB0(KK2F_MAKE, kk2f_make);

PROTOCCALLSFSUB1(KK2F_GETKEYSKIP, kk2f_getkeyskip, PINT)
#define KK2F_GETKEYSKIP(KEY) CCALLSFSUB1(KK2F_GETKEYSKIP, kk2f_getkeyskip,PINT, KEY);

PROTOCCALLSFSUB1(PSIPP_DDBARCUT, psipp_ddbarcut, PINT)
#define PSIPP_DDBARCUT(KEY) CCALLSFSUB1(PSIPP_DDBARCUT, psipp_ddbarcut, PINT, KEY);

PROTOCCALLSFSUB0(KK2F_FINALIZE, kk2f_finalize)
#define KK2F_FINALIZE() CCALLSFSUB0(KK2F_FINALIZE, kk2f_finalize);
PROTOCCALLSFSUB2(KK2F_GETXSECMC,kk2f_getxsecmc,PDOUBLE, PDOUBLE)
#define KK2F_GETXSECMC(xsecpb, xerrpb) CCALLSFSUB2(KK2F_GETXSECMC, kk2f_getxsecmc, PDOUBLE, PDOUBLE, xsecpb, xerrpb);

PROTOCCALLSFSUB1(PYLIST, pylist, INT)
#define PYLIST(LIST) CCALLSFSUB1(PYLIST, pylist, INT, LIST);

PROTOCCALLSFSUB1(PYHEPC, pyhepc, INT)
#define PYHEPC(ICONV) CCALLSFSUB1(PYHEPC, pyhepc, INT, ICONV);

PROTOCCALLSFSUB1(KK2F_SETEVTGENINTERFACE,kk2f_setevtgeninterface, INT)
#define KK2F_SETEVTGENINTERFACE(KEY) CCALLSFSUB1(KK2F_SETEVTGENINTERFACE,kk2f_setevtgeninterface, INT, KEY);
PROTOCCALLSFSUB1(KK2F_GETEVTGENINTERFACE,kk2f_getevtgeninterface, PINT)
#define KK2F_GETEVTGENINTERFACE(KEY) CCALLSFSUB1(KK2F_GETEVTGENINTERFACE,kk2f_getevtgeninterface, PINT, KEY);

PROTOCCALLSFSUB1(HEPEVT_NUMHEP, hepevt_numhep, PINT)
#define HEPEVT_NUMHEP(Nhep) CCALLSFSUB1(HEPEVT_NUMHEP, hepevt_numhep, PINT, Nhep);
PROTOCCALLSFSUB1(HEPEVT_GETF,hepevt_getf, PINT)
#define HEPEVT_GETF(POS) CCALLSFSUB1(HEPEVT_GETF, hepevt_getf,PINT, POS);  
PROTOCCALLSFSUB1(HEPEVT_GETFBAR,hepevt_getfbar, PINT)
#define HEPEVT_GETFBAR(POS) CCALLSFSUB1(HEPEVT_GETFBAR, hepevt_getfbar,PINT, POS);  
PROTOCCALLSFSUB1(HEPEVT_GETKFFIN,hepevt_getkffin, PINT)
#define HEPEVT_GETKFFIN(KFIN) CCALLSFSUB1(HEPEVT_GETKFFIN, hepevt_getkffin,PINT, KFIN);  
PROTOCCALLSFSUB1(HEPEVT_SETPHOTOSFLAGTRUE, hepevt_setphotosflagtrue, INT)
#define HEPEVT_SETPHOTOSFLAGTRUE(IP) CCALLSFSUB1(HEPEVT_SETPHOTOSFLAGTRUE, hepevt_setphotosflagtrue, INT, IP);
PROTOCCALLSFSUB1(PHOTOS, photos, INT)
#define PHOTOS(IP) CCALLSFSUB1(PHOTOS, photos, INT, IP);
PROTOCCALLSFSUB0(PHOINI, phoini)
#define PHOINI() CCALLSFSUB0(PHOINI, phoini);

PROTOCCALLSFSUB0(TURNOFFTAUDECAY, turnofftaudecay)
#define TURNOFFTAUDECAY() CCALLSFSUB0(TURNOFFTAUDECAY, turnofftaudecay);

PROTOCCALLSFSUB2(PYUPDA,  pyupda, INT, INT)
#define PYUPDA(MUPDA, LFN) CCALLSFSUB2(PYUPDA, pyupda, INT, INT, MUPDA, LFN);

	typedef struct { 
		double ddbarmassCUT; 
	} DDBAR_DEF;
#define DDBARMASS COMMON_BLOCK(DDBAR_DEF, ddbarmass)
COMMON_BLOCK_DEF(DDBAR_DEF, DDBARMASS);

typedef struct { 
	int isrtag,fsrtag; 
} PHOTONTAG_DEF;
#define PHOTONTAG COMMON_BLOCK(PHOTONTAG_DEF,photontag)
COMMON_BLOCK_DEF(PHOTONTAG_DEF,PHOTONTAG);

typedef struct { 
	int ich; 
} MODEXS_DEF;
#define MODEXS COMMON_BLOCK(MODEXS_DEF, modexs)
COMMON_BLOCK_DEF(MODEXS_DEF, MODEXS);

extern "C" {
	extern void pygive_(const char *cnfgstr,int length);
} 

using namespace HepMC;
int KKMC::m_runNo=0;

KKMC::KKMC(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{
	m_numberEvent = 0;

	m_kkseed.clear();
	m_kkseed.push_back(123456);
	m_kkseed.push_back(1);
	m_kkseed.push_back(0);


	declareProperty("NumberOfEventPrinted", m_numberEventPrint=100);
	declareProperty("InitializedSeed", m_kkseed);
	declareProperty("CMSEnergy", m_cmsEnergy = 3.773);
	declareProperty("ReadMeasuredEcms", m_RdMeasuredEcms = false);// Read ecms from database (Wu Lianjin)
	declareProperty("BeamEnergySpread", m_cmsEnergySpread = 0.0013); //it should be beam energy spread,pingrg-2009-09-24
	declareProperty("GenerateResonance", m_generateResonance = true);
	declareProperty("GenerateContinuum", m_generateContinuum = true);
	declareProperty("GenerateDownQuark", m_generateDownQuark = true);
	declareProperty("GenerateUpQuark", m_generateUpQuark = true);
	declareProperty("GenerateStrangeQuark", m_generateStrangeQuark = true);
	declareProperty("GenerateCharmQuark", m_generateCharmQuark = true);
	declareProperty("GenerateBottomQuark", m_generateBottomQuark = false);
	declareProperty("GenerateMuonPair", m_generateMuonPair = true);
	declareProperty("GenerateTauPair", m_generateTauPair = true);
	declareProperty("GenerateRho", m_generateRho = true);
	declareProperty("GenerateOmega", m_generateOmega = true);
	declareProperty("GeneratePhi", m_generatePhi = true);
	declareProperty("GenerateJPsi", m_generateJPsi = true);
	declareProperty("GeneratePsiPrime", m_generatePsiPrime = true);
	declareProperty("GeneratePsi3770", m_generatePsi3770 = true);
	declareProperty("GeneratePsi4030", m_generatePsi4030 = true);
	declareProperty("GeneratePsi4160", m_generatePsi4160 = true);
	declareProperty("GeneratePsi4415", m_generatePsi4415 = true);
	declareProperty("GeneratePsi4260", m_generatePsi4260 = true);
	declareProperty("ThresholdCut", m_DdbarCutPsi3770 = -3.0); //generate DDbar decay, pingrg-2009-10-14
	declareProperty("TagISR", m_isrtag = false);               //Tag ISR photon, false: ID=22, true: ID=-22, pingrg-2010-6-30
	declareProperty("TagFSR", m_fsrtag = false);               //Tag FSR photon, false: ID=22, true: ID=-22, pingrg-2010-6-30
	declareProperty("ModeIndexExpXS", m_ich = -10);  //mode index using the measured cross section  
	//Added by Ke LI (like@ihep.ac.cn), 2015-02-16
	declareProperty("IHVP", m_ihvp= 1);// m_ihvp=0, switch off vacuum polarization;
	//1, 2, 3 for three different caculations (Jegerlehner/Eidelman, Jegerlehner(1988), Burkhardt etal.).
	//By default, is 1.

	m_paramRho.clear(); m_paramRho.push_back(0.77457e0); m_paramRho.push_back(147.65e-3); m_paramRho.push_back(6.89e-6);
	m_paramRh2.clear(); m_paramRh2.push_back(1.465e0); m_paramRh2.push_back(310e-3); m_paramRh2.push_back(0.0e-6);
	m_paramRh3.clear(); m_paramRh3.push_back(1.700e0); m_paramRh3.push_back(240e-3); m_paramRh3.push_back(0.0e-6);
	m_paramOme.clear(); m_paramOme.push_back(0.78194e0);m_paramOme.push_back(8.41e-3);m_paramOme.push_back(0.60e-6);
	m_paramOm2.clear(); m_paramOm2.push_back(1.419e0);m_paramOm2.push_back(174e-3);m_paramOm2.push_back(0.00e-6);
	m_paramOm3.clear(); m_paramOm3.push_back(1.649e0);m_paramOm3.push_back(220e-3);m_paramOm3.push_back(0.00e-6);
	m_paramPhi.clear(); m_paramPhi.push_back(1.01942e0);m_paramPhi.push_back(4.46e-3);m_paramPhi.push_back(1.33e-6);
	m_paramPh2.clear(); m_paramPh2.push_back(1.680e0);m_paramPh2.push_back(150e-3);m_paramPh2.push_back(0.00e-6);
	m_paramPsi.clear(); m_paramPsi.push_back(3.096916e0);m_paramPsi.push_back(0.0929e-3);m_paramPsi.push_back(5.40e-6);
	m_paramPs2.clear(); m_paramPs2.push_back(3.686109e0);m_paramPs2.push_back(0.304e-3);m_paramPs2.push_back(2.12e-6);
	m_paramPs3.clear(); m_paramPs3.push_back(3.77315e0);m_paramPs3.push_back(27.2e-3);m_paramPs3.push_back(0.26e-6);
	m_paramPs4.clear(); m_paramPs4.push_back(4.039e0);m_paramPs4.push_back(80e-3);m_paramPs4.push_back(0.86e-6);
	m_paramPs5.clear(); m_paramPs5.push_back(4.153e0);m_paramPs5.push_back(103e-3);m_paramPs5.push_back(0.83e-6);
	m_paramPs6.clear(); m_paramPs6.push_back(4.421e0);m_paramPs6.push_back(62e-3);m_paramPs6.push_back(0.58e-6);
	m_paramPs7.clear(); m_paramPs7.push_back(4.263e0);m_paramPs7.push_back(95e-3);m_paramPs7.push_back(0.47e-6); 
	m_paramPs8.clear(); m_paramPs8.push_back(3.872e0);m_paramPs8.push_back(100e-3);m_paramPs8.push_back(0.00e-6);
	m_paramUps.clear(); m_paramUps.push_back(9.46030e0); m_paramUps.push_back(0.0525e-3); m_paramUps.push_back(1.32e-6);
	m_paramUp2.clear(); m_paramUp2.push_back(10.02326e0); m_paramUp2.push_back(0.044e-3); m_paramUp2.push_back(0.52e-6);
	m_paramUp3.clear(); m_paramUp3.push_back(10.3552e0); m_paramUp3.push_back(0.026e-3); m_paramUp3.push_back(0.00e-6);
	m_paramUp4.clear(); m_paramUp4.push_back(10.580e0); m_paramUp4.push_back(14e-3); m_paramUp4.push_back(0.248e-6);
	m_paramUp5.clear(); m_paramUp5.push_back(10.865e0); m_paramUp5.push_back(110e-3); m_paramUp5.push_back(0.31e-6);
	m_paramUp6.clear(); m_paramUp6.push_back(11.019); m_paramUp6.push_back(79e-3); m_paramUp6.push_back(0.13e-6);
	m_paramZeta.clear(); m_paramZeta.push_back(91.1876e0); m_paramZeta.push_back(2.4952e0); m_paramZeta.push_back(0.08391e0);
	m_paramW.clear(); m_paramW.push_back(80.43); m_paramW.push_back(2.11e0);

	declareProperty("ResParameterRho", m_paramRho);
	declareProperty("ResParameterRh2", m_paramRh2);
	declareProperty("ResParameterRh3", m_paramRh3);
	declareProperty("ResParameterOme", m_paramOme);
	declareProperty("ResParameterOm2", m_paramOm2);
	declareProperty("ResParameterOm3", m_paramOm3);
	declareProperty("ResParameterPhi", m_paramPhi); 
	declareProperty("ResParameterPh2", m_paramPh2);
	declareProperty("ResParameterPsi", m_paramPsi);
	declareProperty("ResParameterPs2", m_paramPs2);
	declareProperty("ResParameterPs3", m_paramPs3);
	declareProperty("ResParameterPs4", m_paramPs4);
	declareProperty("ResParameterPs5", m_paramPs5);
	declareProperty("ResParameterPs6", m_paramPs6);
	declareProperty("ResParameterPs7", m_paramPs7);  
	declareProperty("ResParameterPs8", m_paramPs8);
	declareProperty("ResParameterUps", m_paramUps);
	declareProperty("ResParameterUp2", m_paramUp2);
	declareProperty("ResParameterUp3", m_paramUp3);
	declareProperty("ResParameterUp4", m_paramUp4);
	declareProperty("ResParameterUp5", m_paramUp5);
	declareProperty("ResParameterUp6", m_paramUp6);
	declareProperty("ResParameterZeta", m_paramZeta);
	declareProperty("ResParameterW", m_paramW);

	// psi(3770) decay
	declareProperty("Psi3770toNonDDb", m_ps3toNonDDb = 0.0);
	declareProperty("Psi3770RatioOfD0toDp", m_ps3D0toDp = 0.563);
	// psi(4030) decay
	declareProperty("Psi4030toD0D0b", m_ps4toD0D0b = 0.0227);
	declareProperty("Psi4030toDpDm", m_ps4toDpDm = 0.0167);
	declareProperty("Psi4030toDsDs", m_ps4toDsDs = 0.0383);
	declareProperty("Psi4030toD0D0Star", m_ps4toD0D0Star = 0.2952);
	declareProperty("Psi4030toDpDmStar", m_ps4toDpDmStar = 0.2764);
	declareProperty("Psi4030toD0StarD0Star", m_ps4toD0StarD0Star=0.2476);
	declareProperty("Psi4030toDpStarDmStar", m_ps4toDpStarDmStar=0.1041);
	// psi(4160) decay
	declareProperty("Psi4160toD0D0b", m_ps5toD0D0b = 0.0190);
	declareProperty("Psi4160toDpDm", m_ps5toDpDm = 0.0180);
	declareProperty("Psi4160toDsDs", m_ps5toDsDs = 0.0488);
	declareProperty("Psi4160toD0D0Star", m_ps5toD0D0Star = 0.1248);
	declareProperty("Psi4160toDpDmStar", m_ps5toDpDmStar = 0.1240);
	declareProperty("Psi4160toDsDsStar", m_ps5toDsDsStar = 0.0820);
	declareProperty("Psi4160toD0StarD0Star", m_ps5toD0StarD0Star=0.3036);
	declareProperty("Psi4160toDpStarDmStar", m_ps5toDpStarDmStar=0.2838);
        //beam polarization vector, pingrg:2016-9-27: confirmed by Z.Was at tao2016 Conference at IHEP
        //for the definition of spin density of beam, see arXiv:9905452
        m_beam1PolVec.clear(); m_beam2PolVec.clear();
        declareProperty("Beam1PolVec", m_beam1PolVec);//e.g. beam1 has polarization 0.5: KKMC.Beam1PolVec={0,0,0.5}
        declareProperty("Beam2PolVec", m_beam2PolVec);
	// interface to EvtGen
	declareProperty("ParticleDecayThroughEvtGen", m_evtGenDecay = true);
	declareProperty("RadiationCorrection", m_radiationCorrection = true);
	//interface set pythia pars
	m_pypars.clear();
	declareProperty("setPythiaPars", m_pypars);
}

StatusCode KKMC::initialize() {

	MsgStream log(msgSvc(), name());

	log << MSG::INFO << "KKMC in initialize()" << endreq;

	//set Bes unified random engine
	static const bool CREATEIFNOTTHERE(true);
	StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);
	if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)
	{
		log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
		return RndmStatus;
	}
	CLHEP::HepRandomEngine* engine  = p_BesRndmGenSvc->GetEngine("KKMC");
	KKMCRandom::setRandomEngine(engine);
	//--- 
	if(m_ich == -2 || m_ich>=0 ) {// ISR exclusive default set particle as Psi(4260)
		m_generatePsi4260 = true;
		m_generateJPsi = 0;
		m_generatePsiPrime = 0;
		m_generatePsi3770 = 0;
		m_generatePsi4030 = 0;
		m_generatePsi4160 = 0;
		m_generatePsi4415 = 0;
	}

	WHYM_SETDEF(xwpar);
	xwpar[0] = m_cmsEnergy;        // c.m.s energy
	xwpar[1] = m_cmsEnergySpread;  // energy spread of c.m.s.
	xwpar[3] = 6.0;                // fortran output unit

	// by Ke LI (like@ihep.ac.cn) 2015-02-16
	xwpar[900]=m_ihvp; // (1,2,3)flag of hadronic vacuum polarization, 0: no vacuum polarization(leptonic and hadronic)
        if(m_beam1PolVec.size()==3){
          xwpar[61]=m_beam1PolVec[0];
          xwpar[62]=m_beam1PolVec[1];
          xwpar[63]=m_beam1PolVec[2];
          xwpar[28]= 1;
        }

        if(m_beam2PolVec.size()==3){
          xwpar[64]=m_beam2PolVec[0];
          xwpar[65]=m_beam2PolVec[1];
          xwpar[66]=m_beam2PolVec[2];
          xwpar[28]= 1;
        }

	if(m_generateResonance)   // flag indicate to generate Resonance data
		xwpar[12] = 1.0;
	else
		xwpar[12] = 0.0;

	if(m_generateContinuum )   // generate continuum
		xwpar[3000] = 1.0;
	else
		xwpar[3000] = 0.0;

	if(m_generateDownQuark)         // d quark production
		xwpar[400] = 1.0;
	else
		xwpar[400] = 0.0;

	if(m_generateUpQuark)         // u quark production
		xwpar[401] = 1.0;
	else
		xwpar[401] = 0.0;

	if(m_generateStrangeQuark)         // s quark production
		xwpar[402] = 1.0;
	else
		xwpar[402] = 0.0;

	if(m_generateCharmQuark)         // c quark production
		xwpar[403] = 1.0;
	else
		xwpar[403] = 0.0;

	if(m_generateBottomQuark)         // b quark production
		xwpar[404] = 1.0;
	else
		xwpar[404] = 0.0;


	if(m_generateMuonPair)     // e+ e- --> mu+ mu- 
		xwpar[412] = 1.0;
	else
		xwpar[412] = 0.0; 

	if(m_generateTauPair)      // e+ e- --> tau+ tau-
		xwpar[414] = 1.0;
	else
		xwpar[414] = 0.0;
	int keyuds = 0;
	if(m_generateRho)     keyuds |= 1;
	if(m_generateOmega)   keyuds |= 2;
	if(m_generatePhi)     keyuds |= 4;

	int keycharm = 0;
	if(m_generateJPsi)     keycharm |=  1;
	if(m_generatePsiPrime) keycharm |=  2;
	if(m_generatePsi3770)  keycharm |=  4;
	if(m_generatePsi4030)  keycharm |=  8;
	if(m_generatePsi4160)  keycharm |= 16;
	if(m_generatePsi4415)  keycharm |= 32;
	if(m_generatePsi4260)  keycharm |= 64;
 
	// resonant parameters 
	int offset = 3100;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramRho[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramRh2[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramRh3[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramOme[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramOm2[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramOm3[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramPhi[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramPh2[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramPsi[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramPs2[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramPs3[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramPs4[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramPs5[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramPs6[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramPs7[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramPs8[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramUps[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramUp2[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramUp3[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramUp4[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramUp5[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramUp6[i];
	offset = offset + 3;
	for(int i = 0; i < 3; i++) xwpar[offset+i] = m_paramZeta[i];
	offset = offset + 3;
	for(int i = 0; i < 2; i++) xwpar[offset+i] = m_paramW[i];
	// offset = offset + 2;

	xwpar[3001] = keyuds + 0.0;
	xwpar[3002] = keycharm + 0.0;

	// psi(3770) decay
	offset = 3200;
	xwpar[offset + 0] = m_ps3toNonDDb;
	xwpar[offset + 1] = m_ps3D0toDp;
	DDBARMASS.ddbarmassCUT= m_DdbarCutPsi3770;
	MODEXS.ich = m_ich;
	//tag ISR or FSR photon
	if(m_isrtag){PHOTONTAG.isrtag=1;} else {PHOTONTAG.isrtag=0;}
	if(m_fsrtag){PHOTONTAG.fsrtag=1;} else {PHOTONTAG.fsrtag=0;}

	// psi(4030) decay
	offset = 3210;
	xwpar[offset + 0] = m_ps4toD0D0b;
	xwpar[offset + 1] = m_ps4toDpDm;
	xwpar[offset + 2] = m_ps4toDsDs;
	xwpar[offset + 3] = m_ps4toD0D0Star;
	xwpar[offset + 4] = m_ps4toDpDmStar;
	xwpar[offset + 5] = m_ps4toD0StarD0Star;
	xwpar[offset + 6] = m_ps4toDpStarDmStar;
	// psi(4160) decay
	offset = 3220;
	xwpar[offset + 0] = m_ps5toD0D0b;
	xwpar[offset + 1] = m_ps5toDpDm;
	xwpar[offset + 2] = m_ps5toDsDs;
	xwpar[offset + 3] = m_ps5toD0D0Star;
	xwpar[offset + 4] = m_ps5toDpDmStar;
	xwpar[offset + 5] = m_ps5toDsDsStar;
	xwpar[offset + 6] = m_ps5toD0StarD0Star;
	xwpar[offset + 7] = m_ps5toDpStarDmStar;

	if(!m_radiationCorrection) {
		xwpar[19] = 0;
		xwpar[20] = 0;
		xwpar[26] = 0;
	}

	KK2F_INITIALIZE(xwpar);
	MY_PYUPD();
	PYUPDA(1, 22);
	//for pythia parameter tunning,pingrg-2013-6-9
	for(int i=0;i<m_pypars.size();i++){
		pygive_(m_pypars[i].c_str(),strlen(m_pypars[i].c_str()));
	}

	if(m_evtGenDecay) {
		KK2F_SETEVTGENINTERFACE(1);
		TURNOFFTAUDECAY();
	} else {
		KK2F_SETEVTGENINTERFACE(0);
		PHOINI();
	}

	HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
	HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
	HepMC::HEPEVT_Wrapper::set_max_number_entries(4000);
	//  std::cout << "max entries = " << HepMC::HEPEVT_Wrapper::max_number_entries() <<std::endl;
	//  std::cout << "size of real = " << HepMC::HEPEVT_Wrapper::sizeof_real() <<std::endl;

	/*** Read Database --Wu Lianjin ***/
	if(m_RdMeasuredEcms){
		StatusCode status=serviceLocator()->service("MeasuredEcmsSvc", ecmsSvc, true);
		if(!status.isSuccess()){
			std::cout<<"ERROR: Can not initial the IMeasuredEcmsSvc right"<<std::endl;
			return status;
		}
	}
	/*** ***/

	log <<MSG::INFO<< "Finish KKMC initialize()" <<endreq;
	return StatusCode::SUCCESS;
}

StatusCode KKMC::execute() {
	SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
	int runNo=eventHeader->runNumber();
	int event=eventHeader->eventNumber();
	bool newRunFlag=0;
	if(runNo != 0 && runNo != m_runNo){m_runNo=runNo;newRunFlag = true;}else{newRunFlag=false;}

	//std::cout<<"runNo= "<<runNo<<" m_runNo "<<m_runNo<<" newRunFlag= "<<newRunFlag<<std::endl;

	/****** Lianjin Wu add ******/
	if(m_RdMeasuredEcms&& newRunFlag){// using cms energy of beam read from database
	  if(!ecmsSvc->isRunNoValid(runNo)){
	    std::cout<<"ERROR: Can not read the MeasuredEcms, try to turn off the ReadEcmsDB"<<std::endl;
	    return StatusCode::FAILURE;
	  }
	  double dbEcms=ecmsSvc->getEcms(runNo);
	  std::cout<<"INFO: Read the MeasuredEcms: "<<dbEcms<<" GeV"<<std::endl;
	  m_cmsEnergy=dbEcms;
	  xwpar[0] = m_cmsEnergy;
	  KK2F_INITIALIZE(xwpar);
	}
	/****** ******/
	//std::cout<<"The beam energy is "<<m_cmsEnergy<<", set for RunNo "<<runNo<<std::endl;

	MsgStream log(msgSvc(), name());

	log << MSG::INFO << "KKMC in execute()" << endreq;


	HepMC::IO_HEPEVT HepEvtIO;


	int KeySkip,iflag;
	do {
		HEPEVT_CLEAN();
		KK2F_MAKE();
		KK2F_GETKEYSKIP(KeySkip);
		PSIPP_DDBARCUT(iflag);
	} while (KeySkip != 0 || iflag ==0);

	int KeyInter;
	KK2F_GETEVTGENINTERFACE(KeyInter);

	//  PSIPP_DDBARCUT(iflag);
	//  if(iflag == 0)  {return StatusCode::SUCCESS;}

	if(KeyInter == 0) {  // make photos correction
		int Pos1, Pos2, KFfin, Nhep;
		HEPEVT_NUMHEP(Nhep);
		HEPEVT_GETF(Pos1);
		HEPEVT_GETFBAR(Pos2);
		HEPEVT_GETKFFIN(KFfin);
		int Posn = Pos1;
		if(Pos2 > Posn) Posn = Pos2;
		Posn = Posn + 1;
		if(KFfin < 10) Posn = Posn + 1;
		for(int ip = Posn; ip <= Nhep; ip++) HEPEVT_SETPHOTOSFLAGTRUE(ip);
		for(int ip = Posn; ip <= Nhep; ip++) PHOTOS(ip);
		PYHEPC(2);
	}

	//sleep(5);

	m_numberEvent += 1;

	if(m_numberEvent <= m_numberEventPrint) PYLIST(1);
	log << MSG::INFO <<"  " <<m_numberEvent<<"th event was generated !!" <<endreq;

	PYHEPC(1);

	HepMC::GenEvent* evt = HepEvtIO.read_next_event();
	evt->set_event_number(m_numberEvent);
	evt->set_signal_process_id(1);
	//  evt->print();

	// Check if the McCollection already exists
	SmartDataPtr<McGenEventCol> anMcCol(eventSvc(), "/Event/Gen");
	if (anMcCol!=0) {
		// Add event to existing collection
		MsgStream log(messageService(), name());
		log << MSG::INFO << "Add McGenEvent to existing collection" << endreq;
		McGenEvent* mcEvent = new McGenEvent(evt);
		anMcCol->push_back(mcEvent);
	}  else {
		// Create Collection and add  to the transient store
		McGenEventCol *mcColl = new McGenEventCol;
		McGenEvent* mcEvent = new McGenEvent(evt);
		mcColl->push_back(mcEvent);
		StatusCode sc = eventSvc()->registerObject("/Event/Gen",mcColl);
		if (sc != StatusCode::SUCCESS) {
			log << MSG::ERROR << "Could not register McGenEvent" << endreq;
			delete mcColl;
			delete evt;
			delete mcEvent;
			return StatusCode::FAILURE;
		}  else {
			//      log << MSG::INFO << "McGenEventCol created and " << npart <<" particles stored in McGenEvent" << endreq;
		}
	}




	return StatusCode::SUCCESS;

}

StatusCode KKMC::finalize() {

	MsgStream log(msgSvc(), name());

	log << MSG::INFO << "KKMC in finalize()" << endreq;

	KK2F_FINALIZE();
	double xSecPb, xErrPb;
	KK2F_GETXSECMC(xSecPb, xErrPb);

	log << MSG::INFO << "Total MC Xsec = " << xSecPb << " +/- " << xErrPb << endreq;
	return StatusCode::SUCCESS;
}


