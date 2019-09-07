//------------------------------------------------------------------------------|
//      [File  ]:                       MucStripPlane.h                         |
//      [Brief ]:       Head file of MUC geometry of StripPlane class 		|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_STRIP_PLANE_H
#define MUC_STRIP_PLANE_H

#include<iostream>

#include "MucGeoCreateAlg/MucEntity.h"
#include "MucGeoCreateAlg/MucStripPlane.h"
#include "MucGeoCreateAlg/MucStrip.h"

using namespace std;

class MucStripPlane : public MucEntity
{
	public:
		MucStripPlane( int part, int segment, int layer, int id );
		MucStripPlane( const MucStripPlane &other );
		MucStripPlane& operator =( const MucStripPlane &other );
		virtual ~MucStripPlane();
		
	virtual void	SetAlignment( double dx, double dy, double dz );

	   MucStrip* 	GetStrip( int id );

	protected:			
        virtual void    Init();

	virtual	void    SetTheta();
        virtual	void    SetRc();
        virtual	void    SetRin();
        virtual	void    SetRout();

        virtual void    SetThin();
        virtual void    SetW();
        virtual void    SetH();
        virtual void    SetL();
        virtual void    SetWu();
        virtual void    SetWd();

        virtual void    SetLocOrgInBes();
        virtual void    SetObjOrgInBes();
        virtual void    SetObjOrgInLoc();
	
	private:

		MucStrip*  m_MucStrip; 

};

#endif

