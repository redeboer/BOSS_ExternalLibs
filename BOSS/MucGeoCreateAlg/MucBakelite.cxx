//------------------------------------------------------------------------------|
//      [File  ]:                       MucBakelite.cxx 	                |
//      [Brief ]:       MUC geometry bakelite class	 	  	   	|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#include <iostream>
#include <cmath>

#include "MucGeoCreateAlg/MucGeoConst.h"
#include "MucGeoCreateAlg/MucBakelite.h"

using namespace std;

// Constructor
MucBakelite::MucBakelite( int part, int segment, int layer, int upDown, int rpcId, int id ) 
	: MucEntity( part, segment, layer, upDown, rpcId, id )
{
	MucBakelite::Init();
}

// Copy constructor		
MucBakelite::MucBakelite( const MucBakelite &other ) : MucEntity( other )
{
	m_Phi		= other.m_Phi;
}

// Operator =
MucBakelite& MucBakelite::operator =( const MucBakelite &other )
{
	if( this == &other)
                return *this;

	MucEntity::operator =(other);

	m_Phi           = other.m_Phi;

        return *this;	
}

// Destructor
MucBakelite::~MucBakelite() { ; }

// Initialize
void MucBakelite::Init()
{
	SetPhi();
        SetTheta();
        SetRin();
        SetRout();
        SetRc();

        SetThin();
        SetW();
        SetWu();
        SetWd();
        SetH();
        SetL();

	SetEndcapRpcInBes();
	SetBarrelRpcInBes();

        SetLocOrgInBes();
        SetObjOrgInLoc();
        SetObjOrgInBes();
}
		
// -------------------------- Get methods ----------------------------

double MucBakelite::GetPhi()		{ 	 return m_Phi;		}

// ---------------------------Set motheds -----------------------------

// Set inclination angle of local coordinate x axis and global coordinate x axis
void MucBakelite::SetPhi()
{
        if( m_Part == BRID )
                m_Phi = B_PHI[m_Segment] * (MUC_PI/4.0);
        else
                m_Phi = 0.;
}

void MucBakelite::SetTheta()
{
        if( m_Part == BRID )
                m_Theta = m_Segment * (MUC_PI/4.0);
        else
        {
                if( m_UpDown == SL_UP )
                        m_Theta = ( MUC_PI/4.0 ) + (1 - m_RpcId) * MUC_PI / 8.0 ;
                else
		{
                        if( m_RpcId < 2 ) // RPC NO.1 and NO.2 
                                m_Theta = ( MUC_PI/4.0 ) + MUC_PI / 8.0 ;
                        else if ( m_RpcId == 2 ) // center RPC, NO.3
                                m_Theta = ( MUC_PI/4.0 ) ;
                        else
                                m_Theta = ( MUC_PI/4.0 ) - MUC_PI / 8.0 ;
		}
        }
}

void MucBakelite::SetRin()
{
        if( m_Part == BRID ) // Rin of bakelite
                m_Rin = B_AS_RMIN[m_Layer] - AS_GAP/2.0 + (1-2*m_UpDown) * (STR_TH + RPC_TH)/2.0 
			+ (1-2*m_ID)*(GAS_TH + BKLT_TH)/2.0 - BKLT_TH/2.0;
        else	// Rin of RPC
		m_Rin = E_GP_RMIN[m_Layer] + E_STRPLN_DR + (1-m_UpDown)*RPC_SL_OFFSET;
}

void MucBakelite::SetRout()
{
        if( m_Part == BRID ) // Rout of bakelite
                m_Rout = B_AS_RMIN[m_Layer] - AS_GAP/2.0 + (1-2*m_UpDown) * (STR_TH + RPC_TH)/2.0
                         + (1-2*m_ID)*(GAS_TH + BKLT_TH)/2.0 + BKLT_TH/2.0;
        else
        {
               	m_Rout = E_AS_RMAX - E_BOX_DR - E_STRPLN_DB[(m_Layer==0)?0:1] + (m_UpDown-1)*RPC_SL_OFFSET;
        }
}

void MucBakelite::SetRc()
{
	m_Rc = (m_Rin + m_Rout)/2.0;
}

void MucBakelite::SetThin()
{
        m_Thin = BKLT_TH;
}

