#include "G4ThreeVector.hh"

#include "BesEmcEndGeometry.hh"
#include "BesEmcParameter.hh"
#include "G4ios.hh"

#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>


using namespace std;

BesEmcEndGeometry::BesEmcEndGeometry()
{;}

BesEmcEndGeometry::~BesEmcEndGeometry()
{;}

void BesEmcEndGeometry::ReadParameters()
{
  BesEmcParameter& emcPara=BesEmcParameter::GetInstance();
  
  WorldRmin1 = emcPara.GetWorldRmin1();
  WorldRmax1 = emcPara.GetWorldRmax1();
  WorldRmin2 = emcPara.GetWorldRmin2();//+5.*mm; //add 5mm to avoid warning
  WorldRmax2 = emcPara.GetWorldRmax2();
  WorldDz = emcPara.GetWorldDz();
  WorldZPosition = emcPara.GetWorldZPosition();
  CrystalLength = emcPara.GetCrystalLength();

  SectorRmin1 = 489.27*mm;
  SectorRmax1 = 880.*mm;
  SectorRmin2 = 621.57*mm;
  SectorRmax2 = 1113.53*mm;
  SectorDz = WorldDz-44.*mm;
  SectorZPosition = -18.*mm;

  for(G4int i=0;i<6;i++)
    cryNumInOneLayer[i] = emcPara.GetCryInOneLayer(i);
  for(G4int i=0;i<5;i++)
    pentaInOneSector[i] = emcPara.GetPentaInOneSector(i);

  fTyvekThickness   = emcPara.GetTyvekThickness();
  fAlThickness      = emcPara.GetAlThickness();
  fMylarThickness   = emcPara.GetMylarThickness();
  totalThickness=fTyvekThickness+fAlThickness+fMylarThickness;
      
  G4String ParaPath = getenv("EMCSIMROOT");
  if(!ParaPath){
    G4Exception("BOOST environment not set!");
  }
  ParaPath += "/dat/EmcEndGeometry.dat";

  ifstream fin;
  fin.open(ParaPath);
  assert(fin);
  for(G4int i=0;i<30;i++)
    for (G4int j=0;j<24;j++)
      fin>>param[i][j];
  for(G4int i=0;i<5;i++)
    for (G4int j=0;j<6;j++)
      fin>>penta[i][j];
  fin.close();
}


