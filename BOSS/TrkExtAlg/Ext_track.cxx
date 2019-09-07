//
// File: Ext_track.cc
//

#include        <cstring>
#include        <iostream>

#include	"TrkExtAlg/Ext_track.h"

#include        "G4ParticleTable.hh"
#include        "G4Navigator.hh"
#include        "G4VPhysicalVolume.hh"
#include        "G4VSolid.hh"
#include        "G4GeometryManager.hh"
#include        "G4RegionStore.hh"
#include        "G4ProductionCuts.hh"
#include        "G4Region.hh"
#include        "G4LogicalVolume.hh"
#include        "G4TransportationManager.hh"
#include        "G4PrimaryParticle.hh"
#include        "G4DynamicParticle.hh"
#include        "G4SDManager.hh"
#include        "G4SteppingManager.hh"
//#include        "BesSensitiveManager.hh"
//#include        "BesDetectorConstruction.hh"
#include        "TrkExtAlg/ExtBesDetectorConstruction.h"
#include        "TrkExtAlg/ExtPhysicsList.h"
#include        "G4RunManagerKernel.hh"

using namespace std;

/*
 Constructor
*/
//Ext_track::Ext_track() : myMsgFlag(true),myBFieldOn(true),myGeomOptimization(true),m_dir(0),m_tofversion(2),myUseMucKal(true),myMucWindow(6) 
Ext_track::Ext_track() : myMsgFlag(true),myBFieldOn(true),myGeomOptimization(true),m_dir(0),m_detVer(1),myUseMucKal(true),myMucWindow(6) 
{ 
//   BesSensitiveManager *besSensitiveManager = new BesSensitiveManager;
   bes3DetectorConstruction = new ExtBesDetectorConstruction(myBFieldOn,m_detVer);
   bes3WorldVolume = bes3DetectorConstruction->Construct();
   extPhysicsList = new ExtPhysicsList;
   extTrack = new G4Track;
   
   //for geant4.8.1, move this line to Initialization, extSteppingAction = new ExtSteppingAction;
   extTrackingManager = new G4TrackingManager;
   //RunManagerKernel for geant4.9.0
   extRunManagerKernel = new G4RunManagerKernel;
}

Ext_track::Ext_track(const bool msgFlag, const bool BFieldOn, const bool GeomOptimization, const int m_detVer,const bool aUseMucKal,const int aMucWindow) : myMsgFlag(msgFlag),myBFieldOn(BFieldOn),myGeomOptimization(GeomOptimization),m_dir(0),myUseMucKal(aUseMucKal),myMucWindow(aMucWindow) 
{ 
//   BesSensitiveManager *besSensitiveManager = new BesSensitiveManager;
   bes3DetectorConstruction = new ExtBesDetectorConstruction(myBFieldOn,m_detVer);
   bes3WorldVolume = bes3DetectorConstruction->Construct();
   extPhysicsList = new ExtPhysicsList;
   extTrack = new G4Track;
   
   //for geant4.8.1, move this line to Initialization, extSteppingAction = new ExtSteppingAction;
   extTrackingManager = new G4TrackingManager;
   //RunManagerKernel for geant4.9.0
   extRunManagerKernel = new G4RunManagerKernel;
}
/*
 Destructor
*/
Ext_track::~Ext_track() 
{
   if(extRunManagerKernel) delete extRunManagerKernel;
   if(extTrackingManager) delete extTrackingManager;
// if(extSteppingAction) delete extSteppingAction;
   if(extTrack) delete extTrack;
   if(bes3DetectorConstruction) delete bes3DetectorConstruction;
   if(extPhysicsList) delete extPhysicsList;
   
   // open geometry for deletion
   G4GeometryManager::GetInstance()->OpenGeometry();

   // deletion of Geant4 kernel classes
   G4SDManager* fSDM = G4SDManager::GetSDMpointerIfExist();
   if(fSDM)   
     {
	delete fSDM;
     }
}

/*
  Initialization member function.
*/

void Ext_track::Initialization( const bool aMsgFlag,const bool Bfield,const bool GeomOptimization,const bool aUseMucKal,const int aMucWindow)
{
	myMsgFlag=aMsgFlag;
	myGeomOptimization = GeomOptimization;
	myBFieldOn=Bfield,
        myUseMucKal=aUseMucKal;
	myMucWindow = aMucWindow;
  //add for geant4.8.1
  G4ParticleTable::GetParticleTable()->SetReadiness();
  extPhysicsList->ConstructParticle();

     if(myMsgFlag) cout << "Ext_track::Init will execute geant initialization." << endl;
     if(!GeometryInitialization()) cout << "Error in Ext_track::GeometryInitialization()" << endl;
     PhysicsInitialization();  

  extSteppingAction = new ExtSteppingAction;  
	extSteppingAction->SetMsgFlag(aMsgFlag);
	extSteppingAction->SetMucKalFlag(aUseMucKal);
	extSteppingAction->SetMucWindow(aMucWindow);
  //Set extSteppingAction
  extTrackingManager->SetUserAction(extSteppingAction);

   

}



