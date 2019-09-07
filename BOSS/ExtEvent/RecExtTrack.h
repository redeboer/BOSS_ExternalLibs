//
//File: ExtTrack.h
//Author: L.L.Wang 
//
//Description: Output of track extrapolation results.
//
//History: 2005.7.5 created by L.L.Wang
//

#ifndef RECEXTTRACK_H
#define RECEXTTRACK_H
/*
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
 */
#include "ExtMucHit.h"

#include "DstEvent/DstExtTrack.h"
using namespace std;

extern const CLID& CLID_RecExtTrack;

class RecExtTrack : public DstExtTrack
{
	public:
		const CLID& clID() const 
		{
			return RecExtTrack::classID();
		}

		static const CLID& classID()
		{
			return CLID_RecExtTrack;
		}                     

		RecExtTrack();
		RecExtTrack(const DstExtTrack& aDstExtTrack);
		RecExtTrack& operator = (const DstExtTrack& aDstExtTrack);
		~RecExtTrack();

		//Get ExtMucHitCol
		const ExtMucHitVec getExtMucHitVec() const {return myExtMucHitVec[myParticleType];};
		const ExtMucHitVec getExtMucHitVec(int parId) const {return myExtMucHitVec[parId];};

		const ExtMucHitVec GetExtMucHitVec() const {return myExtMucHitVec[myParticleType];};  
		const ExtMucHitVec GetExtMucHitVec(int parId) const {return myExtMucHitVec[parId];};  
		
		void AddExtMucHit(ExtMucHit aExtMucHit) {myExtMucHitVec[myParticleType].push_back(aExtMucHit);};

		const vector<double> pathInTof1() const {return myPathInTof1[myParticleType];};
		const vector<double> pathInTof1(int parId) const {return myPathInTof1[parId];};
		
		const vector<double> pathInTof2() const {return myPathInTof2[myParticleType];};
		const vector<double> pathInTof2(int parId) const {return myPathInTof2[parId];};

		void setPathInTof1(vector<double> x) {myPathInTof1[myParticleType] = x;};
		void setPathInTof2(vector<double> x) {myPathInTof2[myParticleType] = x;};

		/*
		//get Track ID number;
		const int GetTrackID() const;

		//Get track extrapolation data @ Tof layer1.
		const Hep3Vector GetTof1Position() const;
		const Hep3Vector GetTof1Momentum() const;
		const string GetTof1VolumeName() const;
		const int GetTof1VolumeNumber() const;
		const double GetTof1() const;
		const double GetTof1Path() const;
		const double GetTof1PosSigmaAlongZ() const;
		const double GetTof1PosSigmaAlongT() const;
		const double GetTof1PosSigmaAlongX() const;
		const double GetTof1PosSigmaAlongY() const;
		const HepSymMatrix GetTof1ErrorMatrix() const;

		//Get track extrapolation data @ Tof layer2.
		const Hep3Vector GetTof2Position() const;
		const Hep3Vector GetTof2Momentum() const;
		const string GetTof2VolumeName() const;
		const int GetTof2VolumeNumber() const;
		const double GetTof2() const;
		const double GetTof2Path() const;
		const double GetTof2PosSigmaAlongZ() const;
		const double GetTof2PosSigmaAlongT() const;
		const double GetTof2PosSigmaAlongX() const;
		const double GetTof2PosSigmaAlongY() const;
		const HepSymMatrix GetTof2ErrorMatrix() const;

		//Get track extrapolation data @ EMC.
		const Hep3Vector GetEmcPosition() const;
		const Hep3Vector GetEmcMomentum() const;
		const string GetEmcVolumeName() const;
		const int GetEmcVolumeNumber() const;
		const double GetEmcPosSigmaAlongTheta() const;
		const double GetEmcPosSigmaAlongPhi() const;
		const HepSymMatrix GetEmcErrorMatrix() const;
		const double GetEmcPath() const {return myEmcPath;};

		//Get track extrapolation data @ MUC.
		const Hep3Vector GetMucPosition() const;
		const Hep3Vector GetMucMomentum() const;
		const string GetMucVolumeName() const;
		const int GetMucVolumeNumber() const;
		const double GetMucPosSigmaAlongZ() const;
		const double GetMucPosSigmaAlongT() const;
		const double GetMucPosSigmaAlongX() const;
		const double GetMucPosSigmaAlongY() const;
		const HepSymMatrix GetMucErrorMatrix() const;

		//Get ExtMucHitCol
		const ExtMucHitVec GetExtMucHitVec() const {return myExtMucHitVec;};

		//Set data

		void SetTrackID(int aTrackID);

		void SetTof1Data(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
		int aVolumeNumber,double aTof,double aPath,HepSymMatrix aErrorMatrix,
		double aZSigma=0.,double aTSigma=0.,double aXSigma=0.,double aYSigma=0.);

		void SetTof2Data(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
		int aVolumeNumber,double aTof,double aPath,HepSymMatrix aErrorMatrix,
		double aZSigma=0.,double aTSigma=0.,double aXSigma=0.,double aYSigma=0.);

		void SetEmcData(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
		int aVolumeNumber,double aThetaSigma,double aPhiSigma,
		HepSymMatrix aErrorMatrix);

		void SetEmcPath(double path) {myEmcPath = path;};

		void SetMucData(Hep3Vector aPosition,Hep3Vector aMomentum,string aVolumeName,
				int aVolumeNumber,HepSymMatrix aErrorMatrix,double aZSigma=0.,
				double aTSigma=0.,double aXSigma=0.,double aYSigma=0.);

		void AddExtMucHit(ExtMucHit aExtMucHit) {myExtMucHitVec.push_back(aExtMucHit);};
		*/		


