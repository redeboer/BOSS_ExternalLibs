//************************************************************
//
//   Ekhara generator for BES3
//  
//  Author: Wang Boqun
//  Date:  2011/04/06, created
//
//***********************************************************

#ifndef Generator_Ekhara_H
#define Generator_Ekhara_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "Ekhara/EkharaRandom.h"
#include "AIDA/IHistogram1D.h"

class IBesRndmGenSvc;
class Ekhara: public Algorithm
{
 public:
  Ekhara(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  IBesRndmGenSvc* p_BesRndmGenSvc;
  int m_ngUpperBound;          // # of events to determine the maximum                  
  int m_initSeed;
  int m_channel_id;        // pi+pi-(1),pi0(2), eta(3), eta' (4)
  int m_sw_2pi;          // s (1); t (2); s+t (3); s = signal 
  int m_sw_1pi;          // s (1); t (2); s+t (3); s = signal
  int m_sw_silent;       //output file set up to silent
  int m_sw;
  int m_piggFFsw;          // WZWconst(1), rho pole (2); LMD (3); LMD+V (4); LMD+Vnew (5);
  double m_E, m_Es;        // CMS-energy                        
  double cut_th1min; //positron min angle cut
  double cut_th1max; //positron max angle cut
  double cut_E1min; //positron min energy cut
  double cut_E1max; //positron max energy cut
  double cut_th2min; //electron min angle cut
  double cut_th2max; //electron max angle cut
  double cut_E2min; //electron min energy cut
  double cut_E2max; //electron max energy cut
  double cut_Epionmin; //pion (eta) min energy cut
  double cut_Epionmax; //pion (eta) max energy cut
  double cut_thpionmin; //pion (eta) min angle cut
  double cut_thpionmax; //pion (eta) max angle cut


  IHistogram1D *hMCPosiMom;
  IHistogram1D *hMCPosiThe;
  IHistogram1D *hMCPosiPhi;
  IHistogram1D *hMCElecMom;
  IHistogram1D *hMCElecThe;
  IHistogram1D *hMCElecPhi;
  IHistogram1D *hMCEtaPMom;
  IHistogram1D *hMCEtaPThe;
  IHistogram1D *hMCEtaPPhi;

};

#endif
