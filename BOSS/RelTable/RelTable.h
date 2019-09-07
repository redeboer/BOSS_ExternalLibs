
#ifndef RELTABLE_H
#define RELTABLE_H


#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/SmartRef.h"
#include "Relation.h"
#include <vector>
#include <algorithm>
#include <iterator>

/** 
* @class RelTable
*
* @brief This class is used to wrap a collection of Relations.
*
* The RelTable class wraps a list (a Gaudi ObjectList) of Relations. It
* lets the user search for all object related to a given one. The search can be
* done with respect to an object of the first or the second field of the
* relations. The user can also modify or delete relations.
* 
*/


namespace Event {
  
  template <class T1, class T2>
    class RelTable {
    
public:
  
  RelTable() {}
  RelTable(ObjectList < Relation<T1,T2> >* rels);
  
  
  /// Initialize the internal pointer to an ObjectList of relations
  void init() { m_relations = new ObjectList< Relation<T1,T2> >;}
  
  /**
  * The following method add a new Relation to the table, even if there is
  * already a relation with the same two objects (they could have different
  * infos vector)
  */
  void addDupRel(Relation<T1,T2>* rel);

  /**
  * The following method add a Relation to the table if it doesn't contain
  * a relation between the same two objects, otherwise it appends the info
  * vector to the exsisting relation
  * @param rel is a pointer to a relation between two objects
  * @return true if the relation has been added and false if it is a duplicate
  * and has not been added (in this case the user has to delete it)
  */
  bool addRelation(Relation<T1,T2>* rel);
  
  /**
  * This method search for all relations having obj in the first
  * field. 
  * @param obj it's a pointer to the object given by the user
  * @return A vector of pointers to the relations involving the given object.
  */
  std::vector< Relation<T1,T2>* > getRelByFirst(const T1* pobj) const;
  
  /**
  * This method search for all relations having pobj in the second
  * field. 
  * @param pobj it's a pointer to the object given by the user
  * @return A vector of pointers to the relations involving the given object.
  */
  std::vector< Relation<T1,T2>* > getRelBySecond(const T2* pobj) const;
  
  /**
  * This method erase a particular relation from the table (keeping the 
  * integrity).
  * @param rel it's a pointer to the relation to be erased
  */
  void erase(Relation<T1,T2> *rel);
  
  /**
  * This method change the first data pointer of a given relation contained
  * into the table.
  * @param rel it's a pointer to the relation to be modified
  * @param pobj is the new data value provided by the user
  */
  void changeFirst(Relation<T1,T2> *rel, T1 *pobj);
  
  /**
  * This method change the second data pointer of a given relation contained
  * into the table.
  * @param rel it's a pointer to the relation to be modified
  * @param pobj is the new data value provided by the user
  */
  void changeSecond(Relation<T1,T2> *rel, T2 *pobj);
  
  /// This method returns the number of relations in the table
  unsigned long size() const ;
  
  /// Returns the pointer to the collection of relations.
  ObjectList< Relation<T1,T2> >* getAllRelations() const;
  
private:
  
  /// Pointer to a collection of relations
  ObjectList < Relation<T1,T2> >* m_relations;
  

  void bindRelationFirst(Relation<T1,T2> *rel);
  void bindRelationSecond(Relation<T1,T2> *rel);

  void removeFirst(Relation<T1,T2> *rel);
  void removeSecond(Relation<T1,T2> *rel);

  Relation<T1,T2>* findDup(Relation<T1,T2> *rel1, Relation<T1,T2>* rel2);
  bool bindRelationNoDup(Relation<T1,T2> *rel);


  };
  
  
  
  
  template <class T1,class T2>
    inline RelTable<T1,T2>::RelTable(ObjectList < Relation<T1,T2> >* rels) {
    
    m_relations = rels;
    
  }
  
