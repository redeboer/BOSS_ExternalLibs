#include "EventAssembly/EventAssemblyAlg.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/PropertyMgr.h"
//#include "GaudiKernel/INTupleSvc.h"
//#include "GaudiKernel/IHistogramSvc.h"

#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecObject.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvTimeEvent/RecEsTime.h"
#include "Identifier/Identifier.h"
#include "Identifier/TofID.h"


/////////////////////////////////////////////////////////////////////////////
EventAssemblyAlg::EventAssemblyAlg(const std::string& name, ISvcLocator* pSvcLocator):Algorithm(name, pSvcLocator)
{
  //Declare the properties
  declareProperty("MsgFlag",myMsgFlag=false);
  declareProperty("ActiveCutFlag",myActiveCutFlag=true);
  declareProperty("EmcThetaMatchCut",myEmcThetaCut=-1);
  declareProperty("EmcPhiMatchCut",myEmcPhiCut=-1);
  declareProperty("EmcThetaNSigmaCut",myEmcThetaNSigmaCut=5);
  declareProperty("EmcPhiNSigmaCut",myEmcPhiNSigmaCut=5);
  declareProperty("ParticleId",myParticleId);
  declareProperty("Output", m_Output = 0);
}

//////////////////////////////////////////
StatusCode EventAssemblyAlg::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
  
  if(myEmcThetaCut<0) myEmcThetaCut = 0.1745;
  if(myEmcPhiCut<0) myEmcPhiCut = 0.2618;
  
  log << MSG::INFO <<"EmcThetaCut,EmcPhiCut: "<<myEmcThetaCut<<", "<<myEmcPhiCut<<endreq;
  log << MSG::INFO <<"emcThetaNSigmaCut, emcPhiNSigmaCut: "<<myEmcThetaNSigmaCut<<", "<<myEmcPhiNSigmaCut<<endreq;
  
  if(m_Output == 1){
    NTuplePtr nt1(ntupleSvc(), "FILE998/match");
    if ( nt1 ) myNTuple1 = nt1;
    else {
      myNTuple1 = ntupleSvc()->book("FILE998/match", CLID_ColumnWiseTuple, "match");
      if ( myNTuple1 )    {
        myNTuple1->addItem("exted",myExted);
        myNTuple1->addItem("matched",myMatched);
        myNTuple1->addItem("Ematched",myEnergyMatched);
      }
      else    {
        log << MSG::ERROR << " Cannot book N-tuple:" << long(myNTuple1) << endmsg;
        //return StatusCode::FAILURE;
      }
    }
  }
	return StatusCode::SUCCESS;
}

