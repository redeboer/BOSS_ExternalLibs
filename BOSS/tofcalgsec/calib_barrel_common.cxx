#include "tofcalgsec/calib_barrel_common.h"
#include "TF1.h"

calib_barrel_common::calib_barrel_common( const unsigned int nzbin ):TofCalibFit( true, nBarrelCommon ) {

  nKind          = 4;
  nBinPerCounter = nzbin;

  nHistPerCounter   = 0;
  nCanvasPerCounter = 0;
  nHistogram      = nKind*nBinPerCounter+1;
  nCanvas         = 4;
  CanvasName.push_back( static_cast<string>("Offset") );
  CanvasName.push_back( static_cast<string>("Offset-PlusMinus") );  
  CanvasName.push_back( static_cast<string>("Sigma") );  
  CanvasName.push_back( static_cast<string>("Sigma-TCorrelation") );  
  nGraphPerCanvas.push_back(2);
  nGraphPerCanvas.push_back(2);
  nGraphPerCanvas.push_back(2);
  nGraphPerCanvas.push_back(3);

  int numGraphs = 0;
  std::vector<unsigned int>::iterator iter = nGraphPerCanvas.begin();
  for( ; iter!=nGraphPerCanvas.end(); iter++ ) {
    numGraphs = numGraphs + (*iter);
  }
  if( numGraphs != nGraphTotalCommon ) {
    cout << "tofcalgsec::calib_barrel_common: the number of Graphs is NOT reasonable!!!" << endl;
    exit(0);
  }

  m_name = string("calib_barrel_common");

  const int tbin   = 150;
  const double tbegin = -1.5;
  const double tend   =  1.5;

  char hname[256];
  // histograms
  for( unsigned int j=0; j<nKind; j++ ) {
    for( unsigned int k=0; k<nBinPerCounter; k++ ) {
      if( j==0 )      { sprintf( hname, "tleft-z%i",  k); }
      else if( j==1 ) { sprintf( hname, "tright-z%i", k); }
      else if( j==2 ) { sprintf( hname, "tplus-z%i",  k); }
      else if( j==3 ) { sprintf( hname, "tminus-z%i", k); }
      m_fitresult.push_back( HepVector( nParCommon,0 ) );
      m_histograms.push_back( new TH1F( hname, hname, tbin, tbegin, tend ) );
    }
  }
  sprintf( hname, "deltaT" );
  m_fitresult.push_back( HepVector( nParCommon,0 ) );
  m_histograms.push_back( new TH1F( hname, hname, tbin, tbegin, tend ) );

  zpos.resize( nBinPerCounter );
  zposerr.resize( nBinPerCounter );
  zstep = ( zend - zbegin )/nBinPerCounter;
  for( unsigned int i=0; i<nBinPerCounter; i++ ) {
    zpos[i] = zbegin + ( i+0.5 )*zstep;
    zposerr[i] = 0.5*zstep;
  }

}


calib_barrel_common::~calib_barrel_common() {
  m_fitresult.clear();
  zpos.clear();
  zposerr.clear();
}


void calib_barrel_common::calculate( RecordSet*& data, unsigned int icounter ) {

  std::cout << setiosflags(ios::left) << setw(10) << icounter << setw(8) << data->size() << setw(30) << name() << std::endl;

  if( data->size() > 0 ) {
    std::vector<Record*>::iterator iter = data->begin();
    for( ; iter!=data->end(); iter++ ) {
      fillRecord( (*iter) );
    }
  }

  if( icounter==(NBarrel-1) ) {
    fitHistogram();
    fillGraph();
    fitGraph();
  }

  return;
}



void calib_barrel_common::fillRecord( const Record* r ) {
  double zhit = r->zrhit();
  if( zhit<zbegin || zhit>zend ) return;
  int zbin = static_cast<int>((zhit-zbegin)/zstep);
  if( ( zbin<0 ) || ( zbin>static_cast<int>(nBinPerCounter-1) ) ) {
    cout << "tofcalgsec::calib_barrel_common: zhit is out of range, zhit=" << zhit << " zbin=" << zbin << endl;
    return;
  }

  std::vector<TH1F*>::iterator iter = m_histograms.begin();
  (*(iter+zbin))->Fill( r->tleft() );
  (*(iter+nBinPerCounter+zbin))->Fill( r->tright() );
  (*(iter+2*nBinPerCounter+zbin))->Fill( (r->tleft()+r->tright())/2.0 );
  (*(iter+3*nBinPerCounter+zbin))->Fill( (r->tleft()-r->tright())/2.0 );
  (*(iter+nKind*nBinPerCounter))->Fill( r->tleft() );
  (*(iter+nKind*nBinPerCounter))->Fill( r->tright() );

  return;
}


