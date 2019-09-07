//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
///Description:
//Author: He Miao
//Created: Nov,12 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$ID: ExtBesEmcParameter.cxx

#include "TrkExtAlg/ExtBesEmcParameter.h"
#include <fstream>
#include <strstream>
#include "ReadBoostRoot.hh"
#include <assert.h>

using namespace std;

//Initialize static data member
ExtBesEmcParameter* ExtBesEmcParameter::fpInstance=0;

ExtBesEmcParameter::ExtBesEmcParameter()
{}


ExtBesEmcParameter::~ExtBesEmcParameter()
{}

// static method
//Access to an instance
ExtBesEmcParameter& ExtBesEmcParameter::GetInstance()
{
  if(!Exist()) {
    fpInstance=new ExtBesEmcParameter;
    fpInstance->ReadData();
  }
  return *fpInstance;
}

bool ExtBesEmcParameter::Exist()
{
  return fpInstance!=0;
}

void ExtBesEmcParameter::Kill()
{
  if(Exist()) {
    delete fpInstance;
    fpInstance=0;
  }
}

void ExtBesEmcParameter::ReadData()
{
  //G4String ParaPath = ReadBoostRoot::GetBoostRoot();
  G4String ParaPath = getenv("SIMUTILROOT");

  if(!ParaPath){
    G4Exception("BOOST environment not set!");
  }
  ParaPath += "/dat/BesEmc.txt";
  G4cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<G4endl;
  G4cout<<ParaPath<<G4endl;
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
        istrstream(p)>>WorldRmin1>>WorldRmax1>>WorldRmin2>>WorldRmax2>>WorldDz>>WorldZPosition>>CrystalLength;
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
}
