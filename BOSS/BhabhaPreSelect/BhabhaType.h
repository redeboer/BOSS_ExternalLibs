//--------------------------------------------------------------------------
// Environment:
//      This software was developed for the BESIII collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//	ChunXiu Liu              IHEP
//
//------------------------------------------------------------------------

#ifndef BHABHATYPE_H
#define BHABHATYPE_H

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <list>
#include <iostream>
using namespace std;
using CLHEP::Hep3Vector;


//		---------------------
// 		-- Class Interface --
//		---------------------

 /**
 *  This class holds a Bhabha type, a electron and/or a positron.
 *  It also knows about the true and the expected deposited energies
 *  of Bhabhas. 
 *???whether need to do the energy correction of a Bhabha due to
 *  the tilt of BES3 to the beam line with 11mrad.
 */

class BhabhaType 
{
 public:

  enum Selection{Nothing = 0,
	 	 OneProng=1,
		 TwoProngMatched=2,
		 TwoProngOneMatched=3};

  // Constructor, puts a list of the two particles in the event 
  BhabhaType( );
  
  ~BhabhaType( );

  void setSelectedType( int selectedType) 
    {
      m_selectedType = selectedType;
    }

  int selectedType() const
    {
      return m_selectedType; 
    }


  // print out event information   
  // void print();

private:

  int  m_selectedType ;
 
};

#endif //BHABHATYPE_H

