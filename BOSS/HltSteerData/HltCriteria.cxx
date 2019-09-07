#include <cmath>
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "HltSteerData/HltCriteria.h"
#include "HltDataTypes/CriteriaItemValue.h"

using HltProcessor::HltCriteria;

//HltStoreSvc*  HltCriteria::m_HltStoreSvc = NULL;

HltCriteria::HltCriteria(int id)
  :m_idCriteria(id)
{
    ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
    StatusCode sc = svcLocator->service("HltStoreSvc", m_HltStoreSvc);
    if (sc.isFailure()) {
      std::cout<<"Event Filter: In HltCriteria ==> Could not access HltStoreSvc!"<<std::endl;
      exit(1);
    }
}

bool HltCriteria::satisfyCriteria() const {
 
  CriteriaItemValue* p_itemValue;
  float              itemValue;
  std::vector<CriteriaItem>::const_iterator it = m_items.begin();

  bool judge=true;
  while (it != m_items.end()) {
    //std::cout << "HltCriteria::satisfyCriteria()"<<(int)m_HltStoreSvc<<std::endl;
    //std::cout << it->name << "," << it->value <<std::endl;
    bool status = m_HltStoreSvc->get(it->name, p_itemValue);
    if (!status) return false;
    //std::cout << "HltCriteria::satisfyCriteria()"<<2<<std::endl;
    status = p_itemValue->getValue(itemValue);
    if (!status) return false;
    //std::cout << "HltCriteria::satisfyCriteria()"<<3<<std::endl;
    //std::cout << "HltCriteria::satisfyCriteria() item[" << it->name
    //  << "] type=>" << it->type << "  get value=" << itemValue << std::endl;

    if (it->type == "MIN") {
      if (itemValue <= it->value) judge= false;
    } else if (it->type == "MAX") {
      if (itemValue >= it->value) judge= false;
    } else if (it->type == "TRUE") {
      if (fabs(itemValue - it->value) > 0.000001) judge= false;
    } else if (it->type == "FALSE") {
      if (fabs(itemValue - it->value) < 0.000001) judge= false;
    } else {
      std::cout << "In HltCriteria::satisfyCriteria() ==> Unvalid CriteriaItemType!" << std::endl;
      exit(1);
    }

    if(it->name =="nmuc"){

    }
    else if(it->name =="nmdc"){

    }
    else if(it->name=="etot"&&it->type=="MIN"){
      
    }
    else if(it->name=="etot"&&it->type=="MAX"){
      
    }
    else if(it->name=="acop"&&it->type=="MAX"){
      
    }
    else{
      //std::cout << "In HltCriteria::satisfyCriteria() ==>Criteria bits not enough! " << std::endl;
    }
    it++;
  }

  //std::cout << "HltCriteria::satisfyCriteria() return true" << std::endl;
  return judge;
}

void HltCriteria::addItem(const std::string& name, float value, const std::string& type,int id) {
  CriteriaItem it = {name, value, type,id};
  m_items.push_back(it);
}

