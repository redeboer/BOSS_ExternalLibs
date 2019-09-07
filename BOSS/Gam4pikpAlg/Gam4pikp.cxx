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
#include "McTruth/McParticle.h"
#include "HltEvent/HltInf.h"
#include "HltEvent/DstHltInf.h"

#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "VertexFit/IVertexDbSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"


using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "Gam4pikpAlg/Gam4pikp.h"

#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/SecondVertexFit.h"
#include "VertexFit/Helix.h"   
#include "ParticleID/ParticleID.h"
#include <vector>
const double mpi = 0.13957;
const double mk = 0.493677;
const double mpro = 0.938272;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;
typedef std::vector<double> Vdouble;
typedef std::vector<WTrackParameter> Vw;
typedef std::vector<VertexParameter> Vv;
static int Ncut[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int Mcut[10]={0,0,0,0,0,0,0,0,0,0};
/////////////////////////////////////////////////////////////////////////////

Gam4pikp::Gam4pikp(const std::string& name, ISvcLocator* pSvcLocator) :
   Algorithm(name, pSvcLocator) {

      //Declare the properties
      declareProperty("skim4pi", m_skim4pi=false);
      declareProperty("skim4k", m_skim4k=false);
      declareProperty("skim2pi2pr", m_skim2pi2pr=false);
      declareProperty("rootput", m_rootput=false);
      declareProperty("Ecms", m_ecms=3.6862); 
      declareProperty("Vr0cut", m_vr0cut=1.0);
      declareProperty("Vz0cut", m_vz0cut=5.0);
      declareProperty("EnergyThreshold", m_energyThreshold=0.05);
      declareProperty("GammaPhiCut", m_gammaPhiCut=20.0);
      declareProperty("GammaThetaCut", m_gammaThetaCut=20.0);
      declareProperty("GammaDangCut", m_gammadangcut=20.0);
//      declareProperty("Test4C", m_test4C = 1);
//      declareProperty("Test5C", m_test5C = 1);
//      declareProperty("CheckDedx", m_checkDedx = 1);
//      declareProperty("CheckTof",  m_checkTof = 1);
   }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode Gam4pikp::initialize(){
   MsgStream log(msgSvc(), name());

   log << MSG::INFO << "in initialize()" << endmsg;
//   setFilterPassed(false);  
   StatusCode status;

if(m_rootput)
{
   NTuplePtr nt1(ntupleSvc(), "FILE1/total4c");
   if ( nt1 ) m_tuple1 = nt1;
   else {
 m_tuple1 = ntupleSvc()->book ("FILE1/total4c", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if ( m_tuple1 )    {

	 status = m_tuple1->addItem ("run",  m_run );
	 status = m_tuple1->addItem ("rec",  m_rec );
	 status = m_tuple1->addItem ("mpprecall",  m_mpprecall );
	 status = m_tuple1->addItem ("meeall",  m_meeall );
	 status = m_tuple1->addItem ("ncgjs",  m_ncgjs );  
	 status = m_tuple1->addItem ("cla2kpi",  m_cla2kpi );
	 status = m_tuple1->addItem("indexmc",          m_idxmc, 0, 100);
	 status = m_tuple1->addIndexedItem("pdgid",     m_idxmc, m_pdgid);

	 status = m_tuple1->addIndexedItem("motheridx", m_idxmc, m_motheridx);
	 status = m_tuple1->addItem("indexmdc",       m_idxmdc, 0, 5000);
	 status = m_tuple1->addIndexedItem ("x0js", m_idxmdc,  m_x0js);
	 status = m_tuple1->addIndexedItem ("y0js", m_idxmdc, m_y0js);
	 status = m_tuple1->addIndexedItem ("z0js",m_idxmdc, m_z0js);
	 status = m_tuple1->addIndexedItem ("r0js",m_idxmdc, m_r0js);
	 status = m_tuple1->addIndexedItem ("Rxyjs",m_idxmdc, m_Rxyjs);
	 status = m_tuple1->addIndexedItem ("Rzjs",m_idxmdc, m_Rzjs);
	 status = m_tuple1->addIndexedItem ("Rnxyjs",m_idxmdc, m_Rnxyjs);
	 status = m_tuple1->addIndexedItem ("phinjs",m_idxmdc, m_phinjs);
	 status = m_tuple1->addIndexedItem ("Rnzjs",m_idxmdc, m_Rnzjs);
	 status = m_tuple1->addItem ("ncy20",  m_ncy20);
	 status = m_tuple1->addItem ("ncy30",  m_ncy30);
	 status = m_tuple1->addIndexedItem("angjs5",      m_idxmdc, m_angjs5);
	 status = m_tuple1->addIndexedItem("nearjs5",      m_idxmdc, m_nearjs5);
	 status = m_tuple1->addIndexedItem("angjs6",      m_idxmdc, m_angjs6);
	 status = m_tuple1->addIndexedItem("nearjs6",      m_idxmdc, m_nearjs6);
	 status = m_tuple1->addIndexedItem("ang4pi5",      m_idxmdc, m_ang4pi5);
	 status = m_tuple1->addIndexedItem("near4pi5",      m_idxmdc, m_near4pi5);
	 status = m_tuple1->addIndexedItem("ang4pi6",      m_idxmdc, m_ang4pi6);
	 status = m_tuple1->addIndexedItem("near4pi6",      m_idxmdc, m_near4pi6);
	 status = m_tuple1->addIndexedItem("ppmdcjs",      m_idxmdc, m_ppmdcjs);
	 status = m_tuple1->addIndexedItem("pxmdcjs",      m_idxmdc, m_pxmdcjs);
	 status = m_tuple1->addIndexedItem("pymdcjs",      m_idxmdc, m_pymdcjs);
	 status = m_tuple1->addIndexedItem("pzmdcjs",      m_idxmdc, m_pzmdcjs);
	 status = m_tuple1->addIndexedItem("ppkaljs",      m_idxmdc, m_ppkaljs);
	 status = m_tuple1->addIndexedItem("ptmdcjs",      m_idxmdc, m_ptmdcjs);
	 status = m_tuple1->addIndexedItem("ptkaljs",      m_idxmdc, m_ptkaljs);
	 status = m_tuple1->addIndexedItem("ppmdc2kpi",      m_idxmdc, m_ppmdc2kpi);
	 status = m_tuple1->addIndexedItem("pxmdc2kpi",      m_idxmdc, m_pxmdc2kpi);
	 status = m_tuple1->addIndexedItem("pymdc2kpi",      m_idxmdc, m_pymdc2kpi);
	 status = m_tuple1->addIndexedItem("pzmdc2kpi",      m_idxmdc, m_pzmdc2kpi);
	 status = m_tuple1->addIndexedItem("ppkal2kpi",      m_idxmdc, m_ppkal2kpi);
	 status = m_tuple1->addIndexedItem("ptmdc2kpi",      m_idxmdc, m_ptmdc2kpi);
	 status = m_tuple1->addIndexedItem("charge2kpi",      m_idxmdc, m_charge2kpi);
	 status = m_tuple1->addIndexedItem("ptkal2kpi",      m_idxmdc, m_ptkal2kpi);
	 status = m_tuple1->addItem ("cy2pi", m_cy2kpi, 0, 100 );
	 status = m_tuple1->addIndexedItem("comcs2kpi",      m_cy2kpi, m_comcs2kpi);
	 status = m_tuple1->addItem ("chiejs", m_idxmdc,  m_chiejs);
	 status = m_tuple1->addItem ("chimujs", m_idxmdc,  m_chimujs);
	 status = m_tuple1->addItem ("chipijs", m_idxmdc,  m_chipijs);
	 status = m_tuple1->addItem ("chikjs", m_idxmdc,  m_chikjs);
	 status = m_tuple1->addItem ("chipjs", m_idxmdc,  m_chipjs);
	 status = m_tuple1->addItem ("ghitjs", m_idxmdc,  m_ghitjs);
	 status = m_tuple1->addItem ("thitjs", m_idxmdc,  m_thitjs);
	 status = m_tuple1->addIndexedItem("probphjs",  m_idxmdc, m_probphjs);
	 status = m_tuple1->addIndexedItem("normphjs",  m_idxmdc, m_normphjs);
	 status = m_tuple1->addItem ("pdg", m_idxmdc,  m_pdg);
	 status = m_tuple1->addItem ("cbmc", m_idxmdc,  m_cbmc);
	 status = m_tuple1->addIndexedItem("sigmaetof2kpi",   m_idxmdc, m_sigmaetof2kpi);
 status = m_tuple1->addIndexedItem("sigmamutof2kpi",   m_idxmdc, m_sigmamutof2kpi);
 status = m_tuple1->addIndexedItem("sigmapitof2kpi",   m_idxmdc, m_sigmapitof2kpi);
 status = m_tuple1->addIndexedItem("sigmaktof2kpi",   m_idxmdc, m_sigmaktof2kpi);
 status = m_tuple1->addIndexedItem("sigmaprtof2kpi",   m_idxmdc, m_sigmaprtof2kpi);
	 status = m_tuple1->addIndexedItem("t0tof2kpi",   m_idxmdc, m_t0tof2kpi);
	 status = m_tuple1->addIndexedItem("errt0tof2kpi",   m_idxmdc, m_errt0tof2kpi);

	 status = m_tuple1->addItem ("chie2kpi", m_idxmdc,  m_chie2kpi);
	 status = m_tuple1->addItem ("chimu2kpi", m_idxmdc,  m_chimu2kpi);
	 status = m_tuple1->addItem ("chipi2kpi", m_idxmdc,  m_chipi2kpi);
	 status = m_tuple1->addItem ("chik2kpi", m_idxmdc,  m_chik2kpi);
	 status = m_tuple1->addItem ("chip2kpi", m_idxmdc,  m_chip2kpi);
	 status = m_tuple1->addItem ("ghit2kpi", m_idxmdc,  m_ghit2kpi);
	 status = m_tuple1->addItem ("thit2kpi", m_idxmdc,  m_thit2kpi);
	 status = m_tuple1->addIndexedItem("probph2kpi",  m_idxmdc, m_probph2kpi);
	 status = m_tuple1->addIndexedItem("normph2kpi",  m_idxmdc, m_normph2kpi);
	 status = m_tuple1->addIndexedItem("pidnum2kpi",  m_idxmdc, m_pidnum2kpi);
	 status = m_tuple1->addIndexedItem("bjmucjs",   m_idxmdc, m_bjmucjs);
	 status = m_tuple1->addIndexedItem("bjmuc2kpi",   m_idxmdc, m_bjmuc2kpi);
	 status = m_tuple1->addIndexedItem("bjemcjs",     m_idxmdc, m_bjemcjs);
	 status = m_tuple1->addIndexedItem("bjemc2kpi",     m_idxmdc, m_bjemc2kpi);
	 status = m_tuple1->addIndexedItem("bjtofjs",     m_idxmdc, m_bjtofjs);
	 status = m_tuple1->addIndexedItem("bjtof2kpi",     m_idxmdc, m_bjtof2kpi);
	 status = m_tuple1->addIndexedItem("bjtofvaljs",     m_idxmdc, m_bjtofvaljs);
	 status = m_tuple1->addIndexedItem("bjtofval2kpi",     m_idxmdc, m_bjtofval2kpi);

	 status = m_tuple1->addIndexedItem("emcjs",     m_idxmdc, m_emcjs);
	 status = m_tuple1->addIndexedItem("evpjs",     m_idxmdc, m_evpjs);
	 status = m_tuple1->addIndexedItem("timecgjs",     m_idxmdc, m_timecgjs);
	 status = m_tuple1->addIndexedItem("depthjs",   m_idxmdc, m_depthmucjs);
	 status = m_tuple1->addIndexedItem("layermucjs",   m_idxmdc, m_layermucjs);

	 status = m_tuple1->addIndexedItem("emc2kpi",     m_idxmdc, m_emc2kpi);
	 status = m_tuple1->addIndexedItem("evp2kpi",     m_idxmdc, m_evp2kpi);
	 status = m_tuple1->addIndexedItem("timecg2kpi",     m_idxmdc, m_timecg2kpi);
	 status = m_tuple1->addIndexedItem("depth2kpi",   m_idxmdc, m_depthmuc2kpi);
	 status = m_tuple1->addIndexedItem("layermuc2kpi",   m_idxmdc, m_layermuc2kpi);

	 status = m_tuple1->addIndexedItem("cotof1js",    m_idxmdc, m_cotof1js);
	 status = m_tuple1->addIndexedItem("cotof2js",    m_idxmdc, m_cotof2js);
	 status = m_tuple1->addIndexedItem("counterjs",   m_idxmdc, m_counterjs);
	 status = m_tuple1->addIndexedItem("barreljs",   m_idxmdc, m_barreljs);
	 status = m_tuple1->addIndexedItem("layertofjs",   m_idxmdc, m_layertofjs);
	 status = m_tuple1->addIndexedItem("readoutjs",   m_idxmdc, m_readoutjs);
	 status = m_tuple1->addIndexedItem("clusterjs",    m_idxmdc, m_clusterjs);
	 status = m_tuple1->addIndexedItem("betajs",   m_idxmdc, m_betajs);
	 status = m_tuple1->addIndexedItem("tofjs",   m_idxmdc, m_tofjs);
	 status = m_tuple1->addIndexedItem("tofpathjs",   m_idxmdc, m_tofpathjs);
	 status = m_tuple1->addIndexedItem("zhitjs",   m_idxmdc, m_zhitjs);
	 status = m_tuple1->addIndexedItem("tofIDjs",   m_idxmdc, m_tofIDjs);
	 status = m_tuple1->addIndexedItem("clusterIDjs",   m_idxmdc, m_clusterIDjs);
	 status = m_tuple1->addIndexedItem("texejs",   m_idxmdc, m_texejs);
	 status = m_tuple1->addIndexedItem("texmujs",   m_idxmdc, m_texmujs);
	 status = m_tuple1->addIndexedItem("texpijs",   m_idxmdc, m_texpijs);
	 status = m_tuple1->addIndexedItem("texkjs",   m_idxmdc, m_texkjs);
	 status = m_tuple1->addIndexedItem("texprjs",   m_idxmdc, m_texprjs);
	 status = m_tuple1->addIndexedItem("dtejs",   m_idxmdc, m_dtejs);
	 status = m_tuple1->addIndexedItem("dtmujs",   m_idxmdc, m_dtmujs);
	 status = m_tuple1->addIndexedItem("dtpijs",   m_idxmdc, m_dtpijs);
	 status = m_tuple1->addIndexedItem("dtkjs",   m_idxmdc, m_dtkjs);
	 status = m_tuple1->addIndexedItem("dtprjs",   m_idxmdc, m_dtprjs);
	 status = m_tuple1->addIndexedItem("sigmaetofjs",   m_idxmdc, m_sigmaetofjs);
	 status = m_tuple1->addIndexedItem("sigmamutofjs",   m_idxmdc, m_sigmamutofjs);
	 status = m_tuple1->addIndexedItem("sigmapitofjs",   m_idxmdc, m_sigmapitofjs);
	 status = m_tuple1->addIndexedItem("sigmaktofjs",   m_idxmdc, m_sigmaktofjs);
	 status = m_tuple1->addIndexedItem("sigmaprtofjs",   m_idxmdc, m_sigmaprtofjs);
	 status = m_tuple1->addIndexedItem("t0tofjs",   m_idxmdc,m_t0tofjs);
	 status = m_tuple1->addIndexedItem("errt0tofjs",   m_idxmdc,m_errt0tofjs);
	 status = m_tuple1->addIndexedItem("cotof12kpi",    m_idxmdc, m_cotof12kpi);
	 status = m_tuple1->addIndexedItem("cotof22kpi",    m_idxmdc, m_cotof22kpi);
	 status = m_tuple1->addIndexedItem("counter2kpi",   m_idxmdc, m_counter2kpi);
	 status = m_tuple1->addIndexedItem("barrel2kpi",   m_idxmdc, m_barrel2kpi);
	 status = m_tuple1->addIndexedItem("layertof2kpi",   m_idxmdc, m_layertof2kpi);
	 status = m_tuple1->addIndexedItem("readout2kpi",   m_idxmdc, m_readout2kpi);
	 status = m_tuple1->addIndexedItem("cluster2kpi",    m_idxmdc, m_cluster2kpi);
	 status = m_tuple1->addIndexedItem("beta2kpi",   m_idxmdc, m_beta2kpi);
	 status = m_tuple1->addIndexedItem("tof2kpi",   m_idxmdc, m_tof2kpi);
	 status = m_tuple1->addIndexedItem("tofpath2kpi",   m_idxmdc, m_tofpath2kpi);
	 status = m_tuple1->addIndexedItem("zhit2kpi",   m_idxmdc, m_zhit2kpi);
	 status = m_tuple1->addIndexedItem("tofID2kpi",   m_idxmdc, m_tofID2kpi);
	 status = m_tuple1->addIndexedItem("clusterID2kpi",   m_idxmdc, m_clusterID2kpi);
	 status = m_tuple1->addIndexedItem("texe2kpi",   m_idxmdc, m_texe2kpi);
	 status = m_tuple1->addIndexedItem("texmu2kpi",   m_idxmdc, m_texmu2kpi);
	 status = m_tuple1->addIndexedItem("texpi2kpi",   m_idxmdc, m_texpi2kpi);
	 status = m_tuple1->addIndexedItem("texk2kpi",   m_idxmdc, m_texk2kpi);
	 status = m_tuple1->addIndexedItem("texpr2kpi",   m_idxmdc, m_texpr2kpi);
	 status = m_tuple1->addIndexedItem("dte2kpi",   m_idxmdc, m_dte2kpi);
	 status = m_tuple1->addIndexedItem("dtmu2kpi",   m_idxmdc, m_dtmu2kpi);
	 status = m_tuple1->addIndexedItem("dtpi2kpi",   m_idxmdc, m_dtpi2kpi);
	 status = m_tuple1->addIndexedItem("dtk2kpi",   m_idxmdc, m_dtk2kpi);
	 status = m_tuple1->addIndexedItem("dtpr2kpi",   m_idxmdc, m_dtpr2kpi);
	 status = m_tuple1->addIndexedItem("costpid2kpi",   m_idxmdc, m_costpid2kpi);
	 status = m_tuple1->addIndexedItem("dedxpid2kpi",   m_idxmdc, m_dedxpid2kpi);
	 status = m_tuple1->addIndexedItem("tof1pid2kpi",   m_idxmdc, m_tof1pid2kpi);
	 status = m_tuple1->addIndexedItem("tof2pid2kpi",   m_idxmdc, m_tof2pid2kpi);
	 status = m_tuple1->addIndexedItem("probe2kpi",   m_idxmdc, m_probe2kpi);
	 status = m_tuple1->addIndexedItem("probmu2kpi",   m_idxmdc, m_probmu2kpi);
	 status = m_tuple1->addIndexedItem("probpi2kpi",   m_idxmdc, m_probpi2kpi);
	 status = m_tuple1->addIndexedItem("probk2kpi",   m_idxmdc, m_probk2kpi);
	 status = m_tuple1->addIndexedItem("probpr2kpi",   m_idxmdc, m_probpr2kpi);

 status = m_tuple1->addIndexedItem("chipidxpid2kpi",   m_idxmdc, m_chipidxpid2kpi);
 status = m_tuple1->addIndexedItem("chipitof1pid2kpi",   m_idxmdc, m_chipitof1pid2kpi);
 status = m_tuple1->addIndexedItem("chipitof2pid2kpi",   m_idxmdc, m_chipitof2pid2kpi);
 status = m_tuple1->addIndexedItem("chipitofpid2kpi",   m_idxmdc, m_chipitofpid2kpi);
 status = m_tuple1->addIndexedItem("chipitofepid2kpi",   m_idxmdc, m_chipitofepid2kpi);
 status = m_tuple1->addIndexedItem("chipitofqpid2kpi",   m_idxmdc, m_chipitofqpid2kpi);
 status = m_tuple1->addIndexedItem("probpidxpid2kpi",   m_idxmdc, m_probpidxpid2kpi);
 status = m_tuple1->addIndexedItem("probpitofpid2kpi",   m_idxmdc, m_probpitofpid2kpi);
	 status = m_tuple1->addIndexedItem("chikdxpid2kpi",   m_idxmdc, m_chikdxpid2kpi);
 status = m_tuple1->addIndexedItem("chiktof1pid2kpi",   m_idxmdc, m_chiktof1pid2kpi);
 status = m_tuple1->addIndexedItem("chiktof2pid2kpi",   m_idxmdc, m_chiktof2pid2kpi);
 status = m_tuple1->addIndexedItem("chiktofpid2kpi",   m_idxmdc, m_chiktofpid2kpi);
 status = m_tuple1->addIndexedItem("chiktofepid2kpi",   m_idxmdc, m_chiktofepid2kpi);
 status = m_tuple1->addIndexedItem("chiktofqpid2kpi",   m_idxmdc, m_chiktofqpid2kpi);
 status = m_tuple1->addIndexedItem("probkdxpid2kpi",   m_idxmdc, m_probkdxpid2kpi);
 status = m_tuple1->addIndexedItem("probktofpid2kpi",   m_idxmdc, m_probktofpid2kpi);

 status = m_tuple1->addIndexedItem("chiprdxpid2kpi",   m_idxmdc, m_chiprdxpid2kpi);
 status = m_tuple1->addIndexedItem("chiprtof1pid2kpi",   m_idxmdc, m_chiprtof1pid2kpi);
 status = m_tuple1->addIndexedItem("chiprtof2pid2kpi",   m_idxmdc, m_chiprtof2pid2kpi);
 status = m_tuple1->addIndexedItem("chiprtofpid2kpi",   m_idxmdc, m_chiprtofpid2kpi);
 status = m_tuple1->addIndexedItem("chiprtofepid2kpi",   m_idxmdc, m_chiprtofepid2kpi);
 status = m_tuple1->addIndexedItem("chiprtofqpid2kpi",   m_idxmdc, m_chiprtofqpid2kpi);
 status = m_tuple1->addIndexedItem("probprdxpid2kpi",   m_idxmdc, m_probprdxpid2kpi);
 status = m_tuple1->addIndexedItem("probprtofpid2kpi",   m_idxmdc, m_probprtofpid2kpi);

	 status = m_tuple1->addIndexedItem("cosmdcjs",   m_idxmdc, m_cosmdcjs);
	 status = m_tuple1->addIndexedItem("phimdcjs",   m_idxmdc, m_phimdcjs);
	 status = m_tuple1->addIndexedItem("cosmdc2kpi",   m_idxmdc, m_cosmdc2kpi);
	 status = m_tuple1->addIndexedItem("phimdc2kpi",   m_idxmdc, m_phimdc2kpi);

	 status = m_tuple1->addIndexedItem("dedxpidjs",   m_idxmdc, m_dedxpidjs);
	 status = m_tuple1->addIndexedItem("tof1pidjs",   m_idxmdc, m_tof1pidjs);
	 status = m_tuple1->addIndexedItem("tof2pidjs",   m_idxmdc, m_tof2pidjs);
	 status = m_tuple1->addIndexedItem("probejs",   m_idxmdc, m_probejs);
	 status = m_tuple1->addIndexedItem("probmujs",   m_idxmdc, m_probmujs);
	 status = m_tuple1->addIndexedItem("probpijs",   m_idxmdc, m_probpijs);
	 status = m_tuple1->addIndexedItem("probkjs",   m_idxmdc, m_probkjs);
	 status = m_tuple1->addIndexedItem("probprjs",   m_idxmdc, m_probprjs);
	 status = m_tuple1->addItem ("mchic2kpi",   m_mchic2kpi);
	 status = m_tuple1->addItem ("mpsip2kpi",   m_mpsip2kpi);
	 status = m_tuple1->addItem ("chis2kpi",   m_chis2kpi);
	 status = m_tuple1->addItem ("mchic4c2kpi",   m_mchic4c2kpi);
	 status = m_tuple1->addItem ("mpsip4c2kpi",   m_mpsip4c2kpi);
	 status = m_tuple1->addItem ("chis4c2kpi",   m_chis4c2kpi);
	 
	 status = m_tuple1->addItem("indexemc", m_idxemc, 0, 5000);
	 status = m_tuple1->addIndexedItem("numHits", m_idxemc, m_numHits);
	 status = m_tuple1->addIndexedItem("secmom",  m_idxemc, m_secondmoment);
	 status = m_tuple1->addIndexedItem("latmom",  m_idxemc, m_latmoment);
	 status = m_tuple1->addIndexedItem("timegm",  m_idxemc, m_timegm);
	 status = m_tuple1->addIndexedItem("cellId",  m_idxemc, m_cellId);
	 status = m_tuple1->addIndexedItem("module",  m_idxemc, m_module);
	 status = m_tuple1->addIndexedItem("a20Moment",  m_idxemc, m_a20Moment);
	 status = m_tuple1->addIndexedItem("a42Moment",  m_idxemc, m_a42Moment);
	 status = m_tuple1->addIndexedItem("getEAll",  m_idxemc, m_getEAll);
	 status = m_tuple1->addIndexedItem("getShowerId",  m_idxemc, m_getShowerId);
	 status = m_tuple1->addIndexedItem("getClusterId",  m_idxemc, m_getClusterId);
	 status = m_tuple1->addIndexedItem("x", m_idxemc, m_x);
	 status = m_tuple1->addIndexedItem("y", m_idxemc, m_y);
	 status = m_tuple1->addIndexedItem("z", m_idxemc, m_z);
	 status = m_tuple1->addIndexedItem("cosemc", m_idxemc, m_cosemc);
	 status = m_tuple1->addIndexedItem("phiemc", m_idxemc, m_phiemc);
	 status = m_tuple1->addIndexedItem("energy", m_idxemc, m_energy);
	 status = m_tuple1->addIndexedItem("e1", m_idxemc, m_eSeed);
	 status = m_tuple1->addIndexedItem("e9", m_idxemc, m_e3x3);
	 status = m_tuple1->addIndexedItem("e25", m_idxemc, m_e5x5);
	 status = m_tuple1->addIndexedItem("dang4c", m_idxemc, m_dang4c);
	 status = m_tuple1->addIndexedItem("dthe4c", m_idxemc, m_dthe4c);
	 status = m_tuple1->addIndexedItem("dphi4c", m_idxemc, m_dphi4c);
	 status = m_tuple1->addIndexedItem("dang4crt", m_idxemc, m_dang4crt);
	 status = m_tuple1->addIndexedItem("dthe4crt", m_idxemc, m_dthe4crt);
	 status = m_tuple1->addIndexedItem("dphi4crt", m_idxemc, m_dphi4crt);
	 status = m_tuple1->addIndexedItem("phtof", m_idxemc, 3, m_phgmtof,0.0,10000.0);
	 status = m_tuple1->addIndexedItem("phgmtof0", m_idxemc, m_phgmtof0);
	 status = m_tuple1->addIndexedItem("phgmtof1", m_idxemc, m_phgmtof1);
	 status = m_tuple1->addIndexedItem("phgmtof2", m_idxemc, m_phgmtof2);

      }
      else    {
	 log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
	 return StatusCode::FAILURE;
      }
   }   
}
   
StatusCode sc;
   
   if(m_skim4pi) 
   {       	
       sc =  createSubAlgorithm( "EventWriter", "Selectgam4pi", m_subalg1);
      if( sc.isFailure() ) {
	 log << MSG::ERROR << "Error creating Sub-Algorithm Selectgam4pi" <<endreq;
	 return sc;
      } else {
	 log << MSG::INFO << "Success creating Sub-Algorithm Selectgam4pi" <<endreq;
      }
   }


  if(m_skim4k)
      {
	 sc =  createSubAlgorithm( "EventWriter", "Selectgam4k", m_subalg2);
      if( sc.isFailure() ) {
	 log << MSG::ERROR << "Error creating Sub-Algorithm Selectgam4k" <<endreq;
	 return sc;
      } else {
	 log << MSG::INFO << "Success creating Sub-Algorithm Selectgam4k" <<endreq;
      }
      }

   if(m_skim2pi2pr)
      {
      sc =  createSubAlgorithm( "EventWriter", "Selectgam2pi2pr", m_subalg3);
      if( sc.isFailure() ) {
	 log << MSG::ERROR << "Error creating Sub-Algorithm Selectgam2pi2pr" <<endreq;
	 return sc;
      } else {
	 log << MSG::INFO << "Success creating Sub-Algorithm Selectgam2pi2pr" <<endreq;
      }
      }


    

   //
   //--------end of book--------
   //

   log << MSG::INFO << "successfully return from initialize()" <<endmsg;
   return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode Gam4pikp::execute() {

//   std::cout << "execute()" << std::endl;
 setFilterPassed(false);

   
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in execute()" << endreq;

   SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
   int run=eventHeader->runNumber();
   int event=eventHeader->eventNumber();
   log << MSG::DEBUG <<"run, evtnum = "
      << run << " , "
      << event <<endreq;
//   cout<<"run "<<run<<endl;
//   cout<<"event "<<event<<endl;
if(m_rootput)   
{
   m_run=run;
   m_rec=event;
}   
   Ncut[0]++;
   SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
   log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
      << evtRecEvent->totalCharged() << " , "
      << evtRecEvent->totalNeutral() << " , "
      << evtRecEvent->totalTracks() <<endreq;

   SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);

   if(m_rootput)
{
   Gam4pikp::InitVar();
}

   // for MC topo analysis 
if(m_rootput)
{   
   if (eventHeader->runNumber()<0)
   {
      SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), "/Event/MC/McParticleCol");
      int m_numParticle = 0;
      if (!mcParticleCol)
      {
//	 std::cout << "Could not retrieve McParticelCol" << std::endl;
	 return StatusCode::FAILURE;
      }
      else
      {
	 bool psipDecay = false;
	 int rootIndex = -1;
	 Event::McParticleCol::iterator iter_mc = mcParticleCol->begin();
	 for (; iter_mc != mcParticleCol->end(); iter_mc++)
	 {
	    if ((*iter_mc)->primaryParticle()) continue;
	    if (!(*iter_mc)->decayFromGenerator()) continue;
	    if ((*iter_mc)->particleProperty()==100443)
	    {
	       psipDecay = true;
	       rootIndex = (*iter_mc)->trackIndex();
	    }
	    if (!psipDecay) continue;
	    int mcidx = ((*iter_mc)->mother()).trackIndex() - rootIndex;
	    int pdgid = (*iter_mc)->particleProperty();
	    m_pdgid[m_numParticle] = pdgid;
	    m_motheridx[m_numParticle] = mcidx;
	    m_numParticle += 1;
	 }
      }
      m_idxmc = m_numParticle;
   } 
}

   Vint iGood, ipip, ipim;
   iGood.clear();
   ipip.clear();
   ipim.clear();
   Vp4 ppip, ppim;
   ppip.clear();
   ppim.clear();
   int nCharge = 0;
   int ngdcgx=0;
   int ncgx=0;
   Vdouble pTrkCh;
   pTrkCh.clear();
   Hep3Vector v(0,0,0);
   Hep3Vector vv(0,0,0);

   IVertexDbSvc* vtxsvc;
   Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
   if(vtxsvc->isVertexValid())   
   {  
      double* vertex = vtxsvc->PrimaryVertex(); 
      double* vertexsigma = vtxsvc->SigmaPrimaryVertex();
      v.setX(vertex[0]);
      v.setY(vertex[1]);
      v.setZ(vertex[2]);
      vv.setX(vertexsigma[0]);
      vv.setY(vertexsigma[1]);
      vv.setZ(vertexsigma[2]);
   }


   if(run<0)
   {
      v[0]=0.;
      v[1]=0.;
      v[2]=0.;
      vv[0]=0.;
      vv[1]=0.;
      vv[2]=0.;

   }	 


   for(int i = 0; i < evtRecEvent->totalCharged(); i++){
      if(i>=evtRecTrkCol->size()) break;
      ncgx++;
      EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
      if(!(*itTrk)->isMdcTrackValid()) continue;
      if (!(*itTrk)->isMdcKalTrackValid()) continue;
      ngdcgx++;
      RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();

      HepVector a = mdcTrk->helix();
      HepSymMatrix Ea = mdcTrk->err();
      HepPoint3D pivot(0.,0.,0.);
      HepPoint3D IP(v[0],v[1],v[2]);
      VFHelix helixp(pivot,a,Ea);  
      helixp.pivot(IP);
      HepVector vec = helixp.a();
      pTrkCh.push_back(mdcTrk->p()*mdcTrk->charge());    
      iGood.push_back((*itTrk)->trackId());
      nCharge += mdcTrk->charge();
   }


   int nGood = iGood.size();
   log << MSG::DEBUG << "ngood, totcharge = " << nGood << " , " << nCharge << endreq;
   if((nGood < 4))  
   {
      return StatusCode::SUCCESS;
   }

   Ncut[1]++;
   Vint iGam;
   int ngamx=0;
   int ngdgamx=0;
   iGam.clear();
   Vint iGamnofit;
   iGamnofit.clear();
   for(int i = evtRecEvent->totalCharged(); i< evtRecEvent->totalTracks(); i++) {
      ngamx++;   
      if(i>=evtRecTrkCol->size()) break; 
      EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
      if(!(*itTrk)->isEmcShowerValid()) continue;
      RecEmcShower *emcTrk = (*itTrk)->emcShower();
      Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());
      // find the nearest charged track
//      double dthe = 200.;
//      double dphi = 200.;
//      double dang = 200.;
      double dthe = 200.;
      double dphi = 200.;
      double dang = 200.; 
      
      ngdgamx++;
      for(int j = 0; j < evtRecEvent->totalCharged(); j++) {
	 if(j>=evtRecTrkCol->size()) break;
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

//	 if(fabs(thed) < fabs(dthe)) dthe = thed;
//	 if(fabs(phid) < fabs(dphi)) dphi = phid;
//	 if(angd < dang) dang = angd;

	 if(angd < dang){ dang=angd;
	    dthe=thed;
	    dphi=phid;	 
	 }

      }
      //    if(dang>=200) continue;
      double eraw = emcTrk->energy();
      dthe = dthe * 180 / (CLHEP::pi);
      dphi = dphi * 180 / (CLHEP::pi);
      dang = dang * 180 / (CLHEP::pi);
//      dthert = dthert * 180 / (CLHEP::pi);
//      dphirt = dphirt * 180 / (CLHEP::pi);
//      dangrt = dangrt * 180 / (CLHEP::pi);    
      //    if(eraw < m_energyThreshold) continue;
      iGam.push_back((*itTrk)->trackId());
      if(eraw < m_energyThreshold) continue;
      if(dang < 20.0) continue;
      iGamnofit.push_back((*itTrk)->trackId());

   }
   // Finish Good Photon Selection
   //
   int nGam = iGam.size();
   int nGamnofit = iGamnofit.size();

   log << MSG::DEBUG << "num Good Photon " << nGam  << " , " <<evtRecEvent->totalNeutral()<<endreq;
   if(nGam<1){
      return StatusCode::SUCCESS;
   }
   Ncut[2]++;

   if(nGood>20||nGam>30)   return StatusCode::SUCCESS;	

