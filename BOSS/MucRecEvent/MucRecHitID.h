//$id$
//
//$log$

/*
 *    2003/12/12   Zhengyun You     Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_REC_HIT_ID_H
#define MUC_REC_HIT_ID_H

#include <stdlib.h>
#include <math.h>
#include <vector>
#include <map>
#include <iostream>


/**
 * Class BesMUCRecRawHit contains the four numbers necessary to identify a hit uniquely:
 * part, seg, gap, strip, and hit within the gap.
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class MucRecHitID
{
 public:
  // PUBLIC CONSTANTS =================================================

  // CONSTRUCTOR/DESTRUCTOR METHODS ===================================

  /// Constructor.
  MucRecHitID(const int part = -1,
	      const int seg  = -1,
	      const int gap  = -1,
	      const int hit  = -1);

  /// Copy constructor. 
  MucRecHitID(const MucRecHitID& source);
  
  /// Assignment operator.
  MucRecHitID& operator=(const MucRecHitID& orig);

  /// Comparison operators.
  bool operator ==    (const MucRecHitID& other) const;
  bool operator !=    (const MucRecHitID& other) const;
  bool operator <     (const MucRecHitID& other) const;
  bool operator >     (const MucRecHitID& other) const;
  
  /// Destructor.
  ~MucRecHitID();
  
  /// Set the identifier.
  void SetID(const int part = -1,
	     const int seg  = -1,
	     const int gap  = -1,
	     const int hit  = -1);

  /// Get part number. (0-east cap, 1-barrel, 2-west cap)
  int Part() const { return m_Part; }

  /// Get seg number. (0-7)
  int Seg() const { return m_Seg; }
  
  /// Get gap number. (0-8)
  int Gap() const { return m_Gap; }
  
  /// Get hit number within this gap. (starts at 0)
  int Hit() const { return m_Hit; }
  
 private:
  int m_Part; 
  int m_Seg;  
  int m_Gap;  
  int m_Hit;  
};

/// Print a MucRecHitID hit identifier to a stream.
//ostream& operator << (ostream& s, const MucRecHitID& n);

#endif  /* MUC_REC_HIT_ID_H */
