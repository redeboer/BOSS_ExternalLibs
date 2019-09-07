#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "TH1F.h"
#include "TFile.h"
#include "db_mysql.h"
#include "TF1.h"
using namespace std;
db_mysql sqlonline;
db_mysql sqloffline;
int runNo;
char* offline_user;
char* offline_passwd;
char* file;
const char* server_ip = "bes3db1.ihep.ac.cn";
char* Creator=getenv("USER");  // TODO: set the current creator
char* SftVer=getenv("BES_RELEASE");  // TODO: set the current boss version
char* ParVer;                  // //set soft version for Boss Version
char* LumTau_txt;                  // FOR LumTau
bool ISNeedReFit=0;
bool ISNeedReFitX=0;
bool ISNeedReFitY=0;
bool ISNeedReFitZ=0;
int main(int argc, char* argv[]){
	//define:
	int WriteLumDB();
	int WriteBeamParToDb();
	int ReWriteBeamParToDb();	
	int ReadFromDB();
	int WriteLumTauDB();
	//Execute:
	offline_user=argv[1];
	offline_passwd=argv[2];
	char* runNo_temp=argv[3];
	runNo=atoi(runNo_temp);
	ParVer=argv[4];
	file=argv[5];
	LumTau_txt = argv[6];
	std::cout<<"-----------------------------------"<<runNo<<"-------------------------"<<std::endl;
	sqloffline.Init(server_ip,offline_user,offline_passwd);
	sqloffline.Connect();
	sqloffline.SelectDB("offlinedb");

	WriteLumDB();
	WriteBeamParToDb();
	if(ISNeedReFit){ReWriteBeamParToDb();}
	ReadFromDB();
	WriteLumTauDB();

	sqloffline.Close();

	return 1;
}

int WriteLumDB(){
	char Notes[]="";
	TFile f(file);
	double LumValue;
	TH1F* lum;
	f.GetObject("/zhsLUM/lum",lum);
	LumValue=lum->GetBinContent(3);
	char sql_input[350];
	sqlonline.Init("202.122.33.123","guest","guestpass");
	sqlonline.Connect();
	sqlonline.SelectDB("RunInfo");
	sprintf(sql_input,"select IntLumEndcapEE from OnlineLum where run_number='%d'",runNo);
	sqlonline.Execute(sql_input);
	std::string intLumEndcapEE;
	if(sqlonline.GetRow(0)){
		intLumEndcapEE=sqlonline.GetField(0);
	}
	else intLumEndcapEE="-0";
	sqlonline.Close();

	//LumValue ,intLumEndCapEE write to database;

	sprintf(sql_input,"delete from OfflineLum where RunNo=%d && SftVer='%s' && ParVer='%s'",runNo,SftVer,ParVer);
	sqloffline.Execute(sql_input);

	sprintf(sql_input,"insert into OfflineLum(SerNo,RunNo,IntLumEndcapEE,OfflineTwoGam,SftVer,ParVer,Creator,CreateTime,Notes) values('','%d','%s','%f','%s','%s','%s',now(),'%s')",runNo,intLumEndcapEE.c_str(),LumValue,SftVer,ParVer,Creator,Notes);
	sqloffline.Execute(sql_input);
	return 1;
}

