//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description:
////Author: Dengzy
////Created: Mar, 2004
////Modified:
////Comment:
////---------------------------------------------------------------------------//
////$Id: BesTofDigitizerEcV2.cc

#include "BesTofDigitizerEcV2.hh"
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

BesTofDigitizerEcV2::BesTofDigitizerEcV2()
{
    ReadData();
    m_timeBinSize=0.005;

    //retrieve G4Svc
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IG4Svc* tmpSvc;
    StatusCode sc = svcLocator->service("G4Svc", tmpSvc);
    m_G4Svc = dynamic_cast<G4Svc *>(tmpSvc);

}

void BesTofDigitizerEcV2::ReadData()
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
}

BesTofDigitizerEcV2::~BesTofDigitizerEcV2()
{
    ;
}

void BesTofDigitizerEcV2::Digitize(ScintSingle* scint, BesTofDigitsCollection* DC)
{
    m_beamTime = m_G4Svc->GetBeamTime() * ns;
    m_besTofDigitsCollection = DC;

    G4DigiManager* digiManager = G4DigiManager::GetDMpointer();

    G4int THCID = digiManager->GetHitsCollectionID("BesTofHitsCollection");
    m_THC = (BesTofHitsCollection*) (digiManager->GetHitsCollection(THCID));

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

        if (edep>0.01)
        {
            for (G4int j=0;j<nHits;j++)
            {
                hit= (*m_THC)[( *(scint->GetHitIndexes()) )[j]];
                TofPmtAccum(hit);
            }

            //get final tdc and adc
            TofPmtRspns(partId);

            G4double temp0 = m_ADC[0]+m_TDC[0];
            G4double temp1 = m_ADC[1]+m_TDC[1];
            if ( (partId==1&&temp0>0&&temp1>0) || ((partId!=1)&&temp0>0) )
            {
                if (m_ADC[0]>1000) m_ADC[0]=1000;
                if (m_ADC[1]>1000) m_ADC[1]=1000;
                BesTofDigi* digi = new BesTofDigi;
                digi->SetTrackIndex(m_trackIndex);
                digi->SetPartId(partId);
                digi->SetScinNb(scinNb);
                digi->SetForwADC( m_ADC[0]) ;
                digi->SetBackADC( m_ADC[1]) ;
                if (m_TDC[0]!=-999)
                    m_TDC[0] = m_TDC[0]+m_beamTime;
                if (m_TDC[1]!=-999)
                    m_TDC[1] = m_TDC[1]+m_beamTime;
                digi->SetForwTDC( m_TDC[0]) ;
                digi->SetBackTDC( m_TDC[1]) ;
                m_besTofDigitsCollection->insert(digi);
            }
        }
    }
}

void BesTofDigitizerEcV2::TofPmtInit()
{
    m_trackIndex=-999;
    m_globalTime = 9999;
    m_t1st[0]=100;
    m_t1st[1]=100;
    m_tLast[0]=0.;
    m_tLast[1]=0;
    m_totalPhot[0]=0;
    m_totalPhot[1]=0;
    for (G4int i=0;i<2;i++)
        for (G4int j=0;j<m_profBinNEcV2;j++)
            m_nPhot[j][i]=0;

}

