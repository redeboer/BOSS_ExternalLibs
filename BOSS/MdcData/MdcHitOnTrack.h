//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcHitOnTrack.h,v 1.11 2012/08/13 00:05:12 zhangy Exp $
//
// Description:
//   Contains drift chamber hit info, as hit is used on a particular track
//   Inherits from TrkHitOnTrk.  The drift distance is stored as an 
//   absolute value, but returned as |drift|*ambig.  Ambiguity stored as +/- 1; 
//   ambiguity = 0 => pick better value @ first call to updateMeasurement.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: Steve Schaffner
//
// Revision History:
//	20011018  M. Kelsey -- Make "rawTime()" public.
//	20030923  M. Kelsey -- Add function to replace _dHit pointer
//------------------------------------------------------------------------

#ifndef MDCHITONTRACK_H
#define MDCHITONTRACK_H

#include "TrkBase/TrkHitOnTrk.h"
#include "TrkBase/TrkEnums.h"
#include "MdcGeom/Constants.h"
#include <math.h>
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using CLHEP::Hep3Vector;
class MdcHit;
class MdcLayer;
class Trajectory;

class MdcHitOnTrack : public TrkHitOnTrk {

  public:
    MdcHitOnTrack(const TrkFundHit& fundHit, const MdcHit& baseHit,
	int ambig, double fittime);
    virtual ~MdcHitOnTrack();


    // MdcHitOnTrack specific functions
    double entranceAngleHit() const;
    double entranceAngle() const;
    double entranceAngle(const HepPoint3D pos, const Hep3Vector dir) const;
    // the entrance Angle is the difference in phi between the *direction* 
    // of the track at the hit, and the phi of the *location* of hit
    // Signing convention is such that tracks going to the 'left' 
    // are positive and the ones going to the 'right' are negative
    // The definition was chosen so that radially outgoing tracks have 
    // zero entrance angle (by construction), regardless of 'doca'

    double dipAngle() const;
    // dipangle is just pi/2 - theta of the track at the location of the hit

    // In general ambiguities are: -1 -> right, 0 -> don't know, +1 -> left
    //   note: maybe this should just be an 'enum' or even a seperate class...
    // Note the special case of incoming tracks (i.e. |entranceAngle()|>pi/2) 
    // where a track having a wire on the LEFT ( ambig()=+1) has the hit on the
    // RIGHT ( wireAmbig()=-1 ) of the wire
    int      ambig()  const  { return _ambig; }  // wire wrt track direction
    int     wireAmbig()  const;//   { // hit wrt the wire location
      //return fabs(entranceAngle())<Constants::pi/2?ambig():-ambig();} 

    double   fitTime()  const  { return _fitTime; }
    // note: drift is signed according to ambiguity...
    //   if ambiguity unknown, return the average of the 
    //   absolute value of both ambiguities, i.e. pick a positive number...
    double   drift()  const  { return _ambig!=0 ? _drift[_ambig<0 ? 0:1] 
      : (_drift[1]-_drift[0])*0.5; }
    double  drift(double dca)  const  { return _drift[dca<0?0:1]; }
    double  dcaToWire() const;

    double  rawTime() const;

    //   generic virtual functions (required by TrkHitOnTrk)
    virtual const Trajectory* hitTraj() const;
    virtual const MdcHitOnTrack* mdcHitOnTrack() const;
    virtual bool timeResid(double& t,double& tErr) const;
    virtual bool timeAbsolute(double& t,double& tErr) const;

    // specific virtual functions (required by MdcHitOnTrack)
    virtual const MdcHit*  mdcHit()  const;
    virtual unsigned tdcIndex()        const = 0;
    virtual unsigned status()           const = 0;

    // Forwarded to MdcHit
    int              wire()        const ;
    const MdcLayer*  layer()       const ;
    int              layernumber() const ;
    unsigned         layerNumber() const;
    int              whichView()   const; // 0 for axial, +/- 1 for stereo
    double           charge()      const;

    TrkEnums::TrkViewInfo whatView() const;

    // Set used during persistant -> transient and internally
    void                     setAmbig(int a) { _ambig = a<0?-1:a>0?1:0; }
    void                     setT0(double t0);

  protected:

    MdcHitOnTrack(const TrkFundHit *baseHit, int ambig,
	double fittime, int layer, int wire);
    MdcHitOnTrack(const MdcHitOnTrack &hitToBeCopied, TrkRep *newRep,
	const TrkDifTraj* trkTraj,const MdcHit *hb=0);

    bool              isBeyondEndflange() const 
    { return (hitLen() < _startLen || hitLen() > _endLen); }
    // return forceIteration: ambiguity flipped && hit is active
    bool              updateAmbiguity(double dca); 

    virtual TrkErrCode updateMeasurement(const TrkDifTraj* traj, bool maintainAmbiguity);
    //virtual TrkErrCode updateMeasurement(const TrkDifTraj* traj, bool maintainAmbiguity=false);

    // Allow subclasses to query or replace the underlying hit pointer
    const MdcHit* baseHit() const { return _dHit; }
    void changeBase(MdcHit* newBase);

  private:
    //  friend class MdcHOTData;//yzhang del

    void              updateCorrections();
    double            driftVelocity() const; // in cm/s
    bool              driftCurrent() const { return ambig()*_drift[ambig()<0?0:1]>0; }


    //Data members
    int               _ambig;  // this is the LR ambiguity wrt the TRACK 
    // direction;
    // carefull: the t->d calibration needs it wrt. 
    // the WIRE, and for INCOMING tracks the two are 
    // NOT the same.
    double            _drift[2];  // corrected version of what's in the FundHit,
    // one for each ambiguity: left, right
    const Trajectory* _hitTraj;
    double            _fitTime;   // store last value used for the fit (for calib)
    //   cached information to improve code speed
    double            _startLen;  // start hitlen traj
    double            _endLen;    // end hitlen traj
    const MdcHit *    _dHit;


    //  hide copy constructor and assignment operator
    MdcHitOnTrack(const MdcHitOnTrack&);
    MdcHitOnTrack& operator=(const MdcHitOnTrack&);
};

#endif
