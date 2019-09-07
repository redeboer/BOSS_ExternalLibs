//------------------------------------------------------------------------------|
//      [File  ]:                       MucStrip.cxx                            |
//      [Brief ]:       Class MucStrip for MUC calibration   			|
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Apil 6, 2005                                            |
//------------------------------------------------------------------------------|

#include<iostream>
#include<fstream>
#include<cmath>

using namespace std;

#include "MucGeoCreateAlg/MucGeoConst.h"
#include "MucGeoCreateAlg/MucStrip.h"

// Constructor
MucStrip::MucStrip( int part, int segment, int layer, int id ) : MucEntity( part, segment, layer, id )
{
	MucStrip::Init();

//	m_MucPadChain = NULL;
}

// Copy constructor
MucStrip::MucStrip( const MucStrip& other ) : MucEntity( other )
{
	m_Type		= other.m_Type;
	m_CenterLine	= other.m_CenterLine;
	m_Head		= other.m_Head;
	m_Tail		= other.m_Tail;
	m_Phi		= other.m_Phi;

//	m_MucPadChain	= other.m_MucPadChain;
}

// Operator =
MucStrip& MucStrip::operator =( const MucStrip &other )
{
        if( this == &other)
                return *this;

	MucEntity::operator =(other);

	m_Type          = other.m_Type;
        m_CenterLine    = other.m_CenterLine;
        m_Head          = other.m_Head;
        m_Tail          = other.m_Tail;
        m_Phi           = other.m_Phi;

//	m_MucPadChain	= other.m_MucPadChain;

        return *this;
}

// Destructor
MucStrip::~MucStrip()
{
//	delete m_MucPadChain;
	;
}

// Initialize
void MucStrip::Init()
{
	SetType();
        SetW();
        SetWu();
        SetWd();
	SetCenterLine(); // use strip width
	SetHead();
	SetTail();
	SetPhi();
	
        SetThin();
        SetH();
        SetL();
	SetPadNumber();
	
        SetLocOrgInBes();
        SetObjRotToMot();

	SetEndcapStripInBes();
	SetBarrelStripInLoc();

        SetObjOrgInBes();
        SetObjOrgInLoc(); 
}

// ---------------------------------Get methods ------------------------

int    MucStrip::GetType()	{	return m_Type;		}
double MucStrip::GetCenterLine(){       return m_CenterLine;	}
double MucStrip::GetHead()	{       return m_Head;		}
double MucStrip::GetTail()	{       return m_Tail;		}
double MucStrip::GetPhi()	{	return m_Phi;		}
int    MucStrip::GetPadNumber() {	return m_PadNumber;	}

/*
MucPadChain* MucStrip::GetPadChain( )
{
	if( m_MucPadChain != NULL )
		return m_MucPadChain;
	else
        	return ( m_MucPadChain = new MucPadChain( m_Part, m_Segment, m_Layer, m_PadNumber ) );
}

*/

//-----------------------------------Set methods-------------------------
// Set strip type(readout direction) 
void MucStrip::SetType()
{
	if( m_Part == BRID )
	{
		if( (m_Layer+1)%2 == 1 ) 	m_Type = ZSTR;
		else		     		m_Type = PHISTR;
	}
	else
	{
		if( (m_Layer+1)%2 == 1 ) 	m_Type = XSTR;
		else		     		m_Type = YSTR;
	}
}
 

