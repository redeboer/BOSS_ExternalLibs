#ifndef MYFRAME_H
#define MYFRAME_H

// STL
#include <string>
#include <vector>
#include <list>
#include <utility>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
//#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
// XERCES4C DOM API
#include <xercesc/dom/DOM.hpp>

#include "HltTools/MessageSvcProvider.h"
#include "HltSteerData/HltCriteria.h"

XERCES_CPP_NAMESPACE_USE

namespace HltProcessor 
{
  class Signature;
  class Sequence;

class MyFrame 
{
public:
  /* The instance method for the singleton pattern */
  static MyFrame* instance();
  static void destruct();

  /* The init method in which beasically everything is calculated and done.
   Ti delegates work to the XMLTE and XMLSIgnature classes. */
  void init(const std::string,const std::string);

  void makeTableVector();
  
  /* XML finalisation and some printout and cleanup */
  void final();
  
  /* Methods returning the configuration vector for the L2 and EF. */
  const std::vector<std::pair<Signature*, Sequence*> >& getTablesVector() const;

  /* Needed for some of the algortihmic methods */
  //inline int getNumSequenceTables() const {return m_FrameNumSequenceTables;}

  const double DOMToDouble(const XMLCh * thisDOMString) const;

protected:
  /* constructor and desctructor protected in singleton pattern. */
  MyFrame();
  ~MyFrame();
  
private:
  /* private method tp build the list of sequences from the XML file */
  void buildSequenceList(const DOMNode *, bool);
  
  /* Method to check the sequence list for consistency and completeness */
  //  bool checkSequenceList();

  /* Builds the signature list from the XML file */ 
  void buildSignatureList(const DOMNode *, bool);

private:
  /* needed for singleton pattern */
  static MyFrame* s_FrameInstance;
  
  /* XML file names */
  char* m_FrameSequenceListFile;
  char* m_FrameSignatureListFile;

  /* Members for getting the sizes of the trees */
  int m_FrameNumSequenceTables;
  int m_FrameNumTables;
  int m_FrameAbsolutMaxStep;
 
  /* These are needed for the MessageSvc */
  MessageSvcProvider* m_msgSvcProvider;
  std::string m_thisName;

  /* DOM node pointers */
  DOMNode *m_Signature_DocNode;
  DOMNode *m_Sequence_DocNode;

  std::vector<Signature*> m_SignatureVec;
  std::vector<Sequence*> m_SequenceVec;

  std::vector<std::pair<Signature*, Sequence*> > m_TableVector;

  int m_idSequence;
  int m_idSignature;
  int m_idCriteria;
  int m_idItem;
  
  bool m_first;
}; // End of MyFrame class bracket
 
}  // End of namespace bracket

#endif   // #ifndef MYFRAME_H