void BesEmcEndGeometry::ComputeParameters()
{

   ////////////////////////////////////////////////////////////////////////
  // emc endcap crystal                                                   //
  //////////////////////////////////////////////////////////////////////////
  //                              1                                       //
  //                  0     __  --                                        //
  //                    --         \                                      //
  //                  | \           \                                     //  
  //                  |   \      __ - 2                                   //
  //                  |     \ --     |                                    //
  //                  |     | 3      |                                    //
  //                  |     |        |                                    //
  //                  |     |        |                                    //
  //                  |     |        |                                    //
  //                  |     |    5   |                                    //
  //                4  \    |        |                                    //
  //                     \  |    __ - 6                                   //
  //                       \| --                                          //
  //                         7                                            //
  /////////////////////////////////////////////////////////////////////////
  
  ReadParameters();

  G4int pentaNb=0;
  for(G4int i=0;i<30;i++)
  {
    for (G4int j=0;j<8;j++)
    {
      //use 24 parameters to construct 8 point of one crystal 
      G4ThreeVector* pPnt = new G4ThreeVector(param[i][j],param[i][j+8],param[i][j+16]-WorldZPosition-SectorZPosition);
      fPnt[i][j] = *pPnt;
    }

    if(i==5||i==6||i==14||i==15||i==16)
    //if(0)   //no pentagonal crystal now
    {
      for(G4int j=0;j<8;j++)
        fPnt[30+pentaNb][j] = fPnt[i][j];
      
      //compute the 5th point of the pentagonal crystal
      G4double y0,y1,y4,y5;
      G4ThreeVector v0,v1,v4,v5;
      y0 = penta[pentaNb][0];
      y1 = penta[pentaNb][1];
      y4 = penta[pentaNb][2];
      y5 = penta[pentaNb][3];
      v0 = (y0-fPnt[i][3].getY())*(fPnt[i][0]-fPnt[i][3])/(fPnt[i][0].getY()-fPnt[i][3].getY())
        +fPnt[i][3];
      v1 = (y1-fPnt[i][2].getY())*(fPnt[i][1]-fPnt[i][2])/(fPnt[i][1].getY()-fPnt[i][2].getY())
        +fPnt[i][2];
      v4 = (y4-fPnt[i][7].getY())*(fPnt[i][4]-fPnt[i][7])/(fPnt[i][4].getY()-fPnt[i][7].getY())
        +fPnt[i][7];
      v5 = (y5-fPnt[i][6].getY())*(fPnt[i][5]-fPnt[i][6])/(fPnt[i][5].getY()-fPnt[i][6].getY())
        +fPnt[i][6];

      G4double x1,x5;
      x1 = penta[pentaNb][4];
      x5 = penta[pentaNb][5];
      v1 = (x1-v0.getX())*(v1-v0)/(v1.getX()-v0.getX())+v0;   //v1', the fifth point
      v5 = (x5-v4.getX())*(v5-v4)/(v5.getX()-v4.getX())+v4;   //v5'
      
      fPnt[i][0] = v0;
      fPnt[i][1] = v1;
      fPnt[i][4] = v4;
      fPnt[i][5] = v5;
      
      fPnt[30+pentaNb][2] = v1;
      fPnt[30+pentaNb][3] = v0;
      fPnt[30+pentaNb][6] = v5;
      fPnt[30+pentaNb][7] = v4;

      pentaNb++;
    }
  }

  //reflect point in one sector according to new design
  G4ThreeVector temp[35][8];
  for(G4int i=0;i<35;i++)
  {
    for (G4int j=0;j<8;j++)
    {
      temp[i][j]=fPnt[i][j];
      fPnt[i][j].rotateZ(157.5*deg);
      fPnt[i][j].setX(-fPnt[i][j].getX());
    }

    // point 0<-->3, 1<-->2, 4<-->7, 5<-->6
    for (G4int j=0;j<8;j++)
    {
      if(j<2)
      {
        G4ThreeVector v=fPnt[i][j];
        fPnt[i][j]=fPnt[i][3-j];
        fPnt[i][3-j]=v;
      }
      else if(j>=4&&j<6)
      {
        G4ThreeVector v=fPnt[i][j];
        fPnt[i][j]=fPnt[i][11-j];
        fPnt[i][11-j]=v;
      }
    }
  }

  //exchange sequence in the same layer
  //Exchange(0,3);  Exchange(1,2);  Exchange(4,7);  Exchange(5,6);  
  //Exchange(8,12);  Exchange(9,11);  Exchange(13,17);  Exchange(14,16);
  //Exchange(18,23);  Exchange(19,22);  Exchange(20,21);  Exchange(24,29);  Exchange(25,28); Exchange(26,27);

  Exchange(0,3);  Exchange(1,2);  Exchange(4,7);  Exchange(5,31);  Exchange(6,30);
  Exchange(8,12);  Exchange(9,11);  Exchange(13,17);  Exchange(14,34);  Exchange(15,33);  Exchange(16,32); 
  Exchange(18,23);  Exchange(19,22);  Exchange(20,21);  Exchange(24,29);  Exchange(25,28); Exchange(26,27);

  /*for(G4int i=0;i<35;i++)
  {
    G4cout<<"crystal number: "<<i<<G4endl;
    for(G4int j=0;j<8;j++)
      G4cout<<fPnt[i][j]<<G4endl;
  }*/
    
                
  //compute the 6 crystal beside the 20mm gap
  for(G4int i=0;i<35;i++)
  {
    for (G4int j=0;j<8;j++)
    {
      G4ThreeVector pPnt1 = temp[i][j];
      fPnt1[i][j] = pPnt1.rotateZ(67.5*deg);
    }
    if((i==3)||(i==7)||(i==12)||(i==17)||(i==23)||(i==29))
    {
      fPnt1[i][0].setX(10);
      fPnt1[i][1].setX(10);
      fPnt1[i][4].setX(10);
      fPnt1[i][5].setX(10);

      G4double y0 = fPnt1[i][0].getY()+10*(fPnt1[i][3].getY()-fPnt1[i][0].getY())/fPnt1[i][3].getX();
      G4double y1 = fPnt1[i][1].getY()+10*(fPnt1[i][2].getY()-fPnt1[i][1].getY())/fPnt1[i][2].getX();
      G4double y4 = fPnt1[i][4].getY()+10*(fPnt1[i][7].getY()-fPnt1[i][4].getY())/fPnt1[i][7].getX();
      G4double y5 = fPnt1[i][5].getY()+10*(fPnt1[i][6].getY()-fPnt1[i][5].getY())/fPnt1[i][6].getX();

      G4double z0 = fPnt1[i][0].getZ()+10*(fPnt1[i][3].getZ()-fPnt1[i][0].getZ())/fPnt1[i][3].getX();
      G4double z1 = fPnt1[i][1].getZ()+10*(fPnt1[i][2].getZ()-fPnt1[i][1].getZ())/fPnt1[i][2].getX();
      G4double z4 = fPnt1[i][4].getZ()+10*(fPnt1[i][7].getZ()-fPnt1[i][4].getZ())/fPnt1[i][7].getX();
      G4double z5 = fPnt1[i][5].getZ()+10*(fPnt1[i][6].getZ()-fPnt1[i][5].getZ())/fPnt1[i][6].getX();

      fPnt1[i][0].setY(y0);
      fPnt1[i][1].setY(y1);
      fPnt1[i][4].setY(y4);
      fPnt1[i][5].setY(y5);

      fPnt1[i][0].setZ(z0);
      fPnt1[i][1].setZ(z1);
      fPnt1[i][4].setZ(z4);
      fPnt1[i][5].setZ(z5);
    }
  }
}

