#include "tofcalgsec/calib_barrel_sigma.h"
#include "TF1.h"

//the fit function to the single-end time resolution versus z
/*
static double singleEndFunc(double *x, double *par) {
  double xx = x[0];
  double f = par[0]*barrelRadius/sqrt(pow(barrelRadius,2)+pow(xx,2))+par[1]*xx+par[2]*pow(xx,2)+par[3]*pow(xx,3);
  return f;
}


//the fit function to the double-end time resolution versus z
static double doubleEndFunc(double *x, double *par)
{
  double xx = x[0];
  double f = par[0]*pow(xx,2)/sqrt(pow(barrelRadius,2)+pow(xx,2))+par[1]+par[2]*pow(xx,2);
  return f;
}
*/

calib_barrel_sigma::calib_barrel_sigma( const unsigned int nzbin ):TofCalibFit( true, nBarrelSigma ) {

  nKind          = 5;    // 0:tleft, 1:tright, 2:t0, 3:tplus, 4:tminus
  nBinPerCounter = nzbin;

  nHistPerCounter   = nKind*nBinPerCounter;
  nCanvasPerCounter = 4;
  CanvasPerCounterName.push_back( static_cast<string>("Barrel-offset") );  
  CanvasPerCounterName.push_back( static_cast<string>("Offset-TimeCorrelation") );  
  CanvasPerCounterName.push_back( static_cast<string>("Barrel-sigma") );  
  CanvasPerCounterName.push_back( static_cast<string>("Sigma-TimeCorrelation") );  
  nGraphPerCanvasPerCounter.push_back(3);
  nGraphPerCanvasPerCounter.push_back(2);
  nGraphPerCanvasPerCounter.push_back(3);
  nGraphPerCanvasPerCounter.push_back(3);

  nHistogram      = 0;
  nCanvas         = 0;

  int numGraphs = 0;
  std::vector<unsigned int>::iterator iter = nGraphPerCanvasPerCounter.begin();
  for( ; iter!=nGraphPerCanvasPerCounter.end(); iter++ ) {
    numGraphs = numGraphs + (*iter);
  }
  if( numGraphs != nGraphTotalSigma ) {
    cout << "tofcalgsec::calib_barrel_sigma: the number of Graphs is NOT reasonable!!!" << endl;
    exit(0);
  }

  m_name = string("calib_barrel_sigma");

  const int tbin   = 150;
  const double tbegin = -1.5;
  const double tend   =  1.5;

  // histograms per counter
  char hname[256];
  for( unsigned int i=0; i<NBarrel; i++ ) {
    m_result.push_back( HepVector(nBarrelSigma,0) );
    for( unsigned int j=0; j<nKind; j++ ) {
      for( unsigned int k=0; k<nBinPerCounter; k++ ) {
	if( j==0 )      { sprintf( hname, "tleft-id%i-z%i",  i, k); }
	else if( j==1 ) { sprintf( hname, "tright-id%i-z%i", i, k); }
	else if( j==2 ) { sprintf( hname, "t0-id%i-z%i",     i, k); }
	else if( j==3 ) { sprintf( hname, "tplus-id%i-z%i",  i, k); }
	else if( j==4 ) { sprintf( hname, "tminus-id%i-z%i", i, k); }
	m_histograms.push_back( new TH1F( hname, hname, tbin, tbegin, tend ) );

	m_fitresult.push_back( HepVector(nParSigma,0) );
      }
    }
  }

  zpos.resize( nBinPerCounter );
  zposerr.resize( nBinPerCounter );
  zstep = ( zend - zbegin )/nBinPerCounter;
  for( unsigned int i=0; i<nBinPerCounter; i++ ) {
    zpos[i] = zbegin + ( i+0.5 )*zstep;
    zposerr[i] = 0.5*zstep;
  }

}


calib_barrel_sigma::~calib_barrel_sigma() {
  m_fitresult.clear();
  zpos.clear();
  zposerr.clear();
}


