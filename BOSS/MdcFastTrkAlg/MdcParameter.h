//
//Parameters for Mdc Fast Track Reconstruction
//
//Created by Ma Xiang

#ifndef MdcParameter_FLAG_
#define MdcParameter_FLAG_

#include <iostream>

#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif

#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"

using namespace std;

class MdcParameter {

 public:
   //the public method to get the static instance pointer
   static MdcParameter* instance();
	 double updateAlpha();

 protected:
  //Constructor and destructor
   MdcParameter();
   ~MdcParameter();

 public:
   //public parameters
   //findPrimatyVertex,Primary event vertex finding 
   const int _findEventVertex;  
   //event timing correction
   const int _evtTimeCorr;
   //minimum Pt in finding(this must be >0.)
   const double _minPt;
   //minimum Dr in finding
   const double _minDr;
   //T0 offset for real data
   const float  _t0OffSet;
   //coefficient of x-t for real data
   const float _xtCoEff;
   //doTracking,fast track finding
   const int _doIt;
   //make Mdst table from fzisan
   const bool _mkMdst;
   //make Tds from fzisan
   const bool _mkTds; 
   ////
   //const int _eventNo;
   //magnetic field constant
	 float _mfield;
   float _alpha;
   //chi2 of kappa in FTFinder::linkAxialSegments
   //sigmaKappa at linking
   const float _chi2_kappa;
   //minimum of chi2-chi2_kappa in FTFinder::linkAxialSegments
   const float _Min_chi2;
   //cut of (incomingPhi-NextOuterBoundHit.phi()) in  (0,2*M_pi) in FTFinder::linkAxialSegments
   //deltaphi should be <_deltaphi,or >2*M_PI-_deltaphi
   const float _deltaPhi;
   //angle of two nearest hits on two neighbour axial superlayers in FTFinder::linkAxialSuperLayer234
   //0 ,1 or 2 layers between the two hits (neighbour layer)
   const float _D_phi1,_D_phi2,_D_phi3;
   //minimum quantity of stereo segments,if consider end-cap ,it can  be 1
   const int   _nseg;
   //minimum of stereo hits,if 1,the end-cap efficiency is higher
   const int   _nlength;
   //minium number of stereo hits
   const int   _nc;
   //z cut in s_z fit for two steps
   const float _z_cut1,_z_cut2;
   //kappa fot cut requirements
   const float _chi2_1, _chi2_2, _chi2_3;
   //segment linear fit in superLayer
   const float _chi2_segfit;
   //cut bad hits when do r_phi Refit:1,not cut;2 cut   
   const int  _hitscut;

 private:
   //the instance pointer
   static MdcParameter * s_paramInstance;
	 IMagneticFieldSvc* m_pmgnIMF;
};

#endif  /*MdcParameter_FLAG_*/
