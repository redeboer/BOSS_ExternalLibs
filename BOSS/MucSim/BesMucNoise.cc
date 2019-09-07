//
//
//
//
#include "GaudiKernel/Bootstrap.h"

#include "BesMucNoise.hh"
#include "G4Trap.hh"
#include "G4VSolid.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include "ReadBoostRoot.hh"
#include "Randomize.hh"
#include "math.h"
#include "strstream"
#include "G4Box.hh"
#include "stdlib.h"


using namespace std;

//const int m_kPart              = 3;
const int BesMucNoise::m_kSegment[m_kPart]  = {4, 8, 4};
const int BesMucNoise::m_kAbsorber[m_kPart] = {9, 9, 9};
const int BesMucNoise::m_kGap[m_kPart]      = {8, 9, 8};
const int BesMucNoise::m_kPanel[m_kPart]    = {4, 3, 4};
//const int m_kGasChamber        = 2;

BesMucNoise * BesMucNoise::fPointer=0;
BesMucNoise * BesMucNoise::Instance(void){
  if(!fPointer)fPointer = new BesMucNoise();
  return fPointer;
}


BesMucNoise::BesMucNoise()
{
 if(fPointer)
  {G4Exception("BesMucNoise constructed twice.");}
  fPointer=this;
  
}

BesMucNoise::~BesMucNoise()
{
  if( m_ptrIdTr != NULL ) delete m_ptrIdTr;

}

void BesMucNoise::Initialize(G4String filename,G4LogicalVolume* logicalMuc,G4String temp)
{
  //m_noiseLevel = 2; // set to 2 temporary. In future, expected to get from cal servise,
  //like m_noiseLevel = m_ptrCalibSvc->getNoiseLevel();
/*
  m_noiseLevel = m_ptrCalibSvc->getLevel();

     if(m_noiseLevel!=2 && m_noiseLevel!=3){
      G4cout<<"ERROR, In BesMucNoise, noise level should be 2 or 3 ..."<<G4endl;
      m_noiseLevel = 2;
     }

     for(int i = 0; i < m_kPart; i++) {
      for(int j = 0; j < m_kSegment[i]; j++) {
      for(int k = 0; k < m_kGap[i]; k++) {
        m_noise[i][j][k] = m_ptrCalibSvc->getBoxCnt(i,j,k);
        int stripNum = 0;
        if(i != 1) stripNum = 64; //endcap;
        else{
          if(k%2 == 0) stripNum = 48;
          if(k%2 == 1 && j != 2) stripNum = 96;
          if(k%2 == 1 && j == 2) stripNum = 112;
        }
        
        for(int strip = 0; strip < stripNum; strip++){
          m_noise_strip[i][j][k][strip] = m_ptrCalibSvc->getStripCnt(i,j,k,strip);
        }
      }
     }
   }  
 */
}

