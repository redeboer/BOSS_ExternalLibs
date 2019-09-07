//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Handle database I/O and user interface 
//             for MDC geometry parameters
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 4 Dec, 2003
//Modified:
//Comment: Used in "BesMdc" now, should be insert in framwork later
//         The units are "mm" and "rad". 
//         Datum plane is the East Endplane of MDC.
//---------------------------------------------------------------------------//

#ifndef BesMdcGeoParameter_H
#define BesMdcGeoParameter_H 


#include <string>
#include <vector>


using namespace std;

class BesMdcWire{
public:
  BesMdcWire(void){};
  BesMdcWire(double length, double phi, double r, double rotateAngle);
  ~BesMdcWire(void){};

public:
  //Funtions for read data
  double Length(void)  const   {return fLength;}      //Full length
  double Phi(void) const       {return fPhi;}         //Phi on East Endplane
  double Phi(double z) const;   //Phi at different Z position
  double R(void) const         {return fRadius;}      
  double RotateCell(void) const {return fRotateCell;}
  double RotateAngle(void) const {return fRotateAngle;} // Half twist angle of stereo wire, 
                                                 // for staight wire =0.
  double X(void) const {return fX;} // X,Y on endplane
  double Y(void) const{return fY;} 
  double X(double);           // X,Y at different Z position 
  double Y(double);
  const string Name(void) const {return fName;} 
public:
  //Functions for set data
  void SetLength(double x)      {fLength=x;}
  void SetPhi(double x)         {fPhi=x;}
  void SetRadius(double x)      {fRadius=x;}
  void SetRotateCell(double x) {fRotateCell=x;}
  void SetRotateAngle(double x) {fRotateAngle=x;}
  void SetX(double x) {fX=x;}
  void SetY(double x) {fY=x;}
  void SetName(string x) {fName=x;}  

private:
  double fLength, fPhi, fRadius, fRotateCell,fRotateAngle, fX, fY;
  string fName;   //name of Layer
};


class BesMdcLayer:public BesMdcWire{
//Derived from BesMdcWire, "fPhi" now is the shift angle of first wire. 
public:
  BesMdcLayer(void){};
  ~BesMdcLayer(void){};

public:
  int WireNo(void)  const   {return fWireNo;}     //Wire number of this layer.
  int SumWireNo(void) const {return fSumWireNo;}  //Total wire number from layer 1 to 
                                             //this layer (include this layer).
  int BeginWireNo(void) const{return fBeginWireNo;} //Total wire number from layer 1 to 
                                              //this layer (not include this layer).
  int FirstWire(void) const{return fFirstWire;}     //0: first is field wire; 1: signal wire.
  double ShiftPhi(void) const{return fShiftPhi;}    // Phi between 2 wire of this layer.

public:
  void SetWireNo(int x)    {fWireNo=x;}
  void SetSumWireNo(int x) {fSumWireNo=x;}
  void SetBeginWireNo(int x) {fBeginWireNo=x;}
  void SetShiftPhi(double x) {fShiftPhi=x;}
  void SetFirstWire(int x) {fFirstWire=x;}
private:
  int fWireNo, fSumWireNo, fBeginWireNo, fFirstWire;
  double fShiftPhi;
};


class BesMdcMember{
//class used for construct tube and endplane in MC
public:
  BesMdcMember(void){};
  ~BesMdcMember(void){};

public:
  double Length(void){return fLength;} //Full length In Z direction
  double InnerR(void)     {return fInnerR;} //From Z axes to bottom surface of those segments.
  double OutR(void) {return fOutR;}  //From Z axes to upper surface of those segments.
  double Z(void) {return fZ;}  //Offset value of segment center from Z=0 
  string Name(void) {return fName;} //Name of segment

public:
  void SetLength(double x) {fLength=x;}
  void SetInnerR(double x) {fInnerR=x;}
  void SetOutR(double x)  {fOutR=x;}
  void SetZ(double x)  {fZ=x;}
  void SetName(string x) {fName=x;}
private:
  double fLength, fInnerR, fOutR, fZ;
  string fName;
};


class BesMdcGeoParameter{
public:
  BesMdcGeoParameter(void);
  ~BesMdcGeoParameter(void){};
  
  void InitFromFile(void);  //Get BesMdcGeoParameter from Mdc.txt
  void InitFromSvc(void);  //Get BesMdcGeoParameter from MdcGeomSvc
  void Dump(void);  //Output info

public:
  double SignalWireR() {return fSignalWireR;}
  double FieldWireR() {return fFieldWireR;}
  int Signal2Global(int x) {return  fSignalLayer[x];}
  BesMdcWire Wire(int);
  BesMdcWire SignalWire(int, int);

//  Remember there are 2 F8, F20, F36, F43!!!!!!!!!!!!!!!!!
  int LayerNo(void){return fLayerNo;}  
  int SignalLayerNo(void){return fSignalLayerNo;}
  const BesMdcLayer& Layer(int) const;
  const BesMdcLayer& SignalLayer(int) const;   

  int SegmentNo(void) {return fSegmentNo;}
  BesMdcMember Segment(int x) {return fMdcSegment[x];}

  static BesMdcGeoParameter * GetGeo(void);
private:

  double fSignalWireR, fFieldWireR;
  int fLayerNo, fWireNo; // Total layers & wires number of Mdc.
  int fSignalLayerNo;    // Total signal layers number.
  int fSignalLayer[50];  // Global layer number of signal layers. 

  BesMdcLayer fLayer[100];

  double fWirePhi[30000]; 

  int fSegmentNo; //Total number of Tube and Endplane segments.
  BesMdcMember fMdcSegment[100];

  static  BesMdcGeoParameter * fPointer;
};

#endif