void MucBakelite::SetW()
{
        if( m_Part == BRID)
        {
		if( m_Segment == B_TOP && m_RpcId < 2 ) 	// the first 2 RPCs in top segment
		{
                        if( (m_Layer+1)%2 == 1 ) // odd layer
	                        m_W = (B_BOX_WT[m_Layer] - 2*B_BOX_DT[m_UpDown] - 2*B_BOX_DZ[m_UpDown] - B_BOX_SLOT_WT)/2.0;
                        else 			 // even layer
                                m_W = (B_BOX_WT[m_Layer] - 4*B_BOX_DZ[m_UpDown] - B_BOX_SLOT_WT)/2.0;
		}	
		else 					// other RPCs
		{
                        if( (m_Layer+1)%2 ==1 )  // odd layer
                                m_W = B_BOX_WT[m_Layer] - 2 * B_BOX_DT[m_UpDown];
                        else			 // even layer
                                m_W = B_BOX_WT[m_Layer] - 2 * B_BOX_DZ[m_UpDown];

		} 	
        } // end barrel
        else
        {
		if( m_UpDown == SL_UP ) // up layer
		{	if( m_RpcId == 0 )
				m_W = VALUE * m_Rc - E_GP_DX - E_STRPLN_DA - RPC_SL_OFFSET;
			else if ( m_RpcId == 1 )
				m_W = 2 * VALUE * m_Rc;
			else 
				m_W = VALUE * m_Rc - E_GP_DY - E_STRPLN_DA - RPC_SL_OFFSET;
		}
		else // down layer
		{	
			switch( m_RpcId )
			{
				case 0:
					m_W = VALUE * m_Rc - E_GP_DX - E_STRPLN_DA;
					break;
				case 1:
				case 3:
					m_W = VALUE * ( m_Rc - m_Rin );
					break;
				case 2:
					m_W = 2 * VALUE * m_Rin;
					break;
				case 4:
					m_W = VALUE * m_Rc - E_GP_DY - E_STRPLN_DA;	
					break;
				default: ;
			}
		}
		
        } // else, endcap

        // cut rpc chamber death edge
        if( m_Part == BRID )
                m_W -= RPC_DEATH_EDGE * 2.0;
        else
        {
                if( m_UpDown == SL_UP )
                {
                        if( m_RpcId == 1 ) m_W -= RPC_DEATH_EDGE * 2.0; // center rpc
                        else               m_W -= ( 1 + 1.0/cos(MUC_PI/8.0) ) * RPC_DEATH_EDGE;
                }
                else
                {
                        if( m_RpcId == 2 ) // center rpc
                                m_W -= RPC_DEATH_EDGE * 2.0;
                        else // DOWN 0,1,3,4 RPC
                                m_W -= RPC_DEATH_EDGE;
                }
        }
}

void MucBakelite::SetH()
{
        if( m_Part == BRID )
                m_H = BKLT_TH;
        else
        {
                m_H = m_Rout - m_Rin;	

                // cut rpc chamber death edge
                if( m_UpDown == SL_DOWN && (m_ID == 1 || m_ID == 3) )
                        m_H -= RPC_DEATH_EDGE * (1 + 1.0/VALUE);
                else
                        m_H -= RPC_DEATH_EDGE * 2.0;
        }
}

void MucBakelite::SetL()
{
        if( m_Part == BRID ) // barrel
	{
		if( m_Segment != B_TOP ) // not top segment
		{
			// the first rpc of SL_DOWN is constructed as the same as the last rpc of SL_UP
			if( (m_UpDown == SL_UP && m_RpcId == 3) || (m_UpDown == SL_DOWN && m_RpcId == 0) )
			{
				if( m_Layer != 1 ) // not layer NO.2
					m_L = B_BOX_LT - 2*B_BOX_DZ[m_UpDown] - 3*B_RPC_LTN;
				else		   // layer NO.2 is special
					m_L = B_BOX_LT - 2*B_BOX_DZ[m_UpDown] - 3*B_RPC_LTS;
			}
			else
			{
				if( m_Layer != 1 ) // not layer NO.2
					m_L = B_RPC_LTN;
				else		   // layer NO.2 is special
					m_L = B_RPC_LTS;
			}
		}
		else // top segment
		{
			if( m_RpcId < 2 ) // RPC NO.1 and NO.2
			{
				m_L = B_TOPRPC_LTS[ ((m_Layer == 0)?1:(m_Layer%2)) ];
			}			
			else if( (m_UpDown == SL_UP && m_RpcId == 4) || (m_UpDown == SL_DOWN && m_RpcId == 2) )
			{
				m_L = B_BOX_LT - B_TOPRPC_LTS[ ((m_Layer == 0)?1:(m_Layer%2)) ] - 2*(B_BOX_DZ[m_UpDown]+B_RPC_LTN);
			}
			else
				m_L = B_RPC_LTN;

		} // else, top segment	

		m_L -= RPC_DEATH_EDGE * 2.0;	// cut rpc chamber death edge

	} // if, barrel 
        else // endcap
	{
                m_L = BKLT_TH;
	}

}

