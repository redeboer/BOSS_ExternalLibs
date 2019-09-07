//
// BesStatus.h
//

#ifndef BES_STATUS
#define BES_STATUS

#include <TObject.h>

#ifndef BES_EBESViewType
#define BES_EBESViewType

enum EBESViewType {
  k3DView   = BIT(0),
  kXYView   = BIT(1),
  kZRView   = BIT(2),
  //Long Peixun's update: CGEM unfolded display for each layer
  kCgemUFView0 = BIT(3),
  kCgemUFView1 = BIT(4),
  kCgemUFView2 = BIT(5)
};
#endif

class BesStatus : public TObject {

 private:

 public:
  friend ostream& operator<< (ostream& os,const BesStatus& bs);
  friend istream& operator>> (istream& is,BesStatus& bs);
  BesStatus();
  virtual ~BesStatus();

  //Long Peixun's update: status flags for Cgem Geometry
  Bool_t fCgemGlobal;   //visibility for global CGEM
  Bool_t fCgemLayers;   //visibility for CGEM layers
  Bool_t fCgemXStrips;  //visibility for CGEM X-strips
  Bool_t fCgemVStrips;  //visibility for CGEM V-strips

  // status flags for Mdc geometry

  Bool_t fMdcGlobal;    //! visibility flag for global Mdc
  Bool_t fMdcTubes;     //! visibility flag for Mdc main tubes
  Bool_t fMdcWires;     //! visibility flag for Mdc wires

  // status flags for Tof geometry

  Bool_t fTofGlobal;   //!visibility flag for global Tof
  Bool_t fTofEast;     //!visibility flag for Tof east EC
  Bool_t fTofBarrel;   //!visibility flag for Tof barrel
  Bool_t fTofWest;     //!visibility flag for Tof west EC

  // status flags for Emc geometry

  Bool_t fEmcGlobal;   //! visivility flag for global Emc
  Bool_t fEmcEast;     //! visivility flag for Emc east EC
  Bool_t fEmcBarrel;   //! visivility flag for Emc barrel
  Bool_t fEmcWest;     //! visivility flag for Emc west EC
  Bool_t fEmcSide;     //! visivility flag for Emc side

  // status flags for Muc geometry

  Bool_t fMucGlobal;   //! visivility flag for global Muc
  Bool_t fMucEast;     //! visivility flag for Muc east EC
  Bool_t fMucBarrel;   //! visivility flag for Muc barrel
  Bool_t fMucWest;     //! visivility flag for Muc west EC
  Bool_t fMucStrips;   //! visivility flag for Muc strips

  // status flags for full 3D detector geometry

  Bool_t fFull3DCgem;  //Long Peixun's update: Add Cgem option
  Bool_t fFull3DMdc;   //! visivility flag for full 3D Mdc
  Bool_t fFull3DTof;   //! visivility flag for full 3D Tof
  Bool_t fFull3DEmc;   //! visivility flag for full 3D Emc
  Bool_t fFull3DMuc;   //! visivility flag for full 3D Muc

  // status flags for other parts

  Bool_t fBeamPipe;        //! visibility flag for beam pipe
  Bool_t fZRPlaneOnXY;     //! visibility flag for ZRPlaneOnXY
  Bool_t fAxis;            //! visibility flag for axis

  //Long Peixun's update: add status flags for hits Cgem
  Bool_t fCgemHitsGlobal;   //visibility flag for global CGEM hits
  Bool_t fCgemHitsXStrip;   //visibility flag for CGEM hits X-strip
  Bool_t fCgemHitsVStrip;   //visibility flag for CGEM hits V-strip
  Bool_t fCgemHitsClusters; //visibility flag for CGEM clusters
  Bool_t fCgemHitsFiredHL;  //visibility flag for CGEM highlight fired stirps

  // status flags for hits Mdc
  Bool_t fMdcHitsGlobal;       //! visibility flag for global Mdc Hits
  Bool_t fMdcHits;             //! visibility flag for Mdc Hits

  // status flags for hits Tof

  Bool_t fTofHitsGlobal;    //! visibility flag for global Tof Hits
  Bool_t fTofHitsEast;      //! visibility flag for Tof Hits East
  Bool_t fTofHitsBarrel;    //! visibility flag for Tof Hits Barrel
  Bool_t fTofHitsWest;      //! visibility flag for Tof Hits West
  
  // status flags for hits Emc

  Bool_t fEmcHitsGlobal;    //! visibility flag for global Emc Hits
  Bool_t fEmcHitsEast;      //! visibility flag for Emc Hits East
  Bool_t fEmcHitsBarrel;    //! visibility flag for Emc Hits Barrel
  Bool_t fEmcHitsWest;      //! visibility flag for Emc Hits West
  Bool_t fEmcHitsSide;      //! visibility flag for Emc Hits Side

  // status flags for hits Muc

  Bool_t fMucHitsGlobal;    //! visibility flag for global Muc Hits
  Bool_t fMucHitsEast;      //! visibility flag for Muc Hits East
  Bool_t fMucHitsBarrel;    //! visibility flag for Muc Hits Barrel
  Bool_t fMucHitsWest;      //! visibility flag for Muc Hits West
  
  // status flags for tracks

  Bool_t fTracksGlobal;     //! visibility flag for global Tracks
  Bool_t fTracksMdc;        //! visibility flag for Mdc Tracks
  Bool_t fTracksTof;        //! visibility flag for Tof Tracks
  Bool_t fTracksEmc;        //! visibility flag for Emc Tracks
  Bool_t fTracksMuc;        //! visibility flag for Muc Tracks
  Bool_t fTracksExt;        //! visibility flag for Ext Tracks

  // status flags for general options

  Bool_t fFishEye;           //! flags for fish eye view

  // phi range

  Float_t      fPhiRangeMin;      // Minimum of phi range for Z-R projection (in degrees)
  Float_t      fPhiRangeMax;      // Maximum of phi range for Z-R projection (in degrees)

  // zoom
  Float_t      fZoom;

  void Transfer(BesStatus *right, Bool_t set);

  void TransferGeometryCgem       (BesStatus *right, Bool_t set); //Long Peixun's update: for CGEM
  void TransferGeometryMdc        (BesStatus *right, Bool_t set);
  void TransferGeometryTof        (BesStatus *right, Bool_t set);
  void TransferGeometryEmc        (BesStatus *right, Bool_t set);
  void TransferGeometryMuc        (BesStatus *right, Bool_t set);
  void TransferGeometryFull3D     (BesStatus *right, Bool_t set);
  void TransferGeometryOtherParts (BesStatus *right, Bool_t set);

  void TransferHitsCgem  (BesStatus *right, Bool_t set);  //Long Peixun's update: for CGEM
  void TransferHitsMdc   (BesStatus *right, Bool_t set);
  void TransferHitsTof   (BesStatus *right, Bool_t set);
  void TransferHitsEmc   (BesStatus *right, Bool_t set);
  void TransferHitsMuc   (BesStatus *right, Bool_t set);

  void TransferTracks    (BesStatus *right, Bool_t set);

  void TransferFishEye  (BesStatus *right, Bool_t set);
  void TransferPhiRange (BesStatus *right, Bool_t set);
  void TransferZoom     (BesStatus *right, Bool_t set);

  void Default(EBESViewType type);

  ClassDef(BesStatus,0) // Bes View Status
};
istream& operator>> (istream& is,BesStatus& bs);
ostream& operator<< (ostream& os,const BesStatus& bs);

#endif
