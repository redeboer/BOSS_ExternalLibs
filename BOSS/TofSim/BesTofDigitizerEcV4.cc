//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: This is the Digitizer for the MRPC with doubled sided readout
//Author: An Fenfen
//Created: Nov, 2015

//---------------------------------------------------------------------------//
//$Id: BesTofDigitizerEcV4.cc

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "BesTofDigitizerEcV4.hh"
#include "BesTofDigi.hh"
#include "BesTofHit.hh"
#include "G4DigiManager.hh"
#include "Randomize.hh"
#include "TMath.h"
#include <math.h>
#include "TNtuple.h"
#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TSpline.h"


BesTofDigitizerEcV4::BesTofDigitizerEcV4() 
{
    PropertyMgr m_propMgr;
    m_propMgr.declareProperty("FileName",         m_fileName = "mrpc.root");
    m_propMgr.declareProperty("RootFlag",         m_rootFlag = false);
    m_propMgr.declareProperty("E",                m_V = 7000);
    m_propMgr.declareProperty("Threshold",        m_threshold = 5.5e+08);

    m_propMgr.declareProperty("nstep",            m_nstep = -1);
    m_propMgr.declareProperty("E_weight",         m_E_weight = -1);
    m_propMgr.declareProperty("saturationFlag",   m_saturationFlag = true);
    m_propMgr.declareProperty("tdcRes_const",     tdcRes_const = -1);
    m_propMgr.declareProperty("adcRes_const",     adcRes_const = -1);
    m_propMgr.declareProperty("calTdcRes_charge_flag",  m_calTdcRes_charge_flag=0);
    m_propMgr.declareProperty("charge2Time_flag",       m_charge2Time_flag=0);
    m_propMgr.declareProperty("calAdcRes_charge_flag",  m_calAdcRes_charge_flag=0);

    IJobOptionsSvc* jobSvc;
    Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);
    jobSvc->setMyProperties("BesTofDigitizerEcV4", &m_propMgr);

    initial();

    if(m_rootFlag)
    {
        m_file = new TFile(m_fileName.c_str(), "RECREATE");
        m_tree = new TTree("mrpc", "mrpc");

        m_tree->Branch("event",     &m_event,     "event/D");
        m_tree->Branch("partId",    &m_partId,    "partId/D");
        m_tree->Branch("module", &m_module, "module/D");
        m_tree->Branch("time_leading_sphi", &m_time_leading_sphi, "time_leading_sphi/D");
        m_tree->Branch("time_leading_xphi", &m_time_leading_xphi, "time_leading_xphi/D");
        m_tree->Branch("time_trailing_sphi", &m_time_trailing_sphi, "time_trailing_sphi/D");
        m_tree->Branch("time_trailing_xphi", &m_time_trailing_xphi, "time_trailing_xphi/D");
        m_tree->Branch("tdcRes", &m_tdcRes, "tdcRes/D");
        m_tree->Branch("tdcRes_charge", &m_tdcRes_charge, "tdcRes_charge/D");
        m_tree->Branch("adc", &m_adc, "adc/D");
        m_tree->Branch("adcRes", &m_adcRes, "adcRes/D");
        m_tree->Branch("adcRes_charge", &m_adcRes_charge, "adcRes_charge/D");
        m_tree->Branch("strip", &m_strip, "strip/D");
        m_tree->Branch("trkIndex", &m_trkIndex, "trkIndex/D");
        m_tree->Branch("tStart", &m_tStart, "tStart/D");
        m_tree->Branch("tPropagate_sphi", &m_tPropagate_sphi, "tPropagate_sphi/D");
        m_tree->Branch("tPropagate_xphi", &m_tPropagate_xphi, "tPropagate_xphi/D");
        m_tree->Branch("tThreshold", &m_tThreshold, "tThreshold/D");
        m_tree->Branch("charge", &m_charge, "charge/D");
        m_tree->Branch("nhit", &m_nhit, "nhit/I");
        m_tree->Branch("ions_hit", m_ions_hit, "ions_hit[nhit]/D");
        m_tree->Branch("trkIndex_hit", m_trkIndex_hit, "trkIndex_hit[nhit]/D");
        m_tree->Branch("pdgCode_hit", m_pdgCode_hit, "pdgCode_hit[nhit]/D");
        m_tree->Branch("gap_hit", m_gap_hit, "gap_hit[nhit]/D");
        m_tree->Branch("underStrip_hit", m_underStrip_hit, "underStrip_hit[nhit]/D");
        m_tree->Branch("locx_hit", m_locx_hit, "locx_hit[nhit]/D");
        m_tree->Branch("locy_hit", m_locy_hit, "locy_hit[nhit]/D");
        m_tree->Branch("locz_hit", m_locz_hit, "locz_hit[nhit]/D");
        m_tree->Branch("x_hit", m_x_hit, "x_hit[nhit]/D");
        m_tree->Branch("y_hit", m_y_hit, "y_hit[nhit]/D");
        m_tree->Branch("z_hit", m_z_hit, "z_hit[nhit]/D");
        m_tree->Branch("px_hit", m_px_hit, "px_hit[nhit]/D");
        m_tree->Branch("py_hit", m_py_hit, "py_hit[nhit]/D");
        m_tree->Branch("pz_hit", m_pz_hit, "pz_hit[nhit]/D");
    }
}


