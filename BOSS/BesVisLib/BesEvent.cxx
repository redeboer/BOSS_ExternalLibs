//
// BesEvent.cxx
//

#include <TGeoHelix.h>
#include <TMath.h>
#include <TGeoTube.h>
#include <iostream>

#include "BesVisLib/BesEvent.h"
#include "BesVisLib/BesVisDisplay.h"
#include "BesVisLib/BesGeometry.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "Identifier/TofID.h"
#include "Identifier/EmcID.h"
#include "Identifier/MucID.h"

BesEvent *gEvent = 0;

using namespace std;

#ifndef __CINT__
ClassImp(BesEvent)
#endif

//__________________________________________________________
// BesEvent
// BesVis Event
//
//
BesEvent::BesEvent() : TObject() {
    //
    // BesEvent default constructor

    // Init
    f_Magnetic = 1.0;

    fMdcTrackCol = new TObjArray();
    fTofTrackCol = new TObjArray();
    fEmcTrackCol = new TObjArray();
    fMucTrackCol = new TObjArray();
    fExtTrackCol = new TObjArray();
}

//__________________________________________________________

BesEvent::~BesEvent() {
    //
    // BesEvent default destructor

    // Delete all array elements
    Delete();
}

//__________________________________________________________

void BesEvent::Delete(Option_t *option) {
    //
    // Clear event
    TString opt = option;
    opt.ToUpper();

    if (fMdcTrackCol) {
        for (int j = 0; j < fMdcTrackCol->GetEntries(); j++){
            delete fMdcTrackCol->At(j);
        }
        fMdcTrackCol->Clear("C");
        delete fMdcTrackCol;
    }
    if (fTofTrackCol) {
        for (int j = 0; j < fTofTrackCol->GetEntries(); j++){
            delete fTofTrackCol->At(j);
        }
        fTofTrackCol->Clear("C");
        delete fTofTrackCol;
    }
    if (fEmcTrackCol) {
        for (int j = 0; j < fEmcTrackCol->GetEntries(); j++){
            delete fEmcTrackCol->At(j);
        }
        fEmcTrackCol->Clear("C");
        delete fEmcTrackCol;
    }
    if (fMucTrackCol) {
        for (int j = 0; j < fMucTrackCol->GetEntries(); j++){
            delete fMucTrackCol->At(j);
        }
        fMucTrackCol->Clear("C");
        delete fMucTrackCol;
    }
    if (fExtTrackCol) {
        for (int j = 0; j < fExtTrackCol->GetEntries(); j++){
            delete fExtTrackCol->At(j);
        }
        fExtTrackCol->Clear("C");
        delete fExtTrackCol;
    }
}

//__________________________________________________________

void BesEvent::Clear(Option_t *option) {
    //
    // Clear event
    TString opt = option;
    opt.ToUpper();

    fDigiEvent->Clear("C");
}

//__________________________________________________________
//Long Peixun's update: add parameter "isRec"
void BesEvent::SetEvent(TDigiEvent *digiEvent, TDisTrack *recEvent, TEvtHeader *evtHeader, TRecEvTime *recEvTime, Bool_t isRec)
{
    //fDigiEvent = 0;
    fDigiEvent = digiEvent;
    fEvtHeader = evtHeader;
    fRecEvTime = recEvTime;
    //fTrigEvent = trigEvt;
    fTrigEvent = 0;
    // ======================================== 
    // set trig information
    // ======================================== 
    if (fTrigEvent != 0){
      vector<Int_t> trigConditionVector;
      vector<Int_t> trigChannelVector;

      const TTrigData* trigData = fTrigEvent->getTrigData();
      const Int_t timeType = trigData->getTimingType();
      //std::cout << "BesEvent::timing type: " << timeType << std::endl;
      Int_t trigCondition;
      Int_t trigChannel;
      for (Int_t i = 0; i < 48; i++){
        trigCondition = trigData->getTrigCondition(i);
        if (trigCondition) trigConditionVector.push_back(i);
        //std::cout << "BesEvent::trig condition: " << trigCondition << std::endl;
      }
      for (Int_t i = 0; i < 16; i++){
        trigChannel = trigData->getTrigChannel(i);
        if (trigChannel) trigChannelVector.push_back(i);
        //std::cout << "BesEvent::trigChannel: " << trigChannel << std::endl;
      }
      fEventHeader.SetEventTrig(timeType, trigConditionVector, trigChannelVector);
    }
    else {
      std::cout << "fTrigEvent does not exit!" << std::endl;
    }
    // ======================================== 
    // set time information
    // ======================================== 
    if (fEvtHeader != 0){
        time_t now;
        //now = time(NULL);
        now = (time_t)evtHeader->time();
        //char str_time[256];
        //sprintf(str_time, ctime(&now));
        //cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
        //cout << "time now: " << str_time << endl;
        //cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

        struct tm *local_time;
        local_time = localtime(&now);
        //cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
        //cout << "time now sec: " << local_time->tm_sec << endl;
        //cout << "time now min: " << local_time->tm_min << endl;
        //cout << "time now hour: "<< local_time->tm_hour << endl;
        //cout << "time now day: " << local_time->tm_mday << endl;
        //cout << "time now mon: " << local_time->tm_mon << endl;
        //cout << "time now year: "<< local_time->tm_year << endl;
        //cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

        Int_t time1 = (1900 + local_time->tm_year) * 10000 + (1 + local_time->tm_mon) * 100
                      + local_time->tm_mday;
        Int_t time2 = local_time->tm_hour * 10000 + local_time->tm_min * 100
                      + local_time->tm_sec;

        fEventHeader.SetEventHeaderGeneral(fEvtHeader->getRunId(),
                                           fEvtHeader->getEventId(), time1, time2);
    }
    else {
        cout << "fEvtHeader does not exit" << endl;
    }

    // ======================================== 
    // set event start time information
    // ======================================== 
    //Long Peixun's update: if this event is not a rec event, don't show estime, status and quality
    if (fRecEvTime != 0)
    {
        fEventHeader.SetEventRec(isRec);
        if (isRec)
            fEventHeader.SetEventEvTime(fRecEvTime->estime(),fRecEvTime->status(),fRecEvTime->quality());
        else
            fEventHeader.SetEventEvTime(0.0, 0, 0.0);
    }
    else
    {
        cout << "fRecEvTime does not exit" << endl;
        fEventHeader.SetEventRec(false);
        fEventHeader.SetEventEvTime(0.0, 0, 0.0);
    }

    // ======================================== 
    // set Mdc information
    // ======================================== 
    Double_t p = 0.0, pt = 0.0, px = 0.0, py = 0.0, pz = 0.0;
    for (Int_t i = 0; i < GetMdcTrackNum(recEvent); i++) {
        p  += GetMdcTrack(i,recEvent)->p();
        pt += GetMdcTrack(i,recEvent)->pxy();
        px += GetMdcTrack(i,recEvent)->px();
        py += GetMdcTrack(i,recEvent)->py();
        pz += GetMdcTrack(i,recEvent)->pz();
    }
    fEventHeader.SetEventMdc(p, pt, px, py, pz);

    // ======================================== 
    // set Emc information
    // ======================================== 
    Double_t t = 9999.0;
    Double_t e = 0.0;
    for (Int_t i = 0; i < GetEmcShowerNum(recEvent); i++) {
        e += GetEmcShower(i,recEvent)->energy();
    }
    fEventHeader.SetEventEmc(e);
    // ======================================== 
    // set digi and rec 
    // ======================================== 
    if (fDigiEvent != 0){
        SetHits();
        fEventHeader.SetEventMC(fDigiEvent->getFromMc());
    }
    if (recEvent != 0)
        SetTracks(recEvent);
}

