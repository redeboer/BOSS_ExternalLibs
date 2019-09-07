// MdcPlotHits.cc
// 
#ifdef MDCDISPLAY

//#include "BaBar/BaBar.h"
#include <stdlib.h>
#include "MdcTrkRecon/MdcPlotHits.h"
#include "MdcData/MdcHitOnTrack.h"
#ifdef MDCDISPLAY
#include "MdcOldGraphics/MdcDisplay.h"
#endif
#include "MdcTrkRecon/dchGlobal.h"
#include "MdcData/MdcHit.h"
#include "MdcData/MdcHitOnTrack.h"
#include "CLHEP/Alist/AIterator.h"
using std::cerr;
using std::endl;

int MdcPlotHits::axialColor = 1;
int MdcPlotHits::posStereoColor = 1;
int MdcPlotHits::negStereoColor = 1;

MdcPlotHits::MdcPlotHits() {
}

//**********************************************************************
void MdcPlotHits::plotHits(const HepAList<TrkHitOnTrk> &hits, double tbunch, 
			   int incolor) const {
//**********************************************************************

  int mycolor = 0;
  for (int ihit = 0; ihit < (int) hits.length(); ihit++) {
    const MdcHitOnTrack *dclink = hits[ihit]->dchHitOnTrack();
    if (dclink == 0) continue;
    MdcHit *ahit = dclink->dchHit();
    if (ahit == 0) continue;
    int generAmbig = 0;
    int foundAmbig = dclink->ambig();
    //    if (ahit->mcDrift() > 0.) generAmbig = 1;
    //    else if (ahit->mcDrift() < 0.) generAmbig = -1;
    if (dclink->isActive() == 0) { // Switch to lighter colors for inactive hits
      mycolor = 7;
    }
    //    else if ( foundAmbig!= generAmbig && 
    //	     generAmbig != 0) mycolor = 12;

    else mycolor = incolor;
    this->plotAHit(ahit, tbunch, 1, mycolor, dclink->timeIndex());
  }
  
  return;
}

//**********************************************************************
void MdcPlotHits::plotAHit(const MdcHit *hit, double tbunch, int mode, 
			   int incolor, int tIndex) const {
//**********************************************************************
  // incolor = +int -- use that color
  // incolor = -1   -- use default colors 
  // mode = -1 -- use natural mode
  // mode = +int -- use that mode
  // tIndex -- which time to use in the hit

  // Declare variables.
  float radius;
  int color;
  double driftmin = 0.002;
     //driftmin = floor for drift values -- prevents mode 1 from losing hits

  // Set the color for the hit.
  if (incolor >= 0) 
    {color = incolor;}
  else 
    {
      if (hit->layer()->view() > 0) 
        {color = posStereoColor;}
      else 
        {
          if (hit->layer()->view() < 0.0) 
            {color = negStereoColor;}
          else
            {color = axialColor;} 
        }
    }
  display->setlcolor(color);


  if (mode<0) {mode = display->hitMode(windowXY);}
  switch(mode) 
    {
    case 1:
      // Decide drift distance.      
      radius = (hit->driftDist(tbunch, tIndex) > driftmin) ? 
	hit->driftDist(tbunch, tIndex) : 
	driftmin; 
      display->arc(radius, hit->x() , hit->y() , 1., 1.);   // Draw the circle.
      break;
    case 2:
      display->arc(0.75, hit->x() , hit->y() , 1., 1.);
      break;
    default:
      cerr<<"Unknown mode."<<endl;
      break;
    }

}
//**********************************************************************
void MdcPlotHits::plotinfo(const MdcHit *hit, double scale, double tbunch) 
  const {
//**********************************************************************
  char *string[5];
  int i;
  for (i = 0; i < 5; i++) {
    string[i] = new char[20];
  }

  sprintf(string[4],"%d",hit->layernumber() ); 
  sprintf(string[3],"%d",hit->wire() ); 
  sprintf(string[2],"%d",hit->mcTrack() ); 
  sprintf(string[1],"%6.2f",10.*hit->driftDist(tbunch, 0) );
  sprintf(string[0],"%6.2f",0.); // dummy 

  display->info(windowXY, hit->x(), hit->y(), string, scale, 0);

  for (i = 0; i < 5; i++) {
    delete string[i];
  }

}
#endif






