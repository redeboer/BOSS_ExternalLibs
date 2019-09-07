#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "TofEnergyRec/TofShower.h"
#include "Identifier/Identifier.h"
#include "Identifier/TofID.h"
#include "RawDataProviderSvc/TofData.h"
#include "TofCaliSvc/ITofCaliSvc.h"
#include "TofQCorrSvc/ITofQCorrSvc.h"
#include "TofQElecSvc/ITofQElecSvc.h"
#include "TofEnergyCalibSvc/ITofEnergyCalibSvc.h"


#include "DstEvent/TofHitStatus.h"
#include "globals.hh"
#include <G4String.hh>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include "TofSim/BesTofDigitizer.hh"
#include "EvTimeEvent/RecEsTime.h"
#include "RawEvent/RawDataUtil.h"




TofShower::TofShower():m_output(false),m_isData(true)   // m_output wird auf false und m_isData auf true gesetzt
{
  IJobOptionsSvc* jobSvc;
  Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);
  jobSvc->setMyProperties("TofShower", &m_propMgr);

}

void TofShower::BookNtuple(NTuple::Tuple*& tuple, NTuple::Tuple*& tuple1, NTuple::Tuple*& tuple2)
{
  m_output = true;
  m_tuple = tuple;
  if(!m_tuple) {
    std::cerr << "Invalid ntuple in TofEnergyRec!" << std::endl;
  } else {
    m_tuple->addItem ("part",      m_part);
    m_tuple->addItem ("layer",     m_layer);
    m_tuple->addItem ("im",        m_im);
    m_tuple->addItem ("end",       m_end);
    m_tuple->addItem ("zpos",      m_zpos);
    m_tuple->addItem ("adc1",      m_adc1);
    m_tuple->addItem ("adc2",      m_adc2);
    m_tuple->addItem ("tdc1",      m_tdc1);
    m_tuple->addItem ("tdc2",      m_tdc2);
    m_tuple->addItem ("energy",    m_energy);
  }

  m_tuple1 = tuple1;
  if(!m_tuple1) {
    std::cerr << "Invalid ntuple1 in TofEnergyRec!" << std::endl;
  } else {
    m_tuple1->addItem ("part",      m_shower_part);
    m_tuple1->addItem ("layer",     m_shower_layer);
    m_tuple1->addItem ("im",        m_shower_im);
    m_tuple1->addItem ("zpos",      m_shower_zpos);
    m_tuple1->addItem ("energy",    m_shower_energy);
  }

  m_tuple2 = tuple2;
  if(!m_tuple2) {
    std::cerr << "Invalid ntuple2 in TofEnergyRec!" << std::endl;
  } else {
    m_tuple2->addItem ("dist",      m_seed_dist);
  }
}

