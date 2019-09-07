#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SvcFactory.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"

#include "McTruth/McParticle.h"

#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "VertexFit/IVertexDbSvc.h"
#include "VertexFit/Helix.h"
#include "VertexFit/VertexFit.h"

#include "DTagTool/DTagTool.h"




DTagTool::DTagTool() : m_evtSvc(0), m_iterbegin(0), m_iterend(0),
		       m_iterstag(0),  m_iterdtag1(0), m_iterdtag2(0),
		       m_chargebegin(0),m_chargeend(0),m_showerbegin(0),m_showerend(0),
		       m_tag1desigma(1.0),m_tag2desigma(1.0){
  
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), "/Event/EvtRec/EvtRecEvent");
  if ( ! evtRecEvent ) {
    cout << MSG::FATAL << "Could not find EvtRecEvent" << endl;
    exit(1);
  }
  
  SmartDataPtr<EvtRecTrackCol> evtRecTrackCol( eventSvc(), "/Event/EvtRec/EvtRecTrackCol");
  if ( ! evtRecTrackCol ) {
    cout << MSG::FATAL << "Could not find EvtRecTrackCol" << endl;
    exit(1);
  }


  StatusCode sc = Gaudi::svcLocator()->service("SimplePIDSvc", m_simplePIDSvc);
  if ( sc.isFailure() )
    {
      //log << MSG::FATAL << "Could not load SimplePIDSvc!" << endreq;
      exit(1);
    }



  m_chargebegin=evtRecTrackCol->begin();
  m_chargeend=evtRecTrackCol->begin()+evtRecEvent->totalCharged();
  m_showerbegin=evtRecTrackCol->begin()+evtRecEvent->totalCharged();
  m_showerend=evtRecTrackCol->begin()+evtRecEvent->totalTracks();

  /// Accessing DTagList 
  SmartDataPtr<EvtRecDTagCol> evtRecDTagCol(eventSvc(), EventModel::EvtRec::EvtRecDTagCol);
  if ( ! evtRecDTagCol ) {
    cout << "Could not find EvtRecDTagCol" << endl;
    exit(1);
  }

  /// Accessing Ks list
  SmartDataPtr<EvtRecVeeVertexCol> evtRecVeeVertexCol(eventSvc(), "/Event/EvtRec/EvtRecVeeVertexCol");
  if ( ! evtRecVeeVertexCol ) {
    cout<< "Could not find EvtRecVeeVertexCol" << endl;
    exit(1);
  }

  /// Accessing pi0 list
  SmartDataPtr<EvtRecPi0Col> recPi0Col(eventSvc(), "/Event/EvtRec/EvtRecPi0Col");
  if ( ! recPi0Col ) {
    cout<< "Could not find EvtRecPi0Col" << endl;
    exit(1);
  }

  /// Accessing eta list

  SmartDataPtr<EvtRecEtaToGGCol> recEtaToGGCol(eventSvc(), "/Event/EvtRec/EvtRecEtaToGGCol");
  if ( ! recEtaToGGCol ) {
    cout<< "Could not find EvtRecEtaToGGCol" << endl;
    exit(1);
  }


  
  m_iterbegin=evtRecDTagCol->begin();
  m_iterend=evtRecDTagCol->end();
  m_pi0iterbegin=recPi0Col->begin();
  m_pi0iterend=recPi0Col->end();
  m_etaiterbegin=recEtaToGGCol->begin();
  m_etaiterend=recEtaToGGCol->end();
  m_ksiterbegin=evtRecVeeVertexCol->begin();
  m_ksiterend=evtRecVeeVertexCol->end();
  
  if(evtRecDTagCol->size() > 0)
    m_isdtaglistempty=false;
  else
    m_isdtaglistempty=true;

  //set initial pid requirement
  m_pid=true;

    
  //fill d0, dp, ds modes seprately
  int index=0;
  for(EvtRecDTagCol::iterator iter=m_iterbegin; iter != m_iterend; iter++){

    if( (int)( (*iter)->decayMode())< 200 )
      m_d0modes.push_back( index ); 
    else  if( (int)( (*iter)->decayMode())< 400 )
      m_dpmodes.push_back( index ); 
    else  if( (int)( (*iter)->decayMode())< 1000 )
      m_dsmodes.push_back( index ); 
  
    index++;
  } 

}

void DTagTool::clear(){

  m_d0modes.clear();
  m_dpmodes.clear();
  m_dsmodes.clear();
  
}

DTagTool::~DTagTool(){

  m_d0modes.clear();
  m_dpmodes.clear();
  m_dsmodes.clear();

}

