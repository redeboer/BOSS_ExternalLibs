
#include "EmcGeneralClass/EmcStructure.h"
#include <vector>
#include <iostream>

using namespace std;

//----------------
// Constructors --
//----------------
EmcStructure::EmcStructure()
{

}

//--------------
// Destructor --
//--------------
EmcStructure::~EmcStructure()
{
}

void EmcStructure::setEmcStruc()
{
  int index;
  int nrOfTheRings = getNumberOfTheRings();
  for ( int the = startingTheta(); the< nrOfTheRings; the++) {
       
    for ( int phi=0; phi < crystalsInRing((unsigned int) the ); phi++) {
      
      index = getIndex( (unsigned int)the , (unsigned int)phi);

      m_phiIndex[index]=phi;
      if (the>=0 && the<=5) {
	m_partID[index]=0;
	m_thetaIndex[index]=the;
      }
      if (the>=6 && the<=49) {
	m_partID[index]=1;
	m_thetaIndex[index]=the-6;
      }
      if (the>=50 && the<=55){ 
	m_partID[index]=2;
	m_thetaIndex[index]=55 - the;
      }

    }
    m_ThetaMaxIndex[the]=index;
  }

}
int 
EmcStructure::getGeomIndex( unsigned int PartId, 
			    unsigned int ThetaIndex, 
			    unsigned int PhiIndex)  const
{
  if (PartId>2){
    cout<<"PartId is out of EMC"<<endl;
    return -1;
  } else {
    unsigned int newThetaIndex;
    //The newThetaIndex is defined by Endcap_east(0-5),Barrel(6-49),Endcap_west(50-55)
    if (PartId==0) newThetaIndex = ThetaIndex;
    if (PartId==1) newThetaIndex = ThetaIndex + 6;
    if (PartId==2) newThetaIndex = 55 - ThetaIndex;
    
    if ( isOutofAccep(newThetaIndex, PhiIndex) ) 
      {
	return -1;
      } else {
      
	int index;   
	index = getIndex(newThetaIndex,PhiIndex);
      
	return index;
      }
  }
}

long 
EmcStructure::getIndex(unsigned int  thetaIndex, unsigned int phiIndex ) const
{
  long index = -1;
  if(thetaIndex==0){
    index=phiIndex;
  } else {
    index = getThisThetaMaxIndex( thetaIndex-1 ) +1 + phiIndex;
  }
  return index;
}

bool 
EmcStructure::isOutofAccep(unsigned int thetaIndex, 
			   unsigned int phiIndex) const
{
  if ( thetaIndex > 55 ) 
    {
      cout<<"warning " << " theta out of acceptance !"
	  << endl;
      return true;  
      
    } 
  else 
    {
      if ( ((thetaIndex==0 || thetaIndex==1 || thetaIndex==55 || thetaIndex==54)
	    && (phiIndex > 63))
	   || ((thetaIndex==2 || thetaIndex==3 || thetaIndex==53 || thetaIndex==52)
	       && (phiIndex > 79))
	   || ((thetaIndex==4 || thetaIndex==5 || thetaIndex==51 || thetaIndex==50)
	       && ( phiIndex > 95))
	   || ((thetaIndex>=6 && thetaIndex<=49) && ( phiIndex > 119)) ) 
	{      
	  cout<<"warning "<< " phi out of acceptance !" << endl;
	  return true;	  
	}  
      else 
	{
	  return false;
	} 
    }
}

unsigned int 
EmcStructure::getNumberOfTheRings()
{  
  unsigned int numberOfTheRings=0;
  numberOfTheRings  = (aEmcId->getTHETA_BARREL_MAX() + 1) 
    + (aEmcId->getTHETA_ENDCAP_MAX() + 1 ) * 2;
  
  return  numberOfTheRings;
  
}
unsigned int 
EmcStructure::getNumberOfXtals()
{
  unsigned int numberOfXtals=0;
  unsigned int numberOfBarrelXtals, numberOfEndcapXtals;

  numberOfEndcapXtals = numberOfOneEndcapRings * (aEmcId->getPHI_ENDCAP_MAX(0)+1)
    + numberOfTwoEndcapRings * (aEmcId->getPHI_ENDCAP_MAX(2)+1)
    + numberOfThreeEndcapRings * (aEmcId->getPHI_ENDCAP_MAX(4)+1);

  numberOfBarrelXtals = (aEmcId->getTHETA_BARREL_MAX()+1)*(aEmcId->getPHI_BARREL_MAX()+1);
  
  numberOfXtals = numberOfBarrelXtals + numberOfEndcapXtals * 2;
  
  return numberOfXtals;
}

unsigned int
EmcStructure::crystalsInRing( unsigned int theta ) const
{
  //The theta index is defined by Endcap_east(0-5),Barrel(6-49),Endcap_west(50-55)
  //in Emc Bhabha Calibration
  unsigned int theXtalInRing;

  if (theta==0 || theta==1 || theta==55 || theta==54) 
    {
      theXtalInRing= aEmcId->getPHI_ENDCAP_MAX(0)+1;
    }

  if (theta==2 || theta==3 || theta==53 || theta==52) 
    {
      theXtalInRing=aEmcId->getPHI_ENDCAP_MAX(2)+1;
    }

  if (theta==4 || theta==5 || theta==51 || theta==50) 
    {
      theXtalInRing=aEmcId->getPHI_ENDCAP_MAX(4)+1;
    }

  if (theta>=6 && theta<=49) 
    {
      theXtalInRing=aEmcId->getPHI_BARREL_MAX()+1;
    }

  //  if (theta<0 || theta>55) theXtalInRing= 0;
  if (theta>55) 
    {
      theXtalInRing= 0;
    }

  return theXtalInRing;
}
