//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class EmcBhabhaEvent
//           
// Environment:
//	Software developed for the BESIII Detector at the BEPCII.
//
// Author List:
//	ChunXiu Liu          IHEP
//
//------------------------------------------------------------------------
//-----------------------
// This Class's Header --
//-----------------------

#include "EmcBhaCalib/EmcBhabhaEvent.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
//---------------
// C Headers --
//---------------
#include <cassert>
//---------------
// C++ Headers --
//---------------
#include <fstream>
//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//static elements
bool EmcBhabhaEvent::m_initialized = true;
int EmcBhabhaEvent::m_selectedMDCType = -1;
int EmcBhabhaEvent::m_selectedEmcType = -1;
//double EmcBhabhaEvent::m_eBeamEnergy = 1.548;
//double EmcBhabhaEvent::m_pBeamEnergy = 1.548;

//----------------
// Constructors --
//----------------
EmcBhabhaEvent::EmcBhabhaEvent()
{   
  m_positron = new EmcBhabha();    
  assert( 0 !=  m_positron );
  m_electron = new EmcBhabha();
  assert( 0 != m_electron );
  /*
   if (m_bhaEvList.empty()) {
 
    m_bhaEvList.push_back(*m_positron);
    m_bhaEvList.push_back(*m_electron);
  
  }
  else {
    if (m_bhaEvList.size() != 2) {
      cout<<"error " << "ERROR EmcBhabhaEvent: Length of event list != 2 !" 
                    << endl
                    << "      EmcBhabhaEvent: Length = " 
                    << m_bhaEvList.size() 
                    << endl;
    }
  }
  */
  initData();
}

//--------------
// Destructor --
//--------------
EmcBhabhaEvent::~EmcBhabhaEvent() {

  delete m_positron;
  delete m_electron;

}

//Modifiers
void
EmcBhabhaEvent::initData() 
{
  m_initialized = true;  
}

void
EmcBhabhaEvent::deleteData() 
{
}  

//Accessor
void
EmcBhabhaEvent::print() {

  if ( positron()->found() ) 
    {
      cout << "Positron: " << endl;
      positron()->print( );
    }
  else 
    {
      cout<<"routine "<< "No positron found ! " << endl;
    }

  if ( electron()->found() ) 
    {
      cout << "Electron: " << endl;
      electron()->print( );
    }
  else 
    {
      cout<<"routine "<< "No electron found ! " << endl;
    }
}


/*
double 
EmcBhabhaEvent::getDepExpShowerEnergy(unsigned int thetaIndex,
				      unsigned int phiIndex,
				      double ePeakEne) const {

  if ( ! m_initialized ) 
    {
      cout<<"error "<< " EmcBhabhaEvent not initialized ! " << endl
	  << " Is EmcLoadBhabhaData included in the path ? "
	  << endl;
      return 0;
    }
  
  //the deposited energy
  double depoEne  = 0.;

  EmcStructure* theEmcStruc=new EmcStructure();


  if (theEmcStruc->isOutofAccep(thetaIndex, phiIndex ))
    {   
      cout<<"warning "<< "EmcBhabhaEvent: Theta " << thetaIndex
	  << " or phi " << phiIndex
	  << " out of the calorimeter acceptance !"
	  << "Return 0 !"
	  << endl; 
      delete theEmcStruc;
      return 0;

    }   
  else 
    {
       depoEne  =ePeakEne;
      

      delete theEmcStruc;
      return depoEne;

    }
  
}

*/

double 
EmcBhabhaEvent::getDepoMCShowerEnergy(unsigned int thetaIndex,
				      unsigned int phiIndex,
				      double ePeak,
				      double beamEnergy) const {

  if ( ! m_initialized ) 
    {
      cout<<"error "<< " EmcBhabhaEvent not initialized ! " << endl
	  << " Is EmcLoadBhabhaData included in the path ? "
	  << endl;
      return 0;
    }
  
  //the deposited energy
  double depoEne  = 0.;

  EmcStructure* theEmcStruc=new EmcStructure();


  if (theEmcStruc->isOutofAccep(thetaIndex, phiIndex ))
    {   
      cout<<"warning "<< "EmcBhabhaEvent: Theta " << thetaIndex
	  << " or phi " << phiIndex
	  << " out of the calorimeter acceptance !"
	  << "Return 0 !"
	  << endl; 
      delete theEmcStruc;
      return 0;

    }   
  else 
    {
      depoEne  =ePeak*beamEnergy;
      
  
      delete theEmcStruc;
      return depoEne;

    }
  
}


double 
EmcBhabhaEvent::getDepoMCShowerEnergy_lab(double theta,
					  double phi,
					  unsigned int thetaIndex,
					  unsigned int phiIndex,
					  double ePeak,
					  double beamEnergy) const {

  if ( ! m_initialized ) 
    {
      cout<<"error "<< " EmcBhabhaEvent not initialized ! " << endl
	  << " Is EmcLoadBhabhaData included in the path ? "
	  << endl;
      return 0;
    }
  
  //the deposited energy
  double depoEne  = 0.;

  EmcStructure* theEmcStruc=new EmcStructure();


  if (theEmcStruc->isOutofAccep(thetaIndex, phiIndex ))
    {   
      cout<<"warning "<< "EmcBhabhaEvent: Theta " << thetaIndex
	  << " or phi " << phiIndex
	  << " out of the calorimeter acceptance !"
	  << "Return 0 !"
	  << endl; 
      delete theEmcStruc;
      return 0;

    }   
  else 
    {
       
      HepLorentzVector ptrk;
      ptrk.setPx(beamEnergy*sin(theta)*cos(phi));
      ptrk.setPy(beamEnergy*sin(theta)*sin(phi));
      ptrk.setPz(beamEnergy*cos(theta));
      ptrk.setE(beamEnergy);
      
      ptrk=ptrk.boost(0.011,0,0); //boost from cms to lab
      //double the_lab = ptrk.theta();
      //double phi_lab = ptrk.phi();
      double depoEne_lab  =ePeak*ptrk.e();
      delete theEmcStruc;
      return depoEne_lab;

    }
  
}

double 
EmcBhabhaEvent::getErrorDepoMCShowerEnergy(unsigned int thetaIndex,
					   unsigned int phiIndex,
					   double eSigma) const {

  if ( ! m_initialized ) 
    {
      cout<<"error "<< " EmcBhabhaEvent not initialized ! " << endl
	  << " Is EmcLoadBhabhaData included in the path ? "
	  << endl;
      return 0;
    } 

  double sig=0.; 
  EmcStructure* theEmcStruc=new EmcStructure();

  if (theEmcStruc->isOutofAccep(thetaIndex, phiIndex ))
    {   
      cout<<"warning "<< "EmcBhabhaEvent: Theta " << thetaIndex
	  << " or phi " << phiIndex
	  << " out of the calorimeter acceptance !"
	  << "Return 0 !"
	  << endl; 

      delete theEmcStruc;
      return 0;

    } 
  else 
    {
      sig=eSigma;
      delete theEmcStruc;
      return sig; 
   
    }

}

double 
EmcBhabhaEvent::enLeakageTheta(double theta)
{
  return  1.0;
}

double 
EmcBhabhaEvent::enLeakageThetaErr(double theta)
{
  return 0.0;
}

Hep3Vector
EmcBhabhaEvent::showerVector( EmcShower theShower)
{
  Hep3Vector theShowerVector(1,1,1);
  theShowerVector.setTheta(theShower.theta());
  theShowerVector.setPhi(theShower.phi());
  theShowerVector.setMag(theShower.energy());
  
  return theShowerVector;
}
