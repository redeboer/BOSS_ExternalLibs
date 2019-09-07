#include "CalibData/Mdc/MdcCalibData.h"
//typedef std::map<int, double>::value_type valType;
#include "GaudiKernel/MsgStream.h"
namespace CalibData {
  MdcCalibData::MdcCalibData(){
     newXtTreesBuf = NULL;
     r2tTreesBuf = NULL;
     newXtTreesArray = NULL;
     r2tTreesArray = NULL;
     for(int layid=0; layid<43; layid++){
          m_r2t[layid] = NULL;
          for(int entr=0; entr<18; entr++){
	      for(int lr=0; lr<2; lr++){
	           m_newxt[layid][entr][lr] = NULL;
	      }//end lr
	  }//end entr
     }//end for layid
  } 

  MdcCalibData::~MdcCalibData(){
     for(int layid=0; layid<43; layid++){
	  delete m_r2t[layid];
	  for(int entr=0; entr<18; entr++){
	       for(int lr=0; lr<2; lr++){
		 if(m_newxt[layid][entr][lr]!=NULL) delete m_newxt[layid][entr][lr];
	       }
	  }
     }
     if(newXtTreesBuf!=NULL) newXtTreesBuf->Clear();
     if(r2tTreesBuf!=NULL) r2tTreesBuf->Clear();
     if(newXtTreesArray!=NULL) delete newXtTreesArray;
     if(r2tTreesArray!=NULL) delete r2tTreesArray;
  }

  StatusCode MdcCalibData::update(CalibBase1& other, MsgStream* log)
  {
    MdcCalibData& other1 = dynamic_cast<MdcCalibData& >(other);
    std::cout<<"\n"<<"here is the update in the MdcCalConst"<<std::endl;
    CalibBase1::update(other, log);
    clear();
    m_xtmap = other1.m_xtmap;
    m_t0 = other1.m_t0;
    m_delt0 = other1.m_delt0;
    m_qtpar0 = other1.m_qtpar0;
    m_qtpar1 = other1.m_qtpar1;
    m_sdmap = other1.m_sdmap;
    return StatusCode::SUCCESS;  
}	
				
void MdcCalibData::setXtpar(int xtkey,double val){
     m_xtmap.insert( valType( int(xtkey), val) );
}

void MdcCalibData::setNewXtpar(TObjArray* newXtTrees){
     //put TObjArray to temp buffer
     TBufferFile* tempBuf = new TBufferFile(TBuffer::kWrite);
     newXtTrees->Streamer(*tempBuf);

     //put content of buffer to array
     newXtTreesBuf = new TBufferFile(TBuffer::kRead);
     newXtTreesBuf->SetBuffer(tempBuf->Buffer(), tempBuf->BufferSize());

     newXtTreesArray = new TObjArray();
     newXtTreesArray->Streamer(*newXtTreesBuf);
     for(int layid=0; layid<43; layid++){
	  for(int entr=0; entr<18; entr++){
	       for(int lr=0; lr<2; lr++){
		    char newXtTreeName[20];
		    sprintf(newXtTreeName,"trNewXt%02d_%02d_%d",layid,entr,lr);
		    m_newxt[layid][entr][lr] = (TTree*)newXtTreesArray->FindObject(newXtTreeName);; 
	       }
	  }
     }
     delete tempBuf;
}

void MdcCalibData::setR2tpar(TObjArray* r2tTrees){
     //put TObjArray to temp buffer
     TBufferFile* tempBuf = new TBufferFile(TBuffer::kWrite);
     r2tTrees->Streamer(*tempBuf);

     //put content of buffer to array
     r2tTreesBuf = new TBufferFile(TBuffer::kRead);
     r2tTreesBuf->SetBuffer(tempBuf->Buffer(), tempBuf->BufferSize());

     r2tTreesArray = new TObjArray();
     r2tTreesArray->Streamer(*r2tTreesBuf);
     for(int layid=0; layid<43; layid++){
	  char r2tTreeName[20];
	  sprintf(r2tTreeName,"r2t%02d",layid);
	  m_r2t[layid] = (TTree*)r2tTreesArray->FindObject(r2tTreeName);; 
     }
     delete tempBuf;
}

void MdcCalibData::setT0(double val){
     m_t0.push_back( val );
}
void MdcCalibData::setDelT0( double val){
     m_delt0.push_back( val );
}

void MdcCalibData::setQtpar0(double val){
     m_qtpar0.push_back( val );
}

void MdcCalibData::setQtpar1(double val){
     m_qtpar1.push_back( val );
}

void MdcCalibData::setSdpar(int sdkey, double val){
     m_sdmap.insert( valType( int(sdkey), val) );
}

// wulh updated on 08-3-24
double MdcCalibData::getXtpar(int layid, int entr, int lr, int order){
     double xtpar = -999.0;
     int key = getXtKey(layid, entr, lr, order);
     if( 1 == m_xtmap.count(key) )
	  xtpar = m_xtmap[key];

     return xtpar;
}

// yzhang add 2011-12-12 
TTree* MdcCalibData::getNewXtpar(int layid, int entr, int lr){
     return m_newxt[layid][entr][lr];
}

// yzhang add 2011-12-15
TTree* MdcCalibData::getR2tpar(int layid){
     return m_r2t[layid];
}

// wulh updated on 08-3-24
double MdcCalibData::getSdpar(int layid, int entr, int lr, int bin){
     double sdpar = -999.0;
     int key = getSdKey(layid, entr, lr, bin);
     if( 1 == m_sdmap.count(key) ){
	  return m_sdmap[key];
     }

     return sdpar;
}

// wulh updated on 08-3-24
int MdcCalibData::getXtKey(int layid, int entr, int lr, int order){
     int key;

     key = ( (layid << XTLAYER_INDEX) & XTLAYER_MASK ) |
	  ( (entr << XTENTRA_INDEX) & XTENTRA_MASK ) |
	  ( (lr << XTLR_INDEX) & XTLR_MASK ) |
	  ( (order << XTORDER_INDEX) & XTORDER_MASK );

     return key;
}

// wulh updated on 08-3-24
int MdcCalibData::getSdKey(int layid, int entr, int lr, int bin){
     int key;

     key = ( (layid << SDLAYER_INDEX) & SDLAYER_MASK ) |
	  ( (entr << SDENTRA_INDEX) & SDENTRA_MASK ) |
	  ( (lr << SDLR_INDEX) & SDLR_MASK ) |
	  ( (bin << SDBIN_INDEX) & SDBIN_MASK );

     return key;
}

void MdcCalibData::setXtBegin(){
     m_xtiter = m_xtmap.begin();
}

int MdcCalibData::getNextXtpar(int& key, double& xtpar){
     if( m_xtiter != m_xtmap.end() ){
	  key = (*m_xtiter).first;
	  xtpar = (*m_xtiter).second;
	  m_xtiter++;
	  return 1;
     }
     else return 0;
}

void MdcCalibData::setSdBegin(){
     m_sditer = m_sdmap.begin();
}

int MdcCalibData::getNextSdpar(int& key, double& sdpar){
     if( m_sditer != m_sdmap.end() ){
	  key = (*m_sditer).first;
	  sdpar = (*m_sditer).second;
	  m_sditer++;
	  return 1;
     }
     else return 0;
}
void MdcCalibData::clear(){
     m_xtmap.clear();
     m_t0.clear();
     m_delt0.clear();
     m_qtpar0.clear();
     m_qtpar1.clear();
     m_sdmap.clear();
}

}
