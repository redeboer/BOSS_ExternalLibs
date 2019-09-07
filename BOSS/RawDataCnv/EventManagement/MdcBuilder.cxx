#include <iostream>
#include <fstream>
#include "Identifier/MdcID.h"
#include "RawDataCnv/EventManagement/MdcBuilder.h"
using namespace std;

MdcBuilder::MdcBuilder()
   :Builder()
{
  // set vector size (2^14) and initialize to -1
  m_re2te = new uint32_t[16384];  // we have 2^14 different REID
  memset((void*)m_re2te, 0xFF, 16384*sizeof(uint32_t));

  initialize (Builder::m_confFile);
}

MdcBuilder::~MdcBuilder()
{
   delete m_re2te;
}

void MdcBuilder::unPack(uint32_t reDigi, uint32_t &REId, uint32_t &TEData,
                        uint32_t &overFlow, uint32_t &TorQ)
{
   REId = (reDigi&m_idMask)>>m_idIndex;
   //cout << "MdcBuilder::unPack  reDigi: 0x" << hex << reDigi << "  reid: 0x" << reid << dec << endl;
   TEData = reDigi&m_dataMask;
   overFlow = (reDigi&m_overflowMask)>>m_overflowIndex;
   TorQ = (reDigi&m_TQMask)>>m_TQIndex;
   //cout << "reid" << hex << reid << endl;
   //cout << "teid" << hex << TEId << endl;
   return;
}

StatusCode MdcBuilder::pack(MdcDigiCol* digiCol, WriteRawEvent*& re)
{
  if (digiCol == 0 ) {
    cerr << "MdcBuilder::pack can't get digiCol" << endl;
    return StatusCode::FAILURE;
  }

  uint32_t size = 0;
  uint32_t teid = 0, tetdc = 0, teadc = 0, reid = 0, redigi = 0, overflow = 0;

  MdcDigiCol::const_iterator pMdcDigi = digiCol->begin();
  for (pMdcDigi; pMdcDigi!= digiCol->end(); pMdcDigi++) {
    //zoujh+++++++++++, be consistent with DAQ raw data
    Identifier id = (*pMdcDigi)->identify();
    int layer = MdcID::layer(id), wire = MdcID::wire(id);
    if ( layer == 20 && wire < 8 ) {
	layer = 42;
    }
    else if ( layer == 42 && wire < 8 ) {
	layer = 20;
    }
    else if ( layer == 40 ) {
	if ( wire >= 200 && wire < 208 ) {
	    wire += 8;
	}
	else if ( wire >= 208 && wire < 216 ) {
	    wire -= 8;
	}
    }
    //teid  = (*pMdcDigi)->getIntId();
    teid = MdcID::getIntID(layer, wire);
    //zoujh-----------
    reid  = getREID(teid);
    tetdc = (*pMdcDigi)->getTimeChannel();
    teadc = (*pMdcDigi)->getChargeChannel();
    // overflow
    uint32_t overflow_tmp = (*pMdcDigi)->getOverflow();
    // set tdc
    if(tetdc!=0x7FFFFFFF){
      overflow = ((tetdc > m_dataMask) ? 1 : 0) | (overflow_tmp&1);
      redigi = (tetdc&m_dataMask)|
	       ((reid<<m_idIndex)&m_idMask)|
	       ((overflow<<m_overflowIndex)&m_overflowMask)|
	       ((0<<m_TQIndex)&m_TQMask);
      m_buf[size++] = redigi;
    }
    // set adc
    if(teadc!=0x7FFFFFFF){
      overflow = ((teadc > m_dataMask) ? 1 : 0) | ((overflow_tmp>>1)&1);
      redigi = (teadc&m_dataMask)|
               ((reid<<m_idIndex)&m_idMask)|
               ((overflow<<m_overflowIndex)&m_overflowMask)|
               ((1<<m_TQIndex)&m_TQMask);
      m_buf[size++] = redigi;
    }
  }

  append2event(re, 0xa10000, size);

  return StatusCode::SUCCESS;
}

