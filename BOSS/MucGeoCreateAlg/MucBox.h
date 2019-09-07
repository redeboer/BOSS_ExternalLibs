//------------------------------------------------------------------------------|
//      [File  ]:                       MucBox.h                                |
//      [Brief ]:       Head file of MUC geometry gap of Box class 		|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_BOX_H
#define MUC_BOX_H

#include<iostream>

#include "MucGeoCreateAlg/MucEntity.h"
#include "MucGeoCreateAlg/MucStripPlane.h"
#include "MucGeoCreateAlg/MucRpc.h"
#include "MucGeoCreateAlg/MucBoxCover.h"

using namespace std;

class MucBox : public MucEntity
{
	public:
		MucBox( int part, int segment, int layer, int id );
		MucBox( const MucBox &other );
		MucBox& operator =( const MucBox &other );
		~MucBox();
		
	virtual	void	SetAlignment( double dx, double dy, double dz );

		MucStripPlane* 	GetStripPlane();
                MucRpc* 	GetRpc( int upDown, int id );
                MucBoxCover* 	GetBoxCover( int upDown, int id );

	protected:			
        	
	virtual	void    Init();

        virtual	void    SetTheta();
        virtual	void    SetRin();
        virtual	void    SetRout();
        virtual	void    SetRc();

        virtual void    SetThin();
        virtual void    SetW();
        virtual void    SetH();
        virtual void    SetL();
        virtual void    SetWu();
        virtual void    SetWd();
	virtual void	SetArea();

        virtual void    SetLocOrgInBes();
        virtual void    SetObjRotToMot();
        virtual void    SetObjOrgInBes();
        virtual void    SetObjOrgInLoc();
	
	private:

		MucStripPlane*  m_MucStripPlane; 
		MucRpc*		m_MucRpc;
		MucBoxCover*	m_MucBoxCover;

};

#endif