// Set x or y of endcap strip certerline in BES coordinate system
void MucStrip::SetCenterLine()
{
        // Base on strips width and structure correlations
        // Endcap only 

        int segFlag = 0;

        if( (m_Layer+1)%2 == 1 ) // 1,3,5,7
        {
                if( m_Segment < 2 )     segFlag = 1;
                else                    segFlag = -1;
		
		if( m_ID == 0 ) m_CenterLine = segFlag * (E_GP_DY+E_STRPLN_DA+m_W/2.0);	// NO.0
		else if( m_ID != E_STR_NUM-1 )                                          // NO.1~62
                        m_CenterLine = segFlag * ( E_XSTR_OFFSET + m_ID*E_STR_DST );
                else                                                                    // NO.63 end strip
                        m_CenterLine = segFlag * ( E_XSTR_OFFSET + (E_STR_NUM-2)*E_STR_DST + (E_STR_DST + m_W + STR_GAP)/2);
        }
        else                    // 2,4,6,8
        {
                if( m_Segment%3 == 0  ) segFlag = 1;
                else                    segFlag = -1;

		if( m_ID == 0 ) m_CenterLine = segFlag * (E_GP_DX+E_STRPLN_DA+m_W/2.0);	// NO.0
		else if(m_ID != E_STR_NUM-1)                                            // NO.1~62
                        m_CenterLine = segFlag * ( E_YSTR_OFFSET + m_ID*E_STR_DST );
                else                                                                    // NO.63 end strip
                        m_CenterLine = segFlag * ( E_YSTR_OFFSET + (E_STR_NUM-2)*E_STR_DST + (E_STR_DST + m_W + STR_GAP)/2);
        }
}

// Set x and y of the head of endcap strip in BES coordinate system
void MucStrip::SetHead()
{
	int segFlag = 0;

	// base on strips width and correlations
	if( m_ID+1 < HEAD_TURN_STR_NUM[m_Layer][0] )
		m_Head = FIRST_TURN_HEAD[m_Layer][0];
	else if( m_ID+1 == HEAD_TURN_STR_NUM[m_Layer][0] )
		m_Head = FIRST_TURN_HEAD[m_Layer][1];
	else if( (m_ID+1 > HEAD_TURN_STR_NUM[m_Layer][0]) && (m_ID+1 < HEAD_TURN_STR_NUM[m_Layer][1]) )
		m_Head = FIRST_TURN_HEAD[m_Layer][1] - ( m_ID + 1 - HEAD_TURN_STR_NUM[m_Layer][0] )*E_STR_DST;
	else if( m_ID+1 >= HEAD_TURN_STR_NUM[m_Layer][1] )
		m_Head = SECOND_TURN_HEAD[m_Layer%2];


  	if( (m_Layer+1)%2 == 1 ) // 1,3,5,7
    	{
      		if( m_Segment%3 ==0 ) segFlag = 1;
      		else segFlag = -1;
    	}
  	else 			// 2,4,6,8
    	{
      		if( m_Segment < 2 ) segFlag = 1;
      		else segFlag = -1;
    	}

    	m_Head *= segFlag;
}

// Set x and y of the tail of endcap strip in BES coordinate system
void MucStrip::SetTail()
{
        int segFlag = 0;
 
        if( (m_Layer+1)%2 == 1 ) // 1,3,5,7
        {
                if( m_Segment%3 ==0 )   segFlag = 1;
                else                    segFlag = -1;

                if( m_ID+1 < TAIL_TURN_STR_NUM[0] )
                        m_Tail = ODD_TURN_POINT_TAIL[(m_Layer==0)?0:1][0];
		else if( m_ID+1 == TAIL_TURN_STR_NUM[0] )
                        m_Tail = ODD_TURN_POINT_TAIL[(m_Layer==0)?0:1][1];
		else if( m_ID+1 > TAIL_TURN_STR_NUM[0] )
                        m_Tail = ODD_TURN_POINT_TAIL[(m_Layer==0)?0:1][1]-( m_ID + 1 - TAIL_TURN_STR_NUM[0] )* E_STR_DST;
                if( m_ID == E_STR_NUM - 1 )
                        m_Tail = ODD_TURN_POINT_TAIL[(m_Layer==0)?0:1][1]-( m_ID -TAIL_TURN_STR_NUM[0] ) * E_STR_DST - ( E_STR_DST + m_W + STR_GAP )/2;
        }
        else                    // 2,4,6,8
        {
                if( m_Segment < 2 )     segFlag = 1;
                else                    segFlag = -1;

                if( m_ID+1 < TAIL_TURN_STR_NUM[1] )
                        m_Tail = EVEN_TURN_POINT_TAIL[0];
                if( m_ID+1 == TAIL_TURN_STR_NUM[1] )
                        m_Tail = EVEN_TURN_POINT_TAIL[1];
                if( m_ID+1 > TAIL_TURN_STR_NUM[1] )
                        m_Tail = EVEN_TURN_POINT_TAIL[1] - ( m_ID + 1 - TAIL_TURN_STR_NUM[1] ) * E_STR_DST;
                if( m_ID == E_STR_NUM - 1 )
                        m_Tail = EVEN_TURN_POINT_TAIL[1]-(m_ID - TAIL_TURN_STR_NUM[1]) * E_STR_DST - ( E_STR_DST + m_W + STR_GAP )/2;
        }

        m_Tail *= segFlag;
}

