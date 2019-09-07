//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Dec 15, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#include "BesEmcDigitization.hh"
#include "BesEmcHit.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"

BesEmcDigitization::BesEmcDigitization()
  :calorimeterCollID(-1),fNHits(0),fTotECrystal(0),fTotLCrystal(0),
   fNCrystalSignal(0)
{
  for(int i=0;i<50;i++)
    {for(int j=0;j<150;j++)
      fSingleECrystal[i][j]=0;
    }
  G4SDManager * SDman = G4SDManager::GetSDMpointer();
  calorimeterCollID= SDman->GetCollectionID("CalCollection");
}

BesEmcDigitization::~BesEmcDigitization()
{
  delete[]fEnergySignal;
  delete[]fNThetaSignal;
  delete[]fNPhiSignal;
}

G4bool BesEmcDigitization::Digitize(const G4Event* evt, G4double ecut)
{
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  BesEmcHitsCollection* CHC = 0;

  if (HCE)
    CHC = (BesEmcHitsCollection*)(HCE->GetHC(calorimeterCollID));
  else
    return false;

  G4int i=0,j=0;
  if (CHC)
    {
      fNHits = CHC->entries();
      if(fNHits)
	for (i=0;i<fNHits;i++)
	  {
	    fTotECrystal += (*CHC)[i]->GetEdepCrystal(); 
	    fTotLCrystal += (*CHC)[i]->GetTrakCrystal();
	    G4int nTheta=45-(*CHC)[i]->GetNumThetaCrystal();
	    G4int nPhi  =(*CHC)[i]->GetNumPhiCrystal();
	    if(nTheta>0&&nTheta<50&&nPhi>0&&nPhi<150)//waiting upgrade
	      {
		fSingleECrystal[nTheta][nPhi]+=(*CHC)[i]->GetEdepCrystal();
		//G4cout << "EMC Hit in crystal(" 
		//       << nTheta << "," <<nPhi<< ")" << G4endl;
	      }
	    else
	      {
		G4cout << "EMC Hit out range:"
		       << nTheta <<','<< nPhi << G4endl;
	      }
	  }
    }
  else
    return false;
  
  for(i=1;i<50;i++)
    {for(j=1;j<150;j++)
      {
	if(fSingleECrystal[i][j]>ecut)
	  {
	    fNCrystalSignal++;
	  }
      }
    }
  if((fEnergySignal=new G4double[fNCrystalSignal+1])==NULL||
     (fNThetaSignal=new G4int[fNCrystalSignal+1])==NULL||
     (fNPhiSignal=new G4int[fNCrystalSignal+1])==NULL)
    {
      G4Exception("BesEmcDigitization:can't allocate memory for Signal");
    }
  G4int n=0;
  for(i=1;i<50;i++)
    {for(j=1;j<150;j++)
      {
	if(fSingleECrystal[i][j]>ecut)
	  {
	    n++;
	    fEnergySignal[n]=fSingleECrystal[i][j];
	    fNThetaSignal[n]=45-i;
	    fNPhiSignal  [n]=j;
	  }
      }
    }
  
  return true;
}

G4double BesEmcDigitization::GetEmcSignal(G4int nTheta, G4int nPhi)
{
  if(nTheta>0&&nTheta<50&&nPhi>0&&nPhi<150)
    return fSingleECrystal[nTheta][nPhi];
  else
    return 0;
}

void BesEmcDigitization::Print()
{
  G4cout
    << "   Absorber: total energy: "
    << fTotECrystal/GeV << " GeV" << G4endl
    << "       total track length: "
    << fTotLCrystal/cm  << " cm"  << G4endl;
	  
  G4cout << "\n     " << fNHits
	 << " hits are stored in BesEmcHitsCollection." << G4endl;
}
