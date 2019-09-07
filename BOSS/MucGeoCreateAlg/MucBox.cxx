//------------------------------------------------------------------------------|
//      [File  ]:                       MucBox.cxx                              |
//      [Brief ]:       MUC geometry box creating class 			|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#include <iostream>
#include <cmath>

#include "MucGeoCreateAlg/MucGeoConst.h"
#include "MucGeoCreateAlg/MucBox.h"

using namespace std;

// Constructor
MucBox::MucBox( int part, int segment, int layer, int id ) : MucEntity( part, segment, layer, id )
{
	MucBox::Init();
	
	m_MucStripPlane = NULL;
	m_MucRpc	= NULL;
	m_MucBoxCover	= NULL;
}

// Copy constructor		
MucBox::MucBox( const MucBox &other ) : MucEntity( other )
{
	m_MucStripPlane = other.m_MucStripPlane;
	m_MucRpc	= other.m_MucRpc;
	m_MucBoxCover	= other.m_MucBoxCover;
}

// Operator =
MucBox& MucBox::operator =( const MucBox &other )
{
	if( this == &other)
                return *this;

	MucEntity::operator =(other);

        m_MucRpc        = other.m_MucRpc;
        m_MucBoxCover   = other.m_MucBoxCover;
	m_MucStripPlane = other.m_MucStripPlane;

        return *this;	
}

// Destructor
MucBox::~MucBox()
{	
	delete m_MucStripPlane;
	delete m_MucRpc;
	delete m_MucBoxCover;	
}

// Initialize
void MucBox::Init()
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
	SetArea();

        SetLocOrgInBes();
        SetObjRotToMot();
        SetObjOrgInBes();
        SetObjOrgInLoc();
}
		
//----------------------------- Get methods --------------------------

MucStripPlane* MucBox::GetStripPlane( )
{
	if( m_MucStripPlane != NULL )
		return m_MucStripPlane;
	else
		return ( m_MucStripPlane = new MucStripPlane( m_Part, m_Segment, m_Layer, ((m_Part==BRID)?0:-1) ) );
}

MucRpc* MucBox::GetRpc( int upDown, int id )
{
        if( m_MucRpc != NULL )
                delete m_MucRpc;
	
        return ( m_MucRpc = new MucRpc( m_Part, m_Segment, m_Layer, upDown, id ) );
}

MucBoxCover* MucBox::GetBoxCover( int upDown, int id )
{
	if( m_MucBoxCover != NULL )
		delete m_MucBoxCover;

        return ( m_MucBoxCover = new MucBoxCover( m_Part, m_Segment, m_Layer, upDown, id ) );
}


//------------------------------- Set motheds -------------------------
void MucBox::SetTheta()
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

void MucBox::SetRin()
{
        if( m_Part == BRID )
                m_Rin = B_AS_RMIN[m_Layer] - ( AS_GAP + BOX_TH ) / 2.0;
        else
        {
                if( m_ID == -1 )
                        m_Rin = 0.;
                else
                        m_Rin = E_GP_RMIN[m_Layer];
        }
}

void MucBox::SetRout()
{
        if( m_Part == BRID )
                m_Rout = B_AS_RMIN[m_Layer] -( AS_GAP - BOX_TH ) / 2.0;
        else
                m_Rout = E_AS_RMAX - E_BOX_DR;
}

void MucBox::SetRc()
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

void MucBox::SetThin()
{
        m_Thin = BOX_TH;
}

