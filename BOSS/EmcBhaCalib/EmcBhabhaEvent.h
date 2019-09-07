//--------------------------------------------------------------------------
// Environment:
//      This software was developed for the BESIII collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//	ChunXiu Liu              IHEP
//
//------------------------------------------------------------------------

#ifndef EMCBHABHAEVENT_H
#define EMCBHABHAEVENT_H

#include "EmcBhaCalib/EmcBhabha.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <list>
#include <iostream>
#include "EmcGeneralClass/EmcStructure.h"
#include "EmcBhaCalib/EmcShower.h"
using namespace std;
using CLHEP::Hep3Vector;


//		---------------------
// 		-- Class Interface --
//		---------------------

 /**
 *  This class holds a Bhabha event, a electron and/or a positron.
 *  It also knows about the true and the expected deposited energies
 *  of Bhabhas. 
 *???whether need to do the energy correction of a Bhabha due to
 *  the tilt of BES3 to the beam line with 11mrad.
 */

class EmcBhabhaEvent 
{
 public:

  enum Selection{Nothing = 0,
	 	 MDCOneProng=1,
		 MDCTwoProngMatched=2,
		 MDCTwoProngOneMatched=3,
		 EmcOneProng=4,
		 EmcTwoProng=5 };
  
  // Constructor, puts a list of the two particles in the event 
  EmcBhabhaEvent( );
  
  ~EmcBhabhaEvent( );

  // Accessor to positron 
  EmcBhabha* positron()  const {return m_positron;}
  
  // Accessor to electron 
  EmcBhabha* electron()  const {return m_electron;}

  EmcBhabha* setPositron() {return m_positron;}

  EmcBhabha* setElectron() {return m_electron;}


  //load static data like the deposited MC energies ... 
  void initData();

  //delete the static data at the end 
  void deleteData();
  
  // print out event information   
  void print();

  //
  /*double getDepExpShowerEnergy(unsigned int thetaIndex,
			       unsigned int phiIndex,
			       double ePeakEne) const ;
  */
  //get the expected deposited shower energy of a Bhabha 
  double getDepoMCShowerEnergy(unsigned int thetaIndex,
			       unsigned int phiIndex,
			       double ePeak,
			       double beamEnergy) const;


  double getDepoMCShowerEnergy_lab(double theta,
				   double phi,
				   unsigned int thetaIndex,
				   unsigned int phiIndex,
				   double ePeak,
				   double beamEnergy) const;

  //get the error on the expected deposited shower energy of a Bhabha 
  double getErrorDepoMCShowerEnergy(unsigned int thetaIndex, 
				    unsigned int phiIndex,
				    double eSigma) const;

  double enLeakageTheta(double theta);

  double enLeakageThetaErr(double theta);
  
  static int& selectedMDCType()
    {
      return m_selectedMDCType; 
    }
  
  static int& selectedEmcType()
    {
      return m_selectedEmcType; 
    }
  /*
  static void setElectronBeamEnergy(double ene)
    { 
      m_eBeamEnergy = ene; 
    }

  static double& electronBeamEnergy() 
    { 
      return m_eBeamEnergy; 
    }
  
  static void setPositronBeamEnergy(double ene)
    { 
      m_pBeamEnergy = ene; 
    }
 
  static double& positronBeamEnergy() 
    { 
      return m_pBeamEnergy; 
    }
  */
  Hep3Vector  showerVector( EmcShower theShower);

private:

  //std::list<EmcBhabha> m_bhaEvList;

  static bool m_initialized;

  static int m_selectedMDCType;
  static int m_selectedEmcType;

  //static double m_eBeamEnergy;
  //static double m_pBeamEnergy;
  
  EmcBhabha* m_positron;
  EmcBhabha* m_electron;

};

#endif //EMCBHABHAEVENT_H
