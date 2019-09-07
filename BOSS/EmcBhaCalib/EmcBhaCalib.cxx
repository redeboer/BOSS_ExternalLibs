//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class EmcBhaCalib - performs calibration of EMC Xtals with Bhabha
//      events and a Chi^2 fit, the resulting system of linear equations 
//      of the fit is solved with the SLAP Algebra package
//
// Environment:
//	Software developed for the BESIII Detector at the BEPCII.
//
// Author List:
//      Chunxiu Liu
//
// Copyright Information:
//	Copyright (C) 2005               IHEP
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "EmcBhaCalib/EmcBhaCalib.h"

//-------------
// C Headers --
//-------------
extern "C" {
#include "slap/dlap.h"
}
#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>
#include <cstdlib>
//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/DataObject.h"

#include "CLHEP/Vector/ThreeVector.h"
using namespace std;

using CLHEP::Hep3Vector;

//--------------------
#include "GaudiKernel/MsgStream.h"


#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TObjArray.h"



//----------------
// Constructors --
//----------------
EmcBhaCalib::EmcBhaCalib(const std::string& name, ISvcLocator* pSvcLocator)
  :Algorithm(name, pSvcLocator),  
   m_dirHitsCut(200),
   m_convCrit(0.000001),
   m_askForMatrixInversion(true),
   m_fitPolynom(true),  //no used now
   m_writeToFile(false),
   m_readDataFromDB(false),
   m_equationSolver("GMR"),
   m_fileExt(""),
   m_fileDir("/ihepbatch/besdata/public/liucx/Calib/"),
   m_nrNonZeros(0), 
   m_nrXtalsEnoughHits(0),
   m_runNumberFile("runnumbers.dat"),
   m_MsgFlag(0)

{

  // Declare the properties  
  declareProperty("equationSolver", m_equationSolver);
  declareProperty("dirHitsCut", m_dirHitsCut);
  declareProperty("writeToFile", m_writeToFile);
  declareProperty("fileExt", m_fileExt);
  declareProperty("fileDir", m_fileDir);
  declareProperty("readDataFromDB", m_readDataFromDB);
  declareProperty("askForMatrixInversion", m_askForMatrixInversion);
  declareProperty("fitPolynom", m_fitPolynom);
  declareProperty("convCrit",  m_convCrit);
  declareProperty("runNumberFile", m_runNumberFile);
  declareProperty("MsgFlag", m_MsgFlag);

  m_calibConst = new double[6240];
  m_calibConstUnred = new double[6240];
  m_absoluteConstants = new double[6240];
  m_oldConstants = new double[6240];


  //ntuple
  m_tuple1=0;
 


}

