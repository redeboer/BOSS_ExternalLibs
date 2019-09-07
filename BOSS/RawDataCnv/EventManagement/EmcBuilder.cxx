#include <iostream>
#include <fstream>
#include "Identifier/EmcID.h"
#include "RawDataCnv/EventManagement/EmcBuilder.h"
using namespace std;

EmcBuilder::EmcBuilder()
  :Builder()
{
  // set vector size (2^13) and initialize to -1
  m_re2te = new uint32_t[8192];  // we have 2^14 different REID
  memset((void*)m_re2te, 0xFF, 8192*sizeof(uint32_t));

  initialize (Builder::m_confFile);
}

EmcBuilder::~EmcBuilder()
{
   delete m_re2te;
}

void EmcBuilder::unPack(uint32_t reDigi, uint32_t &REId, uint32_t &TETDC, uint32_t &TEADC,
                        uint32_t &measure)
{
   REId = (reDigi&m_idMask) >> m_idIndex; 
   TETDC = (reDigi&m_tdcMask) >> m_tdcIndex; 
   TEADC = (reDigi&m_adcMask) >> m_adcIndex;
   measure = (reDigi&m_measureMask) >> m_measureIndex; 

   return;
}

StatusCode EmcBuilder::pack(EmcDigiCol* digiCol, WriteRawEvent*& re)
{
  if (digiCol == 0 ) {
    cerr << "EmcBuilder::pack can't get digiCol" << endl;
    return StatusCode::FAILURE;
  }

  uint32_t size = 0;
  uint32_t teid = 0, tetdc = 0, teadc = 0, reid = 0, redigi = 0, measure = 0;

  EmcDigiCol::const_iterator pEmcDigi = digiCol->begin();
  for (pEmcDigi; pEmcDigi!= digiCol->end(); pEmcDigi++) {
    teid  = (*pEmcDigi)->getIntId();
    reid  = getREID(teid);
    tetdc = (*pEmcDigi)->getTimeChannel();
    teadc = (*pEmcDigi)->getChargeChannel();
    measure = (*pEmcDigi)->getMeasure();

    // if negative energy, don't record 
    if(teadc&0x80000000) continue;
    // set ...
    redigi = ((reid<<m_idIndex)&m_idMask)|
             ((tetdc<<m_tdcIndex)&m_tdcMask)|
             ((teadc<<m_adcIndex)&m_adcMask)|
             ((measure<<m_measureIndex)&m_measureMask);
    m_buf[size++] = redigi;
  }

  append2event(re, 0xa30000, size);

  return StatusCode::SUCCESS;
}

// initialize re2te tables