void BesTofDigitizerEcV2::TofPmtAccum(BesTofHit* hit)
{
    G4double cvelScint=c_light/m_refIndexEc;
    //Get information of this step
    G4ThreeVector pos=hit->GetPos();
    G4int trackIndex = hit->GetTrackIndex();
    G4int partId =hit->GetPartId();
    G4double edep=hit->GetEdep();
    G4double stepL=hit->GetStepL();
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

    //Cpe2pmt=cathode area/scint area
    //peCorFac=correction factor for eff. of available Npe

    //number of photons generated in this step
    G4int NphStep;
    NphStep=G4int(edep*m_phNConstEc*m_Cpe2pmtEc*0.66*m_QEEc*m_CEEc*m_peCorFacEc);

    G4double ddS, ddT;
    if (NphStep>0)
    {
        for (G4int i=0;i<NphStep;i++)
        {
            //uniform scintilation in each step
            ddS=stepL*G4UniformRand();
            ddT=deltaT*G4UniformRand();
            G4ThreeVector emtPos;
            emtPos.setX(pos.x() + nx*ddS);
            emtPos.setY(pos.y() + ny*ddS);
            emtPos.setZ(pos.z() + nz*ddS);

            //check scinillation light whether to hit the pmt or not
            //forb=0/1 for forward(z>0, east) and backward(z<0, west)
            G4double pathL=0;
            G4int forb;
            DirectPh(partId, emtPos, pathL, forb);

            //check if photon can reach PMT or not, after attenuation
            G4double ran=G4UniformRand();
            if (pathL>0 && exp(-pathL/m_attenEc) > ran)
            {
                //propagation time in scintillator
                G4double scinSwim=pathL/cvelScint;
                //scintillation timing
                //G4double scinTime=GenPhoton(partId);
                G4double scinTime=Scintillation(partId);

                //PMT transit time
                G4double transitTime=TransitTime();
                //sum up all time components
                G4double endTime= timeFlight + ddT + scinSwim + scinTime + transitTime;

                //store timing into binned buffer
                AccuSignal(endTime, forb);

                //update 1st and last timings here
                if (m_t1st[forb]>endTime)   m_t1st[forb] = endTime;
                if (m_tLast[forb]<endTime)  m_tLast[forb]= endTime;
            }
        }
    }
}


void BesTofDigitizerEcV2::DirectPh(G4int partId, G4ThreeVector emtPos, G4double& pathL, G4int& forb)
{
    //generation photon have random direction
    //optical parameters of scintillation simulation
    //G4double cos_spanEc = 1-1/m_refIndex;
    G4double cos_spanEc = 1;
    G4double ran;
    //dcos=cos_span*(ran*2.0-1.0);
    ran=G4UniformRand();
    //assuming uniform phi distribution, simulate cos distr only
    G4double dcosEc = cos_spanEc*(ran*2.0-1.0);
    G4double r2=sqrt(emtPos.x()*emtPos.x()+emtPos.y()*emtPos.y());
    if (dcosEc > 0.0 && dcosEc != 1)
    {
        forb=0;  //forward
        pathL = (r2-m_ecR1)/(1.0-dcosEc);
    }
    else if (dcosEc < 0 && dcosEc != -1)
    {
        forb=0;
        pathL=(2*838-r2-m_ecR1)/(1.0+dcosEc);
    }
}

G4double BesTofDigitizerEcV2::Scintillation(G4int partId)
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

/*G4double BesTofDigitizerEcV2::GenPhoton(G4int partId)
{
  //get scintilation time
  G4double scinTime;
  static G4double scinA[26000];
  G4double random[2];
  G4double inverseRegion, ran1, ran2, problive;
  static G4int istore=-1;
  if(istore<0)
  {
    istore=1;
    for(G4int i=0;i<26000;i++)
      scinA[i]=Scintillation( (i+1) *0.001 , partId);
  }
  while(1)
  {
    HepRandom::getTheEngine()->flatArray(2, random);
    inverseRegion=random[0]*2.00975218688231;
    ran1=-4.5*log(1-inverseRegion/(0.45*4.5));
    ran2=0.45*exp(-1.*ran1/4.5)*random[1];
    problive=scinA[G4int(ran1*1000)+1];
    if(problive>=ran2)
    { scinTime=ran1;  break; }
  }
  return scinTime;
}*/

G4double BesTofDigitizerEcV2::TransitTime()
{
    //get time transit spread
    return G4RandGauss::shoot(m_ttsMeanEc,m_ttsSigmaEc);
}

