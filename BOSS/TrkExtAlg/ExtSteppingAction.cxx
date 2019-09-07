//
//File: ExtSteppingAction.cc
//Date: 2005.3.17
//Author: L.L.Wang
//History: 2005.3.17 created by Wang Liangliang
//         2005.8.12 find a mistake in caculation of error direction of tof and corrected by Wang Liangliang

//#include "CLHEP/config/CLHEP.h"

#include "TrkExtAlg/ExtSteppingAction.h"

#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4Field.hh"
#include "G4LogicalVolume.hh"
#include "globals.hh"

//#include "TofSim/BesTofDigitizerEcV4.hh"
//#include "TofSim/BesTofDigitizerEcV4_dbs.hh"
#include "MucRecEvent/MucRecHit.h"
#include "TrkExtAlg/ExtMucKal.h"
#include "TrkExtAlg/Ext_xp_err.h"
#include <iostream>
#include <string>
#include "strstream"

using namespace std;


ExtSteppingAction::ExtSteppingAction():chicc(0.0),initialPath(0.),myPathIntoCrystal(0.),myPathOutCrystal(0.),myPathInCrystal(0.),myBetaInMDC(0.),extXpErr(0),myExtTrack(0),msgFlag(true),myUseMucKalFlag(true),m_trackstop(false),myMucnfit_(0),myMucchisq_(0.),myMucdepth_(-99.),myMucbrLastLay_(-1),myMucecLastLay_(-1),myMucnhits_(-1),myMucWindow(6)
{
    myTof1R = 814.001;
    myTof1Z = 1330.0;
    myTof2R = 871.036;
    myEmcR1 = 945.0; 
    myEmcR2 = 983.9;
    myEmcZ  = 1416.8;
    myMucR  = 1700.0-0.01;
    myMucZ  = 2050.0-0.01;
    m_which_tof_version=2;
}

ExtSteppingAction::~ExtSteppingAction(){}

void ExtSteppingAction::Reset()
{
    chicc        = 0.;
    myExtTrack   = 0;
    myPathIntoCrystal = 0.;
    myPathOutCrystal  = 0.;
    myPathInCrystal   = 0.;

    myPathIntoTof1 = 0.0;
    myPathOutTof1  = 0.0;
    //myPathInTof1   = 0.0;
    myPathInTof1.clear();

    myPathIntoTof2 = 0.0;
    myPathOutTof2  = 0.0;
    //myPathInTof2   = 0.0;
    myPathInTof2.clear();

    myTofFlag    = false;
    myTof1Flag   = false;
    myTof2Flag   = false;
    myInTof1     = false;
    myInTof2     = false;
    myOutTof1    = false;
    myOutTof2    = false;
    myPhyEmcFlag = false;
    myEmcFlag    = false;
    myEmcPathFlag= false;
    myMucFlag    = false;

    m_trackstop =false;
    myMucchisq_ =0.;
    myMucnfit_ =0;
    myMucdepth_=-99.;
    myMucbrLastLay_=-1;
    myMucecLastLay_=-1;
    RememberID[0]=-1;
    RememberID[1]=-1;
    RememberID[2]=-1;

}

void ExtSteppingAction::MucReset()
{
    RemStep =0;
    RemDist = 99999.;
    RemDepth = 0.;
    RemID[0]=-1;
    RemID[1]=-1;
    RemID[2]=-1;
    RemVol = "";
}



