#ifndef RootAddress_H
#define RootAddress_H 1

#include <string>
#include <vector>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GenericAddress.h"

#include "TObject.h"

extern const long int ROOT_StorageType;

/** @class RootAddress
 * @brief Definition of a  Root address, derived from IOpaqueAddress.
 *
 * Based on SICb's Address class.
 * Contains Root Treenames and branchnames and nr of entry
 * to be able to retrieve the object.
 *Based on the RootAddress of Glast.
 */
class RootAddress : public GenericAddress   {

public:
  RootAddress() {;}
  RootAddress( unsigned char svc,const CLID& clid, const std::string path ="",const std::string treename ="",const std::string branchname ="",int entry=-1);
  virtual ~RootAddress()    {}
  std::string getTreename() const  {return m_treename;}
  virtual void setTreename(std::string treename) {m_treename=treename;}
  std::string getBranchname(int i) const ;
  std::string getPath()  const {return m_path;}
  Int_t getEntryNr() const {return m_entrynr;}
  Short_t getNrBranches() const {return  m_nrBranches;}
  void setEntryNr(int entry)  {m_entrynr=entry;}
  void Print() const;

private:
  /// name of TTree
  std::string m_treename;
  /// name of TBranch
  std::string m_branchname;
  /// path in TDS
  std::string m_path;
  /// entry number
  Int_t m_entrynr;
  /// nr of branches associated (m_branchname may contain cvoncatenation)
  Short_t m_nrBranches;
};


#endif  // RootAddress_H
