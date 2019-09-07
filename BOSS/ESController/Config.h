//Dear emacs, this is -*- c++ -*-

/**
 * @file Config.h
 * @author <a href="mailto:dosil@ifae.es">Mireia Dosil</a>
 * $Revision: 1.1.1.1 $
 * $Date: 2007/04/26 12:43:06 $
 *
 * @brief Configuration for the implementations of EF algorithms.
 */

#ifndef ESCONTROLLER_CONFIG_H
#define ESCONTROLLER_CONFIG_H

#include <string>

namespace efpsc {
  /**
   * Defines the Dummy Algorithm setup. This class is thread-safe.
   */
  class Config {

  public:

    /**
     * Reads the efpscSteering configuration node.
     *
     * @param objname The name of the object in the configuration database,
     * that contains the entry point for this dummy steering.
     */
    Config(const std::string& objname);

    /**
     * D'tor.
     */
    ~Config();


    inline const std::string& jobOptionsPath() const { return m_jobOptionsPath; }

    inline const std::string& messageSvcType() const { return m_messageSvcType; }

    inline const std::string& evtSel() const { return m_evtSel; }

    inline const std::string& dllName() const { return m_dllName; }

    inline const std::string& factoryName() const { return m_factoryName; }

    inline const std::string& pythonSetupFile() const { return m_pythonSetupFile; }

  private :

    std::string m_jobOptionsPath;
    std::string m_messageSvcType;
    std::string m_evtSel;
    std::string m_dllName;
    std::string m_factoryName;
    std::string m_pythonSetupFile;
  };
}

#endif /* ESCONTROLLER_CONFIG_H */
