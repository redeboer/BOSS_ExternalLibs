//
//  Convert from Digit Map to Hit Map
//
//  Wang.Zhe 2004, 3, 29
//  He.Miao 2005
//
#include <iostream>

#include "EmcRec/EmcRecDigit2Hit.h"
#include "EmcRec/EmcRecParameter.h"

#include "EmcCalibConstSvc/EmcCalibConstSvc.h"
#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "RawDataProviderSvc/EmcRawDataProvider.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

// tianhl for mt
#include "GaudiKernel/ThreadGaudi.h"
// tianhl for mt
// Constructors and destructors
EmcRecDigit2Hit::EmcRecDigit2Hit()
{}

EmcRecDigit2Hit::~EmcRecDigit2Hit()
{}

void EmcRecDigit2Hit::Convert(const RecEmcDigitMap& aDigitMap,
    RecEmcHitMap& aHitMap)
{
  RecEmcDigitMap::const_iterator ciDigitMap;
  RecEmcHit aHit;

  EmcRecParameter& Para=EmcRecParameter::GetInstance();

  StatusCode sc;
  // Get EmcCalibConstSvc.
  IEmcCalibConstSvc *emcCalibConstSvc;
  sc= Gaudi::svcLocator()->service("EmcCalibConstSvc", emcCalibConstSvc);
  if(sc != StatusCode::SUCCESS) {
    //cout << "EmcRecDigit2Hit Error: Can't get EmcCalibConstSvc." << endl;
  }

  // Get RawDataProviderSvc
  IRawDataProviderSvc *rawDataProviderSvc;
  // tianhl for mt
  std::string rawDataProviderSvc_name("RawDataProviderSvc");
  if(isGaudiThreaded(m_algName)) {
    rawDataProviderSvc_name += getGaudiThreadIDfromName(m_algName);
  }
  // tianhl for mt
  sc = Gaudi::svcLocator()->service(rawDataProviderSvc_name.c_str(), rawDataProviderSvc);
  if(sc != StatusCode::SUCCESS) {
    //cout << "EmcRecDigit2Hit Error: Can't get RawDataProviderSvc." << endl;
  }

  for(ciDigitMap=aDigitMap.begin();
      ciDigitMap!=aDigitMap.end();
      ++ciDigitMap) {

    const RecEmcDigit &aDigit=ciDigitMap->second;

    aHit.CellId(aDigit.CellId());

    double ein,ecorr,eout;
    ein=aDigit.ADC();

    //Times the calibration constant in offline mode
    if(!(rawDataProviderSvc->isOnlineMode()) && Para.DigiCalib()) {

      unsigned int partId = EmcID::barrel_ec(aDigit.CellId());
      unsigned int theta = EmcID::theta_module(aDigit.CellId());
      unsigned int phi = EmcID::phi_module(aDigit.CellId());

      int index = emcCalibConstSvc->getIndex(partId,theta,phi);

      //liucx

      double adc2e = emcCalibConstSvc->getDigiCalibConst(index);  

      if(Para.DataMode()==0) { 
	if(Para.ElecSaturation()==1){
	  double emaxData = emcCalibConstSvc->getCrystalEmaxData(index);
	  if (emaxData>0) {
	    //	cout<<adc2e<<"\t"<<emaxData/2.5<<endl;
	    adc2e=emaxData/2.5;
	  }
	}
      }

      ein *= adc2e ;
      //liucx

      //liucx ein *= emcCalibConstSvc->getDigiCalibConst(index);

    }

    // Correct electronic noise bias. Not used now.
    if(ein<Para.ElecBias(4)) {
      ecorr=Para.ElecBias(0)*log(ein/Para.ElecBias(1));     //noise=0.5
    } else {
      ecorr=Para.ElecBias(2)+Para.ElecBias(3)*ein;
    }
    eout=ein;
    aHit.Energy(eout/GeV);

    aHit.Time(aDigit.TDC());
    // For some reason, there are 0 energy hit. 
    // Avoid this to happen, dangerous!
    if(aHit.getEnergy()>=Para.ElectronicsNoiseLevel()){// &&
      //(aHit.time()>=Para.TimeMin()) && 
      //(aHit.time()<=Para.TimeMax()))
      aHitMap[aHit.getCellId()]=aHit;
    }
  }

}

void EmcRecDigit2Hit::Output(const RecEmcHitMap& aHitMap) const
{
  unsigned int module,theta,phi;
  RecEmcID id;
  RecEmcHitMap::const_iterator ci_HitMap;
  RecEmcEnergy e;
  int ie;

  //cout<<"Output a hitmap:"<<endl;

  module=EmcID::EmcID::getBARREL();

  //cout<<"Barrel:"<<endl;
  //cout<<"+--------------------------------------------+"<<endl;
  for(phi=EmcID::getPHI_BARREL_MAX();
      phi!=EmcID::getPHI_BARREL_MIN()-1;
      --phi) {
    //cout<<"|";
    for(theta=EmcID::getTHETA_BARREL_MIN();
        theta<=EmcID::getTHETA_BARREL_MAX();
        ++theta) {
      id=EmcID::crystal_id(module,theta,phi);
      ci_HitMap=aHitMap.find(id);
      if(ci_HitMap!=aHitMap.end()) {
        e=ci_HitMap->second.getEnergy();
        ie=(int)(e*10);
        if(ie>9) { 
          ie=9; 
        }
        //cout<<ie;
      } else {
        //cout<<" ";
      }
    }
    //cout<<"|"<<endl;
  }
  //cout<<"+--------------------------------------------+"<<endl;

  module=EmcID::EmcID::getENDCAP_EAST();
  OutputEndcap(aHitMap,module);
  module=EmcID::EmcID::getENDCAP_WEST();
  OutputEndcap(aHitMap,module);
}

