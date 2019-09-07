//
//   EmcRecROOTGeo
//
//   May 15, 2007, Created by Miao He
//
//   Construct ROOT geometry from gdml
//

#include "EmcRecGeoSvc/EmcRecROOTGeo.h"
#include "EmcRecGeoSvc/EmcRecCrystal.h"
#include "ROOTGeo/EmcROOTGeo.h"
#include "CLHEP/Vector/Rotation.h"

#include <TGeoManager.h>
#include "TGeoArb8.h"

using namespace std;
using namespace CLHEP;

EmcRecROOTGeo::EmcRecROOTGeo()
{
  m_crystalMap.clear();
}

EmcRecROOTGeo::~EmcRecROOTGeo()
{
  m_crystalMap.clear();
}

void EmcRecROOTGeo::InitFromXML()
{
  if(!gGeoManager) gGeoManager = new TGeoManager("BesGeo", "Bes geometry");
    
  EmcROOTGeo *fEmcROOTGeo = new EmcROOTGeo();
  fEmcROOTGeo->SetPhysicalNode();

  //get physical node
  for (int part = 0; part < fEmcROOTGeo->GetPartNb(); part++) {
    for (int phi = 0; phi < fEmcROOTGeo->GetPhiNb(part); phi++) {
      for (int theta = 0; theta < fEmcROOTGeo->GetThetaNb(part); theta++) {
        TGeoPhysicalNode *crystalPhysicalNode 
          = fEmcROOTGeo->GetPhysicalCrystal(part,phi,theta);

        TGeoArb8 *arb = (TGeoArb8*)crystalPhysicalNode->GetShape();
        //TGeoTrap *arb = (TGeoTrap*)crystalPhysicalNode->GetShape();
        
        double *pRot;
        pRot = crystalPhysicalNode->GetMatrix()->GetRotationMatrix();
        Hep3Vector rotX(pRot[0], pRot[3], pRot[6]);
        Hep3Vector rotY(pRot[1], pRot[4], pRot[7]);
        Hep3Vector rotZ(pRot[2], pRot[5], pRot[8]);
        HepRotation rotateCrystalToGlobal(rotX, rotY, rotZ);
        //cout<<"\nrotation: "<<rotateCrystalToGlobal<<"\n"<<endl;

        Hep3Vector rotTX(pRot[0], pRot[1], pRot[2]);
        Hep3Vector rotTY(pRot[3], pRot[4], pRot[5]);
        Hep3Vector rotTZ(pRot[6], pRot[7], pRot[8]);
        HepRotation rotateGlobalToCrystal(rotTX, rotTY, rotTZ);
            
        double *pTrans;
        pTrans = crystalPhysicalNode->GetMatrix()->GetTranslation();
        Hep3Vector translateCrystalToGlobal(pTrans[0], pTrans[1], pTrans[2]);

        Hep3Vector fPnt[10];
        for(int i=0;i<8;i++) {
          if(i<4) {
            fPnt[i] = Hep3Vector(arb->GetVertices()[i*2],
                                 arb->GetVertices()[i*2+1],
                                 -arb->GetDz());
          } else {
            fPnt[i] = Hep3Vector(arb->GetVertices()[i*2],
                                 arb->GetVertices()[i*2+1],
                                 arb->GetDz());
          }
          
          fPnt[i] *= rotateCrystalToGlobal;
          fPnt[i] += translateCrystalToGlobal;
        }

        EmcRecCrystal aCrystal;
        aCrystal.Type(EmcRecCrystal::SixPlane());
        
        Hep3Vector tempVec;
        if(part==1) {
          tempVec = fPnt[3];
          fPnt[3] = fPnt[0];
          fPnt[0] = fPnt[1];
          fPnt[1] = fPnt[2];
          fPnt[2] = tempVec;
          tempVec = fPnt[7];
          fPnt[7] = fPnt[4];
          fPnt[4] = fPnt[5];
          fPnt[5] = fPnt[6];
          fPnt[6] = tempVec;

          for(int i=0;i<8;i++) {
            aCrystal.Set(i,fPnt[i]);
          }
          FillCrystalMap(aCrystal,part,theta,phi);
          
        } else {
          for(int i=0;i<8;i++) {
            if(i%2==0) {
              tempVec = fPnt[i];
              fPnt[i] = fPnt[i+1];
              fPnt[i+1] = tempVec;
            }
            aCrystal.Set(i,fPnt[i]);
          }
          
          int sector, nb;
          if(theta<30) {
            sector = phi;
            nb = theta;
          } else {
            sector = phi;
            aCrystal.Type(EmcRecCrystal::SevenPlane());
            if(theta>=30&&theta<32) {
              nb = theta-25;
            } else {
              nb = theta-18;
            }
            int newTheta, newPhi;
            ComputeThetaPhi(part,nb,sector,newTheta,newPhi);
            Identifier id = EmcID::crystal_id(part,newTheta,newPhi);
            EmcRecCrystal tempCrystal = m_crystalMap[id];
              
            aCrystal.Set(0,tempCrystal.Get(0));
            aCrystal.Set(1,fPnt[0]);
            aCrystal.Set(2,fPnt[1]);
            aCrystal.Set(3,tempCrystal.Get(2));
            aCrystal.Set(4,tempCrystal.Get(3));
            aCrystal.Set(5,tempCrystal.Get(4));
            aCrystal.Set(6,fPnt[4]);
            aCrystal.Set(7,fPnt[5]);
            aCrystal.Set(8,tempCrystal.Get(6));
            aCrystal.Set(9,tempCrystal.Get(7));
          }

          FillCrystalMap(aCrystal,part,nb,sector);
        }
      } //theta
    } //phi
  } //part

  delete fEmcROOTGeo;
}

