#include "GaudiKernel/MsgStream.h"
#include "RootTofQElecDataCnv.h"
#include "CalibData/Tof/TofElecData.h"
#include "CalibDataSvc/IInstrumentName.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TObject.h"
#include "TBuffer.h"

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/GenericAddress.h"

#include "CalibDataSvc/ICalibTreeSvc.h"    //maybe
#include "CalibDataSvc/ICalibMetaCnvSvc.h"

// Temporary.  Hope to find a better way to do this
#include "CalibData/CalibModel.h"
using namespace CalibData;
//static CnvFactory<RootTofElecDataCnv> TofCal_factory;
//const  ICnvFactory& RootTofElecDataCnvFactory = TofCal_factory;



RootTofElecDataCnv::RootTofElecDataCnv( ISvcLocator* svc) :
  RootCalBaseCnv(svc, CLID_TofQ_Elec) { 

}


const CLID& RootTofElecDataCnv::objType() const {
  return CLID_TofQ_Elec;
}

const CLID& RootTofElecDataCnv::classID() {
  return CLID_TofQ_Elec;
}

StatusCode RootTofElecDataCnv::i_createObj(const std::string& fname,
                                       DataObject*& refpObject) {

  MsgStream log(msgSvc(), "RootTofElecDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;
  StatusCode sc = openRead(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }

  TTree *BarBoardNum=(TTree*)m_inFile -> Get("BarBoardNum");
  TTree *EndBoardNum= (TTree*)m_inFile -> Get("EndBoardNum");
  TTree *QElecBarParEast = (TTree*)m_inFile -> Get("QElecBarParEast");
  TTree *QElecBarParWest = (TTree*)m_inFile -> Get("QElecBarParWest");
  TTree *QElecEndPar = (TTree*)m_inFile -> Get("QElecEndPar");
  TTree *SimQElecBarParEast = (TTree*)m_inFile -> Get("SimQElecBarParEast");
  TTree *SimQElecBarParWest = (TTree*)m_inFile -> Get("SimQElecBarParWest");
  TTree *SimQElecEndPar = (TTree*)m_inFile -> Get("SimQElecEndPar");

  CalibData::BarrelElec bTof;
  CalibData::EndcapElec eTof;

  std::vector<CalibData::BarrelElec> tmpbTof;//; = new vector<CalibData::bTofCalibBase>;
  std::vector<CalibData::EndcapElec> tmpeTof;

    // Read in the object
  int cnt;
  // read btoftree ------------------------------------------------------------
  unsigned int num[4];
  double num1[4];
     BarBoardNum -> SetBranchAddress("Board", &num1[0]);
     BarBoardNum -> SetBranchAddress("Channel", &num1[1]);
     BarBoardNum -> SetBranchAddress("Crate", &num1[2]);
     BarBoardNum -> SetBranchAddress("Fee", &num1[3]);
  double p[11];
     QElecBarParEast->SetBranchAddress("P0",&p[0]);
     QElecBarParEast->SetBranchAddress("P1",&p[1]);
     QElecBarParEast->SetBranchAddress("P2",&p[2]);
     QElecBarParEast->SetBranchAddress("P3",&p[3]);
     QElecBarParEast->SetBranchAddress("P4",&p[4]);
     QElecBarParEast->SetBranchAddress("P5",&p[5]);
     QElecBarParEast->SetBranchAddress("P6",&p[6]);
     QElecBarParEast->SetBranchAddress("P7",&p[7]);
     QElecBarParEast->SetBranchAddress("P8",&p[8]);
     QElecBarParEast->SetBranchAddress("P9",&p[9]);
     QElecBarParEast->SetBranchAddress("P10",&p[10]);
   double p_w[11];
     QElecBarParWest->SetBranchAddress("P0",&p_w[0]);
     QElecBarParWest->SetBranchAddress("P1",&p_w[1]);
     QElecBarParWest->SetBranchAddress("P2",&p_w[2]);
     QElecBarParWest->SetBranchAddress("P3",&p_w[3]);
     QElecBarParWest->SetBranchAddress("P4",&p_w[4]);
     QElecBarParWest->SetBranchAddress("P5",&p_w[5]);
     QElecBarParWest->SetBranchAddress("P6",&p_w[6]);
     QElecBarParWest->SetBranchAddress("P7",&p_w[7]);
     QElecBarParWest->SetBranchAddress("P8",&p_w[8]);
     QElecBarParWest->SetBranchAddress("P9",&p_w[9]);
     QElecBarParWest->SetBranchAddress("P10",&p_w[10]);
      double p_se[11];
     SimQElecBarParEast->SetBranchAddress("P0",&p_se[0]);
     SimQElecBarParEast->SetBranchAddress("P1",&p_se[1]);
     SimQElecBarParEast->SetBranchAddress("P2",&p_se[2]);
     SimQElecBarParEast->SetBranchAddress("P3",&p_se[3]);
     SimQElecBarParEast->SetBranchAddress("P4",&p_se[4]);
     SimQElecBarParEast->SetBranchAddress("P5",&p_se[5]);
     SimQElecBarParEast->SetBranchAddress("P6",&p_se[6]);
     SimQElecBarParEast->SetBranchAddress("P7",&p_se[7]);
     SimQElecBarParEast->SetBranchAddress("P8",&p_se[8]);
     SimQElecBarParEast->SetBranchAddress("P9",&p_se[9]);
     SimQElecBarParEast->SetBranchAddress("P10",&p_se[10]);
       double p_sw[11];
     SimQElecBarParWest->SetBranchAddress("P0",&p_sw[0]);
     SimQElecBarParWest->SetBranchAddress("P1",&p_sw[1]);
     SimQElecBarParWest->SetBranchAddress("P2",&p_sw[2]);
     SimQElecBarParWest->SetBranchAddress("P3",&p_sw[3]);
     SimQElecBarParWest->SetBranchAddress("P4",&p_sw[4]);
     SimQElecBarParWest->SetBranchAddress("P5",&p_sw[5]);
     SimQElecBarParWest->SetBranchAddress("P6",&p_sw[6]);
     SimQElecBarParWest->SetBranchAddress("P7",&p_sw[7]);
     SimQElecBarParWest->SetBranchAddress("P8",&p_sw[8]);
     SimQElecBarParWest->SetBranchAddress("P9",&p_sw[9]);
     SimQElecBarParWest->SetBranchAddress("P10",&p_sw[10]);

     for(cnt=0; cnt<352; cnt++){
         BarBoardNum -> GetEntry(cnt);
          num[0]=num1[0];
           num[1]=num1[1];
          num[2]=num1[2];
           num[3]=num1[3];
         if(cnt%2!=0){
          bTof.setNumWest(num);
          }
         if(cnt%2==0){
          bTof.setNumEast(num);
         int kkk=cnt/2;
         QElecBarParEast->GetEntry(kkk);
         QElecBarParWest->GetEntry(kkk);
         SimQElecBarParEast->GetEntry(kkk);
         SimQElecBarParWest->GetEntry(kkk);
           bTof.setP1(p);
           bTof.setP2(p_w);
           bTof.setSimP1(p_se);
           bTof.setSimP2(p_sw);
          }           
         if(cnt%2!=0)
           tmpbTof.push_back(bTof);
	  }

       //read etoftree
       EndBoardNum -> SetBranchAddress("Board", &num1[0]);
       EndBoardNum -> SetBranchAddress("Channel", &num1[1]);
       EndBoardNum -> SetBranchAddress("Crate", &num1[2]);
       EndBoardNum -> SetBranchAddress("Fee", &num1[3]);
       QElecEndPar-> SetBranchAddress("P0",&p[0]);
       QElecEndPar->SetBranchAddress("P1",&p[1]);
       QElecEndPar->SetBranchAddress("P2",&p[2]);
       QElecEndPar->SetBranchAddress("P3",&p[3]);
       QElecEndPar->SetBranchAddress("P4",&p[4]);
       QElecEndPar->SetBranchAddress("P5",&p[5]);
       QElecEndPar->SetBranchAddress("P6",&p[6]);
       QElecEndPar->SetBranchAddress("P7",&p[7]);
       QElecEndPar->SetBranchAddress("P8",&p[8]);
       QElecEndPar->SetBranchAddress("P9",&p[9]);
       QElecEndPar->SetBranchAddress("P10",&p[10]);
       SimQElecEndPar-> SetBranchAddress("P0",&p_w[0]);
       SimQElecEndPar->SetBranchAddress("P1",&p_w[1]);
       SimQElecEndPar->SetBranchAddress("P2",&p_w[2]);
       SimQElecEndPar->SetBranchAddress("P3",&p_w[3]);
       SimQElecEndPar->SetBranchAddress("P4",&p_w[4]);
       SimQElecEndPar->SetBranchAddress("P5",&p_w[5]);
       SimQElecEndPar->SetBranchAddress("P6",&p_w[6]);
       SimQElecEndPar->SetBranchAddress("P7",&p_w[7]);
       SimQElecEndPar->SetBranchAddress("P8",&p_w[8]);
       SimQElecEndPar->SetBranchAddress("P9",&p_w[9]);
       SimQElecEndPar->SetBranchAddress("P10",&p_w[10]);
        for(cnt=0; cnt<96; cnt++){
        EndBoardNum->GetEntry(cnt);
        QElecEndPar->GetEntry(cnt);
        SimQElecEndPar->GetEntry(cnt);
        num[0]=num1[0];
           num[1]=num1[1];
          num[2]=num1[2];
           num[3]=num1[3];
        eTof.setNum(num);
        eTof.setP(p);
        eTof.setSimP(p_w);
        tmpeTof.push_back(eTof);
   }

   CalibData::TofElecData *tmpObject = new CalibData::TofElecData(&tmpbTof,&tmpeTof);

  
      refpObject=tmpObject;
  delete BarBoardNum;
  delete EndBoardNum;
  delete QElecBarParEast;
  delete QElecBarParWest; 
  delete QElecEndPar; 
  delete SimQElecBarParEast;
  delete SimQElecBarParWest; 
  delete SimQElecEndPar; 

  return StatusCode::SUCCESS;


}

