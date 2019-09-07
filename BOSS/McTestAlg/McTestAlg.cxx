////---------------------------------------------------------------------------//
////Description:
////Author:   Dengzy
////Created:  Mar, 2004
////Modified:
////Comment:
//
#include "McTestAlg/McTestAlg.h"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/MsgStream.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Point3D.h"

#include "MdcRawEvent/MdcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "EmcRawEvent/EmcDigi.h"
#include "MucRawEvent/MucDigi.h"

#include "McTruth/McParticle.h"
#include "McTruth/MdcMcHit.h"
#include "McTruth/TofMcHit.h"
#include "McTruth/EmcMcHit.h"
#include "McTruth/MucMcHit.h"

#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "Identifier/TofID.h"
#include "Identifier/EmcID.h"
#include "Identifier/MucID.h"

#include "RawEvent/RawDataUtil.h"
#include "RawEvent/DigiEvent.h"
#include "McTruth/McEvent.h"
#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"

McTestAlg::McTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
Algorithm(name, pSvcLocator)
{
  declareProperty("ParticleRootFlag",m_particleRootFlag=false);
  declareProperty("MdcRootFlag",m_mdcRootFlag=false);
  declareProperty("TofRootFlag",m_tofRootFlag=false);
}

StatusCode McTestAlg::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " McTestAlg initialize()" << endreq;
 
  if(m_particleRootFlag)
  {
    StatusCode sc;
    NTuplePtr ntp(ntupleSvc(), "FILE900/particle");
    if(ntp) tupleParticle = ntp;
    else {
      tupleParticle = ntupleSvc()->book("FILE900/particle",CLID_ColumnWiseTuple,"McTestAlg");
      if(tupleParticle)
        sc = tupleParticle->addItem("me",me);
    }
  }
    
  if(m_mdcRootFlag) 
  { 
    StatusCode sc;
    NTuplePtr nt1(ntupleSvc(), "FILE901/n1");  //for Mdc McTruth
    if(nt1) tupleMdc1 = nt1;
    else {
      tupleMdc1 = ntupleSvc()->book("FILE901/n1",CLID_ColumnWiseTuple,"McTestAlg");
      if(tupleMdc1)
      {
        sc = tupleMdc1->addItem("truthIndex",truthMdcIndex);
        sc = tupleMdc1->addItem("truthLayer",truthMdcLayer);
        sc = tupleMdc1->addItem("truthWire",truthMdcWire);
        sc = tupleMdc1->addItem("truthEdep",truthMdcEdep);
        sc = tupleMdc1->addItem("truthDriftD",truthMdcDriftD);
        sc = tupleMdc1->addItem("truthX",truthMdcX);
        sc = tupleMdc1->addItem("truthY",truthMdcY);
        sc = tupleMdc1->addItem("truthZ",truthMdcZ);
        sc = tupleMdc1->addItem("ntruth",ntruthMdc);
      }
      else    {   // did not manage to book the N tuple....
        log << MSG::ERROR <<"Cannot book MDC N-tuple:" << long(tupleMdc1) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  
    NTuplePtr nt2(ntupleSvc(), "FILE901/n2");  //for Mdc digit
    if(nt2) tupleMdc2 = nt2;
    else {
      tupleMdc2 = ntupleSvc()->book("FILE901/n2",CLID_ColumnWiseTuple,"McTestAlg");
      if(tupleMdc2)
      {
        sc = tupleMdc2->addItem("layer",m_layer);
        sc = tupleMdc2->addItem("cell",m_cell);
        sc = tupleMdc2->addItem("ADC",m_charge);
        sc = tupleMdc2->addItem("TDC",m_time);
      }
    }
  }
  
  if(m_tofRootFlag)
  {
    StatusCode sc;
    NTuplePtr nt(ntupleSvc(), "FILE902/lr");
    if(nt) tupleTof = nt;
    else {
      tupleTof=ntupleSvc()->book("FILE902/lr",CLID_ColumnWiseTuple,"McTestAlg");
      if(tupleTof)
      {
        sc = tupleTof->addItem("truthIndex",truthIndex);
        sc = tupleTof->addItem("truthPartId",truthPartId);
        sc = tupleTof->addItem("truthLayer",truthLayer);
        sc = tupleTof->addItem("truthScinNb",truthScinNb);
        sc = tupleTof->addItem("truthX",truthX);
        sc = tupleTof->addItem("truthY",truthY);
        sc = tupleTof->addItem("truthZ",truthZ);
        sc = tupleTof->addItem("ntruth",ntruth);
        sc = tupleTof->addItem("tleft",tleft);
        sc = tupleTof->addItem("tright",tright);
        sc = tupleTof->addItem("qleft",qleft);
        sc = tupleTof->addItem("qright",qright);
      }
      else    {   // did not manage to book the N tuple....
        log << MSG::ERROR <<"Cannot book N-tuple:" << long(tupleTof) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }  
  return StatusCode::SUCCESS;
}

StatusCode McTestAlg::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "McTestAlg finalize()" << endreq;
      
  return StatusCode::SUCCESS;
  
}

        
StatusCode McTestAlg::execute()
{
  //interface to event data service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc=svcLocator->service("EventDataSvc", m_evtSvc);
  if (sc.isFailure())
    std::cout<<"Could not accesss EventDataSvc!"<<std::endl;

  SmartDataPtr<Event::EventHeader> eventHeader(m_evtSvc,"/Event/EventHeader");
  if(!eventHeader)
    std::cout<<"Could not retrieve EventHeader"<<std::endl;

  int event=eventHeader->eventNumber();
  std::cout<<"event: "<<event<<std::endl;

  RetrieveMcParticle();
  RetrieveMdc();
  RetrieveTof();
  RetrieveEmc();
  RetrieveMuc();

  return StatusCode::SUCCESS;

}

void McTestAlg::RetrieveMcParticle()
{
  SmartDataPtr<Event::McParticleCol> mcParticleCol(m_evtSvc,"/Event/MC/McParticleCol");
  if(!mcParticleCol)
    std::cout<<"Could not retrieve McParticelCol"<<std::endl;
  else
  {
    int pdgcode;
    double px,py,pz,E,mass;
    int nflag=0;
    Event::McParticleCol::iterator iter_mc = mcParticleCol->begin();
    for (;iter_mc != mcParticleCol->end(); iter_mc++)
    {
      //Event::McParticle mp = (*iter_mc)->mother();
      pdgcode = (*iter_mc)->particleProperty();
      if((*iter_mc)->trackIndex()<0)
        std::cout<<"ERROR! trackIndex<0"<<std::endl;
      px=(*iter_mc)->initialFourMomentum().x();
      py=(*iter_mc)->initialFourMomentum().y();
      pz=(*iter_mc)->initialFourMomentum().z();
      E=(*iter_mc)->initialFourMomentum().t();
      if(E*E-px*px-py*py-pz*pz>=0)
        mass=sqrt(E*E-px*px-py*py-pz*pz);
      else
        mass=0;

      if(m_particleRootFlag)
      {
        if(abs(pdgcode)==11)
          me=mass;
        tupleParticle->write();
      }
      if(abs(pdgcode)==2212||abs(pdgcode)==211)
        nflag++;
    }
    if(nflag!=4)
      std::cout<<"nflag!=4"<<std::endl;
  }
}

void McTestAlg::MdcInit()
{
  truthMdcIndex = -9;
  truthMdcLayer = -9;
  truthMdcWire = -9; 
  truthMdcEdep = -9;
  truthMdcDriftD = -9;
  truthMdcX = -9;
  truthMdcY = -9;
  truthMdcZ = -9;
  ntruthMdc = 0;
  
  m_layer = -9;
  m_cell = -9;
  m_charge = -9;
  m_time = -9;
}     


void McTestAlg::RetrieveMdc()
{
  if(m_mdcRootFlag)  
     MdcInit();

  //retrieve MDC McTruth from TDS
  SmartDataPtr<Event::MdcMcHitCol> aMcHitCol(m_evtSvc,"/Event/MC/MdcMcHitCol");
  if(!aMcHitCol)
    std::cout<<"Could not retrieve MDC McTruth collection"<<std::endl;
  else
  {
    Event::MdcMcHitCol::iterator iMcHitCol;
    for(iMcHitCol=aMcHitCol->begin(); iMcHitCol!=aMcHitCol->end(); iMcHitCol++)
    {
      const Identifier ident = (*iMcHitCol)->identify();
      //std::cout<<(*iMcHitCol)->getTrackIndex();
      //std::cout<<" "<<MdcID::layer(ident);
      //std::cout<<" "<<MdcID::wire(ident);
      //std::cout<<" "<<(*iMcHitCol)->getDepositEnergy();
      //std::cout<<" "<<(*iMcHitCol)->getDriftDistance();
      //std::cout<<" "<<(*iMcHitCol)->getPositionX();
      //std::cout<<" "<<(*iMcHitCol)->getPositionY();
      //std::cout<<" "<<(*iMcHitCol)->getPositionZ();
      //std::cout<<std::endl;

     if(m_mdcRootFlag)
     {
      truthMdcIndex = (*iMcHitCol)->getTrackIndex();
      truthMdcLayer = MdcID::layer(ident);
      truthMdcWire = MdcID::wire(ident);
      truthMdcEdep = (*iMcHitCol)->getDepositEnergy();
      truthMdcDriftD = (*iMcHitCol)->getDriftDistance();
      truthMdcX = (*iMcHitCol)->getPositionX();
      truthMdcY = (*iMcHitCol)->getPositionY();
      truthMdcZ = (*iMcHitCol)->getPositionZ();
      ntruthMdc++; 
      tupleMdc1->write();
     }   
    }
    //std::cout<<"end of retrieve MDC McTruth collection"<<std::endl;
  }
  
  //retrieve MDC digits from TDS
  SmartDataPtr<MdcDigiCol> aDigiCol(m_evtSvc,"/Event/Digi/MdcDigiCol");
  if(!aDigiCol)
    std::cout<<"Could not retrieve MDC digi collection"<<std::endl;

  else
  {
    MdcDigiCol::iterator iDigiCol;
    for(iDigiCol=aDigiCol->begin(); iDigiCol!=aDigiCol->end(); iDigiCol++)
    {
      const Identifier ident = (*iDigiCol)->identify();
      //std::cout<<"layer: "<<MdcID::layer(ident);
      //std::cout<<"  cell: "<<MdcID::wire(ident);
      //std::cout<<"  charge: "<<(*iDigiCol)->getChargeChannel();
      //std::cout<<"  time: "<<(*iDigiCol)->getTimeChannel()<<std::endl;
      
     if(m_mdcRootFlag){     
      m_layer = MdcID::layer(ident);
      m_cell = MdcID::wire(ident);
      m_charge = (*iDigiCol)->getChargeChannel()/1.0e6;
      m_time = (*iDigiCol)->getTimeChannel()/1.0e5;
      tupleMdc2->write();
      }
    }
    //std::cout<<"end of retrieve MDC digi collection"<<std::endl;
  }
}

void McTestAlg::TofInit()
{ 
  truthIndex = -9;
  truthPartId = -9;
  truthLayer = -9;
  truthScinNb = -9;
  truthX = -9;
  truthY = -9;
  truthZ = -9;
  ntruth = 0;
  tleft = -9;
  tright = -9;
  qleft = -9;
  qright = -9;
}


void McTestAlg::RetrieveTof()
{ 
  int partId,layer,scinNb,end;
  double charge,time; 
  partId = layer = scinNb = end = -9;
  charge = time = -9;
  if(m_tofRootFlag)
    TofInit();

  //retrieve TOF McTruth from TDS
  SmartDataPtr<Event::TofMcHitCol> aMcHitCol(m_evtSvc,"/Event/MC/TofMcHitCol");
  if(!aMcHitCol)
    std::cout<<"Could not retrieve TOF McTruth collection"<<std::endl;
  else
  {
    Event::TofMcHitCol::iterator iMcHitCol;
    for(iMcHitCol=aMcHitCol->begin(); iMcHitCol!=aMcHitCol->end(); iMcHitCol++)  
    {
      const Identifier ident = (*iMcHitCol)->identify();
      /*std::cout<<(*iMcHitCol)->getTrackIndex();
      std::cout<<" "<<TofID::barrel_ec(ident);
      std::cout<<" "<<TofID::layer(ident);
      std::cout<<" "<<TofID::phi_module(ident);
      std::cout<<" "<<(*iMcHitCol)->getPositionX();
      std::cout<<" "<<(*iMcHitCol)->getPositionY();
      std::cout<<" "<<(*iMcHitCol)->getPositionZ();
      std::cout<<" "<<(*iMcHitCol)->getPx();
      std::cout<<" "<<(*iMcHitCol)->getPy();
      std::cout<<" "<<(*iMcHitCol)->getPz();
      std::cout<<" "<<(*iMcHitCol)->getTrackLength();
      std::cout<<" "<<(*iMcHitCol)->getFlightTime();
      std::cout<<std::endl;*/
      if(m_tofRootFlag)
      {
        truthIndex = (*iMcHitCol)->getTrackIndex();
        truthPartId = TofID::barrel_ec(ident);
        truthLayer = TofID::layer(ident);
        truthScinNb = TofID::phi_module(ident);
        truthX = (*iMcHitCol)->getPositionX();
        truthY = (*iMcHitCol)->getPositionY();
        truthZ = (*iMcHitCol)->getPositionZ();
        ntruth++;
      }	
    }
  }

  //retrieve TOF digits from TDS
  SmartDataPtr<TofDigiCol> aDigiCol(m_evtSvc,"/Event/Digi/TofDigiCol");
  if(!aDigiCol)
    std::cout<<"Could not retrieve TOF digi collection"<<std::endl;
  else
  { 
    TofDigiCol::iterator iDigiCol;
    for(iDigiCol=aDigiCol->begin(); iDigiCol!=aDigiCol->end(); iDigiCol++)
    {
      const Identifier ident = (*iDigiCol)->identify();
      //std::cout<<"partId: "<<TofID::barrel_ec(ident);
      //std::cout<<"  layer: "<<TofID::layer(ident);
      //std::cout<<"  scinNb: "<<TofID::phi_module(ident);
      //std::cout<<"  end: "<<TofID::end(ident);
      //std::cout<<std::endl;
      //std::cout<<"  charge: "<<(*iDigiCol)->getChargeChannel();
      //std::cout<<"  time: "<<(*iDigiCol)->getTimeChannel()<<std::endl;
      //if(TofID::barrel_ec(ident)==barrel_ec && layer == TofID::layer(ident) && 
      //   phi_module == TofID::phi_module(ident) )
      partId=TofID::barrel_ec(ident);
      layer=TofID::layer(ident);
      scinNb=TofID::phi_module(ident);
      end=TofID::end(ident);
      charge = (*iDigiCol)->getChargeChannel()/1.0e6;
      time = (*iDigiCol)->getTimeChannel()/1.0e6;
      if(m_tofRootFlag)
      {
        if(truthPartId==partId && truthLayer==layer && truthScinNb==scinNb)
        {
          if(end==0) {qright = charge; tright=time;}
          else  {qleft = charge; tleft = time;}
         //std::cout<<partId<<" "<<scinNb<<" "<<charge<<" "<<time<<std::endl; 
        }
        else
          std::cout<<"digi doesn't match"<<std::endl;
       }
     }
     if(m_tofRootFlag)
     {
       if(tleft>0&&tright>0&&qleft>0&&qright>0)
         tupleTof->write();
       else
         std::cout<<"no digi match MCtruth"<<std::endl;
     }
   
    //std::cout<<"end of retrieve TOF digits"<<std::endl;
  }
}  

void McTestAlg::RetrieveEmc()
{
  //retrieve EMC McTruth from TDS
  SmartDataPtr<Event::EmcMcHitCol> aMcHitCol(m_evtSvc,"/Event/MC/EmcMcHitCol");
  if(!aMcHitCol)
    std::cout<<"Could not retrieve EMC McTruth collection"<<std::endl;
  else
  {
    Event::EmcMcHitCol::iterator iMcHitCol;
    for(iMcHitCol=aMcHitCol->begin(); iMcHitCol!=aMcHitCol->end(); iMcHitCol++)
    {
      const Identifier ident = (*iMcHitCol)->identify();
      //std::cout<<(*iMcHitCol)->getTrackIndex();
      //std::cout<<" "<<EmcID::barrel_ec(ident);
      //std::cout<<" "<<EmcID::theta_module(ident);
      //std::cout<<" "<<EmcID::phi_module(ident);
      //std::cout<<" "<<(*iMcHitCol)->getPositionX();
      //std::cout<<" "<<(*iMcHitCol)->getPositionY();
      //std::cout<<" "<<(*iMcHitCol)->getPositionZ();
      //std::cout<<" "<<(*iMcHitCol)->getPx();
      //std::cout<<" "<<(*iMcHitCol)->getPy();
      //std::cout<<" "<<(*iMcHitCol)->getPz();
      //std::cout<<" "<<(*iMcHitCol)->getDepositEnergy();
      //std::cout<<std::endl;
    }
    //std::cout<<"end of retrieve EMC McTruth"<<std::endl;
  }

  //retrieve EMC digits from TDS
  SmartDataPtr<EmcDigiCol> aDigiCol(m_evtSvc,"/Event/Digi/EmcDigiCol");
  if(!aDigiCol)
    std::cout<<"Could not retrieve EMC digi collection"<<std::endl;

  else
  {
    EmcDigiCol::iterator iDigiCol;
    for(iDigiCol=aDigiCol->begin(); iDigiCol!=aDigiCol->end(); iDigiCol++)
    {
      const Identifier ident = (*iDigiCol)->identify();
      //std::cout<<"barrel_ec: "<<EmcID::barrel_ec(ident);
      //std::cout<<"  theta: "<<EmcID::theta_module(ident);
      //std::cout<<"  phi: "<<EmcID::phi_module(ident);
      //std::cout<<"  charge: "<<(*iDigiCol)->getChargeChannel();
      //std::cout<<"  time: "<<(*iDigiCol)->getTimeChannel()<<std::endl;
    }
    //std::cout<<"end of retrieve EMC digits"<<std::endl;
  }
}
 
void McTestAlg::RetrieveMuc()
{
  //retrieve MUC McTruth from TDS
  SmartDataPtr<Event::MucMcHitCol> aMcHitCol(m_evtSvc,"/Event/MC/MucMcHitCol");
  if(!aMcHitCol)
    std::cout<<"Could not retrieve MUC McTruth collection"<<std::endl;
  else
  {
    Event::MucMcHitCol::iterator iMcHitCol;
    for(iMcHitCol=aMcHitCol->begin(); iMcHitCol!=aMcHitCol->end(); iMcHitCol++)
    {
      const Identifier ident = (*iMcHitCol)->identify();
      //std::cout<<(*iMcHitCol)->getTrackIndex();
      //std::cout<<" "<<MucID::part(ident);
      //std::cout<<" "<<MucID::seg(ident);
      //std::cout<<" "<<MucID::gap(ident);
      //std::cout<<" "<<MucID::strip(ident);
      //std::cout<<" "<<(*iMcHitCol)->getPositionX();
      //std::cout<<" "<<(*iMcHitCol)->getPositionY();
      //std::cout<<" "<<(*iMcHitCol)->getPositionZ();
      //std::cout<<" "<<(*iMcHitCol)->getPx();
      //std::cout<<" "<<(*iMcHitCol)->getPy();
      //std::cout<<" "<<(*iMcHitCol)->getPz();
      //std::cout<<std::endl;
    }
    //std::cout<<"end of retrieve MUC McTruth"<<std::endl;
  }

  //retrieve MUC digits from TDS
  SmartDataPtr<MucDigiCol> aDigiCol(m_evtSvc,"/Event/Digi/MucDigiCol");
  if(!aDigiCol)
    std::cout<<"Could not retrieve MUC digi collection"<<std::endl;

  else
  {
    MucDigiCol::iterator iDigiCol;
    for(iDigiCol=aDigiCol->begin(); iDigiCol!=aDigiCol->end(); iDigiCol++)
    {
      const Identifier ident = (*iDigiCol)->identify();
      //std::cout<<"Part: "<<MucID::part(ident);
      //std::cout<<"  Seg: "<<MucID::seg(ident);
      //std::cout<<"  Gap: "<<MucID::gap(ident);
      //std::cout<<"  Strip: "<<MucID::strip(ident)<<std::endl;
    }
    //std::cout<<"end of retrieve MUC digits"<<std::endl;
  }
}



