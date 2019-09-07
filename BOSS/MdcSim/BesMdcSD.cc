//#include "DedxPar.hh"
#include "BesMdcSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include "ReadBoostRoot.hh"
#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"
#include "CalibDataSvc/ICalibRootSvc.h"
#include "CalibData/Dedx/DedxCalibData.h"
#include "CalibData/Dedx/DedxSimData.h"
#include "GaudiKernel/DataSvc.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2D.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SmartDataPtr.h"

#include <iostream>


BesMdcSD::BesMdcSD(G4String name)
:BesSensitiveDetector(name)
{
    collectionName.insert("BesMdcHitsCollection");
    collectionName.insert("BesMdcTruthCollection");
  
    mdcGeoPointer=BesMdcGeoParameter::GetGeo();
    mdcCalPointer=new BesMdcCalTransfer;
  
    IMdcGeomSvc* ISvc;
    StatusCode sc=Gaudi::svcLocator()->service("MdcGeomSvc", ISvc);
    if (!sc.isSuccess()) 
      std::cout<<"BesMdcSD::Could not open MdcGeomSvc"<<std::endl;
    mdcGeomSvc=dynamic_cast<MdcGeomSvc *>(ISvc);
  
    IG4Svc* tmpSvc;
    sc=Gaudi::svcLocator()->service("G4Svc", tmpSvc);
    if (!sc.isSuccess())
      G4cout <<" MdcSD::Error,could not open G4Svc"<<G4endl;
    m_G4Svc=dynamic_cast<G4Svc *>(tmpSvc); 
  
    if(m_G4Svc->GetMdcDedxFlag()==1){
      G4cout <<" MdcSD: Use sampled dedx instead of Geant4 value"<<G4endl;
      dedxFuncInti();
    }

    ////dedx sim
    
    //get DedxSimData
    std::string dedxTDSPath = "/Calib/DedxSim";
    IDataProviderSvc* pCalibDataSvc;
    sc = Gaudi::svcLocator()->service("CalibDataSvc", pCalibDataSvc, true);
    if (!sc.isSuccess())
        std::cout << "BesMdcSD::Could not open CalibDataSvc" << std::endl;
    m_calibDataSvc = dynamic_cast<CalibDataSvc*>(pCalibDataSvc);
    if (!sc.isSuccess())
    {
        std::cout << "Could not get CalibDataSvc"
                  << std::endl;
    }
    SmartDataPtr<CalibData::DedxSimData> pDedxSimData(m_calibDataSvc, dedxTDSPath);
    m_version = pDedxSimData->getVersion();
    m_numDedxHists = pDedxSimData->gethistNo();
    m_numBg = pDedxSimData->getRangeNo();
    if (m_version == 0) m_numTheta = 10;
    else m_numTheta = pDedxSimData->getThetaNo();
    m_dedx_hists = new TH1F[m_numDedxHists];
    for (G4int i = 0; i < m_numBg; i++)
    {
        m_bgRange.push_back(pDedxSimData->getRange(i));
    }
    for (G4int i = 0; i < m_numDedxHists; i++)
    {
        m_dedx_hists[i] = pDedxSimData->getHist(i);
    }

    //get CalibCurSvc
    IDedxCurSvc* tmp_dedxCurSvc;
    sc = Gaudi::svcLocator()->service("DedxCurSvc", tmp_dedxCurSvc, true);
    if (!sc.isSuccess())
    {
        std::cout << "Could not get DedxCurSvc"
                  << std::endl;
    }
    m_pDedxCurSvc = dynamic_cast<DedxCurSvc*>(tmp_dedxCurSvc);

    if(m_G4Svc->MdcRootFlag())
    {
      m_tupleMdc = m_G4Svc->GetTupleMdc();
      sc = m_tupleMdc->addItem("betaGamma",m_betaGamma);
      sc = m_tupleMdc->addItem("fitval",m_fitval);
      sc = m_tupleMdc->addItem("dedx",m_dedx);
      sc = m_tupleMdc->addItem("de",m_de);
      //sc = m_tupleMdc->addItem("length",m_length);
      //sc = m_tupleMdc->addItem("random",m_random);
      sc = m_tupleMdc->addItem("charge", m_charge);
      sc = m_tupleMdc->addItem("costheta", m_costheta);
    }
}

BesMdcSD::~BesMdcSD(){
  delete []m_dedx_hists;
}