//--------------
// Destructor --
//--------------
EmcBhaCalib::~EmcBhaCalib() {
  if ( 0 != m_calibConst) {
    delete [] m_calibConst;
    m_calibConst = 0;
  }
  if ( 0 != m_calibConstUnred) {
    delete [] m_calibConstUnred;
    m_calibConstUnred = 0;
  }
  if ( 0 != m_absoluteConstants) {
    delete [] m_absoluteConstants;
    m_absoluteConstants = 0;
  }
  if ( 0 != m_oldConstants) {
    delete [] m_oldConstants;
    m_oldConstants = 0;
  }
  if ( 0 != m_myCalibData) {
    delete m_myCalibData;
    m_myCalibData = 0;
  }

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcBhaCalib::initialize(){
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  m_myCalibData = new EmcBhaCalibData(6240, m_MsgFlag);

  m_calibrationSuccessful = false;  
  
  StatusCode status1;
  
  NTuplePtr nt1(ntupleSvc(),"FILE302/n1");
  if ( nt1 ) m_tuple1 = nt1;
  else {
    m_tuple1=ntupleSvc()->book("FILE302/n1",CLID_ColumnWiseTuple,"Calib");
    if( m_tuple1 ) {
      
	 status1 = m_tuple1->addItem ("ixtal",ixtal); 
	 status1 = m_tuple1->addItem ("gi0",gi0);
	 status1 = m_tuple1->addItem ("calibConst",calibConst);
	 status1 = m_tuple1->addItem ("err",err);
	 status1 = m_tuple1->addItem ("nhitxtal",nhitxtal);
      
    }
    else    {   // did not manage to book the N tuple....
      log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple1) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // use EmcCalibConstSvc
  StatusCode scCalib; 
  scCalib = Gaudi::svcLocator() -> service("EmcCalibConstSvc", m_emcCalibConstSvc);
  if( scCalib != StatusCode::SUCCESS){
    log << MSG::ERROR << "can not use EmcCalibConstSvc" << endreq;
  } 
  else {
    std::cout << "Test EmcCalibConstSvc   DigiCalibConst(0)=  " 
  	      << m_emcCalibConstSvc -> getDigiCalibConst(0) << std::endl;
  }
  
   //init starting values for calibration constants from file or set to 1
  initCalibConst();  

  //digiConstCor();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcBhaCalib::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;   
 //read in accumulated matrix/matrices
  if ( m_readDataFromDB ) {
    m_calibrationSuccessful = readInFromDB();

    log << MSG::INFO << "read in accumulated matrix from DB"<<endreq;

  } else {
    m_calibrationSuccessful = readInFromFile();

    log << MSG::INFO <<"read in accumulated matrix from file"<<endreq; 

  }
  
  if ( m_calibrationSuccessful == true ) {
    
    //ask first if to do a matrix inversion
    if (m_askForMatrixInversion==true) {

      m_calibrationSuccessful = false;
      log << MSG::INFO << " Well, we have " 
	  << m_nrXtalsEnoughHits << " crystals whith more "
	  << "than " <<  m_dirHitsCut 
	  << " direct hits. Do you want do "
	  << "a matrix inversion ? [N]" << endreq; 


      m_calibrationSuccessful = true;

    }
   
    if ( m_calibrationSuccessful == true ) {
      
      //write added matrix and vector to files
      if ( m_writeToFile == true) {	
       	writeOut();
      }
      //cout <<"writeout"<<endl;
	m_myCalibData->printVec(2);

      //reduce to continious array of only non zeros elements for SLAP
      if ( m_calibrationSuccessful = m_myCalibData->reduce() ) {

	cout<<"m_calibrationSuccessful="<<m_calibrationSuccessful<<endl;

	if ( m_myCalibData->nXtalsHit() != m_myCalibData->nXtals() ){
	  log << MSG::INFO << " Reduced number of Xtals for calibration: " 
	      << m_myCalibData->nXtalsHit() 
	      << endreq;
	}
	cout<<"m_myCalibData->nXtalsHit()="<<m_myCalibData->nXtalsHit()
	    <<"m_myCalibData->nXtals()="<<m_myCalibData->nXtals()<<endl;
	m_myCalibData->printVec(10);

	//solve matrix equation
	if ( m_calibrationSuccessful = solveEqua() ) {
	  
	  for (int i=0; i<m_myCalibData->nXtalsHit(); i++){
	    //fill an array of constants for storage in database
	    m_calibConstUnred[m_myCalibData->xtalInd(i)] 
	      = m_calibConst[i];
	    
	    // if (m_myCalibData->xtalHitsDir(i)>10)
	      // cout<<"Index,calibconst="<<"  "<<i <<"  "<<m_myCalibData->xtalInd(i)
	      //  <<"  "<<m_calibConstUnred[m_myCalibData->xtalInd(i)]
	      //  <<"  "<<m_calibConstUnred[m_myCalibData->xtalInd(i)]*
	      //  m_oldConstants[m_myCalibData->xtalInd(i)]<<endl;
	    
	  }
	  //do validation, fit polynom if necessary, fill CalList	  
	  prepareConstants();

	  //if wanted write constants to plain ASCII file 
	  if ( m_writeToFile==true){ 
	    writeOutConst();
	  }

	  ntupleOut();

	}
	
      } else {
	log << MSG::WARNING << " Reduction of the Emc Bhabha calibration matrix FAILED !"
	    << endl
	    << " Will NOT perform Emc Bhabha calibration !"
	    << endreq;
	return( StatusCode::FAILURE);
      }
    }
  } else {
    log << MSG::WARNING << " ERROR in reading in Emc Bhabha calibration data !"
	<< endl
	<< " Will NOT perform Emc Bhabha calibration !"
	<< endreq;
    return( StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcBhaCalib::finalize() {

  MsgStream log(msgSvc(), name());


  log << MSG::INFO << "in endRun()" << endreq;


  return StatusCode::SUCCESS;
}


void
EmcBhaCalib::help() {
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO<< "Performs the Chi square fit of the accumulated " 
      << endreq;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void
EmcBhaCalib::initCalibConst( ) {

  
  MsgStream log(msgSvc(), name());
  
  for ( int i = 0; i< m_myCalibData->nXtals(); i++ ) {
    m_calibConst[i] = 1.;
  }  

  ifstream inConst("EmcCalib.Const");  
  if ( !inConst ) {
    log << MSG::VERBOSE  
	<< " No starting values for calibration constants found ! "
	<< "(EmcCalib.Const)" 
	<< endl
	<< "Set them to 1.0 ! " << endreq;
  } else {
    log << MSG::VERBOSE << " Read in starting values of calibration constants !"
	<< endreq;
    
    int nConst;
    inConst >> nConst;
    log << MSG::VERBOSE << " Have starting values for " 
	<< nConst << " Xtals !" << endl
	<< "Set the others to 1.0 ! " << endmsg;
    
    int numberXtal;
    for ( int i = 0; i< nConst; i++ ) {
      inConst >> numberXtal >> m_calibConst[numberXtal];
    }
  }
  
  int nConstEmc;
      
  nConstEmc= m_emcCalibConstSvc -> getDigiCalibConstNo() ;

  if ( nConstEmc!=6240) cout<<"number of calibconst="<< nConstEmc<<endl;
  
  // log << MSG::VERBOSE << " Have starting values for " 
  //   << nConstEmc << " Xtals !" << endl
  //   << "Set the others to 1.0 ! " << endmsg;
  
  for ( int i = 0; i< nConstEmc; i++ ) {
    //cout<<i<<"  "
    //<<m_emcCalibConstSvc->getThetaIndex(i)<<"  "
    //<<m_emcCalibConstSvc->getPhiIndex(i)
    //<<"  "<<m_emcCalibConstSvc->getEmcID(i)<<endl;  
    m_calibConst[i] = m_emcCalibConstSvc -> getDigiCalibConst(i);
    //m_calibConst[i] =5.0;
    m_oldConstants[i]=m_emcCalibConstSvc -> getDigiCalibConst(i);

    // initialize m_absoluteConstants as  m_oldConstants.
    //m_absoluteConstants[i] =m_emcCalibConstSvc -> getDigiCalibConst(i);
    m_absoluteConstants[i] =1.0;
    // initialize m_calibConstUnred as 1.
    m_calibConstUnred[i] =1.0;
  }


}


//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool
EmcBhaCalib::solveEqua() {
  
  MsgStream log(msgSvc(), name());
  //-----------------------------------------------------
  //  solve system of equations with SLAP package 
  //-----------------------------------------------------
  log << MSG::VERBOSE << " Solve Matrix equation with method " 
      << m_equationSolver
      << endl
      << "Xtals hit: " << m_myCalibData->nXtalsHit() 
      << endl
      << "Nr of non Zeros: " << m_nrNonZeros << endreq;
  
  //input parameters for SLAP
  long int isym = 1;     //matrix M is symmetric
  long int nsave = 20;
  long int itol = 0;     //type of convergence criterion
  double convCriterion = m_convCrit;    //convergence crtiterion
  long int maxIter = 1000;    //maximum number of iterations
  long int errUnit = 6;     //unit on which to write error estimation
  // at each iteration  

  //working arrays needed by SLAP
  long int lengthDoubleWork;
  double* doubleWork;
  long int lengthIntWork;
  long int* intWork;

  //output parameters of slap
  long int iters=0;           //number of iterations required
  double errorv=1000;        //error estimate
  long int errorFlag=9999;  

  //sparse Ax=b solver.
  //uses the generalized minimum residual method 
  //The preconditioner  is   diagonal scaling.
  if ( m_equationSolver == "GMR" ) {

    cout<<m_equationSolver<<endl;
 
    cout<<"errorFlag="<<errorFlag<<endl;

    //workspaces
    lengthDoubleWork = (1 + m_myCalibData->nXtals()*(nsave+7) 
			 + nsave*(nsave+3))
                         + 50000;
    doubleWork = new double[lengthDoubleWork];
    lengthIntWork = 50;
    intWork = new long int [lengthIntWork];
  
    dsdgmr_ ( &(m_myCalibData->nXtalsHit()),
	      m_myCalibData->getVectorR(),
	      m_calibConst,
	      &m_nrNonZeros,
	      m_myCalibData->getMatrixM()->row(),
	      m_myCalibData->getMatrixM()->column(),
	      m_myCalibData->getMatrixM()->matrix(),
	      &isym,
	      &nsave,
	      &itol,
	      &convCriterion,
	      &maxIter,
	      &iters,
	      &errorv,
	      &errorFlag,
	      &errUnit,
	      doubleWork,
	      &lengthDoubleWork,
	      intWork,
	      &lengthIntWork
	     );   
    cout<<"errorFlag="<<errorFlag<<endl;

          
    log << MSG::VERBOSE << " Error flag: " <<  errorFlag << endreq;
    if ( errorFlag < 0 ) errorFlag = labs(errorFlag) + 2;
    switch ( errorFlag ) {
    case 0: log << MSG::VERBOSE << " all went well" 
		<< endreq; break;
    case 1: log << MSG::ERROR << " insufficient storage allocated for _doubleWork or _intWork" 
		<< endreq; break;
    case 2: log << MSG::ERROR << " method failed to reduce norm of current residual" 
		<< endreq; break;
    case 3: log << MSG::ERROR << " insufficient length for _doubleWork" 
		<< endreq; break;
    case 4: log << MSG::ERROR << " inconsistent _itol and  values" 
		<< endreq; break;
    default: log << MSG::ERROR << " unknown flag" << endreq; 
    }
    log << MSG::VERBOSE << " Integer workspace used = " << intWork[8] << endl
	<< " Double  workspace used = " << intWork[9] << endreq;
  }

  //Routine to solve a  symmetric positive definite  linear
  //system Ax  =  b  using   the  Preconditioned  Conjugate
  //Gradient   method.    The preconditioner  is   diagonal scaling.
  if ( m_equationSolver == "PCG" ) {
    
    cout<<m_equationSolver<<endl;

    itol = 1; 

    //workspaces
    lengthDoubleWork = 5 *m_myCalibData->nXtals() + 50000;
    doubleWork = new double[lengthDoubleWork];
    lengthIntWork = 50;
    intWork = new long int [lengthIntWork];
       
    dsdcg_( &(m_myCalibData->nXtalsHit()),
	    m_myCalibData->getVectorR(),
	    m_calibConst,
	    &m_nrNonZeros,
	    m_myCalibData->getMatrixM()->row(),
	    m_myCalibData->getMatrixM()->column(),
	    m_myCalibData->getMatrixM()->matrix(),
	    &isym,
	    &itol,
	    &convCriterion,
	    &maxIter,
	    &iters,
	    &errorv,
	    &errorFlag,
	    &errUnit,
	    doubleWork,
	    &lengthDoubleWork,
	    intWork,
	    &lengthIntWork
	    );
      
    switch ( errorFlag ) {
    case 0: log << MSG::VERBOSE << "all went well" << endreq; break;
    case 1: log << MSG::ERROR << " insufficient storage allocated for WORK or IWORK" 
		<< endreq; break;
    case 2: log << MSG::ERROR << " method failed to to converge in maxIter steps." 
		<< endreq; break;
    case 3:log << MSG::ERROR << " Error in user input.  Check input value of _nXtal,_itol." 
	       << endreq; break;
    case 4:log << MSG::ERROR << " User error tolerance set too tight. "
	       << "Reset to 500.0*D1MACH(3).  Iteration proceeded." 
	       << endreq; break;
    case 5:log << MSG::ERROR << " Preconditioning matrix, M,  is not Positive Definite. "
	       << endreq; break;
    case 6: log << MSG::ERROR << " Matrix A is not Positive Definite."
		<< endreq; break;
    default: log << MSG::ERROR << " unknown flag" << endreq; 
    }
     log << MSG::VERBOSE << " Integer workspace used = " << intWork[9] << endl
		    << "Double  workspace used = " << intWork[10] << endreq;
  }

   log << MSG::VERBOSE << "------ Calibration fit statistics ------- " << endl
		  << "maximum number of iterations =" << maxIter << endl
		  << "        number of iterations =" << iters << endl
		  << "error estimate of error in final solution =" 
		  << errorv << endreq;

  if ( 0 != doubleWork) delete [] doubleWork;	  
  if ( 0 != intWork) delete [] intWork;

  if ( errorFlag != 0 ) return false;
  else return true; 

  return true;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void
EmcBhaCalib::writeOut() {

  ofstream vectorOut;
  std::string vectorFile = m_fileDir;
  vectorFile += m_fileExt;
  vectorFile += "allCalibVector.dat";
  vectorOut.open(vectorFile.c_str());

  ofstream matrixOut;
  std::string matrixFile = m_fileDir;
  matrixFile += m_fileExt;
  matrixFile += "allCalibMatrix.dat";
  matrixOut.open(matrixFile.c_str());

  MsgStream log(msgSvc(), name());

  log << MSG::VERBOSE << " Write out files " 
      << vectorFile << "   " 
      << matrixFile
      << endreq;

  m_myCalibData->writeOut(matrixOut,vectorOut);

  vectorOut.close();
  matrixOut.close();

}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void
EmcBhaCalib::writeOutConst() {
  
  ofstream constOut;  

  std::string constFile = m_fileDir;
  constFile += m_fileExt;
  constFile += "CalibConst.dat";

  constOut.open(constFile.c_str());
  
  constOut << "#crystalIndex relative-constant absolute-constant" << endl;

  int chanIndex;
   
  //output constants to file
  for ( int i=0; i < m_myCalibData->nXtalsHit(); i++) {

     chanIndex=m_myCalibData->xtalInd(i);  //xtalInd(i) array of xtal indices
 
    //---- get the new absolute constant ----
    //set it to 0 if we have not enough hits -> we'll keep the old constant
    if ( m_myCalibData->xtalHitsDir(i) < m_dirHitsCut )
      m_absoluteConstants[chanIndex] = m_oldConstants[chanIndex];
    else 
      m_absoluteConstants[chanIndex] =
	m_oldConstants[chanIndex] * m_calibConstUnred[chanIndex];

  }

  //output constants to file
  for ( int i=0; i < m_myCalibData->nXtals(); i++) {

    long Xtal_Index = i;
      if(m_calibConstUnred[Xtal_Index]>0){
	constOut << Xtal_Index << " " 
		 << m_calibConstUnred[Xtal_Index] << " " 
		 << m_oldConstants[Xtal_Index] << " " 
		 << m_absoluteConstants[Xtal_Index]
		 << endl; 
	
	
      }
  }
  constOut.close();

}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
bool
EmcBhaCalib::readInFromFile() {

  MsgStream log(msgSvc(), name());

  log << MSG::INFO << " Read Bhabha calibration data from files !"
		  << endreq;

  std::string runNumberFile = m_fileDir;
  runNumberFile += m_fileExt;
  runNumberFile += m_runNumberFile;

  bool success = false;
  log << MSG::INFO << " Get file names from input file : " 
      << runNumberFile
      << endreq;
  
  std::string vectorFile;
  std::string matrixFile;

  log << MSG::INFO  << "Runnumber    non-zeros    xtals"
      << endreq;   

  ifstream datafile(runNumberFile.c_str());

  //cout<<"datafile="<<runNumberFile.c_str()<<""<<datafile.good()<<endl;

  if (datafile.good() > 0 ) {
    
    while( !datafile.eof() ) {
      
      ifstream vectorIn;
      ifstream matrixIn;
      
      std::string num;
      datafile >> num;
      
      if ( num.length() > 0 ) {
	
	vectorFile = m_fileDir;
	vectorFile += m_fileExt;
	vectorFile += num;
	vectorFile += "CalibVector.dat"; 

	matrixFile = m_fileDir;
	matrixFile += m_fileExt;
	matrixFile += num;
	matrixFile += "CalibMatrix.dat";

	vectorIn.open(vectorFile.c_str());
	matrixIn.open(matrixFile.c_str());

	if ( vectorIn.good() > 0 && matrixIn.good() > 0 ) {
	  
	  m_myCalibData->readIn(matrixIn,vectorIn);
	  
	  log << MSG::INFO << num 
	      << "          "     
	      << m_myCalibData->getMatrixM()->num_nonZeros()
	      << "       "
	      << m_myCalibData->nXtalsHit() 
	      << endreq;

	  success = true;

	}
	else {
	  if ( !vectorIn )
	    log << MSG::ERROR << " ERROR: Vector input file "
		<< vectorFile
		<<  " doesn't exist !" << endreq;
	  if ( !matrixIn )
	    log << MSG::ERROR << " ERROR: Matrix input file "
		<< matrixFile
		<< " doesn't exist !" << endreq;
	}
	vectorIn.close();
	matrixIn.close();
      }
    }
  }

  if ( success == true) {

    for (int i=0; i < m_myCalibData->nXtals(); i++) {

      if ( m_myCalibData->xtalHitsDir(i) > m_dirHitsCut ) 
	{
	  m_nrXtalsEnoughHits++;
	}
    }
    m_nrNonZeros = m_myCalibData->getMatrixM()->num_nonZeros();
    log << MSG::INFO<< " Final matrix :  "
	<< "Number of non zero elements: " << m_nrNonZeros 
	<< "       "
	<< m_myCalibData->nXtalsHit()
	<< endreq;

  }


  return success;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool
EmcBhaCalib::readInFromDB() {

  bool success = true;

  return success;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool
EmcBhaCalib::prepareConstants() {

  bool successfull=false;

  //the old constant
  //float theCalConst = 1.0;
  int chanIndex;

  for ( int i = 0; i< m_myCalibData->nXtalsHit(); i++ ) {

    chanIndex=m_myCalibData->xtalInd(i);  //xtalInd(i) array of xtal indices
   
    //////////deal with the crystal of ixtal=689, because of ADC is very small;
    // m_calibConstUnred[chanIndex]=1;
    //if (chanIndex==689) m_calibConstUnred[chanIndex]=1.47;
    //if (chanIndex==689)
    //  m_oldConstants[chanIndex]=m_oldConstants[chanIndex]*1.47;
    //---- get the new absolute constant ----
    //set it to 0 if we have not enough hits -> we'll keep the old constant
    if ( m_myCalibData->xtalHitsDir(i) < m_dirHitsCut )
      m_absoluteConstants[chanIndex] = m_oldConstants[chanIndex];
    else 
      m_absoluteConstants[chanIndex] =
	m_oldConstants[chanIndex] * m_calibConstUnred[chanIndex];
  }


  double DigiConst[6240];
  int IxtalNumber[6240];

  for(int ind=0; ind<m_myCalibData->nXtals(); ind++ ) {

    DigiConst[ind]=m_absoluteConstants[ind];
    // cout<<"ind="<<ind<<"\t"<< DigiConst[ind]<<endl;
  }

  int ixtal1,ixtal2,ixtal3;
  ixtal1=m_emcCalibConstSvc->getIndex(1,20,26);
  ixtal2=m_emcCalibConstSvc->getIndex(1,23,26);
  ixtal3=m_emcCalibConstSvc->getIndex(1,24,26);
  //cout<<ixtal1<<"   "<<ixtal2<<"   "<<ixtal3<<"   "<<endl;
  for(int ind=0; ind<m_myCalibData->nXtals(); ind++ ) {

    IxtalNumber[ind]=-1;
    /*    
    if (ind==ixtal1) IxtalNumber[ind]=ixtal3;
    if (ind==ixtal2) IxtalNumber[ind]=ixtal1;
    if (ind==ixtal3) IxtalNumber[ind]=ixtal2;
    */
  }

 TFile fconst("EmcCalibConst.root", "recreate");

 //define tree fill the absolute digicalibconsts into the root-file
  TTree* constr= new TTree("DigiCalibConst","DigiCalibConst");
  constr->Branch("DigiCalibConst",DigiConst,"DigiConst[6240]/D"); 
  constr->Branch("IxtalNumber",IxtalNumber,"IxtalNumber[6240]/I"); 

  constr->Fill();
  /* 

  double aa[6240];
  int Iaa[6240];
  constr->SetBranchAddress("DigiCalibConst", aa);
  constr->SetBranchAddress("IxtalNumber", Iaa);
  constr->GetEntry(0);
  cout<<Iaa[10]<<endl;  
  cout<<aa[10]<<endl;

  cout<<constr->GetEntry(0)<<endl;
  
  */

  constr->Write();
 
  delete constr;
  
  fconst.Close();

  // end tree
   
  successfull=true;
  return successfull;

}

void 
EmcBhaCalib::ntupleOut(){


  for (int i=0; i < m_myCalibData->nXtalsHit(); i++) {
	int xtalIndex=m_myCalibData->xtalInd(i);

	gi0 = m_oldConstants[xtalIndex];

	if (gi0<0) cout<<"gi0="<<gi0<<endl;
	err = (m_calibConstUnred[xtalIndex]-gi0)/gi0;
	calibConst=m_calibConstUnred[xtalIndex];
	ixtal=xtalIndex;
	nhitxtal=m_myCalibData->xtalHitsDir(i); ///
	m_tuple1->write();   
  }

}


void
EmcBhaCalib::printInfo(std::string fileName) {

  ofstream xtalHitsDirOut;
  std::string xtalHitsDirFile = m_fileDir;
  xtalHitsDirFile += m_fileExt;
  xtalHitsDirFile += fileName;
  xtalHitsDirOut.open(xtalHitsDirFile.c_str());

  //nXtalsHit() is number of xtals hit
  for (int i=0; i < m_myCalibData->nXtalsHit(); i++) {
    if ( m_myCalibData->xtalHitsDir(i) > m_dirHitsCut ) 
      {
	int chanindex=m_myCalibData->xtalInd(i);
	xtalHitsDirOut<<chanindex<<endl;
      }
  }
  xtalHitsDirOut.close();

}

void 
EmcBhaCalib::digiConstCor(){

  // ofstream Fuout;
  //std::string Fuoutfile; 

  //Fuoutfile="emccalibconst.txt";
  //Fuout.open(Fuoutfile.c_str());

  double digiConst[6240];

  for(int ind=0; ind<m_myCalibData->nXtals(); ind++ ) {

    digiConst[ind]=m_oldConstants[ind];
    // Fuout<<m_oldConstants[ind]<<endl;
  }

  // Fuout.close();

  ifstream ExpEneIn;
  std::string ExpEneFile; 
  ExpEneFile = "cor.conf";
  ExpEneIn.open(ExpEneFile.c_str());

  double ExpEne[56];
  double Exp_barrel[44],Exp_east[6],Exp_west[6];

  for(int i=0;i<56;i++) {

    ExpEneIn>>ExpEne[i];
    ExpEne[i]=ExpEne[i]*1.843;
    if (i<6) Exp_east[i]=ExpEne[i];
    if (i>=6&&i<50) Exp_barrel[i-6]=ExpEne[i];
    if (i>=50) Exp_west[55-i]=ExpEne[i];

    //cout<<ExpEne[i]<<endl;
  }
  ExpEneIn.close();

  ifstream EDepEneIn;
  std::string EDepEneFile = "allxtal.dat";
  EDepEneIn.open(EDepEneFile.c_str());

  double CorDigiConst[6240];
  int ipart,ithe,iphi;
  double peak,sigma;
  double coeff;

  for(int i=0;i<6240;i++) {

    EDepEneIn>>ipart>>ithe>>iphi>>peak>>sigma;
    int ix=m_emcCalibConstSvc->getIndex(ipart,ithe,iphi);

    if (ipart==0) coeff=peak/Exp_east[ithe];
    if (ipart==1) coeff=peak/Exp_barrel[ithe];
    if (ipart==2) coeff=peak/Exp_west[ithe];
    cout<<coeff<<endl;
    //CorDigiConst[ix]=digiConst[ix]/coeff;
    CorDigiConst[ix]=coeff;
  }
  EDepEneIn.close();


  //define tree fill the absolute digicalibconsts into the root-file

  TTree* constr= new TTree("DigiCalibConst","DigiCalibConst");
    constr->Branch("DigiCalibConst",CorDigiConst,"CorDigiConst[6240]/D");
  constr->Fill();

  double aa[6240];
   constr->SetBranchAddress("DigiCalibConst", aa);
   constr->GetEntry(0);

  TFile fconst("EmcCalibConstCor.root", "recreate");
  constr->Write();

   fconst.Close();
   delete constr;


}