BesTofDigitizerEcV4::~BesTofDigitizerEcV4()
{
    if(m_rootFlag)
    {
        m_file->Write();
        m_file->Close();
    }
}

void BesTofDigitizerEcV4::initial()
{
    m_param = Param();
    m_param.setPar(m_nstep, m_E_weight, m_V);
    m_param.print();
    m_event=-1;
    partId = -999;
    module = -999;
    tdc_sphi = -999;
    tdc_xphi = -999;
    if(tdcRes_const<0) tdcRes_const = 38;
      //45; //sqrt(27*27.+30.*30+20.*20); //ps, 27:TDC; 30:gapNb; 20:cables..
    tdcRes = -999;

    adc = -999;
    if(adcRes_const<0) adcRes_const = 27; //ps TDC
    adcRes = -999;

    time_leading_sphi = -999;
    time_leading_xphi = -999;
    time_trailing_sphi = -999;
    time_trailing_xphi = -999;

    cout<<"Property:"<<endl
        <<"  FileName= "<<m_fileName
        <<"  E= "<<m_V
        <<"  Threshold= "<<m_threshold
        <<"  nstep= "<<m_nstep
        <<"  E_weight= "<<m_E_weight
        <<"  saturationFlag= "<<m_saturationFlag
        <<"  tdcRes_const= "<<tdcRes_const
        <<"  adcRes_const= "<<adcRes_const
        <<"  calTdcRes_charge_flag= "<<m_calTdcRes_charge_flag
        <<"  charge2Time_flag= "<<m_charge2Time_flag
        <<"  calAdcRes_charge_flag= "<<m_calAdcRes_charge_flag
        <<endl;
}