//__________________________________________________________

void BesEvent::SetHits() {
    if (gBesGeometry) {
        gBesGeometry->GetMdcROOTGeo()->SetHits();
        gBesGeometry->GetTofROOTGeo()->SetHits();
        gBesGeometry->GetEmcROOTGeo()->SetHits();
        gBesGeometry->GetMucROOTGeo()->SetHits();
    }
}

//__________________________________________________________

void BesEvent::SetTracks(TDisTrack *recEvent) {
    if (gBesGeometry)
    {
        //Long Peixun's update: Add judging detector geometry code
        SetMdcTracks(recEvent);
        SetTofTracks(recEvent);
        SetEmcShowers(recEvent);
        SetMucTracks(recEvent);
        SetExtTracks(recEvent);
    }
}

//__________________________________________________________

void BesEvent::SetMdcTracks(TDisTrack *recEvent) {
    if (fMdcTrackCol){
        for (int j = 0; j < fMdcTrackCol->GetEntries(); j++){
            delete fMdcTrackCol->At(j);
        }
        fMdcTrackCol->Clear("C");
    }
    int mdc_no = recEvent->getMdcTrackNum();
    for (Int_t i = 0; i < mdc_no; i++) {
        const TRecMdcTrack* recTrack =recEvent->getRecMdcTrack(i);
        BesGeoTrack *mdcTrack = new BesGeoTrack();
        mdcTrack->SetTrackType(0);

        ConstructMdcTrackFromRec(mdcTrack, recTrack,recEvent);
        fMdcTrackCol->Add(mdcTrack);
    }
}

//__________________________________________________________

void BesEvent::SetTofTracks(TDisTrack *recEvent) {
    if (fTofTrackCol) {
        for (int j = 0; j < fTofTrackCol->GetEntries(); j++){
            delete fTofTrackCol->At(j);
        }
      fTofTrackCol->Clear("C");
    }
    int tof_no = recEvent->getTofTrackNum();

    for (Int_t i = 0; i < tof_no; i++) {
        const TRecTofTrack* recTrack = recEvent->getTofTrack(i);
        if ( !Is_tofCounter( recTrack->status())) continue;
        BesGeoTrack *tofTrack = new BesGeoTrack();
        tofTrack->SetTrackType(1);

        ConstructTofTrackFromRec(tofTrack, recTrack,recEvent);
        fTofTrackCol->Add(tofTrack);
    }
}

//__________________________________________________________

void BesEvent::SetEmcShowers(TDisTrack *recEvent) {
    if (fEmcTrackCol) {
        for (int j = 0; j < fEmcTrackCol->GetEntries(); j++){
            delete fEmcTrackCol->At(j);
        }
      fEmcTrackCol->Clear("C");
    }
    int emc_no = recEvent->getEmcShowerNum();

    for (Int_t i = 0; i < emc_no; i++) {
        const TRecEmcShower* recShower =recEvent->getEmcShower(i);
        BesGeoTrack *emcTrack = new BesGeoTrack();
        emcTrack->SetTrackType(2);

        ConstructEmcTrackFromRec(emcTrack, recShower);
        fEmcTrackCol->Add(emcTrack);
    }
}

//__________________________________________________________

void BesEvent::SetMucTracks(TDisTrack *recEvent) {
    if (fMucTrackCol) {
        for (int j = 0; j < fMucTrackCol->GetEntries(); j++){
            delete fMucTrackCol->At(j);
        }
      fMucTrackCol->Clear("C");
    }
    int muc_no = recEvent->getMucTrackNum();
    for (Int_t i = 0; i < muc_no; i++) {
        const TRecMucTrack* recTrack =recEvent->getMucTrack(i);
        BesGeoTrack *mucTrack = new BesGeoTrack();
        mucTrack->SetTrackType(3);

        ConstructMucTrackFromRec(mucTrack, recTrack);
        fMucTrackCol->Add(mucTrack);
    }
}

//__________________________________________________________

void BesEvent::SetExtTracks(TDisTrack *recEvent) {
    //if (fExtTrackCol) fExtTrackCol->Clear("C");

    //for (Int_t i = 0; i < GetExtTrackNum(); i++) {
    //    const TExtTrack* recTrack = GetExtTrack(i);
    //    BesGeoTrack *extTrack = new BesGeoTrack();
    //    extTrack->SetTrackType(4);

    //    ConstructExtTrackFromRec(extTrack, recTrack);
    //    fExtTrackCol->Add(extTrack);
    //}
}

//__________________________________________________________

