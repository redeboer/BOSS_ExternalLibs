#include "GaudiKernel/MsgStream.h"
#include "RootEsTimeCalibDataCnv.h"
#include "CalibData/EsTime/EsTimeCalibData.h"
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
//static CnvFactory<RootEsTimeCalibDataCnv> EsTimeCal_factory;
//const  ICnvFactory& RootEsTimeCalibDataCnvFactory = EsTimeCal_factory;



RootEsTimeCalibDataCnv::RootEsTimeCalibDataCnv( ISvcLocator* svc) :
  RootCalBaseCnv(svc, CLID_Calib_EsTimeCal) { 

}


const CLID& RootEsTimeCalibDataCnv::objType() const {
  return CLID_Calib_EsTimeCal;
}

const CLID& RootEsTimeCalibDataCnv::classID() {
  return CLID_Calib_EsTimeCal;
}

StatusCode RootEsTimeCalibDataCnv::i_createObj(const std::string& fname,
                                       DataObject*& refpObject) {

  MsgStream log(msgSvc(), "RootEsTimeCalibDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;
 
  StatusCode sc = openRead(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
 
  CalibData::EsTimeCalibData *tmpObject = new CalibData::EsTimeCalibData ;
  // Read in our object
  int i;
  int nentries;
  
  // read TestCalibConst ------------------------------------------------------------
     double EsTimeCalibConst;
     vector<double> TestCalibConst;
     TTree *Testtree = (TTree*)m_inFile -> Get("h2");
     Testtree -> SetBranchAddress("meant0", &EsTimeCalibConst);
     int no= Testtree -> GetEntries();
     for(i=0; i<no; i++){
          Testtree->GetEntry(i);
           TestCalibConst.push_back(EsTimeCalibConst);
     }
   
    double t0offsete,t0offsetb;
    int bunchtime;
    TTree *tree2 = (TTree*)m_inFile -> Get("calibconst");
    tree2->SetBranchAddress("t0offsete", &t0offsete);
     tree2->SetBranchAddress("t0offsetb", &t0offsetb);
    tree2->SetBranchAddress("bunchtime", &bunchtime);
    tree2->GetEntry(0);
    tmpObject ->setToffsetb(t0offsetb);
    tmpObject ->setToffsete(t0offsete);
    tmpObject ->setBunchTime(bunchtime);

     tmpObject -> setTestCalibConst(&TestCalibConst);

    refpObject=tmpObject;

  return StatusCode::SUCCESS;
}

StatusCode RootEsTimeCalibDataCnv::createRoot(const std::string& fname, 
                                        CalibData::CalibBase1* pTDSObj) {
 
  MsgStream log(msgSvc(), "RootEsTimeCalibDataCnv");

 // Open the file, create the branch
   StatusCode sc = openWrite(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
  // write the Data in the TCDS to RootFile
 /*    CalibData::EsTimeCalibData* tmpObject = dynamic_cast<CalibData::EsTimeCalibData*>(pTDSObj);
     int tmpNo;
     double EsTimeCalibConst;
     int i;

     //TestCalibConst------------------------------------------------------------------
     TTree *Testtree = new TTree("TestCalibConst", "TestCalibConst");
     Testtree -> Branch("TestCalibConst", &EsTimeCalibConst, "EsTimeCalibConst/D");
     tmpNo = tmpObject -> getTestCalibConstNo();
     for(i=0; i<tmpNo; i++){
      	  EsTimeCalibConst = tmpObject -> getTestCalibConst(i);
	  Testtree -> Fill();
     }

     
     Testtree -> Write();

   
     delete Testtree;
    
     closeWrite();*/
     log<<MSG::INFO<<"successfully create RootFile"<<endreq;
     return sc;

}