if(m_rootput)
{
m_idxmdc = nGood;
m_idxemc=nGam;   
}
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
   // Assign 4-momentum to each charged track
   //
   ParticleID *pid = ParticleID::instance();


   for(int i = 0; i < nGood; i++) {
      EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 
      RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();

      RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();//After ParticleID, use RecMdcKalTrack substitute RecMdcTrack
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);//PID can set to electron, muon, pion, kaon and proton;The default setting is pion

      if(mdcKalTrk->charge() >0) {
	 ipip.push_back(iGood[i]);
	 HepLorentzVector ptrk;
	 ptrk.setPx(mdcKalTrk->px());
	 ptrk.setPy(mdcKalTrk->py());
	 ptrk.setPz(mdcKalTrk->pz());
	 double p3 = ptrk.mag();
	 ptrk.setE(sqrt(p3*p3+mpi*mpi));


	 ppip.push_back(ptrk);
      } else {
	 ipim.push_back(iGood[i]);
	 HepLorentzVector ptrk;
	 ptrk.setPx(mdcKalTrk->px());
	 ptrk.setPy(mdcKalTrk->py());
	 ptrk.setPz(mdcKalTrk->pz());
	 double p3 = ptrk.mag();
	 ptrk.setE(sqrt(p3*p3+mpi*mpi));

	 //      ptrk = ptrk.boost(-0.011,0,0);//boost to cms

	 ppim.push_back(ptrk);
      }
   }
   int npip = ipip.size();
   int npim = ipim.size();
   //  if(npip*npim != 4) return SUCCESS;
   if((npip < 2)||(npim < 2)) return SUCCESS;

   Ncut[3]++;

   //
   // Loop each gamma pair, check ppi0 and pTot 
   //

   int icgp1=-1;
   int icgp2=-1;
   int icgm1=-1;
   int icgm2=-1;
   int igam=-1;
   double chisqtrk = 9999.;
   WTrackParameter wcgp1;
   WTrackParameter wcgp2;
   WTrackParameter wcgm1;
   WTrackParameter wcgm2;
   int ipip1js=-1;
   int ipip2js=-1;
   int ipim1js=-1;
   int ipim2js=-1;
   double mcompall=9999; 
   double mppreclst=9999;
   double meelst=9999;;
   double mchic2kpilst=9999;
   double chis4c2kpilst=9999;
   int type2kpilst=0;
   double dtpr2kpilst[4]={9999,9999,9999,9999};

   double mc1=mpi,mc2=mpi,mc3=mpi,mc4=mpi;
   double mchic01=0.0;
   HepLorentzVector pgam=(0,0,0,0);
   HepPoint3D xorigin(0.,0.,0.);
   xorigin[0]=9999.;
   xorigin[1]=9999.;
   xorigin[2]=9999.;
   HepSymMatrix xem(3,0);

   int cl4pi=0; 
   int clajs=0;
   HepLorentzVector p4psipjs(0.011*m_ecms, 0.0, 0.0, m_ecms);
   double psipBetajs = (p4psipjs.vect()).mag()/(p4psipjs.e());
   HepLorentzVector p4psip(0.011*m_ecms, 0.0, 0.0, m_ecms);
   double psipBeta = (p4psip.vect()).mag()/(p4psip.e()); // beta = P/E
   
   for(int ii = 0; ii < npip-1; ii++) {
      RecMdcKalTrack *pip1Trk = (*(evtRecTrkCol->begin()+ipip[ii]))->mdcKalTrack();
      for(int ij = ii+1; ij < npip; ij++) {
	 RecMdcKalTrack *pip2Trk = (*(evtRecTrkCol->begin()+ipip[ij]))->mdcKalTrack();
	 for(int ik = 0; ik < npim-1; ik++) {
	    RecMdcKalTrack *pim1Trk = (*(evtRecTrkCol->begin()+ipim[ik]))->mdcKalTrack();
	    for(int il = ik+1; il < npim; il++) {  
	       RecMdcKalTrack *pim2Trk = (*(evtRecTrkCol->begin()+ipim[il]))->mdcKalTrack();
	       double squar[3]={9999.,9999.,9999.};
	       double squarkpi[6]={9999.,9999.,9999.,9999.,9999.,9999.};
	       WTrackParameter wvpip1Trk, wvpim1Trk, wvpip2Trk, wvpim2Trk;

	       Vint iGoodjs;
	       iGoodjs.clear();
	       Vdouble pTrkjs;
	       pTrkjs.clear();



	       pTrkjs.push_back(pip1Trk->p()*pip1Trk->charge());
	       pTrkjs.push_back(pip2Trk->p()*pip2Trk->charge());
	       pTrkjs.push_back(pim1Trk->p()*pim1Trk->charge());
	       pTrkjs.push_back(pim2Trk->p()*pim2Trk->charge());
	       iGoodjs.push_back(ipip[ii]);  
	       iGoodjs.push_back(ipip[ij]);
	       iGoodjs.push_back(ipim[ik]);
	       iGoodjs.push_back(ipim[il]);

	       Gam4pikp::BubbleSort(pTrkjs, iGoodjs);
	       Vint jGoodjs;
	       jGoodjs.clear();
	       Vp4 p4chTrkjs; 
	       p4chTrkjs.clear();  
	       Vint i4cpip1js, i4cpip2js, i4cpim1js, i4cpim2js;
	       i4cpip1js.clear(); 
	       i4cpip2js.clear();
	       i4cpim1js.clear();
	       i4cpim2js.clear();   
	       i4cpip1js.push_back(iGoodjs[2]);
	       i4cpip2js.push_back(iGoodjs[3]);
	       i4cpim1js.push_back(iGoodjs[1]);
	       i4cpim2js.push_back(iGoodjs[0]);
	       jGoodjs.push_back(i4cpip1js[0]);
	       jGoodjs.push_back(i4cpim1js[0]);
	       jGoodjs.push_back(i4cpip2js[0]);
	       jGoodjs.push_back(i4cpim2js[0]);

	       for (int i = 0; i < 4; i++)
	       {
		  EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + jGoodjs[i];
		  if (!(*itTrk)->isMdcTrackValid()) continue;
		  RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
		  if (!(*itTrk)->isMdcKalTrackValid()) continue;
		  RecMdcKalTrack *mdcKalTrk = (*itTrk)->mdcKalTrack();
		  double ptrk;
		  HepLorentzVector p4trk;
		  if (i<2)
		  {
		     mdcKalTrk->setPidType(RecMdcKalTrack::pion);
		     ptrk = mdcKalTrk->p();
		     p4trk.setPx(mdcKalTrk->px());
		     p4trk.setPy(mdcKalTrk->py());
		     p4trk.setPz(mdcKalTrk->pz());
		     p4trk.setE(sqrt(ptrk*ptrk+mpi*mpi));
		  }
		  else{
		     mdcKalTrk->setPidType(RecMdcKalTrack::electron);
		     ptrk = mdcKalTrk->p();
		     p4trk.setPx(mdcKalTrk->px());
		     p4trk.setPy(mdcKalTrk->py());
		     p4trk.setPz(mdcKalTrk->pz());
		     p4trk.setE(sqrt(ptrk*ptrk+xmass[0]*xmass[0]));
		  }
		  p4trk.boost(-1.0*psipBetajs, 0.0, 0.0);
		  p4chTrkjs.push_back(p4trk);
	       }
	       p4psipjs.boost(-1.0*psipBetajs, 0.0, 0.0);

	       HepLorentzVector p4pipijs = p4chTrkjs[0] + p4chTrkjs[1];
	       HepLorentzVector p4eejs   = p4chTrkjs[2] + p4chTrkjs[3];
	       HepLorentzVector p4pipirecjs = p4psipjs - p4pipijs;
	       double mpprecjs=p4pipirecjs.m();
	       double mpipijs=p4pipijs.m();
	       double meejs=p4eejs.m();
	       double mcomp=sqrt((mpprecjs-3.097)*(mpprecjs-3.097)+(meejs-3.097)*(meejs-3.097));
	       if(mcomp<mcompall)
	       {
		  mcompall=mcomp;			
		  ipip1js=i4cpip1js[0];
		  ipim1js=i4cpim1js[0];
		  ipip2js=i4cpip2js[0];
		  ipim2js=i4cpim2js[0];
		  mppreclst=mpprecjs;
		  meelst=meejs;
	       }

if(m_rootput)
{
	       m_mpprecall=mppreclst;
	       m_meeall=meelst;
}
	    }

	 }
      }
   }

   //{  
