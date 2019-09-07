#ifndef BhabhaPreSelect_H
#define BhabhaPreSelect_H 
#include <cstring>

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "EvtRecEvent/EvtRecTrack.h"


class BhabhaPreSelect : public Algorithm {

public:
  BhabhaPreSelect(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(); 

  bool WhetherSector(double,double=0.,double=CLHEP::twopi);

  int selectedType() const
    {
      return m_selectedType; 
    }
  int selectedTrkID1() const
    {
      return m_selectedTrkID1;
    }
  int selectedTrkID2() const
    {
      return m_selectedTrkID2;
    }

private:

  RecEmcShower*  matched( int TrkID ) const ;
  double Angle2ClosestShower( int ShowerID ); 
  // Declare r0, z0 cut for charged tracks
  double m_vr0cut;
  double m_vz0cut;

  double m_lowEnergyShowerCut;
  double m_highEnergyShowerCut;
  double m_matchThetaCut ;
  double m_matchPhiCut ;

  double m_highMomentumCut;
  double m_EoPMaxCut;
  double m_EoPMinCut;
  double m_minAngShEnergyCut;
  double m_minAngCut;
  double m_acolliCut;
  double m_eNormCut;
  double m_pNormCut;
  int m_BarrelOrEndcap;

  static int idmax[43];

  double m_oneProngMomentumCut;

  bool m_output;

  /**selected event type */
  int m_selectedType;
  int m_selectedTrkID1;
  int m_selectedTrkID2; 
  /**number of events passed */
  long int m_events;

  /**number of events rejected */
  long int m_rejected;

  int m_oneProngsSelected; 
  int m_twoProngsMatchedSelected;
  int m_twoProngsOneMatchedSelected;

 
  // define Ntuples here

  NTuple::Tuple*  m_tuple1;      //Trk2Sh2
  NTuple::Item<double>  m_trk1_p;
  NTuple::Item<double>  m_trk1_theta;
  NTuple::Item<double>  m_trk1_phi;
  NTuple::Item<double>  m_sh1_ene;
  NTuple::Item<double>  m_sh1_theta;
  NTuple::Item<double>  m_sh1_phi;

  NTuple::Item<double>  m_trk2_p;
  NTuple::Item<double>  m_trk2_theta;
  NTuple::Item<double>  m_trk2_phi;
  NTuple::Item<double>  m_sh2_ene;
  NTuple::Item<double>  m_sh2_theta;
  NTuple::Item<double>  m_sh2_phi;
  NTuple::Item<double>  m_di_the;
  NTuple::Item<double>  m_di_phi;

  NTuple::Item<double>  m_etot;
  NTuple::Item<long>  m_mdc_hit1;
  NTuple::Item<long>  m_mdc_hit2;
  NTuple::Item<double>  m_pNorm;
  NTuple::Item<double>  m_eNorm;
  NTuple::Item<double>  m_acolli;
  NTuple::Tuple*  m_tuple2;  
  NTuple::Item<double>  m_sh_ene;
  NTuple::Item<double>  m_sh_theta;
  NTuple::Item<double>  m_sh_phi;
};

#endif 