void EmcRecDigit2Hit::OutputEndcap(const RecEmcHitMap& aHitMap, const unsigned int module_ew) const
{
  unsigned int module,theta,phi;
  RecEmcID id;
  RecEmcHitMap::const_iterator ci_HitMap;
  RecEmcEnergy e;
  int ie;

  module=module_ew;
  if(module==EmcID::EmcID::getENDCAP_EAST()){
    //cout<<"East endcap:"<<endl;
  }
  else if(module==EmcID::EmcID::getENDCAP_WEST()){
    //cout<<"West endcap:"<<endl;
  }

  //cout<<"+--------------------------------------------------------+"<<endl;
  for(theta=EmcID::getTHETA_ENDCAP_MAX();
      theta!=EmcID::getTHETA_ENDCAP_MIN()-1;
      theta--){
    //cout<<"|";
    for(phi=(EmcID::getPHI_ENDCAP_MAX(theta)-1)/2;
        phi!=EmcID::getPHI_ENDCAP_MIN()-1;
        phi--) {
      id=EmcID::crystal_id(module,theta,phi);
      ci_HitMap=aHitMap.find(id);
      if(ci_HitMap!=aHitMap.end()) {
        e=ci_HitMap->second.getEnergy();
        ie=(int)(e*10);
        if(ie>9) {
          ie=9;
        }
        if((theta<4) && 
            (phi%((EmcID::getPHI_ENDCAP_MAX(theta)+1)/16)==(EmcID::getPHI_ENDCAP_MAX(theta)+1)/16-1)){
          //cout<<"*";
        }
        //cout<<ie;
        if((theta==0||theta==1)&&(phi%((EmcID::getPHI_ENDCAP_MAX(theta)+1)/16)==0)){
          //cout<<"*";
        }
      }
      else{
        if((theta<4) &&
            (phi%((EmcID::getPHI_ENDCAP_MAX(theta)+1)/16)==(EmcID::getPHI_ENDCAP_MAX(theta)+1)/16-1)){
          //cout<<" ";
        }
        //cout<<" ";
        if((theta==0||theta==1)&&(phi%((EmcID::getPHI_ENDCAP_MAX(theta)+1)/16)==0)){
          //cout<<" ";
        }
      }
      if(phi%((EmcID::getPHI_ENDCAP_MAX(theta)+1)/16)==0){
        //cout<<"|";
      }
      if(phi==(EmcID::getPHI_ENDCAP_MAX(theta)+1)/4){
        //cout<<"|";
      }
    }
    //cout<<endl;
  }
  //cout<<"+--------------------------------------------------------+"<<endl;

  for(theta=EmcID::getTHETA_ENDCAP_MIN();
      theta!=EmcID::getTHETA_ENDCAP_MAX()+1;
      theta++){
    //cout<<"|";
    for(phi=(EmcID::getPHI_ENDCAP_MAX(theta)+1)/2;
        phi!=EmcID::getPHI_ENDCAP_MAX(theta)+1;
        phi++) {
      id=EmcID::crystal_id(module,theta,phi);
      ci_HitMap=aHitMap.find(id);
      if(ci_HitMap!=aHitMap.end()) {
        e=ci_HitMap->second.getEnergy();
        ie=(int)(e*10);
        if(ie>9) {
          ie=9;
        }
        if((theta<4) && 
            (phi%((EmcID::getPHI_ENDCAP_MAX(theta)+1)/16)==0)){
          //cout<<"*";
        }
        //cout<<ie;
        if((theta==0||theta==1)&&((phi+1)%((EmcID::getPHI_ENDCAP_MAX(theta)+1)/16)==0)){
          //cout<<"*";
        }
      }
      else{
        if((theta<4) &&
            (phi%((EmcID::getPHI_ENDCAP_MAX(theta)+1)/16)==0)){
          //cout<<" ";
        }
        //cout<<" ";
        if((theta==0||theta==1)&&((phi+1)%((EmcID::getPHI_ENDCAP_MAX(theta)+1)/16)==0)){
          //cout<<" ";
        }
      } 
      if((phi+1)%((EmcID::getPHI_ENDCAP_MAX(theta)+1)/16)==0){
        //cout<<"|";
      }
      if(phi==3*(EmcID::getPHI_ENDCAP_MAX(theta)+1)/4-1){
        //cout<<"|";
      }
    }
    //cout<<endl;
  }
  //cout<<"+--------------------------------------------------------+"<<endl;

}

