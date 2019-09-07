/*
 *    2004/09/10   Zhengyun You     Peking University
 *                 
 */

// $Id: MucGeomSvc.h,v 1.4 2006/11/23 08:55:10 liangyt Exp $ // -*-c++-*-
// header file for a class called "MucGeomSvc"
#ifndef MUC_GEOM_SVC_H
#define MUC_GEOM_SVC_H

#include <vector>
#include <iostream>

//#include "BesGeoMuc/DB2BesGeoMuc.h"  
#include "MucGeomSvc/MucGeoGeneral.h"
#include "MucGeomSvc/MucGeoGap.h"
#include "MucGeomSvc/MucGeoStrip.h"
#include "MucGeomSvc/IMucGeomSvc.h"
#include "G4Geo/MucG4Geo.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"

class MucGeomSvc : public Service, virtual public IMucGeomSvc
{
public:
  // Constructors and destructors
  MucGeomSvc( const std::string& name, ISvcLocator* svcloc );
  ~MucGeomSvc();
  
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize ( );
  virtual StatusCode finalize ( );
  
  // Access by others
  virtual const MucGeoGeneral * const GetGeoGeneral();
  virtual const MucGeoGap     * const GetGap(int part, int seg, int gap);
  virtual const MucGeoStrip   * const GetStrip(int part, int seg, int gap, int strip);
  virtual const MucG4Geo * const GetMucG4Geo();

  virtual void Dump();

  // private method
private:
  void Fill();
  
  // private data members
private:
  MucGeoGeneral *m_pMucGeoGeneral;
  MucG4Geo *m_pMucG4Geo;
  int m_Geometry;  //0: G4  ; 1: Root
};

#endif /* MUC_GEOM_SVC_H */