void calib_barrel_sigma::calculate( RecordSet*& data, unsigned int icounter ) {

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

  if( data->size() > 0 ) {
    std::vector<Record*>::iterator iter = data->begin();
    for( ; iter!=data->end(); iter++ ) {
      updateData( (*iter), icounter );
      fillRecordT0( (*iter), icounter );
    }
  }
  fitHistogramT0( icounter );
  fillGraphT0( icounter );
  fitGraphT0( icounter );

  return;
}


void calib_barrel_sigma::fillRecord( const Record* r, unsigned int icounter ) {

  double zhit = r->zrhit();
  if( zhit<zbegin || zhit>zend ) return;
  int zbin = static_cast<int>((zhit-zbegin)/zstep);
  if( zbin<0 ) { zbin = 0; }
  else if( zbin>static_cast<int>(nBinPerCounter-1) ) {
    cout << "tofcalgsec::calib_barrel_sigma:fillRecord: zhit is out of range, zhit=" << zhit << " zbin=" << zbin << endl;
    return;
  }

  std::vector<TH1F*>::iterator iter = m_histograms.begin();
  unsigned int number = icounter*nKind*nBinPerCounter + zbin;
  (*(iter+number))->Fill( r->tleft() );
  (*(iter+nBinPerCounter+number))->Fill( r->tright() );
  (*(iter+3*nBinPerCounter+number))->Fill( (r->tleft()+r->tright())/2.0 );
  (*(iter+4*nBinPerCounter+number))->Fill( (r->tleft()-r->tright())/2.0 );

  return;
}


void calib_barrel_sigma::fitHistogram( unsigned int icounter ) {
  TF1* g = new TF1("g", "gaus");
  g->SetLineColor(2);
  g->SetLineWidth(1);

  std::vector<TH1F*>::iterator     iter1 = m_histograms.begin() + icounter*nKind*nBinPerCounter;
  std::vector<HepVector>::iterator iter2 = m_fitresult.begin() + icounter*nKind*nBinPerCounter;
  for( unsigned int i=0; i<nKind; i++ ) {
    for( unsigned int j=0; j<nBinPerCounter; j++ ) {
      if( i!=2 ) {
	(*iter1)->Fit( g, "Q");
	(*iter2)[0] = g->GetParameter(1);
	(*iter2)[1] = g->GetParError(1);
	(*iter2)[2] = g->GetParameter(2);
	(*iter2)[3] = g->GetParError(2);
      }
      iter1++;
      iter2++;
    }
  }

  return;

}


