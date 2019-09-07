//------------------------------------------------------------------------------|
//      [File  ]:                       MucStripCal.cxx                         |
//      [Brief ]:       Class MucStripCal for MUC calibration                   |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Apil 6, 2005                                            |
//------------------------------------------------------------------------------|

#include<iostream>
#include<fstream>
#include<cmath>

using namespace std;

#include "MucCalibAlg/MucStructConst.h"
#include "MucCalibAlg/MucStripCal.h"

// Constructor
MucStripCal::MucStripCal( int part, int segment, int layer, int id ) : MucEntityCal( part, segment, layer, id )
{
  MucStripCal::Init();

  //  m_MucPadChain = NULL;
}

// Copy constructor
MucStripCal::MucStripCal( const MucStripCal& other ) : MucEntityCal( other )
{
  m_Type        = other.m_Type;
  m_CenterLine  = other.m_CenterLine;
  m_Head        = other.m_Head;
  m_Tail        = other.m_Tail;
  m_Phi         = other.m_Phi;

//  m_MucPadChain	= other.m_MucPadChain;
}

// Operator =
MucStripCal& MucStripCal::operator =( const MucStripCal &other )
{
  if( this == &other) return *this;
  
  MucEntityCal::operator = (other);
  
  m_Type          = other.m_Type;
  m_CenterLine    = other.m_CenterLine;
  m_Head          = other.m_Head;
  m_Tail          = other.m_Tail;
  m_Phi           = other.m_Phi;
  
  //  m_MucPadChain	= other.m_MucPadChain;
  
  return *this;
}

// Destructor
MucStripCal::~MucStripCal()
{
  //  delete m_MucPadChain;
  ;
}

// Initialize
void MucStripCal::Init()
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
  SetArea();
  SetPadNumber();
}

// ---------------------------------Get methods ------------------------

int    MucStripCal::GetType()       {   return m_Type;        }
double MucStripCal::GetCenterLine() {   return m_CenterLine;  }
double MucStripCal::GetHead()       {   return m_Head;        }
double MucStripCal::GetTail()       {   return m_Tail;        }
double MucStripCal::GetPhi()        {   return m_Phi;         }
int    MucStripCal::GetPadNumber()  {   return m_PadNumber;   }

/*
MucPadChain* MucStripCal::GetPadChain( )
{
  if( m_MucPadChain != NULL )
    return m_MucPadChain;
  else
    return ( m_MucPadChain = new MucPadChain( m_Part, m_Segment, m_Layer, m_PadNumber ) );
}
*/

//-----------------------------------Set methods-------------------------
// Set strip type(readout direction) 
void MucStripCal::SetType()
{
  if( m_Part == BRID ) {
    if( (m_Layer+1)%2 == 1 )  m_Type = ZSTR;
    else                      m_Type = PHISTR;
  }
  else {
    if( (m_Layer+1)%2 == 1 )  m_Type = XSTR;
    else                      m_Type = YSTR;
  }
}

// Set x or y of endcap strip certerline in BES coordinate system
void MucStripCal::SetCenterLine()
{
  // Base on strips width and structure correlations
  // Endcap only 

  int segFlag = 0;

  if( (m_Layer+1)%2 == 1 ) // 1,3,5,7
  {
    if( m_Segment < 2 )  segFlag = 1;
    else                 segFlag = -1;

    if( m_ID != E_STR_NUM-1 )                                               // NO.1~62
      m_CenterLine = segFlag * ( E_XSTR_OFFSET + m_ID*E_STR_DST );
    else                                                                    // NO.63 end strip
      m_CenterLine = segFlag * ( E_XSTR_OFFSET + (E_STR_NUM-2)*E_STR_DST + (E_STR_DST + m_W + STR_GAP)/2);
  }
  else                      // 2,4,6,8
  {
    if( m_Segment%3 == 0  ) segFlag = 1;
    else                    segFlag = -1;

    if(m_ID != E_STR_NUM-1)                                                 // NO.1~62
      m_CenterLine = segFlag * ( E_YSTR_OFFSET + m_ID*E_STR_DST );
    else                                                                    // NO.63 end strip
      m_CenterLine = segFlag * ( E_YSTR_OFFSET + (E_STR_NUM-2)*E_STR_DST + (E_STR_DST + m_W + STR_GAP)/2);
  }
}

// Set x and y of the head of endcap strip in BES coordinate system
void MucStripCal::SetHead()
{
  int segFlag = 0;
  
  // base on strips width and correlations
  if( m_ID+1 < TURN_STR_ID[m_Layer][0] )
    m_Head = FIRST_TURN_HEAD[m_Layer][0];
  
  if( m_ID+1 == TURN_STR_ID[m_Layer][0] )
    m_Head = FIRST_TURN_HEAD[m_Layer][1];
  
  if( (m_ID+1 > TURN_STR_ID[m_Layer][0]) && (m_ID+1 <= TURN_STR_ID[m_Layer][1]) )
    m_Head = FIRST_TURN_HEAD[m_Layer][1] - ( m_ID + 1 - TURN_STR_ID[m_Layer][0] )*E_STR_DST;
  
  if( m_ID+1 > TURN_STR_ID[m_Layer][1] )
    m_Head = SECOND_TURN_HEAD[m_Layer%2];
  
  if( (m_Layer+1)%2 == 1 )  // 1,3,5,7
  {
    if( m_Segment%3 ==0 ) segFlag = 1;
    else                  segFlag = -1;
  }
  else 			                // 2,4,6,8
  {
    if( m_Segment < 2 ) segFlag = 1;
    else                segFlag = -1;
  }
  
  m_Head *= segFlag;
}

