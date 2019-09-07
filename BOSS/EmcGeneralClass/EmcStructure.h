#ifndef EMCSTRUCTURE_H
#define EMCSTRUCTURE_H
#include "Identifier/EmcID.h"

class EmcStructure 
{
public:

  // Constructors
  EmcStructure();

  // Destructors
  ~EmcStructure();

  long getIndex( unsigned int thetaIndex, unsigned int phiIndex ) const;

  bool isOutofAccep(unsigned int thetaIndex,unsigned int phiIndex) const;

  //number Of Theta Rings (from 0 to 55)
  unsigned int getNumberOfTheRings();

  unsigned int getNumberOfXtals();

  //The theta index is defined by Endcap_east(0-5),Barrel(6-49),Endcap_west(50-55)
  //in Emc Bhabha Calibration
  unsigned int startingTheta() {return 0;}

  unsigned int crystalsInRing( unsigned int theta ) const;

 // First call setEmcStruc(), and then getThisThetaMaxIndex,
 // getPartId. getTheta, getPhi, or getGeomIndex can use. 
  void setEmcStruc(); 
 
  long getThisThetaMaxIndex(int Theta) const {return m_ThetaMaxIndex[Theta];}
  unsigned int getPartId( long Index) const {return m_partID[Index];}
  unsigned int getTheta( long Index) const {return m_thetaIndex[Index];}
  unsigned int getPhi( long Index) const {return m_phiIndex[Index];}
  int getGeomIndex( unsigned int PartId, 
		    unsigned int ThetaIndex, 
		    unsigned int PhiIndex) const;

 private:
  
  EmcID* aEmcId;
  static const unsigned int   numberOfOneEndcapRings = 2;
  static const unsigned int   numberOfTwoEndcapRings = 2;
  static const unsigned int   numberOfThreeEndcapRings = 2;
  long m_ThetaMaxIndex[56];
  unsigned int m_partID[6240];
  unsigned int m_thetaIndex[6240];
  unsigned int m_phiIndex[6240];

};

#endif  // EMCSTRUCTURE_H

