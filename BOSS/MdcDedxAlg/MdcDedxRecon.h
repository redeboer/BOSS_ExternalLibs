//    BesIII MDC dE/dx Reconstruction Module
//    Class: MdcDedxRecon
//    Created by Dayong WANG 2003/11

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "GaudiKernel/NTuple.h"
#include "DedxCorrecSvc/DedxCorrecSvc.h"
#include "DedxCurSvc/DedxCurSvc.h"
#include "MdcGeomSvc/MdcGeomSvc.h"

#include "MdcDedxAlg/MdcDedxTrk.h"
#include "MdcDedxAlg/MdcDedxCorrection.h"

#include "TFile.h"
#include "TObjArray.h"
#include "TFolder.h"
#include "TH1F.h"

class DedxCorrection;
class MdcDedxTrk;

class MdcDedxRecon:public Algorithm  {

    public:
        MdcDedxRecon (const std::string& name, ISvcLocator* pSvcLocator ); 
        ~MdcDedxRecon() {};

        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();
        StatusCode beginRun(); 

        const std::vector<MdcDedxTrk>& tracks(void) const; 
        void clearTables(); 

        void mdctrackrec(RecMdcTrackCol::iterator trk, Identifier mdcid, double tes, int RunNO,int runFlag, MsgStream log);
        void kaltrackrec(RecMdcKalTrackCol::iterator trk, Identifier mdcid, double tes, int RunNO,int runFlag, MsgStream log);
        void switchtomdctrack(int trkid, Identifier mdcid, double tes, int RunNO,int runFlag, MsgStream log);


    private:
        int   calib_flag; 
        int   landau;
        int   ntpFlag;
        int   doNewGlobal;
        int   recalg;  //reconstruction method: 
        //0:RecMdcTrack; 1:RecMdcKalTrack; 2:RecMdcTrack when no RecMdcKalTrack 
        int ParticleFlag;//particle type flag: 0: electron, 1:muon, 2: pion, 3:kaon, 4:proton
        int m_alg;  // algorithm 1: bi-truncation; 2: transformation; 3: logorithm mean;
        float truncate; // truncation rate
        std::string m_rootfile;   

        IDedxCurSvc* dedxcursvc;      
        IMdcGeomSvc* geosvc;
        IDedxCorrecSvc* exsvc;
        MdcDedxCorrection * ex_calib;
        std::vector<MdcDedxTrk> ex_trks;
        std::vector<int> m_trkalgs;
        int m_trkalg;

        RecMdcDedxCol* dedxList;
        RecMdcDedx*  resdedx;
        RecMdcDedxHitCol* dedxhitList;
        RecMdcDedxHit* dedxhit;
        DedxHitRefVec* dedxhitrefvec;

        //root file
        TObjArray* m_hlist;
        TFolder* m_hitlevel;
        TH1F* m_hitNo_wire;

        //Ntuple file
        NTuple::Tuple* m_nt1;
        NTuple::Item<float>  m_phtm;
        //NTuple::Item<float>  m_median; 
        //NTuple::Item<float>  m_geometric;
        //NTuple::Item<float>  m_geometric_trunc;
        //NTuple::Item<float>  m_harmonic;
        //NTuple::Item<float>  m_harmonic_trunc;
        //NTuple::Item<float>  m_transform;
        //NTuple::Item<float>  m_logtrunc;   
        NTuple::Item<float>  m_dEdx_meas;

        NTuple::Item<float>  m_ptrk;
        NTuple::Item<float>  m_costheta;
        NTuple::Item<float>  m_sintheta;
        NTuple::Item<float>  m_charge;
        NTuple::Item<float>  m_runNO;                                   
        NTuple::Item<float>  m_evtNO;
        NTuple::Item<float>  m_t0;
        NTuple::Item<float>  m_trackId;
        NTuple::Item<float>  m_poca_x;
        NTuple::Item<float>  m_poca_y;
        NTuple::Item<float>  m_poca_z;
        NTuple::Item<float>  m_recalg;

        NTuple::Item<float>  m_nhit;
        NTuple::Item<float>  m_usedhit;
        NTuple::Item<long>   m_nphlisthit;
        NTuple::Array<double> m_dEdx_hit;

        NTuple::Array<float>  m_probpid;
        NTuple::Array<float>  m_expectid;
        NTuple::Array<float>  m_sigmaid;
        NTuple::Item<float>  m_parttype;
        NTuple::Item<float>  m_prob;
        NTuple::Item<float>  m_expect;
        NTuple::Item<float>  m_sigma;
        NTuple::Item<float>  m_chidedx;
        NTuple::Item<float>  m_chidedxe;
        NTuple::Item<float>  m_chidedxmu;
        NTuple::Item<float>  m_chidedxpi;
        NTuple::Item<float>  m_chidedxk;
        NTuple::Item<float>  m_chidedxp;


        NTuple::Tuple* m_nt2;
        NTuple::Item<double>  m_charge1;
        NTuple::Item<double>  m_phraw;
        NTuple::Item<double>  m_exraw;   
        NTuple::Item<float>  m_wire; 
        NTuple::Item<float>  m_localwid;
        NTuple::Item<double> m_runNO1;
        NTuple::Item<float>  m_nhit_hit;
        NTuple::Item<float>  m_doca_in;
        NTuple::Item<float>  m_doca_ex;
        NTuple::Item<float>  m_driftdist;
        NTuple::Item<float>  m_eangle;
        NTuple::Item<float>  m_costheta1;
        NTuple::Item<float>  m_pathL;
        NTuple::Item<float>  m_layer;
        NTuple::Item<float>  m_ptrk1;
        NTuple::Item<float>  m_ptrk_hit;
        NTuple::Item<float>  m_t01;
        NTuple::Item<float>  m_tdc_raw;
        NTuple::Item<float>  m_driftT;
        NTuple::Item<float>  m_trackId1;
};
