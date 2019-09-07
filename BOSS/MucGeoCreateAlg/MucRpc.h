//------------------------------------------------------------------------------|
//      [File  ]:                       MucRpc.h                         	|
//      [Brief ]:       Head file of MUC geometry  Rpc class 			|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_RPC_H
#define MUC_RPC_H

#include<iostream>

#include "MucGeoCreateAlg/MucEntity.h"
#include "MucGeoCreateAlg/MucRpc.h"
#include "MucGeoCreateAlg/MucGas.h"
#include "MucGeoCreateAlg/MucBakelite.h"

using namespace std;

class MucRpc : public MucEntity
{
	public:
		MucRpc( int part, int segment, int layer, int upDown, int id );
		MucRpc( const MucRpc &other );
		MucRpc& operator =( const MucRpc &other );
		virtual ~MucRpc();
		
	      double  	GetPhi();
	     MucGas* 	GetGas();
	MucBakelite* 	GetBakelite( int id );

//	virtual void	SetAlignment( double dx, double dy, double dz );

	protected:			
        virtual void    Init();

	        void	SetPhi();
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

		int	SetEndcapRpcInBes();
		int	SetBarrelRpcInLoc();

        virtual void    SetLocOrgInBes();
        virtual void    SetObjOrgInBes();
        virtual void    SetObjOrgInLoc();
	
	private:
	   
	       double 	m_Phi;		// inclination angle of BES x axis and Loc x axis
	      MucGas*  	m_MucGas;
         MucBakelite*  	m_MucBakelite; 

};

#endif

