#include "LumiDigi/LumiDigi.h"

//const CLID& CLID_LumiDigi     = 1350;
//const CLID& CLID_LumiDigiCol  = CLID_LumiDigi + CLID_ObjectVector;

LumiDigi::LumiDigi(const Identifier& id, const unsigned int timeChannel, const unsigned int chargeChannel) :
   RawData(id, timeChannel, chargeChannel),
   m_overflow(0)
{
}

LumiDigi::LumiDigi(const unsigned int id) :
   RawData(id),
   m_overflow(0)
{
}

LumiDigi::LumiDigi(TofDigi* tofDigi) :
   RawData(tofDigi->identify(), tofDigi->getTimeChannel(), tofDigi->getChargeChannel()),
   m_overflow(tofDigi->getOverflow())
{
}
