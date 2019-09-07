//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description: Utilize Geant4 full simulation results
////Author: liuy
////Created: Oct, 2008
////Modified:Dec, 2008
////Comment:
////---------------------------------------------------------------------------//
////$Id: BesTofDigitizerEcV3.cc

#include "BesTofDigitizerEcV3.hh"
#include "BesTofHit.hh"
#include "G4DigiManager.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include "G4Poisson.hh"
#include "BesTofDigi.hh"
#include "BesTofGeoParameter.hh"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"
#include "TH1F.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TMath.h"
#include <unistd.h> // Check the file

BesTofDigitizerEcV3::BesTofDigitizerEcV3()
{
  ReadData(); //Get some basic data (not Hit data, but information about smearing from electronics,... )
  m_timeBinSize = 0.005;




  // Simulation/G4Svc/G4Svc-00-01-47/src/G4Svc.cpp
  //Get the basic parameters from the event: Starttime, Beamposition,...

  //retrieve G4Svc   
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IG4Svc* tmpSvc;
  StatusCode sc = svcLocator->service("G4Svc", tmpSvc);
  if(!sc.isSuccess())
  {
    std::cout << " Could not initialize Realization Service in BesTofDigitizerBrV2" << std::endl;
  }
  else
  {
    m_G4Svc = dynamic_cast<G4Svc *>(tmpSvc);
  }

  //retrieve RealizationSvc
  IRealizationSvc *tmpReal;
  StatusCode scReal = svcLocator->service("RealizationSvc",tmpReal);
  if (!scReal.isSuccess())
  {
    std::cout << " Could not initialize Realization Service in BesTofDigitizerEcV3" << std::endl;
  } 
  else 
  {
    m_RealizationSvc = dynamic_cast<RealizationSvc*>(tmpReal);
  }



  for(int i=0;i<50;i++)
  {
    for(int j=0;j<10;j++)
    {
      for(int k=0;k<10;k++)
      {
        for(int m=0;m<num1;m++) // num1 bin number in histogram is 400 at the moment
        {  
          //G4cout << "time:" << propTime[i][j][k][m] << "; prob:" << prob[i][j][k][m] << "; eff:" << eff[i][j][k] << G4endl;
          propTime[i][j][k][m] = 0;
          prob[i][j][k][m] = 0;
          eff[i][j][k] = 0;
        }
      }
    }
  }

  ReadEffTree();
  G4cout << "ETofSim: Reading nTuples of is completed." << G4endl;
}

void BesTofDigitizerEcV3::ReadData()
{
  BesTofGeoParameter* tofPara = BesTofGeoParameter::GetInstance();

  m_ecR1         = tofPara->GetEcR1();
  m_tau1Ec       = tofPara->GetTau1Ec();
  m_tau2Ec       = tofPara->GetTau2Ec();
  m_tau3Ec       = tofPara->GetTau3Ec();
  m_tauRatioEc   = tofPara->GetTauRatioEc();
  m_refIndexEc   = tofPara->GetRefIndexEc();
  m_phNConstEc   = tofPara->GetPhNConstEc();
  m_Cpe2pmtEc    = tofPara->GetCpe2pmtEc();
  m_rAngleEc     = tofPara->GetRAngleEc();
  m_QEEc         = tofPara->GetQEEc();
  m_CEEc         = tofPara->GetCEEc();
  m_peCorFacEc   = tofPara->GetPeCorFacEc();
  m_attenEc      = tofPara->GetAttenEc();

  m_ttsMeanEc    = tofPara->GetTTSmeanEc();
  m_ttsSigmaEc   = tofPara->GetTTSsigmaEc();
  m_PMTgainEc    = tofPara->GetPMTgainEc();
  m_CeEc         = tofPara->GetCeEc();
  m_riseTimeEc   = tofPara->GetRiseTimeEc();
  m_LLthreshEc   = tofPara->GetLLthreshEc();
  m_HLthreshEc   = tofPara->GetHLthreshEc();
  m_preGainEc    = tofPara->GetPreGainEc();
  m_noiseSigmaEc = tofPara->GetNoiseSigmaEc();

  //G4cout << "m_LLthreshEc:" << m_LLthreshEc << "; m_HLthreshEc:" << m_HLthreshEc << G4endl;

}

