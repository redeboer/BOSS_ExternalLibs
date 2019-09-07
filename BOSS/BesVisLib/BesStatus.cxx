//
// BesStatus.cxx
//

#include <iostream>

#include "BesVisLib/BesStatus.h"

using namespace std;

#ifndef __CINT__
ClassImp(BesStatus)
#endif

//__________________________________________________________
// BesStatus
// holds view status flags
//
//
BesStatus::BesStatus() : TObject() {
    //
    // BesStatus default constructor
    if ( gDebug ) cout << "BesStatus default ctor called" << endl;
}

//__________________________________________________________

BesStatus::~BesStatus() {
    //
    // BesStatus default destructor
    if (gDebug) {
        cout << "BesStatus default dtor called" << endl;
    }
}

//__________________________________________________________

void BesStatus::Default(EBESViewType type) {
    //
    // default view settings
    if ( type & k3DView ) {
        if ( gDebug ) cout << "BesStatus default 3D called" << endl;

        //Long Peixun's update: Add CGEM option
        fCgemGlobal = 0;
        fCgemLayers = 1;
        fCgemXStrips = 0;
        fCgemVStrips = 0;

        //Long Peixun's update: Change some default value
        fMdcGlobal = 0;    // nothing, Long Peixun: 1 -> 0
        fMdcTubes  = 1;    // tubes
        fMdcWires  = 0;    // wires

        fTofGlobal = 0;    // nothing, Long Peixun: 1 -> 0
        fTofEast   = 1;    // scintilator
        fTofBarrel = 1;    // scintilator
        fTofWest   = 1;    // scintilator

        fEmcGlobal = 0;    // nothing, Long Peixun: 1 -> 0
        fEmcEast   = 1;    // east frame, one cirlcle
        fEmcBarrel = 1;    // frame,
        fEmcWest   = 1;    // west frame, one circle
        fEmcSide   = 0;    // side

        fMucGlobal = 0;    // nothing, Long Peixun: 1 -> 0
        fMucEast   = 0;    // gap
        fMucBarrel = 1;    // gap
        fMucWest   = 0;    // gap
        fMucStrips = 0;    // strips

        fFull3DMdc = 0;
        fFull3DTof = 0;
        fFull3DEmc = 0;
        fFull3DMuc = 0;

        fBeamPipe       = 0; // beamPipe, Long Peixun: 1 -> 0
        fZRPlaneOnXY    = 0;
        fAxis           = 0;

        //Long Peixun's update: add Cgem hits status
        fCgemHitsGlobal = 1;
        fCgemHitsXStrip = 1;
        fCgemHitsVStrip = 1;
        fCgemHitsClusters = 1;
        fCgemHitsFiredHL = 0;

        fMdcHitsGlobal  = 1;
        fMdcHits        = 1;

        fTofHitsGlobal  = 1;
        fTofHitsEast    = 1;
        fTofHitsBarrel  = 1;
        fTofHitsWest    = 1;

        fEmcHitsGlobal  = 1;
        fEmcHitsEast    = 1;
        fEmcHitsBarrel  = 1;
        fEmcHitsWest    = 1;
        fEmcHitsSide    = 0;

        fMucHitsGlobal  = 1;
        fMucHitsEast    = 1;
        fMucHitsBarrel  = 1;
        fMucHitsWest    = 1;

        fTracksGlobal   = 1;
        fTracksMdc      = 1;
        fTracksTof      = 1;
        fTracksEmc      = 1;
        fTracksMuc      = 1;
        fTracksExt      = 1;

        fFishEye = 0;
        fPhiRangeMin = 90;
        fPhiRangeMax = 120;
        fZoom = 1.0;    //Long Peixun's update: Initialize fZoom in 3D view

    } else if ( type & kXYView ) {

        if ( gDebug ) cout << "BesStatus default XY called" << endl;

        //Long Peixun's update: Add CGEM option
        fCgemGlobal = 1;
        fCgemLayers = 0;
        fCgemXStrips = 0;
        fCgemVStrips = 0;

        fMdcGlobal = 1;    // nothing
        fMdcTubes  = 0;    // tubes
        fMdcWires  = 0;    // wires

        fTofGlobal = 1;    // nothing
        fTofEast   = 0;    // circle
        fTofBarrel = 1;    // circle
        fTofWest   = 0;    // circle

        fEmcGlobal = 1;    // nothing
        fEmcEast   = 0;    // east ec crystals
        fEmcBarrel = 1;    // 120 crystals
        fEmcWest   = 0;    // west ec crystals
        fEmcSide   = 0;    // side

        fMucGlobal = 1;    // nothing
        fMucEast   = 0;    // shape of bakelite or chamber
        fMucBarrel = 1;    // gap
        fMucWest   = 0;    // shape of bakelite or chamber
        fMucStrips = 0;    // strips

        fFull3DMdc = 0;
        fFull3DTof = 0;
        fFull3DEmc = 0;
        fFull3DMuc = 0;

        fBeamPipe       = 1; // circle
        fZRPlaneOnXY    = 0;
        fAxis           = 0;

        //Long Peixun's update: add Cgem hits status
        fCgemHitsGlobal = 1;
        fCgemHitsXStrip = 1;
        fCgemHitsVStrip = 0;
        fCgemHitsClusters = 1;
        fCgemHitsFiredHL = 1;

        fMdcHitsGlobal  = 1;
        fMdcHits        = 1;

        fTofHitsGlobal  = 1;
        fTofHitsEast    = 1;
        fTofHitsBarrel  = 1;
        fTofHitsWest    = 1;

        fEmcHitsGlobal  = 1;
        fEmcHitsEast    = 1;
        fEmcHitsBarrel  = 1;
        fEmcHitsWest    = 1;
        fEmcHitsSide    = 0;

        fMucHitsGlobal  = 1;
        fMucHitsEast    = 1;
        fMucHitsBarrel  = 1;
        fMucHitsWest    = 1;

        fTracksGlobal   = 1;
        fTracksMdc      = 1;
        fTracksTof      = 1;
        fTracksEmc      = 1;
        fTracksMuc      = 1;
        fTracksExt      = 1;

        fFishEye = 0;
        fPhiRangeMin = 90;
        fPhiRangeMax = 120;
	    fZoom = 1.44;

    } else if ( type & kZRView ) {

        if ( gDebug ) cout << "BesStatus default ZR called" << endl;

        //Long Peixun's update: Add CGEM option
        fCgemGlobal = 1;
        fCgemLayers = 0;
        fCgemXStrips = 0;
        fCgemVStrips = 0;

        fMdcGlobal = 1;    // nothing
        fMdcTubes  = 1;    // nothing
        fMdcWires  = 0;    // wire

        fTofGlobal = 1;    // nothing
        fTofEast   = 1;    // box
        fTofBarrel = 1;    // box
        fTofWest   = 1;    // box

        fEmcGlobal = 1;    // nothing
        fEmcEast   = 1;    // intersect ec crystals
        fEmcBarrel = 1;    // 44 crystals
        fEmcWest   = 1;    // intersect ec crystals
        fEmcSide   = 0;    // side

        fMucGlobal = 1;    // nothing
        fMucEast   = 1;    // project of bakelite or chamber
        fMucBarrel = 1;    // gap
        fMucWest   = 1;    // shape of bakelite or chamber
        fMucStrips = 0;    // strips

        fFull3DMdc = 0;
        fFull3DTof = 0;
        fFull3DEmc = 0;
        fFull3DMuc = 0;

        fBeamPipe       = 1; // box
        fZRPlaneOnXY    = 0;
        fAxis           = 0;

        //Long Peixun's update: add Cgem hits status
        fCgemHitsGlobal = 1;
        fCgemHitsXStrip = 1;
        fCgemHitsVStrip = 1;
        fCgemHitsClusters = 1;
        fCgemHitsFiredHL = 0;

        fMdcHitsGlobal  = 1;
        fMdcHits        = 1;

        fTofHitsGlobal  = 1;
        fTofHitsEast    = 1;
        fTofHitsBarrel  = 1;
        fTofHitsWest    = 1;

        fEmcHitsGlobal  = 1;
        fEmcHitsEast    = 1;
        fEmcHitsBarrel  = 1;
        fEmcHitsWest    = 1;
        fEmcHitsSide    = 0;

        fMucHitsGlobal  = 1;
        fMucHitsEast    = 1;
        fMucHitsBarrel  = 1;
        fMucHitsWest    = 1;

        fTracksGlobal   = 1;
        fTracksMdc      = 1;
        fTracksTof      = 1;
        fTracksEmc      = 1;
        fTracksMuc      = 1;
        fTracksExt      = 1;

        fFishEye = 0;
        fPhiRangeMin = 90;
        fPhiRangeMax = 120;
	    fZoom = 1.44;
    }

    //Long Peixun's update: Add CGEM unfolded view for each layer
    else if (type & kCgemUFView0)
    {
        if ( gDebug ) cout << "BesStatus default CGEM UF0 called" << endl;

        fCgemGlobal = 1;
        fCgemLayers = 1;
        fCgemXStrips = 0;
        fCgemVStrips = 0;

        fMdcGlobal = 1;    // nothing
        fMdcTubes  = 0;    // tubes
        fMdcWires  = 0;    // wires

        fTofGlobal = 1;    // nothing
        fTofEast   = 0;    // circle
        fTofBarrel = 1;    // circle
        fTofWest   = 0;    // circle

        fEmcGlobal = 1;    // nothing
        fEmcEast   = 0;    // east ec crystals
        fEmcBarrel = 1;    // 120 crystals
        fEmcWest   = 0;    // west ec crystals
        fEmcSide   = 0;    // side

        fMucGlobal = 1;    // nothing
        fMucEast   = 0;    // shape of bakelite or chamber
        fMucBarrel = 1;    // gap
        fMucWest   = 0;    // shape of bakelite or chamber
        fMucStrips = 0;    // strips

        fFull3DMdc = 0;
        fFull3DTof = 0;
        fFull3DEmc = 0;
        fFull3DMuc = 0;

        fBeamPipe       = 1; // circle
        fZRPlaneOnXY    = 0;
        fAxis           = 0;

        fCgemHitsGlobal = 1;
        fCgemHitsXStrip = 1;
        fCgemHitsVStrip = 1;
        fCgemHitsClusters = 1;
        fCgemHitsFiredHL = 0;

        fMdcHitsGlobal  = 1;
        fMdcHits        = 1;

        fTofHitsGlobal  = 1;
        fTofHitsEast    = 1;
        fTofHitsBarrel  = 1;
        fTofHitsWest    = 1;

        fEmcHitsGlobal  = 1;
        fEmcHitsEast    = 1;
        fEmcHitsBarrel  = 1;
        fEmcHitsWest    = 1;
        fEmcHitsSide    = 0;

        fMucHitsGlobal  = 1;
        fMucHitsEast    = 1;
        fMucHitsBarrel  = 1;
        fMucHitsWest    = 1;

        fTracksGlobal   = 1;
        fTracksMdc      = 1;
        fTracksTof      = 1;
        fTracksEmc      = 1;
        fTracksMuc      = 1;
        fTracksExt      = 1;

        fFishEye = 0;
        fPhiRangeMin = 90;
        fPhiRangeMax = 120;
	    fZoom = 13.0;
    }
    else if (type & kCgemUFView1)
    {
        if ( gDebug ) cout << "BesStatus default CGEM UF1 called" << endl;

        fCgemGlobal = 1;
        fCgemLayers = 1;
        fCgemXStrips = 0;
        fCgemVStrips = 0;

        fMdcGlobal = 1;    // nothing
        fMdcTubes  = 0;    // tubes
        fMdcWires  = 0;    // wires

        fTofGlobal = 1;    // nothing
        fTofEast   = 0;    // circle
        fTofBarrel = 1;    // circle
        fTofWest   = 0;    // circle

        fEmcGlobal = 1;    // nothing
        fEmcEast   = 0;    // east ec crystals
        fEmcBarrel = 1;    // 120 crystals
        fEmcWest   = 0;    // west ec crystals
        fEmcSide   = 0;    // side

        fMucGlobal = 1;    // nothing
        fMucEast   = 0;    // shape of bakelite or chamber
        fMucBarrel = 1;    // gap
        fMucWest   = 0;    // shape of bakelite or chamber
        fMucStrips = 0;    // strips

        fFull3DMdc = 0;
        fFull3DTof = 0;
        fFull3DEmc = 0;
        fFull3DMuc = 0;

        fBeamPipe       = 1; // circle
        fZRPlaneOnXY    = 0;
        fAxis           = 0;

        //Long Peixun's update: add Cgem hits status
        fCgemHitsGlobal = 1;
        fCgemHitsXStrip = 1;
        fCgemHitsVStrip = 1;
        fCgemHitsClusters = 1;
        fCgemHitsFiredHL = 0;

        fMdcHitsGlobal  = 1;
        fMdcHits        = 1;

        fTofHitsGlobal  = 1;
        fTofHitsEast    = 1;
        fTofHitsBarrel  = 1;
        fTofHitsWest    = 1;

        fEmcHitsGlobal  = 1;
        fEmcHitsEast    = 1;
        fEmcHitsBarrel  = 1;
        fEmcHitsWest    = 1;
        fEmcHitsSide    = 0;

        fMucHitsGlobal  = 1;
        fMucHitsEast    = 1;
        fMucHitsBarrel  = 1;
        fMucHitsWest    = 1;

        fTracksGlobal   = 1;
        fTracksMdc      = 1;
        fTracksTof      = 1;
        fTracksEmc      = 1;
        fTracksMuc      = 1;
        fTracksExt      = 1;

        fFishEye = 0;
        fPhiRangeMin = 90;
        fPhiRangeMax = 120;
	    fZoom = 9.0;
    }
    else if (type & kCgemUFView2)
    {
        if ( gDebug ) cout << "BesStatus default CGEM UF2 called" << endl;

        fCgemGlobal = 1;
        fCgemLayers = 1;
        fCgemXStrips = 0;
        fCgemVStrips = 0;

        fMdcGlobal = 1;    // nothing
        fMdcTubes  = 0;    // tubes
        fMdcWires  = 0;    // wires

        fTofGlobal = 1;    // nothing
        fTofEast   = 0;    // circle
        fTofBarrel = 1;    // circle
        fTofWest   = 0;    // circle

        fEmcGlobal = 1;    // nothing
        fEmcEast   = 0;    // east ec crystals
        fEmcBarrel = 1;    // 120 crystals
        fEmcWest   = 0;    // west ec crystals
        fEmcSide   = 0;    // side

        fMucGlobal = 1;    // nothing
        fMucEast   = 0;    // shape of bakelite or chamber
        fMucBarrel = 1;    // gap
        fMucWest   = 0;    // shape of bakelite or chamber
        fMucStrips = 0;    // strips

        fFull3DMdc = 0;
        fFull3DTof = 0;
        fFull3DEmc = 0;
        fFull3DMuc = 0;

        fBeamPipe       = 1; // circle
        fZRPlaneOnXY    = 0;
        fAxis           = 0;

        //Long Peixun's update: add Cgem hits status
        fCgemHitsGlobal = 1;
        fCgemHitsXStrip = 1;
        fCgemHitsVStrip = 1;
        fCgemHitsClusters = 1;
        fCgemHitsFiredHL = 0;

        fMdcHitsGlobal  = 1;
        fMdcHits        = 1;

        fTofHitsGlobal  = 1;
        fTofHitsEast    = 1;
        fTofHitsBarrel  = 1;
        fTofHitsWest    = 1;

        fEmcHitsGlobal  = 1;
        fEmcHitsEast    = 1;
        fEmcHitsBarrel  = 1;
        fEmcHitsWest    = 1;
        fEmcHitsSide    = 0;

        fMucHitsGlobal  = 1;
        fMucHitsEast    = 1;
        fMucHitsBarrel  = 1;
        fMucHitsWest    = 1;

        fTracksGlobal   = 1;
        fTracksMdc      = 1;
        fTracksTof      = 1;
        fTracksEmc      = 1;
        fTracksMuc      = 1;
        fTracksExt      = 1;

        fFishEye = 0;
        fPhiRangeMin = 90;
        fPhiRangeMax = 120;
	    fZoom = 7.0;
    }
}

