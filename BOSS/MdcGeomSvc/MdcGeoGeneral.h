// $Id: MdcGeoGeneral.h,v 1.7 2010/03/26 00:45:11 zhangy Exp $ // -*-c++-*-
// a template header file for a class called "MdcGeoGeneral"
#ifndef MDC_GEO_GENERAL_H
#define MDC_GEO_GENERAL_H

#include <CLHEP/Geometry/Point3D.h>
#include <iostream>
#include <string>
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using namespace CLHEP;
using namespace std;
class MdcGeoGeneral {

    public:
        // Constructor.
        MdcGeoGeneral():fId(0),fRadius(0),fLength(0),fNCell(0),fOffset(0),fnomOffset(0),fShift(0),fnomShift(0),fTwistF(0),fTwistB(0),fPhi(0),fnomPhi(0),fFirst(0),
                        fSxEast(0.),fSyEast(0.),fSzEast(0.),fRxEast(0.),fRyEast(0.),fRzEast(0.),fSxWest(0.),fSyWest(0.),fSzWest(0.),fRxWest(0.),fRyWest(0.),fRzWest(0.)
        {
            HepPoint3D O(0,0,0);
            fOffF=O;
            fOffB=O;
        };
        // Copy and Assignment
        MdcGeoGeneral(const MdcGeoGeneral& e){
            fId=e.Id();
            fLayerName=e.LayerName(); 
            fRadius=e.Radius();
            fLength=e.Length();
            fNCell=e.NCell();
            fOffset=e.Offset();
            fShift=e.Shift();
            fnomOffset=e.nomOffset();
            fnomShift=e.nomShift();
            fTwistF=e.TwistF();
            fTwistB=e.TwistB();
            fOffF=e.OffF();
            fOffB=e.OffB();
            fPhi=e.Phi();    
            fnomPhi=e.nomPhi();
            fFirst=e.First(); 
            fSxEast=e.SxEast();
            fSyEast=e.SyEast();
            fSzEast=e.SzEast();
            fRxEast=e.RxEast();
            fRyEast=e.RyEast();
            fRzEast=e.RzEast();

            fSxWest=e.SxWest();
            fSyWest=e.SyWest();
            fSzWest=e.SzWest();
            fRxWest=e.RxWest();
            fRyWest=e.RyWest();
            fRzWest=e.RzWest();
        };
        MdcGeoGeneral& operator=(const MdcGeoGeneral& e) {
            if(this!=&e)
            {
                fId=e.Id();
                fLayerName=e.LayerName();  
                fLength=e.Length();
                fRadius=e.Radius();
                fNCell=e.NCell();
                fOffset=e.Offset();
                fShift=e.Shift();
                fnomOffset=e.nomOffset();
                fnomShift=e.nomShift();
                fTwistF=e.TwistF();
                fTwistB=e.TwistB();
                fOffF=e.OffF();
                fOffB=e.OffB();
                fPhi=e.Phi();  
                fnomPhi=e.nomPhi();
                fFirst=e.First(); 
                fSxEast=e.SxEast();
                fSyEast=e.SyEast();
                fSzEast=e.SzEast();
                fRxEast=e.RxEast();
                fRyEast=e.RyEast();
                fRzEast=e.RzEast();

                fSxWest=e.SxWest();
                fSyWest=e.SyWest();
                fSzWest=e.SzWest();
                fRxWest=e.RxWest();
                fRyWest=e.RyWest();
                fRzWest=e.RzWest();
            }
            return *this;
        }

        // Destructor
        ~MdcGeoGeneral(){};

    public: // Extractors
        int Id(void) const;
        string LayerName(void) const;  
        double Radius(void) const;
        double Length(void) const;
        int    NCell(void) const;
        double  Offset(void) const;
        double  Shift(void) const;
        double  nomOffset(void) const;
        double  nomShift(void) const;
        double TwistF(void) const;
        double TwistB(void) const;
        double Phi(void) const;   
        double nomPhi(void) const;
        int    First(void) const; 
        HepPoint3D OffF(void) const;
        HepPoint3D OffB(void) const;
        double SxEast(void) const;
        double SyEast(void) const;
        double SzEast(void) const;
        double RxEast(void) const;
        double RyEast(void) const;
        double RzEast(void) const;

        double SxWest(void) const;
        double SyWest(void) const;
        double SzWest(void) const;
        double RxWest(void) const;
        double RyWest(void) const;
        double RzWest(void) const;

    public: // Modifiers
        int Id(int);
        string LayerName(string i);  
        double Radius(double);
        double Length(double);
        int    NCell(int);
        double  Offset(double);
        double  Shift(double);
        double  nomOffset(double);
        double  nomShift(double);

        double TwistF(double);
        double TwistB(double);
        double Phi(double);   
        double nomPhi(double); 
        int    First(int);   
        HepPoint3D OffF(HepPoint3D);
        HepPoint3D OffB(HepPoint3D);
        double SxEast(double); 
        double SyEast(double); 
        double SzEast(double); 
        double RxEast(double); 
        double RyEast(double); 
        double RzEast(double); 

        double SxWest(double); 
        double SyWest(double); 
        double SzWest(double); 
        double RxWest(double); 
        double RyWest(double); 
        double RzWest(double); 