///////////////////////////////////////
StatusCode EventAssemblyAlg::execute()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  int numOfChargedTrks = 0;
  int numOfKalTrks = 0;
  int numOfDedx = 0;
  int numOfExt = 0;
  int numOfTof = 0;
  int numOfShower = 0;
  int numOfMatchedShower = 0;
  int numOfMucTrks = 0;
	
  // Part 1: Get the event header, print out event and run number
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader)
    {
      log << MSG::FATAL << "Could not find Event Header" << endreq;
      return( StatusCode::FAILURE);
    }
  log << MSG::INFO << ": retrieved event: " << eventHeader->eventNumber()  << "  run: " <<  eventHeader->runNumber() << endreq;

  //Part 2: Retrieve Rec data
  SmartDataPtr<RecMdcTrackCol> recMdcTrackCol(eventSvc(),EventModel::Recon::RecMdcTrackCol);
  RecMdcTrackCol::iterator beginOfMdcTrkCol = RecMdcTrackCol::iterator(NULL);
  if(!recMdcTrackCol)
    {
      log<<MSG::INFO<<"Could not find RecMdcTrackCol!"<<endreq;
    }
  else 
    {
      beginOfMdcTrkCol = recMdcTrackCol->begin();
      numOfChargedTrks = recMdcTrackCol->size();
    }
  
  SmartDataPtr<RecMdcKalTrackCol> mdcKalTrackCol(eventSvc(), EventModel::Recon::RecMdcKalTrackCol);
  RecMdcKalTrackCol::iterator beginOfMdcKalTrackCol = RecMdcKalTrackCol::iterator(NULL);
  if(!mdcKalTrackCol)
    {
      log<<MSG::INFO<<"Could not find MdcKalTrackCol!"<<endreq;
    }
  else 
    {
      numOfKalTrks = mdcKalTrackCol->size();
      beginOfMdcKalTrackCol = mdcKalTrackCol->begin();
    }
		
  
  SmartDataPtr<RecMdcDedxCol> mdcDedxCol(eventSvc(), EventModel::Recon::RecMdcDedxCol);
  RecMdcDedxCol::iterator beginOfDedxCol = RecMdcDedxCol::iterator(NULL);
  if(!mdcDedxCol)
    {
      log<<MSG::INFO<<"Could not find MdcDedxCol!"<<endreq;
    }
    else {
      numOfDedx = mdcDedxCol->size();
      beginOfDedxCol = mdcDedxCol->begin();
    }
  

  SmartDataPtr<RecExtTrackCol> extTrackCol(eventSvc(), EventModel::Recon::RecExtTrackCol);
  RecExtTrackCol::iterator beginOfExtTrackCol = RecExtTrackCol::iterator(NULL);
  if(!extTrackCol)
    {
      log<<MSG::INFO<<"Could not find RecExtTrackCol!"<<endreq;
    }
  else
    {
      beginOfExtTrackCol = extTrackCol->begin();
      numOfExt = extTrackCol->size();
    }
  
  SmartDataPtr<RecTofTrackCol> TofTrackCol(eventSvc(), EventModel::Recon::RecTofTrackCol);
  RecTofTrackCol::iterator beginOfTofTrackCol = RecTofTrackCol::iterator(NULL);
  if(!TofTrackCol)
    {
      log<<MSG::INFO<<"Could not find RecTofTrackCol!"<<endreq;
    }
  else
    {
      beginOfTofTrackCol = TofTrackCol->begin();
      numOfTof = TofTrackCol->size();
    }
	
  SmartDataPtr<RecEmcShowerCol> emcRecShowerCol(eventSvc(), EventModel::Recon::RecEmcShowerCol);
  RecEmcShowerCol::iterator beginOfEmcTrackCol = RecEmcShowerCol::iterator(NULL);
  if(!emcRecShowerCol)
    {
      log<<MSG::INFO<<"Could not find RecEmcShowerCol!"<<endreq;
    }
  else
    {
      beginOfEmcTrackCol = emcRecShowerCol->begin();
      numOfShower = emcRecShowerCol->size();
    }
	
  SmartDataPtr<RecMucTrackCol> mucTrackCol(eventSvc(), EventModel::Recon::RecMucTrackCol);
  RecMucTrackCol::iterator beginOfMucTrackCol = RecMucTrackCol::iterator(NULL);
  if(!mucTrackCol)
    {
      log<<MSG::INFO<<"Could not find RecMucTrackCol!"<<endreq;
    }
  else
    {
      beginOfMucTrackCol = mucTrackCol->begin();
      numOfMucTrks = mucTrackCol->size();
    }

    //Some match flag
    
    int MaxHit = numOfChargedTrks+numOfShower+numOfTof;

    vector<bool> dEdxMatched;
    vector<bool> kalTrkMatched;
    vector<bool> extMatched;
    vector<bool> TofMatched;
    vector<bool> emcMatched;
    vector<bool> mucMatched;

    for(int i=0;i<MaxHit;i++)
    {
      dEdxMatched.push_back(false);
      kalTrkMatched.push_back(false);
      extMatched.push_back(false);
      TofMatched.push_back(false);
      emcMatched.push_back(false);
      mucMatched.push_back(false);
    }


  //Part 3: Match tracks
  //  RecTrackListCol* aNewRecTrkListCol = new RecTrackListCol;
  EvtRecTrackCol* aNewEvtRecTrackCol = new EvtRecTrackCol;
  if(numOfChargedTrks+numOfShower)//if mdc trackCol and emc trackCol neither exists,there is no match job.
    {
      for(int i=0;i<numOfChargedTrks;i++)//match charged tracks
	{
	  EvtRecTrack* aNewEvtRecTrack = new EvtRecTrack;
	  int mdcTrkID = (*(beginOfMdcTrkCol+i))->trackId();
	  aNewEvtRecTrack->setTrackId(mdcTrkID);
	  aNewEvtRecTrack->setMdcTrack(*(beginOfMdcTrkCol+i));
	  //	  aNewEvtRecTrack->setMdcTrkIdx(i);
	  for(int j=0;j<numOfKalTrks;j++)//match MdcKalTrack
	    if( !kalTrkMatched[j] && mdcTrkID==(*(beginOfMdcKalTrackCol+j))->trackId() )
	      {
		aNewEvtRecTrack->setMdcKalTrack(*(beginOfMdcKalTrackCol+j));
		//		aNewRecTrkList->setMdcKalTrkIdx(j);
		kalTrkMatched[j]=true;
	      }
	  
	  for(int j=0;j<numOfDedx;j++)//match dEdx
	    if( !dEdxMatched[j] && mdcTrkID==(*(beginOfDedxCol+j))->trackId() )
	      {
		aNewEvtRecTrack->setMdcDedx(*(beginOfDedxCol+j));
		//		aNewRecTrkList->setDedxTrkIdx(j);
		dEdxMatched[j]=true;
	      }
	    
	  for(int j=0;j<numOfExt;j++)//match ExtTrack
	    if( !extMatched[j] && mdcTrkID==(*(beginOfExtTrackCol+j))->trackId() )
	      {
		aNewEvtRecTrack->setExtTrack(*(beginOfExtTrackCol+j));
		//		aNewRecTrkList->setExtTrkIdx(j);
		extMatched[j]=true;
	      }
	  for(int j=0;j<numOfTof;j++)//match Tof
	    if( !TofMatched[j] && mdcTrkID==(*(beginOfTofTrackCol+j))->trackID() )
	      {
		aNewEvtRecTrack->addTofTrack(*(beginOfTofTrackCol+j));
		//		aNewRecTrkList->setTofTrkIdx(j);
		TofMatched[j]=true;
	      }
	  for(int j=0;j<numOfMucTrks;j++)//match MucTrack
	    if( !mucMatched[j] && mdcTrkID==(*(beginOfMucTrackCol+j))->trackId() )
	      {
		aNewEvtRecTrack->setMucTrack(*(beginOfMucTrackCol+j));
		//		aNewRecTrkList->setMucTrkIdx(j);
		mucMatched[j]=true;
	      }

	  if(m_Output==1){
	    myExted = -1.;
	    myMatched = -1.;
	    myEnergyMatched = -1.;
	  }
	  if(aNewEvtRecTrack->isExtTrackValid()&&(aNewEvtRecTrack->extTrack())->emcVolumeNumber()!=-1)//start to match Emc showers
	    {
	      if(m_Output==1)myExted = 1.;
	      //resieve ext information @ Emc
	      Hep3Vector extPos = (aNewEvtRecTrack->extTrack())->emcPosition();
	      double extTheta = extPos.theta();
	      double extPhi = extPos.phi();
	      
	      double dAngleMin = 9999.;
	      int indexOfEmcNearest = -1;
	      
	      for(int j=0;j<numOfShower;j++)//start to find nearest emc shower
		{
		  if(!emcMatched[j])
		    {
		      HepPoint3D emcPot = (*(beginOfEmcTrackCol+j))->position();
		      Hep3Vector emcPos(emcPot.x(),emcPot.y(),emcPot.z());
		      double dAngle = extPos.angle(emcPos);
		      if(dAngle<dAngleMin)
			{
			  dAngleMin = dAngle;
			  indexOfEmcNearest = j;
			}
		    }
		}
	      if(indexOfEmcNearest>=0)//if nearest Emc shower is found
		{
		  HepPoint3D emcPot = (*(beginOfEmcTrackCol+indexOfEmcNearest))->position();
		  double theta = emcPot.theta();
		  double phi = emcPot.phi();
		  double deltaTheta = (extTheta-theta);
		  double deltaPhi = fmod(extPhi-phi+5*M_PI,2*M_PI)-M_PI;
		  if(myActiveCutFlag)
		    {
		      double tanLamda = (*(beginOfMdcTrkCol+i))->helix(4);
		      double kappa = (*(beginOfMdcTrkCol+i))->helix(2);
		      double p = sqrt(1+tanLamda*tanLamda)/fabs(kappa);
		      double aThetaCut = thetaCut(p,myEmcThetaNSigmaCut,myParticleId);
		      double aPhiCUt = phiCut(p,myEmcPhiNSigmaCut,myParticleId);
		      if(fabs(deltaTheta)<aThetaCut  && fabs(deltaPhi)<aPhiCUt)
			{
			  aNewEvtRecTrack->setEmcShower(*(beginOfEmcTrackCol+indexOfEmcNearest));
			  //			  aNewRecTrkList->setEmcTrkIdx(indexOfEmcNearest);
			  emcMatched[indexOfEmcNearest]=true;
			  numOfMatchedShower++;
			  if(m_Output==1)myMatched = 1.;
			}
		    }
		  else if(fabs(deltaTheta)<myEmcThetaCut && fabs(deltaPhi)<myEmcPhiCut)//if the nearest shower is in the fixed match window,it will be matched.
		    {
		      aNewEvtRecTrack->setEmcShower(*(beginOfEmcTrackCol+indexOfEmcNearest));
		      //		      aNewRecTrkList->setEmcTrkIdx(indexOfEmcNearest);
		      emcMatched[indexOfEmcNearest]=true;
		      numOfMatchedShower++;
		      if(m_Output==1)myMatched = 1.;
		    }
		}
	    }					
	  aNewEvtRecTrackCol->push_back(aNewEvtRecTrack);
	  if(m_Output==1){
	    myEnergyMatched = 0.;
	    myNTuple1->write();
	  }
	}//end charged track match
		
      int id=0;
      for(int i=0;i<numOfShower;i++)//If Emc shower isn't macthed, it will be added to the end of RecTrackListCol
	{
	  if(!emcMatched[i])
	    {
	      EvtRecTrack* aNewEvtRecTrack = new EvtRecTrack;
	      aNewEvtRecTrack->setTrackId(id+numOfChargedTrks);
	      aNewEvtRecTrack->setEmcShower(*(beginOfEmcTrackCol+i));
	      //	      aNewRecTrkList->setEmcTrkIdx(i);

  //EMC and TOF match
        HepPoint3D emcPos((*(beginOfEmcTrackCol+i))->position());
        double emcPhi = emcPos.phi();
        emcPhi = emcPhi < 0 ? emcPhi+CLHEP::twopi : emcPhi;
        
        int module=9999, nphi1=9999, nphi2=9999, nphi1_mrpc=9999;
        module = (*(beginOfEmcTrackCol+i))->module();
        if(module==1) {    //barrel
          nphi1 = (int)(emcPhi*88./CLHEP::twopi);      //tof layer1 number
          nphi2 = (int)(emcPhi*88./CLHEP::twopi+0.5);  //tof layer2 number
          nphi2 += 88;
        } else {    //endcap
          nphi1 = (int)(emcPhi*48./CLHEP::twopi+0.5);
	  

      //new mrpc detector 
	  double fi_endtof_mrpc = atan2(emcPos.y(),emcPos.x())-3.141592654/2.; 
	  if(fi_endtof_mrpc<0) fi_endtof_mrpc += 2*3.141592654;
	  nphi1_mrpc=((int)(fi_endtof_mrpc/(3.141593/18)))+1; //We have 36 modules in each endcap (divided again into two layers)! The numbering starts with 1.

	  if(nphi1_mrpc%2==0){nphi1_mrpc=nphi1_mrpc/2;}  //Calculate the correct number as in each endcap we have two layers of modules furnished with numbers 1 to 18.
	  else {nphi1_mrpc=(nphi1_mrpc+1)/2;}

	  if(emcPos.z()>0) {(nphi1_mrpc -= 19); nphi1_mrpc=nphi1_mrpc*(-1);} //Consider that for the east endcap the module at x=0;y=max has the number 18 (the numbering is != to west endcap)
	  // nphi1_mrpc is now the number of the nearest ToF module!
	 
	 }

        int dPhiMin = 9999;
	int partid_dPhiMin = 7;
	bool mrpcmatch = false;

        int indexOfTofNearest = -1;
        for(int j=0;j<numOfTof;j++) { //start to find nearest Tof shower
          if( !TofMatched[j] ) {

            Identifier id((*(beginOfTofTrackCol+j))->tofID());
	    bool is_mrpc = TofID::is_mrpc(id);

	    if(!is_mrpc)
	      {
		int barrel_ec = TofID::barrel_ec(id);
		int layer = TofID::layer(id);
		int im = TofID::phi_module(id);
		im += layer * 88;

		if(module!=barrel_ec) continue;
            
		int dPhi;
		if(layer == 0) {
		  dPhi = abs(im - nphi1);
		} else {
		  dPhi = abs(im - nphi2);
		}
		if(module==1) {
		  dPhi = dPhi>=44 ? 88-dPhi : dPhi;
		} else {
		  dPhi = dPhi>=24 ? 48-dPhi : dPhi;
		} 

		if(dPhi < dPhiMin) {
		  dPhiMin = dPhi;
		  indexOfTofNearest = j;
		}
	      }//close if(!is_mrpc)
	    else//is_mrpc
	      {
		int barrel_ec = TofID::barrel_ec(id);
		int module_mrpc = TofID::module(id);
		
		int dphi;
		dphi = abs(module_mrpc-nphi1_mrpc);
		
		if(dphi < dPhiMin)
		  {
		    dPhiMin=dphi;
		    partid_dPhiMin=barrel_ec;
		    indexOfTofNearest = j;
		    mrpcmatch = true;
		  }
	      }//close else is_mrpc

          }//close if( !TofMatched[j] ) 
        }//close for(int j=0;j<numOfTof;j++)

	/*
	if(0)
	  {
	cout << "Event Assembly Algorithm" <<endl;
	cout << "EMC Shower number   =  " << i << endl;
	cout << "mrpcmatch ?            " << mrpcmatch << endl;
	cout << "indexOfTofNearest      " << indexOfTofNearest <<endl;
	cout << "dphiMin                " << dPhiMin << endl;
	cout << "partid_mrpc            " << partid_dPhiMin <<endl;
	cout << "partid_emc             " << module  <<endl <<endl;
	  }
	*/

        //match in z-direction
        if(indexOfTofNearest>=0) {
          HepPoint3D tofPos(0,0.867,(*(beginOfTofTrackCol+indexOfTofNearest))->zrhit());
          double matWindow = 0;                    
          double eShower = (*(beginOfEmcTrackCol+i))->e5x5();//energy of a 5x5 cluster
          if(eShower>0) {
            matWindow = 0.01277+0.004268/sqrt(eShower);
          }
        }


        if(indexOfTofNearest>=0 && dPhiMin<=1) {  //if nearest Tof shower is found
          // calculate matching window for barrel
          double eShower = (*(beginOfEmcTrackCol+i))->e5x5();
          double matWindow = 0;
          if(eShower>0) {
            matWindow = 0.01277+0.004268/sqrt(eShower);
          }
          matWindow = 0.2;  //temp code

          HepPoint3D tofPos(0,0.867,(*(beginOfTofTrackCol+indexOfTofNearest))->zrhit());
          if(fabs(tofPos.theta()-emcPos.theta())<matWindow || module!=1) {
            aNewEvtRecTrack->addTofTrack(*(beginOfTofTrackCol+indexOfTofNearest));
            TofMatched[indexOfTofNearest]=true;
	    //cout << "EventAssembly :  New neutral Toftrack added  " << endl;

          }
        }//close if(indexOfTofNearest>=0 && dPhiMin<=1)

	      aNewEvtRecTrackCol->push_back(aNewEvtRecTrack);
	      id++;
	    }//close if(!)
	}//close for(int i=0;i<numOfShower;i++) <---This are the EMC showers
    }//end match

  //match statistics
  if(myMsgFlag) cout<<"Charged tracks: "<<numOfChargedTrks<<"	Ext track: "<<numOfExt  <<"  Shower:"<<numOfShower<<"  Matched shower:"<<numOfMatchedShower<<endl;

  //Part 4: register RecTrackListCol and EventList in TDS
  DataObject *aDataObject1;
  eventSvc()->findObject(EventModel::EvtRec::Event, aDataObject1);
  if ( aDataObject1 == NULL ) {
     EvtRecObject* aRecObject = new EvtRecObject; 
     StatusCode sc = eventSvc()->registerObject(EventModel::EvtRec::Event, aRecObject);
     if ( sc.isFailure() ) {
	log << MSG::FATAL << "Could not register EvtRecObject in TDS!" << endreq;
	return StatusCode::FAILURE;
     }
  }

  DataObject* aDataObject2;
  eventSvc()->findObject(EventModel::EvtRec::EvtRecEvent, aDataObject2);
  if ( aDataObject2 == NULL ) {
     EvtRecEvent* aRecEvent = new EvtRecEvent;
     aRecEvent->setTotalTracks(numOfChargedTrks+numOfShower-numOfMatchedShower);
     aRecEvent->setTotalCharged(numOfChargedTrks);
     aRecEvent->setTotalNeutral(numOfShower-numOfMatchedShower);
     StatusCode sc = eventSvc()->registerObject(EventModel::EvtRec::EvtRecEvent, aRecEvent);
     if ( sc.isFailure() ) {
	log << MSG::FATAL << "Could not register EvtRecEvent in TDS!" << endreq;
	return( StatusCode::FAILURE);
     }
  }
  else {
     EvtRecEvent* aRecEvent = dynamic_cast<EvtRecEvent*>(aDataObject2);
     aRecEvent->setTotalTracks(numOfChargedTrks+numOfShower-numOfMatchedShower);
     aRecEvent->setTotalCharged(numOfChargedTrks);
     aRecEvent->setTotalNeutral(numOfShower-numOfMatchedShower);
  }

  DataObject* aDataObject3;
  eventSvc()->findObject(EventModel::EvtRec::EvtRecTrackCol, aDataObject3);
  if ( aDataObject3 != NULL ) {
    //IDataManagerSvc* dataMgrSvc = dynamic_cast<IDataManagerSvc*> (eventSvc());
    SmartIF<IDataManagerSvc> dataMgrSvc(eventSvc());
     dataMgrSvc->clearSubTree(EventModel::EvtRec::EvtRecTrackCol);
     eventSvc()->unregisterObject(EventModel::EvtRec::EvtRecTrackCol);
  }
  
  StatusCode sc = eventSvc()->registerObject(EventModel::EvtRec::EvtRecTrackCol, aNewEvtRecTrackCol);
  if ( sc.isFailure() ) {
     log << MSG::FATAL << "Could not register EvtRecTrackCol in TDS!" << endreq;
     return( StatusCode::FAILURE);
  }
	
  //Part 5: check RecTrackListCol in TDS
  SmartDataPtr<EvtRecTrackCol> evtRecTrackCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);
  if(!evtRecTrackCol)
    {
      log<<MSG::FATAL<<"Could not find RecTrackListCol in TDS!"<<endreq;
    }
  
  EvtRecTrackCol::iterator itOfRecTrkListCol = evtRecTrackCol->begin();
  if(myMsgFlag)
    for(int i=0;itOfRecTrkListCol!=evtRecTrackCol->end();itOfRecTrkListCol++,i++)
      {
	cout<<"******************** Track "<<i<<": *********************"<<endl;
	cout<<"Track ID: "<<(*itOfRecTrkListCol)->trackId()<<endl;
	if((*itOfRecTrkListCol)->isMdcTrackValid())
	  {
	    RecMdcTrack* mdcTrk = (*itOfRecTrkListCol)->mdcTrack();
	    double kappa = mdcTrk->helix(2);
	    double tanl = mdcTrk->helix(4);
	    cout<<"Mdc kappa, tanl: "<<kappa<<", "<<tanl<<endl;
	  }
	if((*itOfRecTrkListCol)->isExtTrackValid())
	  {
	    RecExtTrack* extTrack = (*itOfRecTrkListCol)->extTrack();
	    Hep3Vector emcPos = extTrack->emcPosition();
	    cout<<"Ext Emc pos:"<<emcPos.x()<<","<<emcPos.y()<<","<<emcPos.z()<<endl;
	  }
	if((*itOfRecTrkListCol)->isEmcShowerValid())
	  {
	    RecEmcShower* emcTrack = (*itOfRecTrkListCol)->emcShower();
	    HepPoint3D emcPos = emcTrack->position();
	    double x = emcPos.x();
	    double y = emcPos.y();
	    double z = emcPos.z();
	    cout<<"Emc rec pos:"<<x<<","<<y<<","<<z<<endl;
	  }
      }
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EventAssemblyAlg::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;
  
  return StatusCode::SUCCESS;
}