//__________________________________________________________

void BesStatus::Transfer(BesStatus *right, Bool_t set) {
    //
    // transfer status informations from one status instance to
    // another, from here, all components transfer functions are called
    if ( gDebug ) cout << "BesStatus::Transfer(BesStatus *right) called" << endl;

    this->TransferGeometryCgem       (right, set);  //Long Peixun's update: for CGEM
    this->TransferGeometryMdc        (right, set);
    this->TransferGeometryTof        (right, set);
    this->TransferGeometryEmc        (right, set);
    this->TransferGeometryMuc        (right, set);
    this->TransferGeometryFull3D     (right, set);
    this->TransferGeometryOtherParts (right, set);

    this->TransferHitsCgem(right, set);
    this->TransferHitsMdc (right, set);
    this->TransferHitsTof (right, set);
    this->TransferHitsEmc (right, set);
    this->TransferHitsMuc (right, set);

    this->TransferTracks  (right, set);

    this->TransferFishEye  (right, set);
    this->TransferPhiRange (right, set);
    this->TransferZoom     (right, set);
}

//__________________________________________________________
//Long Peixun's update: for CGEM
void BesStatus::TransferGeometryCgem(BesStatus *right, Bool_t set)
{
    fCgemGlobal = right->fCgemGlobal;
    fCgemLayers = right->fCgemLayers;
    fCgemXStrips = right->fCgemXStrips;
    fCgemVStrips = right->fCgemVStrips;
}

