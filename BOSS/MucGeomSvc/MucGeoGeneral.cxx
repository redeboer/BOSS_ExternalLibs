//$id$
//
//$log$

/*
 *    2003/08/30   Zhengyun You     Peking University
 * 
 *    2004/09/09   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

using namespace std;

#include <fstream>
#include <iostream>
#include <strstream>
#include <vector>
//#include "TRint.h"
//#include <TROOT.h>
//#include <TApplication.h>
#include <TGeoManager.h>

#include "MucGeomSvc/MucGeoGeneral.h"
#include "ROOTGeo/MucROOTGeo.h"
//#include "TGDMLProcessor.h"
//#include "SAXProcessor.h"
//#include "ProcessingConfigurator.h"

int 
MucGeoGeneral::m_gGeometryInit = 0;

MucGeoGeneral* 
MucGeoGeneral::m_gpMucGeoGeneral = 0L;

map<Identifier, MucGeoGap*>
MucGeoGeneral::m_gpMucGeoGap = map<Identifier, MucGeoGap*>();

map<Identifier, MucGeoStrip*>
MucGeoGeneral::m_gpMucGeoStrip = map<Identifier, MucGeoStrip*>();

MucGeoGeneral::MucGeoGeneral()
{ 
  // Default constructor.
}

MucGeoGeneral::~MucGeoGeneral()
{ 
  // Destructor.
  MucGeoGap* pGap = 0;
  while(m_gpMucGeoGap.size() > 0){
    map<Identifier, MucGeoGap*>::iterator iter = m_gpMucGeoGap.end();
    pGap = (*iter).second;
    delete pGap;
    m_gpMucGeoGap.erase(iter);
  }

  MucGeoStrip* pStrip = 0;
  while(m_gpMucGeoStrip.size() > 0){
    map<Identifier, MucGeoStrip*>::iterator iter = m_gpMucGeoStrip.end();
    pStrip = (*iter).second;
    delete pStrip;
    m_gpMucGeoStrip.erase(iter);
  }
}

MucGeoGeneral& 
MucGeoGeneral::operator=(const MucGeoGeneral& orig)
{
  // Assignment operator.
  if ( this != &orig ) {             // Watch out for self-assignment!
    m_gpMucGeoGeneral = orig.m_gpMucGeoGeneral;
    m_gpMucGeoGap     = orig.m_gpMucGeoGap;
    m_gpMucGeoStrip   = orig.m_gpMucGeoStrip;
  }
  return *this;
}


void
MucGeoGeneral::Init()
{
  for(unsigned int part = 0; part < MucID::getPartNum(); part++) {
    for(unsigned int seg = 0; seg < MucID::getSegMax(); seg++) {
      for(unsigned int gap = 0; gap < MucID::getGapMax(); gap++) {
	m_StripNumInGap[part][seg][gap] = 0;
      }
    }
  }
}

void 
MucGeoGeneral::InitFromXML()
{
  //new TRint("ROOT GDML converter", 0, 0);
  
  // Initialize Bes Muc Geometry for XML files.
  bool geomanager = true;
  if (!gGeoManager) {
    gGeoManager = new TGeoManager("BesGeo", "Bes geometry");
    geomanager = false;
  }
  //gGeoManager->SetVisOption(0);  // to show all visible volumes.
  //gGeoManager->SetVisLevel(5);
  
  MucROOTGeo *m_MucROOTGeo = new MucROOTGeo();
  
  TGeoVolume *volMuc = m_MucROOTGeo->GetVolumeMuc();
  if(volMuc) std::cout << "Construct Muc from Muc.gdml" << std::endl;
  else       std::cout << "volume Muc not found " << std::endl;

  float m_BesR = 5200;
  float m_BesZ = 5680;
  TGeoIdentity *identity = new TGeoIdentity();
  
  TGeoMaterial *mat = new TGeoMaterial("VOID",0,0,0);
  TGeoMedium   *med = new TGeoMedium("MED",1,mat);
  TGeoVolume   *m_Bes = gGeoManager->MakeBox("volBes", med, 0.5*m_BesR, 0.5*m_BesR, 0.5*m_BesZ);
  gGeoManager->SetTopVolume(m_Bes);
  m_Bes->AddNode(volMuc, 0, identity); 
  m_MucROOTGeo->SetChildNo(m_Bes->GetNdaughters()-1); 
  
  gGeoManager->SetDrawExtraPaths();  // draw physical node or not;
  if(!geomanager)gGeoManager->CloseGeometry();
  gGeoManager->SetNsegments(20);

  m_MucROOTGeo->SetPhysicalNode();

  for (int part = 0; part < m_MucROOTGeo->GetPartNum(); part++) {
    for (int seg = 0; seg < m_MucROOTGeo->GetSegNum(part); seg++) {
      for (int gap = 0; gap < m_MucROOTGeo->GetGapNum(part); gap++) {
	Identifier gapID = MucID::channel_id(part,seg,gap,0);

	float ironThickness = 0.0;
	if (part == 1) {
	  if (gap > 0) ironThickness = m_MucROOTGeo->GetAbsorberThickness(part, seg, gap-1);
	}
	else {
	  ironThickness = m_MucROOTGeo->GetAbsorberThickness(part, seg, gap);
	}
	//std::cout << "part " << part << " seg " << seg << " gap " << gap << " thick " << ironThickness << std::endl;

	int orient = 0;
	if ( (part == 1 && gap % 2 == 0) || (part != 1 && gap % 2 == 1) ) orient = 1;
 	MucGeoGap *pGap = new MucGeoGap(part, seg, gap, orient, 0, m_MucROOTGeo->GetPhysicalGap(part, seg, gap), ironThickness); 
	m_gpMucGeoGap[gapID] = pGap;

	for (int strip = 0; strip < m_MucROOTGeo->GetStripNum(part, seg, gap); strip++) {
	  Identifier stripID = MucID::channel_id(part,seg,gap,strip);
	  
     	  MucGeoStrip* pStrip = m_gpMucGeoGap[gapID]->AddStrip(strip);
	  pStrip->SetStrip(m_MucROOTGeo->GetPhysicalStrip(part, seg, gap, strip));
	  m_gpMucGeoStrip[stripID] = pStrip;
	}
      }
    }
  }

  m_gGeometryInit = 1;  
}

void 
MucGeoGeneral::InitFromASCII()
{
  // Initialize Bes MUC Geometry for ASCII files.
  string gapSizeFile   = "muc-gap-size.dat";
  string gapGeomFile   = "muc-gap-geom.dat";
  string stripSizeFile = "muc-strip-size.dat";
  string stripGeomFile = "muc-strip-geom.dat";

  static const int bufSize=512;
  char lineBuf[bufSize];

  //
  // File gapSizeFile contains the gap sizes.
  //

  ifstream ifGapSize(gapSizeFile.c_str());
  if(!ifGapSize) {
    cout << "error opening gap size data file : "
	 << gapSizeFile 
	 << endl;
    return;
  }

  int part, seg, gap, strip, orient, panel;
  float xGapTemp, yGapTemp, zGapTemp;
  float xGapSize[m_kPartNum][m_kSegMax][m_kGapMax];
  float yGapSize[m_kPartNum][m_kSegMax][m_kGapMax];
  float zGapSize[m_kPartNum][m_kSegMax][m_kGapMax];;

  // Read the data line by line until we reach EOF.

  while ( ifGapSize.getline(lineBuf,bufSize,'\n')) {
    if (ifGapSize.gcount() > bufSize) {
      cout << "input buffer too small!  gcount = " << ifGapSize.gcount()
	   << endl;
      return;
      }
    
    istrstream stringBuf(lineBuf,strlen(lineBuf));
      
    if (stringBuf >> part >> seg >> gap >> xGapTemp >> yGapTemp >> zGapTemp) {
      xGapSize[part][seg][gap] = xGapTemp;
      yGapSize[part][seg][gap] = yGapTemp;
      zGapSize[part][seg][gap] = zGapTemp;

      //cout << part << "  " << seg << "  " << gap << "  " 
      //   << " x " << xGapSize[part][seg][gap]
      //   << " y " << yGapSize[part][seg][gap]
      //     << " z " << zGapSize[part][seg][gap]
      //   << endl;
    }
    else {
      // Skip any header or comment lines.
      // cout << "read comment line" << endl;
    }
  }
  
  ifGapSize.close();
  
  //
  // File stripSizeFile contains the strip sizes.
  //

  ifstream ifStripSize(stripSizeFile.c_str());
  if (!ifStripSize) {
    cout << "error opening strip size data file : "
	 << stripSizeFile 
	 << endl;
    return;
  }

  float xStripTemp, yStripTemp, zStripTemp;
  float xStripSize[m_kPartNum][m_kSegMax][m_kGapMax][m_kStripMax];
  float yStripSize[m_kPartNum][m_kSegMax][m_kGapMax][m_kStripMax];
  float zStripSize[m_kPartNum][m_kSegMax][m_kGapMax][m_kStripMax];

  // Read the data line by line until we reach EOF.

  while (ifStripSize.getline(lineBuf,bufSize,'\n')) {

    if (ifStripSize.gcount() > bufSize) {
      cout << "input buffer too small!  gcount = " << ifStripSize.gcount()
	   << endl;
      return;
    }

    istrstream stringBuf(lineBuf,strlen(lineBuf));

    if (stringBuf >> part >> seg >> gap >> strip >> xStripTemp >> yStripTemp >> zStripTemp) {
      xStripSize[part][seg][gap][strip] = xStripTemp;
      yStripSize[part][seg][gap][strip] = yStripTemp;
      zStripSize[part][seg][gap][strip] = zStripTemp;

      m_StripNumInGap[part][seg][gap]++;
      //cout << part << "  " << seg << "  " << gap << "  " 
      //     << strip << "  " 
      //     << " x " << xStripSize[part][seg][gap][strip]
      //     << " y " << yStripSize[part][seg][gap][strip]
      //     << " z " << zStripSize[part][seg][gap][strip]
      //     << endl;
    }
    else {
      // Skip any header or comment lines.
      //       cout << "read comment line" << endl;
    }
  }

  ifStripSize.close();

  //for(int part = 0; part < MucSoftID::m_kPart; part++) {
  //for(int seg = 0; seg < MucSoftID::m_kSegInPartMax; seg++) {
  //  for(int gap = 0; gap < MucSoftID::m_kGapInSegMax; gap++) {
  //cout << "nStrips In part " << part << " seg " << seg << " gap " << gap << " is " 
  //     << m_NStripInGap[part][seg][gap] << endl;
  //  }
  //}
  //}

  //
  // File gapGeomFile contains the gap positions, etc.
  //

  ifstream ifGapGeom(gapGeomFile.c_str());
  if (!ifGapGeom) {
    cout << "error opening gap geometry data file : " 
	 << gapGeomFile
	 << endl;
    return;
  }

  float xGapTarg1, yGapTarg1, zGapTarg1;
  float xGapTarg2, yGapTarg2, zGapTarg2;
  float xGapTarg3, yGapTarg3, zGapTarg3;
  float dzHighEdge;

  float dzFarFrontGas, dzNearFrontGas;
  float dzFarBackGas, dzNearBackGas;

  float dxTarget1ToFiducial, dyTarget1ToFiducial;
  float dxFiducialToCenter,  dyFiducialToCenter;

  Identifier gapID   = MucID::channel_id(0,0,0,0);
  Identifier stripID = MucID::channel_id(0,0,0,0);

  // Read the gap geometry data line by line until we reach EOF.

  while (ifGapGeom.getline(lineBuf,bufSize,'\n')) {
    
    if ( ifGapGeom.gcount() > bufSize ) {
      cout << "input buffer too small!  gcount = " << ifGapGeom.gcount()
	   << endl;
      return;
    }

    istrstream stringBuf(lineBuf,strlen(lineBuf));

    if ( stringBuf >> part >> seg >> gap >> orient
	 >> xGapTarg1 >> yGapTarg1 >> zGapTarg1
	 >> xGapTarg2 >> yGapTarg2 >> zGapTarg2
	 >> xGapTarg3 >> yGapTarg3 >> zGapTarg3
	 >> dzHighEdge 
	 >> dzFarFrontGas >> dzNearFrontGas
	 >> dzNearBackGas >> dzFarBackGas
	 >> dxTarget1ToFiducial
	 >> dyTarget1ToFiducial
	 >> dxFiducialToCenter
	 >> dyFiducialToCenter ) {
      
      //cout << " " << part << " " << seg << "  " << gap 
      //   << " " << xGapTarg1 << " " << yGapTarg1 << " " << zGapTarg1
      //   << " " << xGapTarg2 << " " << yGapTarg2 << " " << zGapTarg2
      //   << " " << xGapTarg3 << " " << yGapTarg3 << " " << zGapTarg3
      //   << " " << dzHighEdge 
      //   << " " << dzFarFrontGas << " " << dzNearFrontGas
      //   << " " << dzFarBackGas  << " " << dzNearBackGas
      //   << " " << dxTarget1ToFiducial << " " << dyTarget1ToFiducial
      //   << " " << dxFiducialToCenter  << " " << dyFiducialToCenter
      //   << endl;
      
      gapID = MucID::channel_id(part,seg,gap,0);
      
      MucGeoGap *pGap = new MucGeoGap(part, seg, gap,
				      orient, 0,
				      xGapSize[part][seg][gap],
				      yGapSize[part][seg][gap],
				      zGapSize[part][seg][gap],
				      xGapTarg1, yGapTarg1, zGapTarg1,
				      xGapTarg2, yGapTarg2, zGapTarg2,
				      xGapTarg3, yGapTarg3, zGapTarg3,
				      dzHighEdge,
				      dzFarFrontGas, dzNearFrontGas,
				      dzNearBackGas, dzFarBackGas,  
				      dxTarget1ToFiducial,
				      dyTarget1ToFiducial,
				      dxFiducialToCenter,
				      dyFiducialToCenter);
      m_gpMucGeoGap[gapID] = pGap;
    }
    else {
      // Skip any header or comment lines.
      //      cout << "read comment line" << endl;
    }

  }

  ifGapGeom.close();
 
  //
  // File stripGeomFile contains the strip positions, etc.
  //

  ifstream ifStripGeom(stripGeomFile.c_str());
  if (!ifStripGeom) {
    cout << "error opening strip geometry data file" 
	 << stripGeomFile
	 << endl;
    return;
  }
  
  // Read the strip geometry data line by line until we reach EOF.

  float xStripTarg1, yStripTarg1, xStripTarg2, yStripTarg2;

  while (ifStripGeom.getline(lineBuf,bufSize,'\n')) {

    if (ifStripGeom.gcount() > bufSize) {
      cout << "input buffer too small!  gcount = " << ifStripGeom.gcount()
	   << endl;
      return;
    }

    istrstream stringBuf(lineBuf,strlen(lineBuf));

    if (stringBuf >> part >> seg >> gap >> strip >> panel
	>> xStripTarg1 >> xStripTarg2
	>> yStripTarg1 >> yStripTarg2) {
      
      //       cout << part << " " << seg << " " << gap    << " " 
      //            << strip << " " << panel << " " << orient << " "
      //            << xStripTarg1 << " " << xStripTarg2 << " "
      //            << yStripTarg1 << " " << yStripTarg2
      //            << endl;

      MucGeoStrip* pStrip = 0;
      stripID = MucID::channel_id(part,seg,gap,strip);
      gapID   = MucID::channel_id(part,seg,gap,0);

      if (!m_gpMucGeoStrip[stripID]) {
	if (m_gpMucGeoGap[gapID]) {
	  pStrip = m_gpMucGeoGap[gapID]->AddStrip(strip);
	  pStrip->SetStrip(xStripTarg1, xStripTarg2,
			   yStripTarg1, yStripTarg2,
			   xStripSize[part][seg][gap][strip],
			   yStripSize[part][seg][gap][strip],
			   zStripSize[part][seg][gap][strip]);
	  m_gpMucGeoStrip[stripID] = pStrip;
	}
	else {
	  cout << "missing gap" << gapID << endl;
	  continue;
	}
      }

    }
    else {
      // Skip any header or comment lines.
      //       cout << "read comment line" << endl;
    }

  }

  ifStripGeom.close();

  m_gGeometryInit = 1;  
}

MucGeoGeneral* 
MucGeoGeneral::Instance()
{
  // Get a pointer to the single instance of MucGeoGeneral.
  if(!m_gpMucGeoGeneral) {
    m_gpMucGeoGeneral = new MucGeoGeneral;
  }

  return m_gpMucGeoGeneral;
}

MucGeoGap*
MucGeoGeneral::GetGap(const int part,
		      const int seg,
		      const int gap) const
{
  // Get a pointer to the gap identified by (part,seg,gap).
  Identifier gapID = MucID::channel_id(part, seg, gap, 0);

  return m_gpMucGeoGap[gapID];
}

MucGeoGap*
MucGeoGeneral::GetGap(const Identifier id) const
{
  // Get a pointer to the gap identified by Identifier.
  Identifier gapID = MucID::channel_id(MucID::part(id), 
				       MucID::seg(id),
				       MucID::gap(id),
				       0);
  
  return m_gpMucGeoGap[gapID];
}

MucGeoStrip* 
MucGeoGeneral::GetStrip(const int part,
			const int seg,
			const int gap,
			const int strip) const
{
  // Get a pointer to the strip identified by (part, seg, gap, strip).
  Identifier id = MucID::channel_id(part, seg, gap, strip);
 
  return m_gpMucGeoStrip[id];
}

MucGeoStrip* 
MucGeoGeneral::GetStrip(const Identifier id) const
{
  // Get a pointer to the strip identified Identifier.
  return m_gpMucGeoStrip[id];
}

int
MucGeoGeneral::GetStripNumTotal()
{
  int nStripTotal = 0;
  for(unsigned int part = 0; part < MucID::getPartNum(); part++) {
    for(unsigned int seg = 0; seg < MucID::getSegNum(part); seg++) {
      for(unsigned int gap = 0; gap < MucID::getGapNum(part); gap++) {
	nStripTotal += GetStripNumInGap(part, seg, gap);
      }
    }
  }

  return nStripTotal;
}

vector<Identifier> 
MucGeoGeneral::FindIntersectGaps(const int part,
				 const int gap,
				 const HepPoint3D gPoint,
				 const Hep3Vector gDirection)
{
  // Find the intersect gap of a trajectory with the given part and gap. The trajectory is
  // given by the position gPoint and direction gDirection in global coordinate.
  vector<Identifier> gapList;

  MucGeoGap *pGap = 0;
  Identifier  id = MucID::channel_id(0,0,0,0);
  HepPoint3D intersection, localIntersection;
  Hep3Vector intersectionDir;
  double cos = -1;

  for(unsigned int seg = 0; seg < MucID::getSegNum(part); seg++) {
    id = MucID::channel_id(part, seg, gap, 0);
    pGap = GetGap(id);
    if( pGap ) {
      intersection      = pGap->ProjectToGap(gPoint, gDirection);
      localIntersection = pGap->TransformToGap(intersection);

      intersectionDir = ((CLHEP::Hep3Vector)intersection) - ((CLHEP::Hep3Vector)gPoint);
      if( intersectionDir.mag() == 0 ) {
	cos = 0.0;
      }
      else {
	cos = intersectionDir.dot(gDirection) / 
	  ( intersectionDir.mag() * gDirection.mag() );
      }
      
      if( ( cos >= 0.0 ) &&
	  ( pGap->IsInGap(localIntersection.x(),
			  localIntersection.y(),
			  localIntersection.z()) ) ) { 
	id = MucID::channel_id(part, seg, gap, 0);
	gapList.push_back(id);
      }
      else {
      }
	
    }
    else{
      std::cout << "MucGeoGeneral::FindIntersectGaps(), Bad gap Pointer" 
		<< "  part " << part 
		<< "  seg "  << seg 
		<< "  gap "  << gap 
		<< std::endl;
    }
  }
  
  return gapList;
}

vector<Identifier> 
MucGeoGeneral::FindIntersectStrips(const int part,
				   const int gap,
				   const HepPoint3D gPoint,
				   const Hep3Vector gDirection)
{
  // Find the intersect strip of a trajectory with the given part and gap. The trajectory is
  // given by the position gPoint and direction gDirection in global coordinate.
  vector<Identifier> gapList;
  vector<Identifier> stripList;
  
  MucGeoGap   *pGap;
  MucGeoStrip *pStrip;
  
  int seg, iStripGuess, nStripMax;
  Identifier id;
  HepPoint3D intersection, localIntersection;
  Hep3Vector localDirection;

  gapList = FindIntersectGaps(part, gap, gPoint, gDirection);

  for(unsigned int i = 0; i < gapList.size(); i++) {
    // Get the gap data ...
    seg = MucID::seg(gapList[i]);
    pGap = GetGap(part, seg, gap);
    if(!pGap) {
      cout << "FindIntersectStrips : bad gap pointer!" << endl;
      return stripList;
    }

    intersection      = pGap->ProjectToGap(gPoint, gDirection);
    localIntersection = pGap->TransformToGap(intersection);
    localDirection    = pGap->RotateToGap(gDirection);

    // Search through gap to find the intersect strips.
    nStripMax   = pGap->GetStripNum() - 1;
    iStripGuess = pGap->GuessStrip(localIntersection.x(),
				   localIntersection.y(),
				   localIntersection.z());     
    //cout << "guess " << iStripGuess << endl;

    int iStripLow  = iStripGuess - 2;
    int iStripHigh = iStripGuess + 2;
    iStripLow  = max(0, iStripLow);
    iStripHigh = min(nStripMax,  iStripHigh);

    iStripLow = 0;
    iStripHigh = nStripMax;

    //cout << "intersection      : " << intersection << endl 
    // << "localIntersection : " << localIntersection << endl
    // << "localDirection    : " << localDirection << endl;

    for(int j = iStripLow; j < iStripHigh; j++) {
      pStrip = pGap->GetStrip(j);
    
       if(pStrip->CrossGasChamber(localIntersection, localDirection)) {
	id = MucID::channel_id(part, seg, gap, j);	
	stripList.push_back(id);
      }
    }
   
  }

  return stripList;
}



vector<Identifier> 
MucGeoGeneral::FindIntersectStrips(const int part,
				   const int gap,
				   const HepPoint3D gPoint,
				   const Hep3Vector gDirection,
				   vector<int> &padID, vector<float> &intersection_x,
				   vector<float> &intersection_y,vector<float> &intersection_z)
{
  // Find the intersect strip of a trajectory with the given part and gap. The trajectory is
  // given by the position gPoint and direction gDirection in global coordinate.
  vector<Identifier> gapList;
  vector<Identifier> stripList;
  
  MucGeoGap   *pGap;
  MucGeoStrip *pStrip;
  
  int seg, iStripGuess, nStripMax;
  Identifier id;
  HepPoint3D intersection, localIntersection;
  Hep3Vector localDirection;

  gapList = FindIntersectGaps(part, gap, gPoint, gDirection);

  for(unsigned int i = 0; i < gapList.size(); i++) {
    // Get the gap data ...
    seg = MucID::seg(gapList[i]);
    pGap = GetGap(part, seg, gap);
    if(!pGap) {
      cout << "FindIntersectStrips : bad gap pointer!" << endl;
      return stripList;
    }

    intersection      = pGap->ProjectToGap(gPoint, gDirection);
    localIntersection = pGap->TransformToGap(intersection);
    localDirection    = pGap->RotateToGap(gDirection);

    // Search through gap to find the intersect strips.
    nStripMax   = pGap->GetStripNum() - 1;
    iStripGuess = pGap->GuessStrip(localIntersection.x(),
				   localIntersection.y(),
				   localIntersection.z());     
    //cout << "guess " << iStripGuess << endl;

    int iStripLow  = iStripGuess - 2;
    int iStripHigh = iStripGuess + 2;
    iStripLow  = max(0, iStripLow);
    iStripHigh = min(nStripMax,  iStripHigh);

    iStripLow = 0;
    iStripHigh = nStripMax;

    //cout << "intersection      : " << intersection << endl 
    // << "localIntersection : " << localIntersection << endl
    // << "localDirection    : " << localDirection << endl;

    for(int j = iStripLow; j < iStripHigh; j++) {
      pStrip = pGap->GetStrip(j);
    
       if(pStrip->CrossGasChamber(localIntersection, localDirection)) {
	//get id of intersect strip, now calc pad id!
        /*
        cout<<"Strip: ("<<part<<", "<<seg<<", "<<gap<<", "<<j<<")"<<endl;
	cout<<"xmin: "<<pStrip->GetXmin()<<" xmax: "<<pStrip->GetXmax()<<endl;
        cout<<"ymin: "<<pStrip->GetYmin()<<" ymax: "<<pStrip->GetYmax()<<endl;
	cout<<"zmin: "<<pStrip->GetZmin()<<" zmax: "<<pStrip->GetZmax()<<endl;
	*/
	float posx,posy,posz;
        pStrip->GetCenterPos(posx,posy,posz);
	/*
        cout<<"orient:     "<<pGap->Orient()<<endl;
	cout<<"center pos: "<<posx<<" "<<posy<<" "<<posz<<endl;
        cout<<"inter  pos: "<<localIntersection<<endl;
	*/
	int padid = -1;
	if(pGap->Orient() == 1)
		 padid = (localIntersection.y() - pStrip->GetYmin())/(pStrip->GetXmax() - pStrip->GetXmin());
        else
		 padid = (localIntersection.x() - pStrip->GetXmin())/(pStrip->GetYmax() - pStrip->GetYmin());

	//cout<<"padID: "<<padid<<endl;
        padID.push_back(padid);
        intersection_x.push_back(localIntersection.x());
        intersection_y.push_back(localIntersection.y());
        intersection_z.push_back(localIntersection.z());

	id = MucID::channel_id(part, seg, gap, j);	
	stripList.push_back(id);
      }
    }
   
  }

  return stripList;
}






