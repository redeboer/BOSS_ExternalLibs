//------------------------------------------------------------------------------|
//      [File  ]:                       MucAbsorber.cxx                         |
//      [Brief ]:       MUC geometry yoke creating class 			|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#include <iostream>
#include <cmath>

#include "MucGeoCreateAlg/MucGeoConst.h"
#include "MucGeoCreateAlg/MucAbsorber.h"

using namespace std;

// Constructor
MucAbsorber::MucAbsorber( int part, int segment, int layer, int id ) : MucEntity( part, segment, layer, id )
{
	MucAbsorber::Init();
}

// Copy constructor		
MucAbsorber::MucAbsorber( const MucAbsorber &other ) : MucEntity( other ) { ; }

// Operator =
MucAbsorber& MucAbsorber::operator =( const MucAbsorber &other )
{
	if( this == &other)
                return *this;
	
	MucEntity::operator =(other);

	return *this;
}

// Destructor
MucAbsorber::~MucAbsorber() { ;	}
		
// Initialize
void MucAbsorber::Init()
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


// -------------------------- Set motheds -------------------
void MucAbsorber::SetTheta()
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

void MucAbsorber::SetRin()
{
        if( m_Part == BRID )
                m_Rin = B_AS_RMIN[m_Layer];
        else
        {
                if( m_ID == -1 )
                        m_Rin = 0.0;
                else
                        m_Rin = E_AS_RMIN[m_Layer];
        }
}

void MucAbsorber::SetRout()
{
        if( m_Part == BRID )
                m_Rout = B_AS_RMIN[m_Layer] + B_AS_TH[m_Layer] ;
        else
                m_Rout = E_AS_RMAX;
}


void MucAbsorber::SetRc()
{
	if( m_Part == BRID )
		m_Rc = B_AS_RMIN[m_Layer] + B_AS_TH[m_Layer]/2.0;
	else
	{
		if( m_ID == -1 )
			m_Rc = sqrt(2.0) * E_AS_RMAX / 2.0;
		else
			m_Rc = (m_Rin + m_Rout)/2.0;
	}
}

void MucAbsorber::SetThin()
{
        if( m_Part == BRID )
                m_Thin = B_AS_TH[m_Layer];
        else
                m_Thin = E_AS_TH[m_Layer];
}

void MucAbsorber::SetW()
{
	if( m_Part == BRID)
		m_W = 2.0 * VALUE * m_Rout;
	else
	{
		if( m_ID == -1 ) // temporary local 
			m_W = E_AS_RMAX;
		else
			m_W = 0;
	}
}

void MucAbsorber::SetH()
{
	if( m_Part == BRID )
		m_H = B_AS_TH[m_Layer];
	else
	{
		if( m_ID == -1 ) // temporary local
			m_H = E_AS_RMAX;
		else
			m_H = E_AS_RMAX - E_AS_RMIN[m_Layer];
	}
}

void MucAbsorber::SetL()
{
	if( m_Part == BRID )
		m_L = B_AS_LMAX;
	else
		m_L = E_AS_TH[m_Layer];
}

void MucAbsorber::SetWu()
{
	if( m_Part == BRID )
		m_Wu = 2 * VALUE * m_Rin;
	else
	{
		if( m_ID == -1 ) 	// temporary local
			m_Wu = m_W;
		else if( m_ID == 1 ) 	// center panel 
			m_Wu = 2 * VALUE * m_Rin;
		else
			m_Wu = VALUE * m_Rin;
	}

	m_Wu -= 0.8; //mm, avoid overlaping
}

void MucAbsorber::SetWd()
{
	 if( m_Part == BRID )
		  m_Wd = 2.0 * VALUE * m_Rout;
        else
        {
		if( m_ID == -1 ) 	// temporary local
			m_Wd = m_W;
        	if( m_ID == 1 ) 	// center panel 
                	m_Wd = 2.0 * VALUE * E_AS_RMAX;
                else
                	m_Wd = VALUE * E_AS_RMAX;
        }

	m_Wd -= 0.8;  //mm, avoid overlaping
}