//   HepLorentzVector p4psip(0.011*m_ecms, 0.0, 0.0, m_ecms);
//   double psipBeta = (p4psip.vect()).mag()/(p4psip.e()); // beta = P/E
   Vint iGood4c;
   iGood4c.clear();
   Vdouble pTrk4c;
   pTrk4c.clear();
   Vint jGood;
   jGood.clear();
   Vint jsGood;
   jsGood.clear(); 

   if(mcompall>9997)
      return StatusCode::SUCCESS;

   jsGood.push_back(ipip1js);
   jsGood.push_back(ipim1js);
   jsGood.push_back(ipip2js);
   jsGood.push_back(ipim2js);  

   for(int i = 0; i < evtRecEvent->totalCharged(); i++)
   {
      if(i>=evtRecTrkCol->size()) break;
      EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
      if(!(*itTrk)->isMdcTrackValid()) continue;
      if (!(*itTrk)->isMdcKalTrackValid()) continue;
      RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
      if((i!=ipip1js)&&(i!=ipim1js)&&(i!=ipip2js)&&(i!=ipim2js))
      {
	 jsGood.push_back(i);
      }
   }     

   int njsGood=jsGood.size();
   //ParticleID *pid = ParticleID::instance();
if(m_rootput)   
{
   for (int i = 0; i < njsGood; i++)
   {
      EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + jsGood[i];
      if (!(*itTrk)->isMdcTrackValid()) continue;
      RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
      if (!(*itTrk)->isMdcKalTrackValid()) continue;
      RecMdcKalTrack *mdcKalTrk = (*itTrk)->mdcKalTrack();
      double ptrk;
      ptrk = mdcKalTrk->p();
      m_x0js[i] = mdcTrk->x();
      m_y0js[i] = mdcTrk->y();
      m_z0js[i] = mdcTrk->z();
      m_r0js[i] = mdcTrk->r();
      m_ppmdcjs[i] = mdcTrk->p();
      m_pxmdcjs[i] = mdcTrk->px();
      m_pymdcjs[i] = mdcTrk->py();
      m_pzmdcjs[i] = mdcTrk->pz();
      m_ppkaljs[i] = mdcKalTrk->p();
      Hep3Vector p3jsi(mdcTrk->px(),mdcTrk->py(),mdcTrk->pz());    
      if(njsGood>4){
	 EvtRecTrackIterator  itTrk5 = evtRecTrkCol->begin() + jsGood[4];
	 RecMdcTrack *mdcTrk5 = (*itTrk5)->mdcTrack();
	 Hep3Vector p3js5(mdcTrk5->px(),mdcTrk5->py(),mdcTrk5->pz());
	 m_angjs5[i]=p3jsi.angle(p3js5);
	 m_nearjs5[i]=p3jsi.howNear(p3js5);
      }
      if(njsGood>5){
	 EvtRecTrackIterator  itTrk6 = evtRecTrkCol->begin() + jsGood[5];
	 RecMdcTrack *mdcTrk6 = (*itTrk6)->mdcTrack();
	 Hep3Vector p3js6(mdcTrk6->px(),mdcTrk6->py(),mdcTrk6->pz());
	 m_angjs6[i]=p3jsi.angle(p3js6);
	 m_nearjs6[i]=p3jsi.howNear(p3js6);
      }


      m_ptmdcjs[i] = mdcTrk->pxy();
      m_ptkaljs[i] = mdcKalTrk->pxy();
      double x0=mdcTrk->x();
      double y0=mdcTrk->y();
      double z0=mdcTrk->z();
      double phi0=mdcTrk->helix(1);
      double xv=v[0];
      double yv=v[1];
      double zv=v[2];
      double Rxy=(x0-xv)*cos(phi0)+(y0-yv)*sin(phi0);
      double Rz=z0-zv;
      m_Rxyjs[i]=Rxy;
      m_Rzjs[i]=Rz;
      HepVector a = mdcTrk->helix();
      HepSymMatrix Ea = mdcTrk->err();
      HepPoint3D pivot(0.,0.,0.);
      HepPoint3D IP(v[0],v[1],v[2]);
      VFHelix helixp(pivot,a,Ea);
      helixp.pivot(IP);
      HepVector vec = helixp.a();
      m_Rnxyjs[i]=vec[0];
      m_Rnzjs[i]=vec[3];
      m_phinjs[i]=vec[1];   
      //tof
      if ((*itTrk)->isTofTrackValid())
      {    m_bjtofvaljs[i]=1;
	 m_cotof1js[i]=1;  
	 SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();
	 SmartRefVector<RecTofTrack>::iterator iter_tof=tofTrkCol.begin();
	 for (;iter_tof != tofTrkCol.end(); iter_tof++ ){
	    TofHitStatus *status = new TofHitStatus;
	    status->setStatus((*iter_tof)->status());
	    if(status->is_cluster()){
	       m_bjtofjs[i]=1;		    
	       m_counterjs[i] = status->is_counter();
	       m_barreljs[i]  = status->is_barrel();
	       m_layertofjs[i]   = status->layer();
	       m_readoutjs[i] = status->is_readout();
	       m_clusterjs[i] = status->is_cluster();	    
	       m_cotof2js[i]=2;    
	       m_betajs[i]= (*iter_tof)->beta();
	       m_tofjs[i] = (*iter_tof)->tof();
	       m_tofpathjs[i] = (*iter_tof)->path();
	       m_zhitjs[i]= (*iter_tof)->zrhit();
	       m_texejs[i] = (*iter_tof)->texpElectron();
	       m_texmujs[i] = (*iter_tof)->texpMuon();
	       m_texpijs[i] = (*iter_tof)->texpPion();
	       m_texkjs[i] = (*iter_tof)->texpKaon();
	       m_texprjs[i] = (*iter_tof)->texpProton();
	       m_dtejs[i]  = m_tofjs[i] - m_texejs[i];
	       m_dtmujs[i]  = m_tofjs[i] - m_texmujs[i];
	       m_dtpijs[i]  = m_tofjs[i] - m_texpijs[i];
	       m_dtkjs[i]  = m_tofjs[i] - m_texkjs[i];
	       m_dtprjs[i]  = m_tofjs[i] - m_texprjs[i];

	       m_sigmaetofjs[i]=(*iter_tof)->sigma(0);
	       m_sigmamutofjs[i]=(*iter_tof)->sigma(1);
	       m_sigmapitofjs[i]=(*iter_tof)->sigma(2);
	       m_sigmaktofjs[i]=(*iter_tof)->sigma(3);
	       m_sigmaprtofjs[i]=(*iter_tof)->sigma(4);
	       m_t0tofjs[i]=(*iter_tof)->t0();
	       m_errt0tofjs[i]=(*iter_tof)->errt0();   

	       m_tofIDjs[i]=(*iter_tof)->tofID();
	       m_clusterIDjs[i]=(*iter_tof)->tofTrackID();
	    }
	    delete status;
	 }
      }	
      //dedx
      if ((*itTrk)->isMdcDedxValid())
      {

	 RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();
	 m_chiejs[i] = dedxTrk->chiE();
	 m_chimujs[i] = dedxTrk->chiMu();
	 m_chipijs[i] = dedxTrk->chiPi();
	 m_chikjs[i] = dedxTrk->chiK();
	 m_chipjs[i] = dedxTrk->chiP();
	 m_ghitjs[i] = dedxTrk->numGoodHits();
	 m_thitjs[i] = dedxTrk->numTotalHits();
	 m_probphjs[i] = dedxTrk->probPH();
	 m_normphjs[i] = dedxTrk->normPH();
      }	  



      //emc      
      if ( (*itTrk)->isEmcShowerValid() )
      {
	 RecEmcShower* emcTrk = (*itTrk)->emcShower();
	 m_bjemcjs[i]=1;
	 m_emcjs[i] = emcTrk->energy();
	 m_evpjs[i] = emcTrk->energy()/ptrk;
	 m_timecgjs[i] = emcTrk->time();
	 //            totalEnergy += emcTrk->energy();
      }

      //muc
      if ( (*itTrk)->isMucTrackValid() )
      {
	 RecMucTrack* mucTrk = (*itTrk)->mucTrack();
	 double dpthp = mucTrk->depth()/25.0;//why?
	 //           m_depthmucjs[i] = dpthp<10.0 ? dpthp : 10.0;
	 m_bjmucjs[i]=1;
	 m_depthmucjs[i]=mucTrk->depth();	   
	 m_layermucjs[i] = mucTrk->numLayers();
      }

      m_cosmdcjs[i] = cos(mdcTrk->theta());
      m_phimdcjs[i] = mdcTrk->phi();
      //pid
      pid->init();
      pid->setMethod(pid->methodProbability());
      pid->setChiMinCut(4);
      pid->setRecTrack(*itTrk);
      pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2());
      pid->identify(pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());
      pid->identify(pid->onlyElectron() | pid->onlyMuon());
      pid->calculate();
      if(!(pid->IsPidInfoValid())) continue;
      //      m_costpidjs[i] = cos(mdcTrk->theta());
      m_dedxpidjs[i] = pid->chiDedx(2);
      m_tof1pidjs[i] = pid->chiTof1(2);
      m_tof2pidjs[i] = pid->chiTof2(2);
      m_probejs[i] = pid->probElectron();
      m_probmujs[i] = pid->probMuon();
      m_probpijs[i] = pid->probPion();
      m_probkjs[i] = pid->probKaon();
      m_probprjs[i] = pid->probProton();      



   }

}

    Vint jGam2kpi;
    jGam2kpi.clear();
   
   Vint iGood2kpi, ipip2kpi, ipim2kpi;
   iGood2kpi.clear();
   ipip2kpi.clear();
   ipim2kpi.clear();
   Vp4 ppip2kpi, ppim2kpi;
   ppip2kpi.clear();
   ppim2kpi.clear();

   Vint ipipnofit, ipimnofit, ikpnofit, ikmnofit, ipropnofit, ipromnofit;
   ipipnofit.clear();
   ipimnofit.clear(); 
   ikpnofit.clear();
   ikmnofit.clear(); 
   ipropnofit.clear();
   ipromnofit.clear();
   Vp4 ppipnofit, ppimnofit, pkpnofit, pkmnofit, ppropnofit, ppromnofit;
   ppipnofit.clear();
   ppimnofit.clear();
   pkpnofit.clear();
   pkmnofit.clear();
   ppropnofit.clear();
   ppromnofit.clear();

   Vdouble p3pip2kpi;
   p3pip2kpi.clear();
   Vdouble p3pim2kpi;
   p3pim2kpi.clear();

   Vint itrak2kpi;
   itrak2kpi.clear();
   int cls2kpi;
   double chis2kpi=9999.;
   double m4piall=9999.;
   double m4kall=9999.;
   double mgam4piall=9999.;
   double mgam4kall=9999.;
   for(int i = 0; i < evtRecEvent->totalCharged(); i++){
      if(i>=evtRecTrkCol->size()) break;
      EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
      if(!(*itTrk)->isMdcTrackValid()) continue;
      if (!(*itTrk)->isMdcKalTrackValid()) continue;
      RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
      double    z02kpi = mdcTrk->z();
      double    r02kpi = mdcTrk->r();   
      HepVector a = mdcTrk->helix();
      HepSymMatrix Ea = mdcTrk->err();
      HepPoint3D pivot(0.,0.,0.);
      HepPoint3D IP(v[0],v[1],v[2]);
      VFHelix helixp(pivot,a,Ea);
      helixp.pivot(IP);
      HepVector vec = helixp.a();
      double    Rnxy02kpi=vec[0];
      double    Rnz02kpi=vec[3];    
      //  if(fabs(Rnxy02kpi>1.0)) continue;
      //  if(fabs(Rnz02kpi>10.0)) continue;
      iGood2kpi.push_back((*itTrk)->trackId());
   }
   int nGood2kpi = iGood2kpi.size();
   if(nGood2kpi==4)
   {
      for(int i = 0; i < nGood2kpi; i++) {
	 EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood2kpi[i];
	 RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
	 RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();
	 if(mdcKalTrk->charge() >0) {
	    ipip2kpi.push_back(iGood2kpi[i]);
	    p3pip2kpi.push_back(mdcKalTrk->p());
	    HepLorentzVector ptrk;
	    ptrk.setPx(mdcKalTrk->px());
	    ptrk.setPy(mdcKalTrk->py());
	    ptrk.setPz(mdcKalTrk->pz());
	    double p3 = ptrk.mag();
	    ptrk.setE(sqrt(p3*p3+mpi*mpi));
	    ppip2kpi.push_back(ptrk);
	 } else {
	    ipim2kpi.push_back(iGood2kpi[i]);
	    p3pim2kpi.push_back(mdcKalTrk->p());
	    HepLorentzVector ptrk;
	    ptrk.setPx(mdcKalTrk->px());
	    ptrk.setPy(mdcKalTrk->py());
	    ptrk.setPz(mdcKalTrk->pz());
	    double p3 = ptrk.mag();
	    ptrk.setE(sqrt(p3*p3+mpi*mpi));
	    ppim2kpi.push_back(ptrk);
	 }
      }    
      int npip2kpi = ipip2kpi.size();
      int npim2kpi = ipim2kpi.size();



      ParticleID *pid = ParticleID::instance();

if(m_rootput)
{
      m_cy2kpi=6;
}

      if((npip2kpi == 2)&&(npim2kpi == 2))
      {
	 //if(nGamnofit>=1)
	 

Ncut[4]++;

	 HepPoint3D xorigin2kpi(0.,0.,0.);
	 xorigin2kpi[0]=9999.;
	 xorigin2kpi[1]=9999.;
	 xorigin2kpi[2]=9999.;
	 HepSymMatrix xem2kpi(3,0);

	 Gam4pikp::BubbleSort(p3pip2kpi, ipip2kpi);
	 Gam4pikp::BubbleSort(p3pim2kpi, ipim2kpi);

	 Mcut[1]++;	
 RecMdcKalTrack *pip12kpiTrk = (*(evtRecTrkCol->begin()+ipip2kpi[0]))->mdcKalTrack();
 RecMdcKalTrack *pip22kpiTrk = (*(evtRecTrkCol->begin()+ipip2kpi[1]))->mdcKalTrack();
 RecMdcKalTrack *pim12kpiTrk = (*(evtRecTrkCol->begin()+ipim2kpi[0]))->mdcKalTrack();
 RecMdcKalTrack *pim22kpiTrk = (*(evtRecTrkCol->begin()+ipim2kpi[1]))->mdcKalTrack();
 double squar2kpi[10]={9999.,9999.,9999.,9999.,9999.,9999.,9999.,9999.,9999.,9999.};
	 double mc12kpi,mc22kpi,mc32kpi,mc42kpi;
	 // double chis2kpi=9999.; 
	 WTrackParameter wcgp12kpi;
	 WTrackParameter wcgp22kpi;
	 WTrackParameter wcgm12kpi;
	 WTrackParameter wcgm22kpi;
	 int icgp12kpi=-1;
	 int icgp22kpi=-1;
	 int icgm12kpi=-1;
	 int icgm22kpi=-1;
	 int igam2kpi=-1;
	 double m2kpi=9999;

	 int n20=0;
	 int n30=0;
	 WTrackParameter wvpip12kpiTrk, wvpim12kpiTrk, wvpip22kpiTrk, wvpim22kpiTrk; 
	 for(int k=0;k<6;k++)
	 {
	    if(k==0){mc12kpi=mpi;mc22kpi=mpi;mc32kpi=mpi;mc42kpi=mpi;}
	    if(k==1){mc12kpi=mk;mc22kpi=mk;mc32kpi=mk;mc42kpi=mk;}     
	    if(k==2){mc12kpi=mpi;mc22kpi=mpro;mc32kpi=mpi;mc42kpi=mpro;}
	    if(k==3){mc12kpi=mpro;mc22kpi=mpi;mc32kpi=mpro;mc42kpi=mpi;}
	    if(k==4){mc12kpi=mpi;mc22kpi=mpro;mc32kpi=mpro;mc42kpi=mpi; }
	    if(k==5){mc12kpi=mpro;mc22kpi=mpi;mc32kpi=mpi;mc42kpi=mpro;}



    wvpip12kpiTrk = WTrackParameter(mc12kpi, pip12kpiTrk->getZHelix(), pip12kpiTrk->getZError());
    wvpip22kpiTrk = WTrackParameter(mc22kpi, pip22kpiTrk->getZHelix(), pip22kpiTrk->getZError());
    wvpim12kpiTrk = WTrackParameter(mc32kpi, pim12kpiTrk->getZHelix(), pim12kpiTrk->getZError());
    wvpim22kpiTrk = WTrackParameter(mc42kpi, pim22kpiTrk->getZHelix(), pim22kpiTrk->getZError());
	    HepPoint3D vx(0., 0., 0.);
	    HepSymMatrix Evx(3, 0);
	    double bx = 1E+6;
	    double by = 1E+6;
	    double bz = 1E+6;
	    Evx[0][0] = bx*bx;
	    Evx[1][1] = by*by;
	    Evx[2][2] = bz*bz;
	    VertexParameter vxpar;
	    vxpar.setVx(vx);
	    vxpar.setEvx(Evx);
	    VertexFit* vtxfit = VertexFit::instance();
	    vtxfit->init();
	    vtxfit->AddTrack(0,  wvpip12kpiTrk);
	    vtxfit->AddTrack(1,  wvpim12kpiTrk);
	    vtxfit->AddTrack(2,  wvpip22kpiTrk);
	    vtxfit->AddTrack(3,  wvpim22kpiTrk);
	    vtxfit->AddVertex(0, vxpar,0, 1, 2, 3);
	    if(!vtxfit->Fit(0)) continue;
	    vtxfit->Swim(0);  
	    WTrackParameter wpip12kpi = vtxfit->wtrk(0);
	    WTrackParameter wpim12kpi = vtxfit->wtrk(1);
	    WTrackParameter wpip22kpi = vtxfit->wtrk(2);
	    WTrackParameter wpim22kpi = vtxfit->wtrk(3);  
	    WTrackParameter wpip12kpiys = vtxfit->wtrk(0);
	    WTrackParameter wpim12kpiys = vtxfit->wtrk(1);
	    WTrackParameter wpip22kpiys = vtxfit->wtrk(2);
	    WTrackParameter wpim22kpiys = vtxfit->wtrk(3);  
	    xorigin2kpi = vtxfit->vx(0);
	    xem2kpi = vtxfit->Evx(0);  
	    KalmanKinematicFit * kmfit = KalmanKinematicFit::instance();

	    HepLorentzVector ecms(0.040547,0,0,3.68632);

	    double chisq = 9999.;
	    int ig1 = -1;
	    for(int i = 0; i < nGam; i++) {
	       RecEmcShower *g1Trk = (*(evtRecTrkCol->begin()+iGam[i]))->emcShower();
	       kmfit->init();
	       kmfit->setBeamPosition(xorigin2kpi);
	       kmfit->setVBeamPosition(xem2kpi);      
	       kmfit->AddTrack(0, wpip12kpi);
	       kmfit->AddTrack(1, wpim12kpi);
	       kmfit->AddTrack(2, wpip22kpi);
	       kmfit->AddTrack(3, wpim22kpi);	
	       kmfit->AddTrack(4, 0.0, g1Trk);
	       kmfit->AddFourMomentum(0, p4psip);
	       //        if(!kmfit->Fit(0)) continue;
	       bool oksq = kmfit->Fit();
	       if(oksq) {
		  double chi2 = kmfit->chisq();
		  if(chi2 < chisq) {
		     chisq = chi2;
		     squar2kpi[k]=chi2;
		     ig1 = iGam[i];

		  }
	       }
	    }
	    if(squar2kpi[k]<200&&squar2kpi[k]<chis2kpi)
	    {	 
//	       m_comcs2kpi[k]=squar2kpi[k]; 
	       chis2kpi=squar2kpi[k];
	       if(squar2kpi[k]<20) n20=n20+1;
	       if(squar2kpi[k]<30) n30=n30+1;

	       icgp12kpi=ipip2kpi[0];
	       icgp22kpi=ipip2kpi[1];
	       icgm12kpi=ipim2kpi[0];
	       icgm22kpi=ipim2kpi[1];
	       igam2kpi=ig1;
	       wcgp12kpi=wpip12kpiys;
	       wcgp22kpi=wpip22kpiys;
	       wcgm12kpi=wpim12kpiys;
	       wcgm22kpi=wpim22kpiys;   
	       cls2kpi=k;

	       if(k==0){
		  itrak2kpi.push_back(ipip2kpi[0]); itrak2kpi.push_back(ipim2kpi[0]);
		  itrak2kpi.push_back(ipip2kpi[1]); itrak2kpi.push_back(ipim2kpi[1]);
	       }

	       if(k==1){
		  itrak2kpi.push_back(ipip2kpi[0]); itrak2kpi.push_back(ipim2kpi[0]);
		  itrak2kpi.push_back(ipip2kpi[1]); itrak2kpi.push_back(ipim2kpi[1]);
	       }


	       if(k==2){
		  itrak2kpi.push_back(ipip2kpi[0]); itrak2kpi.push_back(ipim2kpi[0]);
		  itrak2kpi.push_back(ipip2kpi[1]); itrak2kpi.push_back(ipim2kpi[1]);
	       }

	       if(k==3){
		  itrak2kpi.push_back(ipip2kpi[1]); itrak2kpi.push_back(ipim2kpi[1]);
		  itrak2kpi.push_back(ipip2kpi[0]); itrak2kpi.push_back(ipim2kpi[0]);
	       }

	       if(k==4){
		  itrak2kpi.push_back(ipip2kpi[0]); itrak2kpi.push_back(ipim2kpi[1]);
		  itrak2kpi.push_back(ipip2kpi[1]); itrak2kpi.push_back(ipim2kpi[0]);
	       }

	       if(k==5){
		  itrak2kpi.push_back(ipip2kpi[1]); itrak2kpi.push_back(ipim2kpi[0]);
		  itrak2kpi.push_back(ipip2kpi[0]); itrak2kpi.push_back(ipim2kpi[1]);
	       }	      





	       RecEmcShower *g1Trk = (*(evtRecTrkCol->begin()+igam2kpi))->emcShower();
	       kmfit->init();
	       kmfit->setBeamPosition(xorigin2kpi);
	       kmfit->setVBeamPosition(xem2kpi);
	       kmfit->AddTrack(0, wpip12kpi);
	       kmfit->AddTrack(1, wpim12kpi);
	       kmfit->AddTrack(2, wpip22kpi);
	       kmfit->AddTrack(3, wpim22kpi);
	       kmfit->AddTrack(4, 0.0, g1Trk);
	       kmfit->AddFourMomentum(0, p4psip);
	       bool oksq = kmfit->Fit();
	       if(oksq){
                  HepLorentzVector pchic2kpi = kmfit->pfit(0) + kmfit->pfit(1)+kmfit->pfit(2) + kmfit->pfit(3);
		  HepLorentzVector ppsip2kpi = kmfit->pfit(0) + kmfit->pfit(1)+kmfit->pfit(2) + kmfit->pfit(3) + kmfit->pfit(4);
		  mchic2kpilst=pchic2kpi.m();
		  chis4c2kpilst=kmfit->chisq();
if(m_rootput)
{	  
		  m_mchic2kpi = pchic2kpi.m();
		  m_chis2kpi = kmfit->chisq();
		  m_mpsip2kpi=ppsip2kpi.m();
}		  



	       }      


	    }
	 }


	 if(chis2kpi<200)
	 {
Ncut[5]++;
	 if(m_rootput)
	 {
            m_ncy20=n20;
	    m_ncy30=n30; 
	    m_cla2kpi=cls2kpi;
	 }
	    type2kpilst=cls2kpi;
	    
	    Vp4 p2kpi;
	    p2kpi.clear();

	    for (int i = 0; i < 4; i++)
	    {	      
	       EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + itrak2kpi[i];
	       if (!(*itTrk)->isMdcTrackValid()) continue;
	       RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
	       if (!(*itTrk)->isMdcKalTrackValid()) continue;
	       RecMdcKalTrack *mdcKalTrk = (*itTrk)->mdcKalTrack();
	       double ptrk2kpi;
	       HepLorentzVector p4trk2kpi;
	       if(cls2kpi==1)
	       {
		  mdcKalTrk->setPidType(RecMdcKalTrack::kaon);
		  ptrk2kpi = mdcKalTrk->p();
		  p4trk2kpi.setPx(mdcKalTrk->px());
		  p4trk2kpi.setPy(mdcKalTrk->py());
		  p4trk2kpi.setPz(mdcKalTrk->pz());
		  p4trk2kpi.setE(sqrt(ptrk2kpi*ptrk2kpi+mk*mk));    
		  p2kpi.push_back(p4trk2kpi);
	       }	 

	       if(cls2kpi==2)
	       {
		  if (i<2)
		  {
		     mdcKalTrk->setPidType(RecMdcKalTrack::pion);
		     ptrk2kpi = mdcKalTrk->p();
		     p4trk2kpi.setPx(mdcKalTrk->px());
		     p4trk2kpi.setPy(mdcKalTrk->py());
		     p4trk2kpi.setPz(mdcKalTrk->pz());
		     p4trk2kpi.setE(sqrt(ptrk2kpi*ptrk2kpi+mpi*mpi));
		     p2kpi.push_back(p4trk2kpi);	  

		  }					 
		  else{
		     mdcKalTrk->setPidType(RecMdcKalTrack::proton);
		     ptrk2kpi = mdcKalTrk->p();
		     p4trk2kpi.setPx(mdcKalTrk->px());
		     p4trk2kpi.setPy(mdcKalTrk->py());
		     p4trk2kpi.setPz(mdcKalTrk->pz());
		     p4trk2kpi.setE(sqrt(ptrk2kpi*ptrk2kpi+mpro*mpro));
		     p2kpi.push_back(p4trk2kpi);	  

		  }    

	       }	
	       if(cls2kpi!=1&&cls2kpi!=2)
	       {
		  mdcKalTrk->setPidType(RecMdcKalTrack::pion);
		  ptrk2kpi = mdcKalTrk->p();
		  p4trk2kpi.setPx(mdcKalTrk->px());
		  p4trk2kpi.setPy(mdcKalTrk->py());
		  p4trk2kpi.setPz(mdcKalTrk->pz());
		  p4trk2kpi.setE(sqrt(ptrk2kpi*ptrk2kpi+mpi*mpi));
		  p2kpi.push_back(p4trk2kpi);
	       }




	    }		 

            for (int i = 0; i < 4; i++)
                  {
            EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + itrak2kpi[i];
                if (!(*itTrk)->isMdcTrackValid()) continue;
                RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
                if (!(*itTrk)->isMdcKalTrackValid()) continue;
                RecMdcKalTrack *mdcKalTrk = (*itTrk)->mdcKalTrack();
               if ((*itTrk)->isTofTrackValid())
                { 
                 SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();
                 SmartRefVector<RecTofTrack>::iterator iter_tof=tofTrkCol.begin();
                  for (;iter_tof != tofTrkCol.end(); iter_tof++ ){
                 TofHitStatus *status = new TofHitStatus;
                 status->setStatus((*iter_tof)->status());
                   if(status->is_cluster()){				                         dtpr2kpilst[i]=((*iter_tof)->tof()-(*iter_tof)->texpProton());
                   }
		   delete status;
		  }
		}
		  }

		
	    /*	    
	    HepLorentzVector ecmsb(0.040547,0,0,3.68632);
	    double chisq = 9999.;
	    RecEmcShower *g1Trk = (*(evtRecTrkCol->begin()+igam2kpi))->emcShower();
	    KalmanKinematicFit * kmfit = KalmanKinematicFit::instance();
	    kmfit->init();
	    kmfit->setBeamPosition(xorigin2kpi);
	    kmfit->setVBeamPosition(xem2kpi);
	    kmfit->AddTrack(0, wcgp12kpi);
	    kmfit->AddTrack(1, wcgp22kpi);
	    kmfit->AddTrack(2, wcgm12kpi);
	    kmfit->AddTrack(3, wcgm22kpi);
	    kmfit->AddTrack(4, 0.0, g1Trk);
	    kmfit->AddFourMomentum(0, p4psip);
	    bool oksq = kmfit->Fit();
	    if(oksq) {
	       Mcut[3]++;
	       HepLorentzVector pchic4c2kpi = kmfit->pfit(0) + kmfit->pfit(1)+kmfit->pfit(2) + kmfit->pfit(3);
	       HepLorentzVector ppsip4c2kpi = kmfit->pfit(0) + kmfit->pfit(1)+kmfit->pfit(2) + kmfit->pfit(3) + kmfit->pfit(4);

	       m_mchic4c2kpi = pchic4c2kpi.m();
//	       m2kpi=m_mchic4c2kpi;
	       m_chis4c2kpi = kmfit->chisq();
	       m_mpsip4c2kpi=ppsip4c2kpi.m();




	    }      

*/
	    Mcut[2]++;	
if(m_rootput)
{
	    for (int i = 0; i < 4; i++)
	    {                 
	       EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + itrak2kpi[i];
	       if (!(*itTrk)->isMdcTrackValid()) continue;
	       RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
	       if (!(*itTrk)->isMdcKalTrackValid()) continue;
	       RecMdcKalTrack *mdcKalTrk = (*itTrk)->mdcKalTrack();
	       m_ppmdc2kpi[i]=mdcTrk->p();
	       m_pxmdc2kpi[i]=mdcTrk->px();
	       m_pymdc2kpi[i]=mdcTrk->py();
	       m_pzmdc2kpi[i]=mdcTrk->pz();
	       m_ppkal2kpi[i]=mdcKalTrk->p();
	       m_charge2kpi[i]=mdcKalTrk->charge();	      
	       double ptrk;
	       ptrk=mdcKalTrk->p();

	       if (eventHeader->runNumber()<0)
	       {double mcall=9999;
		  SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), "/Event/MC/McParticleCol");
		  int m_numParticle = 0;
		  if (!mcParticleCol)
		  {
//		     std::cout << "Could not retrieve McParticelCol" << std::endl;
		     return StatusCode::FAILURE;
		  }
		  else
		  {
		     Event::McParticleCol::iterator iter_mc = mcParticleCol->begin();
		     for (; iter_mc != mcParticleCol->end(); iter_mc++)
		     {   double commc;
			int  pdgcode = (*iter_mc)->particleProperty();
			float px,py,pz;
			px=(*iter_mc)->initialFourMomentum().x();
			py=(*iter_mc)->initialFourMomentum().y();
			pz=(*iter_mc)->initialFourMomentum().z();

			commc=ptrk*ptrk-px*px-py*py-pz*pz;
			if(fabs(commc)<fabs(mcall))
			{
			   mcall=commc;	
			   m_pdg[i]=pdgcode;
			   m_cbmc[i]=commc;
			}	

		     }

		  }

	       }

	       if ((*itTrk)->isTofTrackValid())
	       { m_bjtofval2kpi[i]=1;
		  m_cotof12kpi[i]=1;
		  SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();
		  SmartRefVector<RecTofTrack>::iterator iter_tof=tofTrkCol.begin();
		  for (;iter_tof != tofTrkCol.end(); iter_tof++ ){
		     TofHitStatus *status = new TofHitStatus;
		     status->setStatus((*iter_tof)->status());

		     if(status->is_cluster()){
			
			
			m_bjtof2kpi[i]=1;		    
			m_counter2kpi[i] = status->is_counter();
			m_barrel2kpi[i]  = status->is_barrel();
			m_layertof2kpi[i]   = status->layer();
			m_readout2kpi[i] = status->is_readout();
			m_cluster2kpi[i] = status->is_cluster();	    
			m_cotof22kpi[i]=2;
			m_beta2kpi[i]= (*iter_tof)->beta();
			m_tof2kpi[i] = (*iter_tof)->tof();
			m_tofpath2kpi[i] = (*iter_tof)->path();
			m_zhit2kpi[i]= (*iter_tof)->zrhit();
			m_texe2kpi[i] = (*iter_tof)->texpElectron();
			m_texmu2kpi[i] = (*iter_tof)->texpMuon();
			m_texpi2kpi[i] = (*iter_tof)->texpPion();
			m_texk2kpi[i] = (*iter_tof)->texpKaon();
			m_texpr2kpi[i] = (*iter_tof)->texpProton();
			m_dte2kpi[i]  = m_tof2kpi[i] - m_texe2kpi[i];
			m_dtmu2kpi[i]  = m_tof2kpi[i] - m_texmu2kpi[i];
			m_dtpi2kpi[i]  = m_tof2kpi[i] - m_texpi2kpi[i];
			m_dtk2kpi[i]  = m_tof2kpi[i] - m_texk2kpi[i];
			m_dtpr2kpi[i]  = m_tof2kpi[i] - m_texpr2kpi[i];
			m_tofID2kpi[i]=(*iter_tof)->tofID();
			m_clusterID2kpi[i]=(*iter_tof)->tofTrackID();
			m_sigmaetof2kpi[i]=(*iter_tof)->sigma(0);
			m_sigmamutof2kpi[i]=(*iter_tof)->sigma(1);
			m_sigmapitof2kpi[i]=(*iter_tof)->sigma(2);
			m_sigmaktof2kpi[i]=(*iter_tof)->sigma(3);
			m_sigmaprtof2kpi[i]=(*iter_tof)->sigma(4);
			m_t0tof2kpi[i]=(*iter_tof)->t0();
			m_errt0tof2kpi[i]=(*iter_tof)->errt0();

		     }
		     delete status;
		  }
	       }		  






	       if ((*itTrk)->isMdcDedxValid())
	       { 
		  RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();
		  m_chie2kpi[i] = dedxTrk->chiE();
		  m_chimu2kpi[i] = dedxTrk->chiMu();
		  m_chipi2kpi[i] = dedxTrk->chiPi();
		  m_chik2kpi[i] = dedxTrk->chiK();
		  m_chip2kpi[i] = dedxTrk->chiP();
		  m_ghit2kpi[i] = dedxTrk->numGoodHits();
		  m_thit2kpi[i] = dedxTrk->numTotalHits();                    
		  m_probph2kpi[i] = dedxTrk->probPH();
		  m_normph2kpi[i] = dedxTrk->normPH();
	       }

	       //emc 
	       if ( (*itTrk)->isEmcShowerValid() )
	       {
		  RecEmcShower* emcTrk = (*itTrk)->emcShower();
		  m_bjemc2kpi[i]=1; 
		  m_emc2kpi[i] = emcTrk->energy();
		  m_evp2kpi[i] = emcTrk->energy()/ptrk;
		  m_timecg2kpi[i] = emcTrk->time();
	       }


	       //muc
	       if ( (*itTrk)->isMucTrackValid() )
	       {
		  RecMucTrack* mucTrk = (*itTrk)->mucTrack();
		  double dpthp = mucTrk->depth()/25.0;//why?
		  m_bjmuc2kpi[i]=1;  
		  m_depthmuc2kpi[i]=mucTrk->depth();
		  m_layermuc2kpi[i] = mucTrk->numLayers();
	       }   

	       m_cosmdc2kpi[i] = cos(mdcTrk->theta());
	       m_phimdc2kpi[i] = mdcTrk->phi();

	       m_pidnum2kpi[i]=(*itTrk)->partId();

	       if(m_skim4pi)
	       {
 if(mppreclst<3.06&&chis4c2kpilst<20&&((mchic2kpilst>3.39&&mchic2kpilst<3.44)||(mchic2kpilst>3.5&&mchic2kpilst<3.57))&&type2kpilst==0)		  
		  {
		     if(i<4) (*itTrk)->setPartId(3);
		  }

if(mppreclst<3.06&&chis4c2kpilst<20&&((mchic2kpilst>3.39&&mchic2kpilst<3.44)||(mchic2kpilst>3.5&&mchic2kpilst<3.57))&&type2kpilst==1&&dtpr2kpilst[0]<-0.4&&dtpr2kpilst[1]<-0.4&&dtpr2kpilst[2]<-0.4&&dtpr2kpilst[3]<-0.4) 
		  {
		     if(i<4) (*itTrk)->setPartId(4);
		  }


if(mppreclst<3.06&&chis4c2kpilst<20&&((mchic2kpilst>3.39&&mchic2kpilst<3.44)||(mchic2kpilst>3.5&&mchic2kpilst<3.57))&&type2kpilst==2)
		  {
		     if(i==0||i==1) (*itTrk)->setPartId(3);
		     if(i==2||i==3) (*itTrk)->setPartId(5);
		  }

	       }
	       //pid
	       ParticleID *pid = ParticleID::instance();
	       pid->init();
	       pid->setMethod(pid->methodProbability());
	       pid->setChiMinCut(4);
	       pid->setRecTrack(*itTrk);
	       pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2()); 
	       pid->identify(pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());
	       pid->identify(pid->onlyElectron() | pid->onlyMuon());
	       pid->calculate();
	       if(!(pid->IsPidInfoValid())) continue;
	       m_costpid2kpi[i] = cos(mdcTrk->theta());


	       m_probe2kpi[i] = pid->probElectron();
	       m_probmu2kpi[i] = pid->probMuon();
	       m_probpi2kpi[i] = pid->probPion();
	       m_probk2kpi[i] = pid->probKaon();
	       m_probpr2kpi[i] = pid->probProton();






	    }
}  

	 }
