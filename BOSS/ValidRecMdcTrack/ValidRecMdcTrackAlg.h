#ifndef Validation_ValidRecMdcTrack_ValidRecMdcTrackAlg_H
#define Validation_ValidRecMdcTrack_ValidRecMdcTrackAlg_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "AIDA/IHistogram1D.h" 
#include "AIDA/IHistogramFactory.h" 
#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"

// Specify the namespace
//using AIDA::IHistogram1D;


class ValidRecMdcTrackAlg  : public  Algorithm {

public:

  ValidRecMdcTrackAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

   NTuple::Tuple*  m_tuple1;
   NTuple::Item<long>   g_hits;
   NTuple::Item<float>  g_eventNo;
   NTuple::Item<float>  g_runNo;


   NTuple::Item<float>  g_track_id;
   NTuple::Item<float>  g_phi0;
   NTuple::Item<float>  g_kappa;
   NTuple::Item<float>  g_tanl;
   NTuple::Item<float>  g_p;
   NTuple::Item<float>  g_dr;
   NTuple::Item<float>  g_dz;
   NTuple::Item<float>  g_chisq_of_fit;
   NTuple::Item<float>  g_phi_terminal;
   NTuple::Item<float>  g_number_of_hits;
   NTuple::Item<float>  g_number_of_stereo_hits;
   

   NTuple::Item<double> g_px;
   NTuple::Item<double> g_py;
   NTuple::Item<double> g_pxy;
   NTuple::Item<double> g_pz;
   NTuple::Item<float>    g_charge;
   NTuple::Item<float>    g_stat;
   NTuple::Item<double> g_x;
   NTuple::Item<double> g_y;
   NTuple::Item<double> g_z;
   NTuple::Item<double> g_r;
   NTuple::Item<double> g_chi2;
   NTuple::Item<float>    g_ndof;
   NTuple::Item<float>    g_firstLayer;
   NTuple::Item<float>    g_lastLayer;
   NTuple::Item<double> g_theta;
   NTuple::Item<double> g_phi;
   
/*
   NTuple::Item<float>  g_hits_id;
   NTuple::Item<float>  g_hits_ddl;
   NTuple::Item<float>  g_hits_ddr;

   NTuple::Item<float>  g_doca;

   NTuple::Item<float>  g_hits_mdc_id;
   NTuple::Item<float>  g_layer;
   NTuple::Item<float>  g_wire;
   NTuple::Item<float>  g_hits_tdc;
   NTuple::Item<float>  g_hits_adc;
*/
   NTuple::Tuple*  m_tuple2;
   NTuple::Item<float>  g_hits_id;
   NTuple::Item<float>  g_hits_ddl;
   NTuple::Item<float>  g_hits_ddr;

   NTuple::Item<float>  g_doca;

   NTuple::Item<float>  g_hits_mdc_id;
   NTuple::Item<float>  g_layer;
   NTuple::Item<float>  g_wire;
   NTuple::Item<float>  g_hits_tdc;
   NTuple::Item<float>  g_hits_adc;


   
};


#endif