// Set x and y of the tail of endcap strip in BES coordinate system
void MucStripCal::SetTail()
{
  int segFlag = 0;

  if( (m_Layer+1)%2 == 1 ) // 1,3,5,7
  {
    if( m_Segment%3 ==0 )   segFlag = 1;
    else                    segFlag = -1;

    if( m_ID+1 < TURN_POINT_NUM )
      m_Tail = ODD_TURN_POINT_TAIL[(m_Layer==0)?0:1][0];
    if( m_ID+1 == TURN_POINT_NUM )
      m_Tail = ODD_TURN_POINT_TAIL[(m_Layer==0)?0:1][1];
    if( m_ID+1 > TURN_POINT_NUM )
      m_Tail = ODD_TURN_POINT_TAIL[(m_Layer==0)?0:1][1]-( m_ID + 1 - TURN_POINT_NUM )* E_STR_DST;
    if( m_ID == E_STR_NUM - 1 )
      m_Tail = ODD_TURN_POINT_TAIL[(m_Layer==0)?0:1][1]-( m_ID -TURN_POINT_NUM )*E_STR_DST - ( E_STR_DST + m_W + STR_GAP )/2;
  }
  else                    // 2,4,6,8
  {
    if( m_Segment < 2 )     segFlag = 1;
    else                    segFlag = -1;

    if( m_ID+1 < TURN_POINT_NUM )
      m_Tail = EVEN_TURN_POINT_TAIL[0];
    if( m_ID+1 == TURN_POINT_NUM )
      m_Tail = EVEN_TURN_POINT_TAIL[1];
    if( m_ID+1 > TURN_POINT_NUM )
      m_Tail = EVEN_TURN_POINT_TAIL[1] - ( m_ID + 1 - TURN_POINT_NUM ) * E_STR_DST;
    if( m_ID == E_STR_NUM - 1 )
      m_Tail = EVEN_TURN_POINT_TAIL[1]-(m_ID - TURN_POINT_NUM)*E_STR_DST - ( E_STR_DST + m_W + STR_GAP )/2;
  }

  m_Tail *= segFlag;
}

// Set inclination angle of local coordinate x axis and global coordinate x axis
void MucStripCal::SetPhi()
{
  if ( m_Part == BRID )   // barrel 
    m_Phi = B_PHI[m_Segment] * (PI/4.0);      
  else    		// endcap , all phi is zer
    m_Phi = 0.;    
}

// set strip thickness
void MucStripCal::SetThin()
{
  m_Thin = STR_TH;
}

// Set strip width
void MucStripCal::SetW()
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
    // Strip0~62: 33;
    // Strip63:   L0: 34; L2/4/6: 43; L1/3/5/7: 53 
    if(m_ID != E_STR_NUM-1) m_W = E_STR_WT; // NO.1~62
    else // NO.63, end strip
    {
      if((m_Layer+1)%2!=0) // 0,2,4,6
      {
        if(m_Layer==0) m_W = 34;
        else m_W = 43;
      }
      else m_W = 53;      // 1,3,5,7
    }
  }
}

// Set strip Wu, no use
void MucStripCal::SetWu()
{
  m_Wu = m_W;
}

// Set strip Wd, no use
void MucStripCal::SetWd()
{
  m_Wd = m_W;
}

// Set strip height
void MucStripCal::SetH()
{
  m_H = STR_TH;
}

// Set strip length 
void MucStripCal::SetL()
{
  if( m_Part==BRID ) 	// barrel
  {
    int ZCUT_HEAD_NUM = B_ZSTR_NUM/2 - B_ZSTR_CUT_NUM[m_Layer] - 1;
    int ZCUT_TAIL_NUM = B_ZSTR_NUM/2 + B_ZSTR_CUT_NUM[m_Layer];
    
    if( m_Segment != B_TOP ) // not top segment
    {
      if( (m_Layer+1)%2 == 1)	// odd layer, Z 
        m_L = B_BOX_LT - 2*B_BOX_DZ[SL_DOWN];
      else 						        // even layer, phi
        m_L = B_BOX_WT[m_Layer] - 2*B_BOX_DZ[SL_DOWN];
    }
    else      			        // top segment
    {
      if((m_Layer+1)%2 == 1)  // odd layer, z
      {
        if( m_ID > ZCUT_HEAD_NUM && m_ID < ZCUT_TAIL_NUM ) 	// be cut
        {
          if( m_Layer == 0 )
            m_L = B_BOX_LT - 2*B_BOX_DZ[SL_DOWN] - B_ZSTR_CUT[1];
          else
            m_L = B_BOX_LT - 2*B_BOX_DZ[SL_DOWN] - B_ZSTR_CUT[0];
        }
        else 									            // no cut
          m_L = B_BOX_LT - 2*B_BOX_DZ[SL_DOWN];
      }
      else 			        // even layer, phi
      {
        if( m_ID < B_PHISTR_CUT_NUM ) 			// no cut
          m_L = B_BOX_WT[m_Layer] - 2*B_BOX_DZ[SL_DOWN];
        else 									// be cut
          m_L = (B_BOX_WT[m_Layer] - 2*B_BOX_DZ[SL_DOWN] - B_PHISTR_CUT)/2;
      }
    }
  }
  else                  // endcap
  {
    m_L = fabs( m_Head - m_Tail );
  }
}

// Set area(cm^2)
void MucStripCal::SetArea()
{
  m_Area = m_W*m_L/100;
}

// Set Pad number of strip
void MucStripCal::SetPadNumber()
{
  int    n    = 0;
  double fn   = 0;

  if(m_W == 0) {
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

// END

