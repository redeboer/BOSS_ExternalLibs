//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Descpirtion: abstract virtual class BES subdetector 
//Author: Liuhm
//Created: May 14, 2003
//Comment:
//---------------------------------------------------------------------------//
#ifndef BesSubdetector_H 
#define BesSubdetector_H 1

class G4LogicalVolume;

#include "GDMLProcessor.hh"
#include "SAXProcessor.hh"
#include "ProcessingConfigurator.hh"

class BesSubdetector
{
 public:
   BesSubdetector(){;}
   virtual ~BesSubdetector(){;}

   //construct your detector and position it in BES world volume
   //You MUST derive your concrete class and implement it
   
   virtual void Construct(G4LogicalVolume* bes) = 0;
   G4LogicalVolume*   FindLogicalVolume( const G4String& vn);
     
 protected:
//   G4LogicalVolume* besWorld;  
   SAXProcessor           m_sxp;
   ProcessingConfigurator m_config;

};

inline 
G4LogicalVolume* BesSubdetector::FindLogicalVolume( const G4String& vn ) {
  return const_cast<G4LogicalVolume*>( GDMLProcessor::GetInstance()->GetLogicalVolume(vn) );
}

#endif

   
     