vector<HepPoint3D> 
MucGeoGeneral::FindIntersections(const int part,
				 const int gap,
				 const HepPoint3D gPoint,
				 const Hep3Vector gDirection)
{
  // Find the intersection position of a trajectory with the given part and gap.
  // The trajectory is given by the position and direction in global coordinate.
  vector<HepPoint3D> intersectionList;
  MucGeoGap* pGap;

  HepPoint3D intersection, localIntersection;
  Hep3Vector intersectionDir;
  double cos = -1;

  for(unsigned int seg = 0; seg < MucID::getSegNum(part); seg++ ) {
    pGap = GetGap(part, seg, gap);
    if( pGap ) {
      intersection      = pGap->ProjectToGap(gPoint, gDirection);
      localIntersection = pGap->TransformToGap(intersection);
      //cout << localIntersection << endl;

      intersectionDir = ((CLHEP::Hep3Vector)intersection) - ((CLHEP::Hep3Vector)gPoint);
      if( intersectionDir.mag() == 0 ) {
	cos = 0.0;
      }
      else {
	cos = intersectionDir.dot(gDirection) / 
	  ( intersectionDir.mag() * gDirection.mag() );
      }
      
      if( ( cos >= 0.0 ) &&
	  ( pGap->IsInGap(localIntersection.x(),
			  localIntersection.y(),
			  localIntersection.z()) ) ) { 
	intersectionList.push_back(intersection);
      }
      else {
      }
    }
    else{
    }
  }

  return intersectionList;
}

