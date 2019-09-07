//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: Oct. 26, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#include "BesMdcDigitizer.hh"
#include "BesMdcDigitizerMessenger.hh"
#include "BesMdcHit.hh"

#include "G4DigiManager.hh"
#include "Randomize.hh"
#include "G4ios.hh"
#include <string>

#include "TFile.h"
#include "TH1F.h"

#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"

BesMdcDigitizer::BesMdcDigitizer(G4String modName):G4VDigitizerModule(modName){
	noiseFlag=0;
	noiseType=3;
	noiseLevel=0.1;//10%
	maxNoiseT=300.;//ns
	smearFlag=1;
	mdcDRes = 0.13; //mm
	effFlag = 0;
	for(G4int i=0; i<43;i++){
		layerEff.push_back(1.);
	}
	collectionName.push_back("BesMdcDigisCollection");
	digitizerMessenger = new BesMdcDigitizerMessenger(this);
	mdcGeoPointer=BesMdcGeoParameter::GetGeo();
	mdcCalPointer=new BesMdcCalTransfer;

	//  ISvcLocator* svcLocator = Gaudi::svcLocator();
	IG4Svc* tmpSvc;
	//G4Svc* m_G4Svc;
	StatusCode sc=Gaudi::svcLocator()->service("G4Svc", tmpSvc);
	if (!sc.isSuccess())
		G4cout <<" MdcDigitizer::Error,could not open G4Svc"<<G4endl;

	m_G4Svc=dynamic_cast<G4Svc *>(tmpSvc); 

	IMdcTunningSvc* IMdcTunningSvc;  
	sc= Gaudi::svcLocator()->service("MdcTunningSvc",IMdcTunningSvc);
	if (!sc.isSuccess()){
		G4cout <<" MdcDigitizer::Error,could not open Mdc Tunning Service"<<G4endl;
	}else{
		G4cout<<" MdcDigitizer:: Open Mdc Tunning Service"<<G4endl; 
	}
	mdcTunningSvc=dynamic_cast<MdcTunningSvc *>(IMdcTunningSvc); 

	std::string noiseFile=m_G4Svc->GetMdcNoiseFile();
	f=new TFile(noiseFile.c_str());
	h1=(TH1F*)f->Get("h703");
	h2=(TH1F*)f->Get("h501");
	h3=(TH1F*)f->Get("h801");    
	/*
	//get Mdc Ntuple from G4Svc
	if(m_G4Svc->MdcRootFlag())
	{
	m_tupleMdc = m_G4Svc->GetTupleMdc();
	sc = m_tupleMdc->addItem("NHits",m_NHits);
	sc = m_tupleMdc->addItem("LayerId",m_layerId);
	sc = m_tupleMdc->addItem("cellId",m_cellId);
	sc = m_tupleMdc->addItem("Edep",m_edep);
	sc = m_tupleMdc->addItem("driftD",m_driftD);
	// sc = m_tupleMdc->addItem("driftT",m_driftT);
	sc = m_tupleMdc->addItem("globalT",m_globalT);
	sc = m_tupleMdc->addItem("theta",m_theta);
	sc = m_tupleMdc->addItem("enterAngle",m_enterAngle);
	sc = m_tupleMdc->addItem("driftDNew",m_driftDNew);  
	sc = m_tupleMdc->addItem("driftTNew",m_driftTNew);  
	// sc = m_tupleMdc->addItem("adc",m_adc);
	// sc = m_tupleMdc->addItem("tdc",m_tdc);
	}
	*/
}

BesMdcDigitizer::~BesMdcDigitizer(){delete digitizerMessenger;}

void BesMdcDigitizer::SetEff(G4int layer, G4double eff){
	if(layer==-1){
		for(G4int i=0; i<43;i++){
			layerEff[i]=eff;
		}
	}else{
		layerEff[layer]=eff;
	}
}

