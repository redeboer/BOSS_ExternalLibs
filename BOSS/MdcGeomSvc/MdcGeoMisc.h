// $Id: MdcGeoMisc.h,v 1.2 2006/11/14 02:33:21 panmh Exp $ // -*-c++-*-
// a template header file for a class called "MdcGeoMisc"
#ifndef MDC_GEO_MISC_H
#define MDC_GEO_MISC_H

#include <iostream>

class MdcGeoMisc {

  public:
    // Constructor.
     MdcGeoMisc():fOuterR(0),fInnerR(0),fOuterTk(0),fInnerTk(0),fNSWire(0),fNFWire(0),
    fLayerNo(0),fWireNo(0),fSLayerNo(0),fSWireR(0),fFWireR(0){};

    // Copy and Assignment
     MdcGeoMisc(const MdcGeoMisc& e):fOuterR(e.OuterR()),fInnerR(e.InnerR()),fOuterTk(e.OuterTk()),fInnerTk(e.InnerTk()),fNSWire(e.NSWire()),fNFWire(e.NFWire()),
    fLayerNo(e.LayerNo()),fWireNo(e.WireNo()),fSLayerNo(e.SLayerNo()),fSWireR(e.SWireR()),fFWireR(e.FWireR()){};

     MdcGeoMisc& operator=(const MdcGeoMisc& e) {
       if(this!=&e)
        {
	  fOuterR=e.OuterR();
	  fInnerR=e.InnerR();
	  fOuterTk=e.OuterTk();
	  fInnerTk=e.InnerTk();
	  fNSWire=e.NSWire();
	  fNFWire=e.NFWire();

          fLayerNo=e.LayerNo();
          fWireNo=e.WireNo();
          fSLayerNo=e.SLayerNo();
          fSWireR=e.SWireR(); 
          fFWireR=e.FWireR();
        }
       return *this;
     }

    // Destructor
    ~MdcGeoMisc(){};

  public: // Extractors
    double OuterR(void) const { return fOuterR; };
    double InnerR(void) const { return fInnerR; };
    double OuterTk(void) const { return fOuterTk; };
    double InnerTk(void) const { return fInnerTk; };
    int NSWire(void) const { return fNSWire; };
    int NFWire(void) const { return fNFWire; };

    int LayerNo(void) const { return fLayerNo;}
    int WireNo(void) const { return fWireNo;}
    int SLayerNo(void) const { return fSLayerNo;}
    double SWireR(void) const { return fSWireR; }
    double FWireR(void) const { return fFWireR; }


  public: // Modifiers
    double OuterR(double i) { return fOuterR=i; };
    double InnerR(double i) { return fInnerR=i; };
    double OuterTk(double i) { return fOuterTk=i; };
    double InnerTk(double i) { return fInnerTk=i; };
    int NSWire(int i) { return fNSWire=i; };
    int NFWire(int i) { return fNFWire=i; };

    int LayerNo(int i) { return fLayerNo=i;}
    int WireNo(int i)  { return fWireNo=i;}
    int SLayerNo(int i) { return fSLayerNo=i;}
    double SWireR(double i)  { return fSWireR=i;}
    double FWireR(double i)  { return fFWireR=i;}


  private:
    double fOuterR;  //radius of outer cylinder (mm)
    double fInnerR;  //radius of inner cylinder (mm)
    double fOuterTk; //thickness of outer wall  (mm)
    double fInnerTk; //thickness of inner wall  (mm)
    int    fNSWire;  //# of sense wires
    int    fNFWire;  //# of field wires

   int fLayerNo;    // Total layers  number of Mdc
   int fWireNo;     // Total wires number of Mdc
   int fSLayerNo;   // Total signal layers number
   double fSWireR;  //radius of signalwire (micrometer)
   double fFWireR;  //radius of fieldwire (micrometer)
};

#endif /* MdcGeoMisc_CLASS */


