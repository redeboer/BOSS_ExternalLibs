//------------------------------------------------------------------------------|
//      [File  ]:                       MucAbsorber.h                           |
//      [Brief ]:       Head file of MUC geometry absorber class 		|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_ABSORBER_H
#define MUC_ABSORBER_H

#include<iostream>

#include "MucGeoCreateAlg/MucEntity.h"

using namespace std;

class MucAbsorber : public MucEntity
{
	public:
		MucAbsorber( int part, int segment, int layer, int id );
		MucAbsorber( const MucAbsorber &other );
		MucAbsorber &operator =( const MucAbsorber &other );
		virtual ~MucAbsorber();
	
	protected:
	virtual void	Init();

	virtual	void 	SetTheta();
        virtual	void    SetRin();
        virtual	void    SetRout();
        virtual	void    SetRc();

	virtual void 	SetThin();	
	virtual void	SetW();
        virtual void    SetH();
	virtual	void    SetL();
        virtual void    SetWu();
        virtual void    SetWd();
	virtual void    SetLocOrgInBes();
        virtual void    SetObjRotToMot();
        virtual void    SetObjOrgInBes();
        virtual void    SetObjOrgInLoc();
			
};

#endif