void BesMdcDigitizer::Digitize(){

	//initialize 
	for(G4int i=0; i<43;i++){
		for(G4int j=0;j<288;j++){
			digiPointer[i][j]=-1;
		}
	}

	G4int  NHits,layerId, cellId, posFlag; 
	G4double edep,driftD,driftT, globalT, theta,cosTheta,enterAngle;
	G4double mean,sigma,mean1,mean2,sigma1, sigma2, f,sig,delSig, fRandom, driftDNew, driftTNew;
	G4double tempEff;
	G4double resLargest,resSmallest,resRatio;//added by liukai

	G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();

	//hits collection ID
	G4int THCID=-1;
	THCID = DigiMan->GetHitsCollectionID("BesMdcHitsCollection");

	//hits collection
	BesMdcHitsCollection* THC = 0;
	THC = (BesMdcHitsCollection*) (DigiMan->GetHitsCollection(THCID));

	if(THC){
		digisCollection=new BesMdcDigisCollection
			(moduleName, collectionName[0]);
		NHits=THC->entries();
		for(G4int i=0;i<NHits;i++){
			layerId = (*THC)[i]->GetLayerNo();
			cellId = (*THC)[i]->GetCellNo();
			edep = (*THC)[i]->GetEdep();
			driftD = (*THC)[i]->GetDriftD();
			globalT = (*THC)[i]->GetGlobalT();
			theta = (*THC)[i]->GetTheta();
			cosTheta = cos(theta); 
			enterAngle = (*THC)[i]->GetEnterAngle();
			posFlag = (*THC)[i]->GetPosFlag();

			//Transfer hit pointer to BesMdcCalTransfer
			mdcCalPointer->SetHitPointer((*THC)[i]);

			//Filter with wire efficiency
			if(effFlag==0){
				//tempEff = mdcCalPointer->GetEff();
				tempEff=mdcTunningSvc->GetEff(layerId,cellId,driftD,cosTheta,posFlag);
			}else{
				tempEff = layerEff[layerId];
			}
			fRandom=G4UniformRand();
			if(fRandom>tempEff)continue;

			//cout<<"layerid "<<layerId<<" cellid "<<cellId<<" theta "<<cosTheta<<" enterangle "<<enterAngle<<endl;
			//Drift distance smear
			if(smearFlag==0){ //No smear
				driftDNew = driftD;
			}else if(smearFlag==1){           //Smear from TuningSvc
				//	mdcTunningSvc->GetRes(layerId,cellId,driftD,cosTheta,posFlag,enterAngle,mean,sigma);
				//mdcTunningSvc->GetRes2(layerId,cellId,driftD,cosTheta,posFlag,enterAngle,f,mean1,sigma1,mean2,sigma2);
				mdcTunningSvc->GetRes3(layerId,cellId,driftD,cosTheta,posFlag,enterAngle,f,mean1,sigma1,mean2,sigma2,resLargest,resSmallest,resRatio);

				//driftDNew = Smear(driftD,f,mean1,sigma1,mean2,sigma2);
				//driftDNew = Smear(driftD-(f*mean1+(1-f)*mean2),f,mean1,sigma1,mean2,sigma2);//new method

				driftDNew = Smear(driftD-(f*mean1+(1-f)*mean2),f,mean1,sigma1,mean2,sigma2,resLargest,resSmallest,resRatio);//----added by liukai 2012-6-4


			}else if(smearFlag==2){ //Smear with fixed resolution
				driftDNew = Smear(driftD);
			}else{
				G4cerr<<"MdcDigitizer::worong smearFlag: "<<smearFlag<<G4endl;
			}

			//Do X-T conversion
			driftTNew = mdcCalPointer->D2T(driftDNew);

			//Do Q-T correct
			driftTNew += mdcCalPointer->GetTimeWalk();

			//Add T0
			driftTNew += mdcCalPointer->GetT0();

			//Add TOF
			driftTNew += globalT;

			//Signal transfer time on wire
			// transferT=Transfer(layerId,cellId,hitPosition);
			//driftTNew+=transferT;

			if(isnan(driftTNew)){
				G4cout<<"MdcDigitizer::error, driftT is nan"<<G4endl;
				continue;
			}

			/*
			   if(m_G4Svc->MdcRootFlag())
			   {
			   m_NHits= NHits;
			   m_layerId= layerId;
			   m_cellId= cellId;
			   m_edep= edep;
			   m_driftD= driftD;
			//     m_driftT= driftT;
			m_globalT = globalT;
			m_enterAngle = enterAngle;
			m_driftDNew = driftDNew;
			m_driftTNew = driftTNew;
			m_theta = theta;
			m_tupleMdc ->write(); 
			}
			*/
			BesMdcDigi* newDigi = new BesMdcDigi(); 
			newDigi->SetTrackID((*THC)[i]->GetTrackID());
			newDigi->SetLayerNo(layerId);
			newDigi->SetCellNo(cellId);
			newDigi->SetEdep(edep);
			newDigi->SetDriftT(driftTNew);
			G4int NbDigis = digisCollection->insert(newDigi);
			digiPointer[layerId][cellId]=NbDigis-1;
		} 

		if(noiseFlag==1)AddNoise();
		if(noiseFlag==2){
			ifstream readNoiseLevel("$MDCSIMROOT/share/noiselevel.txt");
			if(!readNoiseLevel.good()){
				std::cout<<" Error , noiselevel file not exist "<<std::endl;
			}else{
				std::cout<<" MdcDigitizer:: Open noiselevel file  "<<std::endl;
			}
			G4int NLayer=mdcGeoPointer->SignalLayerNo();
			G4double level;
			for(G4int i=0;i<NLayer;i++){
				readNoiseLevel>>level;
				mixLevel.push_back(level);
			}
			AddNoise2();
		}

		if (verboseLevel>0) { 
			G4cout << "\n-------->digis Collection: in this event they are " 
				<< digisCollection->entries() 
				<< " digis in the MDC chambers: " << G4endl;
			digisCollection->PrintAllDigi();
		} 
		StoreDigiCollection(digisCollection);	
	}

}

