
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/INTuple.h" 
#include "GaudiKernel/INTupleSvc.h"
#include "BesEventMixer/MixerAlg.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h"
#include "IRawFile/RawFileExceptions.h"
//#include "DatabaseSvc/IDatabaseSvc.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include "TrigEvent/TrigGTD.h"
#include "HltEvent/DstHltInf.h"

#include "BesKernel/IBesRndmGenSvc.h"
#include "CLHEP/Random/Random.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandExponential.h"

#include "RawDataCnv/Util/MdcConverter.h"
#include "RawDataCnv/Util/MucConverter.h"
#include "RawDataCnv/Util/TofConverter.h"
#include "RawDataCnv/Util/EmcConverter.h"

#include "RootCnvSvc/RootInterface.h"

#include <vector>
#include <map>
#include <algorithm>
       
#include <libgen.h>

using namespace std;
using namespace CLHEP;

MixerAlg::MixerAlg(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator){
  declareProperty("MixMdcDigi", b_mdc=true);
  declareProperty("MixEmcDigi", b_emc=true);
  declareProperty("MixMucDigi", b_muc=true);
  declareProperty("MixTofDigi", b_tof=true);

  declareProperty("DBUserRequest", m_dbUserRequest=false);
  declareProperty("RandomTrgRun", m_run);
  declareProperty("RandomTrgRunRange", m_runs);
  declareProperty("RandomTrgTimeRange", m_dates);

  declareProperty("BackgroundDataFiles", m_bgfiles);
  declareProperty("NumRanTrgEvents", m_ranTrgEvents);
  declareProperty("NBgEventsToSignal", m_nevent=1);
  // declareProperty("LoopBgData", b_loop=true);
  declareProperty("ReplaceDataPath", m_pattern);
  declareProperty("UseNewDataDir", m_newdatadir);
  declareProperty("IfSkip", m_skip=true);
  declareProperty("NSkip", m_NSkip=150);
  declareProperty("OutPut", m_ifOutPut=false);
  declareProperty("MixingMethod", m_mixingMethod=1);
  declareProperty("MaxLoop", m_maxLoop=10);
  declareProperty("SmearT0", m_ifSmearT0=true);
  declareProperty("ReadBGMethod", m_readBGMethod=1);
  
  declareProperty("ExWireFromRun", m_exRunFrom = 0 );
  declareProperty("ExWireToRun",   m_exRunTo = 999999);

  declareProperty("UsingFilter",   m_usingFilter = true);

  m_raw_event = 0;
  m_fr = 0;
  m_runNo = 0;
  m_skipCount = 0;
  currentBGFile = ""; 
  currentMCFile = "";
  m_totalEvent = 0;
  m_totEvtNumInCurFile = 0;
  m_nEventsToEnd = 0;
  m_ranStepLenInCurrentFile.clear();

  m_mdcCnv = MdcConverter::instance();
  m_mucCnv = MucConverter::instance();
  m_tofCnv = TofConverter::instance();
  m_emcCnv = EmcConverter::instance();
}

std::string MixerAlg::prepareDbQuery()
{
  std::string query = "SELECT FilePath,FileName,NumEvent FROM RanTrgData";

  if (! m_run.empty() || m_runs.size()==2 || m_dates.size()==2)
    { // use additional parameters for query
      query = query + " WHERE ";
      bool use_and = false;
      if(! m_run.empty() )
	{
	  query = query + " RunNo=" + m_run;
	  use_and = true;
	}
      if( m_runs.size()==2 )
	{
	  if(use_and) query = query + " AND ";
	  query = query + " RunNo>=" + m_runs[0] + " AND RunNo<=" + m_runs[1];
	  use_and = true;
	}
      if( m_dates.size()==2 )
	{
	  if(use_and) query = query + " AND ";
	  query = query + " TimeSOR>='" + m_dates[0] + "' AND TimeEOR<='" + m_dates[1] + "'";
	}
    }

  query = query + ";";
  return query;
}


