//============================================
/// \warning THIS CLASS IS *NOT* THREAD-SAFE!
//============================================
#ifndef MESSAGESVCPROVIDER_H
#define MESSAGESVCPROVIDER_H

#include <iostream>
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/MsgStream.h"

/// \class MessageSvcProvider
/// \brief Class that provides message services.
/// Add to any classes that need message services.
class MessageSvcProvider {

public:
  static MessageSvcProvider* instance();

  ~MessageSvcProvider(){}

  /// \fn IMessageSvc* msgSvc() const
  /// \warning If this fails, without cout there is no way to tell the user!
  /// \return An IMessageSvc pointer.
  /// To use, your class needs a MessageSvcProvider as a member.
  /// You can then do:
  /// MsgStream log(m_MessageSvcProvider.msgSvc(), m_objectName);
  IMessageSvc* msgSvc() /*const*/ {
//      if(!m_pMsgSvc){
      ISvcLocator* serviceLocator = Gaudi::svcLocator();
      //IService* theSvc;
      //std::cout<<"!!!!!!!!!!!!!!!!!1"<<std::endl;
      StatusCode sc = serviceLocator->service("MessageSvc",m_pMsgSvc);
      if(sc.isFailure()) exit(1); // Service [MessageSvc] not found
      //std::cout<<"!!!!!!!!!!!!!!!!!2"<<std::endl;
      //m_pMsgSvc = dynamic_cast<IMessageSvc*>(theSvc);
      if(!m_pMsgSvc) exit(1); // Dynamic cast IService --> IMessageSvc failed
//    }
    //std::cout<<"!!!!!!!!!!!!!!!!!3"<<std::endl;
    return m_pMsgSvc;
  }

private:
  static MessageSvcProvider* s_instancePointer;

  MessageSvcProvider() : m_pMsgSvc(0){}
  /// \var IMessageSvc* m_pMsgSvc
  /// \see msgSvc()
  IMessageSvc* m_pMsgSvc;
};

#endif // MESSAGESVCPROVIDER_H