void BesMucNoise::Initialize(G4String filename,G4LogicalVolume* logicalMuc)
{ 
  for(G4int part=0;part<3;part++){
     for(G4int seg=0;seg<8;seg++){ 
        for(G4int gap=0;gap<9;gap++){
	       m_noise[part][seg][gap]=1; //init -1
        }
     }
  }
  //read noise from file
  G4cout<<"filename: "<<filename<<G4endl;
  std::ifstream fin(filename);
  if(!fin){
  G4cout<<"error opening muc_noise data"<<G4endl;
  }
  char buffer[200];
  fin.getline(buffer,200,'\n');  //get info whether add noise or not!
  std::istringstream stringBuf(buffer);
  
  //get strip area from logicalMuc!
  G4int tot_NoDaughter = logicalMuc->GetNoDaughters();
  //G4cout<<"---in noise::Init()---  "<<tot_NoDaughter<<G4endl;
  for(G4int i=0; i<tot_NoDaughter;i++){
    G4LogicalVolume* i_LogicalGap = logicalMuc->GetDaughter(i)->GetLogicalVolume();
    G4String i_GapName = i_LogicalGap->GetName();

    if(i_GapName.find("G")==8){
      G4LogicalVolume* i_LogicalBox = i_LogicalGap->GetDaughter(0)->GetLogicalVolume();
      G4LogicalVolume* i_LogicalStripPlane = i_LogicalBox->GetDaughter(0)->GetLogicalVolume();
      //G4cout<<"---in noise::Init()---  "<<i<<"   "<<i_GapName<<" "<<i_LogicalStripPlane->GetDaughter(1)->GetLogicalVolume()->GetName()<<G4endl;
      G4String strPart = i_GapName.substr(5,1);
      G4String strSeg  = i_GapName.substr(7,1);
      G4String strGap  = i_GapName.substr(9,1);

      std::istrstream partBuf(strPart.c_str(),    strlen(strPart.c_str()));
      std::istrstream segBuf(strSeg.c_str(),      strlen(strSeg.c_str()));
      std::istrstream gapBuf(strGap.c_str(),      strlen(strGap.c_str()));

      G4int part,seg,gap;

      partBuf     >> part;
      segBuf      >> seg;
      gapBuf      >> gap;
      //G4cout<<"-------in noise::Init()----  "<<part<<" "<<seg<<"  "<<gap<<" "<<i_LogicalStripPlane->GetNoDaughters()<<G4endl;
      G4int tot_NoStrip = i_LogicalStripPlane->GetNoDaughters();
      area[part][seg][gap][0]=tot_NoStrip;  //the first element is the total strip number
      G4float tot_Area=0;
      // get width between two strip!
      G4LogicalVolume* i_LogicalStrip1 = i_LogicalStripPlane->GetDaughter(1)->GetLogicalVolume();
      G4LogicalVolume* i_LogicalStrip2 = i_LogicalStripPlane->GetDaughter(2)->GetLogicalVolume();
      G4Box *temp1; G4Box *temp2;

      temp1=(G4Box *)i_LogicalStrip1->GetSolid();temp2=(G4Box *)i_LogicalStrip2->GetSolid();
      G4float Width1 =temp1->GetXHalfLength()*2;G4float Width2 =temp2->GetXHalfLength()*2;
      G4float pos1   =i_LogicalStripPlane->GetDaughter(1)->GetObjectTranslation().x();
      G4float pos2   =i_LogicalStripPlane->GetDaughter(2)->GetObjectTranslation().x();
      if ( (part == 1 && gap%2 != 0) || (part != 1 && gap%2 == 0) ) {
        Width1=temp1->GetYHalfLength()*2; Width2 =temp2->GetYHalfLength()*2;
        pos1   =i_LogicalStripPlane->GetDaughter(1)->GetObjectTranslation().y();
        pos2   =i_LogicalStripPlane->GetDaughter(2)->GetObjectTranslation().y();
      }
      G4float width_between_strip=pos2-pos1-Width1/2-Width2/2;
      // G4cout<<"-----what I want---"<<width_between_strip<<"  "<<pos2<<"  "<<pos1<<"  "<<Width1/2<<"  "<<Width2/2<<G4endl;

      for(G4int j=0;j<tot_NoStrip;j++){
        G4LogicalVolume* i_LogicalStrip = i_LogicalStripPlane->GetDaughter(j)->GetLogicalVolume();
        G4Box *temp;
        temp=(G4Box *)i_LogicalStrip->GetSolid();
        G4float Width =temp->GetXHalfLength()*2;
        G4float Length=temp->GetYHalfLength()*2;
        if ( (part == 1 && gap%2 != 0) || (part != 1 && gap%2 == 0) ) {
          Width =temp->GetYHalfLength()*2;
          Length=temp->GetXHalfLength()*2;
        } //end if
        //       G4cout<<"----in noise::init()----  "<<i_LogicalStrip->GetName()<<" "<<Width<<" "<<Length<<G4endl;
        if(j==0||j==(tot_NoStrip-1)) Width=Width+width_between_strip/2;
        else Width=Width+width_between_strip;
        G4float Strip_Area=fabs(Width*Length);
        tot_Area=tot_Area+Strip_Area;
        area[part][seg][gap][j+1]=tot_Area;
        strip_area[part][seg][gap][j] = Strip_Area;
        //G4cout<<" id: ( "<<part<<" "<<seg<<" "<<gap<<") "<<Strip_Area<<" , w: "<<Width<<" L: "<<Length<<G4endl;
      } //end for(j)
      // unitary

      box_area[part][seg][gap] = tot_Area;

      for(G4int k=1;k<tot_NoStrip+1;k++){
        area[part][seg][gap][k]=area[part][seg][gap][k]/tot_Area;
        //       G4cout<<"----in noise::init---"<<area[part][seg][gap][k]<<G4endl;
      } //end for(k)

    } //end if(gap)
  } //end for(i)

  //  G4cout<<"---in noise::init()---area--"<<G4endl;
  //for(G4int kk=0;kk<96;kk++){
  // G4cout<<area[1][0][0][kk+1]-area[1][0][0][kk]<<" ,";
  // }

  // init MucIdTransform and const pointer
  m_ptrIdTr = new MucIdTransform();
  CheckCalibSvc();
  InitProb();
}

