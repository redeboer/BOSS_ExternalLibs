#ifndef Physics_Analysis_TagWriterAlg_H
#define Physics_Analysis_TagWriterAlg_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class RootInterface;

#include "TFile.h"
#include "TTree.h"
using namespace std;

class TagWriterAlg : public Algorithm {

public:
  
  TagWriterAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  
  StatusCode getTagInfo();
  void openOutputTagFile(std::string file);
  //bool eventFilter();

private:

  RootInterface* m_rootInterface;

  int m_fileNum;

  //TFile* m_iFile;
  TFile* m_oFile;
  //TTree* m_iTree;
  TTree* m_oTree;
  TTree* m_oFileTree;
  
  //input tag file name
  //vector<string> m_inputTagFile;

  //output file name
  vector<string> m_outputTagFile;

  //for reading: input
  /*int m_r_entry;
  int m_r_runNo;
  int m_r_eventId;
  int m_r_totalCharged;
  int m_r_totalNeutral;
  int m_r_totalTrks;
  */

  //for writing: output
  string m_dstFile;
  int m_entry;
  int m_runNo;
  int m_eventId;
  int m_totalCharged;
  int m_totalNeutral;
  int m_totalTrks;

  string m_dstOutput;
  int m_entry_in_alg;
  //int m_filterTotalCharged;
};

#endif 