int WriteBeamParToDb(){
	TFile f(file);
	TH1D* x_of_vertex_in_kal;
	TH1D* y_of_vertex_in_kal;
	TH1D* z_of_vertex_in_kal;
	f.GetObject("/zhsVER/x_of_vertex_in_kal",x_of_vertex_in_kal);
	f.GetObject("/zhsVER/y_of_vertex_in_kal",y_of_vertex_in_kal);
	f.GetObject("/zhsVER/z_of_vertex_in_kal",z_of_vertex_in_kal);
	TF1 *func;
	TF1 *funcY;
	TF1 *funcZ;
	if(runNo<=10878 || runNo>20433){ //jpsi,psip
		func = new TF1("func", "gaus", -0.6, 0.6);
		funcY = new TF1("funcY", "gaus", -0.6, 0.2);
		funcZ = new TF1("funcZ", "gaus", -6, 6);
	}
	else if(runNo >10878 && runNo<= 20433){//psipp
		func = new TF1("func", "gaus", 0., 1.8);
		funcY = new TF1("funcY", "gaus", -1., 1.);
		funcZ = new TF1("funcZ", "gaus", -6, 6);
	}
	x_of_vertex_in_kal->Fit("func", "RQ");
	Double_t MeanXKal = func->GetParameter(1);
	Double_t SigmaXKal = func->GetParameter(2);

	y_of_vertex_in_kal->Fit("funcY", "RQ");
	Double_t MeanYKal = funcY->GetParameter(1);
	Double_t SigmaYKal = funcY->GetParameter(2);

	z_of_vertex_in_kal->Fit("funcZ", "RQ");
	Double_t MeanZKal = funcZ->GetParameter(1);
	Double_t SigmaZKal = funcZ->GetParameter(2); 
	if(runNo<=10878 || runNo>20433){
		if(MeanXKal<-0.05 || MeanXKal>0.25 || SigmaXKal>0.12 || SigmaXKal<0.){
			std::cout<<"--------------x_of_vertex_in_kal-------out of range--runID is:"<<runNo<<std::endl;
			ISNeedReFitX=1;ISNeedReFit=1;
		}
		if(MeanYKal<-0.4 || MeanYKal>0 || SigmaYKal>0.12 || SigmaYKal<0)
		{
			std::cout<<"--------------y_of_vertex_in_kal-------out of range--runID is:"<<runNo<<std::endl;
			ISNeedReFitY=1;ISNeedReFit=1;
		}
		if(MeanZKal<-1 || MeanZKal>1.0 || SigmaZKal>1.5 || SigmaZKal<0)
		{
			std::cout<<"--------------z_of_vertex_in_kal-------out of range--runID is:"<<runNo<<std::endl;
			ISNeedReFitZ=1;ISNeedReFit=1;
		}
	}
	else{
		if(MeanXKal<0.6 || MeanXKal>1.2 || SigmaXKal>0.18 || SigmaXKal<0.){
			std::cout<<"--------------x_of_vertex_in_kal-------out of range--runID is:"<<runNo<<std::endl;
			ISNeedReFitX=1;ISNeedReFit=1;
		}
		if(MeanYKal<-0.4 || MeanYKal>0 || SigmaYKal>0.12 || SigmaYKal<0.)
		{
			std::cout<<"--------------y_of_vertex_in_kal-------out of range--runID is:"<<runNo<<std::endl;
			ISNeedReFitY=1;ISNeedReFit=1;
		}
		if(MeanZKal<-0.8 || MeanZKal>0.8 || SigmaZKal>1.5 || SigmaZKal<0.)
		{
			std::cout<<"--------------z_of_vertex_in_kal-------out of range--runID is:"<<runNo<<std::endl;
			ISNeedReFitZ=1;ISNeedReFit=1;
		}

	}
	char VerFitFunc[] = "Kalman"; 
	char DistFitFunc[] = "single Gaussian";

	double AcSigmaVx, AcSigmaVy;

	AcSigmaVx = 0.038;
	AcSigmaVy = 0.00057;
	char query[1024*100];
	
	sprintf(query, "DELETE FROM BeamPar where RunNo = '%d' && SftVer='%s' && ParVer='%s'", runNo,SftVer,ParVer);
	sqloffline.Execute(query);
	sprintf(query,"INSERT INTO BeamPar(BeamParSerNo,RunNo,Vx,Vy,Vz,SigmaVx,SigmaVy,SigmaVz,VerFitFunc,DistFitFunc,SftVer,Creator,AcSigmaVx,AcSigmaVy,ParVer) values('','%d','%f','%f','%f','%f','%f','%f','%s','%s','%s','%s','%f','%f','%s')",runNo,MeanXKal,MeanYKal,MeanZKal,SigmaXKal,SigmaYKal,SigmaZKal,VerFitFunc,DistFitFunc,SftVer,Creator,AcSigmaVx,AcSigmaVy,ParVer);        
	sqloffline.Execute(query);
	
	return 1;

}
int ReWriteBeamParToDb(){
	char query[1024*100];
	int irun=1;
	std::string Vx;
	std::string Vy;
	std::string Vz;
	std::string SigmaVx;
	std::string SigmaVy;
	std::string SigmaVz;
	bool Runloop=0;
	do{
		sprintf(query,"select Vx,Vy,Vz,SigmaVx,SigmaVy,SigmaVz,RunNo,SftVer,ParVer from BeamPar where RunNo='%d' && SftVer='%s' && ParVer='%s'",runNo-irun,SftVer,ParVer);
		sqloffline.Execute(query);
		if(sqloffline.GetRow(0)){
			Vx=sqloffline.GetField(0);
			Vy=sqloffline.GetField(1);
			Vz=sqloffline.GetField(2);
			SigmaVx=sqloffline.GetField(3);
			SigmaVy=sqloffline.GetField(4);
			SigmaVz=sqloffline.GetField(5);
			Runloop=0;
		}
		else Runloop=1;
		irun++;
	}while(Runloop);
	if(ISNeedReFitX){
		sprintf(query,"update BeamPar set Vx='%s',SigmaVx='%s' where RunNo='%d' && SftVer='%s' && ParVer='%s'",Vx.c_str(),SigmaVx.c_str(),runNo,SftVer,ParVer);
		sqloffline.Execute(query);
		//   std::cout<<query<<std::endl;
		std::cout<<"------------------update Vx,SigmaVx successful!--------"<<std::endl;
	}
	if(ISNeedReFitY){
		sprintf(query,"update BeamPar set Vy='%s',SigmaVy='%s' where RunNo='%d' && SftVer='%s' && ParVer='%s'",Vy.c_str(),SigmaVy.c_str(),runNo,SftVer,ParVer);
		sqloffline.Execute(query);
		// std::cout<<query<<std::endl;
		std::cout<<"------------------update Vy,SigmaVy successful!---------"<<std::endl;
	}
	if(ISNeedReFitZ){
		sprintf(query,"update BeamPar set Vz='%s',SigmaVz='%s' where RunNo='%d' && SftVer='%s' && ParVer='%s'",Vz.c_str(),SigmaVz.c_str(),runNo,SftVer,ParVer);
		sqloffline.Execute(query);
		//std::cout<<query<<std::endl;
		std::cout<<"------------------update Vz,SigmaVz successful!---------"<<std::endl;
	}


	return 1;
}
int ReadFromDB(){
	std::cout<<"--------------check RUN LUM and Vertex info -----------------------------------"<<std::endl;
	char sql_input[550];
	sprintf(sql_input,"select SerNo,RunNo,IntLumEndcapEE,OfflineTwoGam,SftVer,ParVer,Creator,CreateTime,Notes from OfflineLum where RunNo=%d && SftVer='%s' && ParVer='%s'",runNo,SftVer,ParVer);
	sqloffline.Execute(sql_input);
	std::cout<<"LUM is:"<<"SerNo,RunNo,IntLumEndcapEE,OfflineTwoGam,SftVer,ParVer,Creator,CreateTime,Notes"<<std::endl;
	if(sqloffline.GetRow(0)){
		std::cout<<sqloffline.GetField(0)<<","<<sqloffline.GetField(1)<<","<<sqloffline.GetField(2)<<","<<sqloffline.GetField(3)<<","
			<<sqloffline.GetField(4)<<","<<sqloffline.GetField(5)<<","<<sqloffline.GetField(6)<<","<<sqloffline.GetField(7)<<","
			<<sqloffline.GetField(8)<<std::endl;
		std::cout<<"---------------------------------------------------------------LUM is OK!"<<std::endl;
	}
	else std::cout<<"-----------------------------------------ERROR to read LUM from OfflineLum db"<<std::endl;

	sprintf(sql_input, "select BeamParSerNo,RunNo,Vx,Vy,Vz,SigmaVx,SigmaVy,SigmaVz,VerFitFunc,DistFitFunc,SftVer,Creator,AcSigmaVx,AcSigmaVy,ParVer FROM BeamPar where RunNo ='%d' && SftVer='%s' && ParVer='%s'", runNo,SftVer,ParVer);
	sqloffline.Execute(sql_input);
	std::cout<<"BeamPar is:"<<"BeamParSerNo,RunNo,Vx,Vy,Vz,SigmaVx,SigmaVy,SigmaVz,VerFitFunc,DistFitFunc,SftVer,Creator,AcSigmaVx,AcSigmaVy,ParVer"<<std::endl;
	if(sqloffline.GetRow(0)){
		std::cout<<sqloffline.GetField(0)<<","<<sqloffline.GetField(1)<<","<<sqloffline.GetField(2)<<","<<sqloffline.GetField(3)<<","
			<<sqloffline.GetField(4)<<","<<sqloffline.GetField(5)<<","<<sqloffline.GetField(6)<<","<<sqloffline.GetField(7)<<","
			<<sqloffline.GetField(8)<<","<<sqloffline.GetField(9)<<","<<sqloffline.GetField(10)<<","<<sqloffline.GetField(11)<<","
			<<sqloffline.GetField(12)<<","<<sqloffline.GetField(13)<<","<<sqloffline.GetField(14)<<std::endl;
		std::cout<<"-------------------------------------------------------BeamPar is OK!"<<std::endl;
	}
	else std::cout<<"-----------------------------------------ERROR to read Vertex  from BeamPar db"<<std::endl;
	return 1;
}

