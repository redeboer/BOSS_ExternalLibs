//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description:
////Author:   Dengzy
////Created:  Mar, 2004
////Modified:
////Comment:
//
#include "BesMdcDigi.hh"
#include "BesTofDigi.hh"
#include "BesEmcDigi.hh"
#include "BesMucDigi.hh"
#include "BesRawDataWriter.hh"
#include "G4DigiManager.hh"
#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/MsgStream.h"

#include "MdcRawEvent/MdcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "EmcRawEvent/EmcDigi.h"
#include "MucRawEvent/MucDigi.h"

#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "Identifier/TofID.h"
#include "Identifier/EmcID.h"
#include "Identifier/MucID.h"

#include "RawEvent/RawDataUtil.h"
#include "RawEvent/DigiEvent.h"
#include "GaudiKernel/SmartDataPtr.h"


BesRawDataWriter::BesRawDataWriter()
{
  m_DigiMan = G4DigiManager::GetDMpointer(); 
  //yuany 2005-2-28
  mdcGeoPointer=BesMdcGeoParameter::GetGeo();

  IG4Svc* tmpSvc;
  StatusCode sc=Gaudi::svcLocator()->service("G4Svc", tmpSvc);
  if (!sc.isSuccess())
    G4cout <<" BesRawDataWriter::Error,could not open G4Svc"<<G4endl;
  m_G4Svc=dynamic_cast<G4Svc *>(tmpSvc); 

  IRealizationSvc *tmpReal;
  StatusCode scReal = Gaudi::svcLocator()->service("RealizationSvc",tmpReal);
  if (!scReal.isSuccess())
  {
    std::cout << " Could not initialize Realization Service in BesRawDataWriter" << std::endl;
  } 
  else 
  {
    m_RealizationSvc=dynamic_cast<RealizationSvc*>(tmpReal);
  }

}

BesRawDataWriter::~BesRawDataWriter()
{
}

void BesRawDataWriter::SaveDigits()
{
  //interface to event data service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc=svcLocator->service("EventDataSvc", m_evtSvc);
  if (sc.isFailure())
    G4cout<<"Could not accesss EventDataSvc!"<<G4endl;

  DigiEvent* aDigiEvent = new DigiEvent;
  sc = m_evtSvc->registerObject("/Event/Digi",aDigiEvent);
  if(sc!=StatusCode::SUCCESS) {
    G4cout<< "Could not register DigiEvent" <<G4endl;
  }

  SaveMdcDigits();
  SaveTofDigits();
  SaveEmcDigits();
  SaveMucDigits();
}

void BesRawDataWriter::SaveMdcDigits()
{
  //mdc digis collection defined in BOSS
  MdcDigiCol* aMdcDigiCol = new MdcDigiCol;

  G4int mdcDigiCollID = -1;
  mdcDigiCollID = m_DigiMan->GetDigiCollectionID("BesMdcDigisCollection");
  if(mdcDigiCollID>=0)
  {  
    BesMdcDigisCollection* mdcDC = (BesMdcDigisCollection*)m_DigiMan->GetDigiCollection(mdcDigiCollID);
    G4int nDigi = mdcDC->entries();
    if(nDigi>0)
    {
      //push back mdc digits to MdcDigiCol in BOSS
      BesMdcDigi* mdcDigi;
      for(int i=0;i<nDigi;i++)
      {
        mdcDigi=(*mdcDC)[i];
        unsigned int charge;
        if(m_G4Svc->GetMdcDedxFlag()==1){
          charge = int(mdcDigi->GetEdep());
        }else{
          charge = RawDataUtil::MdcChargeChannel(mdcDigi->GetEdep());
        }
        unsigned int time = RawDataUtil::MdcTimeChannel(mdcDigi->GetDriftT());
        const Identifier ident =  MdcID::wire_id ( mdcDigi->GetLayerNo(), mdcDigi->GetCellNo() );

        MdcDigi* aMdcDigi = new MdcDigi(ident, time, charge);  
        aMdcDigi->setTrackIndex(mdcDigi->GetTrackID());
        aMdcDigiCol->push_back(aMdcDigi);      
      }
    }
  }

  //register MDC digits collection to TDS
  StatusCode scMdc = m_evtSvc->registerObject("/Event/Digi/MdcDigiCol", aMdcDigiCol);
  if(scMdc!=StatusCode::SUCCESS) 
    G4cout<< "Could not register MDC digi collection" <<G4endl;

  //retrieve MDC digits from TDS
  /*SmartDataPtr<MdcDigiCol> aDigiCol(m_evtSvc,"/Event/Digi/MdcDigiCol");
    if(!aDigiCol)
    G4cout<<"Could not retrieve MDC digi collection"<<G4endl;

    MdcDigiCol::iterator iDigiCol;
    for(iDigiCol=aDigiCol->begin(); iDigiCol!=aDigiCol->end(); iDigiCol++)
    {
    const Identifier ident = (*iDigiCol)->identify();
    G4cout<<"layer: "<<MdcID::layer(ident);
    G4cout<<"  cell: "<<MdcID::wire(ident);
    G4cout<<"  charge: "<<(*iDigiCol)->getChargeChannel();
    G4cout<<"  time: "<<(*iDigiCol)->getTimeChannel()<<G4endl;
    }
    G4cout<<"end of retrieve MDC digi collection"<<G4endl;
   */
}