//	 Ncut[10]++;

if(mppreclst<3.06&&chis4c2kpilst<20&&((mchic2kpilst>3.39&&mchic2kpilst<3.44)||(mchic2kpilst>3.5&&mchic2kpilst<3.57))&&type2kpilst==0)
{
    jGam2kpi.push_back(igam2kpi);
}

  for(int i = evtRecEvent->totalCharged(); i< evtRecEvent->totalTracks(); i++) {
    if(i>=evtRecTrkCol->size()) break;
      EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
      if(!(*itTrk)->isEmcShowerValid()) continue;
if(mppreclst<3.06&&chis4c2kpilst<20&&((mchic2kpilst>3.39&&mchic2kpilst<3.44)||(mchic2kpilst>3.5&&mchic2kpilst<3.57))&&type2kpilst==0)     
	{
           if(i!=igam2kpi) jGam2kpi.push_back((*itTrk)->trackId());
          }
        else{
          jGam2kpi.push_back((*itTrk)->trackId());
             }
					 
        }

int ngam2kpi=jGam2kpi.size();

if(m_rootput)
{
for(int i = 0; i< ngam2kpi; i++) {
       if(i>=evtRecTrkCol->size()) break;
       EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + jGam2kpi[i];
        if(!(*itTrk)->isEmcShowerValid()) continue;
	RecEmcShower *emcTrk = (*(evtRecTrkCol->begin()+jGam2kpi[i]))->emcShower();
	Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());
	double dthe = 200.;
	double dphi = 200.;
	double dang = 200.;
//	double dthert = 200.;
//	double dphirt = 200.;
//	double dangrt = 200.;
	for(int j = 0; j < evtRecEvent->totalCharged(); j++) {
	   if(j>=evtRecTrkCol->size()) break;
	   EvtRecTrackIterator jtTrk = evtRecTrkCol->begin() + j;
	   if(!(*jtTrk)->isExtTrackValid()) continue;
	   RecExtTrack *extTrk = (*jtTrk)->extTrack();
	   if(extTrk->emcVolumeNumber() == -1) continue;
	   Hep3Vector extpos = extTrk->emcPosition();
	   double angd = extpos.angle(emcpos);
	   double thed = extpos.theta() - emcpos.theta();
	   double phid = extpos.deltaPhi(emcpos);
	   thed = fmod(thed+CLHEP::twopi+CLHEP::twopi+pi, CLHEP::twopi) - CLHEP::pi;
	   phid = fmod(phid+CLHEP::twopi+CLHEP::twopi+pi, CLHEP::twopi) - CLHEP::pi;
//	   if(fabs(thed) < fabs(dthe)) dthe = thed;
//	   if(fabs(phid) < fabs(dphi)) dphi = phid;
//	   if(angd < dang) dang = angd;
	   if(angd < dang) { dang = angd;
	      dthe = thed;
	      dphi = phid;
	   }

	}
	if(dang>=200) continue;
	double eraw = emcTrk->energy();
	dthe = dthe * 180 / (CLHEP::pi);
	dphi = dphi * 180 / (CLHEP::pi);
	dang = dang * 180 / (CLHEP::pi);
//	dthert = dthert * 180 / (CLHEP::pi);
//	dphirt = dphirt * 180 / (CLHEP::pi);
//	dangrt = dangrt * 180 / (CLHEP::pi);
	m_numHits[i]= emcTrk->numHits();
	m_secondmoment[i] = emcTrk->secondMoment();
	m_latmoment[i] = emcTrk->latMoment();
	m_timegm[i] = emcTrk->time();
	m_cellId[i]=emcTrk->cellId();
	m_module[i]=emcTrk->module();
	m_a20Moment[i]=emcTrk->a20Moment();
	m_a42Moment[i]=emcTrk->a42Moment();
	m_getShowerId[i]=emcTrk->getShowerId();
	m_getClusterId[i]=emcTrk->getClusterId();
	m_getEAll[i]=emcTrk->getEAll();
	m_x[i]= emcTrk->x();
	m_y[i]= emcTrk->y();
	m_z[i]= emcTrk->z();
	m_cosemc[i]   = cos(emcTrk->theta());
	m_phiemc[i]   = emcTrk->phi();
	m_energy[i]   = emcTrk->energy();
	m_eSeed[i]    = emcTrk->eSeed();
	m_e3x3[i]     = emcTrk->e3x3();
	m_e5x5[i]     = emcTrk->e5x5();
	m_dang4c[i]   = dang;
	m_dthe4c[i]   = dthe;
	m_dphi4c[i]   = dphi;
//	m_dang4crt[i]   = dangrt;
//	m_dthe4crt[i]   = dthert;
//	m_dphi4crt[i]   = dphirt;

}
}	
}	

}

   

   

      //4pi
      if(m_skim4pi)
      {

if(mppreclst<3.06&&chis4c2kpilst<20&&((mchic2kpilst>3.39&&mchic2kpilst<3.44)||(mchic2kpilst>3.5&&mchic2kpilst<3.57))&&type2kpilst==0)	 
	 {m_subalg1->execute();    //  save gam4pi data 
	    Ncut[6]++;
	 }
      }
	 
	 //4k
      if(m_skim4k)
         {

if(mppreclst<3.06&&chis4c2kpilst<20&&((mchic2kpilst>3.39&&mchic2kpilst<3.44)||(mchic2kpilst>3.5&&mchic2kpilst<3.57))&&type2kpilst==1&&dtpr2kpilst[0]<-0.4&&dtpr2kpilst[1]<-0.4&&dtpr2kpilst[2]<-0.4&&dtpr2kpilst[3]<-0.4)	    
	 {m_subalg2->execute();    //   save gam4k data
	    Ncut[7]++;	
	 }
	 }

      if(m_skim2pi2pr)
         {
if(mppreclst<3.06&&chis4c2kpilst<20&&((mchic2kpilst>3.39&&mchic2kpilst<3.44)||(mchic2kpilst>3.5&&mchic2kpilst<3.57))&&type2kpilst==2)	    
	 {m_subalg3->execute();    //   save gam 2(pi p) data
	    // pi+ pi-  with low momentum and p pbar with high momentum.	
	    Ncut[8]++;
	 }
	 }


      