//I do not know what the following function read!
void BesTofDigitizerEcV3::ReadEffTree()
{
  int rBin,phiBin,zBin;
  const int nR = 43;
  const int nPhi = 6;
  const int nZ = 6;
  float efficiency0,x[400],y[400]; 


  G4String dataPath = getenv("TOFSIMROOT");
  if(!dataPath)
  {
    G4Exception("Boss environment is not set!");
  }

  char treePath[200];
  G4int runId = m_RealizationSvc->getRunId();
  if(runId>=-80000 && runId<=-9484)
  {
    // After TOF HV adjustment, endcap attenL was set to 5000mm.
    sprintf(treePath,"%s/dat/effTree_1600mm.root",dataPath.c_str());
  }
  else
  {
    //Before TOF HV adjustment, endcap attenL was set to 1600mm.
    sprintf(treePath,"%s/dat/effTree_1600mm.root",dataPath.c_str());
  }

  TFile *f = new TFile(treePath, "read");
  TTree *t = (TTree*)f->Get("effTree");

  t->SetBranchAddress("rBin", &rBin);
  t->SetBranchAddress("phiBin", &phiBin);
  t->SetBranchAddress("zBin", &zBin);
  t->SetBranchAddress("efficiency0", &efficiency0);
  t->SetBranchAddress("x", x);
  t->SetBranchAddress("y", y);

  int r,phi,z;
  for (Int_t i = 0; i < nR*nPhi*nZ; i++){
    t->GetEntry(i);
    r = rBin;
    phi = phiBin;
    z = zBin;
    eff[r][phi][z] = efficiency0;
    for (Int_t j = 0; j < 400; j++){
      propTime[r][phi][z][j] = x[j];
      prob[r][phi][z][j] = y[j];
      //cout << "\n" << propTime[r][phi][z][j] << "   " << prob[r][phi][z][j] << endl;
    }
  }

}

BesTofDigitizerEcV3::~BesTofDigitizerEcV3()
{;}

