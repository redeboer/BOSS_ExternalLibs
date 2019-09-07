//------------------------------------------------------------------------------|
//      [File  ]:                       MucBoxCal.cxx                           |
//      [Brief ]:       MUC geometry box creating class for calibration         |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 22, 2005                                            |
//------------------------------------------------------------------------------|

#include <iostream>
#include <cmath>

#include "MucCalibAlg/MucStructConst.h"
#include "MucCalibAlg/MucBoxCal.h"

using namespace std;

// Constructor
MucBoxCal::MucBoxCal( int part, int segment, int layer ) : MucEntityCal( part, segment, layer )
{
  MucBoxCal::Init();
}

// Copy constructor		
MucBoxCal::MucBoxCal( const MucBoxCal &other ) : MucEntityCal( other )
{
  ;
}

// Operator =
MucBoxCal& MucBoxCal::operator =( const MucBoxCal &other )
{
  if( this == &other) return *this;

  MucEntityCal::operator =(other);

  return *this;	
}

// Destructor
MucBoxCal::~MucBoxCal()
{
  ;	
}

// Initialize
void MucBoxCal::Init()
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
}
		
//------------------------------- Set motheds -------------------------
void MucBoxCal::SetTheta()
{
  if( m_Part == BRID )
    m_Theta = m_Segment * ( PI/4.0 );
  else
    m_Theta = ( m_Segment + 1 ) * ( PI/4.0 );
}

void MucBoxCal::SetRin()
{
  if( m_Part == BRID )
    m_Rin = B_AS_RMIN[m_Layer] - ( AS_GAP + BOX_TH ) / 2.0;
  else
    m_Rin = E_GP_RMIN[m_Layer] - E_STRPLN_DR;
}

void MucBoxCal::SetRout()
{
  if( m_Part == BRID )
    m_Rout = B_AS_RMIN[m_Layer] -( AS_GAP - BOX_TH ) / 2.0;
  else
    m_Rout = E_AS_RMAX - E_BOX_DR - E_STRPLN_DB[(m_Layer==0)?0:1];
}

void MucBoxCal::SetRc()
{
  if( m_Part == BRID )
    m_Rc = B_AS_RMIN[m_Layer] - AS_GAP/2.0;
  else
    m_Rc = (m_Rin + m_Rout)/2.0;
}

void MucBoxCal::SetThin()
{
  m_Thin = BOX_TH;
}

void MucBoxCal::SetW()
{
  if( m_Part == BRID) {
    m_W = B_BOX_WT[m_Layer];
  }
  else {
    m_W = E_AS_RMAX - E_BOX_DR - E_GP_DX;
  }
}

void MucBoxCal::SetH()
{
  if( m_Part == BRID )
    m_H = BOX_TH;
  else
    m_H = E_AS_RMAX - E_BOX_DR - E_STRPLN_DB[(m_Layer==0)?0:1] - E_STRPLN_DR - E_GP_RMIN[m_Layer];
}

void MucBoxCal::SetL()
{
  if( m_Part == BRID )
    m_L = B_BOX_LT;
  else
    m_L = BOX_TH;
}

void MucBoxCal::SetWu()
{
  if( m_Part == BRID )
    m_Wu = m_W;
  else
    m_Wu = 2 * VALUE * m_Rin ;
}

void MucBoxCal::SetWd()
{
  if( m_Part == BRID )
    m_Wd = m_W;
  else
    m_Wd = 2 * VALUE * m_Rout;
}

void MucBoxCal::SetArea()
{
  if( m_Part == BRID ) {
    m_Area = m_W * m_L;
    if( m_Segment == B_TOP ) m_Area -= B_BOX_SLOT_WT*B_TOPRPC_LTS[(m_Layer==0)?1:(m_Layer%2)];
  }
  else
    m_Area = (m_Wu + m_Wd)*m_H - (E_GP_DX + E_GP_DY + 2*E_STRPLN_DA)* m_H; 
  
  m_Area /= 100; // mm^2 -> cm^2
}

// END