void TofShower::energyCalib(vector<TofData*>& tofDataVec, RecTofTrackCol* recTofTrackCol)
{
  //Get TOF Calibtration Service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  ITofCaliSvc* tofCaliSvc;
  StatusCode sc = svcLocator->service("TofCaliSvc", tofCaliSvc);
  if (sc !=  StatusCode::SUCCESS) {
    cout << "TofEnergyRec Get Calibration Service Failed !! " << endl;
  }

  ITofQCorrSvc* tofQCorrSvc;
  sc = svcLocator->service("TofQCorrSvc", tofQCorrSvc);
  if (sc !=  StatusCode::SUCCESS) {
    cout << "TofEnergyRec Get QCorr Service Failed !! " << endl;
  }

  ITofQElecSvc* tofQElecSvc;
  sc = svcLocator->service("TofQElecSvc", tofQElecSvc);
  if (sc !=  StatusCode::SUCCESS) {
    cout << "TofEnergyRec Get QElec Service Failed !! " << endl;
  }
  
  ITofEnergyCalibSvc* m_TofEnergyCalibSvc=0;
  sc = svcLocator->service("TofEnergyCalibSvc", m_TofEnergyCalibSvc);
  if( sc != StatusCode::SUCCESS){
    cout << "can not use TofEnergyCalibSvc" << endreq;
  } 
  else {
    /*    
    std::cout << "Test TofEnergyCalibSvc  getCalibConst= " 
  	      << m_TofEnergyCalibSvc -> getCalibConst() 
	      <<"para1="<<m_TofEnergyCalibSvc -> getPara1()
	      <<"para2="<<m_TofEnergyCalibSvc -> getPara2()
	      <<"para3="<<m_TofEnergyCalibSvc -> getPara3()
	      <<"para4="<<m_TofEnergyCalibSvc -> getPara4()
	      <<"para5="<<m_TofEnergyCalibSvc -> getPara5()
	      << std::endl;
    */
  }
  


  vector<TofData*>::iterator it;
  for(it=tofDataVec.begin();
      it!=tofDataVec.end();
      it++) {

    Identifier id((*it)->identify());
    if( TofID::is_scin(id) ) {
      int    barrel_ec    = TofID::barrel_ec(id);
      int    layer        = TofID::layer(id);
      int    im           = TofID::phi_module(id);
      int    end          = TofID::end(id);

      if(m_output) {
	m_part = barrel_ec;
	m_layer = layer;
	m_im = im;
	m_end = end;
      }

      if((*it)->barrel()) {
	TofData* bTofData = (*it);
	bTofData->setZpos(99.);
	bTofData->setEnergy(0.);
	if(bTofData->tdc1()<=0||bTofData->tdc1()>8000||bTofData->tdc2()<=0||bTofData->tdc2()>8000) continue;

	double adc1,adc2,tdc1,tdc2;
	tdc1 = bTofData->tdc1();
	tdc2 = bTofData->tdc2();
	adc1 = bTofData->adc1();
	adc2 = bTofData->adc2();

	//from data CalibSvc
	double zpos = tofCaliSvc->ZTDC( tdc1, tdc2, bTofData->tofId() );
	if(fabs(zpos)>115) continue;
	double tofq = tofCaliSvc->BPh( adc1, adc2, zpos, bTofData->tofId());
	if(tofq<100||tofq>10000) continue;
	//double energy = q*0.0036;
	//double energy = tofq*m_calibConst;  //new calibration result in 2009.9.27
	//cout<< "TofShower       Barrel energy " << energy << endl;

	zpos /= 100.;   //cm->m

	///////////////////

	
	double CalibPar[5];

	CalibPar[0]=m_TofEnergyCalibSvc -> getPara1();
	CalibPar[1]=m_TofEnergyCalibSvc -> getPara2();
	CalibPar[2]=m_TofEnergyCalibSvc -> getPara3();
	CalibPar[3]=m_TofEnergyCalibSvc -> getPara4();
	CalibPar[4]=m_TofEnergyCalibSvc -> getPara5();

	double TofEnCalibConst=CalibPar[0]+CalibPar[1]*zpos+CalibPar[2]*pow(zpos,2)+CalibPar[3]*pow(zpos,3)+CalibPar[4]*pow(zpos,4);
	double energy = tofq*TofEnCalibConst;

	//cout<<"tofenergy="<<energy<<endl;
	//cout<<"tofenergy-old="<<tofq*m_calibConst<<endl;
	///////////////////
	bTofData->setZpos(zpos);
	bTofData->setEnergy(energy);

	if(m_output) {
	  m_part = barrel_ec;
	  m_layer = layer;
	  m_im = im;
	  m_end = end;
	  m_adc1 = bTofData->adc1();
	  m_adc2 = bTofData->adc2();
	  m_tdc1 = bTofData->tdc1();
	  m_tdc2 = bTofData->tdc2();
	  m_zpos = zpos;
	  m_energy = energy;
	  m_tuple->write();
	}
	
      }
      else {
	//cout<<"endcap"<<endl;
	//ETofData* eTofData = dynamic_cast<ETofData*>(*it);
	//TofData* bTofData = (*it);
	//double energy = 2*eTofData->adcChannel()/140;
	//eTofData->setEnergy(energy);
      }
    }
    else { 
      int    barrel_ec    = TofID::barrel_ec(id);
      int    endcap       = TofID::endcap(id);
      int    module       = TofID::module(id);
      int    strip        = TofID::strip(id);
      int    end          = TofID::end(id);

      if( barrel_ec!=3 || ( endcap!=0 && endcap!=1 ) ) {
	cout << "TofEnergyRec Get ETF(MRPC) data ERROR !!    barrel_ec:" << barrel_ec << " endcap:" << endcap << endl;
      }

      if(m_output) {
	m_part  = barrel_ec;
	m_layer = endcap;
	m_im    = module;
	m_end   = strip;
      }

      TofData* etfData = (*it);
      etfData->setZpos(99.);
      etfData->setEnergy(0.);
      if(etfData->tdc1()<=0||etfData->tdc1()>8000||etfData->tdc2()<=0||etfData->tdc2()>8000) continue;

      double adc1,adc2,tdc1,tdc2;
      tdc1 = etfData->tdc1();
      tdc2 = etfData->tdc2();
      adc1 = etfData->adc1();
      adc2 = etfData->adc2();

      //from data CalibSvc
      double zpos = tofCaliSvc->ZTDC( tdc1, tdc2, etfData->tofId() );
      if(fabs(zpos)>115) continue;
      double tofq = tofCaliSvc->BPh( adc1, adc2, zpos, etfData->tofId());
      if(tofq<100||tofq>10000) continue;
      //double energy = q*0.0036;
      double energy = tofq*m_calibConst;  //new calibration result in 2009.9.27
      //cout<< "TofShower       Barrel energy " << energy << endl;
      zpos /= 100.;   //cm->m
	
      etfData->setZpos(zpos);
      etfData->setEnergy(energy);

      if(m_output) {
	m_part = barrel_ec;
	m_layer = endcap;
	m_im   = module;
	m_end  = strip;
	m_adc1 = etfData->adc1();
	m_adc2 = etfData->adc2();
	m_tdc1 = etfData->tdc1();
	m_tdc2 = etfData->tdc2();
	m_zpos = zpos;
	m_energy = energy;
	m_tuple->write();
      }
    }
  }
  return;
}

