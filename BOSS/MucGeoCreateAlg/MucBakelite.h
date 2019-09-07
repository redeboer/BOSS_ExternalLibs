//------------------------------------------------------------------------------|
//      [File  ]:                       MucBakelite.h                         	|
//      [Brief ]:       Head file of MUC geometry  bakelite class 		|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_BAKELITE_H
#define MUC_BAKELITE_H

#include<iostream>

#include "MucGeoCreateAlg/MucEntity.h"
#include "MucGeoCreateAlg/MucBakelite.h"

using namespace std;

class MucBakelite : public MucEntity
{
	public:
		MucBakelite( int part, int segment, int layer, int upDown, int rpcId, int id );
		MucBakelite( const MucBakelite &other );
		MucBakelite& operator =( const MucBakelite &other );
		~MucBakelite();
		
		double	GetPhi();

//	virtual	void	SetAlignment( double* offset );

	protected:			

        virtual void    Init();

	virtual	void    SetTheta();
	virtual	void	SetPhi();
        virtual	void    SetRin();
        virtual	void    SetRout();
        virtual	void    SetRc();

        virtual void    SetThin();
        virtual void    SetW();
        virtual void    SetH();
        virtual void    SetL();
        virtual void    SetWu();
        virtual void    SetWd();

		int 	SetEndcapRpcInBes();	
		int	SetBarrelRpcInBes();

        virtual void    SetLocOrgInBes();
        virtual void    SetObjOrgInLoc();
        virtual void    SetObjOrgInBes();
	

	private:

		double  m_Phi;		// the inclination angle of local x axis and BES x axis for coordinates transformation
};

#endif

