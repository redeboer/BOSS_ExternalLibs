//$id$
//
//$log$

/*
 *    2003/12/13   Zhengyun You     Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#include "MucRecEvent/MucRecHitContainer.h"

// Constructor.
MucRecHitContainer::MucRecHitContainer()
{ }

// Destructor.
MucRecHitContainer::~MucRecHitContainer()
{
  //Clear();
}

// Initializes the MucRecHitContainer.
void 
MucRecHitContainer::Init()
{
  //  cout << "MucRecHitContainer::Init-I1  hello world" << endl;

  Clear();

  //cout << "*********** Hit Container Initialized! *********** " << endl;
}

void 
MucRecHitContainer::Clear()
{
  for (int part = 0; part < (int)MucID::getPartNum(); part++) {
    for (int seg = 0; seg < (int)MucID::getSegNum(part); seg++) {
      for (int gap = 0; gap < (int)MucID::getGapNum(part); gap++) {
	MucRecHitID gapID(part, seg, gap);
	m_GapHitCount[gapID] = 0;
      }
    }
  }

  //m_pMucRecHitCol->clear();
  m_pHit.clear();
}

int 
MucRecHitContainer::GetHitCount()
{
  int count = 0;
  for (int part = 0; part < (int)MucID::getPartNum(); part++) {
    for (int seg = 0; seg < (int)MucID::getSegNum(part); seg++) {
      for (int gap = 0; gap < (int)MucID::getGapNum(part); gap++) {
	MucRecHitID gapID(part, seg, gap);
	count += m_GapHitCount[gapID];
      }
    }
  }
  
  return count;
}

int 
MucRecHitContainer::GetGapHitCount(const MucRecHitID gapID)
{
  if (m_GapHitCount.count(gapID)) return m_GapHitCount[gapID];
  return 0;
}

int 
MucRecHitContainer::GetGapHitCount(const int part,
				   const int seg,
				   const int gap)
{
  MucRecHitID gapID(part, seg, gap);
  return m_GapHitCount[gapID];
}

MucRecHit* 
MucRecHitContainer::GetHit(const MucRecHitID id)
{
  if (m_pHit.count(id)) return m_pHit[id];
  else return 0;
}

MucRecHit* 
MucRecHitContainer::GetHit(const int part,
			   const int seg,
			   const int gap,
			   const int hit)
{
  MucRecHitID id(part, seg, gap, hit);
  return m_pHit[id];
}

MucRecHit* 
MucRecHitContainer::GetHitByIdentifier(const Identifier id)
{
  int part  = MucID::part(id);
  int seg   = MucID::seg(id);
  int gap   = MucID::gap(id);
  int strip = MucID::strip(id);
  
  return GetHitByIdentifier(part, seg, gap, strip);
}

MucRecHit* 
MucRecHitContainer::GetHitByIdentifier(const int part,
				       const int seg,
				       const int gap,
				       const int strip)
{
  Identifier id = MucID::channel_id(part, seg, gap, strip);
  for(int i = 0; i < GetGapHitCount(part, seg, gap); i++) {
    MucRecHit *hit = GetHit(part, seg, gap, i);
    if(id == hit->GetID()) {
      return hit;
    }
  }
  
  return 0;
}

// Place a new hit object in the container.
void
MucRecHitContainer::AddHit(const Identifier id)
{
  int part  = MucID::part(id);
  int seg   = MucID::seg(id);
  int gap   = MucID::gap(id);
  int strip = MucID::strip(id);

  AddHit(part, seg, gap, strip);
}

// Place a new hit object in the container.
void
MucRecHitContainer::AddHit(const int part,
			   const int seg,
			   const int gap,
			   const int strip)
{
  if ( (part  >= 0) && (part  < (int)MucID::getPartNum()) &&
       (seg   >= 0) && (seg   < (int)MucID::getSegNum(part)) &&
       (gap   >= 0) && (gap   < (int)MucID::getGapNum(part)) &&
       (strip >= 0) && (strip < (int)MucID::getStripNum(part, seg, gap)) )
    {
      MucRecHitID gapID(part, seg, gap);
      //       cout << "MucRecHitContainer::AddHit-I1  " << idGap << endl;
      
      int hitCount = m_GapHitCount[gapID];
      MucRecHitID id(part, seg, gap, hitCount);
      //       cout << "MucRecHitContainer::AddHit-I2  hit id = " << id << endl;
      
      MucRecHit *pHit = new MucRecHit(part, seg, gap, strip);
      m_pMucRecHitCol->push_back(pHit);

      m_pHit[id] = pHit;
      if(!m_pHit[id]) {
	cout << "MucRecHitContainer::AddHit-F1  failed to create new MucRecHit" << endl;
      }
      else {
	m_GapHitCount[gapID]++;
      }
    }
}

void
MucRecHitContainer::SetMucRecHitCol(MucRecHitCol* p)
{
  m_pMucRecHitCol = p;

  MucRecHitCol::iterator imuchit;
  for(imuchit = p->begin();
      imuchit != p->end();
      imuchit++){
	int part = (*imuchit)->Part();
	int seg  = (*imuchit)->Seg();
	int gap  = (*imuchit)->Gap();
	int strip= (*imuchit)->Strip();
	
	MucRecHitID gapID(part, seg, gap);
	int hitCount = m_GapHitCount[gapID];
	MucRecHitID id(part, seg, gap, hitCount);

	m_pHit[id] = *imuchit;
	if(!m_pHit[id]) {
		cout << "MucRecHitContainer::AddHit-F1  failed to create new MucRecHit" << endl;
	}
	else {
		m_GapHitCount[gapID]++;
	}


  }

}