StatusCode MixerAlg::initialize()
{
  log = new MsgStream(messageService(), name() );
  //Caogf add
  IRealizationSvc *tmpReal;
  StatusCode status = service("RealizationSvc",tmpReal);
  if (!status.isSuccess())
    {
      (*log) << MSG::FATAL << " Could not initialize Realization Service" << endreq;
    } else {
    m_RealizationSvc=dynamic_cast<RealizationSvc*>(tmpReal);
  }

  if(m_RealizationSvc->UseDBFlag() == true && m_RealizationSvc->ifReadRandTrg() == true && m_dbUserRequest == true) {
    std::string query = prepareDbQuery();
    m_bgfiles = m_RealizationSvc->getBgFileName(query);
  }

  if(! m_pattern.empty())
  {
    for(unsigned int k = 0; k < m_bgfiles.size(); k++) {
      size_t pos_round = m_bgfiles[k].rfind("round");
      (*log) << MSG::INFO <<"m_bgfiles: "<<m_bgfiles[k]<<endreq;
      if(pos_round!=string::npos){ 
        m_bgfiles[k].replace(m_bgfiles[k].begin(), m_bgfiles[k].begin()+pos_round, m_pattern);
        (*log) << MSG::INFO<<"new random trigger data path: "<<m_bgfiles[k]<<endreq;
      }
      else{
        (*log) << MSG::ERROR<<"string 'round' not found in random trigger path!"<<endreq; 
        exit(-1);
      }
    }    
  }
  if (! m_newdatadir.empty())
  {
     for(unsigned int k = 0; k < m_bgfiles.size(); k++) {
      char tmp[255];
      std::strcpy (tmp, m_bgfiles[k].c_str()); 
      string fname = basename(tmp);
      m_bgfiles[k].replace(m_bgfiles[k].begin(), m_bgfiles[k].end(), m_newdatadir+'/'+fname);
        (*log) << MSG::INFO<<"new random trigger data path: "<<m_bgfiles[k]<<endreq;
    }
  }


  // initialize MDC converter
  m_mdcCnv->init(m_exRunFrom,m_exRunTo); 

  //caogf for random seed
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)
    {
      (*log) << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
      return RndmStatus;
    }

  //get jobSvc
  IDataInfoSvc *tmpInfoSvc;
  status = service("DataInfoSvc",tmpInfoSvc);
  if (status.isSuccess()) {
    (*log) << MSG::INFO << "get the DataInfoSvc" << endreq;
    m_jobInfoSvc=dynamic_cast<DataInfoSvc *>(tmpInfoSvc);
  }else {
    (*log) << MSG::WARNING << "could not get the DataInfoSvc." << endreq;
  }
  //end caogf add 

  if(m_RealizationSvc->UseDBFlag() == false || m_dbUserRequest == true) {
    if(m_bgfiles.empty()) {
      (*log) << MSG::WARNING << "No background datafiles found" << endreq;
      return StatusCode::SUCCESS;
    }
 
    // open background stream
    try {
      m_bgfilesIndex.clear();
      for(unsigned int bg_index = 0; bg_index < m_bgfiles.size(); bg_index++) {
        m_bgfilesIndex.push_back(m_bgfiles[bg_index] + ".idx");
      }
      if(m_skip == true && m_readBGMethod == 1) m_fr = new RawFileReader(m_bgfiles, m_bgfilesIndex) ;
      else m_fr = new RawFileReader(m_bgfiles) ;
    }
    catch (RawFileException& ex) {
      ex.print();
      return StatusCode::FAILURE;
    }

  }
  m_raw_event = new RAWEVENT;

  if(m_ifOutPut) {
    NTuplePtr nt1(ntupleSvc(), "FILE1/n1");
    if(nt1) m_tuple1 = nt1;
    else {
      m_tuple1 = ntupleSvc()->book("FILE1/n1",CLID_ColumnWiseTuple,"Field");
      if( m_tuple1 ) {
        status = m_tuple1->addItem("time1",  m_time1 );
        status = m_tuple1->addItem("time2",  m_time2 );
        status = m_tuple1->addItem("time3",  m_time3 );
      }
      else {
        (*log) << MSG::ERROR << " Cannot book N-tuple:" <<long(m_tuple1)<< endreq;
        return StatusCode::FAILURE;
      }
    }

    NTuplePtr nt2(ntupleSvc(), "FILE1/n2");
    if(nt2) m_tuple2 = nt2;
    else {
      m_tuple2 = ntupleSvc()->book("FILE1/n2",CLID_ColumnWiseTuple,"Field");
      if( m_tuple2 ) {
        status = m_tuple2->addItem("tdc",  m_tdc );
      }
      else {
        (*log) << MSG::ERROR << " Cannot book N-tuple:" <<long(m_tuple2)<< endreq; 
        return StatusCode::FAILURE;
      }
    }

    NTuplePtr nt3(ntupleSvc(), "FILE1/n3");
    if(nt3) m_tuple3 = nt3;
    else {
      m_tuple3 = ntupleSvc()->book("FILE1/n3",CLID_ColumnWiseTuple,"Field");
      if( m_tuple3 ) {
        status = m_tuple3->addItem("time4",  m_time4 );
        status = m_tuple3->addItem("time5",  m_time5 );
      }
      else {
        (*log) << MSG::ERROR << " Cannot book N-tuple:" <<long(m_tuple3)<< endreq;
        return StatusCode::FAILURE;
      }
    }

    status = service("BesTimerSvc", m_timersvc);
    if (status.isFailure()) {
      (*log) << MSG::ERROR << name() << ": Unable to locate BesTimer Service" << endreq;
      return StatusCode::FAILURE;
    }
    m_timer = m_timersvc->addItem("Read field Time");
    m_timer1 = m_timersvc->addItem("Read raw files");
  }

  //For random seed added by caogf. Note the position of the code, otherwise it is not available.
  CLHEP::HepRandomEngine* engine = p_BesRndmGenSvc->GetEngine("MIX");
  HepRandom::setTheEngine(engine);
  HepRandom::showEngineStatus();

  return StatusCode::SUCCESS;
}