////////////////////////////////////////////////////////////////
bool Ext_track::GeometryInitialization()
{
 if(myMsgFlag) cout << "Ext_track::GeometryInitialization()." << endl;

  //for geant4.9.0, DefaultRegionForTheWorld has been defined in G4RunManagerKernel.
  /*G4RegionStore* rStore = G4RegionStore::GetInstance();
  G4Region* defaultRegion=rStore->GetRegion("DefaultRegionForTheWorld",false);
  if(!defaultRegion)
  {
    defaultRegion=new G4Region("DefaultRegionForTheWorld");
    defaultRegion->SetProductionCuts(G4ProductionCutsTable::GetProductionCutsTable()->GetDefaultProductionCuts());
  }*/
  G4Region *defaultRegion=new G4Region("DefaultRegionForBesWorld");
  defaultRegion->SetProductionCuts(G4ProductionCutsTable::GetProductionCutsTable()->GetDefaultProductionCuts());
  
 // The world volume MUST NOT have a region defined by the user
 if(bes3WorldVolume->GetLogicalVolume()->GetRegion())
   {
    if(bes3WorldVolume->GetLogicalVolume()->GetRegion()!=defaultRegion)
      {
       cout << "The world volume has a user-defined region <"
                 << bes3WorldVolume->GetLogicalVolume()->GetRegion()->GetName()
                 << ">." << G4endl;
       return 0;
      }
   }
        
 // Remove old world logical volume from the default region, if exist
 if(defaultRegion->GetNumberOfRootVolumes())
   {
    if(defaultRegion->GetNumberOfRootVolumes()>size_t(1))
      {
       cout <<"DefaultRegionHasMoreThanOneVolume,Default world region should have a unique logical volume."<<endl;
       return 0;
      }
    std::vector<G4LogicalVolume*>::iterator lvItr = defaultRegion->GetRootLogicalVolumeIterator();
    defaultRegion->RemoveRootLogicalVolume(*lvItr);
    cout << (*lvItr)->GetName()
	<< " is removed from the default region." << endl;
   }
     
 // Set the default region to the world
 G4LogicalVolume* bes3WorldLog = bes3WorldVolume->GetLogicalVolume();
 bes3WorldLog->SetRegion(defaultRegion);
 defaultRegion->AddRootLogicalVolume(bes3WorldLog);
       
 // Set the world volume, notify the Navigator and reset its state
 G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->SetWorldVolume(bes3WorldVolume);

 return 1;
}



bool Ext_track::PhysicsInitialization()
{
   if(myMsgFlag) cout<<"Ext_track::PhysicsInitialization()."<<endl;
   // Following line is tentatively moved from SetPhysics method
//   G4ParticleTable::GetParticleTable()->SetReadiness();
   
//   extPhysicsList->ConstructParticle();
   extPhysicsList->Construct();
   extPhysicsList->SetCuts();
   
   CheckRegions();
   
   //update region
   G4RegionStore::GetInstance()->UpdateMaterialList(bes3WorldVolume);
   G4ProductionCutsTable::GetProductionCutsTable()->UpdateCoupleTable(bes3WorldVolume);
   
   //Build PhysicsTables
   if(myMsgFlag) cout<<"Build PhysicsTables"<<endl;
   extPhysicsList->BuildPhysicsTable();
   if(myMsgFlag) cout<<"Build PhysicsTables end."<<endl;
   G4ProductionCutsTable::GetProductionCutsTable()->PhysicsTableUpdated();
   extPhysicsList->DumpCutValuesTableIfRequested();
   
   //Geometry Optimization
   if(myGeomOptimization)
   {
	  cout<<"Geometry Optimization,please wait for a few minutes."<<endl;
       	  G4GeometryManager* geomManager = G4GeometryManager::GetInstance();
       	  geomManager->OpenGeometry();
       	  geomManager->CloseGeometry(true, false);
   }
   
   return 1;
}




void Ext_track::CheckRegions()
{
  //add for geant4.8.1
  G4RegionStore::GetInstance()->SetWorldVolume();

   for(size_t i=0;i<G4RegionStore::GetInstance()->size();i++)
     {
	G4Region* region = (*(G4RegionStore::GetInstance()))[i];
  //add for geant4.8.1
  if(region->GetWorldPhysical()!=bes3WorldVolume) continue;
	G4ProductionCuts* cuts = region->GetProductionCuts();
	if(!cuts)
	  {
	   region->SetProductionCuts(G4ProductionCutsTable::GetProductionCutsTable()->GetDefaultProductionCuts());
	  }
     }
}

   
   
