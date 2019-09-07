// header file for a class called "MdcGeomSvc"
#ifndef MDCGEOMSVC_H
#define MDCGEOMSVC_H

#include <CLHEP/Units/PhysicalConstants.h>  
#include <CLHEP/Geometry/Point3D.h> 
#include <vector>
#include <iostream>
#include "GaudiKernel/Algorithm.h"

#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"
#include "MdcGeomSvc/MdcGeoGeneral.h"
#include "MdcGeomSvc/MdcGeoSuper.h"
#include "MdcGeomSvc/MdcGeoMisc.h"
#include "MdcGeomSvc/MdcGeoEnd.h"  
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentListener.h"

//class MdcGeomSvc : public Service, virtual public IMdcGeomSvc,virtual public IIncidentListener
class MdcGeomSvc : public Service, virtual public IMdcGeomSvc, virtual public IIncidentListener
{
 public:
   // Constructors and destructors
   MdcGeomSvc( const std::string& name, ISvcLocator* svcloc );
   ~MdcGeomSvc();

   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
   virtual StatusCode initialize ( );
   virtual StatusCode finalize ( );
	 void handle(const Incident& inc);

   // Access by others
   const MdcGeoWire * const Wire(unsigned id);
   const MdcGeoWire * const Wire(unsigned lyrid,unsigned wirid);  
   const MdcGeoLayer * const Layer(unsigned id);
   const MdcGeoSuper * const SuperLayer(unsigned id);
   const MdcGeoGeneral * const GeneralLayer(unsigned id);
   const MdcGeoMisc * const Misc(void);    
   const MdcGeoEnd * const End(unsigned id);    
   const int getWireSize();
   const int getLayerSize();
   const int getSuperLayerSize();
   const int getGeneralLayerSize(); 
   const int getSegmentNo();    
   void Dump();
   static bool getSagFlag(void); 

 public:
   static bool m_doSag;
   static bool m_readAlignParDataBase;
   static bool m_nomcalignment;
   
 // private method
 private:
   void clean();  // clean the fWires,fGenerals,fLayers,fSupers,,fEnd,etc... 
   void ReadFilePar();        //get geometry data from file SimUtil/dat/Mdc.txt
	 void ReadTensionDataBase(std::vector<double> & wireTensionVec);
	 void ReadWirePosDataBase(std::vector<vector<double> > & wirePosVec);
	 void ReadAliParDataBase(vector<double>& Sx, vector<double>& Sy, vector<double>& Sz,
					 vector<double>& Rx, vector<double>& Ry, vector<double>& Rz);
   void Fill();  //get geometry data from Database
   const int getAlignParIndexEast(int lyr) const;
   const int getAlignParIndexWest(int lyr) const;
 

   
 // private data members
 private:
   double m_wholeShiftX; 
   double m_wholeShiftY;
   double m_wholeShiftZ;
   double m_wholeRotatX;
   double m_wholeRotatY;
   double m_wholeRotatZ; 
   int    m_mindex;
   std::string m_alignFilePath;
   std::string m_wirePosFilePath;
   std::string m_wireTensionFilePath;
   bool m_updataalign;
   
   vector <MdcGeoWire*> fWires;  //wire list (6860)
   vector <MdcGeoGeneral> fGenerals; //general layer list (90)
   vector <MdcGeoLayer*> fLayers;	 //sense wire layer list (43)
   vector <MdcGeoSuper*> fSupers;	 //super layer list (11)
   MdcGeoMisc fMisc; 			 // parms. of MDC misc
   vector <MdcGeoEnd*> fEnd;       // tube and endplate list (67)
   IDataProviderSvc* m_pCalibDataSvc;

};


#endif 


