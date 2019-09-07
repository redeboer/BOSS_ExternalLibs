//
// ZHelix.cxx
//
// $Author: longpx $
// $Date: 2018/07/12 00:47:24 $
// $Revision: 1.4 $
//

#include "BesVisLib/ZHelix.h"
#include <iostream>

#include <TMath.h>

#ifndef ROOT_TString
#include <TString.h>
#endif

#ifndef ZEVIS_ZView
#include "BesVisLib/BesView.h"
#endif

#ifndef ZEVIS_ZPad
#include "TPad.h"
#endif

using namespace std;

#ifndef __CINT__
ClassImp(ZHelix)
#endif

//__________________________________________________________
// ZHelix
// Helix class
//
// Drawing options:
//   '3D'     - 3 dimensional view
//   'XY'     - XY projection
//   'ZR'     - ZR projection
//
ZHelix::ZHelix() : TPolyLine3D() {
    //
    // ZHelix standard constructor
    if ( gDebug ) cout << "ZHelix default ctor called" << endl;

    fRSign = 0;
    //  fSegment = 1.;
}

//__________________________________________________________

ZHelix::ZHelix(Double_t Azim, Double_t QovR, Double_t QxDh,
               Double_t refx, Double_t refy,
               Double_t refz, Double_t TDip,
               Double_t phii, Double_t phio,
               Float_t Chi2, Int_t NDoF,
               EZHelixRangeType RangeType,
               Double_t RangeMin, Double_t RangeMax) :
        TPolyLine3D() {
    //
    // ZHelix normal constructor
    if ( gDebug ) cout << "ZHelix normal ctor called" << endl;

    fAzim     = Azim;
    fSinAzim  = TMath::Sin(Azim);
    fCosAzim  = TMath::Cos(Azim);
    fQovR     = QovR;
    fQxDh     = QxDh;
    fRefX     = refx;
    fRefY     = refy;
    fRefZ     = refz;
    fTDip     = TDip;
    fPhiI     = phii;
    fPhiO     = phio;
    fChi2     = Chi2;
    fNDoF     = NDoF;
    fRType    = RangeType;
    fEnable   = kTRUE;

    fRSign = 0;
    //  fSegment = 1.;
    SetRange(RangeType, RangeMin, RangeMax);
}

//__________________________________________________________

ZHelix::~ZHelix() {
    //
    // ZHelix standard destructor
    if ( gDebug ) {
        cout << "ZHelix dtor called" << endl;
    }
}

//__________________________________________________________

void ZHelix::SetRange(EZHelixRangeType RangeType,
                      Double_t RangeMin, Double_t RangeMax) {
    //
    // Set range for drawing
    switch (RangeType) {
    case kHelixPhi:
        fRange[0] = RangeMin;
        fRange[1] = RangeMax;
        break;
    case kHelixX:
        fRange[0] = X2Phi(RangeMin);
        fRange[1] = X2Phi(RangeMax);
        break;
    case kHelixY:
        fRange[0] = Y2Phi(RangeMin);
        fRange[1] = Y2Phi(RangeMax);
        break;
    case kHelixZ:
        fRange[0] = Z2Phi(RangeMin);
        fRange[1] = Z2Phi(RangeMax);
        break;
    case kHelixR:
        fRange[0] = R2Phi(RangeMin);
        fRange[1] = R2Phi(RangeMax);
        break;
    }
}

//__________________________________________________________

