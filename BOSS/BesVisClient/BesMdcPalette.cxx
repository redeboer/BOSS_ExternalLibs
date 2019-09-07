//
// BesMdcPalette.cxx
//

static char* rcsname="$Name: BesVisClient-00-04-10 $";
static char* rcsdate="$Date: 2018/11/06 00:17:31 $";

#include "BesVisClient/BesMdcPalette.h"
#include <iostream>

#include <TGLabel.h>
#include <TList.h>
#include <TEnv.h>
#include <TVirtualX.h>
#include <TGButton.h>
#include <TGClient.h>
#include <TGTab.h>
#include <TGLayout.h>
#include <TGListBox.h>
#include <TString.h>
#include <TGIcon.h>
#include <TGPicture.h>
#include <TASImage.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;


//#ifndef __CINT__
ClassImp(BesMdcPalette)
//#endif


//_________________________________________________________________
// BesMdcPalette
// Information about Bes Vis Info
//
//
BesMdcPalette::BesMdcPalette(const TGWindow *p, const TGWindow *main) :
        TGTransientFrame(p, main, 200, 600) {
    //
    // BesMdcPalette normal constructor
    TString filePath = TString(getenv("BESVISLIBROOT"));
    filePath += "/icons/MdcPalette.gif";
    TASImage asImage(filePath);
    this->SetBackgroundPixmap(asImage.GetPixmap());

    // Widget layout
    SetWindowName("Mdc Wire Palette");
    Resize(GetDefaultSize());
    Resize(200,600);
    MapWindow();
}
////_________________________________________________________________
//
//BesMdcPalette::~BesMdcPalette() {
//    //
//    // BesMdcPalette default destructor
//}
