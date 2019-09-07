//QCMCFilter-00-00-02 Jan. 2013 Dan Ambrose
//Based on QCMCReweightProc program by Werner Sun  
//This version has a corrected Y input from original version
#include "QCMCFilterAlg/QCMCFilter.h"
#include "QCMCFilterAlg/Dalitz.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/RegistryEntry.h"

#include "TMath.h"

#include <cmath>
#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"
//#include "PartPropSvc/PartPropSvc.h"
#include "GaudiKernel/IPartPropSvc.h"

// Monte-Carlo data
#include "McTruth/McParticle.h"
#include "McTruth/MdcMcHit.h"

///////////////////
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include "EvtRecEvent/EvtRecEvent.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
using CLHEP::HepVector;
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;

#include <vector>

//
// constants, enums and typedefs
//
// Partilcle masses
const double xmpi0 =  0.134976;  // BOSS 6.4.1 pdt.table value 
const double xmeta =  0.54784;   // PDG08 = 0.547853, BOSS 6.4.1 pdt.table = 0.54784
const double xmkaon = 0.49368;
const double xmpion = 0.13957;
const double xmk0   = 0.49761;
const double xmrho =  0.77549;
const double xmd0 =   1.86484;  // PDG08 

const double PI = 3.1415926; //pi 

// Antiparticles have negative ID.

static const int kPsi3770ID = 30443 ;
static const int kD0ID = 421 ;
static const int kD0BarID = -421 ;
static const int kDpID = 411 ;
static const int kDmID = -411 ;
static const int kGammaID = 22 ;
static const int kGammaFSRID = -22 ;
static const int kPiPlusID = 211 ;
static const int kPiMinusID = -211 ;
static const int kPi0ID = 111 ;
static const int kEtaID = 221 ;
static const int kEtaPrimeID = 331 ;
static const int kF0ID = 9010221; 
static const int kFPrime0ID = 10221 ;
static const int kF0m1500ID = 50221;
static const int kF2ID = 225;
static const int kA00ID = 10111 ;
static const int kA0PlusID = 10211 ;
static const int kA0MinusID = -10211 ;
static const int kRhoPlusID = 213 ;
static const int kRhoMinusID = -213 ;
static const int kRho0ID = 113 ;
static const int kRho2SPlusID = 30213 ;
static const int kRho2SMinusID = -30213 ;
static const int kRho2S0ID = 30113 ;
static const int kA1PlusID = 20213 ;
static const int kA1MinusID = -20213 ;
static const int kA10ID = 20113 ;
static const int kOmegaID = 223 ;
static const int kPhiID = 333 ;
static const int kKPlusID = 321 ;
static const int kKMinusID = -321 ;
static const int kK0SID = 310 ;
static const int kK0LID = 130 ;
static const int kK0ID = 311 ;
static const int kK0BarID = -311 ;
static const int kKStarPlusID = 323 ;
static const int kKStarMinusID = -323 ;
static const int kKStar0ID = 313 ;
static const int kKStar0BarID = -313 ;
static const int kK0Star0ID = 10311;
static const int kK0Star0BarID = -10311;
static const int kK0StarPlusID = 10321;
static const int kK0StarMinusID = -10321;
static const int kK1PlusID = 10323 ;
static const int kK1MinusID = -10323 ;
static const int kK10ID = 10313 ;
static const int kK10BarID = -10313 ;
static const int kK1PrimePlusID = 20323 ;
static const int kK1PrimeMinusID = -20323 ;
static const int kK1Prime0ID = 20313 ;
static const int kK1Prime0BarID = -20313 ;
static const int kK2StarPlusID = 325 ;
static const int kK2StarMinusID = -325 ;
static const int kK2Star0ID = 315 ;
static const int kK2Star0BarID = -315 ;
static const int kEMinusID = 11 ;
static const int kEPlusID = -11 ;
static const int kMuMinusID = 13 ;
static const int kMuPlusID = -13 ;
static const int kNuEID = 12 ;
static const int kNuEBarID = -12 ;
static const int kNuMuID = 14 ;
static const int kNuMuBarID = -14 ;

//D0 decay modes
static const  int kFlavoredCF    = 0;
static const  int kFlavoredCFBar = 1;
static const  int kFlavoredCS    = 2;
static const  int kFlavoredCSBar = 3;
static const  int kSLPlus        = 4;
static const  int kSLMinus       = 5;
static const  int kCPPlus        = 6;
static const  int kCPMinus       = 7; 
static const  int kDalitz        = 8; 
static const  int kNDecayTypes   = 9; 



//Varibales to keep track of the Events
int  m_nUnknownEvents = 0;
int  m_nUnknownDecays= 0;
int  m_nD0D0barEvents= 0;
int  m_nD0bar = 0;
int  m_nDpDmEvents = 0;
int  m_nD0D0barDiscarded = 0;
int  m_nDpDmDiscarded = 0;
int  m_nCPPlus = 0;
int  m_nCPMinus = 0;
int  m_nFlavoredCFD0 = 0;
int  m_nFlavoredCSD0 = 0;
int  m_nFlavoredDCSD0 = 0;
int  m_nSL = 0;
int  m_nDalitz = 0;
double m_dalitzNumer1 = 0;
double m_dalitzNumer2 = 0;
double m_dalitzDenom = 0;
double dalitzNumer1_fil = 0;
double dalitzNumer2_fil = 0;
double dalitzDenom_fil = 0;
HepSymMatrix     m_weights(10,0);
double       m_rwsCF=0.;
double       m_rwsCS=0.;
double       m_deltaCF=0.;
double       m_deltaCS=0.;  
HepMatrix    m_modeCounter(10,10,0);
HepMatrix    m_keptModeCounter(10,10,0); 

int          m_rho_flag;


/////////////////////////////////////////////////////////////////

QCMCFilter::QCMCFilter(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator){
  //Declare the properties
  declareProperty("x",                 m_x=0.);
  declareProperty("y",                 m_y= 0.0); //For values outside of (-.11 , .06) measured y value may vary from input y
  declareProperty("rForCFModes",       m_rCF= 0.0621); //should be kept near this. based on MC for d0->kpi and d0bar->kpi 0.0621 = sqrt( .00015/.0389)
  declareProperty("zForCFModes",       m_zCF= 2.0);
  declareProperty("wSignForCFModes",   m_wCFSign= true);
  declareProperty("rForCSModes",       m_rCS= 1.0);
  declareProperty("zForCSModes",       m_zCS= -2.0);
  declareProperty("wSignForCSModes",   m_wCSSign= true);
  declareProperty("DplusDminusWeight", m_dplusDminusWeight= 0.5);
  declareProperty("dalitzModel",       m_dalitzModel= 0); // 0 CLEO-c, 1 Babar, 2 Belle
  declareProperty("UseNewWeights",     m_useNewWeights= true); 
  declareProperty("InvertSelection",   m_invertSelection= false); 
}

//***************************************************************