void BesMucNoise::CheckCalibSvc()
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("MucCalibConstSvc", m_ptrCalibSvc, true);
 
  if( sc != StatusCode::SUCCESS){
          G4cout<< "Can not use MucCalibConstSvc!" << G4endl;
  }
}

void BesMucNoise::InitProb()
{
  m_HitMean = 0.08;
  int NHIT = 20;
  
  for( int i=0; i<NHIT; i++)  {
    m_Prob[i][0] = m_Prob[i][1] = 0.;
  }             

  double sum = 0.;
  double EU  = TMath::Power(TMath::Exp(1.0), -m_HitMean);
  for( int i=0; i<NHIT; i++)
  {       
    m_Prob[i][0] = EU*TMath::Power(m_HitMean, i)/TMath::Factorial(i);
    sum += m_Prob[i][0]; 
    m_Prob[i][1] = sum;   
    //G4cout << i << "\tProb:\t" << m_Prob[i][0] << "\tSumProb:\t" << m_Prob[i][1] << G4endl;
  } 
}

G4int BesMucNoise::AddNoise(int model, BesMucHitsCollection* aMucHitCollection,BesMucHitsCollection* aMucHitList)
{
  G4int noiseNum = 0;
  if( model == 1 ) noiseNum = NoiseByCnt(aMucHitCollection, aMucHitList);
  else             noiseNum = NoiseByNosRatio(aMucHitCollection, aMucHitList);
  //G4cout << "Noise hit(s) produced:\t" << noiseNum << "\tby model:\t" << model << G4endl;
  return noiseNum;
  
}

G4int BesMucNoise::NoiseByCnt(BesMucHitsCollection* MucHitCollection,BesMucHitsCollection* MucHitList)
{
  G4int noiseNum = 0;
  m_noiseLevel = m_ptrCalibSvc->getLevel();

    for(int i = 0; i < m_kPart; i++) {
      for(int j = 0; j < m_kSegment[i]; j++) {
        for(int k = 0; k < m_kGap[i]; k++) {
          if(m_noiseLevel == 2)
          {
            G4int hitNum = NoiseSampling( m_noiseLevel, i, j, k, 0 );
            for(G4int ii=0;ii<hitNum;ii++)
            {
              G4int strip = GetStripNo(i,j,k);
              BesMucHit *noiseHit=new BesMucHit(i, j, k, strip, -1, -1);
              BesMucHit *noiseHit2=new BesMucHit(i, j, k, strip, -1, -1);
              bool noiseHitExist = false;
              noiseHitExist = IsExist(noiseHit, MucHitList);
              MucHitCollection->insert(noiseHit);
              if(!noiseHitExist) {
                MucHitList->insert(noiseHit2);
                noiseNum += 1;
              //G4cout<<"-------insert-------  "<<" "<<noiseHit2->GetStrip()<<" "<<i<<" "<<j<<" "<<k<<" "<<l<<" "<<m<<G4endl;
              }
              else delete noiseHit2;
            } // hitNum
          }  //box level
          
          if(m_noiseLevel == 3)
          {
            int stripNum = m_ptrIdTr->GetStripMax(i, j, k);
            for(int strip = 0; strip < stripNum; strip++)
            {
              G4int hitNum = NoiseSampling( m_noiseLevel, i, j, k, strip );
              if(hitNum > 0){
                BesMucHit *noiseHit=new BesMucHit(i, j, k, strip, -1, -1);
                BesMucHit *noiseHit2=new BesMucHit(i, j, k, strip, -1, -1);
                bool noiseHitExist = false;
                noiseHitExist = IsExist(noiseHit, MucHitList);
                MucHitCollection->insert(noiseHit);
                if(!noiseHitExist) {
                  MucHitList->insert(noiseHit2);
                  noiseNum += 1;
                  //G4cout<<"-------insert-------  "<<" "<<noiseHit2->GetStrip()<<" "<<i<<" "<<j<<" "<<k<<" "<<l<<" "<<m<<G4endl;
                }
                else delete noiseHit2;
              }
            } // stripNum
          } // strip level
        } // part
      } // segment
    } // layer

  return noiseNum;
}

