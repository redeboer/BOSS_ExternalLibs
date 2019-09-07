//---------------------------------------------------------------------------//
//      BESIII Object_Oreiented Simulation and Reconstruction Tool           //
//---------------------------------------------------------------------------//
//Descpirtion: Geometry of EMC detector 
//Author: Fu Chengdong
//Created: Oct 23, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#include "G4ThreeVector.hh"

#include "BesEmcGeometry.hh"
#include "BesEmcParameter.hh"
#include "G4ios.hh"

BesEmcGeometry::BesEmcGeometry()
{
  verboseLevel = 0;
}

BesEmcGeometry::~BesEmcGeometry()
{;}

void BesEmcGeometry::ReadEMCParameters()
{
  // Read EMC parameters from database
  BesEmcParameter& emcPara=BesEmcParameter::GetInstance();
    
  TaperRingDz = emcPara.GetTaperRingDz();
  TaperRingThickness1 = emcPara.GetTaperRingThickness1();
  TaperRingThickness2 = emcPara.GetTaperRingThickness2();
  TaperRingThickness3 = emcPara.GetTaperRingThickness3();
  TaperRingTheta = emcPara.GetTaperRingTheta()*deg;
  TaperRingInnerLength = emcPara.GetTaperRingInnerLength();
  TaperRingOuterLength = emcPara.GetTaperRingOuterLength();
 
  BSCRmin           = emcPara.GetBSCRmin();
  BSCDz             = emcPara.GetBSCDz()-TaperRingThickness3;

  BSCRmin1          = emcPara.GetBSCRmin1();
  BSCRmax1          = emcPara.GetBSCRmax1();
  BSCRmin2          = emcPara.GetBSCRmin2();
  //BSCRmax2          = emcPara.GetBSCRmax2();
  BSCDz1            = emcPara.GetBSCDz1()-TaperRingThickness3;
  
  BSCAngleRotat     = emcPara.GetBSCAngleRotat()*deg;
  BSCNbPhi          = emcPara.GetBSCNbPhi();
  BSCNbTheta        = emcPara.GetBSCNbTheta();
  
  BSCCryLength      = emcPara.GetCrystalLength();
  BSCCryLength1     = emcPara.GetCrystalLength1();
  BSCYFront0        = emcPara.GetBSCYFront0();
  BSCYFront         = emcPara.GetBSCYFront();
  BSCYFront1        = emcPara.GetBSCYFront1();
  BSCPosition0      = emcPara.GetBSCPosition0();
  BSCPosition1      = emcPara.GetBSCPosition1();

  fTyvekThickness   = emcPara.GetTyvekThickness();
  fAlThickness      = emcPara.GetAlThickness();
  fMylarThickness   = emcPara.GetMylarThickness();
  
  rearBoxLength     = emcPara.GetRearBoxLength();
  rearBoxDz         = emcPara.GetRearBoxDz();
  
  HangingPlateDz    = emcPara.GetHangingPlateDz();
  OCGirderAngle     = emcPara.GetOCGirderAngle()*deg;

  rearCasingThickness = emcPara.GetRearCasingThickness();

  orgGlassLengthX   = emcPara.GetOrgGlassLengthX();
  orgGlassLengthY   = emcPara.GetOrgGlassLengthY();
  orgGlassLengthZ   = emcPara.GetOrgGlassLengthZ();

  PDLengthX         = emcPara.GetPDLengthX();
  PDLengthY         = emcPara.GetPDLengthY();
  PDLengthZ         = emcPara.GetPDLengthZ();

  AlPlateDz         = emcPara.GetAlPlateDz();
  PABoxDz           = emcPara.GetPABoxDz();
  PABoxThickness    = emcPara.GetPABoxThickness();

  cableDr           = emcPara.GetCableDr();
  waterPipeDr       = emcPara.GetWaterPipeDr();
  waterPipeThickness= emcPara.GetWaterPipeThickness();

  SPBarThickness    = emcPara.GetSPBarThickness();
  SPBarThickness1   = emcPara.GetSPBarThickness1();
  SPBarwidth        = emcPara.GetSPBarwidth();

  EndRingDz         = emcPara.GetEndRingDz();
  EndRingDr         = emcPara.GetEndRingDr();
  EndRingRmin       = emcPara.GetEndRingRmin();
  
  for(G4int i=0;i<BSCNbTheta*6;i++)
  {
    zHalfLength[i]  =0;
    thetaAxis[i]    =0;
    phiAxis  [i]    =0;
    yHalfLength1[i] =0; 
    xHalfLength1[i] =0;
    xHalfLength2[i] =0;
    tanAlpha1[i]    =0;
    yHalfLength2[i] =0;
    xHalfLength3[i] =0; 
    xHalfLength4[i] =0;
    tanAlpha2[i]    =0;
    thetaPosition[i]=0;
    xPosition[i]    =0;
    yPosition[i]    =0;
    zPosition[i]    =0;
  }     
}