void ExtSteppingAction::UserSteppingAction(const G4Step* currentStep)
{

    //Get track and its status
    G4Track* currentTrack = currentStep->GetTrack();
    if(!currentTrack) 
    {
        cout<<"Can't get currentTrack"<<endl;
        return;
    }
    G4TrackStatus currentTrackStatus = currentTrack->GetTrackStatus();

    int stepNumber = currentTrack->GetCurrentStepNumber();
    if(msgFlag)  cout<<"STEP "<<stepNumber<<":"<<endl;

    //Get current position and momentum
    Hep3Vector currentPosition = currentTrack->GetPosition();
    Hep3Vector currentMomentum = currentTrack->GetMomentum();


    //Get current Volume
    G4VPhysicalVolume* oldVolume;
    G4VPhysicalVolume* newVolume;
    if(!currentTrack->GetTouchableHandle()) cout<<"Can't get currentTrack->GetTouchableHandle()"<<endl;
    else  oldVolume= currentTrack->GetTouchableHandle()->GetVolume();
    if(!currentTrack->GetNextTouchableHandle()) cout<<"Can't get currentTrack->GetNextTouchableHandle()"<<endl;
    else  newVolume= currentTrack->GetNextTouchableHandle()->GetVolume();
    if(!oldVolume) cout<<"Can't get oldVolume!"<<endl;

    //****added by LI Chunhua
    if(stepNumber>50000) {
        cout<<"infinite steps in the track "<<endl;
        currentTrack->SetTrackStatus(fStopAndKill); m_trackstop =true;
    }

    G4String ParticleName = currentTrack->GetDefinition()->GetParticleName();
    double x_ = currentPosition.x();
    double y_ = currentPosition.y();
    double z_ = currentPosition.z();
    bool inner = (oldVolume!=newVolume)&&(!( (fabs(x_)>=myMucR) || (fabs(y_)>=myMucR) || ((fabs(x_-y_)/sqrt(2.))>=myMucR) || ((fabs(x_+y_)/sqrt(2.))>=myMucR) || (fabs(z_)>=myMucZ)) );
    bool mucdec = (fabs(x_)>=myMucR) || (fabs(y_)>=myMucR) || ((fabs(x_-y_)/sqrt(2.))>=myMucR) || ((fabs(x_+y_)/sqrt(2.))>=myMucR) || (fabs(z_)>=myMucZ);

    //if current particle is alive.
    if(currentTrackStatus == fAlive)
    {
        //if(oldVolume!=newVolume)//if so, update Error Matrix and chicc.
        if(inner||mucdec)//update Error Matrix for newvolume in MDC,TOF,EMC; update Error Matrix step by step in MUC;
        {
            //Get current B field
            double currentPoint[3]={currentPosition.x(),currentPosition.y(),currentPosition.z()};
            double currentBfield[3]={0.0,0.0,0.0};
            Hep3Vector currentB(0.0,0.0,1.0);
            if(G4TransportationManager::GetTransportationManager())
            {
                G4FieldManager* fieldManager=G4TransportationManager::GetTransportationManager()->GetFieldManager();
                if(fieldManager)       
                {     	  
                    if(fieldManager->GetDetectorField())	
                    {
                        fieldManager->GetDetectorField()->GetFieldValue(currentPoint,currentBfield);      
                        currentB.set(currentBfield[0]/tesla,currentBfield[1]/tesla,currentBfield[2]/tesla);
                        if(msgFlag) cout<<"B:"<<currentB.x()<<","<<currentB.y()<<","<<currentB.z()<<endl;
                    }
                }
            }

            //Get chicc
            G4Material* oldMaterial = oldVolume->GetLogicalVolume()->GetMaterial();
            if(!oldMaterial) std::cout<<"Can't get oldMaterial"<<std::endl;
            else CalculateChicc(oldMaterial);

            //Update Error Matrix
            if(!(extXpErr->move(currentPosition,currentMomentum,currentB,1,chicc)))
              if(msgFlag) cout<<"can not update Error Matrix!!"<<endl;

            //Verbose
            if(msgFlag)
            {
                cout<<"Volume name:"<<newVolume->GetName()<<endl;
                cout<<"Volume number:"<<newVolume->GetCopyNo()<<endl;
                cout<<"x:"<<currentPoint[0]<<"//y:"<<currentPoint[1]<<"//z:"<<currentPoint[2]
                    <<"||px:"<<currentMomentum.x()<<"//py:"<<currentMomentum.y()<<"//pz:"
                    <<currentMomentum.z()<<endl;
                cout<<"Error matrix is:"<<extXpErr->get_err()<<endl;
                cout<<"phi:"<<atan(currentPoint[1]/currentPoint[0])<<endl;
                Hep3Vector nz(0.,0.,1.);
                cout<<"Projected z error:"<<extXpErr->get_plane_err(currentMomentum.unit(),nz)
                    <<endl;
                double x,y,R;
                x = currentPoint[0];
                y = currentPoint[1];
                R = sqrt(x*x+y*y);
                Hep3Vector nt(-y/R,x/R,0.);
                cout<<"Projected phi error:"<<(extXpErr->get_plane_err(currentMomentum.unit(),nt))/R
                    <<endl<<endl;
            }

            //some often used things
            Hep3Vector xVector(1.0,0,0);
            Hep3Vector yVector(0,1.0,0);
            Hep3Vector zVector(0,0,1.0);
            Hep3Vector tzVector;
            tzVector.setRThetaPhi(1.0,M_PI/2.0,currentPosition.phi());
            double r = currentPosition.perp();
            double x = currentPosition.x();
            double y = currentPosition.y();
            double z = currentPosition.z();
            G4String newVolumeName = newVolume->GetName();
            G4String oldVolumeName = oldVolume->GetName();
            G4StepPoint* postStepPoint = currentStep->GetPostStepPoint();
            G4TouchableHistory* theTouchable = (G4TouchableHistory*)(postStepPoint->GetTouchable());
            int newVolumeNumber=newVolume->GetCopyNo();
            //		int newVolumeNumber=theTouchable->GetReplicaNumber(2);

            G4String name1;
            if(newVolumeName=="logical_gasLayer")
            {
                name1=theTouchable->GetVolume(3)->GetName();
                newVolumeNumber=theTouchable->GetReplicaNumber(3);
            }






            //Get PhyTof data

            //std::cout << "ExtSteppingAction        NewVolumeName:  "  <<  newVolumeName << std::endl;
            //std::cout << "ExtSteppingAction        OldVolumeName:  "  <<  oldVolumeName << std::endl;
            //std::cout << "ExtSteppingAction     name1:  " <<name1<<std::endl;
            //Comment this part to remove the airtight Tof. Most hits would be replaced by the following specific sensitive 
            //detector. The remianing few hits are abondoned in Tof reconstruction.
            /*
               if( (!myTofFlag) && (!myTof1Flag) && (newVolumeName.contains("Tof") ))
               {
               newVolumeNumber = -2;
               double currentTrackLength = currentTrack->GetTrackLength();
               double totalTrackLength = currentTrackLength + initialPath;
            //double initialTof = initialPath/(myBetaInMDC*299.792458);
            //cout<<"initialTof = "<<initialTof<<endl;
            //double totalTOF = totalTrackLength/(myBetaInMDC*299.792458);
            double localTime = currentTrack->GetLocalTime();
            //cout<<"localTime = "<<localTime<<endl;
            double totalTOF = localTime + initialTof;
            //cout<<"totalTOF= "<<totalTOF<<endl;

            //std::cout << "ExtSteppingAction        Volumename contains one of the marker words" << std::endl;

            if(myExtTrack)
            {
            double xError = extXpErr->get_plane_err(currentMomentum.unit(),xVector);
            double yError = extXpErr->get_plane_err(currentMomentum.unit(),yVector);
            double zError = extXpErr->get_plane_err(currentMomentum.unit(),zVector);
            double tzError= extXpErr->get_plane_err(currentMomentum.unit(),tzVector);
            myExtTrack->SetTof1Data(currentPosition/10.0,currentMomentum/1000.0,newVolumeName,newVolumeNumber,totalTOF,totalTrackLength/10,myOutputSymMatrix(extXpErr->get_err()),zError/10,tzError/10,xError/10,yError/10);
            myTofFlag = true;
            }
            return;
            }//close if (!myTofFlag)
            */



            //Get Tof layer1 Ext data
            if((!myTof1Flag) && (newVolumeName=="logicalScinBr1" || newVolumeName.contains("ScinEc") || 
                            (newVolumeName=="logical_gasLayer" && (name1=="logical_container_m0" || name1=="logical_container_m1"))))
            {
                double currentTrackLength = currentTrack->GetTrackLength();
                double totalTrackLength = currentTrackLength+initialPath;
                double localTime = currentTrack->GetLocalTime();
                double totalTOF = localTime + initialTof;
                myInTof1 = true;
                myPathIntoTof1 = currentTrackLength;
                if(msgFlag) cout << "myPathIntoTof1 = " << myPathIntoTof1 << endl;

                newVolumeNumber=theTouchable->GetReplicaNumber(2);

                if(newVolumeName.contains("ScinEc")) {
                    newVolumeNumber=(95-newVolumeNumber)/2;
                    newVolumeNumber=newVolumeNumber+176;

                    if(newVolumeName.contains("East")) newVolumeNumber=newVolumeNumber+48;
                    if(myExtTrack) {
                        double xError = extXpErr->get_plane_err(currentMomentum.unit(),xVector);
                        double yError = extXpErr->get_plane_err(currentMomentum.unit(),yVector);
                        double zError = extXpErr->get_plane_err(currentMomentum.unit(),zVector);
                        double tzError= extXpErr->get_plane_err(currentMomentum.unit(),tzVector);
                        myExtTrack->SetTof1Data(currentPosition/10,currentMomentum/1000,newVolumeName,newVolumeNumber,totalTOF,totalTrackLength/10,myOutputSymMatrix(extXpErr->get_err()),zError/10,tzError/10,xError/10,yError/10);
                        myTof1Flag = true;
                    }
                }

                else if(newVolumeName=="logical_gasLayer")
                {
                    G4ThreeVector currentPostPosition = currentStep->GetPostStepPoint()->GetPosition();
                    G4ThreeVector localPosition = theTouchable->GetHistory()->GetTopTransform().TransformPoint(currentPostPosition);
                    //cout<<"localPosition: "<<localPosition.x()<<"  "<<localPosition.y()<<"  "<<localPosition.z()<<endl;

                    //Here we assume the spread of one strip is 24+3=27mm. 
                    //Distance between strips and lower large glass margin: 6, top: 8, z_strip=z_small_glass-0.5
                    double z_mm = localPosition.z()-0.5+(24+3)*6;
                    int strip;
                    if(z_mm<=0)
                    {
                        strip=0;
                    }
                    else if(z_mm>0 && z_mm<12*27)
                    {
                        strip=floor(z_mm/27);
                    }
                    else
                    {
                        strip=11;
                    }
                    if(strip<0) strip=0;
                    if(strip>11) strip=11;

                    newVolumeNumber=theTouchable->GetReplicaNumber(3);
                    newVolumeNumber = 35-newVolumeNumber;
                    if( currentPosition.z() < 0.0  ) { newVolumeNumber = newVolumeNumber + 36; }
                    newVolumeNumber = 12*newVolumeNumber+strip;
                    newVolumeNumber = newVolumeNumber + 176 + 96;
                    if(myExtTrack) {
                        double xError = extXpErr->get_plane_err(currentMomentum.unit(),xVector);
                        double yError = extXpErr->get_plane_err(currentMomentum.unit(),yVector);
                        double zError = extXpErr->get_plane_err(currentMomentum.unit(),zVector);
                        double tzError= extXpErr->get_plane_err(currentMomentum.unit(),tzVector);
                        Hep3Vector locP = Hep3Vector( localPosition.x(), localPosition.y(), localPosition.z() );
                        //cout<<"locP: "<<locP.x()<<"  "<<locP.y()<<"  "<<locP.z()
                        //<<"  currentMomentum: "<<currentMomentum.x()<<"  "<<currentMomentum.y()<<"  "<<currentMomentum.z()
                        //<<"  newVolumeName: "<<newVolumeName<<"  newVolumeNumber: "<<newVolumeNumber
                        //<<"  totalTOF: "<<totalTOF<<"  totalTrackLength: "<<totalTrackLength<<endl;
                        myExtTrack->SetTof1Data(locP/10,currentMomentum/1000,newVolumeName,newVolumeNumber,totalTOF,totalTrackLength/10,myOutputSymMatrix(extXpErr->get_err()),zError/10,tzError/10,xError/10,yError/10);
                        myTof1Flag = true;
                    }
                }
                else
                { 
                    newVolumeNumber=(527-newVolumeNumber)/3;
                    if(myExtTrack) {
                        double xError = extXpErr->get_plane_err(currentMomentum.unit(),xVector);
                        double yError = extXpErr->get_plane_err(currentMomentum.unit(),yVector);
                        double zError = extXpErr->get_plane_err(currentMomentum.unit(),zVector);
                        double tzError= extXpErr->get_plane_err(currentMomentum.unit(),tzVector);
                        myExtTrack->SetTof1Data(currentPosition/10,currentMomentum/1000,newVolumeName,newVolumeNumber,totalTOF,totalTrackLength/10,myOutputSymMatrix(extXpErr->get_err()),zError/10,tzError/10,xError/10,yError/10);
                        myTof1Flag = true;
                    }
                }

                return;

            }//close if (!myTof1Flag)

            if( myInTof1 && ( oldVolumeName=="logicalScinBr1" || oldVolumeName.contains("ScinEc") ||
                            (oldVolumeName=="logical_gasLayer" && (name1=="logical_container_m0" || name1=="logical_container_m1"))) ) {
                myInTof1 = false;
                myOutTof1 = true;
                myPathOutTof1 = currentTrack->GetTrackLength();
                myPathInTof1.push_back(myPathOutTof1 - myPathIntoTof1);
                if(msgFlag) cout << "myPathOutTof1 = " << myPathOutTof1 << endl;
            }

            if( myOutTof1 && ( newVolumeName=="logicalScinBr1" || newVolumeName.contains("ScinEc") || 
                            (newVolumeName=="logical_gasLayer" && (name1=="logical_container_m0" || name1=="logical_container_m1"))) ) {
                myInTof1 = true;
                myOutTof1 = false;
                myPathIntoTof1 = currentTrack->GetTrackLength();
                if(msgFlag) cout << "myPathIntoTof1 = " << myPathIntoTof1 << endl;
            }


            //Get Tof layer2 Ext data
            if( (!myTof2Flag) && (newVolumeName=="logicalScinBr2" || 
                            (newVolumeName=="logical_gasLayer" && (name1=="logical_container_m2" || name1=="logical_container_m3")))) 
            {
                double currentTrackLength = currentTrack->GetTrackLength();
                double totalTrackLength = currentTrackLength+initialPath;
                //double totalTOF = totalTrackLength/(myBetaInMDC*299.792458);
                double localTime = currentTrack->GetLocalTime();
                double totalTOF = localTime + initialTof;
                myInTof2 = true;
                myPathIntoTof2 = currentTrackLength;
                if(msgFlag) cout << "myPathIntoTof2 = " << myPathIntoTof2 << endl;
                newVolumeNumber=theTouchable->GetReplicaNumber(2);

                if(newVolumeName=="logical_gasLayer")
                {
                    G4ThreeVector currentPostPosition = currentStep->GetPostStepPoint()->GetPosition();
                    G4ThreeVector localPosition = theTouchable->GetHistory()->GetTopTransform().TransformPoint(currentPostPosition);
                    //cout<<"localPosition: "<<localPosition.x()<<"  "<<localPosition.y()<<"  "<<localPosition.z()<<endl;

                    //Here we assume the spread of one strip is 24+3=27mm. 
                    //Distance between strips and lower large glass margin: 6, top: 8, z_strip=z_small_glass-0.5
                    double z_mm = localPosition.z()-0.5+(24+3)*6;
                    int strip;
                    if(z_mm<=0)
                    {
                        strip=0;
                    }
                    else if(z_mm>0 && z_mm<12*27)
                    {
                        strip=floor(z_mm/27);
                    }
                    else
                    {
                        strip=11;
                    }
                    if(strip<0) strip=0;
                    if(strip>11) strip=11;

                    newVolumeNumber=theTouchable->GetReplicaNumber(3);
                    newVolumeNumber = 35-newVolumeNumber;
                    if( currentPosition.z() < 0.0  ) { newVolumeNumber = newVolumeNumber + 36; }
                    newVolumeNumber = 12*newVolumeNumber+strip;
                    newVolumeNumber = newVolumeNumber + 176 + 96;

                    if(myExtTrack) 
                    {
                        double xError = extXpErr->get_plane_err(currentMomentum.unit(),xVector);
                        double yError = extXpErr->get_plane_err(currentMomentum.unit(),yVector);
                        double zError = extXpErr->get_plane_err(currentMomentum.unit(),zVector);
                        double tzError= extXpErr->get_plane_err(currentMomentum.unit(),tzVector);
                        Hep3Vector locP = Hep3Vector( localPosition.x(), localPosition.y(), localPosition.z() );
                        //cout<<"locP: "<<locP.x()<<"  "<<locP.y()<<"  "<<locP.z()
                        //<<"  currentMomentum: "<<currentMomentum.x()<<"  "<<currentMomentum.y()<<"  "<<currentMomentum.z()
                        //<<"  newVolumeName: "<<newVolumeName<<"  newVolumeNumber: "<<newVolumeNumber
                        //<<"  totalTOF: "<<totalTOF<<"  totalTrackLength: "<<totalTrackLength<<endl;
                        myExtTrack->SetTof2Data(locP/10,currentMomentum/1000,newVolumeName,newVolumeNumber,totalTOF,totalTrackLength/10,myOutputSymMatrix(extXpErr->get_err()),zError/10,tzError/10,xError/10,yError/10);
                        myTof2Flag = true;
                    }
                }
                else
                {
                    newVolumeNumber=(527-newVolumeNumber)/3;
                    if(myExtTrack)
                    {
                        double xError = extXpErr->get_plane_err(currentMomentum.unit(),xVector);
                        double yError = extXpErr->get_plane_err(currentMomentum.unit(),yVector);
                        double zError = extXpErr->get_plane_err(currentMomentum.unit(),zVector);
                        double tzError= extXpErr->get_plane_err(currentMomentum.unit(),tzVector);
                        myExtTrack->SetTof2Data(currentPosition/10,currentMomentum/1000,newVolumeName,newVolumeNumber,totalTOF,totalTrackLength/10,myOutputSymMatrix(extXpErr->get_err()),zError/10,tzError/10,xError/10,yError/10);
                        myTof2Flag = true;
                    }
                }
                return;
            }//close if( (!myTof2Flag) && newVolumeName=="logicalScinBr2" )

            if( myInTof2 && (oldVolumeName=="logicalScinBr2" || 
                            (oldVolumeName=="logical_gasLayer" && (name1=="logical_container_m2" || name1=="logical_container_m3"))) ) {
                myInTof2 = false;
                myOutTof2 = true;
                myPathOutTof2 = currentTrack->GetTrackLength();
                myPathInTof2.push_back(myPathOutTof2 - myPathIntoTof2);
                if(msgFlag) cout << "myPathOutTof2 = " << myPathOutTof2 << endl;
            }

            if( myOutTof2 && (newVolumeName=="logicalScinBr2" || 
                            (newVolumeName=="logical_gasLayer" && (name1=="logical_container_m2" || name1=="logical_container_m3"))) ) {
                myInTof2 = true;
                myOutTof2 = false;
                myPathIntoTof2 = currentTrack->GetTrackLength();
                if(msgFlag) cout << "myPathIntoTof2 = " << myPathIntoTof2 << endl;
            }


            //Get PhyEmc data.
            if( (!myPhyEmcFlag) && (!myEmcFlag) && (newVolumeName=="EMC" || newVolumeName.contains("BSC") || newVolumeName=="EndPhi"))
            {
                newVolumeNumber = -2;
                if(myExtTrack)
                {
                    //				myPathIntoCrystal = currentTrack->GetTrackLength();
                    Hep3Vector nPhi(-y/r,x/r,0.);
                    double errorPhi = (extXpErr->get_plane_err(currentMomentum.unit(),nPhi))/r;

                    Hep3Vector aPosition = currentPosition;
                    double R = aPosition.r();
                    double aTheta = aPosition.theta();
                    aPosition.setTheta(aTheta + M_PI_2);
                    double errorTheta;
                    errorTheta =(extXpErr->get_plane_err(currentMomentum.unit(),aPosition.unit()))/R;
                    if(msgFlag) cout<<"Theta direction: "<<aPosition<<endl;
                    myExtTrack->SetEmcData(currentPosition/10,currentMomentum/1000,newVolumeName,newVolumeNumber,errorTheta,errorPhi,myOutputSymMatrix(extXpErr->get_err()));
                }
                myPhyEmcFlag = true;
                return;
            }


            if(!myEmcPathFlag &&  newVolumeName.contains("Crystal")  )
            {
                myPathIntoCrystal = currentTrack->GetTrackLength();
                //			cout<<"Enter Crystal, path: "<<myPathIntoCrystal<<endl;
                myEmcPathFlag = true;
            }

            //Get Emc Ext data.
            if( (!myEmcFlag) && newVolumeName.contains("Crystal")  )
            {
                if(myExtTrack)
                {
                    //------------------- record crystal number
                    int npart,ntheta,nphi;
                    if(currentTrack->GetNextTouchableHandle()->GetVolume(1)->GetName().contains("BSC")) { //barrel
                        npart=1;
                        std::istringstream thetaBuf((currentTrack->GetNextTouchableHandle()->GetVolume(1)->GetName()).substr(16,2));
                        thetaBuf >> ntheta ;
                        nphi = 308-currentTrack->GetNextTouchableHandle()->GetCopyNumber(2);
                    } else {  //endcap
                        npart=2-2*currentTrack->GetNextTouchableHandle()->GetCopyNumber(3);
                        int endSector=currentTrack->GetNextTouchableHandle()->GetCopyNumber(2);
                        int endNb,endNbGDML;
                        std::istringstream thetaBuf((currentTrack->GetNextTouchableHandle()->GetVolume(0)->GetName()).substr(20,2));
                        thetaBuf  >> endNb ;
                        endNbGDML=CalculateEmcEndCopyNb(endNb);
                        int endSectorGDML=CalculateEmcEndPhiNb(endSector);
                        CalculateEmcEndThetaPhi(npart,endSectorGDML,endNbGDML,ntheta,nphi);
                    }
                    ostringstream strEmc;
                    if(ntheta<10) {
                        strEmc<<"EmcPart"<<npart<<"Theta0"<<ntheta<<"Phi"<<nphi;
                    } else {
                        strEmc<<"EmcPart"<<npart<<"Theta"<<ntheta<<"Phi"<<nphi;
                    } //------------------------------------------

                    //				myPathIntoCrystal = currentTrack->GetTrackLength();
                    Hep3Vector nPhi(-y/r,x/r,0.);
                    double errorPhi = (extXpErr->get_plane_err(currentMomentum.unit(),nPhi))/r;

                    Hep3Vector aPosition = currentPosition;
                    double R = aPosition.r();
                    double aTheta = aPosition.theta();
                    aPosition.setTheta(aTheta + M_PI_2);
                    double errorTheta;
                    errorTheta =(extXpErr->get_plane_err(currentMomentum.unit(),aPosition.unit()))/R;
                    if(msgFlag) cout<<"Theta direction: "<<aPosition<<endl;
                    myExtTrack->SetEmcData(currentPosition/10,currentMomentum/1000,strEmc.str(),
                                //myExtTrack->SetEmcData(currentPosition/10,currentMomentum/1000,newVolumeName,
                        newVolumeNumber,errorTheta,errorPhi,myOutputSymMatrix(extXpErr->get_err()));
                                }
                                myEmcFlag = true;
                                return;
                                }

                                //Get path in Emc
                                if(myEmcPathFlag &&  oldVolumeName.contains("Crystal") )
                                {
                                myPathOutCrystal = currentTrack->GetTrackLength();
                                myPathInCrystal = myPathInCrystal+myPathOutCrystal-myPathIntoCrystal;
                                //			cout<<"Go out of crystal, path: "<<myPathOutCrystal<<endl;
                                myEmcPathFlag=false;
                                }


                                //Get Muc Ext Data.
                                if( (!myMucFlag) && ( (fabs(x)>=myMucR) || (fabs(y)>=myMucR) || ((fabs(x-y)/sqrt(2.))>=myMucR) || ((fabs(x+y)/sqrt(2.))>=myMucR) || (fabs(z)>=myMucZ) ) )
                                {
                                    int newVolumeNumber = newVolume->GetCopyNo();
                                    if(myExtTrack)
                                    {
                                        Hep3Vector xVector(1.0,0,0); 
                                        Hep3Vector yVector(0,1.0,0);
                                        Hep3Vector zVector(0,0,1.0);
                                        double xError = extXpErr->get_plane_err(currentMomentum.unit(),xVector);
                                        double yError = extXpErr->get_plane_err(currentMomentum.unit(),yVector);
                                        double zError = extXpErr->get_plane_err(currentMomentum.unit(),zVector);
                                        double tzError;
                                        double phi = currentPosition.phi();
                                        if(phi<0.) phi+=M_PI;
                                        int i = int(8.0*phi/M_PI);
                                        if( i==0 || i==7 || i==8 )
                                        {
                                            tzError = yError;
                                        }
                                        if( i==1 || i==2 )
                                        {
                                            Hep3Vector tzVector(-1.0,1.0,0.);
                                            tzError =extXpErr->get_plane_err(currentMomentum.unit(),tzVector.unit());
                                        }
                                        if( i==3 || i==4 )
                                        {
                                            tzError = xError;
                                        } 
                                        if( i==5 || i==6 )
                                        {
                                            Hep3Vector tzVector(1.0,1.0,0.);
                                            tzError =extXpErr->get_plane_err(currentMomentum.unit(),tzVector.unit());
                                        }
                                        myExtTrack->SetMucData(currentPosition/10,currentMomentum/1000,newVolumeName,newVolumeNumber,myOutputSymMatrix(extXpErr->get_err()),zError/10,tzError/10,xError/10,yError/10);
                                    }
                                    myMucFlag = true;
                                    if(!(ParticleName.contains("mu")&&myUseMucKalFlag))
                                    {
                                        //currentTrack->SetKineticEnergy(0.0);
                                        currentStep->GetTrack()->SetTrackStatus(fStopAndKill);
                                        m_trackstop=true;
                                        return;
                                    }
                                }

                    //**************************************************
                    //************inset KalFilter Algorithm in MUC******
                    //**************************************************
                    HepSymMatrix XpErr = extXpErr->get_err();
                    int namesize = oldVolumeName.size();
                    bool Flag1(false),Flag2(false),Flag3(false),Flag4(false),Flag5(false);
                    Flag1 = m_mucdigicol->size()>0;
                    Flag2 = myUseMucKalFlag;
                    Flag3 = ParticleName.contains("mu")&&oldVolumeName.contains("lMuc")&&oldVolumeName.contains("P")&&oldVolumeName.contains("S")&&(oldVolumeName.contains("G")||oldVolumeName.contains("Ab"));
                    Flag4 = oldVolumeName.contains("lMuc")&&oldVolumeName.contains("P")&&oldVolumeName.contains("S")&&((namesize==10&&oldVolumeName.contains("G"))||(namesize==11&&oldVolumeName.contains("Ab")));
                    Flag5 = !((RemID[0]==1&&RemID[2]==9)||((RemID[0]==0||RemID[0]==2)&&RemID[2]==8));
                    // if(!Flag5) {currentStep->GetTrack()->SetTrackStatus(fStopAndKill);m_trackstop=true;}
                    if(Flag1&&Flag2&&Flag3&&Flag5)
                    {
                        //get depth in Ab
                        double depth = currentStep-> GetStepLength();
                        if(oldVolumeName.contains("Ab"))
                          RemDepth = RemDepth+ depth;
                        if(RemStep==0&&Flag4)
                        {
                            Hep3Vector ID_1 = GetGapID(oldVolumeName);
                            RemID = ID_1;

                            bool LastLay = (ID_1[0]==1&&ID_1[2]<9)||((ID_1[0]==0||ID_1[0]==2)&&ID_1[2]<8);
                            if(RememberID[2]!=ID_1[2]&&LastLay)
                            {
                                Hep3Vector pos_loc = MucGeoGeneral::Instance()->GetGap(ID_1[0],ID_1[1],ID_1[2])->TransformToGap(currentPosition);
                                double dist = fabs(pos_loc.z());
                                RemPositon = currentPosition;
                                RemMomentum = currentMomentum;
                                RemXpErr = XpErr;
                                RemDist = dist;
                                RemStep++;
                            }
                        }
                        if(RemStep>0)
                        {
                            bool WillFilter = false;
                            bool LastLay_ = false;
                            double dist_b = 99999.;
                            Hep3Vector ID_2;
                            if(Flag4)
                            {
                                ID_2 = GetGapID(oldVolumeName);
                                LastLay_ =(ID_2[0]==1&&ID_2[2]<9)||((ID_2[0]==0||ID_2[0]==2)&&ID_2[2]<8);
                                if(LastLay_)dist_b=fabs(MucGeoGeneral::Instance()->GetGap(ID_2[0],ID_2[1],ID_2[2])->TransformToGap(currentPosition).z());
                                if(RemID!=ID_2)
                                  WillFilter=true;

                            }

                            Hep3Vector pos_loc = MucGeoGeneral::Instance()->GetGap(RemID[0],RemID[1],RemID[2])->TransformToGap(currentPosition);
                            double dist = fabs(pos_loc.z());
                            //get the nearest point from the center of gap
                            if(!WillFilter&&RemDist>dist)
                            {
                                RemPositon = currentPosition;
                                RemMomentum = currentMomentum;
                                RemXpErr = XpErr;
                                RemDist = dist;
                                RemVol = oldVolumeName;
                            }

                            //if find the nearest point in the gap, Open Fillter 
                            if(WillFilter)
                            {          
                                ExtMucKal* muckal = new ExtMucKal();
                                muckal->SetGapID(RemID);
                                muckal->SetPosMomErr(RemPositon,RemMomentum,RemXpErr);
                                muckal->SetMucDigiCol(m_mucdigicol);
                                muckal->SetMucWindow(myMucWindow);
                                bool iniOK = muckal->MucKalIniti();
                                vector<MucRecHit*> tmp = muckal->TrackHit();
                                bool filter = muckal->ExtMucFilter();
                                //	bool filter = muckal->GetFilterStatus();
                                double chi2 = muckal->GetChi2();
                                bool samestrip = muckal->GetSameStrip();
                                if(filter&&iniOK)
                                {        

                                    myMucnfit_++;
                                    //cout<<"myMucnfit_: "<<myMucnfit_<<endl;
                                    myMucchisq_ = myMucchisq_+chi2;
                                    muckal->XPmod(m_pos_mod,m_mom_mod,m_err_mod);
                                    RememberID = muckal->GetHitGap();
                                    if(RememberID[0]==1)myMucbrLastLay_=RememberID[2];
                                    if(RememberID[0]==0||RememberID[0]==2)myMucecLastLay_=RememberID[2];
                                    if(oldVolumeName.contains("Ab"))
                                      RemDepth = RemDepth-depth;
                                    if(myMucnfit_==1)
                                      myMucdepth_ = RemDepth;
                                    else
                                      myMucdepth_=myMucdepth_+RemDepth;

                                    if(!samestrip)
                                    {

                                        extXpErr->put_err(m_err_mod);
                                        extXpErr->set_pos(m_pos_mod);
                                        extXpErr->set_mom(m_mom_mod);
                                        currentStep->GetTrack()->SetTrackStatus(fStopAndKill);

                                    }
                                    else
                                    {
                                        //cout<<"-------hit exist, but no modify-------"<<endl;
                                        RemPositon = currentPosition;
                                        RemMomentum = currentMomentum;
                                        RemXpErr = XpErr;
                                        RemDist = dist_b;
                                        RemID =ID_2;
                                        if(oldVolumeName.contains("Ab"))
                                          RemDepth = depth;
                                        else
                                          RemDepth = 0.;
                                    }
                                    if(msgFlag)cout<<"---------Filter is OK---------- "<<endl;
                                }
                                else
                                {
                                    // if(LastLay_)
                                    //{
                                    RemPositon = currentPosition;
                                    RemMomentum = currentMomentum;
                                    RemXpErr = XpErr;
                                    RemDist = dist_b;
                                    RemID =ID_2;
                                    // }
                                }
                                delete muckal;
                            }
                        }
                        if(myExtTrack)myExtTrack->SetMucKalData(myMucchisq_,myMucnfit_,myMucdepth_,myMucbrLastLay_,myMucecLastLay_,myMucnhits_);
                    }

                    /*
                       if(Flag1&&Flag2&&Flag3)
                       {
                       ExtMucKal* muckal = new ExtMucKal();
                       muckal->SetVolume(oldVolume,newVolume);
                       muckal->SetMomPosErr(currentPosition,currentMomentum,XpErr);
                       muckal->SetMucDigiCol(mucdigicol);
                       muckal->ExtMucFilter();
                       muckal->XPmod(m_pos_mod,m_mom_mod,m_err_mod);
                       bool filter = muckal->GetFilterStatus();
                       double chi2 = muckal->GetChi2();
                       if(filter)
                       {
                       myMucnfit_++;
                       myMucchisq_ = myMucchisq_+chi2;
                       currentStep->GetTrack()->SetTrackStatus(fStopAndKill);
                       RememberVol.assign(oldVolumeName,10);
                       extXpErr->put_err(m_err_mod);
                       extXpErr->set_pos(m_pos_mod);
                       extXpErr->set_mom(m_mom_mod);
                       }
                       delete muckal;
                       }
                       */
                    //   if(myExtTrack)myExtTrack->SetMucKalData(myMucchisq_,myMucnfit_,myMucdepth_,myMucbrLastLay_,myMucecLastLay_,myMucnhits_);
                    /*		//Get Muc Ext Hits Data.
                            if(newVolumeName.contains("Strip"))
                            {
                            int newVolumeNumber = newVolume->GetCopyNo();
                            if(myExtTrack)
                            {
                            ExtMucHit aExtMucHit;
                            Hep3Vector xVector(1.0,0,0); 
                            Hep3Vector yVector(0,1.0,0);
                            Hep3Vector zVector(0,0,1.0);
                            double xError = extXpErr->get_plane_err(currentMomentum.unit(),xVector);
                            double yError = extXpErr->get_plane_err(currentMomentum.unit(),yVector);
                            double zError = extXpErr->get_plane_err(currentMomentum.unit(),zVector);
                            double tzError;
                            double phi = currentPosition.phi();
                            if(phi<0.) phi+=M_PI;
                            int i = int(8.0*phi/M_PI);
                            if( i==0 || i==7 || i==8 )
                            {
                            tzError = yError;
                            }
                            if( i==1 || i==2 )
                            {
                            Hep3Vector tzVector(-1.0,1.0,0.);
                            tzError =extXpErr->get_plane_err(currentMomentum.unit(),tzVector.unit());
                            }
                            if( i==3 || i==4 )
                            {
                            tzError = xError;
                            } 
                            if( i==5 || i==6 )
                            {
                            Hep3Vector tzVector(1.0,1.0,0.);
                            tzError =extXpErr->get_plane_err(currentMomentum.unit(),tzVector.unit());
                            }
                            aExtMucHit.SetExtMucHit(currentPosition,currentMomentum,newVolumeName,newVolumeNumber,extXpErr->get_err(),zError,tzError,xError,yError);
                            myExtTrack->AddExtMucHit(aExtMucHit);				
                            }
                            }
                            */
        }
        else {
            if(msgFlag) cout<<"x:"<<currentPosition.x()<<"//y:"<<currentPosition.y()<<"//z:"<<currentPosition.z()<<"||px:"<<currentMomentum.x()<<"//py:"<<currentMomentum.y()<<"//pz:"<<currentMomentum.z()<<endl;
            double x = currentPosition.x();
            double y = currentPosition.y();
            double z = currentPosition.z();
            if( (fabs(x)>=2*myMucR) || (fabs(y)>=2*myMucR) || (fabs(z)>=2*myMucZ) )
              //currentTrack->SetKineticEnergy(0.0);// protection in case that a very energetic track travels without touching BESIII                                                                                        
            {currentStep->GetTrack()->SetTrackStatus(fStopAndKill);m_trackstop=true;}
        }

    }
    else if(currentTrackStatus == fStopAndKill)
    {
        m_trackstop=true;
        if(myEmcFlag) myExtTrack->SetEmcPath(myPathInCrystal/10.);
        if(myTof1Flag) myExtTrack->setPathInTof1(myPathInTof1);
        if(myTof2Flag) myExtTrack->setPathInTof2(myPathInTof2);
        if(msgFlag) {
            cout << "myPathInTof1 = " ;
            for(int i=0; i!=myPathInTof1.size(); i++)
              cout << myPathInTof1[i] << "  ";
            cout << endl;
            cout << "myPathInTof2 = " ;
            for(int i=0; i!=myPathInTof2.size(); i++)
              cout << myPathInTof2[i] << "  ";
            cout << endl;
        }

        if(msgFlag)
        {
            if(newVolume!=0)
            {
                std::cout<<"x:"<<currentPosition.x()<<"//y:"<<currentPosition.y()<<"//z:"<<currentPosition.z()<<"||px:"<<currentMomentum.x()<<"//py:"<<currentMomentum.y()<<"//pz:"<<currentMomentum.z()<<"//stoped"<<endl;
                cout<<"Error matrix is:"<<extXpErr->get_err()<<endl;
            }
            else {
                cout<<"x:"<<currentPosition.x()<<"//y:"<<currentPosition.y()<<"//z:"<<currentPosition.z()<<"||px:"<<currentMomentum.x()<<"//py:"<<currentMomentum.y()<<"//pz:"<<currentMomentum.z()<<"//escaped"<<std::endl;
                std::cout<<"Error matrix is:"<<extXpErr->get_err()<<std::endl;
            }
        }
    }
}



