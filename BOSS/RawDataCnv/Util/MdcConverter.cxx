#include "Identifier/MdcID.h"
#include "RawDataCnv/Util/MdcConverter.h"

MdcConverter* MdcConverter::s_instance = 0;

MdcConverter* MdcConverter::instance(int runMode)
{
   if ( s_instance == 0 ) {
      s_instance = new MdcConverter(runMode);
   }

   return s_instance;
}

void MdcConverter::destroy()
{
   if ( s_instance != 0 ) {
      delete s_instance;
      s_instance = 0;
   }
}

void MdcConverter::init(int runFrom, int runTo)
{
   m_runFrom = runFrom;
   m_runTo   = runTo;

   memset((void*)m_mdcTag, 0, 16384*4*4);
}

StatusCode MdcConverter::convert(const BufferHolder& src, MdcDigiCol* des)
{
   //int digiId = 0;
   uint32_t TEId, REId, TEData, TEOverflow, TETorQ;

   typedef pair<uint32_t, uint32_t> PairII;
   vector<uint32_t>  vHits;
   vector<PairII>    vmTDC;

   uint32_t* curTag;
   ////////////////////////////////////

   uint32_t nbuf = src.nBuf();

   for (uint32_t i = 0; i < nbuf; ++i)
   {
      uint32_t* buf = src(i);
      uint32_t* bufend= buf + src.bufSize(i);
      for ( ; buf < bufend; ++buf /*, ++digiId*/)
      {
	 uint32_t digi = *buf;
	 if (((digi>>18) & 0x3FFF) == 0) {
	    //digiId--;
	    continue;
	 }

	 m_builder.unPack(digi, REId, TEData, TEOverflow, TETorQ);

	 TEId = m_builder.getTEID( REId );
	 if ( TEId == 0xFFFFFFFF ) continue;

	 curTag = m_mdcTag[REId];

	 if ( curTag[0] == 0 ) {  //1st signal from detector-unit-REId
	    curTag[1] = 0x7FFFFFFF;
	    curTag[2] = 0x7FFFFFFF;
	    curTag[3] = 0;

	    int layer = MdcID::layer(Identifier(TEId));
	    int wire = MdcID::wire(Identifier(TEId));
	    //yzhang exchange pre-amplifier
	    if ( m_runId >= m_runFrom && m_runId <= m_runTo ) {
	       if (layer == 20 && wire >= 0 && wire <= 7){ 
		  TEId = 268456960 + wire;
		  curTag[3] |= 0x10;  //16
	       }
	       else if (layer == 42 && wire >= 0 && wire <= 7){ 
		  TEId = 268478464 + wire;
		  curTag[3] |= 0x10;  //16
	       }
	       else if (layer == 40 && wire >= 200 && wire <= 207){ 
		  TEId += 8;
		  curTag[3] |= 0x10;  //16
	       }
	       else if (layer == 40 && wire >= 208 && wire <= 215){ 
		  TEId -= 8;
		  curTag[3] |= 0x10;  //16
	       }
	    }

	    curTag[0] = (TEId << 2);

	    vHits.push_back( REId );
	 }

	 if ( TETorQ == 0 ) {  //T
	    if ( (curTag[0]&1) == 0 ) {  //1st-found-T
	       curTag[0] |= 1;
	       curTag[1] = TEData;
	       curTag[3] |= TEOverflow;  //0 or 1
	    }
	    else {  //multi-T
	       curTag[3] |= 0xC;  //12, set the multi-T and 1st-T bits
	       if ( TEData >= curTag[1] ) {
		  if ( TEOverflow ) TEData |= (1<<31);  //LastBit
		  vmTDC.push_back( make_pair(REId, TEData) );
	       }
	       else {
		  if ( curTag[3] & 1 ) curTag[1] |= (1<<31);
		  vmTDC.push_back( make_pair(REId, curTag[1]) );
		  curTag[1] = TEData;
		  curTag[3] &= (0xFFFFFFFF-1);
		  curTag[3] |= TEOverflow;
	       }
	    }
	 }
	 else {  //Q
	    curTag[0] |= 2;
	    curTag[2] = TEData;
	    if ( TEOverflow ) curTag[3] |= 2;
	 }
      }
   }

   MdcDigi* mdcDigi;
   if ( m_runMode == 1 ) {  //OnlineMode, discard the hits that only have T or Q
      for ( vector<PairII>::iterator it = vmTDC.begin(); it != vmTDC.end(); ++it ) {
	 curTag   = m_mdcTag[it->first];
	 if ( (curTag[0]&3) == 3 ) {
	    uint32_t  data  = it->second;
	    if(data>>31) continue;
	    mdcDigi = new MdcDigi(Identifier(curTag[0]>>2), (data&0x7FFFFFFF), curTag[2]);
	    mdcDigi->setOverflow( (curTag[3] & 0x16) | (data>>31) );
	    des->push_back(mdcDigi);
	 }
      }

      for ( vector<uint32_t>::iterator it = vHits.begin(); it != vHits.end(); ++it) {
	 curTag   = m_mdcTag[*it];
	 if ( (curTag[0]&3) == 3 ) {
	    if( ((curTag[3]&3)>0) || (((curTag[3]&12)!=12) && ((curTag[3]&12)!=0)) ) {
	       curTag[0] = 0;
	       continue; 
	    }
	    mdcDigi = new MdcDigi(Identifier(curTag[0]>>2), curTag[1], curTag[2]);
	    mdcDigi->setOverflow(curTag[3]);
	    des->push_back(mdcDigi);
	 }
	 curTag[0] = 0;
      }
   }
   else {  //Not OnlineMode, keep all the hits found
      for ( vector<PairII>::iterator it = vmTDC.begin(); it != vmTDC.end(); ++it ) {
	 curTag   = m_mdcTag[it->first];
	 uint32_t  data     = it->second;
	 mdcDigi = new MdcDigi(Identifier(curTag[0]>>2), (data&0x7FFFFFFF), curTag[2]);
	 mdcDigi->setOverflow( (curTag[3] & 0x16) | (data>>31) );
	 des->push_back(mdcDigi);
      }

      for ( vector<uint32_t>::iterator it = vHits.begin(); it != vHits.end(); ++it) {
	 curTag   = m_mdcTag[*it];
	 mdcDigi = new MdcDigi(Identifier(curTag[0]>>2), curTag[1], curTag[2]);
	 mdcDigi->setOverflow(curTag[3]);
	 des->push_back(mdcDigi);
	 curTag[0] = 0;
      }
   }

   ////////////////////////////////////
   return StatusCode::SUCCESS;
}

StatusCode MdcConverter::convert(MdcDigiCol* src, WriteRawEvent*& des)
{
   return m_builder.pack(src, des);
}

MdcConverter::MdcConverter(int runMode)
   : m_runMode( runMode )
{
}

MdcConverter::~MdcConverter()
{
}
