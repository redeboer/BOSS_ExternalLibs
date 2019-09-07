/********************************************************************
NAME:     StepSequencer.cxx
********************************************************************/
#include "HltSteering/StepSequencer.h"
// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"
#include "HltAlgorithms/IEFAlgorithm.h"

using HltProcessor::Sequence; 
 
inline void findAlgTypeName(const std::string& property,
		  std::string& SubAlg_type,
		  std::string& SubAlg_NameParam) 
{
  int slash_pos = property.find_first_of("/");
  SubAlg_type = property.substr( 0, slash_pos );
  SubAlg_NameParam = (slash_pos > 0) ? property.substr( slash_pos + 1) : SubAlg_type;
}

inline void findParamSet(const std::string& property,
		  std::string& SubAlg_name,
		  std::string& SubAlg_param) 
{
  int slash_pos = property.find_first_of("/");
  SubAlg_name = property.substr( 0, slash_pos );
  SubAlg_param = (slash_pos > 0) ? property.substr( slash_pos + 1) : SubAlg_name;
}

//  CONSTRUCTOR:
StepSequencer::StepSequencer(const std::string& name, ISvcLocator* pSvcLocator): 
  Algorithm(name, pSvcLocator) {
  m_isEnabled = false;
}

// DESTRUCTOR:
StepSequencer::~StepSequencer() {
  /*
  std::vector<Algorithm*>* subAlgms = subAlgorithms();
  std::vector<Algorithm *>::iterator it;
  for (it = subAlgms->begin(); it != subAlgms->end(); it++) {
    std::cout << *it << (*it)->name() << std::endl;
    (*it)->release();
  }
  */
}

/////////////////////////////////////////////////////////////////
void StepSequencer::reset()
{
  MsgStream log( messageService(), name() );
  std::map<std::string, std::vector<Algorithm*> >::iterator it = m_AlgMap.begin();
  log << MSG::DEBUG << "the size of algorithm map is " << m_AlgMap.size() << endreq;
  for ( ; it != m_AlgMap.end(); it++) {
    log << MSG::DEBUG << "reset() of " << it->first << endreq;
    const std::vector<Algorithm*>& algs = it->second;
    int n = algs.size();
    for (int i = 0; i < n; i++) {
      if (IEFAlgorithm* ialg = dynamic_cast<IEFAlgorithm*>(algs[i])) {
	//std::cout << " @reset(): " << algs[i]->name() << "  p: " << ialg << std::endl;
	ialg->reset();
      }
    }
  }
}

