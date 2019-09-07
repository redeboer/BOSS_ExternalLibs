#ifndef Validation_ValidRecTofTrack_ValidRecTofTrackAlg_H
#define Validation_ValidRecTofTrack_ValidRecTofTrackAlg_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"

// Specify the namespace
// using AIDA::IHistogram1D;


class ValidRecTofTrackAlg : public  Algorithm {

public:

  ValidRecTofTrackAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

   NTuple::Tuple*  m_tuple_tof;
   NTuple::Item<double> m_run;
   NTuple::Item<double> m_event;
   NTuple::Item<double> m_tofTrackID;
   NTuple::Item<double> m_trackID;
   NTuple::Item<double> m_tofID;
   NTuple::Item<double> m_status;
   NTuple::Item<double> m_raw;
   NTuple::Item<double> m_readout;
   NTuple::Item<double> m_counter;
   NTuple::Item<double> m_cluster;
   NTuple::Item<double> m_barrel;
   NTuple::Item<double> m_east;
   NTuple::Item<double> m_layer;
   NTuple::Item<double> m_ncounter;
   NTuple::Item<double> m_neast;
   NTuple::Item<double> m_nwest;
   NTuple::Item<double> m_path;
   NTuple::Item<double> m_zrhit;
   NTuple::Item<double> m_ph;
   NTuple::Item<double> m_tof;
   NTuple::Item<double> m_errtof;
   NTuple::Item<double> m_beta;
   NTuple::Item<double> m_texpe;
   NTuple::Item<double> m_texpmu;
   NTuple::Item<double> m_texppi;
   NTuple::Item<double> m_texpk;
   NTuple::Item<double> m_texpp;
   NTuple::Item<double> m_toffe;
   NTuple::Item<double> m_toffmu;
   NTuple::Item<double> m_toffpi;
   NTuple::Item<double> m_toffk;
   NTuple::Item<double> m_toffp;
   NTuple::Item<double> m_toffpb;
   NTuple::Item<double> m_sigmae;
   NTuple::Item<double> m_sigmamu;
   NTuple::Item<double> m_sigmapi;
   NTuple::Item<double> m_sigmak;
   NTuple::Item<double> m_sigmap;
   NTuple::Item<double> m_sigmapb;
   NTuple::Item<double> m_quality;
   NTuple::Item<double> m_t0;
   NTuple::Item<double> m_errt0;
   NTuple::Item<double> m_errz;
   NTuple::Item<double> m_phi;
   NTuple::Item<double> m_errphi;
   NTuple::Item<double> m_energy;
   NTuple::Item<double> m_errenergy;
};

#endif

