// $Header: /bes/bes/BossCvs/Calibration/rdbModel/rdbModel/Management/Manager.h,v 1.1.1.1 2005/10/17 06:10:53 maqm Exp $
#ifndef RDBMODEL_MANAGER_H
#define RDBMODEL_MANAGER_H
// #include <map>

#include "rdbModel/Management/Visitor.h"
#include "rdbModel/Rdb.h"

namespace rdbModel{

  class Builder;

  /**
   * This class implement the manager for the sections part of the XML file. 
   * It is a singleton.
   * @author D.Favretto & R.Giannitrapani
   */
  class Manager {
  public:
    //    enum buildType{all,constants,sections,materials,idDictionary};

  
    /** The destructor must be invoched by the client before leaving the
	application. It will destroy explicitely the Rdb object and this
	will start the deallocation of all the objects hierarchy */
    ~Manager();

    /** This method is used to access the pointer to the singleton */
    static Manager* getManager();
  
    /** This method is used to specify a builder to be used by the manager. */
    void setBuilder(Builder * b);

    /** This method start the parse of the xml file by the builder.
     *  It returns an error code (0 if all ok)
     */
    int build();


    /// This method starts the visit of a hierarchy with a specified visitor
    void startVisitor(Visitor*);
  
    void cleanRdb();

    /** This method gives back the pointer to the Rdb object. It coule be
	used by expert clients to access information without the need of
	a visitor */
    Rdb* getRdb(){return m_rdb;};


    void setInputSource(std::string pname){m_filename = pname;};
    std::string getInputSourceString(){return m_filename;};

    // There will probably be some collection of get and set services...


  protected:
    /** The constructor is protected; in such a way it is forbidden to *
	build directely a manager. Here the new Rdb object is explicitely
	built */
    Manager():m_builder(0)  {m_rdb = new Rdb;};

  private:

    /** 
	This is a private pointer to the Rdb object that
	provide the entry point to all information about all tables. */
    Rdb * m_rdb;  
    /**
       This is the pointer to the only manager that can be instanciated */
    static Manager* s_pMyself;
    /** This is a private pointer to the builder used by the manager to
	build the hierarchy from the XML file */
    Builder * m_builder;

    std::string m_filename;
  };

}
#endif //RDBMODEL_MANAGER_H