void BesRawDataWriter::SaveTofDigits() {
  
  //Tof digits collection defined in BOSS
  TofDigiCol* aTofDigiCol = new TofDigiCol;

  G4int tofDigiCollID = -1;
  tofDigiCollID = m_DigiMan->GetDigiCollectionID("BesTofDigitsCollection");

  if( tofDigiCollID>=0 ) {
    BesTofDigitsCollection* tofDC = (BesTofDigitsCollection*)m_DigiMan->GetDigiCollection(tofDigiCollID);
    G4int nDigi = tofDC->entries();
    if( nDigi>0 ) {
      //arrange digis in digitsCollection in order of trackIndex
      BesTofDigi* digi;
      vector<BesTofDigi*>* vecDC = tofDC->GetVector();
      for( int i=0; i<nDigi-1; i++ ) {
        for( int j=i+1; j<nDigi; j++ ) {
          if( (*vecDC)[i]->GetTrackIndex()>(*vecDC)[j]->GetTrackIndex() ) {
            digi = (*vecDC)[i];
            (*vecDC)[i] = (*vecDC)[j];
            (*vecDC)[j] = digi;
          }
	}
      }

      //push back tof digits to TofDigiCol in BOSS
      for( int i=0; i<nDigi; i++ ) {
	digi = (*tofDC)[i];
	
	// for Scintillator, scinNum is the number of scintillator
	// for MRPC, scinNum is the number of module
	G4int scinNum = digi->GetScinNb();
	// for Scintillator, barrel_ec is 0 for east endcap, 1 for barrel, and 2 for west endcap
	// for MRPC, barrel_ec is 100 + module number for east, 200 + module number for west
	G4int barrel_ec = digi->GetPartId();

	if( TofID::is_scin( barrel_ec ) ) { // for Scintillator
	  unsigned int layer = 0;
	  if (TofID::is_barrel(barrel_ec) && scinNum > TofID::getPHI_BARREL_MAX()) {
	    layer = 1;
	    scinNum = scinNum -  TofID::getPHI_BARREL_MAX() - 1;
	  }
	  
	  // Store tofDigi for end==0
	  Identifier ident = TofID::cell_id ( barrel_ec, layer, scinNum, 0); 
	  unsigned int time;
	  if( digi->GetForwTDC()>0 ) {
	    time = RawDataUtil::TofTimeChannel(digi->GetForwTDC());
	  }
	  else {
	    time = 0;
	  }
	  
	  unsigned int charge;
	  if( digi->GetForwADC()>0 ) {
	    // ADC linear conversion
	    charge = digi->GetForwADC(); // convert ADC(pC) of bTof into Q channel in Data
	    // Treat ADC overflow
	    if( charge>=8192 ) {
	      // The overlow flag is the 20th bit of the ADC_channel.
	      charge = (charge | 0x080000); // if ADC_channel>=8192, set overflow flag = 1.
	    }
	    charge = ( (time & 0x07e000) | charge ); // Store tclock into ADC_channel as qclock
	  }
	  else {
	    charge =0;
	  }
	  
	  TofDigi* tofDigi = new TofDigi(ident, time, charge);
	  if( charge & 0x80000 ) {// Treat overflow flag(20th bit)
	    tofDigi->setOverflow(0x2);
	  }
	  tofDigi->setTrackIndex(digi->GetTrackIndex());
	  aTofDigiCol->push_back(tofDigi);
	  
	  // Store tofDigi for end==1
	  ident = TofID::cell_id ( barrel_ec, layer, scinNum, 1); 
	  if( digi->GetBackTDC()>0 ) {
	    time = RawDataUtil::TofTimeChannel(digi->GetBackTDC());
	  }
	  else {
	    time = 0;
	  }
	  
	  if( digi->GetBackADC()>0 ) {
	    // ADC linear conversion
	    charge = digi->GetBackADC();
	    // Treat ADC overflow
	    if( charge>=8192 ) {
	      // The overlow flag is the 20th bit of the ADC_channel.
	      charge = (charge | 0x080000); // if ADC_channel>=8192, set overflow flag = 1.
	    }
	    charge = ( (time & 0x07e000) | charge); // Store tclock into ADC_channel as qclock
	  }
	  else {
	    charge = 0;
	  }
	  
	  tofDigi = new TofDigi(ident, time, charge);
	  if( charge & 0x80000 ) { // Treat overflow flag(20th bit)
	    tofDigi->setOverflow(0x2);
	  }
	  tofDigi->setTrackIndex(digi->GetTrackIndex());
	  aTofDigiCol->push_back(tofDigi);
	}  // end if is_scin
	else { // for ETF(MRPC)
	  if( barrel_ec==3 || barrel_ec==4 ) {
	    unsigned int endcap = 0;
	    unsigned int module = digi->GetModule();
	    unsigned int strip = digi->GetStrip();
	    if( barrel_ec==4 ) { // west end cap
	      endcap = 1;
	    }
	    
	    // Store tofDigi for end==0
	    Identifier ident = TofID::cell_id( 3, endcap, module, strip, 0 ); 
	    unsigned int tleading;
	    if( digi->GetForwT1() > 0 ) {
	      tleading = RawDataUtil::TofTimeChannel( digi->GetForwT1() );
	    }
	    else { tleading = 0; }
	    unsigned int ttrailing;
	    if( digi->GetForwT2() > 0 ) {
	      ttrailing = RawDataUtil::TofTimeChannel( digi->GetForwT2() );
	    }
	    else { ttrailing = 0; }
	    TofDigi* tofDigi = new TofDigi( ident, tleading, ttrailing );
	    tofDigi->setTrackIndex( digi->GetTrackIndex() );
	    aTofDigiCol->push_back( tofDigi );
	    /*
	    G4cout << "  --------- BesRawDataWriter: TOF ------------------  " << G4endl;
	    G4cout << " endcap=" << endcap << " module=" << module << " strip=" << strip << G4endl;
	    G4cout << " tleading=" << tleading << " ttrailing=" << ttrailing << G4endl;
	    G4cout << "  --------- BesRawDataWriter: TOF ------------------  " << G4endl;
	    */
	    // Store tofDigi for end==1
	    ident = TofID::cell_id( 3, endcap, module, strip, 1 ); 
	    if( digi->GetBackT1() > 0 ) {
	      tleading = RawDataUtil::TofTimeChannel( digi->GetBackT1() );
	    }
	    else { tleading = 0; }
	    if( digi->GetBackT2() > 0 ) {
	      ttrailing = RawDataUtil::TofTimeChannel( digi->GetBackT2() );
	    }
	    else { ttrailing = 0; }
	    tofDigi = new TofDigi( ident, tleading, ttrailing );
	    tofDigi->setTrackIndex( digi->GetTrackIndex() );
	    aTofDigiCol->push_back( tofDigi );
	  }
	} // end if is_MRPC
	
      }
    }
  }
  
  //register TOF digits collection to TDS
  StatusCode scTof = m_evtSvc->registerObject("/Event/Digi/TofDigiCol", aTofDigiCol);
  if( scTof!=StatusCode::SUCCESS ) {
    G4cout<< "Could not register TOF digi collection" <<G4endl;
  }
  return;
}  