void ExtSteppingAction::CalculateChicc(G4Material* currentMaterial)
{	
    int n = currentMaterial->GetNumberOfElements();
    const double *p = currentMaterial->GetFractionVector();
    double density = (currentMaterial->GetDensity())/(g/cm3);
    double temp=0.0;
    for(int i=0;i<n;i++)
    {
        const G4Element* mElement = currentMaterial->GetElement(i);
        double z = mElement->GetZ();
        double a = mElement->GetN();
        //	std::cout<<"Fraction: "<<*p<<" z: "<<z<<" a: "<<a<<std::endl;
        temp += *(p++)/a*z*(z+1);
    }
    chicc = 0.39612e-3*std::sqrt(density*temp);		
    //   std::cout<<"chicc:"<<chicc<<std::endl;
}


HepSymMatrix & ExtSteppingAction::myOutputSymMatrix(const HepSymMatrix & m1)
{
    //  std::cout<<"myOutputSymMatrix:1"<<std::endl;
    HepSymMatrix m;
    //  std::cout<<"myOutputSymMatrix:2"<<std::endl;
    m=m1;
    //  std::cout<<"myOutputSymMatrix:3"<<std::endl;
    for(int i=0;i<6;i++)
    {  for(int j=0;j<=i;j++)
        {  if(i<3) {
                       m[i][j]=m[i][j]/100;     //mm*mm --> cm*cm
                   }
        else if(j<3) {
            m[i][j]=m[i][j]/10000;   //mm*MeV --> cm*GeV
        }
        else {
            m[i][j]=m[i][j]/1000000; //MeV*MeV --> GeV*GeV
        }
        }	
    }
    //  std::cout<<"myOutputSymMatrix:4"<<std::endl;
    //  std::cout<<"m1="<<m1<<std::endl;
    //  std::cout<<"m="<<m<<std::endl;
    myOutputSM=m;
    return myOutputSM;
}