void BesTofDigitizerEcV3::Digitize(ScintSingle* scint, BesTofDigitsCollection* DC)
{
  m_beamTime = m_G4Svc->GetBeamTime() * ns;
  m_besTofDigitsCollection = DC;

  G4DigiManager* digiManager = G4DigiManager::GetDMpointer();

  G4int THCID = digiManager->GetHitsCollectionID("BesTofHitsCollection");
  m_THC = (BesTofHitsCollection*) (digiManager->GetHitsCollection(THCID));

  if (m_G4Svc->TofRootFlag())
  {
    m_eTotal = 0;
    m_nDigi = 0;
    m_partIdMPV = -9;
    m_scinNbMPV = -9;
    m_edepMPV = 0;
    m_nDigiOut = 0;
  }

  if (m_THC)
  {
    //for each digi, compute TDC and ADC
    G4int partId, scinNb, nHits;
    G4double edep;
    BesTofHit* hit;
    partId=scint->GetPartId();
    scinNb=scint->GetScinNb();
    edep = scint->GetEdep();
    nHits=scint->GetHitIndexes()->size();

    TofPmtInit();

    //fill tof Ntuple
    if (m_G4Svc->TofRootFlag())
    {
      if (edep>m_edepMPV)
      {
        m_partIdMPV = partId;
        m_scinNbMPV = scinNb;
        m_edepMPV = edep;
      }
      m_eTotal += edep;
      m_nDigi ++;

      m_partId = partId;
      m_scinNb = scinNb;
      m_edep = edep;
      m_nHits = nHits;
    }

    if (edep>0.01)
    {
      for (G4int j=0;j<nHits;j++)
      {
        hit= (*m_THC)[( *(scint->GetHitIndexes()) )[j]];
        TofPmtAccum(hit);
      }

      if (m_G4Svc->TofRootFlag())
      {
        m_time1st0=m_t1st[0];
        m_time1st1=m_t1st[1];
        m_timelast0=m_tLast[0];
        m_timelast1=m_tLast[1];
        m_totalPhot0=m_totalPhot[0];
        m_totalPhot1=m_totalPhot[1];
      }

      //get final tdc and adc
      TofPmtRspns(partId,scinNb);

      G4double temp0 = m_ADC[0]+m_TDC[0];
      G4double temp1 = m_ADC[1]+m_TDC[1];
      //const double MAX_ADC = 8191*0.3; // channel set up to 8192 will lead to overflow.
      if ( (partId!=1) && temp0>0. )
      {
        BesTofDigi* digi = new BesTofDigi;
        digi->SetTrackIndex(m_trackIndex);
        digi->SetPartId(partId);
        digi->SetScinNb(scinNb);
        digi->SetForwADC( m_ADC[0]) ;
        digi->SetBackADC( m_ADC[1]) ;
        if (m_TDC[0]>0.)
          m_TDC[0] = m_TDC[0]+m_beamTime;
        digi->SetForwTDC( m_TDC[0]) ;
        digi->SetBackTDC( m_TDC[1]) ;
  
	//G4cout << "BesTofDigitizerEcV3:     ForwTDC   |   BackTDC  "  << m_TDC[0] << "  |  " << m_TDC[1] << G4endl;
	//G4cout<<"endcap\nadc0:"<<m_ADC[0]<<"; adc1:"<<m_ADC[1]<<"; tdc0:"<<m_TDC[0]<<"; tdc1:"<<m_TDC[1]<<G4endl;
        m_besTofDigitsCollection->insert(digi);
        if (m_G4Svc->TofRootFlag() )
          m_nDigiOut++;
      }
      if (m_G4Svc->TofRootFlag()  )
        m_tupleTof1->write();
      //cout << "m_tupleTof1->write()" << endl;
    }

  }
  if (m_G4Svc->TofRootFlag())
    m_tupleTof2->write();
  //cout << "m_tupleTof2->write()" << endl;

}

void BesTofDigitizerEcV3::TofPmtInit()
{
  m_ADC[0] = -999.;
  m_ADC[1] = -999.;
  m_TDC[0] = -999.;
  m_TDC[1] = -999.;
  m_trackIndex = -999;
  m_globalTime = 9999;

  m_t1st[0]=100;
  m_t1st[1]=100;
  m_tLast[0]=0.;
  m_tLast[1]=0;
  m_totalPhot[0]=0;
  m_totalPhot[1]=0;
  for (G4int i=0;i<2;i++)
    for (G4int j=0;j<m_profBinNEcV3;j++)
      m_nPhot[j][i]=0;

  if (m_G4Svc->TofRootFlag())
  {
    m_partId = -9;
    m_scinNb = -9;
    m_edep = 0;
    m_nHits = 0;
    m_time1st0 = 100;
    m_time1st1 = 100;
    m_timelast0 = 0;
    m_timelast1 = 0;
    m_totalPhot0 = 0;
    m_totalPhot1 = 0;
    m_NphAllSteps = 0;
    m_max0 = 0;
    m_max1 = 0;
    m_tdc0 = -999;
    m_adc0 = -999;
    m_tdc1 = -999;
    m_adc1 = -999;
  }

}

