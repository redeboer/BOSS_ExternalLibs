/**
 * @class McRelTableDefs
 *
 * @brief This header file serves to define the various relational tables used with 
 *        the Monte Carlo information
 *
 * @author Weidong Li
 *
 * $Header: /bes/bes/BossCvs/Event/McTruth/McTruth/McRelTableDefs.h,v 1.4 2007/11/22 09:37:08 codeman Exp $
 */

#ifndef McRelTableDefs_h
#define McRelTableDefs_h
#include "GaudiKernel/SmartRefVector.h"
#include "RelTable/RelTable.h"

// Start the defintions
namespace Event {

// Monte Carlo objects with the "truth"
class McParticle;
class MdcMcHit;
class TofMcHit;
class EmcMcHit;
class MucMcHit;

// typedefs for relating McParticle to associated MdcMcHit
typedef Event::RelTable<Event::McParticle, Event::MdcMcHit>  McPartToMdcHitTab;
typedef Event::Relation<Event::McParticle, Event::MdcMcHit>  McPartToMdcHitRel;
typedef ObjectList<Event::McPartToMdcHitRel>          McPartToMdcHitList;
typedef std::vector<Event::McPartToMdcHitRel*>        McPartToMdcHitVec;

// typedefs for relating McParticle to associated TofMcHit
typedef Event::RelTable<Event::McParticle, Event::TofMcHit>  McPartToTofHitTab;
typedef Event::Relation<Event::McParticle, Event::TofMcHit>  McPartToTofHitRel;
typedef ObjectList<Event::McPartToTofHitRel>          McPartToTofHitList;
typedef std::vector<Event::McPartToTofHitRel*>        McPartToTofHitVec;

// typedefs for relating McParticle to associated EmcMcHit
typedef Event::RelTable<Event::McParticle, Event::EmcMcHit>  McPartToEmcHitTab;
typedef Event::Relation<Event::McParticle, Event::EmcMcHit>  McPartToEmcHitRel;
typedef ObjectList<Event::McPartToEmcHitRel>          McPartToEmcHitList;
typedef std::vector<Event::McPartToEmcHitRel*>        McPartToEmcHitVec;

// typedefs for relating McParticle to associated MucMcHit
typedef Event::RelTable<Event::McParticle, Event::MucMcHit>  McPartToMucHitTab;
typedef Event::Relation<Event::McParticle, Event::MucMcHit>  McPartToMucHitRel;
typedef ObjectList<Event::McPartToMucHitRel>          McPartToMucHitList;
typedef std::vector<Event::McPartToMucHitRel*>        McPartToMucHitVec;

}

#endif

