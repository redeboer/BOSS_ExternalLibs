//------------------------------------------------------------------------------|
//      [File  ]:                       MucGapCal.cxx                              |
//      [Brief ]:       MUC geometry yoke creating class                        |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#include <iostream>
#include <cmath>

#include "MucCalibAlg/MucStructConst.h"
#include "MucCalibAlg/MucGapCal.h"

using namespace std;

// Constructor
MucGapCal::MucGapCal( int part, int segment, int layer ) : MucEntityCal( part, segment, layer )
{
  MucGapCal::Init();
  m_MucBoxCal = NULL;
}

// Copy constructor		
MucGapCal::MucGapCal( const MucGapCal &other ) : MucEntityCal( other )
{
  m_MucBoxCal  = other.m_MucBoxCal;
}

// Operator =
MucGapCal& MucGapCal::operator =( const MucGapCal &other )
{
  if( this == &other) return *this;
  MucEntityCal::operator =(other);
  m_MucBoxCal = other.m_MucBoxCal;

  return *this;	
}

// Destructor
MucGapCal::~MucGapCal()
{	
 delete m_MucBoxCal;
}

// Initialize
void MucGapCal::Init()
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
}

// ------------------- Get methods --------------------

MucBoxCal* MucGapCal::GetBox( )
{
  if( m_MucBoxCal != NULL ) 
    return m_MucBoxCal;
  else
    return ( m_MucBoxCal = new MucBoxCal(m_Part, m_Segment, m_Layer) );
}


// --------------------- Set motheds ----------------------
void MucGapCal::SetTheta() {
  if( m_Part == BRID )  m_Theta = m_Segment * ( PI/4.0 );
  else                  m_Theta = ( 2*m_Segment + 1 ) * ( PI/4.0 );
}

void MucGapCal::SetRin() {
  if( m_Part == BRID )  m_Rin = B_AS_RMIN[m_Layer] - AS_GAP;
  else                  m_Rin = 0.;
}

void MucGapCal::SetRout() {
  if( m_Part == BRID )  m_Rout = B_AS_RMIN[m_Layer];
  else                  m_Rout = E_AS_RMAX;
}

void MucGapCal::SetRc() {
  if( m_Part == BRID )  m_Rc = B_AS_RMIN[m_Layer] - AS_GAP/2.0;
  else                  m_Rc = sqrt(2.0) * E_AS_RMAX / 2.0;
}

void MucGapCal::SetThin() {
  m_Thin = AS_GAP;
}

void MucGapCal::SetW() {
  if( m_Part == BRID) m_W = B_GP_WT[m_Layer];
  else                m_W = E_AS_RMAX - E_GP_DX;
}

void MucGapCal::SetH()
{
  if( m_Part == BRID )
    m_H = AS_GAP-0.2; // avoid overlap between absorber and gap
  else
    m_H = E_AS_RMAX - E_GP_DY;
}

void MucGapCal::SetL() {
  if( m_Part == BRID )  m_L = B_GP_LT;
  else                  m_L = AS_GAP;
}

void MucGapCal::SetWu() {
  m_Wu = m_W;
}

void MucGapCal::SetWd() {
  m_Wd = m_W;
}

// END