//__________________________________________________________

void BesStatus::TransferGeometryMdc(BesStatus *right, Bool_t set) {

    fMdcGlobal = right->fMdcGlobal;
    fMdcTubes  = right->fMdcTubes;
    fMdcWires  = right->fMdcWires;
}

// ____________________________________________________________

void BesStatus::TransferGeometryTof(BesStatus *right, Bool_t set) {

    fTofGlobal = right->fTofGlobal;
    fTofEast   = right->fTofEast;
    fTofBarrel = right->fTofBarrel;
    fTofWest   = right->fTofWest;
}

//__________________________________________________________

void BesStatus::TransferGeometryEmc(BesStatus *right, Bool_t set) {

    fEmcGlobal = right->fEmcGlobal;
    fEmcEast   = right->fEmcEast;
    fEmcBarrel = right->fEmcBarrel;
    fEmcWest   = right->fEmcWest;
    fEmcSide   = right->fEmcSide;
}

// ____________________________________________________________

void BesStatus::TransferGeometryMuc(BesStatus *right, Bool_t set) {

    fMucGlobal = right->fMucGlobal;
    fMucEast   = right->fMucEast;
    fMucBarrel = right->fMucBarrel;
    fMucWest   = right->fMucWest;
    fMucStrips = right->fMucStrips;
}

