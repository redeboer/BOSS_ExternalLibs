//------------------------------------------------------------------------------|
//      [File  ]:                       MucBoxCover.cxx                         |
//      [Brief ]:       MUC geometry box cover creating class 			|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#include <iostream>
#include <cmath>

#include "MucGeoCreateAlg/MucGeoConst.h"
#include "MucGeoCreateAlg/MucBoxCover.h"

using namespace std;

// Constructor
MucBoxCover::MucBoxCover( int part, int segment, int layer, int upDown, int id ) : MucEntity( part, segment, layer, upDown, id )
{
	MucBoxCover::Init();
}

// Copy constructor		
MucBoxCover::MucBoxCover( const MucBoxCover &other ) : MucEntity( other ) { ; }

// Operator =
MucBoxCover& MucBoxCover::operator =( const MucBoxCover &other )
{
	if( this == &other)
                return *this;

	MucEntity::operator =(other);

        return *this;	
}

// Destructor
MucBoxCover::~MucBoxCover()
{	
	;
}

// Initialize
void MucBoxCover::Init()
{
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

        SetLocOrgInBes();
        SetObjOrgInBes();
        SetObjOrgInLoc();
}
		
//------------------------------- Set motheds -------------------------
// theta of local( box )
void MucBoxCover::SetTheta()
{
        if( m_Part == BRID )
                m_Theta = m_Segment * ( MUC_PI/4.0 );
        else
        {
                if( m_ID == -1 )
                        m_Theta = ( 2*m_Segment + 1 ) * ( MUC_PI/4.0 );
                else
                        m_Theta = ( MUC_PI/4.0 ) + ( m_ID - 1) * MUC_PI / 8.0 ;
        }
}

// Rin of local( box )
void MucBoxCover::SetRin()
{
        if( m_Part == BRID )
                m_Rin = B_AS_RMIN[m_Layer] - ( AS_GAP + BOX_TH ) / 2.0;
        else
        {
                if( m_ID == -1 )
                        m_Rin = 0.0;
                else
                        m_Rin = E_GP_RMIN[m_Layer];
        }
}

// Rout of local( box )
void MucBoxCover::SetRout()
{
        if( m_Part == BRID )
                m_Rout = B_AS_RMIN[m_Layer] -( AS_GAP - BOX_TH ) / 2.0;
        else
                m_Rout = E_AS_RMAX - E_BOX_DR;
}

// Rc of local( box )
void MucBoxCover::SetRc()
{
        if( m_Part == BRID )
                m_Rc = B_AS_RMIN[m_Layer] - AS_GAP/2.0;
        else
        {
                if( m_ID == -1 )
                        m_Rc = sqrt(2.0) * ( E_AS_RMAX - E_BOX_DR ) / 2.0;
                else
                        m_Rc = (m_Rin + m_Rout)/2.0;
        }
}

void MucBoxCover::SetThin()
{
	m_Thin = BOX_COVER_TH;
}

void MucBoxCover::SetW()
{
        if( m_Part == BRID)
        {
                if( m_Segment != B_TOP )
                         m_W = B_BOX_WT[m_Layer];
                else    // top segment
                {
                        if( m_ID == -1 || m_ID == 2 )
                                m_W = B_BOX_WT[m_Layer];
                        else
                                m_W = ( B_BOX_WT[m_Layer] - B_BOX_SLOT_WT )/2.0;
                }
        }
        else
        {
                if( m_ID == -1 ) // virtual encap gap 
                        m_W = E_AS_RMAX - E_BOX_DR - E_GP_DX;
                else
                        m_W = 0.;
        }
}

void MucBoxCover::SetH()
{
        if( m_Part == BRID )
                m_H = BOX_COVER_TH;
        else
        {
                if( m_ID == -1 ) // virtual encap gap
                        m_H = E_AS_RMAX - E_BOX_DR - E_GP_DY;
                else
                        m_H = E_AS_RMAX - E_BOX_DR - E_GP_RMIN[m_Layer];
        }
}