bool DTagTool::compare(EvtRecDTagCol::iterator pair1_iter1,EvtRecDTagCol::iterator pair1_iter2,EvtRecDTagCol::iterator pair2_iter1,EvtRecDTagCol::iterator pair2_iter2, double mD, string smass){
  
  double value1=0;
  double value2=0;
  if(smass=="mbc"){
    value1 = fabs(0.5*((*pair1_iter1)->mBC()+(*pair1_iter2)->mBC())-mD);
    value2 = fabs(0.5*((*pair2_iter1)->mBC()+(*pair2_iter2)->mBC())-mD);
  }
  else if(smass=="de"){
    value1 = pow((*pair1_iter1)->deltaE()/m_tag1desigma,2)+pow((*pair1_iter2)->deltaE()/m_tag2desigma,2);
    value2 = pow((*pair2_iter1)->deltaE()/m_tag1desigma,2)+pow((*pair2_iter2)->deltaE()/m_tag2desigma,2);
  }
  else if(smass=="inv"){
    value1 = fabs(0.5*((*pair1_iter1)->mass()+(*pair1_iter2)->mass())-mD);
    value2 = fabs(0.5*((*pair2_iter1)->mass()+(*pair2_iter2)->mass())-mD);
  }
  
  
  if( value1 <= value2)
    return true;
  else
    return false;
}

vector<int> DTagTool::mode(EvtRecDTag::DecayMode decaymode){

  vector<int> mode;
  int index=0;
  for(EvtRecDTagCol::iterator iter=m_iterbegin; iter != m_iterend; iter++){
    
    if(m_pid){
      if( (*iter)->decayMode() == decaymode && (*iter)->type() == 1 )
	mode.push_back( index ); 
    }
    else{
      if( (*iter)->decayMode() == decaymode )
	mode.push_back( index ); 
    }

    index++;
  }
  
  return mode;
}


vector<int> DTagTool::mode(int decaymode){

  vector<int> mode;
  int index=0;
  for(EvtRecDTagCol::iterator iter=m_iterbegin; iter != m_iterend; iter++){
    
    if(m_pid){
      if( (*iter)->decayMode() == decaymode && (*iter)->type() == 1 )
	mode.push_back( index ); 
    }
    else{
      if( (*iter)->decayMode() == decaymode )
	mode.push_back( index ); 
    }

    index++;
  }
  
  return mode;
}



//search single tag with charm
bool DTagTool::findSTag(EvtRecDTag::DecayMode mode, int tagcharm){
  
  return findSTag(static_cast<int>(mode), tagcharm);

}//end of stag 


//search single tag without charm
bool DTagTool::findSTag(EvtRecDTag::DecayMode mode){
  return findSTag(static_cast<int>(mode));
  
}//end of stag 




//use integer as argument
bool DTagTool::findSTag(int mode, int tagcharm){

  bool isStcand=false;
  double de_min=1;
 
  //loop over the dtag list
  EvtRecDTagCol::iterator iter_dtag=m_iterbegin;
  for ( ; iter_dtag != m_iterend; iter_dtag++){

    if(m_pid){
      if( (*iter_dtag)->type()!=1 || (*iter_dtag)->decayMode() != mode ||  (*iter_dtag)->charm() != tagcharm ) 
	continue;
    }
    else{
      if( (*iter_dtag)->decayMode() != mode ||  (*iter_dtag)->charm() != tagcharm ) 
	continue;
    }
    if(fabs((*iter_dtag)->deltaE())<fabs(de_min)){
      isStcand=true;
      m_iterstag=iter_dtag;
      de_min=(*iter_dtag)->deltaE();
    }
 
  } //end of looping over the entire DTag list
 
  return isStcand;
 
}//end of stag 


bool DTagTool::findSTag(int mode){

  bool isStcand=false;
  double de_min=1;
 
  //loop over the dtag list
  EvtRecDTagCol::iterator iter_dtag=m_iterbegin;
  for ( ; iter_dtag != m_iterend; iter_dtag++){
 
    if(m_pid){
      if(  (*iter_dtag)->type()!=1 || (*iter_dtag)->decayMode() != mode ) 
	continue;
    }
    else{
      if(  (*iter_dtag)->decayMode() != mode ) 
	continue;
    }

    if(fabs((*iter_dtag)->deltaE())<fabs(de_min)){
      isStcand=true;
      m_iterstag=iter_dtag;
      de_min=(*iter_dtag)->deltaE();
    }
 
  } //end of looping over the entire DTag list
 
  return isStcand;
 
}//end of stag 


////////////////////////////////////////////
////////////////////////////////////////////



//generic double tagging searches
bool DTagTool::findDTag(EvtRecDTag::DecayMode mode1, EvtRecDTag::DecayMode mode2, string smass){
  
  return findDTag(static_cast<int>(mode1), static_cast<int>(mode2), smass);
  
}

bool DTagTool::findDTag(EvtRecDTag::DecayMode mode1, int tagcharm1, EvtRecDTag::DecayMode mode2, int tagcharm2, string smass){
  
  return findDTag(static_cast<int>(mode1), tagcharm1, static_cast<int>(mode2), tagcharm2, smass);

} //end of findDtag 


//find all the double tags
bool DTagTool::findADTag(EvtRecDTag::DecayMode mode1, EvtRecDTag::DecayMode mode2){
  
  return findADTag(static_cast<int>(mode1), static_cast<int>(mode2));
  
}

bool DTagTool::findADTag(EvtRecDTag::DecayMode mode1, int tagcharm1, EvtRecDTag::DecayMode mode2, int tagcharm2){
  
  return findADTag(static_cast<int>(mode1), tagcharm1, static_cast<int>(mode2), tagcharm2);

} //end of findDtag 





