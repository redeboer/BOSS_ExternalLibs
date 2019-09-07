#ifndef MucDigi_H
#define MucDigi_H 
#include <vector>
#include "GaudiKernel/ObjectVector.h"
#include "RawEvent/RawData.h"
#include "EventModel/EventModel.h"

using namespace EventModel;
extern const CLID& CLID_MucDigi;

/*
 *
 */

class MucDigi : public RawData { 
public:
 // Constructor 
 MucDigi(const Identifier id);
 MucDigi(const unsigned int id);

 // Retrieve reference to class definition structure
 virtual const CLID& clID() const   { return MucDigi::classID(); }
 static  const CLID& classID()       { return CLID_MucDigi; }

private:

};

typedef ObjectVector<MucDigi> MucDigiCol;

#endif
