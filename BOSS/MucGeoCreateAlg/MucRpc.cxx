//------------------------------------------------------------------------------|
//      [File  ]:                       MucRpc.cxx 	                        |
//      [Brief ]:       MUC geometry RPC creating class	  	   	 	|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#include <iostream>
#include <cmath>

#include "MucGeoCreateAlg/MucGeoConst.h"
#include "MucGeoCreateAlg/MucRpc.h"

using namespace std;

// Constructor
MucRpc::MucRpc( int part, int segment, int layer, int upDown, int id ) : MucEntity( part, segment, layer, upDown, id )
{
	MucRpc::Init();

	m_MucGas 	= NULL;
	m_MucBakelite 	= NULL;
}

// Copy constructor		
MucRpc::MucRpc( const MucRpc &other ) : MucEntity( other )
{
	m_Phi		= other.m_Phi;
	m_MucGas 	= other.m_MucGas;
        m_MucBakelite   = other.m_MucBakelite;
}

// Operator =
MucRpc& MucRpc::operator =( const MucRpc &other )
{
	if( this == &other)
                return *this;

	MucEntity::operator =(other);

        m_Phi           = other.m_Phi;	
        m_MucGas      	= other.m_MucGas;
        m_MucBakelite   = other.m_MucBakelite;

        return *this;	
}

// Destructor
MucRpc::~MucRpc()
{	
	delete m_MucGas;
	delete m_MucBakelite;
}

// Initialize
void MucRpc::Init()
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
	SetBarrelRpcInLoc();

        SetLocOrgInBes();
        SetObjOrgInBes();
        SetObjOrgInLoc();
}
		
// -------------------------- Get methods ----------------------------
double MucRpc::GetPhi()	{	return m_Phi; 	}

MucGas* MucRpc::GetGas()
{
	if( m_MucGas != NULL )	delete m_MucGas;

	return ( m_MucGas = new MucGas( m_Part, m_Segment, m_Layer, m_UpDown, m_ID, 0 ) );
}

MucBakelite* MucRpc::GetBakelite( int id )
{
        if( m_MucBakelite != NULL )  delete m_MucBakelite;

        return ( m_MucBakelite = new MucBakelite( m_Part, m_Segment, m_Layer, m_UpDown, m_RpcId, id ) );
}


// ---------------------------Set motheds -----------------------------

void MucRpc::SetPhi()
{
	if( m_Part == BRID )
		m_Phi = B_PHI[m_Segment] * (MUC_PI/4.0);
	else
		m_Phi = 0.;
}


void MucRpc::SetTheta()
{
        if( m_Part == BRID )
                m_Theta = m_Segment * ( MUC_PI/4.0 );
        else
        {
                if( m_UpDown == SL_UP )
		{
			m_Theta = ( MUC_PI/4.0 ) + ( 1 - m_ID ) * MUC_PI / 8.0 ;
		}
                else
		{	
			if( m_ID < 2 ) // RPC NO.1 and NO.2
	                        m_Theta = ( MUC_PI/4.0 ) + MUC_PI / 8.0 ;
			else if ( m_ID == 2 ) // center RPC, NO.3
				m_Theta = ( MUC_PI/4.0 ) ;
			else	
				m_Theta = ( MUC_PI/4.0 ) - MUC_PI / 8.0 ;
		}
        }
}

void MucRpc::SetRin()
{
        if( m_Part == BRID )
                m_Rin = B_AS_RMIN[m_Layer] - AS_GAP / 2.0 + ( 1-2*m_UpDown ) * (STR_TH / 2.0 + m_UpDown*RPC_TH);
        else
		m_Rin = E_GP_RMIN[m_Layer] + E_STRPLN_DR + (1-m_UpDown)*RPC_SL_OFFSET;
}

void MucRpc::SetRout()
{
        if( m_Part == BRID )
                m_Rout = B_AS_RMIN[m_Layer] - AS_GAP / 2.0 + ( 1-2*m_UpDown )*( STR_TH / 2.0 + (1-m_UpDown)*RPC_TH );
        else
                m_Rout = E_AS_RMAX - E_BOX_DR - E_STRPLN_DB[(m_Layer==0)?0:1] + (m_UpDown-1)*RPC_SL_OFFSET;
}

void MucRpc::SetRc()
{
	m_Rc = (m_Rin + m_Rout)/2.0;
}

void MucRpc::SetThin()
{
        m_Thin = RPC_TH;
}

