#define RootAddress_cpp

#include <iostream>
#include <stdio.h>
#include "RootCnvSvc/RootAddress.h"

RootAddress::RootAddress(unsigned char svc, const CLID& clid, const std::string path,const std::string treename,const std::string branchname,int entry)
  : GenericAddress(svc, clid, path, "", 0, 0) 
{
  // parse branchname and store what has to be read
  m_branchname = branchname;
  int s=branchname.size();
  m_nrBranches=0;
  if (branchname.size()) {
    int pos=0,where=1;
    while (where!=std::string::npos) {
      m_nrBranches++;
      where=branchname.find(":",pos);
      pos=where+1;
    }
  }

  m_treename = treename;
  m_path=path; 
  m_entrynr=entry;
}
void RootAddress::Print() const {
  std::cout << "RootAddress: path "<<m_path<< ", treename "<<m_treename <<" branchname "<<m_branchname<<" entry "<<m_entrynr<<std::endl;
}

std::string RootAddress::getBranchname(int nb) const {
  int pos,where=-1;
  int i=0;
  while (i<=nb) {
    pos=where+1;
    where=m_branchname.find(":",pos);
    i++;
  }
  if (where==std::string::npos) where=m_branchname.size();
 
  return m_branchname.substr(pos,where-pos);
}
