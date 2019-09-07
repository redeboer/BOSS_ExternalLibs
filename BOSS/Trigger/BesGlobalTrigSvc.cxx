//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     /
////
////---------------------------------------------------------------------------/
////
////Description:
////Author:   Caogf
////Created:  Feb, 2006
////Modified:
////Comment:
////
//
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"

#include "Trigger/BesGlobalTrigSvc.h"
#include "Trigger/BesMdcTrig.h"
#include "Trigger/BesTofTrig.h"
#include "Trigger/BesEmcTrig.h"
#include "Trigger/BesMucTrig.h"
#include "Trigger/BesTMTrig.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace CLHEP;
using namespace std;

const std::string BesGlobalTrigSvc::COND_NAME[] = {
   "NClus1",
   "NClus2",
   "BClusBB",
   "EClusBB",
   "Clus_Z",   
   "BClus_PHI", 
   "EClus_PHI", 
   "BEtot_H",   
   "EEtot_H",   
   "Etot_L",    
   "Etot_M",    
   "BL_Z",      
   "NBClus1",    
   "NEClus1",    
   "BL_BBLK",    
   "BL_EBLK",   
   "ETofBB",    
   "BTofBB",    
   "NETof2",    
   "NETof1",    
   "NBTof2",    
   "NBTof1",
   "NTof1",
   "CTrk_BB",
   "NCTrk2",
   "NCTrk1",
   "BTrk_BB",
   "NBTrk2",
   "NBTrk1",
   "ATrk_BB",
   "NATrk2",
   "NATrk1",
   "MUONBB3478",
   "MUONBB37",  
   "NotDefined",
   "NotDefined",
   "NotDefined",
   "NotDefined",
   "STrk_BB",   
   "NStrkN",    
   "NStrk2",    
   "NStrk1",    
   "LTrk_BB",   
   "NLtrkN",    
   "NLtrk2",    
   "NLtrk1",
   "NItrk2",
   "NItrk1"
};

// Instantiation of a static factory class used by clients to create
// instances of this service
//static SvcFactory<BesGlobalTrigSvc> s_factory;
//const ISvcFactory& BesGlobalTrigSvcFactory = s_factory;

