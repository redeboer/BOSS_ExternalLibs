//**************************************************************************
// several value:
//    npar m_npar: the parameter numbers of result
//       nCounter: counter number of TOF, 176 for barrel, 96 for endcap
//          nKind: number of values, barrel_q0 1, barrel_sigma 4, endcap_q 1
// nBinPerCounter: number of bins divide one counter ( z(r) bins number)
//          total: nCounter*nKind*nBinPerCounter
//**************************************************************************
#include "tofcalgsec/TofCalibFit.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TStyle.h"
#include <iostream>
#include <ios>
#include <fstream>

TofCalibFit::TofCalibFit( bool isbarrel, const int npar ) {
  if( isbarrel ) { nCounter = NBarrel; }
  else { nCounter = NEndcap; }

  nKind          = 0;
  nBinPerCounter = 0;

  nHistPerCounter   = 0;
  nCanvasPerCounter = 0;
  nHistogram        = 0;
  nCanvas           = 0;

  m_npar = npar;
  X = HepVector(m_npar,0);
  m_name = string("calibfit");

  m_tcorrelation = HepVector( nBarrelCommon,0);
}


TofCalibFit::~TofCalibFit() {
  std::vector<TH1F*>::iterator iter1 = m_histograms.begin();
  for( ; iter1 != m_histograms.end(); iter1++ ) {
    delete (*iter1);
  }
  m_histograms.clear();
  std::vector<TH1F*>::iterator iter2 = m_graphs.begin();
  for( ; iter2 != m_graphs.end(); iter2++ ) {
    delete (*iter2);
  }
  m_graphs.clear();
  m_result.clear();
}


void TofCalibFit::fillTxt( const char* file ) {
  std::ofstream out(file,ios::out);
  if( out ) {
    std::vector<HepVector>::iterator it;
    for( it=m_result.begin(); it!=m_result.end(); it++ ) {
      for( int i=0; i<(*it).num_row(); i++ ) {
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
      for( int i=0; i<(*it).num_row(); i++ ) {
	cout << (*it)[i] << " ";
      }
      cout << std::endl;
    }
  }

  return;
}


void TofCalibFit::fillRoot( const char* file ) {

  unsigned int nhist = m_histograms.size();
  if( nhist != (nCounter*nHistPerCounter + nHistogram) ) {
    std::cout<<" tofcalgsec::TofCalibFit:" << m_name << ": the number of histograms is NOT same as the number of histograms saved!" << " nhist=" << nhist << " calculated=" << (nCounter*nHistPerCounter + nHistogram) << " nCounter=" << nCounter << " nHistPerCounter=" << nHistPerCounter << " nHistogram=" << nHistogram << std::endl;
    exit(0);
  }

  unsigned int numgraph1 = 0;
  unsigned int numgraph2 = 0;
  if( nCanvasPerCounter!=0 ) {
    std::vector<unsigned int>::iterator iter = nGraphPerCanvasPerCounter.begin();
    for( ; iter!=nGraphPerCanvasPerCounter.end(); iter++ ) {
      numgraph1 = numgraph1 + (*iter);
    }
  }
  if( nCanvas!=0 ) {
    std::vector<unsigned int>::iterator iter = nGraphPerCanvas.begin();
    for( ; iter!=nGraphPerCanvas.end(); iter++ ) {
      numgraph2 = numgraph2 + (*iter);
    }
  }
  unsigned int ngraph = m_graphs.size();
  if( ngraph != ( nCounter*numgraph1+numgraph2) ) {
    std::cout<<" tofcalgsec::TofCalibFit:"<< m_name << ": the number of graphs is NOT same as the number of graphs saved!" << "  ngraph=" << ngraph << "  nCounter=" << nCounter << "  numgraph1=" << numgraph1 << "  numgraph2=" << numgraph2 << std::endl;
    exit(0);
  }

  TFile f(file,"RECREATE");

  gStyle->SetOptStat(2211);
  gStyle->SetOptFit(1111);
  gStyle->SetLabelSize(0.03,"x");
  gStyle->SetLabelSize(0.03,"y");

  char dirname[256];
  char canvasname[256];
  if( nHistPerCounter>0 || nCanvasPerCounter>0 ) {
    std::vector<TH1F*>::iterator iter1 = m_histograms.begin();
    std::vector<TH1F*>::iterator iter2 = m_graphs.begin();
    for( unsigned int i=0; i<nCounter; i++ ) {
      sprintf( dirname, "tofid%i", i );
      TDirectory* cdresult = f.mkdir( dirname );
      cdresult->cd();
      
      for( unsigned int j=0; j<nHistPerCounter; j++ ) {
	(*(iter1+j))->Write();
      }
      iter1 = iter1 + nHistPerCounter;

      for( unsigned int j=0; j<nCanvasPerCounter; j++ ) {
	std::vector<string>::iterator it1 = CanvasPerCounterName.begin() + j;
	std::vector<unsigned int>::iterator it2 = nGraphPerCanvasPerCounter.begin() + j;
	sprintf( canvasname, "%s-tofid-%i", (*it1).c_str(), i );
	TCanvas* c1 = new TCanvas( canvasname, canvasname, 1);
	c1->SetFillColor(10);
	for( unsigned int k=0; k<(*it2); k++ ) {
	  if( k==0 ) {
	    (*(iter2+k))->Draw("E");
	  }
	  else {
	    (*(iter2+k))->Draw("Esame");
	  }
	}
	iter2 = iter2 + (*it2);
	c1->Write();
      }
    }
  }

  if( nHistogram>0 || nCanvas>0 ) {
    sprintf( dirname, "summary" );
    TDirectory* cdresult = f.mkdir( dirname );
    cdresult->cd();

    std::vector<TH1F*>::iterator iter1 = m_histograms.begin() + nCounter*nHistPerCounter;
    std::vector<TH1F*>::iterator iter2 = m_graphs.begin() + nCounter*numgraph1;
    for( ; iter1 != m_histograms.end(); iter1++ ) {
      (*iter1)->Write();
    }

    for( unsigned int j=0; j<nCanvas; j++ ) {
      std::vector<string>::iterator it1 = CanvasName.begin() + j;
      std::vector<unsigned int>::iterator it2 = nGraphPerCanvas.begin() + j;
      sprintf( canvasname, (*it1).c_str() );
      TCanvas* c1 = new TCanvas( canvasname, canvasname, 1);
      c1->SetFillColor(10);
      for( unsigned int k=0; k<(*it2); k++ ) {
	if( k==0 ) {
	  (*(iter2+k))->Draw("E");
	}
	else {
	  (*(iter2+k))->Draw("Esame");
	}
      }
      iter2 = iter2 + (*it2);
      c1->Write();
    }

  }

  f.Close();

  return;
}
