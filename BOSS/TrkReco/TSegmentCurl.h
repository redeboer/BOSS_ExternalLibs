#ifndef TSegmentCurl_FLAG_
#define TSegmentCurl_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#include "TrkReco/TMLink.h"
#include "TrkReco/TCurlFinderParameters.h"
#include "CLHEP/Alist/AList.h"

class TSegmentCurl
{
public:
  TSegmentCurl(const unsigned superLayerId = 9999,
	       const unsigned max = 9999);
  TSegmentCurl(TSegmentCurl &);
  TSegmentCurl(TSegmentCurl *);

  ~TSegmentCurl(void);

  const unsigned maxLocalLayerId(void) const;
  const unsigned superLayerId(void) const;
  const unsigned seqOfLayer(const unsigned);
  const unsigned sizeOfLayer(const unsigned);

  const unsigned maxSeq(void) const;
  const unsigned layerIdOfMaxSeq(void);
  const unsigned numOfSeqOneLayer(void);
  const unsigned numOfLargeSeqLayer(void);

  void setMaxSeq(const unsigned);
  void releaseMaxSeq(void);

  inline const unsigned size(void){ return m_list.length(); }

  const unsigned maxLocalLayerId(const unsigned);
  const unsigned superLayerId(const unsigned);

  void append(TMLink&);
  void append(TMLink*);
  void append(AList<TMLink>&);

  void remove(TMLink&);
  void remove(TMLink*);
  void remove(AList<TMLink>&);

  void removeAll(void);

  inline const AList<TMLink> & list(void){ return m_list; }
  inline const AList<TMLink> & list(unsigned i){ return m_layer[i]; }

  TSegmentCurl & operator=(const TSegmentCurl &);

  void dump(void);

  unsigned wires(const unsigned) const;

  void update(void);
private:
  // main private members
  // flag for the update
  bool m_flagOfUpdate;

  // elements
  AList<TMLink> m_list;
  // max local layer id = 0,1,2...
  unsigned m_MaxLocalLayerId;
  // super layer id = 0,1,2...
  unsigned m_superLayerId;
  // seq. num. of each layer
  unsigned m_seqOfLayer[4];
  // size of each layer
  unsigned m_sizeOfLayer[4];   //Liuqg, origin is 6

  // sub private members
  AList<TMLink> m_layer[4];   //for each superlayer. Liuqg, origin is 6
  void calcuSeq(unsigned);

  unsigned m_maxSeq;
  unsigned m_layerIdOfMaxSeq;
  unsigned m_numOfSeqOneLayer;
  unsigned m_numOfLargeSeqLayer;
};

#endif /* TSegmentCurl_FLAG_ */
