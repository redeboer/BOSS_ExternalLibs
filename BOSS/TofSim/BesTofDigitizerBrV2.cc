//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description:
////Author: Dengzy
////Created: Mar, 2004
////Modified:
////Comment:
////---------------------------------------------------------------------------//
////$Id: BesTofDigitizerBrV2.cc

#include "BesTofDigitizerBrV2.hh"
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
#include "TMath.h"

BesTofDigitizerBrV2::BesTofDigitizerBrV2()
{
  ReadData();
  m_timeBinSize=0.005;

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
    std::cout << " Could not initialize Realization Service in BesTofDigitizerBrV2" << std::endl;
  } 
  else 
  {
    m_RealizationSvc = dynamic_cast<RealizationSvc*>(tmpReal);
  }


}

void BesTofDigitizerBrV2::ReadData()
{
  BesTofGeoParameter* tofPara = BesTofGeoParameter::GetInstance();

  m_scinLength = tofPara->GetBr1L();
  m_tau1       = tofPara->GetTau1();
  m_tau2       = tofPara->GetTau2();
  m_tau3       = tofPara->GetTau3();
  m_tauRatio   = tofPara->GetTauRatio();
  m_refIndex   = tofPara->GetRefIndex();
  m_phNConst   = tofPara->GetPhNConst();
  m_Cpe2pmt    = tofPara->GetCpe2pmt();
  m_rAngle     = tofPara->GetRAngle();
  m_QE         = tofPara->GetQE();
  m_CE         = tofPara->GetCE();
  m_peCorFac   = tofPara->GetPeCorFac();

  m_ttsMean    = tofPara->GetTTSmean();
  m_ttsSigma   = tofPara->GetTTSsigma();
  m_Ce         = tofPara->GetCe();
  m_LLthresh   = tofPara->GetLLthresh();
  m_HLthresh   = tofPara->GetHLthresh();
  m_preGain    = tofPara->GetPreGain();
  m_noiseSigma = tofPara->GetNoiseSigma();


}

BesTofDigitizerBrV2::~BesTofDigitizerBrV2()
{
  ;
}

void BesTofDigitizerBrV2::Digitize(ScintSingle* scint, BesTofDigitsCollection* DC)
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


    //std::cout << "BesTofDigitizerBrV2      Partid   scinNb   " << partId  << "   "  << scinNb << std::endl; 
    //cout << "*** scinNb:" << scinNb << " *** " << m_tofCaliSvc->BAtten(scinNb) << "***" << endl;
    //cout << "*****scinNb:"<< scinNb << " ***** A2:" << m_tofCaliSvc->BGainBackward(scinNb) 
    //  << " ***** A1:" << m_tofCaliSvc->BGainForward(scinNb) << endl;

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
        TofPmtAccum(hit, scinNb);
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
      TofPmtRspns(scinNb);
      //G4cout<<"pre-cut " << partId << "\nadc0:"<<m_ADC[0]<<"; adc1:"
      //  <<m_ADC[1]<<"; tdc0:"<<m_TDC[0]<<"; tdc1:"
      //  <<m_TDC[1]<<G4endl;

      G4double temp0 = m_ADC[0]+m_TDC[0];
      G4double temp1 = m_ADC[1]+m_TDC[1];
      //cout << "partid: " <<  partId << " temp0: " <<  temp0 << " temp1: " <<  temp1 << endl;
      //if ( partId==1 && m_ADC[0]>255 && m_ADC[1]>255 && m_TDC[0]>0. && m_TDC[1]>0.)
      if ( partId==1 && (temp0 > -1998. || temp1 > -1998.))
      {
        //const double MAX_ADC = 8191 * 0.3;  // channel set up to 8192 will lead to overflow.
        BesTofDigi* digi = new BesTofDigi;
        digi->SetTrackIndex(m_trackIndex);
        digi->SetPartId(partId);
        digi->SetScinNb(scinNb);
        digi->SetForwADC( m_ADC[0]) ;
        digi->SetBackADC( m_ADC[1]) ;
        if (m_TDC[0]>0)
          m_TDC[0] = m_TDC[0]+m_beamTime;
        if (m_TDC[1]>0)
          m_TDC[1] = m_TDC[1]+m_beamTime;
        digi->SetForwTDC( m_TDC[0]) ;
        digi->SetBackTDC( m_TDC[1]) ;
        //G4cout<<"+++++++++++++++++++++++++++++barrel\nadc0:"<<m_ADC[0]<<"; adc1:"
        //  <<m_ADC[1]<<"; tdc0:"<<m_TDC[0]<<"; tdc1:"
        //  <<m_TDC[1]<<G4endl;

        m_besTofDigitsCollection->insert(digi);

        if (m_G4Svc->TofRootFlag())
          m_nDigiOut++;

      }
      if (m_G4Svc->TofRootFlag())
        m_tupleTof1->write();

    }

    if (m_G4Svc->TofRootFlag())
      m_tupleTof2->write();
  }
}