void BesEmcEndGeometry::Exchange(G4int cry1, G4int cry2)
{
  G4ThreeVector v;
  for(G4int i=0;i<8;i++)
  {
    v = fPnt[cry1][i];
    fPnt[cry1][i] = fPnt[cry2][i];
    fPnt[cry2][i] = v;
  }
}
  
void BesEmcEndGeometry::ExchangeSector7(G4int cry1, G4int cry2)
{
  G4ThreeVector v;
  for(G4int i=0;i<8;i++)
  {
    v = fPnt1[cry1][i];
    fPnt1[cry1][i] = fPnt1[cry2][i];
    fPnt1[cry2][i] = v;
  }
}

//reflect x axis to construct sectors on the left
void BesEmcEndGeometry::ReflectX()
{
  for(G4int i=0;i<35;i++)
  {
    for(G4int j=0;j<8;j++)
    {
      fPnt1[i][j].setX(-fPnt1[i][j].getX());
    }

    // point 0<-->3, 1<-->2, 4<-->7, 5<-->6
    for (G4int j=0;j<8;j++)
    {
      if(j<2)
      {
        G4ThreeVector v=fPnt1[i][j];
        fPnt1[i][j]=fPnt1[i][3-j];
        fPnt1[i][3-j]=v;
      }
      else if(j>=4&&j<6)
      {
        G4ThreeVector v=fPnt1[i][j];
        fPnt1[i][j]=fPnt1[i][11-j];
        fPnt1[i][11-j]=v;
      }
    }
    
    //change the sequence of eight points according to the requirment of IrregBox
    // point 0<-->1, 2<-->3, 4<-->5, 6<-->7
    /*for(G4int j=0;j<8;j++)
    {
      if((j%2)==0)
      {
        G4ThreeVector v2=fPnt1[i][j];
        fPnt1[i][j]=fPnt1[i][j+1];
        fPnt1[i][j+1]=v2;
      }
    }*/
  }
  ExchangeSector7(0,3);  ExchangeSector7(1,2);  ExchangeSector7(4,7);  
  ExchangeSector7(5,31); ExchangeSector7(6,30); ExchangeSector7(8,12);  
  ExchangeSector7(9,11);  ExchangeSector7(13,17); ExchangeSector7(14,34);  
  ExchangeSector7(15,33);  ExchangeSector7(16,32); ExchangeSector7(18,23);  
  ExchangeSector7(19,22);  ExchangeSector7(20,21);  ExchangeSector7(24,29);  
  ExchangeSector7(25,28); ExchangeSector7(26,27);
}  

