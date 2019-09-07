#ifndef _IRawDataCnvSvc_H
#define _IRawDataCnvSvc_H 1

#include <string>
#include "GaudiKernel/IInterface.h"

/** IRawDataCnvSvc
*   The event conversion service needs an additional interface used 
*   by the converters to declare their object path.
*   
*   Based on GLAST's EbfCnvSvc
*/

//static const InterfaceID IID_IRawDataBaseCnv("IRawDataBaseCnv", 1 , 0);  

class IRawDataCnvSvc : virtual public IInterface      {
public:
  class Leaf : public std::vector<Leaf*>    {
  public:
    long userParameter;
    std::string path;
    std::string bank;
    CLID        clid;
    Leaf(const std::string& p, const CLID& c, const std::string& b, long u)
      : userParameter(u), path(p), bank(b), clid(c) {}
    Leaf(const Leaf& copy) 
      : std::vector<Leaf*>(0), userParameter(copy.userParameter), path(copy.path),
        bank(copy.bank), clid(copy.clid) {}
    Leaf& operator=(const Leaf& copy)   {
      path = copy.path;
      bank = copy.bank;
      clid = copy.clid;
      userParameter = copy.userParameter;
      return *this;
    }
    bool operator==(const Leaf& copy) const  {
      return  path          == copy.path && 
              bank          == copy.bank && 
              clid          == copy.clid && 
              userParameter == copy.userParameter;
    }
  };
  
  /// Declare target leaf
  virtual StatusCode declareObject(const Leaf& leaf) = 0;

};

#endif // _IRawDataCnvSvc_H

