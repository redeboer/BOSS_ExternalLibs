//
//File: ExtMucHit.cxx
//Author: Wang LiangLiang 
//History: 2006.2.25 created by Wang LiangLiang
//                  

#include "ExtEvent/ExtMucHit.h"

ExtMucHit::ExtMucHit()
{
	Hep3Vector zeroVector(0.,0.,0.);
	myPosition = zeroVector;
	myMomentum = zeroVector;
	myVolumeName = "MUC";
	myVolumeNumber = -1;
	myPosSigmaAlongZ = 0.;
	myPosSigmaAlongT = 0.;
	myPosSigmaAlongX = 0.;
	myPosSigmaAlongY = 0.;
	HepSymMatrix zeroMatrix(6,0);
	myErrorMatrix = zeroMatrix;
}

ExtMucHit::~ExtMucHit() {}

void ExtMucHit::SetExtMucHit(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
		             int aVolumeNumber,HepSymMatrix aErrorMatrix,double aZSigma,
			     double aTSigma,double aXSigma,double aYSigma)	
{
	myPosition = aPosition;
	myMomentum = aMomentum;
	myVolumeName = aVolumeName;
	myVolumeNumber = aVolumeNumber;
	myPosSigmaAlongZ = aZSigma;
	myPosSigmaAlongT = aTSigma;
	myPosSigmaAlongX = aXSigma;
	myPosSigmaAlongY = aYSigma;
	myErrorMatrix = aErrorMatrix;
}