void BesTofDigitizerBrV2::TofPmtInit()
{
  Initialize();

  m_t1st[0]=100;
  m_t1st[1]=100;
  m_tLast[0]=0.;
  m_tLast[1]=0;
  m_totalPhot[0]=0;
  m_totalPhot[1]=0;
  for (G4int i=0;i<2;i++)
    for (G4int j=0;j<m_profBinN;j++)
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

void BesTofDigitizerBrV2::TofPmtAccum(BesTofHit* hit, G4int scinNb)
{
  BesTofGeoParameter* tofPara = BesTofGeoParameter::GetInstance();
  //std::cout << "BesTofDigitizerBrV2::TofPmtAccum()" << std::endl;
  G4double cvelScint=c_light/m_refIndex/1.07;
  //Get information of this step
  G4ThreeVector pos=hit->GetPos();
  G4int trackIndex = hit->GetTrackIndex();
  G4int partId =hit->GetPartId();
  G4double edep=hit->GetEdep();
  G4double stepL=hit->GetStepL();
  G4double deltaT=hit->GetDeltaT();
  G4double timeFlight=hit->GetTime()-m_beamTime;
  //std::cout << "timeFlight: " << timeFlight << std::endl;
  if (timeFlight < m_globalTime)
  {
    m_globalTime = timeFlight;
    m_trackIndex = trackIndex;
  }

  G4ThreeVector pDirection=hit->GetPDirection();
  G4double nx=pDirection.x();
  G4double ny=pDirection.y();
  G4double nz=pDirection.z();

  //phNConst=(Nph/MeV)*(QE)*(CE)*(1-cos(crit));
  //        =10000 * 0.2 * 0.6 * (1-cos(39.265))=270.931
  //asin(1/1.58) = 39.265248
  //only the light has theta=0---39.265 can go out to PMT, the probability is computed with solid angle
  //solid angle = phi*(1-cos(theta)), phi=2pi

  //Cpe2pmt=cathode area/scint area
  //peCorFac=correction factor for eff. of available Npe

  //G4double thetaProb = 1-sqrt(m_refIndex*m_refIndex-1)/m_refIndex;
  G4double thetaProb=1-cos( asin(1.43/m_refIndex));
  //G4double thetaProbEc = 1-1/m_refIndex;

  //number of photons generated in this step 
  G4double nMean, nPhoton;
  //std::cout << "0 BirksLaw(): " << std::endl;
  nMean = m_phNConst*BirksLaw(hit);
  G4int runId = m_RealizationSvc->getRunId();
  if( ( runId>=-11396 && runId<=-8093 ) || ( runId>-80000 && runId<=-23463 ) ) {
    nMean = 9000.0*BirksLaw(hit);
  }
  //std::cout << "1 BirksLaw(): " << std::endl;

  if(nMean>10)
  {
    G4double resolutionScale=1.;
    G4double sigma=resolutionScale*sqrt(nMean);
    nPhoton=G4int(G4RandGauss::shoot(nMean,sigma)+0.5);
  }
  else
    nPhoton=G4int(G4Poisson(nMean));
  //G4cout<<"nPhoton:"<<nPhoton<<G4endl;

  G4int NphStep;
  if (partId==1)
    NphStep=G4int(nPhoton*thetaProb*m_rAngle*m_QE*m_CE*m_peCorFac);
  //else
  //NphStep=G4int(edep*m_phNConstEc*m_Cpe2pmtEc*0.66*m_QEEc*m_CE*m_peCorFac);
  //introduce poission distribution of Npe
  //G4double Navr = NphStep;
  //G4double NpePoiss = G4double(G4Poisson(Navr));
  //G4double adcW_corr =5.0;
  //NphStep=G4int( (NpePoiss - Navr)*adcW_corr + Navr );

  if (m_G4Svc->TofRootFlag())
    m_NphAllSteps +=  NphStep;
  //std::cout << "m_G4Svc->TofRootFlag(): " << m_G4Svc->TofRootFlag() << std::endl;

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

      G4double ran = G4UniformRand();
      //G4double attenL = tofPara->GetAtten(scinNb);
      //G4double attenL = 10.*(m_tofCaliSvc->BAtten(scinNb))/0.75; // cm into mm
      G4double attenL = m_tofSimSvc->BarAttenLength(scinNb); 
      attenL = 10.*attenL/0.75; // cm into mm

      if (pathL>0 && exp(-pathL/attenL) > ran)
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
        //std::cout << "endtime: " << endTime << std::endl;

        if (m_G4Svc->TofRootFlag())
        {
          //m_forb = forb;
          //m_timeFlight = timeFlight;
          //m_ddT = ddT;
          m_scinSwim = scinSwim;
          //m_scinTime = scinTime;
          //m_transitTime = transitTime;
          m_endTime = endTime;
          m_tupleTof3->write();
        }
        //store timing into binned buffer
        AccuSignal(endTime, forb);

        //update 1st and last timings here
        if (m_t1st[forb]>endTime)   m_t1st[forb] = endTime;
        if (m_tLast[forb]<endTime)  m_tLast[forb]= endTime;
      }
    }
  }
}