// Set inclination angle of local coordinate x axis and global coordinate x axis
void MucStrip::SetPhi()
{
        if ( m_Part == BRID )   // barrel 
                m_Phi = B_PHI[m_Segment] * (MUC_PI/4.0);      
        else    		// endcap , all phi is zer
		m_Phi = 0.;   
}

// set strip thickness
void MucStrip::SetThin()
{
	m_Thin = STR_TH;
}

// Set strip width
void MucStrip::SetW()
{
        if( m_Part == BRID )
        {
                // Set maximum strip 
                int maxStrip;
                if( (m_Layer+1)%2 == 1 )
                        maxStrip = B_ZSTR_NUM;          // odd layer
                else if( m_Segment != B_TOP )
                        maxStrip = B_PHISTR_NUM;        // even layer not top segment
                else
                        maxStrip = B_TOPSTR_NUM;        // even layer top segment


                if( (m_Layer+1)%2 == 1 ) // odd layer, z
                {
                        if( m_ID > 0 && m_ID < (maxStrip-1))                    // not marginal strip
                                m_W = B_STR_DST[m_Layer] - STR_GAP;
                        else                                                    // marginal strip
                                m_W = (B_BOX_WT[m_Layer] - 2*B_BOX_DT[SL_DOWN] - (maxStrip-2)*B_STR_DST[m_Layer] - STR_GAP)/2;
                }
                else                    // even layer, phi
                {
                        if( m_ID >0 && m_ID < (maxStrip-1) )                    // not marginal strip
                                m_W = B_STR_DST[m_Layer] - STR_GAP;
                        else                                                    // marginal strip
                                m_W = B_BOX_LT/2 - B_BOX_DZ[SL_DOWN] - B_STR_DST[m_Layer]*(B_PHISTR_NUM-2)/2;

                        if( (maxStrip == B_TOPSTR_NUM) && (m_ID == (B_PHISTR_NUM - 1)) ) // special marginal strip
                                m_W = B_BOX_LT/2 - B_BOX_DZ[SL_DOWN] - B_STR_DST[m_Layer]*(B_PHISTR_NUM-2)/2;
                }
        }
        else
        {
		// Strip0:48;
                // Strip1~62: 33;
                // Strip63:   L0: 34; L2/4/6: 43; L1/3/5/7: 53 
		if(m_ID ==0 ) m_W = E_FIRST_STR_WT;
		else if(m_ID != E_STR_NUM-1) m_W = E_STR_WT; // NO.1~62
                else // NO.63, end strip
                {
                        if((m_Layer+1)%2!=0) // 0,2,4,6
                        {
                                if(m_Layer==0) m_W = 31;
                                else m_W = 43;
                        }
                        else m_W = 53; // 1,3,5,7
                }
        }
}

// Set strip Wu, no use
void MucStrip::SetWu()
{
	m_Wu = m_W;
}

// Set strip Wd, no use
void MucStrip::SetWd()
{
        m_Wd = m_W;
}

// Set strip height
void MucStrip::SetH()
{
        m_H = STR_TH;
}