StatusCode QCMCFilter::initialize() {
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  
  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc;
  static const bool CREATEIFNOTTHERE(true);
  StatusCode PartPropStatus = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  if (!PartPropStatus.isSuccess() || 0 == p_PartPropSvc) 
    {
      log << MSG::ERROR << " Could not initialize Particle Properties Service" << endmsg;
      return PartPropStatus;
    }
  m_particleTable = p_PartPropSvc->PDT();


  //Calculates parameters here based on Parameter Input by User
  // (e.g. run expensive algorithms that are based on parameters
  //  specified by user at run-time)

  m_y = (m_y - 0.008)/0.292; 
  //corrects the y input so that we get the desired measurable y from CP:Semi-leptonic doubletag method
  // This equation is a result of solving equations for y dependence.
  //If parent MC Branching ratio's this equation will need resolved. January 2013 -DA
  double x = m_x ;
  double y = m_y ;
  double rCF = m_rCF ;
  double zCF = m_zCF ;
  double rCF2 = rCF * rCF ;
  double zCF2 = zCF * zCF ;
  double rCS = m_rCS ;
  double zCS = m_zCS ;
  double rCS2 = rCS * rCS ;
  double zCS2 = zCS * zCS ;
  
  double rmix = ( x * x + y * y ) / 2. ;
  double wCF = ( m_wCFSign ? 1. : -1. ) * sqrt( 4. - zCF2 ) ;
  double wCS = ( m_wCSSign ? 1. : -1. ) * sqrt( 4. - zCS2 ) ;
  double vCFCSPlus = ( zCF * zCS + wCF * wCS ) / 2. ;
  double vCFCSMinus = ( zCF * zCS - wCF * wCS ) / 2. ;
  
  m_deltaCF = acos( zCF / 2. ) * ( m_wCFSign ? 1. : -1. ) ;
  m_deltaCS = acos( zCS / 2. ) * ( m_wCSSign ? 1. : -1. ) ;

  double bCF = 1. + 0.5 * rCF * ( zCF * y + wCF * x ) + 0.5 * ( y * y - x * x );//second order y term added -DA
  double bCS = 1. + 0.5 * rCS * ( zCS * y + wCS * x ) + 0.5 * ( y * y - x * x );
  double bCPPlus = 1. - y ;
  double bCPMinus = 1. + y ;
  
  if( !m_useNewWeights )
    {
      bCF = 1. ;
      m_rwsCF = rCF2 ;
      bCS = 1. ;
      m_rwsCS = rCS2 ;
      bCPPlus = 1. ;
      bCPMinus = 1. ;
    }
  
  m_rwsCF = ( rCF2 + 0.5 * rCF * ( zCF * y - wCF * x )  + rmix ) / bCF;//though division by b not mentioned in memo it is correct
  m_rwsCS = ( rCS2 + 0.5 * rCS * ( zCS * y - wCS * x )  + rmix ) / bCS;

  
  // Calculate reweighting factors, normalized to the largest factor.
  // Note:A fake Dalitz weight is added in to just to make initial predictions
  // These Dalitz weights are not used in actual code 
  
  // FlavoredCF
  
  // First calculate rate factors.
  m_weights[ kFlavoredCF ][ kFlavoredCFBar ] =
      1. + rCF2 * ( 2. - zCF2 ) + rCF2 * rCF2 ;
  m_weights[ kFlavoredCF ][ kFlavoredCF ] =
    rmix * m_weights[ kFlavoredCF ][ kFlavoredCFBar ] ;
  m_weights[ kFlavoredCF ][ kFlavoredCSBar ] =
    1. + rCF2 * rCS2 - rCF * rCS * vCFCSMinus ;
  m_weights[ kFlavoredCF ][ kFlavoredCS ] =
    rCF2 + rCS2 - rCF * rCS * vCFCSPlus +
    rmix * m_weights[ kFlavoredCF ][ kFlavoredCSBar ] ;
  
  // Nov 2007: correct for r2 -> RWS and BR != A^2
  m_weights[ kFlavoredCF ][ kFlavoredCF ] /= m_rwsCF * bCF * bCF ;
  m_weights[ kFlavoredCF ][ kFlavoredCFBar ] /=
    ( 1. + m_rwsCF * m_rwsCF ) * bCF * bCF ;
  m_weights[ kFlavoredCF ][ kFlavoredCS ] /=
    ( m_rwsCF + m_rwsCS ) * bCF * bCS ;
  m_weights[ kFlavoredCF ][ kFlavoredCSBar ] /=
    ( 1. + m_rwsCF * m_rwsCS ) * bCF * bCS ;
  
  m_weights[ kFlavoredCF ][ kSLPlus ] = 1. / bCF ;
  m_weights[ kFlavoredCF ][ kSLMinus ] = 1. / bCF ;
  
  m_weights[ kFlavoredCF ][ kCPPlus ] =
    ( 1. + rCF2 + rCF * zCF ) / ( 1. + m_rwsCF ) / bCF / bCPPlus ;
  m_weights[ kFlavoredCF ][ kCPMinus ] =
    ( 1. + rCF2 - rCF * zCF ) / ( 1. + m_rwsCF ) / bCF / bCPMinus ;
  m_weights[ kFlavoredCF ][ kDalitz ] = 1. / bCF;

  // FlavoredCFBar
  m_weights[ kFlavoredCFBar ][ kFlavoredCFBar ] = m_weights[ kFlavoredCF ][ kFlavoredCF ] ;
  m_weights[ kFlavoredCFBar ][ kFlavoredCS ]    = m_weights[ kFlavoredCF ][ kFlavoredCSBar ] ;
  m_weights[ kFlavoredCFBar ][ kFlavoredCSBar ] = m_weights[ kFlavoredCF ][ kFlavoredCS ] ;
  m_weights[ kFlavoredCFBar ][ kSLPlus ]        = 1. / bCF ;
  m_weights[ kFlavoredCFBar ][ kSLMinus ]       = 1. / bCF ;
  m_weights[ kFlavoredCFBar ][ kCPPlus ]        = m_weights[ kFlavoredCF ][ kCPPlus ] ;
  m_weights[ kFlavoredCFBar ][ kCPMinus ]       = m_weights[ kFlavoredCF ][ kCPMinus ] ;
  m_weights[ kFlavoredCFBar ][ kDalitz ]        = 1. / bCF;

  // FlavoredCS
  
  // First calculate rate factors.
  m_weights[ kFlavoredCS ][ kFlavoredCSBar ] = 1. + rCS2 * ( 2. - zCS2 ) + rCS2 * rCS2 ;
  m_weights[ kFlavoredCS ][ kFlavoredCS ]    = rmix * m_weights[ kFlavoredCS ][ kFlavoredCSBar ] ;

  // Nov 2007: correct for r2 -> RWS and BR != A^2
  m_weights[ kFlavoredCS ][ kFlavoredCS ]    /= m_rwsCS * bCS * bCS ;
  m_weights[ kFlavoredCS ][ kFlavoredCSBar ] /= ( 1. + m_rwsCS * m_rwsCS ) * bCS * bCS ;
  
  m_weights[ kFlavoredCS ][ kSLPlus ]  = 1. / bCS ;
  m_weights[ kFlavoredCS ][ kSLMinus ] = 1. / bCS ;
  
  m_weights[ kFlavoredCS ][ kCPPlus ]  = ( 1. + rCS2 + rCS * zCS ) / ( 1. + m_rwsCS ) / bCS / bCPPlus ;
  m_weights[ kFlavoredCS ][ kCPMinus ] = ( 1. + rCS2 - rCS * zCS ) / ( 1. + m_rwsCS ) / bCS / bCPMinus ;
  
  m_weights[ kFlavoredCS ][ kDalitz ]  = 1. / bCS;
 
  // FlavoredCSBar
  
  m_weights[ kFlavoredCSBar ][ kFlavoredCSBar ] = m_weights[ kFlavoredCS ][ kFlavoredCS ] ;
  m_weights[ kFlavoredCSBar ][ kSLPlus ]        = 1. / bCS ;
  m_weights[ kFlavoredCSBar ][ kSLMinus ]       = 1. / bCS ;
  m_weights[ kFlavoredCSBar ][ kCPPlus ]        = m_weights[ kFlavoredCS ][ kCPPlus ] ;
  m_weights[ kFlavoredCSBar ][ kCPMinus ]       = m_weights[ kFlavoredCS ][ kCPMinus ] ;
  m_weights[ kFlavoredCSBar ][ kDalitz ]        = 1. / bCS;  

  // SLPlus
  
  // SLPlus/SLPlus should have rate factor = Rmix, but none of these are
  // generated in uncorrelated MC.
  m_weights[ kSLPlus ][ kSLPlus ]  = 0. ;
  m_weights[ kSLPlus ][ kSLMinus ] = 1. ;
  m_weights[ kSLPlus ][ kCPPlus ]  = 1. / bCPPlus ;
  m_weights[ kSLPlus ][ kCPMinus ] = 1. / bCPMinus ;
  m_weights[ kSLPlus ][ kDalitz ]  = 1. ;
  
  // SLMinus
  
  m_weights[ kSLMinus ][ kSLMinus ] = 0. ;
  m_weights[ kSLMinus ][ kCPPlus ] = 1. / bCPPlus ;
  m_weights[ kSLMinus ][ kCPMinus ] = 1. / bCPMinus ;
  m_weights[ kSLMinus ][ kDalitz ]  = 1. ;
  
  // CPPlus
  
  m_weights[ kCPPlus ][ kCPPlus ] = 0. ;
  m_weights[ kCPPlus ][ kCPMinus ] = 2. / bCPPlus / bCPMinus ;
  m_weights[ kCPPlus ][ kDalitz ]  = 1. / bCPPlus;

  // CPMinus
  
  m_weights[ kCPMinus ][ kCPMinus ] = 0. ;
  m_weights[ kCPMinus ][ kDalitz ]  = 1. / bCPMinus;

  //Dalitz estimate
  m_weights[ kDalitz ][ kDalitz ] = 1;

  log << MSG::FATAL << "Weight matrix" << m_weights <<  endmsg ;

  //Boss 6.6.2 MC Branching fractions 
  HepVector fractionsD0( kNDecayTypes+1, 0 ) ; 
  fractionsD0[ kFlavoredCF ]    = 0.531 ;  //.510(CLEO-c BR for comparison)
  fractionsD0[ kFlavoredCFBar ] = 0.0082 ; //.0103
  fractionsD0[ kFlavoredCS ]    = 0.031 ; //.0259
  fractionsD0[ kFlavoredCSBar ] = 0.031 ; //.0259 
  fractionsD0[ kSLPlus ]        = 0.125 ;  //.129
  fractionsD0[ kSLMinus ]       = 0. ;       
  fractionsD0[ kCPPlus ]        = 0.113 ;  //.123
  fractionsD0[ kCPMinus ]       = 0.102 ;  //.110
  fractionsD0[ kDalitz ]        = 0.0588 ; //.0578
  HepVector scales = m_weights * fractionsD0 ;
  log << MSG::INFO << "Integrated scale factors " <<scales << endmsg ;
  

  HepVector fractionsD0bar( kNDecayTypes+1, 0 ) ;
  fractionsD0bar[ kFlavoredCF ]    = 0.0082 ;
  fractionsD0bar[ kFlavoredCFBar ] = 0.531 ;
  fractionsD0bar[ kFlavoredCS ]    = 0.031 ;
  fractionsD0bar[ kFlavoredCSBar ] = 0.031 ;
  fractionsD0bar[ kSLPlus ]        = 0. ;
  fractionsD0bar[ kSLMinus ]       = 0.125 ;
  fractionsD0bar[ kCPPlus ]        = 0.113 ;
  fractionsD0bar[ kCPMinus ]       = 0.102 ;
  fractionsD0bar[ kDalitz ]        = 0.0588 ;
  HepVector weight_norm = fractionsD0bar.T() * scales;
  log << MSG::INFO << "Overall scale factor " << fractionsD0bar.T() * scales << endmsg ;
 


  //Original MC
  //Branching fraction predictions after weight
  double brCF = ( fractionsD0bar[ kFlavoredCFBar ] * scales[ kFlavoredCFBar ] ) / weight_norm[0] ;
  double brCS = ( fractionsD0bar[ kFlavoredCS ] * scales[ kFlavoredCS ] +  fractionsD0bar[ kFlavoredCSBar ] * scales[ kFlavoredCSBar ] ) / weight_norm[0] ;
  double brDCS = ( fractionsD0bar[ kFlavoredCF ] * scales[ kFlavoredCF ] ) / weight_norm[0] ;
  double brCPPlus = ( fractionsD0bar[ kCPPlus ] * scales[ kCPPlus ] ) / weight_norm[0] ;
  double brCPMinus = ( fractionsD0bar[ kCPMinus ] * scales[ kCPMinus ] ) / weight_norm[0] ;

  //y=0.0 dalitz example used for prediction
  double dalitz_y_num = -0.000177536;
  double dalitz_y_den = -0.0150846;

  double numFactCF =
    -rCF * zCF * ( 1. - 0.5 * rCF * wCF * m_x ) ; 
  double numFactCS =
    -rCS * zCS * ( 1. - 0.5 * rCS * wCS * m_x ) ; 
  double denFactCF = 0.5 * rCF2 * zCF2 ; 
  double denFactCS = 0.5 * rCS2 * zCS2 ; 

  double num_pre =
    brCPPlus - brCPMinus + brCF * numFactCF + 0.5 * brCS * numFactCS + dalitz_y_num; 
  double den_pre =
    1. - brCPPlus - brCPMinus - brCF * denFactCF - 0.5 * brCS * denFactCS + dalitz_y_den ; 
  double y_pre = num_pre / den_pre ;
  double num =
    fractionsD0bar[ kCPPlus ] - fractionsD0bar[ kCPMinus ] + fractionsD0bar[ kFlavoredCFBar ] * numFactCF + fractionsD0bar[ kFlavoredCS ] * numFactCS + dalitz_y_num; 
  double den =
    1. - fractionsD0bar[ kCPPlus ] - fractionsD0bar[ kCPMinus ] - fractionsD0bar[ kFlavoredCFBar ] * denFactCF - fractionsD0bar[ kFlavoredCS ] * denFactCS + dalitz_y_den; 
  double y_prematrix = num / den ;
  double y_test1 = 0.25 * ( ( ( scales[ kCPMinus ] * m_weights[ kCPPlus ][ kSLPlus ] ) / ( scales[ kCPPlus ] * m_weights[ kCPMinus ][ kSLPlus ] ) ) -
			   ( ( scales[ kCPPlus ] * m_weights[ kCPMinus ][ kSLPlus ] ) / ( scales[ kCPMinus ] * m_weights[ kCPPlus ][ kSLPlus ] ) ) ) ;

  log << MSG::INFO << "An Input  Y of " << m_y << endmsg ;
  log << MSG::INFO << "Parent MC has an intrinsic value of y as " << y_prematrix << endmsg ;
  log << MSG::INFO << "The BR method predics a y of " << y_pre << endmsg ;
  log << MSG::INFO << "The CP-SL double tag method predicts y of " <<y_test1 << endmsg ; 

  // Renormalize
  m_largestWeight = 0. ;
  for( int i = 0 ; i < kNDecayTypes ; ++i )
    {
      for( int j = 0 ; j < kNDecayTypes ; ++j )
	{
	  if( m_weights[ i ][ j ] < 0 ) m_weights[ i ][ j ] = 0; //gets rid of negative weights in early calculations
	  if( m_weights[ i ][ j ] > m_largestWeight )
	    {
	      m_largestWeight = m_weights[ i ][ j ] ;
	    }
	}
    }
  m_weights /= m_largestWeight ;
  
  log << MSG::INFO <<"Renormalized weight matrix " << m_weights << endmsg ;


  //Set up weight parameters
  Dalitz t(8);
  double D0Sum[ 10 ], D0barSum[ 10 ] ; // last index is for sum over bins
  TComplex D0D0barSum[ 10 ] ;
  int points[ 10 ] ;
  
  for( int i = 0 ; i < 10 ; ++i )
    {
      D0Sum[ i ] = 0. ;
      D0barSum[ i ] = 0. ;
      D0D0barSum[ i ] = TComplex( 0., 0. ) ;
      points[ i ] = 0 ;
    }
  
  double m2min = 0. ;
  double m2max = 3. ;
  int nsteps = 1000 ;
  double stepsize = ( m2max - m2min ) / ( double ) nsteps ;
  
  for( int i = 0 ; i < nsteps ; ++i )
    {
      double m2i = m2min + ( ( double ) i + 0.5 ) * stepsize ;
      
      for( int j = 0 ; j < nsteps ; ++j ) 
	{
	  double m2j = m2min + ( ( double ) j + 0.5 ) * stepsize ;
	  
	  if( t.Point_on_DP( m2i, m2j ) )
	    {
	      double m2k = 1.86450*1.86450 + 0.497648*0.497648 +
		0.139570*0.139570 + 0.139570*0.139570 - m2i - m2j ;
	      
	      TComplex D0, D0bar;
	      if (m_dalitzModel == 0) { //Cleo model
		D0 = t.CLEO_Amplitude( m2i, m2j, m2k ) ;
	        D0bar = t.CLEO_Amplitude( m2j, m2i, m2k ) ;}
	      if (m_dalitzModel == 1) { //Babar model
		D0 = t.Babar_Amplitude( m2i, m2j, m2k ) ;
		D0bar = t.Babar_Amplitude( m2j, m2i, m2k ) ;}
	      if (m_dalitzModel == 2) { //Belle model
		D0 = t.Amplitude( m2i, m2j, m2k ) ;
		D0bar = t.Amplitude( m2j, m2i, m2k ) ;}
	      
	      if ( j <= i ) {// lower half only
		int bin = t.getBin( m2i, m2j, m2k ) ;
		if( bin == -1 ) bin = 8 ;
		
		++points[ bin ] ;
		D0Sum[ bin ] += norm( D0 ) ;
		D0barSum[ bin ] += norm( D0bar ) ;
		D0D0barSum[ bin ] += D0 * conj( D0bar ) ;
		
		++points[ 9 ] ;
		D0Sum[ 9 ] += norm( D0 ) ;
		D0barSum[ 9 ] += norm( D0bar ) ;
		D0D0barSum[ 9 ] += D0 * conj( D0bar ) ;
	      }
		
	    }
	}
    }

  for( int i = 0 ; i < 10 ; ++i )
    {
      double r2 = D0barSum[ i ] / D0Sum[ i ] ;
      double c = real( D0D0barSum[ i ] ) / sqrt( D0barSum[ i ] ) / sqrt( D0Sum[ i ] ) ;
      double s = imag( D0D0barSum[ i ] ) / sqrt( D0barSum[ i ] ) / sqrt( D0Sum[ i ] ) ;
      
      cout << "BIN " << i << ": "
           << points[ i ] << " pts "
           << r2 << " " << c << " " << s << " " << c*c+s*s
           << endmsg ;
    }
  
  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;
}  