// ____________________________________________________________

void BesStatus::TransferGeometryFull3D(BesStatus *right, Bool_t set)
{
    fFull3DCgem = right->fFull3DCgem;   //Long Peixun's update: for CGEM
    fFull3DMdc = right->fFull3DMdc;
    fFull3DTof = right->fFull3DTof;
    fFull3DEmc = right->fFull3DEmc;
    fFull3DMuc = right->fFull3DMuc;
}

// ____________________________________________________________

void BesStatus::TransferGeometryOtherParts(BesStatus *right, Bool_t set) {

    fBeamPipe    = right->fBeamPipe;
    fZRPlaneOnXY = right->fZRPlaneOnXY;
    fAxis        = right->fAxis;
}

//__________________________________________________________
//Long Peixun's update: for CGEM
void BesStatus::TransferHitsCgem(BesStatus *right, Bool_t set) {

    fCgemHitsGlobal = right->fCgemHitsGlobal;
    fCgemHitsXStrip = right->fCgemHitsXStrip;
    fCgemHitsVStrip = right->fCgemHitsVStrip;
    fCgemHitsClusters = right->fCgemHitsClusters;
    fCgemHitsFiredHL = right->fCgemHitsFiredHL;
}

//__________________________________________________________

void BesStatus::TransferHitsMdc(BesStatus *right, Bool_t set) {

    fMdcHitsGlobal = right->fMdcHitsGlobal;
    fMdcHits       = right->fMdcHits;
}