vector<Identifier> TofShower::getNeighbors(const Identifier& id)
{
  vector<int> NeighborVec;
  vector<Identifier> NeighborIdVec;
  NeighborVec.clear();
  NeighborIdVec.clear();

  if( TofID::is_scin(id) ) {
    int    barrel_ec    = TofID::barrel_ec(id);
    int    layer        = TofID::layer(id);
    int    im           = TofID::phi_module(id);
    int    end          = TofID::end(id);

    if(barrel_ec==1) {    //barrel
      int num = im+layer*88;
      if(num<88) {   //layer1 
	if(num==0) {
	  NeighborVec.push_back(1);
	  NeighborVec.push_back(87);
	  NeighborVec.push_back(88);
	  NeighborVec.push_back(89);
	} else if(num==87) {
	  NeighborVec.push_back(0);
	  NeighborVec.push_back(86);
	  NeighborVec.push_back(88);
	  NeighborVec.push_back(175);
	} else {
	  NeighborVec.push_back(num+1);
	  NeighborVec.push_back(num-1);
	  NeighborVec.push_back(num+88);
	  NeighborVec.push_back(num+88+1);
	}
      } else {
	if(num==88) {
	  NeighborVec.push_back(89);
	  NeighborVec.push_back(175);
	  NeighborVec.push_back(0);
	  NeighborVec.push_back(87);
	} else if(num==175) {
	  NeighborVec.push_back(88);
	  NeighborVec.push_back(174);
	  NeighborVec.push_back(86);
	  NeighborVec.push_back(87);
	} else {
	  NeighborVec.push_back(num+1);
	  NeighborVec.push_back(num-1);
	  NeighborVec.push_back(num-88);
	  NeighborVec.push_back(num-88-1);
	}
      }

      int size=NeighborVec.size();
      for(int i=0;i<size;i++) {
	layer = NeighborVec[i]/88;
	im = NeighborVec[i]%88;
	NeighborIdVec.push_back(TofID::cell_id(barrel_ec,layer,im,end));
      }
    }

    else {    //endcap
      if(im==0) {
	NeighborVec.push_back(1);
	NeighborVec.push_back(47);
      } else if(im==47) {
	NeighborVec.push_back(0);
	NeighborVec.push_back(46);
      } else {
	NeighborVec.push_back(im-1);
	NeighborVec.push_back(im+1);
      }
      
      int size=NeighborVec.size();
      for(int i=0;i<size;i++) {
	im = NeighborVec[i];
	NeighborIdVec.push_back(TofID::cell_id(barrel_ec,layer,im,end));
      }
    }
  }
  else { // mrpc
    int    barrel_ec    = TofID::barrel_ec(id);
    int    endcap       = TofID::endcap(id);
    int    module       = TofID::module(id);
    int    strip        = TofID::strip(id);
    int    end          = TofID::end(id);

    if( module==0 ) {
      NeighborVec.push_back(1);
      NeighborVec.push_back(35);
    } else if( module==35 ) {
      NeighborVec.push_back(0);
      NeighborVec.push_back(34);
    } {
      NeighborVec.push_back(module-1);
      NeighborVec.push_back(module+1);
    }

    int size=NeighborVec.size();
    for(int i=0;i<size;i++) {
      int im = NeighborVec[i];
      for(int j=-2; j<3; j++ ) {
	int ist = strip + j;
	if( ist<0 || ist>11 ) continue;
	NeighborIdVec.push_back(TofID::cell_id(barrel_ec,endcap,module,ist,end));
      }
    }

  }

  return NeighborIdVec;
}

