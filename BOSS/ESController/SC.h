// Dear emacs, this is -*- c++ -*-
/**
 * @author <a href="mailto:dosil@ifae.es">Mireia Dosil</a>
 * $Revision: 1.2 $
 * $Date: 2008/04/15 10:12:13 $
 *
 * @brief Pesa Steering Controller for Event Filter algorithms
 */

#ifndef ESC_STEERING_H
#define ESC_STEERING_H

#include "efhlt/Interface.h"
#include "ESController/Config.h"
#include <map>
#include <string>

// Include files for Gaudi
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IEventProcessor.h"

namespace efpsc {
  
  class SC : public efhlt::Interface
  {
  public:    
    /**
     * C'tor. (Nothing happens here...)
     */
    SC() ;
    
    /**
     * D'tor virtualisation
     */
    virtual ~SC() ;
    
    /**
     * Configures the framework
     *
     * @param config Is the only configuration parameter passed. The
     * actual configuration implementation has to parse it, if that is the
     * case in order to find its own way through the framework's configuration
     * system.
     */
    virtual bool configure (const std::string& config);
    
    /**
     * Unconfigures the framework, releasing all acquired resources.
     */
    virtual bool unconfigure (void);

    virtual bool prepareForRun(int run_number);
    
   /**
    * Process one event, taking as input a pointer to the RawEvent serialized in a contiguous
    * memory region (feStore) and giving as output the yes/no decision and the address (sdStore)
    * of the "local" memory region containing the serialized EF fragment
    */
   virtual bool process (
                const uint32_t *&feStore,
                const uint32_t *&sdStore,
                std::string &efDecision_s,
                std::vector<uint32_t> &efBitPattern);

  private: ///< helpers
  
    IEventProcessor *m_eventLoopMgr ;
    
  private: ///< representation
    efpsc::Config* m_config; ///< my own configuration
    
    bool  m_isCreated;   
    int m_outLev; 
    // Pesa Application Manager
    IInterface* m_pesaAppMgr;
    std::string m_nameEventLoopMgr;
  };
}

#endif /* ESC_STEERING_H */
