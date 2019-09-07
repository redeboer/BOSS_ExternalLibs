// --------------------------------------------------
// 
// File:  CosmicGenerator/CosmicGenerator.h
// Description:
// Ported from Atlas to BES
//                      by Beijiang Liu 

//    The output will be stored in the transient event store so it can be
//    passed to the simulation.
// 
// AuthorList:
//         W. Seligman: Initial Code 08-Nov-2002,
//         based on work by M. Shapiro and I. Hinchliffe
//         adapted for the CosmicGun by S Bentvelsen


#ifndef GENERATORMODULESCOSMICGEN_H
#define GENERATORMODULESCOSMICGEN_H

#include "BesKernel/IBesRndmGenSvc.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/NTuple.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "HepMC/Polarization.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Normal3D.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;
using namespace CLHEP;
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double>       HepPoint3D;
typedef HepGeom::Normal3D<float>       HepNormal3D;
#endif

class StoreGateSvc;
class ActiveStoreSvc;

class CosmicGun;

class CosmicGenerator:public Algorithm {
public:
  CosmicGenerator(const std::string& name, ISvcLocator* pSvcLocator);
  ~CosmicGenerator();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();


  HepLorentzVector generateVertex(void);

  static IBesRndmGenSvc*         p_BesRndmGenSvc;

private:

  StoreGateSvc*  m_sgSvc;
  ActiveStoreSvc* m_activeStore;
  NTuple::Tuple* m_tuple;
  NTuple::Item<double> m_cosmicE;
  NTuple::Item<double>  m_cosmicTheta;
  NTuple::Item<double> m_cosmicPhi;
  NTuple::Item<double>  m_cosmicCharge;
  NTuple::Tuple* m_tuple1;
  NTuple::Item<double>  mc_phi;
  NTuple::Item<double> mc_theta;
  NTuple::Item<double>  mc_px;
  NTuple::Item<double> mc_py;
  NTuple::Item<double> mc_pz;

  // event counter, used for event ID
  long int m_events, m_rejected,m_accepted,m_tried;
  std::vector<int> m_pdgCode;
  float m_emin, m_emax;
  float m_ctcut;
  float m_xlow, m_xhig, m_zlow, m_zhig, m_yval, m_IPx, m_IPy, m_IPz;
  float m_radius, m_xpos, m_ypos, m_zpos;

  float m_tmin, m_tmax,m_tcor;
  bool m_sphereOpt;
 
  bool m_swapYZAxis;
  bool m_cubeProj;
  long int m_printEvent, m_printMod;
  long int m_selection ; 
  int m_dumpMC;
  float m_thetamin, m_thetamax, m_phimin, m_phimax;
  
  float m_GeV;
  float m_mm;


  bool m_readfile;
  std::string m_infile;
  std::ifstream    m_ffile;

  // Event scalars, three-vectors, and four-vectors:
  std::vector<HepLorentzVector> m_fourPos;
  std::vector<HepLorentzVector> m_fourMom;
  Hep3Vector m_center;
  std::vector<HepMC::Polarization> m_polarization;

  // Pointer to Athena MessageSvc.
  IMessageSvc* p_msgSvc;

  // Energy dependent position cut for muons to reach the detector.
  bool exzCut(const Hep3Vector& pos,const HepLorentzVector& p); 

  // property for calling exzCut
  bool m_exzCut ; 
  // maximum r used in exzCut 
  float m_rmax ; 

};

#endif
