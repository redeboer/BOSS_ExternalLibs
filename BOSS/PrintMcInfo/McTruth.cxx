#include "PrintMcInfo/PrintMcInfo.h"
using namespace std;

void PrintMcInfo::mkmap()
{
	string str,str1,str2,str3,str4;
	int id;
	
	char* pdt_path=getenv("BESEVTGENROOT");
	if(0 == pdt_path)
		std::cerr<<"ERROR : Can't get env $BESEVTGENROOT"<<std::endl;
	std::string pdtname=std::string(pdt_path) + "/share/pdt.table";
	ifstream fin(pdtname.c_str());
	
	while(getline(fin,str))
	{
		fin>>str;
		if(str == "add")
		{
			fin>>str1>>str2>>str3>>str4;
			id = atoi(str4.c_str());
			map_pid.insert(map<int, string>::value_type(id,str3));
		}
	}
}

void PrintMcInfo::printTitle(ofstream& os,int m_OutputLevel)
{
	if(m_OutputLevel==1)
	{
		os.setf(ios::left);
		os<<"  "<<setw(27)<<"cosTeta  phi"<<setw(50)<<"P4 : momentum Energy"
			<<setw(10)<<" "<<setw(40)<<"initialPosition"
			<<setw(15)<<"finalPosition"<<endl;
		os.setf(ios::left);
		for (int m = 0; m < 29; m++) {os<< " ";}
		os<<"Px, Py, Pz , E (Gev)";
		for (int m = 0; m < 35; m++) {os<< " ";}
		os<<"x(cm) y(cm) z(cm)";
		for (int m = 0; m < 25; m++) {os<< " ";}
		os  <<"x(cm) y(cm) z(cm)"<<endl;
	}
	/*	if(m_OutputLevel==2)  
		{
		os<<'\t'<<"MdcHit"<<'\t'<<'\t'<<"TofHit"<<'\t'<<'\t'<<'\t'<<"EmcHit"<<'\t'<<'\t'<<'\t'<<"MucHit"
		<<endl;
		os<<'\t'<<"(layer wire)"<<"  "<<"(b/ec layer phi_module)"<<"  "<<"(b/ec theta phi)"<<"  "<<"(b/ec segment layer strip)"<<endl;
		}
	 */
}




void PrintMcInfo::printTree(ofstream& os,Event::McParticle* pmcp,int m_OutputLevel,int tab=0)
{	  
	bool decay = (pmcp->daughterList()).size()==0?false:true;
	if(decay)
	{
		for (int m = 0; m < tab; m++) {os<< '\t';}

		m_pid = (*pmcp).particleProperty();
		m_trkIndex = (*pmcp).trackIndex();
		map<int,string>::iterator iterPar;
		iterPar = map_pid.find(m_pid);
		if (iterPar!=map_pid.end())
			os<<iterPar->second<<"["<<m_trkIndex<<"]"<<" "<<"->";

		//loop: to get the daughters of the decayed particle
		SmartRefVector<Event::McParticle>::const_iterator begin = (pmcp->daughterList()).begin();
		SmartRefVector<Event::McParticle>::const_iterator end = (pmcp->daughterList()).end();
		SmartRefVector<Event::McParticle>::const_iterator it;
		for(it = begin;it != end;it++)
		{      
			m_trkIndex = (*it)->trackIndex();
			map<int,string>::iterator iter;
			iter = map_pid.find((*it)->particleProperty());
			if(iter!=map_pid.end())
			{		
				daughters = iter->second;
				m_trkIndex = (*it)->trackIndex();
				os.setf(ios::left);
				os<<daughters<<"["<<m_trkIndex<<"]"<<" ";
				daughters.erase();
			}
			else
				cout<<"can not find the daughter particle in pdt_table"<<endl;
		}
		os<<endl;

		for(it = begin;it != end;it++)
		{	
			SmartRef<Event::McParticle> pdMc = (*it);
			printTree(os,pdMc,m_OutputLevel,tab+1);
		}
	}
	if((!decay)&&(pmcp->primaryParticle()))
	{
		m_pid = (*pmcp).particleProperty();
		map<int,string>::iterator iterPar;
		iterPar = map_pid.find(m_pid);
		if (iterPar!=map_pid.end())
		os<<endl
		  <<"********************************************************"<<endl
	          <<" in this tree, there's only one primary particle  : "<<iterPar->second<<endl
		  <<"********************************************************"<<endl;
	}
}




