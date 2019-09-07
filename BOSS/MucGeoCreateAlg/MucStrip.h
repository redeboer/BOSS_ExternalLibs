//------------------------------------------------------------------------------|
//      [File  ]:                       MucStrip.h  	                        |
//      [Brief ]:       Head file of class MucStrip for MUC calibration         |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 25, 2006                                            |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_STRIP_H
#define MUC_STRIP_H

#include<iostream>

#include "MucGeoCreateAlg/MucEntity.h"
// #include "MucGeoCreateAlg/MucPadChain.h"

using namespace std;


// constants definition
// any change should be confirmed according to the design
const int HEAD_TURN_STR_NUM[8][2]      	= { {12,30},{12,31},{12,32},{13,33},{13,34},{14,35},{14,37},{15,39} };
const double FIRST_TURN_HEAD[8][2] 	= { {1092.5,1062.5}, {1126,1120}, {1156,1158.5}, {1193,1179.5},
                                            {1228.5,1220}, {1267,1249}, {1327.5,1325}, {1388,1385} };
const double SECOND_TURN_HEAD[2]    	= {56,66}; // odd layer: 56, even layer: 66;
const int TAIL_TURN_STR_NUM[2] 		= {26,27};
const double ODD_TURN_POINT_TAIL[2][2]  = { {2317,2304},{2328,2321} };
const double EVEN_TURN_POINT_TAIL[2]    = {2329,2296};
const double E_FIRST_STR_WT		= 48.0;

//-------------------------------Class definition -------------------------------
class MucStrip : public MucEntity
{
	public:

		MucStrip( int part, int segment, int layer,int id ); 
		MucStrip( const MucStrip &other );
		MucStrip& operator =( const MucStrip &other );
		virtual ~MucStrip(); 
	
		int 	GetType();	
		double 	GetCenterLine();
		double 	GetHead();
		double 	GetTail();
		double 	GetPhi();
		int	GetPadNumber();

//		MucPadChain*	GetPadChain();
		
	protected:

        virtual void    Init();

		void	SetType();
		void	SetCenterLine();
		void	SetHead();
		void	SetTail();
		void	SetPhi();

        virtual void    SetThin();
        virtual void    SetW();
        virtual void    SetWu();
        virtual void    SetWd();
        virtual void    SetH();
        virtual void    SetL();
	virtual void    SetArea();
		void	SetPadNumber();
	
		void	SetEndcapStripInBes();
		void 	SetBarrelStripInLoc();

        virtual void    SetLocOrgInBes();
//        virtual void    SetObjRotToMot();
        virtual void    SetObjOrgInBes();
        virtual void    SetObjOrgInLoc();
	
	private:
   	       
   	        // Geometry info 
		int	m_Type;		// strip readout type
                double  m_CenterLine; 	// strip center line position, endcap only
                double  m_Head;       	// strip head coordinate, endcap only
                double  m_Tail;       	// strip tail coordinate, endcap only
		double  m_Phi;		// the inclination angle of local x axis and BES x axis
		int  	m_PadNumber;	// the number of pads in the strip

//		MucPadChain* m_MucPadChain;

		
};

#endif
