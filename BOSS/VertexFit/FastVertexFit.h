#ifndef VertexFit_FastVertexFit_H
#define VertexFit_FastVertexFit_H

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
using CLHEP::HepVector;
using CLHEP::Hep3Vector;
using CLHEP::HepMatrix;
using CLHEP::HepSymMatrix;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include <vector>
#include "VertexFit/VertexParameter.h"

class FastVertexFit {

 public:

  //constructor & deconstructor
  static FastVertexFit * instance();
  ~FastVertexFit();

  // initialization, must be called before fit
  void init();

  //void initVertex(); //FIXME need?

  // add track information
  void addTrack(const int number, const HepVector helix, const HepSymMatrix err);

  // fit function
  bool Fit();

  // Output 
  double chisq() const {return m_chisq;}
  VertexParameter vtx() const {return m_vtx_infit;}
  HepVector Vx() const {return m_Vx;}
  HepSymMatrix Evx() const {return m_Evx;}   
  HepVector Pull() const;

 private:
  void updateMatrices(const HepVector helix, const HepSymMatrix err);
  //std::vector<HepVector> m_helix;
  //std::vector<HepSymMatrix> m_err;
  std::vector<HepMatrix> m_D;
  std::vector<HepSymMatrix> m_W;
  std::vector<HepSymMatrix> m_DTWD;
  std::vector<HepVector> m_xp;
  
 private:
  double m_chisq; // chisq in fitting
  VertexParameter m_vtx_infit; //updated vertex parameters
  HepVector m_Vx; //updated vertex position
  HepSymMatrix m_Evx;  // coviance matrix of updated vertex 

 private:
  FastVertexFit(); // constructor
  static FastVertexFit *m_pointer; // pointer 
};

#endif