void BesTofDigitizerEcV2::AccuSignal(G4double endTime, G4int forb)
{
    G4int ihst;
    ihst=G4int(endTime/m_timeBinSize);
    if (ihst>0 &&ihst<m_profBinNEcV2)
    {
        m_nPhot[ihst][forb]=m_nPhot[ihst][forb]+1;
        m_totalPhot[forb]=m_totalPhot[forb]+1;
    }
}

void BesTofDigitizerEcV2::TofPmtRspns(G4int partId)
{
    //to generate PMT signal shape for single photoelectron.
    //only one time for a job.
    static G4double snpe[m_snpeBinNEcV2];
    static G4int istore_snpe=-1;

    //Model: f(t)=Gain*mv_1pe* t**2 * exp-(t/tau)**2/normal-const
    //normalization const =sqrt(pi)*tau*tau*tau/4.0
    G4double tau = m_riseTimeEc;
    G4double norma_const=sqrt(M_PI)*tau*tau*tau/4.0;    //in unit of ns**3
    G4double echarge=1.6e-7;   //in unit of PC

    //time profile of pmt signals for Back and Forward PMT.
    G4double profPmt[m_profBinNEcV2][2];

    G4double t;
    G4int n1, n2, ii;
    G4int phtn;

    if (istore_snpe<0)
    {
        istore_snpe = 1;
        for (G4int i=0;i<m_snpeBinNEcV2;i++)
        {
            t=(i+1)*m_timeBinSize;
            snpe[i]=m_PMTgainEc*m_CeEc*t*t*exp(- (t/tau) * (t/tau) )/norma_const;
        }
    }
    //for barrel and endcap tof: fb=2 or 1
    G4int fb=1;

    for (G4int j=0;j<2;j++)
    {
        m_TDC[j]=-999.0;
        m_ADC[j]=-999.0;
    }
    for (G4int j=0; j<fb; j++)
    {
        if (m_totalPhot[j] > 0)
        {
            n1=G4int(m_t1st[j]/m_timeBinSize);
            n2=G4int(m_tLast[j]/m_timeBinSize);

            for (G4int i=0;i<m_profBinNEcV2;i++)
                profPmt[i][j]=0.0;

            //generate PMT pulse
            n2 = n2<m_profBinNEcV2 ? n2:m_profBinNEcV2;
            for (G4int i=n1;i<n2;i++)
            {
                phtn=m_nPhot[i][j];
                if (phtn>0)
                {
                    for (G4int ihst=0; ihst<m_snpeBinNEcV2; ihst++)
                    {
                        ii=i+ihst;
                        if (ii<m_profBinNEcV2)
                            profPmt[ii][j] += phtn*snpe[ihst];
                    }
                }
            }

            //add preamplifier and noise
            for (int i=0;i<m_profBinNEcV2;i++)
            {
                if (profPmt[i][j]>0)
                    profPmt[i][j] = m_preGainEc*profPmt[i][j]+G4RandGauss::shoot(0,m_noiseSigmaEc);
            }

            //get pulse height
            G4double max=0;
            for (int i=0;i<m_profBinNEcV2;i++)
            {
                if (profPmt[i][j]>max)
                    max=profPmt[i][j];
            }

            G4double tmp_HLthresh, tmp_LLthresh;
            if (partId==1)  {
                tmp_HLthresh = m_HLthreshEc;
                tmp_LLthresh = m_LLthreshEc;
            }
            else {
                tmp_HLthresh = m_HLthreshEc;
                tmp_LLthresh = m_LLthreshEc;
            }

            //get final tdc and adc
            if (max>=tmp_HLthresh)
            {
                for (int i=0;i<m_profBinNEcV2;i++)
                {
                    if ( profPmt[i][j] >= tmp_LLthresh )
                    {
                        m_TDC[j] = i*m_timeBinSize;
                        m_ADC[j] = m_preGainEc*m_totalPhot[j]*echarge*m_PMTgainEc;
                        break;
                    }
                }
            }
        }
    }
}