void BesMdcSD::Initialize(G4HCofThisEvent* HCE)
{
  hitsCollection = new BesMdcHitsCollection
                          (SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection( HCID, hitsCollection ); 
  G4int i,j;
  for(i=0; i<43;i++){
    for(j=0;j<288;j++){
      hitPointer[i][j]=-1;
      truthPointer[i][j]=-1;
    }
  }
}

//for MC Truth
void BesMdcSD::BeginOfTruthEvent(const G4Event* evt)
{  
  truthCollection = new BesMdcHitsCollection
                          (SensitiveDetectorName,collectionName[1]); 
  //  G4cout<<" begin event "<<evt->GetEventID()<<G4endl;
}

void BesMdcSD::EndOfTruthEvent(const G4Event* evt)
{  static G4int HLID=-1;
  if(HLID<0)
  {
    HLID = G4SDManager::GetSDMpointer()->
		GetCollectionID(collectionName[1]); 
  }
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  HCE->AddHitsCollection(HLID,truthCollection);
}

G4bool BesMdcSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  G4Track* gTrack = aStep->GetTrack() ;

  G4double globalT=gTrack->GetGlobalTime();//Time since the event in which the track belongs is created
  if(isnan(globalT)){
    G4cout<<"MdcSD:error, globalT is nan "<<G4endl;
    return false;
  }
  if(globalT > 2000)return false; //MDC T window is 2 microsecond 

  //skip neutral tracks
  G4int charge = gTrack->GetDefinition()->GetPDGCharge();
  if (charge == 0) return false;

  //skip no energy deposit tracks
  G4double stepLength=aStep->GetStepLength();
  if(stepLength==0){
    //    G4cout<<"step length equal 0!!"<<G4endl;
    return false;
  }

  G4double edep = aStep->GetTotalEnergyDeposit() / stepLength;
  if(edep==0.) return false;

  // get position of the track at the beginning and at the end of step
  G4StepPoint* prePoint  = aStep->GetPreStepPoint() ;
  G4StepPoint* postPoint = aStep->GetPostStepPoint() ;

  //get position coordinate
  G4ThreeVector pointIn  = prePoint->GetPosition();
  G4ThreeVector pointOut = postPoint->GetPosition();

  // get physical volumes
  const G4VTouchable *touchable = prePoint->GetTouchable();
  G4VPhysicalVolume *volume = touchable->GetVolume(0);

  G4double driftD = 0;
  G4double driftT = 0;
  G4double edepTemp = 0;
  G4double lengthTemp = 0;
  G4int cellId=0;
  G4int layerId = touchable->GetVolume(1)->GetCopyNo();
  if(volume->IsReplicated()){
    cellId  = touchable->GetReplicaNumber();
  }else{
    cellId=touchable->GetVolume(0)->GetCopyNo();
  }
  if(layerId==18&&(cellId==27||cellId==42))return false; // no sense wire 

  if(ReadBoostRoot::GetMdc() == 2) { //gdml
    //layerId  0-35 -> CopyNo 0-35 in gdml 
    //layerId 36-42 -> CopyNo (36,37),(38,39),...(48,49)    
    if(layerId >= 36) layerId = 36 + (layerId-36)/2;
  }

  G4double halfLayerLength=mdcGeomSvc->Layer(layerId)->Length()/2.;
  if(((fabs(pointIn.z())-halfLayerLength)>-7.)
     &&((fabs(pointOut.z())-halfLayerLength)>-7.))return false;//Out sensitive area

  G4int trackID= gTrack->GetTrackID(); //G4 track ID of current track.
  G4int truthID, g4TrackID;
  GetCurrentTrackIndex(truthID, g4TrackID); //ID of current primary track.
  
  G4double theta=gTrack->GetMomentumDirection().theta();

  G4ThreeVector hitPosition=0;
  G4double transferT=0;
  driftD  = Distance(layerId,cellId,pointIn,pointOut,hitPosition,transferT);

  G4double posPhi, wirePhi;
  posPhi=hitPosition.phi();//from -pi to pi
  if(posPhi<0)posPhi += 2*pi;
  wirePhi=mdcGeoPointer->SignalWire(layerId,cellId).Phi(hitPosition.z());//from 0 to 2pi

  G4int posFlag;
  if(posPhi<=wirePhi){
    posFlag    = 0;
  }else{
    posFlag    = 1;
  }
  // if x axis is between pos and wire, phi will has a jump of one of them.
  if(posPhi < 1. && wirePhi > 5.)posFlag = 1;
  if(posPhi > 5. && wirePhi < 1.)posFlag = 0;

  G4ThreeVector hitLine=pointOut-pointIn;
  G4double enterAngle=hitLine.phi()-wirePhi;
  while(enterAngle<-pi/2.)enterAngle+=pi;
  while(enterAngle>pi/2.)enterAngle-=pi;

  if(m_G4Svc->GetMdcDedxFlag()==1){
     G4double betaGamma=aStep->GetPreStepPoint()->GetBeta() * aStep->GetPreStepPoint()->GetGamma();
     if(betaGamma<0.01)return false;//too low momentum
     //if (betaGamma < 10.0) betaGamma = 10.0;

     G4double eCount=dedxSample(betaGamma, charge, theta);
     edep=eCount;
  }

  BesMdcHit* newHit = new BesMdcHit();
  newHit->SetTrackID(truthID);
  //newHit->SetTrkID(trackID); 
  newHit->SetLayerNo(layerId);
  newHit->SetCellNo(cellId);
  newHit->SetEdep(edep);
  newHit->SetPos(hitPosition);
  newHit->SetDriftD(driftD);
  newHit->SetTheta(theta);
  newHit->SetPosFlag(posFlag);
  newHit->SetEnterAngle(enterAngle);

  //Transfer hit pointer to BesMdcCalTransfer
  mdcCalPointer->SetHitPointer(newHit);

  driftT=mdcCalPointer->D2T(driftD);
  globalT+=transferT;
  driftT+=globalT;
  
  newHit->SetDriftT (driftT);
  newHit->SetGlobalT(globalT);

  if (hitPointer[layerId][cellId] == -1) {
      hitsCollection->insert(newHit);
      G4int NbHits = hitsCollection->entries();
      hitPointer[layerId][cellId]=NbHits-1;
  } 
  else
  {
      G4int pointer=hitPointer[layerId][cellId]; 
      if (g4TrackID == trackID) {
          G4double preDriftT=(*hitsCollection)[pointer]->GetDriftT();
      }
      
      G4double preDriftT = (*hitsCollection)[pointer]->GetDriftT();
      if (driftT < preDriftT) {   
          (*hitsCollection)[pointer]->SetTrackID(truthID);
          //(*hitsCollection)[pointer]->SetTrkID(trackID);
          (*hitsCollection)[pointer]->SetDriftD(driftD);
          (*hitsCollection)[pointer]->SetDriftT(driftT);
          (*hitsCollection)[pointer]->SetPos(hitPosition);
          (*hitsCollection)[pointer]->SetGlobalT(globalT);
          (*hitsCollection)[pointer]->SetTheta(theta);
          (*hitsCollection)[pointer]->SetPosFlag(posFlag);
          (*hitsCollection)[pointer]->SetEnterAngle(enterAngle);
      }    
   
      delete newHit;
  }

  //for mc truth 
  if(truthCollection){
    if(g4TrackID==trackID){ //This track is the primary track & will appear in MC truth
      G4int pointer=truthPointer[layerId][cellId];  
      if(pointer==-1){
        BesMdcHit* truthHit = new BesMdcHit();
        truthHit->SetTrackID  (truthID);
        truthHit->SetLayerNo(layerId);
        truthHit->SetCellNo(cellId);
        truthHit->SetEdep     (edep);
        truthHit->SetPos      (hitPosition);
        truthHit->SetDriftD (driftD);
        truthHit->SetDriftT (driftT);
        truthHit->SetGlobalT(globalT);
        truthHit->SetTheta(theta);
        truthHit->SetPosFlag(posFlag);
        truthHit->SetEnterAngle(enterAngle);

        truthCollection->insert(truthHit);
        G4int NbHits = truthCollection->entries();
        truthPointer[layerId][cellId]=NbHits-1;
      } 
      else {
        if(truthID == (*truthCollection)[pointer]->GetTrackID()){
            G4double preDriftT=(*truthCollection)[pointer]->GetDriftT();
            if(driftT<preDriftT){
                (*truthCollection)[pointer]->SetDriftD(driftD);
                (*truthCollection)[pointer]->SetDriftT(driftT);
                (*truthCollection)[pointer]->SetPos(hitPosition);
                (*truthCollection)[pointer]->SetGlobalT(globalT);
                (*truthCollection)[pointer]->SetTheta(theta);
                (*truthCollection)[pointer]->SetPosFlag(posFlag);
                (*truthCollection)[pointer]->SetEnterAngle(enterAngle);
	        }
            edepTemp=(*truthCollection)[pointer]->GetEdep();
            (*truthCollection)[pointer]->SetEdep(edepTemp+edep);
            } else {
                BesMdcHit* truthHit = new BesMdcHit();
                truthHit->SetTrackID  (truthID);
                truthHit->SetLayerNo(layerId);
                truthHit->SetCellNo(cellId);
                truthHit->SetEdep(edep);
                truthHit->SetPos(hitPosition);
                truthHit->SetDriftD (driftD);
                truthHit->SetDriftT (driftT);
                truthHit->SetGlobalT(globalT);
                truthHit->SetTheta(theta);
                truthHit->SetPosFlag(posFlag);
                truthHit->SetEnterAngle(enterAngle);

                truthCollection->insert(truthHit);
                G4int NbHits = truthCollection->entries();
                truthPointer[layerId][cellId]=NbHits-1;
            }
      }
    }
  }

  //newHit->Print();
//  newHit->Draw();

  return true;
}

void BesMdcSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
  hitsCollection->PrintAllHits();
  /*
     G4int NbHits = hitsCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
            << " hits in the MDC chambers: " << G4endl;
     for (G4int i=0;i<NbHits;i++) (*hitsCollection)[i]->Print();
  */
  } 
}

G4double BesMdcSD::Distance(G4int layerId, G4int cellId, G4ThreeVector pointIn, G4ThreeVector pointOut,G4ThreeVector& hitPosition,G4double& transferT)
{
  //For two lines r=r1+t1.v1 & r=r2+t2.v2 
  //the closest approach is d=|(r2-r1).(v1 x v2)|/|v1 x v2|
  //the point where closest approach are
  //t1=(v1 x v2).[(r2-r1) x v2]/[(v1 x v2).(v1 x v2)]
  //t2=(v1 x v2).[(r2-r1) x v1]/[(v1 x v2).(v1 x v2)]
  //if v1 x v2=0 means two lines are parallel
  //d=|(r2-r1) x v1|/|v1| 

  G4double t1,distance,dInOut,dHitIn,dHitOut;
  //Get wirepoint @ endplate
  G4ThreeVector east=mdcGeomSvc->Wire(layerId,cellId)->Backward();
  G4ThreeVector west=mdcGeomSvc->Wire(layerId,cellId)->Forward();
  G4ThreeVector wireLine=east-west;
  G4ThreeVector hitLine=pointOut-pointIn;

  G4ThreeVector hitXwire=hitLine.cross(wireLine);
  G4ThreeVector wire2hit=east-pointOut;
  //Hitposition is the position on hit line where closest approach 
  //of two lines, but it may out the area from pointIn to pointOut
  if(hitXwire.mag()==0){
    distance=wireLine.cross(wire2hit).mag()/wireLine.mag();
    hitPosition=pointIn;
  }else{
    t1=hitXwire.dot(wire2hit.cross(wireLine))/hitXwire.mag2();
    hitPosition=pointOut+t1*hitLine;
    
    dInOut=(pointOut-pointIn).mag();
    dHitIn=(hitPosition-pointIn).mag();
    dHitOut=(hitPosition-pointOut).mag();
    if(dHitIn<=dInOut && dHitOut<=dInOut){ //Between point in & out
      distance=fabs(wire2hit.dot(hitXwire)/hitXwire.mag());
    }else if(dHitOut>dHitIn){ // out pointIn
      distance=wireLine.cross(pointIn-east).mag()/wireLine.mag();
      hitPosition=pointIn;
    }else{ // out pointOut
      distance=wireLine.cross(pointOut-east).mag()/wireLine.mag(); 
      hitPosition=pointOut;
    }
  }

  //Calculate signal transferT on wire
  G4double halfLayerLength=mdcGeomSvc->Layer(layerId)->Length()/2.;
  G4double halfWireLength=wireLine.mag()/2.;
  G4double transferZ=0;
  if(layerId%2==0){
    transferZ=halfLayerLength+hitPosition.z(); //West readout
  }else{
    transferZ=halfLayerLength-hitPosition.z(); //East readout
  }
  if(layerId<8){
    transferT=transferZ*halfWireLength/halfLayerLength/220;
  }else{
    transferT=transferZ*halfWireLength/halfLayerLength/240;
  }

  return distance;

}

