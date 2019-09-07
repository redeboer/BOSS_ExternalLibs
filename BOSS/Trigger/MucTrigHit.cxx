//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     /
//// 
////---------------------------------------------------------------------------/
////
////Description:
////Author:   Caogf
////Created:  March, 2007
////Modified:
////Comment:
////
//  
#include "Trigger/MucTrigHit.h"
  
#include "Identifier/Identifier.h"
#include "Identifier/MucID.h"
#include "RawEvent/RawDataUtil.h"
#include "RawEvent/DigiEvent.h"
#include <math.h>
using namespace std;

MucTrigHit* MucTrigHit::muc_Pointer = 0;

MucTrigHit* MucTrigHit::get_Muc(void) {
  if(!muc_Pointer) muc_Pointer = new MucTrigHit();
  return muc_Pointer;
}

MucTrigHit::MucTrigHit()
{
}

MucTrigHit::~MucTrigHit()
{
}

void MucTrigHit::getMucDigi(MucDigiCol* mucDigiCol)
{
  //reset values
  for(int i =0; i<8; i++)
    for(int j=0; j<4; j++)
      for(int k=0; k<112; k++)
      {
        hitBR[i][j][k] = 0;
      }
  for(int i =0; i<2; i++)
    for(int j=0; j<4; j++)
      for(int k=0; k<4; k++)
        for(int l=0; l<64; l++)
        {
          hitEC[i][j][k][l] = 0;
        } 

  int partId, segId, gapId, stripId;
  MucDigiCol::iterator iDigiCol;
  for(iDigiCol=mucDigiCol->begin(); iDigiCol!=mucDigiCol->end(); iDigiCol++)
  {
    const Identifier ident = (*iDigiCol)->identify();
    partId = MucID::part(ident);
    segId = MucID::seg(ident);
    gapId = MucID::gap(ident);
    stripId = MucID::strip(ident);
    if(partId == 1 && gapId < 4) {
      hitBR[segId][gapId][stripId] = 1;
    }
    if(partId == 0 && gapId <4) hitEC[0][segId][gapId][stripId] = 1; //east endcap
    if(partId == 2 && gapId <4) hitEC[1][segId][gapId][stripId] = 1; //west endcap
  }
}

void MucTrigHit::getFastOr()
{
  //reset values
  for(int i =0; i<8; i++)
    for(int j=0; j<4; j++)
      for(int k=0; k<7; k++)
      {
        foBR[i][j][k] = 0;
      }
  for(int i =0; i<2; i++)
    for(int j=0; j<4; j++)
      for(int k=0; k<4; k++)
        for(int l=0; l<4; l++)
        {
          foEC[i][j][k][l] = 0;
        }
  //get fastOr signal
  for(int i =0; i<8; i++)
    for(int j=0; j<4; j++)
      for(int k=0; k<112; k++)
      {
        if(hitBR[i][j][k]==1) {
          int nbox = (int) k/16;
          foBR[i][j][nbox] = 1;
        }
      }
  for(int i =0; i<2; i++)
    for(int j=0; j<4; j++)
      for(int k=0; k<4; k++)
        for(int l=0; l<64; l++)
        {
          if(hitEC[i][j][k][l] == 1) {
            int nbox = (int) l/16;
            foEC[i][j][k][nbox] = 1;
          }
        } 
}