vector<Identifier> TofShower::getNextNeighbors(const Identifier& id)
{
  vector<Identifier> NeighborVec,tmpNeighborVec,tmpNextNeighborVec;
  vector<Identifier>::iterator ci_NV,ci_tmpNV,ci_tmpNNV;
  NeighborVec=getNeighbors(id);
  tmpNeighborVec=getNeighbors(id);
  bool flag=false;           //whether NeighborVec already includes this crystal
  bool flagNeighbor=false;   //whether this crystal belongs to NeighborVec

  //------------------------------------------------------------------
  for(ci_tmpNV=tmpNeighborVec.begin();
      ci_tmpNV!=tmpNeighborVec.end();
      ci_tmpNV++){
    tmpNextNeighborVec=getNeighbors(*ci_tmpNV);
    //================================================================
    for(ci_tmpNNV=tmpNextNeighborVec.begin();
        ci_tmpNNV!=tmpNextNeighborVec.end();
        ci_tmpNNV++){

      for(ci_NV=NeighborVec.begin();
          ci_NV!=NeighborVec.end();
          ci_NV++){
        if(*ci_tmpNNV==*ci_NV){  //this crystal is already included
          flag=true;
          break;
        }
      }

      if(!flag){       //find a new crystal
        //for(ci_tmpNV1=tmpNeighborVec.begin();
        //    ci_tmpNV1!=tmpNeighborVec.end();
        //    ci_tmpNV1++){
        //  if(*ci_tmpNNV==*ci_tmpNV1){  //this crystal belongs to NeighborVec
        //    flagNeighbor=true;
        //    break;
        //  }
        //}

        if(*ci_tmpNNV==id)
          flagNeighbor=true;

        if(!flagNeighbor)
          NeighborVec.push_back(*ci_tmpNNV);
        else
          flagNeighbor=false;
      }
      else
        flag=false;
    }
    //================================================================
  }
  //------------------------------------------------------------------

  return NeighborVec; 
}




