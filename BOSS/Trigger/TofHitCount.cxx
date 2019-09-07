//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     /
////
////---------------------------------------------------------------------------/
////
////Description:
////Author:   Caogf
////Created:  Feb, 2006
////Modified:
////Comment:
////
//
#include "Trigger/TofHitCount.h"

#include "Identifier/Identifier.h"
#include "Identifier/TofID.h"

#include "RawEvent/RawDataUtil.h"
#include "RawEvent/DigiEvent.h"

#include <map>
using namespace std;

TofHitCount* TofHitCount::tof_Pointer = 0;

TofHitCount* TofHitCount::get_Tof(void) {
  if(!tof_Pointer) tof_Pointer = new TofHitCount();
  return tof_Pointer;
}

TofHitCount::TofHitCount()
{
}
TofHitCount::~TofHitCount()
{
}
void TofHitCount::setTofDigi(std::vector<int>& vtofHit)
{
  barrelHit1.clear();
  barrelHit2.clear();
  ecapHit.clear();
  wcapHit.clear();

  for(std::vector<int>::iterator iter = vtofHit.begin(); iter != vtofHit.end(); iter++) {
    int tofId = (*iter);
    int part, layer, im;
    part = int (tofId/10000.);
    layer = int ((tofId - part*10000)/1000.);
    im = int ((tofId - part*10000 - layer*1000)/10.);
    if(part == 0) {
      // east end cap: 0-47
      if(find(ecapHit.begin(),ecapHit.end(),im) == ecapHit.end()) { 
        ecapHit.push_back(im); 
      }
    }
    else if (part == 2) {
      // west end cap: 0-47
      if(find(wcapHit.begin(),wcapHit.end(),im) == wcapHit.end()) {
        wcapHit.push_back(im);    
      }
    }
    else {
      if(layer == 0) {
        //the 1st layer in barrel: 0-87
        if(find(barrelHit1.begin(),barrelHit1.end(),im) == barrelHit1.end()) {
          barrelHit1.push_back(im);
        }
      }
      if(layer == 1) {
        //the 2nd layer in barrel: 0-87
        if(find(barrelHit2.begin(),barrelHit2.end(),im) == barrelHit2.end()) {
          barrelHit2.push_back(im);
        }
      }
    }
  }
}