void BesEmcGeometry::PrintEMCParameters()
{
  // Print EMC parameters
  ;
}

void BesEmcGeometry::ComputeEMCParameters()
{
  ReadEMCParameters();
  
  // Compute derived parameters of the calorimeter
  G4double theta0;
  G4int i;
  //for debug
  //G4Exception("BesEmcGeometry::ComputeEMCParameters() starting.......");
  //
  //support frame
  //
  const G4double delta=1.*mm; //for opening-cut girder
  TaperRingRmin1 = BSCRmax1-TaperRingThickness1;
  TaperRingRmin2 = TaperRingRmin1+TaperRingDz*tan(TaperRingTheta);
  TaperRingDr = TaperRingThickness2/cos(TaperRingTheta);
  TaperRingOuterLength1 = TaperRingOuterLength+TaperRingThickness3*tan(TaperRingTheta);
  
  BSCRmax2 = TaperRingRmin2+TaperRingDr-TaperRingThickness3*tan(TaperRingTheta);
  BSCRmax = BSCRmin+33.8*cm;
  BSCPhiDphi= 360.*deg/BSCNbPhi;
  BSCPhiSphi= -BSCPhiDphi/2.;
  BSCPhiDz  = BSCDz;
  BSCPhiRmax= BSCRmax-0.5*cm;
  BSCPhiRmin= BSCRmin/sin(90.*deg+BSCPhiDphi/2.)
    *sin(90.*deg+BSCPhiDphi/2.-BSCAngleRotat); //Rmin after rotate
  SPBarDphi=2*atan(SPBarwidth/(2*BSCRmax));

  //crystal No.1(from z=0 to big)
  zHalfLength[0] = BSCCryLength/2.;
  yHalfLength1[0]= BSCYFront0/2.;
  xHalfLength1[0]= BSCPhiRmin*tan(BSCPhiDphi)/2.;
  xHalfLength2[0]= xHalfLength1[0];

  //there are 3 choices of rminprojected
  G4double rminprojected=BSCRmin*cos(BSCAngleRotat);
    rminprojected=BSCRmin;//according to hardware design
  //rminprojected=BSCPhiRmin;
    yHalfLength2[0]= yHalfLength1[0]
    +(BSCYFront0-BSCPosition0)/rminprojected*BSCCryLength/2.;
  xHalfLength3[0]= xHalfLength1[0]*(BSCPhiRmin+BSCCryLength)/BSCPhiRmin;
  xHalfLength4[0]= xHalfLength2[0]*(BSCPhiRmin+BSCCryLength)/BSCPhiRmin;

  thetaPosition[0]= 90.*deg;
  xPosition[0]    = BSCPhiRmin+BSCCryLength/2.;
  yPosition[0]    = 
    -(xHalfLength1[0]+xHalfLength3[0]+xHalfLength2[0]+xHalfLength4[0])/4.;
  zPosition[0]    = (yHalfLength1[0]+yHalfLength2[0])/2.;
  
  theta0= 90.*deg-atan((BSCYFront0-BSCPosition0)/rminprojected);
  if(verboseLevel>1)
    {
      G4cout << "------------------------------------>1"<< G4endl
	     << "The direction of crystal is:"
	     << theta0/deg <<"(deg)."<< G4endl;
    }

  rearBoxPosX[0] = xPosition[0]+BSCCryLength/2.+rearBoxDz/2.;
  rearBoxPosY[0] = -(xHalfLength3[0]+xHalfLength4[0])/2.;
  rearBoxPosZ[0] = yHalfLength2[0];

  OCGirderRmin1[0] = rearBoxPosX[0]+rearBoxDz/2.+delta;
  OCGirderRmin2[0] = rearBoxPosX[0]+rearBoxDz/2.+delta;
  OCGirderDz[0] = rearBoxLength;
  OCGirderPosZ[0] = rearBoxLength/2;

  cableLength[0] = BSCDz-rearBoxPosZ[0];
  cablePosX[0] = BSCPhiRmax-cableDr-2.*mm;
  cablePosY[0] = rearBoxPosY[0]-3*cableDr;
  cablePosZ[0] = BSCDz-cableLength[0]/2;

  //crystal No.2
  zHalfLength[1]= BSCCryLength/2.;
  yHalfLength1[1]= BSCYFront0/2.;
  G4double deltaR= BSCYFront0*cos(theta0);
  xHalfLength1[1]= xHalfLength1[0];
  xHalfLength2[1]= xHalfLength1[1]*(BSCPhiRmin+deltaR)/BSCPhiRmin;
  yHalfLength2[1]= yHalfLength1[1]
    +tan(theta0-atan(rminprojected/(BSCYFront0*(1.+1./sin(theta0))-
				    BSCPosition1)))*BSCCryLength/2.;
  deltaR=deltaR+BSCCryLength*sin(theta0);
  xHalfLength4[1]= xHalfLength1[1]*(BSCPhiRmin+deltaR)/BSCPhiRmin;
  deltaR=deltaR-yHalfLength2[1]*2.*cos(theta0);
  xHalfLength3[1]= xHalfLength1[1]*(BSCPhiRmin+deltaR)/BSCPhiRmin;

  thetaPosition[1]= theta0;
  xPosition[1]    = BSCPhiRmin+BSCCryLength/2.*sin(theta0)
                  + (3.*yHalfLength1[1]-yHalfLength2[1])/2.*cos(theta0);
  yPosition[1]    = 
    -(xHalfLength1[1]+xHalfLength3[1]+xHalfLength2[1]+xHalfLength4[1])/4.;
  zPosition[1]    = yHalfLength1[0]*2.
    + (yHalfLength1[1]*2./tan(theta0)+BSCCryLength/2.)*cos(theta0)
    + (yHalfLength1[1]+yHalfLength2[1])/2.*sin(theta0);

  rearBoxPosX[1] = xPosition[1]+(BSCCryLength/2.+rearBoxDz/2.)*sin(theta0);
  rearBoxPosY[1] = -(xHalfLength3[1]+xHalfLength4[1])/2.;
  rearBoxPosZ[1] = zPosition[1]+(BSCCryLength/2.+rearBoxDz/2.)*cos(theta0);

  OCGirderRmin1[1] = rearBoxPosX[1]+rearBoxDz*sin(theta0)/2.+rearBoxLength*cos(theta0)/2+delta;
  OCGirderRmin2[1] = rearBoxPosX[1]+rearBoxDz*sin(theta0)/2.-rearBoxLength*cos(theta0)/2+delta;
  OCGirderDz[1] = rearBoxLength*sin(theta0);
  OCGirderPosZ[1] = rearBoxPosZ[1]+rearBoxDz*cos(theta0)/2.;

  cableLength[1] = BSCDz-rearBoxPosZ[1];
  cablePosX[1] = cablePosX[0];
  cablePosY[1] = cablePosY[0]+2*cableDr;
  cablePosZ[1] = BSCDz-cableLength[1]/2;

  theta0= theta0-atan((yHalfLength2[1]-yHalfLength1[1])*2./BSCCryLength);

  for(i=2;i<BSCNbTheta;i++)
    {
      if(verboseLevel>1)
	{
	  G4cout << "------------------------------------>"<<i<< G4endl
		 << "The direction of crystal is:"
		 << theta0/deg <<"(deg)." << G4endl;
	}
      G4double CryLength;
      if(i==BSCNbTheta-1) //the rightest crystal
      {
        CryLength=BSCCryLength1;    //24cm
        yHalfLength1[i]=BSCYFront1/2.;
      } else {
        //crystal No.i+1
        CryLength=BSCCryLength;   //28cm
        yHalfLength1[i]=BSCYFront/2.;
      }
      zHalfLength[i]=CryLength/2;

      deltaR=yHalfLength1[i]*2.*cos(theta0);
      xHalfLength1[i]=xHalfLength1[0];
      xHalfLength2[i]=xHalfLength1[i]/BSCPhiRmin*(BSCPhiRmin+deltaR);
      yHalfLength2[i]=yHalfLength1[i]
	*(1.+CryLength/(rminprojected/sin(theta0)
			   +yHalfLength1[i]*2./tan(theta0)));
      deltaR=deltaR+CryLength*sin(theta0);
      xHalfLength4[i]=xHalfLength1[i]/BSCPhiRmin*(BSCPhiRmin+deltaR);
      deltaR=deltaR-yHalfLength2[i]*2.*cos(theta0);
      xHalfLength3[i]=xHalfLength1[i]/BSCPhiRmin*(BSCPhiRmin+deltaR);

      thetaPosition[i]=theta0;
      xPosition[i]=BSCPhiRmin+zHalfLength[i]*sin(theta0)
                  + (3.*yHalfLength1[i]-yHalfLength2[i])/2.*cos(theta0);
      yPosition[i]=
	-(xHalfLength1[i]+xHalfLength3[i]+xHalfLength2[i]+xHalfLength4[i])/4.;
      zPosition[i]=BSCPosition1+rminprojected/tan(theta0)
	+(2.*yHalfLength1[i]/tan(theta0)+zHalfLength[i])*cos(theta0)
	+(yHalfLength1[i]+yHalfLength2[i])/2.*sin(theta0);

      rearBoxPosX[i] = xPosition[i]+(CryLength/2.+rearBoxDz/2.)*sin(theta0);
      rearBoxPosY[i] = -(xHalfLength3[i]+xHalfLength4[i])/2.;
      rearBoxPosZ[i] = zPosition[i]+(CryLength/2.+rearBoxDz/2.)*cos(theta0);
      
      OCGirderRmin1[i] = rearBoxPosX[i]+rearBoxDz*sin(theta0)/2.+rearBoxLength*cos(theta0)/2+delta;
      OCGirderRmin2[i] = rearBoxPosX[i]+rearBoxDz*sin(theta0)/2.-rearBoxLength*cos(theta0)/2+delta;
      OCGirderDz[i] = rearBoxLength*sin(theta0);
      OCGirderPosZ[i] = rearBoxPosZ[i]+rearBoxDz*cos(theta0)/2.;

     G4int xCable,yCable;
      xCable = i/4;
      yCable = i-4*(G4int)(i/4);
      cableLength[i] = BSCDz-(rearBoxPosZ[i]+rearBoxDz/2.*cos(theta0));
      cablePosX[i] = cablePosX[0]-xCable*2*cableDr;
      cablePosY[i] = cablePosY[0]+yCable*2*cableDr;
      cablePosZ[i] = BSCDz-cableLength[i]/2;

      theta0=theta0-atan(2.*yHalfLength1[i]/(rminprojected/sin(theta0)
					     +2.*yHalfLength1[i]/tan(theta0)));
      
    }
  thetaPosition[BSCNbTheta]=theta0;
    if(verboseLevel>1)
    {
      G4cout << "------------------------------------>"<<i<< G4endl
	     << "The direction of crystal is:"
	     << theta0/deg <<"(deg)." << G4endl;
    }
   
  G4double oop;
  G4double dx=0.001*mm; //zoom out of crystals to avoid overlap
  for(i=0;i<BSCNbTheta;i++)
    {
      xHalfLength1[i]-=dx;
      xHalfLength2[i]-=dx;
      xHalfLength3[i]-=dx;
      xHalfLength4[i]-=dx;
      yHalfLength1[i]-=dx;
      yHalfLength2[i]-=dx;

      G4double CryLength;
      if(i==BSCNbTheta-1) //the rightest crystal
      {
        CryLength=BSCCryLength1;
      } else {
        CryLength=BSCCryLength;
      }
      oop=sqrt((yHalfLength2[i]-yHalfLength1[i])*(yHalfLength2[i]
						  -yHalfLength1[i])
	       +(xHalfLength3[i]+xHalfLength4[i]-xHalfLength1[i]
		 -xHalfLength2[i])*(xHalfLength3[i]+xHalfLength4[i]
				    -xHalfLength1[i]-xHalfLength2[i])/4);
      thetaAxis[i]=atan(oop/CryLength);
      phiAxis[i]  =180.*deg+atan((yHalfLength2[i]-yHalfLength1[i])
			 /(xHalfLength3[i]+xHalfLength4[i]
			   -xHalfLength1[i]-xHalfLength2[i])*2.);
      tanAlpha2[i]=-(xHalfLength4[i]-xHalfLength3[i])/yHalfLength2[i]/2.;
      tanAlpha1[i]=-(xHalfLength2[i]-xHalfLength1[i])/yHalfLength1[i]/2.;
      //G4cout <<thetaAxis[i]<<",  "
      //     <<phiAxis[i]<<",  "
      //     <<tanAlpha1[i]<<",  "
      //     <<tanAlpha2[i]<<G4endl;
    }
    
}