void MucAbsorber::SetLocOrgInBes()
{
	double x, y, z;
  x = y = z = 0.0;
	
	//---------- set x and y --------
	if( m_Part == BRID || m_ID == -1 ) // the local(mother) in BES
	{	
                x = m_Rc * cos( m_Theta );
                y = m_Rc * sin( m_Theta );
	} // barrel
	else // for endcap panel, calculate the origin of temporary local in BES
	{	
		// set x and y	
		double tempR, tempTheta;
		tempR = sqrt(2.0) * E_AS_RMAX / 2.0;
		tempTheta = ( 2*m_Segment + 1 ) * ( MUC_PI / 4.0 );
		
		x = tempR * cos( tempTheta );
		y = tempR * sin( tempTheta );
		
	}// endcap
	
        //---------- set z  -----------	
	if( m_Part == BRID ) z = 0.;
	else
	{
                for( int i=0; i<m_Layer+1; i++ ) z += E_AS_TH[i];

                z += m_Layer * AS_GAP;
                z += (E_AS_ZMAX - E_AS_TOTAL_TH ) - m_Thin/2.0;  
		z *= cos( m_Part*MUC_PI/2.0 );
	}

       	m_LocOrgInBes[0] = x;
       	m_LocOrgInBes[1] = y;
       	m_LocOrgInBes[2] = z;

	// limit cut
	for(int i=0; i<3; i++) {
		if( fabs(m_LocOrgInBes[i]) < ERR_LIMIT ) m_LocOrgInBes[i] = 0; 
	}		
}

void MucAbsorber::SetObjRotToMot()
{
	m_ObjRotToMot[0] = 0.;
	m_ObjRotToMot[1] = 0.;

	if( m_Part == BRID )
		m_ObjRotToMot[2] = MUC_PI * (m_Segment - 2) / 4.0;
	else
		m_ObjRotToMot[2] = 0.;
}

void MucAbsorber::SetObjOrgInBes()
{
	double x, y;

	if( m_Part == BRID || m_ID == -1 ) // object is concided with local
	{
	        m_ObjOrgInBes[0] = m_LocOrgInBes[0];
	        m_ObjOrgInBes[1] = m_LocOrgInBes[1];
        	m_ObjOrgInBes[2] = m_LocOrgInBes[2];

	} // local 		
	else 
	{
               	//-------- set x and y ------------  
		// endcap panels
		// setting segment 0 as reference 
		if( m_ID == 0 )
		{
			x = m_Rc;
			y = m_Rc * tan( m_Theta ) / 2.0;
		}
		else if ( m_ID ==1 )
		{
			x = m_Rc * cos( m_Theta );
	                y = m_Rc * sin( m_Theta );	
		}
		else // m_ID == 2
		{
			x = m_Rc / tan( m_Theta ) / 2.0;
			y = m_Rc;
		}
			
		// x, y signs of coordinate different by segment 
		if( m_Segment == 0 ) 	   {	;		}
		else if ( m_Segment == 1 ) {	x = -x;		}
		else if ( m_Segment == 2 ) {	x = -x; y = -y; }
		else			   {    y = -y;		}

	        m_ObjOrgInBes[0] = x;
        	m_ObjOrgInBes[1] = y;

        	// limit cut
	        for( int i=0; i<2; i++ ) {
                	if( fabs(m_ObjOrgInBes[i]) < ERR_LIMIT ) m_ObjOrgInBes[i] = 0;
        	}
		
		//---------- set z--------------
		m_ObjOrgInBes[2] = m_LocOrgInBes[2];

	} // else, endcap panels
}

void MucAbsorber::SetObjOrgInLoc()
{
	if( m_Part == BRID || m_ID == -1 ) // for barrel and endcap local
		for(int i=0; i<3; i++) m_ObjOrgInLoc[i] = 0.;
	else 				   // for endcap panels
		for(int i=0; i<3; i++) m_ObjOrgInLoc[i] = m_ObjOrgInBes[i] - m_LocOrgInBes[i];
}


// END