G4double BesTofDigitizerBrV2::BirksLaw(BesTofHit* hit)
{
  const G4double kappa = 0.015*cm/MeV;
  const G4String brMaterial = "BC408";
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
    //  G4cout << "It is BC408. cor_dE is " << cor_dE << G4endl;
    //  G4double ratio = cor_dE/dE;
    //  G4cout << "The ratio cor_dE/edep is "<< ratio << G4endl;
    //}
    //G4cout << "It is BC408. cor_dE is " << cor_dE << G4endl;
    //G4double ratio = cor_dE/dE;
    //G4cout << "The ratio cor_dE/edep is "<< ratio << G4endl;
  }
  return cor_dE;

}

G4double BesTofDigitizerBrV2::Reflectivity(G4double n1,G4double n2,G4double n3,G4double theta)
{
  G4double I1,I2,I3,rp1,rs1,rp2,rs2,Rp1,Rs1,Rp2,Rs2,Rp,Rs;
  G4double R=1.0;
  //n1=m_refIndex;
  //n2=1.0;
  I1=theta;
  if(I1<asin(n2/n1))
  {
    I2=asin(sin(I1)*(n1/n2));
    rp1=(n1/cos(I1)-n2/cos(I2))/(n1/cos(I1)+n2/cos(I2));
    rs1=(n1*cos(I1)-n2*cos(I2))/(n1*cos(I1)+n2*cos(I2));
    Rp1=rp1*rp1;
    Rs1=rs1*rs1;

    I3=asin(sin(I1)*(n1/n3));
    rp2=(n2/cos(I2)-n3/cos(I3))/(n2/cos(I2)+n3/cos(I3));
    rs2=(n2*cos(I2)-n3*cos(I3))/(n2*cos(I2)+n3*cos(I3));
    Rp2=rp2*rp2;
    Rs2=rs2*rs2;
    Rp=(Rp1+Rp2-2*Rp1*Rp2)/(1-Rp1*Rp2);
    Rs=(Rs1+Rs2-2*Rs1*Rs2)/(1-Rs1*Rs2);
    R=(Rp+Rs)/2.;
  }
  return R;
}


G4double BesTofDigitizerBrV2::Reflectivity(G4double n1,G4double n2,G4double theta)
{
  G4double I1,I2,rp,rs,Rp,Rs;
  G4double R=1.0;
  //n1=m_refIndex;
  //n2=1.0;
  I1=theta;
  if (I1<asin(n2/n1))
  {
    I2=asin(sin(I1)*(n1/n2));
    rp=(n1/cos(I1)-n2/cos(I2))/(n1/cos(I1)+n2/cos(I2));
    rs=(n1*cos(I1)-n2*cos(I2))/(n1*cos(I1)+n2*cos(I2));
    Rp=rp*rp;
    Rs=rs*rs;
    R=(Rp+Rs)/2.;
  }
  return R;
}

