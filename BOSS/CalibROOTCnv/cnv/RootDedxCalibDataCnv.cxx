// $Header: /bes/bes/BossCvs/Calibration/CalibSvc/CalibROOTCnv/src/cnv/RootDedxCalibDataCnv.cxx,v 1.12 2010/09/19 00:58:47 huangb Exp $
#include "GaudiKernel/MsgStream.h"
#include "RootDedxCalibDataCnv.h"
#include "CalibData/Dedx/DedxCalibData.h"
#include "CalibDataSvc/IInstrumentName.h"

#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TObject.h"
#include <iostream>
#include <fstream>
#include <string>

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/GenericAddress.h"

#include "CalibDataSvc/ICalibRootSvc.h" 
#include "CalibDataSvc/ICalibMetaCnvSvc.h"
#include "CalibData/CalibBase1.h"

// Temporary.  Hope to find a better way to do this
#include "CalibData/CalibModel.h"
using namespace CalibData;
//static CnvFactory<RootDedxCalibDataCnv> DedxCal_factory;
//const  ICnvFactory& RootDedxCalibDataCnvFactory = DedxCal_factory;



RootDedxCalibDataCnv::RootDedxCalibDataCnv( ISvcLocator* svc) :
  RootCalBaseCnv(svc, CLID_Calib_DedxCal) { 
}


const CLID& RootDedxCalibDataCnv::objType() const {
  return CLID_Calib_DedxCal;
}

const CLID& RootDedxCalibDataCnv::classID() {
  return CLID_Calib_DedxCal;
}