int WriteLumTauDB(){
	ifstream rwite;
	rwite.open(LumTau_txt);
	bool file_open = 1;
	if(!rwite){
		cout<<"************************************"<<endl;
		cout<<LumTau_txt<<" not exist!"<<endl;
		cout<<"************************************"<<endl;
		file_open = 0;
	}
	if(!file_open) return -1;

	int m_runNo = -1;
	double runTime = -1;
	double Lum_start = -1;
	double Lum_end = -1;
	double Tau_Value = -1;

	while(!rwite.eof()){
		rwite>>m_runNo>>runTime>>Lum_start>>Lum_end>>Tau_Value;
		if( Tau_Value<0 )
		    Tau_Value = 99999;	
		char sql_input[350];
		sprintf(sql_input,"update OfflineLum set runTime=%f, Lum_start=%f, Lum_end=%f, Tau_Value=%f where runNo=%d && SftVer='%s' && ParVer='%s' ",runTime,Lum_start,Lum_end,Tau_Value,m_runNo,SftVer,ParVer);
		sqloffline.Execute(sql_input);

		cout<<"************************************"<<endl;
		cout<<"LumTau FOR RUN "<<m_runNo<<" is OK!"<<endl;
		cout<<"************************************"<<endl;

		//              cout<<endl<<m_runNo<<"  "<<runTime<<"   "<<Lum_start<<" "<<Lum_end<<"   "<<Tau_Value<<" "<<SftVer<<"    "<<ParVer<<endl;
		return 1;
	}
}
