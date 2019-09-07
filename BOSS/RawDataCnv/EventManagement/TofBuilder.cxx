#include <iostream>
#include <fstream>
#include "Identifier/TofID.h"
#include "RawDataCnv/EventManagement/TofBuilder.h"
using namespace std;

TofBuilder::TofBuilder()
  :Builder()
{
  // set vector size (2^14) and initialize to -1
  m_re2te = new uint32_t[16384];  // we have 2^14 different REID
  memset((void*)m_re2te, 0xFF, 16384*sizeof(uint32_t));

  initialize (Builder::m_confFile);
}

TofBuilder::~TofBuilder()
{
   delete m_re2te;
}

void TofBuilder::unPack(uint32_t reDigi, uint32_t &REId, uint32_t &TEData,
                        uint32_t &overFlow, uint32_t &TorQ)
{
   REId = (reDigi&m_idMask)>>m_idIndex;
   //cout << "TofBuilder::unPack reDigi: 0x" << hex << reDigi
   //     << "  reid: 0x" << reid << dec << endl;

   TEData = (reDigi&m_dataMask); 
   overFlow = (reDigi&m_overflowMask)>>m_overflowIndex;
   TorQ = (reDigi&m_TQMask)>>m_TQIndex; 
   //if (TorQ) TEData &= 0x1FFF;   // ADC 13 bits
   //cout << "reid" << hex << reid << endl;
   //cout << "teid" << hex << TEId << endl;
   return;
}