G4double BesMdcDigitizer::Smear(G4double driftD){
	G4double r, driftDNew;
	r = G4RandGauss::shoot();
	driftDNew = driftD + mdcDRes * r;
	while(driftDNew<=0){
		r = G4RandGauss::shoot();
		driftDNew = driftD + mdcDRes * r;
	}
	return driftDNew;
}

G4double BesMdcDigitizer::Smear(G4double driftD,G4double sigma,G4double mean){
	G4double r, driftDNew;
	r = G4RandGauss::shoot();
	driftDNew = driftD + sigma * r+mean;
	while(driftDNew <= 0){
		r = G4RandGauss::shoot();
		driftDNew = driftD + sigma * r+mean;
	}
	return driftDNew;
}

G4double BesMdcDigitizer::Smear(G4double driftD,G4double f,G4double mean1,G4double sigma1,G4double mean2,G4double sigma2){
	G4double r, driftDNew,mean,sigma;
	r = G4UniformRand(); 
	if(r<=f){
		mean=mean1;
		sigma=sigma1;
	}else{
		mean=mean2;
		sigma=sigma2;
	}
	int times=0;
	r = G4RandGauss::shoot();
	driftDNew = driftD + sigma * r+mean;
	while(driftDNew <= 0){
		r = G4RandGauss::shoot();
		driftDNew = driftD + sigma * r+mean;
		times++;
		if(times>10)driftDNew=0.01;
	}
	return driftDNew;
}

G4double BesMdcDigitizer::Smear(G4double driftD,G4double f,G4double mean1,G4double sigma1,G4double mean2,G4double sigma2,G4double resLargest,G4double resSmallest,G4double resRatio){
	G4double r, driftDNew,mean,sigma;
	G4double ratio,tempd;
	ratio=G4UniformRand();
	int times;
	if(ratio<=resRatio)
	{
		//for hitOnTrk distribution
		r = G4UniformRand(); 
		if(r<=f){
			mean=mean1;
			sigma=sigma1;
		}else{
			mean=mean2;
			sigma=sigma2;
		}
		times=0;
		r = G4RandGauss::shoot();
		driftDNew = driftD + sigma * r+mean;
	}
	else//for hitNotOnTrk
	{
		tempd=G4UniformRand()*2.0+resLargest;
		times=0;
		driftDNew = driftD + tempd;
	}
	while(driftDNew <= 0){
		r = G4RandGauss::shoot();
		driftDNew = driftD + sigma * r+mean;
		times++;
		if(times>10)driftDNew=0.01;
	}
	return driftDNew;
}

