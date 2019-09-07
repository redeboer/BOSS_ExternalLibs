//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHotListFull.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description: List of hits (as HitOnTrk objects) associated with a 
//  reconstructed track.  This derived class is for tracks that actually 
//  have hits on them.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
// Modified 3-july-97 by Leon Rochester to use Roguewave Sorted Vector
// to store the list of Hots.  At some point the HepAList will go away.
//------------------------------------------------------------------------

#ifndef TRKHOTLISTFULL_HH
#define TRKHOTLISTFULL_HH
#include "TrkBase/TrkHotList.h"
#include "TrkBase/TrkHitOnTrkUpdater.h"
#include "TrkBase/TrkFunctors.h"

class TrkFundHit;
class TrkHitOnTrk;
class TrkRep;

// Class interface //
class TrkHotListFull : public TrkHotList, TrkHitOnTrkUpdater {
public:
  // constructors and such
  TrkHotListFull();
  // create new HotList, cloning old Hots and making the clones point at rep
  TrkHotListFull(const TrkHotList& inHots, TrkBase::Functors::cloneHot  func); // clone hots
  TrkHotListFull(      TrkHotList& inHots, TrkBase::Functors::setParent func); // steal hots
  TrkHotList* clone(TrkBase::Functors::cloneHot func) const;
  virtual ~TrkHotListFull();

  virtual bool      hitCapable()      const;
  virtual int       nActive(TrkEnums::TrkViewInfo view=TrkEnums::bothView)         const;
  virtual int       nMdc(TrkEnums::TrkViewInfo view=TrkEnums::bothView)            const;
  virtual int       nSvt(TrkEnums::TrkViewInfo view=TrkEnums::bothView)            const;
  virtual int       nHit(TrkEnums::TrkViewInfo view=TrkEnums::bothView)            const;
  virtual TrkView   svtView(int layer) const;
  virtual unsigned  firstMdcLayer() const;
  virtual unsigned  lastMdcLayer() const;
  virtual double    startFoundRange() const;
  virtual double    endFoundRange()   const;
  virtual bool         isActive(unsigned ihot) const;

  void              append(TrkHitOnTrk* );
  void              remove(TrkHitOnTrk* );
  TrkHitOnTrk*      findHot(const TrkFundHit*) const;
  void              updateHots();

protected:
  virtual const std::vector<TrkHitOnTrk*>&   hotlist()   const;
  virtual       std::vector<TrkHitOnTrk*>&   hotlist();
private:

  static size_t dfltCapac();                   // default capacity of vector
  std::vector<TrkHitOnTrk*> _hotlist;  // hits are owned by vector

  // Preempt: Hots have to be cloned, not naively copied
  TrkHotListFull(const TrkHotListFull& rhs);  //copy ctor
  TrkHotListFull&   operator= (const TrkHotListFull&);  
};

#endif