StatusCode MixerAlg::execute() 
{
  //calculate time
  if(m_ifOutPut) {
    m_timer->start();
  }

  //caogf add
  SmartDataPtr<Event::EventHeader> evt(eventSvc(),"/Event/EventHeader");
  if( !evt ){
    return StatusCode::FAILURE;
  }

  if(m_RealizationSvc->UseDBFlag() == true && m_RealizationSvc->ifReadRandTrg() == true && m_dbUserRequest == false) {
    int runNo = evt -> runNumber();
    if((runNo != m_runNo) || (RootInterface::Instance(*log)->getCurrentFileName() != currentMCFile)) {
      m_runNo = runNo;
      currentMCFile = RootInterface::Instance(*log)->getCurrentFileName();
      m_mdcCnv->setRunId(runNo);
      std::vector<std::string> bgfiles = m_RealizationSvc->getBgFileName();
      if(bgfiles.size() == 0) {
        (*log) << MSG::ERROR << "No random trigger files are found in the run " << m_runNo << std::endl;
        exit(-1);
      }
      //m_ranTrgEvents = m_RealizationSvc->getRanTrgEvtNum();
      if(! m_pattern.empty())
      {
        for(unsigned int k = 0; k < bgfiles.size(); k++) {
          size_t pos_round = bgfiles[k].rfind("round");
          (*log) << MSG::INFO<<"bgfiles: "<<bgfiles[k]<<endreq;
          if(pos_round!=string::npos){ 
            bgfiles[k].replace(bgfiles[k].begin(), bgfiles[k].begin()+pos_round, m_pattern);
            (*log) << MSG::INFO<<"new random trigger data path: "<<bgfiles[k]<<endreq;
          }
          else{
            (*log) << MSG::ERROR<<"string 'round' not found in random trigger path!"<<endreq;
            exit(-1);
          } 
        }
      }
      if (! m_newdatadir.empty())
      {
       for(unsigned int k = 0; k < bgfiles.size(); k++) {
	char tmp[255];
	std::strcpy (tmp, bgfiles[k].c_str());	
        string fname = basename(tmp);
        bgfiles[k].replace(bgfiles[k].begin(), bgfiles[k].end(), m_newdatadir+'/'+fname);
        (*log) << MSG::INFO<<"new random trigger data path: "<<bgfiles[k]<<endreq;
       }
      }

      // achieve bg index files
      std::vector<std::string> bgfilesIndex;
      bgfilesIndex.clear();
      for(unsigned int bg_index = 0; bg_index < bgfiles.size(); bg_index++) {
        bgfilesIndex.push_back(bgfiles[bg_index] + ".idx");
      }

      // get event number of each bg file
      if(m_fr) delete m_fr;
      m_fr = new RawFileReader(bgfiles);
      std::vector<int> ranTrgEvents = m_fr->getEventNumber(bgfilesIndex);

      // remove bg files with 0 event
      m_bgfiles.clear();
      m_bgfilesIndex.clear();
      m_ranTrgEvents.clear();
      for(unsigned int bg_index = 0; bg_index < bgfiles.size(); bg_index++) {
        if(ranTrgEvents[bg_index] > 0) {
          m_bgfiles.push_back(bgfiles[bg_index]);
          m_bgfilesIndex.push_back(bgfilesIndex[bg_index]);
          m_ranTrgEvents.push_back(ranTrgEvents[bg_index]);
        }
      }

      // get event number of each bg file
      if(m_fr) delete m_fr;
      m_fr = new RawFileReader(m_bgfiles);

      // clear temp vector
      bgfiles.clear();
      bgfilesIndex.clear();
      ranTrgEvents.clear();

      // bg files exist?
      if(m_bgfiles.empty() || m_ranTrgEvents.empty()) {
	(*log) << MSG::WARNING << "No background datafiles found!!!" << endreq;
	return StatusCode::SUCCESS;
      }
  
      if(m_skip == true) {
        if(m_mixingMethod == 1) {
          // Initialize
          m_ranStepLenInCurrentFile.clear();
          currentBGFile = "";
          m_skipCount = 0;
          m_totalEvent = 0; 

          // sort random trigger files by time increasing
          bool ifsucc = file_sort(m_bgfiles,m_ranTrgEvents);
          if( !ifsucc ) return StatusCode::FAILURE;
          
          // achieve bg index files
          m_bgfilesIndex.clear();
          for(unsigned int bg_index = 0; bg_index < m_bgfiles.size(); bg_index++) {
            m_bgfilesIndex.push_back(m_bgfiles[bg_index] + ".idx");
          }

          //count number of sets, total bg events in each set and total bg events in this run
          m_vRanEvtNumInSubSet.clear();
          m_vStreamNumInSubSet.clear();
          m_totRanEvtNum = 0;
          int set_no = -1;
          int ranEvtNumInSubSet = 0;
          int nstream = 0;
          for(unsigned int i = 0; i < m_ranTrgEvents.size(); i++) {
            if(i == 0) set_no = m_numSets[i];
            if((i != 0) && (set_no != m_numSets[i])) {
              m_vRanEvtNumInSubSet.push_back(ranEvtNumInSubSet);
              m_vStreamNumInSubSet.push_back(nstream);
              ranEvtNumInSubSet = 0;
              nstream = 0;
              set_no = m_numSets[i];
            }
  
            m_totRanEvtNum += m_ranTrgEvents[i];
            ranEvtNumInSubSet += m_ranTrgEvents[i];
            nstream++; 
            if(i == m_ranTrgEvents.size() - 1) {
              m_vRanEvtNumInSubSet.push_back(ranEvtNumInSubSet);
              m_vStreamNumInSubSet.push_back(nstream);
            }
          }

          //get total event number in this run
          int evtNumInRun = -1;
          std::vector<int> vtotEvtNo = m_jobInfoSvc->getTotEvtNo();
          for(unsigned int ii = 0; ii < vtotEvtNo.size(); ii+=2) {
            if(std::abs(runNo) == std::abs(vtotEvtNo[ii]))
              evtNumInRun = vtotEvtNo[ii+1];
          }
      
          //generate step length(event number) for each MC event to select background event
          double tau = m_RealizationSvc->getTauValue();
          double totalTime = m_RealizationSvc->getRunTotalTime();
          if(m_RealizationSvc->getTauValue() == 0. || m_RealizationSvc->getRunTotalTime() == 0.) {
            std::cout << "ERROR: In MixerAlg::execute() ---> The tau value or total run time is 0, please check it. Exit! " << std::endl;
            exit(1);
          }

          bool using_exp = true;
          if(totalTime*100 < tau) using_exp = false;
          m_vStepLength.clear();
          while(1) {
            double ranNum;
            if(using_exp == true) ranNum = RandExponential::shoot(tau);
            else ranNum = RandFlat::shoot(0., totalTime);
            if(ranNum > totalTime) continue;
            ranNum = ranNum*m_totRanEvtNum/totalTime;
            m_vStepLength.push_back((int)ranNum);
            if(m_vStepLength.size() == evtNumInRun) break;
          } 

          sort(m_vStepLength.begin(), m_vStepLength.end());      

          //
          //Add a protect here
          //
          if(evtNumInRun <= 0 || m_totRanEvtNum <= 0) {
             (*log) << MSG::ERROR << "The event number (or random trigger event number) in run " << evt->runNumber() << " is zero" << endreq;
             return StatusCode::FAILURE;
          }

          //
          //assigned step length and the number of selected bg events for each bg file
          //
          //1. define a map to store step length selected from each bg file
          map_stepLength.clear();
          for(unsigned int i = 0; i < m_ranTrgEvents.size(); i++) {
            std::vector<int> vstepLength;
            typedef pair<int, std::vector<int> > vpair;
            map_stepLength.insert(vpair(i,vstepLength));
          }
          //2. assign step length for each bg file
          for(unsigned int i = 0; i < m_ranTrgEvents.size(); ) {
            //2.1 calculate total bg event number in this set and previous sets
            int pre_ranEvtNumSubSet = 0;
            int cur_ranEvtNumSubSet = 0;
            set_no = m_numSets[i];
            for(int j = 0; j < set_no; j++) {
              if(j != (set_no - 1)) pre_ranEvtNumSubSet += m_vRanEvtNumInSubSet[j];
              cur_ranEvtNumSubSet += m_vRanEvtNumInSubSet[j]; 
            }
            //2.2 assign step length
            for(unsigned j = 0; j < m_vStepLength.size(); j++) {
              //if current step length is in current set
              if((m_vStepLength[j] >= pre_ranEvtNumSubSet) && (m_vStepLength[j] < cur_ranEvtNumSubSet)) {
                int sub_stepLength = int((m_vStepLength[j]-pre_ranEvtNumSubSet)/m_vStreamNumInSubSet[set_no - 1]);
                int file_id = 0;
                int begin_fileId = -1, end_fileId = -1;
                for(std::map<int,std::vector<int> >::iterator iter = map_stepLength.begin(); iter != map_stepLength.end(); iter++) {
                  //check current set number
                  if(set_no == m_numSets[iter->first]) {
                    if(begin_fileId == -1) begin_fileId = iter->first;
                    file_id++;
                  } 
                } 
                end_fileId = begin_fileId + file_id;
                bool add_succ = false;
                long loop_count = 0;
               
                while(1) {
                  int random_file = int(RandFlat::shootInt(long(begin_fileId), long(end_fileId))); //exclude end_fileId
                  if(sub_stepLength < m_ranTrgEvents[random_file]) { 
                    map_stepLength[random_file].push_back(sub_stepLength);
                    add_succ = true;
                    loop_count = 0;
                  }
                  if(add_succ) break;
                  loop_count++;
                  if(loop_count >= MAX_LOOP_TIMES) {
                    (*log) << MSG::ALWAYS << "Loop time is larger than MAX_LOOP_TIMES(" << MAX_LOOP_TIMES << ") in MixAlg, when assigning step length for each bg file." << endreq;
                    exit(1);
                  }
                }
              } //endif current step length is in current set 
            } //end assign step length
            i += m_vStreamNumInSubSet[set_no - 1];
          }

          // check selected bg events number, equal to MC events?
          unsigned int ranSelectedNum = 0;
          for(std::map<int,std::vector<int> >::iterator iter = map_stepLength.begin(); iter != map_stepLength.end(); iter++) {
            ranSelectedNum += (iter->second).size();
            //std::cout << "file_id: " << iter->first << "  ranEvtNumSelected: " << (iter->second).size() << std::endl;
          }
          if(ranSelectedNum != m_vStepLength.size()) {
            (*log) << MSG::ERROR << "In MixerAlg::excute()--> selected bg events number not equal to MC events" << endreq; 
            return StatusCode::FAILURE;
          }
        }  
      }   
      
      if(m_mixingMethod == 2) {
        // open background stream
        if (m_fr) delete m_fr;
        m_fr = NULL; 
        try {
          m_bgfilesIndex.clear();
          for(unsigned int bg_index = 0; bg_index < m_bgfiles.size(); bg_index++) {
            m_bgfilesIndex.push_back(m_bgfiles[bg_index] + ".idx");
          }
          if(m_skip == true && m_readBGMethod == 1) m_fr = new RawFileReader(m_bgfiles, m_bgfilesIndex) ;
          else m_fr = new RawFileReader(m_bgfiles) ;
        }
        catch (RawFileException& ex) {
          ex.print();
          return StatusCode::FAILURE;
        }
      }

      m_raw_event->reset();
    }
  }
  if(m_ifOutPut) {
    m_timer->stop();
    m_time1 = m_timer->elapsed();
    m_timer->start();
  }
  //end caogf add
  SmartDataPtr<MdcDigiCol> mdcMcDigits(eventSvc(),"/Event/Digi/MdcDigiCol");
  if( ! mdcMcDigits )
    (*log) << MSG::ERROR << "Unable to retrieve MdcDigiCol" << endreq;
  else
    (*log) << MSG::INFO << "MdcDigiCol retrieved of size "<< mdcMcDigits->size() << endreq;
  
  SmartDataPtr<EmcDigiCol> emcMcDigits(eventSvc(),"/Event/Digi/EmcDigiCol");
  if( ! emcMcDigits )
    (*log) << MSG::ERROR << "Unable to retrieve EmcDigiCol" << endreq;
  else
    (*log) << MSG::INFO << "EmcDigiCol retrieved of size "<< emcMcDigits->size() << endreq;
  
  SmartDataPtr<MucDigiCol> mucMcDigits(eventSvc(),"/Event/Digi/MucDigiCol");
  if( ! mucMcDigits )
    (*log) << MSG::ERROR << "Unable to retrieve MucDigiCol" << endreq;
  else
    (*log) << MSG::INFO << "MucDigiCol retrieved of size "<< mucMcDigits->size() << endreq;
  
  SmartDataPtr<TofDigiCol> tofMcDigits(eventSvc(),"/Event/Digi/TofDigiCol");
  if( ! tofMcDigits )
    (*log) << MSG::ERROR << "Unable to retrieve TofDigiCol" << endreq;
  else
    (*log) << MSG::INFO << "TofDigiCol retrieved of size "<< tofMcDigits->size() << endreq;
  
  for(int ievent = 0; ievent<m_nevent; ievent++)
    {
      (*log) << MSG::INFO << "Mixing BG Event " << ievent << endreq;
      //if(m_skip == true) {
      //  nskipped = 0;
      //  m_skipCount = (int(m_NSkip*(RandFlat::shoot())) + 1); 
      //}  
      bool next = false;
      if(m_skip == true) {
        int nskip = 0;
        if(m_mixingMethod == 1) {
          if(m_RealizationSvc->UseDBFlag() == true && m_dbUserRequest == false) {
            if(m_skipCount >= m_ranStepLenInCurrentFile.size()) {
              m_ranStepLenInCurrentFile.clear();
              for(std::map<int,std::vector<int> >::iterator iter = map_stepLength.begin(); iter != map_stepLength.end(); iter++) {
                if(currentBGFile == "") {
                  if((iter->second).size() == 0) continue;
                  if(m_fr) delete m_fr;
                  try {
                    if(m_readBGMethod == 1) m_fr = new RawFileReader(m_bgfiles[iter->first], m_bgfiles[iter->first]+".idx") ;
                    else m_fr = new RawFileReader(m_bgfiles[iter->first]) ;
                    m_totEvtNumInCurFile = m_ranTrgEvents[iter->first];
                  }
                  catch (RawFileException& ex) {
                    ex.print();
                  }
                  m_ranStepLenInCurrentFile = iter->second;
                  m_skipCount = 0;
                  currentBGFile = m_fr->currentFile();
                  break;
                }
                if(currentBGFile == m_bgfiles[iter->first]) {
                  iter++;
                  if(iter == map_stepLength.end()) return StatusCode::FAILURE;
                  if((iter->second).size() == 0) {
                    while(1) {
                      iter++;
                      if(iter == map_stepLength.end()) return StatusCode::FAILURE;
                      if((iter->second).size() > 0) break;
                    }
                  }
                  if(m_fr) delete m_fr;
                  try {
                    if(m_readBGMethod == 1) m_fr = new RawFileReader(m_bgfiles[iter->first], m_bgfiles[iter->first]+".idx") ;
                    else m_fr = new RawFileReader(m_bgfiles[iter->first]) ;
                    m_totEvtNumInCurFile = m_ranTrgEvents[iter->first];
                  }
                  catch (RawFileException& ex) {
                    ex.print();
                  }
                  m_ranStepLenInCurrentFile = iter->second;
                  m_skipCount = 0;
                  currentBGFile = m_fr->currentFile();
                  break;
                }
              }
            }
            //std::cout << "skipcount: " << m_skipCount << "  stepLength: " << m_ranStepLenInCurrentFile[m_skipCount] <<"  total event number: " << m_totEvtNumInCurFile << std::endl;
            
            if(m_skipCount == 0) nskip = m_ranStepLenInCurrentFile[m_skipCount];
            else nskip = m_ranStepLenInCurrentFile[m_skipCount] - m_ranStepLenInCurrentFile[m_skipCount - 1];
            
            m_nEventsToEnd = (m_totEvtNumInCurFile - 1) - m_ranStepLenInCurrentFile[m_skipCount]; //number of events to the end of current file

            if(m_skipCount == 0 && nskip == 0) nskip = 1;
            //std::cout << "nskip: " << nskip << std::endl;
            m_skipCount++;
          } 
          if(m_RealizationSvc->UseDBFlag() == false || m_dbUserRequest == true) nskip = int (2*m_NSkip*(RandFlat::shoot())) + 1;
          if(m_totalEvent == 0 && nskip == 0) nskip = 1;
        }
        if(m_mixingMethod == 2) {
          nskip = int (2*m_NSkip*(RandFlat::shoot())) + 1;
        }
        if(m_ifOutPut) {
          m_timer->stop();
          m_time2 = m_timer->elapsed();
          m_timer->start();
        }

        // get that bg event
        if(m_readBGMethod == 0) {
          // same with previous versions
          for(int j = 0; j < nskip; j++) {
            next =  nextEvent();
	    if ( ! next )
	      {
	        (*log) << MSG::ERROR << "Cannot get next background event" << endreq;
	        return StatusCode::FAILURE;
	      } 
          }
        }
        if(m_readBGMethod == 1) {
          // new method to read bg events, using index file.
          next  =  nextEvent(nskip,0,m_nEventsToEnd);
          if ( ! next )
          {
            (*log) << MSG::ERROR << "Cannot get next background event" << endreq;
            return StatusCode::FAILURE;
          }
        }
        if(m_readBGMethod == 2) {
          // new method to read bg events, using rude localizer.
          next  =  nextEvent(nskip, 14*1024);
          if ( ! next )
          {
            (*log) << MSG::ERROR << "Cannot get next background event" << endreq;
            return StatusCode::FAILURE;
          }
        }
      }
      else { // if skip = false
        next =  nextEvent();
      }

      if(m_mixingMethod == 1) {
        if ( !next && m_totalEvent == 0) {
	  (*log) << MSG::ERROR << "Cannot get next background event" << endreq;
	  return StatusCode::FAILURE;
	} 
      }
 
      if(m_mixingMethod == 2) {
        if ( !next ) {
          (*log) << MSG::ERROR << "Cannot get next background event" << endreq;
          return StatusCode::FAILURE;
        }
      }

      mixDigi(mdcMcDigits, emcMcDigits, mucMcDigits, tofMcDigits);
    }

  m_totalEvent++;

  if(m_ifOutPut) {
    m_timer->stop();
    m_time3 = m_timer->elapsed();
    m_tuple1->write(); 
  }


  return StatusCode::SUCCESS;
}

