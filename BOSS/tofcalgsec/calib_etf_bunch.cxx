#include "tofcalgsec/calib_etf_bunch.h"
#include "TF1.h"

calib_etf_bunch::calib_etf_bunch( const unsigned int nbunch ):TofCalibFit( true, nParEtfBunch ) {

  nKind          = nbunch;
  nBinPerCounter = 3;

  nHistPerCounter   = 0;
  nCanvasPerCounter = 0;
  nHistogram      = 4*nBinPerCounter;
  nCanvas         = 8;
  CanvasName.push_back( static_cast<string>("Offset-bunch0") );
  CanvasName.push_back( static_cast<string>("Offset-bunch1") );  
  CanvasName.push_back( static_cast<string>("Offset-bunch2") );
  CanvasName.push_back( static_cast<string>("Offset-bunch3") );  
  CanvasName.push_back( static_cast<string>("Sigma-bunch0") );  
  CanvasName.push_back( static_cast<string>("Sigma-bunch1") );  
  CanvasName.push_back( static_cast<string>("Sigma-bunch2") );  
  CanvasName.push_back( static_cast<string>("Sigma-bunch3") );  

  nGraphPerCanvas.push_back(1);
  nGraphPerCanvas.push_back(1);
  nGraphPerCanvas.push_back(1);
  nGraphPerCanvas.push_back(1);
  nGraphPerCanvas.push_back(1);
  nGraphPerCanvas.push_back(1);
  nGraphPerCanvas.push_back(1);
  nGraphPerCanvas.push_back(1);

  int numGraphs = 0;
  std::vector<unsigned int>::iterator iter = nGraphPerCanvas.begin();
  for( ; iter!=nGraphPerCanvas.end(); iter++ ) {
    numGraphs = numGraphs + (*iter);
  }
  if( numGraphs != nGraphTotalBunch ) {
    cout << "tofcalgsec::calib_barrel_common: the number of Graphs is NOT reasonable!!!" << endl;
    exit(0);
  }

  m_name = string("calib_etf_bunch");

  const int tbin   = 160;
  const double tbegin = -0.8;
  const double tend   =  0.8;

  char hname[256];
  // histograms
  for( unsigned int j=0; j<4; j++ ) {
    for( unsigned int k=0; k<nBinPerCounter; k++ ) {
      if( k==0 )      { sprintf( hname, "tleft-bunch%i",  j); }
      else if( k==1 ) { sprintf( hname, "tright-bunch%i", j); }
      else if( k==2 ) { sprintf( hname, "tcombine-bunch%i", j); }
      m_fitresult.push_back( HepVector( nParEtfBunch,0 ) );
      m_histograms.push_back( new TH1F( hname, hname, tbin, tbegin, tend ) );
    }
  }
  m_fitresult.push_back( HepVector( nParEtfBunch,0 ) );

  modpos.resize( nBinPerCounter );
  modposerr.resize( nBinPerCounter );
  for( unsigned int i=0; i<nBinPerCounter; i++ ) {
    modpos[i] = 1.0*i;
    modposerr[i] = 0.5;
  }

  return;
}


calib_etf_bunch::~calib_etf_bunch() {
  m_fitresult.clear();
  modpos.clear();
  modposerr.clear();
}


void calib_etf_bunch::calculate( RecordSet*& data, unsigned int icounter ) {

  std::cout << setiosflags(ios::left) << setw(10) << icounter << setw(8) << data->size() << setw(30) << name() << std::endl;

  if( data->size() > 0 ) {
    std::vector<Record*>::iterator iter = data->begin();
    for( ; iter!=data->end(); iter++ ) {
      fillRecord( (*iter) );
    }
  }

  if( icounter==(NEtf*NStrip-1) ) {
    fitHistogram();
    fillGraph();
    fitGraph();
  }

  return;
}


void calib_etf_bunch::fillRecord( const Record* r ) {
  double t0 = r->phi();
  int    ibunch = static_cast<int>(t0/(12000./499.8/nKind)+0.1)%nKind;
  if( ibunch<0 || ibunch>int(nKind) ) return;

  std::vector<TH1F*>::iterator iter = m_histograms.begin();
  (*(iter+nBinPerCounter*ibunch+0))->Fill( r->tleft() );
  (*(iter+nBinPerCounter*ibunch+1))->Fill( r->tright() );
  (*(iter+nBinPerCounter*ibunch+2))->Fill( r->t0() );

  return;
}


