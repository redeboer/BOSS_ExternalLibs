//
//
//
//

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include "BesMucEfficiency.hh"
#include "G4Box.hh"
#include "G4VSolid.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include "TFile.h"
#include "TTree.h"


using namespace std;
BesMucEfficiency * BesMucEfficiency::fPointer=0;
BesMucEfficiency * BesMucEfficiency::Instance(void){
  if(!fPointer)fPointer = new BesMucEfficiency();
  return fPointer;

}

BesMucEfficiency::BesMucEfficiency()
{
  if(fPointer)
  {G4Exception("BesMucEfficiency constructed twice.");}
  fPointer=this;

}

BesMucEfficiency::~BesMucEfficiency()
{
}


void BesMucEfficiency::Initialize(G4String filename)
{ G4int part,seg,gap,strip,pad;
 G4double effi;
//  for(G4int part=0;part<3;part++){
//    for(G4int seg=0;seg<8;seg++){ 
//      for(G4int gap=0;gap<gap_Max;gap++){
//	for(G4int strip=0;strip<strip_Max;strip++){
//	  for(G4int pad=0;pad<pad_Max;pad++){
//            m_effi[part][seg][gap][strip][pad]=1;
//	  }
//	}
  //    }
  //  }
  //}
 // G4cout<<"in BesMucEfficiency::Initialize()"<<G4endl;

 // TFile *f=new TFile("muc-effi.root");
 // TTree *t1=(TTree*)f->Get("t1");


    std::ifstream fin(filename);

     char buffer[100];
     G4int num=0;

     fin.getline(buffer,100,'\n');  //get info whether add effi or not
     std::istringstream stringBuf(buffer);
     stringBuf>>IsAddEffi;
     //G4cout<<"IsAddEffi ="<<IsAddEffi<<G4endl;
     fin.getline(buffer,100,'\n');

    if(!fin){
     G4cout<<"error opening effi data"<<G4endl;	
     IsAddEffi = 1.0;   // no effi data. set effi = 1.0
     }

//      fin.getline(buffer,100,'\n');
//      std::istringstream stringBuf2(buffer);
//      stringBuf2>>part>>seg>>gap>>strip>>pad;
//      G4cout<<"---------- "<<pad<<endl;

     while(fin.getline(buffer,100,'\n')){
       std::istringstream stringBuf2(buffer);
       stringBuf2>>part>>seg>>gap>>strip>>pad>>effi;
       m_effi[part][seg][gap][strip][pad] = effi;
       num++;
    }
    for(G4int seg=0;seg<8;seg++){
      for(G4int strip=0;strip<strip_Max;strip++){
            m_effi[1][seg][0][strip][105]=0;
     }
   }

    //G4cout<<"------------in Effi::init()----  "<<num<<G4endl;
}

void BesMucEfficiency::CheckCalibSvc()
{
 
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  //IMucCalibConstSvc* m_ptrCalibSvc;
  StatusCode sc = svcLocator->service("MucCalibConstSvc", m_ptrCalibSvc, true);
 
  if( sc != StatusCode::SUCCESS){
    G4cout<< "Can not use MucCalibConstSvc!" << G4endl;
  }
 
}

void BesMucEfficiency::SetHit(BesMucHit* hit)
{
  m_pHit = hit;
  G4int part = m_pHit->GetPart();
  G4int gap  = m_pHit->GetGap();
  m_Pos_Hit  = m_pHit->GetPosLocal().y();     //different from BesMucdigit
  if ( (part == 1 && gap%2 != 0) || (part != 1 && gap%2 == 0) ) {
    m_Pos_Hit = m_pHit->GetPosLocal().x();
  }
  
  //set m_Strip, m_Pos_Strip, m_Length, m_Width
  BesMucDigit aDigit;
  aDigit.SetHit(m_pHit);
  m_Strip = aDigit.GetNearestStripNo();
  //  G4VPhysicalVolume* pvGasChamber = m_pHit->GetVolume();
  GetPosLengthWidth(aDigit.GetNearestStrip());

//  G4cout<<"m_Pos_Hit = "<<m_Pos_Hit<<G4endl;
}

G4int BesMucEfficiency::GetPad()
{//it will be better to put this function into Class BesMucDigit
  G4double pad1 = (m_Pos_Hit+m_Length/2-m_Pos_Strip)/m_Width;
  G4int pad =G4int(pad1);
  //G4cout<<"---in Effi::GetPad()--- hit: "<<m_Pos_Hit<<" part: "<<m_pHit->GetPart()<<" gap: "<<m_pHit->GetGap()<<"  L: "<<m_Length<<"  strip: "<<m_Pos_Strip<<"  width: "<<m_Width<<" pad: "<<pad<<G4endl;
  if(abs(m_Pos_Hit-m_Pos_Strip)<m_Length/2)
    return pad;
  else
    return -999;

}

G4double BesMucEfficiency::GetEfficiency()
{
   // look up table with (part;seg;gap;m_Strip) 
  G4int part = m_pHit->GetPart();
  G4int seg  = m_pHit->GetSeg();
  G4int gap  = m_pHit->GetGap(); 
  G4int strip  = m_Strip;
  G4int pad  = GetPad();

  G4double eff = 0;
  if( 0 != m_ptrCalibSvc ){
  	eff = m_ptrCalibSvc->getEff(part, seg, gap, strip);
		//G4cout << "Prt: " << part << "\tseg: " << seg << "\tlay: " << gap << "\tstr: " << m_Strip 
		//			 << "\t:" << eff << endl;
  }	
  else
	{	
		//G4cout << "CalibSvc unavailable!" << G4endl;			
		eff = 0.95;
	}	
  //G4cout<<part<<"\t"<<seg<<"\t"<<gap<<"\t"<<m_Strip<<"\t"<<eff<<G4endl;
  return eff;  
}

void BesMucEfficiency::GetPosLengthWidth(G4VPhysicalVolume* pvStrip)
{
 m_Pos_Strip = 1.0e38;

  G4int part = m_pHit->GetPart();
  G4int gap  = m_pHit->GetGap();

  m_Pos_Strip = pvStrip->GetObjectTranslation().y();
  if ( (part == 1 && gap%2 != 0) || (part != 1 && gap%2 == 0) ) {
    m_Pos_Strip = pvStrip->GetObjectTranslation().x();
  }

  G4String striptype= pvStrip->GetLogicalVolume()->GetSolid()->GetEntityType();
//  G4String striplenght= pvStrip->GetLogicalVolume()->GetName();
  G4Box *temp;
  temp = (G4Box *)pvStrip->GetLogicalVolume()->GetSolid();
  m_Width = temp->GetXHalfLength()*2;
  m_Length = temp->GetYHalfLength()*2;
  if ( (part == 1 && gap%2 != 0) || (part != 1 && gap%2 == 0) ) {
     m_Width =temp->GetYHalfLength()*2;
     m_Length=temp->GetXHalfLength()*2;
  }
  //G4cout<<"in Set  "<<m_Length<<" "<<temp->GetXHalfLength()<<" "<<m_Width<<" "<<m_Pos_Strip<<G4endl;

}
