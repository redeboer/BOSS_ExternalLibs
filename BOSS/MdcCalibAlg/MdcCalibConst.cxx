#include "MdcCalibAlg/MdcCalibConst.h"
#include "MdcCalibAlg/MdcCalParams.h"

#include <iostream>

using namespace std;

typedef map<int, double>::value_type valType;

MdcCalibConst::MdcCalibConst(){
}

MdcCalibConst::MdcCalibConst(const MdcCalibConst& calconst){
     m_xtmap = calconst.m_xtmap;
     m_t0 = calconst.m_t0;
     m_delt0 = calconst.m_delt0;
     m_qtpar0 = calconst.m_qtpar0;
     m_qtpar1 = calconst.m_qtpar1;
     m_sdmap = calconst.m_sdmap;
}

void MdcCalibConst::initCalibConst(){
     int lay;
     int lr;
     int entr;
     int ord;
     int key;
     double val;
     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(entr=0; entr<MdcCalNENTRXT; entr++){
	       for(lr=0; lr<MdcCalLR; lr++){
		    for(ord=0; ord<MdcCalXtNPars; ord++){
			 key = getXtKey(lay, entr, lr, ord);
			 if(1 == ord){
			      val = 0.03;
			 } else if(6 == ord){
			      val = 999.0;
			 } else{
			      val = 0.0;
			 }
			 m_xtmap.insert( valType(key, val) );
		    }
	       }
	  }
     }

     int wir;
     double t0 = 0.0;
     double dt0 = 0.0;
     for(wir=0; wir<MdcCalTotCell; wir++){
	  m_t0.push_back(t0);
	  m_delt0.push_back(dt0);
     }

     double qtpar0 = 0.0;
     double qtpar1 = 0.0;
     for(lay=0; lay<MdcCalNLayer; lay++){
	  m_qtpar0.push_back(qtpar0);
	  m_qtpar1.push_back(qtpar1);
     }

     int bin;
     double sdpar = 0.16;
     for(lay=0; lay<MdcCalNLayer; lay++){
	  for(entr=0; entr<MdcCalNENTRSD; entr++){
	       for(lr=0; lr<2; lr++){
		    for(bin=0; bin<MdcCalSdNBIN; bin++){
			 key = getSdKey(lay, entr, lr, ord);
			 m_sdmap.insert( valType( key, sdpar) );
		    }    
	       }
	  }
     }
}

void MdcCalibConst::fillXtpar(int key, double val){
     m_xtmap.insert( valType( key, val ) );
}

void MdcCalibConst::resetXtpar(int lay, int entr, int lr, int order, double val){
     int key = getXtKey(lay, entr, lr, order);
     m_xtmap[key] = val;
}

double MdcCalibConst::getXtpar(int lay, int entr, int lr, int order){
     double xtpar = -999.0;
     int key = getXtKey(lay, entr, lr, order);
     if( 1 == m_xtmap.count(key) )
	  xtpar = m_xtmap[key];

     return xtpar;
}

int MdcCalibConst::getXtKey(int lay, int entr, int lr, int order) const{
     int key;

     key = ( (lay << XTLAYER_INDEX) & XTLAYER_MASK ) |
	  ( (entr << XTENTRA_INDEX) & XTENTRA_MASK ) |
	  ( (lr << XTLR_INDEX) & XTLR_MASK ) |
	  ( (order << XTORDER_INDEX) & XTORDER_MASK );

     return key;
}

void MdcCalibConst::setXtBegin(){
     m_xtiter = m_xtmap.begin();
}

int MdcCalibConst::getNextXtpar(int& key, double& xtpar) {
     if( m_xtiter != m_xtmap.end() ){
	  key = (*m_xtiter).first;
	  xtpar = (*m_xtiter).second;
	  m_xtiter++;
	  return 1;
     }
     else return 0;
}

void MdcCalibConst::fillQtpar(int order, double val){
     if( 0 == order ){
	  m_qtpar0.push_back( val );
     } else if( 1 == order ){
	  m_qtpar1.push_back( val );
     } else {
	  cout << "Order of Qtpar Error in fillQtpar()!" << endl;
     }
}

void MdcCalibConst::resetQtpar(int lay, int order, double val){
     if( 0 == order ){
	  m_qtpar0[lay] = val;
     } else if( 1 == order ){
	  m_qtpar1[lay] = val;
     } else{
	  cout << "Order of Qtpar Error in resetQtpar()!" << endl;
     }
}

double MdcCalibConst::getQtpar(int lay, int order) const{
     if( 0 == order ){
	  return m_qtpar0[lay];
     } else if( 1 == order ){
	  return m_qtpar1[lay];
     } else{
	  cout << "Order of Qtpar Error in getQtpar()!" << endl;
	  return -9999.0;
     }
}

void MdcCalibConst::fillSdpar(int key, double val){
     m_sdmap.insert( valType( key, val) );
}

void MdcCalibConst::resetSdpar(int lay, int entr, int lr, int bin, double val){
     int key = getSdKey(lay, entr, lr, bin);
     m_sdmap[key] = val;
}

double MdcCalibConst::getSdpar(int lay, int entr, int lr, int bin){
			       
     double sdpar = -999.0;
     int key = getSdKey(lay, entr, lr, bin);
     if( 1 == m_sdmap.count(key) ){
	  return m_sdmap[key];
     }

     return sdpar;
}

int MdcCalibConst::getSdKey(int lay, int entr, int lr, int bin) const{
     int key;

     key = ( (lay << SDLAYER_INDEX) & SDLAYER_MASK ) |
	  ( (entr << SDENTRA_INDEX) & SDENTRA_MASK ) |
	  ( (lr << SDLR_INDEX) & SDLR_MASK ) |
	  ( (bin << SDBIN_INDEX) & SDBIN_MASK );

     return key;
}

void MdcCalibConst::setSdBegin(){
     m_sditer = m_sdmap.begin();
}

int MdcCalibConst::getNextSdpar(int& key, double& sdpar) {
     if( m_sditer != m_sdmap.end() ){
	  key = (*m_sditer).first;
	  sdpar = (*m_sditer).second;
	  m_sditer++;
	  return 1;
     }
     else return 0;
}

void MdcCalibConst::clear(){
     m_xtmap.clear();
     m_t0.clear();
     m_delt0.clear();
     m_qtpar0.clear();
     m_qtpar1.clear();
     m_sdmap.clear();
}
