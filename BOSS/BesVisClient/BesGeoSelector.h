/*
 *      2018/5/11   Long Peixun     IHEP
 *                  Geometry Selection for EventDisplay
*/

#ifndef BES_GEO_SELECTOR_H
#define BES_GEO_SELECTOR_H

#include <string>
#include <vector>

//Structure for saving geometry file info
struct GeomFileInfo
{
    std::string filename;
    int startno;

    GeomFileInfo() {}
    GeomFileInfo(const char* fname, int sno)
        : filename(fname), startno(sno) {}

    bool operator >= (const GeomFileInfo& rval) const { return (startno >= rval.startno); }
    bool operator <= (const GeomFileInfo& rval) const { return (startno <= rval.startno); }
    bool operator > (const GeomFileInfo& rval) const { return (startno > rval.startno); }
    bool operator < (const GeomFileInfo& rval) const { return (startno < rval.startno); }
};

//BesGeoSelector class
class BesGeoSelector
{
private:
    std::vector<GeomFileInfo> fgeom;
public:
    BesGeoSelector();
    void addGeomFile(const char* fname, int startno);
    void clear();
    const char* getGeomFileNameFromRunNo(int run_no);
    bool hasGeomFile(const char* fname);
};

#endif