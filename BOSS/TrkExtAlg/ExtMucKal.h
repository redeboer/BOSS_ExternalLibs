#ifndef ExtMucKal_H_
#define ExtMucKal_H_

//#include "CLHEP/Alist/AList.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
//#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#include "G4VPhysicalVolume.hh"
#include "TrkExtAlg/Ext_xp_err.h"
#include "G4LogicalVolume.hh"
#include "MucRawEvent/MucDigi.h"
#include "MucRecEvent/MucRecHit.h"
//#endif


class ExtMucKal //: public HepAList<KLMKCluster>
{

public:

  ExtMucKal();
  ~ExtMucKal();
  bool ExtMucFilter();
  void SetPosMomErr(Hep3Vector pos,Hep3Vector mom,HepSymMatrix err){m_CurrentPosition=pos;m_CurrentMomentum=mom;m_CurrentXPErr = err;}
  void SetMucDigiCol(MucDigiCol* amucdigi){m_MucDigiCol=amucdigi;}
  void SetMucWindow(int aMucWindow){n_sigm = aMucWindow;}
  void SetGapID(Hep3Vector id ){m_gapid = id;}
  bool MucKalIniti();
  void XPmod(Hep3Vector &pos,Hep3Vector &mom,HepSymMatrix &err);
  bool JCB();
  double Fit( Hep3Vector &pos, Hep3Vector &mom, HepSymMatrix &err);
  HepMatrix GetRoationMatrix(MucGeoGap *box);
  bool GetFilterStatus(){return FilterOK;}
  double GetChi2(){return Chi2_sub;}
  double GetDistance(const MucRecHit* hit);
  double GetPull(){return m_pull;}
  int GetOrient(){return m_orient;}
  Hep3Vector GetHitGap();
  vector<MucRecHit*> GapHit(); 
  vector<MucRecHit*> TrackHit();
  bool GetSameStrip(){return m_samestrip;}
protected:

private:
  Hep3Vector m_gapid;
  Hep3Vector m_hitgap;
  Hep3Vector m_CurrentPosition;
  Hep3Vector m_CurrentMomentum;
  HepSymMatrix m_CurrentXPErr;
  HepVector3D m_CurrentInsct;
  HepSymMatrix m_CurrentInsctXPErr;
  HepMatrix m_jcb;
  MucDigiCol* m_MucDigiCol;
  bool FilterOK;
  double Chi2_sub;
  HepSymMatrix m_err_mod;
  Hep3Vector m_pos_mod;
  Hep3Vector m_mom_mod;
  MucRecHit* m_nearesthit;
  bool HitExist;
  double m_pull;
  double m_sigma;
  int m_orient;
  int n_sigm;
  HepVector m_bm;
  HepSymMatrix m_Ebm;
  int m_iStrip;
  bool m_samestrip;
};

#endif
