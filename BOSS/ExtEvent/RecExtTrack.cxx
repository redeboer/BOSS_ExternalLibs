//
//File: ExtTrack.cxx
//Author: L.L.Wang
//
//Description: Output of track extrapolation results.
//
//History: 2005.7.5 created by L.L.Wang
//
//

#include "ExtEvent/RecExtTrack.h"

RecExtTrack& RecExtTrack::operator=(const DstExtTrack& aTrack)
{
  DstExtTrack::operator=(aTrack);
  return *this;
}

RecExtTrack::RecExtTrack(const DstExtTrack& aTrack)
:DstExtTrack(aTrack)
{
}
  
RecExtTrack::RecExtTrack()
{/*
	myTof1VolumeName="TOF1";
	myTof2VolumeName="TOF2";
	myEmcVolumeName="EMC";
	myMucVolumeName="MUC";
	Hep3Vector zeroVector(0.,0.,0.);
	myTof1Position = zeroVector;
	myTof1Momentum = zeroVector;
	myTof2Position = zeroVector;
	myTof2Momentum = zeroVector;
	myEmcPosition = zeroVector;
	myEmcMomentum = zeroVector;
	myMucPosition = zeroVector;
	myMucMomentum = zeroVector;

	myTof1VolumeNumber=-1;
	myTof2VolumeNumber=-1;
	myEmcVolumeNumber=-1;
	myMucVolumeNumber=-1;

	myTof1=0.;
  	myTof1Path=0.;
	myTof1PosSigmaAlongZ=0.;
	myTof1PosSigmaAlongT=0.;
	myTof1PosSigmaAlongX=0.;
	myTof1PosSigmaAlongY=0.;
	myTof2=0.;
	myTof2Path=0.;
	myTof2PosSigmaAlongZ=0.;
	myTof2PosSigmaAlongT=0.;
	myTof2PosSigmaAlongX=0.;
	myTof2PosSigmaAlongY=0.;
	myEmcPosSigmaAlongTheta=0.;
	myEmcPosSigmaAlongPhi=0.;
	myMucPosSigmaAlongZ=0.;
	myMucPosSigmaAlongT=0.;
	myMucPosSigmaAlongX=0.;
	myMucPosSigmaAlongY=0.;

	HepSymMatrix zeroMatrix(6,0);
	myTof1ErrorMatrix=zeroMatrix;
	myTof2ErrorMatrix=zeroMatrix;
	myEmcErrorMatrix=zeroMatrix;
	myMucErrorMatrix=zeroMatrix;

	myEmcPath = 0;*/
	
	//myPathInTof1 = 0.0;
	//myPathInTof2 = 0.0;

	vector<double> vd;
	ExtMucHitVec muHitVec;
	for(int i=0; i<5; i++)
	{
		myExtMucHitVec.push_back(muHitVec);
		myPathInTof1.push_back(vd);
		myPathInTof2.push_back(vd);
	}
}

RecExtTrack::~RecExtTrack(){}