/*
  Set member function.
*/

bool Ext_track::Set( const Hep3Vector &xv3, const Hep3Vector &pv3,const HepSymMatrix &err, const std::string &particleName, const double pathInMDC, const double tofInMdc)
{ 
  if( err.num_row() != 6 ){          // ?static const int Ndim_err=6, see Ext_errmx.h line58 
    std::cerr << "%ERROR at Ext_track::Set. Dimension of error matrix: "
	<< err.num_row() << " should be 6"  << std::endl;
    exit( 0 );
  }

  m_vect[0] = xv3.x();// ?set starting position,private data
  m_vect[1] = xv3.y();
  m_vect[2] = xv3.z();

//  m_errskip_flag = 0;
//  m_errskip_level = 0;


  //  Check the starting point is inside the setup.   
  if(!CheckVertexInsideWorld(xv3)) return 0; 

  float p( pv3.mag() );
  m_vect[3] = pv3.x()/p; //?set direction of momentum
  m_vect[4] = pv3.y()/p;
  m_vect[5] = pv3.z()/p;
  m_vect[6] = p;

  // check Particlename
  if(particleName!="e+"&&particleName!="e-"&&
     particleName!="mu+"&&particleName!="mu-"&&
     particleName!="pi+"&&particleName!="pi-"&&
     particleName!="kaon+"&&particleName!="kaon-"&&
     particleName!="proton"&&particleName!="anti_proton"&&
     particleName!="gamma")
     {
	std::cerr <<"Unknown or unconstructed Particle."<< std::endl;
	return 0;
     }
     
     
  double		mass;
  double		Q; 
   
  G4ParticleDefinition* particleDefinition=G4ParticleTable::GetParticleTable()->FindParticle(particleName); 
  Q = particleDefinition->GetPDGCharge(); 
  mass = particleDefinition->GetPDGMass();
   

  Hep3Vector xv( m_vect[0], m_vect[1], m_vect[2] );
  Hep3Vector pv(m_vect[3]*m_vect[6], m_vect[4]*m_vect[6], m_vect[5]*m_vect[6]);

  m_xp_err.set_err( err, xv, pv, Q, mass );	// Set error matrix.
   
  extSteppingAction->SetXpErrPointer(&m_xp_err);
  extSteppingAction->SetInitialPath(pathInMDC);
  extSteppingAction->SetInitialTof(tofInMdc);
	  
  double betaInMDC = p/sqrt(mass*mass+p*p);//velocity
  extSteppingAction->SetBetaInMDC(betaInMDC);
//  double tofInMDC = pathInMDC/(betaInMDC*299.792458);
//  if(myMsgFlag) cout<<"TOF in MDC: "<<tofInMDC<<endl;
   
//  extSteppingAction->Reset();
    extSteppingAction->MucReset();
  // extTrack Initialization.

/*  // comment 2008.04.07 due to memory loss       
    // Initialize a G4PrimaryParticle.   
     G4PrimaryParticle* primaryParticle = new G4PrimaryParticle(particleDefinition,pv3.x(),pv3.y(),pv3.z());
     primaryParticle->SetMass(mass);
     primaryParticle->SetCharge(Q);
       
    // Initialize a G4DynamicParticle.
//   G4DynamicParticle* DP = new G4DynamicParticle(particleDefinition,primaryParticle->GetMomentum());
//   DP->SetPrimaryParticle(primaryParticle);
*/
     G4DynamicParticle* DP = new G4DynamicParticle(particleDefinition,pv);
  
   delete extTrack; // add on 2008.04.07 to avoid memory loss
   extTrack = new G4Track(DP,0.0,xv3);
// extTrack->CopyTrackInfo(G4Track::G4Track(DP,0.0,xv3));
  
  // Reset navigator   
  Hep3Vector center(0,0,0);
  G4Navigator* navigator= G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  navigator->LocateGlobalPointAndSetup(center,0,false);

  return 1;
}


// check a position is in the setup 
bool Ext_track::CheckVertexInsideWorld(const Hep3Vector& pos)
{
   G4Navigator* navigator= G4TransportationManager::GetTransportationManager()-> GetNavigatorForTracking();
   
   G4VPhysicalVolume* world= navigator-> GetWorldVolume();
   G4VSolid* solid= world-> GetLogicalVolume()-> GetSolid();
   EInside qinside= solid-> Inside(pos);
   
   if( qinside != kInside) return false;
   else return true;
}






//TrackExtrapotation
void Ext_track::TrackExtrapotation()
{ 
   extTrackingManager->ProcessOneTrack(extTrack);
}


