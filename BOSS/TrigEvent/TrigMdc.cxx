#include <iostream>
#include <fstream>
#include "TrigEvent/TrigMdc.h"
#include <stdlib.h>
using namespace std;

TrigMdc::TrigMdc(const char* name) :  m_name(name){
  // hit
  m_mdchit_one     = new int*[24];
  for (int i = 0; i < 24; i++) m_mdchit_one[i] = new int[256];
  m_mdchit_sone     = new int*[24];
  for (int i = 0; i < 24; i++) m_mdchit_sone[i] = new int[256];
  m_mdchit_pre = new int*[24];
  for(int i=0; i<24; i++) m_mdchit_pre[i] = new int[256];
  m_mdchit_aft = new int*[24];
  for(int i=0; i<24; i++) m_mdchit_aft[i] = new int[256];
  m_mdchit_win = new uint32_t*[24];
  for(int i=0; i<24; i++) m_mdchit_win[i] = new uint32_t[256];
  // tsf
  m_mdctsf_one = new int*[6];
  for (int i=0; i<6; i++) m_mdctsf_one[i] = new int[256];
  m_mdctsf_sone = new int*[6];
  for (int i=0; i<6; i++) m_mdctsf_sone[i] = new int[256];
  m_mdctsf_pre = new int*[6];
  for(int i=0; i<6; i++) m_mdctsf_pre[i] = new int[256];
  m_mdctsf_aft = new int*[6];
  for(int i=0; i<6; i++) m_mdctsf_aft[i] = new int[256];
  m_mdctsf_win = new uint32_t*[6];
  for(int i=0; i<6; i++) m_mdctsf_win[i] = new uint32_t[256];
  // ltrk
  m_mdcltrk_one  = new int[128];
  m_mdcltrk_sone = new int[128];
  m_mdcltrk_pre  = new int[128];
  m_mdcltrk_aft  = new int[128];
  m_mdcltrk_win  = new uint32_t[128];
  // strk
  m_mdcstrk_one  = new int[128];
  m_mdcstrk_sone = new int[128];
  m_mdcstrk_pre  = new int[128];
  m_mdcstrk_aft  = new int[128];
  m_mdcstrk_win  = new uint32_t[128];
  // ltrk in ltrk counter
  m_mdcltrkc_one = new int[132];
  m_mdcltrkc_sone= new int[132];
  // strk in strk counter
  m_mdcstrkc_one   = new int[132];
  m_mdcstrkc_sone  = new int[132];
  // trigger condition
  m_mdcTrigCondi = new int[10];

  filename = getenv("TRIGEVENTROOT");
  filename += "/dat/trig_mdc.txt";
  ifstream inputfile;
  inputfile.open(filename.c_str());
  string data[6];
  std::vector<uint32_t> vtmp;
  uint32_t mark = 226;
  for(uint32_t i = 0; i < 6; i++) inputfile >> data[i];
  for(uint32_t j = 0; j < 4680; j++) {
    for(uint32_t k = 0; k < 6; k++) inputfile >> data[k];
    if((uint32_t (atoi(data[0].c_str())) != mark) || j == 4679) {
      if(j == 4679) {
        for(int m = 1; m < 6; m++) vtmp.push_back(atoi(data[m].c_str()));
      }
      typedef pair<uint32_t, vector<uint32_t> > vpair;
      m_datamap.insert(vpair(mark,vtmp));
      mark = atoi(data[0].c_str());
      vtmp.clear();
    }
    if((uint32_t (atoi(data[0].c_str())) == mark) && j != 4679) {
      for(int l = 1; l < 6; l++) vtmp.push_back(atoi(data[l].c_str()));
    }
  }
}