void calib_etf_bunch::fitHistogram() {
  TF1* g = new TF1("g", "gaus");
  g->SetLineColor(2);
  g->SetLineWidth(1);

  std::vector<TH1F*>::iterator     iter1 = m_histograms.begin();
  std::vector<HepVector>::iterator iter2 = m_fitresult.begin();
  for( ; iter1!=m_histograms.end(); iter1++, iter2++ ) {
    (*iter1)->Fit( g, "Q");
    (*iter2)[0] = g->GetParameter(1);
    (*iter2)[1] = g->GetParError(1);
    (*iter2)[2] = g->GetParameter(2);
    (*iter2)[3] = g->GetParError(2);
  }

  return;
}


void calib_etf_bunch::fillGraph() {

  char gname1[256], gname2[256];
  TH1F *gra[nGraphTotalBunch];

  // 8canvas all counter,
  //   1. offset of tleft, tright and tcombine of bunch0  ---  gra[0]
  //   2. offset of tleft, tright and tcombine of bunch1  ---  gra[1]
  //   3. offset of tleft, tright and tcombine of bunch2  ---  gra[2]
  //   4. offset of tleft, tright and tcombine of bunch3  ---  gra[3]
  //   5. sigma  of tleft, tright and tcombine of bunch0  ---  gra[4]
  //   6. sigma  of tleft, tright and tcombine of bunch1  ---  gra[5]
  //   7. sigma  of tleft, tright and tcombine of bunch2  ---  gra[6]
  //   8. sigma  of tleft, tright and tcombine of bunch3  ---  gra[7]


  std::vector<double> toffset, toffseterr;
  std::vector<double> tsigma, tsigmaerr;
  toffset.resize( nBinPerCounter );
  toffseterr.resize( nBinPerCounter );
  tsigma.resize( nBinPerCounter );
  tsigmaerr.resize( nBinPerCounter );

  unsigned int number = 0;
  std::vector<HepVector>::iterator iter = m_fitresult.begin();
  for( unsigned int j=0; j<4; j++ ) {
    sprintf( gname1, "bunch%i-offset", j );
    sprintf( gname2, "bunch%i-sigma", j );

    gra[j] = new TH1F( gname1, gname1, nBinPerCounter, -0.5, 2.5 );
    gra[j+4] = new TH1F( gname2, gname2, nBinPerCounter, -0.5, 2.5 );

    for( unsigned int k=0; k<nBinPerCounter; k++ ) {
      number = j*nBinPerCounter + k;
      toffset[k]    = (*(iter+number))[0];
      toffseterr[k] = (*(iter+number))[1];
      tsigma[k]     = (*(iter+number))[2];
      tsigmaerr[k]  = (*(iter+number))[3];
      gra[j]->SetBinContent( k+1, toffset[k]    );
      gra[j]->SetBinError(   k+1, toffseterr[k] );
      gra[j+4]->SetBinContent( k+1, tsigma[k]    );
      gra[j+4]->SetBinError(   k+1, tsigmaerr[k] );
    }
  }

  for( int j=0; j<nGraphTotalBunch; j++, j++ ) {
    gra[j]->SetMarkerSize(1.5);
    gra[j]->SetMarkerStyle(20);
    gra[j]->SetMarkerColor(2);
    m_graphs.push_back( gra[j] );

    gra[j+1]->SetMarkerSize(1.5);
    gra[j+1]->SetMarkerStyle(21);
    gra[j+1]->SetMarkerColor(4);
    m_graphs.push_back( gra[j+1] );
  }

  return;
}


void calib_etf_bunch::fitGraph() {
  TF1* p0 = new TF1("p0", "pol0");
  p0->SetLineColor(1);
  p0->SetLineWidth(1);

  X = HepVector( 2, 0 );
  std::vector<TH1F*>::iterator iter=m_graphs.begin();
  for( int i=0; i<nParEtfBunch; i++ ) {
    (*(iter+i))->Fit( "p0", "Q" );
    X[0] = p0->GetParameter(0);
    X[1] = p0->GetParError(0);
    m_result.push_back( X );
  }

  return;
}