void BesRawDataWriter::SaveEmcDigits()
{
  //Emc digits collection defined in BOSS
  EmcDigiCol* aEmcDigiCol = new EmcDigiCol;

  G4int emcDigiCollID = -1;
  emcDigiCollID = m_DigiMan->GetDigiCollectionID("BesEmcDigitsCollection");
  if(emcDigiCollID>=0)
  {
    BesEmcDigitsCollection* emcDC = (BesEmcDigitsCollection*)m_DigiMan->GetDigiCollection(emcDigiCollID);
    G4int nDigi = emcDC->entries();
    if(nDigi>0)
    {
      //arrange digis in digitsCollection in order of trackIndex
      BesEmcDigi* digi;
      vector<BesEmcDigi*>* vecDC = emcDC->GetVector();
      for(int i=0;i<nDigi-1;i++)
        for(int j=i+1;j<nDigi;j++)
          if((*vecDC)[i]->GetTrackIndex()>(*vecDC)[j]->GetTrackIndex())
          {
            digi = (*vecDC)[i];
            (*vecDC)[i] = (*vecDC)[j];
            (*vecDC)[j] = digi;
          }
      //push back EmcDigi to EmcDigiCol in BOSS
      for(int i=0;i<nDigi;i++)
      {
        digi = (*emcDC)[i];

        int charge = RawDataUtil::EmcChargeChannel(digi->GetEnergy());
        int measure = RawDataUtil::EmcChargeMeasure(digi->GetEnergy());
        int time = RawDataUtil::EmcTimeChannel(digi->GetTime());
        Identifier ident =  EmcID::crystal_id (digi->GetPartId(), digi->GetThetaNb(),  digi->GetPhiNb());
        EmcDigi* emcDigi = new EmcDigi(ident, time, charge);
        emcDigi->setTrackIndex(digi->GetTrackIndex());
        emcDigi->setMeasure(measure);
        aEmcDigiCol->push_back(emcDigi);
      }
    }
  }

  //register EMC digits collection to TDS
  StatusCode scEmc = m_evtSvc->registerObject("/Event/Digi/EmcDigiCol", aEmcDigiCol);
  if(scEmc!=StatusCode::SUCCESS)
    G4cout<< "Could not register EMC digi collection" <<G4endl;

  //retrieve EMC digits from TDS
  /*SmartDataPtr<EmcDigiCol> aDigiCol(m_evtSvc,"/Event/Digi/EmcDigiCol");
    if(!aDigiCol)
    G4cout<<"Could not retrieve EMC digi collection"<<G4endl;

    EmcDigiCol::iterator iDigiCol;
    for(iDigiCol=aDigiCol->begin(); iDigiCol!=aDigiCol->end(); iDigiCol++)
    {
    const Identifier ident = (*iDigiCol)->identify();
    G4cout<<"barrel_ec: "<<EmcID::barrel_ec(ident);
    G4cout<<"  theta: "<<EmcID::theta_module(ident);
    G4cout<<"  phi: "<<EmcID::phi_module(ident);
    G4cout<<"  charge: "<<(*iDigiCol)->getChargeChannel();
    G4cout<<"  time: "<<(*iDigiCol)->getTimeChannel()<<G4endl;
    }
    G4cout<<"end of retrieve EMC digits"<<G4endl;
   */
}

