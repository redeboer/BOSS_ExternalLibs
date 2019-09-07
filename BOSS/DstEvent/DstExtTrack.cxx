//
//File: DstExtTrack.cxx
//Author: L.L.Wang
//
//Description: Output of track extrapolation results.
//
//History: 2005.7.5 created by L.L.Wang
//
//

#include "DstEvent/DstExtTrack.h"

DstExtTrack::DstExtTrack()
{
	m_trackId=-99;

	myParticleType=2;

	Hep3Vector zeroVector(-99.,-99.,-99.);
	HepSymMatrix zeroMatrix(6,0);

	for(int i=0; i<5; i++)
	{
		myTof1VolumeName.push_back("TOF1");
		myTof2VolumeName.push_back("TOF2");
		myEmcVolumeName.push_back("EMC");
		myMucVolumeName.push_back("MUC");
		myTof1Position.push_back(zeroVector);
		myTof1Momentum.push_back(zeroVector);
		myTof2Position.push_back(zeroVector);
		myTof2Momentum.push_back(zeroVector);
		myEmcPosition.push_back(zeroVector);
		myEmcMomentum.push_back(zeroVector);
		myMucPosition.push_back(zeroVector);
		myMucMomentum.push_back(zeroVector);

		myTof1VolumeNumber[i]=-99;
		myTof2VolumeNumber[i]=-99;
		myEmcVolumeNumber[i]=-1;
		myMucVolumeNumber[i]=-99;

		myTof1[i]=-99.;
		myTof1Path[i]=-99.;
		myTof1PosSigmaAlongZ[i]=-99.;
		myTof1PosSigmaAlongT[i]=-99.;
		myTof1PosSigmaAlongX[i]=-99.;
		myTof1PosSigmaAlongY[i]=-99.;
		myTof2[i]=-99.;
		myTof2Path[i]=-99.;
		myTof2PosSigmaAlongZ[i]=-99.;
		myTof2PosSigmaAlongT[i]=-99.;
		myTof2PosSigmaAlongX[i]=-99.;
		myTof2PosSigmaAlongY[i]=-99.;
		myEmcPosSigmaAlongTheta[i]=-99.;
		myEmcPosSigmaAlongPhi[i]=-99.;
		myMucPosSigmaAlongZ[i]=-99.;
		myMucPosSigmaAlongT[i]=-99.;
		myMucPosSigmaAlongX[i]=-99.;
		myMucPosSigmaAlongY[i]=-99.;
		myEmcPath[i]=-99.;       
                
		//****added by Li Chunhua
		myMucKalchi2[i]=-1;         
		myMucKaldof[i]= -1;         
		myMucKaldepth[i]= -1;       
		myMucKalbrLastLayer[i]= -1; 
		myMucKalecLastLayer[i]= -1; 
		myMucKalnumHits[i]= -1;     
                //********************

		myTof1ErrorMatrix.push_back(zeroMatrix);
		myTof2ErrorMatrix.push_back(zeroMatrix);
		myEmcErrorMatrix.push_back(zeroMatrix);
		myMucErrorMatrix.push_back(zeroMatrix);
	}
}

DstExtTrack::~DstExtTrack(){}

/*
//Tof layer1 data:
const Hep3Vector DstExtTrack::GetTof1Position() const {return myTof1Position;}

const Hep3Vector DstExtTrack::GetTof1Momentum() const {return myTof1Momentum;}

const string DstExtTrack::GetTof1VolumeName() const {return myTof1VolumeName;}

const int DstExtTrack::GetTof1VolumeNumber() const {return myTof1VolumeNumber;}

const double DstExtTrack::GetTof1() const {return myTof1;}

const double DstExtTrack::GetTof1Path() const {return myTof1Path;}

const double DstExtTrack::GetTof1PosSigmaAlongZ() const {return myTof1PosSigmaAlongZ;}

const double DstExtTrack::GetTof1PosSigmaAlongT() const {return myTof1PosSigmaAlongT;}

const double DstExtTrack::GetTof1PosSigmaAlongX() const {return myTof1PosSigmaAlongX;}

const double DstExtTrack::GetTof1PosSigmaAlongY() const {return myTof1PosSigmaAlongY;}

const HepSymMatrix DstExtTrack::GetTof1ErrorMatrix() const {return myTof1ErrorMatrix;}

//Tof layer2 data:
const Hep3Vector DstExtTrack::GetTof2Position() const {return myTof2Position;}

const Hep3Vector DstExtTrack::GetTof2Momentum() const {return myTof2Momentum;}

const string DstExtTrack::GetTof2VolumeName() const {return myTof2VolumeName;}

const int DstExtTrack::GetTof2VolumeNumber() const {return myTof2VolumeNumber;}

const double DstExtTrack::GetTof2() const {return myTof2;}

const double DstExtTrack::GetTof2Path() const {return myTof2Path;}

const double DstExtTrack::GetTof2PosSigmaAlongZ() const {return myTof2PosSigmaAlongZ;}

const double DstExtTrack::GetTof2PosSigmaAlongT() const {return myTof2PosSigmaAlongT;}

const double DstExtTrack::GetTof2PosSigmaAlongX() const {return myTof2PosSigmaAlongX;}

const double DstExtTrack::GetTof2PosSigmaAlongY() const {return myTof2PosSigmaAlongY;}

const HepSymMatrix DstExtTrack::GetTof2ErrorMatrix() const {return myTof2ErrorMatrix;}

//Emc data
const Hep3Vector DstExtTrack::GetEmcPosition() const {return myEmcPosition;}

const Hep3Vector DstExtTrack::GetEmcMomentum() const {return myEmcMomentum;}

const string DstExtTrack::GetEmcVolumeName() const {return myEmcVolumeName;}

const int DstExtTrack::GetEmcVolumeNumber() const {return myEmcVolumeNumber;}

const double DstExtTrack::GetEmcPosSigmaAlongTheta() const {return myEmcPosSigmaAlongTheta;}

const double DstExtTrack::GetEmcPosSigmaAlongPhi() const {return myEmcPosSigmaAlongPhi;}

const HepSymMatrix DstExtTrack::GetEmcErrorMatrix() const {return myEmcErrorMatrix;}

//Muc data
const Hep3Vector DstExtTrack::GetMucPosition() const {return myMucPosition;}

const Hep3Vector DstExtTrack::GetMucMomentum() const {return myMucMomentum;}

const string DstExtTrack::GetMucVolumeName() const {return myMucVolumeName;}

const int DstExtTrack::GetMucVolumeNumber() const {return myMucVolumeNumber;}

const double DstExtTrack::GetMucPosSigmaAlongZ() const {return myMucPosSigmaAlongZ;}

const double DstExtTrack::GetMucPosSigmaAlongT() const {return myMucPosSigmaAlongT;}

const double DstExtTrack::GetMucPosSigmaAlongX() const {return myMucPosSigmaAlongX;}

const double DstExtTrack::GetMucPosSigmaAlongY() const {return myMucPosSigmaAlongY;}

const HepSymMatrix DstExtTrack::GetMucErrorMatrix() const {return myMucErrorMatrix;}
*/

