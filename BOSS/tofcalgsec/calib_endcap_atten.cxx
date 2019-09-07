#include "tofcalgsec/calib_endcap_atten.h"
#include "TF1.h"

//Q fit function
static double endcapQFunc(double *x, double *par) {
  double xx = x[0];
  double f = par[0] + par[1]*(xx-44.5) + par[2]*(xx-44.5)*(xx-44.5);

  return f;
}


calib_endcap_atten::calib_endcap_atten( const unsigned int nrbin ):TofCalibFit( false, nEndcapAtten ) {

  nKind          = 1;    // pulse height
  nBinPerCounter = nrbin + 1;

  nHistPerCounter   = nKind*nBinPerCounter;
  nCanvasPerCounter = 2;
  CanvasPerCounterName.push_back( static_cast<string>("Pulse Height Most Probable Value") );  
  CanvasPerCounterName.push_back( static_cast<string>("Pulse Height Sigma") );  
  nGraphPerCanvasPerCounter.push_back(1);
  nGraphPerCanvasPerCounter.push_back(1);

  nHistogram      = 0;
  nCanvas         = 2;
  CanvasName.push_back( static_cast<string>("Pulse Height Most Probable Value vs TOF Counter Number") );  
  CanvasName.push_back( static_cast<string>("Pulse Height Sigma vs TOF Counter Number") );  
  nGraphPerCanvas.push_back(1);
  nGraphPerCanvas.push_back(1);

  int numGraphs1 = 0;
  std::vector<unsigned int>::iterator iter = nGraphPerCanvasPerCounter.begin();
  for( ; iter!=nGraphPerCanvasPerCounter.end(); iter++ ) {
    numGraphs1 = numGraphs1 + (*iter);
  }
  if( numGraphs1 != nGraphEcAtten ) {
    cout << "tofcalgsec::calib_endcap_atten: the number of Graphs is NOT reasonable!!!" << endl;
    exit(0);
  }
  int numGraphs2 = 0;
  iter = nGraphPerCanvas.begin();
  for( ; iter!=nGraphPerCanvas.end(); iter++ ) {
    numGraphs2 = numGraphs2 + (*iter);
  }
  if( numGraphs2 != nGraphEcAtten ) {
    cout << "tofcalgsec::calib_endcap_atten: the number of Graphs is NOT reasonable!!!" << endl;
    exit(0);
  }

  m_name = string("calib_endcap_atten");

  const int qbin      = 100;
  const double qbegin = 0.0;
  const double qend   = 5000.0;

  // histograms per counter
  char hname[256];
  for( unsigned int i=0; i<NEndcap; i++ ) {
    m_result.push_back( HepVector(nEndcapAtten,0) );
    for( unsigned int k=0; k<nrbin; k++ ) {
      sprintf( hname, "Q-id%i-r%i", i, k);
      m_histograms.push_back( new TH1F( hname, hname, qbin, qbegin, qend ) );
      m_fitresult.push_back( HepVector(nParEcAtten,0) );
    }
    sprintf( hname, "Q0-id%i", i );
    m_histograms.push_back( new TH1F( hname, hname, qbin, qbegin, qend ) );
    m_fitresult.push_back( HepVector(nParEcAtten,0) );
  }

  rpos.resize( nrbin );
  rposerr.resize( nrbin );
  rstep = ( rend - rbegin )/nrbin;
  for( unsigned int i=0; i<nrbin; i++ ) {
    rpos[i] = rbegin + ( i+0.5 )*rstep;
    rposerr[i] = 0.5*rstep;
  }
  itofid.resize( NEndcap );
  itofiderr.resize( NEndcap );
  itofidstep = 1.0;
  for( unsigned int i=0; i<NEndcap; i++ ) {
    itofid[i] = i*1.0;
    itofiderr[i] = 0.5;
  }

}


calib_endcap_atten::~calib_endcap_atten() {
  m_fitresult.clear();
  rpos.clear();
  rposerr.clear();
  itofid.clear();
  itofiderr.clear();
}


void calib_endcap_atten::calculate( RecordSet*& data, unsigned int icounter ) {

  std::cout << setiosflags(ios::left) << setw(10) << icounter << setw(8) << data->size() << setw(30) << name() << std::endl;

  if( data->size() > 0 ) {
    std::vector<Record*>::iterator iter = data->begin();
    for( ; iter!=data->end(); iter++ ) {
      fillRecord( (*iter), icounter );
    }
    fitHistogram( icounter );
    fillGraph( icounter );
    fitGraph( icounter );
  }
  else {
    fillGraph( icounter );  // keep the m_graphs is not empty()
  }

  if( data->size() > 0 ) {
    std::vector<Record*>::iterator iter = data->begin();
    for( ; iter!=data->end(); iter++ ) {
      updateData( (*iter), icounter );
      fillRecordQ0( (*iter), icounter );
    }
    fitHistogramQ0( icounter );
  }

  if( icounter==(NEndcap-1) ) {
    fillGraphQ0();
  }

  return;
}