void BesEmcGeometry::ModifyForCasing()
{
  // Compute the sizes of the naked crystals and the casing
  // Casing size
  // BSCNbTheta---->2*BSCNbTheta-1 : before crystal
  // 2*BSCNbTheta-->3*BSCNbTheta-1 : a side of crystal
  // 3*BSCNbTheta-->4*BSCNbTheta-1 : b side of crystal
  // 4*BSCNbTheta-->5*BSCNbTheta-1 : c side of crystal
  // 5*BSCNbTheta-->6*BSCNbTheta-1 : d side of crystal
  //                         d
  //                    ----------
  //                   |          |
  //                   |          |  
  //               c   |          |    a
  //                   |          |
  //                   |          |
  //                   |         / 
  //                   |     /
  //                   | / 
  //                         b   
  G4double totalThickness=fTyvekThickness+fAlThickness+fMylarThickness;//
  G4double delta=0.,angle1=0.*deg,angle2=0.*deg;
  G4double oop; //the distance of the centers of the two parallel plane

  G4double rminprojected=BSCRmin*cos(BSCAngleRotat);
  rminprojected=BSCRmin;//accord with hardware design
  //rminprojected=BSCPhiRmin;

  G4int i;
  for(i=0;i<BSCNbTheta;i++)
    {
      G4double CryLength;
      if(i==BSCNbTheta-1) //the rightest crystal
      {
        CryLength=BSCCryLength1;
      } else {
        CryLength=BSCCryLength;
      }
      zHalfLength[BSCNbTheta+i]=totalThickness/2.;
      yHalfLength1[BSCNbTheta+i]=yHalfLength1[i];
      yHalfLength2[BSCNbTheta+i]=yHalfLength1[i]
	+(yHalfLength2[i]-yHalfLength1[i])*totalThickness/CryLength;
      xHalfLength1[BSCNbTheta+i]=xHalfLength1[i];
      xHalfLength2[BSCNbTheta+i]=xHalfLength2[i];
      xHalfLength1[BSCNbTheta*2+i]=xHalfLength3[BSCNbTheta+i]=
	xHalfLength1[i]*(CryLength-totalThickness)/CryLength
	+xHalfLength3[i]*totalThickness/CryLength;
      xHalfLength2[BSCNbTheta*4+i]=xHalfLength4[BSCNbTheta+i]=
	xHalfLength2[i]*(CryLength-totalThickness)/CryLength
	+xHalfLength4[i]*totalThickness/CryLength;
      
      zHalfLength[BSCNbTheta*5+i]=zHalfLength[BSCNbTheta*4+i]=
      zHalfLength[BSCNbTheta*3+i]=zHalfLength[BSCNbTheta*2+i]=
	zHalfLength[i]-totalThickness/2.;
      
      yHalfLength2[BSCNbTheta*2+i]=yHalfLength1[BSCNbTheta*2+i]=
	totalThickness/cos(thetaPosition[i+1]-thetaPosition[i])/2.;
      xHalfLength3[BSCNbTheta*2+i]=xHalfLength3[i];
      xHalfLength4[BSCNbTheta*2+i]=xHalfLength3[i]
	+(xHalfLength4[i]-xHalfLength3[i])*yHalfLength2[BSCNbTheta*2+i]
	/yHalfLength2[i];
      xHalfLength2[BSCNbTheta*2+i]=xHalfLength3[BSCNbTheta+i]
	+(xHalfLength4[BSCNbTheta+i]-xHalfLength3[BSCNbTheta+i])
	*yHalfLength1[BSCNbTheta*2+i]/yHalfLength2[BSCNbTheta*1+i];
      
      yHalfLength2[BSCNbTheta*4+i]=yHalfLength1[BSCNbTheta*4+i]=
	totalThickness/2.;
      xHalfLength4[BSCNbTheta*4+i]=xHalfLength4[i];
      xHalfLength3[BSCNbTheta*4+i]=xHalfLength4[i]
	-(xHalfLength4[i]-xHalfLength3[i])*yHalfLength2[BSCNbTheta*4+i]
	/yHalfLength2[i];
      xHalfLength1[BSCNbTheta*4+i]=xHalfLength4[BSCNbTheta+i]
	-(xHalfLength4[BSCNbTheta+i]-xHalfLength3[BSCNbTheta+i])
	*yHalfLength1[BSCNbTheta*4+i]/yHalfLength2[BSCNbTheta*1+i];
      
      delta=totalThickness/2.+yHalfLength1[BSCNbTheta*2+i];
      angle1=atan(yHalfLength2[i]/(xHalfLength4[i]-xHalfLength3[i]));
      angle2=atan(2.*(xHalfLength4[i]-xHalfLength2[i])*sin(angle1)
		  /CryLength);
      
      yHalfLength1[BSCNbTheta*5+i]=yHalfLength1[BSCNbTheta*3+i]=
	yHalfLength1[i]-delta;
      yHalfLength2[BSCNbTheta*5+i]=yHalfLength2[BSCNbTheta*3+i]=
	yHalfLength2[i]-delta;
      xHalfLength4[BSCNbTheta*3+i]=xHalfLength3[BSCNbTheta*3+i]=
      xHalfLength2[BSCNbTheta*3+i]=xHalfLength1[BSCNbTheta*3+i]=
	totalThickness/cos(angle2)/sin(angle1)/2.;
      xHalfLength4[BSCNbTheta*5+i]=xHalfLength3[BSCNbTheta*5+i]=
      xHalfLength2[BSCNbTheta*5+i]=xHalfLength1[BSCNbTheta*5+i]=
	totalThickness/2.;
      
      zHalfLength[i]=zHalfLength[i]-totalThickness/2.;
      yHalfLength1[i]=yHalfLength1[i]-delta;
      yHalfLength2[i]=yHalfLength2[i]-delta;
      delta=totalThickness*(1.+1./cos(angle2)/sin(angle1))/2.;
      xHalfLength1[i]=xHalfLength1[i]-delta;
      xHalfLength2[i]=xHalfLength2[i]-delta;
      xHalfLength3[i]=xHalfLength3[i]-delta;
      xHalfLength4[i]=xHalfLength4[i]-delta;

      oop=sqrt((yHalfLength2[i]-yHalfLength1[i])*(yHalfLength2[i]
						  -yHalfLength1[i])
	       +(xHalfLength3[i]+xHalfLength4[i]-xHalfLength1[i]
		 -xHalfLength2[i])*(xHalfLength3[i]+xHalfLength4[i]
				    -xHalfLength1[i]-xHalfLength2[i])/4);
      thetaAxis[i]=atan(oop/CryLength);
      // -phi  --->180+phi 
      phiAxis[i]  =180.*deg+atan((yHalfLength2[i]-yHalfLength1[i])
				 /(xHalfLength3[i]+xHalfLength4[i]
				   -xHalfLength1[i]-xHalfLength2[i])*2.);

      oop=sqrt((yHalfLength2[BSCNbTheta+i]-yHalfLength1[BSCNbTheta+i])
	       *(yHalfLength2[BSCNbTheta+i]-yHalfLength1[BSCNbTheta+i])
	       +(xHalfLength3[BSCNbTheta+i]+xHalfLength4[BSCNbTheta+i]
		 -xHalfLength1[BSCNbTheta+i]-xHalfLength2[BSCNbTheta+i])
	       *(xHalfLength3[BSCNbTheta+i]+xHalfLength4[BSCNbTheta+i]
		 -xHalfLength1[BSCNbTheta+i]-xHalfLength2[BSCNbTheta+i])/4);
      thetaAxis[BSCNbTheta+i]=atan(oop/totalThickness);
      phiAxis  [BSCNbTheta+i]=
	-atan((yHalfLength2[BSCNbTheta+i]-yHalfLength1[BSCNbTheta+i])
	      /(xHalfLength3[BSCNbTheta+i]+xHalfLength4[BSCNbTheta+i]
		-xHalfLength1[BSCNbTheta+i]-xHalfLength2[BSCNbTheta+i])*2.);

      oop=sqrt((yHalfLength2[i]-yHalfLength1[i])*(yHalfLength2[i]
						  -yHalfLength1[i])*4
	       +(xHalfLength3[BSCNbTheta*2+i]+xHalfLength4[BSCNbTheta*2+i]
		 -xHalfLength1[BSCNbTheta*2+i]-xHalfLength2[BSCNbTheta*2+i])
	       *(xHalfLength3[BSCNbTheta*2+i]+xHalfLength4[BSCNbTheta*2+i]
		 -xHalfLength1[BSCNbTheta*2+i]-xHalfLength2[BSCNbTheta*2+i])
	       /4);
      thetaAxis[BSCNbTheta*2+i]=atan(oop/(zHalfLength[BSCNbTheta*2+i]*2));
      phiAxis  [BSCNbTheta*2+i]=
	-atan((yHalfLength2[i]-yHalfLength1[i])
	      /(xHalfLength3[BSCNbTheta*2+i]+xHalfLength4[BSCNbTheta*2+i]
		-xHalfLength1[BSCNbTheta*2+i]-xHalfLength2[BSCNbTheta*2+i])*4);

      oop=sqrt((yHalfLength2[i]-yHalfLength1[i])*(yHalfLength2[i]
						  -yHalfLength1[i])*4
	       +(xHalfLength4[i]-xHalfLength2[i])
	       *(xHalfLength4[i]-xHalfLength2[i])*4);
      thetaAxis[BSCNbTheta*3+i]=atan(oop/(zHalfLength[BSCNbTheta*3+i]*2));
      phiAxis  [BSCNbTheta*3+i]=-atan((yHalfLength2[i]-yHalfLength1[i])
				      /(xHalfLength4[i]-xHalfLength2[i]));
      
      thetaAxis[BSCNbTheta*4+i]=
	atan((xHalfLength4[BSCNbTheta*4+i]+xHalfLength3[BSCNbTheta*4+i]
	      -xHalfLength2[BSCNbTheta*4+i]-xHalfLength1[BSCNbTheta*4+i])/2.
	     /(zHalfLength[BSCNbTheta*4+i]*2));
      phiAxis  [BSCNbTheta*4+i]=0;

      thetaAxis[BSCNbTheta*5+i]=atan((xHalfLength3[BSCNbTheta*5+i]
				      -xHalfLength1[BSCNbTheta*5+i])
				     /(zHalfLength[BSCNbTheta*5+i]*2));
      phiAxis  [BSCNbTheta*5+i]=-90.*deg;
      
      tanAlpha2[BSCNbTheta+i]=tanAlpha1[BSCNbTheta+i]=tanAlpha1[i]=
	-(xHalfLength2[i]-xHalfLength1[i])/yHalfLength1[i]/2.;
      tanAlpha1[BSCNbTheta*2+i]=(xHalfLength2[BSCNbTheta*2+i]
				 -xHalfLength1[BSCNbTheta*2+i])
	/yHalfLength1[BSCNbTheta*2+i]/2.;
      tanAlpha1[BSCNbTheta*3+i]=tanAlpha1[i]*2.;
      tanAlpha1[BSCNbTheta*4+i]=(xHalfLength2[BSCNbTheta*4+i]
				 -xHalfLength1[BSCNbTheta*4+i])
	/yHalfLength1[BSCNbTheta*4+i]/2.;
      tanAlpha1[BSCNbTheta*5+i]=(xHalfLength2[BSCNbTheta*5+i]
				 -xHalfLength1[BSCNbTheta*5+i])
	/yHalfLength1[BSCNbTheta*5+i]/2.;
      
      tanAlpha2[i]=-(xHalfLength4[i]-xHalfLength3[i])/yHalfLength2[i]/2.;
      //tanAlpha2[BSCNbTheta+i]=(xHalfLength4[BSCNbTheta+i]-xHalfLength3[BSCNbTheta+i])/yHalfLength2[BSCNbTheta+i]/2.;
      tanAlpha2[BSCNbTheta*2+i]=(xHalfLength4[BSCNbTheta*2+i]
				 -xHalfLength3[BSCNbTheta*2+i])
	/yHalfLength2[BSCNbTheta*2+i]/2.;
      tanAlpha2[BSCNbTheta*3+i]=tanAlpha2[i]*2.;
      tanAlpha2[BSCNbTheta*4+i]=(xHalfLength4[BSCNbTheta*4+i]
				 -xHalfLength3[BSCNbTheta*4+i])
	/yHalfLength2[BSCNbTheta*4+i]/2.;
      tanAlpha2[BSCNbTheta*5+i]=(xHalfLength4[BSCNbTheta*5+i]
				 -xHalfLength3[BSCNbTheta*5+i])
	/yHalfLength2[BSCNbTheta*5+i]/2.;

      zPosition[BSCNbTheta*5+i]=zPosition[BSCNbTheta*3+i]=zPosition[i]=
	zPosition[i]+totalThickness/2.*cos(thetaPosition[i])
	-yHalfLength1[BSCNbTheta*2+i]*sin(thetaPosition[i]);
      zPosition[i]=totalThickness/2.;//for the newest method
      xPosition[BSCNbTheta*5+i]=xPosition[BSCNbTheta*3+i]=xPosition[i]=
	xPosition[i]+totalThickness/2.*sin(thetaPosition[i])
	+totalThickness*(1./cos(thetaPosition[i+1]-thetaPosition[i])-1)/2.
	*cos(thetaPosition[i]);
      xPosition[i]=totalThickness*(1.-1./cos(angle2)/sin(angle1))/2.;
                                                   //for the newest method
      yPosition[i]=yPosition[i]
	+totalThickness*(1.-1./cos(angle2)/sin(angle1))/2.;
      yPosition[i]=yHalfLength1[BSCNbTheta*2+i]-totalThickness/2.;//for the newest method
      yPosition[BSCNbTheta*3+i]=yPosition[i]*2.+xHalfLength1[BSCNbTheta*3+i];
      yPosition[BSCNbTheta*5+i]=xHalfLength1[BSCNbTheta*5+i];

      xPosition[BSCNbTheta+i]=BSCPhiRmin
	+zHalfLength[BSCNbTheta+i]*sin(thetaPosition[i])
	+(3.*yHalfLength1[BSCNbTheta+i]-yHalfLength2[BSCNbTheta+i])/2.
	*cos(thetaPosition[i]);
      yPosition[BSCNbTheta+i]=(xHalfLength1[BSCNbTheta+i]
			       +xHalfLength3[BSCNbTheta+i]
			       +xHalfLength2[BSCNbTheta+i]
			       +xHalfLength4[BSCNbTheta+i])/4.;
      zPosition[BSCNbTheta+i]=BSCPosition1+rminprojected/tan(thetaPosition[i])
	+(2.*yHalfLength1[BSCNbTheta+i]/tan(thetaPosition[i])
	  +zHalfLength[BSCNbTheta+i])*cos(thetaPosition[i])
	+(yHalfLength1[BSCNbTheta+i]+yHalfLength2[BSCNbTheta+i])/2.
	*sin(thetaPosition[i]);

      xPosition[BSCNbTheta*2+i]=xPosition[i]
	+((yHalfLength1[i]+yHalfLength2[i])/2.+yHalfLength1[BSCNbTheta*2+i])
	*cos(thetaPosition[i]);
      zPosition[BSCNbTheta*2+i]=zPosition[i]
	-((yHalfLength1[i]+yHalfLength2[i])/2.+yHalfLength1[BSCNbTheta*2+i])
	*sin(thetaPosition[i]);
      yPosition[BSCNbTheta*2+i]=(xHalfLength1[BSCNbTheta*2+i]
				 +xHalfLength3[BSCNbTheta*2+i]
				 +xHalfLength2[BSCNbTheta*2+i]
				 +xHalfLength4[BSCNbTheta*2+i])/4.;

      xPosition[BSCNbTheta*4+i]=xPosition[i]
	-((yHalfLength1[i]+yHalfLength2[i])/2.+yHalfLength1[BSCNbTheta*4+i])
	*cos(thetaPosition[i]);
      zPosition[BSCNbTheta*4+i]=zPosition[i]
	-((yHalfLength1[i]+yHalfLength2[i])/2.+yHalfLength1[BSCNbTheta*4+i])
	*sin(thetaPosition[i]);
      yPosition[BSCNbTheta*4+i]=(xHalfLength1[BSCNbTheta*4+i]
				 +xHalfLength3[BSCNbTheta*4+i]
				 +xHalfLength2[BSCNbTheta*4+i]
				 +xHalfLength4[BSCNbTheta*4+i])/4.;

    }

  if(verboseLevel>1)
    for(i=0;i<BSCNbTheta*6;i++)
      {
	G4cout << "The sizes of the " << i+1 << " crystal are:" << G4endl
	       << "zHalfLength =" << zHalfLength[i]/cm  << "(cm)," << G4endl
	       << "thetaAxis   =" << thetaAxis[i]/deg   << "(deg),"<< G4endl
	       << "phiAxis     =" << phiAxis[i]/deg     << "(deg),"<< G4endl
	       << "yHalfLength1=" << yHalfLength1[i]/cm << "(cm)," << G4endl
	       << "xHalfLength1=" << xHalfLength1[i]/cm << "(cm)," << G4endl
	       << "xHalfLength2=" << xHalfLength2[i]/cm << "(cm)," << G4endl
	       << "tanAlpha1   =" << tanAlpha1[i]                  << G4endl
	       << "yHalfLength2=" << yHalfLength2[i]/cm << "(cm)," << G4endl
	       << "xHalfLength3=" << xHalfLength3[i]/cm << "(cm)," << G4endl
	       << "xHalfLength4=" << xHalfLength4[i]/cm << "(cm)," << G4endl
	       << "tanAlpha2   =" << tanAlpha2[i]       << "."     << G4endl
	       << "The position of the " << i+1 << " crystal is:" << G4endl
	       << "(" << xPosition[i]/cm << ","
	       << yPosition[i]/cm << ","
	       << zPosition[i]/cm << ")cm" << G4endl;
      }
  
}