/*
const int ExtTrack::GetTrackID() const {return myTrackID;}

//Tof layer1 data:
const Hep3Vector ExtTrack::GetTof1Position() const {return myTof1Position;}

const Hep3Vector ExtTrack::GetTof1Momentum() const {return myTof1Momentum;}

const string ExtTrack::GetTof1VolumeName() const {return myTof1VolumeName;}

const int ExtTrack::GetTof1VolumeNumber() const {return myTof1VolumeNumber;}

const double ExtTrack::GetTof1() const {return myTof1;}

const double ExtTrack::GetTof1Path() const {return myTof1Path;}

const double ExtTrack::GetTof1PosSigmaAlongZ() const {return myTof1PosSigmaAlongZ;}

const double ExtTrack::GetTof1PosSigmaAlongT() const {return myTof1PosSigmaAlongT;}

const double ExtTrack::GetTof1PosSigmaAlongX() const {return myTof1PosSigmaAlongX;}

const double ExtTrack::GetTof1PosSigmaAlongY() const {return myTof1PosSigmaAlongY;}

const HepSymMatrix ExtTrack::GetTof1ErrorMatrix() const {return myTof1ErrorMatrix;}

//Tof layer2 data:
const Hep3Vector ExtTrack::GetTof2Position() const {return myTof2Position;}

const Hep3Vector ExtTrack::GetTof2Momentum() const {return myTof2Momentum;}

const string ExtTrack::GetTof2VolumeName() const {return myTof2VolumeName;}

const int ExtTrack::GetTof2VolumeNumber() const {return myTof2VolumeNumber;}

const double ExtTrack::GetTof2() const {return myTof2;}

const double ExtTrack::GetTof2Path() const {return myTof2Path;}

const double ExtTrack::GetTof2PosSigmaAlongZ() const {return myTof2PosSigmaAlongZ;}

const double ExtTrack::GetTof2PosSigmaAlongT() const {return myTof2PosSigmaAlongT;}

const double ExtTrack::GetTof2PosSigmaAlongX() const {return myTof2PosSigmaAlongX;}

const double ExtTrack::GetTof2PosSigmaAlongY() const {return myTof2PosSigmaAlongY;}

const HepSymMatrix ExtTrack::GetTof2ErrorMatrix() const {return myTof2ErrorMatrix;}

//Emc data
const Hep3Vector ExtTrack::GetEmcPosition() const {return myEmcPosition;}

const Hep3Vector ExtTrack::GetEmcMomentum() const {return myEmcMomentum;}

const string ExtTrack::GetEmcVolumeName() const {return myEmcVolumeName;}

const int ExtTrack::GetEmcVolumeNumber() const {return myEmcVolumeNumber;}

const double ExtTrack::GetEmcPosSigmaAlongTheta() const {return myEmcPosSigmaAlongTheta;}

const double ExtTrack::GetEmcPosSigmaAlongPhi() const {return myEmcPosSigmaAlongPhi;}

const HepSymMatrix ExtTrack::GetEmcErrorMatrix() const {return myEmcErrorMatrix;}

//Muc data
const Hep3Vector ExtTrack::GetMucPosition() const {return myMucPosition;}

const Hep3Vector ExtTrack::GetMucMomentum() const {return myMucMomentum;}

const string ExtTrack::GetMucVolumeName() const {return myMucVolumeName;}

const int ExtTrack::GetMucVolumeNumber() const {return myMucVolumeNumber;}

const double ExtTrack::GetMucPosSigmaAlongZ() const {return myMucPosSigmaAlongZ;}

const double ExtTrack::GetMucPosSigmaAlongT() const {return myMucPosSigmaAlongT;}

const double ExtTrack::GetMucPosSigmaAlongX() const {return myMucPosSigmaAlongX;}

const double ExtTrack::GetMucPosSigmaAlongY() const {return myMucPosSigmaAlongY;}

const HepSymMatrix ExtTrack::GetMucErrorMatrix() const {return myMucErrorMatrix;}



//Set
void ExtTrack::SetTrackID(int aTrackID) {myTrackID = aTrackID;}

void ExtTrack::SetTof1Data(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
		int aVolumeNumber,double aTof,double aPath,HepSymMatrix aErrorMatrix,
		
		
		double aZSigma,double aTSigma,double aXSigma,double aYSigma)
{
   myTof1Position = aPosition;
   myTof1Momentum = aMomentum;
   myTof1VolumeName = aVolumeName;
   myTof1VolumeNumber = aVolumeNumber;
   myTof1 = aTof;
   myTof1Path = aPath;
   myTof1PosSigmaAlongZ = aZSigma;
   myTof1PosSigmaAlongT = aTSigma;
   myTof1PosSigmaAlongX = aXSigma;
   myTof1PosSigmaAlongY = aYSigma;
   myTof1ErrorMatrix = aErrorMatrix;
}

void ExtTrack::SetTof2Data(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
		                int aVolumeNumber,double aTof,double aPath,HepSymMatrix aErrorMatrix,
				                double aZSigma,double aTSigma,double aXSigma,double aYSigma)
{
	   myTof2Position = aPosition;
	   myTof2Momentum = aMomentum;
	   myTof2VolumeName = aVolumeName;
	   myTof2VolumeNumber = aVolumeNumber;
	   myTof2 = aTof;
	   myTof2Path = aPath;
	   myTof2PosSigmaAlongZ = aZSigma;
	   myTof2PosSigmaAlongT = aTSigma;
	   myTof2PosSigmaAlongX = aXSigma;
	   myTof2PosSigmaAlongY = aYSigma;
	   myTof2ErrorMatrix = aErrorMatrix;
}

void ExtTrack::SetEmcData(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
		int aVolumeNumber,double aThetaSigma,double aPhiSigma,
		HepSymMatrix aErrorMatrix)
{
   myEmcPosition = aPosition;
   myEmcMomentum = aMomentum;
   myEmcVolumeName = aVolumeName;
   myEmcVolumeNumber = aVolumeNumber;
   myEmcPosSigmaAlongTheta = aThetaSigma;
   myEmcPosSigmaAlongPhi = aPhiSigma;
   myEmcErrorMatrix = aErrorMatrix;
}

void ExtTrack::SetMucData(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
			  int aVolumeNumber,HepSymMatrix aErrorMatrix,double aZSigma,
			  double aTSigma,double aXSigma,double aYSigma)
{
   myMucPosition = aPosition;
   myMucMomentum = aMomentum;
   myMucVolumeName = aVolumeName;
   myMucVolumeNumber = aVolumeNumber;
   myMucPosSigmaAlongZ = aZSigma;
   myMucPosSigmaAlongT = aTSigma;
   myMucPosSigmaAlongX = aXSigma;
   myMucPosSigmaAlongY = aYSigma;
   myMucErrorMatrix = aErrorMatrix;
}
*/