void calib_endcap_atten::fillRecord( const Record* r, unsigned int icounter ) {

  double rhit = r->zrhit();
  if( rhit<rbegin || rhit>rend ) return;
  int rbin = static_cast<int>((rhit-rbegin)/rstep);
  if( rbin<0 ) { rbin = 0; }
  else if( rbin>static_cast<int>(nBinPerCounter-1-1) ) {
    cout << "tofcalgsec::calib_endcap_atten:fillRecord: rhit is out of range, rhit=" << rhit << " rbin=" << rbin << endl;
    return;
  }

  std::vector<TH1F*>::iterator iter = m_histograms.begin() + icounter*nKind*nBinPerCounter + rbin;
  (*iter)->Fill( r->qleft()*abs(r->theta()) );

  return;
}


void calib_endcap_atten::fitHistogram( unsigned int icounter ) {
  TF1* ld = new TF1("ld", "landau");
  ld->SetLineColor(2);
  ld->SetLineWidth(1);

  std::vector<TH1F*>::iterator     iter1 = m_histograms.begin() + icounter*nKind*nBinPerCounter;
  std::vector<HepVector>::iterator iter2 = m_fitresult.begin() + icounter*nKind*nBinPerCounter;
  for( unsigned int j=0; j<nBinPerCounter-1; j++, iter1++, iter2++ ) {
    (*iter1)->Fit( ld, "Q");
    (*iter2)[0] = ld->GetParameter(1);
    (*iter2)[1] = ld->GetParError(1);
    (*iter2)[2] = ld->GetParameter(2);
    (*iter2)[3] = ld->GetParError(2);
  }

  return;

}


void calib_endcap_atten::fillGraph( unsigned int icounter ) {

  char gname1[256], gname2[256];

  // fill graphs
  // 2 canvas per counter,
  //   1. Q MPV vs z 
  //   2. Q sigma vs z
  std::vector<double> toffset, toffseterr;
  std::vector<double> tsigma, tsigmaerr;
  toffset.resize( nBinPerCounter-1 );
  toffseterr.resize( nBinPerCounter-1 );
  tsigma.resize( nBinPerCounter-1 );
  tsigmaerr.resize( nBinPerCounter-1 );

  std::vector<HepVector>::iterator iter = m_fitresult.begin() + icounter*nKind*nBinPerCounter;
  for( unsigned int k=0; k<nBinPerCounter-1; k++ ) {
    toffset[k]    = log((*(iter+k))[0]);
    toffseterr[k] = log((*(iter+k))[0])*((*(iter+k))[1])/((*(iter+k))[0]);
    tsigma[k]     = (*(iter+k))[2];
    tsigmaerr[k]  = (*(iter+k))[3];
  }

  sprintf( gname1, "Q MPV-tofid-%i", icounter );
  m_graphs.push_back( new TH1F( gname1, gname1, nBinPerCounter-1, rbegin, rend ) );
  std::vector<TH1F*>::iterator itgraph = m_graphs.end() - 1;
  (*itgraph)->SetMarkerSize(1.5);
  (*itgraph)->SetMarkerStyle(20);
  (*itgraph)->SetMarkerColor(2);
  for( unsigned int k=0; k<nBinPerCounter-1; k++ ) {
    (*itgraph)->SetBinContent( k+1, toffset[k]    );
    (*itgraph)->SetBinError(   k+1, toffseterr[k] );
  }

  sprintf( gname2, "Q sigma-tofid-%i", icounter );
  m_graphs.push_back( new TH1F( gname2, gname2, nBinPerCounter-1, rbegin, rend ) );
  itgraph = m_graphs.end() - 1;
  (*itgraph)->SetMarkerSize(1.5);
  (*itgraph)->SetMarkerStyle(21);
  (*itgraph)->SetMarkerColor(4);
  for( unsigned int k=0; k<nBinPerCounter-1; k++ ) {
    (*itgraph)->SetBinContent( k+1, tsigma[k]    );
    (*itgraph)->SetBinError(   k+1, tsigmaerr[k] );
  }

  return;
}


void calib_endcap_atten::fitGraph( unsigned int icounter ) {

  TF1 *fsingleq = new TF1( "fsingleq", endcapQFunc, rbegin, rend, 3 );
  fsingleq->SetLineColor(1);
  fsingleq->SetLineWidth(1);
  fsingleq->SetParameters( 6.5, 0.0, 0.0 );

  std::vector<TH1F*>::iterator itgraph = m_graphs.begin() + icounter*nGraphEcAtten;

  (*itgraph)->Fit( "fsingleq", "Q", "", rbegin, rend );
  X = HepVector( m_npar, 0 );
  X[0] = fsingleq->GetParameter(0);
  X[1] = fsingleq->GetParameter(1);
  X[2] = fsingleq->GetParameter(2);
  X[3] = 0.;
  X[4] = 0.;
  X[5] = 0.;

  std::vector<HepVector>::iterator iter = m_result.begin() + icounter;
  (*iter) = X;

  return;
}


