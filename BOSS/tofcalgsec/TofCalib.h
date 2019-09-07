#ifndef TOFCALIB_H
#define TOFCALIB_H

#include "tofcalgsec/TofDataSet.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

using namespace std;
using namespace CLHEP;

class TofCalib{
 public:
  TofCalib( const int npar ):Npar(npar) {
    F = HepMatrix(Npar,Npar,0);
    Y = HepVector(Npar,0);
    X = HepVector(Npar,0);
    funcs = HepVector(Npar,0);
    m_name=string("calculation");
  }
  ~TofCalib() {}

  void reset();

 public:
  virtual void calculate( RecordSet*& data, unsigned int icounter );
  virtual void updateData( RecordSet*& data ) = 0;
  virtual const string& name()        const { return m_name;}

  void fillTxt( const char* file );

 protected:
  virtual void calculate_funcs(const Record* r) = 0;
  virtual void calculate_y(const Record* r) = 0;

 protected:
  string m_name;
  int Npar;
  HepMatrix F;
  HepVector X;
  HepVector Y;
  HepVector funcs;
  double y;

  std::vector<HepVector> m_result;
};

#endif

