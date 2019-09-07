#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRefVector.h"

#include "EventModel/EventHeader.h"

#include "McTruth/McParticle.h"

#include "DumpDecayTreeAlg/DumpDecayTreeAlg.h"

#include <vector>
#include <string>
#include <map>

using namespace std;


/////////////////////////////////////////////////////////////////////////////

DumpDecayTreeAlg::DumpDecayTreeAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties  
  declareProperty( "Frequency",  m_freq = 1 );

  declareProperty( "BesEvtGenOnly",   m_BesEvtGenOnly = true );
  declareProperty( "PrintParticles",  m_PrintParticles = true );

  declareProperty( "FindRunEvent",  m_FindRunEvent = false );
  declareProperty( "FindRun",       m_FindRun = -5040 );
  declareProperty( "FindEvent",     m_FindEvent = 20 );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DumpDecayTreeAlg::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  
  StatusCode status;

  m_eventCounter = 0;

  // Fill McIds with names (Created from pdt.table from perl script)
  m_map.insert(make_pair(1,"d"));
  m_map.insert(make_pair(-1,"anti-d"));
  m_map.insert(make_pair(2,"u"));
  m_map.insert(make_pair(-2,"anti-u"));
  m_map.insert(make_pair(3,"s"));
  m_map.insert(make_pair(-3,"anti-s"));
  m_map.insert(make_pair(4,"c"));
  m_map.insert(make_pair(-4,"anti-c"));
  m_map.insert(make_pair(5,"b"));
  m_map.insert(make_pair(-5,"anti-b"));
  m_map.insert(make_pair(6,"t"));
  m_map.insert(make_pair(-6,"anti-t"));
  m_map.insert(make_pair(7,"b'"));
  m_map.insert(make_pair(-7,"anti-b'"));
  m_map.insert(make_pair(8,"t'"));
  m_map.insert(make_pair(-8,"anti-t'"));
  m_map.insert(make_pair(21,"g"));
  m_map.insert(make_pair(11,"e-"));
  m_map.insert(make_pair(-11,"e+"));
  m_map.insert(make_pair(12,"nu_e"));
  m_map.insert(make_pair(-12,"anti-nu_e"));
  m_map.insert(make_pair(13,"mu-"));
  m_map.insert(make_pair(-13,"mu+"));
  m_map.insert(make_pair(14,"nu_mu"));
  m_map.insert(make_pair(-14,"anti-nu_mu"));
  m_map.insert(make_pair(15,"tau-"));
  m_map.insert(make_pair(-15,"tau+"));
  m_map.insert(make_pair(16,"nu_tau"));
  m_map.insert(make_pair(-16,"anti-nu_tau"));
  m_map.insert(make_pair(17,"L-"));
  m_map.insert(make_pair(-17,"L+"));
  m_map.insert(make_pair(18,"nu_L"));
  m_map.insert(make_pair(-18,"anti-nu_L"));
  m_map.insert(make_pair(22,"gamma"));
  m_map.insert(make_pair(-22,"gammaFSR"));
  m_map.insert(make_pair(10022,"vpho"));
  m_map.insert(make_pair(20022,"Cerenkov"));
  m_map.insert(make_pair(23,"Z0"));
  m_map.insert(make_pair(24,"W+"));
  m_map.insert(make_pair(-24,"W-"));
  m_map.insert(make_pair(25,"Higgs0"));
  m_map.insert(make_pair(28,"reggeon"));
  m_map.insert(make_pair(29,"pomeron"));
  m_map.insert(make_pair(32,"Z'0"));
  m_map.insert(make_pair(33,"Z''0"));
  m_map.insert(make_pair(34,"W'+"));
  m_map.insert(make_pair(-34,"W'-"));
  m_map.insert(make_pair(35,"Higgs'0"));
  m_map.insert(make_pair(36,"A0"));
  m_map.insert(make_pair(37,"Higgs+"));
  m_map.insert(make_pair(-37,"Higgs-"));
  m_map.insert(make_pair(40,"R0"));
  m_map.insert(make_pair(-40,"anti-R0"));
  m_map.insert(make_pair(41,"Xu0"));
  m_map.insert(make_pair(42,"Xu+"));
  m_map.insert(make_pair(-42,"Xu-"));
  m_map.insert(make_pair(81,"specflav"));
  m_map.insert(make_pair(82,"rndmflav"));
  m_map.insert(make_pair(-82,"anti-rndmflav"));
  m_map.insert(make_pair(83,"phasespa"));
  m_map.insert(make_pair(84,"c-hadron"));
  m_map.insert(make_pair(-84,"anti-c-hadron"));
  m_map.insert(make_pair(85,"b-hadron"));
  m_map.insert(make_pair(-85,"anti-b-hadron"));
  m_map.insert(make_pair(86,"t-hadron"));
  m_map.insert(make_pair(-86,"anti-t-hadron"));
  m_map.insert(make_pair(87,"b'-hadron"));
  m_map.insert(make_pair(-87,"anti-b'-hadron"));
  m_map.insert(make_pair(88,"t'-hadron"));
  m_map.insert(make_pair(-88,"anti-t'-hadron"));
  m_map.insert(make_pair(89,"Wvirt+"));
  m_map.insert(make_pair(-89,"Wvirt-"));
  m_map.insert(make_pair(90,"diquark"));
  m_map.insert(make_pair(-90,"anti-diquark"));
  m_map.insert(make_pair(91,"cluster"));
  m_map.insert(make_pair(92,"string"));
  m_map.insert(make_pair(93,"indep"));
  m_map.insert(make_pair(94,"CMshower"));
  m_map.insert(make_pair(95,"SPHEaxis"));
  m_map.insert(make_pair(96,"THRUaxis"));
  m_map.insert(make_pair(97,"CLUSjet"));
  m_map.insert(make_pair(98,"CELLjet"));
  m_map.insert(make_pair(99,"table"));
  m_map.insert(make_pair(111,"pi0"));
  m_map.insert(make_pair(211,"pi+"));
  m_map.insert(make_pair(-211,"pi-"));
  m_map.insert(make_pair(210,"pi_diff+"));
  m_map.insert(make_pair(-210,"pi_diff-"));
  m_map.insert(make_pair(20111,"pi(2S)0"));
  m_map.insert(make_pair(20211,"pi(2S)+"));
  m_map.insert(make_pair(-20211,"pi(2S)-"));
  m_map.insert(make_pair(221,"eta"));
  m_map.insert(make_pair(20221,"eta(2S)"));
  m_map.insert(make_pair(331,"eta'"));
  m_map.insert(make_pair(113,"rho0"));
  m_map.insert(make_pair(110,"rho_diff0"));
  m_map.insert(make_pair(213,"rho+"));
  m_map.insert(make_pair(-213,"rho-"));
  m_map.insert(make_pair(30113,"rho(2S)0"));
  m_map.insert(make_pair(30213,"rho(2S)+"));
  m_map.insert(make_pair(-30213,"rho(2S)-"));
  m_map.insert(make_pair(40113,"rho(3S)0"));
  m_map.insert(make_pair(40213,"rho(3S)+"));
  m_map.insert(make_pair(-40213,"rho(3S)-"));
  m_map.insert(make_pair(223,"omega"));
  m_map.insert(make_pair(220,"omega_diff"));
  m_map.insert(make_pair(30223,"omega(2S)"));
  m_map.insert(make_pair(333,"phi"));
  m_map.insert(make_pair(330,"phi_diff"));
  m_map.insert(make_pair(10111,"a_00"));
  m_map.insert(make_pair(10211,"a_0+"));
  m_map.insert(make_pair(-10211,"a_0-"));
  m_map.insert(make_pair(9010221,"f_0"));
  m_map.insert(make_pair(10221,"f'_0"));
  m_map.insert(make_pair(10113,"b_10"));
  m_map.insert(make_pair(10213,"b_1+"));
  m_map.insert(make_pair(-10213,"b_1-"));
  m_map.insert(make_pair(10223,"h_1"));
  m_map.insert(make_pair(10333,"h'_1"));
  m_map.insert(make_pair(20113,"a_10"));
  m_map.insert(make_pair(20213,"a_1+"));
  m_map.insert(make_pair(-20213,"a_1-"));
  m_map.insert(make_pair(20223,"f_1"));
  m_map.insert(make_pair(115,"a_20"));
  m_map.insert(make_pair(215,"a_2+"));
  m_map.insert(make_pair(-215,"a_2-"));
  m_map.insert(make_pair(225,"f_2"));
  m_map.insert(make_pair(50221,"f_0(1500)"));
  m_map.insert(make_pair(335,"f'_2"));
  m_map.insert(make_pair(9020221,"eta(1405)"));
  m_map.insert(make_pair(10335,"eta2(1870)"));
  m_map.insert(make_pair(10331,"f_0(1710)"));
  m_map.insert(make_pair(229,"f_4(2050)"));
  m_map.insert(make_pair(20333,"f'_1"));
  m_map.insert(make_pair(8888888,"f_0(1790)"));
  m_map.insert(make_pair(9000223,"f_1(1510)"));
  m_map.insert(make_pair(9050225,"f_2(1950)"));
  m_map.insert(make_pair(9080221,"eta(2225)"));
  m_map.insert(make_pair(9040221,"eta(1760)"));
  m_map.insert(make_pair(9999999,"x(1835)"));
  m_map.insert(make_pair(311,"K0"));
  m_map.insert(make_pair(-311,"anti-K0"));
  m_map.insert(make_pair(310,"K_S0"));
  m_map.insert(make_pair(130,"K_L0"));
  m_map.insert(make_pair(321,"K+"));
  m_map.insert(make_pair(-321,"K-"));
  m_map.insert(make_pair(313,"K*0"));
  m_map.insert(make_pair(-313,"anti-K*0"));
  m_map.insert(make_pair(323,"K*+"));
  m_map.insert(make_pair(-323,"K*-"));
  m_map.insert(make_pair(10311,"K_0*0"));
  m_map.insert(make_pair(-10311,"anti-K_0*0"));
  m_map.insert(make_pair(10321,"K_0*+"));
  m_map.insert(make_pair(-10321,"K_0*-"));
  m_map.insert(make_pair(10313,"K_10"));
  m_map.insert(make_pair(-10313,"anti-K_10"));
  m_map.insert(make_pair(10323,"K_1+"));
  m_map.insert(make_pair(-10323,"K_1-"));
  m_map.insert(make_pair(315,"K_2*0"));
  m_map.insert(make_pair(-315,"anti-K_2*0"));
  m_map.insert(make_pair(325,"K_2*+"));
  m_map.insert(make_pair(-325,"K_2*-"));
  m_map.insert(make_pair(20313,"K'_10"));
  m_map.insert(make_pair(-20313,"anti-K'_10"));
  m_map.insert(make_pair(20323,"K'_1+"));
  m_map.insert(make_pair(-20323,"K'_1-"));
  m_map.insert(make_pair(100313,"K'*0"));
  m_map.insert(make_pair(-100313,"anti-K'*0"));
  m_map.insert(make_pair(100323,"K'*+"));
  m_map.insert(make_pair(-100323,"K'*-"));
  m_map.insert(make_pair(30313,"K''*0"));
  m_map.insert(make_pair(-30313,"anti-K''*0"));
  m_map.insert(make_pair(30323,"K''*+"));
  m_map.insert(make_pair(-30323,"K''*-"));
  m_map.insert(make_pair(317,"K_3*0"));
  m_map.insert(make_pair(-317,"anti-K_3*0"));
  m_map.insert(make_pair(327,"K_3*+"));
  m_map.insert(make_pair(-327,"K_3*-"));
  m_map.insert(make_pair(319,"K_4*0"));
  m_map.insert(make_pair(-319,"anti-K_4*0"));
  m_map.insert(make_pair(329,"K_4*+"));
  m_map.insert(make_pair(-329,"K_4*-"));
  m_map.insert(make_pair(411,"D+"));
  m_map.insert(make_pair(-411,"D-"));
  m_map.insert(make_pair(421,"D0"));
  m_map.insert(make_pair(-421,"anti-D0"));
  m_map.insert(make_pair(413,"D*+"));
  m_map.insert(make_pair(-413,"D*-"));
  m_map.insert(make_pair(423,"D*0"));
  m_map.insert(make_pair(-423,"anti-D*0"));
  m_map.insert(make_pair(10411,"D_0*+"));
  m_map.insert(make_pair(-10411,"D_0*-"));
  m_map.insert(make_pair(10421,"D_0*0"));
  m_map.insert(make_pair(-10421,"anti-D_0*0"));
  m_map.insert(make_pair(10413,"D_1+"));
  m_map.insert(make_pair(-10413,"D_1-"));
  m_map.insert(make_pair(10423,"D_10"));
  m_map.insert(make_pair(-10423,"anti-D_10"));
  m_map.insert(make_pair(415,"D_2*+"));
  m_map.insert(make_pair(-415,"D_2*-"));
  m_map.insert(make_pair(425,"D_2*0"));
  m_map.insert(make_pair(-425,"anti-D_2*0"));
  m_map.insert(make_pair(20413,"D'_1+"));
  m_map.insert(make_pair(-20413,"D'_1-"));
  m_map.insert(make_pair(20423,"D'_10"));
  m_map.insert(make_pair(-20423,"anti-D'_10"));
  m_map.insert(make_pair(431,"D_s+"));
  m_map.insert(make_pair(-431,"D_s-"));
  m_map.insert(make_pair(433,"D_s*+"));
  m_map.insert(make_pair(-433,"D_s*-"));
  m_map.insert(make_pair(10431,"D_s0*+"));
  m_map.insert(make_pair(-10431,"D_s0*-"));
  m_map.insert(make_pair(10433,"D_s1+"));
  m_map.insert(make_pair(-10433,"D_s1-"));
  m_map.insert(make_pair(435,"D_s2*+"));
  m_map.insert(make_pair(-435,"D_s2*-"));
  m_map.insert(make_pair(20433,"D'_s1+"));
  m_map.insert(make_pair(-20433,"D'_s1-"));
  m_map.insert(make_pair(30411,"D(2S)+"));
  m_map.insert(make_pair(-30411,"D(2S)-"));
  m_map.insert(make_pair(30421,"D(2S)0"));
  m_map.insert(make_pair(-30421,"anti-D(2S)0"));
  m_map.insert(make_pair(30413,"D*(2S)+"));
  m_map.insert(make_pair(-30413,"D*(2S)-"));
  m_map.insert(make_pair(30423,"D*(2S)0"));
  m_map.insert(make_pair(-30423,"anti-D*(2S)0"));
  m_map.insert(make_pair(511,"B0"));
  m_map.insert(make_pair(-511,"anti-B0"));
  m_map.insert(make_pair(150,"B0L"));
  m_map.insert(make_pair(510,"B0H"));
  m_map.insert(make_pair(521,"B+"));
  m_map.insert(make_pair(-521,"B-"));
  m_map.insert(make_pair(513,"B*0"));
  m_map.insert(make_pair(-513,"anti-B*0"));
  m_map.insert(make_pair(523,"B*+"));
  m_map.insert(make_pair(-523,"B*-"));
  m_map.insert(make_pair(10511,"B_0*0"));
  m_map.insert(make_pair(-10511,"anti-B_0*0"));
  m_map.insert(make_pair(10521,"B_0*+"));
  m_map.insert(make_pair(-10521,"B_0*-"));
  m_map.insert(make_pair(10513,"B_10"));
  m_map.insert(make_pair(-10513,"anti-B_10"));
  m_map.insert(make_pair(10523,"B_1+"));
  m_map.insert(make_pair(-10523,"B_1-"));
  m_map.insert(make_pair(515,"B_2*0"));
  m_map.insert(make_pair(-515,"anti-B_2*0"));
  m_map.insert(make_pair(525,"B_2*+"));
  m_map.insert(make_pair(-525,"B_2*-"));
  m_map.insert(make_pair(20513,"B'_10"));
  m_map.insert(make_pair(-20513,"anti-B'_10"));
  m_map.insert(make_pair(20523,"B'_1+"));
  m_map.insert(make_pair(-20523,"B'_1-"));
  m_map.insert(make_pair(531,"B_s0"));
  m_map.insert(make_pair(-531,"anti-B_s0"));
  m_map.insert(make_pair(350,"B_s0L"));
  m_map.insert(make_pair(530,"B_s0H"));
  m_map.insert(make_pair(533,"B_s*0"));
  m_map.insert(make_pair(-533,"anti-B_s*0"));
  m_map.insert(make_pair(10531,"B_s0*0"));
  m_map.insert(make_pair(-10531,"anti-B_s0*0"));
  m_map.insert(make_pair(10533,"B_s10"));
  m_map.insert(make_pair(-10533,"anti-B_s10"));
  m_map.insert(make_pair(535,"B_s2*0"));
  m_map.insert(make_pair(-535,"anti-B_s2*0"));
  m_map.insert(make_pair(20533,"B'_s10"));
  m_map.insert(make_pair(-20533,"anti-B'_s10"));
  m_map.insert(make_pair(541,"B_c+"));
  m_map.insert(make_pair(-541,"B_c-"));
  m_map.insert(make_pair(543,"B_c*+"));
  m_map.insert(make_pair(-543,"B_c*-"));
  m_map.insert(make_pair(10541,"B_c0*+"));
  m_map.insert(make_pair(-10541,"B_c0*-"));
  m_map.insert(make_pair(10543,"B_c1+"));
  m_map.insert(make_pair(-10543,"B_c1-"));
  m_map.insert(make_pair(545,"B_c2*+"));
  m_map.insert(make_pair(-545,"B_c2*-"));
  m_map.insert(make_pair(20543,"B'_c1+"));
  m_map.insert(make_pair(-20543,"B'_c1-"));
  m_map.insert(make_pair(441,"eta_c"));
  m_map.insert(make_pair(20441,"eta_c(2S)"));
  m_map.insert(make_pair(443,"J/psi"));
  m_map.insert(make_pair(440,"psi_diff"));
  m_map.insert(make_pair(100443,"psi(2S)"));
  m_map.insert(make_pair(30443,"psi(3770)"));
  m_map.insert(make_pair(9000443,"psi(4040)"));
  m_map.insert(make_pair(9010443,"psi(4160)"));
  m_map.insert(make_pair(9020443,"psi(4415)"));
  m_map.insert(make_pair(10443,"h_c"));
  m_map.insert(make_pair(10441,"chi_c0"));
  m_map.insert(make_pair(20443,"chi_c1"));
  m_map.insert(make_pair(445,"chi_c2"));
  m_map.insert(make_pair(551,"eta_b"));
  m_map.insert(make_pair(20551,"eta_b(2S)"));
  m_map.insert(make_pair(40551,"eta_b(3S)"));
  m_map.insert(make_pair(553,"Upsilon"));
  m_map.insert(make_pair(30553,"Upsilon(2S)"));
  m_map.insert(make_pair(60553,"Upsilon(3S)"));
  m_map.insert(make_pair(70553,"Upsilon(4S)"));
  m_map.insert(make_pair(80553,"Upsilon(5S)"));
  m_map.insert(make_pair(10553,"h_b"));
  m_map.insert(make_pair(40553,"h_b(2P)"));
  m_map.insert(make_pair(100553,"h_b(3P)"));
  m_map.insert(make_pair(10551,"chi_b0"));
  m_map.insert(make_pair(20553,"chi_b1"));
  m_map.insert(make_pair(555,"chi_b2"));
  m_map.insert(make_pair(30551,"chi_b0(2P)"));
  m_map.insert(make_pair(50553,"chi_b1(2P)"));
  m_map.insert(make_pair(10555,"chi_b2(2P)"));
  m_map.insert(make_pair(50551,"chi_b0(3P)"));
  m_map.insert(make_pair(110553,"chi_b1(3P)"));
  m_map.insert(make_pair(20555,"chi_b2(3P)"));
  m_map.insert(make_pair(40555,"eta_b2(1D)"));
  m_map.insert(make_pair(60555,"eta_b2(2D)"));
  m_map.insert(make_pair(120553,"Upsilon_1(1D)"));
  m_map.insert(make_pair(30555,"Upsilon_2(1D)"));
  m_map.insert(make_pair(557,"Upsilon_3(1D)"));
  m_map.insert(make_pair(130553,"Upsilon_1(2D)"));
  m_map.insert(make_pair(50555,"Upsilon_2(2D)"));
  m_map.insert(make_pair(10557,"Upsilon_3(2D)"));
  m_map.insert(make_pair(10222,"sigma_0"));
  m_map.insert(make_pair(1114,"Delta-"));
  m_map.insert(make_pair(-1114,"anti-Delta+"));
  m_map.insert(make_pair(2110,"n_diffr"));
  m_map.insert(make_pair(-2110,"anti-n_diffr"));
  m_map.insert(make_pair(2112,"n0"));
  m_map.insert(make_pair(-2112,"anti-n0"));
  m_map.insert(make_pair(2114,"Delta0"));
  m_map.insert(make_pair(-2114,"anti-Delta0"));
  m_map.insert(make_pair(2210,"p_diff+"));
  m_map.insert(make_pair(-2210,"anti-p_diff-"));
  m_map.insert(make_pair(2212,"p+"));
  m_map.insert(make_pair(-2212,"anti-p-"));
  m_map.insert(make_pair(2214,"Delta+"));
  m_map.insert(make_pair(-2214,"anti-Delta-"));
  m_map.insert(make_pair(2224,"Delta++"));
  m_map.insert(make_pair(-2224,"anti-Delta--"));
  m_map.insert(make_pair(3112,"Sigma-"));
  m_map.insert(make_pair(-3112,"anti-Sigma+"));
  m_map.insert(make_pair(3114,"Sigma*-"));
  m_map.insert(make_pair(-3114,"anti-Sigma*+"));
  m_map.insert(make_pair(3122,"Lambda0"));
  m_map.insert(make_pair(-3122,"anti-Lambda0"));
  m_map.insert(make_pair(13122,"Lambda(1405)0"));
  m_map.insert(make_pair(-13122,"anti-Lambda(1405)0"));
  m_map.insert(make_pair(3124,"Lambda(1520)0"));
  m_map.insert(make_pair(-3124,"anti-Lambda(1520)0"));
  m_map.insert(make_pair(23122,"Lambda(1600)0"));
  m_map.insert(make_pair(-23122,"anti-Lambda(1600)0"));
  m_map.insert(make_pair(33122,"Lambda(1670)0"));
  m_map.insert(make_pair(-33122,"anti-Lambda(1670)0"));
  m_map.insert(make_pair(13124,"Lambda(1690)0"));
  m_map.insert(make_pair(-13124,"anti-Lambda(1690)0"));
  m_map.insert(make_pair(43122,"Lambda(1800)0"));
  m_map.insert(make_pair(-43122,"anti-Lambda(1800)0"));
  m_map.insert(make_pair(53122,"Lambda(1810)0"));
  m_map.insert(make_pair(-53122,"anti-Lambda(1810)0"));
  m_map.insert(make_pair(3126,"Lambda(1820)0"));
  m_map.insert(make_pair(-3126,"anti-Lambda(1820)0"));
  m_map.insert(make_pair(13126,"Lambda(1830)0"));
  m_map.insert(make_pair(-13126,"anti-Lambda(1830)0"));
  m_map.insert(make_pair(13212,"Sigma(1660)0"));
  m_map.insert(make_pair(-13212,"anti-Sigma(1660)0"));
  m_map.insert(make_pair(13214,"Sigma(1670)0"));
  m_map.insert(make_pair(-13214,"anti-Sigma(1670)0"));
  m_map.insert(make_pair(23212,"Sigma(1750)0"));
  m_map.insert(make_pair(-23212,"anti-Sigma(1750)0"));
  m_map.insert(make_pair(3216,"Sigma(1775)0"));
  m_map.insert(make_pair(-3216,"anti-Sigma(1775)0"));
  m_map.insert(make_pair(3212,"Sigma0"));
  m_map.insert(make_pair(-3212,"anti-Sigma0"));
  m_map.insert(make_pair(3214,"Sigma*0"));
  m_map.insert(make_pair(-3214,"anti-Sigma*0"));
  m_map.insert(make_pair(3222,"Sigma+"));
  m_map.insert(make_pair(-3222,"anti-Sigma-"));
  m_map.insert(make_pair(3224,"Sigma*+"));
  m_map.insert(make_pair(-3224,"anti-Sigma*-"));
  m_map.insert(make_pair(3312,"Xi-"));
  m_map.insert(make_pair(-3312,"anti-Xi+"));
  m_map.insert(make_pair(3314,"Xi*-"));
  m_map.insert(make_pair(-3314,"anti-Xi*+"));
  m_map.insert(make_pair(3322,"Xi0"));
  m_map.insert(make_pair(-3322,"anti-Xi0"));
  m_map.insert(make_pair(3324,"Xi*0"));
  m_map.insert(make_pair(-3324,"anti-Xi*0"));
  m_map.insert(make_pair(3334,"Omega-"));
  m_map.insert(make_pair(-3334,"anti-Omega+"));
  m_map.insert(make_pair(14122,"Lambda_c(2593)+"));
  m_map.insert(make_pair(-14122,"anti-Lambda_c(2593)-"));
  m_map.insert(make_pair(14124,"Lambda_c(2625)+"));
  m_map.insert(make_pair(-14124,"anti-Lambda_c(2625)-"));
  m_map.insert(make_pair(4112,"Sigma_c0"));
  m_map.insert(make_pair(-4112,"anti-Sigma_c0"));
  m_map.insert(make_pair(4114,"Sigma_c*0"));
  m_map.insert(make_pair(-4114,"anti-Sigma_c*0"));
  m_map.insert(make_pair(4212,"Sigma_c+"));
  m_map.insert(make_pair(-4212,"anti-Sigma_c-"));
  m_map.insert(make_pair(4214,"Sigma_c*+"));
  m_map.insert(make_pair(-4214,"anti-Sigma_c*-"));
  m_map.insert(make_pair(4222,"Sigma_c++"));
  m_map.insert(make_pair(-4222,"anti-Sigma_c--"));
  m_map.insert(make_pair(4224,"Sigma_c*++"));
  m_map.insert(make_pair(-4224,"anti-Sigma_c*--"));
  m_map.insert(make_pair(4312,"Xi'_c0"));
  m_map.insert(make_pair(-4312,"anti-Xi'_c0"));
  m_map.insert(make_pair(4322,"Xi'_c+"));
  m_map.insert(make_pair(-4322,"anti-Xi'_c-"));
  m_map.insert(make_pair(4324,"Xi_c*+"));
  m_map.insert(make_pair(-4324,"anti-Xi_c*-"));
  m_map.insert(make_pair(4122,"Lambda_c+"));
  m_map.insert(make_pair(-4122,"anti-Lambda_c-"));
  m_map.insert(make_pair(4132,"Xi_c0"));
  m_map.insert(make_pair(-4132,"anti-Xi_c0"));
  m_map.insert(make_pair(4232,"Xi_c+"));
  m_map.insert(make_pair(-4232,"anti-Xi_c-"));
  m_map.insert(make_pair(4314,"Xi_c*0"));
  m_map.insert(make_pair(-4314,"anti-Xi_c*0"));
  m_map.insert(make_pair(4332,"Omega_c0"));
  m_map.insert(make_pair(-4332,"anti-Omega_c0"));
  m_map.insert(make_pair(4334,"Omega_c*0"));
  m_map.insert(make_pair(-4334,"anti-Omega_c*0"));
  m_map.insert(make_pair(5112,"Sigma_b-"));
  m_map.insert(make_pair(-5112,"anti-Sigma_b+"));
  m_map.insert(make_pair(5114,"Sigma_b*-"));
  m_map.insert(make_pair(-5114,"anti-Sigma_b*+"));
  m_map.insert(make_pair(5122,"Lambda_b0"));
  m_map.insert(make_pair(-5122,"anti-Lambda_b0"));
  m_map.insert(make_pair(5132,"Xi_b-"));
  m_map.insert(make_pair(-5132,"anti-Xi_b+"));
  m_map.insert(make_pair(5212,"Sigma_b0"));
  m_map.insert(make_pair(-5212,"anti-Sigma_b0"));
  m_map.insert(make_pair(5214,"Sigma_b*0"));
  m_map.insert(make_pair(-5214,"anti-Sigma_b*0"));
  m_map.insert(make_pair(5222,"Sigma_b+"));
  m_map.insert(make_pair(-5222,"anti-Sigma_b-"));
  m_map.insert(make_pair(5224,"Sigma_b*+"));
  m_map.insert(make_pair(-5224,"anti-Sigma_b*-"));
  m_map.insert(make_pair(5232,"Xi_b0"));
  m_map.insert(make_pair(-5232,"anti-Xi_b0"));
  m_map.insert(make_pair(5312,"Xi'_b-"));
  m_map.insert(make_pair(-5312,"anti-Xi'_b+"));
  m_map.insert(make_pair(5314,"Xi_b*-"));
  m_map.insert(make_pair(-5314,"anti-Xi_b*+"));
  m_map.insert(make_pair(5322,"Xi'_b0"));
  m_map.insert(make_pair(-5322,"anti-Xi'_b0"));
  m_map.insert(make_pair(5324,"Xi_b*0"));
  m_map.insert(make_pair(-5324,"anti-Xi_b*0"));
  m_map.insert(make_pair(5332,"Omega_b-"));
  m_map.insert(make_pair(-5332,"anti-Omega_b+"));
  m_map.insert(make_pair(5334,"Omega_b*-"));
  m_map.insert(make_pair(-5334,"anti-Omega_b*+"));
  m_map.insert(make_pair(1101,"dd_0"));
  m_map.insert(make_pair(-1101,"anti-dd_0"));
  m_map.insert(make_pair(2101,"ud_0"));
  m_map.insert(make_pair(-2101,"anti-ud_0"));
  m_map.insert(make_pair(2201,"uu_0"));
  m_map.insert(make_pair(-2201,"anti-uu_0"));
  m_map.insert(make_pair(3101,"sd_0"));
  m_map.insert(make_pair(-3101,"anti-sd_0"));
  m_map.insert(make_pair(3201,"su_0"));
  m_map.insert(make_pair(-3201,"anti-su_0"));
  m_map.insert(make_pair(3301,"ss_0"));
  m_map.insert(make_pair(-3301,"anti-ss_0"));
  m_map.insert(make_pair(4101,"cd_0"));
  m_map.insert(make_pair(-4101,"anti-cd_0"));
  m_map.insert(make_pair(4201,"cu_0"));
  m_map.insert(make_pair(-4201,"anti-cu_0"));
  m_map.insert(make_pair(4301,"cs_0"));
  m_map.insert(make_pair(-4301,"anti-cs_0"));
  m_map.insert(make_pair(4401,"cc_0"));
  m_map.insert(make_pair(-4401,"anti-cc_0"));
  m_map.insert(make_pair(5101,"bd_0"));
  m_map.insert(make_pair(-5101,"anti-bd_0"));
  m_map.insert(make_pair(5201,"bu_0"));
  m_map.insert(make_pair(-5201,"anti-bu_0"));
  m_map.insert(make_pair(5301,"bs_0"));
  m_map.insert(make_pair(-5301,"anti-bs_0"));
  m_map.insert(make_pair(5401,"bc_0"));
  m_map.insert(make_pair(-5401,"anti-bc_0"));
  m_map.insert(make_pair(5501,"bb_0"));
  m_map.insert(make_pair(-5501,"anti-bb_0"));
  m_map.insert(make_pair(1103,"dd_1"));
  m_map.insert(make_pair(-1103,"anti-dd_1"));
  m_map.insert(make_pair(2103,"ud_1"));
  m_map.insert(make_pair(-2103,"anti-ud_1"));
  m_map.insert(make_pair(2203,"uu_1"));
  m_map.insert(make_pair(-2203,"anti-uu_1"));
  m_map.insert(make_pair(3103,"sd_1"));
  m_map.insert(make_pair(-3103,"anti-sd_1"));
  m_map.insert(make_pair(3203,"su_1"));
  m_map.insert(make_pair(-3203,"anti-su_1"));
  m_map.insert(make_pair(3303,"ss_1"));
  m_map.insert(make_pair(-3303,"anti-ss_1"));
  m_map.insert(make_pair(4103,"cd_1"));
  m_map.insert(make_pair(-4103,"anti-cd_1"));
  m_map.insert(make_pair(4203,"cu_1"));
  m_map.insert(make_pair(-4203,"anti-cu_1"));
  m_map.insert(make_pair(4303,"cs_1"));
  m_map.insert(make_pair(-4303,"anti-cs_1"));
  m_map.insert(make_pair(4403,"cc_1"));
  m_map.insert(make_pair(-4403,"anti-cc_1"));
  m_map.insert(make_pair(5103,"bd_1"));
  m_map.insert(make_pair(-5103,"anti-bd_1"));
  m_map.insert(make_pair(5203,"bu_1"));
  m_map.insert(make_pair(-5203,"anti-bu_1"));
  m_map.insert(make_pair(5303,"bs_1"));
  m_map.insert(make_pair(-5303,"anti-bs_1"));
  m_map.insert(make_pair(5403,"bc_1"));
  m_map.insert(make_pair(-5403,"anti-bc_1"));
  m_map.insert(make_pair(5503,"bb_1"));
  m_map.insert(make_pair(-5503,"anti-bb_1"));
  m_map.insert(make_pair(1011,"deuteron"));
  m_map.insert(make_pair(-1011,"anti-deuteron"));
  m_map.insert(make_pair(1021,"tritium"));
  m_map.insert(make_pair(-1021,"anti-tritium"));
  m_map.insert(make_pair(1012,"He3"));
  m_map.insert(make_pair(-1012,"anti-He3"));
  m_map.insert(make_pair(1022,"alpha"));
  m_map.insert(make_pair(-1022,"anti-alpha"));
  m_map.insert(make_pair(100,"geantino"));
  m_map.insert(make_pair(101,"chargedgeantino"));
  m_map.insert(make_pair(30343,"Xsd"));
  m_map.insert(make_pair(-30343,"anti-Xsd"));
  m_map.insert(make_pair(30353,"Xsu"));
  m_map.insert(make_pair(-30353,"anti-Xsu"));
  m_map.insert(make_pair(30373,"Xdd"));
  m_map.insert(make_pair(-30373,"anti-Xdd"));
  m_map.insert(make_pair(30383,"Xdu"));
  m_map.insert(make_pair(-30383,"anti-Xdu"));
  m_map.insert(make_pair(30363,"Xss"));
  m_map.insert(make_pair(-30363,"anti-Xss"));
  m_map.insert(make_pair(51,"dummy00_1"));
  m_map.insert(make_pair(52,"dummy10_1"));
  m_map.insert(make_pair(53,"dummy01_1"));
  m_map.insert(make_pair(54,"dummy11_1"));
  m_map.insert(make_pair(-51,"anti-dummy00_1"));
  m_map.insert(make_pair(-52,"anti-dummy10_1"));
  m_map.insert(make_pair(-53,"anti-dummy01_1"));
  m_map.insert(make_pair(-54,"anti-dummy11_1"));
  m_map.insert(make_pair(55,"dummy00_2"));
  m_map.insert(make_pair(56,"dummy10_2"));
  m_map.insert(make_pair(57,"dummy01_2"));
  m_map.insert(make_pair(58,"dummy11_2"));
  m_map.insert(make_pair(-55,"anti-dummy00_2"));
  m_map.insert(make_pair(-56,"anti-dummy10_2"));
  m_map.insert(make_pair(-57,"anti-dummy01_2"));
  m_map.insert(make_pair(-58,"anti-dummy11_2"));


  //
  //--------end of book--------
  //

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DumpDecayTreeAlg::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;


  /// Get EventHeader
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");

  if( !eventHeader ) {
    log << MSG::FATAL << "Could not find EventHeader" << endreq;
    return StatusCode::FAILURE;
  }


  //// "Event/MC/McParticleCol" does NOT work
  SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), EventModel::MC::McParticleCol);

  if( !mcParticleCol ) {
    log << MSG::FATAL << "Could not find McParticleCol" << endreq;
    return StatusCode::FAILURE;
  }


  /// Begin counting events for frequency property
  ++m_eventCounter;

  if ( ((m_eventCounter % m_freq) == 0) || m_FindRunEvent ) {
  
    /// Extract run and event number
    int run = eventHeader->runNumber();
    int event = eventHeader->eventNumber();  

    /// if m_FindRunEvent = true, determine if this is correct run and event
    bool validEvent = false;
    if(m_FindRunEvent && (run == m_FindRun) && (event == m_FindEvent)) validEvent = true;
    if(!m_FindRunEvent) validEvent = true;

    /// Only print out information when validEvent = true
    if(validEvent) {

    cout << endl << endl << "-------------------------" << endl;
    cout << "Run: " << run << ", Event: " << event << endl;


    //////////////////////////////
    /// Dump vertices
    cout << "Vertices: " << endl;

    // "Cluster" is first particle before BesEvtGen particle
    bool foundClusterAsMother = false;
    if(!m_BesEvtGenOnly) foundClusterAsMother = true;

    Event::McParticleCol::iterator iter_mc = mcParticleCol->begin();
    for ( ; iter_mc != mcParticleCol->end(); iter_mc++) {
 
      if(!(*iter_mc)->primaryParticle()) {
	if((*iter_mc)->mother().particleProperty() == 91) foundClusterAsMother = true;
      }
      if(!foundClusterAsMother) continue;

      const SmartRefVector<Event::McParticle>& gc = (*iter_mc)->daughterList();

      if( gc.size() > 0) {
	cout << " " << (m_map.find((*iter_mc)->particleProperty()))->second << " [" 
	     << (*iter_mc)->trackIndex() << "] -> ";
	 
	for(unsigned int ii = 0; ii < gc.size(); ii++) {
	  if(ii != (gc.size()-1)) 
	    cout << (m_map.find(gc[ii]->particleProperty()))->second << " [" 
		 << gc[ii]->trackIndex() << "], ";	 
	  else 
	    cout << (m_map.find(gc[ii]->particleProperty()))->second 
		 << " [" << gc[ii]->trackIndex() << "]" << endl;	 
	}

      }  // End of "gc.size() > 0" IF
       
    }  // End of "McParticleCol" FOR LOOP


    //////////////////////////////////////
    /// Dump particles

    if(m_PrintParticles) {

    cout << endl << "Particles:  [#Children, primParticle, leafParticle, decayFromGen, decayInFlight] " << endl;
    
    foundClusterAsMother = false;
    if(!m_BesEvtGenOnly) foundClusterAsMother = true;

    bool firstDecayInFlight = true;
    iter_mc = mcParticleCol->begin();
    for ( ; iter_mc != mcParticleCol->end(); iter_mc++) {
 
      if(!(*iter_mc)->primaryParticle()) {
	if((*iter_mc)->mother().particleProperty() == 91) foundClusterAsMother = true;
      }
      if(!foundClusterAsMother) continue;

      const SmartRefVector<Event::McParticle>& gc = (*iter_mc)->daughterList();
      int numChildren = gc.size(); 

      string primaryParticle = "F";
      if((*iter_mc)->primaryParticle()) primaryParticle = "T";

      string leafParticle = "F";
      if((*iter_mc)->leafParticle()) leafParticle = "T";

      string decayFromGen = "F";
      if((*iter_mc)->decayFromGenerator()) decayFromGen = "T";

      string decayInFlight = "F";
      if((*iter_mc)->decayInFlight()) {
	decayInFlight = "T";

	if(firstDecayInFlight) {
	  cout << endl;
	  firstDecayInFlight = false;
	}
      }

      cout << " " << (*iter_mc)->trackIndex() << ": " 
	   << (m_map.find((*iter_mc)->particleProperty()))->second 
	   << " p4 = " << (*iter_mc)->initialFourMomentum() << " [" 
	   << numChildren << ", " 
	   << primaryParticle << ", " << leafParticle << ", " 
	   << decayFromGen << ", " << decayInFlight << "]"
	   << endl; 
       
    }  // End of "McParticleCol" FOR LOOP
    }  // End of "m_PrintParticle" IF

    } // End of "validEvent" IF
  }  // End of "m_freq" IF


  return StatusCode::SUCCESS;
} 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DumpDecayTreeAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;

  return StatusCode::SUCCESS;
}
