#include "tofcalgsec/calib_barrel_q0.h"
#include "TF1.h"

using namespace CLHEP;

calib_barrel_q0::calib_barrel_q0():TofCalibFit( true, nBarrelQ0 ) {

  nKind          = 1;
  nBinPerCounter = 1;

  nHistPerCounter   = 0;
  nCanvasPerCounter = 0;

  nHistPerCounter   = 0;
  nCanvasPerCounter = 0;
  nHistogram      = NBarrel*nKind*nBinPerCounter;   // NO directory
  nCanvas         = 2;
  CanvasName.push_back( static_cast<string>("Most Probable Value of Q0 vs TOF counter Number (Barrel part)") );
  CanvasName.push_back( static_cast<string>("Sigma of Q0 vs TOF Counter Number (Barrel part)") );
  nGraphPerCanvas.push_back(1);
  nGraphPerCanvas.push_back(1);

  m_name = string("calib_barrel_q0");

  int numGraphs = 0;
  std::vector<unsigned int>::iterator iter = nGraphPerCanvas.begin();
  for( ; iter!=nGraphPerCanvas.end(); iter++ ) {
    numGraphs = numGraphs + (*iter);
  }
  if( numGraphs != nGraphTotalQ0 ) {
    cout << "tofcalgsec::calib_barrel_q0: the number of Graphs is NOT reasonable!!!" << endl;
    exit(0);
  }

  const int qbin   = 100;
  const double qbegin = 0.0;
  const double qend   = 5000.0;

  // histograms
  char hname[256];
  for( unsigned int i=0; i<NBarrel; i++ ) {
    m_result.push_back( HepVector(nBarrelQ0,0) );

    sprintf( hname, "Q0-tofid-%i", i );
    m_histograms.push_back( new TH1F( hname, hname, qbin, qbegin, qend ) );

    m_fitresult.push_back( HepVector(nParQ0,0) );
  }

  itofid.resize( NBarrel );
  itofiderr.resize( NBarrel );
  itofidstep = 1.0;
  for( unsigned int i=0; i<NBarrel; i++ ) {
    itofid[i] = i*1.0;
    itofiderr[i] = 0.5;
  }
}


calib_barrel_q0::~calib_barrel_q0() {
  m_fitresult.clear();
  itofid.clear();
  itofiderr.clear();
}


void calib_barrel_q0::calculate( RecordSet*& data, unsigned int icounter ) {

  std::cout << setiosflags(ios::left) << setw(10) << icounter << setw(8) << data->size() << setw(30) << name() << std::endl;

  if( data->size() > 0 ) {
    std::vector<Record*>::iterator iter = data->begin();
    for( ; iter!=data->end(); iter++ ) {
      fillRecord( (*iter), icounter );
    }
  }
  fitHistogram( icounter );

  if( icounter==(NBarrel-1) ) {
    fillGraph();
    fitGraph();
  }

  return;
}


void calib_barrel_q0::fillRecord( const Record* r, unsigned int icounter ) {
  std::vector<TH1F*>::iterator iter = m_histograms.begin() + icounter;
  (*iter)->Fill( r->q0() );
  return;
}


void calib_barrel_q0::fitHistogram( unsigned int icounter ) {
  TF1* ld = new TF1("ld", "landau");
  ld->SetLineColor(2);
  ld->SetLineWidth(1);

  std::vector<TH1F*>::iterator     iter1 = m_histograms.begin() + icounter;
  std::vector<HepVector>::iterator iter2 = m_fitresult.begin() + icounter;
  (*iter1)->Fit( ld, "Q");
  (*iter2)[0] = ld->GetParameter(1);
  (*iter2)[1] = ld->GetParError(1);
  (*iter2)[2] = ld->GetParameter(2);
  (*iter2)[3] = ld->GetParError(2);

  return;
}


void calib_barrel_q0::fillGraph() {

  std::vector<double> qmean, qmeanerr;
  std::vector<double> qsig, qsigerr;
  qmean.resize( NBarrel );
  qmeanerr.resize( NBarrel );
  qsig.resize( NBarrel );
  qsigerr.resize( NBarrel );

  TH1F* graph1 = new TH1F( CanvasName[0].c_str(), CanvasName[0].c_str(), NBarrel, -0.5, NBarrel-0.5 );
  graph1->SetMarkerSize(1.5);
  graph1->SetMarkerStyle(20);
  graph1->SetMarkerColor(2);

  TH1F* graph2 = new TH1F( CanvasName[1].c_str(), CanvasName[1].c_str(), NBarrel, -0.5, NBarrel-0.5 );
  graph2->SetMarkerSize(1.5);
  graph2->SetMarkerStyle(20);
  graph2->SetMarkerColor(4);

  std::vector<HepVector>::iterator iter = m_fitresult.begin();
  for( unsigned int i=0; i<NBarrel; i++, iter++ ) {
    qmean[i]    = (*iter)[0];
    qmeanerr[i] = (*iter)[1];
    qsig[i]     = (*iter)[2];
    qsigerr[i]  = (*iter)[3];
    graph1->SetBinContent( i+1, qmean[i]    );
    graph1->SetBinError(   i+1, qmeanerr[i] );
    graph2->SetBinContent( i+1, qsig[i]     );
    graph2->SetBinError(   i+1, qsigerr[i]  );
  }

  m_graphs.push_back( graph1 );
  m_graphs.push_back( graph2 );

  return;
}


void calib_barrel_q0::fitGraph() {
  unsigned int number = 0;
  std::vector<HepVector>::iterator iter1 = m_result.begin();
  std::vector<HepVector>::iterator iter2 = m_fitresult.begin();
  for( ; iter1!=m_result.end(); iter1++, number++ ) {
    (*iter1)[0] = (*(iter2+number))[0]/ (*iter2)[0];
    (*iter1)[1] = (*(iter2+number))[0];
    (*iter1)[2] = (*(iter2+number))[2];
  }

  return;
}
