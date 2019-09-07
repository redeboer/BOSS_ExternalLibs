#include <iostream>
#include <fstream>
#include "Identifier/HltID.h"
#include "RawDataCnv/EventManagement/HltBuilder.h"
using namespace std;

HltBuilder::HltBuilder()
  :Builder()
{
  // set vector size (2^14) and initialize to -1
  m_re2te = new uint32_t[256];  // we have 2^8 different REID
  memset((void*)m_re2te, 0xFF, 256*sizeof(uint32_t));

  initialize (Builder::m_confFile);
}

HltBuilder::~HltBuilder()
{
   delete m_re2te;
}

void HltBuilder::unPack(uint32_t reDigi, uint32_t n, 
			uint32_t &REId, uint32_t &TEData1, uint32_t &TEData2)
{
  if ( n == 0 ) {
    REId = 0;
    TEData1 = reDigi;
    TEData2 = 0;
  }
  else if ( n == 1 ) {
    REId = 1;
    TEData1 = (reDigi&m_algMask) >> m_algIndex;
    TEData2 = (reDigi&m_criMask) >> m_criIndex;
    //cout<<hex<<reDigi<<"=>"<<TEData1<<","<<TEData2<<endl
    //	<<m_algMask<<"  "<<m_algIndex<<"  "<<m_criMask
    //	<<"  "<<m_criIndex<<dec<<endl;
  }
  else {
    REId = n+1;
    TEData1 =reDigi;
    TEData2 =0;
  }

   return;
}

StatusCode HltBuilder::pack(HltRawCol* digiCol, WriteRawEvent*& re)
{
  if (digiCol == 0 ) {
    cerr << "HltBuilder::pack can't get digiCol" << endl;
    return StatusCode::FAILURE;
  }

  uint32_t size = 0;
  uint32_t hltdigi = 0;
  Identifier id;
  uint32_t event_filter[4]={0,0,0,0};

  HltRawCol::const_iterator pHltDigi = digiCol->begin();
  for ( ; pHltDigi!= digiCol->end(); pHltDigi++) {
    id = (*pHltDigi)->identify();
    if(HltID::is_algorithm(id)){
      uint32_t alg= (*pHltDigi)->getIntChannel();
      uint32_t cri=0;
      //cout << " debug by fucd: alg=" << alg << endl;
      pHltDigi++;
      if(!HltID::is_criteria((*pHltDigi)->identify())) {
	pHltDigi--;
      }
      else {
	cri= (*pHltDigi)->getIntChannel();
      }
      hltdigi = ((alg<<m_algIndex)&m_algMask) | ((cri<<m_algIndex)&m_criMask);
      event_filter[1]                                    =hltdigi;
    }
    else if(HltID::is_criteria(id)){
      cerr<< "HltBuilder::pack():  HLT raw data error!" << endl;
      return StatusCode::FAILURE;
    }
    else{
      hltdigi = (*pHltDigi)->getIntChannel();
      if(HltID::is_eventtype(id))         event_filter[0]=hltdigi;
      else if(HltID::is_version(id))      event_filter[2]=hltdigi;
      else if(HltID::is_energy(id))  event_filter[3]=hltdigi;
      else                                m_buf[size++] = hltdigi;
    }
    //cout << " debug by fucd: digi="<< hex << hltdigi << dec <<endl;
  }

  re->event_filter_info(&event_filter[0]);
  append2event(re, 0x7c0000, size);

  return StatusCode::SUCCESS;
}

// initialize re2te tables

StatusCode  HltBuilder::initialize(string& initFile)
{
  ifstream f;
 
  //-----------------------------------------------------------
  // read initFile
  f.open( initFile.c_str());
  
  if( f.bad() )
  {
     cerr << "Error: could not open file " << initFile << endl;
     return false;
  }

  if (!Builder::find( f, "##HltRawConf", initFile)) {
    cerr << "Error: could not find '##HltRawConf' in file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if( !Builder::expect( f, "#Index,mask", initFile) ||  
      !Builder::expectInt( f, "id", initFile, m_idIndex, m_idMask) ||   
      !Builder::expectInt( f, "alg", initFile, m_algIndex, m_algMask) ||
      !Builder::expectInt( f, "cri", initFile, m_criIndex, m_criMask) ||
      !Builder::expectInt( f, "var", initFile, m_varIndex, m_varMask))   
    return false; 
  f.close();
  
  cout<<"HltBuilder::   #Index      mask"<<endl
      <<"            "<<m_idIndex<<"   "<<m_idMask<<endl
      <<"            "<<m_algIndex<<"   "<<m_algMask<<endl
      <<"            "<<m_criIndex<<"   "<<m_criMask<<endl
      <<"            "<<m_varIndex<<"   "<<m_varMask<<endl;

  //-----------------------------------------------------------
  //Build re2te table
  for (uint32_t i = 0; i < 256; i++) {
     m_re2te[i] = (0x50000000 | i);  //m_re2te[i]=0x50000000+0x00000000+i;
     //if(i>4) m_re2te[i]+=0x00000000;
     m_te2reMap.insert(TE2REMAP::value_type(m_re2te[i], i)) ; 
  }
  // return successful initialization
  return StatusCode::SUCCESS;
}


uint32_t HltBuilder::getREID(uint32_t teid)
{
 
  TE2REMAP::iterator itr = m_te2reMap.find(teid);
 
  uint32_t reid = 0;
  if(itr!= m_te2reMap.end())
     reid = (*itr).second;
  else
  {
      cout<<"wrong teid in emc "<<teid<<endl;
      exit(8);
  }
 
  return  reid;
 
}