void MucRpc::SetW()
{
        if( m_Part == BRID)
        {
		if( m_Segment == B_TOP && m_ID < 2 ) 	// the first 2 RPCs in top segment
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
		if( m_UpDown == SL_UP ) // up super layer RPCs
		{
			if( m_ID == 0 )
				m_W = VALUE * m_Rc - E_GP_DX - E_STRPLN_DA - RPC_SL_OFFSET;
			else if( m_ID == 1 )
				m_W = VALUE * m_Rc * 2.0;
			else
				m_W = VALUE * m_Rc - E_GP_DY - E_STRPLN_DA - RPC_SL_OFFSET;
		}
		else // down super layer RPCs
		{
			if( m_ID == 0 )
				m_W = VALUE * m_Rc - E_GP_DX - E_STRPLN_DA;
			else if( m_ID == 1 || m_ID == 3 )	
				m_W = VALUE * (m_Rc - m_Rin);
			else if( m_ID == 2 )
				m_W = VALUE * m_Rin * 2.0;
			else 
				m_W = VALUE * m_Rc - E_GP_DY - E_STRPLN_DA; 
		}
		
        } // else, endcap
	
	// cut rpc chamber death edge
	if( m_Part == BRID )
		m_W -= OVERLAP_WIDTH * 2.0;
	else
	{
		if( m_UpDown == SL_UP )
		{
			if( m_ID == 1 ) m_W -= OVERLAP_WIDTH * 2.0; // center rpc
			else		m_W -= ( 1 + 1.0/cos(MUC_PI/8.0) ) * OVERLAP_WIDTH;
		}
		else
		{
			if( m_ID == 2 ) // center rpc
				m_W -= OVERLAP_WIDTH * 2.0;	
			else // DOWN 0,1,3,4 RPC
				m_W -= OVERLAP_WIDTH;
		}
	}
}

void MucRpc::SetH()
{
        if( m_Part == BRID )
                m_H = RPC_TH;
        else
        {
		m_H = m_Rout - m_Rin;
        }
}

void MucRpc::SetL()
{
        if( m_Part == BRID ) // barrel
	{
		if( m_Segment != B_TOP ) // not top segment
		{
	        	if( (m_UpDown == SL_UP && m_ID == 3 ) || (m_UpDown == SL_DOWN && m_ID == 0 ) )
			{
				if( m_Layer != 1 ) // not layer NO. 2
					m_L = B_BOX_LT - 2*B_BOX_DZ[m_UpDown] - 3*B_RPC_LTN;
				else
					m_L = B_BOX_LT - 2*B_BOX_DZ[m_UpDown] - 3*B_RPC_LTS;
			}
			else
			{
                	        if( m_Layer != 1 ) // not layer NO. 2
                        	        m_L = B_RPC_LTN;
	                        else
        	                        m_L = B_RPC_LTS;			
			}
		}
		else // top segment
		{
			if( m_ID < 2 ) // first two RPCs
			{
			        if( m_Layer == 0 ) // the first layer
                                        m_L = B_TOPRPC_LTS[1];
                                else
                                        m_L = B_TOPRPC_LTS[m_Layer%2];
			}
			else if( (m_UpDown == SL_UP && m_ID == 4 ) || (m_UpDown == SL_DOWN && m_ID == 2 ) )
			{
                                if( m_Layer == 0 ) // the first layer
                                        m_L = B_BOX_LT - B_TOPRPC_LTS[1] - 2*(B_BOX_DZ[m_UpDown] + B_TOPRPC_LTN);
                                else
                                        m_L = B_BOX_LT - B_TOPRPC_LTS[m_Layer%2] - 2*(B_BOX_DZ[m_UpDown] + B_TOPRPC_LTN);		
			}
			else
			{
                                m_L = B_TOPRPC_LTN;
			}
		} // end top segment

		m_L -= OVERLAP_WIDTH * 2.0; // cut rpc chamber death edge

	} // end barrel
        else
	{
                m_L = RPC_TH;
	}
}