// Set strip length 
void MucStrip::SetL()
{
	if( m_Part==BRID ) 	// barrel
	{
		int ZCUT_HEAD_NUM = B_ZSTR_NUM/2 - B_ZSTR_CUT_NUM[m_Layer] - 1;
		int ZCUT_TAIL_NUM = B_ZSTR_NUM/2 + B_ZSTR_CUT_NUM[m_Layer];


		if( m_Segment != B_TOP ) // not top segment
		{
			if( (m_Layer+1)%2 == 1) 			// odd layer, Z 
		      		m_L = B_BOX_LT - 2*B_BOX_DZ[SL_DOWN];
		      	else 						// even layer, phi
		        	m_L = B_BOX_WT[m_Layer] - 2*B_BOX_DZ[SL_DOWN];
		}
		else 			// top segment
		{
			if((m_Layer+1)%2 == 1) // odd layer, z
		      	{
		        	if( m_ID > ZCUT_HEAD_NUM && m_ID < ZCUT_TAIL_NUM ) 			// be cut
		          	{
                			if( m_Layer == 0 )
		                  		m_L = B_BOX_LT - 2*B_BOX_DZ[SL_DOWN] - B_ZSTR_CUT[1];
                		  	else
		                  		m_L = B_BOX_LT - 2*B_BOX_DZ[SL_DOWN] - B_ZSTR_CUT[0];
		          	}
		          	else 									// no cut
		          		m_L = B_BOX_LT - 2*B_BOX_DZ[SL_DOWN];
		      	}
      			else 			// even layer, phi
       	 		{
          			if( m_ID < B_PHISTR_CUT_NUM ) 						// no cut
            				m_L = B_BOX_WT[m_Layer] - 2*B_BOX_DZ[SL_DOWN];
          			else 									// be cut
            				m_L = (B_BOX_WT[m_Layer] - 2*B_BOX_DZ[SL_DOWN] - B_PHISTR_CUT)/2;
        		}
    		}
	}
	else			// endcap
	{
		m_L = fabs( m_Head - m_Tail );
	}
}

// Set area
void MucStrip::SetArea()
{
	m_Area = m_W*m_L/100;
}

// Set Pad number of strip
void MucStrip::SetPadNumber()
{
        int    n        = 0;
        double fn       = 0;

         if(m_W == 0)
         {
                cout<<"Error, strip width = 0!"<<endl;
                m_PadNumber = 0;
         }
         else
        {
                fn = (m_L/m_W);
                n  = (int)fn;

                if( (fn - n) > 0.3 )
                        m_PadNumber = n + 1;
                else
                        m_PadNumber = n;
        }
}


// Set local or temporary local(MucStripPlane) origin coordinates in BES
void MucStrip::SetLocOrgInBes()
{
        double x, y, z = 0 ;

        if( m_Part == BRID )
        {
                x = ( B_AS_RMIN[m_Layer] - AS_GAP/2.0 ) * cos( m_Segment * ( MUC_PI/4.0 ) );
                y = ( B_AS_RMIN[m_Layer] - AS_GAP/2.0 ) * sin( m_Segment * ( MUC_PI/4.0 ) );
                z = 0.;

        } // barrel
        else
        {
                //------------- set x and y --------------------
                // segment 0 as reference 
                x = (E_AS_RMAX - E_BOX_DR - E_STRPLN_DB[(m_Layer==0)?0:1] + E_GP_DX + E_STRPLN_DA) / 2.0;
                y = (E_AS_RMAX - E_BOX_DR - E_STRPLN_DB[(m_Layer==0)?0:1] + E_GP_DY + E_STRPLN_DA) / 2.0;
         
                // x, y signs of coordinate different by segment 
                if( m_Segment == 0 )       {    ;               }
                else if ( m_Segment == 1 ) {    x = -x;         }
                else if ( m_Segment == 2 ) {    x = -x; y = -y; }
                else                       {    y = -y;         }

                //-------------- set z --------------------------
                for( int i=0; i<m_Layer+1; i++ ) z += E_AS_TH[i];
                
		z += m_Layer * AS_GAP;
                z += (E_AS_ZMAX - E_AS_TOTAL_TH) + AS_GAP/2.0; 
		z *= cos( m_Part*MUC_PI/2.0 );

        } // else, endcap
       
        m_LocOrgInBes[0] = x;
        m_LocOrgInBes[1] = y;
        m_LocOrgInBes[2] = z;

        // limit cut
        for( int i=0; i<3; i++ ) {
        	if( fabs(m_LocOrgInBes[i]) < ERR_LIMIT ) m_LocOrgInBes[i] = 0;
	}
}

