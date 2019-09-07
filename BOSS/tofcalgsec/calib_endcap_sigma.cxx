#include "tofcalgsec/calib_endcap_sigma.h"
#include "TF1.h"

calib_endcap_sigma::calib_endcap_sigma( const unsigned int nrbin ):TofCalibFit( false, nEndcapSigma ) {

  nKind          = 1;    // 0:tleft
  nBinPerCounter = nrbin;

  nHistPerCounter   = nKind*nBinPerCounter;
  nCanvasPerCounter = 2;
  CanvasPerCounterName.push_back( static_cast<string>("Endcap-offset") );  
  CanvasPerCounterName.push_back( static_cast<string>("Endcap-sigma") );  

  nGraphPerCanvasPerCounter.push_back(1);
  nGraphPerCanvasPerCounter.push_back(1);

  nHistogram      = 0;
  nCanvas         = 0;

  int numGraphs = 0;
  std::vector<unsigned int>::iterator iter = nGraphPerCanvasPerCounter.begin();
  for( ; iter!=nGraphPerCanvasPerCounter.end(); iter++ ) {
    numGraphs = numGraphs + (*iter);
  }
  if( numGraphs != nGraphEcSigma ) {
    cout << "tofcalgsec::calib_endcap_sigma: the number of Graphs is NOT reasonable!!!" << endl;
    exit(0);
  }

  m_name = string("calib_endcap_sigma");

  const int tbin   = 150;
  const double tbegin = -1.5;
  const double tend   =  1.5;

  // histograms per counter
  char hname[256];
  for( unsigned int i=0; i<NEndcap; i++ ) {
    m_result.push_back( HepVector(nEndcapSigma,0) );
    for( unsigned int j=0; j<nKind; j++ ) {
      for( unsigned int k=0; k<nBinPerCounter; k++ ) {
	sprintf( hname, "tleft-tofid%i-r%i",  i, k);
	m_histograms.push_back( new TH1F( hname, hname, tbin, tbegin, tend ) );

	m_fitresult.push_back( HepVector(nParEcSigma,0) );
      }
    }
  }

  rpos.resize( nBinPerCounter );
  rposerr.resize( nBinPerCounter );
  rstep = ( rend - rbegin )/nBinPerCounter;
  for( unsigned int i=0; i<nBinPerCounter; i++ ) {
    rpos[i] = rbegin + ( i+0.5 )*rstep;
    rposerr[i] = 0.5*rstep;
  }

}


calib_endcap_sigma::~calib_endcap_sigma() {
  m_fitresult.clear();
  rpos.clear();
  rposerr.clear();
}


void calib_endcap_sigma::calculate( RecordSet*& data, unsigned int icounter ) {

  std::cout << setiosflags(ios::left) << setw(10) << icounter << setw(8) << data->size() << setw(30) << name() << std::endl;

  if( data->size() > 0 ) {
    std::vector<Record*>::iterator iter = data->begin();
    for( ; iter!=data->end(); iter++ ) {
      fillRecord( (*iter), icounter );
    }
  }
  fitHistogram( icounter );
  fillGraph( icounter );
  fitGraph( icounter );

  return;
}


void calib_endcap_sigma::fillRecord( const Record* r, unsigned int icounter ) {

  double rhit = r->zrhit();
  if( rhit<rbegin || rhit>rend ) return;
  int rbin = static_cast<int>((rhit-rbegin)/rstep);
  if( rbin<0 ) { rbin = 0; }
  else if( rbin>static_cast<int>(nBinPerCounter-1) ) {
    cout << "tofcalgsec::calib_endcap_sigma:fillRecord: rhit is out of range, rhit=" << rhit << " rbin=" << rbin << endl;
    return;
  }

  std::vector<TH1F*>::iterator iter = m_histograms.begin() + icounter*nKind*nBinPerCounter + rbin;
  (*iter)->Fill( r->tleft() );

  return;
}


