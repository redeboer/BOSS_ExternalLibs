#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/StatusCode.h"

#include <fstream>
#include <string>
#include "TFile.h"
#include "TTree.h"

#include "DedxCalibAlg/DedxCalib.h"

DedxCalib::DedxCalib(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{
    declareProperty("ParticleType",ParticleFlag=-1);
    declareProperty("EventType", m_eventType = "isBhabha");
    declareProperty("CalibFlag",m_calibflag=63);
    declareProperty("PhShapeFlag",m_phShapeFlag=0);
    declareProperty("TruncRate",truncate = 0.7);
    declareProperty("RecFileList", m_recFileList = "no file");
    declareProperty("RootFile",m_rootfile = "no file");
    declareProperty("CurveFile",m_curvefile = std::string("no rootfile"));
}

StatusCode DedxCalib::initialize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "DedxCalib initialze()" << endreq;

    StatusCode gesc = service("MdcGeomSvc", geosvc);
    if (gesc ==  StatusCode::SUCCESS)
    {
        log << MSG::INFO <<"MdcGeomSvc is running"<<endreq;
    }
    else
    {
        log << MSG::ERROR <<"MdcGeomSvc is failed"<<endreq;
        return StatusCode::SUCCESS;
    }

    StatusCode scex = service("DedxCorrecSvc", exsvc);
    if (scex ==  StatusCode::SUCCESS) {
        log << MSG::INFO <<"Hi, DedxCorrectSvc is running"<<endl;
    } else {
        log << MSG::ERROR <<"DedxCorrectSvc is failed"<<endl;
        return StatusCode::SUCCESS;
    }
    exsvc->set_flag( m_calibflag );

    initializing();
    BookHists();
    getCurvePar();

    return StatusCode::SUCCESS;
}

StatusCode DedxCalib::execute()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "DedxCalib execute()" << endreq;

    genNtuple();
    FillHists();
    AnalyseHists();
    WriteHists();

    return StatusCode::SUCCESS;
}

StatusCode DedxCalib::finalize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "DedxCalib finalize()" << endreq;

    return StatusCode::SUCCESS;
}

void DedxCalib::initializing() {}

void DedxCalib::genNtuple() {}

void DedxCalib::BookHists() {}

void DedxCalib::FillHists() {}

void DedxCalib::AnalyseHists() {}

void DedxCalib::WriteHists() {}

void DedxCalib::ReadRecFileList()
{   

    MsgStream log(msgSvc(), name());
    log<<MSG::INFO<<"DedxCalib::ReadRecFileList()"<<endreq;

    ifstream filea(m_recFileList.c_str(),ifstream::in);
    cout<<m_recFileList.c_str()<<endl;

    string runlist;
    filea>>runlist;
    do  
    {   
        m_recFileVector.push_back(runlist);
        cout<<runlist.c_str()<<endl;
        filea>>runlist;
    }while(filea);
}

double DedxCalib::cal_dedx_bitrunc(float truncate, std::vector<double>  phlist)
{   
    sort(phlist.begin(),phlist.end());
    int smpl = (int)(phlist.size()*(truncate+0.05));
    int min_cut = (int)( phlist.size()*0.05 + 0.5 );
    double qSum = 0;
    unsigned i = 0;
    for(vector<double>::iterator ql= phlist.begin();ql!=phlist.end();ql++)
    {   
        i++;
        if(i<= smpl && i>=min_cut ) qSum += (*ql);
    }
    double trunc=-99;
    int usedhit = smpl-min_cut+1;
    if(usedhit>0)  trunc=qSum/usedhit;

    return trunc;
}

double DedxCalib::cal_dedx(float truncate, std::vector<double>  phlist)
{
    sort(phlist.begin(),phlist.end());
    int smpl = (int)(phlist.size()*(truncate+0.05));
    int min_cut = 0;
    double qSum = 0;
    unsigned i = 0;
    for(vector<double>::iterator ql= phlist.begin();ql!=phlist.end();ql++)
    {
        i++;
        if(i<= smpl && i>=min_cut ) qSum += (*ql);
    }
    double trunc=-99;
    int usedhit = smpl-min_cut+1;
    if(usedhit>0)  trunc=qSum/usedhit;

    return trunc;
}

void DedxCalib::getCurvePar()
//void DedxCalib::getCurvePar(int runflag)
{
    if(m_curvefile=="no rootfile")
    {
        cout<<"no curve file! can not calculate chi!!! "<<endl;
        return;
    }

    double Curve[100];
    double Sigma[100];
    int CurveSize,SigmaSize;
    TFile* f = new TFile(m_curvefile.c_str());
    TTree *curve = (TTree*) f->Get("curve");
    TTree *sigma = (TTree*) f->Get("sigma");
    curve->SetBranchAddress("CurveSize",&CurveSize);
    curve->SetBranchAddress("curve",Curve);
    sigma->SetBranchAddress("SigmaSize",&SigmaSize);
    sigma->SetBranchAddress("sigma",Sigma);
    curve->GetEntry(0);
    sigma->GetEntry(0);

    for(int i=0; i<CurveSize; i++) // get the dedx curve parameters
    {
        cout<<Curve[i]<<endl;
        if(i==0)  vFlag[0] = (int) Curve[i];  //first element is dedx curve version
        else  Curve_Para.push_back(Curve[i]);  //dedx curve parameters
    }   
    for(int i=0; i<SigmaSize; i++)
    {
        cout<<Sigma[i]<<endl;
        if(i==0) vFlag[1] = (int) Sigma[i];  //dedx sigma version: 2: psip; 3:jpsi
        else  Sigma_Para.push_back(Sigma[i]);  //dedx sigma parameters
    }
    //if(runflag>0)   vFlag[2]=0;
    //else            vFlag[2]=1;
}

void DedxCalib::set_dEdx( int landau, float dEdx, int trkalg, int runflag, int dedxhit_use, float ptrk, float theta, float pl_rp, int vflag[3] , double t0)
{ 
    //landau: 1:landau distribution; 0:gaus distribution
    //pl_rp: 1.5, not know what
    //getCurvePar(runflag);
 if(runflag>0)   vFlag[2]=0;
     else            vFlag[2]=1;

    //some old data with old methods
    if(runflag ==1 || runflag ==2 )
      dedx_pid_exp_old( landau, runflag, (float)dEdx, (int)dedxhit_use,
                  (float)ptrk, (float)theta, (float)t0,(float)pl_rp,
                  m_dedx_exp, m_ex_sigma, m_pid_prob, m_chi_ex);
    //for 2009 psip data and after
    else
      dedx_pid_exp( vflag, (float)dEdx, trkalg,(int)dedxhit_use,
                  (float)ptrk, (float)theta, (float)t0,(float)pl_rp,
                  m_dedx_exp, m_ex_sigma, m_pid_prob, m_chi_ex, Curve_Para, Sigma_Para);
}