void BesTofDigitizerBrV2::DirectPh(G4int partId, G4ThreeVector emtPos, G4double& pathL, G4int& forb)
{
  //std::cout << "BesTofDigitizerBrV2::DirectPh()" << std::endl;
  const static G4double silicon_oil_index = 1.43; 
  const static G4double glass_index = 1.532;
  //generation photon have random direction
  //optical parameters of scintillation simulation
  G4double cos_span=1-cos( asin(silicon_oil_index/m_refIndex) );
  //G4double cos_spanEc = 1;
  G4double dcos, ran;
  ran=G4UniformRand();
  //assuming uniform phi distribution, simulate cos distr only
  dcos=cos_span*(ran*2.0-1.0);
  //G4double dcosEc = cos_spanEc*(ran*2.0-1.0);
  G4double r2=sqrt(emtPos.x()*emtPos.x()+emtPos.y()*emtPos.y());
  G4int nRef=0;
  G4double costheta,sintheta;
  G4double theta,thetaR; // thetaR is scin to air full ref angle. about 39.265 degree.
  costheta=dcos>0?(1-dcos):(1+dcos);
  theta=acos(costheta);
  sintheta=sin(theta);
  thetaR=asin(1/m_refIndex);
  G4double R1;
  R1=Reflectivity(m_refIndex,1.0,theta);
  G4double ratio1Mean=(CLHEP::pi)*25.5*25.5/(57.1+60.7)/25.0; //0.693657
  G4double ratio2Mean=(19.5/25.5)*(19.5/25.5); //0.584775
  G4double ratio1=G4RandGauss::shoot(ratio1Mean,0.015);
  G4double ratio2=G4RandGauss::shoot(ratio2Mean,0.015);
  G4double ratio3Mean=0.945*ratio1Mean*ratio2Mean;
  G4double ratio3=G4RandGauss::shoot(ratio3Mean,0.015);

  G4double R2=1-Reflectivity(m_refIndex,silicon_oil_index, glass_index, theta);
  if (dcos > 0 && dcos != 1)
  {
    if (theta < thetaR) // theta < 39.265 degree
    {
      if (G4UniformRand()<ratio1) //coup1
      {
        if (G4UniformRand()<R2)
        {
          if (G4UniformRand()<ratio2) //PMT 39mm
          {
            forb=0;
            pathL=(m_scinLength/2-emtPos.z())/costheta;
          }
        }
        else
        {
          if (G4UniformRand()<ratio3)
          {
            forb=1;
            pathL=(1.5*m_scinLength-emtPos.z())/costheta;
          }
        }
      }
      else //Air
      {
        if (G4UniformRand()<R1)
        {
          G4double tempran=G4UniformRand();
          if (tempran<ratio3)
          {
            forb=1;
            pathL=(1.5*m_scinLength-emtPos.z())/costheta;
          }
          else if (tempran>ratio1&&G4UniformRand()<R1&&G4UniformRand()<ratio3)
          {
            forb=0;
            pathL=(2.5*m_scinLength-emtPos.z())/costheta;
          }
        }
      }
    }
    else  // 39.265 <= theta < 64.832
    {
      if (G4UniformRand()<ratio1) //coup1
      {
        if (G4UniformRand()<R2)
        {
          if (G4UniformRand()<ratio2) //PMT 39mm
          {
            forb=0;
            pathL=(m_scinLength/2-emtPos.z())/costheta;
          }
        }
        else
        {
          if (G4UniformRand()<ratio3)
          {
            forb=1;
            pathL=(1.5*m_scinLength-emtPos.z())/costheta;
          }
        }
      }
      else //Air
      {
        G4double tempran=G4UniformRand();
        if (tempran<ratio3)
        {
          forb=1;
          pathL=(1.5*m_scinLength-emtPos.z())/costheta;
        }
        else if (tempran>ratio1 && G4UniformRand()<ratio3)
        {
          forb=0;
          pathL=(2.5*m_scinLength-emtPos.z())/costheta;
        }
      }
    }
  }
  else if (dcos < 0 && dcos != -1)
  {
    if (theta < thetaR) // theta < 39.265 degree
    {
      if (G4UniformRand()<ratio1) //coup1
      {
        if (G4UniformRand()<R2)
        {
          if (G4UniformRand()<ratio2) //PMT 39mm
          {
            forb=1;
            pathL=(m_scinLength/2+emtPos.z())/costheta;
          }
        }
        else
        {
          if (G4UniformRand()<ratio3)
          {
            forb=0;
            pathL=(1.5*m_scinLength+emtPos.z())/costheta;
          }
        }
      }
      else //Air
      {
        if (G4UniformRand()<R1)
        {
          G4double tempran=G4UniformRand();
          if (tempran<ratio3)
          {
            forb=0;
            pathL=(1.5*m_scinLength+emtPos.z())/costheta;
          }
          else if (tempran>ratio1&&G4UniformRand()<R1&&G4UniformRand()<ratio3)
          {
            forb=1;
            pathL=(2.5*m_scinLength+emtPos.z())/costheta;
          }
        }
      }
    }
    else  // 39.265 <= theta < 64.832
    {
      if (G4UniformRand()<ratio1) //coup1
      {
        if (G4UniformRand()<R2)
        {
          if (G4UniformRand()<ratio2) //PMT 39mm
          {
            forb=1;
            pathL=(m_scinLength/2+emtPos.z())/costheta;
          }
        }
        else
        {
          if (G4UniformRand()<ratio3)
          {
            forb=0;
            pathL=(1.5*m_scinLength+emtPos.z())/costheta;
          }
        }
      }
      else //Air
      {
        G4double tempran=G4UniformRand();
        if (tempran<ratio3)
        {
          forb=0;
          pathL=(1.5*m_scinLength+emtPos.z())/costheta;
        }
        else if (tempran>ratio1 && G4UniformRand()<ratio3)
        {
          forb=1;
          pathL=(2.5*m_scinLength+emtPos.z())/costheta;
        }
      }
    }
  }

  G4double convFactor=180./3.1415926;
  if (theta>asin(1)-thetaR)
  {
    G4double alpha = G4UniformRand()*asin(1.);
    G4int nRef1 = pathL*sintheta*cos(alpha)/50.0+0.5;
    G4int nRef2 = pathL*sintheta*sin(alpha)/58.9+0.5;
    G4double beta1=acos(sintheta*cos(alpha));
    G4double beta2=acos(sintheta*sin(alpha));
    beta2 -= 3.75/convFactor;
    G4double R21,R22;
    R21=Reflectivity(m_refIndex,1.0,beta1);
    R22=Reflectivity(m_refIndex,1.0,beta2);
    for (G4int i=0;i<nRef1;i++)
    {
      if (G4UniformRand()<(1-R21) && G4UniformRand()<0.15)
        pathL=-9;
    }
    for (G4int i=0;i<nRef2;i++)
    {
      if (G4UniformRand()<(1-R22) && G4UniformRand()<0.15)
        pathL=-9;
    }
  }
}