StatusCode MixerAlg::finalize() {
  if( m_raw_event ) delete m_raw_event;
  if( log ) delete log;
  if( m_fr) delete m_fr;
  return StatusCode::SUCCESS;
}

// Read the next event.  
bool MixerAlg::nextEvent(int nskip, int evtbyte, int eventsToEnd) 
{

   m_raw_event->reset();

   try {
      if(m_ifOutPut) {
        m_timer1->start();
      }

      const uint32_t* fragment;
      if(m_skip == true && m_readBGMethod == 0) fragment = m_fr->nextEvent();
      if(m_skip == true && m_readBGMethod == 1) {
        if(nskip ==  0) fragment = m_fr->currentEvent();
        else fragment = m_fr->nextEvent(nskip - 1);
      }
      if(m_skip == true && m_readBGMethod == 2) {
        if(nskip == 0) fragment = m_fr->currentEvent();
        else fragment = m_fr->roughlyNextEvent(nskip - 1, evtbyte);
      } 
      if(m_skip == false) fragment = m_fr->nextEvent();

      if(m_ifOutPut) {
        m_timer1->stop();
        m_time4 = m_timer1->elapsed();
        m_timer1->start();
      }
      //if (fragment == NULL) {
      //	(*log) << MSG::ERROR << "RawFileReader::nextEvent() Failed!!!" << endreq;
      //  exit(1);
      // }

      RawEvent f(fragment);
      if (!f.check()) {
	 std::cerr << "Found invalid event (traceback):" << std::endl;
	 std::exit(1);
      }
      //1.print basic event information
      uint32_t fFragmentSize = f.fragment_size_word();
      (*log) << MSG::DEBUG << "[Event No. #" << f.global_id()
	 << "] " << f.fragment_size_word() << " words in "
	 << f.nchildren() << " subdetectors "
	 << endreq;
      m_raw_event->setRunNo(f.run_no());
      m_raw_event->setEventNo(f.global_id());

      //fucd: get event filter information
      const uint32_t* ef=NULL;
      f.event_filter_info(ef);
      if(!ef){
	 (*log) << MSG::ERROR << "Event Filter Data Failed!!!" << endreq;
	 exit(1);
      }
      else{
	 (*log) << MSG::DEBUG<< "Event Filter Information*********" <<std::hex<<endreq
	    <<*ef<< "  "<<*(ef+1)<<"  "<<*(ef+2)<<"  "<<*(ef+3)<<std::dec<<endreq;
	 m_raw_event->addReHltRaw((uint32_t*)ef, (uint32_t)4);
      }

      uint32_t *robs[64];
      int nrobs = eformat::get_robs(fragment, (const uint32_t **)robs, 64);

      for (int robi = 0; robi < nrobs; robi++) {
	 eformat::ROBFragment<uint32_t*> rob(robs[robi]);
	 if ((rob.rod_detev_type() & 0x2) != 0) continue;  //bad data caogf add
	 uint32_t* dataptr = NULL;
	 rob.rod_data(dataptr);

	 uint32_t source_id_number = rob.rod_source_id();
	 //std::cout<<"#####source_id_number#####"<<source_id_number<<std::endl;
	 source_id_number <<= 8;
	 source_id_number >>= 24;
	 //std::cout<<"#####(source_id_number<<24)>>29#####"<<source_id_number<<std::endl;
	 //be careful here!!!
	 switch(source_id_number) {
	    case 161:
	       m_raw_event->addReMdcDigi(dataptr, rob.rod_ndata());
	       break;
	    case 163:
	       m_raw_event->addReEmcDigi(dataptr, rob.rod_ndata());
	       break;
	    case 162:
	       m_raw_event->addReTofDigi(dataptr, rob.rod_ndata());
	       break;
	    case 167:
	       m_raw_event->addReEtfDigi(dataptr, rob.rod_ndata());
	       break;
	    case 164:
	       m_raw_event->addReMucDigi(dataptr, rob.rod_ndata());
	       break;
	    case 165:  // trigger !!!
	       //std::cout << "Get Trigger Data -" << std::endl;
	       //for (int i = 0; i < rob.rod_ndata(); i++) {
	       //  std::cout << "\t0x" << std::hex << dataptr[i] << std::dec << std::endl;
	       //}
	       m_raw_event->addReTrigGTD(dataptr, rob.rod_ndata());
	       break;
	    case 124:  // EventFilter
	       m_raw_event->addReHltRaw(dataptr, rob.rod_ndata());
	       break;
	    case 241:  // McParticle
	       m_raw_event->addMcParticle(dataptr, rob.rod_ndata());
	       break;
	    default:
	       //std::cerr << "no such subdetector type: " << source_id_number << std::endl;
	       break;
	 }
      }
      if(m_ifOutPut) {
        m_timer1->stop();
        m_time5 = m_timer1->elapsed();
        m_tuple3->write();
      }

      if(m_usingFilter == true) {
        if(eventType() == "GHadron" || eventType() == "GEBhabha" || eventType() == "GBBhabha" || eventType() == "GCosmic" || eventType() == "GDimuon") {
          if(m_skip == true && m_readBGMethod == 0) {
            return nextEvent(1, evtbyte, eventsToEnd);
          }
          if(m_skip == true && m_readBGMethod == 1) {
            if(m_RealizationSvc->UseDBFlag() == false || m_dbUserRequest == true) return nextEvent(1, evtbyte, eventsToEnd);
            if(eventsToEnd > 0 && m_RealizationSvc->UseDBFlag() == true && m_dbUserRequest == false ) { 
              eventsToEnd--; 
              return nextEvent(1, evtbyte, eventsToEnd);
            }
          }
          if(m_skip == true && m_readBGMethod == 2) {
            return nextEvent(1, evtbyte, eventsToEnd);
          }
          if(m_skip == false) return nextEvent(nskip, evtbyte, eventsToEnd);
        }
      }

      return true;
   }
   catch (ReachEndOfFileList& ex){
      ex.print();

      delete m_fr;
      try {
        if(m_skip == true && m_readBGMethod == 1) m_fr = new RawFileReader(m_bgfiles, m_bgfilesIndex) ;
        else m_fr = new RawFileReader(m_bgfiles);
      }
      catch (RawFileException& ex) {
	ex.print();
	return false;
      }

      return nextEvent(nskip, evtbyte, eventsToEnd);
   }
   catch (RawFileException& ex) {
      ex.print();
   }
   catch (eformat::Issue& ex) {
      std::cerr << std::endl << "Uncaught eformat issue: " << ex.what() << std::endl;
   }
   catch (ers::Issue& ex) {
      std::cerr << std::endl << "Uncaught ERS issue: " << ex.what() << std::endl;
   }
   catch (std::exception& ex) {
      std::cerr << std::endl << "Uncaught std exception: " << ex.what() << std::endl;
   }
   catch (...) {
      std::cerr << std::endl << "Uncaught unknown exception" << std::endl;
   }

   return false;
}