//********************************************************************
StatusCode QCMCFilter::execute() {
  
  //interface to event data service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc=svcLocator->service("EventDataSvc", m_evtSvc);
  if (sc.isFailure())
    std::cout<<"Could not access EventDataSvc!"<<std::endl;

  setFilterPassed(false); 

  MsgStream log(msgSvc(), name());
  log<< MSG::INFO << "in execute()" << endmsg;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");

  if(!eventHeader)
    {
      cout<<" eventHeader "<<endl;
      return StatusCode::FAILURE;
    } 
  long runNo = eventHeader->runNumber();
  long event = eventHeader->eventNumber();
  log << MSG::DEBUG << "run, evtnum = "
      << runNo << " , "
      << event << endmsg;

  // Get McParticle collection
  SmartDataPtr<Event::McParticleCol> mcParticles(eventSvc(), EventModel::MC::McParticleCol);

  //Check if event is Psi(3770)
  int psipp_daughter = 0;
  int d0_count = 0;
  int chd_count = 0;
  for( Event::McParticleCol::iterator it = mcParticles->begin();  it != mcParticles->end(); it++) 
    {     
      int pdg_code = (*it)->particleProperty();
      if ((*it)->primaryParticle()) continue;
      Event::McParticle  it2 =  (*it)->mother();
      int mother_pdg = 0;
      //finds if the particle is daughter of Psi(3770)
      mother_pdg = it2.particleProperty();
      if (mother_pdg == kPsi3770ID) 
	{
	  psipp_daughter++; //Found daughter
	  if (abs(pdg_code) == kD0ID) d0_count++; 
	  if (abs(pdg_code) == kDpID) chd_count++;
	  if (pdg_code == kD0BarID)   m_nD0bar++;  
	}
    }

  if( psipp_daughter == 0 ) //didn't find Psi(3770)
   {
      ++m_nUnknownEvents ;

      if( m_invertSelection )
	{
	  log<< MSG::DEBUG << "Discarding event -- did not find psi(3770). " << endmsg ;
	  return StatusCode::SUCCESS; //discard event
	}
      else
	{
	  // -------- Write to root file
	  setFilterPassed(true); 
	  
	  return StatusCode::SUCCESS; //kept event
	}
   }
  
   log<< MSG::DEBUG << "Found psi(3770) -->" << endmsg ;

   // Check that top particle has only two daughters + possible FSR photon
   if ( psipp_daughter > 3 )
     {
       ++m_nUnknownEvents ;

      if( m_invertSelection )
	{
	  log<< MSG::DEBUG << "Discarding event -- psi(3770) has >3 daughters." << endmsg ;
	  return StatusCode::SUCCESS; //discard event
	}
      else
	{
	  // -------- Write to root file
	  setFilterPassed(true);
	   
	  
	  return StatusCode::SUCCESS; //kept event
	}
     }

   //Check if D+D- state
   if (chd_count == 2) {     // D+D- event
     ++m_nDpDmEvents ;
     
     // D+D- must be rewieghted otherwise too many D+D- events relative to D0D0bar.
     double random = RandFlat::shoot() ;
     
     if( ( random < m_dplusDminusWeight && !m_invertSelection ) ||
	 ( random > m_dplusDminusWeight &&  m_invertSelection ) )
       {
	 // -------- Write to root file
	 setFilterPassed(true);
	  
	 
	 return StatusCode::SUCCESS; //event kept
       }
     else
       {
	 log<< MSG::DEBUG << "Discarding event -- D+D- event failed reweighting." << endmsg ;
	 ++m_nDpDmDiscarded ;
	 return StatusCode::SUCCESS; //event discarded
       }
   }

   //Check if D0D0Bar
   if (d0_count != 2) { 
     if( !m_invertSelection )
       {
	 log<< MSG::DEBUG << "Discarding event -- non-D0-D0bar event." << endmsg ;
	 return StatusCode::SUCCESS; //discard event
       }
     else
       {
	 // -------- Write to root file
	 setFilterPassed(true);
	  
	 
	 return StatusCode::SUCCESS; //kept event
       }
   }
   
   log<< MSG::INFO << "D0-D0bar event." << endmsg ;
   m_nD0D0barEvents++ ;

   m_rho_flag = 0;
   
   //Get D0-D0bar modes and info
   vector<double> d0_decay = findD0Decay(1);  
   vector<double> d0bar_decay = findD0Decay(-1);

   int d0mode = d0_decay[0];
   int d0barmode = d0bar_decay[0];
   m_modeCounter[d0mode][d0barmode]++;



   //if any event unidentified remove
   if (d0_decay[0] == 9 || d0bar_decay[0] == 9 ) { 
     if( !m_invertSelection )
       {
	 log<< MSG::DEBUG << "Discarding event -- unknown D0-D0bar event." << endmsg ;
	 return StatusCode::SUCCESS; //discard event
       }
     else
       {
	 // -------- Write to root file
	 setFilterPassed(true);
	  
	 
	 
	 return StatusCode::SUCCESS; //kept event
       }
   }
   
   // Loop over D's
   double r2D0 = d0_decay[1] ;
   double deltaD0 = d0_decay[2] ;
   double r2D0bar = d0bar_decay[1] ;
   double deltaD0bar = d0bar_decay[2] ;
   
   
   //Weight based on DDbar combination
   double weight ;
   if( d0_decay[0] == kDalitz || d0bar_decay[0] == kDalitz )
     {
       double r2prod = r2D0 * r2D0bar ;
       double x = m_x ;
       double y = m_y ;
	  
       double bD0 = 1. + sqrt( r2D0 ) * ( cos( deltaD0 ) * y + sin( deltaD0 ) * x ) ;
       double rwsD0 = ( r2D0 + sqrt( r2D0 ) * ( cos( deltaD0 ) * y - sin( deltaD0 ) * x ) ) / bD0 ;
       double bD0bar = 1. + sqrt( r2D0bar ) * ( cos( deltaD0bar ) * y + sin( deltaD0bar ) * x ) ;
       double rwsD0bar = ( r2D0bar + sqrt( r2D0bar ) * ( cos( deltaD0bar ) * y - sin( deltaD0bar ) * x ) ) / bD0bar ;
	  
       weight = 1. + r2prod - 2. * sqrt( r2prod ) * cos( deltaD0 + deltaD0bar ) ;
       weight /= ( 1. + rwsD0 * rwsD0bar ) * bD0 * bD0bar ;
       weight /= m_largestWeight ;

       //get dalitz graph information before cut
       int k = -10;
       double m2i= 0;
       double m2j= 0;
       double m2k= 0;
       if( d0_decay[0] == kDalitz) {
	 k = d0bar_decay[0];
	 m2i= d0_decay[3];
	 m2j= d0_decay[4];
         m2k= d0_decay[6];}
       if( d0bar_decay[0] == kDalitz) {
	 k = d0_decay[0];
	 m2i= d0bar_decay[3];
	 m2j= d0bar_decay[4];
	 m2k= d0bar_decay[6];} 

     }
   else
     {
       weight = m_weights[ d0_decay[0] ][ d0bar_decay[0] ] ;
     }
   
   double random = RandFlat::shoot() ;
   log<< MSG::DEBUG << "type:  " << d0_decay[0] << " " << d0bar_decay[0] << ", weight " <<
     weight << " <? random " << random << endmsg ;
   
   if( ( random < weight && !m_invertSelection ) || ( random > weight && m_invertSelection ) )
     {
       // -------- Write to root file
       setFilterPassed(true);
       
       //get dalitz graph information after filter
       int k = -10;
       double m2i= 0;
       double m2j= 0;
       double m2k= 0;
       double cosd = cos( deltaD0 ) ;
       double sind = sin( deltaD0 ) ;
       if( d0_decay[0] == kDalitz) {
	 k = d0bar_decay[0];
	 m2i= d0_decay[3];
	 m2j= d0_decay[4];
	 m2k= d0_decay[6];
	 if ( m2j - m2i > 0. ) {  //avoids doublecounting the branching fraction
	   dalitzNumer1_fil += -2. * sqrt( r2D0 ) * cosd ;
	   dalitzNumer2_fil += 2. * r2D0 * cosd * sind * m_x ;
	   dalitzDenom_fil += -2. * r2D0 * cosd * cosd ; }
       }
       if( d0bar_decay[0] == kDalitz) {
	 k = d0_decay[0];
	 m2i= d0bar_decay[3];
	 m2j= d0bar_decay[4];
	 m2k= d0bar_decay[6]; 
	 cosd = cos( deltaD0bar ) ;
	 sind = sin( deltaD0bar ) ;
	 if( m2j - m2i < 0. ) { //avoids doublecounting the branching fraction
	 dalitzNumer1_fil += -2. * sqrt( r2D0bar ) * cosd ;
	 dalitzNumer2_fil += 2. * r2D0bar * cosd * sind * m_x ;
	 dalitzDenom_fil += -2. * r2D0bar * cosd * cosd ; }
       } 
 
       m_keptModeCounter[d0mode][d0barmode]++;
       return StatusCode::SUCCESS; //event kept
     }
   else
     {
       log << MSG::DEBUG << "Discarding event -- failed reweighting." << endmsg ;
       
       ++m_nD0D0barDiscarded ;
       return StatusCode::SUCCESS; //event discarded
     }
}