void MucBakelite::SetWu()
{
        if( m_Part == BRID )
                m_Wu = m_W;
        else
        {	
		if( m_UpDown == SL_UP )
		{
			if( m_RpcId == 0 )
				m_Wu = VALUE * m_Rin - E_GP_DX - E_STRPLN_DA - RPC_SL_OFFSET;
			else if( m_RpcId == 1 )
				m_Wu = 2.0* VALUE * m_Rin;
			else 
				m_Wu = VALUE * m_Rin - E_GP_DY - E_STRPLN_DA - RPC_SL_OFFSET;
		}
		else
		{
			switch( m_RpcId )
			{
				case 0:
					m_Wu = VALUE * m_Rin - E_GP_DX - E_STRPLN_DA;
					break;
				case 1:
				case 3:
					m_Wu = 0.0;		
					break;
				case 2:
					m_Wu = 2.0* VALUE * m_Rin;
					break;
				case 4:
					m_Wu = VALUE * m_Rin - E_GP_DY - E_STRPLN_DA;
					break;
				default: ;
			}
		}

                // cut rpc chamber death edge
                if( m_Wu > 0.0 )
                {
                        if( m_UpDown == SL_UP )
                        {
                                if( m_RpcId == 1 ) m_Wu -= RPC_DEATH_EDGE * 2.0; // center rpc
                                else            m_Wu -= ( 1 + 1.0/cos(MUC_PI/8.0) ) * RPC_DEATH_EDGE;
                        }
                        else
                        {
                                if( m_RpcId == 2 ) // center rpc
                                        m_Wu -= RPC_DEATH_EDGE * 2.0;
                                else // DOWN 0,4 RPC
                                        m_W -= RPC_DEATH_EDGE;
                        }
                }
        } // else, endcap
}

void MucBakelite::SetWd()
{
	 if( m_Part == BRID )
                m_Wd = m_W;
        else
        {
                if( m_UpDown == SL_UP )
                {
                        if( m_RpcId == 0 )
                                m_Wd = VALUE * m_Rout - E_GP_DX - E_STRPLN_DA - RPC_SL_OFFSET;
			else if( m_RpcId == 1 )
				m_Wd = 2.0* VALUE * m_Rout;
			else
				m_Wd = VALUE * m_Rout - E_GP_DY - E_STRPLN_DA - RPC_SL_OFFSET;
                }
                else
                {
                        switch( m_RpcId )
                        {
                                case 0:
                                        m_Wd = VALUE * m_Rout - E_GP_DX - E_STRPLN_DA;
                                        break;
                                case 1:
                                case 3:
                                        m_Wd = VALUE * (m_Rout - m_Rin);
                                        break;
                                case 2:
                                        m_Wd = 2.0 * VALUE * m_Rin;
                                        break;
                                case 4:
                                        m_Wd = VALUE * m_Rout - E_GP_DY - E_STRPLN_DA;
                                        break;
                                default: ;
                        }
                }

                // cut rpc chamber death edge
                if( m_UpDown == SL_UP )
                {
                        if( m_RpcId == 1 ) m_Wd -= RPC_DEATH_EDGE * 2.0; // center rpc
                        else            m_Wd -= ( 1 + 1.0/cos(MUC_PI/8.0) ) * RPC_DEATH_EDGE;
                }
                else
                {
                        if( m_RpcId == 2 ) // center rpc
                               m_Wd -= RPC_DEATH_EDGE * 2.0;
                        else // DOWN 0,4 RPC
                               m_Wd -= RPC_DEATH_EDGE;
                }

        } // else, endcap	
}

