#include "PrintMcInfo/PrintMcInfo.h"
using namespace std;


void PrintMcInfo::printDigi(ofstream& os)
{

	os<< '\t' <<setw(40)<<"MdcDigi"<<setw(30)<<"TofDigi"<<setw(29)<<"EmcDigi"<<"MucDig"<<endl;
	os<< '\t' <<"(layer , wire, trkIndex)"<<"   "<<"(B/EC, layer, phi_module, trkIndex)"<<"  "<<"(B/EC, theta, phi, trkIndex)"<<"  "<<"(B/EC, segment, layer, strip, trkIndex)"<<endl;


	SmartDataPtr<MdcDigiCol> mdcCol(eventSvc(),"/Event/Digi/MdcDigiCol");
	SmartDataPtr<TofDigiCol> tofCol(eventSvc(),"/Event/Digi/TofDigiCol");
	SmartDataPtr<EmcDigiCol> emcCol(eventSvc(),"/Event/Digi/EmcDigiCol");
	SmartDataPtr<MucDigiCol> mucCol(eventSvc(),"/Event/Digi/MucDigiCol");

	MdcDigiCol::const_iterator it_mdc = mdcCol->begin();
	TofDigiCol::const_iterator it_tof = tofCol->begin();
	EmcDigiCol::const_iterator it_emc = emcCol->begin();
	MucDigiCol::const_iterator it_muc = mucCol->begin();

	vector<MdcDigiCol::const_iterator> vmdc;
	vector<TofDigiCol::const_iterator> vtof;
	vector<EmcDigiCol::const_iterator> vemc;
	vector<MucDigiCol::const_iterator> vmuc;


	if(mdcCol->size() != 0)
	{	
		for(;it_mdc != mdcCol->end();it_mdc++)
		{	
			vmdc.push_back(it_mdc);
		}
	}
	if(tofCol->size() != 0)
	{
		for(;it_tof != tofCol->end();it_tof++)
		{	
			vtof.push_back(it_tof);
		}
	}

	if(emcCol->size() != 0)
	{
		for(;it_emc != emcCol->end();it_emc++)
		{
			vemc.push_back(it_emc);
		}
	}

	if(mucCol->size() != 0)
	{
		for(;it_muc != mucCol->end();it_muc++)
		{
			vmuc.push_back(it_muc);
		}
	}


	for(int i=0;;i++)
	{	
		bool bmdc=(i>vmdc.size())?true:false;
		bool btof=(i>vtof.size())?true:false;
		bool bemc=(i>vemc.size())?true:false;
		bool bmuc=(i>vmuc.size())?true:false;	
		if((i>=vmdc.size())&&(i>=vtof.size())&&(i>=vemc.size())&&(i>=vmuc.size())) break;
		for (int m = 0; m < 8; m++)  os<<" ";
		if(vmdc.size()>0)
		{
			if(i<vmdc.size())
			{
				const Identifier ident_mdc = (*vmdc[i])->identify();
				os<<setw(6)<<MdcID::layer(ident_mdc);
				os<<setw(6)<<MdcID::wire(ident_mdc);
				os<<setw(15)<<(*vmdc[i])->getTrackIndex();
			}else{for (int m = 0; m < 27; m++)  os<<" ";}
		}else{for (int m = 0; m < 27; m++)  os<<" ";}


		if(vtof.size()>0)
		{
			if(i<vtof.size())
			{
				const Identifier ident_tof =(*vtof[i])->identify();
				if(TofID::barrel_ec(ident_tof)==1){os<<"B  "<<"  ";}
				if(TofID::barrel_ec(ident_tof)==0){os<<"E_E"<<"  ";}
				if(TofID::barrel_ec(ident_tof)==2){os<<"W_E"<<"  ";}
				os<<setw(6)<<TofID::layer(ident_tof);
				os<<setw(6)<<TofID::phi_module(ident_tof);
				os<<setw(22)<<(*vtof[i])->getTrackIndex();
			}else{for (int m = 0; m < 39; m++)  os<<" ";}
		}else{for (int m = 0; m < 39; m++)  os<<" ";}


		if(vemc.size()>0)
		{
			if(i<vemc.size())
			{
				const Identifier ident_emc = (*vemc[i])->identify();
				if(EmcID::barrel_ec(ident_emc)==1){os<<"B  "<<"  ";}
				if(EmcID::barrel_ec(ident_emc)==0){os<<"E_E"<<"  ";}
				if(EmcID::barrel_ec(ident_emc)==2){os<<"W_E"<<"  ";}
				os<<setw(6)<<EmcID::theta_module(ident_emc);
				os<<setw(6)<<EmcID::phi_module(ident_emc);
				os<<setw(12)<<(*vemc[i])->getTrackIndex();
			}else{for (int m = 0; m < 29; m++)  os<<" ";}
		}else{for (int m = 0; m < 29; m++)  os<<" ";}

		if(vmuc.size()>0)
		{
			if(i<vmuc.size())
			{
				const Identifier ident_muc = (*vmuc[i])->identify();
				if(MucID::barrel_ec(ident_muc)==1){os<<"B  "<<"  ";}
				if(MucID::barrel_ec(ident_muc)==0){os<<"E_E"<<"  ";}
				if(MucID::barrel_ec(ident_muc)==2){os<<"W_E"<<"  ";}
				os<<setw(6)<<MucID::segment(ident_muc);
				os<<setw(6)<<MucID::layer(ident_muc);
				os<<setw(6)<<MucID::strip(ident_muc);
				os<<setw(6)<<(*vmuc[i])->getTrackIndex();
				os<<endl;
			}else{ os<<endl;}
		}else{os<<endl;}

	}
	vmdc.clear();
	vtof.clear();
	vemc.clear();
	vmuc.clear();
}