void BesTofDigitizerEcV3::TofPmtAccum(BesTofHit* hit)
{
  G4double cvelScint = c_light/m_refIndexEc;
  //Get information of this step
  G4ThreeVector pos = hit->GetPos();
  G4int trackIndex = hit->GetTrackIndex();
  G4int partId = hit->GetPartId();
  G4double edep = hit->GetEdep();
  G4double stepL = hit->GetStepL();
  //G4String particleName = hit->GetPName();
  G4double deltaT=hit->GetDeltaT();
  G4double timeFlight=hit->GetTime()-m_beamTime;
  if (timeFlight < m_globalTime)
  {
    m_globalTime = timeFlight;
    m_trackIndex = trackIndex;
  }

  G4ThreeVector pDirection=hit->GetPDirection();
  G4double nx=pDirection.x();
  G4double ny=pDirection.y();
  G4double nz=pDirection.z();


  //number of photons generated in this step
  G4int NphStep;
  G4double nMean, nPhoton;
  nMean = m_phNConstEc*BirksLaw(hit);

  if(nMean>10)
  {
    G4double resolutionScale=1.;
    G4double sigma=resolutionScale*sqrt(nMean);
    nPhoton=G4int(G4RandGauss::shoot(nMean,sigma)+0.5);
  } 
  else
    nPhoton=G4int(G4Poisson(nMean));


  NphStep=G4int(nPhoton*0.66*m_QEEc*m_CEEc);
  //G4cout << "\nradius:" << radius << "; phi:" << phi << "; z:" << z << G4endl;
  //G4cout << "\nrBin:" << rBin << ";phiBin:" << phiBin << ";zBin" << zBin << G4endl;

  if (m_G4Svc->TofRootFlag())
    m_NphAllSteps += G4int(nPhoton*0.66*m_QEEc*m_CEEc);

  if (NphStep>0)
  {
    for (G4int i=0;i<NphStep;i++)
    {
      //uniform scintilation in each step
      G4double ddS, ddT;
      ddS=stepL*G4UniformRand();
      ddT=deltaT*G4UniformRand();
      G4ThreeVector emtPos;
      emtPos.setX(pos.x() + nx*ddS);
      emtPos.setY(pos.y() + ny*ddS);
      emtPos.setZ(pos.z() + nz*ddS);

      //retrieve the histogram info
      G4double radius = sqrt(emtPos.x()*emtPos.x()+emtPos.y()*emtPos.y())-m_ecR1;
      const G4double pie = 2.*asin(1.);
      G4double phi;
      if(emtPos.x()>0 && emtPos.y()>0) 
        phi = atan(emtPos.y()/emtPos.x());
      else if(emtPos.x()==0 && emtPos.y()>0)
        phi = pie/2.;
      else if(emtPos.x()<0)
        phi = atan(emtPos.y()/emtPos.x())+pie;
      else if(emtPos.x()==0 && emtPos.y()<0)
        phi = 1.5*pie;
      else if(emtPos.x()>0 && emtPos.y()<0)
        phi = 2.*pie+atan(emtPos.y()/emtPos.x());
      phi = phi*180./pie; // in degrees
      G4double z = fabs(emtPos.z());
      // Warning: Should obtain absolute value of z to determine zBinNum

      G4int rBin = G4int(radius/10.); // radius bin no.
      G4double resPhi = phi-(G4int(phi/7.5)*7.5); // residual of phi in period of 7.5
      G4int phiBin = G4int(resPhi/1.25);
      G4int zBin = G4int((z-1332.)/8.);

      //check scinillation light whether to hit the pmt or not
      //forb=0/1 for forward(z>0, east) and backward(z<0, west)
      G4int forb = 0;
      G4double transpTime = 0;
      G4double pathL = 0;
      G4double efficiency1;
      G4double efficiency2;
      efficiency1 = G4RandGauss::shoot(0,0.004);
      if(rBin>=0&&rBin<=nR && phiBin>=0&& phiBin<=nPhi && zBin>=0&&zBin<=nZ)
        efficiency1 += eff[rBin][phiBin][zBin];
      else
        efficiency1 = 0;
      //G4cout << "FATAL: The collection efficiency does NOT exist!" << G4endl;
      if(m_attenEc==0)
      {
        G4cout <<" ERROR: Attenuation Length is null!" << G4endl;
        break;
      }
      //efficiency2 = pow(efficiency1,(1600/m_attenEc));
      if(G4UniformRand() <= efficiency1)
      {
        DirectPh(rBin, phiBin, zBin, transpTime);
        //cout << "transpTime:" << transpTime << endl;
      }

      //check if photon can reach PMT or not, after attenuation
      //G4double ran = G4UniformRand();
      //pathL = transpTime*cvelScint;
      //if (pathL>0 && exp(-pathL/m_attenEc) > ran)  // Note: Do NOT double count attuation!
      if(transpTime>0)
      {
        //propagation time in scintillator
        G4double scinSwim = transpTime;
        //scintillation timing
        G4double scinTime = Scintillation(partId);

        //PMT transit time
        G4double transitTime = TransitTime();
        //sum up all time components
        G4double endTime = timeFlight + ddT + scinSwim + scinTime + transitTime;

        if (m_G4Svc->TofRootFlag())
        {
          //m_forb = forb;
          m_timeFlight = timeFlight+ddT;
          m_ddT = ddT;
          m_scinSwim = scinSwim;
          m_scinTime = scinTime;
          m_transitTime = transitTime;
          m_endTime = endTime;
          m_tupleTof3->write();
        }

        //store timing into binned buffer
        AccuSignal(endTime, forb);

        //update 1st and last timings here
        if (m_t1st[forb]>endTime)   m_t1st[forb] = endTime;
        if (m_tLast[forb]<endTime)  m_tLast[forb]= endTime;
        //if(m_tLast[0]>100)
        //std::cout<<"endTime: "<<endTime<<std::endl;
      }
    }
  }
}