void BesTofDigitizerEcV4::Digitize( ScintSingle* single_module, BesTofDigitsCollection* DC )
{
    m_besTofDigitsCollection = DC;
    G4DigiManager* digiManager = G4DigiManager::GetDMpointer();
    G4int THCID = digiManager->GetHitsCollectionID("BesTofHitsCollection");
    m_THC = (BesTofHitsCollection*) (digiManager->GetHitsCollection(THCID));
    if( !m_THC ) return;

    partId = single_module->GetPartId();
    module = single_module->GetModule_mrpc();
    //cout<<"partId= "<<partId<<"  module= "<<module<<endl;

    //Process the hits
    int nstrip = m_param.nstrip;
    StripStruct stripStruct[12];
    for(int i=0; i<nstrip; i++)
    {
        stripStruct[i].m_param = m_param;
        stripStruct[i].setPar(m_param.alpha, m_param.eta, m_param.v_drift, m_threshold, m_saturationFlag);
    }

    BesTofHit* hit;
    for(unsigned int i=0; i<single_module->GetHitIndexes_mrpc()->size(); i++)
    {
        hit = (*m_THC)[ (*(single_module->GetHitIndexes_mrpc()))[i] ];
        m_event = hit->GetEvent();

        HitStruct hitStruct;
        hitStruct.m_param = m_param;
        hitStruct.trkIndex = hit->GetG4Index();
        hitStruct.pdgCode = hit->GetPDGcode();
        hitStruct.ions = hit->GetIons();
        hitStruct.strip = calStrip(hit->GetLocPos().z()/mm);
        hitStruct.underStrip = underStrip(hit->GetLocPos().x()/mm, hit->GetLocPos().z()/mm);
        hitStruct.gap = hit->GetGapNb();
        hitStruct.glbTime = hit->GetTime()/ns;
        hitStruct.locx = hit->GetLocPos().x()/mm;
        hitStruct.locy = hit->GetLocPos().y()/mm;
        hitStruct.locz = hit->GetLocPos().z()/mm;
        hitStruct.x = hit->GetPos().x()/mm;
        hitStruct.y = hit->GetPos().y()/mm;
        hitStruct.z = hit->GetPos().z()/mm;
        hitStruct.px = hit->GetMomentum().x()/(GeV/(3e+08*m/s));
        hitStruct.py = hit->GetMomentum().y()/(GeV/(3e+08*m/s));
        hitStruct.pz = hit->GetMomentum().z()/(GeV/(3e+08*m/s));
        //hitStruct.print();

        if(hitStruct.ions>0 && hitStruct.glbTime>0) 
        {
            stripStruct[hitStruct.strip].hitStructCol.push_back(hitStruct);
        }
    }

    //test multihit, study the lower peak in charge
    int hitSize=0;

    for(int i=0; i<nstrip; i++)
    {
        if(stripStruct[i].hitStructCol.size()==0) continue;
        stripStruct[i].strip = i;
        stripStruct[i].calFirstHit();
        stripStruct[i].avalanche();
        //stripStruct[i].print();

        if(stripStruct[i].tThreshold<=0) continue;

        tdc_sphi = stripStruct[i].tStart + stripStruct[i].tThreshold + stripStruct[i].tPropagate_sphi;
        tdc_xphi = stripStruct[i].tStart + stripStruct[i].tThreshold + stripStruct[i].tPropagate_xphi;

        double tdcRes_charge;
        if(m_calTdcRes_charge_flag==0)
        {
            tdcRes_charge = calTdcRes_charge(stripStruct[i].charge*1000); //ps, charge in fC
        }
        else if(m_calTdcRes_charge_flag==1)
        {
            tdcRes_charge = calTdcRes_charge1(stripStruct[i].charge*1000);
        }
        else if(m_calTdcRes_charge_flag==2)
        {
            tdcRes_charge = 0;
        }

        tdcRes = sqrt(tdcRes_charge*tdcRes_charge+tdcRes_const*tdcRes_const)/1000; //ns

        tdc_sphi = G4RandGauss::shoot(tdc_sphi, tdcRes);
        tdc_xphi = G4RandGauss::shoot(tdc_xphi, tdcRes);

        if(m_charge2Time_flag==0)
        {
            adc = charge2Time(stripStruct[i].charge*1000); //ns, charge in fC
        }
        else if(m_charge2Time_flag==1)
        {
            adc = charge2Time1(stripStruct[i].charge*1000);
        }

        double adcRes_charge;       
        if(m_calAdcRes_charge_flag==0)
        {
            adcRes_charge = calAdcRes_charge(stripStruct[i].charge*1000); //ps, charge in fC
        }
        else if(m_calAdcRes_charge_flag==1)
        {
            adcRes_charge = calAdcRes_charge1(stripStruct[i].charge*1000);
        }
        else if(m_calAdcRes_charge_flag==2)
        {
            adcRes_charge = 0;
        }

        adcRes = sqrt(adcRes_charge*adcRes_charge+adcRes_const*adcRes_const)/1000;
        adc = G4RandGauss::shoot(adc, adcRes);
        if(adc<0) adc=0;

        time_leading_sphi = tdc_sphi;
        time_leading_xphi = tdc_xphi;
        time_trailing_sphi = tdc_sphi+adc;
        time_trailing_xphi = tdc_xphi+adc;


        //save digi information
        BesTofDigi *digi = new BesTofDigi;
        digi->SetTrackIndex(stripStruct[i].trkIndex);
        digi->SetPartId(partId);
        digi->SetModule(module);
        digi->SetStrip(stripStruct[i].strip);
        int mo = (partId-3)*36+module;
        int st = stripStruct[i].strip;
        if(m_param.deadChannel[mo][st]==0 || m_param.deadChannel[mo][st]==2)
        {
            //Set dead channel
            digi->SetForwT1(-999);
            digi->SetForwT2(-999);
        }
        else
        {
            digi->SetForwT1(time_leading_sphi);
            digi->SetForwT2(time_trailing_sphi);
        }
        if(m_param.deadChannel[mo][st]==1 || m_param.deadChannel[mo][st]==2)
        {
            digi->SetBackT1(-999);
            digi->SetBackT2(-999);
        }
        else
        {
            digi->SetBackT1(time_leading_xphi);
            digi->SetBackT2(time_trailing_xphi);
        }
        m_besTofDigitsCollection->insert(digi);
        //cout<<"Print digi info: "
        //    <<"  partId= "<<partId
        //    <<"  module= "<<module
        //    <<"  strip= "<<stripStruct[i].strip
        //    <<"  time_leading_sphi= "<<time_leading_sphi
        //    <<"  time_leading_xphi= "<<time_leading_xphi
        //    <<"  time_trailing_sphi= "<<time_trailing_sphi
        //    <<"  time_trailing_xphi= "<<time_trailing_xphi
        //    <<endl;


        //save digi information
        if(m_rootFlag)
        {
            m_partId = partId;
            m_module = module;
            m_time_leading_sphi = time_leading_sphi;
            m_time_leading_xphi = time_leading_xphi;
            m_time_trailing_sphi = time_trailing_sphi;
            m_time_trailing_xphi = time_trailing_xphi;
            m_tdcRes = tdcRes;
            m_tdcRes_charge = tdcRes_charge;
            m_adc = adc;
            m_adcRes = adcRes;
            m_adcRes_charge = adcRes_charge;

            m_strip = stripStruct[i].strip;
            m_trkIndex = stripStruct[i].trkIndex;
            m_tStart = stripStruct[i].tStart;
            m_tPropagate_sphi = stripStruct[i].tPropagate_sphi;
            m_tPropagate_xphi = stripStruct[i].tPropagate_xphi;
            m_tThreshold = stripStruct[i].tThreshold;
            m_charge = stripStruct[i].charge;

            m_nhit = stripStruct[i].hitStructCol.size();
            //cout<<"m_nhit= "<<m_nhit<<endl;
            for(int j=0; j<m_nhit; j++)
            {
                m_ions_hit[j] = stripStruct[i].hitStructCol[j].ions;
                m_trkIndex_hit[j] = stripStruct[i].hitStructCol[j].trkIndex;
                m_pdgCode_hit[j] = stripStruct[i].hitStructCol[j].pdgCode;
                m_gap_hit[j] = stripStruct[i].hitStructCol[j].gap;
                m_underStrip_hit[j] = stripStruct[i].hitStructCol[j].underStrip;
                m_locx_hit[j] = stripStruct[i].hitStructCol[j].locx;
                m_locy_hit[j] = stripStruct[i].hitStructCol[j].locy;
                m_locz_hit[j] = stripStruct[i].hitStructCol[j].locz;
                m_x_hit[j] = stripStruct[i].hitStructCol[j].x;
                m_y_hit[j] = stripStruct[i].hitStructCol[j].y;
                m_z_hit[j] = stripStruct[i].hitStructCol[j].z;
                m_px_hit[j] = stripStruct[i].hitStructCol[j].px;
                m_py_hit[j] = stripStruct[i].hitStructCol[j].py;
                m_pz_hit[j] = stripStruct[i].hitStructCol[j].pz;
            }
            m_tree->Fill();
        }
    }
}