template <class T1, class T2> 
void combineDigits (SmartDataPtr<T1>& mcDigits, T1& bgDigits, int verbosity) 
{
  vector<T2*> newDigiCol;
  typename T1::iterator mc;
  typename T1::const_iterator bg;
  bool new_digi;
  for(bg = bgDigits.begin(); bg != bgDigits.end(); bg++ )
    {
      new_digi = true;
      for(mc = mcDigits->begin(); mc != mcDigits->end(); mc++ )
	{
	  if((*mc)->identify()==(*bg)->identify())
	    {
	      if( verbosity < 2 )
		{
		  cout << "****************************************"<<endl;
		  cout << "MC id " << (*mc)->identify().get_value() 
		       << " BG Id " << (*bg)->identify().get_value() << endl;
		  cout<<"==> MC Digi : ";
		  (*mc)->fillStream(cout);
		  cout<<"==> BG Digi : ";
		  (*bg)->fillStream(cout);
		}
	      
	      (*mc)->setTrackIndex((*mc)->getTrackIndex() - 999);
	      *(*mc) += *(*bg);

	      new_digi = false;
	      if( verbosity < 2 )
		{
		  cout<<"==> New MC Digi: ";
		  (*mc)->fillStream(cout);
		  cout << "****************************************"<<endl;
		}
	    }
	}
      
      // no signal digi in this channel. Create new digi with BG only
      if (new_digi) {
        (*bg)->setTrackIndex(-1000);
	newDigiCol.push_back(*bg);
      }
    }
 
  for(bg=newDigiCol.begin(); bg!=newDigiCol.end(); bg++ )
    mcDigits->push_back(*bg);
}