void BesEvent::ConstructMdcTrackFromRec(BesGeoTrack *mdcTrack, const TRecMdcTrack *recTrack,TDisTrack *fRecEvent) {

    Double_t field  = -f_Magnetic;   // Tesla
    Double_t kvC    = 3.0e8; // light speed
    Int_t    charge = recTrack->charge();
    Double_t pt     = recTrack->pxy(); // GeV
    Double_t pz     = recTrack->pz();  // GeV
    Double_t pi     = TMath::Pi();

    // unit is "cm"
    Double_t orgx = recTrack->x()*10;
    Double_t orgy = recTrack->y()*10;
    Double_t orgz = recTrack->z()*10;

    Double_t mdcR   = ((TGeoTube*)gBesGeometry->GetMdcROOTGeo()->GetVolumeMdc()->GetShape())->GetRmax();
    Double_t mdcZ   = ((TGeoTube*)gBesGeometry->GetMdcROOTGeo()->GetVolumeMdc()->GetShape())->GetDz();

    Double_t radius,zStep;
    if (charge == 0) {
        radius = 1e9 ;    // cosmic ray test
        zStep = 1e9;
    }
    else {
        radius = (pt * 1.0e9 / kvC * 1e3) / fabs(charge * field) ;
        zStep = 2*pi*radius * fabs(pz/pt);
    }
    Double_t curvature = 1.0/radius;
    Double_t step = 10.0; // mm
    Double_t delt = step*(1.0e-3)/kvC;

    mdcTrack->AddPoint(orgx, orgy, orgz, 0.0);
    const Double_t *p;
    Int_t nStep = 0;

    Double_t x,y,z,t;
    x = orgx;
    y = orgy;
    z = orgz;

    if (charge == 0){  //cosmic ray test
        do {
            x = recTrack->helix(0)* 10 *
                cos(recTrack->helix(1))
                - nStep * step * sin(recTrack->helix(1));
            y = recTrack->helix(0)* 10 *
                sin(recTrack->helix(1))
                + nStep * step * cos(recTrack->helix(1));
            z = recTrack->helix(3)* 10 +
                nStep * step * recTrack->helix(4);

            mdcTrack->AddPoint(x, y, z, delt*nStep);
            Double_t mp[3];
            mp[0] = 0;
            mp[1] = 0;
            mp[2] = 0;
            mdcTrack->PaintMarker(mp);
            mdcTrack->SetMarkerColor(kBlack);
            mdcTrack->SetMarkerSize(10);
            mdcTrack->SetLineColor(kBlack);

            nStep++;
        }
        while ( (x*x + y*y) < mdcR*mdcR && fabs(z) < mdcZ );

        nStep = 0;
        do {
            x = recTrack->helix(0)* 10 *
                cos(recTrack->helix(1))
                - nStep * step * sin(recTrack->helix(1));
            y = recTrack->helix(0)* 10 *
                sin(recTrack->helix(1))
                + nStep * step * cos(recTrack->helix(1));
            z = recTrack->helix(3)* 10 +
                nStep * step * recTrack->helix(4);

            mdcTrack->AddPoint(x, y, z, delt*nStep);
            Double_t mp[3];
            mp[0] = 0;
            mp[1] = 0;
            mp[2] = 0;
            mdcTrack->PaintMarker(mp);
            mdcTrack->SetMarkerColor(kBlack);
            mdcTrack->SetMarkerSize(10);
            mdcTrack->SetLineColor(kRed);

        }
        while ( (x*x + y*y) < mdcR*mdcR && fabs(z) < mdcZ );
    }
    else{    //normal track
        TGeoHelix helix(curvature, zStep, charge);
        helix.InitPoint(orgx, orgy, orgz);

        helix.InitDirection(recTrack->px(), recTrack->py(), recTrack->pz(), kFALSE);
        //helix.InitDirection(1000, 10, 10, kFALSE);
        helix.SetField(0.0, 0.0, field, kFALSE);

        do {
            // helix Step before GetCurrentPoint, or first point will be wrong
            helix.Step(step);
            p = helix.GetCurrentPoint();

            mdcTrack->AddPoint(p[0], p[1], p[2], delt*nStep);
            Double_t mp[3];
            mp[0] = p[0];
            mp[1] = p[1];
            mp[2] = p[2];
            mdcTrack->PaintMarker(mp);
            mdcTrack->SetMarkerColor(kBlack);
            mdcTrack->SetMarkerSize(10);
            nStep++;
        }
        while ( (p[0]*p[0] + p[1]*p[1]) < mdcR*mdcR && fabs(p[2]) < mdcZ );

    }

    // Add associated mdc hits
    vector<UInt_t> vecHits(0);
    const TObjArray *recMdcHitCol = fRecEvent->getRecMdcHitCol();
    for (Int_t i = 0; i < recMdcHitCol->GetEntriesFast(); i++){
        TRecMdcHit *recMdcHit = (TRecMdcHit*)recMdcHitCol->At(i);
        Int_t recHitId = recMdcHit->getTrkId();
        Int_t recTrkId = recTrack->trackId();
        if (recHitId == recTrkId) vecHits.push_back(recMdcHit->getMdcId());
    }
    for (Int_t i = 0; i < (Int_t)vecHits.size(); i++) {
        Identifier aMdcID( vecHits[i] );
        int layer = MdcID::layer( aMdcID );
        int wire  = MdcID::wire( aMdcID );

        Mdc2DWire* aHit = gBesGeometry->GetMdcROOTGeo()->Get2DWire(layer, wire);
        mdcTrack->AddHit( aHit );
        //Long Peixun's update: for Mdc 3D track display
        TGeoPhysicalNode* pHit = gBesGeometry->GetMdcROOTGeo()->GetPhysicalReplica(layer, wire);
        mdcTrack->Add3DHit(pHit);
    }

    mdcTrack->SetCharge(recTrack->charge());

    // Add mdc track info
    char data[200];     //Long Peixun's update: 100 -> 200
    TString info;

    info = TString("MdcTrack ");
    info += recTrack->trackId();
    mdcTrack->AddInfo( info );

    sprintf(data, "charge=%i, status=%i", recTrack->charge(), recTrack->stat());
    mdcTrack->AddInfo( TString(data) );

    sprintf(data, "P=%-.3f GeV, Pt=%-.3f GeV", recTrack->p(), recTrack->pxy());
    mdcTrack->AddInfo( TString(data) );

    //sprintf(data, "Pt= %-.3f GeV", recTrack->pxy());
    //mdcTrack->AddInfo( TString(data) );

    sprintf(data, "Pxyz=(%-.3f,%-.3f,%-.3f) GeV", recTrack->px(),recTrack->py(),recTrack->pz());
    mdcTrack->AddInfo( TString(data) );

    sprintf(data, "helix(%-.3f,%-.3f,%-.3f,%-.3f,%-.3f)", recTrack->helix(0),recTrack->helix(1),recTrack->helix(2),recTrack->helix(3), recTrack->helix(4));
    mdcTrack->AddInfo( TString(data) );

    sprintf(data, "Origin (%-.3f, %-.3f, %-.3f) cm", orgx/10, orgy/10, orgz/10); //recTrack->x(), recTrack->y(), recTrack->z());
    mdcTrack->AddInfo( TString(data) );

    sprintf(data, "#phi=%-.3f #theta=%-.3f cos#theta=%-.3f", recTrack->phi(),recTrack->theta(),cos(recTrack->theta()));
    mdcTrack->AddInfo( TString(data) );


    sprintf(data, "nHit=%i, #chi^{2}= %-.3f",recTrack->ndof()+5, recTrack->chi2());
    mdcTrack->AddInfo( TString(data) );

    // sprintf(data, "ndof= %i, rms= %-.3f", recTrack->ndof(), recTrack->rms());
    // mdcTrack->AddInfo( TString(data) );

    // sprintf(data, "extrp= %-.3f, pathL= %-.3f", recTrack->extrp(), recTrack->pathLength());
    // mdcTrack->AddInfo( TString(data) );

    //sprintf(data, "nHits= %i, nSter= %i", recTrack->getNhits(), recTrack->nster());
    //mdcTrack->AddInfo( TString(data) );   //Long Peixun's update: this line should be annotated

    mdcTrack->CloseInfo();
}

