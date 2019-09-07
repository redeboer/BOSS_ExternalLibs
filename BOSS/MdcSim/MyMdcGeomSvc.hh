
   #include <fstream>
   #include <sstream>
   #include <iostream>
   using namespace std;



   class MyMdcGeomSvc
    {
     public:
      MyMdcGeomSvc();
      ~MyMdcGeomSvc();
      void ReadFilePar();
      int TotalElecLayerNo();
      int Layerr(int n);
      int ElecNo(int n);
      double X(int n);
      double Y(int n);
      double Z(int n);
      double R(int n);
      double BoxInStep(int n);
      double BoxEndcape(int n);
      double FixRing(int n);
      double InnerRCableFrame(int n);
      double OuterRCableFrame(int n);
      double LengthCableFrame(int n);
      double ZCableFrame(int n);
      
      double ThinPlate(int n);
      double XCenThinPlate(int n);
      double YCenThinPlate(int n);
      double ZCenThinPlate(int n);
      double RotateXAngle(int n);
      double LengthCableTub(int n); 


     private:
      int fTotalElecLayerNo;
      int fLayerr[23];
      int fElecNo[23];
      double fX[23];
      double fY[23];
      double fZ[23];
      double fR[23];
      double fBoxInStep[3];
      double fBoxEndcape[3];
      double fFixRing[3];
      double fInnerRCableFrame[11];
      double fOuterRCableFrame[11];
      double fLengthCableFrame[11];
      double fZCableFrame[11];
      double fLengthCableTub[12];

      double fThinPlate[3];
      double fXCenThinPlate[12];
      double fYCenThinPlate[12];
      double fZCenThinPlate[12];
      double fRotateXAngle[12];

      
   };
