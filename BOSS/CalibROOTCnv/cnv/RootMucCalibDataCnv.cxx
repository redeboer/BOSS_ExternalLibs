// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/cnv/RootMucCalibDataCnv.cxx,v 1.4 2008/10/22 06:33:46 huangb Exp $
#include <iostream>

#include "GaudiKernel/MsgStream.h"
#include "RootMucCalibDataCnv.h"
#include "CalibData/Muc/MucCalibData.h"
#include "CalibDataSvc/IInstrumentName.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
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
//using namespace CalibData;
using CalibData::MucCalibData;
using CalibData::MucIdTransform;

using namespace std;

//static CnvFactory<RootMucCalibDataCnv> MucCal_factory;
//const  ICnvFactory& RootMucCalibDataCnvFactory = MucCal_factory;



RootMucCalibDataCnv::RootMucCalibDataCnv( ISvcLocator* svc) :
  RootCalBaseCnv(svc, CLID_Calib_MucCal) { 
    m_ptrIdTr = new MucIdTransform();
}


const CLID& RootMucCalibDataCnv::objType() const {
  return CLID_Calib_MucCal;
}

const CLID& RootMucCalibDataCnv::classID() {
  return CLID_Calib_MucCal;
}

StatusCode RootMucCalibDataCnv::i_createObj(const std::string& fname,
                                       DataObject*& refpObject) {

  MsgStream log(msgSvc(), "RootMucCalibDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;
 
  StatusCode sc = openRead(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
      return StatusCode::FAILURE;
    }
 
  MucCalibData *tmpObject = new MucCalibData() ;
  // Read in our object
 // int i;
 // int nentries;
  
  // read DigiCalibConst ------------------------------------------------------------

	Double_t lay_eff, box_eff, str_eff;
  Double_t lay_cnt, box_cnt, str_cnt;
  Double_t lay_nos, box_nos, str_nos;
  Double_t lay_nos_ratio, box_nos_ratio, str_nos_ratio;
  lay_eff = box_eff = str_eff = 0.0;
  lay_cnt = box_cnt = str_cnt = 0.0;
  lay_nos = box_nos = str_nos = 0.0;
  lay_nos_ratio = box_nos_ratio = str_nos_ratio = 0.0;

	char name[60];
	
	TTree* tr_Lvl[3];
  //TTree* tr_ClstPro[2];
	
  //TTree *ddgtree = (TTree*)m_inFile -> Get("ddgcalib");
	tr_Lvl[0] = (TTree*)m_inFile->Get("LayConst");
	tr_Lvl[0]->SetBranchAddress("layer_eff", &lay_eff);
	tr_Lvl[0]->SetBranchAddress("layer_cnt", &lay_cnt);
	tr_Lvl[0]->SetBranchAddress("layer_noise", &lay_nos);
  tr_Lvl[0]->SetBranchAddress("layer_nosratio", &lay_nos_ratio);
	
	tr_Lvl[1] = (TTree*)m_inFile->Get("BoxConst");
	tr_Lvl[1]->SetBranchAddress("box_eff", &box_eff);	
	tr_Lvl[1]->SetBranchAddress("box_cnt", &box_cnt);	
	tr_Lvl[1]->SetBranchAddress("box_noise", &box_nos);	
  tr_Lvl[1]->SetBranchAddress("box_nosratio", &box_nos_ratio);
  
	tr_Lvl[2] = (TTree*)m_inFile->Get("StrConst");
	tr_Lvl[2]->SetBranchAddress("strip_eff", &str_eff);
	tr_Lvl[2]->SetBranchAddress("strip_cnt", &str_cnt);
	tr_Lvl[2]->SetBranchAddress("strip_noise", &str_nos);
  tr_Lvl[2]->SetBranchAddress("strip_nosratio", &str_nos_ratio);
  //tr_ClstPro[0] = (TTree*)m_inFile->Get("LayClstPro");
  //tr_ClstPro[1] = (TTree*)m_inFile->Get("BoxClstPro");
			
  int part, segment, layer, strip;
  part = segment = layer = strip = 0;
  for(int i=0; i<LAYER_MAX; i++)
  {

    tr_Lvl[0]->GetEntry(i);
    tmpObject->setLayerEff(lay_eff, i);
    tmpObject->setLayerCnt(lay_cnt, i);
    tmpObject->setLayerNos(lay_nos, i);
    tmpObject->setLayerNosRatio(lay_nos_ratio, i);

    sprintf(name,"LayClstPro");
    //tr_ClstPro[0] = (TTree*)m_inFile->Get(name);
    for(int j=0; j<CLST_MAX; j++) {
      //if( tr_ClstPro[0] != NULL ) tmpObject->setLayerClstPro(tr_ClstPro[0]->GetBinContent(j),i,j);
      //else tmpObject->setLayerClstPro(DEFAULT_CLST_PRO[j],i,j);
      tmpObject->setLayerClstPro(DEFAULT_CLST_PRO[j],i,j);
    }
    //log<<MSG::DEBUG<<"layer: " << i << "\t" << lay_eff <<endreq;		
  }
  
  for(int i=0; i<BOX_MAX; i++)
  {
    m_ptrIdTr->SetBoxPos( i, &part, &segment, &layer );
    tr_Lvl[1]->GetEntry(i);		
    tmpObject->setBoxEff(box_eff, part, segment, layer);
    tmpObject->setBoxCnt(box_cnt, part, segment, layer);
    tmpObject->setBoxNos(box_nos, part, segment, layer);
    tmpObject->setBoxNosRatio(box_nos_ratio, part, segment, layer);

    sprintf(name,"BoxClstPro_B%d",i);
    //tr_ClstPro[1] = (TTree*)m_inFile->Get(name);
    for(int j=0; j<CLST_MAX; j++) {
      //if( tr_ClstPro[1] != NULL ) tmpObject->setBoxClstPro(tr_ClstPro[1]->GetBinContent(j),part,segment,layer,j);
      //else tmpObject->setBoxClstPro(DEFAULT_CLST_PRO[j],part,segment,layer,j);
      tmpObject->setBoxClstPro(DEFAULT_CLST_PRO[j],part,segment,layer,j);
    }
    //log<<MSG::DEBUG<<"box: " << i << "\t" << box_eff <<endreq;               
  }
  
  for(int i=0; i<STRIP_MAX; i++)
  {   
    m_ptrIdTr->SetStripPos( i, &part, &segment, &layer, &strip );
    tr_Lvl[2]->GetEntry(i);
    tmpObject->setStripEff(str_eff, part, segment, layer, strip);
    tmpObject->setStripCnt(str_cnt, part, segment, layer, strip);
    tmpObject->setStripNos(str_nos, part, segment, layer, strip);
    tmpObject->setStripNosRatio(str_nos_ratio, part, segment, layer, strip);
    //log<<MSG::DEBUG<<"strip: " << i << "\t" << str_eff <<endreq;
  }
  refpObject=tmpObject;

  return StatusCode::SUCCESS;
}

StatusCode RootMucCalibDataCnv::createRoot(const std::string& fname, 
                                        CalibData::CalibBase1* pTDSObj) {
 
  MsgStream log(msgSvc(), "RootMucCalibDataCnv");

 // Open the file, create the branch
/*   StatusCode sc = openWrite(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
  // write the Data in the TCDS to RootFile
     CalibData::MucCalibData* tmpObject = dynamic_cast<CalibData::MucCalibData*>(pTDSObj);
     int tmpNo;
     double MucCalibConst;
     double EnCoeff;
     double PosCoeff;
     int i;

     //DigiCalibConst------------------------------------------------------------------
     TTree *Digitree = new TTree("DigiCalibConst", "DigiCalibConst");
     Digitree -> Branch("DigiCalibConst", &MucCalibConst, "MucCalibConst/D");
     tmpNo = tmpObject -> getDigiCalibConstNo();
     for(i=0; i<tmpNo; i++){
      	  MucCalibConst = tmpObject -> getDigiCalibConst(i);
	  Digitree -> Fill();
     }

       
     //EnCoeff-------------------------------------------------------------------------
     TTree *Entree = new TTree("EnCoeff", "EnCoeff");
     Entree -> Branch("EnCoeff", &EnCoeff, "EnCoeff/D");
     tmpNo = tmpObject -> getEnCoeffNo();
     for(i=0; i<tmpNo; i++){
      	  EnCoeff = tmpObject -> getEnCoeff(i);
	  Entree -> Fill();
     }

    //PosCoeff-------------------------------------------------------------------------
     TTree *Postree = new TTree("PosCoeff", "PosCoeff");
     Postree -> Branch("PosCoeff", &PosCoeff, "PosCoeff/D");
     tmpNo = tmpObject -> getPosCoeffNo();
     for(i=0; i<tmpNo; i++){
      	  PosCoeff = tmpObject -> getPosCoeff(i);
	  Postree -> Fill();
     }

     
     Digitree -> Write();
     Entree -> Write();
     Postree -> Write();

   
     delete Digitree;
     delete Entree;
     delete Postree;
    
     closeWrite();
     log<<MSG::INFO<<"successfully create RootFile"<<endreq;
  */
     return StatusCode::SUCCESS;

}
