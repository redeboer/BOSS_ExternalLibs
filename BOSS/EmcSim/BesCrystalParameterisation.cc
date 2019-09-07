//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Sep 4, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#include "BesCrystalParameterisation.hh"

#include "BesEmcConstruction.hh"
#include "BesEmcGeometry.hh"
//#include "BesDetectorConstruction.hh"

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Trap.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesCrystalParameterisation::BesCrystalParameterisation(  
        const G4int    startID,
	const G4int    endID,
	const G4int    thetaAllCrystals,
	BesEmcGeometry* besEMCGeometry,
	const G4int    verboseLevel
        ):fVerboseLevel(0)
{
  //for debug
  //G4Exception("BesCrystalParameterisation::BesCrystalParameterisation() starting........");
  fBesEmcGeometry   =  besEMCGeometry;
  fVerboseLevel     =  verboseLevel; //if need to debug this class,open it
  fStartID          =  startID;
  fAllCrystals      =  thetaAllCrystals;
  fCrystalID        =  0;
  fFlagLeft         =  true;
  if (startID>endID||endID>thetaAllCrystals||startID<1)
    {
      G4Exception("BesCrystalParameterisation construction: ID of crystal>No avaible!");
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesCrystalParameterisation::~BesCrystalParameterisation()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesCrystalParameterisation::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  //for debug
  //G4Exception("BesCrystalParameterisation::ComputeTransformation() starting........");
  ComputeIDAndSide(physVol);

  if(fVerboseLevel>3)
    {
      G4cout<<"*BesCrystalParameterisation::ComputeTransformation()*"<<G4endl;
      G4cout << "copyNo(transfered)=" << copyNo << G4endl
	     << "copyNo(gotten)    =" << physVol->GetCopyNo() << G4endl
	     << "fStartID          =" << fStartID << G4endl
	     << "fCrystalID        =" << fCrystalID << G4endl;
      G4cout << "point of fBesEmcGeometry=" << fBesEmcGeometry << G4endl;
    }
  
  G4double xPosition= fBesEmcGeometry->GetXPosition(fCrystalID);
  G4double yPosition= fBesEmcGeometry->GetYPosition(fCrystalID);
  G4double zPosition= fBesEmcGeometry->GetZPosition(fCrystalID);
  //G4Exception("BesCrystalParameterisation::ComputeTransformation() running........");
  //G4double xPosition=0,yPosition=0,zPosition=0;
  if(fFlagLeft) zPosition=-zPosition;
  G4ThreeVector origin(xPosition,yPosition,zPosition);
  physVol->SetTranslation(origin);

  //G4double thetaPosition=fBesEmcGeometry->GetThetaPosition(fCrystalID);
  //G4RotationMatrix *rotateMatrix;
  //rotateMatrix = new G4RotationMatrix();
  //if(fFlagLeft)
  //  {
  //    rotateMatrix->rotateZ(-90*deg);
  //    rotateMatrix->rotateX(-180*deg+thetaPosition);
  //  }
  //else
  //  {
  //    rotateMatrix->rotateZ(-90*deg);
  //    rotateMatrix->rotateX(-thetaPosition);
  //  }
  //G4double delta = 0*deg;
  //G4ThreeVector axis = G4ThreeVector(0,0,0);
  //rotateMatrix->getAngleAxis(delta, axis);
  if(fVerboseLevel>5)
    {
    G4cout << "The crystals are at the position:" << G4endl
	   << origin/cm <<"(cm)" << G4endl;
    //G4cout << thetaPosition/deg <<"(deg)." << G4endl
    //<< "The axis of crystals in the world system is: " << G4endl
    //<< delta/deg << "(deg)(delta) " << G4endl
    //<< axis  << "(Z axis)" << G4endl;
    }
  //physVol->SetRotation(rotateMatrix);
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesCrystalParameterisation::ComputeDimensions
(G4Trap& trackerChamber, const G4int copyNo,
 const G4VPhysicalVolume* physVol) const
{
  //for debug
  //G4Exception("BesCrystalParameterisation::ComputeDimensions() starting........");
  ComputeIDAndSide(physVol);

  G4double zHalfLength  = fBesEmcGeometry->GetZHalfLength(fCrystalID);
  G4double thetaAxis    = fBesEmcGeometry->GetThetaAxis(fCrystalID);
  G4double phiAxis      = fBesEmcGeometry->GetPhiAxis(fCrystalID);
  G4double yHalfLength1 = fBesEmcGeometry->GetYHalfLength1(fCrystalID);
  G4double xHalfLength1 = fBesEmcGeometry->GetXHalfLength1(fCrystalID);
  G4double xHalfLength2 = fBesEmcGeometry->GetXHalfLength2(fCrystalID);
  G4double tanAlpha1    = fBesEmcGeometry->GetTanAlpha1(fCrystalID);
  G4double yHalfLength2 = fBesEmcGeometry->GetYHalfLength2(fCrystalID);
  G4double xHalfLength3 = fBesEmcGeometry->GetXHalfLength3(fCrystalID);
  G4double xHalfLength4 = fBesEmcGeometry->GetXHalfLength4(fCrystalID);
  G4double tanAlpha2    = fBesEmcGeometry->GetTanAlpha2(fCrystalID);

  if(!fFlagLeft)
    {
      phiAxis=-phiAxis;
      tanAlpha1=-tanAlpha1;
      tanAlpha2=-tanAlpha2;
      G4double tmp;
      tmp=xHalfLength1;
      xHalfLength1=xHalfLength2;
      xHalfLength2=tmp;
      tmp=xHalfLength3;
      xHalfLength3=xHalfLength4;
      xHalfLength4=tmp;
    }
  if(fVerboseLevel>5)
    G4cout << "The size of No." << copyNo << " crystal(placed) are:" << G4endl
	   << "zHalfLength =" << zHalfLength/cm  << "(cm), "     << G4endl
	   << "thetaAxis   =" << thetaAxis/deg   << "(degree), " << G4endl
	   << "phiAxis     =" << phiAxis/deg     << "(degree), " << G4endl
	   << "yHalfLength1=" << yHalfLength1/cm << "(cm), "     << G4endl
	   << "xHalfLength1=" << xHalfLength1/cm << "(cm), "     << G4endl
	   << "xHalfLength2=" << xHalfLength2/cm << "(cm), "     << G4endl
	   << "tanAlpha1   =" << tanAlpha1       << "(), "       << G4endl
	   << "yHalfLength2=" << yHalfLength2/cm << "(cm), "     << G4endl
	   << "xHalfLength3=" << xHalfLength3/cm << "(cm), "     << G4endl
	   << "xHalfLength4=" << xHalfLength4/cm << "(cm)."      << G4endl
	   << "tanAlpha2   =" << tanAlpha2       << "(), "       << G4endl
	   << "(x4-x3)*y1/(x2-x1)/y2="
	   << (xHalfLength4-xHalfLength3)*yHalfLength1/(xHalfLength2-xHalfLength1)/yHalfLength2 << G4endl
	   << "tanAlpha2/tanAlpha1=" << tanAlpha2/tanAlpha1 <<G4endl;
  //G4cout << fStartID << "-->This is an output sentence for debug." << G4endl;

  trackerChamber.SetAllParameters(
				  zHalfLength,
				  thetaAxis,
				  phiAxis,
				  yHalfLength1,
				  xHalfLength1,
				  xHalfLength2,
				  tanAlpha1,
				  yHalfLength2,
				  xHalfLength3,
				  xHalfLength4,
				  tanAlpha2);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VSolid* BesCrystalParameterisation::ComputeSolid(const G4int copyNo,
                                       G4VPhysicalVolume *pPhysicalVol) 
{
  if(0) G4cout<<copyNo<<G4endl;
  ComputeIDAndSide(pPhysicalVol);
  return pPhysicalVol->GetLogicalVolume()->GetSolid();
}

G4Material* BesCrystalParameterisation::ComputeMaterial(const G4int copyNo,
    G4VPhysicalVolume *pPhysicalVol, const G4VTouchable *parentTouch) 
{
  if(0) G4cout<<copyNo<<parentTouch<<G4endl;
  ComputeIDAndSide(pPhysicalVol);
  G4int nBSCTheta=fBesEmcGeometry->GetBSCNbTheta();
  //if(fVerboseLevel>5)
  //G4cout <<"******BesCrystalParameterisation::ComputeMaterial******"<<G4endl
  //   <<"Number of all crystals=" << nBSCTheta << G4endl
  //   <<"No of current crystal =" << pPhysicalVol->GetCopyNo() << G4endl
  //   <<"*******************************************************"<<G4endl;
  if(pPhysicalVol->GetCopyNo()>2*nBSCTheta)
    return BesEmcConstruction::GetBesEmcConstruction()->GetCasingMaterial();
  else
    return BesEmcConstruction::GetBesEmcConstruction()->GetCrystalMaterial();
}

void BesCrystalParameterisation::ComputeIDAndSide(
					const G4VPhysicalVolume* pPhyVol)const
{
  //for debug
  //G4Exception("BesCrystalParameterisation::ComputeIDAndSide() starting......");
  G4int* pCrystalID=const_cast<G4int*>(&fCrystalID);
  G4bool* pFlagLeft=const_cast<G4bool*>(&fFlagLeft);
  G4int* pVerboseLevel=const_cast<G4int*>(&fVerboseLevel);
  
  *pVerboseLevel=
    BesEmcConstruction::GetBesEmcConstruction()->GetVerboseLevel();

  //*pCrystalID=pPhyVol->GetMotherPhysical()->GetCopyNo(); 
  *pCrystalID=-1;
  for(G4int i=fStartID;i<=fAllCrystals;i++)
    {
      //G4cout << fBesEmcGeometry->GetPhysiBSCCrystal(i) << ".vs."
      //     << pPhyVol << G4endl;
      if(fBesEmcGeometry->GetPhysiBSCCrystal(i)==pPhyVol)
	{
	  *pCrystalID=i;
	}
    }
  if(*pCrystalID==-1)
    G4Exception("The point of PhysicCrystal error!!!!!!!!!!!");
 
  //*pCrystalID=pPhyVol->GetCopyNo();
  //if(fVerboseLevel>6)
  //G4cout<<"******BesCrystalParameterisation::ComputeIDAndSide******"<<G4endl
  //  <<"point of pPhyVol =" << pPhyVol << G4endl
  //  <<"point of mother  =" << pPhyVol->GetMother() << G4endl
  //  <<"CopyNo of pPhyVol=" << pPhyVol->GetCopyNo() << G4endl
  //  <<"CopyNo of mother =" << pPhyVol->GetMother()->GetCopyNo() << G4endl
  //  <<"********************************************************"<<G4endl;
  
  if(fCrystalID>fAllCrystals/2)
    {
      *pFlagLeft=false;
      *pCrystalID=fCrystalID-fAllCrystals/2;
    }
  else
    {
      *pFlagLeft=true;
      *pCrystalID=fAllCrystals/2-fCrystalID+1;
    }
  *pCrystalID=*pCrystalID-1;
}