bool DTagTool::findDTag(int mode1, int mode2, string smass){
  
  int tagcharm1= (mode1<10 || mode1>=200)?+1:0;
  int tagcharm2= (mode2<10 || mode2>=200)?-1:0;
    
  if(tagcharm1*tagcharm2>0){
    cout<<"double tagging warning! two  modes can't have same nonzero charmness"<<endl; 
    return false;
  }
  
  //define D candidate mass
  double mDcand=0;
  if( mode1 < 200 && mode2 < 200)
    mDcand = 1.8645;
  else if ( mode1>=200 && mode1 < 400 && mode2>=200 && mode2 < 400)
    mDcand = 1.8693;
  else if ( mode1>=400 && mode1 < 1000 && mode2>=400 && mode2 < 1000)
    mDcand = 1.9682;
  else{
    cout<<"double tag modes are not from same particle ! "<<endl;
    return false;
  }
  
  
  vector<int> igood1, igood2;
  igood1.clear(),igood2.clear();
  int index=0;
  EvtRecDTagCol::iterator iter_dtag=m_iterbegin;

  //charge conjucation considered
  for ( ; iter_dtag != m_iterend; iter_dtag++){
    int iter_mode=(*iter_dtag)->decayMode();
    int iter_charm=(*iter_dtag)->charm();
    int iter_type=(*iter_dtag)->type();

    if(m_pid){
      if( iter_mode == mode1 && iter_charm == tagcharm1 && iter_type==1 )
	igood1.push_back(index);
      if( tagcharm1!=0 && iter_mode == mode1 && iter_charm == -tagcharm1 && iter_type==1 )
	igood1.push_back(index);
      
      if( iter_mode == mode2 && iter_charm == tagcharm2 && iter_type==1 )
	igood2.push_back(index);
      if( tagcharm2!=0 && iter_mode == mode2 && iter_charm == -tagcharm2 && iter_type==1 )
	igood2.push_back(index);
    }
    else{
      if( iter_mode == mode1 && iter_charm == tagcharm1 )
	igood1.push_back(index);
      if( tagcharm1!=0 && iter_mode == mode1 && iter_charm == -tagcharm1 )
	igood1.push_back(index);
      
      if( iter_mode == mode2 && iter_charm == tagcharm2 )
	igood2.push_back(index);
      if( tagcharm2!=0 && iter_mode == mode2 && iter_charm == -tagcharm2 )
	igood2.push_back(index);
    }

    index++;
  }
  
  //look for the best pair of double-tagged event
  if(igood1.size()<1 || igood2.size()<1)
    return false;

  bool isDtcand=false;
  int index_i=0, index_j=0;
 
  EvtRecDTagCol::iterator iter_i, iter_j;
  int count=0;
  for(int i=0; i<igood1.size(); i++){
  
    iter_i=m_iterbegin+igood1[i];
    
    int charm_i=(*iter_i)->charm();
    for(int j=0;j<igood2.size();j++){
      iter_j=m_iterbegin+igood2[j];
      
      int charm_j=(*iter_j)->charm();
      if( charm_i*charm_j>0 || igood2[j] == igood1[i] ) continue;
      
      if(shareTracks(iter_i,iter_j)) continue;
      count++;
      if(count==1){
	m_iterdtag1=iter_i;
	m_iterdtag2=iter_j;
      }
      
      if( compare(iter_i,iter_j,m_iterdtag1,m_iterdtag2,mDcand,smass) ){
	m_iterdtag1=iter_i;
	m_iterdtag2=iter_j;
	isDtcand = true;
      }
      
    } //end of j loop
  } //end of i loop

  
  return isDtcand;
}


bool DTagTool::findDTag(int mode1, int tagcharm1, int mode2, int tagcharm2, string smass){

  if(tagcharm1*tagcharm2>0){
    cout<<"double tagging warning! two  modes can't have same nonzero charmness"<<endl; 
    return false;
  }
  
  //define D candidate mass
  double mDcand=0;
  if( mode1 < 200 && mode2 < 200)
    mDcand = 1.8645;
  else if (mode1>=200 && mode1 < 400 && mode2>=200 && mode2 < 400)
    mDcand = 1.8693;
  else if (mode1>=400 && mode1 < 1000 && mode2>=400 && mode2 < 1000)
    mDcand = 1.9682;
  else{
    cout<<"double tag modes are not from same particle ! "<<endl;
    return false;
  }
  
  
  vector<int> igood1, igood2;
  igood1.clear(),igood2.clear();
  int index=0;
  EvtRecDTagCol::iterator iter_dtag=m_iterbegin;

  for ( ; iter_dtag != m_iterend; iter_dtag++){
    int iter_mode=(*iter_dtag)->decayMode();
    int iter_charm=(*iter_dtag)->charm();
    int iter_type=(*iter_dtag)->type();

    if(m_pid){
      if( iter_mode == mode1 && iter_charm == tagcharm1 && iter_type==1 )
	igood1.push_back(index);
      
      if( iter_mode == mode2 && iter_charm == tagcharm2 && iter_type==1)
	igood2.push_back(index);
    }
    else{
      if( iter_mode == mode1 && iter_charm == tagcharm1)
	igood1.push_back(index);
      
      if( iter_mode == mode2 && iter_charm == tagcharm2)
	igood2.push_back(index);
    }

  
    index++;
  }
  
  //look for the best pair of double-tagged event

  if(igood1.size()<1 || igood2.size()<1)
    return false;

  bool isDtcand=false;
  int index_i=0, index_j=0;
  
  


  EvtRecDTagCol::iterator iter_i, iter_j;
  int count=0;
  for(int i=0; i<igood1.size(); i++){
  
    iter_i=m_iterbegin+igood1[i];
    int charm_i=(*iter_i)->charm();
    for(int j=0;j<igood2.size();j++){
      iter_j=m_iterbegin+igood2[j];
      int charm_j=(*iter_j)->charm();
      if( charm_i*charm_j>0 || igood2[j] == igood1[i] ) continue;
      
      if(shareTracks(iter_i,iter_j)) continue;
      count++;
      if(count==1){
	m_iterdtag1=iter_i;
	m_iterdtag2=iter_j;
      }
      
      if( compare(iter_i,iter_j,m_iterdtag1,m_iterdtag2,mDcand,smass) ){
	m_iterdtag1=iter_i;
        m_iterdtag2=iter_j;
        isDtcand = true;
      }


    } //end of j loop
  } //end of i loop
  

  return isDtcand;

} //end of findDtag 