G4double BesTofDigitizerEcV3::BirksLaw(BesTofHit* hit)
{
  const G4double kappa = 0.015*cm/MeV;
  const G4String brMaterial = "BC404";
  G4double dE = hit->GetEdep();
  //G4cout << "The edep is "<< dE << G4endl;
  G4double dX = hit->GetStepL();
  //G4Material* materiral = hit->GetMaterial();
  G4double charge = hit->GetCharge();
  G4double cor_dE = dE;
  //if((materiral->GetName()==brMaterial) && charge!=0.&& dX!=0.)
  if(charge!=0.&& dX!=0.)
  {
    cor_dE = dE/(1+kappa*dE/dX);
    //if(dE>20)
    //{
    //  G4cout << "\n dE > 20. Details are below:" << G4endl;
    //  G4cout << "dE/dx:" << dE/dX << G4endl;
    //  G4cout << "dE:" << dE << "; dX:" << dX << G4endl;
    //  G4cout << "It is BC404. cor_dE is " << cor_dE << G4endl;
    //  G4double ratio = cor_dE/dE;
    //  G4cout << "The ratio cor_dE/edep is "<< ratio << G4endl;
    //}
    //G4cout << "It is BC408. cor_dE is " << cor_dE << G4endl;
    //G4double ratio = cor_dE/dE;
    //G4cout << "The ratio cor_dE/edep is "<< ratio << G4endl;
  }
  return cor_dE;

}

void BesTofDigitizerEcV3::DirectPh(G4int rBin, G4int phiBin, G4int zBin, G4double& t)
{
  G4double ran = G4UniformRand();
  G4double p = 0; 
  G4int nth = 1;
  G4int key = 0;
  t = 0;
  while(1)
  {
    if(p>ran||nth==400)
    {
      key = nth;
      //G4cout << "Value found!" << G4endl;
      break;
    }
    p = p + prob[rBin][phiBin][zBin][nth];
    nth++;
  }
  t = propTime[rBin][phiBin][zBin][key-1];
}

G4double BesTofDigitizerEcV3::Scintillation(G4int partId)
{
  G4double tmp_tauRatio,tmp_tau1,tmp_tau2,tmp_tau3;
  tmp_tauRatio = m_tauRatioEc;
  tmp_tau1 = m_tau1Ec;
  tmp_tau2 = m_tau2Ec;
  tmp_tau3 = m_tau3Ec;

  G4double UniformR = tmp_tauRatio/(1+tmp_tauRatio);
  G4double EmissionTime;
  if (G4UniformRand()>UniformR) {
    while (1) {
      EmissionTime = -tmp_tau2*log( G4UniformRand() );
      if (G4UniformRand()-exp(EmissionTime/tmp_tau2-EmissionTime/tmp_tau1)>1.E-8)
        break;
    }
  }
  else EmissionTime = -tmp_tau3*log( G4UniformRand() );
  return EmissionTime;
}

