// $Header: /bes/bes/BossCvs/Calibration/rdbModel/src/Management/Manager.cxx,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#include <string>
#include <iostream>
#include <fstream>
#include <vector>


#include "rdbModel/Management/Visitor.h"

#include "rdbModel/Management/Manager.h"
#include "rdbModel/Management/Builder.h"
#include "rdbModel/Rdb.h"

namespace rdbModel{

  Manager* Manager::s_pMyself = 0;

  Manager::~Manager(){
    delete m_rdb;
    delete m_builder;
    s_pMyself = 0;
  }

  Manager* Manager::getManager()
  {
    if (s_pMyself == 0)
      s_pMyself = new Manager;
    return s_pMyself;
  }

  void Manager::cleanRdb(){
    delete m_rdb; 
    m_rdb = new Rdb; 
    //  delete manBuilder;
  }

  void Manager::setBuilder(Builder* b)
  {
    m_builder = b;
  
  }

  int Manager::build() {
    int errCode = m_builder->parseInput(m_filename);

    // Unlike geometry description/detModel, there is only one way to 
    // build here: build everything.
    if (!errCode) {
      return m_builder->buildRdb();
    }
    else return errCode;
  }
  void Manager::startVisitor(Visitor* v)
  {
    //    if (TablesVisitor* tv = dynamic_cast<TablesVisitor*>(v))      
    //    {   // the only kind we support, at least for now
      
    //      if (v->getRecursive())
        m_rdb->accept(v);
        //      else
        //        m_rdb->acceptNotRec(sv);
        //    }
  }

}
