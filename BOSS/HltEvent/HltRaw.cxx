#include "HltEvent/HltRaw.h"

HltRaw::HltRaw(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel) :
  RawData(id, timeChannel, chargeChannel){
}

HltRaw::HltRaw(const unsigned int id) :
  RawData(id){
}