void MucRpc::SetWu()
{
        if( m_Part == BRID )
                m_Wu = m_W;
        else
        {
		if( m_UpDown == SL_UP )
		{
			if( m_ID == 0 )
				m_Wu = VALUE * m_Rin - E_GP_DX - E_STRPLN_DA - RPC_SL_OFFSET;
			else if( m_ID == 1 )
				m_Wu = VALUE * m_Rin * 2.0;
			else 
				m_Wu = VALUE * m_Rin - E_GP_DY - E_STRPLN_DA - RPC_SL_OFFSET;
		}
		else
		{
                        if( m_ID == 0 )
                                m_Wu = VALUE * m_Rin - E_GP_DX - E_STRPLN_DA;
                        else if( m_ID == 1 || m_ID == 3 )
                                m_Wu = 0.;
                        else if( m_ID == 2 )
                                m_Wu = VALUE * m_Rin * 2.0;
                        else
                                m_Wu = VALUE * m_Rin - E_GP_DY - E_STRPLN_DA;			
		}

		// cut rpc chamber death edge
		if( m_Wu > 0.0 ) // except for DOWN 1, 3 RPC 
		{
			if( m_UpDown == SL_UP )
                	{
                        	if( m_ID == 1 ) m_Wu += 2.0*OVERLAP_WIDTH *(sin(BETA)-1)/cos(BETA); // center rpc
                        	else            m_Wu -= ( 1 + 1.0/cos(BETA) ) * OVERLAP_WIDTH;
                	}
                	else
                	{
                               	m_Wu -= 1.5*OVERLAP_WIDTH;
                	}
		}
        }
}
 
