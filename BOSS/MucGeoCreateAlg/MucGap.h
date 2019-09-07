//------------------------------------------------------------------------------|
//      [File  ]:                       MucGap.h                                |
//      [Brief ]:       Head file of MUC geometry gap of Gap class 		|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_GAP_H
#define MUC_GAP_H

#include<iostream>

#include "MucGeoCreateAlg/MucEntity.h"
#include "MucGeoCreateAlg/MucBox.h"

using namespace std;

class MucGap : public MucEntity
{
	public:
		MucGap( int part, int segment, int layer, int id );
		MucGap( const MucGap &other );
		MucGap& operator =( const MucGap &other );
		~MucGap();
		
		MucBox* GetBox();

	protected:	
		
        virtual void    Init();

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
        virtual void    SetObjRotToMot();
        virtual void    SetObjOrgInBes();
        virtual void    SetObjOrgInLoc();
	
	private:

		MucBox* m_MucBox; 
};

#endif