  template <class T1,class T2>
    void RelTable<T1,T2>::addDupRel(Relation<T1,T2>* rel) {
    // Purpose and Method:  This routine add a new relation to the collection, even
    // if there is a already a relation between the same two objects
    // Inputs:  rel is a pointer to the relation to be added.
    
    bindRelationFirst(rel);
    bindRelationSecond(rel);
    m_relations->push_back(rel);
  }
  
  
  template <class T1,class T2>
    bool RelTable<T1,T2>::addRelation(Relation<T1,T2>* rel) {
    // Purpose and Method:  This routine add a relation to the table if it doesn't 
    // contain a relation between the same two objects, otherwise it appends the info
    // vector to the exsisting relation
    // Inputs:  rel is a pointer to the relation to be added.
    // Outputs: a boolean value which is true if the realtion has been added to the
    //          table and false it it is a duplicate and thus has not been added.
    //          In the latter case the user has to delete the relation
    
    if (bindRelationNoDup(rel))
      {
	bindRelationSecond(rel);
	m_relations->push_back(rel);
        return true;
      }
    return false;
  }


  template <class T1,class T2>
    std::vector< Relation<T1,T2>* > RelTable<T1,T2>::getRelByFirst(const T1* pobj) const {
    // Purpose and Method: This routine finds all relations having pobj in the
    // first field.  
    // Inputs: pobj is a pointer to the object to be searched in the first field
    // Outputs: A pointer to a vector of Relation* including pobj
    
    std::vector< Relation<T1,T2>* > rels;
    if (!m_relations->size()) return rels;
    SmartRef< Relation<T1,T2> > r = m_relations->front();
    while (pobj != r->getFirst() && r->m_first.getFirst())
    {
      r = r->m_first.getFirst();
    }
    
    if (pobj == r->getFirst())
    {
      rels.push_back(r);
      while (r->m_first.getSame())
      {
        rels.push_back(r->m_first.getSame());
        r = r->m_first.getSame();
      }
    }
    return rels;
  }
  
  
  template <class T1,class T2>
    std::vector< Relation<T1,T2>* > RelTable<T1,T2>::getRelBySecond(const T2* pobj) const {
    // Purpose and Method: This routine finds all relations having pobj in the
    // second field.  
    // Inputs: pobj is a pointer to the object to be searched in the second field
    // Outputs: A pointer to a vector of Relation* including pobj
    std::vector< Relation<T1,T2>* > rels;
    if (!m_relations->size()) return rels;
    SmartRef< Relation<T1,T2> > r = m_relations->front();
    while (pobj != r->getSecond() && r->m_second.getFirst())
    {
      r = r->m_second.getFirst();
    }
    
    if (pobj == r->getSecond())
    {
      rels.push_back(r);
      while (r->m_second.getSame())
      {
        rels.push_back(r->m_second.getSame());
        r = r->m_second.getSame();
      }
    }
    return rels;
  }
  

  template <class T1,class T2>
    void RelTable<T1,T2>::erase(Relation<T1,T2> *rel) {
    // Purpose: This method remove the given relation from the table

    removeFirst(rel);
    removeSecond(rel);

    m_relations->remove(rel);
    delete rel;
  }

  
  template <class T1,class T2>
    void RelTable<T1,T2>::changeFirst(Relation<T1,T2> *rel, T1 *pobj) {
    // Purpose: This method change the first data pointer of a relation with the
    // one given by the user

    removeFirst(rel);
    removeSecond(rel);
    m_relations->remove(rel);
    rel->setFirst(pobj);
    addRelation(rel);
  }


  template <class T1,class T2>
    void RelTable<T1,T2>::changeSecond(Relation<T1,T2> *rel, T2 *pobj) {
    // Purpose: This method change the second data pointer of a relation with the
    // one given by the user

    removeFirst(rel);
    removeSecond(rel);
    m_relations->remove(rel);
    rel->setSecond(pobj);
    addRelation(rel);
  }


  template <class T1,class T2>
    inline unsigned long RelTable<T1,T2>::size() const {
    // Purpose: This method returns the total number of relations contained in the
    // collection
    return m_relations->size();
  }
  
  template <class T1,class T2>
    inline ObjectList< Relation<T1,T2> >* RelTable<T1,T2>::getAllRelations() const {
    
    return m_relations;
    
  }

  

  template <class T1,class T2>
    inline void RelTable<T1,T2>::bindRelationFirst(Relation<T1,T2> *rel) {
    
    Relation<T1,T2>* temp;

    if (m_relations->size())
    {
      SmartRef< Relation<T1,T2> > r = m_relations->front();
      while ((r->getFirst() != rel->getFirst()))
      {
        if (r->m_first.getFirst())
        {
          r = r->m_first.getFirst();
        }
        else
        {
          break;
        }
      }
      
      if (r->getFirst() != rel->getFirst())
      {
        r->m_first.setFirst(rel);
        rel->m_first.setPrev(r);
      }
      else
      {
        temp = r->m_first.getSame();
        rel->m_first.setSame(temp);
        if (temp)
          temp->m_first.setPrev(rel);
        r->m_first.setSame(rel);
        rel->m_first.setPrev(r);
      }
    }
  }


