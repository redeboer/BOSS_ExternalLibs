//--------------------------------------------------------------------------
//
// Description:
//      Class EmcLSSMatrix - Implementation of a Large Sparse Symmetric Matrix
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
#include "EmcBhaCalib/EmcLSSMatrix.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

#include <fstream>

using namespace std;

//              ----------------------------------------
//              -- Public Function Member Definitions --
//              ----------------------------------------

//----------------
// Constructors --
//----------------
EmcLSSMatrix::EmcLSSMatrix()
  : _size(0), _nrrows(0), _nrcol(0), _matrix(0), _columns(0), _rows(0),
    _nothing(0),_verb(false)  
{
}

EmcLSSMatrix::EmcLSSMatrix( int rows, int nonzero_col)
  : _size(rows * nonzero_col),
    _nrrows(rows),
    _nrcol(nonzero_col)
{
  _matrix = new double[_size];
  _rows = new int[_size];
  _columns = new int[_size];
  for (int i=0; i<_size;i++) 
    {
      _matrix[i] =0.;
      _rows[i] = -1;
      _columns[i] = -1;
    }
  _nothing = new double;
  _verb = false;

  _MsgFlag =0;
}

EmcLSSMatrix::EmcLSSMatrix( int rows, int nonzero_col, int MsgFlag)
  : _size(rows * nonzero_col),
    _nrrows(rows),
    _nrcol(nonzero_col)
{
  _matrix = new double[_size];
  _rows = new int[_size];
  _columns = new int[_size];
  for (int i=0; i<_size;i++) 
    {
      _matrix[i] =0.;
      _rows[i] = -1;
      _columns[i] = -1;
    }
  _nothing = new double;
  _verb = false;

  _MsgFlag = MsgFlag;
}

EmcLSSMatrix::EmcLSSMatrix( const EmcLSSMatrix &m1 ) 
  : _size(m1._size),
    _nrrows(m1._nrrows),
    _nrcol(m1._nrcol)

{
  _matrix = new double[_size]; 
  _rows = new int[_size];
  _columns = new int[_size];
  _nothing = m1._nothing;
  _verb = m1._verb;

  for ( long int i=0;i<_size;i++) 
    {
      _matrix[i] = m1._matrix[i];
      _rows[i] = m1._rows[i];
      _columns[i] = m1._columns[i];
    };
}

//--------------
// Destructor --
//--------------
EmcLSSMatrix::~EmcLSSMatrix() {
  if ( 0 != _matrix) 
    {
      delete [] _matrix;
      _matrix = 0;
    }
  if ( 0 != _rows) 
    {
      delete [] _rows;
      _rows = 0;
    }
  if ( 0 != _columns) 
    {
      delete [] _columns;
      _columns = 0;
    }
  if ( 0 != _nothing) 
    {
      delete _nothing;
      _nothing = 0;
    }
}