void calib_barrel_sigma::fillGraph( unsigned int icounter ) {

  char gname1[256], gname2[256];

  // fill graphs
  // 4 canvas per counter,
  //   1. offset of tleft, tright and t0 vs z 
  //   2. sigma of tleft, tright and t0 vs z
  //   3. offset of tplus and tminus vs z
  //   4. sigma of tplus, tminus and T_Correlation vs z
  std::vector<double> toffset, toffseterr;
  std::vector<double> tsigma, tsigmaerr;
  toffset.resize( nBinPerCounter );
  toffseterr.resize( nBinPerCounter );
  tsigma.resize( nBinPerCounter );
  tsigmaerr.resize( nBinPerCounter );

  unsigned int number = 0;
  std::vector<HepVector>::iterator iter = m_fitresult.begin() + icounter*nKind*nBinPerCounter;
  for( unsigned int j=0; j<nKind; j++ ) {
    if( j==0 )      { sprintf( gname1, "tleft-offset-tofid-%i",  icounter ); }
    else if( j==1 ) { sprintf( gname1, "tright-offset-tofid-%i", icounter ); }
    else if( j==2 ) { sprintf( gname1, "t0-offset-tofid-%i",     icounter ); }
    else if( j==3 ) { sprintf( gname1, "tplus-offset-tofid-%i",  icounter ); }
    else if( j==4 ) { sprintf( gname1, "tminus-offset-tofid-%i", icounter ); }

    m_graphs.push_back( new TH1F( gname1, gname1, nBinPerCounter, zbegin, zend ) );
    std::vector<TH1F*>::iterator itgraph = m_graphs.end() - 1;

    for( unsigned int k=0; k<nBinPerCounter; k++ ) {
      number = j*nBinPerCounter + k;
      toffset[k]    = (*(iter+number))[0];
      toffseterr[k] = (*(iter+number))[1];
      (*itgraph)->SetBinContent( k+1, toffset[k]    );
      (*itgraph)->SetBinError(   k+1, toffseterr[k] );
    }
    (*itgraph)->SetMarkerSize(1.5);
    if( j==0 || j==3) {
      (*itgraph)->SetMarkerStyle(20);
      (*itgraph)->SetMarkerColor(2);
      (*itgraph)->SetMaximum( 0.15 );
      (*itgraph)->SetMinimum(-0.15 );
    }
    else if( j==1 || j==4 ) {
      (*itgraph)->SetMarkerStyle(21);
      (*itgraph)->SetMarkerColor(4);
    }
    else if( j==2 ) {
      (*itgraph)->SetMarkerStyle(4);
      (*itgraph)->SetMarkerColor(2);
    }
  }
    
  for( unsigned int j=0; j<nKind; j++ ) {
    if( j==0 )      { sprintf( gname2, "tleft-sigma-tofid-%i",  icounter ); }
    else if( j==1 ) { sprintf( gname2, "tright-sigma-tofid-%i", icounter ); }
    else if( j==2 ) { sprintf( gname2, "t0-sigma-tofid-%i",     icounter ); }
    else if( j==3 ) { sprintf( gname2, "tplus-sigma-tofid-%i",  icounter ); }
    else if( j==4 ) { sprintf( gname2, "tminus-sigma-tofid-%i", icounter ); }
    m_graphs.push_back( new TH1F( gname2, gname2, nBinPerCounter, zbegin, zend ) );
    std::vector<TH1F*>::iterator itgraph = m_graphs.end() - 1;

    for( unsigned int k=0; k<nBinPerCounter; k++ ) {
      number = j*nBinPerCounter + k;
      tsigma[k]     = (*(iter+number))[2];
      tsigmaerr[k]  = (*(iter+number))[3];
      (*itgraph)->SetBinContent( k+1, tsigma[k]    );
      (*itgraph)->SetBinError(   k+1, tsigmaerr[k] );
    }
    (*itgraph)->SetMarkerSize(1.5);
    if( j==0 || j==3 ) {
      (*itgraph)->SetMarkerStyle(20);
      (*itgraph)->SetMarkerColor(2);
      (*itgraph)->SetMaximum( 0.3 );
      (*itgraph)->SetMinimum( 0.0 );
    }
    else if( j==1 || j==4 ) {
      (*itgraph)->SetMarkerStyle(21);
      (*itgraph)->SetMarkerColor(4);
    }
    else if( j==2 ) {
      (*itgraph)->SetMarkerStyle(4);
      (*itgraph)->SetMarkerColor(2);
    }
  }

  sprintf( gname2, "sigma-tofid-%i", icounter );
  m_graphs.push_back( new TH1F( gname2, gname2, nBinPerCounter, zbegin, zend ) );
  std::vector<TH1F*>::iterator itgraph = m_graphs.end() - 1;
  for( unsigned int k=0; k<nBinPerCounter; k++ ) {
    number = (nKind-1)*nBinPerCounter + k;
    double sigPlus  = (*(iter+number-nBinPerCounter))[2];
    double sigMinus = (*(iter+number))[2];
    double sigErrPlus  = (*(iter+number-nBinPerCounter))[3];
    double sigErrMinus = (*(iter+number))[3];
    tsigma[k]     = sqrt( sigPlus*sigPlus - sigMinus*sigMinus );
    tsigmaerr[k]  = sqrt( sigErrPlus*sigErrPlus + sigErrMinus*sigErrMinus );
    (*itgraph)->SetBinContent( k+1, tsigma[k]    );
    (*itgraph)->SetBinError(   k+1, tsigmaerr[k] );
  }
  (*itgraph)->SetMarkerSize(1.5);
  (*itgraph)->SetMarkerStyle(4);
  (*itgraph)->SetMarkerColor(2);

  return;
}


