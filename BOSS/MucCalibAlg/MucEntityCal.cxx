//------------------------------------------------------------------------------|
//      [File  ]:                       MucEntityCal.cxx                        |
//      [Brief ]:       MUC geometry entity class for calibration               |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Mar 28, 2006                                            |
//------------------------------------------------------------------------------|

#include<iostream>
#include<string>

#include "MucCalibAlg/MucEntityCal.h"

using namespace std;

// Constructor
MucEntityCal::MucEntityCal( int part, int segment, int layer )
{
  m_Part      = part;
  m_Segment   = segment;
  m_Layer     = layer;
  m_ID        = 0;		// only 1 entity, default

  MucEntityCal::Init();
}

MucEntityCal::MucEntityCal( int part, int segment, int layer, int id )
{
  m_Part    = part;
  m_Segment = segment;
  m_Layer   = layer;
  m_ID      = id;
  
  Init();
}

MucEntityCal::MucEntityCal( const MucEntityCal &other )
{
  m_Part    = other.m_Part;
  m_Segment = other.m_Segment;
  m_Layer   = other.m_Layer;
  m_ID      = other.m_ID;
  
  m_Theta   = other.m_Theta;
  m_Thin    = other.m_Thin;
  m_W       = other.m_W;
  m_H       = other.m_H;
  m_L       = other.m_L;
  m_Wu      = other.m_Wu;
  m_Wd      = other.m_Wd;
  m_Area    = other.m_Area;
}

MucEntityCal& MucEntityCal::operator =( const MucEntityCal &other )
{
  if( this == &other) return *this;
	
  m_Part    = other.m_Part;
  m_Segment = other.m_Segment;
  m_Layer   = other.m_Layer;
  m_ID      = other.m_ID;
  
  m_Theta   = other.m_Theta;	
  m_Thin    = other.m_Thin;	
  m_W       = other.m_W;
  m_H       = other.m_H;
  m_L       = other.m_L;
  m_Wu      = other.m_Wu;
  m_Wd      = other.m_Wd;
  m_Area    = other.m_Area;
  
  return *this;
}

// Destructor
MucEntityCal::~MucEntityCal()
{
  ;
}

// Get methods
int MucEntityCal::GetPart()     { return m_Part;    }
int MucEntityCal::GetSegment() 	{ return m_Segment; }
int MucEntityCal::GetLayer()    { return m_Layer;   }
int MucEntityCal::GetID()       { return m_ID;      }
double MucEntityCal::GetTheta() { return m_Theta;   }
double MucEntityCal::GetRin()   { return m_Rin;     }
double MucEntityCal::GetRout()  { return m_Rout;    }
double MucEntityCal::GetRc()    { return m_Rc;      }
double MucEntityCal::GetThin()  { return m_Thin;    } 
double MucEntityCal::GetW()     { return m_W;       }
double MucEntityCal::GetH()     { return m_H;       }
double MucEntityCal::GetL()     { return m_L;       }
double MucEntityCal::GetWu()    { return m_Wu;      }
double MucEntityCal::GetWd()    { return m_Wd;      }
double MucEntityCal::GetArea()  { return m_Area;    }

// Set methods
void MucEntityCal::Init()
{
  MucEntityCal::SetTheta();
  MucEntityCal::SetRin();
  MucEntityCal::SetRout();
  MucEntityCal::SetRc();
  MucEntityCal::SetThin();
  MucEntityCal::SetW();
  MucEntityCal::SetWu();
  MucEntityCal::SetWd();
  MucEntityCal::SetH();
  MucEntityCal::SetL();
  MucEntityCal::SetArea();
}

void MucEntityCal::SetTheta(){ m_Theta  = 0.;   }
void MucEntityCal::SetRin()  { m_Rin    = 0.;   }
void MucEntityCal::SetRout() { m_Rout   = 0.;   }
void MucEntityCal::SetRc()   { m_Rc     = 0.;   }
void MucEntityCal::SetThin() { m_Thin   = 0.; 	}
void MucEntityCal::SetW()    { m_W      = 0.;   }
void MucEntityCal::SetWu()   { m_Wu     = 0.; 	}
void MucEntityCal::SetWd()   { m_Wd     = 0.; 	}
void MucEntityCal::SetH()    { m_H      = 0.;   }
void MucEntityCal::SetL()    { m_L      = 0.;   }
void MucEntityCal::SetArea() { m_Area   = 0.;   }

// END