//__________________________________________________________

void BesStatus::TransferHitsTof(BesStatus *right, Bool_t set) {

    fTofHitsGlobal = right->fTofHitsGlobal;
    fTofHitsEast   = right->fTofHitsEast;
    fTofHitsBarrel = right->fTofHitsBarrel;
    fTofHitsWest   = right->fTofHitsWest;
}

//__________________________________________________________

void BesStatus::TransferHitsEmc(BesStatus *right, Bool_t set) {

    fEmcHitsGlobal = right->fEmcHitsGlobal;
    fEmcHitsEast   = right->fEmcHitsEast;
    fEmcHitsBarrel = right->fEmcHitsBarrel;
    fEmcHitsWest   = right->fEmcHitsWest;
    fEmcHitsSide   = right->fEmcHitsSide;
}

//__________________________________________________________

void BesStatus::TransferHitsMuc(BesStatus *right, Bool_t set) {

    fMucHitsGlobal = right->fMucHitsGlobal;
    fMucHitsEast   = right->fMucHitsEast;
    fMucHitsBarrel = right->fMucHitsBarrel;
    fMucHitsWest   = right->fMucHitsWest;
}

//__________________________________________________________

void BesStatus::TransferTracks(BesStatus *right, Bool_t set) {

    fTracksGlobal = right->fTracksGlobal;
    fTracksMdc    = right->fTracksMdc;
    fTracksTof    = right->fTracksTof;
    fTracksEmc    = right->fTracksEmc;
    fTracksMuc    = right->fTracksMuc;
    fTracksExt    = right->fTracksExt;
}