StatusCode RootDedxCalibDataCnv::i_createObj(const std::string& fname,
                                       DataObject*& refpObject) {

  MsgStream log(msgSvc(), "RootDedxCalibDataCnv");
  log<<MSG::DEBUG<<"SetProperty"<<endreq;
   StatusCode sc = openRead(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }

CalibData::DedxCalibData *tmpObject = new CalibData::DedxCalibData;
  // Read in our object
  int i;
  // read rungcalib ------------------------------------------------------------
  double rungain;
  double runmean;
  int runno;
  double runresol;
  TTree *rungtree = (TTree*)m_inFile -> Get("runcalib");
  rungtree -> SetBranchAddress("rungain", &rungain);
  rungtree -> SetBranchAddress("runmean", &runmean);
  rungtree -> SetBranchAddress("runno", &runno);
  rungtree -> SetBranchAddress("runresol", &runresol);
  int N = rungtree -> GetEntries();
  tmpObject -> setrunNO(N);
  //std::cout<<"N = "<<N<<std::endl;
  for(i=0; i<N; i++){
       rungtree -> GetEntry(i);
       //std::cout<<rungain<<"     "<<runmean<<"      "<<runno<<"       "<<runresol<<std::endl;
       tmpObject -> setrung(rungain,0,i);
       tmpObject -> setrung(runmean,1,i);
       tmpObject -> setrung(runno,2,i);
       tmpObject -> setrung(runresol,3,i);
          }



   // read ddgcalib ------------------------------------------------------------
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

     TTree *ddgtree = (TTree*)m_inFile -> Get("ddgcalib");
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
        for(i=0; i<43; i++){
      	     tmpObject -> setddg(ddg0[i],0,i);
	     tmpObject -> setddg(ddg1[i],1,i);
	     tmpObject -> setddg(ddg2[i],2,i);
	     tmpObject -> setddg(ddg3[i],3,i);
	  }

	for(i=0; i<1600; i++){
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

// read entratree---------------------------------------------
     double entra0[43];
     double entra1[43];
     double entra2[43];
     double entra3[43];
     double engle[100];
     int engle_no;
     TTree *entratree = (TTree*)m_inFile -> Get("entracalib");
     entratree -> SetBranchAddress("entra0", entra0);
     entratree -> SetBranchAddress("entra1", entra1);
     entratree -> SetBranchAddress("entra2", entra2);
     entratree -> SetBranchAddress("entra3", entra3);
     entratree -> SetBranchAddress("1denangle", engle);
     entratree -> SetBranchAddress("1denangle_entry", &engle_no);
       entratree -> GetEntry(0);
        for(i=0; i<43; i++){
      	     tmpObject -> setenta(entra0[i],0,i);
	     tmpObject -> setenta(entra1[i],1,i);
	     tmpObject -> setenta(entra2[i],2,i);
	     tmpObject -> setenta(entra3[i],3,i);
	  }
    tmpObject -> set_enanglesize(engle_no);
      for(i=0; i<engle_no; i++){
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
     TTree *ggstree = (TTree*)m_inFile -> Get("ggscalib");
     ggstree -> SetBranchAddress("ggs0", ggs0);
     ggstree -> SetBranchAddress("ggs1", ggs1);
     ggstree -> SetBranchAddress("ggs2", ggs2);
     ggstree -> SetBranchAddress("ggs3", ggs3);
     ggstree -> SetBranchAddress("hadron", hadron);
     ggstree -> SetBranchAddress("hadronNo", &hadron_entry);
     if(bbb){
     ggstree -> SetBranchAddress("costheta", gcostheta);
     }
     ggstree -> GetEntry(0);
      for(i=0; i<43;i++){
	   tmpObject -> setggs(ggs0[i],0,i);
	   tmpObject -> setggs(ggs1[i],1,i);
	   tmpObject -> setggs(ggs2[i],2,i);
           tmpObject -> setggs(ggs3[i],3,i);
	  }
     
      for(i=0; i<80;i++){
	    if(!bbb) gcostheta[i]=0;
           tmpObject ->set_costheta(gcostheta[i],i);
         }
    
     if(hadron_entry>20){ 
       log<<MSG::FATAL<<"hadron entry is larger than 20, larger than designed"<<endreq;
       return StatusCode::FAILURE;
      }

     tmpObject->set_hadronNo(hadron_entry);

     for(i=0;i<hadron_entry;i++){
          tmpObject->set_hadron(hadron[i],i);     
        }    
  

// read zdepcalib ------------------------------------------------------------
     double zdep0[43];
     double zdep1[43];
     double zdep2[43];
     double zdep3[43];
     TTree *zdeptree = (TTree*)m_inFile -> Get("zdepcalib");
     zdeptree -> SetBranchAddress("zdep0", zdep0);
     zdeptree -> SetBranchAddress("zdep1", zdep1);
     zdeptree -> SetBranchAddress("zdep2", zdep2);
     zdeptree -> SetBranchAddress("zdep3", zdep3);
     zdeptree -> GetEntry(0);

     for(i=0; i<43;i++){
	   tmpObject -> setzdep(zdep0[i],0,i);
	   tmpObject -> setzdep(zdep1[i],1,i);
	   tmpObject -> setzdep(zdep2[i],2,i);
           tmpObject -> setzdep(zdep3[i],3,i);
	  }
      
// read gain ------------------------------------------------------------
     double gain;
     double gt0[35],gdedx[35];
     TTree *gaintree = (TTree*)m_inFile -> Get("gaincalib");
     gaintree -> SetBranchAddress("gain", &gain);
     if(bbb){
     gaintree -> SetBranchAddress("t0",   gt0);
     gaintree -> SetBranchAddress("dedx", gdedx);
     }
	   gaintree -> GetEntry(0);
           tmpObject -> setgain(gain);
     for(i=0; i<35;i++){
      if(!bbb){
	  gt0[i]=0;
	  gdedx[i]=0;
       }
      tmpObject->set_t0(gt0[i],i);
      tmpObject->set_dedx(gdedx[i],i);
      }
	   
// read resol----------------------------------------------------------------
     double resol;
     TTree *resoltree = (TTree*)m_inFile -> Get("resolcalib");
     resoltree -> SetBranchAddress("resol", &resol);
	   resoltree -> GetEntry(0);
           tmpObject -> setresol(resol);
    
// read wireg----------------------------------------------------------------
     double wireg[6796];
     TTree *wiregtree = (TTree*)m_inFile -> Get("wiregcalib");
     wiregtree -> SetBranchAddress("wireg",wireg);
    wiregtree -> GetEntry(0);
     for(i=0;i<6796;i++){
           tmpObject -> setwireg(wireg[i],i);
     }
  
// read layerg----------------------------------------------------------------
     double layerg[43];
     TTree *layergtree = (TTree*)m_inFile -> Get("layergcalib");
     layergtree -> SetBranchAddress("layerg",layerg);
    layergtree -> GetEntry(0);

     for(i=0;i<43;i++){
            tmpObject -> setlayerg(layerg[i],i);
     }
   
    refpObject=tmpObject;
  //  delete pObj;
  // std::cout<<"pObj.m_t0[Cell]="<<*(pObj->gett0()+Cell-1);
  // dynamic_cast<CalibData::Mdct0& >(refpObject);
  //refpObject = pObj;
  // std::cout<<"refObject.m_t0[Cell]="<<*(refpObject->gett0()+Cell-1);
  // refpObject = new  Mdct0(&t0_tmp[0]);
  return StatusCode::SUCCESS;
}

StatusCode RootDedxCalibDataCnv::createRoot(const std::string& fname, 
                                        CalibData::CalibBase1* pTDSObj) {
 
  MsgStream log(msgSvc(), "RootDedxCalibDataCnv");

 // Open the file, create the branch
   StatusCode sc = openWrite(fname);
  if(!sc)
    { log<<MSG::ERROR<<"unable to open files"<<endreq;
    }
  // write the Data in the TCDS to RootFile
     int i;
     CalibData::DedxCalibData* tmpObject = dynamic_cast<CalibData::DedxCalibData*>(pTDSObj);
  // write rungcalib ------------------------------------------------------------
     double rungain;
     double runmean;
     double runno;
     double runresol;
     TTree *rungcalibtree = new TTree("rungcalib", "rungCalib");
     rungcalibtree -> Branch("rungain", &rungain, "S/D");
     rungcalibtree -> Branch("runmean", &runmean, "S/D");
     rungcalibtree -> Branch("runno", &runno, "S/D");
     rungcalibtree -> Branch("runresol", &runresol, "S/D");
     int N = tmpObject->getrunNO();
     for(i=0; i<N; i++){
	     rungain = tmpObject->getrung(0,i);
	     runmean = tmpObject->getrung(1,i);
	     runno = tmpObject->getrung(2,i);
	     runresol = tmpObject->getrung(3,i);
	     rungcalibtree -> Fill();
     } 

 

 // write  ddgcalib----------------------------------------------------------------
     double ddg0;
     double ddg1;
     double ddg2;
     double ddg3;
     TTree *ddgcalibtree = new TTree("ddgcalib", "ddgCalib");
     ddgcalibtree -> Branch("ddg0", &ddg0, "S/D");
     ddgcalibtree -> Branch("ddg1", &ddg1, "S/D");
     ddgcalibtree -> Branch("ddg2", &ddg2, "S/D");
     ddgcalibtree -> Branch("ddg3", &ddg3, "S/D");
     for(i=0; i<43; i++){
	       ddg0 = tmpObject->getddg(0,i);
               ddg1 = tmpObject->getddg(1,i);
               ddg2 = tmpObject->getddg(2,i);
               ddg3 = tmpObject->getddg(3,i);
                    ddgcalibtree -> Fill();
     }
 
  // write  ggscalib----------------------------------------------------------------
     double ggs0;
     double ggs1;
     double ggs2;
     double ggs3;
     TTree *ggscalibtree = new TTree("ggscalib", "ggsCalib");
     ggscalibtree -> Branch("ggs0", &ggs0, "S/D");
     ggscalibtree -> Branch("ggs1", &ggs1, "S/D");
     ggscalibtree -> Branch("ggs2", &ggs2, "S/D");
     ggscalibtree -> Branch("ggs3", &ggs3, "S/D");
     for(i=0; i<43; i++){
	       ggs0 = tmpObject->getggs(0,i);
               ggs1 = tmpObject->getggs(1,i);
               ggs2 = tmpObject->getggs(2,i);
               ggs3 = tmpObject->getggs(3,i);
                    ggscalibtree -> Fill();
     }

// write  zdepcalib----------------------------------------------------------------
     double zdep0;
     double zdep1;
     double zdep2;
     double zdep3;
     TTree *zdepcalibtree = new TTree("zdepcalib", "zdepCalib");
     zdepcalibtree -> Branch("zdep0", &zdep0, "S/D");
     zdepcalibtree -> Branch("zdep1", &zdep1, "S/D");
     zdepcalibtree -> Branch("zdep2", &zdep2, "S/D");
     zdepcalibtree -> Branch("zdep3", &zdep3, "S/D");
     for(i=0; i<43; i++){
	       zdep0 = tmpObject->getzdep(0,i);
               zdep1 = tmpObject->getzdep(1,i);
               zdep2 = tmpObject->getzdep(2,i);
               zdep3 = tmpObject->getzdep(3,i);
                    zdepcalibtree -> Fill();
     }

// write  entacalib----------------------------------------------------------------
     double enta0;
     double enta1;
     double enta2;
     double enta3;
     TTree *entacalibtree = new TTree("entacalib", "entaCalib");
     entacalibtree -> Branch("enta0", &enta0, "S/D");
     entacalibtree -> Branch("enta1", &enta1, "S/D");
     entacalibtree -> Branch("enta2", &enta2, "S/D");
     entacalibtree -> Branch("enta3", &enta3, "S/D");
     for(i=0; i<43; i++){
	       enta0 = tmpObject->getenta(0,i);
               enta1 = tmpObject->getenta(1,i);
               enta2 = tmpObject->getenta(2,i);
               enta3 = tmpObject->getenta(3,i);
                    entacalibtree -> Fill();
     }



// write  gain---------------------------------------------------------------------
     double gain;
     TTree *gaintree = new TTree("gaincalib", "gainCalib");
     gaintree -> Branch("gain", &gain, "S/D");
	       gain = tmpObject->getgain();
               gaintree -> Fill();

// write  resol---------------------------------------------------------------------
     double resol;
     TTree *resoltree = new TTree("resolcalib", "resolCalib");
     resoltree -> Branch("resol", &resol, "S/D");
	       resol = tmpObject->getresol();
               resoltree -> Fill();


// write  wiregcalib----------------------------------------------------------------
     double wireg;
     TTree *wiregcalibtree = new TTree("wiregcalib", "wiregCalib");
     wiregcalibtree -> Branch("wireg", &wireg, "S/D");
     for(i=0; i<6796; i++){
	       wireg = tmpObject->getwireg(i);
               wiregcalibtree -> Fill();
     }

// write  layergcalib----------------------------------------------------------------
     double layerg;
     TTree *layergcalibtree = new TTree("layergcalib", "layergCalib");
     layergcalibtree -> Branch("layerg", &layerg, "S/D");
     for(i=0; i<43; i++){
	       layerg = tmpObject->getlayerg(i);
               layergcalibtree -> Fill();
     }

//  write all the trees
     rungcalibtree -> Write();
     ddgcalibtree -> Write();
     ggscalibtree -> Write();
     zdepcalibtree -> Write();
     entacalibtree -> Write();
     gaintree -> Write();
     resoltree -> Write();
     wiregcalibtree -> Write();
     layergcalibtree -> Write();
     
     delete rungcalibtree;
     delete ddgcalibtree;
     delete ggscalibtree;
     delete zdepcalibtree;
     delete entacalibtree;
     delete gaintree;
     delete resoltree;
     delete wiregcalibtree;
     delete layergcalibtree;
     closeWrite();
     log<<MSG::INFO<<"successfully create RootFile"<<endreq;
     return sc;

}