TrigMdc::~TrigMdc() {
  delete  m_mdchit_one;
  delete  m_mdctsf_one;
  delete  m_mdchit_sone;
  delete  m_mdctsf_sone;

  delete m_mdchit_pre;
  //  delete m_mdchit_aft;

  delete m_mdcltrk_one;
  delete m_mdcstrk_one;
  delete m_mdcltrkc_one;
  delete m_mdcstrkc_one;

  delete m_mdcltrk_sone;
  delete m_mdcstrk_sone;
  delete m_mdcltrkc_sone;
  delete m_mdcstrkc_sone;

  delete m_mdcTrigCondi;
}

void TrigMdc::initialize(){
  //initialize
  for(int i = 0; i < 24; i++)
    for(int j = 0; j < 256; j++)
      {
	m_mdchit_one[i][j] = 0;
	m_mdchit_sone[i][j] = 0;
	m_mdchit_pre[i][j] = 0;
	m_mdchit_aft[i][j] = 0;
	m_mdchit_win[i][j] = 0;
	if(i < 6) {
	  m_mdctsf_one[i][j] = 0;
	  m_mdctsf_sone[i][j] = 0;
	  m_mdctsf_pre[i][j] = 0;
	  m_mdctsf_aft[i][j] = 0;
	  m_mdctsf_win[i][j] = 0;
	}
      }  
  for(int k = 0; k < 132; k++)
    {
      if(k < 10) m_mdcTrigCondi[k] = 0;
      m_mdcltrkc_one[k] = 0;
      m_mdcstrkc_one[k] = 0;
      m_mdcltrkc_sone[k] = 0;
      m_mdcstrkc_sone[k] = 0;
      if(k < 128) { 
	m_mdcltrk_one[k] = 0; 
	m_mdcltrk_sone[k] = 0;
	m_mdcltrk_pre[k] = 0;
	m_mdcltrk_aft[k] = 0;
	m_mdcltrk_win[k] = 0;

	m_mdcstrk_one[k] = 0; 
	m_mdcstrk_sone[k] = 0;
	m_mdcstrk_pre[k] = 0;
	m_mdcstrk_aft[k] = 0;
	m_mdcstrk_win[k] = 0;

      }
    }
}