//__________________________________________________________

//void BesEvent::ConstructTofTrackFromRec(BesGeoTrack *tofTrack, const TRecETofTrack *recETrack){
//
//    // Add associated tof hits
//    //vector<Int_t> vecBHits = recTrack->vecBHits();
//    //vector<Int_t> vecEHits = recTrack->vecEHits();
//    //vector<Int_t> vecHits;
//    //for (Int_t i = 0; i < (Int_t)vecBHits.size(); i++)
//    //    vecHits.push_back( vecBHits[i] );
//    //for (Int_t i = 0; i < (Int_t)vecEHits.size(); i++)
//    //    vecHits.push_back( vecEHits[i] );
//
//    vector<Int_t> vecHits(0);
//    const TObjArray *recETofTrackCol = fRecEvent->getETofTrackCol();
//    for (Int_t i = 0; i < recETofTrackCol->GetEntriesFast(); i++){
//        TRecETofTrack *recHit = (TRecETofTrack*)recETofTrackCol->At(i);
//        if ( !Is_tofCounter( recHit->status())) continue;
//
//        Int_t recHitId = recHit->trackID();
//        Int_t recTrkId = recETrack->trackID();
//        if (recHitId == recTrkId) vecHits.push_back(recHit->tofID());
//    }
//
//    TGeoPhysicalNode *phyNode = 0;
//    Double_t x=0.0, y=0.0, z=0.0;
//    Int_t nHits = vecHits.size();
//    for (Int_t i = 0; i < nHits; i++) {
//        Identifier aTofID( vecHits[i] );
//
//        int part;
//        if (recETrack->tofID() < 48) part   = 0;
//        else part = 2;
//
//        int layer  = TofID::layer( aTofID );
//        int scin   = TofID::phi_module( aTofID );
//
//        Tof2DScin* aHit = gBesGeometry->GetTofROOTGeo()->Get2DScin(part, layer, scin);
//        tofTrack->AddHit( aHit );
//
//        // calculate average position
//        Double_t local[3] = {0.0, 0.0, 0.0};
//        Double_t master[3] = {0.0, 0.0, 0.0};
//        phyNode = gBesGeometry->GetTofROOTGeo()->GetPhysicalScin(part, layer, scin);
//        if (phyNode)
//            phyNode->GetMatrix(-1*phyNode->GetLevel())->LocalToMaster(local, &master[0]);
//        // transform to top
//        x += master[0];
//        y += master[1];
//        z += master[2];
//    }
//    x /= nHits;
//    y /= nHits;
//    z /= nHits;
//
//    //if (recTrack->getPart() == 1) {
//    //		z = 0.5*(recTrack->getZHit1() + recTrack->getZHit2());
//    //}
//    //else {
//    Double_t curR = TMath::Sqrt(x*x+y*y);
//    Double_t newR = recETrack->zrhit();
//    if (curR != 0.0) {
//        x = x * (newR/curR);
//        y = y * (newR/curR);
//    }
//
//
//    tofTrack->SetMarker(x, y, z);
//
//    // Add tof track info
//    char data[100];
//    TString info;
//
//    info = TString("TofTrack ");
//    info += recETrack->trackID();
//    tofTrack->AddInfo(info);
//
//    //sprintf(data, "Part = %i, Qual1 = %i, Qual2 = %i", recTrack->getPart(), recTrack->getQual1(), recTrack->getQual2());
//    //tofTrack->AddInfo( TString(data) );
//
//    //sprintf(data, "Tof   = (%-.3f #pm %-.3f) ns", recTrack->getTof(), recTrack->getETof());
//    //tofTrack->AddInfo( TString(data) );
//
//    //sprintf(data, "Tof1 = (%-.3f #pm %-.3f) ns", recTrack->getTof1(), recTrack->getETof1());
//    //tofTrack->AddInfo( TString(data) );
//
//    //sprintf(data, "Tof2 = (%-.3f #pm %-.3f) ns", recTrack->getTof2(), recTrack->getETof2());
//    //tofTrack->AddInfo( TString(data) );
//
//    //sprintf(data, "Path(1=%-.2f, 2=%-.2f) mm", recTrack->getPath1(), recTrack->getPath2());
//    //tofTrack->AddInfo( TString(data) );
//
//    //sprintf(data, "Pulse(1=%-.0f, 2=%-.0f)", recTrack->getPh1(), recTrack->getPh2());
//    //tofTrack->AddInfo( TString(data) );
//
//    //sprintf(data, "ZHit(1=%-.2fmm, 2=%-.2f)mm", recTrack->getZHit1(), recTrack->getZHit2());
//    //tofTrack->AddInfo( TString(data) );
//
//    ////sprintf(data, "ZTDC1=%-.2fmm, ZTDC2=%-.2fmm", recTrack->getZTDC1(), recTrack->getZTDC2());
//    ////tofTrack->AddInfo( TString(data) );
//
//    //sprintf(data, "RHit = %-.3f, Beta = %-.3f", recTrack->getRHit(), recTrack->getBeta());
//    //tofTrack->AddInfo( TString(data) );
//
//    //sprintf(data, "w_e = %-.2f%%, w_mu = %-.2f%%", recTrack->getWeightElectron()*100.0, recTrack->getWeightMuon()*100.0);
//    //tofTrack->AddInfo( TString(data) );
//
//    //sprintf(data, "w_pi = %-.2f%%, w_K = %-.2f%%", recTrack->getWeightPion()*100.0, recTrack->getWeightKaon()*100.0);
//    //tofTrack->AddInfo( TString(data) );
//
//    //sprintf(data, "w_P = %-.2f%%", recTrack->getWeightProton()*100.0);
//    //tofTrack->AddInfo( TString(data) );
//
//    tofTrack->CloseInfo();
//}