void calib_endcap_atten::updateData( Record* r, unsigned int icounter ) {
  double rhit     = r->zrhit();
  double q        = r->qleft();
  double costheta = abs(r->theta());

  double par[3];
  std::vector<HepVector>::iterator iter = m_result.begin() + icounter;
  for( unsigned int i=0; i<3; i++ ) {
    par[i] = (*iter)[i];
  }

  par[0] = 0.;
  double q0 = q*costheta/exp(endcapQFunc(&rhit,par));
  r->setQ0( q0 );

  return;
}


void calib_endcap_atten::fillRecordQ0( const Record* r, unsigned int icounter ) {
  std::vector<TH1F*>::iterator iter = m_histograms.begin() + icounter*nKind*nBinPerCounter + nBinPerCounter - 1;
  (*iter)->Fill( r->q0() );
 
  return;
}


void calib_endcap_atten::fitHistogramQ0( unsigned int icounter ) {
  TF1* ld = new TF1("ld", "landau");
  ld->SetLineColor(2);
  ld->SetLineWidth(1);

  std::vector<TH1F*>::iterator     iter1 = m_histograms.begin() + icounter*nKind*nBinPerCounter + nBinPerCounter - 1;
  std::vector<HepVector>::iterator iter2 = m_fitresult.begin() + icounter*nKind*nBinPerCounter + nBinPerCounter - 1;
  (*iter1)->Fit( ld, "Q");
  (*iter2)[0] = ld->GetParameter(1);
  (*iter2)[1] = ld->GetParError(1);
  (*iter2)[2] = ld->GetParameter(2);
  (*iter2)[3] = ld->GetParError(2);

  return;
}


void calib_endcap_atten::fillGraphQ0() {
  char gname1[256], gname2[256];

  // fill graphs
  // 2 canvas per counter,
  //   1. Q0 MPV vs z 
  //   2. Q0 sigma vs z
  std::vector<double> toffset, toffseterr;
  std::vector<double> tsigma, tsigmaerr;
  toffset.resize( NEndcap );
  toffseterr.resize( NEndcap );
  tsigma.resize( NEndcap );
  tsigmaerr.resize( NEndcap );

  unsigned int number = 0;
  std::vector<HepVector>::iterator iter1 = m_fitresult.begin() + nBinPerCounter - 1;
  std::vector<HepVector>::iterator iter2 = m_result.begin();
  for( unsigned int i=0; i<NEndcap; i++ ) {
    number = i*nBinPerCounter;
    toffset[i]    = (*(iter1+number))[0];
    toffseterr[i] = (*(iter1+number))[1];
    tsigma[i]     = (*(iter1+number))[2];
    tsigmaerr[i]  = (*(iter1+number))[3];


    (*(iter2+i))[3] = toffset[i]/toffset[0];
    (*(iter2+i))[4] = toffset[i];
  }

  sprintf( gname1, "Q0 MPV vs TOF Counter Number" );
  m_graphs.push_back( new TH1F( gname1, gname1, NEndcap, -0.5, NEndcap-0.5 ) );
  std::vector<TH1F*>::iterator itgraph = m_graphs.end() - 1;
  (*itgraph)->SetMarkerSize(1.5);
  (*itgraph)->SetMarkerStyle(20);
  (*itgraph)->SetMarkerColor(2);
  for( unsigned int k=0; k<nBinPerCounter-1; k++ ) {
    (*itgraph)->SetBinContent( k+1, tsigma[k]    );
    (*itgraph)->SetBinError(   k+1, tsigmaerr[k] );
  }
  for( unsigned int i=0; i<NEndcap; i++ ) {
    (*itgraph)->SetBinContent( i+1, toffset[i]    );
    (*itgraph)->SetBinError(   i+1, toffseterr[i] );
  }

  sprintf( gname2, "Q0 Sigma vs TOF Counter Number" );
  m_graphs.push_back( new TH1F( gname2, gname2, NEndcap, -0.5, NEndcap-0.5 ) );
  itgraph = m_graphs.end() - 1;
  (*itgraph)->SetTitle(gname2);
  (*itgraph)->SetMarkerSize(1.5);
  (*itgraph)->SetMarkerStyle(21);
  (*itgraph)->SetMarkerColor(4);
  for( unsigned int i=0; i<NEndcap; i++ ) {
    (*itgraph)->SetBinContent( i+1, tsigma[i]    );
    (*itgraph)->SetBinError(   i+1, tsigmaerr[i] );
  }

  return;
}
