//------------------------------------------------------------------------------|
//      [File  ]:                       MucBoxCover.h                           |
//      [Brief ]:       Head file of MUC geometry BoxCover class 		|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_BOXCOVER_H
#define MUC_BOXCOVER_H

#include<iostream>

#include "MucGeoCreateAlg/MucEntity.h"

using namespace std;

class MucBoxCover : public MucEntity
{
	public:
		MucBoxCover( int part, int segment, int layer, int upDown, int id );
		MucBoxCover( const MucBoxCover &other );
		MucBoxCover& operator =( const MucBoxCover &other );
		~MucBoxCover();
		
//	virtual	void	SetAlignment( double dx, double dy, double dz );

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

        virtual void    SetLocOrgInBes();
        virtual void    SetObjOrgInBes();
        virtual void    SetObjOrgInLoc();
};

#endif

