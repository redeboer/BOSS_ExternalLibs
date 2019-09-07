//
//File: DstExtTrack.h
//Author: L.L.Wang 

#ifndef DSTEXTTRACK_H
#define DSTEXTTRACK_H

#include <vector>

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
//#include "ExtEvent/ExtMucHit.h"

using namespace std;
using namespace EventModel;

using CLHEP::Hep3Vector;
using CLHEP::HepSymMatrix;
extern const CLID& CLID_DstExtTrack;

class DstExtTrack : virtual public ContainedObject
{
	public:
		const CLID& clID() const 
		{
			return DstExtTrack::classID();
		}

		static const CLID& classID()
		{
			return CLID_DstExtTrack;
		}                     

		DstExtTrack();
		~DstExtTrack();

		const int  GetTrackId() const {return m_trackId ;}
		const int  trackId() const {return m_trackId ;}

		//Get track extrapolation data @ Tof layer1.
		//const Hep3Vector GetTof1Position() const;
		//const Hep3Vector GetTof1Momentum() const;
		//const string GetTof1VolumeName() const;
		//const int GetTof1VolumeNumber() const;
		//const double GetTof1() const;
		//const double GetTof1Path() const;
		//const double GetTof1PosSigmaAlongZ() const;
		//const double GetTof1PosSigmaAlongT() const;
		//const double GetTof1PosSigmaAlongX() const;
		//const double GetTof1PosSigmaAlongY() const;
		//const HepSymMatrix GetTof1ErrorMatrix() const;

		const Hep3Vector tof1Position() const {return myTof1Position[myParticleType];}; 
		const Hep3Vector tof1Position(int parID) const {return myTof1Position[parID];}; 
		const Hep3Vector tof1Momentum() const {return myTof1Momentum[myParticleType];};
		const Hep3Vector tof1Momentum(int parID) const {return myTof1Momentum[parID];};
		const string tof1VolumeName() const {return myTof1VolumeName[myParticleType];};
		const string tof1VolumeName(int parID) const {return myTof1VolumeName[parID];};
		const int tof1VolumeNumber() const {return myTof1VolumeNumber[myParticleType];};
		const int tof1VolumeNumber(int parID) const {return myTof1VolumeNumber[parID];};
		const double tof1() const {return myTof1[myParticleType];};
		const double tof1(int parID) const {return myTof1[parID];};
		const double tof1Path() const {return myTof1Path[myParticleType];};
		const double tof1Path(int parID) const {return myTof1Path[parID];};
		const double tof1PosSigmaAlongZ() const {return myTof1PosSigmaAlongZ[myParticleType];};
		const double tof1PosSigmaAlongZ(int parID) const {return myTof1PosSigmaAlongZ[parID];};
		const double tof1PosSigmaAlongT() const {return myTof1PosSigmaAlongT[myParticleType];};
		const double tof1PosSigmaAlongT(int parID) const {return myTof1PosSigmaAlongT[parID];};
		const double tof1PosSigmaAlongX() const {return myTof1PosSigmaAlongX[myParticleType];};
		const double tof1PosSigmaAlongX(int parID) const {return myTof1PosSigmaAlongX[parID];};
		const double tof1PosSigmaAlongY() const {return myTof1PosSigmaAlongY[myParticleType];};
		const double tof1PosSigmaAlongY(int parID) const {return myTof1PosSigmaAlongY[parID];};
		const HepSymMatrix tof1ErrorMatrix() const {return myTof1ErrorMatrix[myParticleType];};
		const HepSymMatrix tof1ErrorMatrix(int parID) const {return myTof1ErrorMatrix[parID];};

		//Get track extrapolation data @ Tof layer2.
		//const Hep3Vector GetTof2Position() const;
		//const Hep3Vector GetTof2Momentum() const;
		//const string GetTof2VolumeName() const;
		//const int GetTof2VolumeNumber() const;
		//const double GetTof2() const;
		//const double GetTof2Path() const;
		//const double GetTof2PosSigmaAlongZ() const;
		//const double GetTof2PosSigmaAlongT() const;
		//const double GetTof2PosSigmaAlongX() const;
		//const double GetTof2PosSigmaAlongY() const;
		//const HepSymMatrix GetTof2ErrorMatrix() const;

