//$id$
//
//$log$

/*
 *    2003/12/13   Zhengyun You     Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#include "MucRecEvent/MucRecHitID.h"

// IMPLEMENTATION OF MucRecHitID METHODS.  ================================

// Constructor.
MucRecHitID::MucRecHitID(const int part,
			 const int seg,
			 const int gap,
			 const int hit)
  : m_Part(part), m_Seg(seg), m_Gap(gap), m_Hit(hit)
{ }

// Copy constructor.
MucRecHitID::MucRecHitID(const MucRecHitID& source)
  : m_Part(source.m_Part),
    m_Seg(source.m_Seg),
    m_Gap(source.m_Gap),
    m_Hit(source.m_Hit)
{ }

// Destructor.
MucRecHitID::~MucRecHitID()
{ }

// Assignment operator.
MucRecHitID&
MucRecHitID::operator=(const MucRecHitID& orig)
{
  // Assignment operator.
  if (this != &orig) {             // Watch out for self-assignment!
    m_Part = orig.m_Part;
    m_Seg  = orig.m_Seg;
    m_Gap  = orig.m_Gap;
    m_Hit  = orig.m_Hit;
  }
  return *this;
}

bool
MucRecHitID::operator == (const MucRecHitID& other) const
{
  if ( m_Part == other.m_Part && 
       m_Seg  == other.m_Seg  &&
       m_Gap  == other.m_Gap  &&
       m_Hit  == other.m_Hit )
    return true;
  else 
    return false;
}

bool
MucRecHitID::operator != (const MucRecHitID& other) const
{
  if ( m_Part == other.m_Part && 
       m_Seg  == other.m_Seg  &&
       m_Gap  == other.m_Gap  &&
       m_Hit  == other.m_Hit )
    return false;
  else 
    return true;
}

bool
MucRecHitID::operator < (const MucRecHitID& other) const
{
  if (m_Part < other.m_Part ||
      (m_Part == other.m_Part && m_Seg <  other.m_Seg) ||
      (m_Part == other.m_Part && m_Seg == other.m_Seg && m_Gap <  other.m_Gap) ||
      (m_Part == other.m_Part && m_Seg == other.m_Seg && m_Gap == other.m_Gap && m_Hit < other.m_Hit)) {
    return true;
  }
  else {
    return false;
  }
}

bool
MucRecHitID::operator > (const MucRecHitID& other) const
{
  if (m_Part > other.m_Part ||
      (m_Part == other.m_Part && m_Seg >  other.m_Seg) ||
      (m_Part == other.m_Part && m_Seg == other.m_Seg && m_Gap >  other.m_Gap) ||
      (m_Part == other.m_Part && m_Seg == other.m_Seg && m_Gap == other.m_Gap && m_Hit > other.m_Hit)) {
    return true;
  }
  else {
    return false;
  }
}

// Set the identifier variables.
void
MucRecHitID::SetID(const int part,
		   const int seg,
		   const int gap,
		   const int hit)
{
  m_Part = part;
  m_Seg  = seg;
  m_Gap  = gap;
  m_Hit  = hit;
}

// Output to a stream.
//ostream& operator << (ostream& s, const MucRecHitID& hitID)
  //{
  //return s << " Part " << hitID.Part() << " Seg " << hitID.Seg()
    //   << " Gap "  << hitID.Gap()  << " Hit " << hitID.Hit();
  //}
