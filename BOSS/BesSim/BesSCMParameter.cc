//BOOST --- BESIII Object_Oriented Simulation Tool               //
//Description:
//Author: Caogf
//Created: June, 2007
//Modified:
//Comment:
//---------------------------------------------------------------------//
//$ID: BesSCMParameter.cc

#include "BesSCMParameter.hh"
#include <fstream>
#include <strstream>
#include "ReadBoostRoot.hh"
using namespace std;

BesSCMParameter::BesSCMParameter()
{
}
BesSCMParameter::~BesSCMParameter()
{
}
void BesSCMParameter::ReadData()
{
  G4String GeometryPath = getenv("BESSIMROOT");
  if(!GeometryPath){
    G4Exception("BOOST environment not set!");
  }
  GeometryPath += "/dat/BesSCM.txt";
      	
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
        istrstream(p)>>innerCryo[0]>>innerCryo[1]>>innerCryo[2];
      break;
      case 2:
        istrstream(p)>>l1Adia[0]>>l1Adia[1]>>l1Adia[2];
      break;
      case 3:
        istrstream(p)>>innerShield[0]>>innerShield[1]>>innerShield[2];
      break;
      case 4:
        istrstream(p)>>l2Adia[0]>>l2Adia[1]>>l2Adia[2];
      break;
      case 5:
        istrstream(p)>>outerShield[0]>>outerShield[1]>>outerShield[2];
      break;
      case 6:
        istrstream(p)>>l3Adia[0]>>l3Adia[1]>>l3Adia[2];
      break; 	
      case 7:
        istrstream(p)>>endShield[0]>>endShield[1]>>endShield[2]>>endShield[3]>>endShield[4];
      break;	
      case 8:
        istrstream(p)>>l1insu[0]>>l1insu[1]>>l1insu[2];
	break;
      case 9:
	istrstream(p)>>coil[0]>>coil[1]>>coil[2];
	break;
      case 10:
	istrstream(p)>>l2insu[0]>>l2insu[1]>>l2insu[2];
	break;
      case 11:
        istrstream(p)>>supp[0]>>supp[1]>>supp[2];
	break;
      case 12:
	istrstream(p)>>l4Adia[0]>>l4Adia[1]>>l4Adia[2];
	break;
      case 13:
	istrstream(p)>>endCoil[0]>>endCoil[1]>>endCoil[2]>>endCoil[3]>>endCoil[4];
	break;
      case 14:
	istrstream(p)>>outerCryo[0]>>outerCryo[1]>>outerCryo[2];
	break;
      case 15:
	istrstream(p)>>endCryo[0]>>endCryo[1]>>endCryo[2]>>endCryo[3]>>endCryo[4];
	break;
      case 16:
	istrstream(p)>>rein[0]>>rein[1]>>rein[2]>>rein[3]>>rein[4];
	break;
      case 17:
	istrstream(p)>>pipe1[0]>>pipe1[1]>>pipe1[2]>>pipe1[3]>>pipe1[4]>>pipe1[5];
	break;
      case 18:
	istrstream(p)>>pipe2[0]>>pipe2[1]>>pipe2[2]>>pipe2[3]>>pipe2[4]>>pipe2[5];
	break;
      case 19:
	istrstream(p)>>pipe3[0]>>pipe3[1]>>pipe3[2]>>pipe3[3]>>pipe3[4]>>pipe3[5];
	break;
      case 20:
	istrstream(p)>>hole[0]>>hole[1]>>hole[2]>>hole[3]>>hole[4]>>hole[5]>>hole[6]>>hole[7]>>hole[8]>>hole[9]>>hole[10]>>hole[11]>>hole[12];
    }
  }
}