		const Hep3Vector tof2Position() const {return myTof2Position[myParticleType];};
		const Hep3Vector tof2Position(int parID) const {return myTof2Position[parID];};
		const Hep3Vector tof2Momentum() const {return myTof2Momentum[myParticleType];};
		const Hep3Vector tof2Momentum(int parID) const {return myTof2Momentum[parID];};
		const string tof2VolumeName() const {return myTof2VolumeName[myParticleType];};
		const string tof2VolumeName(int parID) const {return myTof2VolumeName[parID];};
		const int tof2VolumeNumber() const {return myTof2VolumeNumber[myParticleType];};
		const int tof2VolumeNumber(int parID) const {return myTof2VolumeNumber[parID];};
		const double tof2() const {return myTof2[myParticleType];};
		const double tof2(int parID) const {return myTof2[parID];};
		const double tof2Path() const {return myTof2Path[myParticleType];};
		const double tof2Path(int parID) const {return myTof2Path[parID];};
		const double tof2PosSigmaAlongZ() const {return myTof2PosSigmaAlongZ[myParticleType];};
		const double tof2PosSigmaAlongZ(int parID) const {return myTof2PosSigmaAlongZ[parID];};
		const double tof2PosSigmaAlongT() const {return myTof2PosSigmaAlongT[myParticleType];};
		const double tof2PosSigmaAlongT(int parID) const {return myTof2PosSigmaAlongT[parID];};
		const double tof2PosSigmaAlongX() const {return myTof2PosSigmaAlongX[myParticleType];};
		const double tof2PosSigmaAlongX(int parID) const {return myTof2PosSigmaAlongX[parID];};
		const double tof2PosSigmaAlongY() const {return myTof2PosSigmaAlongY[myParticleType];};
		const double tof2PosSigmaAlongY(int parID) const {return myTof2PosSigmaAlongY[parID];};
		const HepSymMatrix tof2ErrorMatrix() const {return myTof2ErrorMatrix[myParticleType];};
		const HepSymMatrix tof2ErrorMatrix(int parID) const {return myTof2ErrorMatrix[parID];};

		//Get track extrapolation data @ EMC.
		//const Hep3Vector GetEmcPosition() const;
		//const Hep3Vector GetEmcMomentum() const;
		//const string GetEmcVolumeName() const;
		//const int GetEmcVolumeNumber() const;
		//const double GetEmcPosSigmaAlongTheta() const;
		//const double GetEmcPosSigmaAlongPhi() const;
		//const HepSymMatrix GetEmcErrorMatrix() const;

		const Hep3Vector emcPosition() const {return myEmcPosition[myParticleType];};
		const Hep3Vector emcPosition(int parID) const {return myEmcPosition[parID];};
		const Hep3Vector emcMomentum() const {return myEmcMomentum[myParticleType];};
		const Hep3Vector emcMomentum(int parID) const {return myEmcMomentum[parID];};
		const string emcVolumeName() const {return myEmcVolumeName[myParticleType];};
		const string emcVolumeName(int parID) const {return myEmcVolumeName[parID];};
		const int emcVolumeNumber() const {return myEmcVolumeNumber[myParticleType];};
		const int emcVolumeNumber(int parID) const {return myEmcVolumeNumber[parID];};
		const double emcPosSigmaAlongTheta() const {return myEmcPosSigmaAlongTheta[myParticleType];};
		const double emcPosSigmaAlongTheta(int parID) const {return myEmcPosSigmaAlongTheta[parID];};
		const double emcPosSigmaAlongPhi() const {return myEmcPosSigmaAlongPhi[myParticleType];};
		const double emcPosSigmaAlongPhi(int parID) const {return myEmcPosSigmaAlongPhi[parID];};
		const HepSymMatrix emcErrorMatrix() const {return myEmcErrorMatrix[myParticleType];};
		const HepSymMatrix emcErrorMatrix(int parID) const {return myEmcErrorMatrix[parID];};
		const double emcPath() const {return myEmcPath[myParticleType];};
		const double emcPath(int parID) const {return myEmcPath[parID];};

