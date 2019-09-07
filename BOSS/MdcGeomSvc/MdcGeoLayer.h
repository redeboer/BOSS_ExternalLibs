// $Id: MdcGeoLayer.h,v 1.6 2010/03/26 00:45:11 zhangy Exp $ // -*-c++-*-
// header file for a class called "MdcGeoLayer"
#ifndef MDC_GEO_LAYER_H
#define MDC_GEO_LAYER_H

#include <CLHEP/Geometry/Point3D.h>
#include <iostream>
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif        

using namespace CLHEP;

class MdcGeoSuper;

class MdcGeoLayer {

    public:
        // Constructor.
        MdcGeoLayer():fId(0),fSLayer(0),fWirst(0),fSlant(0.),fnomSlant(0.),fRadius(0.),fLength(0.),fRCSiz1(0.),fRCSiz2(0.),
                      fPCSiz(0.),fNCell(0),fOffset(0.),fShift(0.),fnomOffset(0.),fnomShift(0.),fTwistF(0.),fTwistB(0.){
            HepPoint3D O(0,0,0);
            fOffF=O;
            fOffB=O;
            fSup=0;
            fGenLayer=0;
        };
        // Copy and Assignment
        MdcGeoLayer(const MdcGeoLayer& e){
            fId=e.Id();
            fSLayer=e.SLayer(); 
            fWirst=e.Wirst();
            fSlant=e.Slant();
            fnomSlant=e.nomSlant();
            fRadius=e.Radius();
            fLength=e.Length();
            fRCSiz1=e.RCSiz1();
            fRCSiz2=e.RCSiz2();
            fPCSiz=e.PCSiz();
            fNCell=e.NCell();
            fOffset=e.Offset();
            fShift=e.Shift();
            fnomOffset=e.nomOffset();
            fnomShift=e.nomShift();
            fTwistF=e.TwistF();
            fTwistB=e.TwistB();
            fOffF=e.OffF();
            fOffB=e.OffB();
            fSup=e.Sup();
            fGenLayer=e.Gen();
        };
        MdcGeoLayer& operator=(const MdcGeoLayer& e) {
            if(this!=&e)
            {
                fId=e.Id();
                fSLayer=e.SLayer(); 
                fWirst=e.Wirst();   
                fSlant=e.Slant();
                fnomSlant=e.nomSlant();
                fRadius=e.Radius();
                fLength=e.Length();
                fRCSiz1=e.RCSiz1();
                fRCSiz2=e.RCSiz2();
                fPCSiz=e.PCSiz();
                fNCell=e.NCell();
                fOffset=e.Offset();
                fShift=e.Shift();
                fnomOffset=e.nomOffset();
                fnomShift=e.nomShift();
                fTwistF=e.TwistF();
                fTwistB=e.TwistB();
                fOffF=e.OffF();
                fOffB=e.OffB();
                fSup=e.Sup();
                fGenLayer=e.Gen();
            }
            return *this;
        }

        // Destructor
        ~MdcGeoLayer(){};

    public: // Extractors
        int Id(void) const;
        int SLayer(void) const;  
        int Wirst(void) const;  
        double Slant(void) const;
        double nomSlant(void) const;
        double Radius(void) const;
        double Length(void) const;
        double RCSiz1(void) const;
        double RCSiz2(void) const;
        double PCSiz(void) const;
        int    NCell(void) const;
        double  Offset(void) const;
        double  Shift(void) const;
        double  nomOffset(void) const;
        double  nomShift(void) const;
        double TwistF(void) const;
        double TwistB(void) const;
        HepPoint3D OffF(void) const;
        HepPoint3D OffB(void) const;
        MdcGeoSuper * Sup(void) const;
        int Gen(void) const;

    public: // Modifiers
        int Id(int);
        int SLayer(int);   
        int Wirst(int);
        double Slant(double);
        double nomSlant(double);
        double Radius(double);
        double Length(double);
        double RCSiz1(double);
        double RCSiz2(double);
        double PCSiz(double);
        int    NCell(int);
        double  Offset(double);
        double  Shift(double);
        double  nomOffset(double);
        double  nomShift(double);
        double TwistF(double);
        double TwistB(double);
        HepPoint3D OffF(HepPoint3D);
        HepPoint3D OffB(HepPoint3D);
        MdcGeoSuper * Sup( MdcGeoSuper * );
        int Gen(int);