int BesTofDigitizerEcV4::calStrip(double locZ)
{
    int strip=-1;
    double stripWidth = m_param.strip_z+m_param.strip_gap; //Strip spread: (24+3)mm
    int nstrip = m_param.nstrip;
    //the offset between strip coordinate and gas SD: 0.5
    double length = locZ+stripWidth*nstrip/2-0.5;
    if(length<=0)
    {
        strip=0;
    }
    else if(length<stripWidth*nstrip)
    {
        for(int i=0; i<nstrip; i++)
        {
            if(length>i*stripWidth && length<(i+1)*stripWidth)
            {
                strip = i;
                break;
            }
        }
    }
    else
    {
        strip=nstrip-1;
    }
    if(strip<0) strip=0;
    if(strip>nstrip-1) strip=nstrip-1;

    return strip;
}

bool BesTofDigitizerEcV4::underStrip(double locX, double locZ)
{
    bool flag = 0;
    int strip=-1;
    double stripWidth = m_param.strip_z+m_param.strip_gap; //Strip spread: (24+3)mm
    int nstrip = m_param.nstrip;
    //the offset between strip coordinate and gas SD: 0.5
    double length = locZ+stripWidth*nstrip/2-0.5;
    if(length<stripWidth*nstrip)
    {
        for(int i=0; i<nstrip; i++)
        {
            if(length>i*stripWidth && length<(i+1)*stripWidth)
            {
                strip = i;
                if(length>i*stripWidth+m_param.strip_gap/2 && length<(i+1)*stripWidth-m_param.strip_gap/2
                            && locX>-m_param.strip_x[strip]/2 && locX<m_param.strip_x[strip]/2) flag=1;
                break;
            }
        }
    }

    return flag;
}


