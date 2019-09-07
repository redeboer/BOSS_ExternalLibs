//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Description: RPC detector 
//Author:  Youzy      Peking University      mail: youzy@hep.pku.cn
//Created: Nov, 2003
//Comment:
//---------------------------------------------------------------------------//
//

#include "BesMucConstruction.hh"
#include "BesMucSD.hh"
#include "ReadBoostRoot.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4ios.hh"
#include "globals.hh"
#include <cstdlib>

#include "strstream"
//#include <strstream>
#include "G4Geo/MucG4Geo.h"
//#include "MucGeomSvc/MucGeomSvc.h"

BesMucConstruction::BesMucConstruction()
{
}

BesMucConstruction::~BesMucConstruction()
{
}

void BesMucConstruction::Construct(G4LogicalVolume* logicBes)
{
  //construct Muc
 
  //Dengzy  for boostroot environment
//   ReadBoostRoot* boostroot=new ReadBoostRoot();
//   G4String GeometryPath = boostroot->GetBoostRoot();
//   delete boostroot;

//   if(!GeometryPath){ 
//     G4Exception("BOOST environment not set!");
//   }
//   GeometryPath += "/dat/Muc.gdml";

//   m_config.SetURI(GeometryPath);
//   m_config.SetSetupName( "Muc" );
//   m_config.SetSetupVersion( "1.0" );
//   m_config.SetType( "G4" );
//   m_sxp.Configure(&m_config);
//   m_sxp.Initialize();
//   m_sxp.Run();
  
//   G4cout << "Construct Muc from"  << GeometryPath << G4endl;
//  G4LogicalVolume*  logicalMuc = FindLogicalVolume("logicalMuc");

  // init geo from MucGeoSvc   2006.11.23

  MucG4Geo *aMucG4Geo = new MucG4Geo();
  G4LogicalVolume*  logicalMuc = aMucG4Geo->GetTopVolume();  
  //G4LogicalVolume*  logicalMuc = MucG4Geo::Instance()->GetTopVolume();

  if(!logicalMuc) {
    G4cout << "BesMucConstruction::Construct() 1, logicalMuc not found!" << G4endl;
  }
  else {
    //G4cout << "Muc"  exist"  << G4endl;
    physicalMuc = new G4PVPlacement(0,
				    G4ThreeVector(0,0,0),
				    logicalMuc,
				    "PhysicalMuc",
				    logicBes,
				    false,
				    0);
    logicalMuc->SetVisAttributes (G4VisAttributes::Invisible);
  }
  
  //sensitive detectors
  G4SDManager* aSDman = G4SDManager::GetSDMpointer();
  BesMucSD*    aMucSD = new BesMucSD("BesMucSD", this);
  aSDman->AddNewDetector(aMucSD);
  
  G4LogicalVolume* lv = 0;

  G4int partMax  = 3;
  //G4int segMax = 8, absorberMax = 9, gapMax = 9, panelMax = 4;
  G4int stripMax = 96, bakeliteMax = 4, gasChamberMax = 2; 
  G4int segOnPart[3]      = {4, 8, 4};
  G4int absorberOnPart[3] = {9, 9, 9};
  G4int gapOnPart[3]      = {8, 9, 8};
  G4int panelOnPart[3]    = {3, 4, 3};
  G4int part, seg, absorber, panel, gap, bakelite, gasChamber, strip;

  for(part = 0; part < partMax; part++) {
    //for(seg = 0; seg < 1; seg++) {
    for(seg = 0; seg < segOnPart[part]; seg++) {
      if(seg==0||part==1&&seg==2){
      for(gap = 0; gap < gapOnPart[part]; gap++) {
	for(gasChamber = 0; gasChamber < gasChamberMax; gasChamber++) {
	  int seg_shift = 0; if(part==1&&seg==2) seg_shift = 1;
	  for(panel = 0; panel < panelOnPart[part] + seg_shift; panel++) {
	    std::ostrstream sf;
	    sf << "l" << "Muc" 
	       << "P"    << part
	       << "S"        << seg
	       << "G"        << gap
	       << "R" << gasChamber
	       << "Pn"      << panel
	       << "C"
	       << std::ends;

// 	    sf << "logical" << "Muc" 
// 	       << "Part"    << part
// 	       << "Seg"        << seg
// 	       << "Gap"        << gap
// 	       << "RpcUpDown" << gasChamber
// 	       << "Panel"      << panel
// 	       << "GasChamber"
// 	       << std::ends;
	    
	    lv = FindLogicalVolume(sf.str());
	    if(!lv) {
	      G4cout << "BesMucConstruction::Construct(), " << sf.str() << " not found!" << G4endl;
	    }
	    else {
	      lv->SetSensitiveDetector(aMucSD);
	    }
	  }
	}
      }
      }
    }
  }

  //visulizatioin attibutes
  G4VisAttributes* mucVisAtt        = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0, 0.05));
  G4VisAttributes* absorberVisAtt   = new G4VisAttributes(G4Colour(1.0, 0.3, 0.0, 1.0));
  G4VisAttributes* gapVisAtt        = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 1.0));
  G4VisAttributes* stripVisAtt      = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0, 1.0));
  G4VisAttributes* bakeliteVisAtt   = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 1.0));
  G4VisAttributes* gasChamberVisAtt = new G4VisAttributes(G4Colour(0.6, 0.8, 1.0, 1.0));

  lv = FindLogicalVolume("logicalMuc");
  if(!lv) {
    G4cout << "BesMucConstruction::Construct() 2, logicMuc not found!" << G4endl;
  }
  else {
    lv->SetVisAttributes(mucVisAtt);
    lv->SetVisAttributes (G4VisAttributes::Invisible);
  }
  
  for(part = 0; part < partMax; part++) {
    for(seg = 0; seg < segOnPart[part]; seg++) {
      for(absorber = 0; absorber < absorberOnPart[part]; absorber++) {
	std::ostrstream sf;
	sf << "logical" << "Muc"
	   << "Part" << part
	   << "Seg" << seg
	   << "Absorber" << absorber
	   << std::ends;

	lv = FindLogicalVolume(sf.str());
	if(lv) { 
	  lv->SetVisAttributes(absorberVisAtt);
	  //if(part == 1 || seg == 0 || seg == 1 || seg == 2) {
	  if(part != 1) {
	    lv->SetVisAttributes (G4VisAttributes::Invisible);
	  }
	}
      }
    }
  }

