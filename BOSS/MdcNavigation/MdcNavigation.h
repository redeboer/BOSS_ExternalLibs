#include "GaudiKernel/Algorithm.h"
#include <string>
#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
#include "HepPDT/ParticleDataTable.hh"
#include "GaudiKernel/NTuple.h"
#include "McTruth/McParticle.h"
#include "McTruth/MdcMcHit.h"
#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "AIDA/IHistogram1D.h"
#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include <math.h>

class MdcDetector;
class TH1F;
class TH2F;

class MdcNavigation:public Algorithm {
public:
  MdcNavigation (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode beginRun();

private: 
  //calculate momentum using helix parameters of fitted track
  Hep3Vector momentum(const RecMdcTrack* trk);
  StatusCode bookNTuple();
  StatusCode fillInit();
  StatusCode fillRecTrack(const RecMdcTrack* it, int mcTkNum, int recTkNum);
  StatusCode fillRecHits(RecMdcHitCol& hitCol);
  StatusCode fillEvent();
  StatusCode skipMcParticle(const Event::McParticle* it, int nKindSkiped, int* keepedPart);
  double poca(const MdcDigi* aDigi,const HepVector helixPar,const HepSymMatrix errMat);
  double probab(const int& ndof, const double& chisq);
  
  HepPDT::ParticleDataTable* m_particleTable;
  RawDataProviderSvc* m_rawDataProviderSvc; 
  IMagneticFieldSvc*   m_pIMF;
  
  bool   m_poca;
  bool   m_debug;
  const  MdcDetector * m_gm;
  int    havedigi[43][288]; 
  short  multiTdcCount[43][288];
  bool   m_hist;
  int    m_mc;
  bool   m_doSag;
  bool   m_rawData;
  int    m_maxMdcDigi;
  bool   m_keepBadTdc;
  bool   m_dropHot;
  bool   m_keepUnmatch;
  long	 t_eventNo;
  long   t_runNo;
  double t_t0;
  double t_t0Stat;
  double t_t0Truth;
  int 	 t_nTk;
  int    t_recTkNum;
  int    t_mcTkNum;
  int    t_trkRecoTk;
  int    t_curlTk;
  int    t_patRecTk ;
  int    t_xRecTk;
  int    m_nMcHit;
  int*   keepedParticles;
  double m_d0Cut;
  double m_z0Cut;
  double nNoise;
  long   nDigiTk[100];
  long   digiLayer[100];//mc digi per track; 50 track/event most

  NTuple::Tuple*  g_tupleMc;
  NTuple::Tuple*  g_tupleRec;
  NTuple::Tuple*  g_tupleEvt;

  AIDA::IHistogram1D*  g_layerEff;
  NTuple::Item<double>             m_na_p;
  NTuple::Item<double>             m_na_pt;
  NTuple::Item<double>             m_na_pz;
  NTuple::Item<double>             m_na_d0;
  NTuple::Item<double>             m_na_phi0;
  NTuple::Item<double>             m_na_cpa;
  NTuple::Item<double>             m_na_z0;
  NTuple::Item<double>             m_na_tanl;
  NTuple::Item<double>             m_na_q;
  NTuple::Item<double>             m_na_d0E;
  NTuple::Item<double>             m_na_phi0E;
  NTuple::Item<double>             m_na_cpaE;
  NTuple::Item<double>             m_na_z0E;
  NTuple::Item<double>             m_na_tanlE;

  NTuple::Item<double>             m_na_dP; 
  NTuple::Item<double>             m_na_dPt;
  NTuple::Item<double>             m_na_dPz;
  NTuple::Item<double>             m_na_dD0;
  NTuple::Item<double>             m_na_dPhi0;
  NTuple::Item<double>             m_na_dCpa;
  NTuple::Item<double>             m_na_dZ0;
  NTuple::Item<double>             m_na_dTanl;

  NTuple::Item<double>             m_na_d0Res;
  NTuple::Item<double>             m_na_phi0Res;
  NTuple::Item<double>             m_na_cpaRes;
  NTuple::Item<double>             m_na_z0Res;
  NTuple::Item<double>             m_na_tanlRes;

  NTuple::Item<long>               m_na_nHit;
  NTuple::Item<long>               m_na_nDigi;
  NTuple::Item<double>             m_na_nEvtNoise;
  NTuple::Item<double>             m_na_nAct;
  NTuple::Item<double>             m_na_nNoise;
  NTuple::Item<double>             m_na_nMatch;
  NTuple::Item<double>             m_na_nSt;
  NTuple::Item<double>             m_na_nDof;
  NTuple::Item<double>             m_na_fiTerm;
  NTuple::Item<double>             m_na_chi2;
  NTuple::Item<double>             m_na_chi2Dof;
  NTuple::Item<double>             m_na_chi2Prob;
  NTuple::Item<double>             m_na_mcTkNum;
  NTuple::Item<double>             m_na_mcTkId;
  NTuple::Item<double>             m_na_recTkNum;

  NTuple::Item<double>             m_na_mcpid;
  NTuple::Item<double>             m_na_mcp;
  NTuple::Item<double>             m_na_mcpt;
  NTuple::Item<double>             m_na_mcpx;
  NTuple::Item<double>             m_na_mcpy;
  NTuple::Item<double>             m_na_mcpz;
  NTuple::Item<double>             m_na_mcd0;
  NTuple::Item<double>             m_na_mcphi0;
  NTuple::Item<double>             m_na_mcz0;
  NTuple::Item<double>             m_na_mctheta;
  NTuple::Item<double>             m_na_mccpa;
  NTuple::Item<double>             m_na_mcq;
  NTuple::Item<double>             m_na_mcfifin;
  NTuple::Item<double>             m_na_mcflag;
  NTuple::Item<long>               m_na_tkStat;


  NTuple::Item<long>               m_na_tkId;
  NTuple::Item<double>             m_na_evtNo;
  NTuple::Array<double>            m_na_resid;
  NTuple::Array<double>            m_na_driftD;
  NTuple::Array<long>              m_na_act;
  NTuple::Array<double>            m_na_driftT;
  NTuple::Array<double>            m_na_doca;
  NTuple::Array<double>            m_na_entra;
  NTuple::Array<double>            m_na_zhit;
  NTuple::Array<double>            m_na_chi2add;
  NTuple::Array<double>            m_na_flaglr;
  NTuple::Array<double>            m_na_hitStat;
  NTuple::Array<double>            m_na_Tdc;
  NTuple::Array<double>            m_na_Adc;
  NTuple::Array<double>            m_na_layer;
  NTuple::Array<double>            m_na_wire;
  NTuple::Array<double>            m_na_gwire;
  NTuple::Array<double>            m_na_hitTkId;
  NTuple::Array<double>            m_na_digiTkId;
  NTuple::Array<double>            m_na_digiLayer;

  NTuple::Item<double>             m_na_t2mcp;
  NTuple::Item<double>             m_na_t2mcpt;
  NTuple::Item<double>             m_na_t2mcphi;
  NTuple::Item<double>             m_na_t2mctheta;
  NTuple::Item<double>             m_na_t2tanl;
  NTuple::Item<double>             m_na_t2recTkNum;
  NTuple::Item<double>             m_na_t2mcTkNum;
  NTuple::Item<double>             m_na_t2p;
  NTuple::Item<double>             m_na_t2pt;

  NTuple::Item<double>             m_na_t3recTk;
  NTuple::Item<double>             m_na_t3mcTk;
  NTuple::Item<long>		   m_na_t3TrkReco;
  NTuple::Item<long>		   m_na_t3Curl;
  NTuple::Item<long>		   m_na_t3PatRec;
  NTuple::Item<long>		   m_na_t3XRec;
  NTuple::Item<double>             m_na_t3evtNo;
  NTuple::Item<double>             m_na_t3t0; 
  NTuple::Item<double>             m_na_t3t0Truth; 
  NTuple::Item<double>             m_na_t3t0Stat; 
  NTuple::Item<double>             m_na_t3timing; 

  NTuple::Item<long>               m_na_t3runNo;
  NTuple::Item<long>               m_na_t3nDigi;
  NTuple::Array<long>              m_na_t3layer;
  NTuple::Array<long>              m_na_t3wire;
  NTuple::Array<long>              m_na_t3gwire;
  NTuple::Array<double>            m_na_t3rt;
  NTuple::Array<double>            m_na_t3rtNot0;
  NTuple::Array<double>            m_na_t3rc;
  NTuple::Array<double>            m_na_t3phi;
  NTuple::Array<double>            m_na_t3xMid;
  NTuple::Array<double>            m_na_t3yMid;
  NTuple::Array<long>              m_na_t3ovfl;
  NTuple::Array<long>              m_na_t3tNum;
};

