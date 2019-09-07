//$id$
//
//$log$
/*
 *    2003/08/30   Zhengyun You     Peking University
 * 
 *    2004/09/09   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_SOFT_ID_H
#define MUC_SOFT_ID_H

/**
 * Class MucSoftID contains the "software" representation of a MUC channel address.
 *
 * Indexing a Muc readout channel (in the "software" scheme) requires 4 numbers:
 \begin{enumerate}
 \item the part  number,              m_Part  (0-east cap, 1-barrel, 2-west cap)
 \item the seg   number in a part,    m_Seg   (barrel 0-3, cap 0-7)
 \item the gap   number in a segment, m_Gap   (barrel 0-8, cap 0-7)
 \item the strip number in a gap,     m_Strip (barrel gap 0,2,4,6,8 0-47
 \                                             barrel gap 1,3,5,7   0-95
 \                                             endcap               0-63)
 \end{enumerate}
 *
 * @author Zhengyun You \URL{mailto:youzy@pku.edu.cn}
 *
 */

class MucSoftID
{
 public:

  /// Number of parts in Bes Muc.
  static const int m_kPart = 3; 

  /// Number of segments in a part max.
  static const int m_kSegInPartMax = 8;
  /// Number of segments in a part.
  static const int m_kSegInPart[m_kPart];
  
  /// Number of gaps in a segment max.
  static const int m_kGapInSegMax = 9;
  /// Number of gaps in a segment of part.
  static const int m_kGapInPart[m_kPart];
  
  /// Number of strips in a gap max.
  static const int m_kStripInGapMax = 96;
  /// Number of strips in a gap.
  static const int m_kStripInGap[m_kPart][m_kGapInSegMax];

  /// Number of segments total in the Bes MUC.
  static const int m_kSegTotal;
  /// Number of gaps total in the Bes MUC.
  static const int m_kGapTotal; 
  /// Number of strips total in the Bes MUC.
  static const int m_kStripTotal; 

  /// Maximum number of hits in a gap.
  static const int m_kHitInGapMax;
  /// Maximum number of hits total in the Bes MUC.
  static const int m_kHitTotalMax; 

  /// Constructor.
  MucSoftID(const int part  = -1,
	    const int seg   = -1,
	    const int gap   = -1,
	    const int strip = -1);
  
  /// Assignment constructor.
  MucSoftID& operator=(const MucSoftID& orig);
  
  /// Copy constructor.
  MucSoftID(const MucSoftID& orig);

  /// Destructor.
  ~MucSoftID()
    { };

  /// Compare whehter two MucSoftID is equal.
  bool operator == (const MucSoftID& id) const;

  /// Get part number.  (0-east cap, 1-barrel, 2-west cap)
  int GetPart() const { return m_Part; }

  /// Get segment number. (barrel 0-3, cap 0-7)
  int GetSeg() const { return m_Seg; }

  /// Get gap number. (barrel 0-8, cap 0-7)
  int GetGap() const { return m_Gap; }
  
  /// Get strip number. (barrel 0-47 for gap0,2,4,6,8, 0-95 for gap1,3,5,7, endcap 0-63)
  int GetStrip() const { return m_Strip; }

  /// Set the sub-identifier.
  void SetID(const int part  = -1,
	     const int seg   = -1,
	     const int gap   = -1,
	     const int strip = -1);

 private:
  int m_Part;  
  int m_Seg;   
  int m_Gap;   
  int m_Strip;  
};

/// Return the position of a MucSoftID object in the GapGeo hash table.
size_t GapHash(const MucSoftID& id);

/// Return the position of a MucSoftID object in the StripGeo hash table.
size_t StripHash(const MucSoftID& id);

/// Print MucSoftID information to a stream.
ostream& operator << (ostream& s, const MucSoftID& id);

#endif  /* MUC_SOFT_ID_H */
