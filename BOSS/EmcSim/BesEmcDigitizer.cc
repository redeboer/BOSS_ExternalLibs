//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC digitizer 
//Author: Hemiao
//Created: Sep, 2004
//Comment:
//---------------------------------------------------------------------------//
//$Id:BesEmcDigitizer.cc

#include "BesEmcDigitizer.hh"
#include "BesEmcDigi.hh"
#include "BesEmcHit.hh"
#include "BesEmcWaveform.hh"
#include "G4DigiManager.hh"
#include "BesEmcParameter.hh"
#include "Randomize.hh"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "G4Svc/G4Svc.h"
#include "EmcCalibConstSvc/EmcCalibConstSvc.h"

BesEmcDigitizer::BesEmcDigitizer(G4String modName)
:G4VDigitizerModule(modName),m_emcCalibConstSvc(0)
{
  collectionName.push_back("BesEmcDigitsCollection");
  m_besEmcDigitsCollection = 0;

  //retrieve G4Svc
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IG4Svc* iG4Svc;
  StatusCode sc=svcLocator->service("G4Svc", iG4Svc);
  m_G4Svc=dynamic_cast<G4Svc *>(iG4Svc);

  //get Emc Ntuple from G4Svc
  if(m_G4Svc->EmcRootFlag())
  {
    m_tupleEmc1 = m_G4Svc->GetTupleEmc1();
    sc = m_tupleEmc1->addItem("partId",m_partId);
    sc = m_tupleEmc1->addItem("nTheta",m_nTheta);
    sc = m_tupleEmc1->addItem("nPhi",m_nPhi);
    sc = m_tupleEmc1->addItem("edep",m_eDep);
    sc = m_tupleEmc1->addItem("nHits",m_nHits);
    sc = m_tupleEmc1->addItem("adc",m_adc);
    sc = m_tupleEmc1->addItem("tdc",m_tdc);
    
    m_tupleEmc2 = m_G4Svc->GetTupleEmc2();
    sc = m_tupleEmc2->addItem("etot",m_eTot);
    sc = m_tupleEmc2->addItem("nDigi",m_nDigi);
  }

  // Get EmcCalibConstSvc.
  sc = svcLocator->service("EmcCalibConstSvc", m_emcCalibConstSvc);
  if(sc != StatusCode::SUCCESS) {
    G4cout << "BesEmcDigitizer Error: Can't get EmcCalibConstSvc." << G4endl;
  }

}

BesEmcDigitizer::~BesEmcDigitizer()
{
}

void BesEmcDigitizer::Initialize()
{
}

