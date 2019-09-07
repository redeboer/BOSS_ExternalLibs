//
//
// File:  GeneratorModule/Histsample.h
// Description:
//    This code shows how to make histgrams from a HepMC event
//
// AuthorList:
//         Eva Bouhova Feb 2002

///#include <iostream.h>
#include <math.h>
#include "GaudiKernel/Algorithm.h"

#include "HepPDT/ParticleDataTable.hh"

class IHistogram1D;                    //Forward declaration
class IHistogram2D;                    //Forward declaration
class StoreGateSvc;
class HistSample:public Algorithm {
public:
  HistSample(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  bool        m_produceHistogram;

  //histograms, used if m_produceHistogram is true = 1
  IHistogram1D*   m_hgenerated;
  IHistogram1D*   m_hfinal;
  IHistogram1D*   m_ncharged;
  IHistogram1D*   m_hChargedPt;
  IHistogram1D*   m_hChargedEta;
  IHistogram1D*   m_hZPtall;
  IHistogram1D*   m_hZPt;
  IHistogram1D*   m_hZPte;
  IHistogram1D*   m_hZPtm;
  IHistogram1D*   m_hZPtt;
  IHistogram1D*   m_massZall;
  IHistogram1D*   m_massZ;
  IHistogram1D*   m_massZe;
  IHistogram1D*   m_massZm;
  IHistogram1D*   m_massZt;
  IHistogram1D*   m_hPtPaire;
  IHistogram1D*   m_hPtPairm;
  IHistogram1D*   m_hPtPairt;
  IHistogram1D*   m_massPaire;
  IHistogram1D*   m_massPairm;
  IHistogram1D*   m_massPairt;
  IHistogram1D*   m_rapidity;
  IHistogram1D*   m_pseudorapidity;
  IHistogram1D*   m_hpte;

  StoreGateSvc* m_sgSvc;

  HepPDT::ParticleDataTable* m_particleTable;

};