void ExtSteppingAction::CalculateEmcEndThetaPhi(int npart, int sector, int nb, int &ntheta, int &nphi)
{
    if((sector>=0)&&(sector<3))
      sector+=16;
    //if((sector!=7)&&(sector!=15))
    {
        if((nb>=0)&&(nb<4))
        {
            ntheta = 0;
            nphi = (sector-3)*4+nb;
        }
        else if((nb>=4)&&(nb<8))
        {
            ntheta = 1;
            nphi = (sector-3)*4+nb-4;
        }
        else if((nb>=8)&&(nb<13))
        {
            ntheta = 2;
            nphi = (sector-3)*5+nb-8;
        }
        else if((nb>=13)&&(nb<18))
        {
            ntheta = 3;
            nphi = (sector-3)*5+nb-13;
        }
        else if((nb>=18)&&(nb<24))
        {
            ntheta = 4;
            nphi = (sector-3)*6+nb-18;
        }
        else if((nb>=24)&&(nb<30))
        {
            ntheta = 5;
            nphi = (sector-3)*6+nb-24;
        }
    }

    if(npart==2)
    {
        switch(ntheta)
        {
            case 0:
                if(nphi<32)
                  nphi = 31-nphi;
                else
                  nphi = 95-nphi;
                break;
            case 1:
                if(nphi<32)
                  nphi = 31-nphi;
                else
                  nphi = 95-nphi;
                break;
            case 2:
                if(nphi<40)
                  nphi = 39-nphi;
                else
                  nphi = 119-nphi;
                break;
            case 3:
                if(nphi<40)
                  nphi = 39-nphi;
                else
                  nphi = 119-nphi;
                break;
            case 4:
                if(nphi<48)
                  nphi = 47-nphi;
                else
                  nphi = 143-nphi;
                break;
            case 5:
                if(nphi<48)
                  nphi = 47-nphi;
                else
                  nphi = 143-nphi;
            default:
                ;
        }
    }
}