void BesEmcDigitizer::Digitize()
{
  Initialize();


  m_besEmcDigitsCollection = new BesEmcDigitsCollection
    ("BesEmcDigitizer","BesEmcDigitsCollection");
  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();


  //hits collection ID
  G4int EHCID;
  EHCID = DigiMan->GetHitsCollectionID("BesEmcHitsCollection");

  //hits collection
  BesEmcHitsCollection* EHC = 0;
  EHC = (BesEmcHitsCollection*) (DigiMan->GetHitsCollection(EHCID));

  if (EHC)
  {
    //BesEmcParameter& emcPara=BesEmcParameter::GetInstance();
    m_crystalGroup = new vector<CrystalSingle*>;
    GroupHits(EHC);
    G4int size=m_crystalGroup->size();
    CrystalSingle* cryst;
    G4int partId, nTheta, nPhi, nHits; 
    G4double eTot=0, eDigi;
    BesEmcHit* hit;

    G4double coherentNoise = RandGauss::shoot()*m_G4Svc->EmcCoherentNoise();

    for(G4int i=0;i<size;i++)
    {
      cryst = (*m_crystalGroup)[i];   //all hits in a crystal
      partId = cryst->GetPartId();
      nTheta = cryst->GetNTheta();
      nPhi = cryst->GetNPhi();
      nHits= cryst->GetHitIndexes()->size();
      eDigi = cryst->GetEdep();
      eTot += eDigi;

      BesEmcDigi *digi = new BesEmcDigi;
      BesEmcWaveform *wave = digi->GetWaveform();
      G4long bin = 0;       //time

      const int indexSize = 200;
      G4double e[indexSize];      //energy of the same index
      for(G4int i=0;i<indexSize;i++)
        e[i]=0;
      G4int index=0;
      G4double energy=0;

      for(G4int j=0;j<nHits;j++)
      {
        hit= (*EHC)[( *(cryst->GetHitIndexes()) )[j]];
        energy = hit->GetEdepCrystal();
        index = hit->GetTrackIndex();
        if(index<indexSize&&index>=0)
          e[index]+=energy;
        else
          G4cout<<"Track index overload!"<<G4endl;
      }

      G4double maxi=e[0];         //find the index which gives the most energy in one crystal
      for(G4int i=1;i<indexSize;i++)
      {
        if(e[i]>maxi)
        {
          maxi = e[i];
          index = i;
        }
      }

      if(eDigi>0)
      {
        digi->SetPartId(partId);
        digi->SetThetaNb(nTheta);
        digi->SetPhiNb(nPhi);
        digi->SetEnergy(eDigi);
        digi->SetTime(m_G4Svc->EmcTime());
        digi->SetTrackIndex(index);

        wave->updateWaveform(digi);
        if(m_G4Svc->EmcNoiseLevel()>0)
          wave->addElecNoise(m_G4Svc->EmcIncoherentNoise(),coherentNoise);

        //to avoid error caused by precision, get energy before digitization
        m_energy = wave->max(bin);
        //temp code, subtract pedstal
        m_energy -= 0.46*MeV;
        wave->digitize();
        wave->max(bin);

        if(m_G4Svc->EmcLightOutput())
        {
          G4int index = m_emcCalibConstSvc->getIndex(partId,nTheta,nPhi);
          G4double adc2e = m_emcCalibConstSvc->getDigiCalibConst(index);  


	  if (m_G4Svc->EmcElecSaturation()==1){
	    G4double emaxData = m_emcCalibConstSvc->getCrystalEmaxData(index);
	   
	    if (emaxData>0) {
	     
	      adc2e=emaxData/2.5;
	    }
	  }

          if(adc2e<=1e-5) // dead channel
          {
            m_energy = 0;
          }
          else
          { 

	      m_energy /= adc2e;
	    

            //m_energy /= emcPara.GetLightOutput(partId,nTheta,nPhi);
          }
        }

        //fill Emc Ntuple1
        if(m_G4Svc->EmcRootFlag())
        {
          m_partId = partId;
          m_nTheta = nTheta;
          m_nPhi = nPhi;
          m_eDep = eDigi;
          m_nHits = nHits;
          m_adc = m_energy;
          m_tdc = bin;
          m_tupleEmc1->write();
        }

        digi->SetEnergy(m_energy);
        digi->SetTime(bin);
        digi->SetWaveform(wave);  
        m_besEmcDigitsCollection->insert(digi);     
      }
    }

    //add to noise to no signal crystals
    if(m_G4Svc->EmcNoiseLevel()==2)
      AddNoise5x5(coherentNoise);
    else if(m_G4Svc->EmcNoiseLevel()==3)
      ;
    //AddNoiseAll(coherentNoise);

    //fill Emc Ntuple2
    if(m_G4Svc->EmcRootFlag())
    {
      m_eTot = eTot;
      m_nDigi = size;
      m_tupleEmc2->write();
    }

    StoreDigiCollection(m_besEmcDigitsCollection);

    for(size_t i=0;i<m_crystalGroup->size();i++)
    {
      delete (*m_crystalGroup)[i];
    }
    m_crystalGroup->clear();
    delete m_crystalGroup;
  }
}