void MucBoxCover::SetL()
{
        if( m_Part == BRID )
        {
                if( m_Segment != B_TOP || m_ID == -1 )
                        m_L = B_BOX_LT;
                else // top segment
                {
                        if( m_ID == 2 )
                                m_L = B_BOX_LT - B_TOPRPC_LTS[(m_Layer==0)?1:(m_Layer%2)];
                        else
                                m_L = B_TOPRPC_LTS[(m_Layer==0)?1:(m_Layer%2)];
                }
        }
        else
                m_L = BOX_COVER_TH;
}

void MucBoxCover::SetWu()
{
        if( m_Part == BRID )
                m_Wu = m_W;
        else
        {	
		if( m_ID == -1 )     // virtual box 
			m_Wu = m_W;
                else if( m_ID == 1 ) // center fraction
                        m_Wu = 2 * VALUE * m_Rin ;
                else if( m_ID == 0 )
                        m_Wu = VALUE * m_Rin - E_GP_DY;
		else
			m_Wu = VALUE * m_Rin - E_GP_DX;
        }
}

void MucBoxCover::SetWd()
{
	 if( m_Part == BRID )
                m_Wd = m_W;
        else
        {	
		if( m_ID == -1 )     // virtual box
			m_Wd = m_W;
		else if( m_ID == 1 ) // center fraction
                	m_Wd = 2 * VALUE * m_Rout;
                else if( m_ID == 0 )
                	m_Wd = VALUE * m_Rout - E_GP_DY;
		else
			m_Wd = VALUE * m_Rout - E_GP_DX;
        }
}

// local is box
void MucBoxCover::SetLocOrgInBes()
{
        double x, y, z;
	x = y = z = 0.;

        if( m_Part == BRID )
        {
                x = m_Rc*cos( m_Theta );
                y = m_Rc*sin( m_Theta );
                z = 0.;

        } // barrel
        else
        {
                //------------ set x and y ---------------
		// segment 0 as reference
                x = (E_AS_RMAX - E_BOX_DR + E_GP_DX) / 2.0;
                y = (E_AS_RMAX - E_BOX_DR + E_GP_DY) / 2.0;

	        // sign different by segment
        	if     ( m_Segment == 0 ) {     ;               }
	        else if( m_Segment == 1 ) {     x = -x;         }
        	else if( m_Segment == 2 ) {     x = -x; y = -y; }
	        else                      {     y = -y;         }
		
                //------------- set z --------------------
                for( int i=0; i<m_Layer+1; i++ ) z += E_AS_TH[i];

                z += m_Layer * AS_GAP;
                z += (E_AS_ZMAX - E_AS_TOTAL_TH) + AS_GAP/2.0;  
		z *= cos( m_Part*MUC_PI/2.0 );
	
		if( m_ID != -1 ) z += (1-2*m_UpDown)*(BOX_TH - BOX_COVER_TH)/2.0;

        }// endcap

        m_LocOrgInBes[0] = x;
        m_LocOrgInBes[1] = y;
        m_LocOrgInBes[2] = z;

        // limit cut
        for(int i=0;i<3;i++) {
                if( fabs(m_LocOrgInBes[i]) < ERR_LIMIT ) m_LocOrgInBes[i] = 0.;
        }
}