void TrigMdc::setMdcTrigData( uint32_t boardId, uint32_t window, uint32_t datasize, const uint32_t* data){
  if(datasize%window != 0) cerr<<"ERROR: The data size is not completed in time windows"<<endl;
  uint32_t periods = datasize/window;
  std::map<uint32_t, std::vector<uint32_t> >::iterator iter=m_datamap.begin();
  for(; iter != m_datamap.end(); iter++){
    if(iter->first != boardId) continue;
    std::vector<uint32_t>::iterator iter1 = iter->second.begin();
    for(; iter1 != iter->second.end(); iter1+=5){
      uint32_t byteId = *(iter1+0) -1;
      uint32_t bitId = *(iter1+1);
      int layerId = *(iter1+2) -1;
      int wireId  = *(iter1+3) -1;
      uint32_t data_type = *(iter1+4);
      int seriesOne = -100;
      bool firstSOne = true; // this is a flag of first continue "1" signals
      bool flag_firstone(true), flag_lastone(true); // whether to book up or down
      for(uint32_t i=0; i<window; i++){
	if(layerId < 0 || wireId < 0) continue;
	if((iter->first) >= 226 && (iter->first) <= 233){ // TKF1-8 information
	  if(data_type == 1){ // hit
	    if((data[byteId + i*periods] >> bitId) & 1){
	      if(layerId >= 36 && layerId <= 39 ){
		if(flag_firstone && (i-seriesOne!=1) && (i!=0)){ m_mdchit_pre[layerId-16][wireId] = i+1; flag_firstone = false;}
		if( ((i-seriesOne)!=1) && (m_mdchit_aft[layerId-16][wireId]!=0) ) flag_lastone = false;
		if( (flag_firstone==false) && ((i-seriesOne)==1) && flag_lastone) m_mdchit_aft[layerId-16][wireId]=i+1;
		if(((i-seriesOne)==1) && firstSOne) m_mdchit_sone[layerId-16][wireId] += 1;
		if(m_mdchit_sone[layerId-16][wireId] != 0 && (i-seriesOne) != 1) firstSOne = false;
		seriesOne = i;
		m_mdchit_one[layerId-16][wireId] += 1; 
		m_mdchit_win[layerId-16][wireId] += 1<<i;
	      }
	      else {
		if(flag_firstone && (i-seriesOne!=1) && (i!=0)){ m_mdchit_pre[layerId][wireId] = i+1; flag_firstone = false;}
		if( ((i-seriesOne)!=1) && (m_mdchit_aft[layerId][wireId]!=0) ) flag_lastone = false;
		if( (flag_firstone==false) && ((i-seriesOne)==1) && flag_lastone) m_mdchit_aft[layerId][wireId]=i+1;
		if(((i-seriesOne) == 1) && firstSOne) m_mdchit_sone[layerId][wireId] += 1;
		if(m_mdchit_sone[layerId][wireId] != 0 && (i-seriesOne) != 1) firstSOne = false;
		seriesOne = i;
		m_mdchit_one[layerId][wireId] += 1;
		m_mdchit_win[layerId][wireId] += 1<<i;
	      }
	    }
	  }
	  if(data_type == 0) { // TSF
	    if((data[byteId + i*periods] >> bitId) & 1) {
	      if(layerId == 10) {
		if(flag_firstone && (i-seriesOne!=1) && (i!=0)){ m_mdctsf_pre[2][wireId] = i+1; flag_firstone = false;}
		if( ((i-seriesOne)!=1) && (m_mdctsf_aft[2][wireId]!=0) ) flag_lastone = false;
		if( (flag_firstone==false) && ((i-seriesOne)==1) && flag_lastone) m_mdctsf_aft[2][wireId]=i+1;
		
		if((i-seriesOne) == 1 && firstSOne) m_mdctsf_sone[2][wireId] += 1;
		if(m_mdctsf_sone[2][wireId] != 0 && (i-seriesOne) != 1) firstSOne = false;
		seriesOne = i;
		m_mdctsf_one[2][wireId] += 1;
		m_mdctsf_win[2][wireId] += 1<<i;
	      }
	      if(layerId == 14) {
		if(flag_firstone && (i-seriesOne!=1) && (i!=0)){ m_mdctsf_pre[3][wireId] = i+1; flag_firstone = false;}
		if( ((i-seriesOne)!=1) && (m_mdctsf_aft[3][wireId]!=0) ) flag_lastone = false;
		if( (flag_firstone==false) && ((i-seriesOne)==1) && flag_lastone) m_mdctsf_aft[3][wireId]=i+1;
		
		if((i-seriesOne) == 1 && firstSOne) m_mdctsf_sone[3][wireId] += 1;
		if(m_mdctsf_sone[3][wireId] != 0 && (i-seriesOne) != 1) firstSOne = false;
		seriesOne = i;
		m_mdctsf_one[3][wireId] += 1;
		m_mdctsf_win[3][wireId] += 1<<i;
	      }
	      if(layerId == 17) {
		if(flag_firstone && (i-seriesOne!=1) && (i!=0)){ m_mdctsf_pre[4][wireId] = i+1; flag_firstone = false;}
		if( ((i-seriesOne)!=1) && (m_mdctsf_aft[4][wireId]!=0) ) flag_lastone = false;
		if( (flag_firstone==false) && ((i-seriesOne)==1) && flag_lastone) m_mdctsf_aft[4][wireId]=i+1;
		
		if((i-seriesOne) == 1 && firstSOne) m_mdctsf_sone[4][wireId] += 1;
		if(m_mdctsf_sone[4][wireId] != 0 && (i-seriesOne) != 1) firstSOne = false;
		seriesOne = i;
		m_mdctsf_one[4][wireId] += 1;
		m_mdctsf_win[4][wireId] += 1<<i;
	      }
	      if(layerId == 37) {
		if(flag_firstone && (i-seriesOne!=1) && (i!=0)){ m_mdctsf_pre[5][wireId] = i+1; flag_firstone = false;}
		if( ((i-seriesOne)!=1) && (m_mdctsf_aft[5][wireId]!=0) ) flag_lastone = false;
		if( (flag_firstone==false) && ((i-seriesOne)==1) && flag_lastone) m_mdctsf_aft[5][wireId]=i+1;
		
		if((i-seriesOne) == 1 && firstSOne) m_mdctsf_sone[5][wireId] += 1;
		if(m_mdctsf_sone[5][wireId] != 0 && (i-seriesOne) != 1) firstSOne = false;
		seriesOne = i;
		m_mdctsf_one[5][wireId] += 1;
		m_mdctsf_win[5][wireId] += 1<<i;
	      }
	    }
	  }
	  if(data_type == 2) { // LTK
	    if((data[byteId + i*periods] >> bitId) & 1) {
		if(flag_firstone && (i-seriesOne!=1) && (i!=0)){ m_mdcltrk_pre[wireId] = i+1; flag_firstone = false;}
		if( ((i-seriesOne)!=1) && (m_mdcltrk_aft[wireId]!=0) ) flag_lastone = false;
		if( (flag_firstone==false) && ((i-seriesOne)==1) && flag_lastone) m_mdcltrk_aft[wireId]=i+1;
		
		if((i-seriesOne) == 1 && firstSOne) m_mdcltrk_sone[wireId] += 1;
		if(m_mdcltrk_sone[wireId] != 0 && (i-seriesOne) != 1) firstSOne = false;
		seriesOne = i;
		m_mdcltrk_one[wireId] += 1;
		m_mdcltrk_win[wireId] += 1<<i;
	    }
	  }
	  if(data_type == 3) { // STK
	    if((data[byteId + i*periods] >> bitId) & 1) {
		if(flag_firstone && (i-seriesOne!=1) && (i!=0)){ m_mdcstrk_pre[wireId] = i+1; flag_firstone = false;}
		if( ((i-seriesOne)!=1) && (m_mdcstrk_aft[wireId]!=0) ) flag_lastone = false;
		if( (flag_firstone==false) && ((i-seriesOne)==1) && flag_lastone) m_mdcstrk_aft[wireId]=i+1;
		
		if((i-seriesOne) == 1 && firstSOne) m_mdcstrk_sone[wireId] += 1;
		if(m_mdcstrk_sone[wireId] != 0 && (i-seriesOne) != 1) firstSOne = false;
		seriesOne = i;
		m_mdcstrk_one[wireId] += 1;
		m_mdcstrk_win[wireId] += 1<<i;
	    }
	  }
	} // end of TKF1-8

	if((iter->first) == 234) { // ITKF information
	  if(data_type == 0) {
	    if((data[byteId + i*periods] >> bitId) & 1) {
	      if(layerId == 2) {
		if((i - seriesOne) == 1 && firstSOne) m_mdctsf_sone[0][wireId] += 1;
		if(m_mdctsf_sone[0][wireId] != 0 && (i - seriesOne) != 1) firstSOne = false;
		seriesOne = i;
		m_mdctsf_one[0][wireId] += 1;
	      }
	      if(layerId == 6) {
		if((i - seriesOne) == 1 && firstSOne) m_mdctsf_sone[1][wireId] += 1;
		if(m_mdctsf_sone[1][wireId] != 0 && (i - seriesOne) != 1) firstSOne = false;
		seriesOne = i;
		m_mdctsf_one[1][wireId] += 1;
	      }
	    }
	  }
	  if(data_type == 1) { 
	    if((data[byteId + i*periods] >> bitId) & 1) {
	      if((i - seriesOne) == 1 && firstSOne) m_mdchit_sone[layerId][wireId] += 1;
	      if(m_mdchit_sone[layerId][wireId] != 0 && (i - seriesOne) != 1) firstSOne = false;
	      seriesOne = i;
	      m_mdchit_one[layerId][wireId] += 1;
	    }
	  }
	  if(data_type == 4) {
	    if((data[byteId + i*periods] >> bitId) & 1) {
	      m_mdcTrigCondi[wireId] = 1;
	    }
	  }
	} // end of ITKF 

	if((iter->first) == 235 || (iter->first) == 236) { // LTKC or STKC information
	  if(data_type == 2) {
	    if((data[byteId + i*periods] >> bitId) & 1) {
	      if((i - seriesOne) == 1 && firstSOne) m_mdcltrkc_sone[wireId] += 1;
	      if(m_mdcltrkc_sone[wireId] != 0 && (i - seriesOne) != 1) firstSOne = false;
	      seriesOne = i;
	      m_mdcltrkc_one[wireId] += 1;
	      if(wireId >= 128) m_mdcTrigCondi[wireId - 126] = 1;
	    }
	  }
	  if(data_type == 3) {
	    if((data[byteId + i*periods] >> bitId) & 1) {
	      if((i - seriesOne) == 1 && firstSOne) m_mdcstrkc_sone[wireId] += 1;
	      if(m_mdcstrkc_sone[wireId] != 0 && (i - seriesOne) != 1) firstSOne = false;
	      seriesOne = i;
	      m_mdcstrkc_one[wireId] += 1;
	      if(wireId >= 128) m_mdcTrigCondi[wireId - 122] = 1; 
	    }
	  }
	} //end of LTKC or STKC  
    
      } // loop in windows
     
    }
  }
}