void combineMdcDigits (SmartDataPtr<MdcDigiCol>& mcDigits, MdcDigiCol& bgDigits, int verbosity)
{
  vector<MdcDigi*> newDigiCol;
  MdcDigiCol::const_iterator mc;
  MdcDigiCol::const_iterator bg;
  bool new_digi;
  for(bg = bgDigits.begin(); bg != bgDigits.end(); bg++ )
    {
      if((*bg)->getChargeChannel() < 0x7FFFFFFF)  (*bg)->setChargeChannel(0);
      new_digi = true;
      for(mc = mcDigits->begin(); mc != mcDigits->end(); mc++ )
        {
          if((*mc)->identify()==(*bg)->identify())
            {
              if( verbosity < 2 )
                {
                  cout << "****************************************"<<endl;
                  cout << "MC id " << (*mc)->identify().get_value()
                       << " BG Id " << (*bg)->identify().get_value() << endl;
                  cout<<"==> MC Digi : ";
                  (*mc)->fillStream(cout);
                  cout<<"==> BG Digi : ";
                  (*bg)->fillStream(cout);
                }

              (*mc)->setTrackIndex((*mc)->getTrackIndex() - 999);
              *(*mc) += *(*bg);

              new_digi = false;
              if( verbosity < 2 )
                {
                  cout<<"==> New MC Digi: ";
                  (*mc)->fillStream(cout);
                  cout << "****************************************"<<endl;
                }
            }
        }

      // no signal digi in this channel. Create new digi with BG only
      if (new_digi) {
        (*bg)->setTrackIndex(-1000);
        newDigiCol.push_back(*bg);
      }
    }

  for(bg=newDigiCol.begin(); bg!=newDigiCol.end(); bg++ )
    mcDigits->push_back(*bg);
}

