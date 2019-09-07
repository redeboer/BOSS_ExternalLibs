//------------------------------------------------------------------------------|
//      [File  ]:                       MucEntity.cxx                           |
//      [Brief ]:       MUC geometry entity class                      		|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Mar 28, 2006                                            |
//------------------------------------------------------------------------------|

#include<iostream>
#include<string>
#include<cmath>

#include "MucGeoCreateAlg/MucEntity.h"

using namespace std;

// Constructor
MucEntity::MucEntity( int part, int segment, int layer )
{
        m_Part          = part;
        m_Segment       = segment;
        m_Layer         = layer;
        m_UpDown        = -1;           // no up/down distinction
	m_RpcId		= 0;
        m_ID            = 0;		// only 1 entity, default

	MucEntity::Init();
}

MucEntity::MucEntity( int part, int segment, int layer, int id )
{
	m_Part 		= part;
	m_Segment 	= segment;
	m_Layer 	= layer;
	m_ID		= id;
	m_UpDown	= -1;		
	m_RpcId		= 0;

	Init();
}

MucEntity::MucEntity( int part, int segment, int layer, int upDown, int id )
{
        m_Part          = part;
        m_Segment       = segment;
        m_Layer         = layer;
        m_UpDown        = upDown;          
        m_ID            = id;
	m_RpcId		= id;

	MucEntity::Init();
}

MucEntity::MucEntity( int part, int segment, int layer, int upDown, int rpcId, int id )
{
        m_Part          = part;
        m_Segment       = segment; 
        m_Layer         = layer;
        m_UpDown        = upDown;           
        m_RpcId         = rpcId;
        m_ID            = id;

        MucEntity::Init();
}

MucEntity::MucEntity( const MucEntity &other )
{
	m_Part          = other.m_Part;
        m_Segment       = other.m_Segment;
       	m_Layer         = other.m_Layer;
	m_UpDown	= other.m_UpDown;
	m_RpcId		= other.m_RpcId;
	m_ID		= other.m_ID;
	
	m_Theta		= other.m_Theta;
	m_Thin		= other.m_Thin;
        m_W             = other.m_W;
        m_H             = other.m_H;
        m_L             = other.m_L;
        m_Wu            = other.m_Wu;
        m_Wd            = other.m_Wd;
	m_Area		= other.m_Area;

	 for(int i=0; i<3; i++)
        {
		m_LocOrgInBes[i] = other.m_LocOrgInBes[i];
		m_ObjRotToMot[i] = other.m_ObjRotToMot[i];
                m_ObjOrgInBes[i] = other.m_ObjOrgInBes[i];
                m_ObjOrgInLoc[i] = other.m_ObjOrgInLoc[i];
        }
}

MucEntity& MucEntity::operator =( const MucEntity &other )
{
	if( this == &other)
		return *this;
	
	m_Part          = other.m_Part;
        m_Segment       = other.m_Segment;
        m_Layer         = other.m_Layer;
        m_UpDown        = other.m_UpDown;
	m_RpcId		= other.m_RpcId;
        m_ID            = other.m_ID;

        m_Theta         = other.m_Theta;	
        m_Thin          = other.m_Thin;	
	m_W		= other.m_W;
	m_H		= other.m_H;
	m_L		= other.m_L;
	m_Wu		= other.m_Wu;
	m_Wd		= other.m_Wd;
	m_Area          = other.m_Area;

	  for(int i=0; i<3; i++)
        {
		m_LocOrgInBes[i] = other.m_LocOrgInBes[i];
                m_ObjRotToMot[i] = other.m_ObjRotToMot[i];
                m_ObjOrgInBes[i] = other.m_ObjOrgInBes[i];
                m_ObjOrgInLoc[i] = other.m_ObjOrgInLoc[i];
        }

	return *this;
}

// Destructor
MucEntity::~MucEntity()
{
	delete [] m_LocOrgInBes;
	delete [] m_ObjRotToMot;
	delete [] m_ObjOrgInBes;
	delete [] m_ObjOrgInLoc;
}

// Get methods
int MucEntity::GetPart() 	{ return m_Part; 	}
int MucEntity::GetSegment() 	{ return m_Segment; 	}
int MucEntity::GetLayer() 	{ return m_Layer; 	}
int MucEntity::GetUpDown() 	{ return m_UpDown; 	}
int MucEntity::GetRpcId()	{ return m_RpcId;	}
int MucEntity::GetID() 		{ return m_ID; 		}
double MucEntity::GetTheta()	{ return m_Theta;	}
double MucEntity::GetRin()      { return m_Rin;         }
double MucEntity::GetRout()     { return m_Rout;        }
double MucEntity::GetRc()       { return m_Rc;          }
double MucEntity::GetThin() 	{ return m_Thin; 	} 
double MucEntity::GetW() 	{ return m_W; 		}
double MucEntity::GetH()	{ return m_H; 		}
double MucEntity::GetL() 	{ return m_L; 		}
double MucEntity::GetWu()	{ return m_Wu;		}
double MucEntity::GetWd()	{ return m_Wd;		}
double MucEntity::GetArea()	{ return m_Area;	}

