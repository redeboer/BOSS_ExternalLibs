// Package:     ntupleItem
// Module:      ntupleItem
// 
// Description: class for ntuple items
//

#ifndef ntupleItem_FLAG_
#define ntupleItem_FLAG_

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"


#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"
#include "AIDA/IHistogramFactory.h"

//MCTruth

NTuple::Item<long>   g_ntrkMC, g_eventNo;
NTuple::Array<float> g_theta0MC, g_phi0MC;
NTuple::Array<float> g_pxMC, g_pyMC, g_pzMC, g_ptMC;

//recon

NTuple::Item<long>   g_ntrk;
NTuple::Item<float>  g_eventtime;
NTuple::Array<float> g_px, g_py, g_pz, g_pt, g_p;
NTuple::Array<float> g_phi, g_theta;
NTuple::Array<float> g_vx, g_vy, g_vz;
NTuple::Array<float> g_dr, g_phi0, g_kappa, g_dz, g_tanl;
NTuple::Item<float>  g_estime;

IHistogram1D*   g_sigmaxy;
IHistogram1D*   g_sigmaz;
IHistogram1D*   g_chi2xy;
IHistogram1D*   g_chi2sz;
IHistogram1D*   g_ncellMC;
IHistogram1D*   g_ncell;
IHistogram1D*   g_naxialhit;
IHistogram1D*   g_nstereohit;
IHistogram1D*   g_nhit;
IHistogram2D*   g_hitmap[20];





#endif /* ntupleItem_FLAG_ */