//*********************************************************************
StatusCode QCMCFilter::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  log << MSG::INFO << "Number of unknown events: "  << m_nUnknownEvents << endmsg ;
  log << MSG::INFO << "Number of D+D- events seen: " << m_nDpDmEvents << endmsg ;
  log << MSG::INFO << "Number of D+D- events discarded: "  << m_nDpDmDiscarded << endmsg ;
  if( m_nDpDmEvents > 0 )
    {
      log << MSG::INFO <<"Fraction discarded: " << double( m_nDpDmDiscarded ) / double( m_nDpDmEvents ) << endmsg ;
    }

  log << MSG::INFO << "Number of D0D0bar events seen: " << m_nD0D0barEvents << " " << m_nD0bar << endmsg ;
  log << MSG::INFO << "Number of D0D0bar events discarded: " << m_nD0D0barDiscarded << endmsg ;
  if( m_nD0D0barEvents > 0 &&  m_nCPPlus > 0 && m_nCPMinus > 0)
    {
      log << MSG::INFO << "Fraction discarded: " << double( m_nD0D0barDiscarded ) / double( m_nD0D0barEvents ) << endl <<
	"Fraction unknown decays: " <<	0.5 * double( m_nUnknownDecays ) / double( m_nD0D0barEvents ) <<endmsg ;
      
      double nD0 = 2. * m_nD0D0barEvents ;
      double brSL = m_nSL / nD0 ;
      double dBrSL = sqrt( brSL * ( 1. - brSL ) / nD0 ) ;
      double brCF = m_nFlavoredCFD0 / nD0 ;
      double dBrCF = sqrt( brCF * ( 1. - brCF ) / nD0 ) ;
      double brCS = m_nFlavoredCSD0 / nD0 ;
      double dBrCS = sqrt( brCS * ( 1. - brCS ) / nD0 ) ;
      double brDCS = m_nFlavoredDCSD0 / nD0 ;
      double dBrDCS = sqrt( brDCS * ( 1. - brDCS ) / nD0 ) ;
      double brCPPlus = m_nCPPlus / nD0 ;
      double dBrCPPlus = sqrt( brCPPlus * ( 1. - brCPPlus ) / nD0 ) ;
      double brCPMinus = m_nCPMinus / nD0 ;
      double dBrCPMinus = sqrt( brCPMinus * ( 1. - brCPMinus ) / nD0 ) ;
      double brDalitz = m_nDalitz / nD0 ;
      double dBrDalitz = sqrt( brDalitz * ( 1. - brDalitz ) / nD0 ) ;
      double fdBrDalitz = dBrDalitz / brDalitz ;
      double dalitzNumer1Norm = m_dalitzNumer1 / nD0 ;
      double dDalitzNumer1Norm = dalitzNumer1Norm * fdBrDalitz ;
      double dalitzNumer2Norm = m_dalitzNumer2 / nD0 ;
      double dDalitzNumer2Norm = dalitzNumer2Norm * fdBrDalitz ;
      double dalitzDenomNorm = m_dalitzDenom / nD0 ;
      double dDalitzDenomNorm = dalitzDenomNorm * fdBrDalitz ;

      //after the filter was applied
      double fil_SL = 0, fil_FlavoredCFD0 = 0, fil_FlavoredCSD0 = 0, fil_FlavoredDCSD0 =0, fil_CPPlus = 0, fil_CPMinus = 0, fil_Dalitz = 0;
      for( int i = 0 ; i < 9 ; ++i )
	{
	  fil_SL += m_keptModeCounter[i][4] + m_keptModeCounter[4][i] + m_keptModeCounter[i][5] + m_keptModeCounter[5][i] ;
	  fil_FlavoredCFD0 += m_keptModeCounter[i][1] + m_keptModeCounter[0][i];
	  fil_FlavoredCSD0 += m_keptModeCounter[i][2] + m_keptModeCounter[2][i] + m_keptModeCounter[i][3] + m_keptModeCounter[3][i];
	  fil_FlavoredDCSD0 += m_keptModeCounter[i][0] + m_keptModeCounter[1][i];
	  fil_CPPlus += m_keptModeCounter[i][6] + m_keptModeCounter[6][i];
	  fil_CPMinus += m_keptModeCounter[i][7] + m_keptModeCounter[7][i];
	  fil_Dalitz += m_keptModeCounter[i][8] + m_keptModeCounter[8][i];
	}
      double nD0_fil = 2. * ( m_nD0D0barEvents -  m_nD0D0barDiscarded ) ;
      double brSL_fil = fil_SL / nD0_fil ;
      double dBrSL_fil = sqrt( brSL_fil * ( 1. - brSL_fil ) / nD0_fil ) ;
      double brCF_fil = fil_FlavoredCFD0 / nD0_fil ;
      double dBrCF_fil = sqrt( brCF_fil * ( 1. - brCF_fil ) / nD0_fil ) ;
      double brCS_fil = fil_FlavoredCSD0 / nD0_fil ;
      double dBrCS_fil = sqrt( brCS_fil * ( 1. - brCS_fil ) / nD0_fil ) ;
      double brDCS_fil = fil_FlavoredDCSD0 / nD0_fil ;
      double dBrDCS_fil = sqrt( brDCS_fil * ( 1. - brDCS_fil ) / nD0_fil ) ;
      double brCPPlus_fil = fil_CPPlus / nD0_fil ;
      double dBrCPPlus_fil = sqrt( brCPPlus_fil * ( 1. - brCPPlus_fil ) / nD0_fil ) ;
      double brCPMinus_fil = fil_CPMinus / nD0_fil ;
      double dBrCPMinus_fil = sqrt( brCPMinus_fil * ( 1. - brCPMinus_fil ) / nD0_fil ) ;
      double brDalitz_fil = fil_Dalitz / nD0_fil ;
      double dBrDalitz_fil = sqrt( brDalitz_fil * ( 1. - brDalitz_fil ) / nD0_fil ) ;
      double fdBrDalitz_fil = dBrDalitz_fil / brDalitz_fil ;
      double dalitzNumer1Norm_fil = dalitzNumer1_fil / nD0_fil ;
      double dDalitzNumer1Norm_fil = dalitzNumer1Norm_fil * fdBrDalitz_fil ;
      double dalitzNumer2Norm_fil = dalitzNumer2_fil / nD0_fil ;
      double dDalitzNumer2Norm_fil = dalitzNumer2Norm_fil * fdBrDalitz_fil ;
      double dalitzDenomNorm_fil = dalitzDenom_fil / nD0_fil ;
      double dDalitzDenomNorm_fil = dalitzDenomNorm_fil * fdBrDalitz_fil ;


      log << MSG::INFO <<
	"Original number of SL decays: " << m_nSL <<"  ==> Br(SL) = " << brSL << " +- " << dBrSL << endl <<
	"Filtered number of SL decays: " << fil_SL <<"  ==> Br(SL) = " << brSL_fil << " +- " << dBrSL_fil << endl <<
	"Original number of D0->CF/CS/DCS or D0bar->CFbar/CSBar/DCSbar: " <<m_nFlavoredCFD0 << "/" << m_nFlavoredCSD0 << "/" << m_nFlavoredDCSD0 << endl <<
	"  ==> Br(CF) = " << brCF << " +- " << dBrCF << endl <<
	"  ==> Br(CS) = " << brCS << " +- " << dBrCS << endl <<
	"  ==> Br(DCS) = " << brDCS << " +- " << dBrDCS << endl <<
	"Filtered number of D0->CF/CS/DCS or D0bar->CFbar/CSBar/DCSbar: " <<fil_FlavoredCFD0 << "/" << fil_FlavoredCSD0 << "/" << fil_FlavoredDCSD0 << endl <<
	"  ==> Br(CF) = " << brCF_fil << " +- " << dBrCF_fil << endl <<
	"  ==> Br(CS) = " << brCS_fil << " +- " << dBrCS_fil << endl <<
	"  ==> Br(DCS) = " << brDCS_fil << " +- " << dBrDCS_fil << endl <<
	
	"Original number of CP+/-: " << m_nCPPlus << "/" << m_nCPMinus <<endl << 
	"  ==> Br(CP+) = " << brCPPlus << " +- " << dBrCPPlus <<endl << 
	"  ==> Br(CP-) = " << brCPMinus << " +- " << dBrCPMinus << endl << 
	"Filtered number of CP+/-: " << fil_CPPlus << "/" << fil_CPMinus <<endl << 
	"  ==> Br(CP+) = " << brCPPlus_fil << " +- " << dBrCPPlus_fil <<endl << 
	"  ==> Br(CP-) = " << brCPMinus_fil << " +- " << dBrCPMinus_fil << endl << 

	"Original number of Dalitz: " << m_nDalitz << endl << 
	"  ==> Br(Dalitz) = " << brDalitz << " +- " << dBrDalitz <<endl <<
	"  y contrib. numer1 " << dalitzNumer1Norm << " +- " << dDalitzNumer1Norm << endl <<
	"  numer2 " << dalitzNumer2Norm << " +- " << dDalitzNumer2Norm << endl <<
	"  denom " << dalitzDenomNorm << " +- " << dDalitzDenomNorm << endmsg <<
	"Filtered number of Dalitz: " << fil_Dalitz << endl << 
	"  ==> Br(Dalitz) = " << brDalitz_fil << " +- " << dBrDalitz_fil <<endl <<
	"  y contrib. numer1 " << dalitzNumer1Norm_fil << " +- " << dDalitzNumer1Norm_fil << endl <<
	"  numer2 " << dalitzNumer2Norm_fil << " +- " << dDalitzNumer2Norm_fil << endl <<
	"  denom " << dalitzDenomNorm_fil << " +- " << dDalitzDenomNorm_fil << endmsg ;

      cout<<""<<endl;

      HepMatrix    differencetoWeight(10,10,0);
      for( int i = 0 ; i < 9 ; ++i )  { 
	for( int j = 0 ; j < 9 ; ++j )  {
	  if (m_modeCounter[i][j] != 0) {
	    differencetoWeight[i][j] = m_keptModeCounter[i][j] / m_modeCounter[i][j] - m_weights[i][j]; } } }

      log << MSG::INFO << "Weight matrix" << m_weights <<  endmsg ;
      log << MSG::INFO << "D0 Modes before filter" << m_modeCounter <<  endmsg ;
      log << MSG::INFO << "D0 Modes after filter" << m_keptModeCounter <<  endmsg ;
      log << MSG::INFO << "Compare percentage difference to weights " << differencetoWeight <<  endmsg ;

      //Calculating and comparing Y before/after the filter
      // To calculate y, we want half the CS BF
      brCS /= 2. ;
      dBrCS /= 2. ;
      brCS_fil /= 2 ;
      dBrCS_fil /= 2 ;

      double y, dy, y_fil, dy_fil ;
      double y_cpsl, dy_cpsl, y_fil_cpsl, dy_fil_cpsl ;
      
      //Original MC
      double rCF = m_rCF ;
      double zCF = m_zCF ;
      double rCF2 = rCF * rCF ;
      double zCF2 = zCF * zCF ;
      double rCS = m_rCS ;
      double zCS = m_zCS ;
      double rCS2 = rCS * rCS ;
      double zCS2 = zCS * zCS ;
      
      double wCF = ( m_wCFSign ? 1. : -1. ) * sqrt( 4. - zCF2 ) ;
      double wCS = ( m_wCSSign ? 1. : -1. ) * sqrt( 4. - zCS2 ) ;
      
      double numFactCF =
	-rCF * zCF * ( 1. - 0.5 * rCF * wCF * m_x ) ; 
      double numFactCS =
	-rCS * zCS * ( 1. - 0.5 * rCS * wCS * m_x ) ; 
      double denFactCF = 0.5 * rCF2 * zCF2 ;  
      double denFactCS = 0.5 * rCS2 * zCS2 ;  
      double dalitzNumerNorm = dalitzNumer1Norm + dalitzNumer2Norm ; 
      
      double num =
	brCPPlus - brCPMinus + brCF * numFactCF + brCS * numFactCS +
	dalitzNumerNorm ; 
      double den =
	1. - brCPPlus - brCPMinus - brCF * denFactCF - brCS * denFactCS +
	dalitzDenomNorm ; 
      y = num / den ;
      dy = sqrt(
		( num + den ) * ( num + den ) * dBrCPPlus * dBrCPPlus +
		( num - den ) * ( num - den ) * dBrCPMinus * dBrCPMinus +
		( numFactCF * den + denFactCF * num ) * dBrCF *
		( numFactCF * den + denFactCF * num ) * dBrCF +
		( numFactCS * den + denFactCS * num ) * dBrCS *
		( numFactCS * den + denFactCS * num ) * dBrCS +
		( dalitzNumerNorm * den + dalitzDenomNorm * num ) * fdBrDalitz *
		( dalitzNumerNorm * den + dalitzDenomNorm * num ) * fdBrDalitz
		) / ( den * den ) ;
      
      double n_cpplussl = m_modeCounter[6][4] + m_modeCounter[6][5] + m_modeCounter[4][6] +  m_modeCounter[5][6] ;
      double n_cpminussl = m_modeCounter[7][4] + m_modeCounter[7][5] + m_modeCounter[4][7] +  m_modeCounter[5][7] ;	
      double a_cpsl = ( n_cpplussl  * m_nCPMinus ) / ( n_cpminussl * m_nCPPlus  ) ;
      double b_cpsl = ( n_cpminussl  * m_nCPPlus ) / ( n_cpplussl * m_nCPMinus  ) ;
      y_cpsl = 0.25 * ( a_cpsl  - b_cpsl ) ;
      dy_cpsl = 0.25 * ( a_cpsl + b_cpsl )  * sqrt( ( 1/n_cpplussl ) + ( 1/n_cpminussl ) + ( 1/m_nCPPlus ) + ( 1/m_nCPMinus ) ) ; 
      
      
      //Filtered MC
      double dalitzNumerNorm_fil = dalitzNumer1Norm_fil + dalitzNumer2Norm_fil ;
      double num_fil =
	brCPPlus_fil - brCPMinus_fil + brCF_fil * numFactCF + brCS_fil * numFactCS +
	dalitzNumerNorm_fil ;
      double den_fil =
	1. - brCPPlus_fil - brCPMinus_fil - brCF_fil * denFactCF - brCS_fil * denFactCS +
	dalitzDenomNorm_fil ;  
      y_fil = num_fil / den_fil ;
      dy_fil = sqrt(
		    ( num_fil + den_fil ) * ( num_fil + den_fil ) * dBrCPPlus_fil * dBrCPPlus_fil +
		    ( num_fil - den_fil ) * ( num_fil - den_fil ) * dBrCPMinus_fil * dBrCPMinus_fil +
		    ( numFactCF * den_fil + denFactCF * num_fil ) * dBrCF_fil *
		    ( numFactCF * den_fil + denFactCF * num_fil ) * dBrCF_fil +
		    ( numFactCS * den_fil + denFactCS * num_fil ) * dBrCS_fil *
		    ( numFactCS * den_fil + denFactCS * num_fil ) * dBrCS_fil +
		    ( dalitzNumerNorm_fil * den_fil + dalitzDenomNorm_fil * num_fil ) * fdBrDalitz_fil *
		    ( dalitzNumerNorm_fil * den_fil + dalitzDenomNorm_fil * num_fil ) * fdBrDalitz_fil
		    ) / ( den_fil * den_fil ) ;
      
      double fil_cpplussl = m_keptModeCounter[6][4] + m_keptModeCounter[6][5] + m_keptModeCounter[4][6] +  m_keptModeCounter[5][6] ;
      double fil_cpminussl = m_keptModeCounter[7][4] + m_keptModeCounter[7][5] + m_keptModeCounter[4][7] +  m_keptModeCounter[5][7] ;	
      a_cpsl = ( fil_cpplussl  * fil_CPMinus ) / ( fil_cpminussl * fil_CPPlus  ) ;
      b_cpsl = ( fil_cpminussl  * fil_CPPlus ) / ( fil_cpplussl * fil_CPMinus  ) ;
      y_fil_cpsl = 0.25 * ( a_cpsl  - b_cpsl ) ;
      dy_fil_cpsl = 0.25 * ( a_cpsl + b_cpsl ) * sqrt( ( 1/n_cpplussl ) + ( 1/n_cpminussl ) + ( 1/fil_CPPlus ) + ( 1/fil_CPMinus ) ) ; 
      
      
      
      log << MSG::INFO <<"BR Method : Parent MC    ==> y = " << y << " +- " << dy << endmsg ;
      log << MSG::INFO <<"BR Method : Filtered MC  ==> y = " << y_fil << " +- " << dy_fil << endmsg ;
      log << MSG::INFO <<"CP-SL doubletag : Parent MC   ==> y = " << y_cpsl << " +- " << dy_cpsl <<endl<<"Previous line should be equivalent with 0 as parent MC not corrilated"<< endmsg ;
      log << MSG::INFO <<"CP-SL doubletag : Filtered MC ==> y = " << y_fil_cpsl << " +- " << dy_fil_cpsl << endmsg ;
      
    }

  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