double MucEntity::GetLocOrgInBes( int i )
{
	if( i<1 || i>3 ) i=1;
        return m_LocOrgInBes[i-1];
}

double MucEntity::GetObjRotToMot( int i )
{
	if( i<1 || i>3 ) i=1;
        return m_ObjRotToMot[i-1];
}

double MucEntity::GetObjOrgInBes( int i )
{
        if( i<1 || i>3 ) i=1;
        return m_ObjOrgInBes[i-1];
}

double MucEntity::GetObjOrgInLoc( int i )
{
        if( i<1 || i>3 ) i=1;
        return m_ObjOrgInLoc[i-1];
}

double* MucEntity::GetLocOrgInBes() { return &m_LocOrgInBes[0]; }
double* MucEntity::GetObjRotToMot() { return &m_ObjRotToMot[0]; }
double* MucEntity::GetObjOrgInBes() { return &m_ObjOrgInBes[0]; }
double* MucEntity::GetObjOrgInLoc() { return &m_ObjOrgInLoc[0]; }


// Set methods
void MucEntity::Init()
{
	MucEntity::SetTheta();
	MucEntity::SetRin();
	MucEntity::SetRout();
	MucEntity::SetRc();
	MucEntity::SetThin();
	MucEntity::SetW();
	MucEntity::SetWu();
	MucEntity::SetWd();
	MucEntity::SetH();
	MucEntity::SetL();
	MucEntity::SetArea();
	MucEntity::SetLocOrgInBes();
	MucEntity::SetObjRotToMot();
	MucEntity::SetObjOrgInBes();
	MucEntity::SetObjOrgInLoc();
}

void MucEntity::SetTheta(){ m_Theta     = 0.;   }
void MucEntity::SetRin()  { m_Rin       = 0.;   }
void MucEntity::SetRout() { m_Rout      = 0.;   }
void MucEntity::SetRc()   { m_Rc      	= 0.;   }
void MucEntity::SetThin() { m_Thin 	= 0.; 	}
void MucEntity::SetW()    { m_W		= 0.; 	}
void MucEntity::SetWu()	  { m_Wu 	= 0.; 	}
void MucEntity::SetWd()	  { m_Wd 	= 0.; 	}
void MucEntity::SetH()    { m_H 	= 0.; 	}
void MucEntity::SetL()	  { m_L 	= 0.; 	}
void MucEntity::SetArea() { m_Area	= 0.;	}

void MucEntity::SetLocOrgInBes() { for( int i=0; i<3; i++) m_LocOrgInBes[i] = 0.; }
void MucEntity::SetObjRotToMot() { for( int i=0; i<3; i++) m_ObjRotToMot[i] = 0.; }
void MucEntity::SetObjOrgInBes() { for( int i=0; i<3; i++) m_ObjOrgInBes[i] = 0.; }
void MucEntity::SetObjOrgInLoc() { for( int i=0; i<3; i++) m_ObjOrgInLoc[i] = 0.; }

void MucEntity::SetAlignment( double dx, double dy, double dz ) { ; }

// Coordinate transform function, from global(BES) to local 
// Z coordinate is parellel, phi is the inclination angle of the BES coordinate x direction and the Loc one
void MucEntity::TransBesToLoc( double LocCoord[], double BesCoord[], double LocOrgInBes[], double Rot_z )
{
       LocCoord[0] = (BesCoord[0] - LocOrgInBes[0])*cos(Rot_z) + (BesCoord[1] - LocOrgInBes[1])*sin(Rot_z);
       LocCoord[1] = -(BesCoord[0] - LocOrgInBes[0])*sin(Rot_z) + (BesCoord[1] - LocOrgInBes[1])*cos(Rot_z);
       LocCoord[2] = BesCoord[2] - LocOrgInBes[2];
}

// Coordinate transform function, from local to global(BES)
// Z coordinate is parellel, phi is the inclination angle of the BES coordinate x direction and the Loc one 
void MucEntity::TransLocToBes( double BesCoord[], double LocCoord[], double LocOrgInBes[], double Rot_z )
{
	BesCoord[0] = LocCoord[0]*cos(Rot_z) - LocCoord[1]*sin(Rot_z) + LocOrgInBes[0];
	BesCoord[1] = LocCoord[0]*sin(Rot_z) + LocCoord[1]*cos(Rot_z) + LocOrgInBes[1];
	BesCoord[2] = LocCoord[2] + LocOrgInBes[2];
}


// END