void ZHelix::SetPoints(Option_t *option) {
    //
    // Set PolyLine3D points
    TString opt = option;
    opt.ToUpper();

    // Get view
    BesView *view = dynamic_cast<BesView*>(gPad->GetView());

    // check with which sign the segmnents have to added to fRange[0]
    Int_t sign = 0;
    if ( fQovR >= 0 )
        sign = -1;
    else
        sign = +1;

    // calculate distance between ranges in rad
    Double_t degrad  = TMath::Pi() / 180.0;
    Double_t segment = 1. * degrad;   // 1 degree segments
    Double_t df = 0;
    if ( sign == +1 ) {
        if ( fRange[1] > fRange[0] )
            df = fRange[1]-fRange[0];
        else
            df = 2*TMath::Pi() + fRange[1]-fRange[0];
    } else {
        if ( fRange[1] < fRange[0] )
            df = fRange[1]-fRange[0];
        else
            df = 2*TMath::Pi() - fRange[1]+fRange[0];
    }
    // number of segments with degree distance
    Int_t nSeg = Int_t(TMath::Abs(df) / segment) + 1;

    // solve problem with nseg = 1 , means df too small compared to
    // segment -> new segment
    if ( nSeg < 2 ) {
        segment = 0.01 * degrad;
        nSeg = Int_t(TMath::Abs(df) / segment) + 1;
    }
    //  if ( nSeg < 10 ) nSeg = 10;

    // newly calculation of segment
    segment = TMath::Abs(df) / nSeg;

    Double_t phi, x, y, z, r;
    TPolyLine3D::SetPolyLine(nSeg+1);

    if ( opt.Contains("3D") ) {
        // 3D view
        for ( Int_t i = 0; i <= nSeg; i++ ) {
            phi = fRange[0] + segment * i * sign;
            Phi2XYZ(phi, x, y, z);
            TPolyLine3D::SetPoint(i, x, y, z);
        }
    } else if ( opt.Contains("XY") ) {
        // XY view
        for ( Int_t i = 0; i <= nSeg; i++ ) {
            phi = fRange[0] + segment * i * sign;
            Phi2XYZ(phi, x, y, z);
            TPolyLine3D::SetPoint(i, x, y, 0);
        }
    } else if ( opt.Contains("ZR") ) {
        // ZR view

        // The track is drawn either in the upper or lower region of
        // the ZR view depending on how many of its segements lay in
        // each region. If these numbers are unbalanced, the starting
        // point of the track is taken as criterium
        Int_t isgn = 0;
        for ( Int_t i = 0; i <= nSeg; i++ ) {

            // Get ZR coordinates
            phi = fRange[0] + segment * i * sign;
            Phi2ZR(phi, z, r);

            // Set point
            TPolyLine3D::SetPoint(i, z, r, 0);

            // Get sign for R coordinate
            isgn += view->GetRSign(phi);
            if ( i == 0 ) fRSign = view->GetRSign(phi);
        }
        if ( isgn != 0 ) fRSign = TMath::Sign(1, isgn);

        // The vertex fitted tracks need to be displaced otherwise they are
        // are not drawn to emanate from the vertex.

        Float_t z, r, rref, rdisp;

        rref = TMath::Sqrt((fRefX*fRefX)+(fRefY*fRefY));

        if ( (fTrackType != kVctpar) || (fTrackType != kVcparsec) || (fTrackType != kZttrprm) || (fTrackType != kZttrsec) ) {

            // Set R sign for each point
            for ( Int_t i = 0; i <= nSeg; i++ ) {
                z = GetP()[i*3];
                r = GetP()[i*3+1];
                SetPoint(i, z, r*fRSign, 0);
            }

        } else {

            for ( Int_t i = 0; i <= nSeg; i++ ) {
                z = GetP()[i*3];
                r = GetP()[i*3+1];
                rdisp = r*fRSign;
                if (fRSign < 0) rdisp += 2*rref;
                SetPoint(i, z, rdisp, 0);
            }

        }

    }

}

//__________________________________________________________

void ZHelix::Phi2XYZ(Double_t phi, Double_t& x, Double_t& y, Double_t& z) {
    //
    // Calculate XYZ for a given azimuth
    x = fRefX - TMath::Sin(phi) / fQovR + fSinAzim * (1/fQovR + fQxDh);
    y = fRefY + TMath::Cos(phi) / fQovR - fCosAzim * (1/fQovR + fQxDh);
    z = fRefZ + (fAzim - phi) * fTDip / fQovR;
}

//__________________________________________________________

void ZHelix::Phi2ZR(Double_t phi, Double_t& z, Double_t& r) {
    //
    // Calculate ZR for a given azimuth
    Double_t x, y;
    Phi2XYZ(phi, x, y, z);
    r = TMath::Sqrt(TMath::Power(x,2) + TMath::Power(y,2));
}