void BesEmcGeometry::SetCasingThickness(G4ThreeVector val)
{
  // change Gap thickness and recompute the calorimeter parameters
  fTyvekThickness = val('X');
  fAlThickness    = val('Y');
  fMylarThickness = val('Z');
}

G4double BesEmcGeometry::GetXPosition(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return xPosition[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetYPosition(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return yPosition[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetZPosition(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return zPosition[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetThetaPosition(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return thetaPosition[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetZHalfLength(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return zHalfLength[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetThetaAxis(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return thetaAxis[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetPhiAxis(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return phiAxis[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetYHalfLength1(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return yHalfLength1[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetXHalfLength1(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return xHalfLength1[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetXHalfLength2(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return xHalfLength2[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetTanAlpha1(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return tanAlpha1[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetYHalfLength2(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return yHalfLength2[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetXHalfLength3(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return xHalfLength3[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetXHalfLength4(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return xHalfLength4[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4double BesEmcGeometry::GetTanAlpha2(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return tanAlpha2[NbCrystal];
    }
  else
    {
      return 0.;
    }
}

G4VPhysicalVolume* BesEmcGeometry::GetPhysiBSCCrystal(G4int NbCrystal)
{
  if(NbCrystal>=0&&NbCrystal<BSCNbTheta*6)
    {
      return physiBSCCrystal[NbCrystal];
    }
  else
    {
      return NULL;
    }
}
