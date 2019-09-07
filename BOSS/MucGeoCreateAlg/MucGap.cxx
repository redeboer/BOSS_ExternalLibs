//------------------------------------------------------------------------------|
//      [File  ]:                       MucGap.cxx                              |
//      [Brief ]:       MUC geometry yoke creating class 			|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#include <iostream>
#include <cmath>

#include "MucGeoCreateAlg/MucGeoConst.h"
#include "MucGeoCreateAlg/MucGap.h"

using namespace std;

// Constructor
MucGap::MucGap( int part, int segment, int layer, int id ) : MucEntity( part, segment, layer, id )
{
	MucGap::Init();
	m_MucBox = NULL;
}

// Copy constructor		
MucGap::MucGap( const MucGap &other ) : MucEntity( other )
{
	m_MucBox	= other.m_MucBox;
}

// Operator =
MucGap& MucGap::operator =( const MucGap &other )
{
	if( this == &other)
                return *this;

	MucEntity::operator =(other);

	m_MucBox        = other.m_MucBox;

        return *this;	
}

// Destructor
MucGap::~MucGap()
{	
	delete m_MucBox	;
}

// Initialize
void MucGap::Init()
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
        SetObjRotToMot();
        SetObjOrgInBes();
        SetObjOrgInLoc();
}
		

// ------------------- Get methods --------------------

MucBox* MucGap::GetBox( )
{
	if( m_MucBox != NULL ) 
		return m_MucBox;
	else
		return ( m_MucBox = new MucBox(m_Part, m_Segment, m_Layer, ((m_Part==BRID)?0:-1) ) );
}


// --------------------- Set motheds ----------------------
void MucGap::SetTheta()
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

void MucGap::SetRin()
{
        if( m_Part == BRID )
                m_Rin = B_AS_RMIN[m_Layer] - AS_GAP;
        else
        {
                if( m_ID == -1 )
                        m_Rin = 0.;
                else
                        m_Rin = E_GP_RMIN[m_Layer];
        }
}

void MucGap::SetRout()
{
        if( m_Part == BRID )
                m_Rout = B_AS_RMIN[m_Layer];
        else
                m_Rout = E_AS_RMAX;
}

void MucGap::SetRc()
{
        if( m_Part == BRID )
                m_Rc = B_AS_RMIN[m_Layer] - AS_GAP/2.0;
        else
        {
                if( m_ID == -1 )
                        m_Rc = sqrt(2.0) * E_AS_RMAX / 2.0;
                else
                        m_Rc = (m_Rin + m_Rout)/2.0;
        }
}

void MucGap::SetThin()
{
        m_Thin = AS_GAP;
}

void MucGap::SetW()
{
        if( m_Part == BRID)
	{	
		if( m_Segment != B_TOP )
                	m_W = B_GP_WT[m_Layer];
		else	// top segment
		{
         		if( m_ID == -1 || m_ID == 2 )
				m_W = B_GP_WT[m_Layer];
			else
				m_W = ( B_GP_WT[m_Layer] - B_GP_SLOT_WT )/2.0;
		}		
	}	
        else
        {
                if( m_ID == -1 ) // virtual encap gap 
                        m_W = E_AS_RMAX - E_GP_DX;
                else
                        m_W = 0.;
        }
}

void MucGap::SetH()
{
        if( m_Part == BRID )
                m_H = AS_GAP-0.2; // avoid overlap between absorber and gap
        else
        {
                if( m_ID == -1 ) // virtual encap gap
                        m_H = E_AS_RMAX - E_GP_DY;
		else if( m_ID == 1 )
		{       // for panel logical operation "AND" valid;
			m_H = E_AS_RMAX - E_GP_RMIN[m_Layer] +1.5*OVERLAP_WIDTH;
		}
                else
                        m_H = E_AS_RMAX - E_GP_RMIN[m_Layer];
        }
}

void MucGap::SetL()
{
        if( m_Part == BRID )
	{
		if( m_Segment != B_TOP || m_ID == -1 )
                	m_L = B_AS_LMAX;
		else // top segment
		{
			if( m_ID == 2 )
				m_L = B_AS_LMAX - B_GP_SLOT_LT;
			else
				m_L = B_GP_SLOT_LT;
		}
	}
        else
                m_L = AS_GAP;
}

void MucGap::SetWu()
{
        if( m_Part == BRID )
                m_Wu = m_W;
        else
        {	
		if( m_ID == -1 )     // temporary local 
			m_Wu = m_W;
                else if( m_ID == 1 ) // center fraction
		{	
                        m_Wu = 2 * VALUE * m_Rin ;
			m_Wu -= 2*VALUE*(2.0/2); // for panel operation "AND" valid
			m_Wu += 1.5*OVERLAP_WIDTH;
		}	
                else if( m_ID == 0 )
                        m_Wu = VALUE * m_Rin - E_GP_DY;
		else
			m_Wu = VALUE * m_Rin - E_GP_DX;

		if( m_ID != -1 ) m_Wu+=OVERLAP_WIDTH; // avoid panels overlap

        }
}