bool DTagTool::findADTag(int mode1, int mode2){
  
  int tagcharm1= (mode1<10 || mode1>=200)?+1:0;
  int tagcharm2= (mode2<10 || mode2>=200)?-1:0;
    
  if(tagcharm1*tagcharm2>0){
    cout<<"double tagging warning! two  modes can't have same nonzero charmness"<<endl; 
    return false;
  }
  
  //define D candidate mass
  double mDcand=0;
  if( mode1 < 200 && mode2 < 200)
    mDcand = 1.8645;
  else if ( mode1>=200 && mode1 < 400 && mode2>=200 && mode2 < 400)
    mDcand = 1.8693;
  else if ( mode1>=400 && mode1 < 1000 && mode2>=400 && mode2 < 1000)
    mDcand = 1.9682;
  else{
    cout<<"double tag modes are not from same particle ! "<<endl;
    return false;
  }
  
  
  vector<int> igood1, igood2;
  igood1.clear(),igood2.clear();
  int index=0;
  EvtRecDTagCol::iterator iter_dtag=m_iterbegin;

  //charge conjucation considered
  for ( ; iter_dtag != m_iterend; iter_dtag++){
    int iter_mode=(*iter_dtag)->decayMode();
    int iter_charm=(*iter_dtag)->charm();
    int iter_type=(*iter_dtag)->type();

    if(m_pid){
      if( iter_mode == mode1 && iter_charm == tagcharm1 && iter_type==1 )
	igood1.push_back(index);
      if( tagcharm1!=0 && iter_mode == mode1 && iter_charm == -tagcharm1 && iter_type==1 )
	igood1.push_back(index);
      
      if( iter_mode == mode2 && iter_charm == tagcharm2 && iter_type==1 )
	igood2.push_back(index);
      if( tagcharm2!=0 && iter_mode == mode2 && iter_charm == -tagcharm2 && iter_type==1 )
	igood2.push_back(index);
    }
    else{
      if( iter_mode == mode1 && iter_charm == tagcharm1 )
	igood1.push_back(index);
      if( tagcharm1!=0 && iter_mode == mode1 && iter_charm == -tagcharm1 )
	igood1.push_back(index);
      
      if( iter_mode == mode2 && iter_charm == tagcharm2 )
	igood2.push_back(index);
      if( tagcharm2!=0 && iter_mode == mode2 && iter_charm == -tagcharm2 )
	igood2.push_back(index);
    }

    index++;
  }
  
  //look for the best pair of double-tagged event

  bool isDtcand=false;
  EvtRecDTagCol::iterator iter_i, iter_j;
  
  for(int i=0; i<igood1.size(); i++){
  
    iter_i=m_iterbegin+igood1[i];
    int charm_i=(*iter_i)->charm();
    
    for(int j=0;j<igood2.size();j++){
      iter_j=m_iterbegin+igood2[j];
      int charm_j=(*iter_j)->charm();
      if( charm_i*charm_j>0 || igood2[j] == igood1[i] ) continue;
      if(shareTracks(iter_i,iter_j)) continue;
      
      m_viterdtag1.push_back(m_iterbegin+igood1[i]);
      m_viterdtag2.push_back(m_iterbegin+igood2[j]);
    
      
    } //end of j loop
  } //end of i loop

  if(m_viterdtag1.size()>0){
    isDtcand=true;
  }
  
  return isDtcand;
}

