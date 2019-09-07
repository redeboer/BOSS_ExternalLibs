#ifndef ROOTINTERFACE_H
#define ROOTINTERFACE_H
/*
 *Description:Interface for all Root I/O needed by the RootCnvSvc
 *Based on the RootInterface of Glast.
 */
#include "TObject.h"
#include <vector>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IService.h"
/////////////////////////////////////////////////////////////////////////////

class TTree;
class TClonesArray;
class TFile;
class TList;


class RootInterface {

 public:

  RootInterface (MsgStream log);
  virtual  ~RootInterface();
  virtual   StatusCode finalize();

  /// singleton behaviour
static  RootInterface* Instance(MsgStream log); 
  /// check if all the files is over 2005-11-28
 virtual bool checkEndOfTree();
 virtual bool getENDFILE(){ return m_ENDFILE;}
  /// add input tree to the list
 virtual StatusCode addInput(const std::string& treename,const std::string& file);
  /// add output tree to the list
  virtual StatusCode addOutput(const std::string& treename,const std::string& file,int splitx, int bufsize, int compression);
  /// create a branch in this tree
  virtual StatusCode createBranch(const std::string& tree,const std::string& branch ,const char *classname,void *addr,int & branchnr);
  
  /// set branch address
  virtual StatusCode setBranchAddress(const std::string treename, const std::string branchname, void *addr,int& nb);
  /// get entry from this branch
  virtual StatusCode getBranchEntry( int nr, int entry,int& nb);
  
  /// get entry from this branch with addr
  virtual StatusCode getBranchEntry( int nr, int entry, void * addr, int& nb);
  
  
  /// get EOF flag
  virtual bool       getEOF() { return m_EOF;}
  /// get nr of entries
  virtual Int_t      getEntries() { return m_entries;}
  /// fill in all trees
  virtual StatusCode fillTrees();

  virtual std::string getCurrentFileName() { return m_currentFileName;}  

  virtual std::vector<int> getTotEvtNo();
  virtual std::string getJobOptions();
  virtual std::string getDecayOptions();
  virtual void printJobInfo(TFile* file, int level);

  virtual void setSelectFromTag(bool temp) {m_selectFromTag = temp;}
  virtual bool getSelectFromTag(){ return m_selectFromTag;}

  virtual int getCurrentFileNum() {return m_fileNum;}
  virtual int getTotalFileNum() {return m_fileNames.size();}

  virtual void setTagInputFile(std::vector<std::string> input); 
  virtual std::vector<std::string> getTagInputFile() {return m_tagInputFile;}

  //virtual void setTagFileFormat(int form) {m_tagFileFormat = form;}
  //virtual int getTagFileFormat() {return m_tagFileFormat;}


 private:
  /// get treenr from treename
  virtual StatusCode getTreeNr(const std::string treename, unsigned int& treenr,bool doAdd=false);
  /// get tree
  virtual TTree*  getTree(const std::string treename);
  /// get tree from other files 2005-11-28
  virtual TTree*  getOtherTree(const std::string treename);
  /// create this tree
  virtual StatusCode createTree(unsigned int treenr, const std::string treename);
  /// parallel vectors
  
  

  /// vector of other trees created from others files 2005-11-28
  std::vector<TTree *> m_otherTrees;
   /// vector of input files
  std::vector<TFile *> inputFiles;
  ///files 2005-11-28
  std::vector<std::string> m_fileNames;
  std::string m_currentFileName;
  bool m_selectFromTag;
  std::vector<std::string> m_tagInputFile;
  //int m_tagFileFormat;

  ///the number of the using files 2005-11-29
  int m_fileNum;
  ///signed the tree is end 2005-11-30
  bool m_ENDFILE;
  /// vector of treenames used
  std::vector<std::string> m_treenames;  
  /// vector of input filenames used
  std::vector<std::string> m_inputFilenames;
  /// vector of output filenames used
  std::vector<std::string> m_outputFilenames;
  /// vector of input trees
  std::vector<TTree *> m_inputTrees;
  /// vector of output trees
  std::vector<TTree *> m_outputTrees;
   /// vector of input files
  std::vector<TFile *> m_inputFiles;
  /// vector of output files
  std::vector<TFile *> m_outputFiles;
  /// vector of splitmodes
  std::vector<int> m_splitModes;
  /// vector of buffer sizes
  std::vector<int>  m_bufSizes;
  /// vector of compression levels
  std::vector<int>  m_compressionLevels;


  /// array of branch pointers for writing
  TClonesArray *m_branches;

  /// array of branch pointers for reading
  TClonesArray *m_branchesRead;

  /// EOF flag
  bool m_EOF;

  /// number of entries (<0 if not yet known)
  Int_t  m_entries;

  /// instance of message stream transmitted at construction
  MsgStream log;

  /// static singleton pointer
  static RootInterface *m_rootInterface;
  
  std::vector<std::string> m_jobOptions;
  std::string m_bossVer;
  std::string m_decayOptions;
  std::vector<int> m_totEvtNo;

  // tianhl add for besvis
 private:
  std::vector<std::string>	m_single_treenames;
  std::vector<std::string>	m_single_outputFileNames;
  std::vector<TFile *>		m_single_outputFiles;
  std::vector<TTree *>		m_single_outputTrees;
  std::vector<int>		m_single_splitModes;
  std::vector<int>		m_single_bufSizes;
  std::vector<int>		m_single_compressionLevels;

 public:
  virtual StatusCode f_addOutput(const std::string& treename,
		  const std::string& file,
		  int splitx = 1, int bufsize = 64000,
		  int compression = 1);
  virtual StatusCode f_createTree(unsigned int treenr, 
		  const std::string treename);
  virtual StatusCode f_createBranch(const std::string& treename,
		  const std::string& branchname,
		  const char *classname, 
		  void *addr,int & branchnr);
  virtual StatusCode f_getTreeNr(const std::string treename, 
		  unsigned int& treenr,bool doAdd=false);
  virtual StatusCode f_fillTrees();
  virtual StatusCode f_finalize();

};

#endif 
