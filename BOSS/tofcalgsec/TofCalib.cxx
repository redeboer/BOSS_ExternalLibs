#include "tofcalgsec/TofCalib.h"
#include <iostream>
#include <ios>
#include <fstream>

using namespace CLHEP;

void TofCalib::reset() {
  F = HepMatrix(Npar,Npar,0);
  Y = HepVector(Npar,0);
  X = HepVector(Npar,0);
  funcs  = HepVector(Npar,0);
  return;
}


void TofCalib::calculate( RecordSet*& data, unsigned int icounter ) {

  std::cout << setiosflags(ios::left) << setw(10) << icounter << setw(8) << data->size() << setw(30) << name() << std::endl;

  if( data->size() <= 0 ) {
    X = HepVector(Npar,1.0);
  }
  else {
    std::vector<Record*>::iterator iter = data->begin();
    for( ; iter!=data->end(); iter++ ) {
      calculate_funcs( (*iter) );
      for( int i=0; i<F.num_col(); i++ ) {
	for( int j=0; j<F.num_col(); j++ ) {
	  F[i][j]+=funcs[i]*funcs[j];
	}
      }
      
      calculate_y( (*iter) );
      for( int k=0; k<Y.num_row(); k++ ) {
	Y[k]+=y*funcs[k];
      }
    }

    //    if( abs(F.determinant())>0. ) {
    if( abs(F[0][0])>0. ) {
      X = (qr_solve(F,Y));
    }
    else {
      X = HepVector(Npar,1.0);    // one side has no data
    }
  }

  m_result.push_back(X);

  updateData( data );

  return;
}


void TofCalib::fillTxt( const char* file ) {
  std::ofstream out(file,ios::out);
  if( out ) {
    std::vector<HepVector>::iterator it;
    for( it=m_result.begin(); it!=m_result.end(); it++ ) {
      for( int i=0; i<(*it).num_row(); i++ ) {
	out << (*it)[i] << " ";
      }
      out << std::endl;
    }
    out.close();
  }
  else{
    cerr << "error when open file " << file << " for write in " << name() << "::fillTxt()" << std::endl;
    cout << "print all parameters to srceen: in total " << m_result.size() << " items" << std::endl;
    std::vector<HepVector>::iterator it;
    for( it=m_result.begin(); it!=m_result.end(); it++ ) {
      for( int i=0; i<(*it).num_row(); i++ ) {
	cout << (*it)[i] << " ";
      }
      cout << std::endl;
    }
  }

  return;
}