void TofShower::findSeed(vector<TofData*>& tofDataVec)
{
  bool max=false;
  m_seedVec.clear();

  vector<TofData*>::iterator it;
  for(it=tofDataVec.begin();
      it!=tofDataVec.end();
      it++) {

    if( !((*it)->is_mrpc()) ) {
    if((*it)->barrel()) {   //barrel
      TofData* bTofData = (*it);

      //cout << "Identifier(bTofData->identify())  " << Identifier(bTofData->identify()) << endl;
      //std::cout << "TofShower  bTofData->energy()  =   "  << bTofData->energy() << std::endl;
      if(bTofData->energy()<5.) continue;   //seed energy cut = 6MeV

      max=true;
      vector<Identifier> NeighborVec=getNextNeighbors(Identifier(bTofData->identify()));
      
      //const Identifier & help = Identifier(bTofData->identify());
      //cout << "TofShower::findSeed    Barrel Track base  "<<TofID::layer(help) << "  " << TofID::phi_module(help) << endl;

      vector<Identifier>::iterator iNeigh;
      for(iNeigh=NeighborVec.begin();
          iNeigh!=NeighborVec.end();
          iNeigh++) {
	
	//const Identifier & help2 = Identifier(*iNeigh);
	//cout << "TofShower::findSeed    Barrel Track neigh  "<<TofID::layer(help2) << "  " << TofID::phi_module(help2) << endl;

        vector<TofData*>::iterator it2;
        for(it2=tofDataVec.begin();
            it2!=tofDataVec.end();
            it2++) {
          if((*it2)->identify()==*iNeigh) {
            TofData* bTofData2 = (*it2);
            if(bTofData2->energy()>bTofData->energy()) {
              max=false;
            }
            break;
          }
        }

      }
    }
    else {    //endcap
     
      //cout << "TofShower::findSeed    Endcap Track" << endl;
      TofData* eTofData = (*it);
      if(eTofData->energy()<5.) continue;   //seed energy cut = 5MeV
      max=true;
      vector<Identifier> NeighborVec=getNextNeighbors(Identifier(eTofData->identify()));
      vector<Identifier>::iterator iNeigh;
      for(iNeigh=NeighborVec.begin(); iNeigh!=NeighborVec.end(); iNeigh++) 
	{
	  vector<TofData*>::iterator it2;
	  for(it2=tofDataVec.begin(); it2!=tofDataVec.end();  it2++) 
	    {
	      if((*it2)->identify()==*iNeigh) {
		TofData* eTofData2 = (*it2);
		if(eTofData2->energy()>eTofData->energy()) {
		  max=false;
		}
		break;
	      }
	    }//close for 
	}//Close for
    }//close if(!is_mrpc)

    }
    else
      {

      TofData* etfData = (*it);
      if(etfData->energy()<2.) continue; //Seed energy cut = 2 MeV
      max=true;
      vector<Identifier> NeighborVec=getNextNeighbors(Identifier(etfData->identify()));
      vector<Identifier>::iterator iNeigh;
      for(iNeigh=NeighborVec.begin(); iNeigh!=NeighborVec.end(); iNeigh++)
	{
	  //cout << "TofShower::findSeed    Phimodule MRPC " << TofID::phi_module(*iNeigh) <<endl;
	  vector<TofData*>::iterator it2;
	  for(it2=tofDataVec.begin(); it2!=tofDataVec.end();  it2++)
	    {
	      if((*it2)->identify()==*iNeigh) {
		TofData* etfData2 = (*it2);
		if(etfData2->energy()>etfData->energy()) {
		  max=false;
		}
		break;
	      }
	    }//close for                         
	}//Close for 
      //cout << "TofShower::findSeed    Both forloops done" << endl;
      }//Close else if(is_mrpc)

    if(max) {
      m_seedVec.push_back(Identifier((*it)->identify()));
    }

  }//close loop over tofdata
}//close find seed