bool DTagTool::findADTag(int mode1, int tagcharm1, int mode2, int tagcharm2){

  if(tagcharm1*tagcharm2>0){
    cout<<"double tagging warning! two  modes can't have same nonzero charmness"<<endl; 
    return false;
  }
  
  //define D candidate mass
  double mDcand=0;
  if( mode1 < 200 && mode2 < 200)
    mDcand = 1.8645;
  else if (mode1>=200 && mode1 < 400 && mode2>=200 && mode2 < 400)
    mDcand = 1.8693;
  else if (mode1>=400 && mode1 < 1000 && mode2>=400 && mode2 < 1000)
    mDcand = 1.9682;
  else{
    cout<<"double tag modes are not from same particle ! "<<endl;
    return false;
  }
  
  
  vector<int> igood1, igood2;
  igood1.clear(),igood2.clear();
  int index=0;
  EvtRecDTagCol::iterator iter_dtag=m_iterbegin;

  for ( ; iter_dtag != m_iterend; iter_dtag++){
    int iter_mode=(*iter_dtag)->decayMode();
    int iter_charm=(*iter_dtag)->charm();
    int iter_type=(*iter_dtag)->type();

    if(m_pid){
      if( iter_mode == mode1 && iter_charm == tagcharm1 && iter_type==1 )
	igood1.push_back(index);
      
      if( iter_mode == mode2 && iter_charm == tagcharm2 && iter_type==1)
	igood2.push_back(index);
    }
    else{
      if( iter_mode == mode1 && iter_charm == tagcharm1)
	igood1.push_back(index);
      
      if( iter_mode == mode2 && iter_charm == tagcharm2)
	igood2.push_back(index);
    }

  
    index++;
  }
  
  //look for the best pair of double-tagged event

  bool isDtcand=false;
  double deltaM=1.00;
  int index_i=0, index_j=0;
  EvtRecDTagCol::iterator iter_i, iter_j;
  
  for(int i=0; i<igood1.size(); i++){
  
    iter_i=m_iterbegin+igood1[i];
    int charm_i=(*iter_i)->charm();
    for(int j=0;j<igood2.size();j++){
      iter_j=m_iterbegin+igood2[j];
      int charm_j=(*iter_j)->charm();
      if( charm_i*charm_j>0 || igood2[j] == igood1[i] ) continue;
      
      if(shareTracks(iter_i,iter_j)) continue;

      m_viterdtag1.push_back(m_iterbegin+igood1[i]);
      m_viterdtag2.push_back(m_iterbegin+igood2[j]);      

    } //end of j loop
  } //end of i loop

  if(m_viterdtag1.size()>0)
    isDtcand=true;
  

  return isDtcand;

} //end of findADtag 



//////////////////////////////////
//////////////////////////////////



void DTagTool::operator<< ( EvtRecDTagCol::iterator iter){

  cout<<" print mode:"<< (*iter)->decayMode()<<endl;
  cout<<"beam energy is:"<< (*iter)->beamE()<<endl;
  cout<<"mBC is:"<< (*iter)->mBC()<<endl;
  cout<<"deltaE is:"<< (*iter)->deltaE()<<endl;
  cout<<"inv mass is:"<< (*iter)->mass()<<endl;
  cout<<"charge is:"<< (*iter)->charge()<<endl;
  cout<<"charm is:"<< (*iter)->charm()<<endl;
  cout<<"num of children is:"<< (*iter)->numOfChildren()<<endl;

  cout<<"found "<< (*iter)->tracks().size()<<" same side tracks."<<endl;
  cout<<"found "<< (*iter)->otherTracks().size()<<" other side tracks."<<endl;
  cout<<"found "<< (*iter)->showers().size()<<" same side showers."<<endl;
  cout<<"found "<< (*iter)->otherShowers().size()<<" other side showers."<<endl;

} 

HepLorentzVector DTagTool::pi0p4(EvtRecPi0Col::iterator pi0Itr, bool isconstrain){


  if(isconstrain){
    HepLorentzVector p41= (*pi0Itr)->hiPfit();
    HepLorentzVector p42= (*pi0Itr)->loPfit();
    return (p41+p42);
  }
  else {
    EvtRecTrack* trk1 = const_cast<EvtRecTrack*>((*pi0Itr)->hiEnGamma());
    EvtRecTrack* trk2 = const_cast<EvtRecTrack*>((*pi0Itr)->loEnGamma());
    
    RecEmcShower* emctrk1 = (trk1)->emcShower();
    RecEmcShower* emctrk2 = (trk2)->emcShower();
    
    HepLorentzVector p41=p4shower(emctrk1);
    HepLorentzVector p42=p4shower(emctrk2);
    return (p41+p42);
  }  
  
}

HepLorentzVector DTagTool::etap4(EvtRecEtaToGGCol::iterator etaItr, bool isconstrain){


  if(isconstrain){
    HepLorentzVector p41= (*etaItr)->hiPfit();
    HepLorentzVector p42= (*etaItr)->loPfit();
    return (p41+p42);
  }
  else {
    EvtRecTrack* trk1 = const_cast<EvtRecTrack*>((*etaItr)->hiEnGamma());
    EvtRecTrack* trk2 = const_cast<EvtRecTrack*>((*etaItr)->loEnGamma());
    
    RecEmcShower* emctrk1 = (trk1)->emcShower();
    RecEmcShower* emctrk2 = (trk2)->emcShower();
    
    HepLorentzVector p41=p4shower(emctrk1);
    HepLorentzVector p42=p4shower(emctrk2);
    return (p41+p42);
  }  
  
}