void calib_barrel_common::fitHistogram() {
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

  iter2 = m_fitresult.end() - 1;
  X[2] = (*iter2)[0];
  X[3] = (*iter2)[1];

  return;

}


void calib_barrel_common::fillGraph() {

  char gname1[256], gname2[256];
  TH1F *gra[nGraphTotalCommon];

  // 4 canvas all counter,
  //   1. offset of tleft and tright vs z --- gra[0] and gra[1]
  //   2. common of tleft and tright vs z  --- gra[2] and gra[3]
  //   3. offset of tplus and tminus vs z --- gra[4] and gra[5]
  //   4. common of tplus, tminus and T_Correlation vs z
  //                                      --- gra[6], gra[7] and gra[8]

  std::vector<double> toffset, toffseterr;
  std::vector<double> tsigma, tsigmaerr;
  toffset.resize( nBinPerCounter );
  toffseterr.resize( nBinPerCounter );
  tsigma.resize( nBinPerCounter );
  tsigmaerr.resize( nBinPerCounter );

  unsigned int number = 0;
  std::vector<HepVector>::iterator iter = m_fitresult.begin();
  for( unsigned int j=0; j<nKind; j++ ) {
    if( j==0 ) {
      sprintf( gname1, "tlefttright-offset" );
      sprintf( gname2, "tlefttright-sigma" );
    }
    else if( j==1 ) {
      sprintf( gname1, "tcommon-offset" );
      sprintf( gname2, "tcommon-sigma" );
    }
    else if( j==2 ) {
      sprintf( gname1, "tplusminus-offset" );
      sprintf( gname2, "tplusminus-sigma" );
    }
    else if( j==3 ) {
      sprintf( gname1, "tcorrelation-offset" );
      sprintf( gname2, "tcorrelation-sigma" );
    }

    gra[j] = new TH1F( gname1, gname1, nBinPerCounter, zbegin, zend );
    gra[j+4] = new TH1F( gname2, gname2, nBinPerCounter, zbegin, zend );

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

  gra[nGraphTotalCommon-1] = new TH1F( "Sigma", "Sigma", nBinPerCounter, zbegin, zend );
  for( unsigned int k=0; k<nBinPerCounter; k++ ) {
    number = (nKind-1)*nBinPerCounter + k;
    double sigPlus  = (*(iter+number-nBinPerCounter))[2];
    double sigMinus = (*(iter+number))[2];
    double sigErrPlus  = (*(iter+number-nBinPerCounter))[3];
    double sigErrMinus = (*(iter+number))[3];
    if( sigPlus>sigMinus ) {
      tsigma[k]    = sqrt( sigPlus*sigPlus - sigMinus*sigMinus );
    }
    else {
      tsigma[k]    = 0.0 - sqrt( sigMinus*sigMinus - sigPlus*sigPlus );
    }
    tsigmaerr[k] = sqrt( sigErrPlus*sigErrPlus + sigErrMinus*sigErrMinus );

    gra[nGraphTotalCommon-1]->SetBinContent( k+1, tsigma[k]    );
    gra[nGraphTotalCommon-1]->SetBinError(   k+1, tsigmaerr[k] );
  }

  for( int j=0; j<nGraphTotalCommon; j++, j++ ) {
    gra[j]->SetMarkerSize(1.5);
    gra[j]->SetMarkerStyle(20);
    gra[j]->SetMarkerColor(2);
    if( j==4 ) {
      gra[j]->SetMaximum( 0.22 );
      gra[j]->SetMinimum( 0.07 );
    }
    else if( j==6 ) {
      gra[j]->SetMaximum(  0.20 );
      gra[j]->SetMinimum( -0.02 );
    }
  }
  for( int j=1; j<nGraphTotalCommon; j++, j++ ) {
    gra[j]->SetMarkerSize(1.5);
    gra[j]->SetMarkerStyle(21);
    gra[j]->SetMarkerColor(4);
  }
  gra[nGraphTotalCommon-1]->SetMarkerStyle(4);
  gra[nGraphTotalCommon-1]->SetMarkerColor(6);

  for( int j=0; j<nGraphTotalCommon; j++ ) {
    m_graphs.push_back( gra[j] );
  }

  return;
}


void calib_barrel_common::fitGraph() {
  TF1* p0 = new TF1("p0", "pol0");
  p0->SetLineColor(1);
  p0->SetLineWidth(1);

  std::vector<TH1F*>::iterator iter=m_graphs.end()-1;
  (*iter)->Fit( "p0", "Q" );
  X[0] = p0->GetParameter(0);
  X[1] = p0->GetParError(0);

  m_result.push_back( X );
  return;
}
