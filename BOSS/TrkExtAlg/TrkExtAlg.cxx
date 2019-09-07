//
// File: TrkExtAlg.cxx
// Author: Wang Liangliang
// Date: 2005.4.4
// 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/EventHeader.h"

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "ExtEvent/RecExtTrack.h"
#include "TrkExtAlg/TrkExtAlg.h"
#include "TrkExtAlg/ExtSteppingAction.h"
/////////////////////////////////change
//#include "TrkExtAlg/Helix.h"
#include "TrackUtil/Helix.h"
#include "DetVerSvc/IDetVerSvc.h"
//////////////////////////////////
#include "TrkExtAlg/ExtMdcTrack.h"

using namespace Event;

string parName[5]={"e","mu","pi","kaon","proton"};

//Constructor
TrkExtAlg::TrkExtAlg(const string& name, ISvcLocator* pSvcLocator):Algorithm(name, pSvcLocator)
{
    myParticleName = "pi";
    msgFlag = false;
    myResultFlag = false;
    myInputTrk= "Kal";

    declareProperty("ParticleName",myParticleName);
    declareProperty("GeantGeomOptimization",myGeomOptimization=true);
    declareProperty("MessageFlag",msgFlag);
    declareProperty("ResultMessageFlag",myResultFlag);
    declareProperty("BFieldOn",myBFieldOn=true);
    declareProperty("InputTrk",myInputTrk);
    declareProperty("UseMucKalFilter",myUseMucKal=true);
    declareProperty("MucWindow",myMucWindow=6);
    declareProperty("SetSeed",m_setSeed=false);
}

//Destructor
TrkExtAlg::~TrkExtAlg()
{
    if(myExtTrack) delete myExtTrack;
}

//////////////////////
StatusCode TrkExtAlg::initialize()
{ 
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "initialize()" << endreq;

    IDetVerSvc* detVerSvc;
    StatusCode sc_det = service("DetVerSvc", detVerSvc);
    if( sc_det.isFailure() ) {
        log << MSG::FATAL << "can't retrieve DetVerSvc instance" << endreq;
        return sc_det;
    }

    m_detVer = detVerSvc->phase();
    log << MSG::INFO << "** ~~~~~~ ** : retrieved DetectorStage = " << m_detVer << endreq;

    myExtTrack = new Ext_track(msgFlag,myBFieldOn,myGeomOptimization,m_detVer,myUseMucKal,myMucWindow);
    myExtTrack->Initialization(msgFlag,myBFieldOn,myGeomOptimization,myUseMucKal,myMucWindow);			 		         
    //	myFile = new ofstream("ExtData.txt");

    /*
    //--------- For Ext Test ----------------
    NTuplePtr nt(ntupleSvc(),"FILE501/ext");
    if ( nt ) myNtuple = nt;
    else {
    myNtuple=ntupleSvc()->book("FILE501/ext",CLID_ColumnWiseTuple,"TrkExt");
    if(myNtuple) {
    myNtuple->addItem("charge",myCharge);
    myNtuple->addItem("emcHitFlag",myEmcHitFlag);
    myNtuple->addItem("emcHitTheta",myEmcHitTheta);
    myNtuple->addItem("emcHitPhi",myEmcHitPhi);
    myNtuple->addItem("emcVolNum",myEmcVolNum);
    myNtuple->addItem("emcExtTheta",myEmcExtTheta);
    myNtuple->addItem("emcExtPhi",myEmcExtPhi);
    myNtuple->addItem("dTheta",myDTheta);
    myNtuple->addItem("dPhi",myDPhi);
    }
    else    {   // did not manage to book the N tuple....
    log << MSG::ERROR <<"Cannot book N-tuple:" << long(myNtuple) << endmsg;
    return StatusCode::FAILURE;
    }
    }
    //-------- end Ext Test -----------------
    */
    return StatusCode::SUCCESS;
}