	private:
			//MUC Hits collection
			vector<ExtMucHitVec> myExtMucHitVec;

			vector< vector<double> > myPathInTof1;// path in TOF1
			vector< vector<double> > myPathInTof2;// path in TOF2

			/*		
					int myTrackID;

			//Tof TrkExt data.
			//
			//Tof layer1:
			Hep3Vector myTof1Position;//Position when the particle enters the TOF layer1.
			Hep3Vector myTof1Momentum;//Momentum when the particle enters the TOF layer1.
			string myTof1VolumeName;//Scintillator volume name
			int myTof1VolumeNumber;//Scintillator volume number
			double myTof1;//Time of flight
			double myTof1Path;//Path of flight
			double myTof1PosSigmaAlongZ;//Sigma Z
			double myTof1PosSigmaAlongT;//Sigma along Z x R
			double myTof1PosSigmaAlongX;//Sigma X
			double myTof1PosSigmaAlongY;//Sigma Y
			HepSymMatrix myTof1ErrorMatrix;//Error matrix 6x6 (x,p)

			//Tof layer2:
			Hep3Vector myTof2Position;//Position when the particle enters the TOF layer2.
			Hep3Vector myTof2Momentum;//Momentum when the particle enters the TOF layer2.
			string myTof2VolumeName;//Scintillator volume name
			int myTof2VolumeNumber;//Scintillator volume number
			double myTof2;//Time of flight
			double myTof2Path;//Path of flight
			double myTof2PosSigmaAlongZ;//Sigma Z
			double myTof2PosSigmaAlongT;//Sigma along Z x R
			double myTof2PosSigmaAlongX;//Sigma X
			double myTof2PosSigmaAlongY;//Sigma Y
			HepSymMatrix myTof2ErrorMatrix;//Error matrix 6x6 (x,p)

			//Emc TrkExt data,which is similar to TOF data.
			Hep3Vector myEmcPosition;
			Hep3Vector myEmcMomentum;
			string myEmcVolumeName;
			int myEmcVolumeNumber;
			double myEmcPosSigmaAlongTheta;
			double myEmcPosSigmaAlongPhi;
			HepSymMatrix myEmcErrorMatrix;

			double myEmcPath;

			//Muc TrkExt data,which is similar to TOF data.
			Hep3Vector myMucPosition;
			Hep3Vector myMucMomentum;
			string myMucVolumeName;
			int myMucVolumeNumber;
			double myMucPosSigmaAlongZ;
			double myMucPosSigmaAlongT;
			double myMucPosSigmaAlongX;
			double myMucPosSigmaAlongY;
			HepSymMatrix myMucErrorMatrix;

			//MUC Hits collection
			ExtMucHitVec myExtMucHitVec;
			 */		
};

typedef ObjectVector<RecExtTrack> RecExtTrackCol;

#endif