//__________________________________________________________
void BesEvent::ConstructTofTrackFromRec(BesGeoTrack *tofTrack,
                                        const TRecTofTrack *recTrack,
                                        TDisTrack *fRecEvent){

    vector<Int_t> vecBHits(0);
    vector<Int_t> vecEHits(0);

    const TObjArray *recTofTrackCol = fRecEvent->getTofTrackCol();
    for (Int_t i = 0; i < recTofTrackCol->GetEntriesFast(); i++){
        TRecTofTrack *recHit = (TRecTofTrack*)recTofTrackCol->At(i);
        if ( !Is_tofCounter( recHit->status())) continue;

        Int_t recHitId = recHit->trackID();
        Int_t recTrkId = recTrack->trackID();
        if (recHitId == recTrkId) {
            if ( Is_tofBarrel( recHit->status() ) ) {
                vecBHits.push_back(recHit->tofID());
            }
            else {
                vecEHits.push_back(recHit->tofID());
            }
        }
    }

    TGeoPhysicalNode *phyNode = 0;
    Double_t x=0.0, y=0.0, z=0.0;

    Int_t nHits;
    if (vecBHits.size()){
        nHits = vecBHits.size();
        for (Int_t i = 0; i < nHits; i++) {

            int part = 1;
            int layer = 0;
            int scin = 0;
            if ( ( vecBHits[i] >= 0 ) && ( vecBHits[i] <= 87 ) ) {
                layer = 0;
                scin  = vecBHits[i];
            }
            else {
                layer = 1;
                scin  = vecBHits[i] - 88;
            }

            Tof2DScin* aHit = gBesGeometry->GetTofROOTGeo()->Get2DScin(part, layer, scin);
            tofTrack->AddHit( aHit );
            
            // calculate average position
            Double_t local[3] = {0.0, 0.0, 0.0};
            Double_t master[3] = {0.0, 0.0, 0.0};
            phyNode = gBesGeometry->GetTofROOTGeo()->GetPhysicalScin(part, layer, scin);
            tofTrack->Add3DHit(phyNode);   //Long Peixun's update: for Tof 3D track display
            if (phyNode)
                phyNode->GetMatrix(-1*phyNode->GetLevel())->LocalToMaster(local, &master[0]);
            // transform to top
            x += master[0];
            y += master[1];
            z += master[2];
        }
        x /= nHits;
        y /= nHits;
        z /= nHits;

        z = recTrack->zrhit();

        tofTrack->SetMarker(x, y, z);
    }

    else if (vecEHits.size()){
        nHits = vecEHits.size();
        for (Int_t i = 0; i < nHits; i++) {

            int part = 0;
            int layer = 0;
            int scin = 0;
            if ( ( vecEHits[i] >= 0 ) && ( vecEHits[i] <= 47 ) ) {
                part = 2;
                scin  = vecEHits[i];
            }
            else {
                part = 0;
                scin  = vecEHits[i] - 48;
            }


            Tof2DScin* aHit = gBesGeometry->GetTofROOTGeo()->Get2DScin(part, layer, scin);
            tofTrack->AddHit( aHit );

            // calculate average position
            Double_t local[3] = {0.0, 0.0, 0.0};
            Double_t master[3] = {0.0, 0.0, 0.0};
            phyNode = gBesGeometry->GetTofROOTGeo()->GetPhysicalScin(part, layer, scin);
            tofTrack->Add3DHit(phyNode);   //Long Peixun's update: for Tof 3D track display
            if (phyNode)
                phyNode->GetMatrix(-1*phyNode->GetLevel())->LocalToMaster(local, &master[0]);
            // transform to top
            x += master[0];
            y += master[1];
            z += master[2];
        }
        x /= nHits;
        y /= nHits;
        z /= nHits;

        tofTrack->SetMarker(x, y, z);
    }

    // Add tof track info
    char data[100];
    TString info;

    info = TString("TofTrack ");
    info += recTrack->trackID();
    tofTrack->AddInfo(info);

    //sprintf(data, "Part = %i, Qual1 = %i, Qual2 = %i", recTrack->getPart(), recTrack->getQual1(), recTrack->getQual2());
    //tofTrack->AddInfo( TString(data) );

    //sprintf(data, "Tof   = (%-.3f #pm %-.3f) ns", recTrack->getTof(), recTrack->getETof());
    //tofTrack->AddInfo( TString(data) );

    //sprintf(data, "Tof1 = (%-.3f #pm %-.3f) ns", recTrack->getTof1(), recTrack->getETof1());
    //tofTrack->AddInfo( TString(data) );

    //sprintf(data, "Tof2 = (%-.3f #pm %-.3f) ns", recTrack->getTof2(), recTrack->getETof2());
    //tofTrack->AddInfo( TString(data) );

    //sprintf(data, "Path(1=%-.2f, 2=%-.2f) mm", recTrack->getPath1(), recTrack->getPath2());
    //tofTrack->AddInfo( TString(data) );

    //sprintf(data, "Pulse(1=%-.0f, 2=%-.0f)", recTrack->getPh1(), recTrack->getPh2());
    //tofTrack->AddInfo( TString(data) );

    //sprintf(data, "ZHit(1=%-.2fmm, 2=%-.2f)mm", recTrack->getZHit1(), recTrack->getZHit2());
    //tofTrack->AddInfo( TString(data) );

    ////sprintf(data, "ZTDC1=%-.2fmm, ZTDC2=%-.2fmm", recTrack->getZTDC1(), recTrack->getZTDC2());
    ////tofTrack->AddInfo( TString(data) );

    //sprintf(data, "RHit = %-.3f, Beta = %-.3f", recTrack->getRHit(), recTrack->getBeta());
    //tofTrack->AddInfo( TString(data) );

    //sprintf(data, "w_e = %-.2f%%, w_mu = %-.2f%%", recTrack->getWeightElectron()*100.0, recTrack->getWeightMuon()*100.0);
    //tofTrack->AddInfo( TString(data) );

    //sprintf(data, "w_pi = %-.2f%%, w_K = %-.2f%%", recTrack->getWeightPion()*100.0, recTrack->getWeightKaon()*100.0);
    //tofTrack->AddInfo( TString(data) );

    //sprintf(data, "w_P = %-.2f%%", recTrack->getWeightProton()*100.0);
    //tofTrack->AddInfo( TString(data) );

    tofTrack->CloseInfo();
}