//__________________________________________________________

Double_t ZHelix::X2Phi(Double_t x) {
    //
    // Convert x to phi
    Double_t phi = TMath::ASin(fQovR * (fSinAzim*(1/fQovR + fQxDh) - x + fRefX));

    return phi;
}

//__________________________________________________________

Double_t ZHelix::Y2Phi(Double_t y) {
    //
    // Convert y to phi
    Double_t phi = TMath::ACos(fQovR * (fCosAzim*(1/fQovR + fQxDh) + y - fRefY));

    return phi;
}

//__________________________________________________________

Double_t ZHelix::Z2Phi(Double_t z) {
    //
    // Convert z to phi
    Double_t phi = fAzim - fQovR * (z - fRefZ) / fTDip;

    return phi;
}

//__________________________________________________________

Double_t ZHelix::R2Phi(Double_t r) {
    //
    // Convert r to phi
    Double_t k = 1/fQovR + fQxDh;
    Double_t my_value = fQovR/(2*k) * (-TMath::Power(r,2) + 1/TMath::Power(fQovR,2) + TMath::Power(k,2));
    // check if my_value is ouside acos validity range
    if ( (my_value < -1) || (my_value > 1 ) )
        return 999999999;
    Double_t my_phi = TMath::ACos(my_value);

    Double_t phi1 = fAzim - my_phi;
    Double_t phi2 = fAzim + my_phi;

    Double_t x1  = 0;
    Double_t y1  = 0;
    Double_t z1  = 0;
    Double_t x2  = 0;
    Double_t y2  = 0;
    Double_t z2  = 0;
    Double_t phi = 0;;
    this->Phi2XYZ(phi1,x1,y1,z1);
    this->Phi2XYZ(phi2,x2,y2,z2);


    // check if phi1 would result in a z value with the correct sign corrsponding
    // to fTDip
    if ( TMath::Sign(1.,fTDip) == TMath::Sign(1.,(z1-fRefZ)) ) {
        // check if phi2 would result in a z value with the correct sign corrsponding
        // to fTDip
        if ( TMath::Sign(1.,fTDip) == TMath::Sign(1.,(z2-fRefZ)) ) {
            // take phi with smallest resulting z difference to fRefZ (Zh)
            if ( TMath::Abs(z1-fRefZ) <= TMath::Abs(z2-fRefZ) )
                phi = phi1;
            else
                phi = phi2;
        } else
            phi = phi1;
    } else {
        phi = phi2;
    }

    return phi;
}

//__________________________________________________________

void ZHelix::Print(Option_t *option) const {
    //
    // Dump the helix attributes
    TString opt = option;
    opt.ToUpper();

    cout << endl
    << "Parameters of helix " << this->GetName() << ":" << endl
    << "Azimuth   = " << fAzim << endl
    << "Q/R       = " << fQovR << endl
    << "Q*D_h     = " << fQxDh << endl
    << "Z_h       = " << fRefZ << endl
    << "cot(Dip)  = " << fTDip << endl
    << "PhiI      = " << fPhiI << endl
    << "PhiO      = " << fPhiO << endl
    << "Chi2      = " << fChi2 << endl
    << "D.o.F.    = " << fNDoF << endl
    << "RangeType = " << fRType << endl
    << "Range: " << fRange[0] << " -> " << fRange[1] << endl;
    if ( opt.Contains("ALL") ) TPolyLine3D::Print("ALL");
    cout << endl;
}

//__________________________________________________________

void ZHelix::Draw(Option_t *option) {
    //
    // Draw this helix with its current attributes.
    AppendPad(option);
}

//__________________________________________________________

void ZHelix::Paint(Option_t *option) {
    //
    // Draw this helix with its current attributes
    TString opt = option;
    opt.ToUpper();

    if ( ( (fRange[1] < fRange[0]) || (fRange[1] > fRange[0]) ) && fEnable ) {
        SetPoints(option);
        TPolyLine3D::Paint("SAME");
    }
}

//__________________________________________________________

Double_t ZHelix::Phi2S(Double_t phi) {
    //
    // calculates pathlength for phi

    return -(phi-fAzim)/fQovR;

}