int ExtSteppingAction::CalculateEmcEndPhiNb(int num)
{
    if(num==0||num==1) {
        return 15-num*8;
    } else if(num==2||num==3) {
        return 30-num*8;
    } else if(num<=9) {
        return 17-num;
    } else {
        return 15-num;
    }
}

int ExtSteppingAction::CalculateEmcEndCopyNb(int num)
{
    int copyNb;
    switch(num){
        case 30:
            copyNb = 5;
            break;
        case 31:
            copyNb = 6;
            break;
        case 32:
            copyNb = 14;
            break;
        case 33:
            copyNb = 15;
            break;
        case 34:
            copyNb = 16;
            break;
        default:
            copyNb = num;
            break;
    }
    return copyNb;
}

void ExtSteppingAction::InfmodMuc(Hep3Vector &pos,Hep3Vector &mom,HepSymMatrix &err)
{
    pos = m_pos_mod;
    mom = m_mom_mod;
    err = m_err_mod;
}


Hep3Vector ExtSteppingAction::GetGapID(G4String vol)
{
    int par(-1),se(-1),ga(-1);
    G4String strPart       = vol.substr(5,1);
    //G4String strSeg        = m_MotherVolumeName.substr(7,1);

    G4String strSeg        = vol.substr(7,1);
    G4String strGap;
    if(vol.contains("G")) strGap= vol.substr(9,1);
    if(vol.contains("Ab")) strGap= vol.substr(10,1);
    std::istrstream partBuf(strPart.c_str(),             strlen(strPart.c_str()));
    std::istrstream segBuf(strSeg.c_str(),               strlen(strSeg.c_str()));
    std::istrstream gapBuf(strGap.c_str(),               strlen(strGap.c_str()));

    partBuf       >> par ;
    segBuf        >> se;
    gapBuf        >> ga;
    if(vol.contains("Ab")&&par==1) ga = ga+1;
    //panelBuf      >> m_panel;
    Hep3Vector vec;
    vec[0]=par;
    vec[1]=se;
    vec[2]=ga;
    return vec;
}