void BesTofDigitizerEcV4::StripStruct::calFirstHit()
{
    for(unsigned int i=0; i<hitStructCol.size(); i++)
    {
        if(hitStructCol[i].glbTime<tStart)
        {
            tStart = hitStructCol[i].glbTime;
            trkIndex = hitStructCol[i].trkIndex;
            hitStructCol[i].calTPropagate();
            tPropagate_sphi = hitStructCol[i].tPropagate_sphi;
            tPropagate_xphi = hitStructCol[i].tPropagate_xphi;
        }
    }
}

void BesTofDigitizerEcV4::HitStruct::calTPropagate()
{
    if(strip<0 || strip>m_param.nstrip-1)
    {
        cout<<"!! BesTofDigitizerEcV4::HitStruct::calTPropagate Wrong Strip !!!"<<endl;
        return;
    }

    //It can be minus, consistent with calibration
    double length_sphi = m_param.strip_x[strip]/2-locx; //mm
    tPropagate_sphi = abs(length_sphi)/v_propagate;

    double length_xphi = m_param.strip_x[strip]/2+locx; //mm
    tPropagate_xphi = abs(length_xphi)/v_propagate;
}

double BesTofDigitizerEcV4::HitStruct::calAvaLength()
{
    //This calculation depends on the arangements of the gasLayer order and the turnover of gasContainer.
    //all modules have the same local y trends: y larger, 11->0
    //In units of mm
    double length=0;
    if(gap>=0 && gap<m_param.ngap/2) length = m_param.gapWidth/2+locy;
    else if(gap<m_param.ngap) length = m_param.gapWidth/2-locy;
    else
    {
        cout<<"BesTofDigitizerEcV4::StripStruct::calAvaLength  Wrong gap calculation !!!"<<endl;
        return -999.0;
    }

    return length;
}

void BesTofDigitizerEcV4::StripStruct::avalanche()
{
    //process each hit
    for(unsigned int i=0; i<hitStructCol.size(); i++)
    {
        hitStructCol[i].ava_pos.clear();
        hitStructCol[i].ava_num.clear();

        hitStructCol[i].ava_pos[0] = hitStructCol[i].calAvaLength();
        hitStructCol[i].ava_num[0] = hitStructCol[i].ions;
        //cout<<"i= "<<i<<"  gap= "<<hitStructCol[i].gap<<"  initial pos= "<<hitStructCol[i].ava_pos[0]<<endl;
        for(int j=1; j<m_param.nstep; j++)
        {
            hitStructCol[i].ava_pos[j] = hitStructCol[i].ava_pos[j-1] + m_param.stepWidth;
            if(saturationFlag==true && hitStructCol[i].ava_num[j-1]>1.5e+07) //saturation e+07~e+08, ~2pC, Reather limit
            {
                hitStructCol[i].ava_num[j] = hitStructCol[i].ava_num[j-1];
            }
            else
            {
                hitStructCol[i].ava_num[j] = calNextN(hitStructCol[i].ava_num[j-1]);
            }
            if(hitStructCol[i].ava_pos[j]>m_param.gapWidth) break;
        }
    }

    //decide threshold and charge
    bool over_threshold = false;
    long int sum = 0;
    for(int i=0; i<m_param.nstep; i++)
    {
        for(unsigned int j=0; j<hitStructCol.size(); j++)
        {
            if(i<hitStructCol[j].ava_pos.size() && hitStructCol[j].ava_pos[i]<m_param.gapWidth) 
            {
                sum += hitStructCol[j].ava_num[i];
            }
        }
        //cout<<"sum= "<<sum<<"  avaSize= "<<hitStructCol.size()<<endl;

        if(over_threshold==false)
        {
            if(sum>threshold)
            {
                over_threshold = true;
                tThreshold = (m_param.gapWidth)/(m_param.nstep)/v_drift*(i+1);
            }
        }
    }

    charge = sum*(m_param.E_weight)*(v_drift)*(m_param.eCharge)*(m_param.gapWidth)/(m_param.nstep)/v_drift;
}