// Set barrel rpc in BES CS
int MucBakelite::SetBarrelRpcInBes()
{
        if( m_Part != BRID ) return 0;

        double x, y, z;
        x = y = z = 0.0;

        if( m_Segment != B_TOP )
        {
	        // set x and y
                double r;
		r = (B_AS_RMIN[m_Layer] - AS_GAP / 2.0) + (1-2*m_UpDown)*(STR_TH + RPC_TH)/2.0;
                x = r * cos( m_Theta );
                y = r * sin( m_Theta );

                // set z
                if( (m_UpDown == SL_UP && m_RpcId == 3) || (m_UpDown == SL_DOWN && m_RpcId == 0) )
                {
                	if( m_Layer != 1 ) // not layer NO.2
         	       		z = (2*m_UpDown - 1) * 3 * B_RPC_LTN/2.0;
                        else
                                z = (2*m_UpDown - 1) * 3 * B_RPC_LTS/2.0;
                }
                else if( m_UpDown == SL_UP )
                        z = B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] - (1+2*m_RpcId)*B_RPC_LTN/2.0;
                else
                        z = -B_BOX_LT/2.0 + B_BOX_DZ[m_UpDown] + (1+2*(3-m_RpcId))*B_RPC_LTN/2.0;

        } // end not top segment
        else
        {
		//---------- set x--------
                if( m_RpcId > 1 )  // NO.3, 4, 5
                        x = 0.;
                else // NO.1 and NO.2
			x = (1-2*m_RpcId) * ( (B_BOX_WT[m_Layer] + B_BOX_SLOT_WT)/4.0 + (B_BOX_DZ[m_UpDown] - B_BOX_DT[m_UpDown])/2.0 );
		
		//---------- set y--------
                y = (B_AS_RMIN[m_Layer] - AS_GAP / 2.0) + (1-2*m_UpDown)*(STR_TH + RPC_TH)/2.0;

		//---------- set z--------
                if( m_RpcId < 2 ) // rpc NO.1 and NO.2
	                z = B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] - B_TOPRPC_LTS[ ((m_Layer==0)?1:(m_Layer%2)) ];
                else if( m_UpDown == SL_UP && m_RpcId == 4 )
                        z = -B_RPC_LTN - B_TOPRPC_LTS[ ((m_Layer==0)?1:(m_Layer%2)) ];
                else if( m_UpDown == SL_DOWN && m_RpcId == 2 )
                        z = B_RPC_LTN - B_TOPRPC_LTS[ ((m_Layer==0)?1:(m_Layer%2)) ];
                else if( m_UpDown == SL_UP )
                        z = B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] - B_TOPRPC_LTS[ ((m_Layer==0)?1:(m_Layer%2)) ] + (3-2*m_RpcId)*B_RPC_LTN/2.0;
                else
                        z = -B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] + (1+2*(4-m_RpcId))*B_RPC_LTN/2.0;

        } // end top segment

        m_LocOrgInBes[0] = x;
        m_LocOrgInBes[1] = y;
        m_LocOrgInBes[2] = z;

        // limit cut
        for( int i=0; i<2; i++ ) {
                if( fabs(m_LocOrgInBes[i]) < ERR_LIMIT ) m_LocOrgInBes[i] = 0.0;
        }
	
	return 0;
}