//////////////////////
StatusCode TrkExtAlg::execute()
{
    //cout<<"a new event "<<endl;	
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "execute()" << endreq;			 
    int eventNumber, runNumber;

	if(m_setSeed==true) CLHEP::HepRandom::setTheSeed(9000);
	//cout<<"seed "<<CLHEP::HepRandom::getTheSeed();

    SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
    if (!eventHeader) 
    {
        log << MSG::FATAL << "Could not find Event Header" << endreq;
        return( StatusCode::FAILURE);
    }
    runNumber = eventHeader->runNumber();
    eventNumber = eventHeader->eventNumber();
    //		if(eventNumber!=5&&eventNumber!=8&&eventNumber!=142) return( StatusCode::SUCCESS);//1900736 //839313
    if(msgFlag) 
    {
        cout<<"TrackExt:  ******************* Start a event *******************"<<endl;
        cout<<"run= "<<runNumber<<"; event= "<<eventNumber<<endl;
    }
    //    cout<<"run= "<<runNumber<<"; event= "<<eventNumber<<endl;
    /////
    SmartDataPtr<MucDigiCol> mucDigiCol(eventSvc(),"/Event/Digi/MucDigiCol");
    if(!mucDigiCol)  {
        log << MSG::FATAL << "Could not find MUC digi" << endreq;
        return( StatusCode::SUCCESS);
    }	
    //cout<<"digi col is "<<mucDigiCol->size()<<endl;
    ////
    ExtMdcTrack aExtMdcTrack;
    aExtMdcTrack.SetMsgFlag(msgFlag);

    bool setTrk=false;

    int parID;
    if(myParticleName=="e") parID=0;
    else if(myParticleName=="mu") parID=1;
    else if(myParticleName=="pi") parID=2;
    else if(myParticleName=="kaon") parID=3;
    else if(myParticleName=="proton"||myParticleName=="anti_proton") parID=4;

    if(myInputTrk=="Mdc")
    {
        SmartDataPtr<RecMdcTrackCol> aMdcTrackCol(eventSvc(),"/Event/Recon/RecMdcTrackCol");
        if(!aMdcTrackCol)
        {
            log << MSG::WARNING << "Can't find RecMdcTrackCol in TDS!" << endreq;
            return( StatusCode::SUCCESS);
        }
        setTrk=aExtMdcTrack.SetMdcRecTrkCol(aMdcTrackCol);

    } 
    else if(myInputTrk=="Kal")
    {
        SmartDataPtr<RecMdcKalTrackCol> aMdcKalTrackCol(eventSvc(),"/Event/Recon/RecMdcKalTrackCol");
        if(!aMdcKalTrackCol)
        {
            log << MSG::WARNING << "Can't find RecMdcKalTrackCol in TDS!" << endreq;
            return( StatusCode::SUCCESS);
        }
        setTrk=aExtMdcTrack.SetMdcKalTrkCol(aMdcKalTrackCol);
    }
    else
    {
        log << MSG::WARNING << "Wrong type of inputTrk:" << myInputTrk << endreq;
        return( StatusCode::SUCCESS);
    }

    RecExtTrackCol *aExtTrackCol = new RecExtTrackCol;
    if(setTrk)
    {
        while(aExtMdcTrack.GetOneGoodTrk())
        {
            //cout<<"start a track........................"<<endl; 
            RecExtTrack *aExtTrack = new RecExtTrack;

            for(int i=0; i<5; i++)// extrapolate one track with all 5 hypotheses
            {
                if(aExtMdcTrack.ReadTrk(i))
                {
                    aExtTrack->SetParType(i);

                    int trackID = aExtMdcTrack.GetTrackID();
                    aExtTrack->SetTrackId(trackID);
                    Hep3Vector position = aExtMdcTrack.GetPosition();
                    Hep3Vector momentum = aExtMdcTrack.GetMomentum();
                    HepSymMatrix error =  aExtMdcTrack.GetErrorMatrix();
                    double pathInMDC = aExtMdcTrack.GetTrackLength();		 
                    double tofInMdc = aExtMdcTrack.GetTrkTof();		 

                    if(msgFlag)
                    {
                        cout<<"Start From:"<<position.x()<<' '<<position.y()<<' '
                            <<position.z()<<endl;
                        cout<<"Start Momentum:"<<momentum.x()<<' '<<momentum.y()<<' '<<momentum.z()<<endl;
                        cout<<"Start Error matrix:"<<error<<endl;
                        cout<<"Path before start:"<< pathInMDC << endl;
                    }

                    G4String aParticleName(parName[i]);
                    double charge = aExtMdcTrack.GetParticleCharge();
                    if(!aParticleName.contains("proton"))
                    {
                        if(charge>0) aParticleName += "+";
                        else aParticleName += "-";
                    }
                    else
                    {
                        if(charge>0) aParticleName = "proton";
                        else aParticleName = "anti_proton";
                    }

                    if(msgFlag) 
                    {
                        cout<<"Charge: "<<charge<<endl;
                        cout<<"Particle: "<<aParticleName<<endl;
                    }

                    ExtSteppingAction *extSteppingAction;
                    extSteppingAction = myExtTrack->GetStepAction();
                    extSteppingAction->Set_which_tof_version(m_detVer);//Set the ToFversionnumber. Required for the MRPC

                    extSteppingAction->Reset();
                    extSteppingAction->SetMucDigiColPointer(mucDigiCol); 
                    extSteppingAction->SetExtTrackPointer(aExtTrack);
                    bool m_trackstatus = false;
                    int trk_startpart = 0;
                    while(!m_trackstatus)
                    { 

                        trk_startpart++;//just for protection
                        if(trk_startpart>20)
                        {cout<<"-------has modified more than 20 times---------"<<endl;break;}
                        if(myExtTrack->Set(position,momentum,error,aParticleName,pathInMDC,tofInMdc))
                        {
                            myExtTrack->TrackExtrapotation();
                            extSteppingAction->InfmodMuc(position,momentum,error);
                            m_trackstatus = extSteppingAction->TrackStop();
                        }
                        else
                          m_trackstatus = true;
                    }
                }

            }

            aExtTrack->SetParType(parID);

            if(msgFlag) cout<<"will add aExtTrack!"<<endl;
            if(aExtTrackCol) 
            {
                if(aExtTrack) aExtTrackCol->add(aExtTrack);
                else if(msgFlag) cout<<"No aExtTrack!"<<endl;
            }
            else
            {
                if(msgFlag) cout<<"No aExtTrackCol!"<<endl;
            }
            if(msgFlag) cout<<"add a aExtTrack!"<<endl;

            /*
            //For Test
            if(myFile) 
            {
            (*myFile)<<endPoint.x()<<' '<<endPoint.y()<<' '
            <<endPoint.z()<<' '<<endMomentum.x()
            <<' '<<endMomentum.y()<<' '<<endMomentum.z()
            <<' '<<endErrorMatrix(1,1)<<' '<<endErrorMatrix(2,2)
            <<' '<<endErrorMatrix(3,3)<<' '<<endErrorMatrix(4,4)
            <<' '<<endErrorMatrix(5,5)<<' '<<endErrorMatrix(6,6)
            <<endl;
            }
            else {
            log << MSG::ERROR <<"can't open file" << endreq;

            }
            */
        }//while			 
    }//if

    //Register ExtTrackCol to TDS.
    /*       	ReconEvent *aReconEvent = new ReconEvent();
                StatusCode sc = eventSvc()->registerObject("/Event/Recon",aReconEvent);
                if(sc!=StatusCode::SUCCESS) {
                log << MSG::FATAL << "Could not register ReconEvent" <<endreq;
                return( StatusCode::FAILURE);
                }
                */

    //cout<<"will write in to service "<<endl;	
    SmartIF<IDataManagerSvc> dataManSvc(eventSvc());
    //IDataManagerSvc *dataManSvc = dynamic_cast<IDataManagerSvc*> (eventSvc());

    DataObject *extTrackCol;
    eventSvc()->findObject("/Event/Recon/RecExtTrackCol",extTrackCol);
    if(extTrackCol != NULL) {
        dataManSvc->clearSubTree("/Event/Recon/RecExtTrackCol");
        eventSvc()->unregisterObject("/Event/Recon/RecExtTrackCol");
    }

    //  cout<<"write register object "<<endl;

    StatusCode sc = eventSvc()->registerObject("/Event/Recon/RecExtTrackCol", aExtTrackCol);
    if(sc!=StatusCode::SUCCESS) {
        log << MSG::FATAL << "Could not register RecExtTrackCol in TDS!" << endreq;
        return( StatusCode::FAILURE);
    }

    //Check ExtTrackCol in TDS.
    //cout<<"Check ExtTrackCol in TDS."<<endl;
    SmartDataPtr<RecExtTrackCol> aExtTrkCol(eventSvc(),"/Event/Recon/RecExtTrackCol");
    if (!aExtTrkCol) {
        log << MSG::FATAL << "Can't find RecExtTrackCol in TDS!" << endreq;
        return( StatusCode::FAILURE);
    }

    RecExtTrackCol::iterator iterOfExtTrk;
    int j=1;
    //cout<<"aExtTrkCol->size is "<<aExtTrkCol->size()<<"   --------------"<<endl;
    for(iterOfExtTrk = aExtTrkCol->begin();iterOfExtTrk!=aExtTrkCol->end();iterOfExtTrk++)
    {
        if(myResultFlag)
        {
            for(int i=0; i<5; i++)
            {
                //TOF information.
                cout<<"##########track"<<j<<": "<<"("<<i<<")"<<endl;
                cout<<"******TOF1:******"<<endl;
                cout<<"VolumeName: "<<(*iterOfExtTrk)->tof1VolumeName(i)<<"\t"
                    <<"VolumeNumber: "<<(*iterOfExtTrk)->tof1VolumeNumber(i)<<"\t"<<endl
                    <<"Position: "<<(*iterOfExtTrk)->tof1Position(i)<<"\t"
                    <<"Momentum: "<<(*iterOfExtTrk)->tof1Momentum(i)<<"\t"<<endl
                    <<"Error matrix: "<<(*iterOfExtTrk)->tof1ErrorMatrix(i)
                    <<"Error z: "<<(*iterOfExtTrk)->tof1PosSigmaAlongZ(i)<<"\t"
                    <<"Error Tz: "<<(*iterOfExtTrk)->tof1PosSigmaAlongT(i)<<"\t"
                    <<"Error x: "<<(*iterOfExtTrk)->tof1PosSigmaAlongX(i)<<"\t"
                    <<"Error y: "<<(*iterOfExtTrk)->tof1PosSigmaAlongY(i)<<endl
                    <<"Tof: "<<(*iterOfExtTrk)->tof1(i)<<"\t"
                    <<"PathOF: "<<(*iterOfExtTrk)->tof1Path(i)
                    <<endl;
                cout<<"******TOF2:******"<<endl;
                cout<<"VolumeName: "<<(*iterOfExtTrk)->tof2VolumeName(i)<<"\t"
                    <<"VolumeNumber: "<<(*iterOfExtTrk)->tof2VolumeNumber(i)<<"\t"<<endl
                    <<"Position: "<<(*iterOfExtTrk)->tof2Position(i)<<"\t"
                    <<"Momentum: "<<(*iterOfExtTrk)->tof2Momentum(i)<<"\t"<<endl
                    <<"Error matrix: "<<(*iterOfExtTrk)->tof2ErrorMatrix(i)
                    <<"Error z: "<<(*iterOfExtTrk)->tof2PosSigmaAlongZ(i)<<"\t"		
                    <<"Error Tz: "<<(*iterOfExtTrk)->tof2PosSigmaAlongT(i)<<"\t"
                    <<"Error x: "<<(*iterOfExtTrk)->tof2PosSigmaAlongX(i)<<"\t"
                    <<"Error y: "<<(*iterOfExtTrk)->tof2PosSigmaAlongY(i)<<endl
                    <<"Tof: "<<(*iterOfExtTrk)->tof2(i)<<"\t"
                    <<"PathOF: "<<(*iterOfExtTrk)->tof2Path(i)
                    <<endl;

                //EMC information.
                cout<<"******EMC:******"<<endl
                    <<"VolumeName: "<<(*iterOfExtTrk)->emcVolumeName(i)<<"\t"
                    <<"VolumeNumber: "<<(*iterOfExtTrk)->emcVolumeNumber(i)<<"\t"<<endl
                    <<"Position: "<<(*iterOfExtTrk)->emcPosition(i)<<"\t"
                    <<"Momentum: "<<(*iterOfExtTrk)->emcMomentum(i)<<"\t"<<endl
                    <<"Error matrix: "<<(*iterOfExtTrk)->emcErrorMatrix(i)
                    <<"Error theta: "<<(*iterOfExtTrk)->emcPosSigmaAlongTheta(i)<<"\t"
                    <<"Error phi: "<<(*iterOfExtTrk)->emcPosSigmaAlongPhi(i)<<"\t"
                    <<"EMC path: "<<(*iterOfExtTrk)->emcPath(i)
                    <<endl;

                //MUC information
                cout<<"******MUC:******"<<endl
                    <<"VolumeName: "<<(*iterOfExtTrk)->mucVolumeName(i)<<"\t"
                    <<"VolumeNumber: "<<(*iterOfExtTrk)->mucVolumeNumber(i)<<endl
                    <<"Position: "<<(*iterOfExtTrk)->mucPosition(i)<<"\t"
                    <<"Momentum: "<<(*iterOfExtTrk)->mucMomentum(i)<<"\t"<<endl
                    <<"Error matrix: "<<(*iterOfExtTrk)->mucErrorMatrix(i)
                    <<"Error z: "<<(*iterOfExtTrk)->mucPosSigmaAlongZ(i)<<"\t"
                    <<"Error Tz: "<<(*iterOfExtTrk)->mucPosSigmaAlongT(i)<<"\t"
                    <<"Error x: "<<(*iterOfExtTrk)->mucPosSigmaAlongX(i)<<"\t"
                    <<"Error y: "<<(*iterOfExtTrk)->mucPosSigmaAlongY(i)
                    <<endl;

                cout<<"*******MUC KALMANFILTER***********"<<endl;
                cout<<"Chisq is  "<<(*iterOfExtTrk)->MucKalchi2(i)<<endl;
                cout<<"Nfit is  "<<(*iterOfExtTrk)->MucKaldof(i)<<endl;
                cout<<"chiL "<<(*iterOfExtTrk)->MucKalchi2()<<endl;
                //   cout<<"Pull is  "<<(*iterOfExtTrk)->mucKalPull()<<endl;

                // cout<<"Residual is  "<<(*iterOfExtTrk)->mucKalResidual()<<endl;
                //Muc Ext hits information
                /*			ExtMucHitVec aExtMucHitVec = (*iterOfExtTrk)->GetExtMucHitVec();
                            int numOfMucHits = aExtMucHitVec.size();
                            cout<<"******MUC hits:"<<numOfMucHits<<"******"<<endl;
                            for(int j=0;j<numOfMucHits;j++)
                            {
                            cout<<"###Muc Hit "<<j<<":###"<<endl
                            <<"VolumeName: "<<aExtMucHitVec[j].GetVolumeName()<<"\t"
                            <<"VolumeNumber: "<<aExtMucHitVec[j].GetVolumeNumber()<<"\t"<<endl
                            <<"Position: "<<aExtMucHitVec[j].GetPosition()<<"\t"
                            <<"Momentum: "<<aExtMucHitVec[j].GetMomentum()<<"\t"<<endl
                            <<"Error z: "<<aExtMucHitVec[j].GetPosSigmaAlongZ()<<"\t"
                            <<"Error Tz: "<<aExtMucHitVec[j].GetPosSigmaAlongT()<<"\t"
                            <<"Error x: "<<aExtMucHitVec[j].GetPosSigmaAlongX()<<"\t"
                            <<"Error y: "<<aExtMucHitVec[j].GetPosSigmaAlongY()<<"\t"
                            <<endl;
                            }
                            */
            }
        }
        j++;

    } // loop ExtTrkCol

    if(msgFlag) cout<<"****************** End a event! ****************"<<endl<<endl;

    /*
    //--------- For Ext Test ----------------	
    // Retrieve mc truth
    SmartDataPtr<McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
    if (!mcParticleCol) {
    log << MSG::FATAL << "Could not find McParticle" << endreq;
    return( StatusCode::FAILURE);
    }

    int numOfTrack = mcParticleCol->size();
    if(msgFlag) cout<< "numOfMcTrack: " << numOfTrack << endl;
    if(numOfTrack!=2) return StatusCode::SUCCESS;

    // Retrieve Emc Mc Hits
    SmartDataPtr<EmcMcHitCol> emcMcHitCol(eventSvc(),"/Event/MC/EmcMcHitCol");
    if (!emcMcHitCol) {
    log << MSG::FATAL << "Could not find EMC truth" << endreq;
    return( StatusCode::FAILURE);
    }

    McParticleCol::iterator iter_mc = mcParticleCol->begin();
    EmcMcHitCol::iterator iterEmcBegin = emcMcHitCol->begin();
    int numOfEmcHits = emcMcHitCol->size();

    //	ExtTrackCol *aExtTrackCol = new ExtTrackCol;

    for (int i = 1;iter_mc != mcParticleCol->end(); iter_mc++, i++) {
    bool flag = (*iter_mc)->primaryParticle();
    if(!flag) continue;
    int particleId = (*iter_mc)->particleProperty();
    double charge = particleId/abs(particleId);
    unsigned int sFlag = (*iter_mc)->statusFlags();
    int trackIdx = (*iter_mc)->getTrackIndex();
    HepPoint3D iPos = (*iter_mc)->initialPosition();
    HepPoint3D fPos = (*iter_mc)->finalPosition();
    HepLorentzVector iFMomentum = (*iter_mc)->initialFourMomentum();
    HepLorentzVector fFMomentum = (*iter_mc)->finalFourMomentum();


    bool emcHitFlag = false;
    double thetaOfEmcHit = 0;
    double phiOfEmcHit = 0;
    Hep3Vector posOfEmcHit(0,0,0);
    //Get Emc Truth
    for(int j=0;j<numOfEmcHits;j++)
    {
    if(trackIdx==(*(iterEmcBegin+j))->getTrackIndex())
    {
    emcHitFlag = true;
    double x = (*(iterEmcBegin+j))->getPositionX();
    double y = (*(iterEmcBegin+j))->getPositionY();
    double z = (*(iterEmcBegin+j))->getPositionZ();
    Hep3Vector vec(x,y,z);
    posOfEmcHit = vec;
    thetaOfEmcHit = posOfEmcHit.theta();
    phiOfEmcHit = posOfEmcHit.phi();
    break;
    }
    }

    ExtSteppingAction *extSteppingAction;
    extSteppingAction = myExtTrack->GetStepAction();

    G4String aParticleName(myParticleName);
    if(!aParticleName.contains("proton"))
    {
    if(charge>0) aParticleName += "+"; 
    else aParticleName += "-"; 
    }
    else
    {
    if(charge>0) aParticleName = "proton";
    else aParticleName = "anti_proton";
}

ExtTrack *aExtTrack = new ExtTrack;
aExtTrack->SetTrackID(trackIdx);

Hep3Vector position(iPos);
Hep3Vector momentum(iFMomentum.x(),iFMomentum.y(),iFMomentum.z());
HepSymMatrix error(6,0);

if(myExtTrack->Set(position,momentum,error,aParticleName,0))
{
    extSteppingAction->SetExtTrackPointer(aExtTrack);
    myExtTrack->TrackExtrapotation();
}

Hep3Vector extEmcPos = aExtTrack->GetEmcPosition();
double volumeNum = aExtTrack->GetEmcVolumeNumber();
double thetaExt = extEmcPos.theta();
double phiExt = extEmcPos.phi();

if(myResultFlag)
{
    cout<< "*******Mc Track " << i <<" :******"<<endl;
    cout<< "Parimary particle :" << flag <<"  particleId = " << (*iter_mc)->particleProperty() << endl;
    cout<< "Track Index: " << trackIdx << endl;
    cout<< "initialPosition: "<< iPos.x() << "," << iPos.y() << "," << iPos.z() << endl;
    cout<< "initialFourMomentum: " <<iFMomentum.x()<<","<<iFMomentum.y()<<","<<iFMomentum.z() << endl;
    cout<<"Emc Truth:"<<emcHitFlag<<"!"<<posOfEmcHit<<endl;

    cout<<"Ext Emc: "<<aExtTrack->GetEmcVolumeName()<<aExtTrack->GetEmcPosition()<<endl;
}

myCharge = charge;
myEmcHitFlag = (emcHitFlag)? 1.:-1.;
myEmcHitTheta = thetaOfEmcHit;
myEmcHitPhi = phiOfEmcHit;
myEmcVolNum = volumeNum;
myEmcExtTheta = thetaExt;
myEmcExtPhi = phiExt;
myDTheta = myEmcHitTheta-myEmcExtTheta;
myDPhi = myEmcHitPhi-myEmcExtPhi;
while(myDTheta<-1*M_PI) myDTheta+=2.0*M_PI;
while(myDPhi<-1*M_PI) myDPhi+=2.0*M_PI;
while(myDPhi>M_PI) myDPhi-=2.0*M_PI;
while(myDTheta>M_PI) myDTheta-=2.0*M_PI;
myNtuple->write();

if(aExtTrack) delete aExtTrack;
}

//--------- end Ext Test ----------------
*/

return StatusCode::SUCCESS;
}

/////////////////////
StatusCode TrkExtAlg::finalize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "finalize()" << endreq;	

    //	delete myExtTrack;
    //	myFile->close();

    return StatusCode::SUCCESS;
}