vector<double> QCMCFilter::findD0Decay(int charm)
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO <<"  In findD0Decay " << endmsg ;
  
  vector<double> d0_info(7);
  for(int i=0; i< 7; i++)  d0_info[i]=0;
  double decay_mode = 9; //d0_info[0] = decay mode;
  double r2D0 = -1;       //d0_info[1] = r2D0;
  double deltaD0 = -1;    //d0_info[2] = deltaD0;
  
  double num[26];
  for(int i=0; i< 26; i++)  num[i]=0;
  // Number of each partile type
  int kPiPlus        =  0;// num[0] = Pi+, a0+ 
  int kPiMinus       =  1;// num[1] = Pi-, a0-  
  int kPi0           =  2;// num[2] = Pi0  
  int kEta           =  3;// num[3] = Eta  
  int kEtaPrime      =  4;// num[4] = Eta Prime  
  int kNeutralScalar =  5;// num[5] = Neutral Scalar : f0, f'0, f0(1500), a0,  (f_2 can be included here because it is only used in f_2 pi0 decay)
  int kUFVPlus       =  6;// num[6] = unflavored positive (axial) vectors: rho+, a1+, rho(2S)+  
  int kUFVMinus      =  7;// num[7] = unflavored negative (axial) vectors: rho-, a1-, rho(2S)-  
  int kRho0          =  8;// num[8] = Rho0, Rho(2S)0   
  int kOmega         =  9;// num[9] = Omega  
  int kPhi           = 10;// num[10] = Phi
  int kKPlus         = 11;// num[11] = K+  
  int kKMinus        = 12;// num[12] = K-  
  int kK0S           = 13;// num[13] = K short  
  int kK0L           = 14;// num[14] = K long  
  int kFVPlus        = 15;// num[15] = flavored positive (axial) vectors: K*+, K_1+, K_2*+
  int kFVMinus       = 16;// num[16] = flavored negative (axial) vectors: K*-, K_1-, K_2*- 
  int kKStar0        = 17;// num[17] = K*0 
  int kKStar0Bar     = 18;// num[18] = anti-K*0 
  int kK10           = 19;// num[19] = K1_0
  int kK10Bar        = 20;// num[20] = anti-K1_0
  int kLeptonPlus    = 21;// num[21] =  LeptonPlus: e+, mu+ 
  int kLeptonMinus   = 22;// num[22] =  LeptonMinus: e-, mu- 
  int kNeutrino      = 23;// num[23] =  Neutrino: nu, nubar 
  int kGamma         = 24;// num[24] =  gamma 
  int kUnknown       = 25;// num[25] =  Not identified particle   
  //int kGammaFSR      GammaFSR is ignored as we are interested in the PreFSR quantum # 

  // Get McParticle collection
  SmartDataPtr<Event::McParticleCol> mcParticles(eventSvc(), EventModel::MC::McParticleCol);
  int d0_pdg = charm == 1 ? kD0ID : kD0BarID;

  HepLorentzVector piPlus, piMinus, k0;

  for( Event::McParticleCol::iterator it = mcParticles->begin();  it != mcParticles->end(); it++) 
    {     
      int pdg_code = (*it)->particleProperty();
      if ((*it)->primaryParticle()) continue;
      Event::McParticle  it2 =  (*it)->mother();
      int mother_pdg = 0;

      //finds if the particle is related to the d0 we need
      mother_pdg = it2.particleProperty();

      //special cases
      if (pdg_code == kKStar0ID || pdg_code == kKStar0BarID || pdg_code == kK0ID || pdg_code == kK0BarID 
	  || pdg_code == kK10ID || pdg_code == kK10BarID 
	  || pdg_code == kK0Star0ID || pdg_code == kK0Star0BarID || pdg_code == kK0StarPlusID || pdg_code == kK0BarID ) continue;  //don't count special intermediate states

      if (mother_pdg == kK0ID || mother_pdg == kK0BarID) {//Looks another step up if mother was k0 or k0bar 
	it2 = it2.mother();
	mother_pdg = it2.particleProperty(); }

      if (mother_pdg == kKStar0ID || mother_pdg == kKStar0BarID) {//Looks another step up if mother was k*0 or k*0bar
	//if code is found to be K*0 -> K+Pi- we save  it as a KStar0 and K*0Bar -> K-Pi+ as KStar0Bar
	//if is it a neutral decay K*0(Bar)->K0(bar) Pi0 or Gamma.  We save them as K0(bar) and Pi0 or Gamma.
	if (pdg_code == kPiPlusID || pdg_code == kPiMinusID) continue;
	if (mother_pdg == kKStar0ID && pdg_code == kKPlusID)     pdg_code = kKStar0ID;
	if (mother_pdg == kKStar0BarID && pdg_code == kKMinusID) pdg_code = kKStar0BarID;
	it2 = it2.mother();
	mother_pdg = it2.particleProperty();
      }
	
      if ( mother_pdg == kK0Star0ID || mother_pdg == kK0Star0BarID ) { // Looks another step up if mother was K_0
	it2 = it2.mother();
	mother_pdg = it2.particleProperty();
      }

      if (mother_pdg == kK10ID || mother_pdg == kK10BarID) {//Looks another step up if mother was k_10 or k_10bar
	it2 = it2.mother();
	mother_pdg = it2.particleProperty();
      }
      
      
      
      //Identifies what particles are the daughters
      if (mother_pdg == d0_pdg)  //Found daughter 
	{
	  if (pdg_code ==  kPiPlusID || pdg_code ==  kA0PlusID )                   num[0]++;
	  else if (pdg_code ==  kPiMinusID || pdg_code ==  kA0MinusID )            num[1]++;
	  else if (pdg_code ==  kPi0ID )                                           num[2]++;
	  else if (pdg_code ==  kEtaID )                                           num[3]++;
	  else if (pdg_code ==  kEtaPrimeID )                                      num[4]++;
	  else if (pdg_code ==  kF0ID || pdg_code ==  kA00ID 
		   || pdg_code ==  kFPrime0ID|| pdg_code ==  kF0m1500ID
		   || pdg_code ==  kF2ID)                                          num[5]++;
	  else if (pdg_code ==  kRhoPlusID || pdg_code ==  kRho2SPlusID
		   || pdg_code ==  kA1PlusID )                                     num[6]++;
	  else if (pdg_code ==  kRhoMinusID || pdg_code ==  kRho2SMinusID
		   || pdg_code ==  kA1MinusID)                                     num[7]++;
	  else if (pdg_code ==  kRho0ID || pdg_code ==  kRho2S0ID)                 num[8]++;
	  else if (pdg_code ==  kOmegaID )                                         num[9]++;
	  else if (pdg_code ==  kPhiID )                                           num[10]++;
	  else if (pdg_code ==  kKPlusID )                                         num[11]++;
	  else if (pdg_code ==  kKMinusID )                                        num[12]++;
	  else if (pdg_code ==  kK0SID )                                           num[13]++;
	  else if (pdg_code ==  kK0LID )                                           num[14]++;
	  else if (pdg_code ==  kKStarPlusID || pdg_code == kK1PlusID 
		   || pdg_code == kK2StarPlusID || pdg_code == kK1PrimePlusID
		   || pdg_code == kK0StarPlusID)                                   num[15]++;
	  else if (pdg_code ==  kKStarMinusID || pdg_code == kK1MinusID 
		   || pdg_code == kK2StarMinusID || pdg_code == kK1PrimeMinusID
		   || pdg_code == kK0StarMinusID )                                 num[16]++;
	  else if (pdg_code ==  kKStar0ID )                                        num[17]++;
	  else if (pdg_code ==  kKStar0BarID )                                     num[18]++;
	  else if (pdg_code ==  kK10ID || pdg_code ==  kK1Prime0ID)                num[19]++;
	  else if (pdg_code ==  kK10BarID || pdg_code ==  kK1Prime0BarID)          num[20]++;
	  else if (pdg_code ==  kEPlusID || pdg_code ==  kMuPlusID )               num[21]++;
	  else if (pdg_code ==  kEMinusID || pdg_code ==  kMuMinusID )             num[22]++;
	  else if (pdg_code ==  kNuEID || pdg_code ==  kNuEBarID
		   || pdg_code ==  kNuMuID || pdg_code ==  kNuMuBarID )            num[23]++;
	  else if (pdg_code ==  kGammaID )                                         num[24]++;
	  else if (pdg_code ==  kGammaFSRID )                                       continue;
	  else {
	    num[25]++;
	    cout <<"Unknown particle:  "<< pdg_code << endl;
	  }

	  //if (pdg_code ==  kA10ID )            num[0]++; // not present

	  //Enter Momentums for Dalitz canidates
	  //It would be prefered if we could get PreFSR Momentum
	  if (pdg_code ==  kPiPlusID)  piPlus.setVectM((*it)->initialFourMomentum().vect(), xmpion);
	  if (pdg_code ==  kPiMinusID) piMinus.setVectM((*it)->initialFourMomentum().vect(), xmpion);
	  if (pdg_code ==  kK0SID)     k0.setVectM((*it)->initialFourMomentum().vect(), xmk0);
	  if (pdg_code ==  kK0LID)     k0.setVectM((*it)->initialFourMomentum().vect(), xmk0);	    
	}
    }
  
  // D decay daughters have been tabulated.  Now, classify decay.
  int nDaughters = 0 ;
  for( int i = 0 ; i < 26 ; i++ )
    {
      nDaughters += num[ i ] ;
    }
  
  int nUFP0 = num[ kPi0 ] + num[ kEta ] + num[ kEtaPrime ] ;
  int nUFV0 = num[ kRho0 ] + num[ kPhi ] + num[ kOmega ] + num[ kGamma ] ;
  int nFV0 = num[ kKStar0 ] + num[ kK10 ] ;
  int nFV0Bar = num[ kKStar0Bar ] + num[ kK10Bar ] ;
  int nStrange = num[ kKMinus ] + num[ kFVMinus ] + nFV0Bar ;
  int nAntiStrange = num[ kKPlus ] + num[ kFVPlus ] + nFV0 ;
  int nCPPlusEig = num[ kNeutralScalar ] + num[ kRho0 ] + num[ kOmega ] + num[ kPhi ] + num[ kK0S ] + num[ kGamma ] ;
  int nCPMinusEig = num[ kPi0 ] + num[ kEta ] +  num[ kEtaPrime ] + num[ kK0L ] ;
  int nCPEig = nCPPlusEig + nCPMinusEig ;
  int nChargedPiK = num[ kPiPlus ] + num[ kPiMinus ] + num[ kKPlus ] + num[ kKMinus ] ;
  int nK0 = num[ kK0S ] + num[ kK0L ] ;
  
 
  //check Dalitz modes: KsPi+Pi- or KlPi+Pi-
  double mnm_gen = 0. ;
  double mpn_gen = 0. ;
  double mpm_gen = 0. ;
  bool isKsPiPi = false ;
  
  if( nK0 == 1 && num[ kPiPlus ] == 1 && num[ kPiMinus ] && nDaughters == 3 )
    {
      decay_mode = kDalitz ;
      k0.boost(-0.011, 0, 0);
      piMinus.boost(-0.011, 0, 0);
      piPlus.boost(-0.011, 0, 0);
      mnm_gen = (k0 + piMinus).m2();
      mpn_gen = (k0 + piPlus).m2();
      mpm_gen = (piPlus + piMinus).m2();
      if( num[ kK0S ] == 1)  isKsPiPi = true ;
    }
    
  // The order of if-statements below matters.
  if( decay_mode == kDalitz )
    {
      ++m_nDalitz ;
      
      Dalitz t(8) ;
      TComplex D0, D0bar;
      if (m_dalitzModel == 0) { //Cleo model
	D0 = t.CLEO_Amplitude( mpn_gen, mnm_gen, mpm_gen ) ;
	D0bar = t.CLEO_Amplitude( mnm_gen, mpn_gen, mpm_gen ) ;}
      if (m_dalitzModel == 1) { //Babar model
	D0 = t.Babar_Amplitude( mpn_gen, mnm_gen, mpm_gen ) ;
	D0bar = t.Babar_Amplitude( mnm_gen, mpn_gen, mpm_gen ) ;}
      if (m_dalitzModel == 2) { //Belle model
	D0 = t.Amplitude( mpn_gen, mnm_gen, mpm_gen ) ;
	D0bar = t.Amplitude( mnm_gen, mpn_gen, mpm_gen ) ;}
      
      if( charm == 1){
	r2D0 = norm( D0bar ) / norm( D0 ) ;
	deltaD0 = arg( D0 * conj( D0bar ) ) + ( isKsPiPi ? PI : 0. );
	
	double cosd = cos( deltaD0 ) ;
	double sind = sin( deltaD0 ) ;
	if( mpn_gen - mnm_gen > 0. )
	  {
	    m_dalitzNumer1 += -2. * sqrt( r2D0 ) * cosd ;
	    m_dalitzNumer2 += 2. * r2D0 * cosd * sind * m_x ;
	    m_dalitzDenom += -2. * r2D0 * cosd * cosd ;
	  }
      }
      else {
	r2D0 = norm( D0 ) / norm( D0bar ) ;
	deltaD0 =  arg( D0bar * conj( D0 ) ) + ( isKsPiPi ? PI : 0. ) ;

	double cosd = cos( deltaD0 ) ;
	double sind = sin( deltaD0 ) ;
	if( mpn_gen - mnm_gen < 0. )
	  {
	    m_dalitzNumer1 += -2. * sqrt( r2D0 ) * cosd ;
	    m_dalitzNumer2 += 2. * r2D0 * cosd * sind * m_x ;
	    m_dalitzDenom += -2. * r2D0 * cosd * cosd ;
	  }
      }
    }
  else if( num[ kLeptonPlus ] == 1 )
    {
      decay_mode = kSLPlus ;
      ++m_nSL ;
      
      r2D0 = 0. ;
       deltaD0 = 0. ;
    }
  else if( num[ kLeptonMinus ] == 1 )
    {
      decay_mode = kSLMinus ;
      ++m_nSL ;
      
      r2D0 = 0. ;
      deltaD0 = 0. ;
    }
  //Check CP even modes
  else if(
	  ( nDaughters == 2 &&
	    ( ( num[ kPiPlus ] == 1 && num[ kPiMinus ] == 1 ) ||
	      nUFP0 == 2 ||
	      num[ kNeutralScalar ] == 2 ||
	      ( nUFP0 == 1 && nUFV0 == 1 ) ||
	      ( num[ kNeutralScalar ] == 1 && nUFV0 == 1 ) ||
	      ( num[ kKPlus ] == 1 && num[ kKMinus ] == 1 ) ||
	      num[ kK0S ] == 2 || 
	      num[ kK0L ] == 2 ||
	      ( num[ kK0L ] == 1 && nUFP0 == 1 ) ||
	      ( num[ kK0S ] == 1 && num[ kNeutralScalar ] == 1 ) ||
	      ( num[ kK0L ] == 1 && nUFV0 == 1 ) ) ) ||
	  ( nDaughters == 3 &&
	    ( ( nCPPlusEig == 1 && num[ kPi0 ] == 2 ) ||
	      ( num[ kK0S ] == 3 ) ) ) ||
	  ( nDaughters == 4 &&
	    num[ kK0L ] == 1 && num[ kPi0 ] == 3 )
	  )
    {
      decay_mode = kCPPlus ;
      ++m_nCPPlus ;
      
      r2D0 = 1. ;
      deltaD0 = PI;
    }
  //Check CP odd modes
  else if(
	  ( nDaughters == 2 && 
	    ( ( num[ kK0S ] == 1 && nUFP0 == 1 ) ||
	      ( num[ kK0L ] == 1 && num[ kNeutralScalar ] == 1 ) ||
	      ( num[ kK0S ] == 1 && nUFV0 == 1 ) ||
	      ( nUFP0 == 1 && num[ kNeutralScalar ] == 1 ) ) ) ||
	  ( nDaughters == 3 &&
	    ( ( nCPMinusEig == 3 && num[ kPi0 ] == 2 ) || // pi0 is subset of CP-
	      ( num[ kPi0 ] == 3 ) ||
	      ( num[ kK0L ] == 3 ) ) ) ||
	  ( nDaughters == 4 &&
	    num[ kK0S ] == 1 && num[ kPi0 ] == 3 )
	  )
    {
      decay_mode = kCPMinus ;
      ++m_nCPMinus ;
      
      r2D0 = 1. ;
      deltaD0 = 0. ;
    }
  else if( nStrange == nAntiStrange + 1 )
    {
      decay_mode = kFlavoredCF ;
      
      if( charm == 1 )
	{
	  ++m_nFlavoredCFD0 ;
	  r2D0 = pow(m_rCF,2) ;
	  deltaD0 = m_deltaCF ;
	}
      else
	{
	  ++m_nFlavoredDCSD0 ;
	  r2D0 = 1. / pow( m_rCF,2 ) ;
	  deltaD0 = -m_deltaCF ;
	}
    }
  else if( nAntiStrange == nStrange + 1 )
    {
      decay_mode = kFlavoredCFBar ;
      
      if( charm == -1 )
	{
	  ++m_nFlavoredCFD0 ;
	  r2D0 = pow( m_rCF, 2) ;
	  deltaD0 = m_deltaCF ;
	}
      else
	{
	  ++m_nFlavoredDCSD0 ;
	  r2D0 = 1. / pow( m_rCF, 2) ;
	  deltaD0 = -m_deltaCF ;
	}
    }
  else if( nStrange == nAntiStrange ) // Unflavored or Cabibbo-suppressed
    {
      if( ( num[ kKPlus ] > 0 &&
	    ( num[ kKPlus ] == num[ kFVMinus ] ||
	      num[ kKPlus ] == nFV0Bar ) ) || // for anti-K*0 K+ pi-
	  ( nK0 > 0 &&
	    nFV0 != nFV0Bar &&
	    nK0 == nFV0Bar ) ||
	  ( num[ kPiPlus ] > 0 &&
	    num[ kPiPlus ] == num[ kUFVMinus ] ) )
	{
	  decay_mode = kFlavoredCS ;
	  ++m_nFlavoredCSD0 ;
	  
	  if( charm == 1 )
	    {
	      r2D0 = pow( m_rCS, 2 ) ;
	      deltaD0 = m_deltaCS ;
	    }
	  else
	    {
	      r2D0 = 1. / pow( m_rCS, 2 ) ;
	      deltaD0 = -m_deltaCS ;
	    }
	}
      else if( ( num[ kKMinus ] > 0 && ( num[ kKMinus ] == num[ kFVPlus ] || num[ kKMinus ] == nFV0 ) ) || // for K*0 K- pi+
	       ( nK0 > 0 && nFV0 != nFV0Bar &&  nK0 == nFV0 ) ||
	       ( num[ kPiMinus ] > 0 && num[ kPiMinus ] == num[ kUFVPlus ] ) )
	{
	  decay_mode = kFlavoredCSBar ;
	  ++m_nFlavoredCSD0 ;
	  
	  if( charm == -1 )
	    {
	      r2D0 = pow( m_rCS, 2 ) ;
	      deltaD0 = m_deltaCS ;
	    }
	  else
	    {
	      r2D0 = 1. / pow( m_rCS, 2 ) ;
	      deltaD0 = -m_deltaCS ;
	    }
	}

      // Self-conjugate mixed-CP final states -- pick CP or non-CP at
      // random.  If CP, pick + or - at random.  If non-CP, pick
      // flavored or flavoredBar according to the appropriate value of r.
      else if( ( nDaughters >= 3 && num[ kPiPlus ] == num[ kPiMinus ] &&
		 num[ kKPlus ] == num[ kKMinus ] && nChargedPiK + nCPEig == nDaughters ) ||
	       nUFV0 == nDaughters ||
	       ( num[ kKStar0 ] > 0 && num[ kKStar0 ] == num[ kKStar0Bar ] ) ||
	       ( num[ kK10 ] > 0 && num[ kK10 ] == num[ kK10Bar ] ) ||
	       ( num[ kUFVPlus ] == 1 && num[ kUFVMinus ] == 1 )
	       // for rho+rho-; no a1+a1- and rhoa1 final states in DECAY.DEC
	       )
	{
	  log << MSG::DEBUG <<"    [ Self-conjugate mixed-CP ]" << endmsg ;
	  
	  if( RandFlat::shoot() > 0.5 )
	    {
	      if( RandFlat::shoot() > 0.5 )
		{
		  decay_mode = kCPPlus ;
		  ++m_nCPPlus ;
		  
		  r2D0 = 1. ;
		  deltaD0 =  PI;
		}
	      else
		{
		  decay_mode = kCPMinus ;
		  ++m_nCPMinus ;
		  
		  r2D0 = 1. ;
		  deltaD0 = 0. ;
		}
	    }
	  else
	    {
	      // Odd # of K0S or K0L = CF; even # of K0S or K0L = SC.
	      if( nK0 % 2 )
		{
		  // Nov 2007: correct for r2 -> RWS and BR != A^2
		  double cutoff = m_rwsCF / ( 1. + m_rwsCF ) ;
		  
		  if( charm == -1 )
		    {
		      cutoff = 1. - cutoff ;
		    }
		  
		  log << MSG::DEBUG <<"    [ cutoff = " << cutoff << " ]" << endmsg ;
		  
		  decay_mode = ( RandFlat::shoot() > cutoff ) ? kFlavoredCF : kFlavoredCFBar ;
		  
		  if( ( decay_mode == kFlavoredCF && charm == 1 ) ||
		      ( decay_mode == kFlavoredCFBar &&	charm == -1 ) )
		    {
		      ++m_nFlavoredCFD0 ;
		      
		      r2D0 = sqrt( m_rCF ) ;
		      deltaD0 = m_deltaCF ;
		    }
		  else
		    {
		      ++m_nFlavoredDCSD0 ;
		      
		      r2D0 = 1. / sqrt( m_rCF ) ;
		      deltaD0 = -m_deltaCF ;
		    }
		}
	      else
		{
		  // Nov 2007: correct for r2 -> RWS and BR != A^2
		  double cutoff = m_rwsCS / ( 1. + m_rwsCS ) ;
		  
		  if( charm == -1 )
		    {
		      cutoff = 1. - cutoff ;
		    }
		  
		  log << MSG::DEBUG <<"    [ cutoff = " << cutoff << " ]" << endmsg ;
		  
		  decay_mode = ( RandFlat::shoot() > cutoff ) ?
		    kFlavoredCS : kFlavoredCSBar ;
		  ++m_nFlavoredCSD0 ;
		  
		  if( ( decay_mode == kFlavoredCS && charm == 1 ) ||
		      ( decay_mode == kFlavoredCSBar &&	charm == -1 ) )
		    {
		      r2D0 = sqrt( m_rCS ) ;
		      deltaD0 = m_deltaCS ;
		    }
		  else
		    {
		      r2D0 = 1. / sqrt( m_rCS ) ;
		      deltaD0 = -m_deltaCS ;
		    }
		}
	    }
	}
    }

  if (num[kUnknown] >= 1)
    {
      cout << "decay mode " << decay_mode << endl ;
      cout << "D #" << charm << endl ;
      cout << "pi+:     " <<  num[ kPiPlus ] << endl ;
      cout << "pi-:     " <<  num[ kPiMinus ] << endl ;
      cout << "pi0:     " <<  num[ kPi0 ] << endl ;
      cout << "eta:     " <<  num[ kEta ] << endl ;
      cout << "eta':    " <<  num[ kEtaPrime ] << endl ;
      cout << "f0/a0:   " <<  num[ kNeutralScalar ] << endl ;
      cout << "UFV+:    " <<  num[ kUFVPlus ] << endl ;
      cout << "UFV-:    " <<  num[ kUFVMinus ] << endl ;
      cout << "rho0:    " <<  num[ kRho0 ] << endl ;
      cout << "omega:   " <<  num[ kOmega ] << endl ;
      cout << "phi:     " <<  num[ kPhi ] << endl ;
      cout << "K+:      " <<  num[ kKPlus ] << endl ;
      cout << "K-:      " <<  num[ kKMinus ] << endl ;
      cout << "K0S:     " <<  num[ kK0S ] << endl ;
      cout << "K0L:     " <<  num[ kK0L ] << endl ;
      cout << "FV+:     " <<  num[ kFVPlus ] << endl ;
      cout << "FV-:     " <<  num[ kFVMinus ] << endl ;
      cout << "K*0:     " <<  num[ kKStar0 ] << endl ;
      cout << "K*0b:    " <<  num[ kKStar0Bar ] << endl ;
      cout << "K10:     " <<  num[ kK10 ] << endl ;
      cout << "K10b:    " <<  num[ kK10Bar ] << endl ;
      cout << "l+:      " <<  num[ kLeptonPlus ] << endl ;
      cout << "l-:      " <<  num[ kLeptonMinus ] << endl ;
      cout << "nu:      " <<  num[ kNeutrino ] << endl ;
      cout << "gamma:   " <<  num[ kGamma ] << endl ;
      cout << "Unknown: " <<  num[ 25 ] << endl ;  
    }


  d0_info[0]=decay_mode;
  d0_info[1]=r2D0;
  d0_info[2]=deltaD0;
  d0_info[3]=mnm_gen;
  d0_info[4]=mpn_gen;
  d0_info[5]= double (isKsPiPi);
  d0_info[6]=mpm_gen;
  return d0_info;
}
