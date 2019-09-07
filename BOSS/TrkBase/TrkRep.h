//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkRep.h,v 1.2 2006/03/28 01:02:36 zhangy Exp $
//
// Description: Base class for internal track representation classes -- e.g. 
//   HelixRep, KalRep.  Owns and maintains a TrkHotList; and keeps a 
//   pointer to the track that owns the Rep.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: Steve Schaffner
//
//------------------------------------------------------------------------

#ifndef TRKREP_HH
#define TRKREP_HH
#include <memory>
#include "MdcRecoUtil/PdtPid.h"
#include "TrkBase/TrkDirection.h"
#include "TrkBase/TrkFitStatus.h"
#include "TrkBase/TrkFit.h"
#include "TrkBase/TrkHotList.h"
#include "TrkBase/TrkHitOnTrkUpdater.h"
// These 3 are needed for the OSF compiler:
#include "MdcRecoUtil/BesVectorErr.h"
#include "MdcRecoUtil/BesPointErr.h"
#include "CLHEP/Vector/ThreeVector.h"
// the following is needed by the Sun compiler
#include "ProbTools/ChisqConsistency.h"

class TrkHitOnTrk;
#include <iosfwd>
class TrkDifTraj;
class TrkErrCode;
class TrkExchangePar;
class TrkRecoTrk;
class TrkVolume;
class IfdKey;
class TrkContext;

// Class interface //
class TrkRep : public TrkFitStatus, public TrkFit, public TrkHitOnTrkUpdater {
  
public:
  //******************************************
  // Constructors and such
  //******************************************
// construct from a hotlist -- The rep will not take ownership of anything passed
//                          -- it will _clone_ the hots passed on inHots
  TrkRep(const TrkHotList& inHots, TrkRecoTrk* trk,
         PdtPid::PidType hypo);
  TrkRep(TrkHotList& inHots, TrkRecoTrk* trk,
         PdtPid::PidType hypo, bool stealHots=false);
// construct from a hotlist -- The rep will always _TAKE OWNERSHIP_ of the hots
//                             and if takeownership==true, ALSO of the list.
  TrkRep(const TrkHotList* inHots, TrkRecoTrk* trk,
         PdtPid::PidType hypo);
  TrkRep(TrkHotList* inHots, TrkRecoTrk* trk,
         PdtPid::PidType hypo, bool takeownership=false);
  // For reps w/o hits stored
  TrkRep(TrkRecoTrk* trk, PdtPid::PidType hypo, int nAct, int nSvt,
         int nMdc, double startFoundRange, double endFoundRange);
// rep without explicit hotlist
  TrkRep(TrkRecoTrk* trk, PdtPid::PidType hypo, bool createHotList=false);
  // copy ctor
  TrkRep(const TrkRep& oldRep, TrkRecoTrk* trk, PdtPid::PidType hypo);
  virtual ~TrkRep();
  // clone() used to copy tracks; cloneNewHypo() for new hypos within track
  virtual TrkRep* clone(TrkRecoTrk* newTrack) const = 0;
  virtual TrkRep* cloneNewHypo(PdtPid::PidType hypo) = 0;
  bool operator== (const TrkRep&);

  //******************************************
  // Global quantities:
  //******************************************
  virtual ChisqConsistency    chisqConsistency() const;
  virtual int               nActive()      const;
  virtual int               nSvt()         const;
  virtual int               nMdc()         const;
  virtual PdtPid::PidType   particleType() const;
  TrkRecoTrk*               parentTrack()                {return _parentTrack;}
  const TrkRecoTrk*         parentTrack()  const         {return _parentTrack;}
  double                    startValidRange() const;
  double                    endValidRange()   const;
  virtual double            startFoundRange() const;
  virtual double            endFoundRange()   const;
  virtual const IfdKey&     myKey()           const;
  double                    trackT0()         const; 

  //******************************************
  // Information about track at a given position
  //******************************************
  virtual HepPoint3D                position(double fltL)         const;
  virtual Hep3Vector                direction(double fltL)        const;
  virtual double                    arrivalTime(double fltL)      const;
  virtual BesPointErr               positionErr(double fltL)      const;
  virtual BesVectorErr              directionErr(double fltL)     const;

  //******************************************
  // Hit (list) handling
  //******************************************
  // Simple implementations of these are present in the base class; 
  //   complicated reps (e.g. Kalman) may wish to override.
  virtual void            addHot(TrkHitOnTrk *theHot);
  virtual void            removeHot(TrkHitOnTrk *theHot);
  virtual void            activateHot(TrkHitOnTrk *theHot);
  virtual void            deactivateHot(TrkHitOnTrk *theHot);
  virtual TrkHotList*             hotList()                 {return _hotList.get();}
  virtual const TrkHotList*       hotList() const           {return _hotList.get();}
  virtual void            updateHots();
  virtual bool            resid(const TrkHitOnTrk *theHot,
                                double &residual, double &residErr,
                                bool exclude=false) const;

  // Distinguishes hotLists that actually hold hits from those that just hold 
  //   info (e.g. nActive) about the hits used to create the fit
  bool hitCapable() const                     {return hotList()->hitCapable();}


  //****************************************** 
  // Fitting stuff
  //******************************************
  virtual TrkErrCode fit() = 0;

protected:
  TrkRep&   operator= (const TrkRep&);
  const TrkContext& trkContext() const;

  //protected, not private, so derived classes can create in copy ctor
  std::auto_ptr<TrkHotList>  _hotList;

private:
  void init(TrkRecoTrk* trk, PdtPid::PidType hypo);

  PdtPid::PidType  _partHypo;
  TrkRecoTrk*      _parentTrack;
  mutable double   _betainv;   // cache for arrivalTime
};

#endif
