//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcSuperLayer.cxx,v 1.3 2010/03/24 00:40:06 zhangy Exp $
//
// Description:
//	Class DchSuperLayer
//      Do not use this for DchSuperLayerd class (foo<T>).  use DchSuperLayerDchSuperLayer.hh
//      instead.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	R. Stroili			originator
//	Zhang Yao(zhangyao@ihep.ac.cn)  Migrate to BESIII
//	
//
// Copyright Information:
//	Copyright (C) 1997	INFN - Pd
//
//------------------------------------------------------------------------
//#include "BaBar/BaBar.hh"

//-----------------------
// This Class's Header --
//-----------------------
#include "MdcGeom/MdcSuperLayer.h"
#include <iomanip>
#include <iostream>
using namespace std;

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <assert.h>
using std::endl;
using std::ostream;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

static const char rscid[] = "$Id: MdcSuperLayer.cxx,v 1.3 2010/03/24 00:40:06 zhangy Exp $";
static const int _layInSuper = 4;

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

//----------------
// Constructors --
//----------------
MdcSuperLayer::MdcSuperLayer( int number )
{
  _radius = _delphi = _delphiinv = 0;
  layers[0] = layers[1] = layers[2] = layers[3] = 0;
  _nLayers = 0;
  _view = 0;
  _next = _prev = _nextInView = _prevInView = 0;
  _exist = false;
  _slayer = number;
}

//--------------
// Destructor --
//--------------
MdcSuperLayer::~MdcSuperLayer() {
  //  delete [] layers;
}

//-------------
// Modifiers --
//-------------
void 
MdcSuperLayer::addLayer(int index, const MdcLayer* lay) {
  //---------------------------------------------------------- 
  // here |index| is the index of array of pointers to layers
  // belonging to the superlayer, so this ramges from 0 to 3
  //----------------------------------------------------------
  // check on index number
  assert ( index>=0 && index <4); 
  // check that it was not already set
  assert ( layer(index) == 0 );
  // chack that layer stays in this superlayer
  //assert ( (int)((lay->layNum()-1)/_layInSuper+1) == slayNum() );

  //   lay->setSlayer(this);
  layers[index] = lay;
  _nLayers++;
}

void 
MdcSuperLayer::updateInfo( const MdcSuperLayer* prev, 
    const MdcSuperLayer* next ) {
  //
  // function to set the data-members of this class
  //
  _exist = true;
  _radius = 0.5 * (firstLayer()->rEnd() + lastLayer()->rEnd());
  _view = firstLayer()->view();
  _delphi = firstLayer()->dPhiz();
  _delphiinv = 0.0;
  if ( _delphi != 0. ) _delphiinv = 1./_delphi;
  // now the pointers
  _next = next;
  _prev = prev;
}

void
MdcSuperLayer::print(ostream& o) const 
{
  o<< setw(3)<<index()<<
    setw(3)<<whichView()<<
    setw(3)<<exist()<<    setw(10)<<rEnd()<<
    setw(10)<<rad0()<<
    setw(10)<<zEnd()<<
    setw(10)<<stDip()<<
    setw(10)<<delPhi()<<
    setw(10)<<delPhiinv()<<
    setw(3)<<slayNum()<<
    setw(3)<<nLayers()<<
    std::endl;
}

ostream&  operator<<(ostream& o, MdcSuperLayer& sl) {
  sl.print(o);
  return o;
}
