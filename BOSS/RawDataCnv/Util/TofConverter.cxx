#include "Identifier/TofID.h"
#include "RawDataCnv/Util/TofConverter.h"

TofConverter* TofConverter::s_instance = 0;

TofConverter* TofConverter::instance()
{
   if ( s_instance == 0 ) {
      s_instance = new TofConverter();
   }

   return s_instance;
}

void TofConverter::destroy()
{
   if ( s_instance != 0 ) {
      delete s_instance;
      s_instance = 0;
   }
}

StatusCode TofConverter::convert(const BufferHolder& src, TofDigiCol* des, LumiDigiCol* des2)
{
    BufferHolder tmp;
    return this->convert(src, tmp, des, des2);
}

StatusCode TofConverter::convert(const BufferHolder& tof,
                                 const BufferHolder& etf,
                                 TofDigiCol* des,
                                 LumiDigiCol* des2)
{
   uint32_t REId, TEId, TEData, TEOverflow, TETorQ;
   uint32_t n1buf = tof.nBuf();
   uint32_t n2buf = etf.nBuf();
   uint32_t nbuf = n1buf + n2buf;

   TofDigi* tofDigi;
   typedef std::multimap<uint32_t, TofDigi*>::iterator my_iter;
   std::multimap<uint32_t, TofDigi*> mapIdData;
   my_iter it;

   for (uint32_t i = 0; i < nbuf; ++i) {
      bool isTof = (i < n1buf);
      const BufferHolder& rBH = isTof ? tof : etf;
      uint32_t ii = isTof ? i : (i-n1buf);
      uint32_t* buf = rBH(ii);
      uint32_t  bufSize = rBH.bufSize(ii);
      for (uint32_t j = 0; j < bufSize; ++j) {
         if ( isTof ) {
             m_builder.unPack(buf[j], REId, TEData, TEOverflow, TETorQ);
             TEId = m_builder.getTEID( REId);
         }
         else { //ETF
             this->unPackETF(buf[j], TEId, TEData, TEOverflow, TETorQ);
         }

	 if ( TEId == 0xFFFFFFFF ) {
	    uint32_t data_unit = buf[j];
	    if ( (data_unit >> 25) == 0x7F ) {
	       tofDigi = new TofDigi(Identifier(0xFFFFFFFF), 0x7FFFFFFF, 0x7FFFFFFF);
	       tofDigi->setOverflow(data_unit);
	       des->push_back(tofDigi);
	    }
	    continue;
	 }

	 uint32_t count = mapIdData.count(TEId);

	 if (count == 0) {
	    if (TETorQ) {  //Q
	       tofDigi = new TofDigi(Identifier(TEId), 0x7FFFFFFF, TEData);
	       tofDigi->setOverflow(0x10 | (TEOverflow<<1));
	    }
	    else {  //T
	       tofDigi = new TofDigi(Identifier(TEId), TEData, 0x7FFFFFFF);
	       tofDigi->setOverflow(0x20 | TEOverflow);
	    }
	    mapIdData.insert(make_pair(TEId, tofDigi));
	 }
	 else {
	    pair<my_iter, my_iter> range = mapIdData.equal_range(TEId);
	    it = range.first;
	    tofDigi = it->second;
	    if (TETorQ) {  //Q
	       if (tofDigi->getChargeChannel() == 0x7FFFFFFF) {  //matched Q and T, first Q
		  tofDigi->setChargeChannel(TEData);
		  tofDigi->setOverflow((tofDigi->getOverflow() | (TEOverflow<<1)) & 0xF);
		  while ((++it) != range.second) {  //multiT
		     tofDigi = it->second;
		     tofDigi->setOverflow(tofDigi->getOverflow() & 0xF);
		  }
	       }
	       else {  //multiQ
		  uint32_t flag = (tofDigi->getOverflow() & 0x3C) | 8;
		  while (it != range.second) {
		     tofDigi = (it++)->second;
		     tofDigi->setOverflow((tofDigi->getOverflow()&0x3) | flag);
		  }
		  tofDigi = new TofDigi(Identifier(TEId), 0x7FFFFFFF, TEData);
		  tofDigi->setOverflow(flag | (TEOverflow<<1));
		  mapIdData.insert(make_pair(TEId, tofDigi));
	       }
	    }
	    else {  //T
	       if (tofDigi->getTimeChannel() == 0x7FFFFFFF) {  //matched T and Q, firstT
		  tofDigi->setTimeChannel(TEData);
		  tofDigi->setOverflow((tofDigi->getOverflow() | TEOverflow) & 0xF);
		  while ((++it) != range.second) {  //multiQ
		     tofDigi = it->second;
		     tofDigi->setOverflow(tofDigi->getOverflow() & 0xF);
		  }
	       }
	       else {  //multiT
		  uint32_t flag = (tofDigi->getOverflow() & 0x3C) | 4;
		  while (it != range.second) {
		     tofDigi = (it++)->second;
		     tofDigi->setOverflow((tofDigi->getOverflow()&0x3) | flag);
		  }
		  tofDigi = new TofDigi(Identifier(TEId), TEData, 0x7FFFFFFF);
		  tofDigi->setOverflow(flag | TEOverflow);
		  mapIdData.insert(make_pair(TEId, tofDigi));
	       }
	    }
	 }
      }
   }

   my_iter end = mapIdData.end();
   for (it = mapIdData.begin(); it != end; ++it) {
      TEId = it->first;
      tofDigi = it->second;
      if ( (TEId&0xFFFF7FFF) != 0x20000060 ) {
	 des->push_back(tofDigi);
      }
      //zoujh: for luminosity
      else {
	 if (des2 != 0) {
	    des2->push_back(new LumiDigi(tofDigi));
	 }
	 delete tofDigi;
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode TofConverter::convert(TofDigiCol* src, WriteRawEvent*& des)
{
   return m_builder.pack(src, des);
}

TofConverter::TofConverter()
{
}

TofConverter::~TofConverter()
{
}

void TofConverter::unPackETF(uint32_t reDigi, uint32_t& TEId, uint32_t& TEData,
        uint32_t& TEOverflow, uint32_t& TETorQ)
{
    if ( (reDigi>>25) == 0x7F ) { //Error data!
        TEId = 0xFFFFFFFF;
        return;
    }
    int ew = reDigi>>31;
    int id = ((reDigi>>25)&0x3F);
    int pad = ((reDigi>>21)&0xF);
    int sb = ((reDigi>>20)&1);

    TEId = TofID::getIntID(3/*barrel_ec, 3 means ETF*/, ew, id, pad, sb);
    TEData = reDigi&0x7FFFF;
    TEOverflow = 0;
    TETorQ = ((reDigi>>19)&1);
}