void BesRawDataWriter::SaveMucDigits()
{
  //Muc digits collection defined in BOSS
  MucDigiCol* aMucDigiCol = new MucDigiCol;

  G4int mucDigiCollID =-1;
  mucDigiCollID = m_DigiMan->GetDigiCollectionID("BesMucDigisCollection");
  if(mucDigiCollID>=0)
  {
    BesMucDigisCollection* mucDC = (BesMucDigisCollection*)m_DigiMan->GetDigiCollection(mucDigiCollID);
    G4int nDigi = mucDC->entries();

    if(nDigi > 0) {
      BesMucDigi* digi;
      for(int i = 0; i < nDigi; i++) {
        digi = (*mucDC)[i];
        Identifier ident =  MucID::channel_id( digi->GetPartId(), digi->GetSegId(),digi->GetGapId(),digi->GetStripId() );
        MucDigi* mucDigi = new MucDigi(ident);
        mucDigi->setTrackIndex(digi->GetTrackIndex());
        aMucDigiCol->push_back(mucDigi);
      }
    }
  }

  //register MUC digits collection to TDS
  StatusCode scMuc = m_evtSvc->registerObject("/Event/Digi/MucDigiCol", aMucDigiCol);
  if(scMuc!=StatusCode::SUCCESS)
    G4cout<< "Could not register MUC digi collection" <<G4endl;

  //retrieve MUC digits from TDS
  /*SmartDataPtr<MucDigiCol> aDigiCol(m_evtSvc,"/Event/Digi/MucDigiCol");
    if(!aDigiCol)
    G4cout<<"Could not retrieve MUC digi collection"<<G4endl;

    MucDigiCol::iterator iDigiCol;
    for(iDigiCol=aDigiCol->begin(); iDigiCol!=aDigiCol->end(); iDigiCol++)
    {
    const Identifier ident = (*iDigiCol)->identify();
    G4cout<<"Part: "<<MucID::part(ident);
    G4cout<<"  Seg: "<<MucID::seg(ident);
    G4cout<<"  Gap: "<<MucID::gap(ident);
    G4cout<<"  Strip: "<<MucID::strip(ident)<<G4endl;
    }
    G4cout<<"end of retrieve MUC digits"<<G4endl;
   */
}  