vector<int> DTagTool::pi0Id(EvtRecDTagCol::iterator iter_dtag, int numpi0){
  
  SmartRefVector<EvtRecTrack> showers=(*iter_dtag)->showers();
  if(showers.size()<2*numpi0){
    cout<<"too many pi0 required"<<endl;
    exit(1);
  }
   
  vector<int> canid;
  canid.clear();

  for(EvtRecPi0Col::iterator pi0Itr = m_pi0iterbegin;
      pi0Itr < m_pi0iterend; pi0Itr++){
    
    /// Access pi0 children
    EvtRecTrack* heGammaTrk = const_cast<EvtRecTrack*>((*pi0Itr)->hiEnGamma());
    EvtRecTrack* leGammaTrk = const_cast<EvtRecTrack*>((*pi0Itr)->loEnGamma());

    int heGammaTrkId = heGammaTrk->trackId();
    int leGammaTrkId = leGammaTrk->trackId();
    
    for(int index=0; index<numpi0; ++index){
      bool isheid=false;
      bool isleid=false;
      
      for(int i=index*2; i<index*2+2; ++i){
	
	if(!isheid && heGammaTrkId == showers[i]->trackId())
	  isheid=true;
	if(!isleid && leGammaTrkId == showers[i]->trackId())
	  isleid=true;
      }
      
      if(isheid && isleid)
	canid.push_back( pi0Itr - m_pi0iterbegin); 
    }
    

    if(canid.size()==numpi0){
      return canid;
      break;
    }

  }  // End "pi0Itr" FOR Loop

  return canid;
  
}



vector<int> DTagTool::etaId(EvtRecDTagCol::iterator iter_dtag, int numeta){
  
  SmartRefVector<EvtRecTrack> showers=(*iter_dtag)->showers();
  if(showers.size()<2*numeta){
    cout<<"too many eta required"<<endl;
    exit(1);
  }
   
  vector<int> canid;
  canid.clear();

  for(EvtRecEtaToGGCol::iterator etaItr = m_etaiterbegin;
      etaItr < m_etaiterend; etaItr++){
    
    /// Access eta children
    EvtRecTrack* heGammaTrk = const_cast<EvtRecTrack*>((*etaItr)->hiEnGamma());
    EvtRecTrack* leGammaTrk = const_cast<EvtRecTrack*>((*etaItr)->loEnGamma());

    int heGammaTrkId = heGammaTrk->trackId();
    int leGammaTrkId = leGammaTrk->trackId();
    
    for(int index=0; index<numeta; ++index){
      bool isheid=false;
      bool isleid=false;
      
      for(int i=index*2; i<index*2+2; ++i){
	
	if(!isheid && heGammaTrkId == showers[i]->trackId())
	  isheid=true;
	if(!isleid && leGammaTrkId == showers[i]->trackId())
	  isleid=true;
      }
      
      if(isheid && isleid)
	canid.push_back( etaItr - m_etaiterbegin); 
    }
    

    if(canid.size()==numeta){
      return canid;
      break;
    }

  }  // End "etaItr" FOR Loop

  return canid;
  
}

vector<int> DTagTool::ksId(EvtRecDTagCol::iterator iter_dtag, int numks){
  
  SmartRefVector<EvtRecTrack> tracks=(*iter_dtag)->tracks();
  if(tracks.size()<2*numks){
    cout<<"too many kshort required"<<endl;
    exit(1);
  }
  vector<int> canid;
  canid.clear();
  
  if(tracks.size()==0) 
    return canid;
  

  for(EvtRecVeeVertexCol::iterator ksItr = m_ksiterbegin;
      ksItr < m_ksiterend; ksItr++){

    /// Needed to reject Lambda (and conversion?) combinations
    if((*ksItr)->vertexId() != 310) continue;

    EvtRecTrack* aKsChild1Trk = (*ksItr)->daughter(0);
    EvtRecTrack* aKsChild2Trk = (*ksItr)->daughter(1);

    int ksChild1TrkId = aKsChild1Trk->trackId();
    int ksChild2TrkId = aKsChild2Trk->trackId();
   
    for(int index=0; index<numks; ++index){
      bool isc1id=false;
      bool isc2id=false;
      
      for(int i=index*2; i<index*2+2; ++i){
      	if(!isc1id && ksChild1TrkId == tracks[i]->trackId())
	  isc1id=true;
	if(!isc2id && ksChild2TrkId == tracks[i]->trackId())
	  isc2id=true;
      }
      
      if(isc1id && isc2id)
	canid.push_back( ksItr - m_ksiterbegin); 
    }

    if(canid.size()==numks){
      return canid;
      break;
    }
  }  // End "ksItr" FOR Loop
  
  return canid;
}

HepLorentzVector DTagTool::p4shower(RecEmcShower* shower ){

  double Eemc=shower->energy();
  double phi=shower->phi();
  double theta=shower->theta();
  HepLorentzVector p4(Eemc*sin(theta)*cos(phi),Eemc*sin(theta)*sin(phi),Eemc*cos(theta),Eemc);  
  return p4;
}