//-------------
// Operators --
//-------------
double&
EmcLSSMatrix::operator()(int row, int col) {

  long int found = find( row, col );
  
  
  if ( -1 == found ) 
    {
      if (_MsgFlag <= 5) {
	std::cout << "EmcLSSMatrix:: ERROR "
		  << "EmcLSSMatrix: matrix element not found !!" << endl
		  << "EmcLSSMatrix: Return zero !" << endl;
      }
      _nothing = 0;
      return *_nothing;
    }
  
  return (*(_matrix+found));
}


    
//-------------
// Selectors --
//-------------
long int
EmcLSSMatrix::find( int row, int col) 
{  
  int smaller,larger;
  
 
  if ( col > row ) 
    {
      smaller = row;
      larger = col; 
    }
  else 
    {
      smaller = col;
      larger = row;
    }
  
  int* col_p;
  int* row_p;
  double* matr_p;
  double* matr_row_max;

  if (larger < 0 || larger > _nrrows || smaller < 0 || smaller > _nrrows ) 
    {
      if (_MsgFlag <= 5) {
	std::cout <<"EmcLSSMatrix::ERROR"    
		  << "!!! ERROR in bound check of EmcLSSMatrix !!!"
		  << "!!! Return zero !!!" 
		  << endl;
      }
      matr_p = 0;
    }
  else 
    {     
      col_p = (_columns + (larger * _nrcol));
      row_p = (_rows + (larger * _nrcol));
      matr_p = (_matrix + (larger * _nrcol));
      matr_row_max = (matr_p + _nrcol);
      
      while ( matr_p < matr_row_max ) {
	
	if (_MsgFlag <= 1) {
	  std::cout <<"EmcLSSMatrix::VERBOSE" 
		    << "C: " << larger << " " << smaller << " "
		    << col_p << " " << *col_p << " " 
		    << matr_p << " " << *matr_p << " "
		    << (_matrix+(matr_p-_matrix)) << " "
		    << *(_matrix+(matr_p-_matrix)) << endl;
	}
	/*
	  cout<< "C: " << larger << " " << smaller << " "
	  << col_p << " " << *col_p << " " 
	  << matr_p << " " << *matr_p << " "
	  << (_matrix+(matr_p-_matrix)) << " "
	  << *(_matrix+(matr_p-_matrix)) <<endl;
	*/
	if ( matr_p == (matr_row_max-1) ) 
	  {
	    if (_MsgFlag <= 4) {
	      std::cout <<"EmcLSSMatrix::WARNING " 
			<< "!! WARNING: Reached maximum number of columns "
			<< "in LSSMatrix when searching for row " 
			<< larger << " column " << smaller << " !!" 
			<< endl
			<< "!! Return zero pointer !! " << endl;
	    }
	    matr_p = 0;
	    break;
	  }

	//if row does already exist
	if ( *col_p == smaller ) 
	  {
	    break;
	  }

	//if at the end of the list, use this as new element
	if ( (*matr_p == 0.) ) 
	  {
	    *col_p = smaller;
	    *row_p = larger;
	    // 	if (*row_p == 1616 ) {
	    // 	  nun=(row_p-(_rows + (larger * _nrcol)));
	    //if (_MsgFlag <= 2) {
	    //  std::cout <<"EmcLSSMatrix::DEBUG " 
	    //		<< nun << " " << larger << " " << smaller  << " "
	    //		<< *row_p << " " << *col_p << " " << *matr_p << endl;
	    //}
	    // 	}
	    break;
	  }
      
	matr_p++;
	col_p++;
	row_p++;
	
      }
    }
  
  long int diff = matr_p-_matrix;

  if (matr_p == 0) 
    {
      diff = -1;
    }
  
  return (diff);
}

double*
EmcLSSMatrix::matrix( const int& rowind ) const 
{
  double* here=0;
  
  if (rowind < _nrrows) 
    {
      here = (_matrix+(rowind*_nrcol));
    }
  else 
    {
      if (_MsgFlag <= 5) {
	std::cout <<"EmcLSSMatrix::ERROR " 
		  << "EmcLSSMatrix::matrix: Error "
		  << "- larger row index than existing requested !" 
		  << endl;
	    }
      here = 0;
    }
  return here;
}

int*
EmcLSSMatrix::column( const int& rowind ) const 
{
  int* here=0;
   
  if (rowind < _nrrows) 
    {
      here =  (_columns+(rowind*_nrcol));
    }
  else 
    {
      
      if (_MsgFlag <= 5) {
	std::cout <<"EmcLSSMatrix::ERROR " 
		  << "EmcLSSMatrix.column: Error "
		  << "- larger row index than existing requested !" 
		  << endl;
      }
      here = 0;
    }
  return here;
}

int
EmcLSSMatrix::num_filled_cols( const int row ) const 
{
  double * search_i = _matrix + ( row * _nrcol );
  double * max_i = search_i + _nrcol;
  int nonZeroCol=0;
  
  while ( (*search_i != 0.) && (search_i < max_i) ) 
    {
      nonZeroCol++;
      search_i++;
    }
  return nonZeroCol;
} 