void BesMdcSD::dedxFuncInti(void)
{
    dEdE_mylanfunc = new TF1("dEdE_mylanfunc",
                             "[3]*TMath::Exp([2]*((x[0]-[0])/[1]+TMath::Exp(-1*((x[0]-[0])/[1]))))",
                             0,
                             7500);
    //dEdE_mylanfunc->SetParameters(2009.35,559.776,-1.0932,6327.38);
    dEdE_mylanfunc->SetParNames("MPV","Sigma","constant1","constant2");
}

G4double BesMdcSD::dedxSample(G4double betagamma, G4double charge, G4double theta)
{
    G4double x = betagamma;
    G4double fitval = GetValDedxCurve(x, charge);
    if(fitval <= 0)return 0;
   
    G4double random1, random2, dedx1, dedx2, de;
    G4double standard1, standard2, beta_temp1, beta_temp2;
    G4double dedx = -1;

    G4int range_idx, bg_idx, angle_idx, charge_idx, hist_idx;
    range_idx = GetBetagammaIndex(betagamma);
    angle_idx = GetAngleIndex(theta);
    charge_idx = GetChargeIndex(charge);
    
    if (range_idx == -1)
    {
        while (dedx <= 0)
        {
            bg_idx = 0;
            hist_idx = bg_idx * 2 * m_numTheta + angle_idx * 2 + charge_idx;
            random1 = m_dedx_hists[hist_idx].GetRandom();
            beta_temp1 = (m_bgRange[0] + m_bgRange[1]) / 2;
            standard1 = GetValDedxCurve(beta_temp1, charge); 
            dedx = random1 + fitval - standard1;
        }
    }
    else if (range_idx == m_numBg - 1)
    {
        while (dedx <= 0)
        {
            bg_idx = (G4int)(range_idx / 2);
            hist_idx = bg_idx * 2 * m_numTheta + angle_idx * 2 + charge_idx;
            random1 = m_dedx_hists[hist_idx].GetRandom();
            beta_temp1 = (m_bgRange[m_numBg - 2] +
                          m_bgRange[m_numBg - 1]) / 2;
            standard1 = GetValDedxCurve(beta_temp1, charge);
            dedx = random1 + fitval - standard1;
        }     
    }
    else
    {
        //case 1: Given betagamma fall in one histograph range
        if (range_idx % 2 == 0)
        {
            while (dedx <= 0)
            {
                bg_idx = (G4int)(range_idx / 2);
                hist_idx = bg_idx * 2 * m_numTheta + angle_idx * 2 + charge_idx;
                random1 = m_dedx_hists[hist_idx].GetRandom();
                beta_temp1 = (m_bgRange[range_idx] + 
                              m_bgRange[range_idx + 1]) / 2;
                standard1 = GetValDedxCurve(beta_temp1, charge); 
                dedx1 = random1 + fitval - standard1;
                dedx = dedx1;
            }
        }
        //case 2: Given betagamma fall in interval between
        //        two histographs.
        else
        {
            while (dedx <= 0)
            {
                //standard1
                beta_temp1 = (m_bgRange[range_idx - 1] + 
                              m_bgRange[range_idx]) / 2;
                standard1 = GetValDedxCurve(beta_temp1, charge);

                //stardard2
                beta_temp2 = (m_bgRange[range_idx + 1] + 
                              m_bgRange[range_idx + 2]) / 2;
                standard2 = GetValDedxCurve(beta_temp2, charge);
               
                //random1 
                bg_idx = (G4int)(range_idx / 2);
                hist_idx = bg_idx * 2 * m_numTheta + angle_idx * 2 + charge_idx;
                random1 = m_dedx_hists[hist_idx].GetRandom();

                //random2
                bg_idx++;
                hist_idx = bg_idx * 2 * m_numTheta + angle_idx * 2 + charge_idx;
                random2 = m_dedx_hists[hist_idx].GetRandom();

                //combine dedx1 and dedx2
                dedx1 = random1 + fitval - standard1;
                dedx2 = random2 + fitval - standard2;
                dedx = (dedx2 * (x - m_bgRange[range_idx]) + 
                        dedx1 * (m_bgRange[range_idx + 1] - x)) / 
                       (m_bgRange[range_idx + 1] - m_bgRange[range_idx]);
            }
        }
    }
   
   
    de= dedx;// * y/10./1.5;// stepLength unit is mm in Geant4, cm in Rec.& Cal. software 
                         // dedx counts *1.5 in dedx Cal.
   
    if(m_G4Svc->MdcRootFlag())
    {
       m_betaGamma= x;
       m_fitval= fitval;
       //m_trackId = trackId; 
       //m_layer = layerId; 
       //m_wire  = cellId; 
       //m_random= random;
       m_dedx= dedx;
       m_de= de;
       //m_length=y;
       m_charge = charge;
       m_costheta = cos(theta);
       m_tupleMdc->write(); 
    }
    return de;
}