int MucBakelite::SetEndcapRpcInBes()
{
	if( m_Part == BRID ) return 0;
	
	double x, y, z;
	x = y = z = 0.;

	//------------ set x and y ---------------
	if( m_UpDown == SL_UP )
	{
		if( m_RpcId == 0 )
		{
			x = ( VALUE*m_Rc + E_GP_DX + E_STRPLN_DA + RPC_SL_OFFSET ) / 2.0;
			y = m_Rc;
		}	
		else if( m_RpcId == 1 )
		{
                        x = m_Rc * cos( MUC_PI/4.0 );
                        y = m_Rc * sin( MUC_PI/4.0 );
		}
		else
		{
			x = m_Rc;
			y = ( VALUE*m_Rc + E_GP_DY + E_STRPLN_DA + RPC_SL_OFFSET ) / 2.0;
		}		
	}
	else
	{
		switch( m_RpcId )
		{
			case 0:
				x = ( VALUE*m_Rc + E_GP_DX + E_STRPLN_DA )/2.0 + RPC_DEATH_EDGE/2.0;
				y = m_Rc;
				break;
			case 1:
				x = VALUE*m_Rc + sin(MUC_PI/4.0)*m_Wd/4.0;
				x += RPC_DEATH_EDGE * ( 3 - 2*sqrt(2.0) )/4.0;

				y = m_Rc - cos(MUC_PI/4.0)*m_Wd/4.0;
				y += RPC_DEATH_EDGE * ( 1 + 4.0/sqrt(2.0) )/4.0;

				break;
			case 2:
                                x = m_Rc * cos( MUC_PI/4.0 );
                                y = m_Rc * sin( MUC_PI/4.0 );
				break;
			case 3:
				x = m_Rc - cos(MUC_PI/4.0)*m_Wd/4.0;
				x += RPC_DEATH_EDGE * ( 1 + 4.0/sqrt(2.0) )/4.0;

				y = VALUE*m_Rc + sin(MUC_PI/4.0)*m_Wd/4.0;
				y += RPC_DEATH_EDGE * ( 3 - 2*sqrt(2.0) )/4.0;

				break;
			case 4:
				x = m_Rc;
				y = ( VALUE*m_Rc + E_GP_DY + E_STRPLN_DA )/2.0 + RPC_DEATH_EDGE/2.0;
		}
		
	}
	
        // sign different by segment
        if     ( m_Segment == 0 ) {     ;       	}
        else if( m_Segment == 1 ) {     x = -x; 	}
        else if( m_Segment == 2 ) {	x = -x; y = -y; }					        
        else    		  {	y = -y;		}

	
	//------------ set z ---------------------
        for( int i=0; i<m_Layer+1; i++ ) z += E_AS_TH[i]; 

        z += m_Layer * AS_GAP;
        z +=  ( (E_AS_ZMAX - E_AS_TOTAL_TH ) + AS_GAP/2.0 );  
        z += (1-2*m_UpDown) * ( STR_TH + RPC_TH ) / 2.0;	
	z *= cos( m_Part*MUC_PI/2.0 );

        m_LocOrgInBes[0] = x;
        m_LocOrgInBes[1] = y;
        m_LocOrgInBes[2] = z;

        // limit cut
        for( int i=0; i<2; i++ ) {
                if( fabs(m_LocOrgInBes[i]) < ERR_LIMIT ) m_LocOrgInBes[i] = 0.;
        }
		
	return 0;
}

// Set local origin(RPC) in BES CS
void MucBakelite::SetLocOrgInBes() 
{
        if( m_Part == BRID )
		SetBarrelRpcInBes();
        else
		SetEndcapRpcInBes();
}

// Set bakelite origin in RPC CS
void MucBakelite::SetObjOrgInLoc()
{
	if( m_Part == BRID )
	{
		m_ObjOrgInLoc[0] = 0.;

		m_ObjOrgInLoc[1] = (1-2*m_ID) * (BKLT_TH + GAS_TH)/2.0;

		m_ObjOrgInLoc[2] = 0.;
	}
	else
	{
		m_ObjOrgInLoc[0] = 0.;
		m_ObjOrgInLoc[1] = 0.;
		m_ObjOrgInLoc[2] = (1-2*m_ID) * (BKLT_TH + GAS_TH)/2.0;
	}
}

void MucBakelite::SetObjOrgInBes()
{
	MucEntity::TransLocToBes( m_ObjOrgInBes, m_ObjOrgInLoc, m_LocOrgInBes, m_Phi );

        // limit cut
        for( int i=0; i<3; i++ ) {
        	if( fabs(m_ObjOrgInBes[i]) < ERR_LIMIT ) m_ObjOrgInBes[i] = 0.;
        }
}

/*
void MucBakelite::SetAlignment( double dx, double dy, double dz )
{
	
	m_ObjOrgInLoc[0] += dx;
       	m_ObjOrgInLoc[1] += dy;
       	m_ObjOrgInLoc[2] += dz;
}
*/

// END