void BesEvent::ConstructEmcTrackFromRec(BesGeoTrack *emcTrack, const TRecEmcShower *recTrack) {

    Double_t x=0.0, y=0.0, z=0.0;
    x = recTrack->x() * 10.0;
    y = recTrack->y() * 10.0;
    z = recTrack->z() * 10.0;

    emcTrack->SetMarker(x, y, z);
    // Add associated emc hits
    vector<Int_t> vecHits(0);
    map<Int_t, Double_t> cellMap = recTrack->cellIdMap();
    map<Int_t, Double_t>::iterator iCellMap;
    for (iCellMap = cellMap.begin(); iCellMap != cellMap.end(); iCellMap++){
        Int_t cellId = iCellMap->first;
        vecHits.push_back(cellId);
    }
    for (Int_t i = 0; i < (Int_t)vecHits.size(); i++) {
        Identifier aEmcID( vecHits[i] );
        int part  = EmcID::barrel_ec( aEmcID );
        int theta = EmcID::theta_module( aEmcID );
        int phi   = EmcID::phi_module( aEmcID );
        if (part == 1) theta = 43-theta; // 43 = m_kThetaBr-1

        Emc2DCrystal* aHit = gBesGeometry->GetEmcROOTGeo()->Get2DCrystal(part, phi, theta);
        emcTrack->AddHit( aHit );
        //Long Peixun's update: for Emc 3D track display
        TGeoPhysicalNode* pHit = gBesGeometry->GetEmcROOTGeo()->GetPhysicalCrystal(part, phi, theta);
        emcTrack->Add3DHit(pHit);
    }

    // Add Emc track info
    char data[100];
    TString info;

    info = TString("EmcShower ");
    info += recTrack->trackId();
    emcTrack->AddInfo(info);

    sprintf(data, "nHits = %i, status = %i", recTrack->numHits(), recTrack->status());
    emcTrack->AddInfo( TString(data) );

    sprintf(data, "energy= (%.2f #pm %-.2f) MeV", recTrack->energy()*1000.0, recTrack->dE()*1000.0);
    emcTrack->AddInfo( TString(data) );

    Identifier aEmcID( recTrack->cellId() );
    int part  = EmcID::barrel_ec( aEmcID );
    int theta = EmcID::theta_module( aEmcID );
    int phi   = EmcID::phi_module( aEmcID );

    sprintf(data, "cell Id= (%i, #theta %i, #phi %i)", part, theta, phi);
    emcTrack->AddInfo( TString(data) );

    sprintf(data, "module = %i", recTrack->module());
    emcTrack->AddInfo( TString(data) );

    //sprintf(data, "x = (%-.3f #pm %-.3f) mm", x, recTrack->dx());
    //emcTrack->AddInfo( TString(data) );

    //sprintf(data, "y = (%-.3f #pm %-.3f) mm", y, recTrack->dy());
    //emcTrack->AddInfo( TString(data) );

    //sprintf(data, "z = (%-.3f #pm %-.3f) mm", z, recTrack->dz());
    //emcTrack->AddInfo( TString(data) );

    sprintf(data, "#theta = (%-.3f #pm %-.3f)", recTrack->theta(), recTrack->dtheta());
    emcTrack->AddInfo( TString(data) );

    sprintf(data, "#phi = (%-.3f #pm %-.3f)", recTrack->phi(), recTrack->dphi());
    emcTrack->AddInfo( TString(data) );

//   sprintf(data, "cos (x=%-.3f, y=%-.3f, z=%-.3f)", recTrack->cosx(), recTrack->cosy(), recTrack->cosz());   //from boss6.1.0. no these varibles
//   emcTrack->AddInfo( TString(data) );

    emcTrack->CloseInfo();
}

//__________________________________________________________