// initialize re2te tables

StatusCode  MdcBuilder::initialize(string& initFile)
{
  ifstream f;

  uint32_t NRELAYERPOS, NREWIREPOS, NREEAWEPOS;
  uint32_t NRELAYERMASK, NREWIREMASK, NREEAWEMASK;

  
  //-----------------------------------------------------------
  // read initFile
  f.open( initFile.c_str());
  
  if( f.bad() ) {
     cerr << "Error: could not open file " << initFile << endl;
     return false;
  }

  if (!Builder::find( f, "##MdcDigiConf", initFile)) {
    cerr << "Error: could not find '##MdcDigiConf' in file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if( !Builder::expect( f, "#Index,mask", initFile) ||
      !Builder::expectInt( f, "id", initFile, m_idIndex, m_idMask) ||
      !Builder::expectInt( f, "data", initFile, m_dataIndex, m_dataMask) ||
      !Builder::expectInt( f, "Overflow", initFile, m_overflowIndex, m_overflowMask) ||
      !Builder::expectInt( f, "T/Q", initFile, m_TQIndex, m_TQMask) ||
      !Builder::expectInt( f, "Wire", initFile, NREWIREPOS, NREWIREMASK) ||
      !Builder::expectInt( f, "Layer", initFile, NRELAYERPOS, NRELAYERMASK) )
    return false; 
  
  f.close();
  //-----------------------------------------------------------
  //Build re2te table
  uint32_t  nRELayerPos, nREWirePos, nREEaWePos;  
  uint32_t  nRELayerMask, nREWireMask, nREEaWeMask;  


  uint32_t TELayerMax = MdcID::getAXIAL_LAYER_MAX()+MdcID::getSTEREO_LAYER_MAX()+1;
  uint32_t TEWireMax = MdcID::getAXIAL_WIRE_MAX();

  int i = 0; 

  uint32_t layer, wire;
        
  nRELayerPos =  NRELAYERPOS;
  nREWirePos =  NREWIREPOS;
  nRELayerMask = NRELAYERMASK;
  nREWireMask = NREWIREMASK; 

  for(uint32_t  TELayer = 0; TELayer <= TELayerMax; TELayer++ )
  {
    layer = TELayer + 1;
    // After Layer 32, there are changes in positions of Layers and wires
    if(layer >= 0x20) {
      layer = layer + i ;
      i++;   
      nRELayerMask = nRELayerMask&0xFFFFFBFF;
      nREWireMask = nREWireMask|0x00000400;
      TEWireMax = MdcID::getAXIAL_WIRE_MAX();
    }
    else {
      //When Layer is less than 32, the number of wire is less than 240 to prevent overflow;
      TEWireMax = 240;
    } 

    for(uint32_t TEWire = 0; TEWire <= TEWireMax; TEWire++ )
    {
      wire = TEWire + 1;
      uint32_t reid = (((layer<<nRELayerPos)&nRELayerMask)|
	  ((wire<<nREWirePos)&nREWireMask))>>2;
      //cout << "TELayer" << hex << TELayer;
      //cout << " TEWire" << hex << TEWire;
      //cout << " layer" << hex << layer;
      //cout << " wire" << hex << wire;

      uint32_t teid = MdcID::getIntID(TELayer, TEWire);

      if( reid >= 16384 )
      {
	cerr << "Error: REID overflow !" << reid << endl;
	exit(8);
      }
      //cout << " reid" << hex << reid;
      //cout << " teid" << hex << teid << endl;
      m_re2te[reid] = teid;
      m_te2reMap.insert(TE2REMAP::value_type(teid, reid)) ;
    }
  }

   // return successful initialization
   return StatusCode::SUCCESS;
}


uint32_t MdcBuilder::getREID(uint32_t teid)
{
  TE2REMAP::iterator itr = m_te2reMap.find(teid);
 
  if (itr == m_te2reMap.end()) {
      cout << "wrong teid in mdc " << teid << endl;
      exit(8);
  }

  return itr->second;
}

