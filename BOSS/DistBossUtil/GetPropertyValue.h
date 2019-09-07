#ifndef GET_PROPERTY_VALUE_H
#define GET_PROPERTY_VALUE_H

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Parsers.h"  //in "GaudiSvc/JobOptionsSvc"

namespace DistBoss {

template<typename T>
StatusCode GetPropertyValue(const std::string& client, const std::string& name, T& value)
{
   static IJobOptionsSvc* jobSvc = 0;

   StatusCode sc;

   if ( jobSvc == 0 ) {
      sc = Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);
      if ( sc.isFailure() ) {
	 std::cout << "Can't get the JobOptionsSvc @ DistBoss::GetPropertyValue()" << std::endl;
	 return sc;
      }
   }

   const std::vector<const Property*>* properties = jobSvc->getProperties(client);
   if ( properties == NULL ) {
      std::cout << "In DistBoss::GetPropertyValue(), can't get client: " << client << std::endl;
      return StatusCode::FAILURE;
   }

   for ( unsigned int i = 0; i < properties->size(); ++i ) {
      if ( properties->at(i)->name() == name ) {
	 return Gaudi::Parsers::parse(value, properties->at(i)->toString());
      }
   }

   return StatusCode::FAILURE;
}

}

#endif
