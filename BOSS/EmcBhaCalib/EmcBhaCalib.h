//--------------------------------------------------------------------------
// Environment:
//      This software was developed for the BESIII collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//	Copyright (C) 2005              IHEP
//
//------------------------------------------------------------------------

#ifndef EMCBHACALIB_H
#define EMCBHACALIB_H

//-------------
// C Headers --
//-------------
#include <ctime>

#include <cstring>


#include "GaudiKernel/Algorithm.h"

#include "EmcBhaCalib/EmcLSSMatrix.h"
#include "EmcBhaCalib/EmcBhaCalibData.h"

#include "EmcCalibConstSvc/IEmcCalibConstSvc.h"
#include "EmcCalibConstSvc/EmcCalibConstSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

//Ntuple
#include "GaudiKernel/NTuple.h"

using namespace std;

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

//		---------------------
// 		-- Class Interface --
//		---------------------
/**
  *  class  EmcBhaCalib Algorithm - performs calibration of EMC Xtals with Bhabha
  *  events and Chi^2 fit, the resulting system of linear equations 
  *  of the fit is solved with the SLAP Algebra package
  *
  *  The matrix and the vector can either be fetched from file(s). 
  *  If they are read from files the file names are supposed to have the name 
  *  CalibVector.dat and calibMatrix.dat with an extension at the beginning.
  *  Only these extensions (usually the runnumbers) need to be listed in the read-in-file 
  *  that is specified with runNumberFile.
  *
  *  @author Chunxiu Liu	       (originator/contributor etc.);
  */

class EmcBhaCalib:public Algorithm{

  
 public:
  
  EmcBhaCalib(const std::string& name, ISvcLocator* pSvcLocator);


  //--------------
  // Destructor
  //--------------
  virtual ~EmcBhaCalib(); 
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  virtual void help( );

  //------------------
  // Helper functions
  //------------------

 private:


  // read in calibration constants from file or initialize them to 1   
  void initCalibConst();

  // solve system of linear equations with SLAP package
  bool solveEqua();

  //output constants to file
  void writeOutConst();

  //output matrix and vector to file
  void writeOut();

  //read in matrix and vector from file 
  bool readInFromFile();

  //read in matrix and vector from the database
  bool readInFromDB();

  //make the absolute constants and fit constants vs theta to get values
  //where we did not find some
  bool prepareConstants();

  // fill nutuple
  void ntupleOut();

  //print data information and only used in testing
  void  printInfo(std::string xtalHitsDirFile);
  //

  void digiConstCor();
  //--------------
  //  Data
  //--------------

private:

  double m_peakCor[6240];

  //cut on minimum number of direct hits needed per xtal  
  int m_dirHitsCut;

  //convergence criterion 
  double m_convCrit;

  //ask before doing the matrix inversion 
  bool m_askForMatrixInversion;
 
  //fit a polynom second order to the constants theta distribution
  //to get constants where we did not find one 
  bool m_fitPolynom; // now not been used

  //write the whole matrix, the vector and the constants to file ? 
  bool m_writeToFile;

  //read calibration data (matrix) from the database (or from files) 
  bool m_readDataFromDB;

  //which method for solving the matrix equation 
  std::string m_equationSolver;

  //file name extensions for different output files 
  std::string m_fileExt;

  //file directory 
  std::string m_fileDir;

  // collected calibration data 
  EmcBhaCalibData* m_myCalibData;

  //was calibration successful ? 
  bool m_calibrationSuccessful;
  
  //array of relative calibration constants, will be reduced to only 
  //living xtals for the matrix inversion (no zero hit entries) 
  double* m_calibConst;

  //relative calibration constants, full list including zero entries
  double* m_calibConstUnred;

  //absolute calibration constants (new relative * old constants) 
  double* m_absoluteConstants;

  //the old absolute constants the new once are relative to 
  double* m_oldConstants;
  
  //number of non zeros in matrix M 
  long int m_nrNonZeros;

  //number of xtals with minimum required direct hits 
  int m_nrXtalsEnoughHits;

  //file with a list of runnumbers to read in for which runs matrix file 
  // to load (Default: ./runnumber.dat)
  std::string  m_runNumberFile;

  //message output in the tool classes
  int m_MsgFlag;
  
  // ntuple for analysis 
  NTuple::Tuple*  m_tuple1; 
  NTuple::Item<long>  ixtal;
  NTuple::Item<float>  gi0;
  NTuple::Item<float> calibConst;
  NTuple::Item<float>  err;
  NTuple::Item<long>  nhitxtal;  

  double  DigiCalibConst;

  IEmcCalibConstSvc* m_emcCalibConstSvc;

};

#endif // EMCBHACALIB_H



