void TofShower::findShower(vector<TofData*>& tofDataVec, RecTofTrackCol* recTofTrackCol, double t0)
{
  
  energyCalib(tofDataVec, recTofTrackCol);
  //cout << "TofShower::findShower    energycalib OK" << endl;
  findSeed(tofDataVec);
  //cout << "TofShower::findShower    findseed OK" << endl;

  vector<Identifier>::iterator iSeed;

  for(iSeed=m_seedVec.begin(); iSeed!=m_seedVec.end(); iSeed++) 
    {

      bool is_mrpc = TofID::is_mrpc(*iSeed);

      if(!is_mrpc) //Old TofDetector + barrel
	{
	  int barrel_ec = TofID::barrel_ec(*iSeed);
	  int layer = TofID::layer(*iSeed);
	  int im = TofID::phi_module(*iSeed);
	  im += layer * 88;

	  bool neutral=true;
	  //match with Tof charged track
	  int dphi=999;
	  RecTofTrackCol::iterator iTrack, iMatch;
	  for(iTrack=recTofTrackCol->begin(); iTrack!=recTofTrackCol->end();  iTrack++) 
	    {
	      TofHitStatus *status = new TofHitStatus;
	      status->setStatus((*iTrack)->status());
	      if( barrel_ec==1 && status->is_barrel() ) {
		dphi=abs(im-(*iTrack)->tofID());
		dphi = dphi>=44 ? 88-dphi : dphi;
	      } else if( barrel_ec==2 && !(status->is_barrel()) && ((*iTrack)->tofID()>47) ) {
		dphi=abs(im-(*iTrack)->tofID()+48);
		dphi = dphi>=24 ? 48-dphi : dphi;
	      } else if( barrel_ec==0 && !(status->is_barrel()) && ((*iTrack)->tofID()<48) ) {
		dphi=abs(im-(*iTrack)->tofID());
		dphi = dphi>=24 ? 48-dphi : dphi;
	      }
	      if(abs(dphi)<=2) {
		iMatch = iTrack;
		neutral=false;
		break;
	      }
	    }
	  
	  //energy sum of seed and its neighbors
	  //use avarage mean to calculation position
	  double zpos=0;
	  double energy=0;
	  double seedPos=0;

	  //cout << "Energhy =0 " << endl;
	  vector<TofData*>::iterator it;
	  for(it=tofDataVec.begin(); it!=tofDataVec.end(); it++) 
	    {
	      if((*it)->identify()==*iSeed) {
		//cout<<"iSeed="<<*iSeed<<endl;
		if((*it)->barrel()) {
		  TofData* bTofData = (*it);
		  zpos+=bTofData->zpos()*bTofData->energy();
		  energy+=bTofData->energy();
		  seedPos=bTofData->zpos();
	  
		  //cout << "Add energy barrel seed "<< TofID::layer(*iSeed) <<"  " << TofID::phi_module(*iSeed) << "   " << bTofData->energy() << "   --->   "<< energy  <<endl;
		} else {
		  TofData* eTofData = (*it);
		  energy+=eTofData->energy();

		  //cout << "Add energy"  << endl;
		}
		break;
	      }
	    }

	  vector<Identifier> NeighborVec=getNextNeighbors(*iSeed);
	  vector<Identifier>::iterator iNeigh;
	  for(iNeigh=NeighborVec.begin(); iNeigh!=NeighborVec.end(); iNeigh++) 
	    {

	      vector<TofData*>::iterator it2;
	      for(it2=tofDataVec.begin();  it2!=tofDataVec.end();  it2++) 
		{
		  if((*it2)->identify()==*iNeigh) {
		    //cout<<"iNeigh="<<*iNeigh<<endl;
		    if((*it)->barrel()) {
		      TofData* bTofData2 = (*it2);

		      if(fabs(bTofData2->zpos())>2) continue;
		      if(m_output) {
			m_seed_dist = seedPos-bTofData2->zpos();
			m_tuple2->write();
		      }
		      if(fabs(seedPos-bTofData2->zpos())>0.3) continue;
		      zpos+=bTofData2->zpos()*bTofData2->energy();
		      energy+=bTofData2->energy();
		      //cout << "Add energy barrel neig " << TofID::layer(*iNeigh) <<"  "  <<  TofID::phi_module(*iNeigh) << "   " << bTofData2->energy() << "   --->   "<< energy  <<endl;
		    } else {
		      TofData* eTofData2 = (*it2);
		      energy+=eTofData2->energy();
		    }
		    break;
		  }
		}
	    }
	  if(energy>0) zpos/=energy;

	  //for charged track, set energy
	  if(neutral==false) {
	    if(fabs(zpos)<1.15&&energy>5.&&energy<1000) {
	      (*iMatch)->setEnergy(energy/1000);
	    }
	    continue;
	  }

	  //for neutral track
	  if(fabs(zpos)<1.15&&energy>5.&&energy<1000) {    //shower energy cut = 10MeV
	    RecTofTrack* tof = new RecTofTrack;
	    tof->setTofID(*iSeed);
	    TofHitStatus* hitStatus = new TofHitStatus;
	    hitStatus->init();
	    tof->setStatus( hitStatus->value());
	    tof->setZrHit(zpos);
	    tof->setEnergy(energy/1000);  //MeV-->GeV
	    recTofTrackCol->push_back(tof);

	    if(m_output) {
	      m_shower_part = barrel_ec;
	      m_shower_layer = layer;
	      m_shower_im = im;
	      m_shower_zpos = zpos;
	      m_shower_energy = energy;
	      m_tuple1->write();
	    }
	  }
	}//close if(!is_mrpc)
      else // mrpc_case
	{
	  //Determine wether the track is a neutral one or not:(Compare with charged tracks)
	  //cout << "TofShower::findShower    Seed is mrpc" << endl;

          int barrel_ec = TofID::barrel_ec(*iSeed);
	  int endcap    = TofID::endcap(*iSeed);
          int im        = TofID::module(*iSeed);
          int strip     = TofID::strip(*iSeed);
	  int end       = TofID::end(*iSeed);
	  im += endcap * 36;

	  bool neutral=true;
	  //match with Tof charged track
	  int dphi=999, dtheta=999;
	  RecTofTrackCol::iterator iTrack, iMatch;
	  for(iTrack=recTofTrackCol->begin(); iTrack!=recTofTrackCol->end();  iTrack++) 
	    {
	      TofHitStatus *status = new TofHitStatus;
	      status->setStatus((*iTrack)->status());
	      if( barrel_ec==3 && endcap==0 && ((*iTrack)->tofID()<36) ) {
		dphi = abs(im-(*iTrack)->tofID());
		dphi = dphi>=18 ? 36-dphi : dphi;
		dtheta = abs( strip - status->layer() );
	      } else if( barrel_ec==3 && endcap==1 && ((*iTrack)->tofID()>35) ) {
		dphi=abs(im-(*iTrack)->tofID()+36);
		dphi = dphi>=18 ? 36-dphi : dphi;
		dtheta = abs( strip - status->layer() );
	      }
	      if( ( abs(dphi)==0 && abs(dtheta)<=2 ) || ( abs(dphi)==1 && abs(dtheta)<=1 ) ) {
		iMatch = iTrack;
		neutral = false;
		break;
	      }
	    }

	  //energy sum of seed and its neighbors
	  //use avarage mean to calculation position
	  double zpos=0;
	  double energy=0;
	  double seedPos=0;

	  //cout << "Energhy =0 " << endl;
	  vector<TofData*>::iterator it;
	  for(it=tofDataVec.begin(); it!=tofDataVec.end(); it++) 
	    {
	      if((*it)->identify()==*iSeed) {
		TofData* etfData = (*it);
		zpos+=etfData->zpos()*etfData->energy();
		energy+=etfData->energy();
		seedPos=etfData->zpos();
		break;
	      }
	    }

	  vector<Identifier> NeighborVec=getNextNeighbors(*iSeed);
	  vector<Identifier>::iterator iNeigh;
	  for(iNeigh=NeighborVec.begin(); iNeigh!=NeighborVec.end(); iNeigh++) 
	    {
	      vector<TofData*>::iterator it2;
	      for(it2=tofDataVec.begin();  it2!=tofDataVec.end();  it2++) 
		{
		  if((*it)->barrel()) continue;
		  if((*it2)->identify()==*iNeigh) {
		    TofData* etfData2 = (*it2);
		    if(fabs(etfData2->zpos())>2) continue;
		      if(m_output) {
			m_seed_dist = seedPos-etfData2->zpos();
			m_tuple2->write();
		      }
		      if(fabs(seedPos-etfData2->zpos())>0.3) continue;
		      zpos+=etfData2->zpos()*etfData2->energy();
		      energy+=etfData2->energy();
		      //cout << "Add energy barrel neig " << TofID::layer(*iNeigh) <<"  "  <<  TofID::phi_module(*iNeigh) << "   " << bTofData2->energy() << "   --->   "<< energy  <<endl;
		      break;
		  }
		}
	    }
	  if(energy>0) zpos/=energy;

	  //for charged track, set energy
	  if(neutral==false) {
	    if(fabs(zpos)<1.15&&energy>5.&&energy<1000) {
	      (*iMatch)->setEnergy(energy/1000);
	    }
	    continue;
	  }

	  //for neutral track
	  if(fabs(zpos)<1.15&&energy>5.&&energy<1000) {    //shower energy cut = 10MeV
	    RecTofTrack* tof = new RecTofTrack;
	    tof->setTofID(*iSeed);
	    TofHitStatus* hitStatus = new TofHitStatus;
	    hitStatus->init();
	    tof->setStatus( hitStatus->value());
	    tof->setZrHit(zpos);
	    tof->setEnergy(energy/1000);  //MeV-->GeV
	    recTofTrackCol->push_back(tof);

	    if(m_output) {
	      m_shower_part = barrel_ec;
	      m_shower_layer = strip;
	      m_shower_im = im;
	      m_shower_zpos = zpos;
	      m_shower_energy = energy;
	      m_tuple1->write();
	    }
	  }
	}

    }
}

