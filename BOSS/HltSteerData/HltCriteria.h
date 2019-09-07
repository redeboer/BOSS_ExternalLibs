#ifndef HLTCRITERIA_H
#define HLTCRITERIA_H

#include <string>
#include <vector>
#include "HltStore/HltStoreSvc.h"

namespace HltProcessor {

class HltCriteria {
  
  typedef struct {
    std::string      name;
    float            value;
    std::string      type;
    int              id;
  } CriteriaItem;

public:
  HltCriteria(int);
  virtual ~HltCriteria() {};

  bool satisfyCriteria() const;
  void addItem(const std::string& name, float value, const std::string& type,int id);

  void setBehaviour(const std::string& behaviour) {
    m_behaviour = behaviour;
  }

  const std::string& getBehaviour() const {
    return m_behaviour;
  }

  const int idCriteria() const {
    return m_idCriteria;
  }

  std::vector<CriteriaItem> getItemVec() {
    return m_items;
  }

private:
  std::string  m_behaviour;

  std::vector<CriteriaItem>        m_items;
  int               m_idCriteria;
  //static HltStoreSvc*        m_HltStoreSvc;
  HltStoreSvc*         m_HltStoreSvc;
};

}  //namespace
#endif //HLTCRITERIA_H
