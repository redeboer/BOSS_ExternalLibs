#ifndef RECTOFTRACK_H
#define RECTOFTRACK_H
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRef.h" 
#include "EventModel/EventModel.h"
#include "DstEvent/DstTofTrack.h"

using namespace std;

extern const  CLID &CLID_RecTofTrack;

class RecTofTrack : public DstTofTrack {
 public:

  virtual const CLID& clID()  const { return  RecTofTrack::classID();}
  static  const CLID& classID() { return CLID_RecTofTrack; }

  // Constructor
  RecTofTrack();
  
  // Destructor
  virtual ~RecTofTrack(){}

 public:
    RecTofTrack(const DstTofTrack& aTrack);
    RecTofTrack& operator=(const DstTofTrack& aTrack);

 private:

};

typedef ObjectVector<RecTofTrack> RecTofTrackCol;

#endif //RECTOFTRACK_H