void BesEmcDigitizer::GroupHits(BesEmcHitsCollection* m_EHC)
{
  G4int partId, nTheta, nPhi, size, flag;
  G4double edep;
  BesEmcHit* hit;
  G4int nHits = m_EHC->entries();

  //group the hits which are in the same crystal
  for(G4int i=0;i<nHits;i++)
  {
    hit = (*m_EHC)[i];
    partId = hit->GetPartId();
    nTheta = hit->GetNumThetaCrystal();
    nPhi = hit->GetNumPhiCrystal();
    edep = hit->GetEdepCrystal();
    size = m_crystalGroup->size();
    flag=0;

    if(size>0)
    {
      CrystalSingle* oldCryst;
      for(G4int j=0; j<size;j++)
      {
        oldCryst = (*m_crystalGroup)[j];
        if((oldCryst->GetNTheta()==nTheta)&&(oldCryst->GetNPhi()==nPhi)&&(oldCryst->GetPartId()==partId))
        {
          oldCryst->GetHitIndexes()->push_back(i);
          oldCryst->AddEdep(edep);
          flag=1; 
          break;
        }
      }
    }

    if(flag==0)
    {
      CrystalSingle* newCryst = new CrystalSingle;
      newCryst->SetPartId(partId);
      newCryst->SetNTheta(nTheta);
      newCryst->SetNPhi(nPhi);
      newCryst->SetEdep(edep);
      newCryst->GetHitIndexes()->push_back(i);
      m_crystalGroup->push_back(newCryst);
    }

  }
}

void BesEmcDigitizer::AddNoise5x5(G4double coherentNoise)
{
  BesEmcParameter& emcPara = BesEmcParameter::GetInstance();
  vector<BesEmcDigi*>* vecDC = m_besEmcDigitsCollection->GetVector();
  G4int nDigi = m_besEmcDigitsCollection->entries();
  G4int partMax,thetaMax,phiMax;
  partMax=thetaMax=phiMax=-99;
  G4double eMax = 0;

  for(G4int i=0;i<nDigi;i++) {
    BesEmcDigi *digi = (*vecDC)[i];
    double eDigi = digi->GetEnergy();
    if(eDigi>eMax) {
      eMax = eDigi;
      partMax = digi->GetPartId();
      thetaMax = digi->GetThetaNb();
      phiMax = digi->GetPhiNb();
    }
  }

  if(partMax==1) { // barrel
    G4int thetan,thetap,phin,phip;
    thetan = thetaMax-2;
    thetap = thetaMax+2;
    phin = phiMax-2;
    phip = phiMax+2;

    if(thetaMax==0) { // #0
      thetan = thetaMax;
    } else if(thetaMax==1) {  // #1
      thetan = thetaMax-1;
    } else if(thetaMax==emcPara.GetBSCNbTheta()*2-1) {  // #43
      thetap = thetaMax;
    } else if(thetaMax==emcPara.GetBSCNbTheta()*2-2) {  // #42
      thetap = thetaMax+1;
    }

    if(phiMax==0) {
      phin = emcPara.GetBSCNbPhi()-2;
    } else if(phiMax==1) {
      phin = emcPara.GetBSCNbPhi()-2;
    } else if(phiMax==emcPara.GetBSCNbPhi()-1) {  // #119
      phip = 1;
    } else if(phiMax==emcPara.GetBSCNbPhi()-2) {  // #118
      phip = 0;
    }

    for(G4int theta=thetan;theta<=thetap;theta++) {
      for(G4int phi=phin;phi<=phip;phi++) {
        G4bool flag = true;

        if(nDigi>0) {
          for(G4int i=0;i<nDigi;i++) {
            BesEmcDigi *digi = (*vecDC)[i];
            if( partMax == digi->GetPartId()
                && theta == digi->GetThetaNb()
                && phi == digi->GetPhiNb() ) {
              flag=false;
              break;
            }
          }
        }

        if(flag) {
          BesEmcDigi *digi = new BesEmcDigi;
          BesEmcWaveform *wave = digi->GetWaveform();
          digi->SetPartId(partMax);
          digi->SetThetaNb(theta);
          digi->SetPhiNb(phi);
          digi->SetEnergy(0);
          digi->SetTime(m_G4Svc->EmcTime());
          digi->SetTrackIndex(-9);

          wave->updateWaveform(digi);
          wave->addElecNoise(m_G4Svc->EmcIncoherentNoise(),coherentNoise);
          wave->digitize();

          G4long bin = 0;       //time
          m_energy = wave->max(bin);

          if(m_G4Svc->EmcLightOutput()) {
            m_energy *= emcPara.GetLightOutput(partMax,theta,phi);
          }

          digi->SetEnergy(m_energy);
          digi->SetTime(bin);
          digi->SetWaveform(wave);

          //Correction of electronics bias
          G4double ecorr;
          if(m_energy<625.) {
            ecorr = -0.1285*log(m_energy/6805.);     //noise=0.5
          } else {
            ecorr = -2.418+9.513e-4*m_energy;
          }

          if(m_energy-ecorr>m_G4Svc->EmcNoiseThreshold()) {
            m_besEmcDigitsCollection->insert(digi);
          } else {
            delete digi;
          }
        }
      } //phi
    } //theta

  } //part==1
}

