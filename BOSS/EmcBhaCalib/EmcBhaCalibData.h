//--------------------------------------------------------------------------
// Environment:
//      This software was developed for the BESIII collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 2005           IHEP
//
//------------------------------------------------------------------------

#ifndef EMCBHACALIBDATA_H
#define EMCBHACALIBDATA_H

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <iostream>
#include <fstream>

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "EmcBhaCalib/EmcLSSMatrix.h"

using namespace std;

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
//              ---------------------
//              -- Class Interface --
//              ---------------------

/**
 *  This class holds the information that is needed to do the calibration 
 *  of the EMC with Bhabha events. It stores the accumulated matrix, 
 *  the accumulated vector, the number of hits percrystal and other stuff.
 *
 *  The data are filled in the method EmcMCBhaEvent::CollectBhabha().
 *
 *  This software was developed for the BesIII collaboration.  If you
 *  use all or part of it, please give an appropriate acknowledgement.
 *
 *  Copyright (C) 2005 IHEP
 *
 *  @see 
 *
 * 
 *  @author (Chunxiu Liu)		(originator/contributor etc.);
 */


class EmcBhaCalibData { 

public:

  // Constructor
  EmcBhaCalibData(int numxtals);

  EmcBhaCalibData(int numxtals, int MsgFlag);

  // Destructor
  ~EmcBhaCalibData( );

  // access
  // calibration matrix element
  double& matrixMEle(int row, int column) { return ((*_matrixM)(row,column)); }

  // access
  // pointer to calibration matrix
  EmcLSSMatrix* getMatrixM() { return _matrixM; }

  // calibration vector 
  double& vectorR(int ind) { return (_vectorR[ind]); }

  // pointer to calibration vector 
  double* getVectorR() { return _vectorR; }

  // array of xtal indices 
  int xtalInd(int ind) { return (_xtalInd[ind]); }

  // number of xtals 
  int& nXtals() { return _nXtals; }
  // number of xtals hit 
  int& nXtalsHit() { return _nXtalsHit; }
  // array with number of hits per xtal 
  int& xtalHits(int ind) { return (_xtalHits[ind]); }
  // array with number of direct hits per xtal 
  int& xtalHitsDir(int ind) { return (_xtalHitsMax[ind]); }

  //reduce matrix and vector to an array containing only non zero elements,
  //   needed for matrix inversion package slap 
  bool reduce();

  //reset all data 
  void reset();

  // write matrix and vector to files 
  void writeOut(ostream& OutM, ostream& outV);

  // read in matrix and vector from files 
  void readIn(istream& InM, istream& InV);

  // output vector element 
  void printVec(int number);

 
private:

  // Data members
  // calibration matrix 
  EmcLSSMatrix* _matrixM;
  // calibration vector 
  double* _vectorR;
  // array of xtal indices 
  int* _xtalInd;

  // number of xtals 
  int _nXtals;
  // number of xtals hit 
  int _nXtalsHit;
  // array with number of hits per xtal 
  int* _xtalHits;
  // array with number of direct hits per xtal 
  int* _xtalHitsMax;
  // message output flag, 
  int _MsgFlag;
};

#endif // EMCBHACALIBDATA_H