BesGlobalTrigSvc::BesGlobalTrigSvc( const std::string& name,
            ISvcLocator* svc ) : Service( name, svc ),m_MdcTrig(0),m_TofTrig(0),m_EmcTrig(0),m_MucTrig(0),m_TMTrig(0)
{
  trigTableFile = std::string(getenv( "TRIGGERROOT" ));
  trigTableFile += std::string("/share/TriggerTable.txt");
  declareProperty("TrigTableCon", trigTableFile);
  declareProperty("TofLayerControl", m_tofLayer = 2);
  declareProperty("L1TC_GATE", L1TC_GATE = 70);
  declareProperty("L1TC_THRESH", L1TC_THRESH = 130);
  declareProperty("L1ETOT_L", L1ETOT_L = 0x56);
  declareProperty("L1ETOT_M", L1ETOT_M = 0xba); //0xba(run10584),0x190,0x10a(run9571)
  declareProperty("L1ETOT_BR", L1ETOT_BR = 0x19a);//0x216,0x19a
  declareProperty("L1ETOT_EC", L1ETOT_EC = 0x19a);//0x1bd,0x19a
  declareProperty("L1EBL_BR", L1EBL_BR = 0x30);
  declareProperty("L1EBL_EC", L1EBL_EC = 0x1ec);
  declareProperty("L1EDIFF_BR", L1EDIFF_BR = 0xf6);
  declareProperty("L1EDIFF_EC", L1EDIFF_EC = 0xe1);
  declareProperty("L1BLK_GATE", L1BLK_GATE = 0x30);
  declareProperty("L1EBL_Z", L1EBL_Z = 0xa4);
  declareProperty("MAXMDCTRACK", MAXMDCTRACK = 4);
  declareProperty("TM_ENDCAP_MODE", tm_endcap = 0);
  declareProperty("Energy_Ratio", Energy_Ratio = 1.);
}
BesGlobalTrigSvc::~BesGlobalTrigSvc()
{
}
//=============================================================================
// Initialize
//=============================================================================
StatusCode BesGlobalTrigSvc::initialize()
{
  MsgStream log(msgSvc(), name());

  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) return sc;

  StatusCode status;
  // Get the references to the services that are needed by the ApplicationMgr itself
  IIncidentSvc* incsvc;
  status = service("IncidentSvc", incsvc);
  int priority = 100;
  if( status.isSuccess() ){
    incsvc -> addListener(this, "NewRun", priority);
  }

  IRealizationSvc *tmpReal;
  status = service("RealizationSvc",tmpReal);
  if (!status.isSuccess())
  {
     log << MSG::FATAL << " Could not initialize Realization Service" << endreq;
  } else {
    m_RealizationSvc=dynamic_cast<RealizationSvc*>(tmpReal);
  }

  m_MdcTrig = new BesMdcTrig();
  m_TofTrig = new BesTofTrig();
  m_EmcTrig = new BesEmcTrig();
  m_MucTrig = new BesMucTrig();  
  m_TMTrig  = new BesTMTrig();

  if(!m_MdcTrig) log<<MSG::FATAL<<"can not get BesMdcTrig pointer "<<endreq;
  if(!m_TofTrig) log<<MSG::FATAL<<"can not get BesTofTrig pointer "<<endreq;
  if(!m_EmcTrig) log<<MSG::FATAL<<"can not get BesEmcTrig pointer "<<endreq;
  if(!m_MucTrig) log<<MSG::FATAL<<"can not get BesMucTrig pointer "<<endreq;
  if(!m_TMTrig) log<<MSG::FATAL<<"can not get BesTMTrig pointer "<<endreq;
 
  trigChannel.clear();
  trigCondName.clear();
  trigTable.clear();
  channelNo.clear();

  if(m_RealizationSvc->UseDBFlag() == false) {
    ifstream infile;
    infile.open(trigTableFile.c_str(),ios_base::in);
    if(!infile) log<<MSG::FATAL<<"can not open Trigger Table file: "<<trigTableFile<<endreq;
    char line[ 255 ];
    // Skip the header
    do{
        infile.getline( line, 255 );
    } while( line[0] != 'C' );
    //the name of each trigger channel
    std::string chan;
    int num;
    char* token = strtok( line, " " );
    do{
      if ( token ) { 
              chan = token;
              token = strtok( NULL, " " );
              if(chan!="CHANNEL") trigChannel.push_back(chan);
      }       
      else continue;
    }while ( token != NULL );
    //use this trigger channel, yes or no ?
    infile.getline( line, 255 );
    token = strtok( line, " " );
    do{
      if( token ) {
              chan = token;
              token = strtok(NULL, " ");
              num = atoi(chan.c_str());
              channelNo.push_back(num);
      }
      else continue;
    }while ( token!=NULL);
  
    while(infile) {
            infile.getline( line, 255 );
            if(line[0]==' ') continue;
            if(line[0]=='#') break;
            token = strtok( line, " " );
            do{
              if(token) {
                     chan = token;
                     token = strtok(NULL," ");
                     if(chan=="1"||chan=="0"||chan=="-") { trigTable.push_back(chan); }
                     else {
  			   trigCondName.push_back(chan);
  		   }
              }
              else continue;
            }while (token!=NULL);
    }
  
    for(unsigned int i=0;i<trigChannel.size();i++){ cout<<" "<<trigChannel[i]<<" ";}
    cout<<endl;
    for(unsigned int i=0;i<channelNo.size();i++){ cout<<" "<<channelNo[i]<<" ";}
    cout<<endl;
    for(unsigned int j=0;j<trigCondName.size();j++)
    {
      cout<<trigCondName[j]<<" ";
      for(unsigned int i=0+j*trigChannel.size();i<trigChannel.size()+j*trigChannel.size();i++)
      { cout<<trigTable[i]<<" ";}
      cout<<endl;
    }
    infile.close();
  }

  return StatusCode::SUCCESS;
}