void combineTofDigits(SmartDataPtr<TofDigiCol>& mcDigits, TofDigiCol& bgDigits, int verbosity)
{
  vector<TofDigi*> newDigiCol;
  //typename TofDigiCol::const_iterator bg;
  TofDigiCol::const_iterator bgTof = bgDigits.begin();
  for(; bgTof!=bgDigits.end(); bgTof++ )
    {
      (*bgTof)->setTrackIndex(-1000);
      newDigiCol.push_back(*bgTof);
    } 
  for(bgTof=newDigiCol.begin(); bgTof!=newDigiCol.end(); bgTof++ ) {
    mcDigits->push_back(*bgTof);
  }
}

void MixerAlg::mixDigi(SmartDataPtr<MdcDigiCol>& mdcMcDigits, 
		       SmartDataPtr<EmcDigiCol>& emcMcDigits, 
		       SmartDataPtr<MucDigiCol>& mucMcDigits, 
		       SmartDataPtr<TofDigiCol>& tofMcDigits)
{
  if( b_mdc )// MDC
    {
      MdcDigiCol mdcCol;
      decodeMdc(&mdcCol);
      //combineDigits<MdcDigiCol, MdcDigi>(mdcMcDigits, mdcCol, log->level());

      // Find minimal tdc and maximum tdc and calculate mean tdc.
      if(m_ifSmearT0 && getTiming() > 0) {
        int tdc_min = -9, tdc_max = -9, tdc_tot = 0, tdc_num = 0;
        for(MdcDigiCol::const_iterator bg = mdcCol.begin(); bg != mdcCol.end(); bg++ )
        {
          if((*bg)->getTimeChannel() < 0x7FFFFFFF) {
            tdc_tot += (*bg)->getTimeChannel();
            tdc_num++;
            if(tdc_min < 0) tdc_min = (*bg)->getTimeChannel();
            else {
              if(tdc_min > (*bg)->getTimeChannel()) tdc_min = (*bg)->getTimeChannel();
            }
            if(tdc_max < 0) tdc_max = (*bg)->getTimeChannel();
            else {
              if(tdc_max < (*bg)->getTimeChannel()) tdc_max = (*bg)->getTimeChannel();
            }
          }
        }
        int tdc_mean = (int) ((double)tdc_tot/(double)tdc_num);
        tdc_num = 0;
        int tdc_shift;
        while(1) {
          tdc_shift = tdc_mean - CLHEP::RandFlat::shootInt(long(0), long(80*24/0.09375));
          if((tdc_min - tdc_shift)>=0 && (tdc_max - tdc_shift) <= int(80*24/0.09375)) break;
          tdc_num++;
          if(tdc_num > m_maxLoop) break;
        }
      
        // Set new tdc 
        for(MdcDigiCol::const_iterator bg = mdcCol.begin(); bg != mdcCol.end(); bg++ )
        {
          if((*bg)->getTimeChannel() >= 0x7FFFFFFF) continue;
          int newTDC = (*bg)->getTimeChannel() - tdc_shift;
          if(newTDC < 0 || newTDC > int(80*24/0.09375)) newTDC = int(CLHEP::RandFlat::shoot()*80*24/0.09375);
          (*bg)->setTimeChannel(newTDC);

          //m_tdc = (*bg)->getTimeChannel();
          //m_tuple2->write();
        }
      }
      //combineDigits<MdcDigiCol, MdcDigi>(mdcMcDigits, mdcCol, log->level());
      combineMdcDigits(mdcMcDigits, mdcCol, log->level());
    }
  if( b_emc )//EMC
    {
      EmcDigiCol emcCol;
      decodeEmc(&emcCol);
      combineDigits<EmcDigiCol, EmcDigi>(emcMcDigits, emcCol, log->level());
    }
  if( b_muc )// MUC
    {
      MucDigiCol mucCol;
      decodeMuc(&mucCol);
      combineDigits<MucDigiCol, MucDigi>(mucMcDigits, mucCol, log->level());
    }
  if( b_tof )// TOF
    {
      TofDigiCol tofCol;
      decodeTof(&tofCol);
      //combineDigits<TofDigiCol, TofDigi>(tofMcDigits, tofCol, log->level());
      combineTofDigits(tofMcDigits, tofCol, log->level());
    }
}

void MixerAlg::decodeMdc(MdcDigiCol* digiCol)
{
  const BufferHolder& mdcBuf = m_raw_event->getMdcBuf();
  m_mdcCnv->convert(mdcBuf, digiCol);
}

void MixerAlg::decodeMuc(MucDigiCol* digiCol)
{ 
  const BufferHolder& mucBuf = m_raw_event->getMucBuf();
  m_mucCnv->convert(mucBuf, digiCol);
}

void MixerAlg::decodeEmc(EmcDigiCol* digiCol)
{
  const BufferHolder& emcBuf = m_raw_event->getEmcBuf();
  m_emcCnv->convert(emcBuf, digiCol);
}

void MixerAlg::decodeTof(TofDigiCol* digiCol)
{
  const BufferHolder& tofBuf = m_raw_event->getTofBuf();
  const BufferHolder& etfBuf = m_raw_event->getEtfBuf();
  if( etfBuf.nBuf()>0 ) {
    m_tofCnv->convert(tofBuf, etfBuf, digiCol);
  }
  else {
    m_tofCnv->convert(tofBuf, digiCol);
  }
}