void calib_endcap_sigma::fitHistogram( unsigned int icounter ) {
  TF1* g = new TF1("g", "gaus");
  g->SetLineColor(2);
  g->SetLineWidth(1);

  std::vector<TH1F*>::iterator     iter1 = m_histograms.begin() + icounter*nKind*nBinPerCounter;
  std::vector<HepVector>::iterator iter2 = m_fitresult.begin() + icounter*nKind*nBinPerCounter;
  for( unsigned int j=0; j<nBinPerCounter; j++, iter1++, iter2++ ) {
    (*iter1)->Fit( g, "Q");
    (*iter2)[0] = g->GetParameter(1);
    (*iter2)[1] = g->GetParError(1);
    (*iter2)[2] = g->GetParameter(2);
    (*iter2)[3] = g->GetParError(2);
  }

  return;

}


void calib_endcap_sigma::fillGraph( unsigned int icounter ) {

  char gname1[256], gname2[256];

  // fill graphs per counter
  // 4 canvas per counter,
  //   1. offset of tleft vs z 
  //   2. sigma  of tleft vs z
  std::vector<double> toffset, toffseterr;
  std::vector<double> tsigma, tsigmaerr;
  toffset.resize( nBinPerCounter );
  toffseterr.resize( nBinPerCounter );
  tsigma.resize( nBinPerCounter );
  tsigmaerr.resize( nBinPerCounter );

  std::vector<HepVector>::iterator iter = m_fitresult.begin() + icounter*nKind*nBinPerCounter;
  for( unsigned int k=0; k<nBinPerCounter; k++ ) {
    toffset[k]    = (*(iter+k))[0];
    toffseterr[k] = (*(iter+k))[1];
    tsigma[k]     = (*(iter+k))[2];
    tsigmaerr[k]  = (*(iter+k))[3];
  }

  sprintf( gname1, "endcap-offset-tofid-%i", icounter );
  m_graphs.push_back( new TH1F( gname1, gname1, nBinPerCounter, rbegin, rend ) );
  std::vector<TH1F*>::iterator itgraph = m_graphs.end() - 1;
  (*itgraph)->SetMarkerSize(1.5);
  (*itgraph)->SetMarkerStyle(20);
  (*itgraph)->SetMarkerColor(2);
  for( unsigned int k=0; k<nBinPerCounter; k++ ) {
    (*itgraph)->SetBinContent( k+1, toffset[k]    );
    (*itgraph)->SetBinError(   k+1, toffseterr[k] );
  }

  sprintf( gname2, "endcap-sigma-tofid-%i", icounter );
  m_graphs.push_back( new TH1F( gname2, gname2, nBinPerCounter, rbegin, rend ) );
  itgraph = m_graphs.end() - 1;
  (*itgraph)->SetMarkerSize(1.5);
  (*itgraph)->SetMarkerStyle(21);
  (*itgraph)->SetMarkerColor(4);
  for( unsigned int k=0; k<nBinPerCounter; k++ ) {
    (*itgraph)->SetBinContent( k+1, tsigma[k]    );
    (*itgraph)->SetBinError(   k+1, tsigmaerr[k] );
  }

  return;
}


void calib_endcap_sigma::fitGraph( unsigned int icounter ) {

  TF1* p2 = new TF1("p2", "pol2", rbegin, rend );

  //  std::vector<unsigned int>::iterator itnumber = nGraphPerCanvasPerCounter.begin();
  std::vector<TH1F*>::iterator itgraph = m_graphs.begin() + icounter*nGraphEcSigma + 1;

  (*itgraph)->Fit( "p2", "Q" );
  X = HepVector( m_npar, 0 );
  X[0] = p2->GetParameter(0);
  X[1] = p2->GetParameter(1);
  X[2] = p2->GetParameter(2);

  std::vector<HepVector>::iterator iter = m_result.begin() + icounter;
  (*iter) = X;

  return;
}
