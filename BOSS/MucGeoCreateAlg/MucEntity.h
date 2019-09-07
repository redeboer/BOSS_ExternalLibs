//------------------------------------------------------------------------------|
//      [File  ]:                       MucEntity.h                             |
//      [Brief ]:       Head file of MUC geometry entity class 			|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_ENTITY_H
#define MUC_ENTITY_H

#include<iostream>

//#include "CLHEP/Geometry/Vector3D.h"
//#include "CLHEP/Geometry/Point3D.h"

using namespace std;

class MucEntity 
{
	public:
		MucEntity( int part, int segment, int layer ); 
		MucEntity( int part, int segment, int layer, int id );
		MucEntity( int part, int segment, int layer, int upDown, int id );
		MucEntity( int part, int segment, int layer, int upDown, int rpcId, int id );
		MucEntity( const MucEntity &other );
		MucEntity &operator =( const MucEntity &other );
		virtual	~MucEntity() ;
		
		int	GetPart();
		int	GetSegment();
		int	GetLayer();
		int	GetUpDown();
		int 	GetRpcId();
		int	GetID();

		double	GetTheta();
		double  GetRin();
		double 	GetRout();
		double  GetRc();
		double  GetThin();
		double	GetW();
		double  GetH();
		double  GetL();
		double  GetWu();
		double  GetWd();
		double	GetArea();
		
		double  GetLocOrgInBes( int i );       	// 1: Bes_x, 2: Bes_y, 3, Bes_z
                double  GetObjRotToMot( int i );       	// 1: Rot_x, 2: Rot_y, 3, Rot_z
                double  GetObjOrgInBes( int i );	// 1: Bes_x, 2: Bes_y, 3, Bes_z
                double  GetObjOrgInLoc( int i );       	// 1: Pos_x, 2: Pos_y, 3, Pos_z

		double*	GetLocOrgInBes();	
		double* GetObjRotToMot();	
		double* GetObjOrgInLoc();
		double* GetObjOrgInBes();
	
		// coordinate transform function, from old to new, 
		// Rot_z is the direction angle of new x axis in old coordinate system
		void	TransBesToLoc( double LocCoord[], double BesCoord[], double LocOrgInBes[], double Rot_z );		
		void 	TransLocToBes( double LocCoord[], double BesCoord[], double LocOrgInBes[], double Rot_z ); 
	
	protected:

	virtual	void	Init();			// all geometry information initilization 
	
	virtual	void	SetTheta();		// set theta
	virtual void	SetRin();		// set r inner
	virtual void	SetRout();		// set r outer
	virtual void 	SetRc();		// set r center	
	virtual void    SetThin();		// set thickness
	virtual void	SetW();			// set width  
	virtual void    SetH();			// set height 
	virtual void    SetL();			// set stretching length 
	virtual void    SetWu();		// set up width  
	virtual void    SetWd();		// set down width 
	virtual	void	SetArea();		// set area of entity(mm^2)
	virtual	void    SetLocOrgInBes();	// set x, y, z of local origin in BES coordinate system
	virtual	void	SetObjRotToMot();	// set rotation angles of x, y, z axises of local coordinate system 
						// relative to BES 
	virtual void	SetObjOrgInBes();	// set x, y and z of object origin in BES coordinate system 
	virtual	void	SetObjOrgInLoc();	// set x, y and z of object origin in local or 
						// temporary local coordinate system

	virtual void	SetAlignment( double dx, double dy, double dz); // set alignment 

	protected:
		int	m_Part;			// part id
		int	m_Segment;		// segment id
		int	m_Layer;		// layer id
		int	m_UpDown;		// up or down superlayer id, only valid for superlayer objects
		int 	m_RpcId;		// rpc id in up or down superlayer
		int	m_ID;			// object id

		double  m_Theta;		// barrel: inclination angle of R and Bes_X
	                                        // endcap: inclination angle of panel boudary and Bes_X

		double	m_Rin;			// r of the inner surface relative to BES origin
		double  m_Rout;			// r of the outer surface relative to BES origin
		double  m_Rc;			// r of the center surface relative to BES orign	
		double	m_Thin;			// thickness of entity		
		double  m_W;			// width of cross section
		double  m_H;			// height of cross section
		double  m_L;			// stretching length of cross section
		double  m_Wu;			// up width of trapezoid cross section
		double  m_Wd;			// down width of trapezoid cross section
		double  m_Area;			// area of entity,mm^2
		
		double  m_LocOrgInBes[3];	// x, y, z of the mother origin in BES coordinate system
						// for panel, the mother is temporary local coordinate system
		double	m_ObjRotToMot[3];	// rotation angles of x, y, z axises of the object coordinate system 
						// relative to the mother 
                                                // only rotation angle relative to Bes_z axis is useful
                                                // defined as the angle of local xy plane circling mother's z axis, 
						// not Euler angle 
						// for local or temparory local, the mother is BES, for panel, 
						// the mother is the local or temparory local
		double  m_ObjOrgInBes[3];	// x, y and z of object origin in BES coordinate system
		double  m_ObjOrgInLoc[3];	// x, y and z of object origin in its mother coordinate system
						// for panel, the mother is temporary local coordinate system

//-------------------------------------------------------------NOTE---------------------------------------------------------------
// 1:	There are four coordinate systems: BES(the world), local(the mother), temporary local(the temp, for many panels object) 
//	and object, the posterior one is always located in the anterior one.
// 2:	If the local coordinate system of one entity is BES, m_MotOrgInBes is unuseful. 
// 3:	According to the detector structure, the origin coordinates and rotation angles of geometry entities relative to BES 
//	coordinate system can be calculated easily and conveniently, so these values will be initialized first in constructor.
// 4:	For objects having many panels, the id of integral object is -1, and the id of panel begins from 0 to panel number.
//	These objects must be added a temporary local coordinate system for locating the panels, the temporary local  	  

};

#endif

