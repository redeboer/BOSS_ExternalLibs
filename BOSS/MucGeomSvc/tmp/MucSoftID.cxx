//$id$
//
//$log$

/*
 *    2003/08/30   Zhengyun You     Peking University
 * 
 *    2004/09/09   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

using namespace std;

#include <iostream>

#include "MucGeomSvc/MucSoftID.h"

//const int 
//MucSoftID::m_kPart = 3;

//const int 
//MucSoftID::m_kSegInPartMax = 8;

const int 
MucSoftID::m_kSegInPart[MucSoftID::m_kPart] = {4, 8, 4};

//const int 
//MucSoftID::m_kGapInSegMax = 9;

const int 
MucSoftID::m_kGapInPart[MucSoftID::m_kPart] = {8, 9, 8};

//const int 
//MucSoftID::m_kStripInGapMax = 96;

const int 
MucSoftID::m_kStripInGap[MucSoftID::m_kPart][MucSoftID::m_kGapInSegMax] = { {64, 64, 64, 64, 64, 64, 64, 64},
									    {48, 96, 48, 96, 48, 96, 48, 96, 48},
									    {64, 64, 64, 64, 64, 64, 64, 64} };

const int 
MucSoftID::m_kSegTotal   = MucSoftID::m_kSegInPartMax  * MucSoftID::m_kPart;

const int 
MucSoftID::m_kGapTotal   = MucSoftID::m_kGapInSegMax   * MucSoftID::m_kSegTotal;

const int
MucSoftID::m_kStripTotal = MucSoftID::m_kStripInGapMax * MucSoftID::m_kGapTotal;

// Assume a maximum occupancy of 100% for now.
const int
MucSoftID::m_kHitInGapMax = MucSoftID::m_kStripInGapMax;

const int
MucSoftID::m_kHitTotalMax = MucSoftID::m_kStripTotal;


MucSoftID::MucSoftID(const int part,
		     const int seg,
		     const int gap,
		     const int strip)
  : m_Part(part),
    m_Seg(seg),
    m_Gap(gap),
    m_Strip(strip)
{
  // Constructor.
}

MucSoftID& 
MucSoftID::operator=(const MucSoftID& orig)
{
  // Assignment operator.
  if (this != &orig) {             // Watch out for self-assignment!
    m_Part  = orig.m_Part;
    m_Seg   = orig.m_Seg;
    m_Gap   = orig.m_Gap;
    m_Strip = orig.m_Strip;
  }
  return *this;
}

MucSoftID::MucSoftID(const MucSoftID& orig)
  : m_Part(orig.m_Part),
    m_Seg(orig.m_Seg),
    m_Gap(orig.m_Gap),
    m_Strip(orig.m_Strip)
{
  // Copy constructor.
}

bool
MucSoftID::operator == (const MucSoftID& id) const
{
  if( m_Part     == id.GetPart() 
      && m_Seg   == id.GetSeg() 
      && m_Gap   == id.GetGap() 
      && m_Strip == id.GetStrip() ) {
    return true;
  }
  else {
    return false;
  }
}

void
MucSoftID::SetID(const int part,
		 const int seg,
		 const int gap,
		 const int strip)
{
  // Set the sub-identifier, m_GapID and m_StripID.
  m_Part  = part;
  m_Seg   = seg;
  m_Gap   = gap;
  m_Strip = strip;
}

ostream& operator << (ostream& s, const MucSoftID& id)
{
  // Print MucSoftID information to a stream.
  s << " part "   << id.GetPart()
    << " seg "    << id.GetSeg()
    << " gap "    << id.GetGap() 
    << " strip "  << id.GetStrip();

  return s;
}

size_t GapHash(const MucSoftID& id)
{
  // Return the position of a MucSoftID object in the GapGeo hash table.
  return  ( ( id.GetPart()
	      *MucSoftID::m_kSegInPartMax + id.GetSeg() )
	    *MucSoftID::m_kGapInSegMax + id.GetGap() );
}

size_t StripHash(const MucSoftID& id)
{
  // Return the position of a MucSoftID object in the StripGeo hash table.
  return ( ( ( id.GetPart()
	       *MucSoftID::m_kSegInPartMax + id.GetSeg() )
	     *MucSoftID::m_kGapInSegMax + id.GetGap() )
	   *MucSoftID::m_kStripInGapMax + id.GetStrip() ); 
}