void BesEvent::ConstructMucTrackFromRec(BesGeoTrack *mucTrack, const TRecMucTrack *recTrack) {
    if (recTrack->depth() <= 0.0) return;  // bad track

    Double_t field  = 1e-3;   // Tesla
    Double_t kvC    = 3.0e8; // light speed
    Int_t    charge = 1;
    Double_t pz     = recTrack->pz();  // GeV
    Double_t pt     =
        sqrt(recTrack->px()*recTrack->px() + recTrack->py()*recTrack->py()); // GeV
    Double_t pi     = TMath::Pi();

    Double_t mucR   =
        ((TGeoBBox*)gBesGeometry->GetMucROOTGeo()->GetVolumeMuc()->GetShape())->GetDX();
    Double_t mucZ   =
        ((TGeoBBox*)gBesGeometry->GetMucROOTGeo()->GetVolumeMuc()->GetShape())->GetDZ();

    Double_t radius = 1.0e+9;
    Double_t curvature = 1.0/radius;
    Double_t zStep = 2*pi*radius * fabs(pz/pt);
    Double_t step = 1.0; // mm
    Double_t delt = step*(1.0e-3)/kvC;

    TGeoHelix helix(curvature, zStep, charge);

    Double_t x = recTrack->xPos() * 10.0;
    Double_t y = recTrack->yPos() * 10.0;
    Double_t z = recTrack->zPos() * 10.0;

    helix.InitPoint(x, y, z);
    helix.InitDirection(recTrack->px(), recTrack->py(), recTrack->pz(), kFALSE);
    helix.SetField(0.0, 0.0, field, kFALSE);

    mucTrack->AddPoint(x, y, z, 0.0);
    const Double_t *p;
    Int_t nStep = 0;
    do {
        // helix Step before GetCurrentPoint, or first point will be wrong
        helix.Step(step);
        p = helix.GetCurrentPoint();
        mucTrack->AddPoint(p[0], p[1], p[2], delt*nStep);
        Double_t mp[3];
        mp[0] = p[0];
        mp[1] = p[1];
        mp[2] = p[2];
        mucTrack->PaintMarker(mp);
        mucTrack->SetMarkerColor(kBlack);
        mucTrack->SetMarkerSize(10);
        nStep++;
    }
    while ( (p[0]*p[0] + p[1]*p[1]) < mucR*mucR && fabs(p[2]) < mucZ );

    // Add associated muc hits
    vector<Int_t> vecHits = recTrack->vecHits();
    for (Int_t i = 0; i < (Int_t)vecHits.size(); i++) {
        Identifier aMucID( vecHits[i] );
        int part  = MucID::part(  aMucID );
        int seg   = MucID::seg(   aMucID );
        int gap   = MucID::gap(   aMucID );
        int strip = MucID::strip( aMucID );

        Muc2DStrip* aHit = gBesGeometry->GetMucROOTGeo()->Get2DStrip(part, seg, gap, strip);
        mucTrack->AddHit( aHit );
        //Long Peixun's update: for Muc 3D track display
        TGeoPhysicalNode* pHit = gBesGeometry->GetMucROOTGeo()->GetPhysicalStrip(part, seg, gap, strip);
        mucTrack->Add3DHit(pHit);
    }

    mucTrack->SetCharge(charge);

    // Add Muc track info
    char data[100];
    TString info;

    info = TString("MucTrack ");
    info += recTrack->trackId();
    mucTrack->AddInfo(info);

    sprintf(data, "nHits= %i, maxHits= %i, nLayers= %i", recTrack->numHits(), recTrack->maxHitsInLayer(), recTrack->numLayers());
    mucTrack->AddInfo( TString(data) );

    sprintf(data, "lastLayer (br= %i, ec= %i)", recTrack->brLastLayer(), recTrack->ecLastLayer());
    mucTrack->AddInfo( TString(data) );

    sprintf(data, "depth = %.3f cm", recTrack->depth());
    mucTrack->AddInfo( TString(data) );

    sprintf(data, "#chi^{2}= %-.3f, dof= %i, rms= %-.3f", recTrack->chi2(), recTrack->dof(), recTrack->rms());
    mucTrack->AddInfo( TString(data) );

    sprintf(data, "Origin (%-.2f, %-.2f, %-.2f) cm", recTrack->xPos(), recTrack->yPos(), recTrack->zPos());
    mucTrack->AddInfo( TString(data) );

    sprintf(data, "p (%-.3f, %-.3f, %-.3f) GeV", recTrack->px(), recTrack->py(), recTrack->pz());
    mucTrack->AddInfo( TString(data) );

    mucTrack->CloseInfo();
}

//__________________________________________________________

//void BesEvent::ConstructExtTrackFromRec(BesGeoTrack *extTrack, const TExtTrack *recTrack) {
//
//    extTrack->AddPoint(recTrack->GetTof1PositionX(),
//                       recTrack->GetTof1PositionY(),
//                       recTrack->GetTof1PositionZ(),
//                       0.0);
//    extTrack->AddPoint(recTrack->GetTof2PositionX(),
//                       recTrack->GetTof2PositionY(),
//                       recTrack->GetTof2PositionZ(),
//                       0.1);
//    extTrack->AddPoint(recTrack->GetEmcPositionX(),
//                       recTrack->GetEmcPositionY(),
//                       recTrack->GetEmcPositionZ(),
//                       0.2);
//    extTrack->AddPoint(recTrack->GetMucPositionX(),
//                       recTrack->GetMucPositionY(),
//                       recTrack->GetMucPositionZ(),
//                       0.3);
//
//    // Add Ext track info
//    char data[100];
//    TString info;
//
//    info = TString("ExtTrack ");
//    info += recTrack->GetTrackId();
//    extTrack->AddInfo(info);
//
//    sprintf(data, "Tof1 pos (%-.2f, %-.2f, %-.2f) mm",
//            recTrack->GetTof1PositionX(), recTrack->GetTof1PositionY(), recTrack->GetTof1PositionZ());
//    extTrack->AddInfo( TString(data) );
//
//    sprintf(data, "Tof2 pos (%-.2f, %-.2f, %-.2f) mm",
//            recTrack->GetTof2PositionX(), recTrack->GetTof2PositionY(), recTrack->GetTof2PositionZ());
//    extTrack->AddInfo( TString(data) );
//
//    sprintf(data, "Emc pos (%-.2f, %-.2f, %-.2f) mm",
//            recTrack->GetEmcPositionX(), recTrack->GetEmcPositionY(), recTrack->GetEmcPositionZ());
//    extTrack->AddInfo( TString(data) );
//
//    sprintf(data, "Muc pos (%-.2f, %-.2f, %-.2f) mm",
//            recTrack->GetMucPositionX(), recTrack->GetMucPositionY(), recTrack->GetMucPositionZ());
//    extTrack->AddInfo( TString(data) );
//
//    sprintf(data, "Tof1 p (%-.3f, %-.3f, %-.3f) GeV",
//            recTrack->GetTof1MomentumX()/1000.0, recTrack->GetTof1MomentumY()/1000.0, recTrack->GetTof1MomentumZ()/1000.0);
//    extTrack->AddInfo( TString(data) );
//
//    sprintf(data, "Tof2 p (%-.3f, %-.3f, %-.3f) GeV",
//            recTrack->GetTof2MomentumX()/1000.0, recTrack->GetTof2MomentumY()/1000.0, recTrack->GetTof2MomentumZ()/1000.0);
//    extTrack->AddInfo( TString(data) );
//
//    sprintf(data, "Emc p (%-.3f, %-.3f, %-.3f) GeV",
//            recTrack->GetEmcMomentumX()/1000.0, recTrack->GetEmcMomentumY()/1000.0, recTrack->GetEmcMomentumZ()/1000.0);
//    extTrack->AddInfo( TString(data) );
//
//    sprintf(data, "Muc p (%-.3f, %-.3f, %-.3f) GeV",
//            recTrack->GetMucMomentumX()/1000.0, recTrack->GetMucMomentumY()/1000.0, recTrack->GetMucMomentumZ()/1000.0);
//    extTrack->AddInfo( TString(data) );
//
//    extTrack->CloseInfo();
//}