int
EmcLSSMatrix::num_filled_rows( const int col ) const 
{
  int nonZeroRows = 0;
  for ( long int i=0; i<_size; i++ )
    { 
      if ( (_matrix[i] != 0.) && (_columns[i] == col) ) 
	{
	  nonZeroRows++;  
	}
    }

  return nonZeroRows;
}


long int
EmcLSSMatrix::num_nonZeros() 
{
  int* col_p = _columns;
  double* ele_p = _matrix;
  double* mat_max_p = (_matrix + _size);
  long int nrele = 0;
  
  while ( ele_p < mat_max_p ) 
    {
      if ( *ele_p != 0.) nrele++;
      col_p++;
      ele_p++;
    }
  
  return nrele;
}

//-------------
// Modifiers --
//-------------
void
EmcLSSMatrix::reset() 
{
  for (int i=0; i<_size;i++) 
    {
      _matrix[i] =0.;
      _rows[i] = -1;
      _columns[i] = -1;
    }

}

bool
EmcLSSMatrix::reduce_Matrix(int* xRef_list) 
{
  bool successful = true;

  //delete all zero elements in matrix
  //save only non zero elements

  long int _newIndx = 0;
  
  
  for ( long int _arrayIndx = 0; 
	_arrayIndx < _size; _arrayIndx++) 
    {
      
      //add 1 to matrix indices because SLAP wants indices 1..N,
      //but Xtals counting in geometry starts with 0
      
      if ( _matrix[_arrayIndx] > 0. ) 
	{
	  
	  if ( (xRef_list[(_rows[_arrayIndx])]) >= 0
	       && (xRef_list[(_columns[_arrayIndx])]) >= 0 ) 
	    {	      
	      _matrix[_newIndx] = _matrix[_arrayIndx];
	      _rows[_newIndx] = ((xRef_list[(_rows[_arrayIndx])])+1);
	      _columns[_newIndx] = ((xRef_list[(_columns[_arrayIndx])])+1);	  
	      _newIndx++;
	    } 
	  else 
	    {

	      //int indxtal;
	      if (xRef_list[(_rows[_arrayIndx])] < 0 ) 
		{
		  if (_MsgFlag <= 5) {
		    std::cout <<"EmcLSSMatrix::ERROR " 
			      << "EmcLSSMatrix:  Xtal index "
			      << _rows[_arrayIndx]
			      << " appears in matrix, " 
			      << "but not in vector !!! "
			      << _rows[_arrayIndx] << " "
			      << _columns[_arrayIndx]
			      << endl;
		  }
		} 
	      else 
		{
		  
		  if (_MsgFlag <= 5) {
		    std::cout <<"EmcLSSMatrix::ERROR " 
			      << "EmcLSSMatrix:  Xtal index "
			      << _columns[_arrayIndx]
			      << " appears in matrix, " 
			      << "but not in vector !!! "
			      << _rows[_arrayIndx] << " "
			      << _columns[_arrayIndx]
			      << endl;
		  }
		}
	      successful=false;
	    }
	}
    }

  if (_verb) 
    {
      if (_MsgFlag <= 2) {
	std::cout <<"EmcLSSMatrix::DEBUG " 
		  << "Reduced LSSMatrix !!! Number of non zeros: " 
		  << _newIndx << endl;
      }
    }
  
  return successful;
}


void
EmcLSSMatrix::print_NonZeros() 
{
  int* col_p = _columns;
  int* row_p = _rows;
  double* ele_p = _matrix;
  double* mat_max_p = (_matrix + _size);
  long int wo = 0;
  long int nrele = 0;


  if (_MsgFlag <= 2) {
    std::cout <<"EmcLSSMatrix::DEBUG "  
	      <<"List of nonzero Matrix elements (Matrix size: " 
	      << _size << " ) : " << endl;
  }
  
  while ( ele_p < mat_max_p ) 
    {
      if ( *ele_p != 0. ) 
	{
	  nrele++;
	  if (_MsgFlag <= 2) {
	    std::cout <<"EmcLSSMatrix::DEBUG " 
		      << "nr: " << nrele
		      << " M( " <<  *row_p << " ,  " << *col_p
		      << " ):  " << *ele_p 
		      << "   array index: " << wo << endl;
	  }
	  /*  
	      cout<< "nr: " << nrele
	      << " M( " <<  *row_p << " ,  " << *col_p
	      << " ):  " << *ele_p 
	      << "   array index: " << wo << endl;
	  */
	}
      wo++;
      col_p++;
      row_p++;
      ele_p++;
    }
  
}