void BesMdcDigitizer::AddNoise2(void){
	G4int wireNo;
	G4double random;
	G4double randomT;
	G4double randomQ;
	G4int NLayer=mdcGeoPointer->SignalLayerNo();
	for(G4int i=0;i<NLayer;i++){
		wireNo=mdcGeoPointer->SignalLayer(i).WireNo()/2;
		for(G4int j=0;j<wireNo;j++){
			random=G4UniformRand(); 
			if(random < mixLevel[i]){
				randomT=G4UniformRand() * 2000;
				if(isnan(randomT)){
					G4cout<<"MdcDigitizer: error, randomT is nan"<<G4endl;
					continue;
				}

				randomQ=200.;
				if(digiPointer[i][j]!=-1){
					G4int pointer=digiPointer[i][j];
					G4double preDriftT=(*digisCollection)[pointer]->GetDriftT();
					if(preDriftT <= randomT)continue;
					(*digisCollection)[pointer]->SetDriftT(randomT);
					(*digisCollection)[pointer]->SetTrackID(-1);
				}else{
					BesMdcDigi* newDigi = new BesMdcDigi(); 
					newDigi->SetTrackID(-1);
					newDigi->SetLayerNo(i);
					newDigi->SetCellNo(j);
					newDigi->SetEdep(randomQ);
					newDigi->SetDriftT(randomT);
					digisCollection->insert(newDigi);  
				}
			}
		}
	}
}


void BesMdcDigitizer::AddNoise(void){
	G4double r0,r;
	vector<G4double>  noise; //Noise level of each layer

	G4int NLayer=mdcGeoPointer->SignalLayerNo();
	if(noiseType==0){
		for(G4int i=0;i<NLayer;i++){
			noise.push_back(noiseLevel);
		}
	}else if(noiseType==1){
		r0=mdcGeoPointer->SignalLayer(0).R();
		for(G4int i=0;i<NLayer;i++){
			r=mdcGeoPointer->SignalLayer(i).R();
			noise.push_back(noiseLevel * r0 / r);
		}
	}else if(noiseType==2){
		r0=mdcGeoPointer->SignalLayer(0).R();
		for(G4int i=0;i<NLayer;i++){
			r=mdcGeoPointer->SignalLayer(i).R();
			noise.push_back(noiseLevel * r0 * r0 / r / r);
		}
	}else if(noiseType==3){ // liugc add 22:11 4/14/06
		Int_t Nbins=(Int_t)h1->GetNbinsX();

		double xmax=h1->GetXaxis()->GetXmax();
		double xmin=h1->GetXaxis()->GetXmin();
		double dx=(xmax-xmin)/Nbins;
		G4double y;
		for(G4int i=0;i<Nbins;i++){
			double x=double(i+1)*dx;
			y=(G4double)h1->GetBinContent(x);
			y=y*noiseLevel/0.05608559;// Normalize use noise level of 1st layer in "run23096noise.root" 
			noise.push_back(y);
		}
	}

	G4int wireNo;
	G4double random;
	G4double randomT;
	G4double randomQ;
	G4double T0=m_G4Svc->GetBeamStartTime();
	for(G4int i=0;i<NLayer;i++){
		wireNo=mdcGeoPointer->SignalLayer(i).WireNo()/2;
		for(G4int j=0;j<wireNo;j++){
			random=G4UniformRand(); 
			if(random < noise[i]){
				//randomT=G4UniformRand() * maxNoiseT;
				randomT=h2->GetRandom()+T0;
				//        randomT=randomT;
				//        randomQ=h3->GetRandom();
				//        randomQ=randomQ*0.001*0.001; //Transfer from TDC channels to Mev, coef. is temporary one.
				if(isnan(randomT)){
					G4cout<<"MdcDigitizer: error, randomT is nan"<<G4endl;
					continue;
				}

				randomQ=0.;
				if(digiPointer[i][j]!=-1){
					G4int pointer=digiPointer[i][j];
					G4double signalEdep=(*digisCollection)[pointer]->GetEdep();
					(*digisCollection)[pointer]->SetEdep(randomQ+signalEdep);
					G4double preDriftT=(*digisCollection)[pointer]->GetDriftT();
					if(preDriftT <= randomT)continue;
					(*digisCollection)[pointer]->SetDriftT(randomT);
					(*digisCollection)[pointer]->SetTrackID(-1);
				}else{
					BesMdcDigi* newDigi = new BesMdcDigi(); 
					newDigi->SetTrackID(-1);
					newDigi->SetLayerNo(i);
					newDigi->SetCellNo(j);
					newDigi->SetEdep(randomQ);
					newDigi->SetDriftT(randomT);
					digisCollection->insert(newDigi);  
				}
			}
		}
	}
}