long int BesTofDigitizerEcV4::StripStruct::calNextN(int num)
{
    if(num<150)
    {
        long int nextN = 0;
        double rdm;
        for(int i=0; i<num; i++)
        {
            rdm = G4UniformRand();
            nextN += multiply(rdm);
        }
        return nextN;
    }
    else
    {
        double nbar = exp((alpha-eta)*m_param.stepWidth);
        double sigma = calSigma();
        double mean = num*nbar;
        double resolution = G4RandGauss::shoot(0,(sqrt(num)*sigma));
        long int nextN = mean+resolution;
        return nextN;
    }
}

long int BesTofDigitizerEcV4::StripStruct::multiply(double rdm)
{
    double nbar = exp((alpha-eta)*m_param.stepWidth);
    double k = eta/alpha;
    double rdm_border = k*(nbar-1)/(nbar-k);
    if(rdm<rdm_border)
    {
        return 0;
    }
    else
    {
        long int number = 1.+1./log((nbar-1.)/(nbar-k))*log((nbar-k)*(rdm-1)/(k-1)/nbar);
        return number;
    }
}

double BesTofDigitizerEcV4::StripStruct::calSigma()
{
    double nbar = exp((alpha-eta)*m_param.stepWidth);
    double k = eta/alpha;
    double sigma = sqrt((1+k)/(1-k)*nbar*(nbar-1));
    return sigma;
}

double BesTofDigitizerEcV4::calTdcRes_charge(double charge_fC)
{
    double time=0;
    if( charge_fC<250) 
    {
        time =  100.764*exp(-charge_fC*0.0413966+0.377154)+ 13.814;
    }
    else
    {
        time = 12.8562+0.000507299*charge_fC;
    }
    if(time<0) time=0;
    return time; //ps
}

double BesTofDigitizerEcV4::charge2Time(double charge_fC)
{
    double time=0;
    time=-120.808/log(charge_fC*30.1306)+26.6024; //ns
    if(time<0) time=0;
    return time;
}

double BesTofDigitizerEcV4::calAdcRes_charge(double charge_fC)
{
    double time=0;
    if(charge_fC<250)
    {
        time = 72.6005*exp(-charge_fC*0.0302626 + 1.49059) + 40.8627;
    }
    else 
    {
        time = 32.6233+0.00404149*charge_fC;
    }
    if(time<0) time=0;
    return time; //ps
}

double BesTofDigitizerEcV4::calTdcRes_charge1(double charge_fC)
{
    double result =0;
    if( charge_fC > 50.)
    {
        result =  67.6737*exp(-charge_fC/50.9995-0.27755)+9.06223;
    }
    else
    {
        result = 176.322-2.98345*charge_fC;
    }
    if(result<0) result=0;
    return result; //ps
}

double BesTofDigitizerEcV4::charge2Time1(double charge_fC)
{
    double time=0;
    time=-4.50565/log(charge_fC*0.0812208)+16.6653; //ns
    if(time<0) time=0;
    return time;
}

double BesTofDigitizerEcV4::calAdcRes_charge1(double charge_fC)
{
    double time = 64.3326*exp(-charge_fC/25.4638 + 0.944184)+19.4846;
    if(time<0) time=0;
    return time; //ps
}


BesTofDigitizerEcV4::HitStruct::HitStruct()
{
    initial();
}

//in units of mm or ns
void BesTofDigitizerEcV4::HitStruct::initial()
{
    trkIndex = -999.0;
    pdgCode = -999.0;
    ions = -999.0;
    strip = -999.0;
    gap = -999.0;
    glbTime = -999.0;
    locx = -999.0;
    locy = -999.0;
    locz = -999.0;
    x = -999.0;
    y = -999.0;
    z = -999.0;
    px = -999.0;
    py = -999.0;
    pz = -999.0;
    v_propagate = 0.5*0.299792458e+3; //mm/ns
    tPropagate_sphi = -999.0;
    tPropagate_xphi = -999.0;
}

BesTofDigitizerEcV4::StripStruct::StripStruct()
{
    initial();
}

