/*
 *      2018/5/11   Long Peixun     IHEP
 *                  Geometry Selection for EventDisplay
*/

#include "BesVisClient/BesGeoSelector.h"
#include "GaudiKernel/Bootstrap.h"
#include "DetVerSvc/DetVerSvc.h"
#include <algorithm>
#include <iostream>
using namespace std;

BesGeoSelector::BesGeoSelector()
{}

void BesGeoSelector::addGeomFile(const char* fname, int startno)
{
    cout << "BesGeoSelector: " << fname << " start phase " << startno << endl; 
    fgeom.push_back(GeomFileInfo(fname, startno));
    sort(fgeom.begin(), fgeom.end());
}

void BesGeoSelector::clear()
{
    fgeom.clear();
}

//Given run number, select a suitable geometry file, and return file name.
const char* BesGeoSelector::getGeomFileNameFromRunNo(int run_no)
{
    DetVerSvc dvs("DetVerSvc", Gaudi::svcLocator());    //For converting runno to phase
    dvs.initialize();
    int absno = (run_no >= 0 ? run_no : -run_no);

    for (vector<GeomFileInfo>::reverse_iterator i = fgeom.rbegin(); i != fgeom.rend(); ++i)
    {
        // if (absno >= i->startno)
        // {
        //     return i->filename.c_str();
        // }

        //In BOSS 7.0.3, use DecVerSvc package to choose geometry
        int phase = dvs.fromRun(absno);
        if (phase >= i->startno)
        {
            return i->filename.c_str();
        }
    }
    cout << "BesGeoSelector Error: There is no suitable detector geometry!" << endl;
    return "";
}

//Given file name, find whether the list contains this file.
bool BesGeoSelector::hasGeomFile(const char* fname)
{
    string filename(fname);
    for (vector<GeomFileInfo>::iterator i = fgeom.begin(); i != fgeom.end(); ++i)
    {
        if (filename == i->filename) return true;
    }
    return false;
}