//__________________________________________________________

void BesStatus::TransferFishEye(BesStatus *right, Bool_t set) {
    //
    // transfer function for fish eye flag
    if ( gDebug ) cout << "BesStatus::TransferFishEye(BesStatus *right) called" << endl;

    fFishEye     = right->fFishEye;
}

//__________________________________________________________

void BesStatus::TransferPhiRange(BesStatus *right, Bool_t set) {
    //
    // transfer function for fish phi range
    if ( gDebug ) cout << "BesStatus::TransferPhiRange(BesStatus *right) called" << endl;

    fPhiRangeMin     = right->fPhiRangeMin;
    fPhiRangeMax     = right->fPhiRangeMax;
}

//__________________________________________________________

void BesStatus::TransferZoom(BesStatus *right, Bool_t set) {
    //
    // transfer function for zoom
    if ( gDebug ) cout << "BesStatus::TransferZoom(BesStatus *right) called" << endl;

    fZoom          = right->fZoom;
}

istream& operator>> (istream& is,BesStatus& bs)
{
    //Long Peixun's update: Add CGEM flags;
    unsigned int cMdc, cTof, cEmc, cMuc, cFull3D, cOther, cMdcHits, cCgem, cCgemHits;
    unsigned int cTofHits, cEmcHits, cMucHits, cTracks;
    Float_t cZoom;

    is >> hex >> cMdc >> cTof >> cEmc >> cMuc >> cFull3D >>
    cOther >> cMdcHits >> cTofHits >> cEmcHits >> cMucHits >> cTracks >> cZoom
    >> cCgem >> cCgemHits;      //Long Peixun's update: for CGEM

    bs.fMdcGlobal =  1 & (cMdc>>2);
    bs.fMdcTubes  =  1 & (cMdc>>1);
    bs.fMdcWires  =  1 &  cMdc;

    bs.fTofGlobal =  1 & (cTof>>3);
    bs.fTofEast   =  1 & (cTof>>2);
    bs.fTofBarrel =  1 & (cTof>>1);
    bs.fTofWest   =  1 &  cTof;

    bs.fEmcGlobal =  1 & (cEmc>>4);
    bs.fEmcEast   =  1 & (cEmc>>3);
    bs.fEmcBarrel =  1 & (cEmc>>2);
    bs.fEmcWest   =  1 & (cEmc>>1);
    bs.fEmcSide   =  1 &  cEmc;

    bs.fMucGlobal =  1 & (cMuc>>4);
    bs.fMucEast   =  1 & (cMuc>>3);
    bs.fMucBarrel =  1 & (cMuc>>2);
    bs.fMucWest   =  1 & (cMuc>>1);
    bs.fMucStrips =  1 &  cMuc;

    bs.fFull3DMdc =  1 & (cFull3D>>3);
    bs.fFull3DTof =  1 & (cFull3D>>2);
    bs.fFull3DEmc =  1 & (cFull3D>>1);
    bs.fFull3DMuc =  1 &  cFull3D;

    bs.fBeamPipe    =  1 & (cOther>>2);
    bs.fZRPlaneOnXY =  1 & (cOther>>1);
    bs.fAxis        =  1 &  cOther;

    bs.fMdcHitsGlobal = 1 & (cMdcHits>>1);
    bs.fMdcHits       = 1 &  cMdcHits;

    bs.fTofHitsGlobal = 1 & (cTofHits>>3);
    bs.fTofHitsEast   = 1 & (cTofHits>>2);
    bs.fTofHitsBarrel = 1 & (cTofHits>>1);
    bs.fTofHitsWest   = 1 &  cTofHits;

    bs.fEmcHitsGlobal = 1 & (cEmcHits>>4);
    bs.fEmcHitsEast   = 1 & (cEmcHits>>3);
    bs.fEmcHitsBarrel = 1 & (cEmcHits>>2);
    bs.fEmcHitsWest   = 1 & (cEmcHits>>1);
    bs.fEmcHitsSide   = 1 &  cEmcHits;

    bs.fMucHitsGlobal = 1 & (cMucHits>>3);
    bs.fMucHitsEast   = 1 & (cMucHits>>2);
    bs.fMucHitsBarrel = 1 & (cMucHits>>1);
    bs.fMucHitsWest   = 1 &  cMucHits;

    bs.fTracksGlobal  = 1 & (cTracks>>5);
    bs.fTracksMdc     = 1 & (cTracks>>4);
    bs.fTracksTof     = 1 & (cTracks>>3);
    bs.fTracksEmc     = 1 & (cTracks>>2);
    bs.fTracksMuc     = 1 & (cTracks>>1);
    bs.fTracksExt     = 1 &  cTracks;
    bs.fZoom        = cZoom;

    //Long Peixun's update: Read CGEM flags
    //Long Peixun's update: Read CGEM flags
    bs.fCgemGlobal = 1 & (cCgem >> 3);
    bs.fCgemLayers = 1 & (cCgem >> 2);
    bs.fCgemXStrips = 1 & (cCgem >> 1);
    bs.fCgemVStrips = 1 & cCgem;
    bs.fCgemHitsGlobal = 1 & (cCgemHits >> 4);
    bs.fCgemHitsXStrip = 1 & (cCgemHits >> 3);
    bs.fCgemHitsVStrip = 1 & (cCgemHits >> 2);
    bs.fCgemHitsClusters = 1 & (cCgemHits >> 1);
    bs.fCgemHitsFiredHL = 1 & cCgemHits;

    return is;
}