void calib_barrel_sigma::fitGraph( unsigned int icounter ) {

  TF1* fsingle = new TF1("fsingle", "pol4");
  fsingle->SetLineColor(1);
  fsingle->SetLineWidth(1);

  std::vector<unsigned int>::iterator itnumber = nGraphPerCanvasPerCounter.begin();
  std::vector<TH1F*>::iterator        itgraph  = m_graphs.begin() + icounter*nGraphTotalSigma + (*itnumber) + (*(itnumber+1));

  fsingle->SetParameter( 0, 0.14    );
  fsingle->SetParameter( 1, -4.0e-4 );
  (*itgraph)->Fit( "fsingle", "QR", "", zbegin, zend );
  X = HepVector( m_npar, 0 );
  for( unsigned int i=0; i<5; i++ ) {
    X[i] = fsingle->GetParameter(i);
  }

  fsingle->SetParameter( 0, 0.14   );
  fsingle->SetParameter( 1, 4.0e-4 );
  (*(itgraph+1))->Fit( "fsingle", "QR", "", zbegin, zend );
  for( unsigned int i=0; i<5; i++ ) {
    X[i+5] = fsingle->GetParameter(i);
  }

  std::vector<HepVector>::iterator iter = m_result.begin() + icounter;
  (*iter) = X;

  return;
}


void calib_barrel_sigma::updateData( Record* r, unsigned int icounter ) {
  double zhit = r->zrhit();
  double t1 = r->tleft();
  double t2 = r->tright();

  double par1[5], par2[5];
  std::vector<HepVector>::iterator iter = m_result.begin() + icounter;
  for( unsigned int i=0; i<5; i++ ) {
    par1[i] = (*iter)[i];
    par2[i] = (*iter)[i+5];
  }

  double tsigma1 = par1[0]+par1[1]*zhit+par1[2]*pow(zhit,2)+par1[3]*pow(zhit,3) + par1[4]*pow(zhit,4);
  double tsigma2 = par2[0]+par2[1]*zhit+par2[2]*pow(zhit,2)+par2[3]*pow(zhit,3) + par2[4]*pow(zhit,4);
  double tc      = m_tcorrelation[0];

  double weight1 = (tsigma2*tsigma2-tc*tc)/(tsigma1*tsigma1+tsigma2*tsigma2-2.0*tc*tc);
  double weight2 = (tsigma1*tsigma1-tc*tc)/(tsigma1*tsigma1+tsigma2*tsigma2-2.0*tc*tc);
  double t0 = weight1*t1 + weight2*t2;

  r->setT0( t0 );
  
  return;
}


void calib_barrel_sigma::fillRecordT0( const Record* r, unsigned int icounter ) {
  double zhit = r->zrhit();
  if( zhit<zbegin || zhit>zend ) return;
  int zbin = static_cast<int>((zhit-zbegin)/zstep);
  if( zbin<0 ) { zbin = 0; }
  else if( zbin>static_cast<int>(nBinPerCounter-1) ) {
    cout << "tofcalgsec::calib_barrel_sigma:fillRecordT0: zhit is out of range, zhit=" << zhit << " zbin=" << zbin << endl;
    return;
  }

  std::vector<TH1F*>::iterator iter = m_histograms.begin();
  unsigned int number = icounter*nKind*nBinPerCounter + 2*nBinPerCounter + zbin;
  (*(iter+number))->Fill( r->t0() );
 
  return;
}


