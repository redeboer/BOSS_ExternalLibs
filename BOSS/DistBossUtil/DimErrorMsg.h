#ifndef DIM_ERROR_MSG_H
#define DIM_ERROR_MSG_H

#include "dim_common.h"

namespace DimError {

const char* toSeverity(int severity) {
   switch ( severity ) {
      case ( DIM_INFO ) :
	 return "DIM_INFO";
      case ( DIM_WARNING ) :
	 return "DIM_WARNING";
      case ( DIM_ERROR) :
	 return "DIM_ERROR";
      default :
	 return "DIM_FATAL";
   }
}

const char* toMsg(int code) {
   switch ( code ) {
      case ( DIMDNSUNDEF ):
	 return "DIM_DNS_NODE undefined";
      case ( DIMDNSREFUS ):
	 return "DIM_DNS refuses connection";
      case ( DIMDNSDUPLC ):
	 return "Service already exists in DNS";
      case ( DIMDNSEXIT ):
	 return "DNS requests server to EXIT";
      case ( DIMDNSTMOUT ):
	 return "Server failed sending Watchdog";

      case ( DIMDNSCNERR ):
	 return "Connection to DNS failed";
      case ( DIMDNSCNEST ):
	 return "Connection to DNS established";

      case ( DIMSVCDUPLC ):
	 return "Service already exists in Server";
      case ( DIMSVCFORMT ):
	 return "Bat format string for service";
      case ( DIMSVCINVAL ):
	 return "Invalid Service ID";

      case ( DIMTCPRDERR ):
	 return "TCP/IP read error";
      case ( DIMTCPWRRTY ):
	 return "TCP/IP write error - Retrying";
      case ( DIMTCPWRTMO ):
	 return "TCP/IP write error - Disconnected";
      case ( DIMTCPLNERR ):
	 return "TCP/IP listen error";
      case ( DIMTCPOPERR ):
	 return "TCP/IP open server error";
      case ( DIMTCPCNERR ):
	 return "TCP/IP connection error";
      case ( DIMTCPCNEST ):
	 return "TCP/IP connection established";
      default :
	 return "Unknown Error Code!!!";
   }
}

}

#endif