// INITIALIZE METHOD:
StatusCode StepSequencer::initSequencer( Sequence* sequence) {
  MsgStream log( messageService(), name() );

  std::string SubAlg_type, SubAlg_name, SubAlg_param, SubAlg_NameParam,AlgNameFromConfig ;
  Algorithm* pAlg;
  Algorithm* bAlg;
  
  //read in algorithms in sequence and initialize them
  log<< MSG::DEBUG << "Sequence " << sequence->getSeqID()
     <<" has " << sequence->algoNum() << " algorithms " << endreq;

  std::vector<Algorithm*> helpAlgVec;
  std::vector<std::string>::iterator algI = sequence->algoVector().begin();

  for ( ; algI != sequence->algoVector().end(); algI++){
      std::string alg_def = *algI;
      log << MSG::DEBUG << "algorithm    : " << alg_def << endreq;     
      findAlgTypeName(alg_def,SubAlg_type,SubAlg_NameParam);
      findParamSet(SubAlg_NameParam,AlgNameFromConfig,SubAlg_param);
      SubAlg_name=AlgNameFromConfig+"_"+SubAlg_param;
      
      //check if the name exists, don't create again the subalgorithm, just added to the AlgoTePairs vector
      bool subAlgExist = doesExistAlready(SubAlg_name);
      if(!subAlgExist) {   
         StatusCode sc = createSubAlgorithm(SubAlg_type, SubAlg_name, pAlg);
         if(sc.isFailure() ) {
              log << MSG::FATAL << alg_def <<" sub-algorithm create failed!" << endreq;
              return sc;
         }
	 log << MSG::DEBUG << "created " << SubAlg_type << "/" << SubAlg_name << endreq;
         bAlg = dynamic_cast<Algorithm*>(pAlg);
	 bAlg->initialize();
	 bAlg->beginRun();
         StringProperty paramSet("ParamSetFile", SubAlg_param);
         bAlg->setProperty(paramSet);

	 helpAlgVec.push_back(bAlg);
      } else {
	 pAlg = existingAlg(SubAlg_name);
	 bAlg = dynamic_cast<Algorithm*>(pAlg);
	 //bAlg->set_isReRunable();
	 StringProperty paramSet("ParamSetFile",SubAlg_param);
         bAlg->setProperty(paramSet);
	 helpAlgVec.push_back(bAlg);
      }
  }
  m_AlgMap[sequence->getSeqID()] = helpAlgVec;
  
  log << MSG::INFO << "Initialization of "<< name() << " completed successfully" << endreq;
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////

Algorithm* StepSequencer::existingAlg (std::string subAlgName)
{
 MsgStream log( messageService(), name() );
 Algorithm* exAlg;
 std::vector<Algorithm*>* subAlgms = subAlgorithms();
 std::vector<Algorithm*>::iterator it = subAlgms->begin();
 for ( ; it != subAlgms->end(); it++) {
   if(subAlgName==(*it)->name()) {
       log << subAlgName<<" already created, return pointer to it "<< endreq;
       return *it;
   }
 }
 return exAlg;
}

/////////////////////////////////////////////////////////////////
bool StepSequencer::doesExistAlready(std::string aName) 
{  
  MsgStream log( messageService(), name() );
  bool doesExist=false;
  std::vector<Algorithm*>* subAlgms = subAlgorithms();
  
  if(subAlgms->size()==0) {
    doesExist=false;
    return doesExist;     
  }
  else {
    std::vector<Algorithm*>::iterator it = subAlgms->begin();
    for ( ; it != subAlgms->end(); it++) {
         if(aName==(*it)->name()) {
	      log << MSG::WARNING << aName <<" sub-algorithm already created" << endreq;
	      doesExist=true;
	      break;
         }
    }
  }     
  return doesExist; 
}

////////////////////////////////////////////////////////////
// MY EXECUTE METHOD:
bool StepSequencer::execSequencer(const std::string& seqID)
{
  MsgStream log( messageService(), name() );
  const std::vector<Algorithm*>& algVec = m_AlgMap[seqID];
  /*log << MSG::DEBUG << " Execute Sequencer on a step with "
      << algVec.size() << " algorithms." << endreq;*/

  log << MSG::DEBUG << "=============================" << endreq;

  std::vector<Algorithm*>::const_iterator it = algVec.begin();
  for(; it != algVec.end(); it++) {
    StatusCode sc = (*it)->execute();
    if (sc.isFailure()) {
      log << MSG::ERROR << (*it)->name() << " execute failed" << endreq;
      return false;
    }
  }

  return true;
}

std::vector<Algorithm*> StepSequencer::searchAlgForEachTE_o( Sequence* seq ) 
{

  MsgStream log( messageService(), name() );
  std::string SubAlg_type, SubAlg_name, SubAlg_param, SubAlg_NameParam,AlgNameFromConfig;

  /* std::string n=name();
  std::string::size_type p_end, p_begin;
  p_end=n.size();
  p_begin=p_end-3;
  std::string instance=n.substr( p_begin, p_end);*/

  vMatchAlg.clear();
     std::vector<std::string>::iterator It = seq->algoVector().begin();
     for ( ; It < seq->algoVector().end(); It++) {
       std::string alg_defin = *It;
       findAlgTypeName(alg_defin,SubAlg_type,SubAlg_NameParam);
       findParamSet(SubAlg_NameParam,AlgNameFromConfig,SubAlg_param);
       SubAlg_name=AlgNameFromConfig+"_"+SubAlg_param;

       /*std::vector<std::pair<Algorithm*,HltElement* > >::iterator I;
       for(I=AlgoTEPairs.begin();I!=AlgoTEPairs.end();++I) {       

	  if( SubAlg_name==(I->first)->name() && (I->second)->label()==seq->outputLabel() ) {
            matchAlg=I->first;
            vMatchAlg.push_back(matchAlg);
	  } 
       }*/
      }   
  return vMatchAlg;
}