StatusCode TofBuilder::pack(TofDigiCol* digiCol, WriteRawEvent*& re)
{
  if (digiCol == 0 ) {
    cerr << "TofBuilder::pack can't get digiCol" << endl;
    return StatusCode::FAILURE;
  }

  uint32_t size = 0;
  uint32_t teid = 0, tetdc = 0, teadc = 0, reid = 0, redigi = 0, overflow = 0;
  std::vector<TofDigi*> vEtfDigi;

  // for TOF Digis
  TofDigiCol::const_iterator pTofDigi = digiCol->begin();
  for (pTofDigi; pTofDigi!= digiCol->end(); pTofDigi++) {
    if ( TofID::is_mrpc((*pTofDigi)->identify()) )
    {
        vEtfDigi.push_back(*pTofDigi);
        continue;
    }
    teid  = (*pTofDigi)->getIntId();
    if ( teid == 0xFFFFFFFF ) {
       m_buf[size++] = (*pTofDigi)->getOverflow();
       continue;
    }
    // check weither it's a valid teid (for a mistake in simulation)
    if ((teid&0x4001) == 1)  continue;
    reid  = getREID(teid);
    tetdc = (*pTofDigi)->getTimeChannel();
    teadc = (*pTofDigi)->getChargeChannel();
    // overflow
    uint32_t overflow_tmp = (*pTofDigi)->getOverflow();
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

  append2event(re, 0xa20000, size);

  // for ETF Digis
  uint32_t shift = size;
  for (std::vector<TofDigi*>::const_iterator pEtfDigi = vEtfDigi.begin();
          pEtfDigi != vEtfDigi.end();
          ++pEtfDigi )
  {
      teid  = (*pEtfDigi)->getIntId();
      if ( teid == 0xFFFFFFFF ) {
          m_buf[size++] = (*pEtfDigi)->getOverflow();
          continue;
      }
      reid  = getREID(teid);
      tetdc = (*pEtfDigi)->getTimeChannel();
      teadc = (*pEtfDigi)->getChargeChannel();
      // set tdc ( the L/T for ETF )
      if ( tetdc != 0x7FFFFFFF )
      {
          redigi = (tetdc&0x7FFFF) |
              // ((0<<19)&0x80000) |
              ((reid<<20)&0xFFF00000);
          m_buf[size++] = redigi;
      }
      // set adc ( the L/T for ETF )
      if ( teadc != 0x7FFFFFFF )
      {
          redigi = (teadc&0x7FFFF) |
              ((1<<19)&0x80000) |
              ((reid<<20)&0xFFF00000);
          m_buf[size++] = redigi;
      }
  }

  if ( size != shift ) append2event(re, 0xa70000, size-shift, shift);

  return StatusCode::SUCCESS;
}

// initialize re2te tables

StatusCode  TofBuilder::initialize(string& initFile)
{
  ifstream f;

  uint32_t nRELayerPos, nRECellPos, nREEaWePos;
  uint32_t nRELayerMask, nRECellMask, nREEaWeMask;

  //-----------------------------------------------------------
  // read initFile
  f.open( initFile.c_str());

  if( f.bad() ) {
    cerr << "Error: could not open file " << initFile << endl;
    return false;
  }

  if (!Builder::find( f, "##TofDigiConf", initFile)) {
    cerr << "Error: could not find '##TofDigiConf' in file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if( !Builder::expect( f, "#Index,mask", initFile) ||  
      !Builder::expectInt( f, "id", initFile, m_idIndex, m_idMask) ||   
      !Builder::expectInt( f, "data", initFile, m_dataIndex, m_dataMask) ||   
      !Builder::expectInt( f, "Overflow", initFile, m_overflowIndex, m_overflowMask) ||   
      !Builder::expectInt( f, "T/Q", initFile, m_TQIndex, m_TQMask) ||   
      !Builder::expectInt( f, "Cell", initFile, nRECellPos, nRECellMask) ||  
      !Builder::expectInt( f, "Layer", initFile, nRELayerPos, nRELayerMask) ||
      !Builder::expectInt( f, "EaWe", initFile, nREEaWePos, nREEaWeMask) ) 
    return false; 


  f.close();

  //-----------------------------------------------------------
  //Build re2te table
  for(uint32_t  barrel_ec_eawe = TofID::getBARREL_EC_MIN();
      barrel_ec_eawe <= TofID::getBARREL_EC_MAX(); barrel_ec_eawe++)
  {
    uint32_t TELayerMax, TEphiMax, TEBaEaWe;
    uint32_t eawe = 0, layer = 0, cell = 0;

    if(barrel_ec_eawe == 1) {
      //it is BARREL
      TELayerMax = TofID::getLAYER_BARREL_MAX();
      TEphiMax = TofID::getPHI_BARREL_MAX();
    } else {
      //it is ease and west ENDCAP
      TELayerMax = TofID::getLAYER_ENDCAP_MAX();
      //zoujh: for luminosity !!! the right value should be 47
      TEphiMax = 48;  //TofID::getPHI_ENDCAP_MAX();
    }

    for(TEBaEaWe = 0; TEBaEaWe < 2; TEBaEaWe++) {
      if (barrel_ec_eawe != 1 && TEBaEaWe > 0) break;
      eawe = TEBaEaWe + (barrel_ec_eawe/2);
      for(uint32_t TELayer = 0; TELayer <= TELayerMax; TELayer++) {
	if (barrel_ec_eawe == 1) {
	  layer = TELayer + 1;
	} else {
	  //if (barrel_ec_eawe == TofID::getBARREL_EC_MIN()) layer = TELayer + 0;
	  //else layer = TELayer + 3;
	  layer = 3;
	}
	if (layer <= 3) {
	  for (uint32_t TEphi = 0; TEphi <= TEphiMax; TEphi++) {
	    cell = TEphi + 1;
	    uint32_t reid = (((eawe<<nREEaWePos)&nREEaWeMask)|
		((layer<<nRELayerPos)&nRELayerMask)|
		((cell<<nRECellPos)&nRECellMask));
	    uint32_t teid = TofID::getIntID(barrel_ec_eawe, TELayer, TEphi, TEBaEaWe);
	    //std::cout << "reid " << reid << " eawe " << eawe << " layer " << layer << " cell " << cell
	    //          << " teid " << hex << teid << dec << " barrel_ec_eawe " << barrel_ec_eawe  << " TELayer "
	    //          << TELayer << " TEphi " << TEphi << " TEBaEaWe " << TEBaEaWe << std::endl;
	    m_re2te[reid] = teid;
	    m_te2reMap.insert(TE2REMAP::value_type(teid, reid)) ;
	  }
	}
      }
    }
  }

  //-----------------------------------------------------------
  //Append the ETF te2re map
  for ( uint32_t module = 0; module < 36; ++module )
  {
      for ( uint32_t strip = 0; strip < 12; ++strip )
      {
          //uint32_t ew = 0 or 1, sb = 0 or 1;
          //uint32_t teid = TofID::getIntID(3/*barrel_ec, 3 means ETF*/, ew, module, strip, sb);
          //uint32_t reid = teid & 0xFFF;
          //m_te2reMap.insert(TE2REMAP::value_type(teid, reid));
          uint32_t teid = TofID::getIntID(3, 0, module, strip, 0);
          m_te2reMap.insert(TE2REMAP::value_type(teid, (teid&0xFFF)));
          teid = TofID::getIntID(3, 0, module, strip, 1);
          m_te2reMap.insert(TE2REMAP::value_type(teid, (teid&0xFFF)));
          teid = TofID::getIntID(3, 1, module, strip, 0);
          m_te2reMap.insert(TE2REMAP::value_type(teid, (teid&0xFFF)));
          teid = TofID::getIntID(3, 1, module, strip, 1);
          m_te2reMap.insert(TE2REMAP::value_type(teid, (teid&0xFFF)));
      }
  }

  // return successful initialization
  return StatusCode::SUCCESS;
}

 
uint32_t TofBuilder::getREID(uint32_t teid)
{
  TE2REMAP::iterator itr = m_te2reMap.find(teid);

  if (itr == m_te2reMap.end()) {
      cout << "wrong teid in tof" << teid << endl;
      exit(8);
  }
 
  return itr->second;
}

