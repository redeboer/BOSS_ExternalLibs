#ifndef PrintMcInfo_Header
#define PrintMcInfo_Header

#include "GaudiKernel/Algorithm.h"
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
#include "McTruth/MdcMcHit.h"
#include "McTruth/TofMcHit.h"
#include "McTruth/EmcMcHit.h"
#include "McTruth/MucMcHit.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EventModel/EventHeader.h"

#include "Identifier/MdcID.h"
#include "Identifier/TofID.h"
#include "Identifier/EmcID.h"
#include "Identifier/MucID.h"

#include "MdcRawEvent/MdcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "EmcRawEvent/EmcDigi.h"
#include "MucRawEvent/MucDigi.h"

#include "Identifier/Identifier.h"
#include <map>
#include <string>

#include <cstdlib>
#include <fstream>
#include <iomanip>

//
//namespace
using namespace std;
class PrintMcInfo:public Algorithm {
	public:
		PrintMcInfo(const std::string& name, ISvcLocator* pSvcLocator);
		~PrintMcInfo();
		StatusCode initialize();
		StatusCode beginRun();   
		StatusCode execute();
		StatusCode endRun();
		StatusCode finalize();
		void mkmap();
                void printTitle(ofstream& os,int);
                void printTree(ofstream&,Event::McParticle*,int,int);
                void printPartInf(ofstream&,Event::McParticle*,int,int);
		void printHitInf(ofstream&);
		void printHit(ofstream&,Event::MdcMcHitCol&,Event::TofMcHitCol&,Event::EmcMcHitCol&,Event::MucMcHitCol&,int&);
		void printDigi(ofstream&);
	private:
		int m_OutputLevel;
                int m_pid;//particleProperty
                int m_trkIndex;
		
		int m_fstreamOption;
                bool m_firstTime;
                map<int,string> map_pid;

		ofstream os;

                string daughters;
                string m_FileName;
	protected:

};
//add your inline methods

//

#endif//PrintMcInfo_Header