StatusCode  EmcBuilder::initialize(string& initFile)
{
  ifstream f;

  uint32_t nREThetaPos, nREPhiPos, nREEaWePos;
  uint32_t nREThetaMask, nREPhiMask, nREEaWeMask;
  
  //-----------------------------------------------------------
  // read initFile
  f.open( initFile.c_str());
  
  if( f.bad() )
  {
     cerr << "Error: could not open file " << initFile << endl;
     return false;
  }

  if (!Builder::find( f, "##EmcDigiConf", initFile)) {
    cerr << "Error: could not find '##EmcDigiConf' in file " << initFile << endl;
    return StatusCode::FAILURE;
  }

  if( !Builder::expect( f, "#Index,mask", initFile) ||  
      !Builder::expectInt( f, "id", initFile, m_idIndex, m_idMask) ||   
      !Builder::expectInt( f, "tdc", initFile, m_tdcIndex, m_tdcMask) ||   
      !Builder::expectInt( f, "measure", initFile, m_measureIndex, m_measureMask) ||   
      !Builder::expectInt( f, "adc", initFile, m_adcIndex, m_adcMask) ||   
      !Builder::expectInt( f, "Phi", initFile, nREPhiPos, nREPhiMask) ||  
      !Builder::expectInt( f, "Theta", initFile, nREThetaPos, nREThetaMask) ||
      !Builder::expectInt( f, "EaWe", initFile, nREEaWePos, nREEaWeMask))
    return false; 
 
  
  f.close();

  //-----------------------------------------------------------
  //Build re2te table
  for(uint32_t  barrel_ec_eawe = EmcID::getBARREL_EC_MIN(); 
      barrel_ec_eawe <= EmcID::getBARREL_EC_MAX(); barrel_ec_eawe++)
  {
      uint32_t TEthetaMax, TEphiMax;
      uint32_t TEthetaMin, TEphiMin;
      uint32_t eawe, theta, phi;
      if(barrel_ec_eawe == EmcID::getBARREL())
      {
           //it is BARREL
           TEthetaMax = EmcID::getTHETA_BARREL_MAX();//43
           TEthetaMin = EmcID::getTHETA_BARREL_MIN();//0
      } else
      {
           //it is ease and west ENDCAP
           TEthetaMax = EmcID::getTHETA_ENDCAP_MAX();//5
           TEthetaMin = EmcID::getTHETA_ENDCAP_MIN();//0
           if (barrel_ec_eawe == EmcID::getBARREL_EC_MIN()) 
               eawe = 0;//east
           else
               eawe = 1;//west
      }
      for(uint32_t  TEtheta = TEthetaMin; TEtheta <= TEthetaMax; TEtheta++ )
      {
           if(barrel_ec_eawe == EmcID::getBARREL())
           {
               //it is BARREL
	       if (TEtheta <= EmcID::getTHETA_BARREL_MAX()/2) 
               {
                  eawe = 0; //east
                  theta = EmcID::getTHETA_BARREL_MAX()/2 +1 - TEtheta; //0-21=>22-1
               }
               else {
                  eawe = 1; //west
                  theta = TEtheta - EmcID::getTHETA_BARREL_MAX()/2; //22-43=>1-22
	       }
               TEphiMax = EmcID::getPHI_BARREL_MAX();//119
               TEphiMin = EmcID::getPHI_BARREL_MIN();//0

           } else
           {
               //it is ease and west ENDCAP
               theta = TEtheta + EmcID::getTHETA_BARREL_MAX()/2 + 2;
               TEphiMax = EmcID::getPHI_ENDCAP_MAX(TEtheta);//64,80,96
               TEphiMin = EmcID::getPHI_ENDCAP_MIN();

           }

           for(uint32_t TEphi = TEphiMin; TEphi <= TEphiMax; TEphi++ )
           {
             phi = TEphi + 1;
	     //cout << "eawe" << hex << eawe <<endl;
	     //cout << "__FILE__ theta " << theta << "phi " << phi << endl;
	     uint32_t reid = ((eawe<<nREEaWePos)&nREEaWeMask)|
	       ((theta<<nREThetaPos)&nREThetaMask)|
	       ((phi<<nREPhiPos)&nREPhiMask);
	     uint32_t teid = EmcID::getIntID(barrel_ec_eawe, TEtheta, TEphi);
	     //cout << "barrel_ec_eawe" << barrel_ec_eawe <<endl;
	     //cout << "TEtheta" << hex << TEtheta << endl;
	     //cout << "TEphi" << hex << TEphi <<endl;

	     if( reid >= 8192 )
	     {
	       cerr << "Error: REID overflow !" << reid << endl;
	       exit(8);
	     }
	     //cout << "reid" << hex << reid << endl;
	     //cout << "teid" << hex << teid << endl; 
	     m_re2te[reid] = teid;
	     m_te2reMap.insert(TE2REMAP::value_type(teid, reid)) ; 
	   }
       }
    }
    // return successful initialization
    return StatusCode::SUCCESS;
}


uint32_t EmcBuilder::getREID(uint32_t teid)
{
  TE2REMAP::iterator itr = m_te2reMap.find(teid);
 
  if (itr == m_te2reMap.end()) {
      cout<<"wrong teid in emc "<<teid<<endl;
      exit(8);
  }

  return itr->second;
}