void MucRpc::SetWd()
{
	 if( m_Part == BRID )
                m_Wd = m_W;
        else
        {	
                if( m_UpDown == SL_UP )
                {
                        if( m_ID == 0 )
                                m_Wd = VALUE * m_Rout - E_GP_DX - E_STRPLN_DA - RPC_SL_OFFSET;
                        else if( m_ID == 1 )
                                m_Wd = VALUE * m_Rout * 2.0;
                        else
                                m_Wd = VALUE * m_Rout - E_GP_DY - E_STRPLN_DA - RPC_SL_OFFSET;
                }
                else
                {
                        switch( m_ID )
                        {
                                case 0:
                                        m_Wd = VALUE * m_Rout - E_GP_DX - E_STRPLN_DA;
                                        break;
                                case 1:
                                case 3:
                                        m_Wd = VALUE * ( m_Rout - m_Rin );
                                        break;
                                case 2:
                                        m_Wd = 2 * VALUE * m_Rin;
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
	                if( m_ID == 1 ) m_Wd -= 2.0*OVERLAP_WIDTH*(sin(BETA)+1)/cos(BETA); // center rpc
                        else            m_Wd -= ( 1 + 1.0/cos(BETA) ) * OVERLAP_WIDTH;
                }
                else
                {
			if( m_ID ==0 || m_ID == 4 )
         	              // m_Wd -= 2.0*OVERLAP_WIDTH*(sin(BETA)-1)/cos(BETA);
         	         m_Wd -= 1.5*OVERLAP_WIDTH;
                }
        }
}

int MucRpc::SetEndcapRpcInBes()
{
	if( m_Part == BRID )
		return 0;
	else
	{
		double x, y, z;
		x = y = z = 0.;

		// -----------------set x and y -----------------
                if( m_UpDown == SL_UP ) // up super layer
                {
                        if( m_ID == 0 )
			{
				x = ( VALUE * m_Rc + E_GP_DX + E_STRPLN_DA + RPC_SL_OFFSET ) / 2.0;
				y = m_Rc;  
			}
                        else if( m_ID == 1 )
			{
                                x = m_Rc * cos( m_Theta );
				y = m_Rc * sin( m_Theta );
			}
                        else
			{
                        	x = m_Rc;
				y = ( VALUE * m_Rc + E_GP_DY + E_STRPLN_DA + RPC_SL_OFFSET ) / 2.0; 
			}
                }
                else	// down super layer
                {
                        if( m_ID == 0 )
			{
                                x = ( VALUE * m_Rc + E_GP_DX + E_STRPLN_DA )/2.0 + OVERLAP_WIDTH/2.0;
                                y = m_Rc; 
			}
                        else if( m_ID == 1 )
			{
				x = VALUE * m_Rc + sin(MUC_PI/4.0)*m_Wd/4.0;
		//		x += OVERLAP_WIDTH * ( 3 - 2*sqrt(2.0) )/4.0;

                                y = m_Rc - cos(MUC_PI/4.0)*m_Wd/4.0;
		//		y += OVERLAP_WIDTH * ( 1 + 4.0/sqrt(2.0) )/4.0;
			}
                        else if( m_ID == 2 )
			{
                                x = m_Rc * cos( MUC_PI/4.0 );
                                y = m_Rc * sin( MUC_PI/4.0 );
			}
			else if( m_ID == 3 )
			{
                                x = m_Rc - cos(MUC_PI/4.0)*m_Wd/4.0;
		//		x += OVERLAP_WIDTH * ( 1 + 4.0/sqrt(2.0) )/4.0;

                                y = VALUE * m_Rc + sin(MUC_PI/4.0)*m_Wd/4.0;
		//		y += OVERLAP_WIDTH * ( 3 - 2*sqrt(2.0) )/4.0;
			}
                        else
			{
                                x = m_Rc;
                                y = ( VALUE * m_Rc + E_GP_DY + E_STRPLN_DA )/2.0 + OVERLAP_WIDTH/2.0;
			}
                }

                // x, y signs of coordinate different by segment 
                if( m_Segment == 0 )       {    ;               }
                else if ( m_Segment == 1 ) {    x = -x;         }
                else if ( m_Segment == 2 ) {    x = -x; y = -y; }
                else                       {    y = -y;         }

                //-------------- set z -----------------------------
                for( int i=0; i<m_Layer+1; i++ ) z += E_AS_TH[i];

                z += m_Layer * AS_GAP;
                z += ( (E_AS_ZMAX - E_AS_TOTAL_TH) + AS_GAP/2.0 );
		z += (1-2*m_UpDown) * (RPC_TH + STR_TH)/2.0 ;
		z *= cos( m_Part*MUC_PI/2.0 );
		
	        m_ObjOrgInBes[0] = x;
	        m_ObjOrgInBes[1] = y;
        	m_ObjOrgInBes[2] = z;

	        // limit cut
	        for(int i=0;i<3;i++) {
        	        if( fabs(m_ObjOrgInBes[i]) < ERR_LIMIT ) m_ObjOrgInBes[i] = 0.;
	        }

	} // else, endcap

	return 0;
}

int MucRpc::SetBarrelRpcInLoc()
{
	if( m_Part != BRID )
		return 0;
	else
	{
		double x, y, z;
		x = y = z = 0.;

		if( m_Segment != B_TOP ) // not top segment
		{
			//---------- set x--------
			x = 0.;

                        //---------- set y--------
                        y = (1-2*m_UpDown) * ( RPC_TH + STR_TH )/2.0;

                        //---------- set z--------
			if( (m_UpDown == SL_UP && m_ID == 3) || (m_UpDown == SL_DOWN && m_ID == 0) )
			{
				if( m_Layer != 1 ) // not layer NO.2
					z = (2*m_UpDown - 1) * 3 * B_RPC_LTN / 2.0;
				else
					z = (2*m_UpDown - 1) * 3 * B_RPC_LTS / 2.0; 
			}
			else 
			{	
				if( m_Layer != 1) // not layer NO.2
				{
					if( m_UpDown == SL_UP )
						z = B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] - (1+2*m_ID)*B_RPC_LTN/2.0;
					else
						z = -B_BOX_LT/2.0 + B_BOX_DZ[m_UpDown] + (1+2*(3-m_ID))*B_RPC_LTN/2.0;
				}
				else
				{
					if( m_UpDown == SL_UP )
						z = B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] - (1+2*m_ID)*B_RPC_LTS/2.0;
					else
						z = -B_BOX_LT/2.0 + B_BOX_DZ[m_UpDown] + (1+2*(3-m_ID))*B_RPC_LTS/2.0;	
				}
			}
		}
		else // top segment
		{
                        //---------- set x--------
			if( m_ID > 1 )
        	                x = 0.;
			else
				x = (1-2*m_ID) * ( (B_BOX_WT[m_Layer] + B_BOX_SLOT_WT)/4.0 + (B_BOX_DZ[m_UpDown]-B_BOX_DT[m_UpDown])/2.0 ); 

                        //---------- set y--------
                        y = (1-2*m_UpDown) * ( RPC_TH + STR_TH )/2.0;

                        //---------- set z--------
			if( m_UpDown == SL_UP )
			{
				switch( m_ID )
				{
				 case 0:
				 case 1:
					if( m_Layer == 0 )
						z = B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] - B_TOPRPC_LTS[1]/2.0;
					else
						z = B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] - B_TOPRPC_LTS[m_Layer%2]/2.0;
					break;
				 case 2:
				 case 3:
                                        if( m_Layer == 0 )
                                                z = B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] - B_TOPRPC_LTS[1] +  (3-2*m_ID)*B_RPC_LTN/2.0;
                                        else
                                                z = B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] - B_TOPRPC_LTS[m_Layer%2] + (3-2*m_ID)*B_RPC_LTN/2.0;			
					break;
				 case 4:
                                        if( m_Layer == 0 )
                                                z = -B_RPC_LTN - B_TOPRPC_LTS[1]/2.0;
                                        else
                                                z = -B_RPC_LTN - B_TOPRPC_LTS[m_Layer%2]/2.0;
                                        break;
				 default: ;
				}
			}else // down super layer
                        {
				switch( m_ID )
				{
                                 case 0: ;
                                 case 1:
                                        if( m_Layer == 0 )
                                                z = B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] - B_TOPRPC_LTS[1]/2.0;
                                        else
                                                z = B_BOX_LT/2.0 - B_BOX_DZ[m_UpDown] - B_TOPRPC_LTS[m_Layer%2]/2.0;
                                        break;
                                 case 2:
                                        if( m_Layer == 0 )
                                                z = B_RPC_LTN - B_TOPRPC_LTS[1]/2.0;
                                        else
                                                z = B_RPC_LTN - B_TOPRPC_LTS[m_Layer%2]/2.0;
                                        break;
                                 case 3:
                                 case 4:
				        z = -B_BOX_LT/2.0 + B_BOX_DZ[m_UpDown] + (1+2*(4-m_ID))*B_RPC_LTN/2.0;
                                        break;
                                 default: ;
                                }
                        } // else, down super layer
		} // else, top segment

	        m_ObjOrgInLoc[0] = x;
	        m_ObjOrgInLoc[1] = y;
        	m_ObjOrgInLoc[2] = z;

	        // limit cut
        	for(int i=0;i<3;i++) {
	                if( fabs(m_ObjOrgInLoc[i]) < ERR_LIMIT ) m_ObjOrgInLoc[i] = 0.;
        	}

	} // else, barrel
	
	return 0;
}

