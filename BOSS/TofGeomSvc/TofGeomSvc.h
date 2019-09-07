// $Id: TofGeomSvc.h,v 1.5 2011/02/18 04:19:03 maqm Exp $ // -*-c++-*-
// header file for a class called "TofGeomSvc"
#ifndef TOFGEOMSVC_H
#define TOFGEOMSVC_H

#include <vector>
#include <iostream>

//#include "DBBesGeoBarTOF/DB2BesGeoBarTOF.h"
//#include "DBBesGeoEndTOF/DB2BesGeoEndTOF.h"

#include "TofGeomSvc/BTofGeo.h"
#include "TofGeomSvc/ETofGeo.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "TofGeomSvc/ITofGeomSvc.h"
using namespace std;

class TofGeomSvc : public Service, virtual public ITofGeomSvc
{
 public:
   // Constructors and destructors
   TofGeomSvc( const std::string& name, ISvcLocator* svcloc );
   ~TofGeomSvc();

   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
   virtual StatusCode initialize ( );
   virtual StatusCode finalize ( );

   // Access by others
   BTofGeo* BTof(unsigned id) const;
   ETofGeo* ETof(unsigned id) const;
   const double getBPhiMax(unsigned id); 
   const double getBPhiMin(unsigned id); 
   const double getEPhiMax(unsigned id); 
   const double getEPhiMin(unsigned id); 
   void  Dump();

   // private method
 private:
  // void Fill();

   // private data members
 private:
   vector<BTofGeo*> fBTofGeo;
   vector<ETofGeo*> fETofGeo;
};

#endif /* TOF_REC_GEO_H */


