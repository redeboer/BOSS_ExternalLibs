
////////////////
//#define Helix Ext_Helix
//File: ExtMdcTrack.cxx
//Author: L.L.Wang
//Deccription: A class to get MDC reconstuction track,
//             and supply information to  track extrapolation.
//             It is a interface.
//History: 2005.7.4 created by L.L.Wang
//

#include "CLHEP/Matrix/Matrix.h"
//#include "CLHEP/config/CLHEP.h"  //For constant PI(3.14......).

/////////////////////////////change
#include "TrkExtAlg/Helix.h"
//#include "TrackUtil/Helix.h"
/////////////////////////////////////////

//#include "MdcFastTrkAlg/helix/Helix.h"

#include "TrkExtAlg/ExtMdcTrack.h"
#include <float.h>

using namespace std;

const double mass[5] = {0.511,105.658,139.57,493.677,938.27203};

ExtMdcTrack::ExtMdcTrack():myMsgFlag(true),myInputTrk("Kal"),myParID(2),myHelixPar(NumHelixPar),myMdcErr(NdimMdcErr,0),myPhiTerm(0.){}

ExtMdcTrack::~ExtMdcTrack(){}

bool ExtMdcTrack::SetMdcRecTrkCol(RecMdcTrackCol * aPointer)
{
	myMdcTrackCol = aPointer;
	myMdcRecTrkIter = myMdcTrackCol->begin();
	myMdcRecTrkIter--;
	myInputTrk="Mdc";
	return true;
}

bool ExtMdcTrack::SetMdcKalTrkCol(RecMdcKalTrackCol * aPointer)
{
	myMdcKalTrackCol = aPointer;
	myMdcKalTrkIter = myMdcKalTrackCol->begin();
	myMdcKalTrkIter--;
	myInputTrk="Kal";
	return true;
}

bool ExtMdcTrack::GetOneGoodTrk()
{	
	if(myMsgFlag) cout<<"ExtMdcTrack::GetOneGoodTrk() begin"<<endl;
	if(myInputTrk=="Mdc")
	{
		myMdcRecTrkIter++;
		if(myMdcRecTrkIter!=myMdcTrackCol->end()) return true;
		else
		{
			if(myMsgFlag) cout<<"No more track!"<<endl; 
			return false;
		}
	}
	else if(myInputTrk=="Kal") // use KalTrack as input
	{
		myMdcKalTrkIter++;
		if(myMdcKalTrkIter!=myMdcKalTrackCol->end()) return true;
		else
		{
			if(myMsgFlag) cout<<"No more track!"<<endl; 
			return false;
		}
	}
}


