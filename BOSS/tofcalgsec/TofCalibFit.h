#ifndef TOFCALIBFIT_H
#define TOFCALIBFIT_H

#include "tofcalgsec/TofDataSet.h"
#include "CLHEP/Matrix/Vector.h"
#include "TH1F.h"
#include "TCanvas.h"

using namespace std;
using namespace CLHEP;

const double zbegin = -115.0;
const double zend   =  115.0;
const double barrelRadius = 87.05;

const double rbegin = 53.5;
const double rend   = 80.5;

const int nBarrelCommon = 4;  // number of calib_barrel_common.txt

class TofCalibFit{
 public:
  TofCalibFit( bool isbarrel, const int npar );
  ~TofCalibFit();

 public:
  const string& name()          const { return m_name; }
  virtual void calculate( RecordSet*& data, unsigned int icounter ) = 0;

  void fillTxt( const char* file );
  void fillRoot( const char* file );

 public:
  HepVector tcorrelation()             { return X;            }
  void setTCorrelation( HepVector tc ) { m_tcorrelation = tc; }

 protected:
  int m_npar;

  unsigned int nKind;
  unsigned int nBinPerCounter;

  unsigned int nHistPerCounter;
  unsigned int nCanvasPerCounter;
  std::vector<unsigned int> nGraphPerCanvasPerCounter;
  unsigned int nHistogram;
  unsigned int nCanvas;
  std::vector<unsigned int> nGraphPerCanvas;

  string m_name;
  HepVector X;

  std::vector<TH1F*>     m_histograms;
  std::vector<TH1F*>     m_graphs;
  std::vector<HepVector> m_result;

  std::vector<string> CanvasPerCounterName;
  std::vector<string> CanvasName;

  HepVector m_tcorrelation;

 private:
  unsigned int nCounter;
};

#endif