G4double BesTofDigitizerEcV3::TransitTime()
{
  //get time transit spread
  //G4cout << "TTS mean:" << m_ttsMeanEc << "; " << m_ttsSigmaEc << G4endl;
  return G4RandGauss::shoot(m_ttsMeanEc,m_ttsSigmaEc);
}

void BesTofDigitizerEcV3::AccuSignal(G4double endTime, G4int forb)
{
  G4int ihst;
  ihst=G4int(endTime/m_timeBinSize);
  if (ihst>0 &&ihst<m_profBinNEcV3)
  {
    m_nPhot[ihst][forb]=m_nPhot[ihst][forb]+1;
    m_totalPhot[forb]=m_totalPhot[forb]+1;
  }
}

void BesTofDigitizerEcV3::TofPmtRspns(G4int partId, G4int scinNb)
{
  //to generate PMT signal shape for single photoelectron.
  //only one time for a job.
  static G4double snpe[m_snpeBinNEcV3];
  static G4int istore_snpe=-1;

  //Model: f(t)=Gain*mv_1pe* t**2 * exp-(t/tau)**2/normal-const
  //normalization const =sqrt(pi)*tau*tau*tau/4.0
  G4double tau = m_riseTimeEc;
  G4double norma_const=sqrt(M_PI)*tau*tau*tau/4.0;    //in unit of ns**3
  G4double echarge=1.6e-7;   //in unit of PC

  //time profile of pmt signals for Back and Forward PMT.
  G4double profPmt[m_profBinNEcV3][2];

  G4double t;
  G4int n1, n2, ii;
  G4int phtn;

  if (istore_snpe<0)
  {
    istore_snpe = 1;
    for (G4int i=0;i<m_snpeBinNEcV3;i++)
    {
      t=(i+1)*m_timeBinSize;
      snpe[i]=m_CeEc*t*t*exp(- (t/tau) * (t/tau) )/norma_const;
    }
  }
  //for barrel and endcap tof: fb=2 or 1
  G4int fb=1;   // for endcap part

  G4double tmpADC[2] = {0,0};

  for (G4int j=0; j<fb; j++)
  {
    if (m_totalPhot[j] > 0)
    {
      n1=G4int(m_t1st[j]/m_timeBinSize);
      n2=G4int(m_tLast[j]/m_timeBinSize);

      for (G4int i=0;i<m_profBinNEcV3;i++)
        profPmt[i][j]=0.0;

      //generate PMT pulse
      n2 = n2<m_profBinNEcV3 ? n2:m_profBinNEcV3;
      for (G4int i=n1;i<n2;i++)
      {
        phtn=m_nPhot[i][j];
        if (phtn>0)
        {
          G4double Npoisson;
          while(1) {
            Npoisson=G4Poisson(10.0);
            if(Npoisson>0) break;
          }
          G4double tmpPMTgain;
          while(1) {
	    m_PMTgainEc = m_tofSimSvc->EndPMTGain(); 
            tmpPMTgain=G4RandGauss::shoot(m_PMTgainEc,m_PMTgainEc/sqrt(Npoisson));
            //tmpPMTgain = m_PMTgainEc;
            if(tmpPMTgain>0) break;
          }
          tmpADC[j]+=phtn*tmpPMTgain;

          for (G4int ihst=0; ihst<m_snpeBinNEcV3; ihst++)
          {
            ii=i+ihst;
            if (ii<m_profBinNEcV3)
              profPmt[ii][j] += tmpPMTgain*phtn*snpe[ihst];
            else
              break;
          }
        }
      }

      //add preamplifier and noise
      for (int i=0;i<m_profBinNEcV3;i++)
      {
        if (profPmt[i][j]>0)
          profPmt[i][j] = m_preGainEc*profPmt[i][j]+G4RandGauss::shoot(0,m_noiseSigmaEc);
      }

      //get pulse height
      G4double max=0;
      for (int i=n1;i<m_profBinNEcV3;i++)
      {
        if (profPmt[i][j]>max)
          max=profPmt[i][j];
      }
      if (m_G4Svc->TofRootFlag())
      {
        if (j==0)  m_max0=max;
        else      m_max1=max;
      }


      G4double tmp_HLthresh, tmp_LLthresh, ratio;

//      G4int runId = m_RealizationSvc->getRunId();
//      if(runId>=-80000 && runId<=-9484)
//      {
//        // After TOF HV adjustment, PMT Gain of barrel TOF in MC is set to 5.E5
//        // High/Low Threshold for barrel: 500/125 mV
//        tmp_HLthresh = m_HLthreshEc;
//        tmp_LLthresh = m_LLthreshEc;
//      }
//      else
//      {
//        // Before TOF HV adjustment, PMT Gain of barrel TOF in MC is set to 2.5E5
//        // High/Low Threshold for barrel: 600/150 mV
//        tmp_HLthresh = 800;
//        tmp_LLthresh = 200;
//      }

      G4double adcFactor = 3.35;
//      double ratio;
//      if (runId>=-80000 && runId<=-9484) 
//      {
//        ratio = 1.22*1354.4/1282.1;
//      }
//      else 
//      {
//        ratio = 1.77*2028.8/1931.4;
//      }
 
      tmp_HLthresh = m_tofSimSvc->EndHighThres();
      tmp_LLthresh = m_tofSimSvc->EndLowThres();
      ratio = m_tofSimSvc->EndConstant();
     
      //get final tdc and adc
      if (max>=tmp_HLthresh)
      {
        for (int i=0;i<m_profBinNEcV3;i++)
        {
          if ( profPmt[i][j] >= tmp_LLthresh )
          {
            m_TDC[j] = i*m_timeBinSize + G4RandGauss::shoot(0,0.025); // Adding Electronical Uncertainty of 25ps
	    G4double NoiseSigma;
	    G4int EndNoiseSwitch = int(m_tofSimSvc->EndNoiseSwitch());
	    //std::cout << " EndNoiseSwitch = " << EndNoiseSwitch << std::endl;
	    switch (EndNoiseSwitch) {
	      case 0:
		NoiseSigma = 0.;                
		break;
	      case 1:
		if (partId==0) { NoiseSigma = m_tofSimSvc->EndNoiseSmear(scinNb); }
		if (partId==2) { NoiseSigma = 0.; }
		break;
	      case 2: 
		if (partId==0) { NoiseSigma = m_tofSimSvc->EndNoiseSmear(scinNb); }
		if (partId==2) { NoiseSigma = m_tofSimSvc->EndNoiseSmear(scinNb+48.); }
		break;	    
	    }
	    //std::cout << "ID : " << scinNb + 48.*partId/2. << " Sigma : " << NoiseSigma << std::endl;
            m_TDC[j] = m_TDC[j] + G4RandGauss::shoot(0,NoiseSigma);   // Adding Endcap Noise Smear
            
            //use the saturation curve

            if( m_G4Svc->TofSaturationFlag())
            {
              double x = tmpADC[j]*m_preGainEc*echarge*ratio;
              int id;	      
              if (partId==0) { id = scinNb;}
              if (partId==2) { id = scinNb+48;}

              m_ADC[j] = m_tofQElecSvc->EQChannel(id,x);
            }
            else
              m_ADC[j] = tmpADC[j]*m_preGainEc*echarge*adcFactor;

            if (m_G4Svc->TofRootFlag())
            {
              if (j==0) {
                m_tdc0 = m_TDC[0];
                m_adc0 = m_ADC[0];
              }
              else     {
                m_tdc1 = m_TDC[1];
                m_adc1 = m_ADC[1];
              }
            }
            break;
          }
        }
      }
    }
  }
}

