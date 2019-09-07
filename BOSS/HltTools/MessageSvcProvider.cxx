#include "HltTools/MessageSvcProvider.h"

MessageSvcProvider* MessageSvcProvider::s_instancePointer = NULL;

MessageSvcProvider* MessageSvcProvider::instance() {
  if (s_instancePointer == NULL) {
    s_instancePointer = new MessageSvcProvider;
  }
  return s_instancePointer;
}