void TrigMdc::print() {
  cout<<"The number of board is "<<m_datamap.size()<<endl;
  std::map<uint32_t, std::vector<uint32_t> >::iterator iter = m_datamap.begin();
  if(iter->first == 0xe2)
    cout<<"The data size of E2 board is "<<iter->second.size()<<endl;

  for(int i = 0; i < 24; i++)
    for(int j = 0; j < 256; j++)
      {
	m_mdchit_one[i][j] = 0;
	m_mdchit_sone[i][j] = 0;
	cout<<"MDC hits: "<<i<<" "<<j<<" "<<m_mdchit_one[i][j]<<endl;
	cout<<"MDC hits: "<<i<<" "<<j<<" "<<m_mdchit_sone[i][j]<<endl;
	if(i < 6) {
	  m_mdctsf_one[i][j] = 0;
	  m_mdctsf_sone[i][j] = 0;
	  cout<<"MDC tsf: "<<i<<" "<<j<<" "<<m_mdctsf_one[i][j]<<endl;
	  cout<<"MDC tsf: "<<i<<" "<<j<<" "<<m_mdctsf_sone[i][j]<<endl;
	}
      }
  for(int k = 0; k < 132; k++)
    {
      m_mdcltrkc_one[k] = 0;
      m_mdcstrkc_one[k] = 0;
      m_mdcltrkc_sone[k] = 0;
      m_mdcstrkc_sone[k] = 0;
      cout<<"MDC ltrkc: "<<k<<" "<<m_mdcltrkc_one[k]<<endl;
      cout<<"MDC strkc: "<<k<<" "<<m_mdcstrkc_one[k]<<endl;
      cout<<"MDC ltrkc: "<<k<<" "<<m_mdcltrkc_sone[k]<<endl;
      cout<<"MDC strkc: "<<k<<" "<<m_mdcstrkc_sone[k]<<endl;
      if(k < 128) {
	m_mdcltrk_one[k] = 0;
	m_mdcstrk_one[k] = 0;
	m_mdcltrk_sone[k] = 0;
	m_mdcstrk_sone[k] = 0;
	cout<<"MDC ltrk: "<<k<<" "<<m_mdcltrk_one[k]<<endl;
	cout<<"MDC strk: "<<k<<" "<<m_mdcstrk_one[k]<<endl;
	cout<<"MDC ltrk: "<<k<<" "<<m_mdcltrk_sone[k]<<endl;
	cout<<"MDC strk: "<<k<<" "<<m_mdcstrk_sone[k]<<endl;
      }
    }
}
