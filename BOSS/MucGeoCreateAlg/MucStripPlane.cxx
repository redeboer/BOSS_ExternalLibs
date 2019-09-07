//------------------------------------------------------------------------------|
//      [File  ]:                       MucStripPlane.cxx                       |
//      [Brief ]:       MUC geometry strip plane creating class			|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#include <iostream>
#include <cmath>

#include "MucGeoCreateAlg/MucGeoConst.h"
#include "MucGeoCreateAlg/MucStripPlane.h"

using namespace std;

// Constructor
MucStripPlane::MucStripPlane( int part, int segment, int layer, int id ) : MucEntity( part, segment, layer, id )
{
	MucStripPlane::Init();
	m_MucStrip = NULL;
}

// Copy constructor		
MucStripPlane::MucStripPlane( const MucStripPlane &other ) : MucEntity( other )
{
	m_MucStrip 	= other.m_MucStrip;
}

// Operator =
MucStripPlane& MucStripPlane::operator =( const MucStripPlane &other )
{
	if( this == &other)
                return *this;

	MucEntity::operator =(other);

        m_MucStrip      = other.m_MucStrip;

        return *this;	
}

// Destructor
MucStripPlane::~MucStripPlane()
{	
	delete m_MucStrip;
}

// Initialize
void MucStripPlane::Init()
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
		
// -------------------------- Get methods ----------------------------

MucStrip* MucStripPlane::GetStrip( int id )
{
	if( m_MucStrip != NULL ) delete m_MucStrip;

	return ( m_MucStrip = new MucStrip( m_Part, m_Segment, m_Layer, id ) );
}

// ---------------------------Set motheds -----------------------------
void MucStripPlane::SetTheta()
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

void MucStripPlane::SetRin()
{
        if( m_Part == BRID )
                m_Rin = B_AS_RMIN[m_Layer] - ( AS_GAP + STR_TH ) / 2.0;
        else
        {
                if( m_ID == -1 )
                        m_Rin = 0.;
                else
                        m_Rin = E_GP_RMIN[m_Layer] + E_STRPLN_DR;
        }
}

void MucStripPlane::SetRout()
{
        if( m_Part == BRID )
                m_Rout = B_AS_RMIN[m_Layer] - ( AS_GAP - STR_TH ) / 2.0;
        else
                m_Rout = E_AS_RMAX - E_BOX_DR - E_STRPLN_DB[(m_Layer==0)?0:1];
}

void MucStripPlane::SetRc()
{
        if( m_Part == BRID )
                m_Rc = B_AS_RMIN[m_Layer] - AS_GAP/2.0;
        else
        {
                if( m_ID == -1 )
                        m_Rc = sqrt(2.0) * ( E_AS_RMAX - E_BOX_DR - E_STRPLN_DB[(m_Layer==0)?0:1] ) / 2.0;
                else
                        m_Rc = (m_Rin + m_Rout)/2.0;
        }
}

void MucStripPlane::SetThin()
{
	m_Thin = STR_TH;
}

void MucStripPlane::SetW()
{
	double offset = 0;
	if( (m_Layer+1)%2 == 0 ) offset = 2*B_BOX_DZ[1]; // even layers
	else 			 offset = 2*B_BOX_DT[1]; // odd layers

        if( m_Part == BRID)
	{	
           if( m_Segment == B_TOP ) 
	   {		   
		if(m_ID == -1 || m_ID == 2)
			m_W = B_BOX_WT[m_Layer] - offset;
		else
			m_W = (B_BOX_WT[m_Layer] - offset - B_PHISTR_CUT)/2.0;	
	   }	   
	   else
		 m_W = B_BOX_WT[m_Layer] - offset;
	}	
        else
        {
                if( m_ID == -1 ) // temparory strip plane  
                        m_W = E_AS_RMAX - E_BOX_DR - E_GP_DX - (E_STRPLN_DA + E_STRPLN_DB[(m_Layer==0)?0:1]);
                else
                        m_W = 0.;
        }
}

