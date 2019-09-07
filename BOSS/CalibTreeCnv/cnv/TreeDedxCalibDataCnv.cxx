#include "GaudiKernel/MsgStream.h"
#include "TreeDedxCalibDataCnv.h"
#include "CalibData/Dedx/DedxCalibData.h"
#include "CalibDataSvc/IInstrumentName.h"
#include "CalibMySQLCnv/TreeAddress.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TObject.h"
#include "TBuffer.h"
#include "TBufferFile.h"

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
//static CnvFactory<TreeDedxCalibDataCnv> DedxCal_factory;
//const  ICnvFactory& TreeDedxCalibDataCnvFactory = DedxCal_factory;



TreeDedxCalibDataCnv::TreeDedxCalibDataCnv( ISvcLocator* svc) :
  TreeCalBaseCnv(svc, CLID_Calib_DedxCal) { 

}


const CLID& TreeDedxCalibDataCnv::objType() const {
  return CLID_Calib_DedxCal;
}

const CLID& TreeDedxCalibDataCnv::classID() {
  return CLID_Calib_DedxCal;
}

StatusCode TreeDedxCalibDataCnv::i_createObj(IOpaqueAddress* addr,
                                       DataObject*& refpObject) {

  MsgStream log(msgSvc(), "TreeDedxCalibDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;

  CalibData::DedxCalibData *tmpObject = new CalibData::DedxCalibData;
 TreeAddress* add = dynamic_cast<TreeAddress*>(addr);
  DatabaseRecord *records=add->pp();

 /*TBuffer *buf1 = new TBuffer(TBuffer::kRead);
 TBuffer *buf2 = new TBuffer(TBuffer::kRead);
 TBuffer *buf3 = new TBuffer(TBuffer::kRead);
 TBuffer *buf4 = new TBuffer(TBuffer::kRead);
 TBuffer *buf5 = new TBuffer(TBuffer::kRead);
 TBuffer *buf6 = new TBuffer(TBuffer::kRead);
 TBuffer *buf7 = new TBuffer(TBuffer::kRead);
 TBuffer *buf8 = new TBuffer(TBuffer::kRead);
 TBuffer *buf9 = new TBuffer(TBuffer::kRead);*/
 
TBufferFile *buf1 = new TBufferFile(TBuffer::kRead);
 TBufferFile *buf2 = new TBufferFile(TBuffer::kRead);
 TBufferFile *buf3 = new TBufferFile(TBuffer::kRead);
 TBufferFile *buf4 = new TBufferFile(TBuffer::kRead);
 TBufferFile *buf5 = new TBufferFile(TBuffer::kRead);
 TBufferFile *buf6 = new TBufferFile(TBuffer::kRead);
 TBufferFile *buf7 = new TBufferFile(TBuffer::kRead);
 TBufferFile *buf8 = new TBufferFile(TBuffer::kRead);
 TBufferFile *buf9 = new TBufferFile(TBuffer::kRead);

 buf1->SetBuffer((*records)["DriftDist"],512000,kFALSE);
 buf2->SetBuffer((*records)["EntranceAng"],512000,kFALSE);
 buf3->SetBuffer((*records)["MeanGain"],512000,kFALSE);
 buf4->SetBuffer((*records)["GasGain"],512000,kFALSE);
 buf5->SetBuffer((*records)["LayerGain"],512000,kFALSE);
 buf6->SetBuffer((*records)["Resolution"],512000,kFALSE);
 buf7->SetBuffer((*records)["WireGain"],512000,kFALSE);
 buf8->SetBuffer((*records)["ZDep"],512000,kFALSE);
 buf9->SetBuffer((*records)["RunGain"],512000,kFALSE);



 std::cout<<" SftVer is "<<(*records)["SftVer"];
 std::cout<<" CalVerSft is "<<(*records)["CalParVer"];
 std::cout<<"  File name  is "<<(*records)["FileName"]<<std::endl;



  TTree* ddgtree = new TTree(); 
  ddgtree->Streamer(*buf1); 
 
  TTree* entratree = new TTree(); 
  entratree->Streamer(*buf2);

  TTree* gaintree= new TTree(); 
  gaintree->Streamer(*buf3); 

  TTree* ggstree = new TTree(); 
  ggstree->Streamer(*buf4); 

  TTree* layergtree = new TTree(); 
  layergtree->Streamer(*buf5); 

  TTree* resoltree = new TTree(); 
  resoltree->Streamer(*buf6); 

  TTree* wiregtree = new TTree(); 
  wiregtree->Streamer(*buf7); 

  TTree* zdeptree = new TTree(); 
  zdeptree->Streamer(*buf8); 

  TTree *rungtree = new TTree();
  rungtree->Streamer(*buf9);

  double rungain;
  double runmean;
  int runno;
  double runresol;

  rungtree -> SetBranchAddress("rungain", &rungain);
  rungtree -> SetBranchAddress("runmean", &runmean);
  rungtree -> SetBranchAddress("runno", &runno);
  rungtree -> SetBranchAddress("runresol", &runresol);
  int N = rungtree -> GetEntries();
  tmpObject -> setrunNO(N);
  for(int i=0; i<N; i++){
       rungtree -> GetEntry(i);
       tmpObject -> setrung(rungain,0,i);
       tmpObject -> setrung(runmean,1,i);
       tmpObject -> setrung(runno,2,i);
       tmpObject -> setrung(runresol,3,i);
          }

    
     double ddg0[43];
     double ddg1[43];
     double ddg2[43];
     double ddg3[43];
     double id_doca[1600];
     double iner_chi[1600];
     double iner_gain[1600]; 
     double iner_hits[1600]; 
     double ip_eangle[1600]; 
     double out_chi[1600];
     double out_gain[1600];
     double out_hits[1600];

     ddgtree -> SetBranchAddress("ddg0", ddg0);
     ddgtree -> SetBranchAddress("ddg1", ddg1);
     ddgtree -> SetBranchAddress("ddg2", ddg2);
     ddgtree -> SetBranchAddress("ddg3", ddg3);
     TBranch *bbb = ddgtree->FindBranch("Id_doca");
     if(bbb){
	ddgtree -> SetBranchAddress("Id_doca", id_doca);
	ddgtree -> SetBranchAddress("Iner_chi", iner_chi);
	ddgtree -> SetBranchAddress("Iner_gain", iner_gain);
	ddgtree -> SetBranchAddress("Iner_hits", iner_hits);
	ddgtree -> SetBranchAddress("Ip_eangle", ip_eangle);
	ddgtree -> SetBranchAddress("Out_chi", out_chi);
	ddgtree -> SetBranchAddress("Out_gain", out_gain);
	ddgtree -> SetBranchAddress("Out_hits", out_hits);
     }

      ddgtree -> GetEntry(0);
        for(int i=0; i<43; i++){

      	     tmpObject -> setddg(ddg0[i],0,i);
	     tmpObject -> setddg(ddg1[i],1,i);
	     tmpObject -> setddg(ddg2[i],2,i);
	     tmpObject -> setddg(ddg3[i],3,i);
	  }
  
      for(int i=0; i<1600; i++){
	 if(!bbb){
	    id_doca[i]=0;
	    iner_chi[i]=0;
	    iner_gain[i]=0;
	    iner_hits[i]=0;
	    ip_eangle[i]=0;
	    out_chi[i]=0;
	    out_gain[i]=0;
	    out_hits[i]=0;
	 }
                tmpObject -> set_id_doca(id_doca[i],i);
                tmpObject -> set_iner_chi(iner_chi[i],i);
                tmpObject -> set_iner_gain(iner_gain[i],i);
                tmpObject -> set_iner_hits(iner_hits[i],i);
                tmpObject -> set_ip_eangle(ip_eangle[i],i);
                tmpObject -> set_out_chi(out_chi[i],i);
                tmpObject -> set_out_gain(out_gain[i],i);
                tmpObject -> set_out_hits(out_hits[i],i);
        }
 
 
     double entra0[43];
     double entra1[43];
     double entra2[43];
     double entra3[43];
     double engle[100];
     int engle_no;
     entratree -> SetBranchAddress("entra0", entra0);
     entratree -> SetBranchAddress("entra1", entra1);
     entratree -> SetBranchAddress("entra2", entra2);
     entratree -> SetBranchAddress("entra3", entra3);
     entratree -> SetBranchAddress("1denangle", engle);
     entratree -> SetBranchAddress("1denangle_entry", &engle_no);
       entratree -> GetEntry(0);
        for(int i=0; i<43; i++){
      	     tmpObject -> setenta(entra0[i],0,i);
	     tmpObject -> setenta(entra1[i],1,i);
	     tmpObject -> setenta(entra2[i],2,i);
	     tmpObject -> setenta(entra3[i],3,i);
	  }

    tmpObject -> set_enanglesize(engle_no);
      for(int i=0; i<engle_no; i++){
             tmpObject -> set_enangle(engle[i],i);
          }




// read ggscalib ------------------------------------------------------------
     double ggs0[43];
     double ggs1[43];
     double ggs2[43];
     double ggs3[43];
     double gcostheta[80];
     int hadron_entry;
     double hadron[20];
     ggstree -> SetBranchAddress("ggs0", ggs0);
     ggstree -> SetBranchAddress("ggs1", ggs1);
     ggstree -> SetBranchAddress("ggs2", ggs2);
     ggstree -> SetBranchAddress("ggs3", ggs3);
     ggstree -> SetBranchAddress("hadron", hadron);
     ggstree -> SetBranchAddress("hadronNo", &hadron_entry);
     if(bbb){
     ggstree -> SetBranchAddress("costheta", gcostheta);}
     ggstree -> GetEntry(0);
      for(int i=0; i<43;i++){
	   tmpObject -> setggs(ggs0[i],0,i);
	   tmpObject -> setggs(ggs1[i],1,i);
	   tmpObject -> setggs(ggs2[i],2,i);
           tmpObject -> setggs(ggs3[i],3,i);
	  }
      for(int i=0; i<80;i++){
	   if(!bbb) gcostheta[i]=0;
           tmpObject ->set_costheta(gcostheta[i],i);
         }
     if(hadron_entry>20){
       log<<MSG::FATAL<<"hadron entry is larger than 20, larger than designed"<<endreq;
       return StatusCode::FAILURE;
      }
     tmpObject->set_hadronNo(hadron_entry);
     for(int i=0;i<hadron_entry;i++){
          tmpObject->set_hadron(hadron[i],i);
        }

 
// read zdepcalib ------------------------------------------------------------
     double zdep0[43];
     double zdep1[43];
     double zdep2[43];
     double zdep3[43];
     zdeptree -> SetBranchAddress("zdep0", zdep0);
     zdeptree -> SetBranchAddress("zdep1", zdep1);
     zdeptree -> SetBranchAddress("zdep2", zdep2);
     zdeptree -> SetBranchAddress("zdep3", zdep3);
     zdeptree -> GetEntry(0);

     for(int i=0; i<43;i++){
	   tmpObject -> setzdep(zdep0[i],0,i);
	   tmpObject -> setzdep(zdep1[i],1,i);
	   tmpObject -> setzdep(zdep2[i],2,i);
           tmpObject -> setzdep(zdep3[i],3,i);
	  }
// read gain ------------------------------------------------------------
     double gain;
     double gt0[35],gdedx[35];
     gaintree -> SetBranchAddress("gain", &gain);
     if(bbb){
     gaintree -> SetBranchAddress("t0",   gt0);
     gaintree -> SetBranchAddress("dedx", gdedx);}
	   gaintree -> GetEntry(0);
           tmpObject -> setgain(gain);
     for(int i=0; i<35;i++){
      if(!bbb){
	 gt0[i]=0;
	 gdedx[i]=0;
      }
      tmpObject->set_t0(gt0[i],i);
      tmpObject->set_dedx(gdedx[i],i);
      }

// read resol----------------------------------------------------------------
     double resol;
     resoltree -> SetBranchAddress("resol", &resol);
	   resoltree -> GetEntry(0);
           tmpObject -> setresol(resol);
    
// read wireg----------------------------------------------------------------
     double wireg[6796];
     wiregtree -> SetBranchAddress("wireg",wireg);
    wiregtree -> GetEntry(0);
     for(int i=0;i<6796;i++){
           if(wireg[i]>5||wireg[i]<0.2) wireg[i]=0;
           tmpObject -> setwireg(wireg[i],i);
     }
 
// read layerg----------------------------------------------------------------
     double layerg[43];
     layergtree -> SetBranchAddress("layerg",layerg);
    layergtree -> GetEntry(0);

     for(int i=0;i<43;i++){
            tmpObject -> setlayerg(layerg[i],i);
     }



    // Read in the object

  
      refpObject=tmpObject;

   delete ddgtree;
  delete entratree;
  delete gaintree;
  delete ggstree;
  delete layergtree;
  delete resoltree;
  delete wiregtree;
  delete zdeptree;
  delete rungtree;

  return StatusCode::SUCCESS;


}