std::string MixerAlg::eventType()
{
  const BufferHolder& hltBuf = m_raw_event->getHltBuf();
  DstHltInf* hlt = new DstHltInf();
  hlt->setEventType(hltBuf(0)[0]);

  std::string evtType = hlt->getEventName();

  if(hlt) delete hlt;

  return evtType;
}

int MixerAlg::getTiming()
{
  int timing = 0;

  TrigGTD*  trigGTD = NULL;
  TrigGTDCol* gtdCol = new TrigGTDCol;

  const BufferHolder& gtdBuf = m_raw_event->getGTDBuf();
  uint32_t nbuf = gtdBuf.nBuf();

  for (uint32_t i = 0; i < nbuf; i++) {
    uint32_t* buf = gtdBuf(i);
    uint32_t bufSize = gtdBuf.bufSize(i);
    uint32_t index = 0;
    while (bufSize - index > 1) {
      uint32_t blockSize = ( ((*(buf+index))>>14) & 0x3FF);
      uint32_t id = ((*(buf+index))>>24);
      if (blockSize == 0 || (index+blockSize) > bufSize) break;
      if ((id> 0xD1 && id < 0xD8 && id != 0xD5) || id == 0xDA || (id > 0xE1 && id < 0xED)) {
        trigGTD = new TrigGTD(buf+index);
        gtdCol->push_back(trigGTD);
      }
      index += blockSize;
    }
  }

  TrigGTDCol::iterator iter = gtdCol->begin();
  for (;iter != gtdCol->end(); iter++ ) {
    const uint32_t  boardId = (*iter)->getId();   //The board Id 0xd3: GTL, 0xD2: SAF1, 0xD4: SAF2, 0xD6: SAF3
    const uint32_t  timeWindow = (*iter)->getTimeWindow(); //Time window, bit8 to bit13, total: 0--31
    const uint32_t  size = (*iter)->getDataSize();    //The size of trigger data, not include head
    const uint32_t* trigData = (*iter)->getDataPtr(); //Trigger data

    //Get data group 5 in GTL, including trigger channel, timing and prescale.
    if(boardId == 0xd3) {
      if(size%timeWindow != 0) {
        std::cout << "GTL data is NOT completed, exit." << std::endl;
        exit(0);
      }
      for(uint32_t j = 0; j < size; j++) {
        uint32_t dataId = ((trigData[j] >> 24) & 0x7);
        if(dataId != 5) continue; //find data group 5
        for(uint32_t i = 1, loop = 0; loop < 24; i <<= 1, loop++) {
          if((loop == 16) && (trigData[j] & i)) timing = 1;
          if((loop == 17) && (trigData[j] & i) && (timing != 1)) timing = 2;
          if((loop == 18) && (trigData[j] & i) && (timing == 0)) timing = 3;
        }
      }
    }
  }

  return timing;
}

bool MixerAlg::file_sort(std::vector<std::string>& files, std::vector<int>& ranEvtNums) {
  std::vector<std::string> tmp_files = files;
  std::vector<int> tmp_ranEvtNums = ranEvtNums;
  files.clear();
  ranEvtNums.clear();
  m_numSets.clear();

  const char* file_index[100];
  int num_index[100];
  int set_index[100];
  for(int i = 0; i < 100; i++) {
    file_index[i] = "";
    num_index[i] = 0;
    set_index[i] = 0;
  }

  if(tmp_files.size() >= 100) { 
    std::cout << "ERROR: In MixerAlg::file_sort(), please change bigger array size" << std::endl;
    return false;
  }

  for(unsigned int i = 0; i < tmp_files.size(); i++) {
    int index = 0;
    const char* file1 = tmp_files[i].c_str();
    const char* substr1 = strstr(file1,"_file");
    int strlen1 = strlen(substr1);
    char cset1[4];
    char cnum1[2];
    
    for(int sub1 = 0; sub1 < strlen1; sub1++) {
      if(substr1[sub1] == 'e') {
        cset1[0] = substr1[sub1+1];
        cset1[1] = substr1[sub1+2];
        cset1[2] = substr1[sub1+3];
        cset1[3] = '\0';
      }
      else if(substr1[sub1] == '-') {
        cnum1[0] = substr1[sub1+1];
        cnum1[1] = '\0';
        break;
      }
      else {
        continue;
      }
    }

    int set1 = atoi(cset1);
    int num1 = atoi(cnum1);
    int encode_set1 = set1*100 + num1;

    for(unsigned int j = 0; j < tmp_files.size(); j++) {
      if(i == j) continue;
      const char* file2 =  tmp_files[j].c_str();
      const char* substr2 = strstr(file2,"_file");
      int strlen2 = strlen(substr2);
      char cset2[4];
      char cnum2[2];
      for(int sub2 = 0; sub2 < strlen2; sub2++) {
        if(substr2[sub2] == 'e') {
          cset2[0] = substr2[sub2+1];
          cset2[1] = substr2[sub2+2];
          cset2[2] = substr2[sub2+3];
          cset2[3] = '\0';
        }
        else if(substr2[sub2] == '-') {
          cnum2[0] = substr2[sub2+1];
          cnum2[1] = '\0';
          break;
        }
        else {
          continue;
        }
      }
      int set2 = atoi(cset2);
      int num2 = atoi(cnum2);
      int encode_set2 = set2*100 + num2;
      if(encode_set1 > encode_set2) index++;
    }
    file_index[index] = tmp_files[i].c_str();
    num_index[index] = tmp_ranEvtNums[i];
    set_index[index] = set1;
  }

  int setNo = -10;
  for(unsigned int i = 0; i < tmp_files.size(); i++) {
    files.push_back(file_index[i]);
    ranEvtNums.push_back(num_index[i]);
    if(setNo != set_index[i]) {
      setNo = set_index[i];
      int numSets_size = m_numSets.size();
      if(numSets_size == 0) m_numSets.push_back(1);
      if(numSets_size != 0) m_numSets.push_back(m_numSets[numSets_size - 1] + 1);
    }
    else {
      int numSets_size = m_numSets.size();
      m_numSets.push_back(m_numSets[numSets_size - 1]);
    } 
  }

  return true;
}
