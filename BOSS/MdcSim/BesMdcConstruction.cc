//---------------------------------------------------------------------------
//      BOOST --- BESIII Object_Oriented Simulation Tool                     
//---------------------------------------------------------------------------/
//Description: Main part for construct MDC geometry 
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 4 Dec, 2003
//Modified:
//Comment: 
//---------------------------------------------------------------------------/

#include "BesMdcConstruction.hh"
#include "ReadBoostRoot.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Hype.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4TwistedTubs.hh"
#include "G4SDManager.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ios.hh"
#include "G4Geo/MdcG4Geo.h"

#include "MyMdcGeomSvc.hh"
#include "G4Cons.hh"

BesMdcConstruction::BesMdcConstruction()
 : mdc_log(0),mdc_phys(0){
   mdc=BesMdcGeoParameter::GetGeo();
}

BesMdcConstruction::~BesMdcConstruction(){
}
 
//**************************************************************************

void BesMdcConstruction::Construct(G4LogicalVolume* logicBes){
  //------------------------------------------------ 
  // Sensitive detectors
  //------------------------------------------------ 
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String mdcSDname = "BesMdcSD";
  aTrackerSD = new BesMdcSD( mdcSDname);
  SDman->AddNewDetector( aTrackerSD );

  if(ReadBoostRoot::GetTuning())return; //No construct when tuning
  //Construct 
  G4LogicalVolume *logicalMdc = 0;
  G4LogicalVolume *lv         = 0;
  
  if(ReadBoostRoot::GetMdc()==2){
    MdcG4Geo* aMdcG4Geo = new MdcG4Geo();
    logicalMdc = aMdcG4Geo->GetTopVolume(); 
    if(!logicalMdc){
      G4cout<<"BesMdcConstruction::Construct(), logicalMdc not found"<<G4endl;
    }else{
      //construct Mdc
      mdc_phys = new G4PVPlacement(0,G4ThreeVector(0,0,0),
				   logicalMdc, "physicalMdc",logicBes, false, 0);
      logicalMdc->SetVisAttributes(G4VisAttributes::Invisible);
      
      //visual attributes and sensitive detector
      G4VisAttributes* visAttStereoLayer = new G4VisAttributes(G4Colour(0.,1.,0.));
      G4VisAttributes* visAttAxialLayer  = new G4VisAttributes(G4Colour(1.,0.,0.));
      G4int segmentNo=mdc->SegmentNo();
      G4double colorShift = 0., shift = 1.0 / (segmentNo-1);
      
      for (int i = 1; i < segmentNo; i++) { 
	std::ostringstream osnameSegment;
	osnameSegment << "logical" << "Mdc" << "Segment" << i;
	lv = (G4LogicalVolume*)GDMLProcessor::GetInstance()->GetLogicalVolume( osnameSegment.str() );
	G4VisAttributes* visAttSegment     = new G4VisAttributes(G4Colour(1.0-colorShift, 0.0+colorShift, 0.0));
	if(i<3) visAttSegment->SetForceWireframe(true);
	if(lv) {
	  lv->SetVisAttributes(visAttSegment);
	  //lv->SetVisAttributes(G4VisAttributes::Invisible);
	}
	else G4cout << "logical volume " << osnameSegment.str() << "not found " << G4endl;
	colorShift +=shift;
      }
      
      for (int i = 0; i < 8; i++) { 
	std::ostringstream osnameStereoLayer;
	osnameStereoLayer << "logical" << "Mdc" << "StereoLayer" << i;
	lv=(G4LogicalVolume*)GDMLProcessor::GetInstance()->GetLogicalVolume( osnameStereoLayer.str() );
	if(lv) {
	  lv->SetVisAttributes(visAttStereoLayer);
	  lv->SetVisAttributes(G4VisAttributes::Invisible);
	}
	else G4cout << "logical volume " << osnameStereoLayer.str() << "not found " << G4endl;
	
	std::ostringstream osnameTwistedTubs;
	osnameTwistedTubs << "logical" << "Mdc" << "StereoLayer" << i << "Cell";
	lv=(G4LogicalVolume*)GDMLProcessor::GetInstance()->GetLogicalVolume( osnameTwistedTubs.str() );
	if(lv) {
	  lv->SetSensitiveDetector( aTrackerSD );
	  lv->SetVisAttributes(G4VisAttributes::Invisible);
	}
	else G4cout << "logical volume " << osnameTwistedTubs.str() << "not found " << G4endl;
      }
      
      for (int i = 20; i < 36; i++) { 
	std::ostringstream osnameStereoLayer;
	osnameStereoLayer << "logical" << "Mdc" << "StereoLayer" << i;
	lv=(G4LogicalVolume*)GDMLProcessor::GetInstance()->GetLogicalVolume( osnameStereoLayer.str() );
	if(lv) {
	  lv->SetVisAttributes(visAttStereoLayer);
	  lv->SetVisAttributes(G4VisAttributes::Invisible);
	}
	else G4cout << "logical volume " << osnameStereoLayer.str() << "not found " << G4endl;
	
	std::ostringstream osnameTwistedTubs;
	osnameTwistedTubs << "logical" << "Mdc" << "StereoLayer" << i << "Cell";
	lv=(G4LogicalVolume*)GDMLProcessor::GetInstance()->GetLogicalVolume( osnameTwistedTubs.str() );
	if(lv) {
	  lv->SetSensitiveDetector( aTrackerSD );
	  lv->SetVisAttributes(G4VisAttributes::Invisible);
	}
	else G4cout << "logical volume " << osnameTwistedTubs.str() << "not found " << G4endl;
      }
      
      for (int i = 8; i < 20; i++) { 
	std::ostringstream osnameAxialLayer;
	osnameAxialLayer << "logical" << "Mdc" << "AxialLayer" << i;
	lv=(G4LogicalVolume*)GDMLProcessor::GetInstance()->GetLogicalVolume( osnameAxialLayer.str() );
	if(lv) {
	  lv->SetVisAttributes(visAttAxialLayer);
	  lv->SetVisAttributes(G4VisAttributes::Invisible);
	}
	else G4cout << "logical volume " << osnameAxialLayer.str() << "not found " << G4endl;
	
	std::ostringstream osnameReplica;
	osnameReplica << "logical" << "Mdc" << "AxialLayer" << i << "Cell";
	lv=(G4LogicalVolume*)GDMLProcessor::GetInstance()->GetLogicalVolume( osnameReplica.str() );
	if(lv) {
	  lv->SetSensitiveDetector( aTrackerSD );
	  lv->SetVisAttributes(G4VisAttributes::Invisible);
	}
	else G4cout << "logical volume " << osnameReplica.str() << "not found " << G4endl;
      }
      
      for (int i = 36; i < 43; i++) { 
	for (int n = 0; n < 2; n++) {
	  std::ostringstream osnameAxialLayer;
	  osnameAxialLayer << "logical" << "Mdc" << "AxialLayer" << i << "_" << n;
	  lv=(G4LogicalVolume*)GDMLProcessor::GetInstance()->GetLogicalVolume( osnameAxialLayer.str() );
	  if(lv) {
	    lv->SetVisAttributes(visAttAxialLayer);
	    lv->SetVisAttributes(G4VisAttributes::Invisible);
	  }
	  else G4cout << "logical volume " << osnameAxialLayer.str() << "not found " << G4endl;
	  
	  std::ostringstream osnameReplica;
	  osnameReplica << "logical" << "Mdc" << "AxialLayer" << i << "_" << n << "Cell";
	  lv=(G4LogicalVolume*)GDMLProcessor::GetInstance()->GetLogicalVolume( osnameReplica.str() );
	  if(lv) {
	    lv->SetSensitiveDetector( aTrackerSD );
	    lv->SetVisAttributes(G4VisAttributes::Invisible);
	  }
	  else G4cout << "logical volume " << osnameReplica.str() << "not found " << G4endl;
	}
      }
    }
    delete aMdcG4Geo;
  }else {
    //construct with previous code. 
    if(ReadBoostRoot::GetMdc()==3)G4cout<<"Nowire in Mdc Construct"<<G4endl;   

    //--------- Materials  ---------
    
    // Mdcgas He/C3H8 (60:40)
    G4Material* MdcGas = G4Material::GetMaterial("Mdcgas");
    G4Material* Al = G4Material::GetMaterial("Aluminium");
    G4Element *C  =G4Element::GetElement("Carbon");
    G4Element *H  =G4Element::GetElement("Hydrogen");
    G4Element *O  =G4Element::GetElement("Oxygen");

    G4double density = 19.3*g/cm3;
    G4double a = 183.84*g/mole;
    G4Material* W = new G4Material("Tungsten", 74., a, density);

    density=19.32*g/cm3;
    a = 196.967*g/mole;
    G4Material* Au= new G4Material("Au",79,a,density);

    density=1.57*g/cm3;
    G4int nElement=3;
    G4Material* CarbonFiber=new G4Material("CarbonFiber",density,nElement);
    CarbonFiber->AddElement(C,0.697);
    CarbonFiber->AddElement(H,0.0061);
    CarbonFiber->AddElement(O,0.2969);

    density = 1.42*g/cm3;
    G4Material* Kapton = new G4Material("Kapton", density, 3);
    Kapton->AddElement(O,2);
    Kapton->AddElement(C,5);
    Kapton->AddElement(H,4);

    //Begin construct
    G4int i,j,k,n;
    G4double outR, innerR, length;
    G4double startAngle, spanAngle;
    G4double posX, posY, posZ;
    G4VisAttributes* visAtt;
    string name; 
    //=======Mdc container
    
    outR=mdc->Segment(0).OutR()*mm;
    innerR=mdc->Segment(0).InnerR()*mm;
    length=mdc->Segment(0).Length()/2.*mm; //In Geant4, soild has central-symmetry 
    
    startAngle=0.*deg;
    spanAngle=360.*deg;
    posX  = 0.*m;
    posY  = 0.*m;
    posZ  = 0.*m;
    
    G4Tubs* mdc_tube=new G4Tubs("solidMdc",innerR,outR,length,startAngle,spanAngle);
    mdc_log=new G4LogicalVolume(mdc_tube, MdcGas,"logicalMdc",0,0,0);
    mdc_log->SetVisAttributes(G4VisAttributes::Invisible);

    //limits of step length
    //    G4double maxStep = 20.*mm;
 
  
    mdc_phys= new G4PVPlacement(0,
             G4ThreeVector(posX ,posY ,posZ),
             mdc_log,"physicalMdc",logicBes,false,0);

         
    //======Tube and Endplane
        
    G4double colorShift=0., shift=1./(mdc->SegmentNo()-1);
    G4double gap=0.5*micrometer;
    for(i=1; i<mdc->SegmentNo(); i++){
      outR=mdc->Segment(i).OutR()*mm-gap;
      innerR=mdc->Segment(i).InnerR()*mm+gap;
      length=mdc->Segment(i).Length()/2.*mm-gap; 
      startAngle=0*deg;
      spanAngle=360*deg; 
      posZ=mdc->Segment(i).Z()*mm;
      //name=mdc->Segment(i).Name();
      name="MdcSegment";
      std::ostringstream osnameSolid;
      osnameSolid << "solid"<<name<<i;      
      G4Tubs* segment_tube=new G4Tubs(osnameSolid.str(),innerR,outR,length,startAngle,spanAngle);
      std::ostringstream osnameLogical;
      osnameLogical << "logical"<<name<<i;  
      G4LogicalVolume* segment_log;
      if(i==1||i==2){
      segment_log=new G4LogicalVolume(segment_tube, CarbonFiber,osnameLogical.str(),0,0,0);
      }else{
      segment_log=new G4LogicalVolume(segment_tube, Al,osnameLogical.str(),0,0,0);
      }
            
      visAtt= new G4VisAttributes(G4Colour(1.0-colorShift,0.0+colorShift,0.0));
      if(i<3)visAtt->SetForceWireframe(true);
      segment_log->SetVisAttributes(visAtt);
      //segment_log->SetVisAttributes(G4VisAttributes::Invisible);
      colorShift +=shift;
      
      G4VPhysicalVolume* segment_phys;
      std::ostringstream osnamePhys1;
      osnamePhys1 << "physical"<<name<<i<<"p";
      segment_phys=new G4PVPlacement(0,
				     G4ThreeVector(posX ,posY ,posZ),
				     segment_log,osnamePhys1.str(),mdc_log,false,0);
      //Endplane have two parts at +-Z
      if(i>2){
      std::ostringstream osnamePhys2;
      osnamePhys2  << "physical"<<name<<i<<"m";
	segment_phys=new G4PVPlacement(0,
				       G4ThreeVector(posX ,posY ,-posZ),
				       segment_log,osnamePhys2.str(),mdc_log,false,0);
      }
      
    }
    //shielding film of inner & outer tube

      outR=mdc->Segment(2).InnerR()*mm-gap;
      innerR=mdc->Segment(2).InnerR()*mm-100*micrometer;
      length=mdc->Segment(2).Length()/2.*mm-gap-10*mm; 
      startAngle=0*deg;
      spanAngle=360*deg; 
      posZ=mdc->Segment(2).Z()*mm;
      G4Tubs* segment_tube1=new G4Tubs("SolidMdcInnerFilm0",innerR,outR,length,startAngle,spanAngle);
      G4LogicalVolume* segment_log1;
      segment_log1=new G4LogicalVolume(segment_tube1, Al,"LogicalMdcInnerFilm0",0,0,0);
      G4VisAttributes* visAtt1= new G4VisAttributes(G4Colour(0,1.0,0.0));
      segment_log1->SetVisAttributes(visAtt1);
      G4VPhysicalVolume* segment_phys1;
      segment_phys1=new G4PVPlacement(0,
				     G4ThreeVector(posX ,posY ,posZ),
				     segment_log1,"PhysicalMdcInnerFilm0",mdc_log,false,0);

      outR=mdc->Segment(2).OutR()*mm+50*micrometer;
      innerR=mdc->Segment(2).OutR()*mm+gap;
      length=mdc->Segment(2).Length()/2.*mm-gap; 
      startAngle=0*deg;
      spanAngle=360*deg; 
      posZ=mdc->Segment(2).Z()*mm;
      G4Tubs* segment_tube2=new G4Tubs("SolidMdcInnerFilm1",innerR,outR,length,startAngle,spanAngle);
      G4LogicalVolume* segment_log2;
      segment_log2=new G4LogicalVolume(segment_tube2, Al,"LogicalMdcInnerFilm1",0,0,0);
      G4VisAttributes* visAtt2= new G4VisAttributes(G4Colour(0,0.0,1.0));
      segment_log2->SetVisAttributes(visAtt2);
      G4VPhysicalVolume* segment_phys2;
      segment_phys2=new G4PVPlacement(0,
				     G4ThreeVector(posX ,posY ,posZ),
				     segment_log2,"PhysicalMdcInnerFilm1",mdc_log,false,0);

      outR=mdc->Segment(1).InnerR()*mm-gap;
      innerR=mdc->Segment(1).InnerR()*mm-100*micrometer;
      length=mdc->Segment(1).Length()/2.*mm-gap; 
      startAngle=0*deg;
      spanAngle=360*deg; 
      posZ=mdc->Segment(1).Z()*mm;
      G4Tubs* segment_tube3=new G4Tubs("SolidMdcOutFilm0",innerR,outR,length,startAngle,spanAngle);
      G4LogicalVolume* segment_log3;
      segment_log3=new G4LogicalVolume(segment_tube3, Al,"LogicalMdcOutFilm0",0,0,0);
      G4VisAttributes* visAtt3= new G4VisAttributes(G4Colour(0,1.0,0.0));
      segment_log3->SetVisAttributes(visAtt3);
      G4VPhysicalVolume* segment_phys3;
      segment_phys3=new G4PVPlacement(0,
				     G4ThreeVector(posX ,posY ,posZ),
				     segment_log3,"PhysicalMdcOutFilm0",mdc_log,false,0);

      outR=mdc->Segment(1).OutR()*mm+100*micrometer;
      innerR=mdc->Segment(1).OutR()*mm+gap;
      length=mdc->Segment(1).Length()/2.*mm-gap; 
      startAngle=0*deg;
      spanAngle=360*deg; 
      posZ=mdc->Segment(1).Z()*mm;
      G4Tubs* segment_tube4=new G4Tubs("SolidMdcOutFilm1",innerR,outR,length,startAngle,spanAngle);
      G4LogicalVolume* segment_log4;
      segment_log4=new G4LogicalVolume(segment_tube4, Al,"LogicalMdcOutFilm1",0,0,0);
      G4VisAttributes* visAtt4= new G4VisAttributes(G4Colour(0,0.0,1.0));
      segment_log4->SetVisAttributes(visAtt4);
      G4VPhysicalVolume* segment_phys4;
      segment_phys4=new G4PVPlacement(0,
				     G4ThreeVector(posX ,posY ,posZ),
				     segment_log4,"PhysicalMdcOutFilm1",mdc_log,false,0);

      outR=mdc->Segment(1).OutR()*mm+150*micrometer;
      innerR=mdc->Segment(1).OutR()*mm+100*micrometer+gap;
      length=mdc->Segment(1).Length()/2.*mm-gap; 
      startAngle=0*deg;
      spanAngle=360*deg; 
      posZ=mdc->Segment(1).Z()*mm;
      G4Tubs* segment_tube5=new G4Tubs("SolidMdcOutFilm2",innerR,outR,length,startAngle,spanAngle);
      G4LogicalVolume* segment_log5;
      segment_log5=new G4LogicalVolume(segment_tube5, Kapton,"LogicalMdcOutFilm2",0,0,0);
      G4VisAttributes* visAtt5= new G4VisAttributes(G4Colour(0,0.5,0.5));
      segment_log5->SetVisAttributes(visAtt5);
      G4VPhysicalVolume* segment_phys5;
      segment_phys5=new G4PVPlacement(0,
				     G4ThreeVector(posX ,posY ,posZ),
				     segment_log5,"PhysicalMdcOutFilm2",mdc_log,false,0);

          
      MyMdcGeomSvc* elecGeomPointer= new MyMdcGeomSvc(); 
    
    //  sustain ring which is used to fix electronic box

      G4Tubs*fixTub=new G4Tubs("solidFixTub",elecGeomPointer->FixRing(0)*mm,elecGeomPointer->FixRing(1)*mm,elecGeomPointer->FixRing(2)/2.*mm,0.*deg,360.*deg);
      G4LogicalVolume*fixTub_log=new G4LogicalVolume(fixTub,Al,"logicalFixTub");
      G4VPhysicalVolume*fixTub_phys1=new G4PVPlacement(0,G4ThreeVector(0,0,1134.5*mm),fixTub_log,"physcalFixTub1",mdc_log,false,0);
      G4VPhysicalVolume*fixTub_phys2=new G4PVPlacement(0,G4ThreeVector(0,0,-1134.5*mm),fixTub_log,"physicalFixTub2",mdc_log,false,0);


      G4Element *Cu=G4Element::GetElement("Copper");
      G4Element *Si=G4Element::GetElement("Silicon");

   //  construct east and west endcape
      G4double boxDensityEndcape=1.9745*g/cm3;
      G4int boxNcomponenEndcape=6;
      G4Material *Cuu = G4Material::GetMaterial("Copper");
    // construct material or element using another method!but you must include "G4NistManager.hh"
    // G4NistManager*man=G4NistManager::Instance();
    // G4Element*Cl=man->FindOrBuildElement("Cl");

      G4Element *Cl =new G4Element("Chlorine","Cl",17.,35.5*g/mole);
      G4Material*synEndcape=new G4Material("M_synthesis",boxDensityEndcape,boxNcomponenEndcape);
      synEndcape->AddElement(Cu,0.50);
      synEndcape->AddElement(C,0.134);
      synEndcape->AddElement(H,0.033);
      synEndcape->AddElement(Si,0.11);
      synEndcape->AddElement(Cl,0.014);
      synEndcape->AddElement(O,0.209);

      G4Box*boxEndcape=new G4Box("solidboxEndcape",elecGeomPointer->BoxEndcape(0)/2.*mm,elecGeomPointer->BoxEndcape(1)/2.*mm,elecGeomPointer->BoxEndcape(2)/2.*mm);
      G4LogicalVolume*boxEndcape_log=new G4LogicalVolume(boxEndcape,synEndcape,"logical boxEndcape"); 
      visAtt= new G4VisAttributes(G4Colour(0.0,1.0,0.0));
      boxEndcape_log->SetVisAttributes(visAtt);

    // Here is my estimated values 
      double boxCuEndDx=8.5*mm;
      double boxCuEndDy=12.*mm;
      double boxCuEndDz=29.*mm;

      G4Box* boxCuEnd=new G4Box("solidboxCuEnd",boxCuEndDx/2.,boxCuEndDy/2.,boxCuEndDz/2.);
      G4LogicalVolume*boxCuEnd_log=new G4LogicalVolume(boxCuEnd,Cuu,"logical boxCuEnd");
      visAtt= new G4VisAttributes(G4Colour(1.0,1.0,0.0));
      boxCuEnd_log->SetVisAttributes(visAtt);
    // to construct a virtual box used to contain boxCuEnd and boxEndcape
      double dxVirtualBox=18.*mm;
      double dyVirtualBox=106.*mm;
      double dzVirtualBox=53.*mm;
      G4Box*virtualBoxForEndcape=new G4Box("solid virtualBoxEndcape",dxVirtualBox/2.,dyVirtualBox/2.*mm,dzVirtualBox/2.*mm);
      G4LogicalVolume*virtualBoxForEndcape_log=new G4LogicalVolume(virtualBoxForEndcape,G4Material::GetMaterial("Air"),"logical virtualBoxEndcape");
    // virtualBoxForEndcape_log->SetVisAttributes(G4VisAttributes::Invisible);
      virtualBoxForEndcape_log->SetVisAttributes(visAtt);
      G4VPhysicalVolume*boxEndcape_phys=new G4PVPlacement(0,G4ThreeVector(elecGeomPointer->BoxEndcape(0)/2.,0,0),boxEndcape_log,"physical boxEndcape",virtualBoxForEndcape_log,false,0);
      G4VPhysicalVolume*boxCuEnd_physFront=new G4PVPlacement(0,G4ThreeVector(-boxCuEndDx*0.5-0.1*mm,elecGeomPointer->BoxEndcape(1)/2.-boxCuEndDy/2,0),boxCuEnd_log,"physical boxCuEnd",virtualBoxForEndcape_log,false,0);
      G4VPhysicalVolume*boxCuEnd_physBehind=new G4PVPlacement(0,G4ThreeVector(-boxCuEndDx*0.5-0.1*mm,-(elecGeomPointer->BoxEndcape(1)/2.-boxCuEndDy/2),0),boxCuEnd_log,"physical boxCuEnd",virtualBoxForEndcape_log,false,0);
      const double pi=3.141593;
      double posXArray;
      double posYArray;
      double posZArray;
      double startAngleArray;

      for (int i=0;i<elecGeomPointer->TotalElecLayerNo();i++)
      {
	 startAngleArray=acos(elecGeomPointer->X(i)/elecGeomPointer->R(i));

	 for(int j=0;j<elecGeomPointer->ElecNo(i);j++)
	 {
	    G4RotationMatrix*boxRotj=new G4RotationMatrix;
	    boxRotj->rotateZ(-startAngleArray-j*2*pi/elecGeomPointer->ElecNo(i));
	    posXArray=elecGeomPointer->R(i)*cos((startAngleArray+j*2*pi/elecGeomPointer->ElecNo(i)));
	    posYArray=elecGeomPointer->R(i)*sin((startAngleArray+j*2*pi/elecGeomPointer->ElecNo(i)));
	    posZArray=elecGeomPointer->Z(i);
            std::ostringstream osnamephys;
	    osnamephys<<"physicalLayer"<<i<<"Ebox"<<j;
	    G4VPhysicalVolume*box_phys=new G4PVPlacement(boxRotj,G4ThreeVector(posXArray,posYArray,posZArray),virtualBoxForEndcape_log,osnamephys.str(),mdc_log,false,j);

	 }
      }

   //  cable on endcape
      G4double mCableDensity=6.4*g/cm3;
      G4Material*materialCable=new G4Material("M_materialCable",mCableDensity,3);
      materialCable->AddElement(Cu,0.4);
      materialCable->AddElement(C,0.3);
      materialCable->AddElement(H,0.3);

      double gapR=0.1*micrometer;

      //  The west cable tub can be constructed via recursive alogrithm
      double innerRCableTub;
      double outerRCableTub;
      for(int i=0;i<11;i++)
      { 
	 innerRCableTub=elecGeomPointer->R(i)*mm-dxVirtualBox/2.+gapR;
	 outerRCableTub=elecGeomPointer->R(i+1)*mm-dxVirtualBox/2.-gapR;
	 std::ostringstream osnameCableSolid;
	 osnameCableSolid<<"MdcCableSolidWest"<<i;
	 G4Tubs*cableTub=new G4Tubs(osnameCableSolid.str(),innerRCableTub,outerRCableTub,elecGeomPointer->LengthCableTub(i+1)*1.15/2*mm,0*deg,360*deg);

	 std::ostringstream osnameCableLog;
	 osnameCableLog<<"MdcCableLogWest"<<i;
	 G4LogicalVolume*cableTub_log=new G4LogicalVolume(cableTub,materialCable,osnameCableLog.str());
	 visAtt= new G4VisAttributes(G4Colour(1,0.3,0.5));
	 cableTub_log->SetVisAttributes(visAtt);

	 std::ostringstream osnameCablePhys;
	 osnameCablePhys<<"MdcCablePhysWest"<<i;
	 G4VPhysicalVolume*cableTub_phys=new G4PVPlacement(0,G4ThreeVector(0,0,elecGeomPointer->Z(i)*mm-dzVirtualBox/2.*mm-(elecGeomPointer->LengthCableTub(i+1))*1.15/2.*mm-0.2*mm),cableTub_log,osnameCablePhys.str(),mdc_log,false,0);
      }

      // The east too
      for(int i=0;i<10;i++)
      {
	 innerRCableTub=elecGeomPointer->R(i+12)-dxVirtualBox/2.+gapR;
	 outerRCableTub=elecGeomPointer->R(i+12+1)-dxVirtualBox/2.-gapR;
	 std::ostringstream osnameCableSolid;
	 osnameCableSolid<<"MdcCableSolidEast"<<i;
	 G4Tubs*cableTub=new G4Tubs(osnameCableSolid.str(),innerRCableTub,outerRCableTub,elecGeomPointer->LengthCableTub(i+14)*1.15/2*mm,0*deg,360*deg);

	 std::ostringstream osnameCableLog;
	 osnameCableLog<<"MdcCableLogEast"<<i;
	 G4LogicalVolume*cableTub_log=new G4LogicalVolume(cableTub,materialCable,osnameCableLog.str());
	 visAtt= new G4VisAttributes(G4Colour(1,0.3,0.5));
	 cableTub_log->SetVisAttributes(visAtt);

	 std::ostringstream osnameCablePhys;
	 osnameCablePhys<<"MdcCablePhysEast"<<i;
	 G4VPhysicalVolume*cableTub_phys=new G4PVPlacement(0,G4ThreeVector(0,0,elecGeomPointer->Z(i+12)*mm+dzVirtualBox/2.*mm+(elecGeomPointer->LengthCableTub(i+14))*1.15/2.*mm+0.2*mm),cableTub_log,osnameCablePhys.str(),mdc_log,false,0);
      }


     // There's need to construct the two first cable tub .

      G4Tubs*cableTubBeginnerW=new G4Tubs("solid cableTubBeginnerW",mdc->Segment(30).InnerR()*mm+gapR,elecGeomPointer->R(0)*mm-dxVirtualBox/2.-gapR,elecGeomPointer->LengthCableTub(0)*1.15/2.*mm,0*deg,360*deg);
      G4LogicalVolume*cableTubBeginnerW_log=new G4LogicalVolume(cableTubBeginnerW,materialCable,"logical cableTubBeginnerW");
      visAtt= new G4VisAttributes(G4Colour(0,0.3,0.8));
      cableTubBeginnerW_log->SetVisAttributes(visAtt);
      G4VPhysicalVolume*cableTubBeginnerW_phys=new G4PVPlacement(0,G4ThreeVector(0,0,elecGeomPointer->Z(0)*mm-dzVirtualBox/2.*mm-(elecGeomPointer->LengthCableTub(0))*1.15/2.*mm-0.2*mm),cableTubBeginnerW_log,"physical cableTubBeginnerW",mdc_log,false,0);
     //Mutiplying 1.15 is used to estimate the practicle length according to Dong Ming-yi and Liu Rong-guang.  
      G4Tubs*cableTubBeginnerE=new G4Tubs("solid cableBeginnerE",mdc->Segment(30).InnerR()*mm+gapR,elecGeomPointer->R(12)*mm-dxVirtualBox/2.-gapR,elecGeomPointer->LengthCableTub(13)*1.15/2.*mm,0*deg,360*deg);
      G4LogicalVolume*cableTubBeginnerE_log=new G4LogicalVolume(cableTubBeginnerE,materialCable,"logical cabieTubBeginnerE");
      visAtt= new G4VisAttributes(G4Colour(1,0.3,0.5));
      cableTubBeginnerE_log->SetVisAttributes(visAtt);

      G4VPhysicalVolume*cableTubBeginnerE_phys=new G4PVPlacement(0,G4ThreeVector(0,0,elecGeomPointer->Z(13)*mm+dzVirtualBox/2.*mm+(elecGeomPointer->LengthCableTub(13))*1.15/2.*mm+0.2*mm),cableTubBeginnerE_log,"physical cableTubBeginnerE",mdc_log,false,0);


     // There's need to construct the two cable tub(west and east) to tail seperately too.
      
      G4Tubs*cableTubToTailW=new G4Tubs("solid cabletubToTailW",elecGeomPointer->R(11)*mm+gapR,mdc->Segment(6).InnerR()*mm-gapR,elecGeomPointer->LengthCableTub(12)*1.15/2.*mm,0*deg,360*deg);
      G4LogicalVolume*cableTubToTailW_log=new G4LogicalVolume(cableTubToTailW,materialCable,"logical cableTubToTailW");
      visAtt= new G4VisAttributes(G4Colour(0,0.3,0.8));
      cableTubToTailW_log->SetVisAttributes(visAtt);

      G4VPhysicalVolume*cableTubToTailW_phys=new G4PVPlacement(0,G4ThreeVector(0,0,elecGeomPointer->Z(11)*mm-dzVirtualBox/2.*mm-(elecGeomPointer->LengthCableTub(12))*1.15/2.*mm-0.2*mm),cableTubToTailW_log,"physical cableTubToTailW",mdc_log,false,0);

      G4Tubs*cableTubToTailE=new G4Tubs("solid cabletubToTailE",elecGeomPointer->R(22)*mm-dxVirtualBox/2.+gapR,mdc->Segment(6).InnerR()*mm-gapR,elecGeomPointer->LengthCableTub(24)*1.15/2.*mm,0*deg,360*deg);
    
      G4LogicalVolume*cableTubToTailE_log=new G4LogicalVolume(cableTubToTailE,materialCable,"logical cabieTubToTailE");
      visAtt= new G4VisAttributes(G4Colour(1.0,0.3,0.5));
      cableTubToTailE_log->SetVisAttributes(visAtt);

      G4VPhysicalVolume*cableTubToTailE_phys=new G4PVPlacement(0,G4ThreeVector(0,0,elecGeomPointer->Z(22)*mm+dzVirtualBox/2.*mm+(elecGeomPointer->LengthCableTub(24))*1.15/2.*mm+0.2*mm),cableTubToTailE_log,"physical cableTubToTailE",mdc_log,false,0);
   
     // Construct shield plate beyond endcape

      G4Box* box0=new G4Box("box0",10.,15.,3.);
      G4LogicalVolume* box0_log=new G4LogicalVolume(box0,Al,"logicalBox0",0,0,0);
      visAtt= new G4VisAttributes(G4Colour(1.0,0.8,0.0));
      box0_log->SetVisAttributes(visAtt);

      G4Box* box1=new G4Box("box1",3.*cos(11.28*deg),15.,81./sin(11.28*deg)/2.);
      G4LogicalVolume* box1_log=new G4LogicalVolume(box1,Al,"logicalBox1",0,0,0);
      box1_log->SetVisAttributes(visAtt);

      G4Box* box2=new G4Box("box2",63.5,15.,3.);
      G4LogicalVolume* box2_log=new G4LogicalVolume(box2,Al,"logicalBox2",0,0,0);
      box2_log->SetVisAttributes(visAtt);


      for(i=0;i<8;i++){
	 G4RotationMatrix* boxRot0=new G4RotationMatrix();
	 boxRot0->rotateZ(-45*i*deg);
	 posX=784.*cos(45*i*deg);
	 posY=784.*sin(45*i*deg);
	 posZ=1309.*mm;
	 std::ostringstream osnameBox0PhysEast;
	 osnameBox0PhysEast<<"physical"<< "box0p"<<i;
	 G4VPhysicalVolume* box0p_phys=new G4PVPlacement(boxRot0,G4ThreeVector(posX ,posY ,posZ),
	       box0_log,osnameBox0PhysEast.str(),mdc_log,false,i);

	 std::ostringstream osnameBox0PhysWest;
	 osnameBox0PhysWest<<"physical"<< "box0m"<<i;
	 G4VPhysicalVolume* box0m_phys=new G4PVPlacement(boxRot0,G4ThreeVector(posX ,posY ,-posZ),
	       box0_log,osnameBox0PhysWest.str(),mdc_log,false,i);


	 G4RotationMatrix* boxRot1p=new G4RotationMatrix();
	 boxRot1p->rotateZ(-45*i*deg);
	 boxRot1p->rotateY(-78.72*deg);
	 posX=570.5*cos(45*i*deg);
	 posY=570.5*sin(45*i*deg);
	 posZ=1268.5*mm;
	 std::ostringstream osnameBox1PhysEast;
	 osnameBox1PhysEast<<"physical"<< "box1p"<<i;
	 G4VPhysicalVolume* box1p_phys=new G4PVPlacement(boxRot1p,G4ThreeVector(posX ,posY ,posZ),
	       box1_log,osnameBox1PhysEast.str(),mdc_log,false,i);

	 G4RotationMatrix* boxRot1m=new G4RotationMatrix();
	 boxRot1m->rotateZ(-45*i*deg);
	 boxRot1m->rotateY(78.72*deg);
	 std::ostringstream osnameBox1PhysWest;
	 osnameBox1PhysWest<<"physical"<< "box1m"<<i;
	 G4VPhysicalVolume* box1m_phys=new G4PVPlacement(boxRot1m,G4ThreeVector(posX ,posY ,-posZ),
	       box1_log,osnameBox1PhysWest.str(),mdc_log,false,i);

	 G4RotationMatrix* boxRot2=new G4RotationMatrix();
	 boxRot2->rotateZ(-45*i*deg);
	 posX=303.5*cos(45*i*deg);
	 posY=303.5*sin(45*i*deg);
	 posZ=1228.*mm;
	 std::ostringstream osnameBox2PhysEast;
	 osnameBox2PhysEast<<"physical"<< "box2p"<<i;
	 G4VPhysicalVolume* box2p_phys=new G4PVPlacement(boxRot2,G4ThreeVector(posX ,posY ,posZ),
	       box2_log,osnameBox2PhysEast.str(),mdc_log,false,i);

	 std::ostringstream osnameBox2PhysWest;
	 osnameBox2PhysWest<<"physical"<< "box2m"<<i;
	 G4VPhysicalVolume* box2m_phys=new G4PVPlacement(boxRot2,G4ThreeVector(posX ,posY ,-posZ),
	       box2_log,osnameBox2PhysWest.str(),mdc_log,false,i);

      }

      G4double rMax2=774.*mm;
      G4double rMin2=rMax2 - 1.*mm/sin(11.28*deg);
      G4double rMin1=rMin2 - 80.*mm/tan(11.28*deg);
      G4double rMax1=rMin1 + 1.*mm/sin(11.28*deg);
      G4Cons* shieldPlate0p=new G4Cons("shieldPlate0p",rMin1,rMax1,rMin2,rMax2,40.*mm,0.,360.*deg);
      G4LogicalVolume* shieldPlate0p_log=new G4LogicalVolume(shieldPlate0p,Al,"logicalShieldPlate0p",0,0,0);
      visAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0));
      shieldPlate0p_log->SetVisAttributes(visAtt);
      posX=0.;
      posY=0.;
      posZ=1272.*mm;
      G4VPhysicalVolume* shieldPlate0p_phys=new G4PVPlacement(0,G4ThreeVector(posX ,posY ,posZ),
	    shieldPlate0p_log,"physicalShieldPalte0p",mdc_log,false,0);

      G4Cons* shieldPlate0m=new G4Cons("shieldPlate0m",rMin2,rMax2,rMin1,rMax1,40.*mm,0.,360.*deg);
      G4LogicalVolume* shieldPlate0m_log=new G4LogicalVolume(shieldPlate0m,Al,"logicalShieldPlate0m",0,0,0);
      visAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0));
      shieldPlate0m_log->SetVisAttributes(visAtt);
      posX=0.;
      posY=0.;
      posZ=1272.*mm;
      G4VPhysicalVolume* shieldPlate0m_phys=new G4PVPlacement(0,G4ThreeVector(posX ,posY ,-posZ),
	    shieldPlate0m_log,"physicalShieldPalte0m",mdc_log,false,0);


      G4Tubs* shieldPlate1=new G4Tubs("shieldPlate1",240.*mm,367.*mm,0.5*mm,0.,360.*deg);
      G4LogicalVolume* shieldPlate1_log=new G4LogicalVolume(shieldPlate1,Al,"logicalShieldPlate1",0,0,0);
      visAtt= new G4VisAttributes(G4Colour(1.0,0.0,0.0));
      shieldPlate1_log->SetVisAttributes(visAtt);
      posX=0.;
      posY=0.;
      posZ=1231.5*mm;
      G4VPhysicalVolume* shieldPlate1p_phys=new G4PVPlacement(0,G4ThreeVector(posX ,posY ,posZ),
	    shieldPlate1_log,"physicalShieldPalte1p",mdc_log,false,0);
      G4VPhysicalVolume* shieldPlate1m_phys=new G4PVPlacement(0,G4ThreeVector(posX ,posY ,-posZ),
	    shieldPlate1_log,"physicalShieldPalte1m",mdc_log,false,0);

                 
    //========Layer  
    G4int signalLayer,firstWire;
    G4int replicaNo;
    G4double offset;
    
    G4double signalWireR=mdc->SignalWireR()*micrometer;
    G4double fieldWireR=mdc->FieldWireR()*micrometer;
    G4double thickOfAu=0.7*micrometer; // All wires are goldplated.
    
    G4double innerLength,innerTwistAngle,innerTan,midInnerR,innerStereo;
    G4double outLength,outTwistAngle,outTwistAngleFixed,outTan,midOutR,outStereo,outRFixed,outTanFixed; 
    
    //----Axial layers 37-43 
    for(i=42; i>35; i--){
      G4int signalLayer=mdc->Signal2Global(i);
      firstWire=mdc->Layer(signalLayer).FirstWire();
      for(n=1;n>-1;n--){
	innerR=mdc->Layer(signalLayer-1+n).R()*mm-fieldWireR;
	outR=mdc->Layer(signalLayer+n).R()*mm-fieldWireR;
	if(i==42&&n==1)outR=mdc->Layer(signalLayer+n).R()*mm+fieldWireR;
	length=(mdc->Layer(signalLayer-1+n).Length())/2.*mm;
	startAngle=0.*deg;
	spanAngle=360.*deg;
	posX  = 0.*m;
	posY  = 0.*m;
	posZ  = 0.*m;
	
	//Layer	
	std::ostringstream osnameLayerSolid;
	osnameLayerSolid <<"solid"<< "MdcAxialLayer"<<i<<"_"<<n;  
	G4Tubs* axialLayer_tube=new G4Tubs(osnameLayerSolid.str(),innerR,outR,length,startAngle,spanAngle);
	
	std::ostringstream osnameLayerLogical;
	osnameLayerLogical <<"logical"<< "MdcAxialLayer"<<i<<"_"<<n;
	G4LogicalVolume* axialLayer_log=new G4LogicalVolume(axialLayer_tube, MdcGas,osnameLayerLogical.str(),0,0,0);

	axialLayer_log->SetVisAttributes(G4VisAttributes::Invisible);

	replicaNo=mdc->Layer(signalLayer).WireNo()/2;
	spanAngle=360./replicaNo*deg;
	
	offset=mdc->Layer(signalLayer).Phi()*rad-firstWire*spanAngle/2.;      
	G4RotationMatrix* layerRot=new G4RotationMatrix();
	layerRot->rotateZ(-offset);

	std::ostringstream osnameLayerPhys;
	osnameLayerPhys <<"physical"<< "MdcAxialLayer"<<i<<"_"<<n;
	G4VPhysicalVolume* axialLayer_phys;
	axialLayer_phys=new G4PVPlacement(layerRot,
					     0,
					  axialLayer_log,osnameLayerPhys.str(),mdc_log,false,i);
	
	//Cell
	std::ostringstream osnameCellSolid;
	osnameCellSolid<<"solid"<< "MdcAxialLayer"<<i<<"_"<<n<<"Cell"; 
	G4Tubs* replica_tube=new G4Tubs(osnameCellSolid.str(),innerR,outR,length,startAngle,spanAngle);

	std::ostringstream osnameCellLogical;
	osnameCellLogical<<"logical"<< "MdcAxialLayer"<<i<<"_"<<n<<"Cell"; 
	G4LogicalVolume* replica_log=new G4LogicalVolume(replica_tube,MdcGas,osnameCellLogical.str(),0,0,0);
	
	replica_log->SetSensitiveDetector( aTrackerSD );
	visAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0));
	replica_log->SetVisAttributes(visAtt);
	replica_log->SetVisAttributes(G4VisAttributes::Invisible);
	//       replica_log->SetUserLimits(new G4UserLimits(maxStep));
	//Wire
	if(ReadBoostRoot::GetMdc()==1){
	  std::ostringstream osnameFieldWireSolid;
	  osnameFieldWireSolid<<"solid"<< "AxialLayer"<<i<<"_"<<n<<"FieldWire";
	  G4Tubs* fieldWire_tube=new G4Tubs(osnameFieldWireSolid.str(),0.,fieldWireR,length,0.,360*deg);
	  std::ostringstream osnameFieldWireLogical;
	  osnameFieldWireLogical<<"logical"<< "AxialLayer"<<i<<"_"<<n<<"FieldWire";
	  G4LogicalVolume* fieldWire_log=new G4LogicalVolume(fieldWire_tube,Au,osnameFieldWireLogical.str(),0,0,0);
	  
	  std::ostringstream osnameFieldWireSubSolid;
	  osnameFieldWireSubSolid<<"solid"<< "AxialLayer"<<i<<"_"<<n<<"FieldWireSub";
	  G4Tubs* fieldWireSub_tube=new G4Tubs(osnameFieldWireSubSolid.str(),0.,fieldWireR-thickOfAu,length,0.,360*deg);
	  std::ostringstream osnameFieldWireSubLogical;
	  osnameFieldWireSubLogical<<"logical"<< "AxialLayer"<<i<<"_"<<n<<"FieldWireSub";
	  G4LogicalVolume* fieldWireSub_log=new G4LogicalVolume(fieldWireSub_tube,Al,osnameFieldWireSubLogical.str(),0,0,0);
	  std::ostringstream osnameFieldWireSubPhys;
	  osnameFieldWireSubPhys<<"physical"<< "AxialLayer"<<i<<"_"<<n<<"FieldWireSub";
	  G4VPhysicalVolume* fieldWireSub_phys;
	  fieldWireSub_phys=new G4PVPlacement(0,0,fieldWireSub_log,
					      osnameFieldWireSubPhys.str(),fieldWire_log,false,0);
	  
	  std::ostringstream osnameFieldWireHalfSolid;
	  osnameFieldWireHalfSolid<<"solid"<< "AxialLayer"<<i<<"_"<<n<<"FieldWireHalf";
	  G4Tubs* fieldWireHalf_tube=new G4Tubs(osnameFieldWireHalfSolid.str(),0.,fieldWireR,length,0.,180*deg);
	  std::ostringstream osnameFieldWireHalfLogical;
	  osnameFieldWireHalfLogical<<"logical"<< "AxialLayer"<<i<<"_"<<n<<"FieldWireHalf";
	  G4LogicalVolume* fieldWireHalf_log=new G4LogicalVolume(fieldWireHalf_tube,Au,osnameFieldWireHalfLogical.str(),0,0,0);
	  
	  std::ostringstream osnameFieldWireHalfSubSolid;
	  osnameFieldWireHalfSubSolid<<"solid"<< "AxialLayer"<<i<<"_"<<n<<"FieldWireHalfSub";
	  G4Tubs* fieldWireHalfSub_tube=new G4Tubs(osnameFieldWireHalfSubSolid.str(),0.,fieldWireR-thickOfAu,length,0.,360*deg);
	  std::ostringstream osnameFieldWireHalfSubLogical;
	  osnameFieldWireHalfSubLogical<<"logical"<< "AxialLayer"<<i<<"_"<<n<<"FieldWireHalfSub";
	  G4LogicalVolume* fieldWireHalfSub_log=new G4LogicalVolume(fieldWireHalfSub_tube,Al,osnameFieldWireHalfSubLogical.str(),0,0,0);
	  std::ostringstream osnameFieldWireHalfSubPhys;
	  osnameFieldWireHalfSubPhys<<"physical"<< "AxialLayer"<<i<<"_"<<n<<"FieldWireHalfSub";
	  G4VPhysicalVolume* fieldWireHalfSub_phys;
	  fieldWireHalfSub_phys=new G4PVPlacement(0,0,fieldWireHalfSub_log,
						  osnameFieldWireHalfSubPhys.str(),fieldWireHalf_log,false,0);
	  
	  //phi  <------------------->-phi 
	  //      |F8      F5     F2|  There are 1 signal wire S4,2 half field wire F1,7 
	  // sub1 |                 |  in each cell of sub1 except Layer 42,  
	  //      |F7      S4     F1|  1 more full field wire F5, 2 more half field wire F2,8
	  //      |-----------------|
	  // sub0 |                 |  Each cell of sub0 have 1 full field wire F3, 2 half 
	  //      |F6      F3     F0|  field wire F0,6
	  //      -------------------
	  if(n==0){
	    std::ostringstream osnameFieldWire0Phys;
	    osnameFieldWire0Phys<<"physicalAxialLayer"<<i<<"_"<<n<<"FieldWire0";
	    posX=mdc->Layer(signalLayer-1).R()*mm;
	    G4VPhysicalVolume* fieldWire0_phys;
	    fieldWire0_phys=new G4PVPlacement(0,G4ThreeVector(posX,0,0),fieldWireHalf_log,
					      osnameFieldWire0Phys.str(),replica_log,false,0);
	    
	    std::ostringstream osnameFieldWire3Phys;
	    osnameFieldWire3Phys<<"physicalAxialLayer"<<i<<"_"<<n<<"FieldWire3";
	    posX=mdc->Layer(signalLayer-1).R()*mm*cos(spanAngle/2.);
	    posY=mdc->Layer(signalLayer-1).R()*mm*sin(spanAngle/2.);
	    G4VPhysicalVolume* fieldWire3_phys;
	    fieldWire3_phys=new G4PVPlacement(0,G4ThreeVector(posX,posY,0),fieldWire_log,
					      osnameFieldWire3Phys.str(),replica_log,false,3);	    
	    std::ostringstream osnameFieldWire6Phys;
	    osnameFieldWire6Phys<<"physicalAxialLayer"<<i<<"_"<<n<<"FieldWire6";
	    posX=mdc->Layer(signalLayer-1).R()*mm*cos(spanAngle);
	    posY=mdc->Layer(signalLayer-1).R()*mm*sin(spanAngle);
	    G4RotationMatrix* wireRot6=new G4RotationMatrix();
	    wireRot6->rotateZ(180*deg-spanAngle);
	    G4VPhysicalVolume* fieldWire6_phys;
	    fieldWire6_phys=new G4PVPlacement(wireRot6,G4ThreeVector(posX,posY,0),fieldWireHalf_log,
					      osnameFieldWire6Phys.str(),replica_log,false,6);
	  }
	  
	  if(n==1){
	    std::ostringstream osnameFieldWire1Phys;
	    osnameFieldWire1Phys<<"physicalAxialLayer"<<i<<"_"<<n<<"FieldWire1";
	    posX=mdc->Layer(signalLayer).R()*mm;
	    G4VPhysicalVolume* fieldWire1_phys;
	    fieldWire1_phys=new G4PVPlacement(0,G4ThreeVector(posX,0,0),fieldWireHalf_log,
					      osnameFieldWire1Phys.str(),replica_log,false,1);	    
	    std::ostringstream osnameSignalWireSolid;
	    osnameSignalWireSolid<<"solid"<< "AxialLayer"<<i<<"_"<<n<<"SignalWire";
	    G4Tubs* signalWire_tube=new G4Tubs(osnameSignalWireSolid.str(),0.,signalWireR,length,0.,360*deg);
	    std::ostringstream osnameSignalWireLogical;
	    osnameSignalWireLogical<<"logical"<< "AxialLayer"<<i<<"_"<<n<<"SignalWire";
	    G4LogicalVolume* signalWire_log=new G4LogicalVolume(signalWire_tube,Au,osnameSignalWireLogical.str(),0,0,0);
	    
	    std::ostringstream osnameSignalWireSubSolid;
	    osnameSignalWireSubSolid<<"solid"<< "AxialLayer"<<i<<"_"<<n<<"SignalWireSub";
	    G4Tubs* signalWireSub_tube=new G4Tubs(osnameSignalWireSubSolid.str(),0.,signalWireR-thickOfAu,length,0.,360*deg);
	    std::ostringstream osnameSignalWireSubLogical;
	    osnameSignalWireSubLogical<<"logical"<< "AxialLayer"<<i<<"_"<<n<<"SignalWireSub";
	    G4LogicalVolume* signalWireSub_log=new G4LogicalVolume(signalWireSub_tube,W,osnameSignalWireSubLogical.str(),0,0,0);
	    std::ostringstream osnameSignalWireSubPhys;
	    osnameSignalWireSubPhys<<"physical"<< "AxialLayer"<<i<<"_"<<n<<"SignalWireSub";
	    G4VPhysicalVolume* signalWireSub_phys;
	    signalWireSub_phys=new G4PVPlacement(0,0,signalWireSub_log,
						 osnameSignalWireSubPhys.str(),signalWire_log,false,0);

	    std::ostringstream osnameSignalWirePhys;
	    osnameSignalWirePhys<<"physicalAxialLayer"<<i<<"_"<<n<<"SignalWire4";
	    posX=mdc->Layer(signalLayer).R()*mm*cos(spanAngle/2.);
	    posY=mdc->Layer(signalLayer).R()*mm*sin(spanAngle/2.);
	    G4VPhysicalVolume* signalWire_phys;
	    signalWire_phys=new G4PVPlacement(0,G4ThreeVector(posX,posY,0),signalWire_log,
					      osnameSignalWirePhys.str(),replica_log,false,4);
	    
	    std::ostringstream osnameFieldWire7Phys;
	    osnameFieldWire7Phys<<"physicalAxialLayer"<<i<<"_"<<n<<"FieldWire7";
	    posX=mdc->Layer(signalLayer).R()*mm*cos(spanAngle);
	    posY=mdc->Layer(signalLayer).R()*mm*sin(spanAngle);
	    G4RotationMatrix* wireRot7=new G4RotationMatrix();
	    wireRot7->rotateZ(180*deg-spanAngle);
	    G4VPhysicalVolume* fieldWire7_phys;
	    fieldWire7_phys=new G4PVPlacement(wireRot7,G4ThreeVector(posX,posY,0),fieldWireHalf_log,
					      osnameFieldWire7Phys.str(),replica_log,false,7);
	    if(i==42){
	      std::ostringstream osnameFieldWire2Phys;
	      osnameFieldWire2Phys<<"physicalAxialLayer"<<i<<"_"<<n<<"FieldWire2";
	      posX=mdc->Layer(signalLayer+1).R()*mm;
	      G4VPhysicalVolume* fieldWire2_phys;
	      fieldWire2_phys=new G4PVPlacement(0,G4ThreeVector(posX,0,0),fieldWireHalf_log,
						osnameFieldWire2Phys.str(),replica_log,false,2);
	      
	      std::ostringstream osnameFieldWire5Phys;
	      osnameFieldWire5Phys<<"physicalAxialLayer"<<i<<"_"<<n<<"FieldWire5";
	      posX=mdc->Layer(signalLayer+1).R()*mm*cos(spanAngle/2.);
	      posY=mdc->Layer(signalLayer+1).R()*mm*sin(spanAngle/2.);
	      G4VPhysicalVolume* fieldWire5_phys;
	      fieldWire5_phys=new G4PVPlacement(0,G4ThreeVector(posX,posY,0),fieldWire_log,
						osnameFieldWire5Phys.str(),replica_log,false,5);
	      
	      std::ostringstream osnameFieldWire8Phys;
	      osnameFieldWire8Phys<<"physicalAxialLayer"<<i<<"_"<<n<<"FieldWire8";
	      posX=mdc->Layer(signalLayer+1).R()*mm*cos(spanAngle);
	      posY=mdc->Layer(signalLayer+1).R()*mm*sin(spanAngle);
	      G4RotationMatrix* wireRot8=new G4RotationMatrix();
	      wireRot8->rotateZ(180*deg-spanAngle);
	      G4VPhysicalVolume* fieldWire8_phys;
	      fieldWire8_phys=new G4PVPlacement(wireRot8,G4ThreeVector(posX,posY,0),fieldWireHalf_log,
						osnameFieldWire8Phys.str(),replica_log,false,8);
	    }
	  }
	}
	//Put cells into layer
	for(j=replicaNo-1;j>-1;j--){
	  G4RotationMatrix* cellRot=new G4RotationMatrix();
	  cellRot->rotateZ(-spanAngle*j);
	  std::ostringstream osnameCellPhys;
	  osnameCellPhys<<"physical"<< "MdcAxialLayer"<<i<<"_"<<n<<"Cell"<<j; 
	  G4VPhysicalVolume* replica_phys;
	  replica_phys=new G4PVPlacement(cellRot,0,replica_log,
					 osnameCellPhys.str(),axialLayer_log,false,j);
	}	      
	
      }
      
    }
    //----Stereo layers 21-36
            
    for(i=35; i>19; i--){
      signalLayer=mdc->Signal2Global(i);

      innerR=mdc->Layer(signalLayer-1).R()*mm-fieldWireR;
      outR=mdc->Layer(signalLayer+1).R()*mm-fieldWireR;
      if(i==35)outR=mdc->Layer(signalLayer+1).R()*mm+fieldWireR;

      innerLength=(mdc->Layer(signalLayer-1).Length())/2.*mm;
      outLength=(mdc->Layer(signalLayer+1).Length())/2.*mm;

      innerTwistAngle=mdc->Layer(signalLayer-1).RotateAngle()*rad;
      outTwistAngle=mdc->Layer(signalLayer+1).RotateAngle()*rad;

      innerTan=innerR/innerLength*sin(innerTwistAngle);
		
      midInnerR=innerR*cos(innerTwistAngle);
      innerStereo=atan(innerTan);

      outTan=outR/outLength*sin(outTwistAngle);
      outTwistAngleFixed=atan(innerLength/outLength*tan(outTwistAngle));

      if(abs(outTwistAngleFixed) >= abs(innerTwistAngle)){
	midOutR=outR*cos(outTwistAngle);
	outRFixed=midOutR/cos(innerTwistAngle);
	outR=outRFixed;

	outTanFixed=midOutR/innerLength*tan(innerTwistAngle);
	outStereo=atan(outTanFixed);
      }else{	
	outRFixed=sqrt(outR*outR+(innerLength*innerLength-outLength*outLength)*outTan*outTan);
	outR=outRFixed;
	midOutR=outR*cos(innerTwistAngle);

	outTanFixed=outRFixed/innerLength*sin(innerTwistAngle);
	outStereo=atan(outTanFixed);
      }
      //Layer
      std::ostringstream osnameLayerSolid;
      osnameLayerSolid <<"solid"<< "MdcStereoLayer"<<i;  

      G4Hype* stereoLayer_hype=new G4Hype(osnameLayerSolid.str(),midInnerR, midOutR,innerStereo,
					  outStereo,innerLength);

      std::ostringstream osnameLayerLogical;
      osnameLayerLogical <<"logical"<< "MdcStereoLayer"<<i;
      G4LogicalVolume* stereoLayer_log=new G4LogicalVolume(stereoLayer_hype,MdcGas,
							   osnameLayerLogical.str(),0,0,0);
      stereoLayer_log->SetVisAttributes(G4VisAttributes::Invisible);

      replicaNo=mdc->Layer(signalLayer).WireNo()/2;
      spanAngle=360./replicaNo*deg;
      firstWire=mdc->Layer(signalLayer).FirstWire();

      G4RotationMatrix* layerRot=new G4RotationMatrix();
      layerRot->rotateZ(-(spanAngle*(1-firstWire)/2.+innerTwistAngle));

      std::ostringstream osnameLayerPhys;
      osnameLayerPhys<<"physical"<< "MdcStereoLayer"<<i;
      G4VPhysicalVolume* stereoLayer_phys;
      stereoLayer_phys=new G4PVPlacement(layerRot,0,stereoLayer_log,osnameLayerPhys.str(),
					 mdc_log,false,i);
      //Cell
      std::ostringstream osnameCellSolid;
      osnameCellSolid<<"solid"<< "MdcStereoLayer"<<i<<"Cell";
      
      G4TwistedTubs* twistTub=new G4TwistedTubs(osnameCellSolid.str(), -innerTwistAngle*2, innerR,outR-1.0*micrometer,innerLength,spanAngle);
      
      std::ostringstream osnameCellLogical;
      osnameCellLogical<<"logical"<< "MdcStereoLayer"<<i<<"Cell";
      G4LogicalVolume* twistTub_log=new G4LogicalVolume(twistTub, MdcGas,osnameCellLogical.str(),0,0,0);

      twistTub_log->SetVisAttributes(G4VisAttributes::Invisible);
      twistTub_log->SetSensitiveDetector( aTrackerSD );
      //       twistTub_log->SetUserLimits(new G4UserLimits(maxStep));       

      //Wire
      if(ReadBoostRoot::GetMdc()==1){
	std::ostringstream osnameFieldWireSolid;
	osnameFieldWireSolid<<"solid"<< "StereoLayer"<<i<<"FieldWire";
	G4Tubs* fieldWire_tube=new G4Tubs(osnameFieldWireSolid.str(),0.,fieldWireR,innerLength,0.,360*deg);
	std::ostringstream osnameFieldWireLogical;
	osnameFieldWireLogical<<"logical"<< "StereoLayer"<<i<<"FieldWire";
	G4LogicalVolume* fieldWire_log=new G4LogicalVolume(fieldWire_tube,Au,osnameFieldWireLogical.str(),0,0,0);
	
	std::ostringstream osnameFieldWireSubSolid;
	osnameFieldWireSubSolid<<"solid"<< "StereoLayer"<<i<<"FieldWireSub";
	G4Tubs* fieldWireSub_tube=new G4Tubs(osnameFieldWireSubSolid.str(),0.,fieldWireR-thickOfAu,innerLength,0.,360*deg);
	std::ostringstream osnameFieldWireSubLogical;
	osnameFieldWireSubLogical<<"logical"<< "StereoLayer"<<i<<"FieldWireSub";
	G4LogicalVolume* fieldWireSub_log=new G4LogicalVolume(fieldWireSub_tube,Al,osnameFieldWireSubLogical.str(),0,0,0);
	std::ostringstream osnameFieldWireSubPhys;
	osnameFieldWireSubPhys<<"physical"<< "StereoLayer"<<i<<"FieldWireSub";
	G4VPhysicalVolume* fieldWireSub_phys;
	fieldWireSub_phys=new G4PVPlacement(0,0,fieldWireSub_log,
					    osnameFieldWireSubPhys.str(),fieldWire_log,false,0);
	
	std::ostringstream osnameSignalWireSolid;
	osnameSignalWireSolid<<"solid"<< "StereoLayer"<<i<<"SignalWire";
	G4Tubs* signalWire_tube=new G4Tubs(osnameSignalWireSolid.str(),0.,signalWireR,innerLength,0.,360*deg);
	std::ostringstream osnameSignalWireLogical;
	osnameSignalWireLogical<<"logical"<< "StereoLayer"<<i<<"SignalWire";
	G4LogicalVolume* signalWire_log=new G4LogicalVolume(signalWire_tube,Au,osnameSignalWireLogical.str(),0,0,0);
	
	std::ostringstream osnameSignalWireSubSolid;
	osnameSignalWireSubSolid<<"solid"<< "StereoLayer"<<i<<"SignalWireSub";
	G4Tubs* signalWireSub_tube=new G4Tubs(osnameSignalWireSubSolid.str(),0.,signalWireR-thickOfAu,innerLength,0.,360*deg);
	std::ostringstream osnameSignalWireSubLogical;
	osnameSignalWireSubLogical<<"logical"<< "StereoLayer"<<i<<"SignalWireSub";
	G4LogicalVolume* signalWireSub_log=new G4LogicalVolume(signalWireSub_tube,W,osnameSignalWireSubLogical.str(),0,0,0);
	std::ostringstream osnameSignalWireSubPhys;
	osnameSignalWireSubPhys<<"physical"<< "StereoLayer"<<i<<"SignalWireSub";
	G4VPhysicalVolume* signalWireSub_phys;
	signalWireSub_phys=new G4PVPlacement(0,0,signalWireSub_log,
					     osnameSignalWireSubPhys.str(),signalWire_log,false,0);
	
	//phi  <------------------->-phi 
	//      |      F5     F2|  There are 1 signal wire S4,3 full field  
	//      |               |  wire F0,1,3 in each cell except Layer 35, 
	//      |      S4     F1|  2 more full field wire F2,5.
	//      |               |  In stereo cell we can't put half wire at edge because of
	//      |      F3     F0|  overlap, so all wires are full wire,Positon of field   
	//      ----------------|  wire is not at cell edge but move into cell avoid overlap
	G4double shiftR,midR,eastX,eastY,westX,westY;
	shiftR=fieldWireR+1*micrometer;
	midR=mdc->Layer(signalLayer).R()*mm;
	
	std::ostringstream osnameFieldWire0Phys;
	osnameFieldWire0Phys<<"physicalStereoLayer"<<i<<"FieldWire0";
	
	eastX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg-innerTwistAngle);
	eastY=(innerR+shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle)+shiftR*sin(90*deg-innerTwistAngle);
	westX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg+innerTwistAngle);
	westY=(innerR+shiftR/cos(innerTwistAngle))*sin(innerTwistAngle)+shiftR*sin(90*deg+innerTwistAngle);
	
	G4ThreeVector east0(eastX,eastY,innerLength);
	G4ThreeVector west0(westX,westY,-innerLength);
	east0.rotateZ(-spanAngle/2.);
	west0.rotateZ(-spanAngle/2.);
	
	posX=(east0.x()+west0.x())/2.;
	posY=(east0.y()+west0.y())/2.;
	G4ThreeVector line0=east0-west0;
	G4RotationMatrix* wireRot0=new G4RotationMatrix();
	wireRot0->rotateZ(-line0.phi());
	wireRot0->rotateY(-line0.theta());	
	G4VPhysicalVolume* fieldWire0_phys;
	fieldWire0_phys=new G4PVPlacement(wireRot0,G4ThreeVector(posX,posY,0),fieldWire_log,
					  osnameFieldWire0Phys.str(),twistTub_log,false,0);
	
	std::ostringstream osnameFieldWire1Phys;
	osnameFieldWire1Phys<<"physicalStereoLayer"<<i<<"FieldWire1";
	eastX=midR*cos(innerTwistAngle)+shiftR*cos(90*deg-innerTwistAngle);
	eastY=midR*sin(-innerTwistAngle)+shiftR*sin(90*deg-innerTwistAngle);
	westX=midR*cos(innerTwistAngle)+shiftR*cos(90*deg+innerTwistAngle);
	westY=midR*sin(innerTwistAngle)+shiftR*sin(90*deg+innerTwistAngle);
	G4ThreeVector east1(eastX,eastY,innerLength);
	G4ThreeVector west1(westX,westY,-innerLength);
	east1.rotateZ(-spanAngle/2.);
	west1.rotateZ(-spanAngle/2.);
	posX=(east1.x()+west1.x())/2.;
	posY=(east1.y()+west1.y())/2.;
	G4ThreeVector line1=east1-west1;
	G4RotationMatrix* wireRot1=new G4RotationMatrix();
	wireRot1->rotateZ(-line1.phi());
	wireRot1->rotateY(-line1.theta());	
	G4VPhysicalVolume* fieldWire1_phys;
	fieldWire1_phys=new G4PVPlacement(wireRot1,G4ThreeVector(posX,posY,0),fieldWire_log,
					  osnameFieldWire1Phys.str(),twistTub_log,false,1);
	
	std::ostringstream osnameFieldWire3Phys;
	osnameFieldWire3Phys<<"physicalStereoLayer"<<i<<"FieldWire3";
	
	eastX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	eastY=(innerR+shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle);
	westX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	westY=(innerR+shiftR/cos(innerTwistAngle))*sin(innerTwistAngle);
	
	G4ThreeVector east3(eastX,eastY,innerLength);
	G4ThreeVector west3(westX,westY,-innerLength);
	
	posX=(east3.x()+west3.x())/2.;
	posY=(east3.y()+west3.y())/2.;
	G4ThreeVector line3=east3-west3;
	G4RotationMatrix* wireRot3=new G4RotationMatrix();
	wireRot3->rotateZ(-line3.phi());
	wireRot3->rotateY(-line3.theta());	
	G4VPhysicalVolume* fieldWire3_phys;
	fieldWire3_phys=new G4PVPlacement(wireRot3,G4ThreeVector(posX,posY,0),fieldWire_log,
					  osnameFieldWire3Phys.str(),twistTub_log,false,3);
	
	std::ostringstream osnameSignalWire4Phys;
	osnameSignalWire4Phys<<"physicalStereoLayer"<<i<<"SignalWire4";
	
	eastX=midR*cos(innerTwistAngle);
	eastY=midR*sin(-innerTwistAngle);
	westX=midR*cos(innerTwistAngle);
	westY=midR*sin(innerTwistAngle);
	
	G4ThreeVector east4(eastX,eastY,innerLength);
	G4ThreeVector west4(westX,westY,-innerLength);
	
	posX=(east4.x()+west4.x())/2.;
	posY=(east4.y()+west4.y())/2.;
	G4ThreeVector line4=east4-west4;
	G4RotationMatrix* wireRot4=new G4RotationMatrix();
	wireRot4->rotateZ(-line4.phi());
	wireRot4->rotateY(-line4.theta());	
	G4VPhysicalVolume* signalWire4_phys;
	signalWire4_phys=new G4PVPlacement(wireRot4,G4ThreeVector(posX,posY,0),signalWire_log,
					   osnameSignalWire4Phys.str(),twistTub_log,false,4);
	
	if(i==35){
	  std::ostringstream osnameFieldWire2Phys;
	  osnameFieldWire2Phys<<"physicalStereoLayer"<<i<<"FieldWire2";
	  
	  eastX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg-innerTwistAngle);
	  eastY=(outR-shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle)+shiftR*sin(90*deg-innerTwistAngle);
	  westX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg+innerTwistAngle);
	  westY=(outR-shiftR/cos(innerTwistAngle))*sin(innerTwistAngle)+shiftR*sin(90*deg+innerTwistAngle);
	  
	  G4ThreeVector east2(eastX,eastY,innerLength);
	  G4ThreeVector west2(westX,westY,-innerLength);
	  east2.rotateZ(-spanAngle/2.);
	  west2.rotateZ(-spanAngle/2.);
	  
	  posX=(east2.x()+west2.x())/2.;
	  posY=(east2.y()+west2.y())/2.;
	  G4ThreeVector line2=east2-west2;
	  G4RotationMatrix* wireRot2=new G4RotationMatrix();
	  wireRot2->rotateZ(-line2.phi());
	  wireRot2->rotateY(-line2.theta());	
	  G4VPhysicalVolume* fieldWire2_phys;
	  fieldWire2_phys=new G4PVPlacement(wireRot2,G4ThreeVector(posX,posY,0),fieldWire_log,
					    osnameFieldWire2Phys.str(),twistTub_log,false,2);
	  
	  std::ostringstream osnameFieldWire5Phys;
	  osnameFieldWire5Phys<<"physicalStereoLayer"<<i<<"FieldWire5";
	  
	  eastX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	  eastY=(outR-shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle);
	  westX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	  westY=(outR-shiftR/cos(innerTwistAngle))*sin(innerTwistAngle);
	  
	  G4ThreeVector east5(eastX,eastY,innerLength);
	  G4ThreeVector west5(westX,westY,-innerLength);
	  
	  posX=(east5.x()+west5.x())/2.;
	  posY=(east5.y()+west5.y())/2.;
	  G4ThreeVector line5=east5-west5;
	  G4RotationMatrix* wireRot5=new G4RotationMatrix();
	  wireRot5->rotateZ(-line5.phi());
	  wireRot5->rotateY(-line5.theta());	
	  G4VPhysicalVolume* fieldWire5_phys;
	  fieldWire5_phys=new G4PVPlacement(wireRot5,G4ThreeVector(posX,posY,0),fieldWire_log,
					    osnameFieldWire5Phys.str(),twistTub_log,false,5);
	}
      }
      //Put cells into layer
      for(j=replicaNo-1;j>-1;j--){
	G4RotationMatrix* cellRot=new G4RotationMatrix();
	cellRot->rotateZ(-spanAngle*j);

	std::ostringstream osnameCellPhys;
	osnameCellPhys<<"physical"<< "MdcStereoLayer"<<i<<"Cell"<<j;
	G4VPhysicalVolume* twistTub_phys;
	twistTub_phys=new G4PVPlacement(cellRot,0,twistTub_log,
					osnameCellPhys.str(),stereoLayer_log,false,j);
      }
    }

    //----Axial layers 9-20    
    for(i=19; i>7; i--){
      signalLayer=mdc->Signal2Global(i);
      innerR=mdc->Layer(signalLayer-1).R()*mm-fieldWireR;
      outR=mdc->Layer(signalLayer+1).R()*mm-fieldWireR;
      if(i==19)outR=mdc->Layer(signalLayer+1).R()*mm+fieldWireR;
      length=(mdc->Layer(signalLayer).Length())/2.*mm;
      startAngle=0.*deg;
      spanAngle=360.*deg;
      firstWire=mdc->Layer(signalLayer).FirstWire();
      posX  = 0.*m;
      posY  = 0.*m;
      posZ  = 0.*m;
      
      //Layer
      std::ostringstream osnameLayerSolid;
      osnameLayerSolid <<"solid"<< "MdcAxialLayer"<<i;  
      G4Tubs* axialLayer_tube=new G4Tubs(osnameLayerSolid.str(),innerR,outR,length,startAngle,spanAngle);

      std::ostringstream osnameLayerLogical;
      osnameLayerLogical <<"logical"<< "MdcAxialLayer"<<i;
      G4LogicalVolume* axialLayer_log=new G4LogicalVolume(axialLayer_tube, MdcGas,osnameLayerLogical.str(),0,0,0);
      axialLayer_log->SetVisAttributes(G4VisAttributes::Invisible);

      replicaNo=mdc->Layer(signalLayer).WireNo()/2;
      spanAngle=360./replicaNo*deg;
      
      offset=mdc->Layer(signalLayer).Phi()*rad-firstWire*spanAngle/2.;
      G4RotationMatrix* layerRot=new G4RotationMatrix();
      layerRot->rotateZ(-offset);

      std::ostringstream osnameLayerPhys;
      osnameLayerPhys<<"physical"<< "MdcAxialLayer"<<i;
      G4VPhysicalVolume* axialLayer_phys;
      axialLayer_phys=new G4PVPlacement(layerRot,0,axialLayer_log,
					osnameLayerPhys.str(),mdc_log,false,i);

      //Cell      
      std::ostringstream osnameCellSolid;
      osnameCellSolid<<"solid"<< "MdcAxialLayer"<<i<<"Cell"; 
      G4Tubs* replica_tube=new G4Tubs(osnameCellSolid.str(),innerR,outR,length,startAngle,spanAngle);

      std::ostringstream osnameCellLogical;
      osnameCellLogical<<"logical"<< "MdcAxialLayer"<<i<<"Cell";
      G4LogicalVolume* replica_log=new G4LogicalVolume(replica_tube,MdcGas,osnameCellLogical.str(),0,0,0);
      
      replica_log->SetSensitiveDetector( aTrackerSD );
      visAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0));
      replica_log->SetVisAttributes(visAtt);
      replica_log->SetVisAttributes(G4VisAttributes::Invisible);
      //       replica_log->SetUserLimits(new G4UserLimits(maxStep));
      //Wire
      if(ReadBoostRoot::GetMdc()==1){
	std::ostringstream osnameFieldWireSolid;
	osnameFieldWireSolid<<"solid"<< "AxialLayer"<<i<<"FieldWire";
	G4Tubs* fieldWire_tube=new G4Tubs(osnameFieldWireSolid.str(),0.,fieldWireR,length,0.,360*deg);
	std::ostringstream osnameFieldWireLogical;
	osnameFieldWireLogical<<"logical"<< "AxialLayer"<<i<<"FieldWire";
	G4LogicalVolume* fieldWire_log=new G4LogicalVolume(fieldWire_tube,Au,osnameFieldWireLogical.str(),0,0,0);
	
	std::ostringstream osnameFieldWireSubSolid;
	osnameFieldWireSubSolid<<"solid"<< "AxialLayer"<<i<<"FieldWireSub";
	G4Tubs* fieldWireSub_tube=new G4Tubs(osnameFieldWireSubSolid.str(),0.,fieldWireR-thickOfAu,length,0.,360*deg);
	std::ostringstream osnameFieldWireSubLogical;
	osnameFieldWireSubLogical<<"logical"<< "AxialLayer"<<i<<"FieldWireSub";
	G4LogicalVolume* fieldWireSub_log=new G4LogicalVolume(fieldWireSub_tube,Al,osnameFieldWireSubLogical.str(),0,0,0);
	std::ostringstream osnameFieldWireSubPhys;
	osnameFieldWireSubPhys<<"physical"<< "AxialLayer"<<i<<"FieldWireSub";
	G4VPhysicalVolume* fieldWireSub_phys;
	fieldWireSub_phys=new G4PVPlacement(0,0,fieldWireSub_log,
					    osnameFieldWireSubPhys.str(),fieldWire_log,false,0);
	
	std::ostringstream osnameFieldWireHalfSolid;
	osnameFieldWireHalfSolid<<"solid"<< "AxialLayer"<<i<<"FieldWireHalf";
	G4Tubs* fieldWireHalf_tube=new G4Tubs(osnameFieldWireHalfSolid.str(),0.,fieldWireR,length,0.,180*deg);
	std::ostringstream osnameFieldWireHalfLogical;
	osnameFieldWireHalfLogical<<"logical"<< "AxialLayer"<<i<<"FieldWireHalf";
	G4LogicalVolume* fieldWireHalf_log=new G4LogicalVolume(fieldWireHalf_tube,Au,osnameFieldWireHalfLogical.str(),0,0,0);
	
	std::ostringstream osnameFieldWireHalfSubSolid;
	osnameFieldWireHalfSubSolid<<"solid"<< "AxialLayer"<<i<<"FieldWireHalfSub";
	G4Tubs* fieldWireHalfSub_tube=new G4Tubs(osnameFieldWireHalfSubSolid.str(),0.,fieldWireR-thickOfAu,length,0.,360*deg);
	std::ostringstream osnameFieldWireHalfSubLogical;
	osnameFieldWireHalfSubLogical<<"logical"<< "AxialLayer"<<i<<"FieldWireHalfSub";
	G4LogicalVolume* fieldWireHalfSub_log=new G4LogicalVolume(fieldWireHalfSub_tube,Al,osnameFieldWireHalfSubLogical.str(),0,0,0);
	std::ostringstream osnameFieldWireHalfSubPhys;
	osnameFieldWireHalfSubPhys<<"physical"<< "AxialLayer"<<i<<"FieldWireHalfSub";
	G4VPhysicalVolume* fieldWireHalfSub_phys;
	fieldWireHalfSub_phys=new G4PVPlacement(0,0,fieldWireHalfSub_log,
						osnameFieldWireHalfSubPhys.str(),fieldWireHalf_log,false,0);
	
	std::ostringstream osnameSignalWireSolid;
	osnameSignalWireSolid<<"solid"<< "AxialLayer"<<i<<"SignalWire";
	G4Tubs* signalWire_tube=new G4Tubs(osnameSignalWireSolid.str(),0.,signalWireR,length,0.,360*deg);
	std::ostringstream osnameSignalWireLogical;
	osnameSignalWireLogical<<"logical"<< "AxialLayer"<<i<<"SignalWire";
	G4LogicalVolume* signalWire_log=new G4LogicalVolume(signalWire_tube,Au,osnameSignalWireLogical.str(),0,0,0);
	
	std::ostringstream osnameSignalWireSubSolid;
	osnameSignalWireSubSolid<<"solid"<< "AxialLayer"<<i<<"SignalWireSub";
	G4Tubs* signalWireSub_tube=new G4Tubs(osnameSignalWireSubSolid.str(),0.,signalWireR-thickOfAu,length,0.,360*deg);
	std::ostringstream osnameSignalWireSubLogical;
	osnameSignalWireSubLogical<<"logical"<< "AxialLayer"<<i<<"SignalWireSub";
	G4LogicalVolume* signalWireSub_log=new G4LogicalVolume(signalWireSub_tube,W,osnameSignalWireSubLogical.str(),0,0,0);
	std::ostringstream osnameSignalWireSubPhys;
	osnameSignalWireSubPhys<<"physical"<< "AxialLayer"<<i<<"SignalWireSub";
	G4VPhysicalVolume* signalWireSub_phys;
	signalWireSub_phys=new G4PVPlacement(0,0,signalWireSub_log,
					     osnameSignalWireSubPhys.str(),signalWire_log,false,0);
	
	//phi  <------------------->-phi 
	//      |F8      F5     F2|  There are 1 signal wire S4,1 full field wire F3,
	//      |                 |   4 half field wire F0,1,6,7 in each cell except
	//      |F7      S4     F1|  layer 19 , each cell have 1 more full
	//      |                 |  field wire F5, 2 more half field wire F2,8
	//      |F6      F3     F0|   
	//      -------------------
	std::ostringstream osnameFieldWire0Phys;
	osnameFieldWire0Phys<<"physicalAxialLayer"<<i<<"FieldWire0";
	posX=mdc->Layer(signalLayer-1).R()*mm;
	G4VPhysicalVolume* fieldWire0_phys;
	fieldWire0_phys=new G4PVPlacement(0,G4ThreeVector(posX,0,0),fieldWireHalf_log,
					  osnameFieldWire0Phys.str(),replica_log,false,0);
	
	std::ostringstream osnameFieldWire1Phys;
	osnameFieldWire1Phys<<"physicalAxialLayer"<<i<<"FieldWire1";
	posX=mdc->Layer(signalLayer).R()*mm;
	G4VPhysicalVolume* fieldWire1_phys;
	fieldWire1_phys=new G4PVPlacement(0,G4ThreeVector(posX,0,0),fieldWireHalf_log,
					  osnameFieldWire1Phys.str(),replica_log,false,1);
	
	if(i==19){
	  std::ostringstream osnameFieldWire2Phys;
	  osnameFieldWire2Phys<<"physicalAxialLayer"<<i<<"FieldWire2";
	  posX=mdc->Layer(signalLayer+1).R()*mm;
	  G4VPhysicalVolume* fieldWire2_phys;
	  fieldWire2_phys=new G4PVPlacement(0,G4ThreeVector(posX,0,0),fieldWireHalf_log,
					    osnameFieldWire2Phys.str(),replica_log,false,2);
	}
	
	std::ostringstream osnameFieldWire3Phys;
	osnameFieldWire3Phys<<"physicalAxialLayer"<<i<<"FieldWire3";
	posX=mdc->Layer(signalLayer-1).R()*mm*cos(spanAngle/2.);
	posY=mdc->Layer(signalLayer-1).R()*mm*sin(spanAngle/2.);
	G4VPhysicalVolume* fieldWire3_phys;
	fieldWire3_phys=new G4PVPlacement(0,G4ThreeVector(posX,posY,0),fieldWire_log,
					  osnameFieldWire3Phys.str(),replica_log,false,3);
	
	std::ostringstream osnameSignalWirePhys;
	osnameSignalWirePhys<<"physicalAxialLayer"<<i<<"SignalWire4";
	posX=mdc->Layer(signalLayer).R()*mm*cos(spanAngle/2.);
	posY=mdc->Layer(signalLayer).R()*mm*sin(spanAngle/2.);
	G4VPhysicalVolume* signalWire_phys;
	signalWire_phys=new G4PVPlacement(0,G4ThreeVector(posX,posY,0),signalWire_log,
					  osnameSignalWirePhys.str(),replica_log,false,4);
	
	if(i==19){
	  std::ostringstream osnameFieldWire5Phys;
	  osnameFieldWire5Phys<<"physicalAxialLayer"<<i<<"FieldWire5";
	  posX=mdc->Layer(signalLayer+1).R()*mm*cos(spanAngle/2.);
	  posY=mdc->Layer(signalLayer+1).R()*mm*sin(spanAngle/2.);
	  G4VPhysicalVolume* fieldWire5_phys;
	  fieldWire5_phys=new G4PVPlacement(0,G4ThreeVector(posX,posY,0),fieldWire_log,
					    osnameFieldWire5Phys.str(),replica_log,false,5);
	}
	
	std::ostringstream osnameFieldWire6Phys;
	osnameFieldWire6Phys<<"physicalAxialLayer"<<i<<"FieldWire6";
	posX=mdc->Layer(signalLayer-1).R()*mm*cos(spanAngle);
	posY=mdc->Layer(signalLayer-1).R()*mm*sin(spanAngle);
	G4RotationMatrix* wireRot6=new G4RotationMatrix();
	wireRot6->rotateZ(180*deg-spanAngle);
	G4VPhysicalVolume* fieldWire6_phys;
	fieldWire6_phys=new G4PVPlacement(wireRot6,G4ThreeVector(posX,posY,0),fieldWireHalf_log,
					  osnameFieldWire6Phys.str(),replica_log,false,6);
	
	std::ostringstream osnameFieldWire7Phys;
	osnameFieldWire7Phys<<"physicalAxialLayer"<<i<<"FieldWire7";
	posX=mdc->Layer(signalLayer).R()*mm*cos(spanAngle);
	posY=mdc->Layer(signalLayer).R()*mm*sin(spanAngle);
	G4RotationMatrix* wireRot7=new G4RotationMatrix();
	wireRot7->rotateZ(180*deg-spanAngle);
	G4VPhysicalVolume* fieldWire7_phys;
	fieldWire7_phys=new G4PVPlacement(wireRot7,G4ThreeVector(posX,posY,0),fieldWireHalf_log,
					  osnameFieldWire7Phys.str(),replica_log,false,7);
	
	if(i==19){
	  std::ostringstream osnameFieldWire8Phys;
	  osnameFieldWire8Phys<<"physicalAxialLayer"<<i<<"FieldWire8";
	  posX=mdc->Layer(signalLayer+1).R()*mm*cos(spanAngle);
	  posY=mdc->Layer(signalLayer+1).R()*mm*sin(spanAngle);
	  G4RotationMatrix* wireRot8=new G4RotationMatrix();
	  wireRot8->rotateZ(180*deg-spanAngle);
	  G4VPhysicalVolume* fieldWire8_phys;
	  fieldWire8_phys=new G4PVPlacement(wireRot8,G4ThreeVector(posX,posY,0),fieldWireHalf_log,
					    osnameFieldWire8Phys.str(),replica_log,false,8);
	}
      } 
      //Put cells into layer
      for(j=replicaNo-1;j>-1;j--){
	G4RotationMatrix* cellRot=new G4RotationMatrix();
	cellRot->rotateZ(-spanAngle*j);
	
	std::ostringstream osnameCellPhys;
	osnameCellPhys<<"physical"<< "MdcAxialLayer"<<i<<"Cell"<<j;
	G4VPhysicalVolume* replica_phys;
	replica_phys=new G4PVPlacement(cellRot,0,replica_log,
				       osnameCellPhys.str(),axialLayer_log,false,j);
      }	 
    }  

    //----Stereo layers 1-8
    for(i=7; i>-1; i--){
      signalLayer=mdc->Signal2Global(i);

      innerR=mdc->Layer(signalLayer-1).R()*mm-fieldWireR;
      outR=mdc->Layer(signalLayer+1).R()*mm-fieldWireR;
      if(i==7)outR=mdc->Layer(signalLayer+1).R()*mm+fieldWireR;

      innerLength=(mdc->Layer(signalLayer-1).Length())/2.*mm;
      outLength=(mdc->Layer(signalLayer+1).Length())/2.*mm;

      innerTwistAngle=mdc->Layer(signalLayer-1).RotateAngle()*rad;
      outTwistAngle=mdc->Layer(signalLayer+1).RotateAngle()*rad;

      innerTan=innerR/innerLength*sin(innerTwistAngle);
		
      midInnerR=innerR*cos(innerTwistAngle);
      innerStereo=atan(innerTan);

      outTan=outR/outLength*sin(outTwistAngle);
      outTwistAngleFixed=atan(innerLength/outLength*tan(outTwistAngle));
 
      if(abs(outTwistAngleFixed) >= abs(innerTwistAngle)){
	midOutR=outR*cos(outTwistAngle);
	outRFixed=midOutR/cos(innerTwistAngle);
	outR=outRFixed;

	outTanFixed=midOutR/innerLength*tan(innerTwistAngle);
	outStereo=atan(outTanFixed);
      }else{
	outRFixed=sqrt(outR*outR+(innerLength*innerLength-outLength*outLength)*outTan*outTan);
	outR=outRFixed;
	midOutR=outR*cos(innerTwistAngle);

	outTanFixed=outRFixed/innerLength*sin(innerTwistAngle);
	outStereo=atan(outTanFixed);
      }

      //Layer
      std::ostringstream osnameLayerSolid;
      osnameLayerSolid <<"solid"<< "MdcStereoLayer"<<i;  

      G4Hype* stereoLayer_hype=new G4Hype(osnameLayerSolid.str(),midInnerR, midOutR,innerStereo,
					  outStereo,innerLength);
      std::ostringstream osnameLayerLogical;
      osnameLayerLogical <<"logical"<< "MdcStereoLayer"<<i;
      G4LogicalVolume* stereoLayer_log=new G4LogicalVolume(stereoLayer_hype,MdcGas,
							   osnameLayerLogical.str(),0,0,0);
      stereoLayer_log->SetVisAttributes(G4VisAttributes::Invisible);

      replicaNo=mdc->Layer(signalLayer).WireNo()/2;
      spanAngle=360./replicaNo*deg;
      firstWire=mdc->Layer(signalLayer).FirstWire();

      G4RotationMatrix* layerRot=new G4RotationMatrix();
      layerRot->rotateZ(-(spanAngle*(1-firstWire)/2.+innerTwistAngle));

      std::ostringstream osnameLayerPhys;
      osnameLayerPhys<<"physical"<< "MdcStereoLayer"<<i;

      G4VPhysicalVolume* stereoLayer_phys;
      stereoLayer_phys=new G4PVPlacement(layerRot,0,stereoLayer_log,osnameLayerPhys.str(),
					 mdc_log,false,i);

      //Cell
      std::ostringstream osnameCellSolid;
      osnameCellSolid<<"solid"<< "MdcStereoLayer"<<i<<"Cell";

      G4TwistedTubs* twistTub=new G4TwistedTubs(osnameCellSolid.str(), -innerTwistAngle*2, innerR,outR-1.0*micrometer,innerLength,spanAngle);
      
     std::ostringstream osnameCellLogical;
     osnameCellLogical<<"logical"<< "MdcStereoLayer"<<i<<"Cell";
      G4LogicalVolume* twistTub_log=new G4LogicalVolume(twistTub, MdcGas,osnameCellLogical.str(),0,0,0);
      
      twistTub_log->SetVisAttributes(G4VisAttributes::Invisible);
      twistTub_log->SetSensitiveDetector( aTrackerSD );
      //       twistTub_log->SetUserLimits(new G4UserLimits(maxStep));       
      //Wire
      if(ReadBoostRoot::GetMdc()==1){
	std::ostringstream osnameFieldWireSolid;
	osnameFieldWireSolid<<"solid"<< "StereoLayer"<<i<<"FieldWire";
	G4Tubs* fieldWire_tube=new G4Tubs(osnameFieldWireSolid.str(),0.,fieldWireR,innerLength,0.,360*deg);
	std::ostringstream osnameFieldWireLogical;
	osnameFieldWireLogical<<"logical"<< "StereoLayer"<<i<<"FieldWire";
	G4LogicalVolume* fieldWire_log=new G4LogicalVolume(fieldWire_tube,Au,osnameFieldWireLogical.str(),0,0,0);
	
	std::ostringstream osnameFieldWireSubSolid;
	osnameFieldWireSubSolid<<"solid"<< "StereoLayer"<<i<<"FieldWireSub";
	G4Tubs* fieldWireSub_tube=new G4Tubs(osnameFieldWireSubSolid.str(),0.,fieldWireR-thickOfAu,innerLength,0.,360*deg);
	std::ostringstream osnameFieldWireSubLogical;
	osnameFieldWireSubLogical<<"logical"<< "StereoLayer"<<i<<"FieldWireSub";
	G4LogicalVolume* fieldWireSub_log=new G4LogicalVolume(fieldWireSub_tube,Al,osnameFieldWireSubLogical.str(),0,0,0);
	std::ostringstream osnameFieldWireSubPhys;
	osnameFieldWireSubPhys<<"physical"<< "StereoLayer"<<i<<"FieldWireSub";
	G4VPhysicalVolume* fieldWireSub_phys;
	fieldWireSub_phys=new G4PVPlacement(0,0,fieldWireSub_log,
					    osnameFieldWireSubPhys.str(),fieldWire_log,false,0);
	
	
	std::ostringstream osnameSignalWireSolid;
	osnameSignalWireSolid<<"solid"<< "StereoLayer"<<i<<"SignalWire";
	G4Tubs* signalWire_tube=new G4Tubs(osnameSignalWireSolid.str(),0.,signalWireR,innerLength,0.,360*deg);
	std::ostringstream osnameSignalWireLogical;
	osnameSignalWireLogical<<"logical"<< "StereoLayer"<<i<<"SignalWire";
	G4LogicalVolume* signalWire_log=new G4LogicalVolume(signalWire_tube,Au,osnameSignalWireLogical.str(),0,0,0);
	
	std::ostringstream osnameSignalWireSubSolid;
	osnameSignalWireSubSolid<<"solid"<< "StereoLayer"<<i<<"SignalWireSub";
	G4Tubs* signalWireSub_tube=new G4Tubs(osnameSignalWireSubSolid.str(),0.,signalWireR-thickOfAu,innerLength,0.,360*deg);
	std::ostringstream osnameSignalWireSubLogical;
	osnameSignalWireSubLogical<<"logical"<< "StereoLayer"<<i<<"SignalWireSub";
	G4LogicalVolume* signalWireSub_log=new G4LogicalVolume(signalWireSub_tube,W,osnameSignalWireSubLogical.str(),0,0,0);
	std::ostringstream osnameSignalWireSubPhys;
	osnameSignalWireSubPhys<<"physical"<< "StereoLayer"<<i<<"SignalWireSub";
	G4VPhysicalVolume* signalWireSub_phys;
	signalWireSub_phys=new G4PVPlacement(0,0,signalWireSub_log,
					     osnameSignalWireSubPhys.str(),signalWire_log,false,0);
	
	//phi  <------------------->-phi 
	//      |      F5     F2|  There are 1 signal wire S4,3 full field  
	//      |               |  wire F0,1,3 in each cell except layer 7 , 
	//      |      S4     F1|  2 more full field wire F2,5.
	//      |               |  In stereo cell we can't put half wire at edge because of
	//      |      F3     F0|  overlap, so all wires are full wire,Positon of field   
	//      ----------------|  wire is not at cell edge but move into cell avoid overlap
	G4double shiftR,midR,eastX,eastY,westX,westY;
	shiftR=fieldWireR+1*micrometer;
	midR=mdc->Layer(signalLayer).R()*mm;
	
	std::ostringstream osnameFieldWire0Phys;
	osnameFieldWire0Phys<<"physicalStereoLayer"<<i<<"FieldWire0";
	
	eastX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg-innerTwistAngle);
	eastY=(innerR+shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle)+shiftR*sin(90*deg-innerTwistAngle);
	westX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg+innerTwistAngle);
	westY=(innerR+shiftR/cos(innerTwistAngle))*sin(innerTwistAngle)+shiftR*sin(90*deg+innerTwistAngle);
	
	G4ThreeVector east0(eastX,eastY,innerLength);
	G4ThreeVector west0(westX,westY,-innerLength);
	east0.rotateZ(-spanAngle/2.);
	west0.rotateZ(-spanAngle/2.);
	
	posX=(east0.x()+west0.x())/2.;
	posY=(east0.y()+west0.y())/2.;
	G4ThreeVector line0=east0-west0;
	G4RotationMatrix* wireRot0=new G4RotationMatrix();
	wireRot0->rotateZ(-line0.phi());
	wireRot0->rotateY(-line0.theta());	
	G4VPhysicalVolume* fieldWire0_phys;
	fieldWire0_phys=new G4PVPlacement(wireRot0,G4ThreeVector(posX,posY,0),fieldWire_log,
					  osnameFieldWire0Phys.str(),twistTub_log,false,0);
	
	std::ostringstream osnameFieldWire1Phys;
	osnameFieldWire1Phys<<"physicalStereoLayer"<<i<<"FieldWire1";
	eastX=midR*cos(innerTwistAngle)+shiftR*cos(90*deg-innerTwistAngle);
	eastY=midR*sin(-innerTwistAngle)+shiftR*sin(90*deg-innerTwistAngle);
	westX=midR*cos(innerTwistAngle)+shiftR*cos(90*deg+innerTwistAngle);
	westY=midR*sin(innerTwistAngle)+shiftR*sin(90*deg+innerTwistAngle);
	G4ThreeVector east1(eastX,eastY,innerLength);
	G4ThreeVector west1(westX,westY,-innerLength);
	east1.rotateZ(-spanAngle/2.);
	west1.rotateZ(-spanAngle/2.);
	posX=(east1.x()+west1.x())/2.;
	posY=(east1.y()+west1.y())/2.;
	G4ThreeVector line1=east1-west1;
	G4RotationMatrix* wireRot1=new G4RotationMatrix();
	wireRot1->rotateZ(-line1.phi());
	wireRot1->rotateY(-line1.theta());	
	G4VPhysicalVolume* fieldWire1_phys;
	fieldWire1_phys=new G4PVPlacement(wireRot1,G4ThreeVector(posX,posY,0),fieldWire_log,
					  osnameFieldWire1Phys.str(),twistTub_log,false,1);
	
	std::ostringstream osnameFieldWire3Phys;
	osnameFieldWire3Phys<<"physicalStereoLayer"<<i<<"FieldWire3";
	
	eastX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	eastY=(innerR+shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle);
	westX=(innerR+shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	westY=(innerR+shiftR/cos(innerTwistAngle))*sin(innerTwistAngle);
	
	G4ThreeVector east3(eastX,eastY,innerLength);
	G4ThreeVector west3(westX,westY,-innerLength);
	
	posX=(east3.x()+west3.x())/2.;
	posY=(east3.y()+west3.y())/2.;
	G4ThreeVector line3=east3-west3;
	G4RotationMatrix* wireRot3=new G4RotationMatrix();
	wireRot3->rotateZ(-line3.phi());
	wireRot3->rotateY(-line3.theta());	
	G4VPhysicalVolume* fieldWire3_phys;
	fieldWire3_phys=new G4PVPlacement(wireRot3,G4ThreeVector(posX,posY,0),fieldWire_log,
					  osnameFieldWire3Phys.str(),twistTub_log,false,3);
	
	std::ostringstream osnameSignalWire4Phys;
	osnameSignalWire4Phys<<"physicalStereoLayer"<<i<<"SignalWire4";
	
	eastX=midR*cos(innerTwistAngle);
	eastY=midR*sin(-innerTwistAngle);
	westX=midR*cos(innerTwistAngle);
	westY=midR*sin(innerTwistAngle);
	
	G4ThreeVector east4(eastX,eastY,innerLength);
	G4ThreeVector west4(westX,westY,-innerLength);
	
	posX=(east4.x()+west4.x())/2.;
	posY=(east4.y()+west4.y())/2.;
	G4ThreeVector line4=east4-west4;
	G4RotationMatrix* wireRot4=new G4RotationMatrix();
	wireRot4->rotateZ(-line4.phi());
	wireRot4->rotateY(-line4.theta());	
	G4VPhysicalVolume* signalWire4_phys;
	signalWire4_phys=new G4PVPlacement(wireRot4,G4ThreeVector(posX,posY,0),signalWire_log,
					   osnameSignalWire4Phys.str(),twistTub_log,false,4);
	
	if(i==7){
	  std::ostringstream osnameFieldWire2Phys;
	  osnameFieldWire2Phys<<"physicalStereoLayer"<<i<<"FieldWire2";
	  
	  eastX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg-innerTwistAngle);
	  eastY=(outR-shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle)+shiftR*sin(90*deg-innerTwistAngle);
	  westX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle)+shiftR*cos(90*deg+innerTwistAngle);
	  westY=(outR-shiftR/cos(innerTwistAngle))*sin(innerTwistAngle)+shiftR*sin(90*deg+innerTwistAngle);
	  
	  G4ThreeVector east2(eastX,eastY,innerLength);
	  G4ThreeVector west2(westX,westY,-innerLength);
	  east2.rotateZ(-spanAngle/2.);
	  west2.rotateZ(-spanAngle/2.);
	  
	  posX=(east2.x()+west2.x())/2.;
	  posY=(east2.y()+west2.y())/2.;
	  G4ThreeVector line2=east2-west2;
	  G4RotationMatrix* wireRot2=new G4RotationMatrix();
	  wireRot2->rotateZ(-line2.phi());
	  wireRot2->rotateY(-line2.theta());	
	  G4VPhysicalVolume* fieldWire2_phys;
	  fieldWire2_phys=new G4PVPlacement(wireRot2,G4ThreeVector(posX,posY,0),fieldWire_log,
					    osnameFieldWire2Phys.str(),twistTub_log,false,2);
	  
	  std::ostringstream osnameFieldWire5Phys;
	  osnameFieldWire5Phys<<"physicalStereoLayer"<<i<<"FieldWire5";
	  
	  eastX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	  eastY=(outR-shiftR/cos(innerTwistAngle))*sin(-innerTwistAngle);
	  westX=(outR-shiftR/cos(innerTwistAngle))*cos(innerTwistAngle);
	  westY=(outR-shiftR/cos(innerTwistAngle))*sin(innerTwistAngle);
	  
	  G4ThreeVector east5(eastX,eastY,innerLength);
	  G4ThreeVector west5(westX,westY,-innerLength);
	  
	  posX=(east5.x()+west5.x())/2.;
	  posY=(east5.y()+west5.y())/2.;
	  G4ThreeVector line5=east5-west5;
	  G4RotationMatrix* wireRot5=new G4RotationMatrix();
	  wireRot5->rotateZ(-line5.phi());
	  wireRot5->rotateY(-line5.theta());	
	  G4VPhysicalVolume* fieldWire5_phys;
	  fieldWire5_phys=new G4PVPlacement(wireRot5,G4ThreeVector(posX,posY,0),fieldWire_log,
					    osnameFieldWire5Phys.str(),twistTub_log,false,5);
	}
      }
      //Put cells into layer
      for(j=replicaNo-1;j>-1;j--){
	G4RotationMatrix* cellRot=new G4RotationMatrix();
	cellRot->rotateZ(-spanAngle*j);
	
	std::ostringstream osnameCellPhys;
	osnameCellPhys<<"physical"<< "MdcStereoLayer"<<i<<"Cell"<<j;
	G4VPhysicalVolume* twistTub_phys;
	twistTub_phys=new G4PVPlacement(cellRot,0,twistTub_log,
					osnameCellPhys.str(),stereoLayer_log,false,j);
      }
    }
            
  } 
}
