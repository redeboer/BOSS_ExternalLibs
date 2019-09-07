#include "HltConfig/MyFrame.h"
#include "HltSteerData/Sequence.h"
#include "HltSteerData/Signature.h"
#include "HltSteerData/HltCriteria.h"

#include "xercesc/framework/XMLFormatter.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/XMLUniDefs.hpp"
#include "xercesc/util/TranscodingException.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"

#include <xercesc/dom/DOM.hpp>

namespace HltProcessor
{

// Necessary for singleton pattern for MyFrame
MyFrame* MyFrame::s_FrameInstance = 0;

// Necessary for singleton pattern for MyFrame: The instance method.
MyFrame* MyFrame::instance() {
  if(s_FrameInstance == 0) {
    s_FrameInstance = new MyFrame;
  }
  return s_FrameInstance;    
}
void MyFrame::destruct() {
  if (s_FrameInstance) delete s_FrameInstance;
}
    
// Constructor
MyFrame::MyFrame():
    m_FrameNumSequenceTables(0),m_FrameNumTables(0),
    m_FrameAbsolutMaxStep(0),
    m_Signature_DocNode(NULL), m_Sequence_DocNode(NULL),
    m_SignatureVec(0),m_SequenceVec(0),m_TableVector(0),
    m_idSequence(0),m_idSignature(0),m_idCriteria(0),m_idItem(0),
    m_first(true)
{
  if(s_FrameInstance){
    std::cout << " MyFrame constructed twice!"<< std::endl;
  }
  s_FrameInstance=this;
  m_thisName = "HltConfig";
  m_msgSvcProvider = MessageSvcProvider::instance();
}

// Destructor
MyFrame::~MyFrame()
{
  for (unsigned int i = 0; i < m_SignatureVec.size(); i++) delete m_SignatureVec[i];
  for (unsigned int j = 0; j < m_SequenceVec.size(); j++) delete m_SequenceVec[j];
  //destruct();
}

// Initialisation, execution and finalization routines.
void MyFrame::init(const std::string sequenceFile, const std::string signatureFile)
{
  //if(m_msgSvcProvider==0)m_msgSvcProvider = MessageSvcProvider::instance();
  //MsgStream log(m_msgSvcProvider->msgSvc(), m_thisName);
  //MsgStream log(messageService(),name());
  //std::cout<<"@@@@@@@@@@@@@@@@@@@1"<<std::endl;
  try {
    XMLPlatformUtils::Initialize();
    //log << MSG::INFO << "MyFrame : Initialized XML!" << endreq;
  }

  catch (const XMLException& toCatch) {
    //log << MSG::ERROR << "MyFrame : Init failed!" << endreq;
    exit(1);
  }
  //std::cout<<"@@@@@@@@@@@@@@@@@@@2"<<std::endl;
  // Instantiate DOM parser for signature XML file. 
  XercesDOMParser* signatureParser = new XercesDOMParser;
  signatureParser->setValidationScheme(XercesDOMParser::Val_Always);    // optional.
  signatureParser->setDoNamespaces(true);   // optional

  // Instantiate DOM parser for sequence XML file. 
  XercesDOMParser* sequenceParser = new XercesDOMParser;
  sequenceParser->setValidationScheme(XercesDOMParser::Val_Always);    // optional.
  sequenceParser->setDoNamespaces(true);   // optional

  // Now try to parse. 
  try {
    signatureParser->parse(signatureFile.c_str());
    //zoujh:log << MSG::INFO << "Parsing " << signatureFile.c_str() << endreq;

    sequenceParser->parse(sequenceFile.c_str());
    //zoujh:log << MSG::INFO << "Parsing " << sequenceFile.c_str() << endreq;
  }

  catch (const XMLException& toCatch) {
    //log << MSG::ERROR << "Error 1" << endreq;
    exit(1);
  }
  catch (const DOMException& toCatch) {
    //log << MSG::ERROR << "Error 4" << endreq;
    XMLPlatformUtils::Terminate();
    exit(1);
  }
  catch (...) {
    //log << MSG::ERROR << "Error 5" << endreq;
    XMLPlatformUtils::Terminate();
    exit(1);
  }
  //std::cout<<"@@@@@@@@@@@@@@@@@@@3"<<std::endl;
  // Get document handle. 
  m_Sequence_DocNode  = sequenceParser->getDocument();
  m_Signature_DocNode = signatureParser->getDocument();
  //std::cout<<"1@@@@@@@@@@@@@@@@@@@"<<m_SequenceVec.size()<<std::endl;
  //std::cout<<"2@@@@@@@@@@@@@@@@@@@"<<m_SignatureVec.size()<<std::endl;
  //std::cout<<"3@@@@@@@@@@@@@@@@@@@"<<m_TableVector.size()<<std::endl;
  if(m_first){
	  m_first=false;
  }
  else{
     for (unsigned int i = 0; i < m_SignatureVec.size(); i++) delete m_SignatureVec[i];
     for (unsigned int j = 0; j < m_SequenceVec.size(); j++) delete m_SequenceVec[j];
     m_SignatureVec.clear();
     m_SequenceVec.clear();
     m_TableVector.clear();
     m_idCriteria=0;
     m_idSignature=0;
     m_idSequence=0;
     m_idItem=0;
  }
  //std::cout<<"1@@@@@@@@@@@@@@@@@@@"<<m_SequenceVec.size()<<std::endl;
  //std::cout<<"2@@@@@@@@@@@@@@@@@@@"<<m_SignatureVec.size()<<std::endl;
  //std::cout<<"3@@@@@@@@@@@@@@@@@@@"<<m_TableVector.size()<<std::endl;
  //std::vector<Sequence*> tmp1(0);
  //m_SequenceVec=tmp1;
  //std::cout<<"1@@@@@@@@@@@@@@@@@@@"<<m_SequenceVec.size()<<std::endl;
  //std::vector<Signature*> tmp2(0);
  //m_SignatureVec=tmp2;
  //std::cout<<"2@@@@@@@@@@@@@@@@@@@"<<m_SignatureVec.size()<<std::endl;
  //std::vector<std::pair<Signature*,Sequence*> > tmp3(0);
  //m_TableVector=tmp3;
  //std::cout<<"3@@@@@@@@@@@@@@@@@@@"<<m_TableVector.size()<<std::endl;
  // Build global sequence list (and print it).
  buildSequenceList(m_Sequence_DocNode, true);
  //std::cout<<"1@@@@@@@@@@@@@@@@@@@"<<m_SequenceVec.size()<<std::endl;
  //std::vector<Sequence*>::iterator iter=m_SequenceVec.begin();
  //for(;iter!=m_SequenceVec.end();iter++){
  //       std::cout<< "@@@@@@@@"<<((*iter)->getSeqID())<<std::endl;
  //}
  //for (unsigned int i = 0; i < m_SequenceVec.size(); i++) {
  //  std::string seqId = m_SequenceVec[i]->getSeqID();
  //  std::cout<< "@@@@@@@@"<<seqId<<std::endl;
  //}
  // Build global signature list (and print it). 
  buildSignatureList(m_Signature_DocNode, true);
  //std::cout<<"2@@@@@@@@@@@@@@@@@@@"<<m_SignatureVec.size()<<std::endl;
  //m_Signature_DocNode->release();
  //m_Sequence_DocNode->release(); 
  delete sequenceParser;
  delete signatureParser;
  //std::cout<<"@@@@@@@@@@@@@@@@@@@6"<<std::endl;
  makeTableVector();
  //std::cout<<"3@@@@@@@@@@@@@@@@@@@"<<m_TableVector.size()<<std::endl;
  m_first=false;
  std::cout << "MyFrame init() succuss" << "criteria num=" << m_idCriteria << std::endl;
}

void MyFrame::makeTableVector()
{
  //MsgStream log(m_msgSvcProvider->msgSvc(), m_thisName);

  for (unsigned int i = 0; i < m_SequenceVec.size(); i++) {
    std::string seqId = m_SequenceVec[i]->getSeqID();
    bool matched = false;
    for (unsigned int j = 0; j < m_SignatureVec.size(); j++) {
      if (m_SignatureVec[j]->label() == m_SequenceVec[i]->getSeqID()) {
	m_TableVector.push_back(std::make_pair(m_SignatureVec[j], m_SequenceVec[i]));
	matched = true;
	break;
      }
    }
    if (!matched) {
      //log << MSG::ERROR << "No matched Signature to Sequence(" << seqId << ")" << endreq;
      exit(1);
    }
  }
}

void MyFrame::final()
{
  /*zoujh:MsgStream log(m_msgSvcProvider->msgSvc(), m_thisName);
  log << MSG::INFO << endreq;
  log << MSG::INFO << "***" << endreq;
  log << MSG::INFO << "Finalizing XML." << endreq;
  log << MSG::INFO << "***" << endreq;*/
  //m_msgSvcProvider=0;
  // Call the XML termination method 
  XMLPlatformUtils::Terminate();
}

void MyFrame::buildSequenceList(const DOMNode* thisNode, bool first)
{
  //MsgStream log(m_msgSvcProvider->msgSvc(), m_thisName);
  if ( first ) {
    first=false;
    //log << MSG::INFO << endreq;
    //log << MSG::INFO << "--------------------------------------" << endreq;
    //log << MSG::INFO << "    Building global sequence list!    " << endreq;
    //log << MSG::INFO << "--------------------------------------" << endreq;
  }
  std::string outString, inputString, seq_idString, algoString;

#define XMLCF XMLString::compareString
#define XMLTF XMLString::transcode
  DOMNode *helpNode = thisNode->getFirstChild();
  // loop through the tree 
  while (helpNode != 0) {

    if(XMLCF(XMLTF(helpNode->getNodeName()),"SEQUENCE")==0) {
      DOMNamedNodeMap *attributes = helpNode->getAttributes();
      int attributesCount = attributes->getLength();
      for(int i = 0; i < attributesCount; i++) {
	if(XMLCF(XMLTF(attributes->item(i)->getNodeName()),"sequence_id")==0)
	  seq_idString = XMLTF(attributes->item(i)->getNodeValue());
	if(XMLCF(XMLTF(attributes->item(i)->getNodeName()),"input")==0)
	  inputString = XMLTF(attributes->item(i)->getNodeValue());
	if(XMLCF(XMLTF(attributes->item(i)->getNodeName()),"output")==0)
	  outString = XMLTF(attributes->item(i)->getNodeValue());
      }
      std::cout <<"+-Found SEQUENCE: " << seq_idString << std::endl;
      //log << MSG::INFO <<"+-Found SEQUENCE: " << seq_idString << endreq;
      Sequence* m_helpSequence = new Sequence();
      m_helpSequence->setSeqID(seq_idString);
      m_helpSequence->setInputID(inputString);
      m_helpSequence->setOutputID(outString);
      m_SequenceVec.push_back(m_helpSequence);
    }

    if (XMLCF(XMLTF(helpNode->getNodeName()),"ESALGORITHM") == 0) {
      DOMNamedNodeMap *attributes = helpNode->getAttributes();
      int attributesCount = attributes->getLength();
      for (int i = 0; i < attributesCount; i++) {
	if(XMLCF(XMLTF(attributes->item(i)->getNodeName()),"algorithm")==0)
	  algoString = XMLTF(attributes->item(i)->getNodeValue());
      }
      std::cout << "|   +-Found ESALGORITHM: " << algoString << std::endl;
      //log << MSG::INFO << "|   +-Found ESALGORITHM: " << algoString << endreq;
      m_SequenceVec.back()->makeVectors(algoString);
    }

    // and crowl on ...
    MyFrame::buildSequenceList(helpNode, first);
    helpNode = helpNode->getNextSibling();
  }
#undef XMLCF
#undef XMLTF
  //std::cout << m_SequenceVec.size() << " sequences have been built. " << std::endl;
}

// build the global signature list. 
void MyFrame::buildSignatureList(const DOMNode* thisNode, bool first)
{
  //MsgStream log(m_msgSvcProvider->msgSvc(), m_thisName);
  
  if ( first ) {
    first = false;
    //log << MSG::INFO << endreq;
    //log << MSG::INFO << "--------------------------------------" << endreq;
    //log << MSG::INFO << "    Building global signature list!    " << endreq;
    //log << MSG::INFO << "--------------------------------------" << endreq;
  }
  std::string helpString, helpPrescale, behaviour, itemName, itemType;
  int prescale=1;
  float forcedAccept=1, itemValue=0;

#define XMLCF XMLString::compareString
#define XMLTF XMLString::transcode
  DOMNode *helpNode = thisNode->getFirstChild();
  // loop through the tree 
  while (helpNode != 0) {
    static std::string current_sigID = "";
    if(XMLCF(XMLTF(helpNode->getNodeName()),"SIGNATURE")==0) {
      DOMNamedNodeMap *attributes = helpNode->getAttributes();
      unsigned int attributesCount = attributes->getLength();
      for(unsigned int i = 0; i < attributesCount; i++) {
	if(XMLCF(XMLTF(attributes->item(i)->getNodeName()),"signature_id")==0)
	  helpString = XMLTF(attributes->item(i)->getNodeValue());
	current_sigID = helpString;
	if(XMLCF(XMLTF(attributes->item(i)->getNodeName()),"prescale")==0) {
	  helpPrescale = XMLTF(attributes->item(i)->getNodeValue());
	  prescale = atoi(helpPrescale.c_str());
	}
	if(XMLCF(XMLTF(attributes->item(i)->getNodeName()),"forced_accept")==0)
	  forcedAccept = (float)DOMToDouble(attributes->item(i)->getNodeValue());
      } 
      std::cout<< "+-Found SIGNATURE: " << helpString << std::endl;
      //log << MSG::INFO << "+-Found SIGNATURE: " << helpString << endreq;
      Signature* m_helpSignature = new Signature();
      m_helpSignature->setSigLabel(helpString);
      m_helpSignature->setPrescale(prescale);
      m_helpSignature->setForcedAccept(forcedAccept);
      m_SignatureVec.push_back(m_helpSignature);
    }
    //std::cout<<"***************************2"<<std::endl;
    static HltCriteria* current_HltCriteria = NULL;
    if(XMLCF(XMLTF(helpNode->getNodeName()),"CRITERIA")==0) {
      HltCriteria *m_HltCriteria = new HltCriteria(m_idCriteria);
      DOMNamedNodeMap *attributes = helpNode->getAttributes();
      unsigned int attributesCount = attributes->getLength();
      //std::cout <<"---------------------------1"<<current_sigID<<std::endl;
      for(unsigned int i = 0; i < attributesCount; i++) {
	if(XMLCF(XMLTF(attributes->item(i)->getNodeName()),"behaviour")==0) {
	  behaviour = XMLTF(attributes->item(i)->getNodeValue());
	  if (behaviour == "reject") behaviour += "_reject";
	}
	if(XMLCF(XMLTF(attributes->item(i)->getNodeName()),"next")==0) {
	  if (behaviour != "jump") {
            std::cout << "Unconsistent of behaviour and next" << std::endl;
	    //log << MSG::ERROR << "Unconsistent of behaviour and next" << endreq;
	    exit(1);
	  }
	  std::string nextID = XMLTF(attributes->item(i)->getNodeValue());
          //std::cout <<"---------------------------2"<<nextID<<" " << m_SequenceVec.size()<<std::endl;
	  for (unsigned int i = 0; i < m_SequenceVec.size(); i++) {
            //std::cout << "======="<<m_SequenceVec[i]->getSeqID()<<std::endl;
	    if (m_SequenceVec[i]->getSeqID() == current_sigID) {
	      while (++i < m_SequenceVec.size()) {
		//std::cout << "======="<<m_SequenceVec[i]->getSeqID()<<std::endl;
		if (m_SequenceVec[i]->getSeqID() == nextID) break;
	      }
	      if (i == m_SequenceVec.size()) {
                std::cout << "Undefined jump target: " << nextID << std::endl;
		//log << MSG::ERROR << "Undefined jump target: " << nextID << endreq;
		exit(1);
	      }
	      break;
	    }
	  }
	  behaviour = behaviour +'_'+ nextID;
	}
	if(XMLCF(XMLTF(attributes->item(i)->getNodeName()),"mark")==0) {
	  if ((behaviour!= "accept")&&(behaviour!= "continue")) {
            std::cout << "Unconsistent of behaviour and mark" << std::endl;
	    //log << MSG::ERROR << "Unconsistent of behaviour and mark" << endreq;
	    exit(1);
	  }
	  behaviour = behaviour +'_'+ XMLTF(attributes->item(i)->getNodeValue());
	}
      }
      if (behaviour.find('_') == string::npos) {
	std::cout << "Unvalid behaviour ==> " << behaviour << std::endl;
	//log << MSG::ERROR << "Unvalid behaviour ==> " << behaviour << endreq;
	exit(1);
      }
      //std::cout << "-----------------" << behaviour << std::endl;
      m_HltCriteria->setBehaviour(behaviour);
      std::cout << "|   +-Found CRITERIA: behaviour as " << m_HltCriteria->getBehaviour() << std::endl;
      //log << MSG::INFO << "|   +-Found CRITERIA: behaviour as " << behaviour << endreq;
      m_SignatureVec.back()->makeVectors(m_HltCriteria);
      m_idCriteria++;
      current_HltCriteria = m_HltCriteria;
    }
    //std::cout<<"***************************3"<<std::endl;
    if(XMLCF(XMLTF(helpNode->getNodeName()),"ITEM") == 0) {
      DOMNamedNodeMap *itemAttr = helpNode->getAttributes();
      if (itemAttr->getLength() != 3) {
	//log << MSG::ERROR << "Unvalid CriteriaItem" << endreq;
	exit(1);
      }
      for (int i = 0; i < 3; i++) {
	if(XMLCF(XMLTF(itemAttr->item(i)->getNodeName()), "name") == 0)
	  itemName = XMLTF(itemAttr->item(i)->getNodeValue());
	if(XMLCF(XMLTF(itemAttr->item(i)->getNodeName()), "value") == 0)
	  itemValue = (float)DOMToDouble(itemAttr->item(i)->getNodeValue());
	if(XMLCF(XMLTF(itemAttr->item(i)->getNodeName()), "type") == 0)
	  itemType = XMLTF(itemAttr->item(i)->getNodeValue());
      }
      //std::cout << "|   |   +-Found CRITERIA_ITEM (" << itemName << ", " << itemValue << ", " << itemType << ")" << std::endl;
      current_HltCriteria->addItem(itemName, itemValue, itemType,m_idItem);
      m_idItem++;
    }
    //std::cout<<"***************************4"<<std::endl;
    // and crowl on ...
    MyFrame::buildSignatureList(helpNode, first);
    helpNode = helpNode->getNextSibling();
  }
  //if(helpNode==0)std::cout<<"MyFrame::buildSignatureList() finished"<<std::endl;
#undef XMLCF
#undef XMLTF
}

// Get the final vector.
const std::vector<std::pair<Signature*, Sequence*> >& MyFrame::getTablesVector() const
{
  return m_TableVector;
}

const double MyFrame::DOMToDouble(const XMLCh * thisDOMString) const
{
  double returnValue(0);
  std::string helpString(XMLString::transcode(thisDOMString));  
  std::istringstream is(helpString);
  is >> returnValue;
  return returnValue;                         
}

} // End of namespace  bracket