/*
void MucStrip::SetObjRotToMot()
{
	// The x, y and z of strip coordinate system is always parallele with those of strip plane  
	for( int i=0; i<3; i++ )
		m_ObjRotToMot[i] = 0.0;
}
*/

// Set endcap strip origin coordinates in global
void MucStrip::SetEndcapStripInBes()
{	
	double x, y, z;

	// Set x 
        if( (m_Layer+1)%2 == 0 ) 	// even layer
        	x = m_CenterLine;
        else			 	// odd layer
                x = ( m_Head + m_Tail )/2;
 
        // Set y
        if( (m_Layer+1)%2 == 0 ) 	// even layer
                y = ( m_Head + m_Tail )/2;
        else				// odd layer
                y = m_CenterLine;

        // Set z
        z = 0.;
        for(int i=0; i<m_Layer+1; i++)	z += E_AS_TH[i];

        z += m_Layer * AS_GAP;
        z = ( (E_AS_ZMAX - E_AS_TOTAL_TH + z) + AS_GAP/2.0 ) * cos( m_Part*MUC_PI/2.0 );

        m_ObjOrgInBes[0] = x;
        m_ObjOrgInBes[1] = y;	
       	m_ObjOrgInBes[2] = z;

        // limit cut
        for( int i=0; i<3; i++ ) {
	        if( fabs(m_ObjOrgInBes[i]) < ERR_LIMIT ) m_ObjOrgInBes[i] = 0.;
        }	
}

