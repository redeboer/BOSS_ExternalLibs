#ifndef HLTSTORESVC_H
#define HLTSTORESVC_H

#include <string>
#include <stdio.h>
#include <iostream>
#include <map>

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"

using namespace std;

static const InterfaceID  IID_IHltStoreSvc("IID_IHltStoreSvc", 1, 0);

class HltStoreSvc :public Service , virtual public IInterface {

public:
  
  HltStoreSvc(const std::string& name, ISvcLocator* sl);
  ~HltStoreSvc();
 
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  virtual StatusCode initialize ( );
  virtual StatusCode finalize ( );

private:
  //HltStoreSvc* m_HltStore;
  
public:

  static const InterfaceID& interfaceID() { return IID_IHltStoreSvc; }

 protected:

  class BaseHolder;

  typedef map<string, BaseHolder*> ContainerType;


 public:

  void printKeys();
  int size();
  int max_size();
  string sListLength();
  bool exists(const std::string& name);

  // 
  // since these are member template functions, we have to
  // put the whole implementation here
  //
  
  //
  // insert a named value into the store: put("name", data)
  // if the key "name" already exists, it is replaced.
  //

  template<class T>
  bool put(const std::string& name, const T& value) {
    Holder<T> *ptr = new Holder<T>(value);
    if(BaseHolder *old = m_map[name]) {
      std::cout << "demanded key already exists, overwrite old one" << std::endl;
      delete old;
    }
    m_map[name] = ptr;
    return true;
  }
  
  //
  // retrieve a named value from the store: get("name", value)
  // returns false if entry not found; in this case the input
  // object is unchanged
  //
  template<class T>
  bool get(const std::string& name, T& value) {
    //std::cout << "HltStoreSvc::get()  "<<"start"<<std::endl;
    ContainerType::iterator it = m_map.find(name);
    //std::cout << "HltStoreSvc::get()  "<<"middle"<<std::endl;
    if(it != m_map.end()) {
      if(Holder<T>* ptr = static_cast<Holder<T>* >((*it).second)) {
        value = ptr->value();
        return true;
      }
    } 
    //std::cout << "HltStoreSvc::get()  "<<"end"<<std::endl;
    return false;
  }
  

 //
  // erase an entry from the store; the corresponding object is
  // destructed automatically or deleted if a pointer
  //
  bool erase(const std::string& name) {
    ContainerType::iterator it = m_map.find(name);
    if(it != m_map.end()) {
      delete (*it).second;
      m_map.erase(it);
      return true;
    }
    return false;
  }
  //
  // clear the store; the corresponding objects are
  // destructed automatically or deleted if a pointer
  //
  bool clear(void) {
    for (ContainerType::iterator it = m_map.begin(); it != m_map.end(); it++) {
      delete (*it).second;
    }
    m_map.erase(m_map.begin(), m_map.end());
    return true;
  }
  
protected:

  // BaseHolder base class allows the use of a single hash_map for all
  // templated holder classes.

  class BaseHolder {
  public:
    virtual ~BaseHolder() {};
  };
  
  template<class T>
  class Holder : public BaseHolder {
  public:
    Holder(const T& value) : m_value(value) {};
    ~Holder() { 
      erase(m_value); } 
    const T& value() const { return m_value; }
  private:
    template<class T1>
    void erase(T1 value) {}
    
    template<class T1>
    void erase(T1 *value) { 
      delete value; }
    
  private:
    T m_value; 
  };
  
  ContainerType m_map;
  

};


#endif
