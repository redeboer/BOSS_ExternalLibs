#ifndef TRKCOMPTRK_HH
#define TRKCOMPTRK_HH

#include "TrkBase/TrkAbsFit.h"
#include "GaudiKernel/DataObject.h"
#include <memory>

#include <iosfwd>
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"

using CLHEP::Hep3Vector;
using CLHEP::HepMatrix;
using CLHEP::HepSymMatrix;
class IfdKey;
class TrkExchangePar;
class TrkVolume;
class TrkDifTraj;
class TrkSimpTraj;
class BField;


class TrkCompTrk : public TrkAbsFit,public DataObject {
public:

  //  friend class TrkCompMaker;

  //****************
  //Global track quantities:
  //****************
  int                 charge()       const;
  double              chisq()        const;
  int                 nDof()         const;
  const TrkDifTraj&   traj()         const;
  const   BField&     bField()       const                       {return *_bf;}

  //****************
  //Information about track at a given position (flight length)
  //****************

  HepPoint3D           position(double fltL)     const;
  Hep3Vector         direction(double fltL)    const;
  Hep3Vector         momentum(double fltL=0.)  const;
  double             pt(double fltL=0.)        const;
  BesPointErr        positionErr(double fltL)  const;
  BesVectorErr       directionErr(double fltL) const;
  BesVectorErr       momentumErr(double fltL)  const;

  // maybe this isn't needed?
  //  TrkExchangePar     helix(double fltL)        const;

  //****************
  // Valid flight range
  //****************
  //  validRange = trajectory's valid range = flight length range 
  //  over which the track can be used. 
  double              startValidRange() const;
  double              endValidRange() const;

  // Interface to vertexing algorithms (M.Bondioli 7/17/98)
  // covariance matrices of the track at fixed flight length
  virtual HepMatrix         posmomCov(double fltL)        const;
  virtual void              getAllCovs(double fltL,
                                       HepSymMatrix& xxCov,
                                       HepSymMatrix& ppCov,
                                       HepMatrix& xpCov)  const;

  // accessors to 2nd derivative of chi2 wrt x and p.
  // x0 and p0 are filled with the pos and 3mom around which expansion
  // takes place, whilst Weights are filled with the 2nd deriv of chi2.
  // so that:
  //  dx=x-x0   dp=p-p0
  //
  //  chi2(x,p)=0.5 dx^t*Wxx*dx + dx^t*Wxp*dp + 0.5 dp^t*Wpp*dp
  // where:
  //  pos and mom are 3-dim Vectors,
  //  xxWeight ppWeight and xpWeight are 3 by 3 matrices 
  //
  virtual void              getAllWeights(double fltL,
                                          HepVector& pos,
                                          HepVector& mom,
                                          HepSymMatrix& xxWeight,
                                          HepSymMatrix& ppWeight,
                                          HepMatrix&    xpWeight)
                                            const;

  virtual void getAllWeights(const HepPoint3D& pt,
				  HepVector& pos,
				  HepVector& mom,
				  HepSymMatrix& xxWeight,
				  HepSymMatrix& ppWeight,
				  HepMatrix&    xpWeight)  const;
				 
//****************
  // Printing
  //****************
  virtual void        print(std::ostream& ) const;
  virtual void        printAll(std::ostream& ) const;

  //****************
  // Constructors and such
  //****************
  // Constructor from parameters
  TrkCompTrk(const BesPointErr& pos, 
	     const BesVectorErr& mom, 
	     const HepMatrix& xpCov,
	     int charge, double chisq, int nDoF, const BField*bf);
  // Copy constructor (leaves original unchanged):
  TrkCompTrk(const TrkCompTrk& right);
  // Destructor
  virtual ~TrkCompTrk();
  const TrkCompTrk& operator=(const TrkCompTrk& right);

private:
  std::auto_ptr<TrkSimpTraj> _traj;
  const BField* _bf;
  double _chisq;
  int _charge;
  int _nDof;
};

#endif
