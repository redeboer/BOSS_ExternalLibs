#ifndef TOFCALIBRATION_H
#define TOFCALIBRATION_H

#include "tofcalgsec/TofDataSet.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "TH1F.h"
#include "TGraph.h"

using namespace std;
using namespace CLHEP;

class TofCalibration{
 public:
  TofCalibration( const int npar );
  ~TofCalibration();

 public:
  virtual const string& name()        const { return m_name; }

  virtual void calculate( RecordSet*& data, unsigned int icounter );
  //  virtual void updateData( RecordSet*& data ) = 0;

  void fillTxt( const char* file );
  void fillRoot( const char* file, bool isbarrel );

 protected:
  virtual void calculate_record(const Record* r, unsigned int icounter) = 0;
  virtual void calculate_result( unsigned int icounter ) = 0;

 protected:
  string m_name;
  int Npar;
  unsigned int Npar2;
  unsigned int nHistPerCounter;
  unsigned int nGraphPerCounter;
  unsigned int nHistogram;
  unsigned int nGraph;

  HepVector X;

  std::vector<TH1F*>     histograms;
  std::vector<TGraph*>   graphs;
  std::vector<HepVector> m_result;

};

#endif
