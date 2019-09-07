#include "PrintMcInfo/PrintMcInfo.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartDataLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "TMath.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "McTruth/McParticle.h"
#include <fstream>

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EventModel/EventHeader.h"

#include <string>
using namespace std;

//------------------
PrintMcInfo::PrintMcInfo(const std::string& name, ISvcLocator* pSvcLocator):Algorithm(name,pSvcLocator)
{

	declareProperty("PrintLevel", m_OutputLevel=0);
	declareProperty("FileName", m_FileName );
	declareProperty("fstreamOption",m_fstreamOption=1);
}
PrintMcInfo::~PrintMcInfo() {  }

StatusCode PrintMcInfo::initialize(){
	//make a map for tanslating the PDG code to its name 
	PrintMcInfo::mkmap();

	if(m_fstreamOption==0)
	{
		os.open(m_FileName.c_str(),ios::app);	
	}
	else    os.open(m_FileName.c_str());





	MsgStream log(msgSvc(), name());
	log<<MSG::INFO<<"PrintMcInfo::initialize()"<<endreq;

	StatusCode status;
	return StatusCode::SUCCESS;
}
StatusCode PrintMcInfo::beginRun(){
	MsgStream log(msgSvc(), name());
	log<<MSG::INFO<<"PrintMcInfo::beginRun()"<<endreq;
	//add your code here
	return StatusCode::SUCCESS;

}


//--------------------execute----------------


StatusCode PrintMcInfo::execute(){
	MsgStream log(msgSvc(), name());
	log<<MSG::INFO<<"PrintMcInfo::execute()"<<endreq;


/*	if(m_fstreamOption==0)
	{
		os.open(m_FileName.c_str(),ios::app);	
	}
	else    os.open(m_FileName.c_str());
*/
	SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
	int runNo=eventHeader->runNumber();
	int event=eventHeader->eventNumber();
	os<<"run number:"<<runNo<<endl
		<<"event number:"<<event<<endl<<endl;


	SmartDataPtr<Event::McParticleCol> mcPartCol(eventSvc(), "/Event/MC/McParticleCol");
	if (runNo < 0) 
	{
		if(!mcPartCol)
		{
			log << MSG::ERROR << "Could not retrieve McParticelCol" << endreq;
			return StatusCode::FAILURE;
		}

		Event::McParticleCol::iterator iter_mc = mcPartCol->begin();
		for (;iter_mc !=mcPartCol->end();iter_mc++)
		{
			bool primary = (*iter_mc)->primaryParticle();
			bool gamma = ((*iter_mc)->particleProperty())==22?true:false;
			if(primary&&(!gamma))
			{	
				Event::McParticle* pMcPart = (*iter_mc);
				pMcPart->trackIndex();
				PrintMcInfo::printTree(os,pMcPart,m_OutputLevel,0);
				if(m_OutputLevel>0)
				{	
					os<<endl<<"\n- - - - - - - - - - - - - -  Information in this decay tree :   particle level  - - - - - - - - - - - - - -- - -- - -\n"<<endl;
					PrintMcInfo::printTitle(os,1);
					PrintMcInfo::printPartInf(os,pMcPart,1,0);
				}
				if(m_OutputLevel>1)
				{	
					os<<endl<<"\n- - - - - - - - - - - - - -  Information in this decay tree :      hit level  - - - - - - - - - - - - - - - - - - - -\n"<<endl;
					PrintMcInfo::printPartInf(os,pMcPart,2,0);
				}
			}			
		}
		if(m_OutputLevel>2)
		{
			os<<endl<<"\n- - - - - -  - - - - - - - -  Digi Information in this EVENT - - - - - - - - - - - - - - - - - -  -\n"<<endl;
			printDigi(os);
		}

		os<<endl<<"\n********************************   end of this event   *********************************\n"<<endl;


	}//end of if(runNo < 0)
	return StatusCode::SUCCESS;
}




StatusCode PrintMcInfo::endRun(){
	MsgStream log(msgSvc(), name());
	log<<MSG::INFO<<"PrintMcInfo::endRun()"<<endreq;
	return StatusCode::SUCCESS;

}
StatusCode PrintMcInfo::finalize(){
	MsgStream log(msgSvc(), name());
	log<<MSG::INFO<<"PrintMcInfo::finalize()"<<endreq;
	return StatusCode::SUCCESS;

}


