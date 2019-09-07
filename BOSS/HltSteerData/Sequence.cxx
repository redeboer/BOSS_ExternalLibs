#include "HltSteerData/Sequence.h"

namespace HltProcessor {
    
  Sequence::Sequence():
  m_sequenceAlgoVector(), m_sequenceInputNum(0), m_sequenceAlgoNum(0) {
    m_msgSvcProvider = MessageSvcProvider::instance();
  }

  Sequence::~Sequence() {}

  /// Retrieve algorithm vector reference.
  std::vector<std::string>& Sequence::algoVector() {
    return m_sequenceAlgoVector;
  }

  /// Retrieve number of algorithms.
  int Sequence::algoNum() const {
    return m_sequenceAlgoNum;
  }

  /// Retrieve number of inputs.
  int Sequence::inputNum() const {
    return m_sequenceInputNum;
  }

  /// Retrieve number of algorithms.
  void Sequence::setAlgoNum(int algoNum){
    m_sequenceAlgoNum = algoNum;
  }

  /// Set number of inputs.
  void Sequence::setInputNum(int inputNum){
    m_sequenceInputNum = inputNum;
  }

  /// Build all std::vectors from string XML information.
  void Sequence::makeVectors(std::string algoString){
    std::string::size_type begIdx,endIdx;
    std::string helpString;
    std::string delims("    ");

    // Do it for the algorithm string:
    begIdx = algoString.find_first_not_of(delims);
    while (begIdx != std::string::npos) {
      endIdx = algoString.find_first_of(delims, begIdx);
      if (endIdx == std::string::npos)
	endIdx = algoString.length();
      helpString = algoString.substr(begIdx, endIdx - begIdx);
      //std::cout<<"Sequence  "<<helpString<<std::endl;
      algoVector().push_back(helpString);
      setAlgoNum(algoNum() + 1);
      begIdx = algoString.find_first_not_of(delims, endIdx);
    }
  }

  /// Print all std::vectors and all other information for a given sequence.
  void Sequence::print(){
    MsgStream log(m_msgSvcProvider->msgSvc(), "Sequence");
    log << MSG::INFO << "EventFilter: ";
    log << MSG::INFO << "\t Algo(s): ";
    for(std::vector<std::string>::iterator i = algoVector().begin();
	i!=algoVector().end(); ++i){
      log << MSG::INFO << *i << ' ';
    }
    log << endreq;
  }
} // End of namespace bracket
