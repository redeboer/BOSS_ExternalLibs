#include "tofcalgsec/TofCalibration.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TStyle.h"
#include <iostream>
#include <ios>
#include <fstream>

using namespace CLHEP;

TofCalibration::TofCalibration( const int npar ):Npar(npar) {
  X = HepVector(Npar,0);
  m_name = string("calibration");
}

TofCalibration::~TofCalibration() {
  std::vector<TH1F*>::iterator iter = histograms.begin();
  for( ; iter != histograms.end(); iter++ ) {
    delete (*iter);
  }
  histograms.clear();
  m_result.clear();
}


void TofCalibration::calculate( RecordSet*& data, unsigned int icounter ) {

  std::cout << setiosflags(ios::left) << setw(10) << icounter << setw(8) << data->size() << setw(30) << name() << std::endl;

  if( data->size() <= 0 ) {
    for( int i=0; i<X.num_row(); i++ ) {
      X[i] = 1.0;
    }
  }
  else {
    std::vector<Record*>::iterator iter = data->begin();
    for( ; iter!=data->end(); iter++ ) {
      calculate_record( (*iter), icounter );
    }

    calculate_result( icounter );
  }

  return;
}


void TofCalibration::fillTxt( const char* file ) {
  std::ofstream out(file,ios::out);
  if( out ) {
    std::vector<HepVector>::iterator it;
    for( it=m_result.begin(); it!=m_result.end(); it++ ) {
      //      for( unsigned int i=0; i<(*it).num_row(); i++ ) {
      for( unsigned int i=0; i<Npar2; i++ ) {
	out << (*it)[i] << " ";
      }
      out << std::endl;
    }
    out.close();
  }
  else{
    cerr << "error when open file " << file << " for write in " << name() << "::fillTxt()" << std::endl;
    cout << "print all parameters to srceen: in total " << m_result.size() << " items" << std::endl;
    std::vector<HepVector>::iterator it;
    for( it=m_result.begin(); it!=m_result.end(); it++ ) {
      //      for( unsigned int i=0; i<(*it).num_row(); i++ ) {
      for( unsigned int i=0; i<Npar2; i++ ) {
	cout << (*it)[i] << " ";
      }
      cout << std::endl;
    }
  }

  return;
}


void TofCalibration::fillRoot( const char* file, bool isbarrel ) {

  unsigned int ncounter = 1;
  if( isbarrel ) {
    ncounter = NBarrel;
  }
  else {
    ncounter = NEndcap;
  }

  unsigned int nhist = histograms.size();
  if( nhist != (ncounter*nHistPerCounter + nHistogram) ) {
    std::cout<<" tofcalgsec::TofCalibration: the number of histograms is NOT integral number of times of counter number!"<<std::endl;
    exit(0);
  }
  unsigned int ngraph = graphs.size();
  if( ngraph != (ncounter*nGraphPerCounter + nGraph) ) {
    std::cout<<" tofcalgsec::TofCalibration: the number of histograms is NOT integral number of times of counter number!"<<std::endl;
    exit(0);
  }

  TFile f(file,"RECREATE");

  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(1111);
  gStyle->SetLabelSize(0.03,"x");
  gStyle->SetLabelSize(0.03,"y");

  char dirname[256];
  if( nHistPerCounter>1 ) {
    std::vector<TH1F*>::iterator iter = histograms.begin();
    for( unsigned int i=0; i<ncounter; i++ ) {
      sprintf( dirname, "tofid%i", i );
      TDirectory* cdresult = f.mkdir( dirname );
      cdresult->cd();
      
      for( unsigned int i=0; i<nHistPerCounter; i++ ) {
	(*(iter+i))->Write();
      }
      iter = iter + nHistPerCounter;
    }
  }
  else {
    sprintf( dirname, "tofid" );
    TDirectory* cdresult = f.mkdir( dirname );
    cdresult->cd();
    
    std::vector<TH1F*>::iterator iter = histograms.begin();
    for( ; iter != histograms.end(); iter++ ) {
      (*iter)->Write();
    }
  }
  
  f.Close();

  return;
}