void BesTofDigitizerEcV4::StripStruct::initial()
{
    //properties to get
    strip = -999.0;
    trkIndex = -999.0;
    tStart = 99999.0;
    tPropagate_sphi = -999.0;
    tPropagate_xphi = -999.0;
    tThreshold = -999.0;
    charge = -999.0;

    //parameters to tune
    E = 106;
    alpha = 144800./1000; //-999.0; /mm^-1
    eta = 5013./1000; //-999.0; /mm^-1
    threshold = 1.5e+08; //Correspond to induced charge of 15 fC
    v_drift = 210.9e-3; // mm/ns

    hitStructCol.clear();
}

void BesTofDigitizerEcV4::StripStruct::setPar(double alpha_n, double eta_n, double drift_n, double threshold_n, bool saturationFlag_n)
{
    alpha = alpha_n;
    eta = eta_n;
    v_drift = drift_n;
    threshold = threshold_n;

    saturationFlag = saturationFlag_n;
}

void BesTofDigitizerEcV4::Param::setPar(int nstep_n, double E_weight_n, double E_n)
{
    if(nstep_n>0) nstep = nstep_n;
    if(E_weight_n>0) E_weight = E_weight_n;
    
    E = E_n;
    double E_eff = E/1000*2/6/(gapWidth/10); //kV/cm
    alpha = getAlpha(E_eff); //mm^-1
    eta = getEta(E_eff); //mm^-1
    v_drift = getV(E_eff); //mm/ns
}

BesTofDigitizerEcV4::Param::Param()
{
    //parameters fixed
    tofPara = BesTofGeoParameter::GetInstance();
    nstrip = 12;
    nmodule = 72;
    std::stringstream ss;
    for(int i=0; i<nstrip; i++)
    {
        ss.str("");
        ss<<"strip_x["<<i<<"]";
        strip_x[i] = tofPara->Get(ss.str().c_str()); //mm
    }
    strip_z = tofPara->Get("strip_z");
    strip_gap = tofPara->Get("strip_gap");

    ngap = 12;
    gapWidth = 0.22; //mm
    nstep = 200;
    stepWidth = gapWidth/nstep;
    E_weight = 1./(6.*0.22+(5.*0.4+2*0.55)/3.7); //V/mm
    eCharge = 1.60217733e-7; //pC
    tofPara->Get_deadChannel(deadChannel);

    //print();
}

double BesTofDigitizerEcV4::Param::getAlpha(double E)
{
    //electric field: kV/cm; alpha: mm-1
    //kV/cm
    double e[22] =
    {
        65,
        70  ,
        75  ,
        80  ,
        85  ,
        90  ,
        95  ,
        100 ,
        102 ,
        104 ,
        106 ,
        108 ,
        110 ,
        112 ,
        114 ,
        116 ,
        118 ,
        120 ,
        125 ,
        130 ,
        135 ,
        140
    };

    //mm-1
    double alpha[22]=
    {
        383.5/10  ,
        471  /10  ,
        564.5/10  ,
        663.6/10  ,
        777.1/10  ,
        877  /10  ,
        990.8/10  ,
        1106 /10  ,
        1154 /10  ,
        1199 /10  ,
        1253 /10  ,
        1296 /10  ,
        1344 /10  ,
        1396 /10  ,
        1448 /10  ,
        1502 /10  ,
        1545 /10  ,
        1597 /10  ,
        1726 /10  ,
        1858 /10  ,
        1992 /10  ,
        2124 /10  ,
    };

    TSpline3* sp_alpha = new TSpline3("sp_alpha", e, alpha, 22);
    double alphaVal = sp_alpha->Eval(E);
    delete sp_alpha;
    return alphaVal;
}

double BesTofDigitizerEcV4::Param::getEta(double E)
{
    //electric field: kV/cm; eta: mm-1
    //kV/cm
    double e[22] =
    {
        65,
        70  ,
        75  ,
        80  ,
        85  ,
        90  ,
        95  ,
        100 ,
        102 ,
        104 ,
        106 ,
        108 ,
        110 ,
        112 ,
        114 ,
        116 ,
        118 ,
        120 ,
        125 ,
        130 ,
        135 ,
        140
    };

    //mm-1
    double eta[22]=
    {
        132.6/10  ,
        117.2/10  ,
        102.6/10  ,
        88.26/10  ,
        79.81/10  ,
        74.0 /10  ,
        66.7 /10  ,
        62.7 /10  ,
        61.4 /10  ,
        57.4 /10  ,
        55.45/10  ,
        54.35/10  ,
        52.48/10  ,
        51.3 /10  ,
        50.1 /10  ,
        48.3 /10  ,
        48.28/10  ,
        46.00/10  ,
        44.08/10  ,
        41.67/10  ,
        39.97/10  ,
        38.04/10
    };

    TSpline3* sp_eta = new TSpline3("sp_eta", e, eta, 22);
    double etaVal = sp_eta->Eval(E);
    delete sp_eta;
    return etaVal;
}