void MucStripPlane::SetH()
{
        if( m_Part == BRID )
                m_H = STR_TH;
        else
        {
                if( m_ID == -1 ) // temparory strip plane
                        m_H = E_AS_RMAX - E_BOX_DR - E_GP_DY - (E_STRPLN_DA + E_STRPLN_DB[(m_Layer==0)?0:1]);
                else
		{
                        m_H = E_AS_RMAX - E_BOX_DR - E_GP_RMIN[m_Layer] - E_STRPLN_DR - E_STRPLN_DB[(m_Layer==0)?0:1];
			if(m_ID==1) m_H += (E_STR_DST+5.0)/sqrt(2.0);

			m_H += 4*OVERLAP_WIDTH;
		}
        }
}

void MucStripPlane::SetL()
{
        if( m_Part == BRID )
	{
	   if( m_Segment == B_TOP )
	   {
		// 0,1,2,3,7: CUT = 1; 2,4,6,8: CUT = 0;   
		if( m_ID == -1) m_L = B_BOX_LT - 2*B_BOX_DZ[SL_DOWN];
		else if( m_ID == 2 ) m_L = B_BOX_LT - 2*B_BOX_DZ[SL_DOWN] - B_ZSTR_CUT[(m_Layer==0)?1:(m_Layer%2)];
		else            m_L = B_ZSTR_CUT[(m_Layer==0)?1:(m_Layer%2)];
	   }
	   else	   
                m_L = B_BOX_LT - 2*B_BOX_DZ[SL_DOWN];
	}	
        else
                m_L = STR_TH;
}

void MucStripPlane::SetWu()
{
        if( m_Part == BRID )
                m_Wu = m_W;
        else
        {	
		if( m_ID == -1 )     //  temparory strip plane 
			m_Wu = m_W;
                else if( m_ID == 1 ) // center panel
		{	
                        m_Wu = 2 * VALUE * m_Rin ;
			m_Wu -= VALUE*(E_STR_DST+5.0)/sqrt(2.0);
		}	
                else if( m_ID == 0 )
                        m_Wu = VALUE * m_Rin - E_GP_DY - E_STRPLN_DA;
		else
			m_Wu = VALUE * m_Rin - E_GP_DX - E_STRPLN_DA;

		//m_Wu += (1-VALUE)*OVERLAP_WIDTH;
		//m_Wu += 2*OVERLAP_WIDTH;
        }
}

void MucStripPlane::SetWd()
{
	 if( m_Part == BRID )
                m_Wd = m_W;
        else
        {	
		if( m_ID == -1 )     // temparory strip plane
			m_Wd = m_W;
		else if( m_ID == 1 ) // center panel 
		{	
                	m_Wd = 2 * VALUE * m_Rout;
			// zoom in with a small dimension for covering strips 
			m_Wd += VALUE*(E_STR_DST+5.0)/sqrt(2.0);
		}	
                else if( m_ID == 0 )
                	m_Wd = VALUE * m_Rout - E_GP_DY - E_STRPLN_DA;
		else
			m_Wd = VALUE * m_Rout - E_GP_DX - E_STRPLN_DA;

		// for "AND" operation of panels
		//m_Wd += (VALUE + 1.0)*OVERLAP_WIDTH;
		m_Wd += 4*OVERLAP_WIDTH;
        }
}