// Set local( box ) in BES CS
void MucRpc::SetLocOrgInBes()
{
        double x, y, z;
	x = y = z = 0.;

        if( m_Part == BRID )
        {
                x = (B_AS_RMIN[m_Layer] - AS_GAP/2.0) * cos( m_Theta );
                y = (B_AS_RMIN[m_Layer] - AS_GAP/2.0) * sin( m_Theta );
                z = 0.;

        } // barrel
        else
        {
                //-------------- set x and y ----------------------
		// segment 0 as reference
                x = (E_AS_RMAX - E_BOX_DR + E_GP_DX) / 2.0;
       	        y = (E_AS_RMAX - E_BOX_DR + E_GP_DY) / 2.0;			
		
                // x, y signs of coordinate different by segment 
                if( m_Segment == 0 )       {    ;               }
                else if ( m_Segment == 1 ) {    x = -x;         }
                else if ( m_Segment == 2 ) {    x = -x; y = -y; }
                else                       {    y = -y;         }

                //-------------- set z -----------------------------
                for( int i=0; i<m_Layer+1; i++ ) z += E_AS_TH[i];

                z += m_Layer * AS_GAP;
                z += ( (E_AS_ZMAX - E_AS_TOTAL_TH) + AS_GAP/2.0 );
		z *= cos( m_Part*MUC_PI/2.0 );

        }// endcap

        m_LocOrgInBes[0] = x;
        m_LocOrgInBes[1] = y;
        m_LocOrgInBes[2] = z;

        // limit cut
        for(int i=0;i<3;i++) {
                if( fabs(m_LocOrgInBes[i]) < ERR_LIMIT ) m_LocOrgInBes[i] = 0.;
        }
}

void MucRpc::SetObjOrgInBes()
{
        if( m_Part == BRID )
        {
		MucEntity::TransLocToBes( m_ObjOrgInBes, m_ObjOrgInLoc, m_LocOrgInBes, m_Phi );

	        // limit cut
        	for(int i=0;i<3;i++) {
                	if( fabs(m_ObjOrgInBes[i]) < ERR_LIMIT ) m_ObjOrgInBes[i] = 0.;
        	}
        }
}

void MucRpc::SetObjOrgInLoc()
{
        if( m_Part != BRID )
	{
                MucEntity::TransBesToLoc( m_ObjOrgInLoc, m_ObjOrgInBes, m_LocOrgInBes, m_Phi );
	        // limit cut
	        for(int i=0;i<3;i++) {
        	        if( fabs(m_ObjOrgInLoc[i]) < ERR_LIMIT ) m_ObjOrgInLoc[i] = 0.;
        	}
	}
}

/*
void MucRpc::SetAlignment( double dx, double dy, double dz )
{
	
        if( m_Part == BRID || m_ID == -1 )
        {
		m_ObjOrgInLoc[0] += dx;
        	m_ObjOrgInLoc[1] += dy;
        	m_ObjOrgInLoc[2] += dz;
        }
}
*/

// END