HepLorentzVector DTagTool::p4(RecMdcKalTrack* mdcKalTrack, int pid){

  HepVector zhelix;
  double mass=0;
  
  if(pid==0){
    zhelix=mdcKalTrack->getZHelixE();
    mass=0.000511;
  }
  else if(pid==1){
    zhelix=mdcKalTrack->getZHelixMu();
    mass= 0.105658;
  }
  else if(pid==2){
    zhelix=mdcKalTrack->getZHelix();
    mass=0.139570;
  }
  else if(pid==3){
    zhelix=mdcKalTrack->getZHelixK();
    mass= 0.493677;
  }
  else{
    zhelix=mdcKalTrack->getZHelixP();
    mass= 0.938272;
  }
 
  double dr(0),phi0(0),kappa(0),dz(0),tanl(0);
  dr=zhelix[0];
  phi0=zhelix[1];
  kappa=zhelix[2];
  dz=zhelix[3];
  tanl=zhelix[4];
 
  int charge=0;

  if (kappa > 0.0000000001)
    charge = 1;
  else if (kappa < -0.0000000001)
    charge = -1;
 
  double pxy=0;
  if(kappa!=0) pxy = 1.0/fabs(kappa);
 
  double px = pxy * (-sin(phi0));
  double py = pxy * cos(phi0);
  double pz = pxy * tanl;
 
  double e  = sqrt( pxy*pxy + pz*pz + mass*mass );
 
  return HepLorentzVector(px, py, pz, e);


}

bool DTagTool::isPion(EvtRecTrack* trk){
  
  SmartRefVector<EvtRecTrack> pionid=(*m_iterbegin)->pionId();
  
  for(int i=0; i < pionid.size() ;i++){
    if( trk->trackId() == pionid[i]->trackId()){
      return true;
      break;
    }
  }
  
  return false;
}


bool DTagTool::isKaon(EvtRecTrack* trk){
  SmartRefVector<EvtRecTrack> kaonid=(*m_iterbegin)->kaonId();

  for(int i=0; i < kaonid.size() ;i++){
    if( trk->trackId() == kaonid[i]->trackId()){
      return true;
      break;
    }
  }
 
  return false;
}



bool DTagTool::isElectron(EvtRecTrack* trk){
  
  m_simplePIDSvc->preparePID(trk);
  return ( m_simplePIDSvc->iselectron(true));

  /*

  double dedxchi=-99;
  double Eemc=0;
  double ptrk=-99;
 
  if(trk->isMdcDedxValid()){
    RecMdcDedx* dedxTrk=trk->mdcDedx();
    dedxchi=dedxTrk->chiE();
  }
   
  if( trk->isMdcKalTrackValid() ){
    RecMdcKalTrack *mdcKalTrk = trk->mdcKalTrack();
    ptrk= mdcKalTrk->p();
  }
  if( trk->isEmcShowerValid()){
    RecEmcShower *emcTrk = trk->emcShower();
    Eemc=emcTrk->energy();
  }
 
  double eop = Eemc/ptrk;
 
  if( fabs(eop)>0.8 && fabs(dedxchi)<5)
    return true;
  else
    return false;
  */
}

bool DTagTool::isMuon(EvtRecTrack* trk){
  
  double depth=-99;

  double dedxchi=-99;
  double Eemc=0;
  double ptrk=-99;
 
  if(trk->isMdcDedxValid()){
    RecMdcDedx* dedxTrk=trk->mdcDedx();
    dedxchi=dedxTrk->chiMu();
  }
   
  if( trk->isMdcKalTrackValid() ){
    RecMdcKalTrack *mdcKalTrk = trk->mdcKalTrack();
    ptrk= mdcKalTrk->p();
  }
  if( trk->isEmcShowerValid()){
    RecEmcShower *emcTrk = trk->emcShower();
    Eemc=emcTrk->energy();
  }
 
  double eop = Eemc/ptrk;

  if(  trk->isMucTrackValid() ){
    RecMucTrack* mucTrk=trk->mucTrack();
    depth=mucTrk->depth();
  }

  if( fabs(dedxchi)<5 && fabs(Eemc)>0.15 && fabs(Eemc)<0.3 && (depth>=80*ptrk-60 || depth>40))
    return true;
  return false;
}

bool DTagTool::isGoodTrack(EvtRecTrack* trk){
  
  if( !trk->isMdcKalTrackValid()) {
    return false;
  }

  Hep3Vector xorigin(0,0,0);
  IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if(vtxsvc->isVertexValid()){
    double* dbv = vtxsvc->PrimaryVertex();
    double*  vv = vtxsvc->SigmaPrimaryVertex();
    xorigin.setX(dbv[0]);
    xorigin.setY(dbv[1]);
    xorigin.setZ(dbv[2]);
  }
  
  
  RecMdcKalTrack *mdcKalTrk = trk->mdcKalTrack();
  mdcKalTrk->setPidType(RecMdcKalTrack::pion);
  HepVector    a  = mdcKalTrk->getZHelix();
  HepSymMatrix Ea = mdcKalTrk->getZError();
  HepPoint3D pivot(0.,0.,0.);
  HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]);
  VFHelix helixp(pivot,a,Ea);
  helixp.pivot(IP);
  HepVector vec    = helixp.a();
  double    vrl    = vec[0];
  double    vzl    = vec[3];
  double costheta  = cos(mdcKalTrk->theta());
  
  if(fabs(vrl)<1 && fabs(vzl)<10 && fabs(costheta)<0.93)
    return true;
  return false;
}