  template <class T1,class T2>
    inline void RelTable<T1,T2>::bindRelationSecond(Relation<T1,T2> *rel) {
    Relation<T1,T2>* temp;

    if (m_relations->size())
    {
      SmartRef< Relation<T1,T2> > r = m_relations->front();

      while ((r->getSecond() != rel->getSecond()))
      {
        if (r->m_second.getFirst())
        {
          r = r->m_second.getFirst();
        }
        else
        {
          break;
        }
      }
      
      if (r->getSecond() != rel->getSecond())
      {
        r->m_second.setFirst(rel);
        rel->m_second.setPrev(r);
      }
      else
      {
        temp = r->m_second.getSame();
        rel->m_second.setSame(temp);
        if (temp) 
          temp->m_second.setPrev(rel);
        r->m_second.setSame(rel);
        rel->m_second.setPrev(r);
      }
    }    
  }


 
  template <class T1,class T2>
    inline Relation<T1,T2>* RelTable<T1,T2>::findDup(Relation<T1,T2> *rel1, Relation<T1,T2>* rel2)
    {
      while (rel1)
	{
	  if (rel1->getSecond() == rel2->getSecond())
	    return rel1;
	  rel1 = rel1->m_first.getSame();
	}
      return rel1;
    }
	  
 

  template <class T1,class T2>
    inline bool RelTable<T1,T2>::bindRelationNoDup(Relation<T1,T2> *rel) {
    
    Relation<T1,T2>* temp;

    if (m_relations->size())
    {
      SmartRef< Relation<T1,T2> > r = m_relations->front();
      while ((r->getFirst() != rel->getFirst()))
      {
        if (r->m_first.getFirst())
        {
          r = r->m_first.getFirst();
        }
        else
        {
          break;
        }
      }
      
      if (r->getFirst() != rel->getFirst())
      {
        r->m_first.setFirst(rel);
        rel->m_first.setPrev(r);
	return true;
      }
      else
      {
	temp = findDup(r,rel);
	if (!temp)
	  {
	    temp = r->m_first.getSame();
	    rel->m_first.setSame(temp);
	    if (temp)
	      temp->m_first.setPrev(rel);
	    r->m_first.setSame(rel);
	    rel->m_first.setPrev(r);
	    return true;
	  }
	else
	  {
	    std::copy(rel->m_infos.begin(),rel->m_infos.end(),std::back_inserter(temp->m_infos));
	    return false;
	  }
      }
    }
    return true;
  }



  template <class T1,class T2>
    inline void RelTable<T1,T2>::removeFirst(Relation<T1,T2> *rel) {

    Relation<T1,T2> *prev, *next, *first;

    prev = rel->m_first.getPrev();
    next = rel->m_first.getSame();
    if (next)
      next->m_first.setPrev(prev);
    if (prev)
    {
      if (prev->m_first.getFirst())
        prev->m_first.setFirst(next);
      else
        prev->m_first.setSame(next);
    }
    first = rel->m_first.getFirst();
    if (first)
      first->m_first.setPrev(next);
    rel->m_first.setPrev(0);
    rel->m_first.setSame(0);
    rel->m_first.setFirst(0);
  }


  template <class T1,class T2>
    inline void RelTable<T1,T2>::removeSecond(Relation<T1,T2> *rel) {

    Relation<T1,T2> *prev, *next, *first;

    prev = rel->m_second.getPrev();
    next = rel->m_second.getSame();
    if (next)
      next->m_second.setPrev(prev);
    if (prev)
    {
      if (prev->m_second.getFirst())
        prev->m_second.setFirst(next);
      else
        prev->m_second.setSame(next);
    }
    first = rel->m_second.getFirst();
    if (first)
      first->m_second.setPrev(next);
    rel->m_second.setPrev(0);
    rel->m_second.setSame(0);
    rel->m_second.setFirst(0);
  }

}

#endif // RELTABLE_H 