		//Get track extrapolation data @ MUC.
		//const Hep3Vector GetMucPosition() const;
		//const Hep3Vector GetMucMomentum() const;
		//const string GetMucVolumeName() const;
		//const int GetMucVolumeNumber() const;
		//const double GetMucPosSigmaAlongZ() const;
		//const double GetMucPosSigmaAlongT() const;
		//const double GetMucPosSigmaAlongX() const;
		//const double GetMucPosSigmaAlongY() const;
		//const HepSymMatrix GetMucErrorMatrix() const;

		const Hep3Vector mucPosition() const {return myMucPosition[myParticleType];};
		const Hep3Vector mucPosition(int parID) const {return myMucPosition[parID];};
		const Hep3Vector mucMomentum() const {return myMucMomentum[myParticleType];};
		const Hep3Vector mucMomentum(int parID) const {return myMucMomentum[parID];};
		const string mucVolumeName() const {return myMucVolumeName[myParticleType];};
		const string mucVolumeName(int parID) const {return myMucVolumeName[parID];};
		const int mucVolumeNumber() const {return myMucVolumeNumber[myParticleType];};
		const int mucVolumeNumber(int parID) const {return myMucVolumeNumber[parID];};
		const double mucPosSigmaAlongZ() const { return myMucPosSigmaAlongZ[myParticleType];};
		const double mucPosSigmaAlongZ(int parID) const {return myMucPosSigmaAlongZ[parID];};
		const double mucPosSigmaAlongT() const {return myMucPosSigmaAlongT[myParticleType];};
		const double mucPosSigmaAlongT(int parID) const {return myMucPosSigmaAlongT[parID];};
		const double mucPosSigmaAlongX() const {return myMucPosSigmaAlongX[myParticleType];};
		const double mucPosSigmaAlongX(int parID) const {return myMucPosSigmaAlongX[parID];};
		const double mucPosSigmaAlongY() const {return myMucPosSigmaAlongY[myParticleType];};
		const double mucPosSigmaAlongY(int parID) const {return myMucPosSigmaAlongY[parID];};
		const HepSymMatrix mucErrorMatrix() const {return myMucErrorMatrix[myParticleType];};
		const HepSymMatrix mucErrorMatrix(int parID) const {return myMucErrorMatrix[parID];};
                
		//**** added by LI Chunhua
                const double MucKalchi2() const { return  myMucKalchi2[1];}; 
		const double MucKalchi2(int parID) const { return  myMucKalchi2[1];}
		const int    MucKaldof() const { return  myMucKaldof[1];};
                const int    MucKaldof(int parID) const { return  myMucKaldof[1];};
                const double MucKaldepth() const { return  myMucKaldepth[1];};
                const double MucKaldepth(int parID) const { return  myMucKaldepth[1];};
                const int    MucKalbrLastLayer() const { return  myMucKalbrLastLayer[1];};
                const int    MucKalbrLastLayer(int parID) const { return  myMucKalbrLastLayer[1];};
                const int    MucKalecLastLayer() const { return  myMucKalecLastLayer[1];};                          
		const int    MucKalecLastLayer(int parID) const { return  myMucKalecLastLayer[1];};
                const int    MucKalnumHits() const { return  myMucKalnumHits[1];};
	  	const int    MucKalnumHits(int parID) const { return  myMucKalnumHits[1];};
                //***********************************************************

		//Get ExtMucHitCol
		//const ExtMucHitVec GetExtMucHitVec() const {return myExtMucHitVec;};
		//const ExtMucHitVec extMucHitVec() const {return myExtMucHitVec;};

		// set functions
		void SetTrackId(int trackId){m_trackId = trackId;}
		void SetParType(int aParType=2) 
		{
			if(aParType>=0&&aParType<=4) myParticleType=aParType;
			else {
				cout<<"DstExtTrack::warning: Invalid particle number: "<<aParType<<endl;
				myParticleType=2;
			}
		}

		void SetTof1Data(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
				int aVolumeNumber,double aTof,double aPath,HepSymMatrix aErrorMatrix,
				double aZSigma=0.,double aTSigma=0.,double aXSigma=0.,double aYSigma=0.);