void MucGap::SetWd()
{
	 if( m_Part == BRID )
                m_Wd = m_W;
        else
        {	
		if( m_ID == -1 )     // temporary local
			m_Wd = m_W;
		else if( m_ID == 1 ) // center fraction
		{
                	m_Wd = 2 * VALUE * m_Rout;
			m_Wd += 2*VALUE*(2.0/2); // for panel operation "AND" valid
			m_Wd += 1.5*OVERLAP_WIDTH;
		}	
                else if( m_ID == 0 )
                	m_Wd = VALUE * m_Rout - E_GP_DY;
		else
			m_Wd = VALUE * m_Rout - E_GP_DX;

		if( m_ID != -1 ) m_Wd+=OVERLAP_WIDTH; // avoid panels overlap
        }
}


void MucGap::SetLocOrgInBes()
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
                //---------- set x and y ------------
		// segment 0 as reference 
               	x = (E_AS_RMAX + E_GP_DX) / 2.0;
               	y = (E_AS_RMAX + E_GP_DY) / 2.0;

                // x, y signs of coordinate different by segment 
                if( m_Segment == 0 )       {    ;               }
                else if ( m_Segment == 1 ) {    x = -x;         }
                else if ( m_Segment == 2 ) {    x = -x; y = -y; }
                else                       {    y = -y;         }

                //----------- set z ----------------
                for( int i=0; i<m_Layer+1; i++ ) z += E_AS_TH[i];
                z += m_Layer * AS_GAP;
                z += (E_AS_ZMAX - E_AS_TOTAL_TH ) + AS_GAP/2.0;
		z *= cos( m_Part*MUC_PI/2.0 );

        }// endcap

        m_LocOrgInBes[0] = x;
        m_LocOrgInBes[1] = y;
        m_LocOrgInBes[2] = z;

        // limit cut
        for(int i=0;i<3;i++) {
                if( fabs(m_LocOrgInBes[i]) < ERR_LIMIT ) m_LocOrgInBes[i] = 0;
        }
}

void MucGap::SetObjRotToMot()
{
        m_ObjRotToMot[0] = 0.;
        m_ObjRotToMot[1] = 0.;

        if( m_Part == BRID )
                m_ObjRotToMot[2] = MUC_PI * (m_Segment - 2) / 4.0;
        else
                m_ObjRotToMot[2] = 0.;
}

void MucGap::SetObjOrgInBes()
{
        double x, y, z;
	x = y = z = 0.;

        if( m_Part == BRID )
        {
		if( m_Segment != B_TOP  || m_ID == -1 )
                	for( int i=0; i<3; i++ ) m_ObjOrgInBes[i] = m_LocOrgInBes[i];
		else // top segment
		{
			// set x
			if( m_ID == 2 )	x = 0.;
			else		x = (1-2*m_ID) * ( B_GP_WT[m_Layer] + B_GP_SLOT_WT )/4.0; 
			

			// set z
			if( m_ID == 2 ) z = -B_GP_SLOT_LT / 2.0;
			else		z = ( B_AS_LMAX - B_GP_SLOT_LT )/2.0;
			
			m_ObjOrgInBes[0] = x;
			m_ObjOrgInBes[2] = z;
	                
			// set y
			m_ObjOrgInBes[1] = m_LocOrgInBes[1];

			// limit cut
        	        for( int i=0; i<3; i++ ) {
                	        if( fabs(m_ObjOrgInBes[i]) < ERR_LIMIT ) m_ObjOrgInBes[i] = 0;

                	}
		}
        }
        else
        {
		// -------------- set x and y --------------------
		// setting segment 0 as reference 
                if( m_ID == 0 )
                {
                	x = m_Rc;
                        y = ( m_Rc * tan( m_Theta ) + E_GP_DY ) / 2.0;
                }
                else if ( m_ID ==1 )
                {
                        x = m_Rc * cos( m_Theta );
                        y = m_Rc * sin( m_Theta );
                }
                else // m_ID == 2
                {
                        x = ( m_Rc / tan( m_Theta ) + E_GP_DX ) / 2.0;
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
                        if( fabs(m_ObjOrgInBes[i]) < ERR_LIMIT ) m_ObjOrgInBes[i] = 0;
                }

		// ------------- set z ------------------------
                m_ObjOrgInBes[2] = m_LocOrgInBes[2];
                
        } // else
}

void MucGap::SetObjOrgInLoc()
{
        if( m_Part == BRID)
	{	
		if( m_Segment != B_TOP )
	                for(int i=0; i<3; i++) m_ObjOrgInLoc[i] = 0.;
		else
			for(int i=0; i<3; i++) m_ObjOrgInLoc[i] = m_ObjOrgInBes[i] - m_LocOrgInBes[i];		
	}	
        else
                for(int i=0; i<3; i++) m_ObjOrgInLoc[i] = m_ObjOrgInBes[i] - m_LocOrgInBes[i];
}


// END