//void BesTofDigitizerBrV2::DirectPh(G4int partId, G4ThreeVector emtPos, G4double& pathL, G4int& forb)
//{
//  //generation photon have random direction
//  //optical parameters of scintillation simulation
//  G4double cos_span=1-cos( asin(1./m_refIndex) );
//  G4double dcos, ran;
//  ran=G4UniformRand();
//  //assuming uniform phi distribution, simulate cos distr only
//  dcos=cos_span*(ran*2.0-1.0);
//  if (dcos > 0.0&& dcos != 1)
//  {
//    forb=0;  //forward
//    pathL=(m_scinLength/2-emtPos.z())/(1.0-dcos);
//  }
//  else if (dcos < 0 && dcos != -1)
//  {
//    forb=1;
//    pathL=(m_scinLength/2+emtPos.z())/(1.0+dcos);
//  }
//}

G4double BesTofDigitizerBrV2::Scintillation(G4int partId)
{
  G4double tmp_tauRatio,tmp_tau1,tmp_tau2,tmp_tau3;
  tmp_tauRatio = m_tauRatio;
  tmp_tau1 = m_tau1;
  tmp_tau2 = m_tau2;
  tmp_tau3 = m_tau3;
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

/*G4double BesTofDigitizerBrV2::GenPhoton(G4int partId)
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

G4double BesTofDigitizerBrV2::TransitTime()
{
  //get time transit spread
  return G4RandGauss::shoot(m_ttsMean,m_ttsSigma);
}

void BesTofDigitizerBrV2::AccuSignal(G4double endTime, G4int forb)
{
  G4int ihst;
  ihst=G4int(endTime/m_timeBinSize);
  if (ihst>0 &&ihst<m_profBinN)
  {
    m_nPhot[ihst][forb]=m_nPhot[ihst][forb]+1;
    m_totalPhot[forb]=m_totalPhot[forb]+1;
  }
}

void BesTofDigitizerBrV2::TofPmtRspns(G4int scinNb)
{
  //std::cout << "BesTofDigitizerBrV2::TofPmtRspns()" << std::endl;
  BesTofGeoParameter* tofPara = BesTofGeoParameter::GetInstance();
  //to generate PMT signal shape for single photoelectron.
  //only one time for a job.
  G4double snpe[m_snpeBinN][2];

  //Model: f(t)=Gain*mv_1pe* t**2 * exp-(t/tau)**2/normal-const
  //normalization const =sqrt(pi)*tau*tau*tau/4.0
  //G4double tau = m_riseTime;
  G4double norma_const;
  G4double echarge=1.6e-7;   //in unit of PC

  //time profile of pmt signals for Back and Forward PMT.
  G4double profPmt[m_profBinN][2];

  G4double t;
  G4double tau;
  G4int n1, n2, ii;
  G4int phtn;
  // forb = 0, east
  for (G4int i=0;i<m_snpeBinN;i++)
  {
    tau = tofPara->GetBrERiseTime(scinNb);
    norma_const = sqrt(CLHEP::pi)*tau*tau*tau/4.0;    //in unit of ns**3
    t = (i+1)*m_timeBinSize;
    snpe[i][0] = m_Ce*t*t*exp(- (t/tau) * (t/tau) )/norma_const;// Pulse of one single photoelectron
  }
  // forb = 1, west
  for (G4int i=0;i<m_snpeBinN;i++)
  {
    tau = tofPara->GetBrWRiseTime(scinNb);
    norma_const = sqrt(CLHEP::pi)*tau*tau*tau/4.0;    //in unit of ns**3
    t = (i+1)*m_timeBinSize;
    snpe[i][1] = m_Ce*t*t*exp(- (t/tau) * (t/tau) )/norma_const;
  }
  //for barrel and endcap tof: fb=2 or 1
  G4int fb=2;
  G4double Npoisson;
  G4double pmtGain0,pmtGain,relativeGain,smearPMTgain;
  G4double smearADC[2] = {0.,0.};
  G4int runId = m_RealizationSvc->getRunId();
  pmtGain0 = m_tofSimSvc->BarPMTGain();

//  if(runId>=-80000 && runId<=-9484)
//  {
//    // After TOF HV adjustment, PMT Gain of barrel TOF in MC is set to 5.E5
//    // High/Low Threshold for barrel: 500/125 mV
//    pmtGain0 = 6.E5;
//  }
//  else
//  {
//    // Before TOF HV adjustment, PMT Gain of barrel TOF in MC is set to 2.5E5
//    // High/Low Threshold for barrel: 600/150 mV
//    pmtGain0 = 5.E5;
//  }

  G4double timeSmear = G4RandGauss::shoot(0,0.020);
  if (runId>=-22913 && runId<=-20448) {//for 2011-psipp(20448-22913), smear barrel TOF resolution to ~78ps
     timeSmear = G4RandGauss::shoot(0,0.040);
  }
  else if( ( runId>=-11396 && runId<=-8093 ) || ( runId>-80000 && runId<=-23463 ) ) {
     timeSmear = G4RandGauss::shoot(0,0.025);
  }

  for (G4int j=0; j<fb; j++)
  {
    if (m_totalPhot[j] > 0)
    {
      n1=G4int(m_t1st[j]/m_timeBinSize);
      n2=G4int(m_tLast[j]/m_timeBinSize);
      //std::cout << "n1: " << n1 << std::endl;

      for (G4int i=0;i<m_profBinN;i++)
        profPmt[i][j]=0.0;

      //generate PMT pulse
      n2 = n2<m_profBinN ? n2:m_profBinN;
      for (G4int i=n1;i<n2;i++)
      {
        phtn=m_nPhot[i][j];
        if (phtn>0)
        {
          while(1) {
            Npoisson = G4Poisson(10.0);
            if(Npoisson>0.) break;
          }
          while(1) {
            //pmtGain = j ? tofPara->GetBrWPMTgain(scinNb) : tofPara->GetBrEPMTgain(scinNb);
            //relativeGain = j ? m_tofCaliSvc->BGainBackward(scinNb) : m_tofCaliSvc->BGainForward(scinNb);
            relativeGain = j ? m_tofSimSvc->BarGain2(scinNb) : m_tofSimSvc->BarGain1(scinNb);
	    pmtGain = pmtGain0*relativeGain;
            smearPMTgain = G4RandGauss::shoot(pmtGain,pmtGain/sqrt(Npoisson));
            //smearPMTgain = pmtGain;
            if(smearPMTgain>0) break;
          }
          smearADC[j] += phtn*smearPMTgain;

          for (G4int ihst=0; ihst<m_snpeBinN; ihst++)
          {
            ii=i+ihst;
            if (ii<m_profBinN)
              profPmt[ii][j] += smearPMTgain*phtn*snpe[ihst][j];
            else
              break;
          }
        }
      }

      //add preamplifier and noise
      for (int i=0;i<m_profBinN;i++)
      {
        if (profPmt[i][j]>0)
          profPmt[i][j] = m_preGain*profPmt[i][j]+G4RandGauss::shoot(0,m_noiseSigma);
      }

      //get pulse height
      G4double max=0;
      for (int i=n1;i<m_profBinN;i++)
      {
        if (profPmt[i][j]>max)
          max=profPmt[i][j];
      }
      if (m_G4Svc->TofRootFlag())
      {
        if (j==0)  m_max0=max;
        else      m_max1=max;
      }

      G4double tmp_HLthresh, tmp_LLthresh, adcFactor;
      G4double ratio;

      if(runId>=-80000 && runId<=-9484)
      {
        // After TOF HV adjustment, PMT Gain of barrel TOF in MC is set to 5.E5
        // High/Low Threshold for barrel: 500/125 mV
        tmp_HLthresh = m_HLthresh;
        tmp_LLthresh = m_LLthresh;
        adcFactor = 5.89;
      }
      else
      {
        // Before TOF HV adjustment, PMT Gain of barrel TOF in MC is set to 2.5E5
        // High/Low Threshold for barrel: 600/150 mV
        tmp_HLthresh = 600;
        tmp_LLthresh = 150;
        adcFactor = 4.8;
      }
//      if(runId>=-80000 && runId<=-9484)
//      {
//        ratio=2.16*1923.8/2197.8;
//      }
//      else
//      {
//        ratio = 2.11*2437.0/3102.9;
//      }
      tmp_HLthresh = m_tofSimSvc->BarHighThres();
      tmp_LLthresh = m_tofSimSvc->BarLowThres();
      ratio = m_tofSimSvc->BarConstant();

      //get final tdc and adc
      if (max>=tmp_HLthresh)
      {
        for (int i=0;i<m_profBinN;i++)
        {
          if ( profPmt[i][j] >= tmp_LLthresh )
          {
            m_TDC[j] = i*m_timeBinSize + timeSmear +G4RandGauss::shoot(0,0.025); // Adding Electronical Uncertainty of 25ps
// hhliu 20140724, setting tdc-smear for inner and outer separately
	    if( ( runId>=-11396 && runId<=-8093 ) || ( runId>-80000 && runId<=-23463 ) ) {
	      if( scinNb<88 ) {
		m_TDC[j] = i*m_timeBinSize + timeSmear +G4RandGauss::shoot(0,0.055);
	      }
	      else {
		m_TDC[j] = i*m_timeBinSize + timeSmear +G4RandGauss::shoot(0,0.062);
	      }
            }

            if( m_G4Svc->TofSaturationFlag())
            {
              //get ADC[j] using tofQElecSvc
              double x = m_preGain*smearADC[j]*echarge*ratio;
              if (j==0) 
              { 
                m_ADC[j] = m_tofQElecSvc->BQChannel1(scinNb,x);
              }
              else 
              {
                m_ADC[j] = m_tofQElecSvc->BQChannel2(scinNb,x);
              }
            }
            else
              m_ADC[j] = m_preGain*smearADC[j]*echarge*adcFactor;


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