/*-----------------------------------------------------
Func: GetValDedxCurve
Pre: A betagamma is given.
Post: Return dE/dx value from betagamma~dE/dx curve.
-----------------------------------------------------*/
G4double BesMdcSD::GetValDedxCurve(G4double x, G4double charge)
{
    G4int dedxflag = -1;
    G4int size = -1;
    G4double A = 0.;
    G4double B = 0.;
    G4double C = 0.;
    std::vector<G4double> par;
    G4double val;
    G4int index = -1;

    par.clear();
    dedxflag = m_pDedxCurSvc->getCurve(0);
    size = m_pDedxCurSvc->getCurveSize();
    for (G4int i = 1; i < size; i++) {
        par.push_back(m_pDedxCurSvc->getCurve(i));
    }

    if (x < 4.5)
        A = 1;
    else if(x < 10)
        B = 1;
    else
        C = 1;
    
    G4double partA = par[0] * pow(sqrt(x * x + 1), par[2]) / pow(x, par[2]) *
                  (par[1] - par[5] * log(pow(1 / x, par[3]))) -
                   par[4] + exp(par[6] + par[7] * x);
    G4double partB = par[8] * pow(x, 3) + par[9] * pow(x, 2) + par[10] * x + par[11];
    G4double partC = - par[12] * log(par[15] + pow(1 / x, par[13])) + par[14];
    
    val = 550 * (A * partA + B * partB + C * partC);
    return val;
    
}

