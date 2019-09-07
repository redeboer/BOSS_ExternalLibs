#include "BesVisAlg/BesVisAlg.h"
#include "RootEventData/TRecTrackEvent.h"
#include "RootEventData/TDigiEvent.h"
#include "RootEventData/TEvtHeader.h"
#include "RootEventData/TTrigEvent.h"
#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Digi/DigiCnv.h"
#include "RootCnvSvc/EvtHeaderCnv.h"
#include "RootCnvSvc/Trig/TrigCnv.h"
#include "RootCnvSvc/Trig/TrigDataCnv.h"
#include "EventModel/EventHeader.h"
#include "EventModel/Event.h"
#include "EventModel/EventModel.h"
#include <assert.h>
/////////////////////////////////////////////////////////////////////////////
int BesVisAlg::counter = 0;	// static data member

BesVisAlg::BesVisAlg(const std::string& name, ISvcLocator* pSvcLocator) :
        Algorithm(name, pSvcLocator)
{
    // Part 1: Declare the properties
    declareProperty("TestVec",testVec);
    declareProperty("OutputShareFile", f_rootOutputFile);
    declareProperty("InputGeometryFile", f_geoInputFile);
    declareProperty("DisplayMode", m_mode=0); // offline mode: 0 online mode:1
    // Part 2: Define some semaphore operate flag
    // semaphore start_value
    start_val[0] = 1;	// free-space = 1
    start_val[1] = 0;	// output-shore = 0

    // acquire semaphore, decrease resource supply
    acquire.sem_num = 0;
    acquire.sem_op = -1;
    acquire.sem_flg = SEM_UNDO;

    // release semaphore, increase resource supply
    release.sem_num = 0;
    release.sem_op = 1;
    release.sem_flg = SEM_UNDO;

    recdis = NULL;
    recTrack1 = NULL;
    mdchit = NULL;
    tofTrack = NULL;
    muctrk = NULL;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
BesVisAlg::~BesVisAlg(){
    // Part 1: Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << " BesVisAlg ~BesVisAlg()" << endreq;


    // Part 2: Delete semaphore
    //if (semctl(semid, 0, IPC_RMID, 0) == -1)
    //    log << MSG::ERROR << "Delete semaphore" << endreq;
    //else
    //    log << MSG::INFO <<  "Delete semaphore" << endreq;

    // Part 3: Delete temp file
    //if (remove(BUFFER) == -1)
    //    log << MSG::ERROR << "delete -- temp file" << endreq;
    //else
    //    log << MSG::INFO << "Delete temp file" << endreq;

}

//**************************************************************************
StatusCode BesVisAlg::initialize(){

    // Part 1: Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << " BesVisAlg initialize()" << endreq;
    StatusCode status = StatusCode::SUCCESS;

    // Part 2: Print out the property values
//############################################################
    IService* isvc = 0;
    status = serviceLocator()->service("RootCnvSvc", isvc, false);
    if ( !status.isSuccess() )
        status = serviceLocator()->service("EventCnvSvc", isvc, true);
    if ( status.isSuccess() )
        status = isvc->queryInterface(IID_IRootCnvSvc, (void**)&m_cnvSvc);
    status = hasWrite();
    if ( status.isSuccess()){
        log << MSG::INFO << "share file writed -- success"
        << endreq;
    }
    else {
        log << MSG::ERROR << "share file writed -- ERROR!!!!!"
        << endreq;
    }
//############################################################

    // Part 2: Print out the property values
    log << MSG::INFO << "  OutputFile = " << f_rootOutputFile << endreq;

    for (unsigned int i=0; i<testVec.size(); i++) {
        log << MSG::INFO << "  MyStringVec[" << i << "] = "
        << testVec[i] << endreq;
    }

    // Part 3: Initialize semphore
    m_pid = getpid();
    log << MSG::ERROR << "BesVisAlg process PID: [" << m_pid << "]"
    << endreq;

    log << MSG::ERROR << "Create and initialize semaphore" << endreq;
    if ((semid = semget((int)m_pid, 2, IPC_CREAT | IPC_EXCL | 0666)) != -1){
        arg.array = start_val;
        log << MSG::ERROR << "Semaphore ID:" << semid << endreq;

        if (semctl(semid, 0, SETALL, arg) == -1){
            log << MSG::ERROR << "semctl -- parent -- initialization"
            << endreq;
            exit(1);
        }

        if ((sem_value_F = semctl(semid, FREE_SPACE, GETVAL, 0)) == -1){
            log << MSG::ERROR << "semctl -- GETVAL" << endreq;
            exit(1);
        }

        if ((sem_value_O = semctl(semid, OUTPUT_STORE, GETVAL, 0)) == -1){
            log << MSG::ERROR << "semctl -- GETVAL" << endreq;
            exit(1);
        }
    }
    else {
        log << MSG::INFO << "semget -- parent -- creation" << endreq;
        exit(2);
    }
    //******************************
    // Debug information
    //*******************************
    //log << MSG::INFO << "semaphore ID: " << semid << endreq;
    //log << MSG::ERROR << "set init sem_value_FREE-SPACE: " << sem_value_F << endreq;
    //log << MSG::ERROR << "set init sem_value_OUTPUT-STORE: " << sem_value_O << endreq;

    // Part 4: fork consumer process
    if ((c_pid = fork()) == -1){
        log << MSG::ERROR << "fork -- consumer" << endreq;
        exit(5);
    }
    else if (c_pid == 0){
        // enter consumer logic
        log << MSG::INFO << "Parent fork consumer: ppid = " << getppid()
        << "; pid = " << getpid() << endreq;

        // convert msg to char[] and transfer them to child process
        char	msg_pid[25];
        sprintf(msg_pid, "%d", m_pid);

        const char	*msg_sleep = testVec[1].c_str();

        log << MSG::INFO << "msg_pid: " << msg_pid << endreq;
        log << MSG::INFO << "Consumer sleep time: " << msg_sleep
        << " seconds." << endreq;

        //execute besvis.exe
        execlp("besvis.exe", "besvis.exe",
               "-B", msg_pid, "-e", f_rootOutputFile.c_str(),
               "-g", f_geoInputFile.c_str(), (char *)NULL );

        //execlp("./a.out", "consumer", msg_pid, "5", (char *)NULL);
        log << MSG::ERROR << "exec -- consumer" << endreq;
        exit(6);
    }


    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode BesVisAlg::execute() {

    // Part 1: Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "BesVisAlg execute()" << endreq;

    // Part 2: Producer execute
    const char	*msg_sleep = testVec[0].c_str();
    producer(atoi(msg_sleep));

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode BesVisAlg::finalize() {

    // Part 1: Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "BesVisAlg finalize()" << endreq;

    // Part 2: Waiting consumer process accessing the data
    // and then kill it
    while (1){
        if (hasRead()) break;
        else	sleep(5);
    }

    // Part 3: Delete semaphore
    if (semctl(semid, 0, IPC_RMID, 0) == -1)
        log << MSG::ERROR << "Delete semaphore" << endreq;
    else
        log << MSG::INFO <<  "Delete semaphore" << endreq;

    int	  status;
    pid_t wait_pid;
    cout << "wait for besvis terminate" << endl;
    while ( wait_pid = waitpid(c_pid, &status, 0) ){
        log << MSG::INFO << "Wait on PID: " << c_pid << " returns status of: "
        << status << endreq;
        if (wait_pid == -1) break;
        sleep(5);
    }
    cout << "finalize" << endl;
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode BesVisAlg::producer(int p_sleep){
    // Part 1: Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "Producer work" << endreq;
    StatusCode status = StatusCode::SUCCESS;

    // Part 3: operate semaphore -- FREE_SPACE
    acquire.sem_num = FREE_SPACE;
    //++++++++++++++++++++++++++++++
    // Debug information
    //++++++++++++++++++++++++++++++
    //log << MSG::ERROR << "A -- No[" << counter << "] event "
    //<< "will generate by producer." << endreq;
    //
    //log << MSG::ERROR << "semid: " << semid << endreq;
    //
    //log << MSG::ERROR << "Before change FREE_SPACE" << endreq;
    //log << MSG::ERROR << "FREE_SPACE will change from 1 to 0" << endreq;
    //if ((sem_value_F = semctl(semid, FREE_SPACE, GETVAL, 0)) == -1){
    //    log << MSG::ERROR << "semctl: GETVAL -- FREE_SPACE"
    //    << endreq;
    //    exit(5);
    //}
    //log << MSG::ERROR << "Semaphore FREE_SPACE has value of(refer 1) " <<
    //sem_value_F << endreq;

    //if ((sem_value_O = semctl(semid, OUTPUT_STORE, GETVAL, 0)) == -1){
    //    log << MSG::ERROR << "semctl: GETVAL -- OUTPUT_STORE"
    //    << endreq;
    //    exit(5);
    //}
    //log << MSG::ERROR << "Semaphore OUTPUT_STORE has value of(refer 0) " <<
    //sem_value_O << endreq;

    //++++++++++++++++++++++++++++++
    if (semop(semid, &acquire, 1) == -1){
        log << MSG::ERROR << "Producer -- decrease -- freeSpace"
        << endreq;
        exit(2);
    }

    //++++++++++++++++++++++++++++++
    // Debug information
    //++++++++++++++++++++++++++++++
    //log << MSG::ERROR << "semid: " << semid << endreq;
    //
    //log << MSG::ERROR << "After change FREE_SPACE" << endreq;
    //log << MSG::ERROR << "FREE_SPACE has changed from 1 to 0" << endreq;
    //if ((sem_value_F = semctl(semid, FREE_SPACE, GETVAL, 0)) == -1){
    //    log << MSG::ERROR << "semctl: GETVAL -- FREE_SPACE"
    //    << endreq;
    //    exit(5);
    //}
    //log << MSG::ERROR << "Semaphore FREE_SPACE has value of(refer 0) " <<
    //sem_value_F << endreq;

    //if ((sem_value_O = semctl(semid, OUTPUT_STORE, GETVAL, 0)) == -1){
    //    log << MSG::ERROR << "semctl: GETVAL -- OUTPUT_STORE"
    //    << endreq;
    //    exit(5);
    //}
    //log << MSG::ERROR << "Semaphore OUTPUT_STORE has value of(refer 0) " <<
    //sem_value_O << endreq;
    //++++++++++++++++++++++++++++++

    //Part 4: open file and write
    // produce rand number, will be replaced by data industry
    //sleep(p_sleep);
    //srand((unsigned)m_pid);
    //int n = rand() % 99 + 1;

    //FILE		*fptr;
    //if ((fptr = fopen("./buffer", "w")) == NULL){
    //    log << MSG::ERROR << "BUFFER" << endreq;
    //    exit(3);
    //}
    //fprintf(fptr, "%d \n", n);
    //fclose(fptr);
    //++++++++++++++++++++++++++++++

//    status = hasWrite();
//    if ( status.isSuccess()){
//        log << MSG::INFO << "share file writed -- success"
//        << endreq;
//    }
//    else {
//        log << MSG::ERROR << "share file writed -- ERROR!!!!!"
//        << endreq;
//    }
//############################################################
    /*   status = collectObjects();
       if ( !status.isSuccess()){
           log <<MSG::ERROR << "collectObjects() error!" << endreq;
           return status;
       }
       */

    DataObject* obj = 0;
    IOpaqueAddress *pAddress = 0;
    //    StatusCode status;
    status = m_pDataProvider->retrieveObject("/Event", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }

    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Trig", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Trig/TrigData", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/EventHeader", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Digi", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Digi/MdcDigiCol", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Digi/TofDigiCol", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Digi/EmcDigiCol", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Digi/MucDigiCol", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Recon", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Recon/RecMdcHitCol", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Recon/RecMdcTrackCol", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Recon/MdcHOTCol", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Recon/RecMdcDedxCol", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Recon/RecTofTrackCol", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    //
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Recon/RecEmcShowerCol", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    obj = 0;
    pAddress = 0;
    status = m_pDataProvider->retrieveObject("/Event/Recon/RecMucTrackCol", obj);
    if ( status.isSuccess() )  {
        status = m_pConversionSvc->createRep(obj, pAddress);
    }
    TRecTrackEvent *recEvt     = m_cnvSvc->getRecTrackCnv()->getWriteObject();
    TTrigEvent     *trigEvt    = m_cnvSvc->getTrigCnv()->getWriteObject();
    TEvtHeader     *evtHeader  = m_cnvSvc->getEvtHeaderCnv()->getWriteObject();
    TDigiEvent     *digiEvt    = m_cnvSvc->getDigiCnv()->getWriteObject();

    if (recTrack1){
        delete [] recTrack1;
        recTrack1 = NULL;
    }
    if (recdis){
        delete recdis;
        recdis = NULL;
    }
    recdis = new TDisTrack();
//add mdc tracks
    recTrack1 = new TRecMdcTrack[20];
    int no = 0;
    no=(recEvt->getRecMdcTrackCol())->GetEntries();

    std::cout<<" mdc trk number ="<< no <<std::endl;
    if (no>20) no=20;
    for (int i=0;i<no;i++){
        const TRecMdcTrack* recTrack =recEvt->getRecMdcTrack(i);
        /*  Int_t    charge = recTrack->charge();
          Double_t pt     = recTrack->pxy();
          Double_t pz     = recTrack->pz();*/
        (recTrack1+i)->setTRecMdcTrack(recTrack);
        recdis->addRecMdcTrack(recTrack1+i);
    }

//add tof tracks
    if (tofTrack){
        delete [] tofTrack;
        tofTrack = NULL;
    }
    tofTrack = new TRecTofTrack[200];
    no=0;
    no = (recEvt->getTofTrackCol())->GetEntries();
    std::cout<<" tof trk number ="<< no <<std::endl;
    if (no>200) no =200;
    for (int i=0;i<no;i++){
        const TRecTofTrack* tofTrack1 =recEvt->getTofTrack(i);
        (tofTrack+i)->setTRecTofTrack(tofTrack1);
        recdis->addTofTrack(tofTrack+i);
    }

//add mdchits
    if (mdchit){
        delete [] mdchit;
        mdchit = NULL;
    }
    mdchit = new TRecMdcHit[1000];
    no=0;
    no = (recEvt->getRecMdcHitCol())->GetEntries();
    std::cout<<" mdc hits number ="<< no <<std::endl;
    if (no>1000) no =1000;
    for (int i=0;i<no;i++){
        const TRecMdcHit* mdchit1 =recEvt->getRecMdcHit(i);
        (mdchit+i)->setTRecMdcHit(mdchit1);
        recdis->addRecMdcHit(mdchit+i);
    }
//add muctrks
    if (muctrk){
        delete []  muctrk;
        muctrk = NULL;
    }
    muctrk = new TRecMucTrack[20];
    no=0;
    no = (recEvt->getMucTrackCol())->GetEntries();

    std::cout<<" muc trk number ="<< no <<std::endl;
    if (no>20) no=20;
    for (int i=0;i<no;i++){
        const TRecMucTrack* mucTrack1 =recEvt->getMucTrack(i);
        (muctrk+i)->setTRecMucTrack(mucTrack1);
        recdis->addMucTrack(muctrk+i);
    }

//add emcshowers
    if (emcshower){
        delete [] emcshower;
        emcshower=NULL;
    }
    emcshower = new TRecEmcShower[20];
    no=0;
    no = (recEvt->getEmcShowerCol())->GetEntries();
    std::cout<<" emc trk number ="<< no <<std::endl;
    if (no>20) no=20;
    for (int i=0;i<no;i++){
        const TRecEmcShower* rec_emc =recEvt->getEmcShower(i);
        (emcshower+i)->setTRecEmcShower(rec_emc);
        recdis->addEmcShower(emcshower+i);
    }


    TFile file(f_rootOutputFile.c_str(),"RECREATE");
    TTree tree("Event","Event");
    tree.Branch("TDisTrack","TDisTrack",&recdis,64000,1);
    tree.Branch("TDigiEvent", "TDigiEvent", &digiEvt, 64000, 1);
    tree.Branch("TEvtHeader","TEvtHeader",&evtHeader,64000,1);
    tree.Branch("TTrigEvent","TTrigEvent",&trigEvt,64000,1);
    tree.Fill();
    file.Write();
    file.Close();
    recdis->Clear();

    if (m_mode == 1){
        commonData tmp;
        tmp.clear();
    }



//############################################################
    //Part 5: operate semaphore -- OUTPUT_STORE, add event counter
    release.sem_num = OUTPUT_STORE;
    //++++++++++++++++++++++++++++++
    // Debug information
    //++++++++++++++++++++++++++++++
    //log << MSG::ERROR << "B -- No[" << counter++ << "] event "
    //<< "has deposited by producer." << endreq;
    //
    //log << MSG::ERROR << "Before change OUTPUT_STORE" << endreq;
    //log << MSG::ERROR << "OUTPUT_STORE will change from 0 to 1" << endreq;
    //log << MSG::ERROR << "semid: " << semid << endreq;
    //
    //if ((sem_value_F = semctl(semid, FREE_SPACE, GETVAL, 0)) == -1){
    //    log << MSG::ERROR << "semctl: GETVAL -- FREE_SPACE"
    //    << endreq;
    //    exit(5);
    //}
    //log << MSG::ERROR << "Semaphore FREE_SPACE has value of(refer 0) " <<
    //sem_value_F << endreq;
    //
    //if ((sem_value_O = semctl(semid, OUTPUT_STORE, GETVAL, 0)) == -1){
    //    log << MSG::ERROR << "semctl: GETVAL -- OUTPUT_STORE"
    //    << endreq;
    //    exit(5);
    //}
    //log << MSG::ERROR << "Semaphore OUTPUT_STORE has value of(refer 0) " <<
    //sem_value_O << endreq;
    //++++++++++++++++++++++++++++++

    if (semop(semid, &release, 1) == -1){
        log << MSG::ERROR << "Producer -- increase -- storage"
        << endreq;
        exit(4);
    }
    //++++++++++++++++++++++++++++++
    // Debug information
    //++++++++++++++++++++++++++++++
    //log << MSG::ERROR << "After change OUTPUT_STORE" << endreq;
    //log << MSG::ERROR << "OUTPUT_STORE has changed from 0 to 1" << endreq;

    //if ((sem_value_F = semctl(semid, FREE_SPACE, GETVAL, 0)) == -1){
    //    log << MSG::ERROR << "semctl: GETVAL -- FREE_SPACE"
    //    << endreq;
    //    exit(5);
    //}
    //log << MSG::ERROR << "Semaphore FREE_SPACE has value of(refer 0) " <<
    //sem_value_F << endreq;
    //
    //if ((sem_value_O = semctl(semid, OUTPUT_STORE, GETVAL, 0)) == -1){
    //    log << MSG::ERROR << "semctl: GETVAL -- OUTPUT_STORE"
    //    << endreq;
    //    exit(5);
    //}
    //log << MSG::ERROR << "Semaphore OUTPUT_STORE has value of(refer 1) " <<
    //sem_value_O << endreq;
    //++++++++++++++++++++++++++++++

    //TFile* f = new TFile("myShare.root", "read");
    //TTree* t = (TTree*)f->Get("Event");
    //std::cout << "BesVisAlg read file for debug" << std::endl;
    //t->Print();
    return status;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool BesVisAlg::hasRead(){
    // Part 1: Get the messaging service, print where you are
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "hadRead()" << endreq;

    // Part 2: Testing semaphore value
    if ((sem_value_F = semctl(semid, FREE_SPACE, GETVAL, 0)) == -1){
        log << MSG::ERROR << "semctl: GETVAL -- FREE_SPACE"
        << endreq;
        exit(5);
    }
    log << MSG::INFO << "Semaphore FREE_SPACE has value of " <<
    sem_value_F << endreq;

    if ((sem_value_O = semctl(semid, OUTPUT_STORE, GETVAL, 0)) == -1){
        log << MSG::ERROR << "semctl: GETVAL -- OUTPUT_STORE"
        << endreq;
        exit(5);
    }
    log << MSG::INFO << "Semaphore OUTPUT_STORE has value of " <<
    sem_value_O << endreq;

    if  ((sem_value_F == 1) && (sem_value_O == 0))	return true;
    else return false;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode BesVisAlg::hasWrite(){
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "hasWrite()" << endreq;
    StatusCode status = StatusCode::SUCCESS;

    int statusCode;
    //statusCode = remove(f_rootOutputFile.c_str());
    //log << MSG::ERROR << "remove file(" << f_rootOutputFile << "): " << statusCode << endreq;

    status = getSvc();
    if ( !status.isSuccess()){
        log <<MSG::ERROR << "getSvc() error!" << endreq;
        return StatusCode::SUCCESS;
    }


    status = collectObjects();
    if ( !status.isSuccess()){
        log <<MSG::ERROR << "collectObjects() error!" << endreq;
        return StatusCode::SUCCESS;
    }

//************************************************************************
//
//
//***********************************************************************
//   status = write2file();
//   if ( !status.isSuccess()){
//       log <<MSG::ERROR << "write2file() error!" << endreq;
//       return status;
//   }
//
//   status = finishSvc();
//   if ( !status.isSuccess()){
//       log <<MSG::ERROR << "finishSvc() error!" << endreq;
//       return status;
//   }

    return status;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode BesVisAlg::getSvc(){
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "getSvc()" << endreq;
    StatusCode status = StatusCode::SUCCESS;

//#################################
    ItemNames	m_itemNames;
    m_itemNames.push_back("/Event#1");
    m_itemNames.push_back("/Event/EventHeader#1");

    m_itemNames.push_back("/Event/Digi#1");
    m_itemNames.push_back("/Event/Digi/MdcDigiCol#1");
    m_itemNames.push_back("/Event/Digi/TofDigiCol#1");
    m_itemNames.push_back("/Event/Digi/EmcDigiCol#1");
    m_itemNames.push_back("/Event/Digi/MucDigiCol#1");

    m_itemNames.push_back("/Event/Recon#1");
    m_itemNames.push_back("/Event/Recon/RecMdcHitCol#1");
    m_itemNames.push_back("/Event/Recon/RecMdcTrackCol#1");

    m_itemNames.push_back("/Event/Trig#1");
    m_itemNames.push_back("/Event/Trig/TrigData#1");
    //m_itemNames.push_back("/Event/Recon/MdcHOTCol#1");
    //m_itemNames.push_back("/Event/Recon/RecMdcDedxCol#1");
    //m_itemNames.push_back("/Event/Recon/RecMdcDedxHitCol#1");
    //m_itemNames.push_back("/Event/Recon/RecMdcKalTrackCol#1");
    //m_itemNames.push_back("/Event/Recon/RecEsTimeCol#1");
    //m_itemNames.push_back("/Event/Recon/RecExtTrackCol#1");
    //m_itemNames.push_back("/Event/Recon/RecBTofHitCol#1");
    //m_itemNames.push_back("/Event/Recon/RecETofHitCol#1");
    //m_itemNames.push_back("/Event/Recon/RecTofTrackCol#1");
    //m_itemNames.push_back("/Event/Recon/RecBTofCalHitCol#1");
    //m_itemNames.push_back("/Event/Recon/RecETofCalHitCol#1");
    //m_itemNames.push_back("/Event/Recon/RecEmcHitCol#1");
    //m_itemNames.push_back("/Event/Recon/RecEmcClusterCol#1");
    //m_itemNames.push_back("/Event/Recon/RecEmcShowerCol#1");
    //m_itemNames.push_back("/Event/Recon/MucRecHitCol#1");
    //m_itemNames.push_back("/Event/Recon/RecMucTrackCol#1");
    //m_itemNames.push_back("/Event/Recon/RecMdcKalHelixSegCol#1");
    //m_itemNames.push_back("/Event/Recon/EvtRecEvent#1");
    //m_itemNames.push_back("/Event/Recon/EvtRecTrackCol#1");
    //m_itemNames.push_back("/Event/Recon/EvtRecVertexCol#1");
//#################################
    clearItems(m_itemList);
    ItemNames::iterator i;
    for (i = m_itemNames.begin(); i != m_itemNames.end(); i++){
        addItem(m_itemList, *i);
    }

    status = serviceLocator()->service("EventDataSvc", m_pDataManager, true);
    if ( !status.isSuccess() ) {
        log << MSG::FATAL << "Unable to locate IDataManagerSvc interface"
        << endreq;
        return status;
    }

    status = serviceLocator()->service("EventDataSvc", m_pDataProvider, true);
    if ( !status.isSuccess() ) {
        log << MSG::FATAL << "Unable to locate IDataProviderSvc interface"
        << endreq;
        return status;
    }

    status = serviceLocator()->service("EventCnvSvc", m_pConversionSvc, true);
    if ( !status.isSuccess() ) {
        log << MSG::FATAL << "Unable to locate IConversionSvc interface"
        << endreq;
        return status;
    }

    return status;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode BesVisAlg::finishSvc(){
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "finishSvc()" << endreq;
    StatusCode status = StatusCode::SUCCESS;

    status = m_pRootInterface->f_finalize();
    if ( !status.isSuccess())
        log << MSG::ERROR << "f_finalize() error" << endreq;

    status = m_pConversionSvc->commitOutput(f_rootOutputFile.c_str(), true);
    if ( !status.isSuccess())
        log << MSG::ERROR << "commitOutput() error" << endreq;

    clearItems(m_itemList);

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Add item to output streamer list
void BesVisAlg::addItem(Items& itms, const std::string& descriptor)   {
    MsgStream log(msgSvc(), name());
    int level = 0;
    size_t sep = descriptor.rfind("#");
    std::string obj_path (descriptor,0,sep);
    std::string slevel   (descriptor,sep+1,descriptor.length());
    if ( slevel == "*" )  {
        level = 9999999;
    }
    else   {
        level = atoi(slevel.c_str());
    }
    size_t idx = obj_path.find("/",1);
    while (idx != std::string::npos)  {
        std::string sub_item = obj_path.substr(0,idx);
        if ( 0 == findItem(sub_item) )   {
            addItem(itms, sub_item+"#1");
        }
        idx = obj_path.find("/",idx+1);
    }
    DataStoreItem* item = new DataStoreItem(obj_path, level);
    log << MSG::DEBUG << "Adding OutputStream item " << item->path()
    << " with " << item->depth()
    << " level(s)." << endreq;
    itms.push_back( item );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void BesVisAlg::clearItems(Items& itms)     {
    for ( Items::iterator i = itms.begin(); i != itms.end(); i++ )    {
        delete (*i);
    }
    itms.erase(itms.begin(), itms.end());
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Find single item identified by its path (exact match)
DataStoreItem*
BesVisAlg::findItem(const std::string& path)  {
    for (Items::const_iterator i=m_itemList.begin(); i != m_itemList.end(); ++i)  {
        if ( (*i)->path() == path )  return (*i);
    }
    return 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode BesVisAlg::collectObjects(){
    MsgStream log(msgSvc(), name());
    StatusCode status = StatusCode::SUCCESS;
    Items::iterator i;

    // Traverse the tree and collect the requested objects

    for ( i = m_itemList.begin(); i != m_itemList.end(); i++ )    {
        DataObject* obj = 0;
        IOpaqueAddress *pAddress = 0;
        m_currentItem = (*i);

        status = m_pDataProvider->retrieveObject(m_currentItem->path(), obj);
        if ( status.isSuccess() )  {
            status = m_pConversionSvc->createRep(obj, pAddress);
        }
        else  {
            log << MSG::WARNING << "Cannot write mandatory object(s) (Not found): "
            << m_currentItem->path() << endreq;
        }
    }


    return StatusCode::SUCCESS;
}

StatusCode BesVisAlg::write2file(){
    MsgStream log(msgSvc(), name());
    StatusCode status = StatusCode::SUCCESS;

    log << MSG::INFO << "write2file()" << endreq;

    const std::string treename("Event");
// ************************************************************************
//
// single event output stream
//
// ***********************************************************************
// TDigiEvent
// ***********************************************************************

    TDigiEvent *pTRawEvent = commonData::m_recEvt;
    if ( pTRawEvent == 0) {
        log << MSG::WARNING << "Can not get point TDigiEvent" << endreq;
    }

    const std::string rawBranchname("TDigiEvent");
    int branchnr = -1;

    m_pRootInterface = RootInterface::Instance(log);
    m_pRootInterface->f_addOutput(treename, f_rootOutputFile.c_str(), 1, 64000, 1);

    if (pTRawEvent != 0) m_pRootInterface->f_createBranch(treename, rawBranchname,
                pTRawEvent->ClassName(),
                &pTRawEvent, branchnr);

// ***********************************************************************
// TEvtHeader
// ***********************************************************************
    TEvtHeader *pTEvtHeader = commonData::m_EvtHeader;
    if ( pTEvtHeader == 0) {
        log << MSG::WARNING << "Can not get point TEvtHeader" << endreq;
    }

    const std::string headerBranchname("TEvtHeader");
    branchnr = -1;
    if (pTEvtHeader != 0) m_pRootInterface->f_createBranch(treename, headerBranchname,
                pTEvtHeader->ClassName(),
                &pTEvtHeader, branchnr);

// ***********************************************************************
// TTrigEvent
// ***********************************************************************
    TTrigEvent *pTTrigEvent = commonData::m_trigEvt;
    if ( pTTrigEvent == 0) {
        log << MSG::WARNING << "Can not get point TTrigEvent" << endreq;
    }

    const std::string trigBranchname("TTrigEvent");
    branchnr = -1;
    if (pTTrigEvent != 0) m_pRootInterface->f_createBranch(treename, trigBranchname,
                pTTrigEvent->ClassName(),
                &pTTrigEvent, branchnr);


// ***********************************************************************
// TRecEvent
// ***********************************************************************
    TRecTrackEvent *pTRecEvent = commonData::m_rectrackEvt;
    if ( pTRecEvent == 0) {
        log << MSG::WARNING << "Can not get point TRecEvent" << endreq;
    }

    const std::string recBranchname("TRecEvent");
    branchnr = -1;
    if (pTRecEvent != 0) m_pRootInterface->f_createBranch(treename, recBranchname,
                pTRecEvent->ClassName(),
                &pTRecEvent, branchnr);


// ***********************************************************************
    status = m_pRootInterface->f_fillTrees();
    if (status.isFailure())
        log << MSG::ERROR << "No Root tree was filled" << endreq;


// ***********************************************************************
    return status;

}
