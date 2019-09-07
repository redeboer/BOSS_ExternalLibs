// $Id: MdcGeoWire.h,v 1.6 2010/03/26 00:45:11 zhangy Exp $ // -*-c++-*-
// header file for a class called "MdcGeoWire"
#ifndef MDC_GEO_WIRE_H
#define MDC_GEO_WIRE_H

#include <CLHEP/Geometry/Point3D.h>
#include <iostream>
#include "MdcGeomSvc/MdcGeoLayer.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif        

using namespace CLHEP;


class MdcGeoWire {

    public:
        // Constructor.
        MdcGeoWire()
            : fId(0),
            fSlant(0),
            fnomSlant(0),
            fCell(0),
            fLayer(0),
            fStat(0)
            {
                HepPoint3D O(0,0,0);
                fBackward=O;
                fForward=O;
                fBwirepos=O;
                fFwirepos=O;
                fnomBackward=O;
                fnomForward=O;
                fLyr=NULL;
            }

        // Copy and Assignment
        MdcGeoWire(const MdcGeoWire& e){
            fId=e.Id();
            fBackward=e.Backward(); 
            fForward=e.Forward();
            fBwirepos=e.BWirePos(); 
            fFwirepos=e.FWirePos();
            fSlant=e.Slant();
            fCell=e.Cell();
            fLayer=e.Layer();
            fStat=e.Stat();
            fLyr=e.Lyr();
        };

        MdcGeoWire& operator=(const MdcGeoWire& e) {
            if(this!=&e)
            {
                fId=e.Id();
                fBackward=e.Backward();
                fForward=e.Forward();
                fBwirepos=e.BWirePos();
                fFwirepos=e.FWirePos();
                fSlant=e.Slant();
                fCell=e.Cell();
                fLayer=e.Layer();
                fStat=e.Stat();
                fLyr=e.Lyr();
            }
            return *this;
        }

        // Destructor
        ~MdcGeoWire(){ };

    public: // Extractors
        int Id(void) const;
        HepPoint3D Backward(void) const;
        HepPoint3D Forward(void) const;
        HepPoint3D BWirePos(void) const;
        HepPoint3D FWirePos(void) const;
        double Slant(void) const;
        HepPoint3D nomBackward(void) const;
        HepPoint3D nomForward(void) const;
        double nomSlant(void) const;
        double Tension(void) const;
        const double Sag(void) const;
        const double Sagz(const double z) const;

        int Cell(void) const;
        int Layer(void) const;
        int Stat(void) const;
        MdcGeoLayer * Lyr(void) const;

    public: // Modifiers
        int Id(int);
        HepPoint3D Backward(HepPoint3D);
        HepPoint3D Forward(HepPoint3D);
        HepPoint3D BWirePos(HepPoint3D);
        HepPoint3D FWirePos(HepPoint3D);
        double Slant(double);
        HepPoint3D nomBackward(HepPoint3D);
        HepPoint3D nomForward(HepPoint3D);
        double nomSlant(double);
        double Tension(double);

        int Cell(int);
        int Layer(int);
        int Stat(int);
        MdcGeoLayer*  Lyr( MdcGeoLayer * );

    private:
        int fId;
        HepPoint3D fBackward;  //wire position at backward endplate (mm)
        HepPoint3D fForward;   //wire position at forward endplate (mm)
        HepPoint3D fBwirepos;  //wire position at backward endplate (mm)
        HepPoint3D fFwirepos;   //wire position at forward endplate (mm)
        double fSlant;         //slant angle of wire (rad)
        HepPoint3D fnomBackward;  //nominal wire position at backward endplate (mm)
        HepPoint3D fnomForward;   //nominal wire position at forward endplate (mm)
        double fnomSlant;         //nominal slant angle of wire (rad)
        double fTension;          // the tension of wire

        int fCell;		   //#cell
        int fLayer;		   //#layer
        int fStat;	           //status (32 bits)
        MdcGeoLayer * fLyr;   
};

// Extractors
inline int MdcGeoWire::Id(void) const { return fId; }
inline HepPoint3D MdcGeoWire::Backward(void) const { return fBackward;}
inline HepPoint3D MdcGeoWire::Forward(void) const { return fForward;}
inline HepPoint3D MdcGeoWire::BWirePos(void) const { return fBwirepos;}
inline HepPoint3D MdcGeoWire::FWirePos(void) const { return fFwirepos;}
inline double MdcGeoWire::Slant(void) const { return fSlant; }
inline HepPoint3D MdcGeoWire::nomBackward(void) const { return fnomBackward;}
inline HepPoint3D MdcGeoWire::nomForward(void) const { return fnomForward;}
inline double MdcGeoWire::nomSlant(void) const { return fnomSlant; }
inline double MdcGeoWire::Tension(void) const { return  fTension;}
inline int MdcGeoWire::Cell(void) const { return fCell; }
inline int MdcGeoWire::Layer(void) const { return fLayer; }
inline int MdcGeoWire::Stat(void) const { return fStat; }
inline MdcGeoLayer* MdcGeoWire::Lyr(void) const { return fLyr; }

// Modifiers
inline int MdcGeoWire::Id(int i) {return fId=i;}
inline HepPoint3D MdcGeoWire::Backward(HepPoint3D i) {return fBackward=i;}
inline HepPoint3D MdcGeoWire::Forward(HepPoint3D i) {return fForward=i;}
inline HepPoint3D MdcGeoWire::BWirePos(HepPoint3D i) {return fBwirepos=i;}
inline HepPoint3D MdcGeoWire::FWirePos(HepPoint3D i) {return fFwirepos=i;}
inline HepPoint3D MdcGeoWire::nomBackward(HepPoint3D i) {return fnomBackward=i;}
inline HepPoint3D MdcGeoWire::nomForward(HepPoint3D i) {return fnomForward=i;}
inline double MdcGeoWire::Slant(double i) {return fnomSlant=i;}
inline double MdcGeoWire::nomSlant(double i) {return fnomSlant=i;}
inline double MdcGeoWire::Tension(double i) {return  fTension=i;}
inline int MdcGeoWire::Cell(int i) {return fCell=i;}
inline int MdcGeoWire::Layer(int i) {return fLayer=i;}
inline int MdcGeoWire::Stat(int i) {return fStat=i;}
inline MdcGeoLayer* MdcGeoWire::Lyr(MdcGeoLayer* i) {return fLyr = i;}

#endif /* MdcGeoWire_CLASS */