G4int BesMucNoise::NoiseByNosRatio(BesMucHitsCollection* MucHitCollection,BesMucHitsCollection* MucHitList)
{
  G4int noiseNum = 0;
  G4float random = 0;

  //sample noise hit number;
  random = G4UniformRand();
  for(int i=0; i<20; i++) {
    if(random<m_Prob[i][1]) {noiseNum = i; break;}
  } 
  //G4cout << "Random for noiseNum:\t" << random << "\tProbSum:\t" << m_Prob[noiseNum][1] << "\t" << noiseNum << G4endl;

  int prt, seg, lay, str, tmp_strip;
  G4float nosRatio = 0.;
  
  for(int i=0; i<noiseNum; i++)
  {
    do{
      random = G4UniformRand();
      tmp_strip = TMath::Nint(random*STRIP_MAX); //9152 
      m_ptrIdTr->SetStripPos(tmp_strip, &prt, &seg, &lay, &str);
      nosRatio = m_ptrCalibSvc->getNosRatio(prt, seg, lay, str);  
      random = G4UniformRand();
      if( random<nosRatio ) break; 
    }while( 1 );
    
    //G4cout<<"Strip:\t"<<prt<<"\t"<<seg<<"\t"<<lay<<"\t"<<str<<"\t"<<random<<" "<<nosRatio<<G4endl;

    BesMucHit *noiseHit   = new BesMucHit(prt, seg, lay, str, -1, -1);
    BesMucHit *noiseHit2  = new BesMucHit(prt, seg, lay, str, -1, -1);

    bool noiseHitExist = false;
    noiseHitExist = IsExist(noiseHit, MucHitList);
    MucHitCollection->insert(noiseHit);
    if(!noiseHitExist) MucHitList->insert(noiseHit2);
    else delete noiseHit2;
  } // noiseNum;

  return noiseNum;
}

bool BesMucNoise::IsExist(BesMucHit* aNoiseHit, BesMucHitsCollection* aMucHitList) 
{
  bool isExist = false;
  G4int n_hit = aMucHitList->entries();
  for(G4int iNoise=0;iNoise<n_hit;iNoise++) 
  {
    if ( aNoiseHit->GetTrackIndex()%1000 == (*aMucHitList)[iNoise]->GetTrackIndex()%1000 &&
      aNoiseHit->GetPart()  == (*aMucHitList)[iNoise]->GetPart() &&
      aNoiseHit->GetSeg()   == (*aMucHitList)[iNoise]->GetSeg()  &&
      aNoiseHit->GetGap()   == (*aMucHitList)[iNoise]->GetGap()  &&
      aNoiseHit->GetStrip() == (*aMucHitList)[iNoise]->GetStrip() )
    {
      isExist = true;
      break;
    }
  }
  
  return isExist;
}

