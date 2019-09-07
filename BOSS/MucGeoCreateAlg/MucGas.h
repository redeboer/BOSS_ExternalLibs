//------------------------------------------------------------------------------|
//      [File  ]:                       MucGas.h                         	|
//      [Brief ]:       Head file of MUC geometry  gas class 			|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_GAS_H
#define MUC_GAS_H

#include<iostream>

#include "MucGeoCreateAlg/MucEntity.h"
#include "MucGeoCreateAlg/MucGas.h"

using namespace std;

class MucGas : public MucEntity
{
	public:
		MucGas( int part, int segment, int layer, int upDown, int rpcId, int id );
		MucGas( const MucGas &other );
		MucGas& operator =( const MucGas &other );
		~MucGas();
		
		double 	GetPhi();

//	virtual	void	SetAlignment( double dx, double dy, double dz );

	protected:			
        virtual void    Init();

        virtual	void    SetTheta();
		void	SetPhi();
        virtual	void    SetRc();
        virtual	void    SetRin();
        virtual	void    SetRout();

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

