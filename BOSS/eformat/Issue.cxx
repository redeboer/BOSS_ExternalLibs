//Dear emacs, this is -*- c++ -*-

/**
 * @file Issue.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * Defines an eformat Issue.
 */

#include "eformat/Issue.h"

eformat::Issue::Issue(const ers::Context& context, ers::severity_t severity)
  : ers::Issue(context,severity)
{
  finish_setup("Base eformat issue");
}