//////////////////////////////
double EventAssemblyAlg::thetaCut(double p,double nSigma,int parId)
{
  if(p<0.3) p=0.3;
  if(parId<1||parId>3) parId=3;
  if(nSigma<=0) nSigma = 5.;
  double cut = myEmcThetaCut;
  switch(parId)
    {
    case 1:cut = fabs(-0.00159)+(exp(-3.566*p-4.14)+0.006804)*nSigma; break;
    case 2:cut = fabs(-0.00145)+(exp(-4.268*p-3.305)+0.01129)*nSigma; break;
    case 3:cut = fabs(-0.00161)+(exp(-5.955*p-3.052)+0.01751)*nSigma; break;
    }

  return cut;
}

///////////////////////////////
double EventAssemblyAlg::phiCut(double p,double nSigma,int parId)
{
  if(p<0.3) p=0.3;
  if(parId<1||parId>3) parId=3;
  if(nSigma<=0) nSigma = 5.;
  double cut = myEmcPhiCut;
  switch(parId)
    {
    case 1:cut = fabs(exp(-2.187*p-2.945)+0.03236)+(exp(-2.977*p-3.558)+0.005566)*nSigma; break;
    case 2:cut = fabs(exp(-2.216*p-2.13)+0.03314)+(exp(-2.436*p-3.392)+0.005049)*nSigma; break;
    case 3:cut = fabs(exp(-1.63*p-2.931)+0.03223)+(exp(-3.192*p-2.756)+0.01533)*nSigma; break;
    }
  
  return cut;
}