void EmcRecROOTGeo::FillCrystalMap(EmcRecCrystal& aCrystal,
    const int part, const int theta, const int phi)
{
  Identifier id;
  if(part==1) {   //barrel
    id = EmcID::crystal_id(EmcID::getBARREL(),theta,phi);
    m_crystalMap[id]=aCrystal;
  } else {    //endcap
    int newTheta, newPhi;
    ComputeThetaPhi(part,theta,phi,newTheta,newPhi);
    id = EmcID::crystal_id(part,newTheta,newPhi);
    m_crystalMap[id]=aCrystal;
  }
}

EmcRecCrystal EmcRecROOTGeo::GetCrystal(const Identifier& id) const
{
  return m_crystalMap.find(id)->second;
}

HepPoint3D EmcRecROOTGeo::GetCCenter(const Identifier& id) const
{
  return GetCrystal(id).Center();
}

HepPoint3D EmcRecROOTGeo::GetCFrontCenter(const Identifier& id) const
{
  return GetCrystal(id).FrontCenter();
}

void EmcRecROOTGeo::ComputeThetaPhi(const int part, 
    const int theta, const int phi, int& newTheta, int& newPhi)
{
  int sector = phi;
  if((sector>=0)&&(sector<3))
    sector+=16;
  //if((sector!=7)&&(sector!=15))
  {
    if((theta>=0)&&(theta<4))
    {
      newTheta = 0;
      newPhi = (sector-3)*4+theta;
    }
    else if((theta>=4)&&(theta<8))
    {
      newTheta = 1;
      newPhi = (sector-3)*4+theta-4;
    }
    else if((theta>=8)&&(theta<13))
    {
      newTheta = 2;
      newPhi = (sector-3)*5+theta-8;
    }
    else if((theta>=13)&&(theta<18))
    {
      newTheta = 3;
      newPhi = (sector-3)*5+theta-13;
    }
    else if((theta>=18)&&(theta<24))
    {
      newTheta = 4;
      newPhi = (sector-3)*6+theta-18;
    }
    else if((theta>=24)&&(theta<30))
    {
      newTheta = 5;
      newPhi = (sector-3)*6+theta-24;
    }
  }
  
  if(part==2)
  {
    switch(newTheta)
    {
      case 0:
        if(newPhi<32)
          newPhi = 31-newPhi;
        else
          newPhi = 95-newPhi;
        break;
      case 1:
        if(newPhi<32)
          newPhi = 31-newPhi;
        else
          newPhi = 95-newPhi;
        break;
      case 2:
        if(newPhi<40)
          newPhi = 39-newPhi;
        else
          newPhi = 119-newPhi;
        break;
      case 3:
        if(newPhi<40)
          newPhi = 39-newPhi;
        else
          newPhi = 119-newPhi;
        break;
      case 4:
        if(newPhi<48)
          newPhi = 47-newPhi;
        else
          newPhi = 143-newPhi;
        break;
      case 5:
        if(newPhi<48)
          newPhi = 47-newPhi;
        else
          newPhi = 143-newPhi;
      default:
        ;
    }
  }

}