    private:
        int fId;
        string  fLayerName;   //name of layer
        double fRadius; //radius of layer(mm)
        double fLength; //wire length of the layer(mm)
        int    fNCell;     //# of cells of that layer
        double  fOffset;  // offset at endplate (unit in cell)
        double  fnomOffset;  // nominal offset at endplate (unit in cell)
        double  fShift;   //# of cells to be shifte at forward w.r.t. backward endplate
        double  fnomShift;   //nominal # of cells to be shifte at forward w.r.t. backward endplate
        double fTwistF; //twist angle for fwrd (rad) 
        double fTwistB; //twist angle for bckwrd (rad)
        double fPhi;    //Phi on East Endplate
        double fnomPhi;    //nominal Phi on East Endplate
        int    fFirst;    //0: first is field wire; 1: signal wire.
        double fSxEast;
        double fSyEast;
        double fSzEast;
        double fRxEast;
        double fRyEast;
        double fRzEast;

        double fSxWest;
        double fSyWest;
        double fSzWest;
        double fRxWest;
        double fRyWest;
        double fRzWest;
        HepPoint3D fOffF; //Offset of fwrd wire  
        HepPoint3D fOffB; //Offset of bckwrd wire
};  

//-----------------------------------------------------
// Extractors
inline int    MdcGeoGeneral::Id(void) const { return fId; }
inline string  MdcGeoGeneral::LayerName(void) const { return fLayerName; }
inline double MdcGeoGeneral::Radius(void) const { return fRadius; }
inline double MdcGeoGeneral::Length(void) const { return fLength; }
inline int    MdcGeoGeneral::NCell(void) const { return fNCell; }
inline double  MdcGeoGeneral::Offset(void) const { return fOffset; }
inline double  MdcGeoGeneral::Shift(void) const { return fShift; }
inline double  MdcGeoGeneral::nomOffset(void) const { return fnomOffset; }
inline double  MdcGeoGeneral::nomShift(void) const { return fnomShift; }
inline double MdcGeoGeneral::TwistF(void) const { return fTwistF; }
inline double MdcGeoGeneral::TwistB(void) const { return fTwistB; }
inline double MdcGeoGeneral::Phi(void) const { return fPhi; } 
inline double MdcGeoGeneral::nomPhi(void) const { return fnomPhi; }
inline int    MdcGeoGeneral::First(void) const { return fFirst; } 
inline HepPoint3D MdcGeoGeneral::OffF(void) const { return fOffF;}
inline HepPoint3D MdcGeoGeneral::OffB(void) const { return fOffB;}
inline double MdcGeoGeneral::SxEast(void) const { return fSxEast; }
inline double MdcGeoGeneral::SyEast(void) const { return fSyEast; }
inline double MdcGeoGeneral::SzEast(void) const { return fSzEast; }
inline double MdcGeoGeneral::RxEast(void) const { return fRxEast; }
inline double MdcGeoGeneral::RyEast(void) const { return fRyEast; }
inline double MdcGeoGeneral::RzEast(void) const { return fRzEast; }

inline double MdcGeoGeneral::SxWest(void) const { return fSxWest; }
inline double MdcGeoGeneral::SyWest(void) const { return fSyWest; }
inline double MdcGeoGeneral::SzWest(void) const { return fSzWest; }
inline double MdcGeoGeneral::RxWest(void) const { return fRxWest; }
inline double MdcGeoGeneral::RyWest(void) const { return fRyWest; }
inline double MdcGeoGeneral::RzWest(void) const { return fRzWest; }

// Modifiers
inline int    MdcGeoGeneral::Id(int i) { return fId=i; }
inline string MdcGeoGeneral::LayerName(string i) { return fLayerName = i; } 
inline double MdcGeoGeneral::Radius(double i) { return fRadius=i; }
inline double MdcGeoGeneral::Length(double i) { return fLength=i; }
inline int    MdcGeoGeneral::NCell(int i) { return fNCell=i; }
inline double  MdcGeoGeneral::Offset(double i) { return fOffset=i; }
inline double  MdcGeoGeneral::Shift(double i) { return fShift=i; }
inline double  MdcGeoGeneral::nomOffset(double i) { return fnomOffset=i; }
inline double  MdcGeoGeneral::nomShift(double i) { return fnomShift=i; }

inline double MdcGeoGeneral::TwistF(double i) { return fTwistF=i; }
inline double MdcGeoGeneral::TwistB(double i) { return fTwistB=i; }
inline double MdcGeoGeneral::Phi(double i) { return fPhi=i; } 
inline double MdcGeoGeneral::nomPhi(double i) { return fnomPhi=i; }

inline int    MdcGeoGeneral::First(int i) { return fFirst=i; }
inline HepPoint3D MdcGeoGeneral::OffF(HepPoint3D i) { return fOffF=i;}
inline HepPoint3D MdcGeoGeneral::OffB(HepPoint3D i) { return fOffB=i;}

inline double MdcGeoGeneral::SxEast(double i) { return fSxEast=i; }
inline double MdcGeoGeneral::SyEast(double i) { return fSyEast=i; }
inline double MdcGeoGeneral::SzEast(double i) { return fSzEast=i; }
inline double MdcGeoGeneral::RxEast(double i) { return fRxEast=i; }
inline double MdcGeoGeneral::RyEast(double i) { return fRyEast=i; }
inline double MdcGeoGeneral::RzEast(double i) { return fRzEast=i; }

inline double MdcGeoGeneral::SxWest(double i) { return fSxWest=i; }
inline double MdcGeoGeneral::SyWest(double i) { return fSyWest=i; }
inline double MdcGeoGeneral::SzWest(double i) { return fSzWest=i; }
inline double MdcGeoGeneral::RxWest(double i) { return fRxWest=i; }
inline double MdcGeoGeneral::RyWest(double i) { return fRyWest=i; }
inline double MdcGeoGeneral::RzWest(double i) { return fRzWest=i; }


//-----------------------------------------------------

#endif /* MdcGeoGeneral_CLASS */