void DstExtTrack::SetTof1Data(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
		int aVolumeNumber,double aTof,double aPath,HepSymMatrix aErrorMatrix,
		double aZSigma,double aTSigma,double aXSigma,double aYSigma)
{
	myTof1Position[myParticleType] = aPosition;
	myTof1Momentum[myParticleType] = aMomentum;
	myTof1VolumeName[myParticleType] = aVolumeName;
	myTof1VolumeNumber[myParticleType] = aVolumeNumber;
	myTof1[myParticleType] = aTof;
	myTof1Path[myParticleType] = aPath;
	myTof1PosSigmaAlongZ[myParticleType] = aZSigma;
	myTof1PosSigmaAlongT[myParticleType] = aTSigma;
	myTof1PosSigmaAlongX[myParticleType] = aXSigma;
	myTof1PosSigmaAlongY[myParticleType] = aYSigma;
	myTof1ErrorMatrix[myParticleType] = aErrorMatrix;
}

void DstExtTrack::SetTof2Data(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
		int aVolumeNumber,double aTof,double aPath,HepSymMatrix aErrorMatrix,
		double aZSigma,double aTSigma,double aXSigma,double aYSigma)
{
	myTof2Position[myParticleType] = aPosition;
	myTof2Momentum[myParticleType] = aMomentum;
	myTof2VolumeName[myParticleType] = aVolumeName;
	myTof2VolumeNumber[myParticleType] = aVolumeNumber;
	myTof2[myParticleType] = aTof;
	myTof2Path[myParticleType] = aPath;
	myTof2PosSigmaAlongZ[myParticleType] = aZSigma;
	myTof2PosSigmaAlongT[myParticleType] = aTSigma;
	myTof2PosSigmaAlongX[myParticleType] = aXSigma;
	myTof2PosSigmaAlongY[myParticleType] = aYSigma;
	myTof2ErrorMatrix[myParticleType] = aErrorMatrix;
}

void DstExtTrack::SetEmcData(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
		int aVolumeNumber,double aThetaSigma,double aPhiSigma,
		HepSymMatrix aErrorMatrix)
{
	myEmcPosition[myParticleType] = aPosition;
	myEmcMomentum[myParticleType] = aMomentum;
	myEmcVolumeName[myParticleType] = aVolumeName;
	myEmcVolumeNumber[myParticleType] = aVolumeNumber;
	myEmcPosSigmaAlongTheta[myParticleType] = aThetaSigma;
	myEmcPosSigmaAlongPhi[myParticleType] = aPhiSigma;
	myEmcErrorMatrix[myParticleType] = aErrorMatrix;
}

void DstExtTrack::SetMucData(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
		int aVolumeNumber,HepSymMatrix aErrorMatrix,double aZSigma,
		double aTSigma,double aXSigma,double aYSigma)
{
	myMucPosition[myParticleType] = aPosition;
	myMucMomentum[myParticleType] = aMomentum;
	myMucVolumeName[myParticleType] = aVolumeName;
	myMucVolumeNumber[myParticleType] = aVolumeNumber;
	myMucPosSigmaAlongZ[myParticleType] = aZSigma;
	myMucPosSigmaAlongT[myParticleType] = aTSigma;
	myMucPosSigmaAlongX[myParticleType] = aXSigma;
	myMucPosSigmaAlongY[myParticleType] = aYSigma;
	myMucErrorMatrix[myParticleType] = aErrorMatrix;
}


//added by LI Chunhua
void DstExtTrack::SetMucKalData(double chi2,int dof, double depth, int brLastLay, int ecLastLay, int nhits)
{

 myMucKalchi2[1] = chi2;
 myMucKaldof[1]=dof;
 myMucKaldepth[1]=depth;
 myMucKalbrLastLayer[1]=brLastLay;
 myMucKalecLastLayer[1]=ecLastLay;
 myMucKalnumHits[1]=nhits;


}