void PrintMcInfo::printPartInf(ofstream& os,Event::McParticle* pMc,int m_OutputLevel,int tab=0)
{
	bool decay = (pMc->daughterList()).size()==0?false:true;

	SmartDataPtr<Event::MdcMcHitCol> mdcMcHitCol(eventSvc(), "/Event/MC/MdcMcHitCol");
	SmartDataPtr<Event::TofMcHitCol> tofMcHitCol(eventSvc(), "/Event/MC/TofMcHitCol");
	SmartDataPtr<Event::EmcMcHitCol> emcMcHitCol(eventSvc(), "/Event/MC/EmcMcHitCol");
	SmartDataPtr<Event::MucMcHitCol> mucMcHitCol(eventSvc(), "/Event/MC/MucMcHitCol");

	if((!decay)&&(pMc->primaryParticle()))
	{
		os.setf(ios::left);
		map<int,string>::iterator iter_map;
		iter_map = map_pid.find(pMc->particleProperty());
		if(iter_map!=map_pid.end())
		{	
			if(m_OutputLevel==1)
			{
				string name  = iter_map->second;
				os<<"["<<pMc->trackIndex()<<"]"<<name<<endl;
				HepLorentzVector p4 = (pMc)->initialFourMomentum();
				os<<"  "<<"["<<setw(12)<<p4.vect().cosTheta()<<setw(12)<<p4.vect().phi()<<"]  ";
				os<<"["<<setw(12)<<p4.x()<<setw(12)<<p4.y()
					<<setw(12)<<p4.z()<<" "<<setw(10)<<p4.t()<<"]  ";

				HepLorentzVector ipst = pMc->initialPosition();
				os<<"["<<setw(12)<<ipst.x();
				os<<setw(12)<<ipst.y();
				os<<setw(10)<<ipst.z()<<"]  ";

				HepLorentzVector fpst = (pMc)->finalPosition();
				os<<"["<<setw(12)<<fpst.x();
				os<<setw(12)<<fpst.y();
				os<<setw(10)<<fpst.z()<<"]  ";
				os<<endl;
			}
			if(m_OutputLevel==2)
			{
				int trkIdx = pMc->trackIndex();
				PrintMcInfo::printHit(os,mdcMcHitCol,tofMcHitCol,emcMcHitCol,mucMcHitCol,trkIdx);
			}

		}
	}
	if(decay)
	{
		SmartRefVector<Event::McParticle>::const_iterator begin = (pMc->daughterList()).begin();
		SmartRefVector<Event::McParticle>::const_iterator end = (pMc->daughterList()).end();
		SmartRefVector<Event::McParticle>::const_iterator it;
		for(it = begin;it != end;it++)
		{	
			m_trkIndex = (*it)->trackIndex();
			map<int,string>::iterator iter;
			iter = map_pid.find((*it)->particleProperty());
			if(iter!=map_pid.end())
			{	
				daughters = iter->second;
				if(m_OutputLevel ==1)
				{	
					os.setf(ios::left);
					os<<"["<<m_trkIndex<<"]"<<setw(20)<<daughters<<endl;

					HepLorentzVector p4 = (*it)->initialFourMomentum();
					os<<"  "<<"["<<setw(12)<<p4.vect().cosTheta()<<setw(12)<<p4.vect().phi()<<"]  ";
					os<<"["<<setw(12)<<p4.x()<<setw(12)<<p4.y()
						<<setw(12)<<p4.z()<<" "<<setw(10)<<p4.t()<<"]  ";

					//os<<"["<<setw(4)<<(*it)->vertexIndex0();
					HepLorentzVector ipst = (*it)->initialPosition();
					os<<"["<<setw(12)<<ipst.x();
					os<<setw(12)<<ipst.y();
					os<<setw(10)<<ipst.z()<<"]  ";
					//os<<setw(8)<<ipst.t()<<"]  ";					

					//os<<"["<<setw(4)<<(*it)->vertexIndex1();
					HepLorentzVector fpst = (*it)->finalPosition();
					os<<"["<<setw(12)<<fpst.x();
					os<<setw(12)<<fpst.y();
					os<<setw(10)<<fpst.z()<<"]  ";
					//os<<setw(8)<<fpst.t()<<"]";
				}
				if(m_OutputLevel==2)
				{	
					if(((*mdcMcHitCol).size()==0&&(*tofMcHitCol).size()==0&&(*emcMcHitCol).size()==0&&(*mucMcHitCol).size()==0)&&tab==0)
						os<<" --------------------------------------------------------"<<endl
							<<" mdcMcHitCol,tofMcHitCol,emcMcHitCol,mucMcHitCol all empty"<<endl
							<<" --------------------------------------------------------"<<endl;
					if(!((*mdcMcHitCol).size()==0&&(*tofMcHitCol).size()==0&&(*emcMcHitCol).size()==0&&(*mucMcHitCol).size()==0))

					{
						os.setf(ios::left);
						os<<"["<<m_trkIndex<<"]"<<setw(12)<<daughters<<endl;
					}

					int trkIdx = (*it)->trackIndex();
					PrintMcInfo::printHit(os,mdcMcHitCol,tofMcHitCol,emcMcHitCol,mucMcHitCol,trkIdx);
				}
				os<<endl;
			}
			else cout<<"can not find the daughter particle in pdt_table"<<endl;
		}
		daughters.erase();

		for(it = begin;it != end;it++)
		{
			SmartRef<Event::McParticle> pdMc = (*it);
			printPartInf(os,pdMc,m_OutputLevel,tab+1);
		}
	}
}



