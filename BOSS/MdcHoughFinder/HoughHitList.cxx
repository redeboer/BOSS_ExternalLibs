#include "MdcHoughFinder/HoughHitList.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "McTruth/MdcMcHit.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"

HoughHitList::HoughHitList(){
}

HoughHitList::HoughHitList(MdcDigiVec mdcDigiVec){
  addMdcDigiList(mdcDigiVec);
}

HoughHitList::~HoughHitList(){
  clearHoughHitList();
}

void HoughHitList::clearHoughHitList(){
  std::vector<HoughHit> emptyHitList; 
  emptyHitList.swap(_houghHitList);
}


//int HoughHitList::nMdcHit() const{
//  int nMdcHit = 0;
//  std::vector<HoughHit>::const_iterator iter = _houghHitList.begin();
//  for (;iter != _houghHitList.end(); iter++) {
//    if(*iter->detectorType()==HoughHit::detectorType::MDC) nMdcHit++;
//  }
//  return nMdcHit;
//}
//
//int HoughHitList::nCgemHit() const{
//  int nCgemHit = 0;
//  std::vector<HoughHit>::const_iterator iter = _houghHitList.begin();
//  for (;iter != _houghHitList.end(); iter++) {
//    if(*iter->detectorType()==HoughHit::detectorType::CGEM) nCgemHit++;
//  }
//  return nCgemHit;
//}

//FIXME
void HoughHitList::addHit(HoughHit& a){
  _houghHitList.push_back(a);
}

int HoughHitList::addMdcDigiList(MdcDigiVec mdcDigiVec){
  int nHitAdd = 0;
  MdcDigiVec::iterator iter = mdcDigiVec.begin();
  for (;iter != mdcDigiVec.end(); iter++) {
    const MdcDigi* const digi = (*iter);
    HoughHit hit(digi);//yzhang memory leakage?
	if( hit.driftTime()>1000 ) continue;              //to big hit ,maybe error hit
	if( hit.driftTime()<=0) continue;              //to big hit ,maybe error hit
//	int stype= hit.getSlayerType();
	_houghHitList.push_back(hit);
	nHitAdd++;
  }
  //continousHit();
  return nHitAdd;
}

bool small_layer(const HoughHit& a,const HoughHit& b){
  return a.getWireId() < b.getWireId();
}

