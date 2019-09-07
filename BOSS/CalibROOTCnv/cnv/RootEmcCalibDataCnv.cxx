// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/cnv/RootEmcCalibDataCnv.cxx,v 1.7 2009/05/13 07:13:06 hem Exp $
#include "GaudiKernel/MsgStream.h"
#include "RootEmcCalibDataCnv.h"
#include "CalibData/Emc/EmcCalibData.h"
#include "CalibDataSvc/IInstrumentName.h"

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TObject.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/GenericAddress.h"

#include "CalibDataSvc/ICalibRootSvc.h"    //maybe
#include "CalibDataSvc/ICalibMetaCnvSvc.h"

// Temporary.  Hope to find a better way to do this
#include "CalibData/CalibModel.h"
using namespace CalibData;
//static CnvFactory<RootEmcCalibDataCnv> EmcCal_factory;
//const  ICnvFactory& RootEmcCalibDataCnvFactory = EmcCal_factory;



RootEmcCalibDataCnv::RootEmcCalibDataCnv( ISvcLocator* svc) :
  RootCalBaseCnv(svc, CLID_Calib_EmcCal) { 

}


const CLID& RootEmcCalibDataCnv::objType() const {
  return CLID_Calib_EmcCal;
}

const CLID& RootEmcCalibDataCnv::classID() {
  return CLID_Calib_EmcCal;
}

StatusCode RootEmcCalibDataCnv::i_createObj(const std::string& fname,
                                       DataObject*& refpObject) {

  MsgStream log(msgSvc(), "RootEmcCalibDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;
 
  StatusCode sc = openRead(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
 
  CalibData::EmcCalibData *tmpObject = new CalibData::EmcCalibData ;
  // Read in our object
  int i;
  int nentries;
  
  // read DigiCalibConst ------------------------------------------------------------
     double EmcCalibConst[6240];
     int EmcIxtalNumber[6240];
     vector<double> DigiCalibConst;
     vector<int> IxtalNumber;
     TTree *Digitree = (TTree*)m_inFile -> Get("DigiCalibConst");
     Digitree -> SetBranchAddress("DigiCalibConst", EmcCalibConst);
     Digitree -> SetBranchAddress("IxtalNumber", EmcIxtalNumber);
     Digitree -> GetEntry(0);
     for(i=0; i<6240; i++){
           DigiCalibConst.push_back(EmcCalibConst[i]);
           IxtalNumber.push_back(EmcIxtalNumber[i]); 
     }
     tmpObject -> setDigiCalibConst(&DigiCalibConst);
     tmpObject -> setIxtalNumber(&IxtalNumber);
    
    refpObject=tmpObject;

  return StatusCode::SUCCESS;
}

StatusCode RootEmcCalibDataCnv::createRoot(const std::string& fname, 
                                        CalibData::CalibBase1* pTDSObj) {
 
  MsgStream log(msgSvc(), "RootEmcCalibDataCnv");

 // Open the file, create the branch
   StatusCode sc = openWrite(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
  // write the Data in the TCDS to RootFile
     CalibData::EmcCalibData* tmpObject = dynamic_cast<CalibData::EmcCalibData*>(pTDSObj);
     int tmpNo;
     double EmcCalibConst;
     int EmcIxtalNumber;
     int i;

     //DigiCalibConst------------------------------------------------------------------
     TTree *Digitree = new TTree("DigiCalibConst", "DigiCalibConst");
     Digitree -> Branch("DigiCalibConst", &EmcCalibConst, "EmcCalibConst/D");
     Digitree -> Branch("IxtalNumber", &EmcIxtalNumber, "IxtalNumber/I");
     tmpNo = tmpObject -> getDigiCalibConstNo();
     for(i=0; i<tmpNo; i++){
      	  EmcCalibConst = tmpObject -> getDigiCalibConst(i);
          EmcIxtalNumber = tmpObject -> getIxtalNumber(i);
	  Digitree -> Fill();
     }

     
     Digitree -> Write();

   
     delete Digitree;
    
     closeWrite();
     log<<MSG::INFO<<"successfully create RootFile"<<endreq;
     return sc;

}
