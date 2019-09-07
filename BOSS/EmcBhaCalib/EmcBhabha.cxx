//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class EmcBhabha - see header
//           
// Environment:
//	Software developed for the BESIII Detector at the BEPCII.
//
// Author List:
//	ChunXiu Liu              IHEP        
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "EmcBhaCalib/EmcBhabha.h"

//---------------
// C++ Headers --
//---------------
#include <iostream>
#include <list>

using namespace std;
//----------------
// Constructors --
//----------------
EmcBhabha::EmcBhabha() 
  :m_calibEnergy(0),
   m_errCalibEnergy(0),
   m_shower(EmcShower()),
   m_theta(-1.),
   m_found(false),
   m_thetaIndex(9999),
   m_phiIndex(9999)
{

}

//copy constructor
EmcBhabha::EmcBhabha(const EmcBhabha& otherBhabha) {
  m_calibEnergy = otherBhabha.m_calibEnergy;
  m_errCalibEnergy = otherBhabha.m_errCalibEnergy;
  m_shower = otherBhabha.m_shower;
  m_theta = otherBhabha.m_theta;
  m_found = otherBhabha.m_found;
  m_thetaIndex = otherBhabha.m_thetaIndex;
  m_phiIndex = otherBhabha.m_phiIndex;
}

//--------------
// Destructor --
//--------------
EmcBhabha::~EmcBhabha() {
 
}

double
EmcBhabha::errorOnShowerEnergy() const {

  double sig=0,sig1=0,sig2=0;

  //energy measurement error of calorimeter 
  /*
  sig1 = 0.0034/shower().energy();
  sig1 *= sig1;
  sig2 = (0.0070/pow(shower().energy(),0.25));
  sig2 *= sig2;
  sig = sig1+sig2;
  sig += (0.0134*0.0134);
  sig = sqrt(sig) * shower().energy();
  */
  return sig;
}

double
EmcBhabha::sigma2() const{

  double sigSh=0., sigDepo=0., sig2=0.;

  //error on measured shower energy
  sigSh = errorOnShowerEnergy();
  
  //error of energy to calibrate on
  sigDepo = errorOnCalibEnergy();

  sig2 = (sigSh * sigSh) + (sigDepo * sigDepo);
  return sig2;
}

//Acessor
void 
EmcBhabha::print( ) {
  
  cout << "EmcBhabha: " << endl
       << "Energies:  calib: " << calibEnergy()
       << "  shower: " << m_shower.energy() 
       << " Theta: " << theta()
       << "  nr Xtals: " << m_shower.digiList().size() 
       << endl
       << "Errors: calibEne: " << errorOnCalibEnergy()
       << " shower: " << errorOnShowerEnergy()
       << " sum: " << sigma2() 
       << endl;

}