//judge continous  -- remove noise before
int HoughHitList::continousHit(){
  const int maxCell[43] = { 
	40,44,48,56,     64,72,80,80,     76,76,88,88,
	100,100,112,112, 128,128,140,140, 160,160,160,160,
	176,176,176,176, 208,208,208,208, 240,240,240,240,
	256,256,256,256, 288,288,288 };
  vector<HoughHit> vec_HoughHit[43];
  vector<HoughHit>::const_iterator iter=_houghHitList.begin();
  for(int order=0; iter!=_houghHitList.end();iter++,order++){
	int layer = (*iter).getLayerId();
	//int wire= (*iter).getWireId();
	vec_HoughHit[layer].push_back(*iter);
  }
	vector<HoughHit> vec_HoughHit_del;
  //find continuous hits 5 hits or more
  for(int i=0;i<43;i++){
  //cout<<" layer "<<i<<" collect "<<vec_HoughHit[i].size()<<" hits "<<endl;
  if(i<8)  continue;   //don't do for inner chamber
	vector<int> vec_seeds;
	std::sort(vec_HoughHit[i].begin(),vec_HoughHit[i].end(),small_layer);
	if(vec_HoughHit[i].size()<=4) continue;
//	for(unsigned int j=0;j<vec_HoughHit[i].size();j++){
//	  cout<<"("<<i<<","<<vec_HoughHit[i][j].getWireId()<<")";
//	}
	for(unsigned int j=0;j<vec_HoughHit[i].size()-4;j++){
	  vector<int>::iterator iter_hit = find(vec_seeds.begin(),vec_seeds.end(),vec_HoughHit[i][j].getWireId());
	  if( iter_hit!=vec_seeds.end() ) continue;
	  int wire_last=vec_HoughHit[i][j].getWireId();
	  int wire=-999;
//	  bool conti=true;
	  int seeds=1;
	  if(wire_last==0) {
		//	cout<<"maybe end conect to begin "<<endl;
		for(unsigned int k=vec_HoughHit[i].size()-j-1;k>0;k--){
		  wire=vec_HoughHit[i][j+k].getWireId();
		  int charge = vec_HoughHit[i][j+k].getCharge();
		  int driftTime = vec_HoughHit[i][j+k].driftTime();
		  // cout<<"wirelast ("<<i<<","<<wire_last<<")"<<endl;
		  // cout<<"wire ("<<i<<","<<wire<<")"<<endl;
		  //if( (wire-maxCell[i]+1)!=wire_last || ( driftTime>0 && driftTime<800) ) break;
		  if( (wire-maxCell[i]+1)!=wire_last || (charge>0 && driftTime>0 && driftTime<800) ) break;
		  //cout<<"conti hit "<<"("<<i<<","<<wire<<")"<<endl;
		  wire_last= wire-maxCell[i];
		  seeds++;
		  //  cout<<"seeds "<<seeds<<endl;
		  if(seeds==5) {
			vec_seeds.push_back(wire+4-maxCell[i]); 
			vec_seeds.push_back(wire+3); 
			vec_seeds.push_back(wire+2); 
			vec_seeds.push_back(wire+1); 
			vec_seeds.push_back(wire); 
		  }
		  if(seeds>5) vec_seeds.push_back(wire); 
		}
		wire_last=0;

		for(unsigned int k=1;k<vec_HoughHit[i].size()-j;k++){
		  vector<int>::iterator iter_hit0 = find(vec_seeds.begin(),vec_seeds.end(),vec_HoughHit[i][j].getWireId());
		  if( iter_hit0!=vec_seeds.end() ) continue;
		  wire=vec_HoughHit[i][j+k].getWireId();
		  int charge = vec_HoughHit[i][j+k].getCharge();
		  int driftTime = vec_HoughHit[i][j+k].driftTime();
		  // cout<<"wirelast ("<<i<<","<<wire_last<<")"<<endl;
		  //  cout<<"wire ("<<i<<","<<wire<<")"<<endl;
		  if( wire<=maxCell[i] ) {
			//if( (wire-1)!=wire_last || ( driftTime>0 && driftTime<800))  break;
			if( (wire-1)!=wire_last || (charge>0 && driftTime>0 && driftTime<800))  break;
			//	cout<<"conti hit "<<"("<<i<<","<<wire<<")"<<endl;
			wire_last= wire;
			seeds++;
			//	cout<<"seeds "<<seeds<<endl;
			if(seeds==5) {
			  vec_seeds.push_back(wire-4); 
			  vec_seeds.push_back(wire-3); 
			  vec_seeds.push_back(wire-2); 
			  vec_seeds.push_back(wire-1); 
			  vec_seeds.push_back(wire); 
			}
			if(seeds>5) vec_seeds.push_back(wire); 
		  }
		}
	  }

	  else {
		for(unsigned int k=1;k<vec_HoughHit[i].size()-j;k++){
		  wire=vec_HoughHit[i][j+k].getWireId();
		  int charge = vec_HoughHit[i][j+k].getCharge();
		  int driftTime = vec_HoughHit[i][j+k].driftTime();
//		  cout<<"wirelast ("<<i<<","<<wire_last<<")"<<endl;
//		  cout<<"wire ("<<i<<","<<wire<<") tdc "<<vec_HoughHit[i][j+k].driftTime()<<" adc "<<vec_HoughHit[i][j+k].getCharge()<<endl;
		  if( wire<=maxCell[i] ) {
			//if( (wire-1)!=wire_last || ( driftTime>0 && driftTime<800))  break;
			if( (wire-1)!=wire_last || (charge>0 && driftTime>0 && driftTime<800))  break;
//			cout<<"conti hit "<<"("<<i<<","<<wire<<")"<<endl;
			wire_last= wire;
			seeds++;
//			cout<<"seeds "<<seeds<<endl;
			if(seeds==5) {
			  vec_seeds.push_back(wire-4); 
			  vec_seeds.push_back(wire-3); 
			  vec_seeds.push_back(wire-2); 
			  vec_seeds.push_back(wire-1); 
			  vec_seeds.push_back(wire); 
			}
			if(seeds>5) vec_seeds.push_back(wire); 
		  }
		}
	  }

	}
	for(unsigned int ihit=0;ihit<vec_seeds.size();ihit++){
//	  cout<<"vec_seeds "<<"("<<i<<","<<vec_seeds[ihit]<<")"<<endl;
	  for(unsigned int jhit=0;jhit<vec_HoughHit[i].size();jhit++){
	  if(vec_HoughHit[i][jhit].getWireId()==vec_seeds[ihit])  vec_HoughHit_del.push_back(vec_HoughHit[i][jhit]);
	  }
	}
  }
	for(unsigned int ihit=0;ihit<vec_HoughHit_del.size();ihit++){
//	cout<<"("<<vec_HoughHit_del[ihit].getLayerId()<<","<<vec_HoughHit_del[ihit].getWireId()<<")"<<" tdc adc "<<vec_HoughHit_del[ihit].driftTime()<<","<<vec_HoughHit_del[ihit].getCharge()<<endl;
	remove(vec_HoughHit_del[ihit]);
	}
}