// Set x, y and z of barrel strip origin in local or temparory local(MucStripPlane) coordinate
void MucStrip::SetBarrelStripInLoc()
{
  	int j = m_Layer;
  	int n = m_ID;
  	
	int maxStrip = 0;
	double x, y, z = 0;
 
  	if( m_Type == ZSTR )		maxStrip = B_ZSTR_NUM; 		// odd layer
  	else if( m_Segment != B_TOP )	maxStrip = B_PHISTR_NUM; 	// even layer not top segment
  	else				maxStrip = B_TOPSTR_NUM; 	// even layer top segment

  	if( m_Segment != B_TOP ) // not top segment
    	{
      		if( m_Type == ZSTR ) 	// odd layer, ZSTR
        	{
          		// Set x
          		if( n == 0 )
             			x = ( B_BOX_WT[j] - 2*B_BOX_DT[SL_DOWN] +  (maxStrip-2)*B_STR_DST[j] + STR_GAP )/4.0;
          		else if( n == (maxStrip-1) )
             			x = ( 2*B_BOX_DT[SL_DOWN] - B_BOX_WT[j] - (maxStrip-2)*B_STR_DST[j] - STR_GAP )/4.0;
          		else
            			x =( ((maxStrip-1) - 2*n) * B_STR_DST[j] )/2.0;

          		// Set y
          		y = 0.;

          		// Set z
          		z = 0.;

        	}
      		else 			// even layer, phi
        	{
          		// Set x
          		x = 0.;

          		// Set y
          		y = 0.;

	          	// Set Z
          		if( n == 0 )
            			z = -B_BOX_LT/4.0 + B_BOX_DZ[SL_DOWN]/2.0 - B_STR_DST[j]*(B_PHISTR_NUM-2)/4.0;
          		else if( n == (B_PHISTR_NUM - 1) )
            			z = B_BOX_LT/4.0 - B_BOX_DZ[SL_DOWN]/2.0 + B_STR_DST[j]*(B_PHISTR_NUM-2)/4.0;
          		else
            			z = ( 2*n - B_PHISTR_NUM + 1 ) * B_STR_DST[j] / 2.0;
        	}

    	} 	// end not top segment
  	else 	// top segment
    	{
      		if( m_Type == ZSTR ) 	// odd layer, z
        	{
          		// Set x
          		if( n == 0 )
            			x = B_BOX_WT[j]/4.0 - B_BOX_DT[SL_DOWN]/2.0 +  (maxStrip-2)*B_STR_DST[j]/4.0 + STR_GAP/4.0;
          		else if( n == (maxStrip-1) )
            			x = -B_BOX_WT[j]/4.0 + B_BOX_DT[SL_DOWN]/2.0 -  (maxStrip-2)*B_STR_DST[j]/4.0 - STR_GAP/4.0;
          		else
            			x = ( (maxStrip-1) - 2*n ) * B_STR_DST[j] / 2.0;

          		// Set y
          		y = 0.;

          		// Set z
          		if( (n > (B_ZSTR_NUM/2 - 1 - B_ZSTR_CUT_NUM[j])) && (n < (B_ZSTR_NUM/2 + B_ZSTR_CUT_NUM[j])) ) 	// be cut
            			z = -( (j==0) ? B_ZSTR_CUT[1] : B_ZSTR_CUT[0] )/2;
          		else 												// no cut
            			z = 0.;
        	} 
      		else 			// even layer, phi
        	{
          		// Set x
          		if( n < B_PHISTR_CUT_NUM ) 	// no cut
            			x = 0.;	
          		else 				// be cut
            		{
              			if( n < B_PHISTR_NUM )
                			x = -B_BOX_WT[j]/4.0 - B_PHISTR_CUT/4.0 + B_BOX_DZ[SL_DOWN]/2.0;
              			else
                			x = B_BOX_WT[j]/4.0 + B_PHISTR_CUT/4.0 - B_BOX_DZ[SL_DOWN]/2.0;
            		}

          		// Set y 
          		y = 0.;

	         	// Set z
          		if( n < B_PHISTR_NUM )
            		{
              			if( n == 0 )
                			z = -B_BOX_LT/4.0 + B_BOX_DZ[SL_DOWN]/2.0 - B_STR_DST[j]*(B_PHISTR_NUM-2)/4.0;
              			else if( n == (B_PHISTR_NUM - 1) )
                  			z = B_BOX_LT/4.0 - B_BOX_DZ[SL_DOWN]/2.0 + B_STR_DST[j]*(B_PHISTR_NUM-2)/4.0;
                		else
                  			z = ( 2*n - B_PHISTR_NUM + 1 ) * B_STR_DST[j] / 2.0;
            		}
          		else
            		{
              			if( n == (B_TOPSTR_NUM-1) )
                			z = B_BOX_LT/4.0 - B_BOX_DZ[SL_DOWN]/2.0 + B_STR_DST[j]*(B_PHISTR_NUM-2)/4.0;
              			else
                			z = ( 2*n - 3*B_PHISTR_NUM + 2*B_PHISTR_CUT_NUM + 1 ) * B_STR_DST[j]/2.0;
            		}
        	} 			// end even 		
    	} 	// end top segment

	m_ObjOrgInLoc[0] = x;
        m_ObjOrgInLoc[1] = y;
        m_ObjOrgInLoc[2] = z;	

        // limit cut
        for( int i=0; i<3; i++ ) {
                if( fabs(m_ObjOrgInLoc[i]) < ERR_LIMIT )
                m_ObjOrgInLoc[i] = 0.;
        }	
}

// Set strip origin in global coordinates
void MucStrip::SetObjOrgInBes()
{
	if( m_Part == BRID )
	{
		MucEntity::TransLocToBes( m_ObjOrgInBes, m_ObjOrgInLoc, m_LocOrgInBes, m_Phi );
		
	        // limit cut
	        for( int i=0; i<3; i++ ) {
                	if( fabs(m_ObjOrgInBes[i]) < ERR_LIMIT ) m_ObjOrgInBes[i] = 0.0;
        	}
	}
}

// Set strip local and global coordinates
void MucStrip::SetObjOrgInLoc()
{
	if( m_Part != BRID )
	{
		MucEntity::TransBesToLoc( m_ObjOrgInLoc, m_ObjOrgInBes, m_LocOrgInBes, m_Phi );
		
	        // limit cut
	        for( int i=0; i<3; i++ ) {
                	if( fabs(m_ObjOrgInLoc[i]) < ERR_LIMIT ) m_ObjOrgInLoc[i] = 0.;
        	}
	}
}


// END