		void SetTof2Data(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
				int aVolumeNumber,double aTof,double aPath,HepSymMatrix aErrorMatrix,
				double aZSigma=0.,double aTSigma=0.,double aXSigma=0.,double aYSigma=0.);

		void SetEmcData(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
				int aVolumeNumber,double aThetaSigma,double aPhiSigma,HepSymMatrix aErrorMatrix);

		void SetEmcPath(double path) {myEmcPath[myParticleType] = path;};

		void SetMucData(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
				int aVolumeNumber,HepSymMatrix aErrorMatrix,double aZSigma=0.,
				double aTSigma=0.,double aXSigma=0.,double aYSigma=0.);

		//**** added by LI Chunhua
		void SetMucKalData(double chi2,int dof, double depth, int brLastLay, int ecLastLay, int nhits);
		
		
		//void AddExtMucHit(ExtMucHit aExtMucHit) {myExtMucHitVec.push_back(aExtMucHit);}

	protected:
		int myParticleType;// default particle type

		
	private:
		int m_trackId;    // Track ID wensp Add 2005-10-19
		//Tof TrkExt data.
		//
		//Tof layer1:
		vector<Hep3Vector> myTof1Position;//Position when the particle enters the TOF layer1.
		vector<Hep3Vector> myTof1Momentum;//Momentum when the particle enters the TOF layer1.
		vector<string> myTof1VolumeName;//Scintillator volume name
		int myTof1VolumeNumber[5];//Scintillator volume number
		double myTof1[5];//Time of flight
		double myTof1Path[5];//Path of flight
		double myTof1PosSigmaAlongZ[5];//Sigma Z
		double myTof1PosSigmaAlongT[5];//Sigma along Z x R
		double myTof1PosSigmaAlongX[5];//Sigma X
		double myTof1PosSigmaAlongY[5];//Sigma Y
		vector<HepSymMatrix> myTof1ErrorMatrix;//Error matrix 6x6 (x,p)

		//Tof layer2:
		vector<Hep3Vector> myTof2Position;//Position when the particle enters the TOF layer2.
		vector<Hep3Vector> myTof2Momentum;//Momentum when the particle enters the TOF layer2.
		vector<string> myTof2VolumeName;//Scintillator volume name
		int myTof2VolumeNumber[5];//Scintillator volume number
		double myTof2[5];//Time of flight
		double myTof2Path[5];//Path of flight
		double myTof2PosSigmaAlongZ[5];//Sigma Z
		double myTof2PosSigmaAlongT[5];//Sigma along Z x R
		double myTof2PosSigmaAlongX[5];//Sigma X
		double myTof2PosSigmaAlongY[5];//Sigma Y
		vector<HepSymMatrix> myTof2ErrorMatrix;//Error matrix 6x6 (x,p)

		//Emc TrkExt data
		vector<Hep3Vector> myEmcPosition;
		vector<Hep3Vector> myEmcMomentum;
		vector<string> myEmcVolumeName;
		int myEmcVolumeNumber[5];
		double myEmcPosSigmaAlongTheta[5];
		double myEmcPosSigmaAlongPhi[5];
		vector<HepSymMatrix> myEmcErrorMatrix;
		double myEmcPath[5];

		//Muc TrkExt data
		vector<Hep3Vector> myMucPosition;
		vector<Hep3Vector> myMucMomentum;
		vector<string> myMucVolumeName;
		int myMucVolumeNumber[5];
		double myMucPosSigmaAlongZ[5];
		double myMucPosSigmaAlongT[5];
		double myMucPosSigmaAlongX[5];
		double myMucPosSigmaAlongY[5];
		vector<HepSymMatrix> myMucErrorMatrix;
                
                 //**** added by LI Chunhua
                 double myMucKalchi2[5];
                 int    myMucKaldof[5];
		 double myMucKaldepth[5];
		 int    myMucKalbrLastLayer[5];
		 int    myMucKalecLastLayer[5];
		 int    myMucKalnumHits[5];
                 //**********************


		//MUC Hits collection
		//ExtMucHitVec myExtMucHitVec;
};

typedef ObjectVector<DstExtTrack> DstExtTrackCol;

#endif