//    for(part = 0; part < partMax; part++) {
//      for(seg = 0; seg < segOnPart[part]; seg++) {
//        for(absorber = 0; absorber < absorberOnPart[part]; absorber++) {
//  	for(panel = 0; panel < panelOnPart[part]; panel++) { 
//  	  std::ostrstream sf;
//  	  sf << "logical" 
//  	     << "MucPart" << part
//  	     << "Seg" << seg
//  	     << "Absorber" << absorber
//  	     << "Panel" << panel
//  	     << std::ends;
	
//  	  lv = FindLogicalVolume(sf.str());
//  	  if(lv) {
//  	    lv->SetVisAttributes(absorberVisAtt);
//  	    if(part != 1) {
//  	      lv->SetVisAttributes (G4VisAttributes::Invisible);
//  	    }
//  	    //lv->SetVisAttributes (G4VisAttributes::Invisible);
//  	  }
//  	}
//        }
//      }
//    }

  for(part = 0; part < partMax; part++) {
    for(seg = 0; seg < segOnPart[part]; seg++) {
      for(gap = 0; gap < gapOnPart[part]; gap++) {
	std::ostrstream sf;
	sf << "logical" << "Muc"
	   << "Part" << part
	   << "Seg" << seg
	   << "Gap" << gap
	   << std::ends;

	lv = FindLogicalVolume(sf.str());
	if(lv) {
	  lv->SetVisAttributes(gapVisAtt);
	  lv->SetVisAttributes(G4VisAttributes::Invisible);
	}
      }
    }
  }

  for(part = 0; part < partMax; part++) {
    for(seg = 0; seg < 1; seg++) {
      //    for(seg = 0; seg < segOnPart[part]; seg++) {
      for(gap = 0; gap < gapOnPart[part]; gap++) {
	std::ostrstream sf;
	sf << "logical" << "Muc"
	   << "Part" << part
	   << "Seg" << seg
	   << "Gap" << gap
	   << "StripPlane"
	   << std::ends;

	lv = FindLogicalVolume(sf.str());
	if(lv) {
	  lv->SetVisAttributes(gapVisAtt);
	  lv->SetVisAttributes(G4VisAttributes::Invisible);
	}
      }
    }
  }

  for(part = 0; part < partMax; part++) {
    for(seg = 0; seg < 1; seg++) {
      //for(seg = 0; seg < segOnPart[part]; seg++) {
      for(gap = 0; gap < gapOnPart[part]; gap++) {
	for(strip = 0; strip < stripMax; strip++) {
	  std::ostrstream sf;
	  if(strip >= 0 && strip < 10) {
	    sf << "logical" << "Muc"
	       << "Part" << part
	       << "Seg" << seg
	       << "Gap" << gap
	       << "Strip0" << strip
	       << std::ends;
	  }
	  else if(strip >= 10 && strip < 100) {
	    sf << "logical" << "Muc"
	       << "Part" << part
	       << "Seg" << seg
	       << "Gap" << gap
	       << "Strip" << strip
	       << std::ends;
	  } 
	  else {
	    G4cout << "BesMucConstruction::Construct(), strip=" << strip << " error!" << G4endl;
	  }
	  
	  lv = FindLogicalVolume(sf.str());
	  if(lv) {
	    lv->SetVisAttributes(stripVisAtt);
	    lv->SetVisAttributes(G4VisAttributes::Invisible);
	  } 
	}
      }
    }
  }

  for(part = 0; part < partMax; part++) {
    for(seg = 0; seg < 1; seg++) {
      //    for(seg = 0; seg < segOnPart[part]; seg++) {
      for(gap = 0; gap < gapOnPart[part]; gap++) {
	for(panel = 0; panel < panelOnPart[part]; panel++) {
	  for(bakelite = 0; bakelite < bakeliteMax; bakelite++) {
	    std::ostrstream sf;
	    sf << "logical" << "Muc"
	       << "Part" << part
	       << "Seg" << seg
	       << "Gap" << gap
	       << "Panel" << panel
	       << "Bakelite" << bakelite
	       << std::ends;
	    
	    lv = FindLogicalVolume(sf.str());
	    if(lv) {
	      lv->SetVisAttributes(bakeliteVisAtt);
	      lv->SetVisAttributes(G4VisAttributes::Invisible);
	    }
	  }
	}
      }
    }
  }


  for(part = 0; part < partMax; part++) {
    for(seg = 0; seg < 1; seg++) {
      //    for(seg = 0; seg < segOnPart[part]; seg++) {
      for(gap = 0; gap < gapOnPart[part]; gap++) {
	for(panel = 0; panel < panelOnPart[part]; panel++) {
	  for(gasChamber = 0; gasChamber < gasChamberMax; gasChamber++) {
	    std::ostrstream sf;
	    sf << "logical" << "Muc"
	       << "Part" << part
	       << "Seg" << seg
	       << "Gap" << gap
	       << "Panel" << panel
	       << "GasChamber" << gasChamber
	       << std::ends;
	    
	    lv = FindLogicalVolume(sf.str());
	    if(lv) {
	      lv->SetVisAttributes(gasChamberVisAtt);
	      //if( (part == 1 && (seg == 0 || seg == 1 || seg == 3 || seg == 4) )
	      //  || (part == 0 && (seg == 0 || seg == 1 || seg ==2) ) ) {
	      //lv->SetVisAttributes (G4VisAttributes::Invisible);
	      //}
	      if(part != 1) {
		lv->SetVisAttributes(G4VisAttributes::Invisible);
	      }
	    }
	  }
	}
      }
    }
  }
  //m_sxp.Finalize();
  delete aMucG4Geo;
}