void TofShower::readCalibPar()
{
  string paraPath = getenv("TOFENERGYRECROOT");
  paraPath += "/share/peak.dat";
  ifstream in;
  in.open(paraPath.c_str());
  assert(in);
  for(int i=0;i<176;i++) {
    in>>m_ecalib[i];
  }
  in.close();

  paraPath = getenv("TOFENERGYRECROOT");
  paraPath += "/share/calib.dat";
  ifstream in1;
  in1.open(paraPath.c_str());
  assert(in1);
  for(int i=0;i<176;i++) {
    in1>>m_calib[i][0]>>m_calib[i][1]>>m_calib[i][2]>>m_calib[i][3];
  }
  in1.close();
}

double TofShower::ecalib(const int nsci) const
{
  if(nsci<176) {
    return m_ecalib[nsci];
  } else {
    return 0;
  }
}

void TofShower::setEcalib(const int nsci, const double ecalib)
{
  if(nsci<176) {
    m_ecalib[nsci]=ecalib;
  }
}

double TofShower::calib(const int n, const int m) const
{
  if(n<176&&m<4) {
    return m_calib[n][m];
  } else {
    return 0;
  }
}

void TofShower::setCalib(const int n, const int m, const double ecalib)
{
  if(n<176&&m<4) {
    m_calib[n][m]=ecalib;
  }
}