// Find the intersection position of a trajectory with the given gap.
// The trajectory is given by unit vector (vx,vy,vz) and intercept (x0,y0,z0)
// in global coordinates, such that (x-x0)/vx = (y-y0)/vy = (z-z0)/vz .
// If more than one point lies along the trajectory, take the first one
// intersect with the trajectory.
void
MucGeoGeneral::FindIntersection(const int part,
				const int seg,
				const int gap,
				const float vx,
				const float vy,
				const float vz,
				const float x0,
				const float y0,
				const float z0,
				const float sigmaVx,
				const float sigmaVy,
				const float sigmaVz,
				const float sigmaX0,
				const float sigmaY0,
				const float sigmaZ0,
				float& x,
				float& y,
				float& z,
				float& sigmaX,
				float& sigmaY,
				float& sigmaZ)
{
  x = 0.0; sigmaX = 0.0;
  y = 0.0; sigmaY = 0.0;
  z = 0.0; sigmaZ = 0.0;

  if ( (part < 0) || (part >= (int)MucID::getPartNum()) ) {
    cout << "MucGeoGeneral::FindIntersection-E101  invalid part = "
	 << part << endl;
    return;
  }

  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    //cout << "MucGeoGeneral::FindIntersection-E102  invalid gap = "
	// << gap << endl;
    return;
  }
  
  // "Brute-force" algorithm.
  //   1a. Find intersection with gap.
  //   1b. Transform intersection position to gap coords.
  //   2.  Check intersection position against gap boundaries.
  //       Save the intersection position and gap id if within
  //       boundaries.

  //bool found = false;
  float distance = 1e30;
  
  MucGeoGap *p = GetGap(part, seg, gap);
  if (p) {
    
    Hep3Vector  v(vx, vy, vz);
    HepPoint3D r0(x0, y0, z0);  // MucGeoGeneral::FindIntersection
   
    //Hep3Vector  vSigma(sigmaVx, sigmaVy, sigmaVz);
    //HepPoint3D r0Sigma(sigmaX0, sigmaY0, sigmaZ0);

    //HepPoint3D gCross(0.0, 0.0, 0.0);
    //HepPoint3D gCrossSigma(0.0, 0.0, 0.0);
    //cout <<"in MucGeoGeneral v ro"<< v << r0 << endl;
    //HepPoint3D r = p->ProjectToGapWithSigma(r0, v, r0Sigma, vSigma, gCross, gCrossSigma);
    HepPoint3D r = p->ProjectToGap(r0, v);
    HepPoint3D localIntersection = p->TransformToGap(r);
    //cout << "intersect gap point " << r << " local " << localIntersection << endl;    


    distance = r.distance(r0);

    x = r.x();
    y = r.y();
    z = r.z();
    
    //sigmaX = gCrossSigma.x();
    //sigmaY = gCrossSigma.y();
    //sigmaZ = gCrossSigma.z();
    
    
    // Should be in Gap? 
    // No, return intersection position however. 

    //if ( p->IsInGap(localIntersection.x(),
    //	    localIntersection.y(),
    //	    localIntersection.z()) ) {
    //cout << "found project in gap" << endl;
    //  found = true;
    //}
    //else{
    //cout << " not in gap" << endl;
    //}


  }
  
  else {
    cout << "FindIntersection-E103  bad panel pointer!"
	 << "  Part" << part << " Seg" << seg << " Gap" << gap << endl;
  }
  
  // FIXME:  need a calculation of the uncertainty in the intercept!
  
  return;
}

