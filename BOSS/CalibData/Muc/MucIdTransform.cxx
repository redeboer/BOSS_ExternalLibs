//------------------------------------------------------------------------------|
//      [File  ]:                       MucIdTransform.cxx                      |
//      [Brief ]:       Source file of MucIdTransform class for encapsulation   |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Oct 19, 2006                                            |
//------------------------------------------------------------------------------|


#include<iostream>
#include<cmath>

using namespace std;

#include "CalibData/Muc/MucIdTransform.h"
#include "CalibData/Muc/MucCalibConst.h"
namespace CalibData {

// Constructor
MucIdTransform::MucIdTransform()
{
	m_Id	  = 0;
	m_Part 	  = 0;
	m_Segment = 0;
	m_Layer   = 0;
	m_Strip   = 0;
}

// Destructor
MucIdTransform::~MucIdTransform() { ; }

// Get properties 
int MucIdTransform::GetId()	 { return m_Id;	     }
int MucIdTransform::GetPart()    { return m_Part;    }
int MucIdTransform::GetSegment() { return m_Segment; }
int MucIdTransform::GetLayer()   { return m_Layer;   }
int MucIdTransform::GetStrip()   { return m_Strip;   }

// Return the maximum strip in a box
int MucIdTransform::GetStripMax( int part, int segment, int layer )
{
        int max = 0;

        if( part != BRID )              max = E_STR_NUM;
        else if( (layer+1)%2 == 1 )     max = B_ZSTR_NUM;
        else if( segment == B_TOP )     max = B_TOPSTR_NUM;
        else                            max = B_PHISTR_NUM;

        return max;
}

// Return the id of a box according its position
int MucIdTransform::GetBoxId( int part, int segment, int layer )
{
        int boxId = 0;
        for(int i=0; i<part; i++) {
                boxId += BOX_PER_PART[i];
        }

        if( segment == 0 )
                boxId += layer;
        else
                boxId += ( segment * BOX_PER_SEG[part] + layer );

        return boxId;
}

// Return the id of a strip according its position
int MucIdTransform::GetStripId( int part, int segment, int layer, int strSubId )
{
	int strId = 0;
        int boxId = 0;

        boxId = GetBoxId( part, segment, layer );

        if( part == EEID )
        {
                strId = boxId*E_STR_NUM + strSubId;
        }
        else if( part == BRID )
        {
                strId = STR_PER_PART[0];

                if( segment > B_TOP )
                        strId += segment * B_STR_PER_SEG[0] + E_STR_NUM;
                else
                        strId += segment * B_STR_PER_SEG[0];

                strId += ((1+layer)/2) * B_ZSTR_NUM;
                strId += (layer/2) * ( (segment == B_TOP)?B_TOPSTR_NUM:B_PHISTR_NUM );

                strId += strSubId;
        }
        else
        {
                strId = STR_PER_PART[0] + STR_PER_PART[1];
                strId += (boxId - BOX_SUM[1])*E_STR_NUM + strSubId;
        }

        return strId;
}

// Get the box posistion according to histogram pointer id of the box
bool MucIdTransform::SetBoxPos( int boxId, int* part, int* segment, int* layer )
{

	if( (boxId < 0) || (boxId > BOX_MAX-1) )
        {
                *part 	 = 0;
                *segment = 0;
                *layer 	 = 0;
                cout << "box id out range:\t" << boxId << "!" << endl;
		return false;
        }

        // get part
        if( boxId < BOX_SUM[0] )         { *part = 0;                       }
        else if( boxId < BOX_SUM[1] )    { *part = 1; boxId -= BOX_SUM[0];  }
        else                             { *part = 2; boxId -= BOX_SUM[1];  }

        // get segment and layer
        if( *part == BRID )
        {
                *segment = boxId / B_LAY_NUM;
                *layer = boxId % B_LAY_NUM;
        }
        else
        {
                *segment = boxId / E_LAY_NUM;
                *layer = boxId % E_LAY_NUM;
        }

        return true;
}	

// Get the strip posistion according to histogram pointer id of the strip
bool MucIdTransform::SetStripPos( int stripId, int *part, int *segment, int *layer, int *strSubId )
{
        if( (stripId < 0) || (stripId > STRIP_MAX-1) )
        {
                *part       = 0;
                *segment    = 0;
                *layer      = 0;
		*strSubId   = 0;
                cout << "strip id out range:\t" << stripId << "!" << endl;
                return false;
        }

        // get part
        if( stripId < STR_SUM[0] )       { *part = 0;                        }
        else if ( stripId < STR_SUM[1] ) { *part = 1; stripId -= STR_SUM[0]; }
        else                             { *part = 2; stripId -= STR_SUM[1]; }

        // get segment and layer
        if( *part == BRID )
        {
                int temp = 0;
                if ( stripId >= 2*B_STR_PER_SEG[0] && stripId < 2*B_STR_PER_SEG[0] + B_STR_PER_SEG[1] )
                {
                        // get segment
                        *segment = B_TOP;

                        stripId -= 2*B_STR_PER_SEG[0];
                        temp = stripId % ( B_ZSTR_NUM + B_TOPSTR_NUM );

                        // get layer
                        if( temp < B_ZSTR_NUM ) *layer = 2*( stripId / ( B_ZSTR_NUM + B_TOPSTR_NUM ) );
                        else                    *layer = 2*( stripId / ( B_ZSTR_NUM + B_TOPSTR_NUM ) ) + 1;

                        // get strip
                        if( temp < B_ZSTR_NUM ) *strSubId = temp;
                        else                    *strSubId = temp - B_ZSTR_NUM;

                } // top segment
                else
                {
                        if (stripId >= 2*B_STR_PER_SEG[0] + B_STR_PER_SEG[1] ) stripId -= E_STR_NUM;

                        // get segment
                        *segment = stripId / B_STR_PER_SEG[0];

                        stripId %= B_STR_PER_SEG[0];
                        temp = stripId % ( B_ZSTR_NUM + B_PHISTR_NUM );

                        // get layer
                        if( temp < B_ZSTR_NUM ) *layer = 2*( stripId / ( B_ZSTR_NUM + B_PHISTR_NUM ) );
                        else                    *layer = 2*( stripId / ( B_ZSTR_NUM + B_PHISTR_NUM ) ) + 1;

                        // get strip
                        if( temp < B_ZSTR_NUM ) *strSubId = temp;
                        else                    *strSubId = temp - B_ZSTR_NUM;
                }
        } // barrel
        else
        {
                *strSubId   = stripId % E_STR_NUM;
                *layer      = ( stripId / E_STR_NUM ) % E_LAY_NUM ;
                *segment    = ( stripId / E_STR_NUM ) / E_LAY_NUM ;
        }
	
	return true;
}
   
// Show transform results
void MucIdTransform::Print( int mode )	
{
	if( mode == 0 ) // position to id
	{
        	cout << "prt: "   << m_Part
             	     << "\tseg: " << m_Segment
                     << "\tlay: " << m_Layer
                     << "\tstr: " << m_Strip 
		     << "\tid: "  << m_Id << endl;		
	}
	else		// id to position
	{
		cout << "id: "    << m_Id			
        	     << "\tprt: " << m_Part
             	     << "\tseg: " << m_Segment
                     << "\tlay: " << m_Layer
                     << "\tstr: " << m_Strip << endl;
	}
}
}
//END