void calib_barrel_sigma::fitHistogramT0( unsigned int icounter ) {
  TF1* g = new TF1("g", "gaus");
  g->SetLineColor(2);
  g->SetLineWidth(1);

  std::vector<TH1F*>::iterator     iter1 = m_histograms.begin() + icounter*nKind*nBinPerCounter + 2*nBinPerCounter;
  std::vector<HepVector>::iterator iter2 = m_fitresult.begin() + icounter*nKind*nBinPerCounter + 2*nBinPerCounter;
  for( unsigned int j=0; j<nBinPerCounter; j++, iter1++, iter2++ ) {
    (*iter1)->Fit( g, "Q");
    (*iter2)[0] = g->GetParameter(1);
    (*iter2)[1] = g->GetParError(1);
    (*iter2)[2] = g->GetParameter(2);
    (*iter2)[3] = g->GetParError(2);
  }

  return;
}


void calib_barrel_sigma::fillGraphT0( unsigned int icounter ) {
  char gname1[256], gname2[256];

  // fill graphs
  // 4 canvas per counter,
  //   1. offset of tleft, tright and t0 vs z 
  //   2. sigma of tleft, tright and t0 vs z
  //   3. offset of tplus and tminus vs z
  //   4. sigma of tplus, tminus and T_Correlation vs z
  std::vector<double> toffset, toffseterr;
  std::vector<double> tsigma, tsigmaerr;
  toffset.resize( nBinPerCounter );
  toffseterr.resize( nBinPerCounter );
  tsigma.resize( nBinPerCounter );
  tsigmaerr.resize( nBinPerCounter );

  std::vector<HepVector>::iterator iter = m_fitresult.begin() + icounter*nKind*nBinPerCounter + 2*nBinPerCounter;
  for( unsigned int k=0; k<nBinPerCounter; k++ ) {
    toffset[k]    = (*(iter+k))[0];
    toffseterr[k] = (*(iter+k))[1];
    tsigma[k]     = (*(iter+k))[2];
    tsigmaerr[k]  = (*(iter+k))[3];
  }

  sprintf( gname1, "offset-tofid-%i", icounter );
  std::vector<TH1F*>::iterator itgraph = m_graphs.begin() + icounter*nGraphTotalSigma + 2;
  for( unsigned int i=0; i<nBinPerCounter; i++ ) {
    //    (*itgraph)->SetPoint( i, zpos[i], toffset[i] );
    //    (*itgraph)->SetPointError( i, zposerr[i], toffseterr[i] );
    (*itgraph)->SetBinContent( i+1, toffset[i]    );
    (*itgraph)->SetBinError(   i+1, toffseterr[i] );
  }

  sprintf( gname2, "sigma-tofid-%i", icounter );
  itgraph = m_graphs.begin() + icounter*nGraphTotalSigma + 7;
  for( unsigned int i=0; i<nBinPerCounter; i++ ) {
    //    (*itgraph)->SetPoint( i, zpos[i], tsigma[i] );
    //    (*itgraph)->SetPointError( i, zposerr[i], tsigmaerr[i] );
    (*itgraph)->SetBinContent( i+1, tsigma[i]    );
    (*itgraph)->SetBinError(   i+1, tsigmaerr[i] );
  }

  return;
}


void calib_barrel_sigma::fitGraphT0( unsigned int icounter ) {

  //  TF1 *fdouble = new TF1( "fdouble", doubleEndFunc, zbegin, zend, 3 );
  TF1 *fdouble = new TF1( "fdouble", "pol4", zbegin, zend );
  fdouble->SetLineColor(1);
  fdouble->SetLineWidth(1);

  std::vector<unsigned int>::iterator itnumber = nGraphPerCanvasPerCounter.begin();
  std::vector<TH1F*>::iterator itgraph = m_graphs.begin() + icounter*nGraphTotalSigma + (*itnumber) + (*(itnumber+1)) + 2;
  (*itgraph)->Fit( "fdouble", "Q", "", zbegin, zend );

  std::vector<HepVector>::iterator iter = m_result.begin() + icounter;
  (*iter)[10] = fdouble->GetParameter(0);
  (*iter)[11] = fdouble->GetParameter(1);
  (*iter)[12] = fdouble->GetParameter(2);
  (*iter)[13] = fdouble->GetParameter(3);
  (*iter)[14] = fdouble->GetParameter(4);

  return;
}