void HoughHitList::remove(const HoughHit& hit){
  vector<HoughHit>::iterator iter = _houghHitList.begin();
  for(;iter!=_houghHitList.end();iter++){
	if( hit.digi() == (*iter).digi() ) {
	_houghHitList.erase(iter);iter--;
//	cout<<"remove ("<<hit.getLayerId()<<","<<hit.getWireId()<<")"<<endl;
	}
  }
}

int HoughHitList::addTruthInfo(std::map<int, const HepVector > mcTkPars){
  //initialize digi info.
  const MdcMcHit* truthHitPtr[43][288];
  for(int i=0;i<43;i++){ for(int j=0;j<288;j++){ truthHitPtr[i][j]=NULL; } }
  IDataProviderSvc* eventSvc = NULL;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
  SmartDataPtr<MdcMcHitCol> mdcMcHitCol(eventSvc,"/Event/MC/MdcMcHitCol");
  if(mdcMcHitCol){
	MdcMcHitCol::iterator iter_mchit = mdcMcHitCol->begin();
	for (;iter_mchit != mdcMcHitCol->end(); iter_mchit++) {
	  const Identifier id= (*iter_mchit)->identify();
	  int l = MdcID::layer(id);
	  int w = MdcID::wire(id);
	  const MdcMcHit* mcHit = (*iter_mchit);
	  truthHitPtr[l][w] = mcHit;
	}
  }else{
	std::cout<<__FILE__<<" Could not get MdcMcHitCol  "<<std::endl;
	return 0;
  }


  //add truth info. to HoughHit in list
  int nHitTruthAdd = 0;
  for(std::vector<HoughHit>::iterator iter = _houghHitList.begin();iter!= _houghHitList.end(); iter++){
	int l = (*iter).getLayerId();
	int w = (*iter).getWireId();
	int mcTkId = (*iter).digi()->getTrackIndex();
	if( mcTkId>=1000 ) mcTkId-=1000;
	if(mcTkId>=0 && NULL!=truthHitPtr[l][w]) {
	  (*iter).setTruthInfo(truthHitPtr[l][w]);
	  //get truth track param to calc. flight length
	  if(mcTkPars.find(mcTkId)!= mcTkPars.end()){
		(*iter).setStyle(0);  // truth hit  no decay no deltae
		//HepVector helix = mcTkPars.find(mcTkId)->second;
		//fltLen = (z_pos - dz)/tanl
		//double rtemp = 333.567 * (1./fabs(helix[2]));
		//double flt = (truthHitPtr[l][w]->getPositionZ()/10.-helix[3])/helix[4]; 
		//		(*iter).setFltLenTruth( flt );
		/*
		   double cR = (-333.567)/helix[2];
		   double cX = (cR+helix[0])*cos(helix[1]);
		   double cY = (cR+helix[0])*sin(helix[1]);
		   double x= truthHitPtr[l][w]->getPositionX()/10;
		   double y= truthHitPtr[l][w]->getPositionY()/10;
		//cout<<"circle ("<<cX<<","<<cY<<" )"<<cR<<endl;
		//cout<<"hit ("<<x<<","<<y<<" )"<<endl;
		double theta_temp;
		if( cX==0 || x-cX==0 ){
		theta_temp=0;
		}
		else{
		theta_temp=M_PI-atan2(y-cY,x-cX)+atan2(cY,cX);
		if(theta_temp>2*M_PI){
		theta_temp=theta_temp-2*M_PI;
		}
		if(theta_temp<0){
		theta_temp=theta_temp+2*M_PI;
		}
		if( fabs(helix[2])/helix[2] == 1)
		theta_temp = 2*M_PI-theta_temp;
		}
		//cout<<theta_temp<<" cir "<<(*iter).getCirList()<<endl;
		if( theta_temp > M_PI && (*iter).getCirList() ==0 ) {
		(*iter).setCirList(1);
		//cout<<"("<<l<<" ,"<<w<<" ) "<<" theta "<<theta_temp<<(*iter).getCirList()<<endl;
		}
		*/
	  }else{
		(*iter).setStyle(1);  //decay
	  }
	  nHitTruthAdd++;
	}
	else  {
	  (*iter).setStyle(2);    //delta
	}
	if( mcTkId<0 )  (*iter).setStyle(-999);  //noise 
	//	if( mcTkId<0 )  (*iter).setOuter(0);  //  outer not influence noise
	//	if (m_debug>0) cout<<"style "<<(*iter).getStyle()<<endl;
	//cout<<" hit ("<<l<<" ,"<<w<<" )"<<(*iter).getCirList()<<" style "<<(*iter).getStyle()<<"  tq "<<(*iter).digi()->getTimeChannel()<<"  "<<(*iter).digi()->getChargeChannel()<<endl;
	//cout<<" hit ("<<l<<" ,"<<w<<" )"<<(*iter).getStyle()<<" trackindex "<<(*iter).digi()->getTrackIndex()<<endl;
	//cout<<" hit in list "<<(*iter).getStyle()<<endl; 
  }

  //set cir
  int cirlist=0;
  if( _houghHitList.size()!=0) _houghHitList[0].setCirList(0);
  for(unsigned int id = 1; id <_houghHitList.size(); id++){
	//	if( ((_houghHitList[id].getLayerId() < _houghHitList[id-1].getLayerId()) || (_houghHitList[id].getLayerId() == _houghHitList[id-1].getLayerId()&& _houghHitList[id-1].getLayerId() ==_houghHitList[id+1].getLayerId()) )&& (cirlist==0||cirlist==2) )	 cirlist++;
	if( ((_houghHitList[id].getLayerId() < _houghHitList[id-1].getLayerId())  )&& (cirlist==0||cirlist==2) )	 cirlist++;
	if(_houghHitList[id].getLayerId() > _houghHitList[id-1].getLayerId() && (cirlist==1||cirlist==3) )	 cirlist++;
	if(cirlist>3) cirlist=999;	
	_houghHitList[id].setCirList(cirlist);
  }
  int label_out_circle=999;
  int label_num=0;
  int label_fist=0;
  int max_layer=0;
  for(unsigned int id = 0; id <_houghHitList.size(); id++){
	if(_houghHitList[id].getStyle()==-999) continue;
	if( _houghHitList[id].getLayerId()>max_layer)  max_layer=_houghHitList[id].getLayerId(); 
  }
  for(unsigned int id = 1; id <_houghHitList.size(); id++){
	if( (_houghHitList[id-1].getLayerId() == _houghHitList[id].getLayerId()) && (_houghHitList[id].getLayerId() == _houghHitList[id+1].getLayerId()) && (_houghHitList[id+1].getLayerId() == _houghHitList[id+2].getLayerId()) && (_houghHitList[id+2].getLayerId() == _houghHitList[id+3].getLayerId()) && _houghHitList[id].getLayerId() == max_layer)	 label_out_circle=_houghHitList[id-1].getLayerId();
  }
  for(int unsigned id = 0; id <_houghHitList.size(); id++){
	if ( _houghHitList[id].getLayerId() == label_out_circle )   label_num++;
	if( label_num==1 ) label_fist = id;
  }
  for(int id = 0; id <label_num/2; id++){
	if( label_num %2 == 0 ) _houghHitList[label_fist+label_num/2+id].setCirList(1);	
	if( label_num %2 == 1 ) _houghHitList[label_fist+label_num/2+id+1].setCirList(1);	
  }

  return nHitTruthAdd;
}

