//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
///Description:
//Author: He Miao
//Created: Nov,12 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$ID: BesEmcParameter.hh

#include "BesEmcParameter.hh"
#include <fstream>
#include <strstream>
//#include "ReadBoostRoot.hh"
#include "EmcGeneralClass/EmcStructure.h"
using namespace std;

//Initialize static data member
BesEmcParameter* BesEmcParameter::fpInstance=0;

BesEmcParameter::BesEmcParameter()
{
}


BesEmcParameter::~BesEmcParameter()
{
}

// static method
//Access to an instance
BesEmcParameter& BesEmcParameter::GetInstance()
{
  if(!Exist()) {
    fpInstance=new BesEmcParameter;
    fpInstance->ReadData();
  }
  return *fpInstance;
}

bool BesEmcParameter::Exist()
{
  return fpInstance!=0;
}

void BesEmcParameter::Kill()
{
  if(Exist()) {
    delete fpInstance;
    fpInstance=0;
  }
}

void BesEmcParameter::ReadData()
{
  G4String ParaPath = getenv("EMCSIMROOT");
  if(!ParaPath){
    G4Exception("BOOST environment not set!");
  }
  
  G4String ParaPath1 = ParaPath;
  
  ParaPath += "/dat/BesEmc.txt";
  ifstream fin;
  fin.open(ParaPath);
  assert(fin);
  
  const int maxCharOfOneLine=255;
  char temp[maxCharOfOneLine],*p;
  int lineNo=0,inputNo=0;
  
  while(fin.peek()!=EOF)
  {
    fin.getline(temp,maxCharOfOneLine);
    p=temp;
    lineNo++;
    while(*p!='\0')
    {
      if(*p=='#')
      {
        *p='\0';  //delete the comments.
        break;
      }
      p++;
    }
    p=temp; //reset the pointer to the beginning of the string.
    while(*p==' '||*p=='\t')p++;
    if(*p=='\0')continue;
    inputNo++;
    switch(inputNo)
    {
      case 1:
        istrstream(p)>>array_size>>m_tau>>m_highRange>>m_midRange>>m_lowRange>>m_sampleTime>>m_bitNb>>m_photonsPerMeV>>m_nonuniformity>>m_peakTime>>m_timeOffset;
        break;
      case 2:
        istrstream(p)>>WorldRmin1>>WorldRmax1>>WorldRmin2>>WorldRmax2>>WorldDz>>WorldZPosition>>CrystalLength>>CrystalLength1;
        break;
      case 3:
        istrstream(p)>>cryNumInOneLayer[0]>>cryNumInOneLayer[1]>>cryNumInOneLayer[2]>>cryNumInOneLayer[3]>>cryNumInOneLayer[4]>>cryNumInOneLayer[5];
        break;
      case 4:
        istrstream(p)>>pentaInOneSector[0]>>pentaInOneSector[1]>>pentaInOneSector[2]>>pentaInOneSector[3]>>pentaInOneSector[4];
        break;
      case 5:
        istrstream(p)>>fTyvekThickness>>fAlThickness>>fMylarThickness;
        break;
      case 6:
        istrstream(p)>>BSCRmin>>BSCDz>>BSCRmin1>>BSCRmax1>>BSCRmin2>>BSCRmax2>>BSCDz1;
        break;
      case 7:
        istrstream(p)>>BSCAngleRotat>>BSCNbPhi>>BSCNbTheta;
        break;
      case 8:
        istrstream(p)>>BSCYFront0>>BSCYFront>>BSCYFront1>>BSCPosition0>>BSCPosition1;
        break;
      case 9:
        istrstream(p)>>TaperRingDz>>TaperRingThickness1>>TaperRingThickness2>>TaperRingThickness3
          >>TaperRingTheta>>TaperRingInnerLength>>TaperRingOuterLength;
      case 10:
        istrstream(p)>>rearBoxLength>>rearBoxDz>>HangingPlateDz>>OCGirderAngle>>rearCasingThickness;
      case 11:
        istrstream(p)>>orgGlassLengthX>>orgGlassLengthY>>orgGlassLengthZ
          >>PDLengthX>>PDLengthY>>PDLengthZ>>AlPlateDz>>PABoxDz>>PABoxThickness;
      case 12:
        istrstream(p)>>cableDr>>waterPipeDr>>waterPipeThickness
          >>SPBarThickness>>SPBarThickness1>>SPBarwidth>>EndRingDz>>EndRingDr>>EndRingRmin;
      default:
        ;
    }
  }
  fin.close();
  
  ifstream fin1;
  ParaPath1 += "/dat/EmcLightOutput.dat";
  fin1.open(ParaPath1);
  assert(fin1);
  for(G4int i=0;i<6240;i++) {
    fin1>>lightOutput[i];
  }
  fin1.close();
}

G4double BesEmcParameter::GetLightOutput(G4int partId, G4int theta, G4int phi)
{
  EmcStructure struc;
  struc.setEmcStruc();
  G4int index = struc.getGeomIndex(partId,theta,phi);
  if(index>=0&&index<6240) {
    return lightOutput[index];
  } else {
    return 1.;
  }
}