void
MucGeoGeneral::FindIntersectionQuadLocal(const int part,
                                const int seg,
                                const int gap,
                                const float a,   //y = a*x*x + b*x +c
                                const float b,   
                                const float c,
                                const int whichhalf,
                                float& x1,
                                float& y1,
                                float& z1,
                                float& x2,
                                float& y2,
                                float& z2,
                                float& sigmaX,
                                float& sigmaY,
                                float& sigmaZ)
{
  x1 = 0.0; sigmaX = 0.0; x2 = 0.0;
  y1 = 0.0; sigmaY = 0.0; y2 = 0.0;
  z1 = 0.0; sigmaZ = 0.0; z2 = 0.0;

  if ( (part < 0) || (part >= (int)MucID::getPartNum()) ) {
    cout << "MucGeoGeneral::FindIntersection-E101  invalid part = "
         << part << endl;
    return;
  }

  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    //cout << "MucGeoGeneral::FindIntersection-E102  invalid gap = "
        // << gap << endl;
    return;
  }


  float distance = 1e30;

  MucGeoGap *p = GetGap(part, seg, gap);
  if (p) {

    int orient = 0;
    if(part == 1 && gap%2 == 0) orient = 1;
    if(part != 1 && gap%2 == 1) orient = 1;

    HepPoint3D cross1(0,0,0), cross2(0,0,0);
    //cout <<"in MucGeoGeneral v ro"<< v << r0 << endl;
    HepPoint3D r = p->ProjectToGapQuadLocal(part, orient, a, b, c,whichhalf, cross1, cross2);
    //cout<<"in MucGeoGeneral r_quad = "<<r<<endl;
    HepPoint3D localIntersection = p->TransformToGap(r);
    //cout << "intersect gap point " << r << " local " << localIntersection << endl;    


    x1 = cross1.x();
    y1 = cross1.y();
    z1 = cross1.z();

    x2 = cross2.x();
    y2 = cross2.y();
    z2 = cross2.z();

    // Should be in Gap? 
    // No, return intersection position however. 

    //if ( p->IsInGap(localIntersection.x(),
    //      localIntersection.y(),
    //      localIntersection.z()) ) {
    //cout << "found project in gap" << endl;
    //  found = true;
    //}
    //else{
    //cout << " not in gap" << endl;
    //}
  }

  else {
    cout << "FindIntersection-E103  bad panel pointer!"
         << "  Part" << part << " Seg" << seg << " Gap" << gap << endl;
  }

  return;

}



