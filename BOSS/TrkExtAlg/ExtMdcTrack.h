//
//File: ExtMdcTrack.h
//Author: L.L.Wang
//
//Deccription: A class to get MDC reconstuction track,
//             and supply information to  track extrapolation.
//             It is only a interface.
//             
//History: 2005.7.4 created by L.L.Wang
//

#ifndef EXTMDCTRACK_H
#define EXTMDCTRACK_H
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Geometry/Point3D.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Vector3D<double> HepVector3D;
#endif
      


#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"

using namespace std;

using namespace CLHEP;

static const int NdimMdcErr(5);
static const int NdimExtErr(6);
static const int NumHelixPar(5);

class ExtMdcTrack
{
	public:
		ExtMdcTrack(void);//Construction.
		~ExtMdcTrack(void);//Destruction.

		void SetMsgFlag(bool aFlag) {myMsgFlag=aFlag;};
		bool SetMdcRecTrkCol(RecMdcTrackCol * aPointer);//Get RecMdcTrackCol from TDS.
		bool SetMdcKalTrkCol(RecMdcKalTrackCol * aPointer);//Get RecMdcTrackCol from TDS.
		bool SetParId(int pid) { myParID=pid; return true;};
		bool GetOneGoodTrk();//Get a good MdcRecTrk.
		bool ReadTrk(int pid);
		int GetTrackID() {return myTrackID;}//Get RecMdcTrackl ID.
		RecMdcTrack *GetMdcRecTrkPtr() const;//Get RecMdcTrackl pointer.
		const Hep3Vector GetPosition() const;//Get coordinate of the track at the Mdc->Tof point.
		const Hep3Vector GetMomentum() const;//Get the momentum of the track at the Mdc_>Tof point.
		const HepSymMatrix GetErrorMatrix() const;//Get 6x6 (x,p)Err matrix at the Mdc_>Tof point.
		double GetTrackLength() const;//Get the track length of the track at the Mdc->Tof point.
		inline double GetTrkTof() const {return myTrkTof;};
//		double GetTOF() const;//Get the Tof of the track at the Mdc->Tof point.
		double GetPt() const;//Get the pt of the track at the Mdc->Tof point.
		double GetParticleCharge() const;//Get the charge of the track(+1,-1).

	private:
		void Convert();//Convert data due to unit problem.
		bool myMsgFlag;
		int myTrackID;
		RecMdcTrackCol *myMdcTrackCol;
		RecMdcTrackCol::iterator myMdcRecTrkIter;//Iterater of MdcRecTrk.
		RecMdcKalTrackCol *myMdcKalTrackCol;
		RecMdcKalTrackCol::iterator myMdcKalTrkIter;//Iterater of MdcRecTrk.
		string myInputTrk;
		int myParID;
		
		HepVector myHelixPar;//Mdc reconstucted track's helix 5 parameters.
		HepPoint3D myPivot;//Pivot point of the MdcRecTrk' helix.
		HepSymMatrix myMdcErr;//Error matrix(5x5) of the MdcRecTrk's helix.
		double myPhiTerm;//Phi angle of the MdcRecTrk's helix at the Mdc.
		double myTrkLength;
		double myTrkTof;
		HepPoint3D myLPosition;
//		double myTrackLength[5];//Track length from Kalman fit.
//		string myParticleName;//Particle name used by GetTOF().
};


#endif