void BesEmcDigitizer::AddNoiseAll(G4double coherentNoise)
{
  BesEmcParameter& emcPara = BesEmcParameter::GetInstance();
  vector<BesEmcDigi*>* vecDC = m_besEmcDigitsCollection->GetVector();
  G4int nDigi = m_besEmcDigitsCollection->entries();
  //G4cout<<"nDigi="<<nDigi<<G4endl;

  for(G4int part=0;part<3;part++) {

    G4int thetaNb;
    if(part == 1) { //barrel
      thetaNb = emcPara.GetBSCNbTheta()*2;
    } else {  //endcap
      thetaNb = 6;
    }

    for(G4int theta=0;theta<thetaNb;theta++) {

      G4int phiNb;
      if(part == 1) {
        phiNb = emcPara.GetBSCNbPhi();
      } else {
        phiNb = emcPara.GetCryInOneLayer(theta);
      }

      for(G4int phi=0;phi<phiNb;phi++) {

        G4bool flag = true;

        if(nDigi>0) {
          //G4cout<<"nDigi="<<nDigi<<"\t";

          for(G4int i=0;i<nDigi;i++) {
            BesEmcDigi *digi = (*vecDC)[i];
            if( part == digi->GetPartId()
                && theta == digi->GetThetaNb()
                && phi == digi->GetPhiNb() ) {
              //cout<<theta<<"\t"<<phi<<endl;
              flag=false;
              break;
            }
          }
        }

        if(flag) {
          BesEmcDigi *digi = new BesEmcDigi;
          digi->SetTrackIndex(-9);
          digi->SetPartId(part);
          digi->SetThetaNb(theta);
          digi->SetPhiNb(phi);

          bool fastSimulation = true;
          if(fastSimulation) {

            m_energy = RandGauss::shoot()*m_G4Svc->EmcNoiseSigma();
            m_energy += m_G4Svc->EmcNoiseMean();
            digi->SetTime((G4int)(G4UniformRand()*60));

          } else {

            BesEmcWaveform *wave = digi->GetWaveform();
            digi->SetTime(m_G4Svc->EmcTime());

            wave->updateWaveform(digi);
            wave->addElecNoise(m_G4Svc->EmcIncoherentNoise(),coherentNoise);
            wave->digitize();

            G4long bin = 0;       //time
            m_energy = wave->max(bin);
            digi->SetTime(bin);
            digi->SetWaveform(wave);  
          }

          if(m_G4Svc->EmcLightOutput()) {
            m_energy *= emcPara.GetLightOutput(part,theta,phi);
          }
          digi->SetEnergy(m_energy);

          //Correction of electronics bias
          G4double ecorr;
          if(m_energy<625.) {
            ecorr = -0.1285*log(m_energy/6805.);     //noise=0.5
          } else {
            ecorr = -2.418+9.513e-4*m_energy;
          }

          if(m_energy-ecorr>m_G4Svc->EmcNoiseThreshold()) {
            m_besEmcDigitsCollection->insert(digi);
          } else {
            delete digi;
          }
        }

      } //phi
    } //theta
  } //part
}