bool DTagTool::isGoodShower(EvtRecTrack* trk){
  if( !(trk->isEmcShowerValid() )) return false;
  RecEmcShower *emcTrk = trk->emcShower();
  double eraw = emcTrk->energy();
  double time = emcTrk->time();
  double costh = cos(emcTrk->theta());
  if( ( 
       (fabs(costh)<0.80 && eraw>0.025) 
       || (fabs(costh)>0.86 && eraw>0.05) 
       ) && time>0 && time<14 ) 
    return true;

  return false;
}

bool DTagTool::cosmicandleptonVeto(bool emc){
  
  //good track list
  vector<EvtRecTrackIterator> iGood;
  iGood.clear();
  for(EvtRecTrackIterator iter=m_chargebegin; iter!= m_chargeend; iter++){ 
    if(isGoodTrack(*iter))
      iGood.push_back(iter);
  }
  
  if(iGood.size() != 2)
    return true;
  
  //cosmic veto
  double time1=-99,time2=-99;
  for(vector<EvtRecTrackIterator>::size_type i=0;i<2;i++){
    if( (*iGood[i])->isTofTrackValid() ){
      SmartRefVector<RecTofTrack> tofTrkCol= (*iGood[i])->tofTrack();
      SmartRefVector<RecTofTrack>::iterator iter_tof=tofTrkCol.begin();
      
      for(;iter_tof!=tofTrkCol.end();iter_tof++){
	TofHitStatus* status =new TofHitStatus;
	status->setStatus( (*iter_tof)->status() );
	if(status->is_cluster()){
	  if(i==0)
	    time1=(*iter_tof)->tof();
	  else
	    time2=(*iter_tof)->tof();
	}
	delete status;
      }
    }
  }
  if( time1!=-99 && time2!=-99 && fabs(time1-time2)>5)
    return false;
  
  //rad bhabha veto
  if(isElectron( *iGood[0]) && isElectron(*iGood[1]))
    return false;
  
  //rad dimu veto
  if(isMuon( *iGood[0]) && isMuon(*iGood[1]))
    return false;


  //require additonal emc shower in the event
  if(emc){

    bool gotgoodshower=false;
    
    for(EvtRecTrackIterator iter=m_showerbegin; iter!= m_showerend; iter++){ 
      
      if(!(*iter)->isEmcShowerValid()) continue;
      RecEmcShower *emcTrk = (*iter)->emcShower();
      
      double eraw = emcTrk->energy();
      double time = emcTrk->time();
      if( !(eraw>0.05 && time>0 && time<14 ))
	continue;
      
      
      double angle1=angleShowerwithTrack(*iter, *iGood[0]);
      double angle2=angleShowerwithTrack(*iter, *iGood[1]);
      
      if(angle1>20 && angle2>20){
	gotgoodshower=true;
	break;
      }
	
      
    }

    return gotgoodshower;

  }//end of adding emc

  return true;
}

double DTagTool::angleShowerwithTrack(EvtRecTrack *shower, EvtRecTrack *track){
  
  double angle=-100;
  
  RecEmcShower *emcTrk = shower->emcShower();
  Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());
  
  
  if ( !track->isExtTrackValid() ) return angle;
  RecExtTrack* extTrk = track->extTrack();
  if ( extTrk->emcVolumeNumber() == -1 ) return angle;
  Hep3Vector extpos = extTrk->emcPosition();
  if(extpos==(-99,-99,-99))
    return angle;
  
  return extpos.angle(emcpos)*180/CLHEP::pi;
  
}

bool DTagTool::shareTracks(EvtRecDTagCol::iterator iter1,EvtRecDTagCol::iterator iter2){
  
  SmartRefVector<EvtRecTrack> tracks1=(*iter1)->tracks();
  SmartRefVector<EvtRecTrack> showers1=(*iter1)->showers();
  SmartRefVector<EvtRecTrack> tracks2=(*iter2)->tracks();
  SmartRefVector<EvtRecTrack> showers2=(*iter2)->showers();
  
  //charged tracks
  for(int i=0; i<tracks1.size(); i++){
    for(int j=0; j<tracks2.size(); j++){
      if(tracks1[i]==tracks2[j])
	return true;
    }
  }

  //neutral showers
  for(int i=0; i<showers1.size(); i++){
    for(int j=0; j<showers2.size(); j++){
      if(showers1[i]==showers2[j])
	return true;
    }
  }  

  return false;
}

IDataProviderSvc* DTagTool::eventSvc(){

  if(m_evtSvc == 0){

    StatusCode sc = Gaudi::svcLocator()->service ( "EventDataSvc", m_evtSvc, true);
    if( sc.isFailure() ) {
      assert(false);
    }

  }

  return m_evtSvc;

}
