
#ifndef RELATION_H
#define RELATION_H

#include "RelKey.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ContainedObject.h"
#include <vector>
#include <string>
#include <iostream>



/** 
 * @class Relation
 *
 * @brief This class is used to relate pair of objets.
 *
 * The Relation class build a relation between two objects. It can be an m to n
 * relation and it stores additional information as a vector of strings
 *
 */

static const CLID CLID_Relation = 5100;


namespace Event {

template <class T1, class T2>
class RelTable;



template <class T1, class T2>
class Relation: public ContainedObject {
    
public:
    
    virtual const CLID& clID() const   { return Relation::classID(); }
    static const CLID& classID()       { return CLID_Relation; }


    Relation(T1* obj1, T2* obj2): m_first(obj1), m_second(obj2) {}
    Relation(T1* obj1, T2* obj2, std::string info);
    Relation(T1* obj1, T2* obj2, std::vector<std::string> infos);

    const T1* getFirst() const {return m_first.getData();} 
    T1* getFirst() { return m_first.getData();}

    const T2* getSecond() const {return m_second.getData();} 
    T2* getSecond() { return m_second.getData();}    

    /// Add additional information (as a string) to the relation
    void addInfo(std::string inf);
    std::vector<std::string> getInfos() const;

    /// Fill the ASCII output stream
    std::ostream& fillStream( std::ostream& s ) const;

    friend class RelTable<T1,T2>;
    
private:
    
    /// Key associated to the first object to be related
    RelKey<T1,T1,T2> m_first;
    /// Key associated to the second object to be related
    RelKey<T2,T1,T2> m_second;
    /// Additional information associated to the relation
    std::vector<std::string> m_infos;

    void setFirst(T1* obj) {m_first.setData(obj);}
    void setSecond(T2 *obj) {m_second.setData(obj);}
        
};



template <class T1, class T2>
inline Relation<T1,T2>::Relation(T1* obj1, T2* obj2, std::string info): 
  m_first(obj1), m_second(obj2), m_infos(1,info) {}


template <class T1, class T2>
inline Relation<T1,T2>::Relation(T1* obj1, T2* obj2, std::vector<std::string> infos): 
  m_first(obj1), m_second(obj2), m_infos(infos) {}



template <class T1, class T2>
void Relation<T1,T2>::addInfo(std::string inf) {
    // Purpose and Method:  This routine add additional information to the relation.
    // Inputs:  inf is the information to be added.
    
  m_infos.push_back(inf);

}

template <class T1, class T2>
std::vector<std::string> Relation<T1,T2>::getInfos() const {
    // Purpose and Method: This routine get the additional information
    // associated to the relation. 
    // Outputs: a vector of strings

    return m_infos;
}


template <class T1, class T2>
inline std::ostream& Relation<T1,T2>::fillStream( std::ostream& s ) const {
  // Fill the ASCII output stream
  s << "      Base class Relation"
    << "\n        First Column: ";
  m_first.toStream(s);
  s << "\n        Second Column: "; 
  m_second.toStream(s);
  s << "\n        Additional Information: ";
  
  std::vector<std::string>::const_iterator i;
  for(i = m_infos.begin(); i != m_infos.end(); i++)
    {
      s << "\n          " << *i;
    }
  return s;
}

}

#endif // RELATION_H 