//__________________________________________________________

void BesEvent::DrawHits(Option_t *option) {
    if (gBesGeometry)
    {
        gBesGeometry->GetMdcROOTGeo()->DrawHits(option);
        gBesGeometry->GetTofROOTGeo()->DrawHits(option);
        gBesGeometry->GetEmcROOTGeo()->DrawHits(option);
        gBesGeometry->GetMucROOTGeo()->DrawHits(option);
    }
}

//__________________________________________________________

void BesEvent::DrawTracks(Option_t *option) {

    BesView *view = dynamic_cast<BesView*>(gPad->GetView());

    //Long Peixun's update: Always construct 3D line, whether draw it or not
    if (view)
    {
        for (Int_t i = 0; i < fMdcTrackCol->GetEntries(); i++)
        {
            BesGeoTrack *track = (BesGeoTrack*)fMdcTrackCol->At(i);
            if (view->GetVisTracksGlobal() && view->GetVisTracksMdc())
                track->Draw();
            else track->Construct3DLine();
        }

        for (Int_t i = 0; i < fTofTrackCol->GetEntries(); i++)
        {
            BesGeoTrack *track = (BesGeoTrack*)fTofTrackCol->At(i);
            if (view->GetVisTracksGlobal() && view->GetVisTracksTof())
                track->Draw();
            else track->Construct3DLine();
        }

        for (Int_t i = 0; i < fEmcTrackCol->GetEntries(); i++)
        {
            BesGeoTrack *track = (BesGeoTrack*)fEmcTrackCol->At(i);
            if (view->GetVisTracksGlobal() && view->GetVisTracksEmc())
                track->Draw();
            else track->Construct3DLine();
        }

        for (Int_t i = 0; i < fMucTrackCol->GetEntries(); i++)
        {
            BesGeoTrack *track = (BesGeoTrack*)fMucTrackCol->At(i);
            if (view->GetVisTracksGlobal() && view->GetVisTracksMuc())
                track->Draw();
            else track->Construct3DLine();
        }

        for (Int_t i = 0; i < fExtTrackCol->GetEntries(); i++)
        {
            BesGeoTrack *track = (BesGeoTrack*)fExtTrackCol->At(i);
            if (view->GetVisTracksGlobal() && view->GetVisTracksExt())
                track->Draw();
            else track->Construct3DLine();
        }
    }
}

//__________________________________________________________

void BesEvent::Print(Option_t *option) {
    //
    // Print event information
    //
    // Options available:
    //   ''               - quiet, i.e. only numbers of hits and tracks etc. (default)
    //   'ALL'            - very detailed output (might be hundreds of lines!)
    //   'Digi'           - only Digi information
    //   'Dst'            - only Dst information
    //
    TString opt = option;
    opt.ToUpper();
    Int_t i;

    //cout << endl
    // << "==========================================================" << endl
    // << "Run:                            " << GetRun() << endl
    // << "Event:                          " << GetEvent() << endl
    // << "----------------------------------------------------------" << endl
    // << "Mdc digis:                      " << GetMdcDigiNum() << endl
    // << "Tof digis:                      " << GetTofDigiNum() << endl
    // << "Emc digis:                      " << GetEmcDigiNum() << endl
    // << "Muc digis:                      " << GetMucDigiNum() << endl
    // << "Mdc tracks:                     " << GetMdcTrackNum() << endl
    // << "Tof tracks:                     " << "debug++++++" /*GetTofTrackNum()*/ << endl
    // << "Emc tracks:                     " << GetEmcShowerNum() << endl
    // << "Muc tracks:                     " << GetMucTrackNum() << endl
    // << "Ext tracks:                     " << GetExtTrackNum() << endl
    // << "Dedx :                          " << GetDedxNum() << endl
    // << "----------------------------------------------------------" << endl;

    if ( opt.Contains("ALL") || opt.Contains("Digi") ) {
        for ( i = 0; i < GetMdcDigiNum(); i++ ) GetMdcDigi(i)->Print();
        for ( i = 0; i < GetTofDigiNum(); i++ ) GetTofDigi(i)->Print();
        for ( i = 0; i < GetEmcDigiNum(); i++ ) GetEmcDigi(i)->Print();
        for ( i = 0; i < GetMucDigiNum(); i++ ) GetMucDigi(i)->Print();
    }

    /* if ( opt.Contains("ALL") || opt.Contains("Dst") ) {
         for ( i = 0; i < GetMdcTrackNum(); i++ ) GetMdcTrack(i)->Print();
         // Tof -> BTof ETof
         //for ( i = 0; i < GetTofTrackNum(); i++ ) GetTofTrack(i)->Print();
         for ( i = 0; i < GetEmcShowerNum(); i++ ) GetEmcShower(i)->Print();
         for ( i = 0; i < GetMucTrackNum(); i++ ) GetMucTrack(i)->Print();
         cout << "wait for TRecExtEvent, BesEvent" << endl;
         //for ( i = 0; i < GetExtTrackNum(); i++ ) GetExtTrack(i)->Print();
         for ( i = 0; i < GetDedxNum();     i++ ) GetDedx(i)->Print();
     }
    */
    //cout << "=========================================================="
    //<< endl << endl;
}

bool BesEvent::Is_tofCounter(UInt_t status){
    const unsigned int	Counter_Mask = 0x00000004;
    const unsigned int	Counter_Index = 2;
    return ((status & Counter_Mask) >> Counter_Index) ? true: false;
}

bool BesEvent::Is_tofBarrel(UInt_t status) {
    const unsigned int Barrel_Index  = 4;
    const unsigned int Barrel_Mask   = 0x00000010;
    return ((status & Barrel_Mask) >> Barrel_Index ) ? true : false;
}