ostream& operator<< (ostream& os,const BesStatus& bs) {
    return os << hex << " "
        << (bs.fMdcGlobal<<2) + (bs.fMdcTubes<<1)  + bs.fMdcWires << " "
        << (bs.fTofGlobal<<3) + (bs.fTofEast<<2)   + (bs.fTofBarrel<<1) + bs.fTofWest << " "
        << (bs.fEmcGlobal<<4) + (bs.fEmcEast<<3)   + (bs.fEmcBarrel<<2) + (bs.fEmcWest<<1) + bs.fEmcSide << " "
        << (bs.fMucGlobal<<4) + (bs.fMucEast<<3)   + (bs.fMucBarrel<<2) + (bs.fMucWest<<1) + bs.fMucStrips << " "
        << (bs.fFull3DMdc<<3) + (bs.fFull3DTof<<2) + (bs.fFull3DEmc<<1) + bs.fFull3DMuc << " "
        << (bs.fBeamPipe<<2)  + (bs.fZRPlaneOnXY<<1) + bs.fAxis << " "
        << (bs.fMdcHitsGlobal<<1) + bs.fMdcHits << " "
        << (bs.fTofHitsGlobal<<3) + (bs.fTofHitsEast<<2) + (bs.fTofHitsBarrel<<1) + bs.fTofHitsWest << " "
        << (bs.fEmcHitsGlobal<<4) + (bs.fEmcHitsEast<<3) + (bs.fEmcHitsBarrel<<2) + (bs.fEmcHitsWest<<1) + bs.fEmcHitsSide << " "
        << (bs.fMucHitsGlobal<<3) + (bs.fMucHitsEast<<2) + (bs.fMucHitsBarrel<<1) + bs.fMucHitsWest << " "
        << (bs.fTracksGlobal<<5)  + (bs.fTracksMdc<<4)   + (bs.fTracksTof<<3)     + (bs.fTracksEmc<<2)  + (bs.fTracksMuc<<1)  + bs.fTracksExt << " " << bs.fZoom << " "
        //Long Peixun's update: Write CGEM flags
        << (bs.fCgemGlobal << 3) + (bs.fCgemLayers << 2) + (bs.fCgemXStrips << 1) + bs.fCgemVStrips << " "
        << (bs.fCgemHitsGlobal << 4) + (bs.fCgemHitsXStrip << 3) + (bs.fCgemHitsVStrip << 2) + (bs.fCgemHitsClusters << 1) + bs.fCgemHitsFiredHL;
}