    private:
        int fId;
        int fSLayer;   //SignalLayer
        int fWirst;  //No. of 1st sense wire in this layer.
        double fSlant;
        double fnomSlant;
        double fRadius;
        double fLength;
        double fRCSiz1;
        double fRCSiz2;
        double fPCSiz;
        int    fNCell;
        double  fOffset;
        double  fShift;
        double  fnomOffset;
        double  fnomShift;
        double fTwistF;
        double fTwistB;
        HepPoint3D fOffF;
        HepPoint3D fOffB;
        MdcGeoSuper * fSup;
        int fGenLayer;
};

//-----------------------------------------------------
// Extractors
inline int    MdcGeoLayer::Id(void) const { return fId; }
inline int    MdcGeoLayer::SLayer(void) const { return fSLayer; } 
inline int    MdcGeoLayer::Wirst(void) const { return fWirst;}   
inline double MdcGeoLayer::Slant(void) const { return fSlant; }
inline double MdcGeoLayer::nomSlant(void) const { return fnomSlant; }
inline double MdcGeoLayer::Radius(void) const { return fRadius; } 
inline double MdcGeoLayer::Length(void) const { return fLength; }
inline double MdcGeoLayer::RCSiz1(void) const { return fRCSiz1; }
inline double MdcGeoLayer::RCSiz2(void) const { return fRCSiz2; }
inline double MdcGeoLayer::PCSiz(void) const { return fPCSiz; }
inline int    MdcGeoLayer::NCell(void) const { return fNCell; }
inline double  MdcGeoLayer::Offset(void) const { return fOffset; }
inline double  MdcGeoLayer::Shift(void) const { return fShift; }
inline double  MdcGeoLayer::nomOffset(void) const { return fnomOffset; }
inline double  MdcGeoLayer::nomShift(void) const { return fnomShift; }
inline double MdcGeoLayer::TwistF(void) const { return fTwistF; }
inline double MdcGeoLayer::TwistB(void) const { return fTwistB; }
inline HepPoint3D MdcGeoLayer::OffF(void) const { return fOffF;}
inline HepPoint3D MdcGeoLayer::OffB(void) const { return fOffB;}
inline MdcGeoSuper* MdcGeoLayer::Sup(void) const { return fSup; }
inline int MdcGeoLayer::Gen(void) const {return fGenLayer;}

// Modifiers
inline int    MdcGeoLayer::Id(int i) { return fId=i; }
inline int    MdcGeoLayer::SLayer(int i) { return fSLayer=i; }  
inline int    MdcGeoLayer::Wirst(int i) { return fWirst=i; }
inline double MdcGeoLayer::Slant(double i) { return fSlant=i; }
inline double MdcGeoLayer::nomSlant(double i) { return fnomSlant=i; }
inline double MdcGeoLayer::Radius(double i) { return fRadius=i; }
inline double MdcGeoLayer::Length(double i) { return fLength=i; }
inline double MdcGeoLayer::RCSiz1(double i) { return fRCSiz1=i; }
inline double MdcGeoLayer::RCSiz2(double i) { return fRCSiz2=i; }
inline double MdcGeoLayer::PCSiz(double i) { return fPCSiz=i; }
inline int    MdcGeoLayer::NCell(int i) { return fNCell=i; }
inline double  MdcGeoLayer::Offset(double i) { return fOffset=i; }
inline double  MdcGeoLayer::Shift(double i) { return fShift=i; }
inline double  MdcGeoLayer::nomOffset(double i) { return fnomOffset=i; }
inline double  MdcGeoLayer::nomShift(double i) { return fnomShift=i; }
inline double MdcGeoLayer::TwistF(double i) { return fTwistF=i; }
inline double MdcGeoLayer::TwistB(double i) { return fTwistB=i; }
inline HepPoint3D MdcGeoLayer::OffF(HepPoint3D i) { return fOffF=i;}
inline HepPoint3D MdcGeoLayer::OffB(HepPoint3D i) { return fOffB=i;}
inline MdcGeoSuper* MdcGeoLayer::Sup(MdcGeoSuper* i) { return fSup=i; }
inline int MdcGeoLayer::Gen(int i) {return fGenLayer=i;}
//-----------------------------------------------------

#endif /* MdcGeoLayer_CLASS */