bool ExtMdcTrack::ReadTrk(int pid=2)
{	
	if(myMsgFlag) cout<<"ExtMdcTrack::ReadTrk() begin"<<endl;
	myParID = pid;
	if(myInputTrk=="Mdc")
	{
		if(myMdcRecTrkIter!=myMdcTrackCol->end())
		{
			RecMdcTrack *aMdcTrack = *myMdcRecTrkIter;
			//		        if(!aMdcTrack->getStat())//If good track.Now status default 0
			{
				if(myMsgFlag) cout<<"Read a good track!"<<endl;

				//get MdcTrk data
				myTrackID = aMdcTrack->trackId();
				myHelixPar = aMdcTrack->helix();
				myPivot = aMdcTrack->getPivot();
				myMdcErr = aMdcTrack->err();
				//				double aPhiTerm = aMdcTrack->getFiTerm();
				myPhiTerm = aMdcTrack->getFiTerm();
				myTrkLength = GetTrackLength();
				double p = (GetMomentum()).mag();
				double beta = p/sqrt(mass[myParID]*mass[myParID]+p*p);
				myTrkTof = myTrkLength/(beta*299.792458);

				/*				//Start caculation of myPhiTerm.(when pivot is (0,0,0).)
								double r = DBL_MAX;
								double rMdc = 81.0;
								double dro = myHelixPar(1);
								if(myHelixPar(3)!=0.0)
								r = 333.564095/myHelixPar(3);
								if( fabs(2.0*r+dro)>rMdc && fabs(dro)<rMdc )
								{
								double aValue = ((dro+r)*(dro+r)+r*r-rMdc*rMdc)/2.0/fabs(r)/fabs(dro+r);
								if(r<0.0) myPhiTerm = acos(aValue);
								else myPhiTerm = -1.0*acos(aValue);
								}
								else
								{
								if(myMsgFlag) cout<<"Get a track which can't go out of MDC!"<<endl;
								myMdcRecTrkIter++;
								continue;
								}
				 */				
				//				double aTrackLength=fabs(r*myPhiTerm)*sqrt(myHelixPar(5)*myHelixPar(5)+1);
				if(myMsgFlag)
				{
					cout<<"MDC track:"<<myTrackID<<endl;
					cout<<"Helix: "<<myHelixPar;
					cout<<"pivot: "<<myPivot<<"cm"<<endl;
					cout<<"Error: "<<myMdcErr<<endl;

					/*				Convert();
									cout<<"********after convert:"<<endl;
									cout<<"Helix: "<<myHelixPar;
									cout<<"pivot: "<<myPivot;
									cout<<"Error: "<<myMdcErr<<endl;
					 */
					cout<<"Pt: "<<GetPt()<<"GeV/c"<<endl;
					cout<<"PhiTerm: "<<myPhiTerm<<endl;
					//					cout<<"recPhiTerm: "<<aPhiTerm<<endl;
					cout<<"trackLength(mm): "<<GetTrackLength()<<endl;
					//					cout<<"trackLengthTest(cm): "<<aTrackLength<<endl;
				}

				return true;
			}
			//			else
			//			{
			//				myMdcRecTrkIter++;
			//				cout<<"Get a bad track!"<<endl;
			//			}
		}
		else
		{
			if(myMsgFlag) cout<<"No more track!"<<endl; 
			return false;
		}
	}
	else if(myInputTrk=="Kal") // use KalTrack as input
	{
		if(myMdcKalTrkIter!=myMdcKalTrackCol->end())
		{
			RecMdcKalTrack *aMdcKalTrack = *myMdcKalTrkIter;
			{
				if(myMsgFlag) cout<<"Get a good track!"<<endl;

				//get MdcTrk data
				myTrackID = aMdcKalTrack->getTrackId();
				myHelixPar = aMdcKalTrack->getLHelix(myParID);
				//myPivot = HepPoint3D(0,0,0);
				myPivot = aMdcKalTrack->getLPivot(myParID);
				myMdcErr = aMdcKalTrack->getLError(myParID);
				myPhiTerm = aMdcKalTrack->getFiTerm(myParID);
				myPhiTerm = 0.0;
				myTrkLength = aMdcKalTrack->getPathSM(myParID);
				myLPosition = aMdcKalTrack->getLPoint(myParID);
				myTrkTof = aMdcKalTrack->getTof(myParID);
				//cout<<"myInitialTof = "<<myTrkTof<<endl;

				if(myMsgFlag)
				{
					cout<<"Kal track:"<<myTrackID<<endl;
					cout<<"myParID:"<<myParID<<endl;
					cout<<"Helix: "<<myHelixPar;
					cout<<"pivot: "<<myPivot<<"cm"<<endl;
					cout<<"Error: "<<myMdcErr<<endl;
					cout<<"Pt: "<<GetPt()<<"GeV/c"<<endl;
					cout<<"PhiTerm: "<<myPhiTerm<<endl;
//					cout<<"trackLength(mm): "<<GetTrackLength()<<endl;
					cout<<"trackLength(cm): "<<myTrkLength<<endl;
					cout<<"myTrkTof: "<<myTrkTof<<endl;
					cout<<"Last point position: "<<myLPosition<<endl;
				}
				return true;
			}
		}
		else
		{
			if(myMsgFlag) cout<<"No more track!"<<endl; 
			return false;
		}
	    }
}

void ExtMdcTrack::Convert()
{
	myHelixPar(1)*=10.;
	myHelixPar(3)*=0.001;
	myHelixPar(4)*=10.;
	myPivot*=10.;
	myMdcErr.fast(1,1)*=100.;
	myMdcErr.fast(2,1)*=10.;
	myMdcErr.fast(3,1)*=0.01;
	myMdcErr.fast(3,2)*=0.001;
	myMdcErr.fast(3,3)*=0.000001;
	myMdcErr.fast(4,1)*=100.;
	myMdcErr.fast(4,2)*=10.;
	myMdcErr.fast(4,3)*=0.01;
	myMdcErr.fast(4,4)*=100.;
	myMdcErr.fast(5,1)*=10.;
	myMdcErr.fast(5,3)*=0.001;
	myMdcErr.fast(5,4)*=10.;
}
	
RecMdcTrack *ExtMdcTrack::GetMdcRecTrkPtr() const
{
	return (*myMdcRecTrkIter);
}

const Hep3Vector ExtMdcTrack::GetPosition() const
{
//	if(myInputTrk=="Mdc") {
		Ext_Helix aHelix(myPivot,myHelixPar,myMdcErr);
		Hep3Vector aPoint = aHelix.x(myPhiTerm);
		if(myMsgFlag) cout<<"PhiTerm Position: "<<aPoint<<endl;
		aPoint*=10.0;
		return (aPoint);
/*	}
	else if(myInputTrk=="Kal") {
		return (myLPosition);
	}
*/	
}

const Hep3Vector ExtMdcTrack::GetMomentum() const
{
	Ext_Helix aHelix(myPivot,myHelixPar,myMdcErr);
	Hep3Vector aMomentum = aHelix.momentum(myPhiTerm);//In GeV/c units.
	if(myMsgFlag) cout<<"PhiTerm Momentum: "<<aMomentum<<endl;
	aMomentum*=1000.0;//In MeV/c uints.
	return (aMomentum);
}

