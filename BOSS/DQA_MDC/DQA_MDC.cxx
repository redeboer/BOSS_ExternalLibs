#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Algorithm.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ITHistSvc.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "RawEvent/RawDataUtil.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"

#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "ReconEvent/ReconEvent.h"

#include "DQAEvent/DQAEvent.h"
#include "DQA_MDC/DQA_MDC.h"

using CLHEP::Hep3Vector;
/////////////////////////////////////////////////////////////////////////////

DQA_MDC::DQA_MDC(const std::string& name, ISvcLocator* pSvcLocator) :
    Algorithm(name, pSvcLocator) {

        //Declare the properties  
    }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_MDC::initialize(){
    MsgStream log(msgSvc(), name());

    log << MSG::INFO << "in initialize()" << endmsg;
    StatusCode status;

    // Call Histogram service
    if(service("THistSvc", m_thsvc).isFailure()) {
        log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
        return StatusCode::FAILURE;
    }

    // Resolution histograms
    m_hresAllIncBb = new TH1F("HResAllIncBb", "", 200, -1.0, 1.0);
    if(m_thsvc->regHist("/DQAHist/MDC/hresAllIncBb", m_hresAllIncBb).isFailure()) 
    { log << MSG::ERROR << "Couldn't register HResAllIncBb" << endreq; }

    m_hresAllExcBb = new TH1F("HResAllExcBb", "", 200, -1.0, 1.0);
    if(m_thsvc->regHist("/DQAHist/MDC/hresAllExcBb", m_hresAllExcBb).isFailure()) 
    { log << MSG::ERROR << "Couldn't register HResAllExcBb" << endreq; }
 
    m_hresAllEvaBb = new TH1F("HResAllEvaBb", "", 200, -1.0, 1.0);
    if(m_thsvc->regHist("/DQAHist/MDC/hresAllEvaBb", m_hresAllEvaBb).isFailure()) 
    { log << MSG::ERROR << "Couldn't register HResAllEvaBb" << endreq; }
 
    m_ppLabBb = new TH1F("PpLabBb", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hppLabBb", m_ppLabBb).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PpLabBb" << endreq; }
    m_pmLabBb = new TH1F("PmLabBb", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hpmLabBb", m_pmLabBb).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PmLabBb" << endreq; }

    m_ppCmsBb = new TH1F("PpCmsBb", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hppCmsBb", m_ppCmsBb).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PpCmsBb" << endreq; }
    m_pmCmsBb = new TH1F("PmCmsBb", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hpmCmsBb", m_pmCmsBb).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PmCmsBb" << endreq; }

    m_pTotLabBb = new TH1F("PTotLabBb", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hpTotLabBb", m_pTotLabBb).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PTotLabBb" << endreq; }
    m_pTotCmsBb = new TH1F("PTotCmsBb", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hpTotCmsBb", m_pTotCmsBb).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PTotCmsBb" << endreq; }

    m_hresAllIncDimu = new TH1F("HResAllIncDimu", "", 200, -1.0, 1.0);
    if(m_thsvc->regHist("/DQAHist/MDC/hresAllIncDimu", m_hresAllIncDimu).isFailure()) 
    { log << MSG::ERROR << "Couldn't register HResAllIncDimu" << endreq; }

    m_hresAllExcDimu = new TH1F("HResAllExcDimu", "", 200, -1.0, 1.0);
    if(m_thsvc->regHist("/DQAHist/MDC/hresAllExcDimu", m_hresAllExcDimu).isFailure()) 
    { log << MSG::ERROR << "Couldn't register HResAllExcDimu" << endreq; }
 
    m_hresAllEvaDimu = new TH1F("HResAllEvaDimu", "", 200, -1.0, 1.0);
    if(m_thsvc->regHist("/DQAHist/MDC/hresAllEvaDimu", m_hresAllEvaDimu).isFailure()) 
    { log << MSG::ERROR << "Couldn't register HResAllEvaDimu" << endreq; }
 
    m_ppLabDimu = new TH1F("PpLabDimu", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hppLabDimu", m_ppLabDimu).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PpLabDimu" << endreq; }
    m_pmLabDimu = new TH1F("PmLabDimu", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hpmLabDimu", m_pmLabDimu).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PmLabDimu" << endreq; }

    m_ppCmsDimu = new TH1F("PpCmsDimu", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hppCmsDimu", m_ppCmsDimu).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PpCmsDimu" << endreq; }
    m_pmCmsDimu = new TH1F("PmCmsDimu", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hpmCmsDimu", m_pmCmsDimu).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PmCmsDimu" << endreq; }

    m_pTotLabDimu = new TH1F("PTotLabDimu", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hpTotLabDimu", m_pTotLabDimu).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PTotLabDimu" << endreq; }
    m_pTotCmsDimu = new TH1F("PTotCmsDimu", "", 800, 0, 3);
    if(m_thsvc->regHist("/DQAHist/MDC/hpTotCmsDimu", m_pTotCmsDimu).isFailure()) 
    { log << MSG::ERROR << "Couldn't register PTotCmsDimu" << endreq; }


    log << MSG::INFO << "Initialize done!" <<endmsg;
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_MDC::execute() {

    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "in execute()" << endreq;

    SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
    m_run   = eventHeader->runNumber();
    m_event = eventHeader->eventNumber();
    log << MSG::DEBUG <<"Run " << m_run << "\tEvent "  << m_event <<endreq;

    SmartDataPtr<DQAEvent::DQAEvent> dqaevt(eventSvc(), "/Event/DQATag");
    if ( dqaevt ) {
        log << MSG::INFO << "success get DQAEvent" << endreq;
    } else {
        log << MSG::ERROR << "Error accessing DQAEvent" << endreq;
        return StatusCode::FAILURE;
    }
    log << MSG::DEBUG << "DQA event tag = " << dqaevt->EventTag() << endreq;

    double p;
    double p_cms;
    bool isDimu = false;
    if ( dqaevt->Dimu() )  isDimu = true;
    log << MSG::INFO << "DQADimuTag:\t" << dqaevt->Dimu() << endreq;
    //cout << "DQADimuTag:\t" << dqaevt->Dimu() << endl;

    bool isBb = false;
    if ( dqaevt->Bhabha() )  isBb= true;
    log << MSG::INFO << "DQABbTag:\t" << dqaevt->Bhabha() << endreq;
    //cout << "DQABbTag:\t" << dqaevt->Bhabha() << endl;

    SmartDataPtr<RecMdcKalTrackCol> kaltrkCol(eventSvc(),"/Event/Recon/RecMdcKalTrackCol");
    if (!kaltrkCol) {
        log << MSG::FATAL << "Could not find RecMdcKalTrackCol" << endreq;
        return StatusCode::FAILURE;
    }
    RecMdcKalTrackCol::iterator iter_trk = kaltrkCol->begin();
    for(; iter_trk != kaltrkCol->end(); iter_trk++) {
        if (isBb) RecMdcKalTrack::setPidType(RecMdcKalTrack::electron);
        else if (isDimu) RecMdcKalTrack::setPidType(RecMdcKalTrack::muon);
        //else return StatusCode::SUCCESS;

        double dr = (*iter_trk)->dr();
        double phi0 = (*iter_trk)->fi0();
        double kappa = (*iter_trk)->kappa();
        double dz = (*iter_trk)->dz();
        double tanl = (*iter_trk)->tanl();
        double pt = 1.0 / kappa;
        HepLorentzVector p4;
        p4.setPx(- sin(phi0) / fabs(kappa));
        p4.setPy(cos(phi0) / fabs(kappa));
        p4.setPz(tanl / fabs(kappa));

        double p3 = p4.mag();
        double mass;
        if(isBb) mass = 0.000511;
        else if(isDimu) mass = 0.105658;

        p4.setE(sqrt(p3 * p3 + mass * mass));
        p = p4.rho();

        double ecm = 3.68632;
        HepLorentzVector psip(0.011 * ecm, 0, 0.0075, ecm);
        Hep3Vector boostv = psip.boostVector();
        p4.boost(- boostv);
        p_cms = p4.rho();
        TH1 *hmom(0);
        if (isBb){
            if (m_thsvc->getHist("/DQAHist/MDC/hpTotLabBb", hmom).isSuccess()) {
                hmom->Fill(p);
            } else {
                log << MSG::ERROR << "Couldn't retrieve hpTotLabBb" << endreq;
            }
            if (m_thsvc->getHist("/DQAHist/MDC/hpTotCmsBb", hmom).isSuccess()) {
                hmom->Fill(p_cms);
            } else {
                log << MSG::ERROR << "Couldn't retrieve hpTotCmsBb" << endreq;
            }
            if (pt > 0) {
                if (m_thsvc->getHist("/DQAHist/MDC/hppLabBb", hmom).isSuccess()) {
                    hmom->Fill(p);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hppLabBb" << endreq;
                }
                if (m_thsvc->getHist("/DQAHist/MDC/hppCmsBb", hmom).isSuccess()) {
                    hmom->Fill(p);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hppCmsBb" << endreq;
                }
            } else if (pt < 0) {
                if (m_thsvc->getHist("/DQAHist/MDC/hpmLabBb", hmom).isSuccess()) {
                    hmom->Fill(p);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hpmLabBb" << endreq;
                }
                if (m_thsvc->getHist("/DQAHist/MDC/hpmCmsBb", hmom).isSuccess()) {
                    hmom->Fill(p);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hpmCmsBb" << endreq;
                }
            }
        } else if (isDimu){
            if (m_thsvc->getHist("/DQAHist/MDC/hpTotLabDimu", hmom).isSuccess()) {
                hmom->Fill(p);
            } else {
                log << MSG::ERROR << "Couldn't retrieve hpTotLabDimu" << endreq;
            }
            if (m_thsvc->getHist("/DQAHist/MDC/hpTotCmsDimu", hmom).isSuccess()) {
                hmom->Fill(p_cms);
            } else {
                log << MSG::ERROR << "Couldn't retrieve hpTotCmsDimu" << endreq;
            }
            if (pt > 0) {
                if (m_thsvc->getHist("/DQAHist/MDC/hppLabDimu", hmom).isSuccess()) {
                    hmom->Fill(p);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hppLabDimu" << endreq;
                }
                if (m_thsvc->getHist("/DQAHist/MDC/hppCmsDimu", hmom).isSuccess()) {
                    hmom->Fill(p);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hppCmsDimu" << endreq;
                }
            } else if (pt < 0) {
                if (m_thsvc->getHist("/DQAHist/MDC/hpmLabDimu", hmom).isSuccess()) {
                    hmom->Fill(p);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hpmLabDimu" << endreq;
                }
                if (m_thsvc->getHist("/DQAHist/MDC/hpmCmsDimu", hmom).isSuccess()) {
                    hmom->Fill(p);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hpmCmsDimu" << endreq;
                }
            }
        }

    }

    SmartDataPtr<RecMdcTrackCol> newtrkCol(eventSvc(), "/Event/Recon/RecMdcTrackCol");
    if(!newtrkCol){
        log << MSG::ERROR << "Could not find RecMdcTrackCol" << endreq;
        return ( StatusCode::FAILURE );
    }

    RecMdcTrackCol::iterator it_trk = newtrkCol->begin();
    for(; it_trk != newtrkCol->end(); it_trk++){
        HitRefVec gothits = (*it_trk) -> getVecHits();
        HitRefVec::iterator it_hit = gothits.begin();
        for(; it_hit != gothits.end(); it_hit++){

            //cout << __LINE__ << endl;
            //cout << __LINE__ << endl;
            double lr = (*it_hit) -> getFlagLR();
            if(-1 == lr) lr = 0;	// definition not same as MdcRecHit
            double resilrInc;
            double resilrExc;
            //for boss before 650
            double docaInc = (*it_hit) -> getDoca();
            double docaExc = docaInc;
            //boss650
            //double docaInc = (*it_hit) -> getDocaIncl();
            //double docaExc = (*it_hit) -> getDocaExcl();
            double dmeas;
            if(0 == lr){
                dmeas = (*it_hit) -> getDriftDistLeft();
            }else {
                dmeas = (*it_hit) -> getDriftDistRight();
            }
            // the following is for cm to mm
            double df = 10.0;
            docaInc *= df;
            docaExc *= df;
            dmeas *= df;
            double resiInc = fabs(dmeas) - fabs(docaInc);
            if( 0 == lr )     resilrInc = -1.0 * resiInc;
            else              resilrInc = resiInc;

            double resiExc = fabs(dmeas) - fabs(docaExc);
            if( 0 == lr )     resilrExc = -1.0 * resiExc;
            else              resilrExc = resiExc;

            double resilrEva = 0.5 * (resilrInc + resilrExc);
            //cout << "resilrInc = " << resilrInc << ", resilrExc = " << resilrExc << ", resilrEva = " << resilrEva << endl;
            TH1 *htmp(0);
            if (isBb){
                if (m_thsvc->getHist("/DQAHist/MDC/hresAllIncBb", htmp).isSuccess()) {
                    htmp->Fill(resilrInc);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hresAllIncBb" << endreq;
                }
                if (m_thsvc->getHist("/DQAHist/MDC/hresAllExcBb", htmp).isSuccess()) {
                    htmp->Fill(resilrExc);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hresAllExcBb" << endreq;
                }     
                if (m_thsvc->getHist("/DQAHist/MDC/hresAllEvaBb", htmp).isSuccess()) {
                    htmp->Fill(resilrEva);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hresAllEvaBb" << endreq;
                }     

            } else if (isDimu){
                if (m_thsvc->getHist("/DQAHist/MDC/hresAllIncDimu", htmp).isSuccess()) {
                    htmp->Fill(resilrInc);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hresAllIncDimu" << endreq;
                }
                if (m_thsvc->getHist("/DQAHist/MDC/hresAllExcDimu", htmp).isSuccess()) {
                    htmp->Fill(resilrExc);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hresAllExcDimu" << endreq;
                }     
                if (m_thsvc->getHist("/DQAHist/MDC/hresAllEvaDimu", htmp).isSuccess()) {
                    htmp->Fill(resilrEva);
                } else {
                    log << MSG::ERROR << "Couldn't retrieve hresAllEvaDimu" << endreq;
                }     
            }
        }
    }

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_MDC::finalize() {

    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "in finalize()" << endmsg;
    log << MSG::INFO << "Finalize successfully! " << endmsg;

    return StatusCode::SUCCESS;
}