// Find the intersection position of a trajectory with the given gap.
// The trajectory is given by unit vector (vx,vy,vz) and intercept (x0,y0,z0)
// in global coordinates, such that (x-x0)/vx = (y-y0)/vy = (z-z0)/vz .
// If more than one point lies along the trajectory, take the first one
// intersect with the trajectory.
void
MucGeoGeneral::FindIntersection(const int part,
				const int seg,
				const int gap,
				const float vy,
				const float x0,
				const float y0,
				const float z0,
				const float a,   //y = a*x*x + b*x +c
				const float b,
				const float c,
				const int whichhalf,
				const float,// sigmaVx,
				const float,// sigmaVy,
				const float,// sigmaVz,
				const float,// sigmaX0,
				const float,// sigmaY0,
				const float,// sigmaZ0,
				float& x1,
				float& y1,
				float& z1,
				float& x2,
				float& y2,
				float& z2,
				float& sigmaX,
				float& sigmaY,
				float& sigmaZ)
{
  x1 = 0.0; sigmaX = 0.0; x2 = 0.0;
  y1 = 0.0; sigmaY = 0.0; y2 = 0.0;
  z1 = 0.0; sigmaZ = 0.0; z2 = 0.0; 

  if ( (part < 0) || (part >= (int)MucID::getPartNum()) ) {
    cout << "MucGeoGeneral::FindIntersection-E101  invalid part = "
	 << part << endl;
    return;
  }

  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    //cout << "MucGeoGeneral::FindIntersection-E102  invalid gap = "
	// << gap << endl;
    return;
  }
  
  // "Brute-force" algorithm.
  //   1a. Find intersection with gap.
  //   1b. Transform intersection position to gap coords.
  //   2.  Check intersection position against gap boundaries.
  //       Save the intersection position and gap id if within
  //       boundaries.

  //bool found = false;
  float distance = 1e30;
  
  MucGeoGap *p = GetGap(part, seg, gap);
  if (p) {
    
    HepPoint3D r0(x0, y0, z0);  // MucGeoGeneral::FindIntersection
    

    
    HepPoint3D cross1(0,0,0), cross2(0,0,0);
    //cout <<"in MucGeoGeneral v ro"<< v << r0 << endl;
    HepPoint3D r = p->ProjectToGap(r0, vy, y0, a, b, c,whichhalf, cross1, cross2);
    //cout<<"in MucGeoGeneral r_quad = "<<r<<endl;
    HepPoint3D localIntersection = p->TransformToGap(r);
    //cout << "intersect gap point " << r << " local " << localIntersection << endl;    

    distance = r.distance(r0);

    x1 = cross1.x();
    y1 = cross1.y();
    z1 = cross1.z();

    x2 = cross2.x();
    y2 = cross2.y();
    z2 = cross2.z();
    
    // Should be in Gap? 
    // No, return intersection position however. 

    //if ( p->IsInGap(localIntersection.x(),
    //	    localIntersection.y(),
    //	    localIntersection.z()) ) {
    //cout << "found project in gap" << endl;
    //  found = true;
    //}
    //else{
    //cout << " not in gap" << endl;
    //}
  }
  
  else {
    cout << "FindIntersection-E103  bad panel pointer!"
	 << "  Part" << part << " Seg" << seg << " Gap" << gap << endl;
  }
  
  // FIXME:  need a calculation of the uncertainty in the intercept!
  
  return;
}


