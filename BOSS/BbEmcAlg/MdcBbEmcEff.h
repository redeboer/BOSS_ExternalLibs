#ifndef MDCBBEMCEFF_H
#define MDCBBEMCEFF_H
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "MdcRecEvent/RecMdcTrack.h"

class MdcBbEmcEff:public Algorithm {
public:
  MdcBbEmcEff (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  int bookNTuple();
  int getEventInfo();
  int selectBbByEmcShower();
  int bbEmcMdcTrackingEff();

  int  m_evtIndex;
  bool m_hist;
  int  m_debug;

  //Emc shower Cut
  double m_emcDangCutLow;
  double m_emcDangCutHigh;
  double m_emcEneCutLow; 
  double m_emcEneCutHigh; 
  double m_emcEneCutTot; 
  
  //Mdc track Cut
  double m_dPhiCut;
  double m_dCosThetaCut;
  double m_d0Cut;
  double m_z0Cut;
  double m_momCutLow;
  double m_momCutHigh;
  double m_dPhiMdcEmcCut;

  //Barrel or endcap cut
  double m_barrelCut;
  double m_endcapCutLow;
  double m_endcapCutHigh;
  enum   TrackPosition{OUT=-1, BARREL=0, ENDCAP=1, GAP=2};
  TrackPosition m_posFlag;
  

  long	 t_evtNo;
  long   t_runNo;
  double t_t0;
  int    t_t0Stat;
  int    t_nTk;
  double t_emcPhi[2];

  int    m_effAllN1;
  int    m_effAllN2;
  int    m_effN1[3];
  int    m_effN2[3];

  NTuple::Tuple*  		   m_tuple1;
  NTuple::Item<long>               m_runNo;
  NTuple::Item<long>               m_evtNo;
  NTuple::Item<double>		   m_t0;
  NTuple::Item<long>		   m_t0Stat;

  NTuple::Item<long>  		   m_index;
  NTuple::Array<double>            m_emcEne;
  NTuple::Array<double>  	   m_emcTheta;
  NTuple::Array<double>            m_emcPhi;
  NTuple::Item<double>             m_emcDang;

  NTuple::Item<float>		   m_dCosTheta;
  NTuple::Item<float>		   m_dPhi;
  NTuple::Item<long>		   m_nTk;
  NTuple::Array<double>            m_d0;
  NTuple::Array<double>            m_z0;
  NTuple::Array<double>            m_cosTheta;
  NTuple::Array<double>            m_phi;
  NTuple::Array<double>            m_p;
  NTuple::Array<double>            m_pt;

};
#endif