void HoughHitList::printAll(){
  print();
  for(std::vector<HoughHit>::iterator iter = _houghHitList.begin();iter!= _houghHitList.end(); iter++){
	(*iter).printAll();
  }
  std::cout<<std::endl;
}
void HoughHitList::printAll(int){
  print();
  for(std::vector<HoughHit>::iterator iter = _houghHitList.begin();iter!= _houghHitList.end(); iter++){
	(*iter).printTruth();
  }
  std::cout<<std::endl;
}

void HoughHitList::print(){
  std::cout<<"MdcHoughFinder hit list: nHit="<<_houghHitList.size()<<std::endl;//<<" type "<<type()<<std::endl;
}

// get hit list
const std::vector<HoughHit>& HoughHitList::getList() const{
  return _houghHitList;
}
std::vector<HoughHit>& HoughHitList::getList() {
  return _houghHitList;
}
int HoughHitList::getHitNum(int select ) const{
  int size=_houghHitList.size();
  int n0,n1,n2,n3,n4,n5,n6;
  n0=n1=n2=n3=n4=n5=n6=0;
  for(int i=0;i<size;i++){
	int cir=   _houghHitList[i].getCirList();
	int index = _houghHitList[i].digi()->getTrackIndex();
	int style=  _houghHitList[i].getStyle();
	n0++;
	if( style == 1 ) n1++;
	if( style == 2 ) n2++;
	if( index < 0 )  n3++;
	if( index >=0 && cir<=1 && style==0)  n4++;
	if( index <0 || cir>1 )   n5++;
	if( index >=0 && cir==0 && style==0)  n6++;
  }
  if( select == 0 ) return n0;
  else if( select == 1 ) return n1;
  else if( select == 2 ) return n2;
  else if( select == 3 ) return n3;
  else if( select == 4 ) return n4;
  else if( select == 5 ) return n5;
  else if( select == 6 ) return n6;
  else return -999;
}
int HoughHitList::getHitNumA(int select ) const{
  int size=_houghHitList.size();
  int n0,n1,n2,n3,n4,n5,n6;
  n0=n1=n2=n3=n4=n5=n6=0;
  for(int i=0;i<size;i++){
	int cir=   _houghHitList[i].getCirList();
	int index = _houghHitList[i].digi()->getTrackIndex();
	int type =  _houghHitList[i].getSlayerType();
	int style=  _houghHitList[i].getStyle();
	if( type ==0 ) n0++;
	if( type==0 && style == 1 ) n1++;
	if( type==0 && style == 2 ) n2++;
	if( type==0 && index < 0 )  n3++;
	if( type==0 && index >=0 && cir<=1 && style ==0)  n4++;
	if( type==0 && (index <0 || cir>1) )   n5++;
	if( type==0 && index >=0 && cir==0 && style ==0)  n6++;
  }
  if ( select == 0 ) return n0;
  else if ( select == 1 ) return n1;
  else if ( select == 2 ) return n2;
  else if ( select == 3 ) return n3;
  else if ( select == 4 ) return n4;
  else if ( select == 5 ) return n5;
  else if ( select == 6 ) return n6;
  else return -999;
}
int HoughHitList::getHitNumS(int select ) const{
  int size=_houghHitList.size();
  int n0,n1,n2,n3,n4,n5,n6;
  n0=n1=n2=n3=n4=n5=n6=0;
  for(int i=0;i<size;i++){
	int cir=   _houghHitList[i].getCirList();
	int index = _houghHitList[i].digi()->getTrackIndex();
	int type =  _houghHitList[i].getSlayerType();
	int style=  _houghHitList[i].getStyle();
	if( type !=0 ) n0++;
	if( type!=0 && style == 1 ) n1++;
	if( type!=0 && style == 2 ) n2++;
	if( type!=0 && index < 0 )  n3++;
	if( type!=0 && index >=0 && cir<=1 && style==0)  n4++;
	if( type!=0 && (index <0 || cir>1) )   n5++;
	if( type!=0 && index >=0 && cir==0 && style==0)  n6++;
  }
  if( select == 0 ) return n0;
  else if( select == 1 ) return n1;
  else if( select == 2 ) return n2;
  else if( select == 3 ) return n3;
  else if( select == 4 ) return n4;
  else if( select == 5 ) return n5;
  else if( select == 6 ) return n6;
  else return -999;

}