// Find the intersection position of a trajectory with two surface of the given gap.
// The trajectory is given by unit vector (vx,vy,vz) and intercept (x0,y0,z0)
// in global coordinates, such that (x-x0)/vx = (y-y0)/vy = (z-z0)/vz .
// If more than one point lies along the trajectory, take the first one
// intersect with the trajectory.
void
MucGeoGeneral::FindIntersectionSurface(const int part,
				       const int seg,
				       const int gap,
				       const float vx,
				       const float vy,
				       const float vz,
				       const float x0,
				       const float y0,
				       const float z0,
				       const float,// sigmaVx,
				       const float,// sigmaVy,
				       const float,// sigmaVz,
				       const float,// sigmaX0,
				       const float,// sigmaY0,
				       const float,// sigmaZ0,
				       float& x1,
				       float& y1,
				       float& z1,
				       float& x2,
				       float& y2,
				       float& z2,
				       float& sigmaX1,
				       float& sigmaY1,
				       float& sigmaZ1,
				       float& sigmaX2,
				       float& sigmaY2,
				       float& sigmaZ2)
{
  x1 = 0.0; sigmaX1 = 0.0;
  y1 = 0.0; sigmaY1 = 0.0;
  z1 = 0.0; sigmaZ1 = 0.0;
  x2 = 0.0; sigmaX2 = 0.0;
  y2 = 0.0; sigmaY2 = 0.0;
  z2 = 0.0; sigmaZ2 = 0.0;

  if ( (part < 0) || (part >= (int)MucID::getPartNum()) ) {
    cout << "MucGeoGeneral::FindIntersection-E101  invalid part = "
	 << part << endl;
    return;
  }

  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    //cout << "MucGeoGeneral::FindIntersection-E102  invalid gap = "
	// << gap << endl;
    return;
  }
  
  // "Brute-force" algorithm.
  //   1a. Find intersection with gap.
  //   1b. Transform intersection position to gap coords.
  //   2.  Check intersection position against gap boundaries.
  //       Save the intersection position and gap id if within
  //       boundaries.

  //bool found = false;
  float distance = 1e30;
  
  MucGeoGap *p = GetGap(part, seg, gap);
  if (p) {
    
    Hep3Vector  v(vx, vy, vz);
    HepPoint3D r0(x0, y0, z0);  // MucGeoGeneral::FindIntersection
    

    //cout <<"in MucGeoGeneral v ro"<< v << r0 << endl;

    HepPoint3D cross1, cross2;
    p->ProjectToGapSurface(r0, v, cross1, cross2);

    x1 = cross1.x();
    y1 = cross1.y();
    z1 = cross1.z();
    x2 = cross2.x();
    y2 = cross2.y();
    z2 = cross2.z();

    // Should be in Gap? 
    // No, return intersection position however. 

    //if ( p->IsInGap(localIntersection.x(),
    //	    localIntersection.y(),
    //	    localIntersection.z()) ) {
    //cout << "found project in gap" << endl;
    //  found = true;
    //}
    //else{
    //cout << " not in gap" << endl;
    //}
  }
  
  else {
    cout << "FindIntersection-E103  bad panel pointer!"
	 << "  Part" << part << " Seg" << seg << " Gap" << gap << endl;
  }
  
  // FIXME:  need a calculation of the uncertainty in the intercept!
  
  return;
}