void
EmcLSSMatrix::print_row(int rownr) 
{
  int* col_p =  _columns+(_nrcol*rownr);
  int* row_p = _rows+(_nrcol*rownr);
  double* ele_p = _matrix+(_nrcol*rownr);
  double* mat_max_p = (ele_p + _nrcol);
  long int wo = 0;
  long int nrele = 0;
  
  if (_MsgFlag <= 2) {
    std::cout <<"EmcLSSMatrix::DEBUG " 
	      <<"row length: " << num_filled_cols(rownr) << endl;
  }
  while ( ele_p < mat_max_p ) 
    {
      if ( *ele_p != 0. ) 
	{
	  nrele++;
	  if (_MsgFlag <= 2) {
	    std::cout <<"EmcLSSMatrix::DEBUG " 
		      << "nr: " << nrele
		      << " M( " <<  *row_p << " ,  " << *col_p
		      << " ):  " << *ele_p 
		      << "   array index: " << wo << endl;
	  }	  
	  /*
	    cout<< "nr: " << nrele
	    << " M( " <<  *row_p << " ,  " << *col_p
	    << " ):  " << *ele_p 
	    << "   array index: " << wo << endl;
	  */
	}
      wo++;
      col_p++;
      row_p++;
      ele_p++;
    }
  
}

void
EmcLSSMatrix::writeOut( ostream& Out) 
{
  int* col_p = _columns;
  int* row_p =  _rows;
  double* ele_p = _matrix;
  double* mat_max_p = (_matrix + _size);
  //  long int nrele = 0;

  long int nonz = num_nonZeros();
  Out << nonz << " ";
  
  while ( ele_p < mat_max_p ) 
    {
      if ( *ele_p != 0.) 
	{
	  Out<< *row_p << " " 
	     << *col_p << " " 
	     << *ele_p << " ";
	}
      col_p++;
      row_p++;
      ele_p++;
    }
  
  if (_MsgFlag <= 2) {
    std::cout <<"EmcLSSMatrix::DEBUG " 
	      << "Wrote " << nonz 
	      << " non zero matrix elements to file !!!" << endl;
  }
  
}


void
EmcLSSMatrix::readIn( istream& In) 
{  
  //  long int nonz = num_nonZeros();
  long int nonz = 0;
  In >> nonz;
  
  cout<<"nonz="<<nonz<<endl;
  
  int theRow;
  int theCol;
  long int index;
  double theEle;
  

  for (long int i=0; i<nonz; i++ ) 
    {
      In >> theRow >> theCol >> theEle;
      index = find (theRow,theCol);
      /*
      cout<<"index = "<<index
	  <<"row="<<theRow
	  <<"col="<<theCol<<endl;
      */
      if ( -1 != index ) 
	{
	  _matrix[index] += theEle;
	  if (_verb) 
	    {
	      if ( i < 50 || i > (nonz-10) )
		{
		  if (_MsgFlag <= 2) {
		    std::cout <<"EmcLSSMatrix::DEBUG " 
			      << "M: " << _rows[index] << " " << _columns[index]
			      << " " << _matrix[index] << " " << index << endl;
		  }   
		}
	    }
	}
    }
  
  if (_verb) 
    {
      if (_MsgFlag <= 2) {
	std::cout <<"EmcLSSMatrix::DEBUG " 
		  << "Have read in " << nonz 
		  << " non zero matrix elements from file !!!" << endl;
	    }
    }
}