G4int BesMucNoise::NoiseSampling(int level, int prt, int seg, int lay, int strip)
{
  G4int hitNum = 0;
  G4double lambda;
  //must get the time of this event
  G4double t=800;  //800ns  temporary!
  G4double e=2.71828182845904590;
  
  if( level == 2 )  
    lambda = m_ptrCalibSvc->getBoxCnt(prt,seg,lay) * box_area[prt][seg][lay] * 1E-2 * 1E-9; //1E-2:mm2->cm2 1E-9:ns
  else if( level == 3) 
    lambda = m_ptrCalibSvc->getStripCnt(prt,seg,lay,strip) * strip_area[prt][seg][lay][strip] * 1E-2 * 1E-9; //1E-2:mm2->cm2 1E-9:ns
  else
    lambda = 0.;
  
  //sample noise hit number;
  G4float random=G4UniformRand();  //***use other random
  G4double prob = 0;
  do{
    prob=prob+pow(e,-lambda*t)*pow(lambda*t,hitNum)/Factorial(hitNum);
    if(random<prob) break;
    hitNum++;
  }while(1);
  
  // G4cout<<" hitNum: "<<hitNum<<G4endl;
  return hitNum; 
}

G4float BesMucNoise::Factorial(G4int i)
{
  G4float fact=1;
  if(i==0||i==1)return 1;
  else{
    for(G4int ii=2;ii<=i;ii++){
      fact=fact*ii;}
    return fact;
  }

}

G4int BesMucNoise::GetStripNo(G4int part,G4int seg,G4int gap)
{ G4int stripno;
  //G4float random=G4UniformRand();//***use other random
  G4float random=(rand()%100000)/100000.0;
  if(part==1){   //berrel
  G4float width=area[part][seg][gap][3]-area[part][seg][gap][2];
  stripno=G4int((random-area[part][seg][gap][1])/width)+2;
  if(stripno<1)stripno=1;
  if(stripno>111)stripno=111;
 // G4cout<<"---in noise::GetStripNo()---stripno= "<<stripno<<"  "<<(random-area[part][seg][gap][1])/width<<G4endl;
  G4int step = IsNearestStrip(stripno,part,seg,gap,random);
  while(step!=0) {
   // G4cout<<"---in noise::GetStripNo()---while "<<G4endl;
    stripno += step;
    step = IsNearestStrip(stripno,part,seg,gap,random);
    }//endl while
  stripno--;  //the first element is total strip number
  return stripno;
  }
  else{     //endcap
  G4int max,min,mid,pass;
  min=1;
  max=area[part][seg][gap][0]; 
  mid=G4int((min+max)/2); 
  //G4cout<<"---in noise first---"<<min<<" "<<mid<<" "<<max<<G4endl; 
  do{
  //  G4cout<<"-----in noise---"<<random<<" "<<area[part][seg][gap][min]<<" "<<area[part][seg][gap][mid]<<" "<<area[part][seg][gap][max]<<G4endl;
    pass=0;
    if(random>area[part][seg][gap][mid]){
      min=mid;
      mid=G4int((min+max)/2); 
      }
      else if(random<area[part][seg][gap][mid-1]){
      if(random<area[part][seg][gap][1]){
        pass=1; mid=1;max=1;
        }else{
        max=mid-1;
        mid=G4int((min+max)/2);
        }
      }else{pass=1;}

    if(min==mid)mid=max;
  // G4cout<<"-----in noise---"<<min<<" "<<mid<<" "<<max<<G4endl;
  }while(pass==0&&(max>mid&&mid>min));
  //G4cout<<"-----in noise---  "<<mid-1<<G4endl; 
  return mid-1;
  
  }

}

G4int BesMucNoise::IsNearestStrip(G4int stripno,G4int part,G4int seg,G4int gap,G4float random)
{
  if(stripno==1){
    return 0;
  }else{
    if(area[part][seg][gap][stripno]!=0){
    //   G4cout<<"--in noise::IsNearestStrip()--  "<<area[part][seg][gap][stripno]<<" "<<area[part][seg][gap][stripno-1]<<" "<<random<<G4endl;
       if(random<=area[part][seg][gap][stripno]&&random>area[part][seg][gap][stripno-1])return 0;
       if(random<=area[part][seg][gap][stripno-1]) return -1;
       if(random>area[part][seg][gap][stripno]) return 1;
    }else{
       return -1;
    }
  } //end else
}