double BesTofDigitizerEcV4::Param::getV(double E)
{
    //electric field: kV/cm; velocity: mm/ns
    //kV/cm
    double e[22] =
    {
        65,
        70  ,
        75  ,
        80  ,
        85  ,
        90  ,
        95  ,
        100 ,
        102 ,
        104 ,
        106 ,
        108 ,
        110 ,
        112 ,
        114 ,
        116 ,
        118 ,
        120 ,
        125 ,
        130 ,
        135 ,
        140
    };

    //mm/ns
    double v[22]=
    {
        130.2/1000   ,
        138.5/1000   ,
        146.7/1000   ,
        155.0/1000   ,
        163.3/1000   ,
        171.4/1000   ,
        179.7/1000   ,
        187.7/1000   ,
        191.2/1000   ,
        194.5/1000   ,
        197.9/1000   ,
        201.2/1000   ,
        204.5/1000   ,
        207.6/1000   ,
        210.9/1000   ,
        214.4/1000   ,
        217.5/1000   ,
        220.9/1000   ,
        228.8/1000   ,
        237.0/1000   ,
        244.7/1000   ,
        252.9/1000
    };

    TSpline3* sp_v = new TSpline3("sp_v", e, v, 22);
    double vVal = sp_v->Eval(E);
    delete sp_v;
    return vVal;
}

void BesTofDigitizerEcV4::Param::print()
{
    cout<<"Fixed parameters: "<<endl;
    for(int i=0; i<nstrip; i++)
    {
        cout<<"  strip_x["<<i<<"]= "<<strip_x[i];
    }
    for(int i=0; i<nmodule; i++)
    {
        for(int j=0; j<nstrip; j++)
        {
            if(deadChannel[i][j]!=-999)
            {
                cout<<"  deadChannel["<<i<<"]["<<j<<"]= "<<deadChannel[i][j];
            }
        }
    }

    cout<<"  strip_z= "<<strip_z
        <<"  strip_gap= "<<strip_gap
        <<"  ngap= "<<ngap
        <<"  gapWidth= "<<gapWidth
        <<"  nstep= "<<nstep
        <<"  stepWidth= "<<stepWidth
        <<"  E_weight= "<<E_weight
        <<"  eCharge= "<<eCharge
        <<"  E= "<<E
        <<"  alpha= "<<alpha
        <<"  eta= "<<eta
        <<"  v_drift= "<<v_drift
        <<endl;
}

void BesTofDigitizerEcV4::HitStruct::print()
{
    cout<<"Hit information:  "<<endl;
    cout<<"  trkIndex= "<<trkIndex
        <<"  pdgCode= "<<pdgCode
        <<"  ions= "<<pdgCode
        <<"  strip= "<<strip
        <<"  gap= "<<gap
        <<"  glbTime= "<<glbTime
        <<"  locx= "<<locx
        <<"  locy= "<<locy
        <<"  locz= "<<locz
        <<"  x= "<<x
        <<"  y= "<<y
        <<"  z= "<<z
        <<"  px= "<<px
        <<"  py= "<<py
        <<"  pz= "<<pz
        <<"  v_propagate= "<<v_propagate
        <<"  tPropagate_sphi= "<<tPropagate_sphi
        <<"  tPropagate_xphi= "<<tPropagate_xphi
        <<endl;
}

void BesTofDigitizerEcV4::StripStruct::print()
{
    cout<<"Strip information:  "<<endl;
    cout<<"  strip= "<<strip
        <<"  trkIndex= "<<trkIndex
        <<"  tStart= "<<tStart
        <<"  tPropagate_sphi= "<<tPropagate_sphi
        <<"  tPropagate_xphi= "<<tPropagate_xphi
        <<"  tThreshold "<<tThreshold
        <<"  charge= "<<charge
        <<"  alpha= "<<alpha
        <<"  eta= "<<eta
        <<"  threshold= "<<threshold
        <<"  v_drift= "<<v_drift
        <<endl;
}
