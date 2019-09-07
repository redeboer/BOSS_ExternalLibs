//--------------------------------------------------------------------------
// Environment:
//      This software was developed for the BESIII collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//	Copyright (C) 2005         IHEP
//
//------------------------------------------------------------------------

#ifndef EMCLSSMATRIX_H
#define EMCLSSMATRIX_H

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <iostream>

//---------------
// Gaudi Headers --
//---------------
using namespace std;


//              ---------------------
//              -- Class Interface --
//              ---------------------

/**
  *  Implementation of a Large Sparse Symmetric Matrix,
  *
  *  Per row only the nonzero elements are stored together with their column
  *  indices. 
  *  (the storiage of the row index is a tribute to SLAP which needs it)
  *
  *  @see 
  *
  *  @author Chunxiu Liu	       (originator/contributor etc.);
  */

class EmcLSSMatrix { 

//--------------------
// Instance Members --
//--------------------

public:

  //Constructors
  EmcLSSMatrix();

  // constructor with number of rows and expected number of nonzeros per row 
  EmcLSSMatrix( int rows, int nonzero_col);

  // constructor with number of rows and expected number of nonzeros per row
  // , message output flag 
  EmcLSSMatrix( int rows, int nonzero_col, int MsgFlag);

  //Copy Constructor
  EmcLSSMatrix(const EmcLSSMatrix &m1 );

  // Destructor
  ~EmcLSSMatrix();

  // Get matrix element, indexing starts from (0,0)   
  double& operator()(int row, int col);

  // number of rows
  const int& num_rows() const { return _nrrows; }

  // number of columns     
  const int& num_cols() const { return _nrcol ; }

  // size of matrix   
  const long& size() const { return _size; }

  double* matrix( const int& rowind =0 ) const;
  int* row() const { return _rows; }
  int* column(const int& rowind = 0) const;

  // number of non Zero elements in a column
  int num_filled_rows(const int col) const;
  
  // number of non Zero elements in a row
  int num_filled_cols(const int row) const;

  //number of non Zero elements of the matrix
  long int num_nonZeros();

  //print a list of all non zero elements
  void print_NonZeros();

  // print a row
  void print_row(int rownr);

  //write out rows, columns and matrix elements to a file
  void writeOut( ostream& Out);

  //read in rows, columns and matrix elements from a file
  void readIn( istream& In);

  // Modifiers

  // set verbose
  void set_verbose(bool setting) { _verb = setting; }

  //reset matrix to zero
  void reset();

  //reduce matrix, make a array with only non-zero elements    
  bool reduce_Matrix(int* xRef_list);

protected:
  
  // Helper functions
  long int find(int row, int col);
  
 private:
  
  // Data members
  const long _size;
  const int _nrrows;
  const int _nrcol;
  double * _matrix;
  int * _columns;
  int * _rows;
  double* _nothing;
  bool _verb;
  // message output flag, 
  int _MsgFlag;
};

#endif // EMCLSSMATRIX_H 

