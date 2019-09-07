#include "MyMdcGeomSvc.hh"
#include <fstream> 
#include <iostream>
#include <sstream>
using namespace std;
#include <stdlib.h>
#include <assert.h>


MyMdcGeomSvc::MyMdcGeomSvc(){ReadFilePar();}
MyMdcGeomSvc::~MyMdcGeomSvc(){}
void MyMdcGeomSvc::ReadFilePar()
{
	std::string geometryFilePath1 = getenv("MDCSIMROOT");
	geometryFilePath1 +="/dat/MdcElectronics.txt";

	std::ifstream in;
	in.open(geometryFilePath1.c_str());
	assert(in);
	const int maxCharOneLine=255;
	char  temp[maxCharOneLine];
	int i=0;
	while(in.peek()!=EOF)
	{
		in.getline(temp,maxCharOneLine);
		i++;
		if (temp[0]=='#')
			continue;
		// i is the row number of MdcElectronics.dat
		if(i==2) {istringstream(temp)>>fTotalElecLayerNo;
			continue;
		}
		if(i>=4&&i<=26)
		{
			istringstream(temp)>>fLayerr[i-4]>>fElecNo[i-4];
			continue;
		}
		if(i>28&&i<=51)
		{
			istringstream(temp)>>fX[i-29]>>fY[i-29]>>fZ[i-29]>>fR[i-29];
			continue;
		}
		if(i==55)
		{
			istringstream(temp)>>fBoxInStep[0]>>fBoxInStep[1]>>fBoxInStep[2];
			continue;
		}
		if(i==59)
		{ 
			istringstream(temp)>>fBoxEndcape[0]>>fBoxEndcape[1]>>fBoxEndcape[2];
			continue;
		}
		if(i==63){istringstream(temp)>>fFixRing[0]>>fFixRing[1]>>fFixRing[2];}
		if(i>=76&&i<87)
		{
			istringstream(temp)>>fInnerRCableFrame[i-76]>>fOuterRCableFrame[i-76]>>fLengthCableFrame[i-76]>>fZCableFrame[i-76];
			continue;
		}
		if(i==90)
		{
			istringstream(temp)>>fThinPlate[0]>>fThinPlate[1]>>fThinPlate[2];
			continue;  
		}

		if(i>=93&&i<105)
		{
			istringstream(temp)>>fXCenThinPlate[i-93]>>fYCenThinPlate[i-93]>> fZCenThinPlate[i-93]>>fRotateXAngle[i-93];
			continue;
		}


		if(i==107)
		{
			istringstream(temp)>>fLengthCableTub[0]>>fLengthCableTub[1]>>fLengthCableTub[2]>>fLengthCableTub[3]>>fLengthCableTub[4]>>fLengthCableTub[5]>>fLengthCableTub[6]>>fLengthCableTub[7]>>fLengthCableTub[8]>>fLengthCableTub[9]>>fLengthCableTub[10]>>fLengthCableTub[11]>>fLengthCableTub[12]; 
			continue;
		}
		if (i==109)
		{
			istringstream(temp)>>fLengthCableTub[13]>>fLengthCableTub[14]>>fLengthCableTub[15]>>fLengthCableTub[16]>>fLengthCableTub[17]>>fLengthCableTub[18]>>fLengthCableTub[19]>>fLengthCableTub[20]>>fLengthCableTub[21]>>fLengthCableTub[22]>>fLengthCableTub[23]>>fLengthCableTub[24];
			continue;
		}

	}
}


int MyMdcGeomSvc::TotalElecLayerNo()
{
	return fTotalElecLayerNo;
}
int MyMdcGeomSvc::Layerr(int n)
{
	return fLayerr[n];
}
int MyMdcGeomSvc::ElecNo(int n)
{
	return fElecNo[n];
}
double MyMdcGeomSvc::X(int n)
{
	return fX[n];
}
double MyMdcGeomSvc::Y(int n)
{
	return fY[n];
}
double MyMdcGeomSvc::Z(int n)
{
	return fZ[n];
}
double MyMdcGeomSvc::R(int n)
{
	return fR[n];
}
double MyMdcGeomSvc::BoxInStep(int n)
{
	return fBoxInStep[n];
}
double MyMdcGeomSvc::BoxEndcape(int n)
{
	return fBoxEndcape[n];
}
double MyMdcGeomSvc::LengthCableTub(int n)
{   
	return   fLengthCableTub[n];
}
double MyMdcGeomSvc::InnerRCableFrame(int n)
{
	return fInnerRCableFrame[n];
}
double MyMdcGeomSvc::OuterRCableFrame(int n)
{
	return  fOuterRCableFrame[n];
}
double MyMdcGeomSvc::LengthCableFrame(int n)
{
	return  fLengthCableFrame[n];

}
double MyMdcGeomSvc::ZCableFrame(int n)
{
	return  fZCableFrame[n];

}

double MyMdcGeomSvc::ThinPlate(int n)
{
	return   fThinPlate[n];
}
double MyMdcGeomSvc::XCenThinPlate(int n)
{
	return   fXCenThinPlate[n];
}
double MyMdcGeomSvc::YCenThinPlate(int n)
{
	return   fYCenThinPlate[n];
}
double MyMdcGeomSvc::ZCenThinPlate(int n)
{
	return   fZCenThinPlate[n];
}
double MyMdcGeomSvc::RotateXAngle(int n)
{
	return  fRotateXAngle[n];
}

double MyMdcGeomSvc::FixRing(int n)
{
	return  fFixRing[n];
}