void BesEmcEndGeometry::Zoom(const G4ThreeVector pos[8], const G4double factor)
{
  G4ThreeVector center1(0,0,0);
  G4ThreeVector center2(0,0,0);
  for(G4int i=0;i<8;i++)
    zoomPoint[i]=0;
  
  for(G4int i=0;i<8;i++)
  {
    if(i<4) center1+=pos[i];
    else center2+=pos[i];
  }
  center1/=4;
  center2/=4;

  for(G4int i=0;i<8;i++)
  {
    if(i<4) zoomPoint[i]=factor*pos[i]+(1-factor)*center1;
    else zoomPoint[i]=factor*pos[i]+(1-factor)*center2;
  }
}
  
//subtract the casing      
void BesEmcEndGeometry::ModifyForCasing(G4ThreeVector pos[8], G4int CryNb)
{
  G4ThreeVector center1=0;        //the center of large surface
  G4ThreeVector center2=0;        //the center of small surface
  
  const G4double dt=1e-5; //avoid overlap between crystal and casing
  
  if(CryNb==5||CryNb==6||CryNb==14||CryNb==15||CryNb==16)
  {
    center1 = (pos[0]+pos[1])*(1-dt)/2+(pos[2]+pos[3])*dt/2;
    center2 = (pos[4]+pos[5])*(1-dt)/2+(pos[6]+pos[7])*dt/2;
  }
  else if(CryNb>=30&&CryNb<35)
  {
    center1 = (pos[0]+pos[1])*dt/2+(pos[2]+pos[3])*(1-dt)/2;
    center2 = (pos[4]+pos[5])*dt/2+(pos[6]+pos[7])*(1-dt)/2;
  }
  else
  {
    center1 = (pos[0]+pos[1]+pos[2]+pos[3])/4;
    center2 = (pos[4]+pos[5]+pos[6]+pos[7])/4;
  }
  
  G4double r1=(pos[1]-center1).r();
  G4double r2=(pos[2]-center1).r();
  G4double r12=(pos[1]-pos[2]).r();
  G4double theta=acos((r2*r2+r12*r12-r1*r1)/(2*r2*r12));
  G4double h=r2*sin(theta);       //distance from the center to the vertical side
  G4double t1=totalThickness/h;

  r1=(pos[5]-center2).r();
  r2=(pos[6]-center2).r();
  r12=(pos[5]-pos[6]).r();
  theta=acos((r2*r2+r12*r12-r1*r1)/(2*r2*r12));
  h=r2*sin(theta);
  G4double t2=totalThickness/h;
  
  for(G4int i=0;i<8;i++)
  {
    if(i<4)
    {
      cryPoint[i] = (1-t1)*pos[i]+t1*center1;
    }
    else
    {
      G4ThreeVector temp = (1-t2)*pos[i]+t2*center2;
      cryPoint[i] = (1-totalThickness/CrystalLength)*temp+(totalThickness/CrystalLength)*pos[i-4];
    }
  //G4cout<<"casing="<<pos[i]<<"\t"<<"crystal="<<cryPoint[i]<<G4endl;
  }
}