/*-----------------------------------------------------
Func: GetBetagammaIndex
Pre : A betagamma of a track is given. 
Post: Return index of the betagamma range.
-----------------------------------------------------*/
G4int BesMdcSD::GetBetagammaIndex(G4double bg)
{
    if (bg < m_bgRange[0]) return -1;
    for (G4int i = 0; i < m_numBg - 1; i++)
    {
        if (bg > m_bgRange[i] && bg < m_bgRange[i + 1])
        {
            return i;
        }
    }
    if (bg > m_bgRange[m_numBg - 1])
        return (m_numBg - 1);
    return -1;
}

/*-----------------------------------------------------
Func: GetAngleIndex
Pre : A theta of a track is given.
Post: version 0 - Return index of the angle (|cos(theta)| in 10 bins),
      version 1 - Return index of the angle ( cos(theta)  in m_numTheta bins).
-----------------------------------------------------*/
G4int BesMdcSD::GetAngleIndex(G4double theta)
{   
    if (m_version == 0) {
        if (fabs(cos(theta)) >= 0.93) return 9;
        return (G4int)(fabs(cos(theta)) * 10 / 0.93);
    }
    else {
        G4double cos_min = -0.93;
        G4double cos_max = 0.93;
        G4int nbin = m_numTheta;
        G4double cos_step = (cos_max - cos_min) / nbin;

        if (cos(theta) < cos_min) return 0;
        else if (cos_min < cos(theta) && cos(theta) < cos_max) {
            return (G4int)((cos(theta) - cos_min) / cos_step);
        }
        else return nbin - 1;
    }
}

/*-----------------------------------------------------
Func: GetChargeIndex
Pre : A charge of a track is given.
Post: Return index of charge (pos->0 ~ neg->1).
-----------------------------------------------------*/
G4int BesMdcSD::GetChargeIndex(G4int charge)
{
    if (charge > 0)  return 0;
    if (charge == 0) return -1;  // warning: -1 is illegal, for charged tracks are expected.
    if (charge < 0)  return 1;
    return -1;
}

