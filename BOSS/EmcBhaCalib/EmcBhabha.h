//--------------------------------------------------------------------------
// Environment:
//      This software was developed for the BESIII collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//     ChunXiu Liu     IHEP
//
//------------------------------------------------------------------------

#ifndef EMCBHABHA_H
#define EMCBHABHA_H
#include "EmcBhaCalib/EmcShDigi.h"
#include "EmcBhaCalib/EmcShower.h"
#include <iostream>
using namespace std;

//		---------------------
// 		-- Class Interface --
//		---------------------

/**
 *  This class holds the information of one Bhabha particle like.
 *  the energy, the shower  ...
 *  It is filled in SelectBhabha and read out in CollectBhabha
 *  to fill the calibration matrix and vector.
 */

class EmcBhabha 
{
 public:

  // Constructors
  EmcBhabha();

  // copy 
  EmcBhabha(const EmcBhabha& otherBhabha);

  // Destructor
  ~EmcBhabha( );

  // Was the particle found in event ? 
  const bool& found() {return m_found;}

  // the energy to calibrate on 
  const double& calibEnergy() const {return m_calibEnergy;}

  // the error on the energy to calibrate on 
  const double& errorOnCalibEnergy() const {return m_errCalibEnergy;}

  //error on the measured shower energy 
  double errorOnShowerEnergy() const; 
  
  //the cluster of this bhabha 
  EmcShower shower() const {return m_shower;}

  // its theta 

  const double& theta() const 
    {
      if ( m_theta < 0 )
	{
	  cout<<"warning: " << " EmcBhabha::theta not yet set !" << endl;
	}
      return m_theta;
    }
  
  const double& phi() const 
    {
      return m_phi;
    }

  const unsigned int & thetaIndex() const 
    {      
      return m_thetaIndex;
    }

  const unsigned int& phiIndex() const 
    {
      return m_phiIndex;
    }
  
  void print();

  //this is the error on the Bhabha (measurement error + 
  //  error on the energy to calibrate on)
  double sigma2() const;

  void setFound(bool what) {m_found=what;}
  void setCalibEnergy(double energy) {m_calibEnergy=energy;}
  void setErrorOnCalibEnergy(double error) { m_errCalibEnergy = error;}
  void setShower(EmcShower aShower){ m_shower=aShower;}
  void setTheta(double theta) {m_theta=theta;}
  void setPhi(double phi) {m_phi=phi;}
  void setThetaIndex(unsigned int thetaIndex) {m_thetaIndex=thetaIndex;}
  void setPhiIndex(unsigned int phiIndex) {m_phiIndex=phiIndex;}

private:

  // the energy to calibrate on (either the true or the deposited cluster energy 
  double m_calibEnergy;

  // error of the energy to calibrate on  
  double m_errCalibEnergy;

  // shower of bhabha particle 
  EmcShower m_shower;

  // theta of Bhabha 
  double m_theta;

  double m_phi;

  // was Bhabha found in event ?
  bool m_found;

  // the theta index of the shower 
  //  from east endcap(0-5) ,barrel(6-49) to west endcap (50-55) 
  unsigned int  m_thetaIndex;

  // the phi index of the shower
  unsigned int m_phiIndex;
};

#endif  //EMCBHABHA_H
