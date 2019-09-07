/*
 *    2005/09/26   Zhengyun You      Peking University
 *                 First import
 *
 */

// Main program for Bes3 EventDisplay


#include <TApplication.h>
#include <TGClient.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TEnv.h>
#include <TString.h>
#include <TMath.h>

#include <TGFrame.h>
#include <TGLayout.h>
#include <TGFileDialog.h>

#include <iostream>
#include <stdlib.h>

#include "BesVisClient/BesClient.h"
#include "BesVisLib/BesVisDisplay.h"

using namespace std;


int main(int argc, char **argv) {
    //Long Peixun's update: Output Besvis and developer information
    printf("\nWELCOME to BES3 Event Display, Version B.\n");
    printf("This version contains old detectors and MRPC, but does not contain CGEM.\n");
    printf("MRPC display is developed by Huang Shuhui.\n\n");

    gEnv->SetValue("Gui.BackgroundColor", "#d6def7");
    gEnv->SetValue("Gui.SelectBackgroundColor", "#ffeec2");
    gEnv->SetValue("Gui.SelectForegroundColor", "black");

    if (getenv("BESVISLIBROOT") == 0){
        setenv("BESVISLIBROOT",  "/ihepbatch/bes/tianhl/workarea/630/EventDisplay/BesVisLib/BesVisLib-00-00-02", 1);
    }
    gEnv->SetValue("+Gui.IconPath", ":${BESVISLIBROOT}/icons");

    TApplication theApp("App", &argc, argv);

    if (gROOT->IsBatch()) {
        fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
        return 1;
    }

    int width  = 800;
    int height = 500;
    BesClient client(gClient->GetRoot(), "BesVis", width, height, "", theApp.Argc(), theApp.Argv());
    try{
        theApp.Run();
    }
    catch (const char *s){
        std::cout << s << std::endl;
    }

    return 0;
}