void MucBoxCover::SetObjOrgInBes()
{
        double x, y, z;
	x = y = z =0.;

        if( m_Part == BRID ) // barrel
        {
		if( m_Segment != B_TOP || m_ID ==-1 )
		{
			double r;
			r = m_Rc + (1-2*m_UpDown)*(BOX_TH - BOX_COVER_TH) / 2.0;
        	        x = r * cos( m_Theta );
                	y = r * sin( m_Theta );
	                z = 0.;
		}
		else // top segment
		{
                        // set x
                        if( m_ID == 2 ) x = 0.;
                        else            x = (1-2*m_ID) * ( B_BOX_WT[m_Layer] + B_BOX_SLOT_WT )/4.0;

                        // set y
                        y = m_Rc + (1-2*m_UpDown)*(BOX_TH - BOX_COVER_TH) / 2.0;
			
                        // set z
                        if( m_ID == 2 ) z = -B_TOPRPC_LTS[(m_Layer==0)?1:(m_Layer%2)] / 2.0;
                        else            z = ( B_BOX_LT - B_TOPRPC_LTS[(m_Layer==0)?1:(m_Layer%2)] )/2.0;
		}
        }
        else // endcap 
        {
		//------------- set x, y --------------------------
                // segment 0 as reference 
		if( m_ID == -1 ) // box cover
		{
	                x = (E_AS_RMAX - E_BOX_DR + E_GP_DX) / 2.0;
        	        y = (E_AS_RMAX - E_BOX_DR + E_GP_DY) / 2.0;	
		}
		else if( m_ID == 0 ) // box cover panels
                {
                        x = m_Rc;
                        y = ( m_Rc * tan( m_Theta ) + E_GP_DY ) / 2.0;
                }
                else if ( m_ID == 1 )
                {
                        x = m_Rc * cos( m_Theta );
                        y = m_Rc * sin( m_Theta );
                }
                else // m_ID == 2
                {
                        x = ( m_Rc / tan( m_Theta ) + E_GP_DX ) / 2.0;
                        y = m_Rc;
                }

	        // sign different by segment
	        if     ( m_Segment == 0 ) {     ;               }
        	else if( m_Segment == 1 ) {     x = -x;         }
	        else if( m_Segment == 2 ) {     x = -x; y = -y; }
        	else                      {     y = -y;         }

		//---------- set z --------------------
		if( m_ID == -1 )
			z = m_LocOrgInBes[2] + (1-2*m_UpDown)*(BOX_TH - BOX_COVER_TH)/2.0;
		else
			z = m_LocOrgInBes[2];

        } // else, endcap 

	m_ObjOrgInBes[0] = x;
	m_ObjOrgInBes[1] = y;	
 	m_ObjOrgInBes[2] = z;

        // limit cut
       	for( int i=0; i<3; i++ ) {
       	        if( fabs(m_ObjOrgInBes[i]) < ERR_LIMIT ) m_ObjOrgInBes[i] = 0.;
        }

}

void MucBoxCover::SetObjOrgInLoc()
{
        if( m_Part == BRID )
        {

		// set local x
		if( m_Segment != B_TOP || m_ID == -1 || m_ID == 2 )
			m_ObjOrgInLoc[0] = 0.;
		else
	        	m_ObjOrgInLoc[0] = (1-2*m_ID) * ( B_BOX_WT[m_Layer] + B_BOX_SLOT_WT )/4.0;

		// set local y
		m_ObjOrgInLoc[1] = (1-2*m_UpDown)*(BOX_TH - BOX_COVER_TH)/2.0;
//		m_ObjOrgInLoc[1] = 0.;
		
		// set local z
		if( m_Segment != B_TOP || m_ID == -1 )
			m_ObjOrgInLoc[2] = 0.;
		else
		{
                        if( m_ID == 2 ) m_ObjOrgInLoc[2] = -B_TOPRPC_LTS[(m_Layer==0)?1:(m_Layer%2)] / 2.0;
                        else            m_ObjOrgInLoc[2] = ( B_BOX_LT - B_TOPRPC_LTS[(m_Layer==0)?1:(m_Layer%2)] )/2.0;
		}	
        }
        else // endcap 
        {
                for(int i=0; i<3; i++)
                	m_ObjOrgInLoc[i] = m_ObjOrgInBes[i] - m_LocOrgInBes[i];
        }

       	// limit cut
       	for( int i=0; i<3; i++ ) {
        	if( fabs(m_ObjOrgInLoc[i]) < ERR_LIMIT ) m_ObjOrgInLoc[i] = 0.;
        }

}

/*
void MucBoxCover::SetAlignment( double dx, double dy, double dz )
{
	m_ObjOrgInLoc[0] += dx;
       	m_ObjOrgInLoc[1] += dy;
       	m_ObjOrgInLoc[2] += dz;
}
*/

// END