void MucBox::SetW()
{
        if( m_Part == BRID)
	{
		if( m_Segment != B_TOP )
			 m_W = B_BOX_WT[m_Layer];
		else	// top segment
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

void MucBox::SetH()
{
        if( m_Part == BRID )
                m_H = BOX_TH;
        else
        {
                if( m_ID == -1 ) // virtual encap gap
                        m_H = E_AS_RMAX - E_BOX_DR - E_GP_DY;
                else if( m_ID == 1 )
		{       // for panel logical operation "AND" valid;
			m_H = E_AS_RMAX - E_BOX_DR - E_GP_RMIN[m_Layer] + OVERLAP_WIDTH;
		}
		else	
                        m_H = E_AS_RMAX - E_BOX_DR - E_GP_RMIN[m_Layer];
        }
}

void MucBox::SetL()
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
                m_L = BOX_TH;
}

void MucBox::SetWu()
{
        if( m_Part == BRID )
                m_Wu = m_W;
        else
        {	
		if( m_ID == -1 )     // virtual box 
			m_Wu = m_W;
                else if( m_ID == 1 ) // center fraction
		{	
                        m_Wu = 2 * VALUE * m_Rin ;
			m_Wu -= 2*VALUE*(2.0/2); // for panel operation "AND" valid
			m_Wu += OVERLAP_WIDTH;
		}	
                else if( m_ID == 0 )
                        m_Wu = VALUE * m_Rin - E_GP_DY;
		else
			m_Wu = VALUE * m_Rin - E_GP_DX;

		if( m_ID != -1 ) m_Wu+=OVERLAP_WIDTH; // avoid panels overlap
        }
}

void MucBox::SetWd()
{
	 if( m_Part == BRID )
                m_Wd = m_W;
        else
        {	
		if( m_ID == -1 )     // virtual box
			m_Wd = m_W;
		else if( m_ID == 1 ) // center fraction
		{	
                	m_Wd = 2 * VALUE * m_Rout;
			m_Wd += 2*VALUE*(2.0/2); // for panel operation "AND" valid
			m_Wd += OVERLAP_WIDTH;
		}	
                else if( m_ID == 0 )
                	m_Wd = VALUE * m_Rout - E_GP_DY;
		else
			m_Wd = VALUE * m_Rout - E_GP_DX;

		if( m_ID != -1 ) m_Wd+=OVERLAP_WIDTH; // avoid panels overlap

        }
}

void MucBox::SetArea()
{
	if( m_Part == BRID )
		m_Area = m_W * m_L;
	else
		m_Area = m_W * m_L; 
}


// local is gap
void MucBox::SetLocOrgInBes()
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
		if( m_ID == -1 ) // for box, local is gap
		{
               		x = (E_AS_RMAX + E_GP_DX) / 2.0;
               		y = (E_AS_RMAX + E_GP_DY) / 2.0;
		}
		else // for box panel, local is box
		{
	                x = (E_AS_RMAX - E_BOX_DR + E_GP_DX) / 2.0;
        	        y = (E_AS_RMAX - E_BOX_DR + E_GP_DY) / 2.0;
		}

                // x, y signs of coordinate different by segment 
                if	( m_Segment == 0 ) {    ;               }
                else if ( m_Segment == 1 ) {    x = -x;         }
                else if ( m_Segment == 2 ) {    x = -x; y = -y; }
                else                       {    y = -y;         }

                //------------- set z --------------------
                for( int i=0; i<m_Layer+1; i++ ) z += E_AS_TH[i];
                
		z += m_Layer * AS_GAP;
                z += (E_AS_ZMAX - E_AS_TOTAL_TH) + AS_GAP/2.0;  
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

void MucBox::SetObjRotToMot()
{
        m_ObjRotToMot[0] = 0.;
        m_ObjRotToMot[1] = 0.;

        if( m_Part == BRID )
        {
		// for barrel segment 5, 6 and 7, the box is uppended in the gap
             	if( m_Segment == 5 || m_Segment == 6 || m_Segment == 7 )
		   	m_ObjRotToMot[2] = MUC_PI;
		else
			m_ObjRotToMot[2] = 0.;
        }
        else
        {
                m_ObjRotToMot[2] = 0.;
        }
}

void MucBox::SetObjOrgInBes()
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
			else		x = (1-2*m_ID) * ( B_BOX_WT[m_Layer] + B_BOX_SLOT_WT )/4.0; 
			

			// set z
			if( m_ID == 2 ) z = -B_TOPRPC_LTS[(m_Layer==0)?1:(m_Layer%2)] / 2.0;
			else		z = ( B_BOX_LT - B_TOPRPC_LTS[(m_Layer==0)?1:(m_Layer%2)] )/2.0;
			
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
        else // endcap 
        {
		//------------- set x, y --------------------------
                // segment 0 as reference 
		if( m_ID == -1 )
		{
	                x = (E_AS_RMAX - E_BOX_DR + E_GP_DX) / 2.0; 
        	        y = (E_AS_RMAX - E_BOX_DR + E_GP_DY) / 2.0;	
		}
		else if( m_ID == 0 )
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
                if	( m_Segment == 0 ) {    ;               }
                else if ( m_Segment == 1 ) {    x = -x;         }
                else if ( m_Segment == 2 ) {    x = -x; y = -y; }
                else                       {    y = -y;         }
		
		m_ObjOrgInBes[0] = x;
		m_ObjOrgInBes[1] = y;	

	        // limit cut
        	for( int i=0; i<2; i++ ) {
        	        if( fabs(m_ObjOrgInBes[i]) < ERR_LIMIT ) m_ObjOrgInBes[i] = 0;
	        }
		
		//---------- set z ---------------------
                m_ObjOrgInBes[2] = m_LocOrgInBes[2];

        } // else, panels
}

void MucBox::SetObjOrgInLoc()
{
        if( m_Part == BRID )
	{
		if( m_Segment != B_TOP )
	                for(int i=0; i<3; i++) m_ObjOrgInLoc[i] = 0.;
		else
			for(int i=0; i<3; i++) m_ObjOrgInLoc[i] = m_ObjOrgInBes[i] - m_LocOrgInBes[i];
	}
        else
                for(int i=0; i<3; i++) m_ObjOrgInLoc[i] = m_ObjOrgInBes[i] - m_LocOrgInBes[i];
}

void MucBox::SetAlignment( double dx, double dy, double dz )
{
	
        if( m_Part == BRID || m_ID == -1 )
        {
		m_ObjOrgInLoc[0] += dx;
        	m_ObjOrgInLoc[1] += dy;
        	m_ObjOrgInLoc[2] += dz;
        }
}

// END
