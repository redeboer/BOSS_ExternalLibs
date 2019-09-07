//$id$
//
//$log$

/*
 *    2003/12/12   Zhengyun You     Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_REC_HIT_CONTAINER_H
#define MUC_REC_HIT_CONTAINER_H

#include <map>
#include "Identifier/Identifier.h"

#include "MucRecEvent/MucRecHitID.h"
#include "MucRecEvent/MucRecHit.h"

/**
 * MucRecHitContainer has a MucRecHitCol which contains all MucRecHit objects for a single event.
 * Access is optimized for referencing by BesMUCRecHitID indices.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 * @see MucRecHit
 */

class MucRecHitContainer
{
 public:
  typedef std::map<MucRecHitID, int>        GapHitCountMap;
  typedef std::map<MucRecHitID, MucRecHit*> pHitMap;       
  
  // Constructor.
  MucRecHitContainer();
  
  // Copy constructor.
  MucRecHitContainer(const MucRecHitContainer& source);
  
  // Assignment operator.
  MucRecHitContainer& operator=(const MucRecHitContainer& orig);

  // Destructor.
  ~MucRecHitContainer();

 public:

  /// Points to the *single* instance of MucRecHitContainer.
  //static MucRecHitContainer *Instance();
  
  /// Initializes the MucRecHitContainer with MucRecHitCol.
  void Init();
  
  /// Destroys the *single* instance of MucRecHitContainer.
  void Destroy();

  void SetMucRecHitCol(MucRecHitCol* p);// { m_pMucRecHitCol = p; }
  
  /// Get MucRecHitCol pointer.
  MucRecHitCol* GetMucRecHitCol() { return m_pMucRecHitCol; }

  /// How many hits are there in this event?
  int GetHitCount();
  
  /// How many hits are there in this gap?
  int GetGapHitCount(const MucRecHitID gapID);

  /// How many hits are there in this gap?
  int GetGapHitCount(const int part,
		     const int seg,
		     const int gap);

  /// Get a MucRecHit object by hit identifier.
  MucRecHit *GetHit(const MucRecHitID hitID);

  /// Get a MucRecHit by part, seg, gap, and hit within the gap.
  MucRecHit *GetHit(const int part,
		    const int seg,
		    const int gap,
		    const int hit);
 
  /// Get a MucRecHit by Identifier.
  /// If doesnt exist, return null.
  MucRecHit *GetHitByIdentifier(const Identifier id);

  /// Get a MucRecHit by part, seg, gap, and strip.
  /// If doesnt exist, return null.
  MucRecHit *GetHitByIdentifier(const int part,
				const int seg,
				const int gap,
				const int strip);
  
  void AddHit(const Identifier id);

  /// Place a new hit object in the container.
  void AddHit(const int part,
	      const int seg,
	      const int gap,
	      const int strip);

  /// Remove all hit objects from the container, and destroy them.
  void Clear();

 private:
  MucRecHitCol *m_pMucRecHitCol;

  MucRecHitContainer::GapHitCountMap m_GapHitCount;
  MucRecHitContainer::pHitMap        m_pHit;
  
};

#endif  /* MUC_REC_HIT_CONTAINER_H */
