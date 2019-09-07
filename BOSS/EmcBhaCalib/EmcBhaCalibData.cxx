//--------------------------------------------------------------------------
// File and Version Information:
//     
//
// Description:
//      Class EmcBhaCalibData - see header
//
// Environment:
//      Software developed for the BESIII Detector at the BEPCII.
//
// Author List:
//      Chunxiu Liu
//
// Copyright Information:
//      Copyright (C) 2005           IHEP
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "EmcBhaCalib/EmcBhaCalibData.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

#include <cstring>

using namespace std;
//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//              ----------------------------------------
//              -- Public Function Member Definitions --
//              ----------------------------------------

//----------------
// Constructors --
//----------------
EmcBhaCalibData::EmcBhaCalibData(int numxtals)
  : _nXtals(numxtals),
    _nXtalsHit(0)
{
  //  _matrixM = new EmcLSSMatrix(_nXtals,600);
  _matrixM = new EmcLSSMatrix(_nXtals,_nXtals);
  _vectorR = new double[_nXtals];
  _xtalInd = new int[_nXtals];
  _xtalHits = new int[_nXtals];
  _xtalHitsMax = new int[_nXtals];
  for (int i=0;i<_nXtals;i++) {
    _vectorR[i] = 0.;
    _xtalInd[i] = i;  
    _xtalHits[i] = 0;
    _xtalHitsMax[i] = 0;
  }
  _MsgFlag =0;
}

//----------------
// Constructors --
//----------------
EmcBhaCalibData::EmcBhaCalibData(int numxtals, int MsgFlag)
  : _nXtals(numxtals),
    _nXtalsHit(0)
{
  //  _matrixM = new EmcLSSMatrix(_nXtals,600);
  _matrixM = new EmcLSSMatrix(_nXtals,_nXtals, MsgFlag);
  _vectorR = new double[_nXtals];
  _xtalInd = new int[_nXtals];
  _xtalHits = new int[_nXtals];
  _xtalHitsMax = new int[_nXtals];
  for (int i=0;i<_nXtals;i++) {
    _vectorR[i] = 0.;
    _xtalInd[i] = i;
    _xtalHits[i] = 0;
    _xtalHitsMax[i] = 0;
  }

  _MsgFlag = MsgFlag;
}

//--------------
// Destructor --
//--------------
EmcBhaCalibData::~EmcBhaCalibData() {
  if ( 0 != _matrixM) {
    delete _matrixM; 
    _matrixM = 0;
  }
  if ( 0 != _vectorR) {
    delete[] _vectorR;
    _vectorR = 0;
  }
  if ( 0 != _xtalInd) {
    delete[] _xtalInd;
    _xtalInd = 0;
  }
  if ( 0 != _xtalHits) {
    delete[] _xtalHits;
    _xtalHits = 0;
  }
  if ( 0 != _xtalHitsMax) {
    delete[] _xtalHitsMax;
    _xtalHitsMax = 0;
  }
}
//-------------
// Operators --
//-------------

    
//-------------
// Selectors --
//-------------


//-------------
// Modifiers --
//-------------

void
EmcBhaCalibData::writeOut( ostream& OutM, ostream& OutV) {

  //write only non zero elements to file
  OutM.setf(ios::scientific,ios::floatfield);
  OutV.setf(ios::scientific,ios::floatfield);
  OutV.precision(12);
  OutM.precision(12);

  _matrixM->writeOut( OutM );

  _nXtalsHit = 0;

  double* vect_ele = _vectorR;
  double* vect_end = _vectorR + _nXtals;
  int* xtal_ind = _xtalInd;
  int* hits_max = _xtalHitsMax;
  int* hits = _xtalHits;

  while ( vect_ele < vect_end ) {
    if ( *vect_ele != 0.  ){
      _nXtalsHit++;
    }
    vect_ele++;
  }

  OutV << _nXtalsHit << " ";

  vect_ele = _vectorR;

  while ( vect_ele < vect_end ) {
    if ( *vect_ele != 0. ) {
      OutV << *xtal_ind << " " << *hits << " " << *hits_max << " " 
	   << *vect_ele << " ";
    }
    xtal_ind++;
    vect_ele++;
    hits_max++;
    hits++;
  }

}


void
EmcBhaCalibData::readIn( istream& InM, istream& InV) {

  _matrixM->readIn( InM );
  
  _matrixM->print_NonZeros();

  //cout<<"num_filled_rows(48)="<<_matrixM->num_filled_rows(48)<<endl;
  //cout<<"num_filled_cols(48)="<<_matrixM->num_filled_cols(48)<<endl;

  //cout<<"matrix(47,47)"<<_matrixM->operator()(47,47)<<endl;

  double vect_ele;
  int nr_ele;
  int xtal_ind;
  int hits;
  int hits_max;

  InV >> nr_ele;

  for (int i=0; i<nr_ele; i++) {

    InV >> xtal_ind >> hits >> hits_max
	>> vect_ele ;

    if (_vectorR[xtal_ind] == 0. ) {
      _nXtalsHit++;
    }
    
    _vectorR[xtal_ind] += vect_ele ;
    _xtalInd[xtal_ind] = xtal_ind;
    _xtalHits[xtal_ind] += hits;
    _xtalHitsMax[xtal_ind] += hits_max;

 }

}

bool
EmcBhaCalibData::reduce() {

  bool successful = true;

  //delete all zero elements in matrix and vector R
  //save only non zero elements (this format is needed by slap),
  //keep the original xtal index in array xtalInd

  //array to store the new row/column reference for later matrix reduction 
  int* xRef = new int[_nXtals];
  memset(xRef, -1, (_nXtals*sizeof(int)));

  _nXtalsHit = 0;
  for ( int _arrayIndx = 0; 
	_arrayIndx < _nXtals; _arrayIndx++) {

    if ( _vectorR[_arrayIndx] > 0. ) {

      _vectorR[_nXtalsHit] = _vectorR[_arrayIndx];
      _xtalInd[_nXtalsHit] = _xtalInd[_arrayIndx];
      _xtalHits[_nXtalsHit] = _xtalHits[_arrayIndx];
      _xtalHitsMax[_nXtalsHit] = _xtalHitsMax[_arrayIndx];
      xRef[_arrayIndx] = _nXtalsHit;

      _nXtalsHit++;
    }
  }

  successful = _matrixM->reduce_Matrix( xRef );

  delete [] xRef;

  return successful;
}


void
EmcBhaCalibData::reset() 
{
  _nXtalsHit = 0;
  _matrixM->reset();

  for (int i=0;i<_nXtals;i++) {
    _vectorR[i]= 0.;
    _xtalInd[i] = i;
    _xtalHits[i] = 0;
    _xtalHitsMax[i] = 0;
  }

}


void
EmcBhaCalibData::printVec(int number) {

  if (_MsgFlag <= 3) {
    std::cout << "EmcBhaCalibData:: INFO "<< "R ( " << number << " ): " << _vectorR[number] 
	<< "  hits: " << _xtalHits[number] 
	<< "  direct hits: " << _xtalHitsMax[number]
	<< endl;
  }
}



