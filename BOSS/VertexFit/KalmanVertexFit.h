#ifndef Kalman_Vertex_Fit_H
#define Kalman_Vertex_Fit_H

#include <vector>
#include "VertexFit/HTrackParameter.h"
#include "VertexFit/VertexParameter.h"
#include "VertexFit/WTrackParameter.h"

using namespace std;
//
// Kalman vertex fit interface 
//
// Apr. 13, 2007, K.L. He created
//

class KalmanVertexFit {

 public:
  
  static KalmanVertexFit *instance();
  ~KalmanVertexFit();
  
  //
  //  initialization
  void init();
  //
  // initial vertex parameter
  //
  void initVertex(const VertexParameter vtx);
  //
  //  track interface
  //
  void addTrack(const HTrackParameter);
  //
  //  an interface for magnetic field
  //

  /* will in added in the future */
     
  //
  //  the method of filter
  //
  int filter(const int k);  // iteration for track k;
  void filter();
  //
  //  the method of inverse filter
  //
  void inverse(const int k);  // inverse kalman filter
  //
  //  the method of smooth
  //
  void smooth(const int k);
  void smooth();
  //
  // remove track k
  //
  void remove(const int k);  // remove track k from vertex, done by user

  // xum
  HepVector pull(const int k);
  double pullmomentum(const int k);
  //
  //  output of Kalman vertex fit
  //

  // updated vertex information
  VertexParameter vtx() const; 
  HepVector x() const {return m_x;}
  HepSymMatrix Ex() const;

  //
  // number of track, chi-square, etc
  //
  int numTrack() const;
  int ndof() const {return m_ndof;}
  double chisq() {return m_chisq;}
  int trackID(const int k) const {return m_hTrkOrigin[k].trackID();}
  std::vector<int> trackIDList() const; // ID of tracks constribute to vertex fitting 

  //
  // chi-square of filter and smooth 
  //
  double chiF(const int k) const {return m_chiF[k];}  // chi-square of filter for track k
  double chiS(const int k) const;  // chi-square of smooth for track k
  
  //
  // updated helix parameter after fit
  //
  HTrackParameter hTrack(const int k) const;  // at cloest point approach to origin
  WTrackParameter wTrack(const int k, const double mass) const;  // at the vertex position

  //
  //  set the number of iteration, chi-square cut for tracks
  //
  void setVertexIteration(const int num) {m_maxVertexIteration = num;}
  void setTrackIteration(const int num) {m_maxTrackIteration = num;}
  void setChisqCut(const double chicut) {m_chisqCutforTrack = chicut;}
  void setTrackIterationCut(const double chicut) {m_chisqCutforTrackIteration = chicut;} //xum
  //
  //  set input vector<HTrackParameter>, wait for some days
  //
  
 private:
  void updateMatrices(const int k);  // update derivative matrices during filter  
  void updateMatrices(const int k, const HepVector p, const HepVector x);  // update derivative matrices during filter  
  KalmanVertexFit(); // constructor
  static KalmanVertexFit *m_pointer;  // pointer 

 private:
  HepVector m_x;    // vertex position
  HepSymMatrix m_C0;   // Initial inverse covariance matrix of vertex
  HepSymMatrix m_C;    // current inverse covariance matrix of vertex
  int m_ndof;       // number of degree of freedom = 2 * nTrk -3
  double m_chisq;   // total chisq of vertex fit
 private:
  std::vector<int> m_flag;   // remove flag for track k;
  std::vector<HepVector> m_p; // 3-momentum at vertex position for track k
  std::vector<HTrackParameter> m_hTrkOrigin;  // measured helix parameter for track k
  std::vector<HTrackParameter> m_hTrkInfit;  // xum add
 private:
  std::vector<HepSymMatrix> m_G;             // 5x5 matrix G=V^{-1}, inversion of error matrix for track k, do not change during fitting
  std::vector<HepMatrix> m_A;                // 5x3 derivative matrix da/dx for track k
  std::vector<HepMatrix> m_B;                // 5x3 derivative matrix da/dp for track k
  std::vector<HepVector> m_c;                // c0 vector for track k
  std::vector<HepSymMatrix> m_W;             // 3x3 matrix (BT * G * B)^{-1}
  std::vector<HepSymMatrix> m_GB;            // 5x5 matrix 
  std::vector<double> m_chiF;                // chisq of filter
 
 private: //xum
  double calculationP(const double kappa, const double lamb); 
  double calculationSigmaP(const double kappa, const double lamb, const double Vkappa,
                   const double Vlamb, const double Vkappa_lamb); 

 private:
  double m_chisqCutforTrack;  // chisq cut
  int m_maxVertexIteration;  // number of iteration, 3 times are enough
  int m_maxTrackIteration;
  double m_chisqCutforTrackIteration;
};
#endif
