//BOOST --- BESIII Object_Oriented Simulation Tool               //
//Description:
//Author: Caogf
//Created: June, 2008
//Modified:
//Comment:
//---------------------------------------------------------------------//
//$ID: BesPipParameter.cc

#include "BesPipParameter.hh"
#include <fstream>
#include <strstream>
#include "ReadBoostRoot.hh"
using namespace std;

BesPipParameter::BesPipParameter()
{
}
BesPipParameter::~BesPipParameter()
{
}
void BesPipParameter::ReadData()
{
  G4String GeometryPath = getenv("BESSIMROOT");
  if(!GeometryPath){
    G4Exception("BOOST environment not set!");
  }
  GeometryPath += "/dat/BesPip.txt";

  ifstream fin;
  fin.open(GeometryPath);

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
        istrstream(p)>>goldLayer[0]>>goldLayer[1]>>goldLayer[2];
      break;
      case 2:
        istrstream(p)>>innerBe[0]>>innerBe[1]>>innerBe[2];
      break;
      case 3:
        istrstream(p)>>oilLayer[0]>>oilLayer[1]>>oilLayer[2];
      break;
      case 4:
        istrstream(p)>>outerBe[0]>>outerBe[1]>>outerBe[2];
      break;
      case 5:
        istrstream(p)>>innerBeSide[0]>>innerBeSide[1]>>innerBeSide[2]>>innerBeSide[3]>>innerBeSide[4];
      break;
      case 6:
        istrstream(p)>>innerAl[0]>>innerAl[1]>>innerAl[2]>>innerAl[3]>>innerAl[4];
      break;
      case 7:
        istrstream(p)>>AlRing[0]>>AlRing[1]>>AlRing[2]>>AlRing[3]>>AlRing[4];
      break; 
      case 8:
        istrstream(p)>>outerAl[0]>>outerAl[1]>>outerAl[2]>>outerAl[3]>>outerAl[4];
      break;
      case 9:
        istrstream(p)>>AgLayer[0]>>AgLayer[1]>>AgLayer[2]>>AgLayer[3]>>AgLayer[4];
      break;
      case 10:
        istrstream(p)>>CuLayer[0]>>CuLayer[1]>>CuLayer[2]>>CuLayer[3]>>CuLayer[4];
      break;
      case 11:
        istrstream(p)>>AlCover[0]>>AlCover[1]>>AlCover[2]>>AlCover[3]>>AlCover[4];
    }
  }
}