//cout<<"chis4c2kpilst="<<chis4c2kpilst<<endl;
if(m_rootput)
{

if((mppreclst<3.06&&chis4c2kpilst<40)||((meelst>3.06&&meelst<3.12)&&(fabs(mppreclst-3.097)<0.01)))   
      {      Ncut[9]++;
	 m_tuple1->write();

      }
}

      return StatusCode::SUCCESS;
   }


   // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
   StatusCode Gam4pikp::finalize() {
//      cout<<"total number:                "<<Ncut[0]<<endl;	
//      cout<<"nGood>=4, nCharge==0:        "<<Ncut[1]<<endl;
//      cout<<"nGam>=1:                     "<<Ncut[2]<<endl;
//      cout<<"cgp>2 cgm>2:                 "<<Ncut[3]<<endl;
//      cout<<"2+ 2-:                       "<<Ncut[4]<<endl;
//      cout<<"all cg cycle, chisq<200:     "<<Ncut[5]<<endl;
//      cout<<"4pi ok:                      "<<Ncut[6]<<endl;
//      cout<<"4k ok:                       "<<Ncut[7]<<endl;
//      cout<<"2pi 2p ok:                   "<<Ncut[8]<<endl;
//      cout<<"ntuple write:                "<<Ncut[9]<<endl;
      MsgStream log(msgSvc(), name());
      log << MSG::INFO << "in finalize()" << endmsg;
      return StatusCode::SUCCESS;
   }
   //*************************************************************************
   void Gam4pikp::InitVar()
   {


      m_chis4c2kpi=9999.;
      m_chis2kpi=9999.;
      m_cla2kpi=9999.;
      m_ncy20=9999;
      m_ncy30=9999;
      m_meeall=9999;
      m_mpprecall=9999;
      for (int i=0; i<100; i++)
      {    

	 m_angjs5[i]=9999;
	 m_nearjs5[i]=9999;
	 m_angjs6[i]=9999;
	 m_nearjs6[i]=9999;
	 m_ang4pi5[i]=9999;
	 m_near4pi5[i]=9999;
	 m_ang4pi6[i]=9999;
	 m_near4pi6[i]=9999;
	 m_probe2kpi[i]=-1;
	 m_probmu2kpi[i]=-1;
	 m_probpi2kpi[i]=-1;
	 m_probk2kpi[i]=-1;
	 m_probpr2kpi[i]=-1;

	 m_probejs[i]=-1;
	 m_probmujs[i]=-1;
	 m_probpijs[i]=-1;
	 m_probkjs[i]=-1;
	 m_probprjs[i]=-1;

	 m_cbmc[i]=9999;  
	 m_bjemcjs[i]=0;
	 m_bjemc2kpi[i]=0;
	 m_bjtofjs[i]=0;
	 m_bjtof2kpi[i]=0;
	 m_bjmucjs[i]=0;
	 m_bjmuc2kpi[i]=0;
	 m_charge2kpi[i]=9999;
	 m_ghitjs[i] = 9999;
	 m_thitjs[i] = 9999;
	 m_probphjs[i] = 99999;
	 m_normphjs[i] = 99999;

	 m_ghit2kpi[i] = 9999;
	 m_thit2kpi[i] = 9999;
	 m_probph2kpi[i] = 99999;
	 m_normph2kpi[i] = 99999;



	 m_counterjs[i] = 9999;
	 m_barreljs[i]  = 9999;
	 m_layertofjs[i] = 9999;
	 m_readoutjs[i] = 9999;
	 m_clusterjs[i] = 9999;

	 m_counter2kpi[i] = 9999;
	 m_barrel2kpi[i]  = 9999;
	 m_layertof2kpi[i] = 9999;
	 m_readout2kpi[i] = 9999;
	 m_cluster2kpi[i] = 9999;    

	 m_comcs2kpi[i]=9999;	 
	 m_dte2kpi[i]=9999;
	 m_dtmu2kpi[i]=9999;
	 m_dtpi2kpi[i]=9999;
	 m_dtk2kpi[i]=9999;
	 m_dtpr2kpi[i]=9999;
	 m_sigmaetof2kpi[i]=9999;
	 m_sigmamutof2kpi[i]=9999;
	 m_sigmapitof2kpi[i]=9999;
	 m_sigmaktof2kpi[i]=9999;
	 m_sigmaprtof2kpi[i]=9999;
	 m_t0tof2kpi[i]=9999;
	 m_errt0tof2kpi[i]=9999;

	 m_sigmaetofjs[i]=9999;
	 m_sigmamutofjs[i]=9999;
	 m_sigmapitofjs[i]=9999;
	 m_sigmaktofjs[i]=9999;
	 m_sigmaprtofjs[i]=9999;
	 m_t0tofjs[i]=9999;
	 m_errt0tofjs[i]=9999;

	 m_dtejs[i]=9999;
	 m_dtmujs[i]=9999;
	 m_dtpijs[i]=9999;
	 m_dtkjs[i]=9999;
	 m_dtprjs[i]=9999;

	 m_chie2kpi[i]=9999;
	 m_chimu2kpi[i]=9999;
	 m_chipi2kpi[i]=9999;
	 m_chik2kpi[i]=9999;
	 m_chip2kpi[i]=9999;  
	 m_pidnum2kpi[i]=9999;
	 m_chiejs[i]=9999;
	 m_chimujs[i]=9999;
	 m_chipijs[i]=9999;
	 m_chikjs[i]=9999;
	 m_chipjs[i]=9999;  




      }

      
   }

   void Gam4pikp::BubbleSort(std::vector<double> &p, std::vector<int> &trkid)
   {
      if ( (int) p.size() != (int) trkid.size() )
      {
	 std::cout << "the two vector have different length!" << std::endl;
	 std::cout << "the size of p is " << (int) p.size() << std::endl;
	 std::cout << "the size of trkid is " << (int) trkid.size() << std::endl;
	 std::cout << "Please check your code!" << std::endl;
	 exit(1);
      }
      unsigned int vsize = p.size();
      double ptemp;
      int idtemp;
      for ( unsigned int i=0; i<vsize-1; i++ )
      {
	 for ( unsigned int j=i+1; j<vsize; j++ )
	 {
	    if ( p[i] > p[j] )
	    {
	       ptemp = p[i]; idtemp = trkid[i];
	       p[i] = p[j];  trkid[i] = trkid[j];
	       p[j] = ptemp; trkid[j] = idtemp;
	    }
	 } // for j
      } // for i

   }      

