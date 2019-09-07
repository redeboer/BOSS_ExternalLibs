
#include "EmcRec/EmcRecShowerEnergy.h"
#include "EmcRec/EmcRecNeighbor.h"
#include "EmcRec/EmcRecParameter.h"

void EmcRecShowerEnergy::Energy(RecEmcShower& aShower)
{
  RecEmcFractionMap::const_iterator cit;
  RecEmcEnergy e1=0;
  RecEmcEnergy e9=0;
  RecEmcEnergy e25=0;
  RecEmcEnergy elepton=0;
  RecEmcEnergy eall=0;

  EmcRecNeighbor nhb;

  RecEmcID CellId=aShower.getShowerId();
  int module=EmcID::barrel_ec(CellId);
  RecEmcIDVector NearCell=nhb.GetNeighbors(CellId);
  RecEmcIDVector NextNearCell=nhb.GetNextNeighbors(CellId);
  RecEmcIDVector tmpNearCell;
  RecEmcIDVector tmpNextNearCell;
  i_RecEmcIDVector pNearCell;
  i_RecEmcIDVector pNextNearCell;
  vector<RecEmcEnergy> eVec;
  vector<RecEmcEnergy>::const_iterator ciVec;

  tmpNearCell.push_back(CellId);
  tmpNextNearCell.push_back(CellId);

  cit=aShower.Find(CellId);
  //int time_seed = cit->second.getTime();
  e1=(cit->second.getEnergy())*(cit->second.getFraction());
  e9+=(cit->second.getEnergy())*(cit->second.getFraction());
  e25+=(cit->second.getEnergy())*(cit->second.getFraction());

  //e3x3
  for(pNearCell=NearCell.begin();
      pNearCell!=NearCell.end();
      pNearCell++) {
    cit=aShower.Find(*pNearCell);
    if(cit!=aShower.End()) {
      tmpNearCell.push_back(*pNearCell);
      tmpNextNearCell.push_back(*pNearCell);
      e9+=cit->second.getEnergy()*cit->second.getFraction();
      e25+=cit->second.getEnergy()*cit->second.getFraction();
    }
  }
  
  //e5x5
  for(pNextNearCell=NextNearCell.begin();
      pNextNearCell!=NextNearCell.end();
      pNextNearCell++) {
    cit=aShower.Find(*pNextNearCell);
    if(cit!=aShower.End()) {
      tmpNextNearCell.push_back(*pNextNearCell);
      e25+=cit->second.getEnergy()*cit->second.getFraction();
    }
  }

  //eall
  for(cit=aShower.Begin();cit!=aShower.End();++cit) {
    eall+=(cit->second.getEnergy())*(cit->second.getFraction());
    eVec.push_back(cit->second.getEnergy()*cit->second.getFraction());
  }

  //calculate number of hits from MC
  int nHit,n;
  EmcRecParameter& Para=EmcRecParameter::GetInstance();
  nHit=(int)(Para.HitNb(0)*log(Para.HitNb(1)*e9+Para.HitNb(2)));
  n=0;

  //sort by energy
  sort(eVec.begin(), eVec.end(), greater<RecEmcEnergy>());

  for(ciVec=eVec.begin();ciVec!=eVec.end();ciVec++) {
    if(n<nHit) {
      elepton+=*ciVec;
      n++;
    }
  }

  //energy correction
  //RecEmcEnergy eCorr=ECorrTheta(e25,CellId);
  //RecEmcEnergy eCorr=ECorrection(e25);
  int getthetaid = EmcID::theta_module(CellId);
  int getmodule = EmcID::barrel_ec(CellId);
  if(getthetaid>21)getthetaid=43-getthetaid;
  if(getmodule==1)getthetaid=getthetaid+6;
  double dthetaid=double(getthetaid);
  double eCorr = Para.ECorrMC(e25,dthetaid);

  //energy error
  RecEmcEnergy de,de1,de2,de3;
  de1 = Para.SigE(0)/eCorr;
  de2 = Para.SigE(1)/pow(eCorr,0.25);
  de3 = Para.SigE(2);
  de = sqrt(de1*de1+de2*de2+de3*de3)*perCent*eCorr;

  double err = Para.ErrMC(e25,dthetaid);
  if(err>0) de = err*e25;

  aShower.setTrackId(-1);
  aShower.setCellId(CellId);
  aShower.setModule(module);
  aShower.setNumHits(aShower.getSize());
  aShower.setDE(de);
  aShower.CellId3x3(tmpNearCell);
  aShower.CellId5x5(tmpNextNearCell);
  aShower.setESeed(e1);
  aShower.setE3x3(e9);
  aShower.setE5x5(e25);
  aShower.ELepton(elepton);
  aShower.EAll(eall);
  aShower.setEnergy(eCorr);

  //cout<<"e1="<<aShower.eSeed()
  //  <<"\te9="<<aShower.e3x3()
  //  <<"\te25="<<aShower.e5x5()
  //  <<"\telepton="<<aShower.getELepton()
  //  <<"\teall="<<aShower.getEAll()
  //  <<"\tenergy="<<aShower.energy()<<endl;
}

RecEmcEnergy EmcRecShowerEnergy::ECorrection(const RecEmcEnergy eIn)
{
  if(eIn>3.) return eIn;

  EmcRecParameter& Para=EmcRecParameter::GetInstance();

  RecEmcEnergy eOut=0;
  double par[4];
  for(int i=0;i<4;i++) {
    par[i]=Para.ECorr(i);
  }

  eOut = eIn/(par[0]+par[1]*eIn+par[2]*eIn*eIn+par[3]*eIn*eIn*eIn);
  return eOut;
}

RecEmcEnergy EmcRecShowerEnergy::ECorrTheta(const RecEmcEnergy eIn, const RecEmcID &id)
{
  EmcRecParameter& Para=EmcRecParameter::GetInstance();
  RecEmcEnergy eOut=eIn;

  unsigned int npart = EmcID::barrel_ec(id);
  unsigned int ntheta = EmcID::theta_module(id);

  if(npart==1) {
    eOut *= 1.843/Para.Peak(ntheta);
  } else if(npart==0) {
    eOut *= 1.843/Para.Peak(ntheta+44);
  } else if(npart==2) {
    eOut *= 1.843/Para.Peak(ntheta+50);
  }

  return eOut;
}