const HepSymMatrix ExtMdcTrack::GetErrorMatrix() const
{
	Ext_Helix aHelix(myPivot,myHelixPar,myMdcErr);
//	aHelix.pivot(aHelix.x(myPhiTerm));//Move the pivot to the PhiTerm position.

	Hep3Vector aPoint = aHelix.x(myPhiTerm);
	double phi0 = aHelix.phi0();
        double kappaInv = 1.0/aHelix.kappa();
	double cosPhi0 = cos(phi0);
	double sinPhi0 = sin(phi0);
        // 0 -> myPhiTerm, 2009-5-29, by wangll
//	double px = aHelix.momentum(0).x(); 
//	double py = aHelix.momentum(0).y();
//	double pz = aHelix.momentum(0).z();
	double px = aHelix.momentum(myPhiTerm).x(); 
	double py = aHelix.momentum(myPhiTerm).y();
	double pz = aHelix.momentum(myPhiTerm).z();

	//Calculate the Jacobian: d(xp)/d(helix).
	HepMatrix Helix2XpJcb(NdimExtErr,NdimMdcErr,0);

	Helix2XpJcb(1,1) = cosPhi0;
	Helix2XpJcb(1,2) = myPivot.y()-aPoint.y();
	Helix2XpJcb(1,3) = (myPivot.x()-aPoint.x()+myHelixPar(1)*cosPhi0)*kappaInv;
	Helix2XpJcb(2,1) = sinPhi0;
	Helix2XpJcb(2,2) = aPoint.x()-myPivot.x();
	Helix2XpJcb(2,2) = (myPivot.y()-aPoint.y()+myHelixPar(1)*sinPhi0)*kappaInv;
	Helix2XpJcb(3,3) = (myPivot.z()-aPoint.z()+myHelixPar(4))*kappaInv;
	Helix2XpJcb(3,4) = 1.0;
	Helix2XpJcb(3,5) = (aPoint.z()-myPivot.z()-myHelixPar(4))/myHelixPar(5);
	Helix2XpJcb(4,2) = -py;
	Helix2XpJcb(4,3) = -px * kappaInv;
	Helix2XpJcb(5,2) =  px;
	Helix2XpJcb(5,3) = -py * kappaInv;
	Helix2XpJcb(6,3) = -pz * kappaInv;
	Helix2XpJcb(6,5) = fabs(kappaInv);

	//Calculate the Ext error matrix(6x6) in (x,p).
//	if(myMsgFlag) cout<<"Error matrix at new pivot: "<<endl;
//	aHelix.Ea();
//	cout<<"haha"<<endl;
	HepSymMatrix aErrorMatrix = aHelix.Ea().similarity(Helix2XpJcb);
	if(myMsgFlag) cout<<"PhiTerm Error matrix:"<<aErrorMatrix<<endl;
	
	//Uints Conversion.
	for(int i=1;i<=6;i++)
	{
		for(int j=1;j<=i;j++)
		{
			if(i<=3 && j<=3) aErrorMatrix.fast(i,j)*=100.0;
			if(i>=4 && j>=4) aErrorMatrix.fast(i,j)*=1.0e+6;
			if(i>=4 && j<=3) aErrorMatrix.fast(i,j)*=10000.0;
		}
	}

	return aErrorMatrix;
}

double ExtMdcTrack::GetTrackLength() const
{
  	if(myInputTrk=="Mdc") {

/*	//old version
	Helix aHelix(myPivot,myHelixPar,myMdcErr);
	
	double phi0 = aHelix.phi0();
	HepPoint3D w = -(*this).GetParticleCharge() * aHelix.center();
	Hep3Vector center( w.x(),w.y(),w.z() );
	double centerX = center.x();
	double centerY = center.y();
	double phiCenter;
	if(fabs(centerX) > 1.0e-10)
	{
		phiCenter = atan2(centerY,centerX);
		if(phiCenter < 0.) phiCenter += 2.0*M_PI;
	}
	else
	{
		phiCenter = (centerY>0)? M_PI_2:3.0*M_PI_2;
	}
	double dPhi = fabs(phi0 + myPhiTerm - phiCenter);
	if(dPhi >= 2.0*M_PI) dPhi -= 2.0*M_PI;
	double tanLambda = aHelix.tanl();
	double cosLambdaInv = sqrt(tanLambda*tanLambda + 1.0);
	return (10*fabs(aHelix.radius() * dPhi * cosLambdaInv));//10* for cm -> mm
*/	

	Ext_Helix aHelix(myPivot,myHelixPar,myMdcErr);
	double dPhi = fabs(myPhiTerm);
	double tanLambda = aHelix.tanl();
	double cosLambdaInv = sqrt(tanLambda*tanLambda + 1.0);
	return (10*fabs(aHelix.radius() * dPhi * cosLambdaInv));//10* for cm -> mm

  	}
  	else if(myInputTrk=="Kal")
  	{
  		return 10*myTrkLength; //10* for cm -> mm
  	}
}

double ExtMdcTrack::GetPt() const
{
	return (1.0/fabs(myHelixPar[2]));
}

double ExtMdcTrack::GetParticleCharge() const
{
	return ((myHelixPar[2]>0.)? 1.0:-1.0);
}
