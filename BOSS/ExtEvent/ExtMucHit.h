//
//File: ExtMucHit.h
//Author: Wang LiangLiang 
//History: 2006.2.25 created by Wang LiangLiang
//

#ifndef EXTMUCHIT_H
#define EXTMUCHIT_H

using namespace std;

#include "GaudiKernel/ObjectVector.h"

#include "CLHEP/Matrix/Vector.h" 
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"

using namespace CLHEP;

class ExtMucHit
{
	public:
		ExtMucHit();
		~ExtMucHit();
		Hep3Vector GetPosition() const {return myPosition;};
		Hep3Vector GetMomentum() const {return myMomentum;};
		string GetVolumeName() const {return myVolumeName;};
		int GetVolumeNumber() const {return myVolumeNumber;};
		double GetPosSigmaAlongZ() const {return myPosSigmaAlongZ;};
		double GetPosSigmaAlongT() const {return myPosSigmaAlongT;};
		double GetPosSigmaAlongX() const {return myPosSigmaAlongX;};
		double GetPosSigmaAlongY() const {return myPosSigmaAlongY;};
		HepSymMatrix GetErrorMatrix() const {return myErrorMatrix;};
		void SetExtMucHit(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
				  int aVolumeNumber,HepSymMatrix aErrorMatrix,double aZSigma=0.,
				  double aTSigma=0.,double aXSigma=0.,double aYSigma=0.);
	private:
		Hep3Vector myPosition;
		Hep3Vector myMomentum;
		string myVolumeName;
		int myVolumeNumber;
		double myPosSigmaAlongZ;
		double myPosSigmaAlongT;
		double myPosSigmaAlongX;
		double myPosSigmaAlongY;
		HepSymMatrix myErrorMatrix;
		
};

typedef vector<ExtMucHit> ExtMucHitVec;

#endif