void PrintMcInfo::printHit(ofstream& os,Event::MdcMcHitCol& mdcCol,Event::TofMcHitCol& tofCol,Event::EmcMcHitCol& emcCol,Event::MucMcHitCol& mucCol,int& trk_Idx)
{
	if(!(mdcCol.size()==0&&tofCol.size()==0&&emcCol.size()==0&&mucCol.size()==0))
	{
		os<< '\t' <<setw(66)<<"MdcHit"
			<<setw(25)<<"TofHit"
			<<setw(17)<<"EmcHit"
			<<"MucHit"<<endl;
		os<< '\t' <<"(layer, wire, position(x,y,z)(mm) ,drift_d(mm), DeDx(MeV/m))"<<"  "<<"(B/EC layer phi_module)"<<"  "<<"(B/EC theta phi)"<<"  "<<"(B/EC segment layer strip)"<<endl;
	}
	Event::MdcMcHitCol::const_iterator it_mdc = mdcCol.begin();
	Event::TofMcHitCol::const_iterator it_tof = tofCol.begin();
	Event::EmcMcHitCol::const_iterator it_emc = emcCol.begin();
	Event::MucMcHitCol::const_iterator it_muc = mucCol.begin();
	vector<Event::MdcMcHitCol::const_iterator> vmdc;
	vector<Event::TofMcHitCol::const_iterator> vtof;
	vector<Event::EmcMcHitCol::const_iterator> vemc;
	vector<Event::MucMcHitCol::const_iterator> vmuc;
	if(mdcCol.size() != 0)
	{	
		for(;it_mdc != mdcCol.end();it_mdc++)
		{	
			int trkIndexmdc = (*it_mdc)->getTrackIndex();
			if(trkIndexmdc == trk_Idx)   
			{  
				vmdc.push_back(it_mdc);
			}
		}
	}
	if(tofCol.size() != 0)
	{
		for(;it_tof != tofCol.end();it_tof++)
		{	
			int trkIndextof = (*it_tof)->getTrackIndex();
			if(trkIndextof==trk_Idx)      vtof.push_back(it_tof);
		}
	}

	if(emcCol.size() != 0)
	{
		for(;it_emc != emcCol.end();it_emc++)
		{
			int trkIndexemc = (*it_emc)->getTrackIndex();
			if(trkIndexemc==trk_Idx)      vemc.push_back(it_emc);
		}
	}

	if(mucCol.size() != 0)
	{
		for(;it_muc != mucCol.end();it_muc++)
		{
			int trkIndexmuc = (*it_muc)->getTrackIndex();
			if(trkIndexmuc==trk_Idx)      vmuc.push_back(it_muc);
		}
	}


	for(int i=0;;i++)
	{	
		bool bmdc=(i>vmdc.size())?true:false;
		bool btof=(i>vtof.size())?true:false;
		bool bemc=(i>vemc.size())?true:false;
		bool bmuc=(i>vmuc.size())?true:false;	
		if((i>=vmdc.size())&&(i>=vtof.size())&&(i>=vemc.size())&&(i>=vmuc.size())) break;
		os<< '\t' ;
		if(vmdc.size()>0)
		{
			if(i<vmdc.size())
			{
				const Identifier ident_mdc = (*vmdc[i])->identify();
				os<<setw(5)<<MdcID::layer(ident_mdc);
				os<<setw(5)<<MdcID::wire(ident_mdc);
				os<<"[ ";
				os<<setw(10)<<(*vmdc[i])->getPositionX();
				os<<setw(10)<<(*vmdc[i])->getPositionY();
				os<<setw(10)<<(*vmdc[i])->getPositionZ();
				os<<"] ";
				os<<setw(10)<<(*vmdc[i])->getDriftDistance();
				os<<setw(10)<<(*vmdc[i])->getDepositEnergy();
			}else{for (int m = 0; m < 64; m++)  os<<" ";}
		}else{for (int m = 0; m < 64; m++)  os<<" ";}


		if(vtof.size()>0)
		{
			if(i<vtof.size())
			{
				const Identifier ident_tof =(*vtof[i])->identify();
				if(TofID::barrel_ec(ident_tof)==1){os<<"B  "<<"  ";}
				if(TofID::barrel_ec(ident_tof)==0){os<<"E_E"<<"  ";}
				if(TofID::barrel_ec(ident_tof)==2){os<<"W_E"<<"  ";}
				os<<setw(3)<<TofID::layer(ident_tof);
				os<<setw(17)<<TofID::phi_module(ident_tof);
			}else{for (int m = 0; m < 25; m++)  os<<" ";}
		}else{for (int m = 0; m < 25; m++)  os<<" ";}


		if(vemc.size()>0)
		{
			if(i<vemc.size())
			{
				const Identifier ident_emc = (*vemc[i])->identify();
				if(EmcID::barrel_ec(ident_emc)==1){os<<"B  "<<"  ";}
				if(EmcID::barrel_ec(ident_emc)==0){os<<"E_E"<<"  ";}
				if(EmcID::barrel_ec(ident_emc)==2){os<<"W_E"<<"  ";}
				os<<setw(5)<<EmcID::theta_module(ident_emc);
				os<<setw(9)<<EmcID::phi_module(ident_emc);
			}else{for (int m = 0; m < 19; m++)  os<<" ";}
		}else{for (int m = 0; m < 19; m++)  os<<" ";}

		if(vmuc.size()>0)
		{
			if(i<vmuc.size())
			{
				const Identifier ident_muc = (*vmuc[i])->identify();
				if(MucID::barrel_ec(ident_muc)==1){os<<"B  "<<"  ";}
				if(MucID::barrel_ec(ident_muc)==0){os<<"E_E"<<"  ";}
				if(MucID::barrel_ec(ident_muc)==2){os<<"W_E"<<"  ";}
				os<<setw(3)<<MucID::segment(ident_muc);
				os<<setw(3)<<MucID::layer(ident_muc);
				os<<setw(4)<<MucID::strip(ident_muc);
				os<<endl;
			}else{ os<<endl;}
		}else{os<<endl;}


		//i++;
	}
	vmdc.clear();
	vtof.clear();
	vemc.clear();
	vmuc.clear();

}