StatusCode BesGlobalTrigSvc::finalize()
{
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "Service finalized successfully" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================
// QueryInterface
//=============================================================================
StatusCode BesGlobalTrigSvc::queryInterface( const InterfaceID& riid, 
                                             void** ppvInterface      )
{   
  if ( IBesGlobalTrigSvc::interfaceID().versionMatch(riid) )    {
     *ppvInterface = (IBesGlobalTrigSvc*)this;
  }else{
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

void BesGlobalTrigSvc::handle(const Incident& inc) {
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;
  if ( inc.type() != "NewRun" ){
    return;
  } 
  log << MSG::DEBUG << "Begin New Run" << endreq;
  if(m_RealizationSvc->UseDBFlag() == true && m_RealizationSvc->ifReadTrg() == true) {

    std::vector<uint32_t> trgTable_DB = m_RealizationSvc->getTrgTable();

    trigChannel.clear();     
    trigCondName.clear();
    trigTable.clear();
    channelNo.clear();

    for(unsigned int i = 0, mask = 1; i <16; i++,mask<<=1) {
      if(trgTable_DB[0] & mask) {
        if(i == 9) channelNo.push_back(0);
        else channelNo.push_back(1);
      }
      else channelNo.push_back(0);
      std::ostringstream osname;
      osname << "Channel_"<<i;
      std::string name = osname.str();
      trigChannel.push_back(name);
    }
    
    string table_tmp[16][64];
    for(unsigned int i = 1; i <65; i++) {
      for(unsigned int j = 0; j <16; j++) {
        int bit2 = (trgTable_DB[i]>>2*j) & 0x3;
        int row = (int) (i-1)/4;
        int column = ((i-1)%4)*16 + j;
        if(bit2 == 0) table_tmp[row][column] = "1";
        else if(bit2 == 1) table_tmp[row][column] = "0";
        else table_tmp[row][column] = "-";
      }
    }

    for(int j = 0; j < 48; j++) {
      trigCondName.push_back(COND_NAME[j]);
      for(int i = 0; i < 16; i++) {
        trigTable.push_back(table_tmp[i][j]);
      }
    }
    //print
    for(unsigned int i=0;i<trigChannel.size();i++){ cout<<" "<<trigChannel[i]<<" ";}
    cout<<endl;
    for(unsigned int i=0;i<channelNo.size();i++){ cout<<" "<<channelNo[i]<<" ";}
    cout<<endl;
    for(unsigned int j=0;j<trigCondName.size();j++)
    {
      cout<<setw(15)<<trigCondName[j]<<" ";
      for(unsigned int i=0+j*trigChannel.size();i<trigChannel.size()+j*trigChannel.size();i++)
      { cout<<trigTable[i]<<" ";}
      cout<<endl;
    }
  }

}

int BesGlobalTrigSvc::getL1ETOT_L() { 
  L1ETOT_L = m_RealizationSvc->getVthEtotL();
  return L1ETOT_L; 
}

int BesGlobalTrigSvc::getL1ETOT_M() { 
  L1ETOT_M = m_RealizationSvc->getVthEtotM();
  return L1ETOT_M; 
}

int BesGlobalTrigSvc::getL1ETOT_BR() { 
  L1ETOT_BR = m_RealizationSvc->getVthBEtotH();
  return L1ETOT_BR; 
}

int BesGlobalTrigSvc::getL1ETOT_EC() { 
  L1ETOT_EC = m_RealizationSvc->getVthEEtotH();
  return L1ETOT_EC; 
}

int BesGlobalTrigSvc::getL1EBL_BR() { 
  L1EBL_BR = m_RealizationSvc->getVthBalBLK();
  return L1EBL_BR; 
}

int BesGlobalTrigSvc::getL1EBL_EC() { 
  L1EBL_EC = m_RealizationSvc->getVthBalEEMC();
  return L1EBL_EC; 
}

int BesGlobalTrigSvc::getL1EDIFF_BR() { 
  L1EDIFF_BR = m_RealizationSvc->getVthDiffB();
  return L1EDIFF_BR; 
}

int BesGlobalTrigSvc::getL1EDIFF_EC() { 
  L1EDIFF_EC = m_RealizationSvc->getVthDiffE();
  return L1EDIFF_EC; 
}

int BesGlobalTrigSvc::getL1BLK_GATE() { 
  L1BLK_GATE = m_RealizationSvc->getVthBalBLK();
  return L1BLK_GATE; 
}

int BesGlobalTrigSvc::getL1EBL_Z() { 
  L1EBL_Z = m_RealizationSvc->getVthBLZ();
  return L1EBL_Z; 
}

void BesGlobalTrigSvc::startMdcTrig() {
  if(m_MdcTrig) m_MdcTrig->startMdcTrig();
}

void BesGlobalTrigSvc::startTofTrig() {
  if(m_TofTrig) m_TofTrig->startTofTrig();
}

void BesGlobalTrigSvc::startEmcTrig() {
  if(m_EmcTrig) m_EmcTrig->startEmcTrig();
}

void BesGlobalTrigSvc::startTMTrig() {
  if(m_TMTrig) m_TMTrig->startTMTrig();
}

StatusCode BesGlobalTrigSvc::GlobalTrig()
{
  //initialize
  ifpass = false;
  trigOut.clear();

  for(int i = 0; i < 16; i++) {
    if(i < 16) m_trigChannel[i] = 0;
  }
 
  MsgStream log(msgSvc(), name());

  //get Muc trigger information
  muc_vlayerSeg = m_MucTrig->getNlayerSeg();
  
  muc_nlayerEE = m_MucTrig->getNlayerPart(0);
  muc_nlayerBR = m_MucTrig->getNlayerPart(1);
  muc_nlayerWE = m_MucTrig->getNlayerPart(2);
  muc_vhitLayer = m_MucTrig->getNhitLayer();
  muc_vhitSeg = m_MucTrig->getNhitSeg();
  muc_nhitEE = m_MucTrig->getNhitPart(0);
  muc_nhitBR = m_MucTrig->getNhitPart(1);
  muc_nhitWE = m_MucTrig->getNhitPart(2);
  muc_nhitTotal = m_MucTrig->getNhitTotal();

  //set trigger condition id
  StatusCode status;
  //StatusCode status = setTrigCondition();

  //make out trigger conditon table;
  for(unsigned int i = 0; i < 48; i++) {
    for(unsigned int j = 0; j < trigChannel.size(); j++) {
      if(trigCond[i]) trigOut.push_back(1);
      else trigOut.push_back(0);
    }
  }

  //compare trigger table and trigger output
  
  for(unsigned int i=0; i<channelNo.size(); i++)
  {
    if(channelNo.size() > 16) {
      log << MSG::FATAL <<"Trigger Channel is greater than 16!!!!!" << endreq;
      return StatusCode::FAILURE;
    }
    int condition = 0;
    int conditionNo = 0;
    if(channelNo[i]==0) 
    {
      m_trigChannel[i] = 0; //data structure member in TDS
      continue;
    }
    else
    {
      log<<MSG::INFO<<trigChannel[i]<<": ";
      for(unsigned int j=0; j<trigCondName.size(); j++)
      {
        if(trigTable[i+j*channelNo.size()]=="1"&&trigOut[i+j*channelNo.size()]==1) condition++;
        if(trigTable[i+j*channelNo.size()]=="0"&&trigOut[i+j*channelNo.size()]==0) condition++;
        if(trigTable[i+j*channelNo.size()]=="1") log<<MSG::INFO<<trigCondName[j]<<" ";
        if(trigTable[i+j*channelNo.size()]=="0") log<<MSG::INFO<<"anti_"<<trigCondName[j]<<" ";
      } 
      for(unsigned int k=0; k<trigCondName.size(); k++)
      {
        if(trigTable[i+k*channelNo.size()]=="1"||trigTable[i+k*channelNo.size()]=="0") { 
          log<<MSG::INFO<<trigOut[i+k*channelNo.size()]<<" "; 
          conditionNo++; }
      }
      log<<MSG::INFO<<endreq;
      if((condition == conditionNo) && conditionNo!=0) { 
        ifpass = true; 
        m_trigChannel[i] = 1; //data structure member in TDS
      }
      else {
        m_trigChannel[i] = 0; //data structure member in TDS
      }
    }
  }

  return status;
}

StatusCode BesGlobalTrigSvc::setTrigCondition()
{
  for(int i = 0; i < 48; i++) {
    trigCond[i] = false;
    m_trigCondition[i] = 0;
  }
  
  trigCond[0] = emc_NClus1;
  trigCond[1] = emc_NClus2;
  trigCond[2] = emc_BClusBB;
  trigCond[3] = emc_EClusBB;
  trigCond[4] = emc_Clus_Z;
  trigCond[5] = emc_BClus_PHI;
  trigCond[6] = emc_EClus_PHI;
  trigCond[7] = emc_BEtot_H;
  trigCond[8] = emc_EEtot_H;
  trigCond[9] = emc_Etot_L;
  trigCond[10] = emc_Etot_M;
  trigCond[11] = emc_BL_Z;
  trigCond[12] = emc_NBClus1;
  trigCond[13] = emc_NEClus1;
  trigCond[14] = emc_BL_BBLK;
  trigCond[15] = emc_BL_EBLK;
  trigCond[16] = tof_ETofBB;
  trigCond[17] = tof_BTofBB;
  trigCond[18] = tof_NETof2;
  trigCond[19] = tof_NETof1;
  trigCond[20] = tof_NBTof2;
  trigCond[21] = tof_NBTof1;
  trigCond[22] = tof_NTof1;
  trigCond[23] = tm_CTrk_BB;
  trigCond[24] = tm_NCTrk2;
  trigCond[25] = tm_NCTrk1;
  trigCond[26] = tm_BTrk_BB;
  trigCond[27] = tm_NBTrk2;
  trigCond[28] = tm_NBTrk1;
  trigCond[29] = tm_ATrk_BB;
  trigCond[30] = tm_NATrk2;
  trigCond[31] = tm_NATrk1;
  trigCond[32] = muc_BB3478;
  trigCond[33] = muc_BB37;
  trigCond[34] = false;
  trigCond[35] = false;
  trigCond[36] = false;
  trigCond[37] = false;
  trigCond[38] = mdc_STrk_BB;
  trigCond[39] = mdc_NStrkN;
  trigCond[40] = mdc_NStrk2;
  trigCond[41] = mdc_NStrk1;
  trigCond[42] = mdc_LTrk_BB;
  trigCond[43] = mdc_NLtrkN;
  trigCond[44] = mdc_NLtrk2;
  trigCond[45] = mdc_NLtrk1;
  trigCond[46] = mdc_NItrk2;
  trigCond[47] = mdc_NItrk1;

  MsgStream log(msgSvc(), name());
  if(trigCondName.size() != 48) {
    log << MSG::FATAL << "The trigger condition CAN NOT be successfully read!!!" << endreq;
    return StatusCode::FAILURE;
  }
 
  for(unsigned int i = 0; i < 48; i++) {
    if(trigCond[i]) m_trigCondition[i] = 1; //data structure member in TDS
  }

  return StatusCode::SUCCESS; 
}