void
MucGeoGeneral::FindIntersectionSurface(const int part,
				       const int seg,
				       const int gap,
				       const float vy,
				       const float x0,
				       const float y0,
				       const float z0,
				       const float a,   //y = a*x*x + b*x +c
				       const float b,
				       const float c,
				       const int whichhalf,
				       const float,// sigmaVx,
				       const float,// sigmaVy,
				       const float,// sigmaVz,
				       const float,// sigmaX0,
				       const float,// sigmaY0,
				       const float,// sigmaZ0,
				       float& x1,
				       float& y1,
				       float& z1,
				       float& x2,
				       float& y2,
				       float& z2,
				       float& sigmaX,
				       float& sigmaY,
				       float& sigmaZ)
{
  x1 = 0.0; sigmaX = 0.0; x2 = 0.0;
  y1 = 0.0; sigmaY = 0.0; y2 = 0.0;
  z1 = 0.0; sigmaZ = 0.0; z2 = 0.0; 

  if ( (part < 0) || (part >= (int)MucID::getPartNum()) ) {
    cout << "MucGeoGeneral::FindIntersection-E101  invalid part = "
	 << part << endl;
    return;
  }

  if ( (gap < 0) || (gap >= (int)MucID::getGapNum(part)) ) {
    //cout << "MucGeoGeneral::FindIntersection-E102  invalid gap = "
	// << gap << endl;
    return;
  }
  
  // "Brute-force" algorithm.
  //   1a. Find intersection with gap.
  //   1b. Transform intersection position to gap coords.
  //   2.  Check intersection position against gap boundaries.
  //       Save the intersection position and gap id if within
  //       boundaries.

  //bool found = false;
  float distance = 1e30;
  
  MucGeoGap *p = GetGap(part, seg, gap);
  if (p) {
    
    HepPoint3D r0(x0, y0, z0);  // MucGeoGeneral::FindIntersection
    

    
    HepPoint3D cross1(0,0,0), cross2(0,0,0);
    p->ProjectToGapSurface(r0, vy, y0, a, b, c,whichhalf, cross1, cross2);


    x1 = cross1.x();
    y1 = cross1.y();
    z1 = cross1.z();

    x2 = cross2.x();
    y2 = cross2.y();
    z2 = cross2.z();
    
    // Should be in Gap? 
    // No, return intersection position however. 

    //if ( p->IsInGap(localIntersection.x(),
    //	    localIntersection.y(),
    //	    localIntersection.z()) ) {
    //cout << "found project in gap" << endl;
    //  found = true;
    //}
    //else{
    //cout << " not in gap" << endl;
    //}
  }
  
  else {
    cout << "FindIntersection-E103  bad panel pointer!"
	 << "  Part" << part << " Seg" << seg << " Gap" << gap << endl;
  }
  
  // FIXME:  need a calculation of the uncertainty in the intercept!
  
  return;
}