void MucStripPlane::SetLocOrgInBes()
{
        double x, y, z=0;

        if( m_Part == BRID )
        {
                x = m_Rc*cos( m_Theta );
                y = m_Rc*sin( m_Theta );
                z = 0.;
        } // barrel
        else
        {
                //-------------- set x and y ----------------------
		// segment 0 as reference
		if( m_ID == -1 )
		{	
                	x = (E_AS_RMAX - E_BOX_DR + E_GP_DX) / 2.0;
       	        	y = (E_AS_RMAX - E_BOX_DR + E_GP_DY) / 2.0;			
		}
		else
		{
			x = (E_AS_RMAX+E_GP_DX+E_STRPLN_DA -E_BOX_DR-E_STRPLN_DB[(m_Layer==0)?0:1])/2.0;
			y = (E_AS_RMAX+E_GP_DY+E_STRPLN_DA -E_BOX_DR-E_STRPLN_DB[(m_Layer==0)?0:1])/2.0;
		}
	
                // x, y signs of coordinate different by segment 
                if( m_Segment == 0 )       {    ;               }
                else if ( m_Segment == 1 ) {    x = -x;         }
                else if ( m_Segment == 2 ) {    x = -x; y = -y; }
                else                       {    y = -y;         }

                //-------------- set z -----------------------------
                for( int i=0; i<m_Layer+1; i++ ) z += E_AS_TH[i];

                z += m_Layer * AS_GAP;
                z += (E_AS_ZMAX - E_AS_TOTAL_TH + z) + AS_GAP/2.0;
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

void MucStripPlane::SetObjOrgInBes()
{
        if( m_Part == BRID )
        {
                for( int i=0; i<3; i++ )
                        m_ObjOrgInBes[i] = m_LocOrgInBes[i];
        }
        else // endcap
        {
		//------------- set x and y --------------------
                // segment 0 as reference 
        	double x, y;
                if( m_ID == -1 )
                {
                     	x = E_GP_DX + E_STRPLN_DA + m_W/2.0;
                     	y = E_GP_DY + E_STRPLN_DA + m_H/2.0;
                }
                else if( m_ID == 0 )
                {
                     	x = m_Rc;
                     	y = ( m_Rc * tan( m_Theta ) + E_GP_DY + E_STRPLN_DA ) / 2.0;
                }
                else if ( m_ID ==1 )
                {
                     	x = m_Rc * cos( m_Theta );
                     	y = m_Rc * sin( m_Theta );
                }
                else // m_ID == 2
                {
	        	x = ( m_Rc / tan( m_Theta ) + E_GP_DX + E_STRPLN_DA ) / 2.0;
                        y = m_Rc;
                }

                // x, y signs of coordinate different by segment 
                if( m_Segment == 0 )       {    ;               }
                else if ( m_Segment == 1 ) {    x = -x;         }
                else if ( m_Segment == 2 ) {    x = -x; y = -y; }
                else                       {    y = -y;         }

		m_ObjOrgInBes[0] = x;	
		m_ObjOrgInBes[1] = y;

	        // limit cut
        	for( int i=0; i<2; i++ ) {
        	        if( fabs(m_ObjOrgInBes[i]) < ERR_LIMIT ) m_ObjOrgInBes[i] = 0.;
	        }

		// ---------------- set z ----------------------
               	m_ObjOrgInBes[2] = m_LocOrgInBes[2];

        } // else, endcap
}

void MucStripPlane::SetObjOrgInLoc()
{
	double x, y, z;
        if( m_Part == BRID )
	{
		if( m_Segment == B_TOP && m_ID !=-1 )
		{
			// x
			if(m_ID == 2) 	   x = 0.0;
			else if(m_ID == 0) x = (B_PHISTR_CUT + m_W)/2.0;
			else 		   x = -(B_PHISTR_CUT + m_W)/2.0;

			// y
			y = 0.0;

			// z
			if(m_ID == 2)      z =  - B_ZSTR_CUT[(m_Layer==0)?1:(m_Layer%2)]/2.0;
			else               z =  B_BOX_LT/2.0 - B_BOX_DZ[SL_DOWN] - B_ZSTR_CUT[(m_Layer==0)?1:(m_Layer%2)]/2;

			m_ObjOrgInLoc[0] = x;
			m_ObjOrgInLoc[1] = y;
			m_ObjOrgInLoc[2] = z;
		}
		else	
		{	for(int i=0; i<3; i++) m_ObjOrgInLoc[i] = 0.;  }
	}	
        else
                for(int i=0; i<3; i++) m_ObjOrgInLoc[i] = m_ObjOrgInBes[i] - m_LocOrgInBes[i];
}

void MucStripPlane::SetAlignment( double dx, double dy, double dz )
{
	
        if( m_Part == BRID || m_ID == -1 )
        {
		m_ObjOrgInLoc[0] += dx;
        	m_ObjOrgInLoc[1] += dy;
        	m_ObjOrgInLoc[2] += dz;
        }
}

// END