G4ThreeVector BesEmcEndGeometry::ComputeDimAndPos
(const G4int partId, const G4int numTheta, const G4int numPhi)
{
  //ComputeParameters();
  G4int sector=-1, cryNb=-1;
  G4int leftFlag=0;
  G4int downFlag=0;
  G4int pentaFlag=0;
  G4int flag=0;
  G4double A1=0,a1=0,B1=0,b1=0,C1=0,c1=0,D1=0,d1=0,E1=0,e1=0; //dimension of pentagonal crystal
  G4int m_numPhi=0;
  G4ThreeVector position=0;
  G4int cryInOneSector = cryNumInOneLayer[numTheta]/16; //number of crystal in one layer in one sector
  G4ThreeVector pos[8];

  if(partId==2) //west end
  {
    if(numPhi>=0&&numPhi<8*cryInOneSector)
      m_numPhi=8*cryInOneSector-1-numPhi;
    else if(numPhi>=8*cryInOneSector&&numPhi<16*cryInOneSector)
      m_numPhi=16*cryInOneSector-1-numPhi;
  }
  else          //east end
    m_numPhi=numPhi;

  if(numPhi>=4*cryInOneSector&&numPhi<5*cryInOneSector) //crystal in 4th sector
  {
    leftFlag = 1;
    m_numPhi=8*cryInOneSector-1-numPhi;
  }
  else if(numPhi>=11*cryInOneSector&&numPhi<12*cryInOneSector) //crystal in 11th sector
  {
    leftFlag = 1;
    downFlag = 1;
    m_numPhi=numPhi-8*cryInOneSector;
  }
  if(numPhi>=12*cryInOneSector&&numPhi<13*cryInOneSector) //crystal in 12th sector
  {
    downFlag = 1;
    m_numPhi=16*cryInOneSector-1-numPhi;
  }
  
  //translate numTheta and numPhi to sector and cryNb
  G4int cryNbOffset = 0;
  for(G4int i=0;i<numTheta;i++)
    cryNbOffset += cryNumInOneLayer[i]/16;
  if(cryInOneSector)
    sector = m_numPhi/cryInOneSector;
  cryNb = m_numPhi-cryInOneSector*sector+cryNbOffset;
  sector += 3;
  if(sector>15&&sector<=18)
    sector -= 16;
    
  //sector beside the gap
  if(sector==6)
    for(G4int i=0;i<8;i++)
      pos[i]=fPnt1[cryNb][i];
  else
    for(G4int i=0;i<8;i++)
    {
      pos[i]=fPnt[cryNb][i];
      pos[i].rotateZ(-67.5*deg+sector*22.5*deg);
    }

  //crystal dimension
  Zoom(pos,0.999);
  ModifyForCasing(zoomPoint,cryNb);
  G4double A = (cryPoint[0]-cryPoint[3]).r();
  G4double a = (cryPoint[4]-cryPoint[7]).r();
  G4double B = (cryPoint[1]-cryPoint[2]).r();
  G4double b = (cryPoint[5]-cryPoint[6]).r();
  G4double C = (cryPoint[0]-cryPoint[1]).r();
  G4double c = (cryPoint[4]-cryPoint[5]).r();
  G4double D = (cryPoint[2]-cryPoint[3]).r();
  G4double d = (cryPoint[6]-cryPoint[7]).r();

  //reflect the axis according to the flag
  for(G4int i=0;i<8;i++)
  {
    pos[i].setZ(pos[i].getZ()+WorldZPosition+SectorZPosition);  //give the absolute z coordinate
    if(leftFlag==1)
       pos[i].setX(-pos[i].getX());
    if(downFlag==1)
      pos[i].setY(-pos[i].getY());
    if(partId==2)
    {
      pos[i].setX(-pos[i].getX());
      pos[i].setZ(-pos[i].getZ());
    }
  }
  
  //compute the position
  for(G4int j=4;j<8;j++)
    position += pos[j];
  position /= 4;

  //compute pentagonal crystal
  for(G4int i=0;i<5;i++)
  {
    if(cryNb==pentaInOneSector[i])
    {
      pentaFlag = 1;
      G4ThreeVector penta[8];
      
      //sector beside the gap
      if(sector==6)
        for(G4int j=0;j<8;j++)
          penta[j]=fPnt1[30+i][j];
      else
        for(G4int j=0;j<8;j++)
        {
          penta[j]=fPnt[30+i][j];
          penta[j].rotateZ(-67.5*deg+sector*22.5*deg);
        }
      
      //crystal dimension
      ModifyForCasing(penta,30+i);
      A1 = (cryPoint[1]-cryPoint[2]).r();
      a1 = (cryPoint[5]-cryPoint[6]).r();
      B1 = (cryPoint[1]-cryPoint[0]).r();
      b1 = (cryPoint[5]-cryPoint[4]).r();
      C1 = (cryPoint[0]-cryPoint[3]).r()+A;
      c1 = (cryPoint[4]-cryPoint[7]).r()+a;
      D1 = D;
      d1 = d;
      E1 = B;
      e1 = b;

      //reflect the axis according to the flag
      for(G4int j=0;j<8;j++)
      {
        penta[j].setZ(penta[j].getZ()+WorldZPosition+SectorZPosition);  //give the absolute z coordinate
        if(leftFlag==1)
          penta[j].setX(-penta[j].getX());
        if(downFlag==1)
          penta[j].setY(-penta[j].getY());
        if(partId==2)
        {
          penta[j].setX(-penta[j].getX());
          penta[j].setZ(-penta[j].getZ());
        }
      }
      
      //compute the position
      for(G4int j=4;j<8;j++)
      {
        if(j!=0&&j!=4)
          position += pos[j];
        if(j==0||j==1||j==4||j==5)
          position += penta[j];
      }
      position /= 10;
      
      flag = leftFlag+downFlag;
      if(flag==1)
      {
        G4double temp1 = B1; B1 = D1; D1 = temp1;
        temp1 = A1; A1 = E1; E1 = temp1;
        temp1 = b1; b1 = d1; d1 = temp1;
        temp1 = a1; a1 = e1; e1 = temp1;
      }

      break;
    }
  }
              
  flag = leftFlag+downFlag+partId/2;
  if(flag==1||flag==3)
  {
    G4double temp = C;    C = D;    D = temp;
    temp = c;    c = d;    d = temp;
  }

  /*G4cout<<"##########################################################################"<<G4endl;
  G4cout<<"###sector="<<sector<<",cryNb="<<cryNb<<",left flag="<<leftFlag<<",down flag="<<downFlag<<G4endl;
  G4cout<<"partId="<<partId<<"\t"<<"theta number="<<numTheta<<"\t"<<"phi number="<<numPhi<<G4endl;
  G4cout<<"crystal point:"<<G4endl;
  for(G4int i=0;i<8;i++)
    G4cout<<pos[i]<<G4endl;
  G4cout<<"crystal position:"<<"\t"<<position<<"\t"<<"phi="<<position.phi()/deg<<G4endl;
  G4cout<<"crystal dimension:"<<G4endl;
  if(pentaFlag==1)
    G4cout<<"A="<<A1<<",a="<<a1<<",B="<<B1<<",b="<<b1<<",C="<<C1<<",c="<<c1<<",D="<<D1<<",d="<<d1<<",E="<<E1<<",e="<<e1<<G4endl;
  else
    G4cout<<"A="<<A<<",a="<<a<<",B="<<B<<",b="<<b<<",C="<<C<<",c="<<c<<",D="<<D<<",d="<<d<<G4endl;
  G4cout<<"##########################################################################"<<G4endl;*/

  return position;
}
